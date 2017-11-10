#include "Adafruit_NeoPixel.h"

#define STRIP_PIN 6
#define NUMPIXELS 18 
#define PUSHBUTTON 2

#define LIGHTNING_START  0  //The Whole lightning strip
#define LIGHTNING_STOP   8
#define LIGHTNING1_START 0  //The first half that has a short flash
#define LIGHTNING1_STOP  3
#define LIGHTNING2_START 4  //The Second half of the strap that has longer flash
#define LIGHTNING2_STOP  8
#define SUNSET_START     9  //Sunset strip 
#define SUNSET_STOP      17

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS,STRIP_PIN,NEO_GRB + NEO_KHZ800);
  
enum STATE {
 STATE_FADE_SUNSET,
 STATE_FADE_COLORS,
 STATE_LIGHTNING_AND_SUNSET,
 STATE_LIGHTNING_01,
 STATE_LIGHTNING_02,
 STATE_LIGHTNING_03,
 STATE_LIGHTNING_04,
 STATE_LIGHTNING_05,
 STATE_LIGHTNING_06,
 STATE_LIGHTNING_07,
 STATE_LIGHTNING_08,
 STATE_LIGHTNING_09,
 STATE_LIGHTNING_10
};

STATE currentState;
unsigned long time;
unsigned long count;
int brightness;
int lastFade;
bool t;

void setup() {
// Serial.begin(9600); //DEBUG output
 pinMode(2, INPUT);
 currentState = STATE_LIGHTNING_01;
 time = 0;
 count = 0;
 lastFade = 0;
 brightness = 255;
 strip.begin();

 //make sure all pixels are off
 for(int i=0;i<NUMPIXELS;i++){
   strip.setPixelColor(i, strip.Color(0,0,0));
   strip.show();
 }
}

//Reset the brightness
void startFadeSunset() {
  //Make sure the lightning strip aren't on
  for(int i=LIGHTNING_START;i<LIGHTNING_STOP;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
    strip.show();
  }
  //reset brightness for fade
  brightness = 255;
  currentState = STATE_FADE_COLORS;
}

void fader(int16_t r1,int16_t g1,int16_t b1,int16_t r2,int16_t g2,int16_t b2, uint32_t time) {
  const uint8_t steps = 8;              // higher the steps the more course the transition steps=10 would be a 1 second pixel update
  const uint8_t scaler = 7;             // magic number defines the bit shifting of colors
  const uint16_t duration = 1 << steps; // milisecond duration  = 2^steps

  time = time >> steps;

  const int16_t rinc=((1 << scaler)/(double)time)*(r2-r1);
  const int16_t ginc=((1 << scaler)/(double)time)*(g2-g1);
  const int16_t binc=((1 << scaler)/(double)time)*(b2-b1);

  int32_t newr = (int32_t)r1 << scaler;
  int32_t newg = (int32_t)g1 << scaler;
  int32_t newb = (int32_t)b1 << scaler;

  for(uint32_t i = 0 ; i < time; i++) 
  {
    newr += rinc;
    newg += ginc;
    newb += binc;

    for(int i=SUNSET_START;i<=SUNSET_STOP;i++){
      strip.setPixelColor(i,newr >> scaler,newg >> scaler,newb >> scaler);
      strip.show();
    }

    delay(duration);
  }
}

void loop() {
  static int32_t newr;
  static int32_t newg;
  static int32_t newb;

  if(digitalRead(PUSHBUTTON))
    startFadeSunset();

  if(lastFade==1)
  {
    if(count == 0)
    {
      newr = 139;
      newg = 0;
      newb = 139;
      t = false;
      currentState = STATE_LIGHTNING_01;
    }

    count++;

    if(count % 50 == 0 || (newr < 20 && count % 5 == 0))
    {
      newr-=1;
      newb-=1;

      if(newr < 1)
      {
        for(int i=SUNSET_START;i<=SUNSET_STOP;i++){
          strip.setPixelColor(i,0,0,0);
          strip.show();
        }

        lastFade = 0;
      }

      for(int i=SUNSET_START;i<=SUNSET_STOP;i++){
        strip.setPixelColor(i,newr ,newg ,newb );
        strip.show();
      }
    }
  }

  switch(currentState){
    case STATE_FADE_COLORS:
      {
        fader(166,42,42, //Start Color
              140,50,0,  //End Color
              75000);    //Duration of fade

        fader(139,50,0,
              178,34,34,
              75000);

        fader(178,34,34,
              255,20,147,
              75000);

        fader(255,20,147,
              199,21,133,
              75000);

        fader(199,21,133,
              139,0,139,
              75000);

        lastFade = 1;
        count = 0;
        currentState = STATE_LIGHTNING_01;
      }
      break;
    case STATE_LIGHTNING_01:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING1_START;i<=LIGHTNING1_STOP;i++){
          strip.setPixelColor(i,255,255,225); 
          strip.show(); // This sends the updated pixel color to the hardware.
        }

        if(millis()-time > 110)
        {
          currentState = STATE_LIGHTNING_02;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_02:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }
//        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
//          strip.setPixelColor(i, strip.Color(0,0,0));
//          strip.show();
//        }

        if(millis()-time > 100)
        {
          currentState = STATE_LIGHTNING_03;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_03:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING2_START;i<=LIGHTNING2_STOP;i++){
          strip.setPixelColor(i,255,255,225); 
          strip.show();
        }

        if(millis()-time > 50)
        {
          currentState = STATE_LIGHTNING_04;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_04:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }

        if(millis()-time > 100)
        {
          currentState = STATE_LIGHTNING_05;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_05:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING2_START;i<=LIGHTNING2_STOP;i++){
          strip.setPixelColor(i,255,255,225); 
          strip.show();
        }

        if(millis()-time > 190)
        {
          currentState = STATE_LIGHTNING_06;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_06:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }

        for(int i=LIGHTNING2_START;i<=LIGHTNING2_STOP;i++){
          strip.setPixelColor(i,255,255,225); 
          strip.show();
        }

        if(millis()-time > 50)
        {
          currentState = STATE_LIGHTNING_07;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_07:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }

        if(millis()-time > 40)
        {
          currentState = STATE_LIGHTNING_08;
          time = 0;
        }
      }
      break;

    case STATE_LIGHTNING_08:
      {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING2_START;i<=LIGHTNING2_STOP;i++){
          strip.setPixelColor(i,255,255,225); 
          strip.show();
        }

        if(millis()-time > 50)
        {
          currentState = STATE_LIGHTNING_09;
          time = 0;
        }
      }
      break;
    case STATE_LIGHTNING_09: {
        if(time == 0 )
          time = millis();

        for(int i=LIGHTNING_START;i<=LIGHTNING_STOP;i++){
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }

        if(millis()-time > 9000)
        {
          currentState = STATE_LIGHTNING_01;
          time = 0;
        }
      }
      break;
  }
}

