#include <Wire.h>
#include <Servo.h>
#include <Adafruit_MPR121.h>
#include <Adafruit_NeoPixel.h>

/**
* Iron Man Project - arduino control code - this version should be compatible with the Uno and 101
* 9/24/2016.
*/

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V or 5 V DC
   Connect GROUND to common ground */

// customize for your hardware here
#define HELMETPIN 9
#define DOWNANGLE 0
#define UPANGLE 120
#define UP 1
#define DOWN 0
#define REACTORPIN 6
#define REACTORPIXELS 24

class Helmet
{
  int helmetPin;
  boolean helmetUp;
  Servo helmetServo;
  long lastSwitched; // possible variable to prevent rapid change in helmet state?
  int downAngle;
  int upAngle;

  int directionality;
  long lastFlipped;
  boolean flip, flipping;

  int currentPos;
  long lastMoved;
  
  public:

  Helmet(int pinNumber, boolean helmetState, int down, int up)
  {
    helmetPin = pinNumber;
    helmetUp = helmetState;
    downAngle = down;
    upAngle = up;
  } 

  int getHelmetPin() {return helmetPin;}
  void setHelmetPin(int pinNumber) {helmetPin = pinNumber;}

  boolean getHelmetUp() {return helmetUp;}
  void setHelmetUp(boolean helmetState) {helmetUp = helmetState;}

  long getLastSwitched() {return lastSwitched;}
  void setLastSwitched(long timeSwitched) {lastSwitched = timeSwitched;}

  int getDownAngle() {return downAngle;}
  void setDownAngle(int angle) {downAngle = angle;}

  int getUpAngle() {return upAngle;}
  void setUpAngle(int angle){upAngle = angle;}

  void init()
  {
    // attach the srvo and set the initial position to up or open.
    helmetServo.attach(helmetPin);
    helmetServo.write(upAngle);
    lastSwitched = millis();
    flip = false;
    flipping = false;
    lastFlipped = 0;
    lastMoved = 0;
  }


  void flipFaceplate() 
  {
    // timing block to protect against chatter/bounce
    if ((millis()-lastFlipped) > 500 )
    {
      lastFlipped = millis();
      if (flip) 
      { 
        flipping = true; 
        currentPos = 0;
        directionality = UP;
      }
      else 
      { 
        flipping = true;
        currentPos = 120;
        directionality = DOWN;  
      } 
      flip = !flip;
    }
  }
  
  void Update()
  {
    if ( flipping & (millis()-lastMoved) >= 15 )
    {
      lastMoved = millis();
      if ( directionality == UP ) 
      {
        currentPos = currentPos + 3;
      }
      else 
      {
       currentPos = currentPos - 3;
      }
       Serial.print("current position: ");Serial.print(currentPos);
       helmetServo.write(currentPos);
    }
    if ((currentPos >= upAngle) || (currentPos <= downAngle)) { flipping= false; }
    
  }
};


class ArcReactor
{
  int reactorPin;
  int reactorPixels;
  int reactorState;
  long lastSwitched;
  
  Adafruit_NeoPixel reactor;

    public:

    int getReactorPin() { return reactorPin; }
    void setReactorPin(int pinNumber) { reactorPin = pinNumber;}

    int getReactorPixels() {return reactorPixels;}
    void setReactorPixels(int pixelNumber) { reactorPixels = pixelNumber;}

    /**
     *   0 = OFF
     *   1 = ON
     *   2 = P1 - not sure what pattern to make at this time
     */
    int getReactorState() {return reactorState;}
    void setReactorState(int stateNumber) { reactorState = stateNumber;}
      
    ArcReactor(int pinNumber, int pixelNumber)
    {
      setReactorPin(pinNumber);
      setReactorPixels( pixelNumber);
      reactor = Adafruit_NeoPixel(getReactorPixels(), getReactorPin(), NEO_RGBW + NEO_KHZ800);
    }
    void init()
    { 
      reactor.begin();
      lastSwitched = millis();
    }

    void switchState()
    { 
      
      Serial.print("Switch State: ");Serial.println( getReactorState() );
      if ((millis()-lastSwitched)>500) 
      {  
         if (getReactorState() == 2) { setReactorState(0); }
         else { setReactorState(getReactorState()+1);}
         lastSwitched = millis();  
         switch(getReactorState())
         {
           case 0:
             off();
             break;
           case 1:
             on();
             break;
           case 2:
             // off();
             break;  
          }
       }
    }

    void off() 
    {
      for(int i=0;i<getReactorPixels();i++)
      {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        reactor.setPixelColor(i, reactor.Color(0,0,0)); 
        // reactor.show();
      }
      reactor.show();
    }
     
    void on() 
    {
      for(int i=0;i<getReactorPixels();i++)
      {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        reactor.setPixelColor(i, reactor.Color(0,0,255)); 
        // reactor.show();
      }
      reactor.show();
    }
    
    void scanner()
    {
      Serial.println("scanner selected");
    }

};

Helmet helmet(HELMETPIN,true,DOWNANGLE,UPANGLE);
ArcReactor arcReactor( REACTORPIN, REACTORPIXELS);

Adafruit_MPR121 touchControl = Adafruit_MPR121();
uint16_t currTouched = 0;

void setup(void) 
{
  Serial.begin(9600); // use for debugging
  helmet.init();
  arcReactor.init();
  if (!touchControl.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");  
}

void loop(void) 
{
  currTouched = touchControl.touched();
  Serial.print("touched: ");Serial.println(currTouched); 
  
  switch(currTouched) 
  {
     case 1:
      helmet.flipFaceplate();
      break;
     case 2: 
      // do nothing for now
      break;
     case 16:
      arcReactor.switchState();
      break;   
  }
  // check to see if the helmet face plate is being moved/flipping
  helmet.Update();
  
}
