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

uint8_t text_size = 1;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

void draw_cell_voltages(const DisplayData& data) {
  String display_text;

  for (int i = 0; i < 12; i++) {
    if (data.measurements.cell_voltages[i]<10.0 && data.measurements.cell_voltages[i]>=0)
    {
      display_text = String(data.measurements.cell_voltages[i], 3);
    }
    else
    {
      display_text = String("invld");
    }
    tft.setCursor(0*8*4*text_size, i*8*text_size);
    tft.print(display_text.c_str());
  }

  for (int i = 0; i < 12; i++) 
  {
    String display_text;
    if(data.balance_bits[i])
    {
      display_text = String("-");

    }
    else
    {
      display_text = String(" ");
    }
    tft.setCursor(1*8*4*text_size, i*8*text_size);
    tft.print(display_text.c_str());
  }

  int celldiffmv = data.measurements.cell_diff*1000.0;
  if (celldiffmv<9999 && celldiffmv>=0)
    display_text = "dif:"+String(celldiffmv)+"mV";
  else
    display_text = "dif:-1";
  tft.setCursor(6*8*text_size, 0*8*text_size);
  tft.print(display_text.c_str());

  if (data.measurements.min_cell_voltage<9 && data.measurements.min_cell_voltage>=0)
    display_text = "min:"+String(data.measurements.min_cell_voltage, 3);
  else
    display_text = "min:-1";
  tft.setCursor(6*8*text_size, 1*8*text_size);
  tft.print(display_text.c_str());
  if (data.measurements.max_cell_voltage<9 && data.measurements.max_cell_voltage>=0)
    display_text = "max:"+String(data.measurements.max_cell_voltage, 3);
  else
    display_text = "min:-1";
  tft.setCursor(6*8*text_size, 2*8*text_size);
  tft.print(display_text.c_str());
  if (data.measurements.module_temp_1<999 && data.measurements.module_temp_1>=-99)
    display_text = "t1:"+String(data.measurements.module_temp_1, 1);
  else
    display_text = "t1:-1";
  tft.setCursor(6*8*text_size, 3*8*text_size);
  tft.print(display_text.c_str());
  if (data.measurements.module_temp_2<999 && data.measurements.module_temp_2>=-99)
    display_text = "t2:"+String(data.measurements.module_temp_2, 1);
  else
    display_text = "t2:-1";
  tft.setCursor(6*8*text_size, 4*8*text_size);
  tft.print(display_text.c_str());
  if (data.measurements.chip_temp<999 && data.measurements.chip_temp>=-99)
    display_text = "ti:"+String(data.measurements.chip_temp, 1);
  else
    display_text = "ti:-1";
  tft.setCursor(6*8*text_size, 5*8*text_size);
  tft.print(display_text.c_str());

  /*
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
  */
}

void display_init(void) {
  tft.init();
  tft.setCursor(0, 0);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setTextSize(text_size);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(false);
  //tft.loadFont(AA_FONT);    // Must load the font first
  //tft.setFreeFont(FM9);
}
