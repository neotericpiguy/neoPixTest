#include "Adafruit_NeoPixel.h"

#define PIN 6
#define NUMPIXELS 4
#define PUSHBUTTON 2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS,PIN,NEO_GRB +
   NEO_KHZ800);

  
 enum STATE {
 STATE_FADE_SUNSET,
 STATE_FADE_COLORS,
 STATE_LIGHTNING
};

STATE currentState;
unsigned long time;
int brightness;
int switchState = 0;
int count = 0;
int lastState = 0;
int pushCount =0;

void setup() {
 pinMode(2, INPUT);
 currentState = STATE_LIGHTNING;
 brightness = 255;
 pixels.begin();
  
}

//This function will wait for milliseconds and read the 
//if you did readSwitchDelay(1000) it would wait 1000 milliseoncds or 1 second, BUT
//it would also check the push button for if some presses it
bool readSwitchDelay(int millis) {
  while(millis>0 && !digitalRead(PUSHBUTTON)) //keep delaying until a push button is pressed or the milliseconds run out
  {
    delay(1);
    millis--;
  }

  if(millis <= 0)
    return false;

  return true; //return true if button  was pushed
}

//Reset the brightness
void startFadeSunset() {
  //Make sure the lightning pixels aren't on
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show();
  }
  //reset brightness for fade
  brightness = 255;
  currentState = STATE_FADE_COLORS;
}

void fader(int16_t r1,int16_t g1,int16_t b1,int16_t r2,int16_t g2,int16_t b2, uint16_t time) {
  const uint8_t steps = 3;              // higher the steps the more course the transition steps=10 would be a 1 second pixel update
  const uint8_t scaler = 8;             // magic number defines the bit shifting of colors
  const uint16_t duration = 1 << steps; // milisecond duration  = 2^steps

  time = time >> steps;

  const int16_t rinc=((1 << scaler)/(double)time)*(r2-r1);
  const int16_t ginc=((1 << scaler)/(double)time)*(g2-g1);
  const int16_t binc=((1 << scaler)/(double)time)*(b2-b1);

  int32_t newr = (int32_t)r1 << scaler;
  int32_t newg = (int32_t)g1 << scaler;
  int32_t newb = (int32_t)b1 << scaler;

  for(int i = 0 ; i < time; i++) {
    newr += rinc;
    newg += ginc;
    newb += binc;

    pixels.setPixelColor(0,newr >> scaler,newg >> scaler,newb >> scaler);
    pixels.setPixelColor(1,newr >> scaler,newg >> scaler,newb >> scaler);
    pixels.show();

    delay(duration);
  }
}

void loop() {
  time = millis();
  switch(currentState){
    case STATE_FADE_COLORS:
      {
        fader(165,42,42, //Start Color
              139,50,0,  //End Color
              60000);    //Duration of fade

        fader(139,50,0,
              178,34,34,
              60000);

        fader(178,34,34,
              255,20,147,
              60000);

        fader(255,20,147,
              199,21,133,
              60000);

        fader(199,21,133,
              139,0,139,
              60000);

        fader(139,0,139,
              0,0,0,
              60000);

        currentState = STATE_LIGHTNING;
      }
      break;
    case STATE_FADE_SUNSET: //Not using this state anymore
      if(time % 100 == 0){  
        brightness--;
        pixels.setPixelColor(0,(brightness*250/255),(brightness*38/255),(brightness*0/255));
        pixels.setPixelColor(1,(brightness*250/255),(brightness*38/255),(brightness*0/255));
        pixels.show();

        if(brightness==5)
        {
          currentState = STATE_LIGHTNING;
        }
      }
      break;
    case STATE_LIGHTNING:
      while(true)
      {
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(0,255,255,225); 
          pixels.setPixelColor(1,200,200,200);
          //pixels.setPixelColor(2,140,140,225);
          pixels.show(); // This sends the updated pixel color to the hardware.
        }

        //if DUring the 110 millisecond delay the user pushes the button the readSwitchDelay function will exit and 
        //run the startFadeSunset function
        //  The start FadeSunset will reset the bright ness to max and change the state to STATE_FADE_SUNSET
        if(readSwitchDelay(110))
        {
          startFadeSunset();
          break;
        }

        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          pixels.show();
        }
        if(readSwitchDelay(100))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(2,255,255,225); 
          pixels.setPixelColor(3,200,200,200);
          pixels.show();
        }
        if(readSwitchDelay(50))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          pixels.show();
        }
        if(readSwitchDelay(100))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(2,255,255,225); 
          pixels.setPixelColor(3,200,200,200);
          pixels.show();
        }
        if(readSwitchDelay(190))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          pixels.show();
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(2,255,255,225); 
          pixels.setPixelColor(3,200,200,200);
          pixels.show();
        }
        if(readSwitchDelay(50))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          pixels.show();
        }
        if(readSwitchDelay(40))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(2,255,255,225); 
          pixels.setPixelColor(3,200,200,200);
          pixels.show();
        }
        if(readSwitchDelay(50))
        {
          startFadeSunset();
          break;
        }
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          pixels.show();
        }
        if(readSwitchDelay(9000))
        {
          startFadeSunset();
          break;
        }
      }
      break;
  }
}

