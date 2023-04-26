#pragma once

#include "Measurements.hpp"
#include <WString.h>
/*
#define TFT_CS     D0
#define TFT_RST    -1  // TFT RST = Arduino RST
#define TFT_DC   D3
#define TFT_SCLK D5
#define TFT_MOSI D7
*/

struct DisplayData {
    Measurements measurements;
    std::bitset<12> balance_bits;
    long uptime_seconds;
};
//Initialize the display, sets rotation and font size etc.
void display_init(void);
//Main function to print all BMS data
void draw_cell_voltages(const DisplayData& data);
//Helper to print one value to the display, len of the value may differ
void printValue(int x, int y, float value, int decplaces, float min, float max, String prefix, String unit);
//Helper to print one value to the display, len must be static
void printValueStatic(int x, int y, float value, int decplaces, float min, float max, String prefix, String unit);