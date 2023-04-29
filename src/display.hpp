#pragma once
#include "Measurements.hpp"

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

// Initialize the display, sets rotation and font size etc.
void display_init(void);

// Main function to print all BMS data
void display_draw(const DisplayData& data);