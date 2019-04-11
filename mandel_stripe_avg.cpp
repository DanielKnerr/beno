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
#include <complex>

struct ComplexNumber
{
    float x, y;
};

const int MAX_ITERATIONS = 500;

double arg(double r, double i) {
    // return 1 / std::tan(i / r);
    return std::tan(r / i);
}

double normalized_iterations_count(double c_a, double c_b, unsigned int limit) {
    double z_a = 0, z_b = 0;
    double buffer = 0;

    double t_orbit[MAX_ITERATIONS] = {0};

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        if ((z_a * z_a + z_b * z_b) > 9000000000000000000) {
            double total = 0;
            for (int j = 0; j < i; j++) {
                total += t_orbit[j];
            }
            return total / (double) i;
        }

        buffer = z_a * z_a - z_b * z_b + c_a;
        z_b = 2 * z_a * z_b + c_b;
        z_a = buffer;

        std::complex<double> c(z_a, z_b);
        t_orbit[i] = std::abs(std::sin(1 * std::arg(c))) / 2 + 1 / 2;
    }
    return 0.0;
}

// TODO: use threading
int main(int argc, char **argv)
{

    const int image_width = 1000, image_height = 1000;
    png::image<png::rgb_pixel> image(image_width, image_height);
    
    // float left = -2.0f, right = 1.0f, top = 1.5f, bottom = -1.5f;
    float left = -1.0f, right = 0.5f, top = 1.5f, bottom = 0.0f;
    // float left = -0.75f, right = 0.25f, top = 1.5f, bottom = 0.5f;

    float range_horizontal = std::abs(left - right), range_vertical = std::abs(top - bottom);
    float step_horizontal = range_horizontal / image_width, step_vertical = range_vertical / image_height;

    for (int x = 0; x < image_width; x++)
    {
        for (int y = 0; y < image_height; y++)
        {
            float coord_y = y * step_vertical + bottom;
            float coord_x = x * step_horizontal + left;

            double brightness = normalized_iterations_count(coord_x, coord_y, 2);
            brightness *= 600;
            if (brightness > 255) brightness = 255;
            image[image_height - 1 - y][x] = png::rgb_pixel(brightness, brightness, brightness);
        }
    }


    image.write("output.png");

    return 0;
}