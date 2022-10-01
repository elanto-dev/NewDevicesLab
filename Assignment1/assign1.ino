#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include <LOLIN_I2C_MOTOR.h>
#include <Wire.h>
#include "paj7620.h"
#include  <ESP8266WiFi.h>
#include  <ESP8266WebServer.h>
#include  <ESP8266mDNS.h>

#define  LISTEN_PORT  80

#define OLED_RESET 1 //GPIO1
#define GES_REACTION_TIME 500
#define GES_ENTRY_TIME 800
#define GES_QUIT_TIME 1000
#define I2C_ADDRESS 0x43
#define I2C_ADDRESS2 0x44

const  char*  ssid  =  "NDL_24G";              //  name  of  local  WiFi  network  in  the  NDL
const  char*  password  =  "RT-AC66U";    //  the  password  of  the  WiFi  network

MDNSResponder  mdns;
ESP8266WebServer  server(LISTEN_PORT);
String  webPage  =  "<h1>WiFi  LED  control</h1>";

int corY = 11;
int corX = 6;
int score = 0;
int scoreReward = 5;
int lastScoreMotorHasTurned = 0;
bool gameover = false;

class Bullet{
  private:
    int cordX = 0;
    int cordY = 0;
    bool inBound = true;

  public:
    bool isInBound(void){
      return inBound;
    }
    
    int getX(void){
      return cordX;
    }

    int getY(void){
      return cordY;
    }
    
    void move(void){
      if(willBeInBounds()){
        cordX = cordX + 2;
      }else{
        inBound = false;
      }
    }

    bool willBeInBounds(void){
      if(cordX + 3 > 62){
        return false;
      }
      return true;
    }

    void notInBound(void){
      inBound = false;
    }

    void shoot(void){
      cordX = corX;
      cordY = corY+3;
    }
};

class Astroid{
  private:
    int radius = random(2,5);
    int health = radius;
    int cordY = random(10 + radius, 46 - radius);
    int cordX = 62 - radius;
    bool inBound = true;
    bool killed = false;

  public:
    bool isInBound(void){
      return inBound;
    }
    
    int getX(void){
      return cordX;
    }

    int getY(void){
      return cordY;
    }

    int getR(void){
      return radius;
    }

    bool wasKilled(void){
      return killed;
    }
    
    void move(void){
      if(!killed && willBeInBounds()){
        cordX = cordX - 1;
      }else{
        inBound = false;
      }
    }

    bool willBeInBounds(void){
      if(cordX - 1 - radius < 1){
        score = score + 1;
        return false;
      }
      return true;
    }

    void reduceHealth(void){
      health = health - 1;
      radius = health;
      if(health == 1){
        killed = true;
        score = score + 1;
      }
    }

    void notInBound(void){
      inBound = false;
    }
};

Adafruit_SSD1306 display(OLED_RESET);


const int bulletArraySize = 7;
Bullet *bullets[bulletArraySize];

const int astroidArraySize = 4;
Astroid *astroids[astroidArraySize];

LOLIN_I2C_MOTOR motor;



