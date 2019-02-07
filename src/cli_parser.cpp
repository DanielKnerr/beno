#include "cli_parser.hpp"

#include <iostream>

void initiliaze_arguments(Arguments* arguments) { arguments->config_file = ""; }

Arguments parse_arguments(int arg_count, char** arg_vector) {
    Arguments arguments;
    initiliaze_arguments(&arguments);

    for (int i = 1; i < arg_count; i++) {
        std::string argument(arg_vector[i]);
        
        if (argument.at(0) == '-' && argument.at(1) == '-') {
            int seperator = argument.find('=');
            if (argument.substr(2, seperator - 2) == "config") {
                arguments.config_file = argument.substr(seperator + 1);
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