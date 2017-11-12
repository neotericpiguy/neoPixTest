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

//0-4
//5-8
//9-12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS,STRIP_PIN,NEO_GRB + NEO_KHZ800);
  
enum STATE {
 STATE_FADE_SUNSET,
 STATE_FADE_COLORS,
 STATE_LIGHTNING_AND_SUNSET,
 STATE_LIGHTNING,
};

struct LightningDef{
  int startPixel;
  int stopPixel;
  int r;
  int g;
  int b;
  unsigned long duration;
};

int currentState;
unsigned long count;
int brightness;
int lastFade;
bool t;

LightningDef lightning[] = {
  {0, 4,  135, 206, 250, 20},
  {0, 12, 0,   0,   0,   109},
  {0, 4,  135, 206, 250, 44},
  {0, 12, 0,   0,   0,   89},
  {0, 4,  135, 206, 250, 29},
  {0, 12, 0,   0,   0,   23000}, //end of flash 1
  {0, 4,  135, 206, 250, 35},
  {0, 12, 0,   0,   0,   150},
  {0, 4,  135, 206, 250, 15},
  {0, 12, 0,   0,   0,   300},
  {0, 2,  135, 206, 255, 290},
  {0, 12, 0,   0,   0,   130},
  {0, 2,  135, 206, 255, 50},
  {0, 12, 0,   0,   0,   28000}, //end of flash 2
};

void setup() {
// Serial.begin(9600); //DEBUG output
 pinMode(2, INPUT);
 currentState = STATE_LIGHTNING;
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

void setLightning(int start, int stop, int16_t r, int16_t g, int16_t b, unsigned long duration)
{
  static unsigned long time = 0;

  if (time == 0)
  {
    time = millis();

    for (int i = start ; i <= stop; i++) {
      strip.setPixelColor(i, r, g, b);
    }

    strip.show();  // This sends the updated pixel color to the hardware.
    return;
  }

  if (millis() - time > duration) {
    currentState++;
    time = 0;
  }

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
    }
    strip.show();

    delay(duration);
  }
}

void loop() {
  if(digitalRead(PUSHBUTTON))
    startFadeSunset();

  if (lastFade == 1) {
    static int32_t newr;
    static int32_t newg;
    static int32_t newb;

    if (count == 0) {
      newr = 11; // last fade starting color
      newg = 0;
      newb = newr;
      t = false;
      currentState = STATE_LIGHTNING;
    }

    count++;

//    if (count % 1000 == 0 || (newr < 20 && count % 5 == 0)) {
//    if (count % 6666 == 0) { //10.30 minutes
    if (count % 6666 == 0) { //10.30 minutes
      newr -= 1;
      newb -= 1;

      if (newr < 1) {
        for (int i = SUNSET_START; i <= SUNSET_STOP; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();

        lastFade = 0;
      }

      for (int i = SUNSET_START; i <= SUNSET_STOP; i++) {
        strip.setPixelColor(i, newr, newg, newb);
      }
      strip.show();
    }
  }

  if(currentState >= (int)STATE_LIGHTNING)
  {
    int lightningIndex = currentState - (int)STATE_LIGHTNING;
    setLightning(lightning[lightningIndex].startPixel,
                 lightning[lightningIndex].stopPixel, 
                 lightning[lightningIndex].r,
                 lightning[lightningIndex].g, 
                 lightning[lightningIndex].b,
                 lightning[lightningIndex].duration);
      return;
  }

  switch (currentState) {
    case STATE_FADE_COLORS: {
      fader(166, 42, 42,  // Start Color
            140, 50, 0,   // End Color
            420000);       // Duration of fade

      fader(139, 50, 0, 
            178, 34, 34,
            420000);

      fader(178, 34, 34, 
            255, 20, 147, 
            420000);

      fader(255, 20, 147, 
            199, 21, 133, 
            420000);

      fader(199, 21, 133, 
            139, 0, 139, 
            300000);

      fader(139, 0, 139, 
            11, 0, 11, 
            240000);

      lastFade = 1;
      count = 0;
      currentState = STATE_LIGHTNING;
    } break;
  }
}
