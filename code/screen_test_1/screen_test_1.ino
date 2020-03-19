#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>


#define TFT_CS     21 // 10
#define TFT_RST    19 // -1 
#define TFT_DC     20 // 9

#define TFT_SCLK 22 // 13   
#define TFT_MOSI 23 // 11   

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup(void) {
  tft.initR(INITR_BLACKTAB);  // You will need to do this in every sketch
  tft.fillScreen(ST7735_BLACK); 
  tft.setRotation(1); 

  //tft print function!
//  tft.setTextColor(ST7735_WHITE);
//  tft.setTextSize(0);
//  tft.setCursor(30,80);
//  tft.println("Hello World!");  
//  delay(1000);
//  tft.fillScreen(ST77XX_BLACK);  
  tft.fillScreen(ST77XX_WHITE);

  Serial.begin(115200);
  Serial.println(MOSI);
  Serial.println(MISO);
  Serial.println(SCK);
  Serial.println(SS);
//  Serial.println(RST);
}

void loop() {
//  tft.invertDisplay(true);
//  delay(500);
//  tft.invertDisplay(false);
//  delay(500);

    //tft print function!
//  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.setCursor(5,10);
  tft.println("B1"); 
  
  delay(1000);  
  tft.fillRect(5, 10, 30, 20, ST7735_WHITE);

    //tft print function!
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(5,10);
//  tft.setCursor(35,10);
  tft.println("B2");
  
  delay(1000);  
  tft.fillRect(5, 10, 30, 20, ST7735_WHITE);
  
    //tft print function!
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(5,10);
//  tft.setCursor(65,10);
  tft.println("B3"); 
  
  delay(1000);  
  tft.fillRect(5, 10, 30, 20, ST7735_WHITE);
  
    //tft print function!
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(5,10);
//  tft.setCursor(95,10);
  tft.println("B4"); 
  
  delay(1000);  
  tft.fillRect(5, 10, 20, 20, ST7735_WHITE);
  
    //tft print function!
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(5,10);
//  tft.setCursor(125,10);
  tft.println("B5"); 
  
  delay(1000);  
  tft.fillRect(5, 10, 30, 20, ST7735_WHITE);
}
