// This Softare is licenced under under the LPGL 3.0
// Use at your own risk!
// Author: Laurin Vierrath / SunZilla UG (haftungsbeschraenkt), Berlin
// Jan 2017

boolean first_data_submission = true;

void get_identity(){
    
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    identity = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    identity.toUpperCase(); 
}

boolean check_ssl_cert(){
  Serial << "Checking SSL Cert\n";
  send_command_to_arduino("68");
  if(!wait_for_answer_and_timeout(68)){
    // Set up with default credentials
  }
  else{
    Serial << "received SSl cert. veryfing\n";
  
  char char_ssl_cert[ssl_cert.length() + 1];
  memset(char_ssl_cert, 0, ssl_cert.length() + 1);
  for (int i=0; i<ssl_cert.length(); i++)  char_ssl_cert[i] = ssl_cert.charAt(i);  
    if ( client.connect(host, httpsPort)) {
      Serial << ssl_cert << "\n" << char_ssl_cert;
          Serial.println("connected to sunzilla.de");
            if (client.verify(char_ssl_cert, host)) {
              Serial.println("SSL certificate matches");
              return true;
             } else {
               Serial.println("SSL certificate doesn't match");
               return false;
             }
    }
    }
}
void get_time(){
  Serial << "Requesting real time\n";
    if ( client.connect(host, httpsPort)) {
          client.println("GET /score/get_time.php HTTP/1.0\r\nHost: sunzilla.de\n");

          delay(100);
          String time;
          while (client.available()) {
            char c = client.read();
            //Serial.print(c);
            if(String(c) == "#" || time.length() > 0) time = time + c;
          }        
          delay(200);
          Serial.print("score 5,");
          Serial.println(time.substring(1));
          client.println("Connection: close");
     }    
     else {
      Serial.println("no connection to get time.");
     }
}       

void send_IP_to_server(){
    if ( client.connect(host, httpsPort))
      client.println("GET /score/insert_ip.php?IP="+String(IP)+" HTTP/1.0\r\nHost: sunzilla.de\n");
           else {
      Serial.println("no connection to transmitt IP.");
     }
}
void send_to_server(String query){
  if (client.connect(host, httpsPort)) {  
          if(!first_data_submission) client.println("GET /score/insert_data.php?id="+identity+"&query="+query+" HTTP/1.0\r\nHost: sunzilla.de\n");
          else {
            client.println("GET /score/insert_data.php?first=1&id="+identity+"&query="+query+" HTTP/1.0\r\nHost: sunzilla.de\n");
            first_data_submission = false;
          }
  }
}
