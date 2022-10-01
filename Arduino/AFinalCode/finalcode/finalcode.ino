#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>

#include "structs.h"
#include "helperFunctions.h"

/*----------------------IMPORTANT SETTINGS OF THE SHELF------------------------*/
bool isMainShelf = true;
String shelfUniqueIndex = "AZ420";

/*----------------------Networking------------------------*/
// name and password of NDL local WiFi network
const  char*  ssid  =  "NDL_24G"; 
const  char*  password  =  "RT-AC66U"; 

// Back-end url
String shelfDataUrl = "http://192.168.1.3:11226/api/Trips/Shelf/";//

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

//Request JSON objects
const String reminderDateTimeJson = "reminderDateTime";
const String earliestCheckoutDateTimeJson = "earliestCheckOutDateTime";
const String categoriesJson = "categories";
const String nameJson = "name";
const String indexJson = "index";
const String tripIdJson = "tripId";

bool alarmWentOff = false;

/*--------------------Time server------------------------*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

/*--------------------Request Data-----------------------*/
int tripId = -1;
#define NUM_OF_CATEGORIES 4
Category *categories[NUM_OF_CATEGORIES];
datetime reminderDateTime = {};
datetime earliestCheckoutDateTime = {};

/*-----------------I2C Multiplexer setup-------------------*/
#define TCAADDR 0x70 

/*-----------------IR sensors setup-------------------*/
/*-Shelf sections: Screen index 0 --------- Screen index 1 ----*/
/*-Hooks:          Screen index 2 --------- Screen index 3 ----*/
/*-Shelf sections: shelfItem0OnPlace ------ shelfItem1OnPlace ----*/
/*-Hooks:          hookItem0OnPlace ------- hookItem1OnPlace  ----*/
const int IRSensorShelvesSize= 4;
int IRSensorShelves[IRSensorShelvesSize] = {14,25,26,27}; 
bool shelfItem0OnPlace = false;
bool shelfItem1OnPlace = false;
const int IRSensorHooksSize= 2;
int IRSensorsHooks[IRSensorHooksSize] = {12,13};
bool hookItem0OnPlace = false;
bool hookItem1OnPlace = false;

/*---------------------NFC setup-----------------------*/
const int RST_PIN = 33; // Reset pin
const int SS_PIN = 5; // Slave select pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String tagCode = "7C18F49";
String cardCode = "53743A6";

/*---------------------Speaker setup-----------------------*/
#define SPEAKER_PIN 32
#define SPEAKER_CHANNEL 0

/*----------------------Update data------------------------*/
bool updateScreens = false;

/*---------------------Setup-----------------------*/
void setup() {
  // Initialise Serial monitor
  Serial.begin(115200);
  while(!Serial);

  // Initialise Wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  timeClient.begin();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Wire.begin();
  for(uint8_t i=0;i<NUMSCREENS;i++) {
    tcaselect(OLEDS[i].i2cnum);
    OLEDS[i].display->begin();
    OLEDS[i].width = OLEDS[i].display->getDisplayWidth();
    OLEDS[i].height = OLEDS[i].display->getDisplayHeight();
  }
  Serial.println("OLEDs initialised.");
  if(isMainShelf){
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    Serial.println("NFC initialised.");
  }
  for (int i = 0; i < IRSensorHooksSize; i++){
    pinMode(IRSensorsHooks[i], INPUT); // sensor pin INPUT
  }
  Serial.println("Hooks IR initialised.");
  for (int i = 0; i < IRSensorShelvesSize; i++){
    pinMode(IRSensorShelves[i], INPUT); // sensor pin INPUT
  }
   Serial.println("Shelves IR initialised.");
}

void loop() {
  //Send an HTTP POST request every 5 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String response = httpGETRequest((shelfDataUrl + shelfUniqueIndex).c_str());
      Serial.println("respone:");
      Serial.println(response);
      deserializeResponse(response);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
    Serial.println("HTTP Request done.");
    checkIRSensorsOutput();
    Serial.println("IR sensors checked.");
    printCategories();
    Serial.println("Screens updated.");
  }
  Serial.println("Alarm?");
  Serial.println(alarmWentOff);
  if(isMainShelf){
    Serial.println("Is main shelf.");
    currentTimeIsReminderTime();
    Serial.println("Check reminder time.");
    if(alarmWentOff){
      Serial.println("Alarm went off.");
      playPirates(SPEAKER_PIN, SPEAKER_CHANNEL);
      alarmWentOff = false;
    }
  
    if(mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()){
      Serial.println("NFC detected, check.");
      String content= "";
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      if (content == tagCode || content == cardCode)
      {
        Serial.println("NFC detected, card/tag valid.");
        nfcCheckOutDetected();
      }
    }
  }
}

/*----------------I2C multiplexer controls--------------------------*/
void tcaselect(uint8_t i) {
  Serial.println("tcaselect");
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
  Serial.println("tcaselect done.");
}

