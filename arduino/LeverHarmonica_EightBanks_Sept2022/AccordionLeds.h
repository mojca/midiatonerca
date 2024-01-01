#include <Adafruit_NeoPixel.h>

#define LED_PIN     A7
#define LED_COUNT 48
#define LED_BRIGHTNESS 127

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int noteHue[12];

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void testLeds() {
  for (;;) {
    colorWipe(strip.Color(80,0,0),100);
    colorWipe(strip.Color(0,80,0),100);
    colorWipe(strip.Color(0,0,80),100);
  }
}


// note: 0..11
void noteToRgb(int note, byte& r, byte& g, byte& b) {
  int hue = noteHue[note];
}

void updateLeds() {
  //Serial.println("Updating LEDs...");
  byte r = 10;
  byte g = 127;
  byte b = 10;
  uint32_t color = r | g<<8 | b<<16;

  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(r,g,b));         //  Set pixel's color (in RAM)
  }

  strip.show();
}

void setupLeds() {
  for (int i=0;i<12;i++) {
    noteHue[i] = round((65536.0/12.0)*i);
  }
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(LED_BRIGHTNESS);
}