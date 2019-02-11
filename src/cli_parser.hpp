#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP
#include <string>

struct Arguments
{
    std::string config_file;
    int x, y;
};

Arguments parse_arguments(int arg_count, char **arg_vector);
bool validate_arguments(Arguments* arguments);

#endif