void setup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C address
  delay(1000);

  while (motor.PRODUCT_ID != PRODUCT_ID_I2C_MOTOR) //wait motor shield ready.
  {
    motor.getInfo();
  }

  for(int i = 0; i < bulletArraySize ; i++){
    bullets[i] = new Bullet();
    bullets[i]->notInBound();
  }
  for(int i = 0; i < astroidArraySize ; i++){
    astroids[i] = new Astroid();
    astroids[i]->notInBound();
  }

  Serial.begin(9600);
  uint8_t error = paj7620Init( ) ;  // initialize Paj7620 registers
  if(error){
    Serial.print("INIT ERROR,CODE: ") ;
    Serial.println() ;
  }

  WiFi.begin(ssid,  password);              //  make  the  WiFi  connection
  Serial.println("Start  connecting.");
  while  (WiFi.status()  !=  WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected  to  ");
  Serial.print(ssid);
  Serial.print(".  IP  address:  ");
  Serial.println(WiFi.localIP());
  if  (mdns.begin("esp8266",  WiFi.localIP()))  {
    Serial.println("MDNS  responder  started");
  }
  //  make  handlers  for  input  from  WiFi  connection
  server.on("/",  [](){
    webPage  =  "<h1>Spaceship game control</h1>";
    webPage  +=  "<p>Reset game  <a  href=\"resetgame\">";
    webPage  +=  "<button  style=\"background-color:red;color:white;\">";
    webPage  +=  "reset</button></a></p>"; 
    
    webPage  +=  "<p id=\"score\">Set score: ";
    webPage  += scoreReward;
    webPage  += "<a  href=\"reduce\">";
    webPage  +=  "<button style=\"background-color:#98AFC7;color:black;\">";
    webPage  +=  "DOWN</button></a>"; 
    webPage  += "<a  href=\"increase\">";
    webPage  +=  "<button style=\"background-color:#7FFFD4;color:black;\">";
    webPage  +=  "UP</button></a></p>";
  server.send(200,  "text/html",  webPage);
  });
  server.on("/increase",  [](){
    scoreReward = scoreReward +1;
    webPage  =  "<h1>Spaceship game control</h1>";
    webPage  +=  "<p>Reset game  <a  href=\"resetgame\">";
    webPage  +=  "<button  style=\"background-color:red;color:white;\">";
    webPage  +=  "reset</button></a></p>"; 
    
    webPage  +=  "<p id=\"score\">Set score: ";
    webPage  += scoreReward;
    webPage  += "<a  href=\"reduce\">";
    webPage  +=  "<button style=\"background-color:#98AFC7;color:black;\">";
    webPage  +=  "DOWN</button></a>"; 
    webPage  += "<a  href=\"increase\">";
    webPage  +=  "<button style=\"background-color:#7FFFD4;color:black;\">";
    webPage  +=  "UP</button></a></p>";
    server.send(200,  "text/html",  webPage);
    Serial.println(scoreReward);
    delay(1000);
  });
  server.on("/reduce",  [](){
    if(scoreReward > 1){
      scoreReward = scoreReward - 1;
    }
    webPage  =  "<h1>Spaceship game control</h1>";
    webPage  +=  "<p>Reset game  <a  href=\"resetgame\">";
    webPage  +=  "<button  style=\"background-color:red;color:white;\">";
    webPage  +=  "reset</button></a></p>"; 
    
    webPage  +=  "<p id=\"score\">Set score: ";
    webPage  += scoreReward;
    webPage  += "<a  href=\"reduce\">";
    webPage  +=  "<button style=\"background-color:#98AFC7;color:black;\">";
    webPage  +=  "DOWN</button></a>"; 
    webPage  += "<a  href=\"increase\">";
    webPage  +=  "<button style=\"background-color:#7FFFD4;color:black;\">";
    webPage  +=  "UP</button></a></p>";
    server.send(200,  "text/html",  webPage);
    Serial.println(scoreReward);
    delay(1000);
  });
    server.on("/resetgame",  [](){
    webPage  =  "<h1>Spaceship game control</h1>";
    webPage  +=  "<p>Reset game  <a  href=\"resetgame\">";
    webPage  +=  "<button  style=\"background-color:red;color:white;\">";
    webPage  +=  "reset</button></a></p>"; 
    
    webPage  +=  "<p id=\"score\">Set score: ";
    webPage  += scoreReward;
    webPage  += "<a  href=\"reduce\">";
    webPage  +=  "<button style=\"background-color:#98AFC7;color:black;\">";
    webPage  +=  "DOWN</button></a>"; 
    webPage  += "<a  href=\"increase\">";
    webPage  +=  "<button style=\"background-color:#7FFFD4;color:black;\">";
    webPage  +=  "UP</button></a></p>";
    resetGame();
  server.send(200,  "text/html",  webPage);
  });
  server.begin();              //  start  the  server  for  WiFi  input
  Serial.println("HTTP  server  started");
}

