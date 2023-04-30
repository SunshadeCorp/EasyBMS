#include "display.hpp"

#include <SPI.h>

#include <array>

Display::Display() : _tft(TFT_eSPI()) {
    for (auto& row : _old_chars) {
        for (auto& c : row) {
            c = ' ';
        }
    }
    clear();
}

void Display::init() {
    _tft.init();
    _tft.fillScreen(_background_color);
    _tft.setRotation(1);
    _tft.setFreeFont(&Roboto_Mono_Light_13);
}

String Display::format(float value, uint8_t decplaces, float min, float max, String unit) {
    if (value < max && value >= min) {
        return String(value, decplaces) + unit;
    } else {
        return "invld";
    }
}

String Display::format_temp(float value) {
    return format(value, 1, -99.9, 99.9, "C");
}

String Display::format_cell_voltage(float value) {
    return format(value, 3, 0, 9.999);
}

void Display::clear() {
    for (auto& row : _screen_chars) {
        for (auto& c : row) {
            c = ' ';
        }
    }
}

void Display::print(uint8_t column, uint8_t row, String text) {
    for (size_t x = column; x < _screen_chars[row].size(); x++) {
        _screen_chars[row][x] = text[x - column];
    }
}

void Display::flip() {
    for (size_t row = 0; row < _screen_chars.size(); row++) {
        for (size_t column = 0; column < _screen_chars[row].size(); column++) {
            auto c_new = _screen_chars[row][column];
            auto c_old = _old_chars[row][column];
            if (c_new != c_old) {
                // Paint over
                _tft.setTextColor(_background_color);
                _tft.setCursor(_char_width * column, (row + 1) * _char_height);
                _tft.print(_old_chars[row][column]);

                // Paint new
                _tft.setTextColor(_text_color);
                _tft.setCursor(_char_width * column, (row + 1) * _char_height);
                _tft.print(_screen_chars[row][column]);
            }
        }
    }

    _old_chars = _screen_chars;
    clear();
}

void Display::draw(const DisplayData& data) {
    // Print Cell Voltages
    for (int i = 0; i < 12; i++) {
        String cell_voltage = format_cell_voltage(data.measurements.cell_voltages[i]);
        print(0, i, cell_voltage);
    }

    // Print Balance Bits
    for (int i = 0; i < 12; i++) {
        if (data.balance_bits[i]) {
            print(5, i, "-");
        }
    }

    // Print Stats
    String cell_diff = format_cell_voltage(data.measurements.cell_diff);
    String cell_diff_trend = format(data.measurements.cell_diff_trend, 0, -99, 99, "mVh");
    String soc = format(data.measurements.soc, 1, -99.9, 999.9);
    String module_voltage = format(data.measurements.module_voltage, 1, 0, 99.9, "V");
    String min_cell_voltage = format_cell_voltage(data.measurements.min_cell_voltage);
    String avg_cell_voltage = format_cell_voltage(data.measurements.avg_cell_voltage);
    String max_cell_voltage = format_cell_voltage(data.measurements.max_cell_voltage);
    String module_temp_1 = format_temp(data.measurements.module_temp_1);
    String module_temp_2 = format_temp(data.measurements.module_temp_2);
    String chip_temp = format_temp(data.measurements.chip_temp);

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

    flip();
}