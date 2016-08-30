#include "Adafruit_NeoPixel.h"

#define PIN 6
#define NUMPIXELS 4

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS,PIN,NEO_GRB + NEO_KHZ800);

// Arduino Clock is 16MHz
// 1/16MHz = 6.25e-8 = 62.5e-9

enum STATE {
  STATE_FADE_IN,
  STATE_FADE_OUT
};

STATE currentState;
unsigned int clockCount;
int brightness;
int clockCountDelay = 1000;

void setup(){
  pixels.begin();
  currentState = STATE_FADE_IN;
  clockCount = 0;
  brightness = 0;
}

void loop () {
  clockCount++;

  switch(currentState){
    case STATE_FADE_IN:
      if(clockCount % clockCountDelay!= 0)
        break;
      brightness++;
      pixels.setPixelColor(0,(brightness*200/255),(brightness*69/255),(brightness*5/255));
      pixels.setPixelColor(1,(brightness*220/255),(brightness*80/255),(brightness*10/255));
      pixels.show();
      if(brightness>=255)
      {
        clockCount = 0;
        currentState = STATE_FADE_OUT;
      }
    break;

    case STATE_FADE_OUT:
      if(clockCount % clockCountDelay != 0)
        break;
      brightness--;
      pixels.setPixelColor(0,(brightness*200/255),(brightness*69/255),(brightness*5/255));
      pixels.setPixelColor(1,(brightness*220/255),(brightness*80/255),(brightness*10/255));
      pixels.show();
      if(brightness<=0)
      {
        clockCount = 0;
        currentState = STATE_FADE_IN;
      }
    break;
  }
  
  // CHECK switches service Buttons
}
// Sleep method - No serviceing.
//void loop () {
//  int sleepDelay = 10;
//  for(int brightness = 255 ; brightness >=0 ; brightness--)
//  {
//    pixels.setPixelColor(0,(brightness*200/255),(brightness*69/255),(brightness*5/255));
//    pixels.setPixelColor(1,(brightness*220/255),(brightness*80/255),(brightness*10/255));
//    pixels.show();
//    delay(sleepDelay);
//  }
//  
//  for(int brightness = 0 ; brightness <=255 ; brightness++)
//  {
//    pixels.setPixelColor(0,(brightness*200/255),(brightness*69/255),(brightness*5/255));
//    pixels.setPixelColor(1,(brightness*220/255),(brightness*80/255),(brightness*10/255));
//    pixels.show();
//    delay(sleepDelay);
//  }
//
//}
