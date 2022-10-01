int IRSensor[4] = {12,13,14,27}; // set pin for ir sensor
int IRSensorSize= 4;

void setup() 
{
  for (int i = 0; i < IRSensorSize; i++){
    pinMode (IRSensor[i], INPUT); // sensor pin INPUT
  }
  
  Serial.begin(115200);
}

void loop()
{
  for (int i = 0; i < IRSensorSize; i++){
    Serial.println(i);  
    if (digitalRead(IRSensor[i])){
      Serial.print("clear");                        // D1 Mini: turns the LED *on*
    }
  
    else{
      Serial.print("OBSTACLE!");                                   // D1 Mini: turns the LED *off*
    }
  }


  delay(2000);
}
