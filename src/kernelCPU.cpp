#include <math.h>
#include <mpfr.h>
#include <stdio.h>
#include "kernel.hpp"

double normalized_iterations_count(double c_a, double c_b, unsigned int limit, unsigned int max_iterations) {
    ComplexNumber c = {c_a, c_b};
    ComplexNumber z = {0.0, 0.0};
    double buffer = 0;

    for (int i = 0; i < max_iterations; i++) {
        if ((z.a * z.a + z.b * z.b) > limit * limit) {
            // four more iterations to smooth it out
            for (int j = 0; j < 4; j++) {
                buffer = z.a * z.a - z.b * z.b + c.a;
                z.b = 2 * z.a * z.b + c.b;
                z.a = buffer;
            }
            double modulus = std::sqrt(z.a * z.a + z.b * z.b);
            double smooth_iterations = 5 + i - std::log(std::log(modulus)) / std::log(2);

            return smooth_iterations;
        } else {
            buffer = z.a * z.a - z.b * z.b + c.a;
            z.b = 2 * z.a * z.b + c.b;
            z.a = buffer;
        }
    }
    return 0.0;
}

double normalized_iterations_count_mpf(double c_a_dbl, double c_b_dbl, unsigned int limit, unsigned int max_iterations) {
    mpf_t z_a, z_b, c_a, c_b, buffer_1, buffer_2;
    mpf_init_set_d(z_a, 0);
    mpf_init_set_d(z_b, 0);
    mpf_init_set_d(c_a, c_a_dbl);
    mpf_init_set_d(c_b, c_b_dbl);

    mpf_init_set_d(buffer_1, 0);
    mpf_init_set_d(buffer_2, 0);

    for (int i = 0; i < max_iterations; i++) {
        mpf_mul(buffer_1, z_a, z_a);
        mpf_mul(buffer_2, z_b, z_b);

        mpf_add(buffer_1, buffer_1, buffer_2);
        int comparison = mpf_cmp_ui(buffer_1, limit * limit);

        if (comparison > 0) {
            // four more iterations to smooth it out
            for (int j = 0; j < 4; j++) {
                mpf_mul(buffer_1, z_a, z_a);
                mpf_mul(buffer_2, z_b, z_b);

                mpf_sub(buffer_1, buffer_1, buffer_2);
                mpf_add(buffer_1, buffer_1, c_a);

                mpf_mul(buffer_2, z_a, z_b);
                mpf_mul_ui(buffer_2, buffer_2, 2);
                mpf_add(z_b, buffer_2, c_b);  // output directly into z_b

                mpf_set(z_a, buffer_1);
            }
            double zad = mpf_get_d(z_a);
            double zbd = mpf_get_d(z_b);

            double modulus = std::sqrt(zad * zad + zbd * zbd);
            double smooth_iterations = 5 + i - std::log(std::log(modulus)) / std::log(2);

            return smooth_iterations;
        } else {
            mpf_mul(buffer_1, z_a, z_a);
            mpf_mul(buffer_2, z_b, z_b);

            mpf_sub(buffer_1, buffer_1, buffer_2);
            mpf_add(buffer_1, buffer_1, c_a);

            mpf_mul(buffer_2, z_a, z_b);
            mpf_mul_ui(buffer_2, buffer_2, 2);
            mpf_add(z_b, buffer_2, c_b);  // output directly into z_b

            mpf_set(z_a, buffer_1);
        }
    }
    return 0.0;
}

void startThreadsCPU(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *max_iterations, unsigned int *image_points) {
    for (unsigned int i = 0; i < *image_points; i++) {
        // output[i] = normalized_iterations_count(input[i].a, input[i].b, *limit, *max_iterations);
        output[i] = normalized_iterations_count_mpf(input[i].a, input[i].b, *limit, *max_iterations);
    }
}