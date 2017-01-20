
  int chef;
  String Bat_state;
  String text_PV;
  String button_PV;
  String text_AC;
  String button_AC;
  String text_DC;
  String button_DC;    
  int redirect;

void calculate_powers(){
  int now_now = millis();
  int delta_T = now_now - last_data;
  if (delta_T > 900){
  last_data = now_now;
  
  //Serial.println(delta_T);
  

  float E_Bat_int = E_Bat.toInt();
  float E_PV_int = E_PV.toInt();
  float E_AC_int = E_AC.toInt();
  float E_DC_int = E_DC.toInt();

  E_Bat = String (E_Bat_int/3600);
  E_PV = String (E_PV_int/3600);
  E_AC = String (E_AC_int/3600);
  E_DC = String (E_DC_int/3600);

  P_Bat = 1000*(E_Bat_int - last_E_Bat)/delta_T;
  P_PV = 1000*(E_PV_int - last_E_PV)/delta_T;
  P_AC = 1000*(E_AC_int - last_E_AC)/delta_T;
  P_DC = 1000*(E_DC_int - last_E_DC)/delta_T;
//  Serial.println(E_Bat);
//  Serial.println(last_E_Bat);  
//  Serial.println("");
//  Serial.println(P_AC);
//  Serial.println(P_PV);  
//  Serial.println("");

  last_E_Bat = E_Bat_int;
  last_E_PV = E_PV_int;
  last_E_AC = E_AC_int;
  last_E_DC = E_DC_int;
  }
}

void create_wifi_answers(){
  
  // Web-App Status page
    server.on("/", [](){
      if(!server.authenticate(www_username, www_password) && permission == 3)
        return server.requestAuthentication();
      server.send(200, "text/html", create_status_webpage(false));} );

  // Web-App Admin Panel
    server.on("/admin", [](){
      if(!server.authenticate(www_username, www_password) && permission != 1)
        return server.requestAuthentication();
      else if(server.arg("v") == "" && server.arg("c") == "") {
        server.send(200, "text/html", create_status_webpage(true));
      }
      else if(server.arg("v") == "1" && server.arg("c").toInt() == 1) {
        server.send(200, "text/plain", "#1;"+live_data+";%");
      }
      else if(server.arg("v") == "1" && server.arg("c").toInt() < 100) {
        Serial.print("score " + server.arg("c"));
        // 2
        if(server.arg("c") == "2") {
          Serial.print(","+server.arg("lines")+","+server.arg("offset")+","+server.arg("file"));
           int wait_for_data = millis();
                  int r = 0;
                  String hist_data = "";
                  while((wait_for_data+2000)>millis() || Serial.available()){
                     if(Serial.available()) {
                              readdata();
                              hist_data = hist_data + data;
                     }
                     
                }  
                server.send(200, "text/plain", hist_data);
        }
        else if(server.arg("c") == "3") {
          wait_for_answer_and_timeout(3);
          server.send(200, "text/plain", filename);
          }           
        else if(server.arg("c") == "4") {
          server.send(200, "text/plain", identity);
          } 
       else if(server.arg("c") == "5") {
          Serial.print(","+server.arg("hr")+","+server.arg("min")+","+server.arg("sec")+","+server.arg("dy")+","+server.arg("mth")+","+server.arg("yr"));
          } 
       else if(server.arg("c") == "6") {
          get_time();
          }           
          
        // 15
        else if(server.arg("c") == "15") {
          Serial.print(","+server.arg("SOC"));
          }        
        // commands 51 53 57
        else if(server.arg("c") == "51" || server.arg("c") == "53" || server.arg("c") == "57") {
          Serial.println(";"+server.arg("name")+";"+server.arg("pwd"));
          if(server.arg("c") == "57") {
              Serial << ";" << server.arg("perm") << "\n";
              delay(50); // dont like this delay here
              send_command_to_arduino(String(server.arg("c").toInt()-1)); // renew permission
          }
          else {
              Serial << "\n";
              delay(50); // dont like this delay here
              send_command_to_arduino(String(server.arg("c").toInt()-1)); // renew permission           
          }
        
        }
        // 55
        else if(server.arg("c") == "55") {
          Serial.print(","+server.arg("mode"));
          }
        // 59
        else if(server.arg("c") == "59") {
          Serial.print(","+server.arg("E_Bat") + ";" + server.arg("P_2") + ";" + server.arg("P_3") + ";" + server.arg("P_4"));
          }
        if(server.arg("c") == "60") {
          send_command_to_arduino("60");
          wait_for_answer_and_timeout(60);
          server.send(200, "text/plain", initial_config);
          }             
        // 61
        else if(server.arg("c") == "61") {
          Serial.print(","+server.arg("CH2") + server.arg("CH3") + server.arg("CH4"));
          }
        // commands 64
        else if(server.arg("c") == "64") {
          server.send(200, "text/plain", IP);
        }   
        // commands 65
        else if(server.arg("c") == "65") {
          if(String(www_username) == server.arg("name") && String(www_password) == server.arg("pwd")) server.send(200, "text/plain", "correct");
          else server.send(200, "text/plain", "error");
        }
        else if(server.arg("c") == "69") {
          get_start_wifi();// renew WiFi
        }
  
                  
        Serial << "\n";
        if(server.arg("return") == "1") server.send(200, "text/html", create_wait_page());
        else server.send(200, "text/plain", "OK");
      }
      else server.send(200, "text/plain", "BAD REQUEST");
    });
}

