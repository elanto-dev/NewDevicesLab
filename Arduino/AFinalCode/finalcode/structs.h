#include "icons.h"
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

#define LOGO_COUNT 9
#define LOGO_HEIGHT 32
#define LOGO_WIDTH 32
#define NUMSCREENS 4

class Category{ 
  private:
    String catName = "";
    int screenIndex = -1;
  public:
    void setName(String cName){
      catName = cName;
    }
    void setScreenIndex(int cIndex){
      screenIndex = cIndex;
    }
    String getName(){
      return catName;
    }
    int getScreenIndex(){
      return screenIndex;
    }
};

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

display OLEDS[NUMSCREENS] = {
/*{ &type, TCA I2C index, orientation, shared },*/
  { &display128x32, 0 },
  { &display128x32, 1 },
  { &display128x32, 2 },
  { &display128x32, 3 }
};
