/*
 __  __     _               _        __     _      __ _    
|  \/  |   (_)    _ _      (_)      / _|   (_)    / _` |   
| |\/| |   | |   | ' \     | |     |  _|   | |    \__, |   
|_|__|_|  _|_|_  |_||_|   _|_|_   _|_|_   _|_|_   |___/    
_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|   
"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'   
            ___      _                      _              
    o O O  / __|    | |     ___     __     | |__     o O O 
   o      | (__     | |    / _ \   / _|    | / /    o      
  TS__[O]  \___|   _|_|_   \___/   \__|_   |_\_\   TS__[O] 
 {======|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""| {======| 
./o--000'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'./o--000'

A modification of DIY Machines' Giant Hidden Shelf Edge Clock
 with a full 4th digit, NTP time, support for other data and 
 Lego Minifigs
 
 Link to original: https://github.com/DIY-Machines/DigitalClockSmartShelving
 
 L. Doig, 2021

 Required libaries
 Adafruit's Neopixel        ?
 ezTime                     https://github.com/ropg/ezTime

 
 To modify for yourself: 
    * Set the SSID and password in credentials
    * Set the TZ_INFO to your timezone
    * Adjust the position of each digit and the LEDs per segment (if not using 4 per segment)
    * 

*/

/*
SPECIFIC TO DO: 
 * Move the current pulser into a subroutine, takes colour as input
 * Open Weather Map updater
 * Pulse the neopixels in the background for things like connecting to wifi etc (heartbeat stuff)
 * Wifi manager - red wifi when down
 * better colour management - inc ability to loop through colour ranges
 * Scrolling text ability
 * Serial Debug levels (Off/Info/Debug)
 * OTA updater
 * Wifi interface for mode/freetext/etc
 * Shift digit mapping to proper byte matrix
 * Colour mapping for temperatures/UV/etc
 * Modes 
 *        0   Clock, update every minute, pulse the colon
 *        1   Day, date, month, year, back to clock
 *        2   Temperature (sensor), display ~5seconds, back to clock
 *        3   Temperature min (Blue) - max(red) - current(?) (From online or HA)
 *        4   Sunrise/sunset times (From online or HA)
 *        5   Number of unread emails
 *        6   Number of notifications/followers/etc (TBD)
 *		  7	  Free text via MQTT or HA
 * 
 * PART DONE better use of matrix/array rather than brute force
 * PART DONE Have a more readable char to display map. Ideal is ASCII, with case inconsistent ability 
 * DONE Basic Serial debug
 * DONE switch to esp and online time
 * DONE A void that handles any number w left/center/right justify
 * DONE Decimal place/colon pulse
 * DONE Brightness management via light sensor

  
REFERENCE
 * Colours: 
	Red			    (255,   0, 0  )
	Yellow		  (255, 255, 0  )
	Blue		    (0  ,   0, 255)
	Light Blue	(173, 216, 230)
	Green		    (0  , 255, 0)
*/


// Libary Includes
#include <Adafruit_NeoPixel.h>
#include <ezTime.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h> // http web access library

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#else
	#include <WiFi.h>
#endif

//#include "Credentials.ino" // Secret wifi detail stuff.

// Version
#define VERSION_NUM        3

// Time details/preferences
// Provide official timezone names
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const char* TZ_INFO    = "Australia/Melbourne";  
const int twentyfourhr = 1; //1 for 24 hour, 0 for 12 hour
Timezone myTZ;
  
// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN        5
#define LEDDOWNLIGHT_PIN    2

// Which pin on the Arduino is connected to the push button?
//#define BUTTON_PIN          2

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT      114
#define LEDDOWNLIGHT_COUNT  14

// How many LEDs per segment?
#define LED_SEGMENT         4

// What is the number of the first LED in each digit?
//const int digit[] = {84, 56, 28, 0};
const int digit[] = {0, 28, 56, 84};

const int displaywait = 1500;

// Variables for brightness
// If powering through USB, keep these values really low.
// If powering through adapter, keep max at ~200.
// Min needs to be higher than 2.
// FUTURE: Work out a current estimate and limit that way.
const int CF_Bright_min = 10;
const int CF_Bright_max = 75;
const int DL_Bright_min = 10;
const int DL_Bright_max = 175;
int CF_Bright = 5;               // initial brightness - keep low so can reprogram via USB
int DL_Bright = 5;               // initial brightness - keep low so can reprogram via USB

