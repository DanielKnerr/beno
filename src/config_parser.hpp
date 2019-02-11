#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
#include <gmpxx.h>
#include <string>
#include "definitions.hpp"

struct KeyValuePair {
    std::string key, value;
};

struct Config {
    unsigned int image_width, image_height, max_iterations;
    mpf_t center_x, center_y, height_y;
    Number center_x_double, center_y_double, height_y_double;

    unsigned int num_palette_points;
    PalettePoint* palette_points;
    std::string output_path;
    Device device;
    int limit;
};

KeyValuePair get_key_value_pair(std::string line);
void process_config_file(std::string file, Config* config);
Config get_default_config();

#endif