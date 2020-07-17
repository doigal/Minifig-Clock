#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN        5
#define LEDDOWNLIGHT_PIN    2

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT      114
#define LEDDOWNLIGHT_COUNT  14

int CF_Bright = 200;
int DL_Bright = 200;

int wait = 250;

// Define the neopixel instances
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_RGBW + NEO_KHZ800);

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);
  Serial.println("/n /n"); 
  Serial.println("Neopixel 7 Segment clock");
  Serial.println("L. Doig");
  Serial.println("ALL LED TEST");
  Serial.println(""); 

  // Init the clock LEDs
  stripClock.begin();                                 // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.clear(); 
  stripClock.show();                                  // Turn OFF all pixels ASAP
  stripClock.setBrightness(CF_Bright);                // Set inital BRIGHTNESS (max = 255)
 
  // Init the downlight LEDs
  stripDownlighter.begin();                           // INITIALIZE NeoPixel Downlight object (REQUIRED)
  stripDownlighter.clear();
  stripDownlighter.show();                            // Turn OFF all pixels ASAP
  stripDownlighter.setBrightness(DL_Bright);          // Set BRIGHTNESS (max = 255) 200~ 3/4 brightness

}

void loop() {
  // put your main code here, to run repeatedly:


  Serial.println("Testing Downlight Red");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripDownlighter.clear();                                             //  Turn all LEDs off in RAM
       stripDownlighter.setPixelColor(i, 255, 0, 0, 0);
       stripDownlighter.show(); 
       delay(wait);
  }
  Serial.println("Testing Downlight Green");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripDownlighter.clear();                                             //  Turn all LEDs off in RAM
       stripDownlighter.setPixelColor(i, 0, 255, 0, 0);
       stripDownlighter.show(); 
       delay(wait);
  }
  Serial.println("Testing Downlight Blue");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripDownlighter.clear();                                             //  Turn all LEDs off in RAM
       stripDownlighter.setPixelColor(i, 0, 0, 255, 0);
       stripDownlighter.show(); 
       delay(wait);
  }
  Serial.println("Testing Downlight White");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripDownlighter.clear();                                             //  Turn all LEDs off in RAM
       stripDownlighter.setPixelColor(i, 0, 0, 0, 255);
       stripDownlighter.show(); 
       delay(wait);
  }
  
  stripDownlighter.clear(); 
  stripDownlighter.show(); 
  
  Serial.println("Testing Clock Red");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripClock.clear();                                             //  Turn all LEDs off in RAM
       stripClock.setPixelColor(i, 255, 0, 0);
       stripClock.show(); 
       delay(wait);
  }
  Serial.println("Testing Clock Green");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripClock.clear();                                             //  Turn all LEDs off in RAM
       stripClock.setPixelColor(i, 0, 255, 0);
       stripClock.show(); 
       delay(wait);
  }
  Serial.println("Testing Clock Blue");
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {                                    
       stripClock.clear();                                             //  Turn all LEDs off in RAM
       stripClock.setPixelColor(i, 0, 0, 255);
       stripClock.show(); 
       delay(wait);
  }
  stripClock.clear();
  stripClock.show();
}