const int numReadings = 7;       // Size of averaging matrix
int readings[numReadings];       // the readings from the analog input
int readIndex = 0;               // the index of the current reading
long total = 0;                  // the running total
long average = 0;                // the average

// Weather Variables
// FUTURE: Consider shifting all to HA data.
const char *weatherHost = "api.openweathermap.org";
String weatherLang = "&lang=en";

String sunrisestr;
String sunsetstr;
int owm_tempcurr;
int owm_tempfeel;
int owm_tempMin;
int owm_tempMax;
int owm_pressure;
int owm_humidity;
int owm_uvi;
int owm_clouds;
int owm_windspeed;
int owm_winddir;
int owm_rain;

WiFiClient client;

// Define the neopixel instances
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRBW + NEO_KHZ800);

// VOID SETUP
void setup() {
  // Start Serial for debugging
  Serial.begin(115200);
  Serial.println("/n /n"); 
  Serial.println("Neopixel 7 Segment clock");
  Serial.println("L. Doig");
  Serial.print("Version: ");
  Serial.println(VERSION_NUM);
  Serial.println(""); 
  
  // Connect to wifi
  WiFi.begin(ssid, password);		// This occours first as it take a couple of seconds, and the other stuff isn't as important
  //displayVersion(stripClock.Color(0,206,209),1000); 
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  //Serial.print("Using Key: ");
  //Serial.println(password);  

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

  // Set the downlights on
  downlighteron();  
  
  //Connecting to wifi status/watchdog
  int WiFicounter = 0;
  int WiFicounter_max = 60;

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    displayWifistatus(stripClock.Color(255,map(WiFicounter, 0, WiFicounter_max, 255, 0),0)); 
    Serial.print("\n Connecting to Wifi, ");
    Serial.print(WiFicounter);
    delay(1000);
	if (++WiFicounter > WiFicounter_max){
		displayWifistatus(stripClock.Color(255,0,0));
		Serial.println ( "Wifi Connection Failed. Restarting" );
		ESP.restart();
	} 
	Serial.print('.');
  }
  displayWifistatus(stripClock.Color(0,255,0));
  delay(displaywait);
  Serial.print("\n WiFi Connection established!");  

  //Display IP
  Serial.print("\n IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  displayIP(stripClock.Color(0,255,0),displaywait);
  // Format for 192.168.0.5 : IP 192 168 0 5
  
  //Sync Time with NTP using ezTime libary
  displayAny('N','T','P',0,1,1,stripClock.Color(255,255,0));
  displayNTPstatus(stripClock.Color(255,255,0));
  //setInterval(30); //NTP Sync interval is ~30 mins by default
  setDebug(INFO);
  waitForSync();
  displayAny('S','Y','N','C',1,1,stripClock.Color(0,255,0));
  delay(displaywait);
  

  myTZ.setLocation(TZ_INFO);
  Serial.print(F("Local time:     "));
  Serial.println(myTZ.dateTime());

  //Show the day, DDMM, Year
  displayDDMY(displaywait,stripClock.Color(0,255,0));   
  //Show the time
  displayTime(twentyfourhr,stripClock.Color(255,255,255));

  // Smoothing brightness array
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  //delay(5000);

  // TESTS
  // counttest_1(stripClock.Color(0,0,255),1000);
  // counttest_4(stripClock.Color(255,0,0),250);

  //owm_weatherupdate();
}

