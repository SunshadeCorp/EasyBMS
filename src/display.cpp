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
    String display_text;
void draw_cell_voltages(const DisplayData& data) 
{


  for (int i = 0; i < 12; i++) 
  {
    printValue(0,i,data.measurements.cell_voltages[i],3,0,9.999,"","");
  }

  for (int i = 0; i < 12; i++) 
  {
    if(data.balance_bits[i])
      display_text = String("-");
    else
      display_text = String(" ");
    tft.setCursor(1*8*4*text_size, i*8*text_size);
    tft.print(display_text.c_str());
  }
  //printValue(int x, int y, float value, int decplaces, float min, float max, String prefix, String unit)
  printValue(6,0,data.measurements.cell_diff,3,0,9.999,"dif:","");
  printValue(6,1,data.measurements.cell_diff_trend,1,-99.9,99.9,"dtr:","");
  printValue(6,2,data.measurements.soc,1,-99.9,999.9,"SOC:","");
  printValue(6,3,data.measurements.module_voltage,1,0,99.9,"Mod:","V");
  printValue(6,4,data.measurements.min_cell_voltage,3,0,9.999,"min:","");
  printValue(6,5,data.measurements.max_cell_voltage,3,0,9.999,"max:","");
  printValue(6,6,data.measurements.module_temp_1,1,0,99.9,"t1:","C");
  printValue(6,7,data.measurements.module_temp_2,1,-99.9,99.9,"t2:","C");
  printValue(6,8,data.measurements.chip_temp,1,-99.9,99.9,"ti:","C");

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

void printValue(int x, int y, float value, int decplaces, float min, float max, String prefix, String unit)
{
    String display_text;
    if (value<max && value>=min)
    {
      display_text = prefix+String(value, decplaces)+unit;
      if(display_text.length()<9)
        display_text=display_text+" ";
    }
    else
    {
      display_text = prefix+"invld";
    }
    tft.setCursor(8*x, y*8*text_size);
    tft.print(display_text.c_str());
}