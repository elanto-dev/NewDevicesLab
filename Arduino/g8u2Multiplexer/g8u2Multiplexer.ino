#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "icons.h"

#define TCAADDR 0x70 // I2C Multiplexer ADDR
#define LOGO_HEIGHT 32
#define LOGO_WIDTH 32
#define LOGO_COUNT 9

uint8_t dnum; // screen id

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

void setup()   {
  Serial.begin(115200);
  while (!Serial);
  Wire.begin();

  uint16_t pos = 0;

  for(uint8_t i=0;i<NUMSCREENS;i++) {
    tcaselect(OLEDS[i].i2cnum);
    OLEDS[i].display->begin();
    OLEDS[i].width = OLEDS[i].display->getDisplayWidth();
    OLEDS[i].height = OLEDS[i].display->getDisplayHeight();
  }
}

void loop() {
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