void printCategories(){
  Serial.println("test");
  if(!updateScreens) {
    Serial.println("No screen update needed");
    return;
  }
    
  Serial.println("Screen update needed");
  for(uint8_t i=0;i<NUMSCREENS;i++) {
    Serial.print("Screen: ");
    Serial.println(i);
    logo curLogo;
    bool curLogoExists = false;
    for(uint8_t j=0;j<NUM_OF_CATEGORIES;j++){
      Serial.println("Category OK.");
      if(categories[j]->getScreenIndex() == i){
        Serial.println("Category match OK.");
        curLogo = logoArray[categoryIndex(categories[j]->getName())];
        Serial.println(curLogo.text1);
        curLogoExists = true;
        break;
      }
    }
    Serial.println("Before logo OK.");
    if(curLogoExists) {
      Serial.println("Logo exists OK.");
      tcaselect(OLEDS[i].i2cnum);
      OLEDS[i].display->clearBuffer();
      OLEDS[i].display->setDrawColor(1);
      OLEDS[i].display->drawXBM(0, 0, LOGO_WIDTH, LOGO_HEIGHT, curLogo.binary);
      OLEDS[i].display->setFont(curLogo.font);
      OLEDS[i].display->drawStr(curLogo.margin_x, curLogo.margin_y, curLogo.text.c_str());
      if(curLogo.text1 != ""){
        OLEDS[i].display->drawStr(curLogo.margin1_x, curLogo.margin1_y, curLogo.text1.c_str());
      }
      OLEDS[i].display->sendBuffer();
    }else{
      
      Serial.println("Empty screen OK.");
      OLEDS[i].display->clearBuffer();
    }
    
  }
  updateScreens = false;
  Serial.println("Screen updated");
}

/*----------------NFC controls--------------------------*/
void checkIRSensorsOutput(){
  Serial.println("checkIRSensorsOutput");
  for (int i = 0; i < IRSensorHooksSize; i++){
    if (digitalRead(IRSensorsHooks[i])){ // No bstacle or black item
      if(i == 0){
        hookItem0OnPlace = true;
      }else{
        hookItem1OnPlace = true;
      }
      Serial.println("No obstacle or black");
    }
    else{ // Obstacle detected
      if(i == 0){
        hookItem0OnPlace = false;
      }else{
        hookItem1OnPlace = false;
      }
      Serial.println("Obstacle or not black");
    }
  }

  for (int i = 0; i < IRSensorShelvesSize; i++){
    if (digitalRead(IRSensorShelves[i])){ // No bstacle or black item
      if(i == 0 || i == 1){
        shelfItem0OnPlace = false;
      }else{
        shelfItem1OnPlace = false;
      }
      Serial.println("No obstacle or black");
    }
    else{
      if(i == 0 || i == 1){
        shelfItem0OnPlace = true;
      }else{
        shelfItem1OnPlace = true;
      }
      Serial.println("Obstacle or not black");
    }
  }
  httpPOSTirSensorsOutput();
}

/*----------------NFC controls--------------------------*/
void nfcCheckOutDetected(){
  datetime currentDT = getCurrentTime(timeClient);
  Serial.println("nfcCheckOutDetected");

  // Compare current time to reminder time
  if(earliestCheckoutDateTime.year <= currentDT.year){
    if(earliestCheckoutDateTime.month <= currentDT.month){
      if(earliestCheckoutDateTime.day <= currentDT.day){
        if(earliestCheckoutDateTime.hour <= currentDT.hour){
          if(earliestCheckoutDateTime.minute <= currentDT.minute){
            if(earliestCheckoutDateTime.second <= currentDT.second){ 
              // If user checked out at most one hour before trip, detect that they left
              httpPOSTUserLeft(); 
            }
          }
        }
      }
    }
  }
}


/*----------------Compare trip start datetime to current--------------------------*/
void currentTimeIsReminderTime(){
  Serial.println("currentTimeIsReminderTime");
  // Reminder not set, no need to process current datetime
  
//  if(reminderDateTime.year == 1) {
//    alarmWentOff = false;
//    return;
//  }

  datetime currentDT = getCurrentTime(timeClient);
//  Serial.println("reminder");
//  Serial.println(reminderDateTime.second);
//  Serial.println("current");
//  Serial.println(currentDT.second);
//  Serial.println(currentDT.second >= reminderDateTime.second);
//  Serial.println(currentDT.second <= reminderDateTime.second + 10);
//  Serial.println(!alarmWentOff);
  // Compare current time to reminder time
  if(reminderDateTime.year == currentDT.year){
    if(reminderDateTime.month == currentDT.month){
      if(reminderDateTime.day == currentDT.day){
        if(reminderDateTime.hour == currentDT.hour){
          if(reminderDateTime.minute == currentDT.minute){
//            Serial.println(reminderDateTime.minute);
//            Serial.println(reminderDateTime.second);
//            Serial.println(currentDT.second);
//            Serial.println(reminderDateTime.second >= currentDT.second);
//            Serial.println(reminderDateTime.second <= currentDT.second - 10);
//            Serial.println(!alarmWentOff);
            if(currentDT.second >= reminderDateTime.second && currentDT.second <= reminderDateTime.second + 10 && !alarmWentOff){
//              Serial.println(reminderDateTime.second);
              alarmWentOff = true;
            }
          } else if (alarmWentOff == true){
//            alarmWentOff = false;
          }
        }
      }
    }
  }
//  alarmWentOff = false;
}


