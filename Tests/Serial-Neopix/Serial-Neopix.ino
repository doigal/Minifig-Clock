// Serial to 7 segment neopixel test

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN        D6

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT      28

// How many LEDs per segment?
#define LED_SEGMENT         4

#define VERSION_NUM 		1

// What is the number of the first LED in each digit?
const int digit[] = {84, 56, 28, 0};

// Variables for brightness
const int CF_Bright_min = 25;
const int CF_Bright_max = 200;
int CF_Bright = 50;
int DL_Bright = 200;

int incomingByte = 0; // for incoming serial data

// Define the neopixel instances
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);

// VOID SETUP
void setup() {
  // Start Serial for debugging
  Serial.begin(115200);
  Serial.println("/n /n"); 
  Serial.println("Neopixel 7 Segment display based on serial input");
  Serial.println("L. Doig");
  Serial.print("Version: ");
  Serial.println(VERSION_NUM);
  Serial.println(""); 
  
  // Display version number on LEDs
  // Delay 1 sec
 
  // Init the clock LEDs
  stripClock.begin();                                 // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.clear(); 
  stripClock.show();                                  // Turn OFF all pixels ASAP
  stripClock.setBrightness(CF_Bright);                // Set inital BRIGHTNESS (max = 255)
 
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    
    // say what you got:
    displayCharecter(incomingByte, digit[3],stripClock.Color(0,255,0));

 }
}

void displayCharecter(char charToDisplay, int offsetBy, uint32_t colourToUse){
  // Function to take a ASCII charecter and call the required display function
  // The input must be any number, letter (regardless of case), +, -, etc within the standard ASCII table
  // (providing it has been mapped)
  // Can pick which digit it goes to via offsetBy, and the 32bit colour
  
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
