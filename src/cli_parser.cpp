#include "cli_parser.hpp"

#include <iostream>

void initiliaze_arguments(Arguments* arguments) {
    arguments->config_file = "";
    arguments->x = 0.0;
    arguments->y = 0.0;
}

Arguments parse_arguments(int arg_count, char** arg_vector) {
    Arguments arguments;
    initiliaze_arguments(&arguments);

    for (int i = 1; i < arg_count; i++) {
        std::string argument(arg_vector[i]);

        if (argument.at(0) == '-' && argument.at(1) == '-') {
            int seperator = argument.find('=');

            std::string arg_name = argument.substr(2, seperator - 2);
            std::string arg_value = argument.substr(seperator + 1);

            if (arg_name == "config") {
                arguments.config_file = arg_value;
            } else if (arg_name == "x") {
                arguments.x = std::stoi(arg_value);
            } else if (arg_name == "y") {
                arguments.y = std::stoi(arg_value);
            }
        }
    }

    return arguments;
}

bool validate_arguments(Arguments* arguments) {
    bool valid = false;

    if (arguments->config_file != "") valid = true;

    return valid;
}