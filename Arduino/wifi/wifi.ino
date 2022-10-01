//#include <ArduinoJson.h>
#include <WiFi.h>
#include <NTPClient.h>
const  char*  ssid  =  "No, better not...";              //  name  of  local  WiFi  network  in  the  NDL
const  char*  password  =  "99922856";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  timeClient.begin();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  initWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