// FUNCTIONS TO RECEIVE LARGE AMOUNTS OF DATA THROUGH SERIAL
// https://blog.ostanin.org/2012/01/08/sending-large-strings-of-data-to-arduino/

String create_status_webpage(boolean admin){
   
  String s = "<!DOCTYPE HTML>\r\n<html><head><title>SunZilla Energy "+identity+"</title><style> body { font-family: Arial, Verdana; } .button { border: none; \n color: white; \n  padding: 5px 5px; \n text-align: center;\n   text-decoration: none; \n  display: inline-block; \n  font-size: 16px; } </style><meta http-equiv=\"refresh\" content=\"5; url=/";
  if(admin) s += "admin";
  
  s +="\"><meta http-equiv=\"cache-control\" content=\"no-cache, must-revalidate, post-check=0, pre-check=0\">  <meta http-equiv=\"expires\" content=\"Sat, 31 Oct 2014 00:00:00 GMT\">  <meta http-equiv=\"pragma\" content=\"no-cache\"><meta http-equiv=\"expires\" content=\"0\"><META HTTP-EQUIV=\"CACHE-CONTROL\" CONTENT=\"NO-CACHE\"><META HTTP-EQUIV=\"PRAGMA\" CONTENT=\"NO-CACHE\">";
  s += create_no_cache();
  
//  s += "<meta name=\"viewport\" content=\"width";
//  s += "=device-width, initial-scale=0.6\">";
  s += "</head><body><center>\r\n";
      s += "<h1 style=\"color:#ff5000;\">SunZilla Energy</h1><br>";
      
    s += "<br><table border=0  cellpadding=4>";
    s += "<tr><td><b>Battery State</b></td><td align=center>" + String(SOC) + "%<br>(" + V_Bus + " V)</td><td><b>Time</b></td><td align=center colspan=2><center>" + uptime + "</center></tr>";
          if(admin){
       s += "<tr><td><b>Last Event</b></td><td colspan=4>" + event +"</td></tr><tr><td>&nbsp;</td></tr>";
          }
    s += "<tr><td><b>Channels</b></td><td align=center><font><b>Battery</b></font></td><td align=center><b>Solar</b></td><td align=center><b>Output</b></td><!--<td align=center><b>Output 2</b></td>--></tr>";
    if(P_Bat  > 0) Bat_state = " <font color=green>(charging)</font>";
    else if(P_Bat < 0) Bat_state = " <font color=red>(discharging)</font>";
    else Bat_state = "";
    
    s += "<tr><td>Current Power [W]</td><td align=center>" + String(P_Bat) + Bat_state+ "</td><td align=center>" + P_PV + "</td><td align=center>" + P_AC + "</td><!--<td align=center>" + P_DC + "</td>--></tr>";
    s += "<tr><td>Energy at Uptime [Wh]</td><td align=center>" + E_Bat + "</td><td align=center>" + E_PV + "</td><td align=center>" + E_AC + "</td><!--<td align=center>" + E_DC + "</td>--></tr>";


      if(state_PV == "0"){
        text_PV = "<font color=red>OFF</font>";
        button_PV = "<a href=\"/admin?v=1&return=1&c=21\"><button class=\"button\" style=\"border: 2px solid green; background-color:grey\">connect</button></a>";
      }
      else if(state_PV == "1"){
        text_PV = "<font color=green>ON</font>";
        button_PV = "<a href=\"/admin?v=1&return=1&c=20\"><button class=\"button\" style=\"border: 2px solid red; background-color:grey\">disconnect</button></a>";
      }    
//      if(state_DC == "0"){
//        text_DC = "<font color=red>OFF</font>";
//        button_DC = "<a href=\"3,1\"><button class=\"button\" style=\"background-color:grey\">connect</button></a>";
//      }
//      else if(state_DC == "1"){
//        text_DC = "<font color=green>ON</font>";
//        button_DC = "<a href=\"3,0\"><button class=\"button\" style=\"background-color:grey\">disconnect</button></a>";
//      } 
      if(state_AC == "0"){
        text_AC = "<font color=red>OFF</font>";
        button_AC = "<a href=\"/admin?v=1&return=1&c=31\"><button class=\"button\" style=\"border: 2px solid green; background-color:grey\">connect</button></a>";
      }
      else if(state_AC == "1"){
        text_AC = "<font color=green>ON</font>";
        button_AC = "<a href=\"/admin?v=1&return=1&c=30\"><button class=\"button\" style=\"border: 2px solid red; background-color:grey\">disconnect</button></a>";
      }    
      
      s += "<tr align=center><td align=left>Status [on/off]</td><td></td><td>"+ text_PV +"</td><td>"+text_AC+"</td><!--<td>"+text_DC+"</td>--></tr>";
    if(admin){
      s += "<tr><td align=left>Switch Channel</td><td></td><td>"+ button_PV +"</td><td>"+ button_AC +"</td><!--<td>"+ button_DC +"</td>--></tr>";
      s += "<tr align=center><td colspan=2><a href=\"/admin?v=1&return=1&c=11\"><button class=\"button\" style=\"background-color:green\">Connect All Channels</button></a></td><td colspan=3><a href=\"/admin?v=1&return=1&c=10\"><button class=\"button\" style=\"background-color:red\">Disconnect All Channels</button></a></td></tr>";
    }
 
    
 
    s += "</table><br><br>Page is refreshed every 5 seconds";
    if(admin){
        s += "<br><a href=\"/admin\"><button class=\"button\" style=\"background-color:grey\">Refresh Now</button></a> <a href=\"/admin?v=1&c=8&return=1\"><button class=\"button\" style=\"background-color:grey\">Reset Energy values</button></a>";
    }
    s += "</center></body><head>" + create_no_cache() +"</head></html>";
    redirect = 5;
    return s;
}
String create_no_cache(){
  return "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.6\">";
  //<meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, must-revalidate\" /><meta http-equiv=\"Pragma\" content=\"no-cache\" /><meta http-equiv=\"Expires\" content=\"0\" />";
}

