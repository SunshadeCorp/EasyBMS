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

void display_init(void);
void draw_cell_voltages(const DisplayData& data);
void printValue(int x, int y, float value, int decplaces, float min, float max, String prefix, String unit);