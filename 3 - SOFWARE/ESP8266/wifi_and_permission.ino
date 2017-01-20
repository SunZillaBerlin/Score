

void get_start_wifi(){
        send_command_to_arduino(String(54));
        wait_for_answer_and_timeout(54); 
      
        if(wifi_mode == 1)      { WiFi.mode(WIFI_AP);     setup_own_wifi(); }
        else if(wifi_mode == 2) { WiFi.mode(WIFI_AP_STA); setup_own_wifi(); setup_wifi_client(); }
        else if(wifi_mode == 3) { WiFi.mode(WIFI_STA);                      setup_wifi_client(); }
  }
  
 void setup_own_wifi(){
    send_command_to_arduino(String(52));
    Serial.println("Setting up WiFi ");
    if(!wait_for_answer_and_timeout(52)){
      Serial << " Set up with default credentials";
    }
    else {
      Serial << "received creds";
    }
      //String AP_NameString = "SunZilla Energy " + identity;
      AP_NameString = AP_NameString +"_" + identity;
      
      char AP_NameChar[AP_NameString.length() + 1];
      memset(AP_NameChar, 0, AP_NameString.length() + 1);
    
      for (int i=0; i<AP_NameString.length(); i++)
        AP_NameChar[i] = AP_NameString.charAt(i);
  
            char AP_pwdChar[AP_pwdString.length() + 1];
      memset(AP_pwdChar, 0, AP_pwdString.length() + 1);
    
      for (int i=0; i<AP_pwdString.length(); i++)
        AP_pwdChar[i] = AP_pwdString.charAt(i);   

    WiFi.softAP(AP_NameChar, AP_pwdChar);
    Serial << AP_NameChar << "\n" << AP_pwdChar << "\n";
        Serial.print("Logon 192.168.4.1");
  }
void setup_wifi_client(){
  
  send_command_to_arduino("50");
  if(!wait_for_answer_and_timeout(50)){
    // Set up with default credentials
  }
  else{
    Serial << "received. trying to connect";
      
  }
  char char_ssid_client[ssid_client.length() + 1];
  memset(char_ssid_client, 0, ssid_client.length() + 1);
  for (int i=0; i<ssid_client.length(); i++)  char_ssid_client[i] = ssid_client.charAt(i);

  char char_pwd_client[pwd_client.length() + 1]; 
  memset(char_pwd_client, 0, pwd_client.length() + 1);
  for (int i=0; i<pwd_client.length(); i++)  char_pwd_client[i] = pwd_client.charAt(i);  
    WiFi.begin(char_ssid_client, char_pwd_client);
    WiFi.begin(char_ssid_client, char_pwd_client);
    WiFi.begin(char_ssid_client, char_pwd_client);

    int wifi_connect_begin = millis();

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
        if((wifi_connect_begin+timeout)<millis()){
          
          Serial.print("No connection to " + ssid_client +". Setting up own Wifi.");
          setup_own_wifi();
          break;
        }
    } 
}


void convert_IP(){
        IPAddress localAddr = WiFi.localIP();
        byte oct1 = localAddr[0];
        byte oct2 = localAddr[1];
        byte oct3 = localAddr[2];
        byte oct4 = localAddr[3];
        sprintf(IP, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
}
