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

int state[]   = {0,  0,  0,  0,  0 };
float vR[]    = {0,  0,  0,  0,  0 };
float iR[]    = {0,  0,  0,  0,  0 };
float pR[]    = {0,  0,  0,  0,  0 };

unsigned long tStart[]  = {0,  0,  0,  0,  0 };
float mAh[]     = {0,  0,  0,  0,  0 };
float mWh[]     = {0,  0,  0,  0,  0 };

int sDischarge[] = {2, 4, 6, 8, 10};
int sCharge[]    = {3, 5, 7, 9, 11};
int bState[]     = {12, 1};

int screenState = 0;
int holdup = 0;

float R = 5.0;

#define VMIN 3.9
#define VMAX 4.0
#define VREF 4.8
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
  Serial.println(digitalRead(bState[0]));
  for (int i = 0; i < MY_SIZE; i++) {
    if ( state[i] == 0 ) { fHold(i); }
    else if ( state[i] == 1) { fCharge(i); }
    else if ( state[i] == 2) { fChargeTopoff(i); }
    else if ( state[i] == 3) { fChargeDone(i);   }
    else if ( state[i] == 4) { fDischargeStart(i); }
    else if ( state[i] == 5) { fDischarge(i);  }
    else if ( state[i] == 6) { fDischargeTopoff(i); }
    else if ( state[i] == 7) { fDischargeDone(i);   }
    else if ( state[i] == 8) { fCharge(i);   }
    else if ( state[i] == 9) { fChargeTopoff(i);  }
    else if ( state[i] == 10) { fHold(i); }
//  Serial.println(state[0]);
  }

  testdrawstyle();

  if ( holdup == 1 ) {
    delay(1000);
    holdup = 0;
  }
  
  if (  (state[0] == 0) && (digitalRead(bState[0]) == 0) &&
        (state[1] == 0) && 
        (state[2] == 0) &&
        (state[3] == 0) &&
        (state[4] == 0) )
  {
    for (int i = 0; i < MY_SIZE; i++) {
      state[i] = 1;
    }
    holdup = 1;
  } else if ( (digitalRead(bState[0]) == 0) &&
        ( (state[0] != 0) && 
        (state[1] != 0) && 
        (state[2] != 0) &&
        (state[3] != 0) &&
        (state[4] != 0) ) )
  {
    for (int i = 0; i < MY_SIZE; i++) {
      state[i] = 0;
    }
    holdup = 1;
  }
  
  if ( (screenState == 0) && (digitalRead(bState[1]) == 0) ) {
    screenState = 1;
  } else if ( (screenState == 1) && (digitalRead(bState[1]) == 0) ) {
    screenState = 2;
  } else if ( (screenState == 2) && (digitalRead(bState[1]) == 0) ) {
    screenState = 3;
  } else if ( (screenState == 3) && (digitalRead(bState[1]) == 0) ) {
    screenState = 4;
  } else if ( (screenState == 4) && (digitalRead(bState[1]) == 0) ) {
    screenState = 0;
  }
  
}

/* 
 *  State functions
 */
void fHold( int batNum ) {
  fSetHold( batNum );
  fRead( batNum );
}

void fCharge( int batNum ) {
  fSetCharge( batNum );
  fRead( batNum );
}

void fChargeTopoff( int batNum ) {
  fSetHold( batNum );
  delay(150);
  fRead( batNum );
  delay(50);
  fSetCharge( batNum );
}

void fChargeDone( int batNum ) {
  state[batNum] = state[batNum] + 1;
}

void fDischargeStart( int batNum ) {
  fRead( batNum );
  tStart[batNum] = millis();
  state[batNum] = state[batNum] + 1;
  fSetDischarge( batNum );
}

void fDischarge( int batNum ) {
  fRead( batNum );
  mAh[batNum] = mAh[batNum] + (millis() - tStart[batNum])*iR[batNum];
  mWh[batNum] = mWh[batNum] + (millis() - tStart[batNum])*pR[batNum]*1000;
  fSetDischarge( batNum );
}

void fDischargeTopoff( int batNum ) {
  fSetHold( batNum );
  delay(150);
  fRead( batNum );
  mAh[batNum] = mAh[batNum] + (millis() - tStart[batNum])*iR[batNum];
  mWh[batNum] = mWh[batNum] + (millis() - tStart[batNum])*pR[batNum]*1000;
  delay(50);
  fSetDischarge( batNum );
}

void fDischargeDone( int batNum ) {
  fSetHold( batNum );
  delay(200);
  fRead( batNum );
  mAh[batNum] = mAh[batNum] + (millis() - tStart[batNum])*iR[batNum];
  mWh[batNum] = mWh[batNum] + (millis() - tStart[batNum])*pR[batNum]*1000;
  state[batNum] = state[batNum] + 1;
}

/* 
 *  Set functions
 */