void loop() {

  int rgb_r = 0;
  int rgb_g = 0;
  int rgb_b = 255;

   
  // Days to XMAS mode
  if (minuteChanged()){
     //Colons off
     colonoff();

     // XMAS is day 360 of the year 20hell (2020/2021)
     int delta = 357 - myTZ.dateTime("z").toInt();
     displayLongNum(delta, 0, 1, stripClock.Color(255,0,0));
     delay(displaywait);
     displayAny('D','A','Y','S',1,1,stripClock.Color(0,255,0));
     delay(displaywait);
     displayAny('T','I','L',0,1,1,stripClock.Color(255,0,0));
     delay(displaywait);
     displayAny('X','M','A','S',1,1,stripClock.Color(0,255,0));

     //Flash the downlights green-red in alternating pattern
     int A[7] = {0, 2, 4, 6, 8, 10, 12}; // 0 2 4 6 8 10 12
     int B[7] = {1, 3, 5, 7, 9, 11, 13}; // 1 3 5 7 9 11 13
     
     for(int i=0; i<=4; i++) {     
        for(int j=0; j<=6; j++){
           stripDownlighter.setPixelColor(A[j], 255, 0, 0, 0);
           stripDownlighter.setPixelColor(B[j], 0, 255, 0, 0);
        }
        
        stripDownlighter.show();
        delay(displaywait/3);
        
        for(int j=0; j<=6; j++){
           stripDownlighter.setPixelColor(A[j], 0, 255, 0, 0);
           stripDownlighter.setPixelColor(B[j], 255, 0, 0, 0);
        
        }
        stripDownlighter.show();
        delay(displaywait/3);
        
        }
       
     downlighteron();
     

     // Normal time display
     displayTime(twentyfourhr,stripClock.Color(rgb_r, rgb_g, rgb_b));
     
  }

  // Brightness adjust based on LDR reading. Only done every 2 secs to be smooth
  if (secondChanged()){
    if (myTZ.dateTime("s").toInt() % 2 == 0){
      brightnessAdj();
    }
   } 

  // Pulse the colons
  // Pattern: full bright at ms = 000, off by 750ms
  colonpulse(0,750,rgb_r, rgb_g, rgb_b);
  
  // Final show of the loop 
  stripClock.show();

  // Required for ezTime update with NTP Server
  events();




  

//  // Update the Weather data every 15 minutes
//  if (minuteChanged()){
//    if (myTZ.dateTime("i").toInt() % 15 == 0){
//      // Colons off during update    
//      stripClock.setPixelColor(113, stripClock.Color(0,0,0));
//      stripClock.setPixelColor(112, stripClock.Color(0,0,0));
//      stripClock.show();
//
//      // Update weather
//      owm_weatherupdate();
//    }
//   else
//    displayTime(twentyfourhr,stripClock.Color(rgb_r,rgb_g,rgb_b));
//   
//  }
//
//  //Modes:
//  //  0     Clock, update every ~5seconds, pulse the colon
//  //  1     Day, date, month, year, timezone, back to clock
//  //  2     RESERVED for local sensor data
//  //  3     Open Weather Map Short (Daily min(blue) / max (red) / current (graded))
//  //  4     Open Weather Map Full : 
//  //  5     Sunrise/sunset times from online
//  //  6     RESERVED for Number of unread emails
//  //  7     RESERVED for Number of notifications/followers/etc (TBD)
//  //  8     RESERVED for Complicated way to change colour via single button?
//  
//  // Check for button press, mode = 0
//  // if button press mode = mode + 1
//  // Display mode on display as M__1, and clear the pulsing colons
//  // Wait 1.5 seconds, then execute mode. once mode is finished go back to time (mode 0)
//  // if mode > 7 reset to 0
//  
//  // on ntp sync flash Sync +/- MS out?
//  
//  // Code for updating the time on display
//
//  int rgb_r = 0;
//  int rgb_g = 255;
//  int rgb_b = 0;
//
//
//  //if mode 0 (default)
//  if (minuteChanged()) displayTime(twentyfourhr,stripClock.Color(rgb_r,rgb_g,rgb_b));
//  // pulse colons
//
//  //if mode 1: displayDDMY(displaywait,stripClock.Color(rgb_r,rgb_g,rgb_b)), delay, mode 0
//  //if mode 2: get sensor temp, delay, go back to clock
//  //if mode 3: min temp/ max temp/ curr temp/ every 10 mins, display blue/red/colour map 
//  
//  


 
  }

// *** TESTING FUNCTIONS ***
// Test that cycles through all valid ASCII charecters on the first digit display
void counttest_1(uint32_t colourToUse, int wait){
  for(int i=48; i<=57; i++) {                                       //  ASCII Numbers
    stripClock.clear();                                             //  Turn all LEDs off in RAM
    displayCharecter(i , digit[3], colourToUse);							//  Push 'i' to the first digit
    stripClock.show();                                              //  Update strip to match
    delay(wait);                                                    //  Pause for a moment
  } 
  for(int i=65; i<=90; i++) {                                       //  ASCII Upper Case Letters
    stripClock.clear();                                             //  Turn all LEDs off in RAM
    displayCharecter(i , digit[3], colourToUse);							//  Push 'i' to the first digit
    stripClock.show();                                              //  Update strip to match
    delay(wait);                                                    //  Pause for a moment
  }   
  for(int i=97; i<=122; i++) {                                      //  ASCII Lower Case Letters - 'should' be no difference to upper case
    stripClock.clear();                                             //  Turn all LEDs off in RAM
    displayCharecter(i , digit[3], colourToUse);							//  Push 'i' to the first digit
    stripClock.show();                                              //  Update strip to match
    delay(wait);                                                    //  Pause for a moment
  }   
}

