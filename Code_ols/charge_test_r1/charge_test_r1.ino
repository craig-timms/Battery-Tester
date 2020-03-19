/**************************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x32 pixel display using I2C to communicate
  3 pins are required to interface (two I2C and one reset).

  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license, check license.txt for more information
  All text above, and the splash screen below must be
  included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int aBatteryPin = A1;
int aBatteryVal = 0;
float vBat = 0;

int aRgndPin = A2;
int aRgndVal = 0;
float vRgnd = 0;

char state = 'H';

int sDischarge = 2; // D2
int sCharge = 3;    // D3
int bState = 13;    // D13

float R = 2.5;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

void setup() {
  Serial.begin(9600);

  pinMode(sDischarge, OUTPUT);
  pinMode(sCharge, OUTPUT);

  digitalWrite(sDischarge, LOW);
  digitalWrite(sCharge, LOW);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  
  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(1000);
  display.clearDisplay();

}

void loop() {
  aBatteryVal = analogRead(aBatteryPin);  // read the input pin
  vBat = 5 * float(aBatteryVal) / 1023.0;

  aRgndVal = analogRead(aRgndPin);  // read the input pin
  vRgnd = 5 * float(aRgndVal) / 1023.0;

  testdrawstyle(state, vBat, vRgnd);

  if ( (state=='H') && (digitalRead(bState)==HIGH) ) {
    state = 'C';  
    delay(400);   }
  else if ( (state=='C') && (digitalRead(bState)==HIGH) ) {
    state = 'D';
    delay(400);   }
  else if ( (state=='D') && (digitalRead(bState)==HIGH) ) {
    state = 'H';
    delay(400);   }
 
  if ( state=='H' ) {
    digitalWrite(sCharge, HIGH);
    digitalWrite(sDischarge, LOW);  }
  else if ( state=='C' ) {
    digitalWrite(sCharge, LOW);
    digitalWrite(sDischarge, LOW);  }
  else if ( state=='D' ) {
    digitalWrite(sCharge, HIGH);
    digitalWrite(sDischarge, HIGH); }

  delay(200);
    
}

void testdrawstyle(char state, float val_1, float val_2) {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Draw 'inverse' text
  display.println(state);
  display.print(val_1, 2);
  display.setTextSize(1);
  display.print(F("V "));
  float vR = float(val_1)-float(val_2);
  float iR = 1000 * vR / R;
  float pR = vR * iR / 1000;
  display.setTextSize(2);
  display.print( iR, 0 );
  display.setTextSize(1);
  display.print(F("mA"));
  display.setTextSize(2);
  display.print(F("\n"));
  display.println( pR, 2 );
//  display.println(F(""));
//  display.println(val_2, 1);
  int textSize = 2;
  
//  display.setTextSize(textSize);
//  display.print(F("4.20"));
//  display.setTextSize(1);
//  display.print(F("V "));
//  display.setTextSize(textSize);
//  display.print(F("580"));
//  display.setTextSize(1);
//  display.print(F("mA"));
//  display.setTextSize(textSize);
//  display.print(F("\n"));

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("0x")); display.println(0XDEADBEEF, HEX);

  Serial.println(val_1);          // debug value

  display.display();
  //  delay(500);
}
