#include "Adafruit_NeoPixel.h"

#define STRIP_PIN 6
#define NUMPIXELS 18  // can't be more than 256
#define PUSHBUTTON 2

#define LIGHTNING_START  18  //The Whole lightning strip
#define LIGHTNING_STOP   30 
#define SUNSET_START     0  //Sunset strip 
#define SUNSET_STOP      17

//0-4
//5-8
//9-12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS,STRIP_PIN,NEO_GRB + NEO_KHZ800);
  
enum STATE {
 STATE_FADE_COLORS,
 STATE_LIGHTNING,
};

struct LightningDef{
  uint8_t startPixel;  
  uint8_t stopPixel;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  unsigned int duration; 
};

const int startingBrightness     = 0;
const int maxLightningBrightness = 200;
const int lightningFadeInc       = 1;
int lightningBrightness          = 100;

int currentState;
int lastFade;
bool t;

const LightningDef lightning[] = {
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

  {0, 4,  135, 206, 250, 80},
  {0, 12, 0,   0,   0,   39},
  {0, 4,  135, 206, 250, 50},
  {0, 12, 0,   0,   0,   15},
  {0, 4,  135, 206, 250, 75},
  {0, 12, 0,   0,   0,   35},
  {0, 4,  135, 206, 250, 55},
  {0, 12, 0,   0,   0,   25},
  {5, 8,  135, 206, 250, 84},
  {0, 12, 0,   0,   0,   40},
  {5, 8,  135, 206, 250, 46},
  {0, 12, 0,   0,   0,   75},
  {5, 8,  135, 206, 250, 84},
  {0, 12, 0,   0,   0,   29},
  {5, 8,  135, 206, 250, 25},
  {0, 12, 0,   0,   0,   2000},  //end of flash 3

  {0, 4,  135, 206, 250, 110},
  {0, 12, 0,   0,   0,   34},
  {0, 4,  135, 206, 250, 300},
  {0, 12, 0,   0,   0,   30},
  {0, 4,  135, 206, 250, 30},
  {0, 12, 0,   0,   0,   30},
  {0, 4,  135, 206, 250, 380},
  {0, 12, 0,   0,   0,   28000}, //end of flash 4

  {5, 8,  135, 206, 250, 28},
  {0, 12, 0,   0,   0,   98},
  {5, 8,  135, 206, 250, 41},
  {0, 12, 0,   0,   0,   1000},
  {9, 12, 135, 206, 250, 35},
  {0, 12, 0,   0,   0,   32},
  {9, 12, 135, 206, 250, 30},
  {0, 12, 0,   0,   0,   41},
  {9, 12, 135, 206, 250, 185},
  {0, 12, 0,   0,   0,   40},
  {9, 12, 135, 206, 250, 25},
  {0, 12, 0,   0,   0,   24000}, //end of flash 5

  {0, 4,  135, 206, 250, 35},
  {0, 12, 0,   0,   0,   100},
  {0, 4,  135, 206, 250, 15},
  {0, 12, 0,   0,   0,   300},
  {0, 4,  135, 206, 250, 290},
  {0, 12, 0,   0,   0,   30},
  {0, 4,  135, 206, 250, 50},
  {0, 12, 0,   0,   0,   0},
  {0, 4,  135, 206, 250, 16},
  {0, 12, 0,   0,   0,   36},
  {0, 4,  135, 206, 250, 23},
  {0, 12, 0,   0,   0,   35},
  {0, 4,  135, 206, 250, 16},
  {0, 12, 0,   0,   0,   24000}, //end of flash 6

  {5, 8,  135, 206, 250, 110},
  {0, 12, 0,   0,   0,   34},
  {5, 8,  135, 206, 250, 300},
  {0, 12, 0,   0,   0,   31},
  {5, 8,  135, 206, 250, 28},
  {0, 12, 0,   0,   0,   34},
  {5, 8,  135, 206, 250, 378},
  {0, 12, 0,   0,   0,   427},
  {5, 8,  135, 206, 250, 79},
  {0, 12, 0,   0,   0,   49},
  {5, 8,  135, 206, 250, 46},
  {0, 12, 0,   0,   0,   52},
  {5, 8,  135, 206, 250, 93},
  {0, 12, 0,   0,   0,   29},
  {5, 8,  135, 206, 250, 25},
  {0, 12, 0,   0,   0,   4000},  //end of flash 7

  {0, 4,  135, 206, 250, 35},
  {0, 12, 0,   0,   0,   150},
  {0, 4,  135, 206, 250, 15},
  {0, 12, 0,   0,   0,   290},
  {0, 2,  135, 206, 250, 290},
  {0, 12, 0,   0,   0,   130},
  {0, 2,  135, 206, 250, 50},
  {0, 12, 0,   0,   0,   130},   //end of flash 8

  {0, 4,  135, 206, 250, 20},
  {0, 12, 0,   0,   0,   109},
  {0, 4,  135, 206, 250, 44},
  {0, 12, 0,   0,   0,   89},
  {0, 4,  135, 206, 250, 29},
  {0, 12, 0,   0,   0,   0},     //end of flash 9

  {0, 4,  135, 206, 250, 80},
  {0, 12, 0,   0,   0,   39},
  {0, 4,  135, 206, 250, 50},
  {0, 12, 0,   0,   0,   15},
  {0, 4,  135, 206, 250, 75},
  {0, 12, 0,   0,   0,   35},
  {0, 4,  135, 206, 250, 55},
  {0, 12, 0,   0,   0,   25},
  {5, 8,  135, 206, 250, 84},
  {0, 12, 0,   0,   0,   40},
  {5, 8,  135, 206, 250, 46},
  {0, 12, 0,   0,   0,   75},
  {5, 8,  135, 206, 250, 84},
  {0, 12, 0,   0,   0,   29},
  {5, 8,  135, 206, 250, 25},
  {0, 12, 0,   0,   0,   2000},  //end of flash 10

  {5, 8,  135, 206, 250, 28},
  {0, 12, 0,   0,   0,   98},
  {5, 8,  135, 206, 250, 41},
  {0, 12, 0,   0,   0,   1000},
  {9, 12, 135, 206, 250, 35},
  {0, 12, 0,   0,   0,   32},
  {9, 12, 135, 206, 250, 20},
  {0, 12, 0,   0,   0,   41},
  {9, 12, 135, 206, 250, 185},
  {0, 12, 0,   0,   0,   40},
  {9, 12, 135, 206, 250, 25},
  {0, 12, 0,   0,   0,   0},
  {5, 8,  135, 206, 250, 110},
  {0, 12, 0,   0,   0,   34},
  {5, 8,  135, 206, 250, 300},
  {0, 12, 0,   0,   0,   31},
  {5, 8,  135, 206, 250, 28},
  {0, 12, 0,   0,   0,   34},
  {5, 8,  135, 206, 250, 378},
  {0, 12, 0,   0,   0,   427},
  {5, 8,  135, 206, 250, 79},
  {0, 12, 0,   0,   0,   49},
  {5, 8,  135, 206, 250, 46},
  {0, 12, 0,   0,   0,   52},
  {5, 8,  135, 206, 250, 93},
  {0, 12, 0,   0,   0,   29},
  {5, 8,  135, 206, 250, 25},
  {0, 12, 0,   0,   0,   21000}, //end of flash 11

  {9, 12, 135, 206, 250, 110},
  {0, 12, 0,   0,   0,   34},
  {9, 12, 135, 206, 250, 300},
  {0, 12, 0,   0,   0,   30},
  {9, 12, 135, 206, 250, 30},
  {0, 12, 0,   0,   0,   30},
  {9, 12, 135, 206, 250, 380},
  {0, 12, 0,   0,   0,   26000}, //end of flash 12
};

