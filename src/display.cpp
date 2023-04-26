#include "display.hpp"

// New background colour
#define TFT_BROWN 0x38E0

#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "NotoSansMonoSCB20.h"
#include "Free_Fonts.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36
#define AA_FONT NotoSansMonoSCB20

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 500

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

unsigned long targetTime = 0; // Used for testing draw times

void draw_cell_voltages(const DisplayData& data) {
  //tft.loadFont(AA_FONT);    // Must load the font first
  tft.setFreeFont(FM9);

  targetTime = millis();

  // First we test them with a background colour set
  //tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 10);
  //tft.print("Hi ");

  tft.print("12345678901234567");
  for (int i = 1; i < 12; i++) {
    //tft.drawString(String(i), 0, 13*i, 2);
  }
  
  //tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
  //tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
  //tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
  //int xpos = 0;
  //xpos += tft.drawString("xyz{|}~", 0, 64, 2);
  //tft.drawChar(127, xpos, 64, 2);
  //delay(WAIT);

  delay(4000);
}

void display_init(void) {
  tft.init();
  tft.setRotation(1);
}