// Test that cycles through all numbers from 0 to 9999
void counttest_4(uint32_t colourToUse, int wait){
  for(int i=0; i<10000; i++) {                                      //  For every possible number in the array
	  displayLongNum(i,1,2,colourToUse);
    delay(wait);                                                    //  Pause for a moment
  }  
}

// *** BRIGHTNESS CONTROL ***
void brightnessAdj() {
    //  Record a reading from the light sensor and add it to the array
    readings[readIndex] = analogRead(A0); //get an average light level from previouse set of samples
//    Serial.print("Light sensor value added to array = ");
//    Serial.println(readings[readIndex]);
    readIndex = readIndex + 1; // advance to the next position in the array:

    // if we're at the end of the array move the index back around...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }
    
    // now work out the sum of all the values in the array
    int sumBrightness = 0;
    for (int i=0; i < numReadings; i++)
      {
          sumBrightness += readings[i];
      }
    //Serial.print("Sum of the brightness array = ");
    //Serial.println(sumBrightness);

    // and calculate the average: 
    int lightSensorValue = sumBrightness / numReadings;
    //Serial.print("Average light sensor value = ");
    //Serial.println(lightSensorValue);    

    //set the brightness based on ambiant light levels
    CF_Bright = map(lightSensorValue, 1, 1000, CF_Bright_min, CF_Bright_max); 
    DL_Bright = map(lightSensorValue, 1, 1000, DL_Bright_min, DL_Bright_max); 
    stripClock.setBrightness(CF_Bright);         // Set brightness value of the LEDs
    stripDownlighter.setBrightness(DL_Bright);   // Set brightness value of the LEDs
    
//    Serial.print("Mapped brightness Clock / Down Light value = ");
//    Serial.print(CF_Bright);
//    Serial.print(" / ");
//    Serial.println(DL_Bright);

    stripClock.show();
    stripDownlighter.show();
}

// *** COLON CONTROL ***
void colonpulse(int blinkfull, int blinkoff, int rgb_r, int rgb_g, int rgb_b){
  int colon_t = myTZ.dateTime("v").toInt();
  int colon_r = constrain(map(colon_t, blinkoff, blinkfull, 0, rgb_r),0,255);
  int colon_g = constrain(map(colon_t, blinkoff, blinkfull, 0, rgb_g),0,255);
  int colon_b = constrain(map(colon_t, blinkoff, blinkfull, 0, rgb_b),0,255);
  stripClock.setPixelColor(113, stripClock.Color(colon_r,colon_g,colon_b));
  stripClock.setPixelColor(112, stripClock.Color(colon_r,colon_g,colon_b));
  }

void colonoff(){
  stripClock.setPixelColor(113, stripClock.Color(0,0,0));
  stripClock.setPixelColor(112, stripClock.Color(0,0,0));
}

// *** DOWNLIGHT CONTROL ***
void downlighteron() {
  for(int i=0; i<=LEDDOWNLIGHT_COUNT; i++) {     
    stripDownlighter.setPixelColor(i, 125, 125, 125, 255);
  } 
  stripDownlighter.show();
}

// *** STANDARD DISPLAYS ***
void displayVersion(int colourToUse, int wait){
    stripClock.clear();
    displayLongNum(VERSION_NUM,0,2,colourToUse);
	  displayCharecter('V', digit[0], colourToUse);                   //  This has to be called after longnum
    stripClock.show(); 
    delay(wait);                                                    //  Pause for a moment	
}

void displayWifistatus(uint32_t colourToUse){
    // Displays WiFI in the selected colour
    displayAny('W','i','F','i',1,1,colourToUse);
}

void displayNTPstatus(uint32_t colourToUse){
    // Displays NTP in the selected colour
    displayAny('N','T','P',0,1,1,colourToUse);
}

