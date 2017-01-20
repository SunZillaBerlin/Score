// This Softare is licenced under under the LPGL 3.0
// Use at your own risk!
// Author: Laurin Vierrath / SunZilla UG (haftungsbeschraenkt), Berlin
// Jan 2017

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <Time.h>
#include <WiFiClientSecure.h>  
WiFiClientSecure client;



const char* host = "sunzilla.de";
const int httpsPort = 443;  



String identity;

      const char accesspoint_pwd[] = "";
ESP8266WebServer server(80);
String ssid_client="";
String pwd_client ="";
char* www_username = "sunzilla";
char* www_password = "energy";
String var;
int wifi_mode = 0;
int timeout = 10000;
int permission = 3;
String AP_NameString = "SunZilla";
String AP_pwdString = "energy";

String live_data;
String filename;
String initial_config;
String ssl_cert;
char IP[16]; 
char data[1024];  
char dataBuffer[128];  
int dataBufferSize = 0;
unsigned int last_sent_to_server;
unsigned int last_data = 0;;
  int P_Bat = 0;
  int P_PV = 0;
  int P_AC = 0;
  int P_DC = 0;
  String E_Bat = "0";
  String E_PV = "0";
  String E_AC = "0";
  String E_DC = "0";
  long last_E_Bat;
  long last_E_PV;
  long last_E_AC;
  long last_E_DC;  
  String state_PV = "0";
  String state_AC = "0";
  String state_DC = "0";
  String uptime = "";
  String V_Bus = "0";
  String SOC;
  String event;




void setup() {
  Serial.begin(57600);
    Serial.println("Hello!");
      delay(1000);
  // Start WiFi & Sever
 Serial.setTimeout(10);


    
    send_command_to_arduino(String(56));
    wait_for_answer_and_timeout(56);   

  get_identity();// unique identity will be stored in var identity

  // GET WIFI MODE
  get_start_wifi();
  




//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
//  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
//    Serial.println("WiFi Connect Failed! Rebooting...");
//    delay(1000);
//    ESP.restart();
//  }
//  ArduinoOTA.begin();
  
  // create the answers to the wifi requests
  create_wifi_answers();
  
  server.begin();
  delay(200); // without it doenst work
  
  check_ssl_cert();
  get_time();
  
  convert_IP(); // converts IP to String 
  send_IP_to_server();
  
}
void loop() {
  handle_Serial_Command();
//  ArduinoOTA.handle();
  server.handleClient();
    // Check if a client has connected
//  WiFiClient client = server.available();
//  if (!client) {
//    return;
//  }
}
