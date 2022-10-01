#include <NTPClient.h>

#include "Arduino.h"
#include "pitches.h"
#include "themes.h"

int categoryIndex(String name){
  Serial.println("catIndex");
  Serial.println(name);
	if(name == "Key"){ return 0; }
	if(name == "Bag"){ return 1; }
	if(name == "Bottle"){ return 2; }
	if(name == "Glasses"){ return 3; }
	if(name == "Headphones"){ return 4; }
	if(name == "Jacket"){ return 5; }
	if(name == "Money"){ return 6; }
	if(name == "Power bank"){ return 7; }
	if(name == "Umbrella"){ return 8; }
	else { return -1; }
}

void noTone(uint8_t pin, uint8_t channel)
{
    ledcDetachPin(pin);
    ledcWrite(channel, 0);
}

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

void playPirates(int pin, int channel) {
  for (int thisNote = 0; thisNote < (sizeof(Pirates_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / Pirates_duration[thisNote];//convert duration to time delay
    tone(pin, Pirates_note[thisNote], noteDuration, channel);
    int pauseBetweenNotes = noteDuration * 0.5; //Here 1.05 is tempo, increase to play it slower
    delay(pauseBetweenNotes);
    noTone(pin, channel);
  }
}

struct datetime {   
  int second = 0;
  int minute = 0; 
  int hour = 0;
  int day = 0;
  int month = 0; 
  int year = 0;
};

datetime getCurrentTime(NTPClient timeClient){
  timeClient.update();
  
  unsigned long epochTime = timeClient.getEpochTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();    
  int currentSecond = timeClient.getSeconds();
 
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int currentDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1; 
  int currentYear = ptm->tm_year+1900;

  datetime dt = {};
  dt.second = currentSecond;
  dt.minute = currentMinute;
  dt.hour = currentHour;
  dt.day = currentDay;
  dt.month = currentMonth;
  dt.year = currentYear;
  return dt;
}
