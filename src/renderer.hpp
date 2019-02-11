#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <gmpxx.h>
#include "config_parser.hpp"

struct ComplexNumberMPF {
    mpf_t r, i;
};

struct ComplexNumber {
    Number a, b;
};

struct ComplexNumberGPU {
    double a, b; // CUDA can only handle 64bits
};

void startThreadsCPU(ComplexNumberMPF *start_point, ComplexNumberMPF *step_size, double *output, Config *config, bool use_mpf);
void startThreadsGPU(ComplexNumberMPF *start_point, ComplexNumberMPF *step_size, double *output, Config *config);

#endif