void loop(){
  server.handleClient();
  display.clearDisplay( ) ;
  display.setCursor(0 , 0) ;
  display.setTextColor(WHITE) ;
  display.setTextSize( 1 ) ;
  display.print("score:  ");
  display.print(score);
  testdrawrect();
  if(gameover){
    display.setCursor(3,11);
    display.println("GAME OVER!");
    display.println("press reset");
  }
  else{
    drawShip();
    moveAllBullets();
    moveAllAstroids();
    checkCollision();
    checkShot(); 
    uint8_t data = 0 ,data1=  0, error;
    error= paj7620ReadReg(I2C_ADDRESS,  1 , &data); // Read gesture result .
    int bulletIndex = emptyBulletIndex();
    int astroidIndex = emptyAstroidIndex();
    if(astroidIndex < astroidArraySize && astroidDistance()){
      astroids[astroidIndex] = new Astroid;
    }
    
    if( !error){
      switch(data){
        case GES_RIGHT_FLAG:
          Serial.println("Right");
          if(bulletIndex < bulletArraySize){
            shootBullet(bulletIndex);
          }
          break;
        case GES_LEFT_FLAG:
          Serial.println("Left");
          if(bulletIndex < bulletArraySize){
            shootBullet(bulletIndex);
          }
          break;
        case GES_UP_FLAG:
          Serial.println("Up");
          if(corY >= 14){
            corY = corY - 4;
          }
          break;
        case GES_DOWN_FLAG:
          Serial.println("Down");
          if(corY <= 35){
            corY = corY + 4;
          }
          break;
        case GES_FORWARD_FLAG:
          Serial.println("Forward"); 
          delay(GES_QUIT_TIME);
          break;
        case GES_BACKWARD_FLAG:
          Serial.println("Backward");
          delay(GES_QUIT_TIME);
          break;
        case GES_CLOCKWISE_FLAG:
          Serial.println("Clockwise");
          break;
        case GES_COUNT_CLOCKWISE_FLAG:
          Serial.println("anti-clockwise"); 
          break;
        default:
          paj7620ReadReg(I2C_ADDRESS2, 1, &data);
          if(data==GES_WAVE_FLAG){
            Serial.println("wave");
            if(bulletIndex < bulletArraySize){
              shootBullet(bulletIndex);
            }
          }else{
            Serial.print("");
          }
          break;
      }
    }
  
   
    motor.changeFreq(MOTOR_CH_A, 1000);
    motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_CCW);
  
   if(lastScoreMotorHasTurned < score && score % scoreReward == 0 && score != 0){
      motor.changeDuty(MOTOR_CH_A, 100);
      delay(1000);
      motor.changeDuty(MOTOR_CH_A, 0);
      lastScoreMotorHasTurned = score;
   }
  }  
  display.display();
  delay(150);
}

int emptyBulletIndex(void){
  for(int i = 0; i < bulletArraySize ; i++){
    if (bullets[i]->isInBound() == false) {
      return i;
    }
  }
  return bulletArraySize;
}

int emptyAstroidIndex(void){
  for(int i = 0; i < astroidArraySize ; i++){
    if (astroids[i]->isInBound() == false || astroids[i]->wasKilled() == true) {
      return i;
    }
  }
  return astroidArraySize;
}

void moveAllBullets(void){
  for(int i = 0; i < bulletArraySize ; i++){
    if (bullets[i]->isInBound()) {
        bullets[i]->move();
        int cordX =  bullets[i]->getX();
        int cordY =  bullets[i]->getY();
        display.drawLine(cordX, cordY, cordX + 1, cordY, WHITE);
    }
  }
}

void moveAllAstroids(void){
  for(int i = 0; i < astroidArraySize ; i++){
    if (astroids[i]->isInBound()) {
        astroids[i]->move();
        display.fillCircle(astroids[i]->getX(), astroids[i]->getY(), astroids[i]->getR(), WHITE);
    }
  }
}

void testdrawrect(void) {
   display.drawRect(0, 9, display.width(), display.height()-9, WHITE);
}

