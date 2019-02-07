#include <iostream>
#include <png++/png.hpp>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <vector>
#include <gmpxx.h>
#include <chrono>

#include "palette_generator.cpp"
#include "kernel.hpp"
#include "cli_parser.hpp"
#include "config_parser.hpp"
#include "definitions.hpp"

int main(int argc, char **argv)
{
    Arguments arguments = parse_arguments(argc, argv);
    if (!validate_arguments(&arguments)) {
        std::cout << "Invalid arguments!\n";
        throw std::exception();
    }
    Config config = get_default_config();
    process_config_file(arguments.config_file, &config);

    if (config.num_palette_points <= 0) {
        std::cout << "Invalid palette!\n";
        throw std::exception();
    }

    const int palette_res = 256;
    Color palette[palette_res];
    linear_interpolation(config.palette_points, config.num_palette_points - 1, palette, palette_res);

    unsigned int numPixels = config.image_width * config.image_height;

    double aspect_ratio = (double)config.image_width / config.image_height;

    ComplexNumber *image_points_data = new ComplexNumber[numPixels];

    for (unsigned int y = 0; y < config.image_height; y++)
    {
        for (unsigned int x = 0; x < config.image_width; x++)
        {
            // scale
            double a = ((double)y / config.image_height) * (2 * config.radius_y);
            double b = ((double)x / config.image_width) * (2 * aspect_ratio * config.radius_y);

            a -= config.radius_y;
            b -= (aspect_ratio * config.radius_y);

            // apply offset
            b += config.center_x;
            a += config.center_y;

            image_points_data[y * config.image_width + x] = ComplexNumber{b, a};
        }
    }

    double *output = new double[numPixels];
    png::image<png::rgb_pixel> image(config.image_width, config.image_height);
    unsigned int limit = 2;

    auto t1 = std::chrono::high_resolution_clock::now();
    if (false) {
        printf("GPU: ");
        startThreadsGPU(image_points_data, output, &limit, &config.max_iterations, &numPixels);
    } else {
        printf("CPU: ");
        startThreadsCPU(image_points_data, output, &limit, &config.max_iterations, &numPixels);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    printf("%fms\n", time_span.count() * 1000);

    std::vector<double> smooth_iterations_grid;
    smooth_iterations_grid.assign(output, output + numPixels);
    std::vector<double> smooth_iterations_grid_sorted(smooth_iterations_grid);

    std::sort(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end());

    for (unsigned int y = 0; y < config.image_height; y++)
    {
        for (unsigned int x = 0; x < config.image_width; x++)
        {
            double col = smooth_iterations_grid[y * config.image_width + x];

            auto lowerIter = std::lower_bound(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end(), col);
            int idx = std::distance(smooth_iterations_grid_sorted.begin(), lowerIter);

            double hue = (double)idx / numPixels;

            Color color = palette[(int)(hue * palette_res)];
            image[y][x] = png::rgb_pixel(color.r, color.g, color.b);
        }
    }

    image.write("output.png");

    return 0;
}