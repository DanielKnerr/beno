#include <math.h>
#include <mpfr.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>

#include "renderer.hpp"

double normalized_iterations_count(Number c_a, Number c_b, unsigned int limit, unsigned int max_iterations) {
    ComplexNumber c = {c_a, c_b};
    ComplexNumber z = {0.0, 0.0};
    Number buffer = 0;

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

double normalized_iterations_count_mpf(mpf_t c_real, mpf_t c_imag, unsigned int limit, unsigned int max_iterations) {
    mpf_t z_real, z_imag, buffer_1, buffer_2;
    mpf_init_set_d(z_real, 0);
    mpf_init_set_d(z_imag, 0);

    mpf_init_set_d(buffer_1, 0);
    mpf_init_set_d(buffer_2, 0);

    for (int i = 0; i < max_iterations; i++) {
        mpf_mul(buffer_1, z_real, z_real);
        mpf_mul(buffer_2, z_imag, z_imag);

        mpf_add(buffer_1, buffer_1, buffer_2);
        int comparison = mpf_cmp_ui(buffer_1, limit * limit);

        if (comparison > 0) {
            // four more iterations to smooth it out
            for (int j = 0; j < 4; j++) {
                mpf_mul(buffer_1, z_real, z_real);
                mpf_mul(buffer_2, z_imag, z_imag);

                mpf_sub(buffer_1, buffer_1, buffer_2);
                mpf_add(buffer_1, buffer_1, c_real);

                mpf_mul(buffer_2, z_real, z_imag);
                mpf_mul_ui(buffer_2, buffer_2, 2);
                mpf_add(z_imag, buffer_2, c_imag);  // output directly into z_imag

                mpf_set(z_real, buffer_1);
            }
            double z_real_dbl = mpf_get_d(z_real);
            double z_imag_dbl = mpf_get_d(z_imag);

            double modulus = std::sqrt(z_real_dbl * z_real_dbl + z_imag_dbl * z_imag_dbl);
            double smooth_iterations = 5 + i - std::log(std::log(modulus)) / std::log(2);

            return smooth_iterations;
        } else {
            mpf_mul(buffer_1, z_real, z_real);
            mpf_mul(buffer_2, z_imag, z_imag);

            mpf_sub(buffer_1, buffer_1, buffer_2);
            mpf_add(buffer_1, buffer_1, c_real);

            mpf_mul(buffer_2, z_real, z_imag);
            mpf_mul_ui(buffer_2, buffer_2, 2);
            mpf_add(z_imag, buffer_2, c_imag);  // output directly into z_imag

            mpf_set(z_real, buffer_1);
        }
    }
    return 0.0;
}

void startThreadBlock(int start, int end, ComplexNumberMPF *start_point, ComplexNumberMPF *step_size, double *output, Config *config, bool use_mpf) {
    int image_points = config->image_height * config->image_width;

    mpf_t r, i;
    mpf_init(r);
    mpf_init(i);

    for (unsigned int point_idx = start; point_idx < end && point_idx < image_points; point_idx++) {
        int idx_i = point_idx / config->image_width;
        int idx_r = point_idx - idx_i * config->image_width;

        mpf_set_ui(i, 0);
        mpf_set_ui(r, 0);

        mpf_mul_ui(r, (*step_size).r, idx_r);
        mpf_add(r, r, (*start_point).r);

        mpf_mul_ui(i, (*step_size).i, idx_i);
        mpf_add(i, i, (*start_point).i);

        // for (unsigned int i = 0; i < *image_points; i++) {
        // output[i] = normalized_iterations_count(input[i].a, input[i].b, *limit, *max_iterations);

        if (use_mpf) {
            output[point_idx] = normalized_iterations_count_mpf(r, i, config->limit, config->max_iterations);
        } else {
            long double a = mpf_get_d(r);
            long double b = mpf_get_d(i);
            output[point_idx] = normalized_iterations_count(a, b, config->limit, config->max_iterations);
        }
    }
}

void startThreadsCPU(ComplexNumberMPF *start_point, ComplexNumberMPF *step_size, double *output, Config *config, bool use_mpf) {
    mpf_set_default_prec(64);

    int image_points = config->image_height * config->image_width;
    const int numThreads = 8;
    int pointsPerBlock = image_points / numThreads;
    std::thread *threads = new std::thread[numThreads];

    for (int i = 0; i < numThreads; i++) {
        threads[i] = std::thread(startThreadBlock, pointsPerBlock * i, pointsPerBlock * (i + 1), start_point, step_size, output, config, use_mpf);
    }

    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }
    delete[] threads;
}