void displayIP(uint32_t colourToUse, int wait){
  // Displays the IP to the neopixel array
  displayAny(0,'I','P',0,1,1,colourToUse);
  delay(wait);
  displayLongNum(WiFi.localIP()[0], 0, 1, colourToUse);
  delay(wait);
  displayLongNum(WiFi.localIP()[1], 0, 1, colourToUse);
  delay(wait);
  displayLongNum(WiFi.localIP()[2], 0, 1, colourToUse);
  delay(wait);
  displayLongNum(WiFi.localIP()[3], 0, 1, colourToUse);
  delay(wait);
}

void displayDDMY (int wait, uint32_t colourToUse)
{
  String s_day = myTZ.dateTime("D");
  String s_date = myTZ.dateTime("d");
  String s_month = myTZ.dateTime("M");
  String s_year = myTZ.dateTime("Y");
  String s_TZ = myTZ.dateTime("T");

  Serial.print("\n Day");
  displayAny(0,s_day[0],s_day[1],s_day[2],1,1,colourToUse);
  delay(wait);
  
  Serial.print("\n Date");
  displayAny(0,s_date[0],s_date[1],0,1,1,colourToUse);
  delay(wait);

  Serial.print("\n Month");
  displayAny(0,s_month[0],s_month[1],s_month[2],1,1,colourToUse);
  delay(wait);
  
  Serial.print("\n Year");
  displayAny(s_year[0],s_year[1],s_year[2],s_year[3],1,1,colourToUse);
  delay(wait);

  Serial.print("\n Timezone");
  displayAny(s_TZ[0],s_TZ[1],s_TZ[2],s_TZ[3],1,1,colourToUse);
  delay(wait);  
}

void displayTime (int twentyfourhr, uint32_t colourToUse)
{
  String s_time = "";
  if (twentyfourhr) s_time = myTZ.dateTime("Hi");
  else s_time = myTZ.dateTime("hi");

  Serial.print("\n The time is : ");
  Serial.print(s_time);
  displayAny(s_time[0],s_time[1],s_time[2],s_time[3],1,0,colourToUse);
}

 
void displayAny (int A, int B, int C, int D, int clearall, int showall, uint32_t colourToUse)
{
  if (clearall) stripClock.clear();
  displayCharecter(A, digit[3], colourToUse);    
  displayCharecter(B, digit[2], colourToUse);  
  displayCharecter(C, digit[1], colourToUse);  
  displayCharecter(D, digit[0], colourToUse);    
  if (showall) stripClock.show();
}
 
void displayLongNum (int num, int pad, int justification, uint32_t colourToUse){
   // Function to display any number from 0 to 9999
   // DO NOT FEED LETTERS! ONLY NUMBERS
   // Extra switches allow:
   // Padding in the format of 1 -> 0001 (1 = true, 0 = false) 
   //   Will ALWAYS be right justified
   // Justification (0/1/2)
   //  0  Left:   1234 123_ 12__ 1___
   //  1  Centre: 1234 _123 _12_ __1_
   //  2  Right:  1234 _123 __12 ___1
 
   // All padded numbers treated as Right justified
   int digit1=-48;
   int digit2=-48;
   int digit3=-48;
   int digit4=-48;

   // CLEAR ALL NEOPIX BUFFER
   stripClock.clear();
   
   int quicklen = 0;
   if (num > 999) 
     quicklen = 4;
   else if (num > 99)
     quicklen = 3;
   else if (num > 9)
     quicklen = 2;
   else
     quicklen = 1;

   if(pad || quicklen == 4){
     digit1 = num % 10;
     digit2 = int(floor(num/10))%10;
     digit3 = int(floor(num/100))%10;
     digit4 = int(floor(num/1000))%10;
   }
   else if (justification == 2)    //Right Justified
   {
     if (quicklen == 1)
      {  digit1 = num;
      }
      else if (quicklen == 2)
      {  digit1 = num % 10;
         digit2 = int(floor(num/10))%10;
      }
      else if (quicklen == 3)
      {  digit1 = num % 10;
         digit2 = int(floor(num/10))%10;
         digit3 = int(floor(num/100))%10;
      }
   }
   else if (justification == 1)    //Centre Justified
   {
      if (quicklen == 1)
      {  digit3 = num;
      }
      else if (quicklen == 2)
      {  digit2 = num % 10;
         digit3 = int(floor(num/10))%10;
      }
      else if (quicklen == 3)
      {  digit1 = num % 10;
         digit2 = int(floor(num/10))%10;
         digit3 = int(floor(num/100))%10;
      }
   }  
   else if (justification == 0)    //Left Justified
   {
      if (quicklen == 1)
      {  digit4 = num;
      }
      else if (quicklen == 2)
      {  digit3 = num % 10;
         digit4 = int(floor(num/10))%10;
      }
      else if (quicklen == 3)
      {  digit2 = num % 10;
         digit3 = int(floor(num/10))%10;
         digit4 = int(floor(num/100))%10;
      } 
   }
      
   // Debug
   Serial.print("\n LongNum. Rec: ");
   Serial.print(num);
   Serial.print(" Pad: ");
   Serial.print(pad);     
   Serial.print(" Just: ");
   Serial.print(justification);      
   Serial.print("\t Len: ");
   Serial.print(quicklen);     
   Serial.print("\t 1: ");
   Serial.print(digit4);
   Serial.print("\t 2: ");
   Serial.print(digit3);
   Serial.print("\t 3: ");
   Serial.print(digit2);
   Serial.print("\t 4: ");
   Serial.print(digit1);

   // Output to LED array
   // +48 is a dirty dirty hack, do not feed this letters.
   displayCharecter(digit4+48,digit[3],colourToUse);
   displayCharecter(digit3+48,digit[2],colourToUse);
   displayCharecter(digit2+48,digit[1],colourToUse);
   displayCharecter(digit1+48,digit[0],colourToUse);
   stripClock.show(); 

}

