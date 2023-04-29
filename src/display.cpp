#include "display.hpp"

#include <Free_Fonts.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <array>

const uint8_t char_height = 10;
const uint8_t char_width = 8;
const uint16_t text_color = 0xFFFF;
const uint16_t background_color = 0x0000;

std::array<std::array<char, 22>, 12> old_chars;
std::array<std::array<char, 22>, 12> screen_chars;

// Configured in User_Setup.h
TFT_eSPI tft = TFT_eSPI();

String format(float value, uint8_t decplaces, float min, float max, String unit = "") {
    if (value < max && value >= min) {
        return String(value, decplaces) + unit;
    } else {
        return "invld";
    }
}

void clear() {
    for (auto& row : screen_chars) {
        for (auto& c : row) {
            c = ' ';
        }
    }
}

void print(uint8_t column, uint8_t row, String text) {
    for (size_t x = column; x < screen_chars[row].size(); x++) {
        screen_chars[row][x] = text[x - column];
    }
}

void draw() {
    for (size_t row = 0; row < screen_chars.size(); row++) {
        for (size_t column = 0; column < screen_chars[row].size(); column++) {
            auto c_new = screen_chars[row][column];
            auto c_old = old_chars[row][column];
            if (c_new != c_old) {
                // Paint over
                tft.setTextColor(background_color);
                tft.setCursor(char_width * column, (row + 1) * char_height);
                tft.print(old_chars[row][column]);

                // Paint New
                tft.setTextColor(text_color);
                tft.setCursor(char_width * column, (row + 1) * char_height);
                tft.print(screen_chars[row][column]);
            }
        }
    }

    old_chars = screen_chars;
    clear();
}

void display_draw(const DisplayData& data) {
    String display_text;

    // Print Cell Voltages
    for (int i = 0; i < 12; i++) {
        String cell_voltage = format(data.measurements.cell_voltages[i], 3, 0, 9.999);
        print(0, i, cell_voltage);
    }

    // Print Balance Bits
    for (int i = 0; i < 12; i++) {
        if (data.balance_bits[i]) {
            print(5, i, "-");
        }
    }

    // Print Stats
    String cell_diff = format(data.measurements.cell_diff, 3, 0, 9.999);
    String cell_diff_trend = format(data.measurements.cell_diff_trend, 0, -99, 99, "mVh");
    String soc = format(data.measurements.soc, 1, -99.9, 999.9);
    String module_voltage = format(data.measurements.module_voltage, 1, 0, 99.9, "V");
    String min_cell_voltage = format(data.measurements.min_cell_voltage, 3, 0, 9.999);
    String avg_cell_voltage = format(data.measurements.avg_cell_voltage, 3, 0, 9.999);
    String max_cell_voltage = format(data.measurements.max_cell_voltage, 3, 0, 9.999);
    String module_temp_1 = format(data.measurements.module_temp_1, 1, 0, 99.9, "C");
    String module_temp_2 = format(data.measurements.module_temp_2, 1, -99.9, 99.9, "C");
    String chip_temp = format(data.measurements.chip_temp, 1, -99.9, 99.9, "C");

    print(7, 0, "Dif:" + cell_diff);
    print(7, 1, "Tre:" + cell_diff_trend);
    print(7, 2, "SOC:" + soc);
    print(7, 3, "Mod:" + module_voltage);
    print(7, 4, "Min:" + min_cell_voltage);
    print(7, 5, "Avg:" + avg_cell_voltage);
    print(7, 6, "Max:" + max_cell_voltage);
    print(7, 7, "t1: " + module_temp_1);
    print(7, 8, "t2: " + module_temp_2);
    print(7, 9, "ti: " + chip_temp);

    draw();
}

void display_init(void) {
    tft.init();
    tft.fillScreen(background_color);
    tft.setRotation(1);
    tft.setFreeFont(&Roboto_Mono_Light_13);
    clear();
}