#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP
#include <string>

struct Options
{
    unsigned int max_iterations;
    unsigned int palette_resolution;
    int width, height;
    std::string output;
    bool use_gpu;
};

Options defaultOptions();
void parse_options(int arg_count, char **arg_vector, Options *options);
void print_options(Options *options);
#endif