void drawBullets(){
  //display.drawLine(x1, y1, x2, y2, WHITE);
}

void drawShip(void){
    display.drawPixel(corX-3, corY, WHITE);
    display.drawPixel(corX-3, corY+2, WHITE);
    display.drawPixel(corX-3, corY+4, WHITE);
    display.drawPixel(corX-3, corY+6, WHITE);
    display.drawPixel(corX-2, corY+1, WHITE);
    display.drawPixel(corX-2, corY+2, WHITE);
    display.drawPixel(corX-2, corY+4, WHITE);
    display.drawPixel(corX-2, corY+5, WHITE);
    display.drawPixel(corX-1, corY+2, WHITE);
    display.drawPixel(corX-1, corY+3, WHITE);
    display.drawPixel(corX-1, corY+4, WHITE);
    display.drawPixel(corX, corY+3, WHITE);
}

bool astroidDistance(void){
  int distance = 62;
  for(int i = 0; i < astroidArraySize ; i++){
    if(astroids[i]->isInBound() && !astroids[i]->wasKilled()){
      int curDistance = 62 - astroids[i]->getX() - astroids[i]->getR();
      if(curDistance < distance){
        distance = curDistance;
      }
    }
  }
  if (distance > 12){
    return true;
  }
  return false;
}

// BROKEN!!!!!!!!!!!!!!!
void checkShot(void){
  for(int b = 0; b < bulletArraySize; b++){
    int bulletFrontX = bullets[b]->getX() + 1;
    int bulletY = bullets[b]->getY();
    for(int a = 0; a < astroidArraySize; a++){
      int astroidRadius = astroids[a]->getR();
      int astroidFrontX = astroids[a]->getX() - astroidRadius / 2;
      int astroidBackX = astroids[a]->getX() + astroidRadius / 2;
      int astroidFrontY = astroids[a]->getY();
      if((bulletFrontX >= astroidFrontX && bulletFrontX <= astroidBackX) && (astroidFrontY - astroidRadius <= bulletY) && (bulletY <= astroidFrontY + astroidRadius)){
        bullets[b]->notInBound();
        astroids[a]->reduceHealth();
      }
    }
  }
  
}

void checkCollision(void){
  int topShip = corY - 3;
  int bottomShip = corY + 3;
  int midShip = corY;
  for(int a = 0; a < astroidArraySize; a++){
    int astroidRadius = astroids[a]->getR();
    int astroidFrontX = astroids[a]->getX() - astroidRadius / 2;
    int astroidFrontY = astroids[a]->getY();
    int astroidBackX = astroids[a]->getX() + astroidRadius / 2;
    if(((topShip >= astroidFrontY -astroidRadius && topShip <= astroidFrontY + astroidRadius && corX-2 >= astroidFrontX  && corX-2 <= astroidBackX)   //check top ship
            ||(midShip >= astroidFrontY -astroidRadius && midShip <= astroidFrontY +astroidRadius && corX >= astroidFrontX  && corX <= astroidBackX)  //check collision with mid ship
            ||(bottomShip >= astroidFrontY -astroidRadius && bottomShip <= astroidFrontY +astroidRadius && corX-2 >= astroidFrontX  && corX-2 <= astroidBackX)  //check bottom of the ship 
           )
      ){
         Serial.println("Collision!!!!!!!!!!!!!!!!!!!!!!");
         gameover = true;
    }
  }
}

void resetGame(void){
    for(int i = 0; i < bulletArraySize ; i++){
    bullets[i] = new Bullet();
    bullets[i]->notInBound();
  }
  for(int i = 0; i < astroidArraySize ; i++){
    astroids[i] = new Astroid();
    astroids[i]->notInBound();
  }
  
  corY = 11;
  corX = 6;
  score = 0;
  gameover = false;
  lastScoreMotorHasTurned = 0;
}

void shootBullet(int bulletIndex){
     bullets[bulletIndex] = new Bullet;
     bullets[bulletIndex] -> shoot();
}
