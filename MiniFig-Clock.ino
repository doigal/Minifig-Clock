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
 with a full 4th digit, NTP time, support for text and Lego Minifigs
 
 Link to original: https://github.com/DIY-Machines/DigitalClockSmartShelving
 
 L. Doig, May 2020
 
 To modify for yourself: 
    * Set the SSID and password in credentials
    * Set the TZ_INFO to your timezone
    * Adjust the position of each digit and the LEDs per segment (if not using 4 per segment)

*/

/*
SPECIFIC TO DO: 
 * Decimal place
 * Pulse the neopixels in the background for things like connecting to wifi etc (heartbeat stuff)
 * switch to esp and online time
 * have a single digit display mode for testing
 * better use of matrix/array rather than brute force
 * Wifi manager - red wifi when down
 * better colour management - inc ability to loop through colour ranges
 * Brightness management via light sensor
 * Scrolling text ability
 * Wifi interface
 * PART DONE Have a more readable char to display map. Ideal is ASCII, with case inconsistent ability 
 * PART DONE Serial debug 
 * DONE A void that handles any number w left/center/right justify

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
//#include <time.h>
#include <ezTime.h>

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#else
	#include <WiFi.h>
#endif

//#include <Credentials.h> // Secret wifi detail stuff.

// Version
#define VERSION_NUM        1

// Time zone details
// Provide official timezone names
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const char* TZ_INFO    = "Australia/Melbourne";  

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN        D2
#define LEDDOWNLIGHT_PIN    D5

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT      28
#define LEDDOWNLIGHT_COUNT  2

// How many LEDs per segment?
#define LED_SEGMENT         4

// What is the number of the first LED in each digit?
const int digit[] = {84, 56, 28, 0};

// Variables for brightness
const int CF_Bright_min = 25;
const int CF_Bright_max = 200;
const int DL_Bright_min = 50;
const int DL_Bright_max = 200;
int CF_Bright = 50;
int DL_Bright = 200;

// Define the neopixel instances
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_RGBW + NEO_KHZ800);

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
  displayVersion(stripClock.Color(0,206,209),1000); 
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);

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
  delay(500);
  Serial.print("\n WiFi Connection established!");  

  //Display IP
  Serial.print("\n IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  displayIP(stripClock.Color(0,255,0),500);
  // Format for 192.168.0.5 : IP 192 168 0 5
  
  //Sync Time with NTP using ezTime libary
  displayAny('N','T','P',0,1,1,stripClock.Color(255,255,0));
  displayNTPstatus(stripClock.Color(255,255,0));
  //setInterval(30); //NTP Sync interval is ~30 mins by default
  setDebug(INFO);
  waitForSync();
  displayAny('S','Y','N','C',1,1,stripClock.Color(0,255,0));
  delay(500);
  
  Timezone myTZ;
  myTZ.setLocation(TZ_INFO);
  Serial.print(F("Local time:     "));
  Serial.println(myTZ.dateTime());

      
  // TESTS
  //counttest_1(stripClock.Color(0,0,255),1000);
  //counttest_4(stripClock.Color(255,0,0),250);
  
}

void loop() {
  // Read the time
  // Push the display values to the LEDs
  // Adjust the brightness
  // Display the LEDs
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
	  displayLongNum(i,digit[3],2,colourToUse);
    delay(wait);                                                    //  Pause for a moment
  }  
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
   displayCharecter(digit4+48,1,colourToUse);
   displayCharecter(digit3+48,2,colourToUse);
   displayCharecter(digit2+48,3,colourToUse);
   displayCharecter(digit1+48,4,colourToUse);
   stripClock.show(); 
   
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
  //40 is deg sym.
  //case 248:
  //digit_char_deg(offsetBy,colourToUse);
  //  break;

      
    default:
     break;
  }
}

//void displayDay(int dayToDisplay, int colourToUse)
//	stripClock.clear();
//    switch (dayToDisplay){
//		
//	case 1:		//Mon
//		displayCharecter(22, OFFS_FOUTH, colourToUse);  //m
//		displayCharecter(24, OFFS_THIRD, colourToUse);  //o
//		displayCharecter(23, OFFS_SECON, colourToUse);  //n
//		stripClock.show();		
//		break;
//		
//	case 2:		//Tue
//		displayCharecter(29, OFFS_FOUTH, colourToUse);  //t
//		displayCharecter(30, OFFS_THIRD, colourToUse);  //u
//		displayCharecter(14, OFFS_SECON, colourToUse);  //e
//		stripClock.show();		
//		break;
//
//	case 3:		//Wen
//		displayCharecter(32, OFFS_FOUTH, colourToUse);  //W
//		displayCharecter(14, OFFS_THIRD, colourToUse);  //e
//		displayCharecter(23, OFFS_SECON, colourToUse);  //n
//		stripClock.show();		
//		break;
//		
//	case 4:		//Thu
//		displayCharecter(22, OFFS_FOUTH, colourToUse);  //t
//		displayCharecter(24, OFFS_THIRD, colourToUse);  //h
//		displayCharecter(23, OFFS_SECON, colourToUse);  //u
//		stripClock.show();		
//		break;
//
//	case 5:		//Fri
//		displayCharecter(15, OFFS_FOUTH, colourToUse);  //f
//		displayCharecter(27, OFFS_THIRD, colourToUse);  //r
//		displayCharecter(24, OFFS_SECON, colourToUse);  //i
//		stripClock.show();		
//		break;
//
//	case 6:		//Sat
//		displayCharecter(28, OFFS_FOUTH, colourToUse);  //s
//		displayCharecter(10, OFFS_THIRD, colourToUse);  //a
//		displayCharecter(29, OFFS_SECON, colourToUse);  //t
//		stripClock.show();		
//		break;
//		
//	case 7:		//Sun
//		displayCharecter(28, OFFS_FOUTH, colourToUse);  //s
//		displayCharecter(30, OFFS_THIRD, colourToUse);  //u
//		displayCharecter(23, OFFS_SECON, colourToUse);  //n
//		stripClock.show();		
//		break;
//
//    default:
//     break;
//    }
//}