int daycountdown(int target){

  int CurrDate = myTZ.dateTime("z").toInt();
    
  Serial.println("Countdown!");
  Serial.print("Target is day : ");
  Serial.println(target);
  Serial.print("Current is day : ");
  Serial.println(CurrDate);
  int delta = target - CurrDate;
  Serial.print("Days to go : ");
  Serial.println(delta);
  

  
}


// WEATHER UPDATER
void owm_weatherupdate(){
  Serial.println("");
  Serial.println("*****************************************************");
  Serial.println("Weather Update from Open Weather Map");
  Serial.print("Local Time: ");  Serial.println(myTZ.dateTime("G:i"));
  Serial.print("Connecting to "); Serial.println(weatherHost);

  // Connect to server
  client.setTimeout(10000);
  if (client.connect(weatherHost, 80)) {
    Serial.println("Connected to Client");
    String getstr = "GET /data/2.5/onecall?lat=" + owm_lat + "&lon=" + owm_lon + "&units=metric&lang=en&exclude=minutely,hourly&appid=" + owm_weatherKey + " HTTP/1.0";
    Serial.println("Get: " + getstr);
    client.println(getstr);
    client.println(String("Host: api.openweathermap.org"));
    client.println("Connection: close");
    if (client.println() == 0) {
       Serial.println(F("Failed to send request"));
    }
  } else {
    Serial.println("connection failed");
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }
  
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
  
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("waiting for client");
    repeatCounter++;
  }
  Serial.println("Client available");
 
  size_t capacity = 9*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(8) + 17*JSON_OBJECT_SIZE(4) + 9*JSON_OBJECT_SIZE(6) + 4*JSON_OBJECT_SIZE(14) + 5*JSON_OBJECT_SIZE(15) + 2030;
 
  DynamicJsonDocument doc(capacity);
  
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
 
  client.stop();

  long owm_tzoffs    = doc["timezone_offset"];

  //for floats, 0.5 is added for rounding.
  
  JsonObject current = doc["current"];
  owm_tempcurr  = int(float(current["temp"])+0.5);
  owm_tempfeel  = int(float(current["feels_like"])+0.5);
  owm_pressure  = current["pressure"];
  owm_humidity  = current["humidity"];
  owm_uvi       = int(float(current["uvi"])+0.5); 
  owm_clouds    = current["clouds"];
  owm_windspeed = int(float(current["wind_speed"])+0.5); 
  owm_winddir   = current["wind_deg"];
  owm_rain      = current["rain"]["1h"];
  long owm_sunrise   = current["sunrise"];
  long owm_sunset    = current["sunset"];
  //owm_tempMin   = root["daily"]["temp_min"];
  //owm_tempMax   = root["daily"]["temp_max"];

  JsonObject tmp_minmax = doc["daily"][0]["temp"];
  owm_tempMin = int(float(tmp_minmax["min"])+0.5); 
  owm_tempMax = int(float(tmp_minmax["max"])+0.5);

  time_t sunrise_t = owm_sunrise + owm_tzoffs;
  time_t sunset_t = owm_sunset + owm_tzoffs;

  String buffer_r  = ctime(&sunrise_t);
  String buffer_s  = ctime(&sunset_t);
  sunrisestr = buffer_r.substring( buffer_r.indexOf(":") - 2, buffer_r.indexOf(":") + 3);
  sunsetstr = buffer_s.substring( buffer_s.indexOf(":") - 2, buffer_s.indexOf(":") + 3);

  Serial.print("Daily Min:       "); Serial.println(owm_tempMin);
  Serial.print("Daily Max:       "); Serial.println(owm_tempMax);
  Serial.print("Current Temp:    "); Serial.println(owm_tempcurr);
  Serial.print("Feels like Temp: "); Serial.println(owm_tempfeel);
  Serial.print("Pressure:        "); Serial.println(owm_pressure);
  Serial.print("Humidity:        "); Serial.println(owm_humidity);
  Serial.print("UV Index:        "); Serial.println(owm_uvi);
  Serial.print("Cloud Cover:     "); Serial.println(owm_clouds);
  Serial.print("Wind Speed:      "); Serial.println(owm_windspeed);
  Serial.print("Wind Direct:     "); Serial.println(owm_winddir);
  Serial.print("Rain:            "); Serial.println(owm_rain);
  Serial.print("Sun Rise local:  "); Serial.println(sunrisestr);
  Serial.print("Sun Set local:   "); Serial.println(sunsetstr);
  Serial.println("*****************************************************");

  displaytemp(owm_tempcurr, stripClock.Color(255,0,0));
  
}

