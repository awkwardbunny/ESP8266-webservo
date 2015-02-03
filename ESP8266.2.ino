#define SSID        "KMGZ2"
#define PASS        "18014c80ec"
#define TIMEOUT     10000 // mS

#include <Servo.h>
#include <SoftwareSerial.h>

boolean locked = true;

Servo servo;
SoftwareSerial E(2, 3); // RX, TX

boolean echoPage(int id){
  String stat;
  if(locked)
    stat = "LOCKED";
  else
    stat = "UNLOCKED";
    
  String html = "<html><body>Status: " + stat + "</br><FORM><INPUT Type=\"BUTTON\" VALUE=\"LOCK!\" ONCLICK=\"window.location.href='/1'\"><INPUT Type=\"BUTTON\" VALUE=\"UNLOCK!\" ONCLICK=\"window.location.href='/0'\"></FORM></body></html>";
  int htmll = html.length();
  
  String sendd = "AT+CIPSEND="; sendd += id; sendd += ","; sendd += htmll;
  E.println(sendd);
  sfind(">");
  E.print(html);
  return sfind("SEND OK");
}
  
void lock(){
  locked = true;
  servo.write(160);
}

void unlock(){
  locked = false;
  servo.write(0);
}

boolean sfind(String k){
  byte current_char = 0;
  byte length = k.length();
  long deadline = millis() + TIMEOUT;
  
  while(millis() < deadline)
  {
    if (E.available())
    {
      char ch = E.read();
      //Serial.print(ch);
      if (ch == k[current_char]){
        if (++current_char == length){
          return true;
        }
      }else{
        current_char = 0;
      }
    }
  }
  return false;
}

boolean connect_wifi(){
  String cmd = "AT+CWJAP=\""; cmd += SSID; cmd += "\",\""; cmd += PASS; cmd += "\"";
  E.println(cmd);
  if (sfind("OK")){
    return true;
  }else{
    return false;
  }
}

void setup(){
  Serial.begin(9600);
  E.begin(9600);
  
  E.setTimeout(TIMEOUT);
  
  Serial.println("Starting program in...");
  Serial.print("3...");
  delay(1000);
  Serial.print("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  
  Serial.println("Attempting reset...");
  E.println("AT+RST");
  if(sfind("ready"))
    Serial.println("Ready!");

//Get Firmware Version
  E.println("AT+GMR");
  String f = "";
  for(int i = 0; i < 9; i++){ //offset of 6 char and a newline
    E.read();
  }
  char c = E.read();
  while(c != 'O' ){
    if(c != '\n')
      f += c;
    c = E.read();
  }
  Serial.println("Firmware Version:"+f);
  E.read(); //clear the 'K' left in buffer

//Get Current Wifi Mode
  E.println("AT+CWMODE?");
  while(E.available()){
    char d = E.read();
    if(d == ':')
      break;
  }
  int wifiMode = E.read()-48;
  sfind("OK");
  Serial.print("Wifi Mode:");
  Serial.println(wifiMode);
  
  if(wifiMode != 1){
    Serial.print("Setting wifi mode to 1...");
    E.println("AT+CWMODE=1");
    sfind("OK");
    Serial.println("Done!");
  }
  
//Set CIPMUX
  Serial.print("Setting CIPMUX on...");
  E.println("AT+CIPMUX=1");
  sfind("OK");
  Serial.println("Done!");
  
//Connect to wifi
  boolean connection_established = false;
  for(int i=0;i<5;i++)
  {
    Serial.print("Establishing wireless connection... Attempt #");
    Serial.println(i+1);
    if(connect_wifi())
    {
      connection_established = true;
      break;
    }
  }
  if (!connection_established)
    Serial.println("Connection could not be established :(");
  else
    Serial.println("Successfully connected to wifi!");
    
//Get the IP
  E.println("AT+CIFSR");
  String ip = "";
  for(int i = 0; i < 11; i++){ //offset of 6 char and a newline
    E.read();
  }
  char d = E.read();
  while(d != 'O' ){
    if(d != '\n')
      ip += d;
    d = E.read();
  }
  Serial.println("IP:"+ip);
  E.read(); //clear the 'K' left in buffer
  
//Start server
  Serial.print("Starting server on port 80...");
  E.println("AT+CIPSERVER=1,80");
  sfind("OK");
  Serial.println("Done!");
  
  Serial.println("-----------------------");
  
//Set up the servo
  Serial.print("Initializing servo...");
  servo.attach(9);
  Serial.println("Done!");
  servo.write(160);
  Serial.println("LOCKED!");
  
  Serial.println("-----------------------");
}

void loop(){
  if(E.available() > 20){
    //get ID
    if(sfind("+IPD,")){
    int id = E.read()-48;
    Serial.print("ID:");
    Serial.println(id);
    
    //get GET
    while(true){
      char d = E.read();
      if(d == '/')
        break;
    }
    while(!E.available()){}
    int r = E.read()-48;
    if(r == ' '-48)
      r = -1;
    Serial.print("Request:");
    Serial.print(r);
    sfind("OK");
    
    if(r == 1){
      Serial.println(" LOCK!");
      lock();
    }else if(r == 0){
      Serial.println(" UNLOCK!");
      unlock();
    }else if(r == 54){
      Serial.println(" Favicon");
      String closes = "AT+CIPCLOSE="; closes += id;
      E.println(closes);
      Serial.println("-----------------------");
    }else{
      Serial.println();
    }
    
    if(echoPage(id)){
      Serial.println("Page sent sucessfully");
    }else{
      Serial.println("Page not sent sucessfully! :)");
    }
    
    String closes = "AT+CIPCLOSE="; closes += id;
    E.println(closes);
    Serial.println("-----------------------");
  }
  }
}