void fSetHold( int batNum ) {
  digitalWrite(sCharge[batNum], 1);
  digitalWrite(sDischarge[batNum], 0);
}
void fSetCharge( int batNum ) {
  digitalWrite(sCharge[batNum], 0);
  digitalWrite(sDischarge[batNum], 0);
}
void fSetDischarge( int batNum ) {
  digitalWrite(sCharge[batNum], 1);
  digitalWrite(sDischarge[batNum], 1);
}

/* 
 *  Read function
 */
void fRead( int batNum ) {
  delay(50);
  aBatteryVal[batNum] = analogRead(aBatteryPin[batNum]);  // read the input pin
  vBat[batNum] = VREF * float(aBatteryVal[batNum]) / 1023.0;

  aRgndVal[batNum] = 0.0; // analogRead(aRgndPin[i]);  // read the input pin
  vRgnd[batNum] = 0.0; // 5 * float(aRgndVal[i]) / 1023.0;

  vR[batNum] = float(vBat[batNum]) - float(aRgndVal[batNum]);
  iR[batNum] = 1000 * vR[batNum] / R;
  pR[batNum] = vR[batNum] * iR[batNum] / 1000;
}

/* 
 *  State functions
 */
void testdrawstyle() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Draw 'inverse' text
  display.println(state[0]);
  
  if ( screenState == 0 ) {
    // Battery Voltage 1
    display.print(vR[0], 2);
    display.setTextSize(1);
    display.print(F("V "));
    display.setTextSize(2);
    // Battery Voltage 2
    display.print(vR[1], 2);
    display.setTextSize(1);
    display.print(F("V "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Voltage 3
    display.print(vR[2], 2);
    display.setTextSize(1);
    display.print(F("V "));
    display.setTextSize(2);
    // Battery Voltage 4
    display.print(vR[3], 2);
    display.setTextSize(1);
    display.print(F("V "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Voltage 5
    display.print(vR[4], 2);
    display.setTextSize(1);
    display.print(F("V "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    /*
     * Current
     */
  } else if ( screenState == 1 ) {
    // Battery Current 1
    display.print(iR[0], 0);
    display.setTextSize(1);
    display.print(F("mA "));
    display.setTextSize(2);
    // Battery Current 2
    display.print(iR[1], 0);
    display.setTextSize(1);
    display.print(F("mA "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Current 3
    display.print(iR[2], 0);
    display.setTextSize(1);
    display.print(F("mA "));
    display.setTextSize(2);
    // Battery Current 4
    display.print(iR[3], 0);
    display.setTextSize(1);
    display.print(F("mA "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Current 5
    display.print(iR[4], 0);
    display.setTextSize(1);
    display.print(F("mA "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    /*
     * mAh
     */
  } else if ( screenState == 2 ) {
    // Battery mAh 1
    display.print(mAh[0], 0);
    display.setTextSize(1);
    display.print(F("mAh "));
    display.setTextSize(2);
    // Battery mAh 2
    display.print(mAh[1], 0);
    display.setTextSize(1);
    display.print(F("mAh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery mAh 3
    display.print(mAh[2], 0);
    display.setTextSize(1);
    display.print(F("mAh "));
    display.setTextSize(2);
    // Battery mAh 4
    display.print(mAh[3], 0);
    display.setTextSize(1);
    display.print(F("mAh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery mAh 5
    display.print(mAh[4], 0);
    display.setTextSize(1);
    display.print(F("mAh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    /*
     * Power
     */
  } else if ( screenState == 3 ) {
    // Battery Power 1
    display.print(pR[0], 2);
    display.setTextSize(1);
    display.print(F("W "));
    display.setTextSize(2);
    // Battery Power 2
    display.print(pR[1], 2);
    display.setTextSize(1);
    display.print(F("W "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Power 3
    display.print(pR[2], 2);
    display.setTextSize(1);
    display.print(F("W "));
    display.setTextSize(2);
    // Battery Power 4
    display.print(pR[3], 2);
    display.setTextSize(1);
    display.print(F("W "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Power 5
    display.print(pR[4], 2);
    display.setTextSize(1);
    display.print(F("W "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    /*
     * Wh
     */
  } else if ( screenState == 4 ) {
    // Battery Wh 1
    display.print(mWh[0]*1000, 2);
    display.setTextSize(1);
    display.print(F("Wh "));
    display.setTextSize(2);
    // Battery Wh 2
    display.print(mWh[1]*1000, 2);
    display.setTextSize(1);
    display.print(F("Wh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Wh 3
    display.print(mWh[2]*1000, 2);
    display.setTextSize(1);
    display.print(F("Wh "));
    display.setTextSize(2);
    // Battery Wh 4
    display.print(mWh[3]*1000, 2);
    display.setTextSize(1);
    display.print(F("Wh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
    // Battery Wh 5
    display.print(mWh[4]*1000, 2);
    display.setTextSize(1);
    display.print(F("Wh "));
    display.setTextSize(2);
    display.print(F("\n"));       // new line
  }


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
  delay(100);
  //  delay(500);
}