/*----------------POST Requests--------------------------*/
void httpPOSTUserLeft(){  
  Serial.println("httpPOSTUserLeft");
  WiFiClient client;
  HTTPClient http;
  String portRequestUrl = shelfDataUrl + "/UserLeft/" + String(tripId);
  http.begin(client, portRequestUrl.c_str());

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpResponseCode = http.POST("");  
}

void httpPOSTirSensorsOutput(){  
  // The problem in the code is here. The request url is of the type http://192.168.1.236:11226/api/Trips/Shelf/1/AZ421/-1/ItemPresent/1
  // While it should be of the type https://localhost:44315/api/Trips/Shelf/1/AZ421/Glasses/ItemPresent/true
  Serial.println("httpPOSTirSensorsOutput");
  WiFiClient client;
  HTTPClient http;
  for(uint8_t j=0;j<NUM_OF_CATEGORIES;j++){
    String portRequestUrl = shelfDataUrl + String(tripId) + "/" + shelfUniqueIndex + "/" + categories[j]->getName() + "/ItemPresent/";
    if(j == 0) { portRequestUrl = portRequestUrl + String(shelfItem0OnPlace ? "true" : "false"); }
    else if(j == 1) { portRequestUrl = portRequestUrl + String(shelfItem1OnPlace ? "true" : "false"); }
    else if(j == 2) { portRequestUrl = portRequestUrl + String(hookItem0OnPlace ? "true" : "false"); }
    else { portRequestUrl = portRequestUrl + String(hookItem1OnPlace ? "true" : "false"); }

    Serial.println(portRequestUrl);
    http.begin(client, portRequestUrl.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST("");
  }
}

/*----------------Response getting and processing--------------------------*/
String httpGETRequest(const char* serverName) {
  Serial.println("httpGETRequest");
  WiFiClient client;
  HTTPClient http;
  Serial.print("HTTP request link: ");
    Serial.println(serverName);
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = ""; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  Serial.println(payload);
  // Free resources
  http.end();
  return payload;
}

void deserializeResponse(String response){
//  Serial.print("test???????????????????????");
  DynamicJsonDocument doc(2048);
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, response);
  Serial.println(doc.size());
  // Test if parsing succeeds.
  Serial.println(error.f_str());
  if (error) {
    Serial.print(F("DeserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  Serial.println(doc.memoryUsage());
  Serial.println("deserial:");
//  Serial.println(doc[0]);
  int test = doc["tripId"];
  Serial.println(test);
//  Serial.println(doc[earliestCheckOutDateTimeJson]);
//  Serial.println(doc[reminderDateTimeJson]);
//  Serial.println(doc[categoriesJson.length()]);
  // Test if parsing succeeds.
  if (doc[tripIdJson] == -1) {
    Serial.println("No such shelf in the database or no trip for the shelf exists.");
  }

  // Test if parsing succeeds.
  if (doc[tripIdJson] == tripId) {
    return;
  }

  // Initialize all data objects
  String reminderDT = doc[reminderDateTimeJson];
  String earliestCheckoutDT = doc[earliestCheckoutDateTimeJson];
  tripId = doc[tripIdJson];
  reminderDateTime = parseDateTime(reminderDT);
  earliestCheckoutDateTime = parseDateTime(earliestCheckoutDT);
  for(int i = 0; i < 4; i++){
    categories[i] = new Category();
    String namei = doc[categoriesJson][i][nameJson];
    categories[i]->setName(namei);
    Serial.println("hier");
    Serial.println(namei);
    int ind = doc[categoriesJson][i][indexJson];
    Serial.println(ind);
    categories[i]->setScreenIndex(ind);
  }
  updateScreens = true;
  Serial.println("Deserialised successfully.");
}

datetime parseDateTime(String dateTimeString){
  // Datetime example from request - "2021-12-06T17:16:40"
  datetime dt = {};
  int indexOfT = dateTimeString.indexOf('T');
  String date = dateTimeString.substring(0, indexOfT);
  String reminderTime = dateTimeString.substring(indexOfT + 1, dateTimeString.length());
  int ind1Dash = date.indexOf('-');
  int ind2Dash = date.indexOf('-', ind1Dash+1);
  dt.year = date.substring(0, ind1Dash).toInt();
  dt.month = date.substring(ind1Dash + 1, ind2Dash).toInt();
  dt.day = date.substring(ind2Dash + 1).toInt();

  int ind1Column = reminderTime.indexOf(':');
  int ind2Column = reminderTime.indexOf(':', ind1Column+1);
  dt.hour = reminderTime.substring(0, ind1Column).toInt();
  dt.minute = reminderTime.substring(ind1Column + 1, ind2Column).toInt();    
  dt.second = reminderTime.substring(ind2Column + 1).toInt();
  return dt;
}