void setup() {
  // Serial.begin(9600); //DEBUG output
  pinMode(PUSHBUTTON, INPUT);
  currentState = STATE_LIGHTNING;
  lastFade = 0;
  strip.begin();

  // make sure all pixels are off
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

//Reset the lightningBrightness
void startFadeSunset() {
  //Make sure the lightning strip aren't on
  for(int i=LIGHTNING_START;i<LIGHTNING_STOP;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
  //reset lightningBrightness for fade
  currentState = STATE_FADE_COLORS;
}

void setLightning(int start, int stop, int16_t r, int16_t g, int16_t b, unsigned int duration) {
  static unsigned long time = 0;

  if (time == 0) {
    time = millis();

    if (lastFade == 1 && lightningBrightness < maxLightningBrightness) {
      if (lightningBrightness + lightningFadeInc < maxLightningBrightness)
        lightningBrightness += lightningFadeInc;
      else {
        lastFade = 0;
        lightningBrightness = maxLightningBrightness;
      }
    }

    for (int i = start; i <= stop; i++) {
      strip.setPixelColor(i, r * lightningBrightness / maxLightningBrightness, g * lightningBrightness / maxLightningBrightness, b * lightningBrightness / maxLightningBrightness);
    }

    strip.show();  // This sends the updated pixel color to the hardware.
    return;
  }

  if (millis() - time >= duration) {
    currentState++;
    time = 0;
  }
}

void fader(int16_t r1,int16_t g1,int16_t b1,int16_t r2,int16_t g2,int16_t b2, uint32_t time) {
  const uint8_t steps = 8;              // higher the steps the more course the transition steps=10 would be a 1 second pixel update
  const uint8_t scaler = 8;             // magic number defines the bit shifting of colors
  const uint16_t duration = 1 << steps; // milisecond duration  = 2^steps

  time = time >> steps;

  const int16_t rinc = ((1 << scaler) / (double)time) * (r2 - r1);
  const int16_t ginc = ((1 << scaler) / (double)time) * (g2 - g1);
  const int16_t binc = ((1 << scaler) / (double)time) * (b2 - b1);

  int32_t newr = (int32_t)r1 << scaler;
  int32_t newg = (int32_t)g1 << scaler;
  int32_t newb = (int32_t)b1 << scaler;

  for (uint32_t i = 0; i < time; i++) {
    newr += rinc;
    newg += ginc;
    newb += binc;

    for (int i = SUNSET_START; i <= SUNSET_STOP; i++) {
      strip.setPixelColor(i, newr >> scaler, newg >> scaler, newb >> scaler);
    }
    strip.show();

    delay(duration);
  }
}

void loop() {
  static unsigned long count = 0; 

  if(digitalRead(PUSHBUTTON))
    startFadeSunset();

//  if (lastFade == 1) {
//    static int32_t newr;
//    static int32_t newg;
//    static int32_t newb;
//
//    if (count == 0) {
//      newr = 11; // last fade starting color
//      newg = 0;
//      newb = newr;
//      t = false;
//      currentState = STATE_LIGHTNING;
//    }
//
//    count++;
//
////    if (count % 1000 == 0 || (newr < 20 && count % 5 == 0)) {
////    if (count % 6666 == 0) { //10.30 minutes
//    if (count % 6666 == 0) { //10.30 minutes
//      newr -= 1;
//      newb -= 1;
//
//      if (newr < 1) {
//        for (int i = SUNSET_START; i <= SUNSET_STOP; i++) {
//          strip.setPixelColor(i, 0, 0, 0);
//        }
//        strip.show();
//
//        lastFade = 0;
//      }
//
//      for (int i = SUNSET_START; i <= SUNSET_STOP; i++) {
//        strip.setPixelColor(i, newr, newg, newb);
//      }
//      strip.show();
//    }
//  }

  if (currentState >= (int)STATE_LIGHTNING) {
    const int lightningIndex = currentState - (int)STATE_LIGHTNING;

    if (lightningIndex > sizeof(lightning) / sizeof(lightning[0]))
      currentState = STATE_LIGHTNING;

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
      fader(186, 32, 15, 10, 2, 0, 240000);

      fader(10, 2, 0, 0, 0, 0, 100000);

      lastFade = 1;
      lightningBrightness = startingBrightness;
      //      count = 0;
      currentState = STATE_LIGHTNING;
    } break;
  }
}
