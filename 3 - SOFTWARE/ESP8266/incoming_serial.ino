// This Softare is licenced under under the LPGL 3.0
// Use at your own risk!
// Author: Laurin Vierrath / SunZilla UG (haftungsbeschraenkt), Berlin
// Jan 2017


String serial_msg_ok;
String serial_msg;
String hist_data;
int begin_msg;
int end_msg;  
  void send_command_to_arduino(String command){
    Serial << F("score ") << command << "\n";    
  }
  
boolean wait_for_answer_and_timeout(int command){
      int begin_time = millis();  
      int got_answer = 0;
        while (got_answer == 0) {
          Serial.print(".");
           int incoming_command = handle_Serial_Command();  
           //Serial << incoming_command;
           if(incoming_command == command) { return true; break; got_answer = 1;}
           delay(100);
          if((begin_time+timeout)<millis()){
            wifi_mode = 1;
            Serial.println("No answer recevied.");
            break;
            return false;
          }
      }   
}
int handle_Serial_Command(){
        if ( Serial.available()) {
        
        serial_msg = Serial.readStringUntil('\n');
        begin_msg = serial_msg.lastIndexOf('#');
        end_msg = serial_msg.lastIndexOf('%');
        if(end_msg != -1 && begin_msg != -1){
          serial_msg_ok = serial_msg.substring(begin_msg+1,end_msg-1);   
          int received_command = seperate_string(serial_msg_ok); 
          
          
          return received_command;          
        }
      }
      
}

int seperate_string(String str){
  int command = str.substring(0,str.indexOf(';')).toInt();

  str = str.substring(str.indexOf(';')+1);
  if(command == 1){  // Read Genearl status Message
    live_data = str;
    uptime = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    V_Bus = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
  
    SOC = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    E_Bat = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    
    E_PV = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    state_PV = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    E_AC = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    state_AC = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    E_DC = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    state_DC = str.substring(0,str.indexOf(';'));
    str = str.substring(str.indexOf(';')+1);
    
    event = str.substring(0,str.indexOf(';'));
             if(last_sent_to_server + 30000 < millis())
            {
              send_to_server(live_data);
              last_sent_to_server = millis();
            }
            calculate_powers();
  }
  else if (command == 2){
        hist_data = hist_data+str;
        Serial.println("histrocial data collected");
  }
  else if (command == 3){
        filename = str;
  }  
else if (command == 50){ // Got Wifi credentials
        ssid_client = str.substring(0,str.indexOf(';'));
         str = str.substring(str.indexOf(';')+1);
        pwd_client = str.substring(0,str.indexOf(';'));        
  }

  else if (command == 52){ 
          //Serial.println(str);
          AP_NameString = str.substring(0,str.indexOf(';'));
        
          str = str.substring(str.indexOf(';')+1);
          AP_pwdString = str.substring(0,str.indexOf(';'));   

  }  
  else if (command == 54){
        
        wifi_mode = str.substring(0,str.indexOf(';')).toInt();

  }

  else if (command == 56){
        String user = str.substring(0,str.indexOf(';'));
      
        str = str.substring(str.indexOf(';')+1);
        String pwd = str.substring(0,str.indexOf(';'));  
        Serial << user << "\n"<< pwd << "\n";
        str = str.substring(str.indexOf(';')+1);
        permission = str.substring(0,str.indexOf(';')).toInt(); 
        
        www_username[user.length() + 1];
        memset(www_username, 0, user.length() + 1);
      
        for (int i=0; i<user.length(); i++)
          www_username[i] = user.charAt(i);
    
        
        www_password[user.length() + 1];
        memset(www_password, 0, pwd.length() + 1);
      
        for (int i=0; i<pwd.length(); i++)
          www_password[i] = pwd.charAt(i);      

   }
  else if (command == 60){
        
        initial_config = str.substring(0,str.indexOf(';')).toInt();

  }   
  else if (command == 68){
        
        ssl_cert = str.substring(0,str.indexOf(';'));
        //Serial << "\n yes cert " << ssl_cert << "\n";

  }  
   return command;
}
