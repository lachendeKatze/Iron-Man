#include <Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

#define faceplateServoPin 9

long currentMillis, previousMillis, switchedAtMillis;
Servo faceplateServo;
boolean faceplateUp;
   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

uint16_t currentLux, previousLux;

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup(void) {
  Serial.begin(9600);

  faceplateServo.attach(faceplateServoPin);
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  currentLux = 300;
  previousLux = currentLux;
  // Now we're ready to get readings!
  faceplateUp = false;
}

void loop(void) {
  
  uint16_t r, g, b, c, colorTemp;
  tcs.getRawData(&r, &g, &b, &c);
  currentLux = tcs.calculateLux(r, g, b);
  if ((previousLux-currentLux) > 2500) { previousLux = currentLux; }
  // Serial.print("currentLux: "); Serial.print(currentLux, DEC); Serial.print(" previusLux: " ); Serial.println(previousLux, DEC);

  if ( (currentLux - previousLux) > 2500 ) {
    if (faceplateUp) {
      // Serial.println("faceplate down");
      faceplateUp = false;
      faceplateServo.write(0);
    }
    else{
     //  Serial.println("faceplate up");
      faceplateUp = true;
      faceplateServo.write(120);
    }
    previousLux = currentLux;
  }

  // if ((previousLux-currentLux) > 2500) { previousLux = currentLux; }
  
}



