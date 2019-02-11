#include <iostream>
#include <limits>

#include "cli_parser.hpp"
#include "config_parser.hpp"
#include "definitions.hpp"

int main(int argc, char **argv) {
    Arguments arguments = parse_arguments(argc, argv);
    if (!validate_arguments(&arguments)) {
        std::cout << "Invalid arguments!\n";
        throw std::exception();
    }
    Config config = get_default_config();
    process_config_file(arguments.config_file, &config);

    typedef std::numeric_limits<Number> number_type;

    std::cout.precision(number_type::max_digits10);

    Number out_x = config.center_x_double - config.height_y_double + (Number)arguments.x / config.image_width * 2 * config.height_y_double;
    Number out_y = config.center_y_double + config.height_y_double - (Number)arguments.y / config.image_height * 2 * config.height_y_double;
    std::cout << out_x << "\n";
    std::cout << out_y << "\n";
    return 0;
}