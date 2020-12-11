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

#define NUM_LIGHTS 30

#define TICK_SIZE 2

// Define the array of leds
CRGB leds[NUM_LEDS];

int columnStart[NUM_STRIPS];
bool columnReverse[NUM_STRIPS];

enum light_type {
  NORMAL,
  NOVA
};

struct light {
  int pos;
  byte colour;
  byte time;
  light_type type;
};

light lights[NUM_LIGHTS];

void setup() { 

  // Initialize FASTLED
  Serial.begin(9600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(128);
    
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

  for(int i = 0; i < NUM_LIGHTS; ++i) {
    createNewLight(i);
    lights[i].time = random8();
  }
}

void loop() { 
  // Shuffle the snowflakes
  for(int i = 0; i < NUM_LIGHTS; ++i) {
    int tickIncr = TICK_SIZE + random(0, TICK_SIZE);
    if( tickIncr + lights[i].time > 255) {
      // regen light
      createNewLight(i);
    }
    lights[i].time += tickIncr;
  }
  
  render();
    
  FastLED.show();
    
  //delay(TICK_SIZE);
}

void createNewLight(int i) {
  lights[i].colour = random8(0,4);
  lights[i].pos = random16(0, NUM_LEDS);
  int rnd = random16(0,1000);
  if(rnd >= 990) {
    lights[i].type = NORMAL;
  }
  else {
    lights[i].type = NOVA;
  }
}

/*
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
*/

void render() {
  
  // fade all
  for(int i = 0; i < NUM_LEDS; ++i) {
    leds[i].nscale8(245);
  }

  // trigger active
  for(int i = 0; i < NUM_LIGHTS; ++i) {
    if( lights[i].type == NORMAL ) {
      drawLight(lights[i]);

    }
    else {
      // NOVA
    }
    
  }
}

void drawLight( const light& thisLight ) { 
  int index = thisLight.pos;
  CRGB target;
  switch(thisLight.colour) {
    case 0:
      target = CRGB::Red;
      break;
    case 1:
      target = CRGB::Green;
      break;
    case 2:
      target = CRGB::Blue;
      break;
    case 3:
      target = CRGB::Yellow;
      break;
  }
  for(int j = 0; j < 3; ++j ) {
    leds[thisLight.pos][j] = lerp8by8(leds[thisLight.pos][j], target[j], 5);
  }

}
