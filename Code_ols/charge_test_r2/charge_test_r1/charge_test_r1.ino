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

int aBatteryPin[] = {A1, A2, A3, A6, A7};
int aBatteryVal[] = {0,  0,  0,  0,  0 };
float vBat[]      = {0,  0,  0,  0,  0 };

//int aRgndPin = {A1, A2, A3, A4, A5};
int aRgndVal[] = {0,  0,  0,  0,  0 };
float vRgnd[]  = {0,  0,  0,  0,  0 };

float vR[] = {0,  0,  0,  0,  0 };
float iR[] = {0,  0,  0,  0,  0 };
float pR[] = {0,  0,  0,  0,  0 };

char state[] = "HHHHH";

int sDischarge[] = {2, 4, 6, 8, 10};
int sCharge[]    = {3, 5, 7, 9, 11};
int bState[]     = {12, 13};

float R = 5.0;

#define MY_SIZE 5
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

  for (int i = 0; i < MY_SIZE; i++) {
    pinMode(sDischarge[i], OUTPUT);
    pinMode(sCharge[i], OUTPUT);

    digitalWrite(sDischarge[i], 1);
    digitalWrite(sCharge[i], 1);
  }

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
  for (int i = 0; i < MY_SIZE; i++) {
    aBatteryVal[i] = analogRead(aBatteryPin[i]);  // read the input pin
    vBat[i] = 4.8 * float(aBatteryVal[i]) / 1023.0;
    delay(50);

    aRgndVal[i] = 0.0; // analogRead(aRgndPin[i]);  // read the input pin
    vRgnd[i] = 0.0; // 5 * float(aRgndVal[i]) / 1023.0;

    vR[i] = float(vBat[i]) - float(aRgndVal[i]);
    iR[i] = 1000 * vR[i] / R;
    pR[i] = vR[i] * iR[i] / 1000;

    delay(50);
  }

  testdrawstyle(state, vBat, vRgnd);
  Serial.println(state[0]);

  if ( (state[0] == 'H') && (digitalRead(bState[0]) == 0) ) {
    state[0] = 'C';
    delay(400);
  }
  else if ( (state[0] == 'C') && (digitalRead(bState[0]) == 0) ) {
    state[0] = 'D';
    delay(400);
  }
  else if ( (state[0] == 'D') && (digitalRead(bState[0]) == 0) ) {
    state[0] = 'H';
    delay(400);
  }

  for (int i = 0; i < MY_SIZE; i++) {
    if ( state[0] == 'H' ) {
      digitalWrite(sCharge[i], 1);
      digitalWrite(sDischarge[i], 0);
    }
    else if ( state[0] == 'C' ) {
      digitalWrite(sCharge[i], 0);
      digitalWrite(sDischarge[i], 0);
    }
    else if ( state[0] == 'D' ) {
      digitalWrite(sCharge[i], 1);
      digitalWrite(sDischarge[i], 1);
    }
  }

  delay(200);

}

void testdrawstyle(char state[], float val_1[], float val_2[]) {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Draw 'inverse' text
  display.println(state);

//  for (int i = 0; i < MY_SIZE; i++) {
  // Battery Voltage 1
  display.print(vBat[0], 2);
  display.setTextSize(1);
  display.print(F("V "));
  display.setTextSize(2);
  // Battery Voltage 2
  display.print(vBat[1], 2);
  display.setTextSize(1);
  display.print(F("V "));
  display.setTextSize(2);  
  display.print(F("\n"));       // new line
  // Battery Voltage 3
  display.print(vBat[2], 2);
  display.setTextSize(1);
  display.print(F("V "));
  display.setTextSize(2);
  // Battery Voltage 4
  display.print(vBat[3], 2);
  display.setTextSize(1);
  display.print(F("V "));
  display.setTextSize(2);  
  display.print(F("\n"));       // new line
  // Battery Voltage 5
  display.print(vBat[4], 2);
  display.setTextSize(1);
  display.print(F("V "));
  display.setTextSize(2);  
  display.print(F("\n"));       // new line

  
//  display.print( iR[0], 0 );
//  display.setTextSize(1);
//  display.print(F("mA"));
//  display.setTextSize(2);
//  display.print(F("\n"));
  
  
//  display.println( pR, 2 );

//  display.setTextSize(2);             // Draw 2X-scale text
//  display.setTextColor(WHITE);
//  display.print(F("0x")); display.println(0XDEADBEEF, HEX);

  //  Serial.println(val_1[0]);          // debug value

  display.display();
  //  delay(500);
}