void display_curr_weather_all(uint32_t colourToUse){

  // Current Temperature according to OWM, in degC
  displayAny('T','C','U','R',1,1,colourToUse);
  delay(displaywait/2);
  displaytemp(owm_tempcurr, colourToUse);
  delay(displaywait);

  // 'Feels Like' Temperature according to OWM, in degC
  displayAny('F','E','E','L',1,1,colourToUse);
  delay(displaywait/2);
  displaytemp(owm_tempfeel, colourToUse);
  delay(displaywait);

  // Current Humidity according to OWM, in %
  displayAny('H','U','M','D',1,1,colourToUse);
  delay(displaywait/2);
  owm_humidity; // and percent
  delay(displaywait);

  // Current Pressure according to OWM, in ?
  displayAny('P','R','E','S',1,1,colourToUse);
  delay(displaywait/2);
  owm_pressure;
  delay(displaywait);

  // Current UV Index according to OWM
  displayAny('U','V',' ','I',1,1,colourToUse);
  delay(displaywait/2);
  //Colour Scaling:
  if (owm_uvi<=2){                          // UVI = Green for 0-2
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(0,255,0)); 
  }
  else if ((owm_uvi>=3) && (owm_uvi<=5)){   // UVI = Yellow for 3-5
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(255,255,0)); 
  }
  else if ((owm_uvi>=6) && (owm_uvi<=7)){   // UVI = Orange for 6-7
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(255,165,0));   
  }
  else if ((owm_uvi>=8) && (owm_uvi<=10)){  // UVI = Red for 8-10
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(255,0,0));  
  }
  else if (owm_uvi>=11){                    // UVI = Purple for 11+
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(128,0,128));         
  }
  else{
      displayLongNum(owm_uvi, 0, 1, stripClock.Color(255,255,255));         
  }
  delay(displaywait);

  displayAny('C','L','D','S',1,1,colourToUse);
  delay(displaywait/2);
  owm_clouds; // and percent
  delay(displaywait);

 
}


void displaytemp(int tempdisplay, uint32_t colourToUse){
  // Cant accept negative
  // Need to add Colour scale (outside?)

  displayLongNum(tempdisplay, 0, 0, colourToUse);
  displayCharecter(176, digit[1], colourToUse);  
  displayCharecter('C', digit[0], colourToUse);    
  
}



