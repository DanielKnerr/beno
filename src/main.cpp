#include <gmpxx.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <png++/png.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "cli_parser.hpp"
#include "config_parser.hpp"
#include "definitions.hpp"
#include "renderer.hpp"
#include "palette_generator.cpp"

int main(int argc, char **argv) {
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

    const int palette_res = 512;
    Color palette[palette_res];
    linear_interpolation(config.palette_points, config.num_palette_points - 1, palette, palette_res);

    unsigned int numPixels = config.image_width * config.image_height;

    double aspect_ratio = (double)config.image_width / config.image_height;

    mpf_t buffer_1, buffer_2, buffer_3;

    mpf_init_set_ui(buffer_1, 0);
    mpf_init_set_ui(buffer_2, 0);
    mpf_init_set_ui(buffer_3, 0);

    double *output = new double[numPixels];
    png::image<png::rgb_pixel> image(config.image_width, config.image_height);
    unsigned int limit = 2;

    auto t1 = std::chrono::high_resolution_clock::now();
    if (false) {
        printf("GPU: ");
        // startThreadsGPU(image_points_data, output, &limit, &config.max_iterations, &numPixels);
    } else {
        printf("CPU: ");
        ComplexNumberMPF start_point, step_size;
        mpf_init(start_point.r);
        mpf_init(start_point.i);
        mpf_init(step_size.r);
        mpf_init(step_size.i);

        mpf_sub(start_point.r, config.center_x, config.height_y);
        mpf_add(start_point.i, config.center_y, config.height_y);

        mpf_t buffer_1;
        mpf_init(buffer_1);
        mpf_add(buffer_1, config.height_y, config.height_y);
        mpf_div_ui(buffer_1, buffer_1, config.image_width);
        mpf_set(step_size.r, buffer_1);
        mpf_set(step_size.i, buffer_1);

        mpf_neg(step_size.i, step_size.i);

        printf("start_point.r: %f\n", mpf_get_d(start_point.r));
        printf("start_point.i: %f\n", mpf_get_d(start_point.i));

        printf("step_size.r: %f\n", mpf_get_d(step_size.r));
        printf("step_size.i: %f\n", mpf_get_d(step_size.i));

        switch (config.device) {
            case Device::CPU_MPF: {
                printf("Using CPU_MPF\n");
                startThreadsCPU(&start_point, &step_size, output, &config, true);
                break;
            }
            case Device::CPU: {
                printf("Using CPU\n");
                startThreadsCPU(&start_point, &step_size, output, &config, false);
                break;
            }
            case Device::GPU: {
                printf("Using GPU\n");
                startThreadsGPU(&start_point, &step_size, output, &config);
                break;
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    printf("%fms\n", time_span.count() * 1000);

    printf("%f\n", output[100 * config.image_width + 100]);

    std::vector<double> smooth_iterations_grid;
    smooth_iterations_grid.assign(output, output + numPixels);
    std::vector<double> smooth_iterations_grid_sorted(smooth_iterations_grid);

    std::sort(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end());

    for (unsigned int y = 0; y < config.image_height; y++) {
        for (unsigned int x = 0; x < config.image_width; x++) {
            double col = smooth_iterations_grid[y * config.image_width + x];

            auto lowerIter = std::lower_bound(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end(), col);
            int idx = std::distance(smooth_iterations_grid_sorted.begin(), lowerIter);

            double hue = (double)idx / numPixels;

            Color color = palette[(int)(hue * palette_res)];
            // image[x][y] = png::rgb_pixel(color.r, color.g, color.b);
            image[y][x] = png::rgb_pixel(color.r, color.g, color.b);
        }
    }

    image.write(config.output_path + ".png");

    return 0;
}