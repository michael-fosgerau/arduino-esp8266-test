#include <SoftwareSerial.h>

#define SSID "indtast-ssid-her"
#define PASS "indtast-wifi-kodeord-her"
#define LOCATIONID "2624652" // "aarhus" - se http://openweathermap.org/help/city_list.txt
#define DST_IP "188.226.224.148" // api.openweathermap.org
SoftwareSerial esp(10, 11); // RX, TX for debugging
int i;
   
void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing ESP8266 module...");

  esp.begin(115200); // har prøvet med 57600 også
  esp.setTimeout(5000);

  esp.println("AT+RST");
  delay(1000);
    
  if(esp.find("ready")) {
    Serial.println("ESP8266 module is ready");
  }else{
    Serial.println("ESP8266 module does not respond.");
    while(1);
  }
  delay(1000);

  boolean connected=false;
  for(int i=0;i<5;i++){
    if(connectWiFi()){
      connected = true;
      Serial.println("Connected to WiFi...");
      break;
    }
  }
  if (!connected){
    Serial.println("Coudn't connect to WiFi.");
    while(1);
  }
  delay(5000);
  
  esp.println("AT+CIPMUX=0");
}
void loop()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  esp.println(cmd);
  Serial.println(cmd);
  
  if(esp.find("Error"))
  {
    return;
  }
  
  cmd = "GET /data/2.5/weather?id=";
  cmd += LOCATIONID;
  cmd += " HTTP/1.0\r\nHost: api.openweathermap.org\r\n\r\n";
  esp.print("AT+CIPSEND=");
  esp.println(cmd.length());
  Serial.println(cmd);
  
  if(esp.find(">"))
  {
    Serial.print(">");
  }
  else
  {
    esp.println("AT+CIPCLOSE");
    Serial.println("connection timeout");
    delay(1000);
    return;
  }
  Serial.println("Recieving...");
  esp.print(cmd);
  unsigned int i = 0;
  int n = 1;
  char json[100]="{";
  while (!esp.find("\"main\":{")){}
  while (i<60000) {
    Serial.println("Recieving...");
    if(esp.available()) {
      Serial.println("Reading data...");
      char c = esp.read();
      json[n]=c;
      if(c=='}') break;
      n++;
      i=0;
    }
    i++;
  }
  Serial.println("Response data is:");
  Serial.println(json);
  delay(600000);
    
}

boolean connectWiFi()
{
  Serial.println("Connecting to WiFi...");
  esp.println("AT+CWMODE=1");
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp.println(cmd);
  delay(2000);
  if(esp.find("OK")){
    Serial.println("OK, Connected to WiFi.");
    return true;
  }else{
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
}
