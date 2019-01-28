#include "kernel.hpp"
#include <math.h>

double normalized_iterations_count(double c_a, double c_b, unsigned int limit, unsigned int max_iterations) {
    ComplexNumber c = {c_a, c_b};
    ComplexNumber z = {0.0, 0.0};
    for (int i = 0; i < max_iterations; i++) {
        if ((z.a * z.a + z.b * z.b) > limit * limit) {
            // four more iterations to smooth it out
            for (int j = 0; j < 4; j++) {
                double newA = z.a * z.a - z.b * z.b + c.a;
                double newB = 2 * z.a * z.b + c.b;
                z.a = newA;
                z.b = newB;
            }
            double modulus = std::sqrt(z.a * z.a + z.b * z.b);
            double smooth_iterations =
                5 + i - std::log(std::log(modulus)) / std::log(2);

            return smooth_iterations;
        } else {
            double newA = z.a * z.a - z.b * z.b + c.a;
            double newB = 2 * z.a * z.b + c.b;
            z.a = newA;
            z.b = newB;
        }
    }
    return 0.0;
}

void startThreadsCPU(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *max_iterations, unsigned int *image_points) {
    for (unsigned int i = 0; i < *image_points; i++) {
        output[i] = normalized_iterations_count(input[i].a, input[i].b, *limit, *max_iterations);
    }
}