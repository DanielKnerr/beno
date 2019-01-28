#ifndef KERNEL_HPP
#define KERNEL_HPP

struct ComplexNumber
{
    double a;
    double b;
};

void startThreadsGPU(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *max_iterations, unsigned int *image_points);
void startThreadsCPU(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *max_iterations, unsigned int *image_points);
#endif