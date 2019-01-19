#ifndef KERNEL_HPP
#define KERNEL_HPP

struct ComplexNumber
{
    double a;
    double b;
};

void startThreads(ComplexNumber *input, double *output, int *max_iterations, int *image_points);
#endif