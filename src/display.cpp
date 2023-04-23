#include "display.hpp"

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)

// For 1.44" and 1.8" TFT with ST7735 use
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Fast
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST); // Slow

void drawtext(const char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

/*
void draw_cell_voltage(const char* text, int row, int column) {
    uint8_t text_size = 1;
    tft.setTextSize(text_size);
    tft.setCursor(column*8*4, row*8*text_size);
    tft.setTextWrap(true);
    tft.print(text);
}
*/

void draw_cell_voltages(const DisplayData& data) {
  /*
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
  */
  

  uint8_t text_size = 1;
  tft.setTextSize(text_size);
  tft.setRotation(1);
  tft.setTextWrap(true);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(true);

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

  //tft.drawLine(60,0,60, 128, ST77XX_WHITE);
  //testdrawtext(display_text.c_str(), ST77XX_WHITE);
 
}

void display_init(void) {
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST77XX_BLACK);
  delay(50);
}
