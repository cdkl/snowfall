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

struct line {
  CRGB lineColour;
  int start;
  int spacing;
  int incr;
  byte time;
  byte flicker;
};

#define NUM_LINES 6
line lines[NUM_LINES];

#define LED_BRIGHTNESS 128
#define TICK_SIZE 1
#define FLICKER_CHANCE 998 // greater than, in a 0-999 range
#define LIGHTNING_CHANCE 920

void setup() { 

  // Initialize FASTLED
  Serial.begin(9600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(LED_BRIGHTNESS);
    
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



  // configure our lines
  lines[0].lineColour = CRGB::Red;
  lines[0].start = 0;
  lines[0].spacing = 10;
  lines[0].incr = 3;
  lines[0].time = random8(255);
  lines[0].flicker = 0;

  lines[1].lineColour = CRGB::Purple;
  lines[1].start = 3;
  lines[1].spacing = 10;
  lines[1].incr = 3;
  lines[1].time = random8(255);
  lines[1].flicker = 0;

  lines[2].lineColour = CRGB::Blue;
  lines[2].start = 6;
  lines[2].spacing = 10;
  lines[2].incr = 3;
  lines[2].time = random8(255);
  lines[2].flicker = 0;

  lines[3].lineColour = CRGB::Red;
  lines[3].start = 9;
  lines[3].spacing = 10;
  lines[3].incr = -3;
  lines[3].time = random8(255);
  lines[3].flicker = 0;

  lines[4].lineColour = CRGB::Purple;
  lines[4].start = 12;
  lines[4].spacing = 10;
  lines[4].incr = -3;
  lines[4].time = random8(255);
  lines[4].flicker = 0;

  lines[5].lineColour = CRGB::Blue;
  lines[5].start = 15;
  lines[5].spacing = 10;
  lines[5].incr = -3;
  lines[5].time = random8(255);
  lines[5].flicker = 0;

}

void loop() { 
  render();
    
  FastLED.show();
    
  delay(5);
  //delay(TICK_SIZE);
}

void setLight(int column, int index, CRGB led, bool force) {
  int target = columnStart[column]+(index * (columnReverse[column] ? -1 : 1));
/*  Serial.print(column);
  Serial.print(", ");
  Serial.print(index);
  Serial.print(" -> ");
  Serial.print(target);
  Serial.print("\n");
*/
  if(force) {
    leds[target] = led;
  } else {
    for(int i = 0; i < 3; ++i) {
        leds[target][i] = lerp8by8(leds[target][i], led[i], 15);
    }
  }
}

void render() {
  
  // fade all
  for(int i = 0; i < NUM_LEDS; ++i) {
    leds[i].nscale8(240);
  }

  for(int i = 0; i < NUM_LINES; ++i) {
    byte incr = TICK_SIZE + random8(0,2);

    if(((int) lines[i].time) + incr > 255) {
      // About to tick over, so let's move the line
      lines[i].start = (lines[i].start + random16(-5, 5)) % STRIP_LEN;
    }

    lines[i].time += incr;

    if(lines[i].time > 127) {
      continue;
    }

    bool startFlicker = false;
    bool endFlicker = false;

    if(lines[i].flicker > 0) {
      --lines[i].flicker;
      if(lines[i].flicker == 0 ){
        endFlicker = true;
      }
      else {
        startFlicker = true;
      }
    }
    else {
      // Chance to start flicker
      startFlicker = false;
      endFlicker = false;

      if( random16(0,1000) > FLICKER_CHANCE ) {
        lines[i].flicker = random8(1,5);
        startFlicker = true;
      }      
    }

    // Iterate across each column
    for(int column = 0; column < NUM_STRIPS; ++column) {
      int start = lines[i].start + column*lines[i].incr;
      for(int index = start; index < start + STRIP_LEN; index += lines[i].spacing) {
        if(startFlicker) {
          if(random16(0, 1000) > LIGHTNING_CHANCE) { 
            setLight(column, index % STRIP_LEN, CRGB::White, true);
          } else {
            setLight(column, index % STRIP_LEN, CRGB::Black, true);
          }
        } else if(endFlicker) {
          setLight(column, index % STRIP_LEN, lines[i].lineColour, true);
        }
        else {
          setLight(column, index % STRIP_LEN, lines[i].lineColour, false);
        }
      }
    }
  }

}
