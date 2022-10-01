#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "icons.h"
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <NTPClient.h>
#include "pitches.h"
#include "themes.h"

//display stuff
#define TCAADDR 0x70 // I2C Multiplexer ADDR
#define LOGO_HEIGHT 32
#define LOGO_WIDTH 32
#define LOGO_COUNT 9

//nfc stuff
const int RST_PIN = 33; // Reset pin
const int SS_PIN = 5; // Slave select pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//speaker stuff
#define BUZZER_PIN 32
#define BUZZER_CHANNEL 0

void tone(uint8_t pin, unsigned int frequency, unsigned long duration, uint8_t channel)
{
    if (ledcRead(channel)) {
        log_e("Tone channel %d is already in use", channel);
        return;
    }
    ledcAttachPin(pin, channel);
    ledcWriteTone(channel, frequency);
    if (duration) {
        delay(duration);
        noTone(pin, channel);
    }    
}

void noTone(uint8_t pin, uint8_t channel)
{
    ledcDetachPin(pin);
    ledcWrite(channel, 0);
}

void speaker(){
  for (int thisNote = 0; thisNote < (sizeof(Pirates_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / Pirates_duration[thisNote];//convert duration to time delay
    tone(BUZZER_PIN, Pirates_note[thisNote], noteDuration, BUZZER_CHANNEL);
    int pauseBetweenNotes = noteDuration * 0.5; //Here 1.05 is tempo, increase to play it slower
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
  }
}

//wifi stuff
const  char*  ssid  =  "NDL_24G";              //  name  of  local  WiFi  network  in  the  NDL
const  char*  password  =  "RT-AC66U";
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
}

//IR stuff
int IRSensor[6] = {13,12,14,27,26,25};
int IRSensorSize= 6;

uint8_t dnum; // screen id
uint16_t pos = 0;

struct logo {
  const uint8_t *binary;
  String text;
  String text1;
  const uint8_t *font;
  int margin_x;
  int margin_y;
  int margin1_x;
  int margin1_y;
};

logo logoArray[LOGO_COUNT] = {
  { key_icon , "KEYS", "", u8g2_font_ncenB18_tr, 40, 25, 0, 0 },
  { bag_icon , "BAG", "", u8g2_font_ncenB24_tr, 42, 30, 0, 0 },
  { bottle_icon , "BOTTLE", "", u8g2_font_ncenB14_tr, 40, 25, 0, 0 },
  { glasses_icon , "GLASSES", "", u8g2_font_ncenB14_tr, 40, 24, 0, 0 },
  { headphones_icon , "HEAD", "PHONES", u8g2_font_ncenB14_tr, 36, 15, 36, 31},
  { jacket_icon , "JACKET", "", u8g2_font_ncenB14_tr, 40, 25, 0, 0 },
  { money_icon , "MONEY", "", u8g2_font_ncenB14_tr, 40, 23, 0, 0 },
  { power_icon , "POWER", "BANK", u8g2_font_ncenB14_tr, 40, 16, 40, 32 },
  { umbrella_icon , "UBMBRELLA", "", u8g2_font_ncenB10_tr, 30, 22, 0, 0 }
};

struct display {
  U8G2 *display;
  uint8_t i2cnum;
  uint8_t width;
  uint8_t height;
};

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display128x32(U8G2_R0, U8X8_PIN_NONE);

#define NUMSCREENS 4

display OLEDS[NUMSCREENS] = {
/*{ &type, TCA I2C index, orientation, shared },*/
  { &display128x32, 0 },
  { &display128x32, 1 },
  { &display128x32, 2 },
  { &display128x32, 3 }
};


/* I2C multiplexer controls */
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}

void displayScreens(){
  for(uint8_t i=0;i<NUMSCREENS;i++) {
      logo curLogo = logoArray[i];
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
  }
}

void detectObjects(){
  //Serial.println("");
  for (int i = 0; i < IRSensorSize; i++){
    //Serial.print(i);  
    if (digitalRead(IRSensor[i])){
      //Serial.print(" clear  ");                        // D1 Mini: turns the LED *on*
    }
    else{
      //Serial.print(" OBSTACLE!  ");                                   // D1 Mini: turns the LED *off*
    }
  }
}

bool NFC() {
  //Serial.println("Starting NFC read");
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
  {
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1) == "7C 18 0F 49") //change here the UID of the card/cards that you want to give access
    {
      Serial.println("Authorized access");
      Serial.println();
      return true
    }
   
    else   {
      Serial.println(" Access denied");
    }
    return false
  }
  //Show UID on serial monitor
  
}

TaskHandle_t Task1;

int counter = 0;

void Task1code( void * pvParameters ){
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
  
    for(;;){
      timeClient.update();
      //Serial.print("running task1 on core: ");
      //Serial.println(xPortGetCoreID());
      counter++;
      delay(1000);
      if (NFC()){
        //speaker();
      }
    } 
}


void setup()   {
  Serial.begin(115200);
  while (!Serial);
  Wire.begin();

  for (int i = 0; i <IRSensorSize; i++){
    pinMode (IRSensor[i], INPUT); // sensor pin INPUT
  }
  

  initWiFi();
  timeClient.begin();
  for(uint8_t i=0;i<NUMSCREENS;i++) {
    tcaselect(OLEDS[i].i2cnum);
    OLEDS[i].display->begin();
    OLEDS[i].width = OLEDS[i].display->getDisplayWidth();
    OLEDS[i].height = OLEDS[i].display->getDisplayHeight();
  }
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  
  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
  delay(500);
}

void loop() {
  detectObjects();
  displayScreens();
  //Serial.print("running task2 on core: ");
  //Serial.println(xPortGetCoreID());
  Serial.println(counter);
  delay(1000);

}
