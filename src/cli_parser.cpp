#include "cli_parser.hpp"

#include <iostream> // TODO: DELETE

void parse_options(int arg_count, char **arg_vector, Options *options)
{
    // the first element is the command itself
    for (int i = 1; i < arg_count; i++)
    {
        if (arg_vector[i][0] == '-')
        {
            std::string command = arg_vector[i];

            // TODO
            // help page NOT ALL COMMANDS HAVE A SHORT VERSION
            // increase i after processing arguments

            // remove the dash(es) at the beginning
            if (arg_vector[i][1] == '-')
            {
                command = command.substr(2);
                if (command == "output")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->output = std::string(arg_vector[i + 1]);
                }

                else if (command == "max-iterations")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->max_iterations = std::atoi(arg_vector[i + 1]);
                }
                else if (command == "p-resolution")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->palette_resolution = std::atoi(arg_vector[i + 1]);
                }
                else if (command == "size")
                {
                    if (arg_count <= i + 2)
                        goto error;
                    options->width = std::atoi(arg_vector[i + 1]);
                    options->height = std::atoi(arg_vector[i + 2]);
                }
            }
            else
            {
                command = command.substr(1);
                if (command == "o")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->output = std::string(arg_vector[i + 1]);
                }
                else if (command == "i")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->max_iterations = std::atoi(arg_vector[i + 1]);
                }
                else if (command == "r")
                {
                    if (arg_count <= i + 1)
                        goto error;
                    options->palette_resolution = std::atoi(arg_vector[i + 1]);
                }
                else if (command == "s")
                {
                    if (arg_count <= i + 2)
                        goto error;
                    options->width = std::atoi(arg_vector[i + 1]);
                    options->height = std::atoi(arg_vector[i + 2]);
                }
            }
        }
    }

    return;

error:
    std::cout << "ERRO" << std::endl;
}

void print_options(Options *options)
{
    std::cout << options->max_iterations << '\n'
              << options->output << '\n'
              << options->palette_resolution << '\n'
              << options->width << '\n'
              << options->height << '\n';
}

Options defaultOptions()
{
    Options options;
    options.max_iterations = 128;
    options.palette_resolution = 256;
    options.width = 400;
    options.height = 300;
    options.output = "output";

    return options;
}