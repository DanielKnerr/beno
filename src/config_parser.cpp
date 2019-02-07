#include "config_parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void process_config_file(std::string file, Config* config) {
    std::ifstream in_file(file);

    std::string line;
    while (std::getline(in_file, line)) {
        if (line.at(0) != '#') {
            KeyValuePair kvp = get_key_value_pair(line);

            if (kvp.key == "width") {
                config->image_width = std::stoi(kvp.value);
            } else if (kvp.key == "height") {
                config->image_height = std::stoi(kvp.value);
            } else if (kvp.key == "center_x") {
                config->center_x = std::stod(kvp.value);
            } else if (kvp.key == "center_y") {
                config->center_y = std::stod(kvp.value);
            } else if (kvp.key == "radius_y") {
                config->radius_y = std::stod(kvp.value);
            } else if (kvp.key == "max_iterations") {
                config->max_iterations = std::stoi(kvp.value);
            } else if (kvp.key == "palette") {
                int idx = -1;
                std::vector<int> indexes;
                while (true) {
                    idx = kvp.value.find('{', idx + 1);

                    if (idx == std::string::npos) {
                        break;
                    }
                    indexes.push_back(idx);
                }

                std::vector<PalettePoint> points;
                for (int i = 0; i < indexes.size(); i++) {
                    std::string point_string = kvp.value.substr(indexes[i], indexes[i + 1] - indexes[i]);

                    idx = 0;
                    std::vector<int> comma_indexes;
                    comma_indexes.push_back(0);
                    for (int j = 0; j < 3; j++) {
                        idx = point_string.find(',', idx + 1);
                        comma_indexes.push_back(idx);
                    }

                    int red = 0, green = 0, blue = 0;
                    double position = 0.0;

                    int j = 0;
                    red = std::stoi(point_string.substr(comma_indexes[j] + 1, comma_indexes[j + 1] - comma_indexes[j] - 1));

                    j = 1;
                    green = std::stoi(point_string.substr(comma_indexes[j] + 1, comma_indexes[j + 1] - comma_indexes[j] - 1));

                    j = 2;
                    blue = std::stoi(point_string.substr(comma_indexes[j] + 1, comma_indexes[j + 1] - comma_indexes[j] - 1));

                    j = 3;
                    position = std::stod(point_string.substr(comma_indexes[j] + 1, comma_indexes[j + 1] - comma_indexes[j] - 1));

                    points.push_back(PalettePoint{Color{red, green, blue}, position});
                }

                config->num_palette_points = points.size();
                config->palette_points = new PalettePoint[config->num_palette_points];
                std::copy(points.begin(), points.end(), config->palette_points);
            }
        }
    }
}

KeyValuePair get_key_value_pair(std::string line) {
    KeyValuePair kvp;
    int seperator = line.find(": ");
    kvp.key = line.substr(0, seperator);
    kvp.value = line.substr(seperator + 2);
    return kvp;
}

Config get_default_config() {
    Config config;

    config.image_height = 400;
    config.image_width = 400;
    config.max_iterations = 100;

    config.center_x = -0.75;
    config.center_y = 0;
    config.radius_y = 1.5;

    config.num_palette_points = 0;
    config.palette_points = nullptr;

    return config;
}