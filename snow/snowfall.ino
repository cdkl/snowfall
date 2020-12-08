// BitArray - Version: 0.2.1
#include <BitArray.h>

#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 450 
#define NUM_STRIPS 9
#define STRIP_LEN 50

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 5
#define CLOCK_PIN 13

// Snow fall speed, milliseconds per row
#define FALL_SPEED 1000
// Tick size, milliseconds
#define TICK_SIZE 200
#define TICK_JITTER 300

// Probability of precipitation
#define POP 5

// Define the array of leds
CRGB leds[NUM_LEDS];
CRGB targetColour;
BitArray snow;

int columnStart[NUM_STRIPS];
bool columnReverse[NUM_STRIPS];
long columnTime[NUM_STRIPS];

fract8 lerpScale = 10;

void setup() { 

  // Initialize FASTLED
  Serial.begin(9600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(128);
  targetColour = CHSV(45,80,255);
  
  snow.begin(1, 450);
  
  for(int i = 0; i < NUM_LEDS; ++i) {
    snow.set(i,0);
  }
  
  // Initialize rnd
  randomSeed(analogRead(0));
  
  // Configure our strips
  columnStart[0] = 0;
  columnReverse[0] = false;

  columnStart[1] = STRIP_LEN*2-1;
  columnReverse[1] = true;

  columnStart[2] = STRIP_LEN*2;
  columnReverse[2] = false;

  columnStart[3] = STRIP_LEN*4-1;
  columnReverse[3] = true;

  columnStart[4] = STRIP_LEN*4;
  columnReverse[4] = false;

  columnStart[5] = STRIP_LEN*6-1;
  columnReverse[5] = true;

  columnStart[6] = STRIP_LEN*6;
  columnReverse[6] = false;

  columnStart[7] = STRIP_LEN*8-1;
  columnReverse[7] = true;

  columnStart[8] = STRIP_LEN*8;
  columnReverse[8] = false;

  for(int i = 0; i < NUM_STRIPS; ++i) {
    columnTime[i] = random(0,FALL_SPEED);
    Serial.print(columnTime[i]);
    Serial.print("\n");
  }

}

void loop() { 
  // Shuffle the snowflakes
  for(int column=0; column<NUM_STRIPS; ++column) {
    long columnTickSize = TICK_SIZE + random(-TICK_JITTER, TICK_JITTER);
    columnTime[column] += columnTickSize;
    if(columnTime[column] >= FALL_SPEED) {
      columnTime[column] -= FALL_SPEED;
      updateStrip(columnStart[column], STRIP_LEN, columnReverse[column]);
    }
  }

  
  render();
    
  FastLED.show();
    
  //delay(TICK_SIZE);
}

void updateStrip(int start, int len, bool reverse) {
  
    int end = reverse? start - STRIP_LEN : start + STRIP_LEN;
    int last = reverse? end+1 : end-1;
    int incr = reverse? -1 : 1;
  
    for(int row = start; row != end; row += incr) {
      // Work up the row, moving snowflakes down
      if(row != last) {
        snow.set(row, snow.get(row+incr));
      }
      else {
        // Let's make sure there's at least 1 gap between flakes
        if(! snow.get(row-incr)) {
          snow.set(row, random(0,100) < POP);
        }
        else {
          snow.set(row,0);
        }
      }
    }

}

void render() {
  
  for(int i = 0; i < NUM_LEDS; ++i) {
    if(snow.get(i)) {
//      leds[i] = leds[i].lerp8(targetColour, 5); //CRGB(lerp8by8(currentLED.r, targetColour.r, lerpScale), lerp8by8(currentLED.g, targetColour.g, lerpScale), lerp8by8(currentLED.b, targetColour.b, lerpScale));
/*
      Serial.print("LED ");
      Serial.print(i);
      Serial.print(" - ");
      Serial.print(leds[i].r);
      Serial.print(",");
      Serial.print(leds[i].g);
      Serial.print(",");
      Serial.print(leds[i].b);
      Serial.print("\n");
  */    

      leds[i] = CHSV(170,255,random(220,255));
    }
    else {
      leds[i].nscale8(205);
    }
  }
}
