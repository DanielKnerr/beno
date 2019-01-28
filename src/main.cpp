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

Options options = defaultOptions();

int main(int argc, char **argv)
{
    parse_options(argc, argv, &options);
    print_options(&options);

    // Red
    // PalettePoint palettePoints[] = {
    //     PalettePoint{Color{0, 0, 0}, 0.0f},
    //     PalettePoint{Color{150, 0, 0}, 0.8f},
    //     PalettePoint{Color{200, 0, 0}, 0.95f},
    //     PalettePoint{Color{255, 180, 0}, 1.0f}};

    // Red (more yellow)
    PalettePoint palettePoints[] = {
        PalettePoint{Color{0, 0, 0}, 0.0f},
        PalettePoint{Color{255, 0, 50}, 0.8f},
        PalettePoint{Color{255, 210, 0}, 1.0f}
    };

    // Teal
    // PalettePoint palettePoints[] = {
    //     PalettePoint{Color{0, 0, 0}, 0.0f},
    //     PalettePoint{Color{0, 20, 50}, 0.2f},
    //     PalettePoint{Color{0, 20, 60}, 0.45f},
    //     PalettePoint{Color{0, 100, 100}, 0.6f},
    //     PalettePoint{Color{0, 255, 255}, 0.85f},
    //     PalettePoint{Color{255, 255, 255}, 1.0f}
    // };

    // White
    // PalettePoint palettePoints[] = {
    //     PalettePoint{Color{0, 0, 0}, 0.0f},
    //     PalettePoint{Color{128,128, 128}, 0.75f},
    //     PalettePoint{Color{255, 255, 255}, 1.0f}
    // };

    int points = (sizeof(palettePoints) / sizeof(PalettePoint)) - 1;

    Color palette[options.palette_resolution];
    linear_interpolation(palettePoints, points, palette, options.palette_resolution);

    unsigned int numPixels = options.width * options.height;

    // double offsetX = -0.75;
    // double offsetY = 0;
    // double radiusY = 1.5f;

    double offsetX = -0.745428;
    double offsetY = 0.113009;
    double radiusY = 0.0001f;

    double aspect_ratio = (double)options.width / options.height;

    ComplexNumber *image_points_data = new ComplexNumber[numPixels];

    for (unsigned int y = 0; y < options.height; y++)
    {
        for (unsigned int x = 0; x < options.width; x++)
        {
            // scale
            double a = ((double)y / options.height) * (2 * radiusY);
            double b = ((double)x / options.width) * (2 * aspect_ratio * radiusY);

            a -= radiusY;
            b -= (aspect_ratio * radiusY);

            // apply offset
            b += offsetX;
            a += offsetY;

            image_points_data[y * options.width + x] = ComplexNumber{b, a};
        }
    }

    double *output = new double[numPixels];
    png::image<png::rgb_pixel> image(options.width, options.height);
    unsigned int limit = 2;

    auto t1 = std::chrono::high_resolution_clock::now();
    if (options.use_gpu) {
        printf("GPU: ");
        startThreadsGPU(image_points_data, output, &limit, &options.max_iterations, &numPixels);
    } else {
        printf("CPU: ");
        startThreadsCPU(image_points_data, output, &limit, &options.max_iterations, &numPixels);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    printf("%fms\n", time_span.count() * 1000);

    std::vector<double> smooth_iterations_grid;
    smooth_iterations_grid.assign(output, output + numPixels);
    std::vector<double> smooth_iterations_grid_sorted(smooth_iterations_grid);

    std::sort(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end());

    for (unsigned int y = 0; y < options.height; y++)
    {
        for (unsigned int x = 0; x < options.width; x++)
        {
            double col = smooth_iterations_grid[y * options.width + x];

            auto lowerIter = std::lower_bound(smooth_iterations_grid_sorted.begin(), smooth_iterations_grid_sorted.end(), col);
            int idx = std::distance(smooth_iterations_grid_sorted.begin(), lowerIter);

            double hue = (double)idx / numPixels;

            Color color = palette[(int)(hue * options.palette_resolution)];
            image[y][x] = png::rgb_pixel(color.r, color.g, color.b);
        }
    }

    image.write(options.output + ".png");

    return 0;
}