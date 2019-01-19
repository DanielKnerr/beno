#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP
#include <string>

struct Options
{
    int max_iterations;
    int palette_resolution;
    int width, height;
    std::string output;
};

Options defaultOptions();
void parse_options(int arg_count, char **arg_vector, Options *options);
void print_options(Options *options);
#endif