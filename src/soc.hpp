#pragma once

#include <vector>

class SOC {
   public:
    static float voltage_to_soc(float cell_voltage);

   private:
    static std::vector<std::pair<float, float>> soc_data_points;

    static void initialize_soc_data_points();
};