void displayCharecter(char charToDisplay, int offsetBy, uint32_t colourToUse){
  // Function to take a ASCII charecter and call the required display function
  // The input must be any number, letter (regardless of case), +, -, deg etc.
  // PREF: That its a string.
  // Can pick which digit it goes to via offsetBy, and the 32bit colour
  // Only updates the RAM value - still requires a show command.
  
  int digitToDisplay = charToDisplay;  // Convert from human to ASCII (might not work anyway)
  
  // Case Check - only handle upper case letters
  int caseflip = 0;
  if ((digitToDisplay >=97) && (digitToDisplay<= 122)){
    // Convert upper case to lower case
    digitToDisplay = digitToDisplay - 32;
    caseflip = 1;
  }
 
  // SERIAL DEBUG
  Serial.print("\n Display. Rec: ");
  Serial.print(charToDisplay);
  Serial.print("\t\t DEC: ");
  Serial.print(digitToDisplay);
  Serial.print("\t ASCII: ");
  Serial.print(char(digitToDisplay));
  Serial.print("\t Offset by: ");
  Serial.print(offsetBy);
  Serial.print("\t Colour: ");
  Serial.print(colourToUse);
  Serial.print("\t CaseFlip: ");
  Serial.print(caseflip);  

  // FUTURE: Make this an array map, where each ASCII value gets a segment mapped out.
  // EG: 0 = [1,1,1,0,1,1,1]

  switch (digitToDisplay){
  // MAPPING IS ASCII!
  
  // NUMBERS  
  case 48:
  digitZero(offsetBy,colourToUse);
    break;
  case 49:
    digitOne(offsetBy,colourToUse);
    break;
  case 50:
  digitTwo(offsetBy,colourToUse);
    break;
  case 51:
  digitThree(offsetBy,colourToUse);
    break;
  case 52:
  digitFour(offsetBy,colourToUse);
    break;
  case 53:
  digitFive(offsetBy,colourToUse);
    break;
  case 54:
  digitSix(offsetBy,colourToUse);
    break;
  case 55:
  digitSeven(offsetBy,colourToUse);
    break;
  case 56:
  digitEight(offsetBy,colourToUse);
    break;
  case 57:
  digitNine(offsetBy,colourToUse);
    break;

  // LETTERS
  // All letters are treated as upper case.
  // but theres a module above that accepts lower case letters and flips them.

  case 65:
  digit_char_a(offsetBy,colourToUse);
    break;
  case 66:
  digit_char_b(offsetBy,colourToUse);
    break;
  case 67:
  digit_char_c(offsetBy,colourToUse);
    break;
  case 68:
  digit_char_d(offsetBy,colourToUse);
    break;
  case 69:
  digit_char_e(offsetBy,colourToUse);
    break;
  case 70:
  digit_char_f(offsetBy,colourToUse);
    break;
  case 71:
  digit_char_g(offsetBy,colourToUse);
    break;
  case 72:
  digit_char_h(offsetBy,colourToUse);
    break;
  case 73:
  digit_char_i(offsetBy,colourToUse);
    break;
  case 74:
  digit_char_j(offsetBy,colourToUse);
    break;                      
  case 75:
  digit_char_k(offsetBy,colourToUse);
    break;
  case 76:
  digit_char_l(offsetBy,colourToUse);
    break;
  case 77:
  digit_char_m(offsetBy,colourToUse);
    break;
  case 78:
  digit_char_n(offsetBy,colourToUse);
    break;
  case 79:
  digit_char_o(offsetBy,colourToUse);
    break;
  case 80:
  digit_char_p(offsetBy,colourToUse);
    break;
  case 81:
  digit_char_q(offsetBy,colourToUse);
    break;
  case 82:
  digit_char_r(offsetBy,colourToUse);
    break;
  case 83:
  digit_char_s(offsetBy,colourToUse);
    break;
  case 84:
  digit_char_t(offsetBy,colourToUse);
    break;
  case 85:
  digit_char_u(offsetBy,colourToUse);
    break;
  case 86:
  digit_char_v(offsetBy,colourToUse);
    break;
  case 87:
  digit_char_w(offsetBy,colourToUse);
    break;
  case 88:
  digit_char_x(offsetBy,colourToUse);
    break;
  case 89:
  digit_char_y(offsetBy,colourToUse);
    break;
  case 90:
  digit_char_z(offsetBy,colourToUse);
    break;

  //SPECIALS
  //176 is deg sym?
  case 176:
  digit_char_deg(offsetBy,colourToUse);
    break;

      
    default:
     break;
  }
}
