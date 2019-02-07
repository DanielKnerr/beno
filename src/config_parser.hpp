#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
#include <string>
#include "definitions.hpp"

struct KeyValuePair {
    std::string key, value;
};

struct Config {
    unsigned int image_width, image_height, max_iterations;
    double center_x, center_y, radius_y;

    unsigned int num_palette_points;
    PalettePoint* palette_points;
};

KeyValuePair get_key_value_pair(std::string line);
void process_config_file(std::string file, Config* config);
Config get_default_config();

#endif