/*
 * en is used for analog data from 0 to 255 (but is mapped, so 0-100 over serial comms)
 * in is used for digital - HIGH and LOW data, like instant h'bam... you know?
 * input: DIRECTIONspeed (e.g. N100 - north wheel move at 100% power) 
 * output: ToFNumber:Distance (e.g. 1:234 - ToF sensor 1 measures 234mm)
 */
//#include "Adafruit_VL53L0X.h" // Created by Adafruit - library for ToF
#include <Wire.h> // to communicate with i2c devices
#include <Adafruit_Sensor.h> // for compass
#include <Adafruit_HMC5883_U.h> // specifically for compass

// board 1 motor 1
const int en1A = 13;
const int in11 = 12;
const int in12 = 11;
// board 1 motor 2
const int en1B = 8;
const int in13 = 10;
const int in14 = 9;
// board 2 motor 3
const int en2A = 6;
const int in21 = 5;
const int in22 = 4;
// board 2 motor 4
const int en2B = 1;
const int in23 = 3;
const int in24 = 2;

// momentary switch stuff for ball detection
const int sw = 22;
int swOut = HIGH;
int swState;
int lastSwState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // CHANGE TO FIT

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345); // assign ID to compass

//// dual ToF setup (i2c address and shut down pins - to set custom address)
//#define LOX1_ADDRESS 0x30
//#define LOX2_ADDRESS 0x31
//#define SHT_LOX1 14
//#define SHT_LOX2 15
//// ToF objects
//Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
//Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
//
////Input and Output Data
//VL53L0X_RangingMeasurementData_t measure1; // ToF measurements
//VL53L0X_RangingMeasurementData_t measure2;
int nSpeed = 0; // specific motor speed (percentage)
int eSpeed = 0;
int sSpeed = 0;
int wSpeed = 0;
String inputString = ""; // holds serial comm from rPi
String outputString = ""; // sends ToF data to rPi
boolean stringComplete = false; 
int motNum = 1;

void serialInput()
  //Select correct motor driver and motor for control, and wheel direction (i.e. CW or ACW)
{
   while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    
    if (inChar == 'FL')
    {
      motNum = 1;
      stringComplete = true;

      if (inChar == '-')
      {
        digitalWrite(in11, LOW);
        digitalWrite(in12, HIGH);
      }
      else
      {
        digitalWrite(in11, HIGH);
        digitalWrite(in12, LOW);
      }
    }
    else if (inChar == 'FR')
    {
      motNum = 2;
      stringComplete = true;

      if (inChar == '-')
      {
        digitalWrite(in13, LOW);
        digitalWrite(in14, HIGH);
      }
      else
      {
        digitalWrite(in13, HIGH);
        digitalWrite(in14, LOW);
      }
    }
    else if (inChar == 'BL')
    {
      motNum = 3;
      stringComplete = true;
      
      if (inChar == '-')
      {
        digitalWrite(in21, LOW);
        digitalWrite(in22, HIGH);
      }
      else
      {
        digitalWrite(in21, HIGH);
        digitalWrite(in22, LOW);
      }
    }
    else if (inChar == 'BR')
    {
      motNum = 4;
      stringComplete = true;

      if (inChar == '-')
      {
        digitalWrite(in23, LOW);
        digitalWrite(in24, HIGH);
      }
      else
      {
        digitalWrite(in23, HIGH);
        digitalWrite(in24, LOW);
      }
    }
    else if (inChar == '\n') //remove if necessary
    {
      stringComplete = true;
    }
    else
    {
      inputString += inChar;
    }
  } 
}

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
}

void serialOutput() {
  // ToF Code  
//  lox1.rangingTest(&measure1, false); // change to 'true' to get debug data
//  lox2.rangingTest(&measure2, false);
//
//  // print sensor readings
//  Serial.print("1:");
//  if(measure1.RangeStatus != 4) {     // if not out of range
//    Serial.println(measure1.RangeMilliMeter);
//  } else {
//    Serial.println("out of range");
//  }
//  Serial.print("2:");
//  if(measure2.RangeStatus != 4) {
//    Serial.println(measure2.RangeMilliMeter);
//  } else {
//    Serial.println("out of range");
//  }

  // debounced momentary switch code - UNTESTED
  int reading = digitalRead(sw);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis()-lastDebounceTime > debounceDelay) {
    if (reading != swState) {
      swState = reading;
      if (swState == HIGH) {
        swOut = !swOut;
      }
    }
  }
  Serial.println(swState);
  lastSwState = reading;
}

void displaySensorDetails(void) // use in setup for debug or info if needed
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup()
{
  Serial.begin(115200);
  inputString.reserve(200);
  outputString.reserve(200);
  
//  // wait for serial ready, shut down and reset for separate addresses
//  while (! Serial) { delay(1); }
//  pinMode(SHT_LOX1, OUTPUT);
//  pinMode(SHT_LOX2, OUTPUT);
//  digitalWrite(SHT_LOX1, LOW);
//  digitalWrite(SHT_LOX2, LOW);
//  setID();

  // initialise HMC5883L
  while (!mag.begin())
  {
    Serial.println("Could not find a valid HMC5883L sensor");
    delay(500);
  }

//displaySensorDetails(); // uncomment for debug  
  
  // motor driver outputs and momentary switch input
  pinMode(en1A, OUTPUT);
  pinMode(en2A, OUTPUT);
  pinMode(en1B, OUTPUT);
  pinMode(en2B, OUTPUT);
  pinMode(in11, OUTPUT);
  pinMode(in21, OUTPUT);
  pinMode(in12, OUTPUT);
  pinMode(in22, OUTPUT);
  pinMode(in13, OUTPUT);
  pinMode(in23, OUTPUT);
  pinMode(in14, OUTPUT);
  pinMode(in24, OUTPUT);
  pinMode(sw, INPUT);
}

void loop()
{
  serialInput();
  if (stringComplete) {
    if (motNum == 1) {
    nSpeed = map(inputString.toInt(),0,100,0,255); //Converts 0-100 values to 0-255
    }
    else if (motNum == 2) {
    eSpeed = map(inputString.toInt(),0,100,0,255);
    }
    else if (motNum == 3) {
    sSpeed = map(inputString.toInt(),0,100,0,255);
    }
    else if (motNum == 4) {
    wSpeed = map(inputString.toInt(),0,100,0,255);
    }
    
    analogWrite(en1A, nSpeed);
    analogWrite(en1B, eSpeed);
    analogWrite(en2A, sSpeed);
    analogWrite(en2B, wSpeed);
    
    Serial.print(nSpeed); //For testing only! Delete to prevent unnecessary undefined serial comms to rPi
    Serial.print(eSpeed);
    Serial.print(sSpeed);
    Serial.println(wSpeed);
    inputString = "";
    //Add lines to clear xSpeed here if value retention not wanted
    stringComplete = false;

  // create new compass sensor events
  sensors_event_t event; 
  mag.getEvent(&event);

  // Calculate heading
  float heading = atan2(event.magnetic.y, event.magnetic.x); // z axis points up
  float declinationAngle = (11.0 + (37.0 / 60.0)) / (180 / M_PI); // set declination angle
  heading += declinationAngle;

  // Correct for when signs are reversed
  if(heading < 0)
    heading += 2*PI;
  // Check for wrap due to addition of declination
  if(heading > 2*PI)
    heading -= 2*PI;
  // Convert radians to degrees
  float headingDegrees = heading * 180/M_PI; 

  Serial.println(headingDegrees); // degrees to north

  serialOutput(); // currently only momentary switch, no ToF
  }
}