String create_wait_page() { 
            // Create wait page.
            String s = "<html><head><script type=\"text/JavaScript\">\n setTimeout(\"location.href = '/admin";
            s += "';\",2000);\n</script>";
            s += create_wait_vizu_style();
            s += create_no_cache();
      
            s += "</head><body><br><br> ";
            s += create_wait_vizu();
            s += "<center> <font face=Arial>Command will be processed....</font> </body>";
            s += "<head>" + create_no_cache() +"</head></html>";
            return s;
            }
String create_wait_vizu_style(){
  return "<style>\n.ball {\n    background-color: rgba(0,0,0,0);\n    border: 5px solid rgba(255,80,0,0.9);\n    opacity: .9;\n    border-top: 5px solid rgba(0,0,0,0);\n    border-left: 5px solid rgba(0,0,0,0);\n    border-radius: 50px;\n    box-shadow: 0 0 35px #FF9762;\n    width: 50px;\n    height: 50px;\n    margin: 0 auto;\n    -moz-animation: spin .5s infinite linear;\n    -webkit-animation: spin .5s infinite linear;\n}\n\n.ball1 {\n    background-color: rgba(0,0,0,0);\n    border: 5px solid rgba(255,80,0,0.9);\n    opacity: .9;\n    border-top: 5px solid rgba(0,0,0,0);\n    border-left: 5px solid rgba(0,0,0,0);\n    border-radius: 50px;\n    box-shadow: 0 0 15px FF9762;\n    width: 30px;\n    height: 30px;\n    margin: 0 auto;\n    position: relative;\n    top: -50px;\n    -moz-animation: spinoff .5s infinite linear;\n    -webkit-animation: spinoff .5s infinite linear;\n}\n\n@-moz-keyframes spin {\n    0% {\n        -moz-transform: rotate(0deg);\n    }\n\n    100% {\n        -moz-transform: rotate(360deg);\n    };\n}\n\n@-moz-keyframes spinoff {\n    0% {\n        -moz-transform: rotate(0deg);\n    }\n    100% {\n        -moz-transform: rotate(-360deg);\n    };\n}\n@-webkit-keyframes spin {\n    0% {\n        -webkit-transform: rotate(0deg);\n    }\n    100% {\n        -webkit-transform: rotate(360deg);\n    };\n}\n@-webkit-keyframes spinoff { \n0% { \n -webkit-transform: rotate(0deg); \n} \n100% {\n -webkit-transform: rotate(-360deg);\n };\n}\n</style>";
}
String create_wait_vizu(){
  return "<div class=\"ball\"></div><div class=\"ball1\"></div>";
}
            
#include <stdio.h>
void readdataBuffer(int bytesToRead) {  
    int i = 0;
    char c = 0;
    while (i < 128 && (i < bytesToRead || bytesToRead <= 0)) {
        while (!Serial.available())
            ;
        c = Serial.read();
        if (c == '\r' || c == '\n') {
            break;
        }
        dataBuffer[i] = c;
        i++;
    }
    dataBufferSize = i;
}

void readdata() {  
    data[0] = '\0';
    readdataBuffer(0);
    if (strncmp(dataBuffer, "RCV", 3) == 0) {
        dataBuffer[dataBufferSize] = '\0';
        int expectedSize = atoi(dataBuffer + 4);
        if (expectedSize <= 0 || expectedSize > 1024) {
            return;
        }
        Serial.println("RDY");
        int bytesRead = 0;
        while (bytesRead < expectedSize) {
            readdataBuffer(expectedSize - bytesRead);
            memcpy(data + bytesRead, dataBuffer, dataBufferSize);
            bytesRead += dataBufferSize;
            Serial.print("ACK ");
            Serial.println(dataBufferSize);
        }
        data[bytesRead] = '\0';
    } else {
        memcpy(data, dataBuffer, dataBufferSize);
        data[dataBufferSize] = '\0';
    }
}
