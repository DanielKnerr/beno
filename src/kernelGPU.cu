#include <cuda.h>
#include "kernel.hpp"
#include <math.h>

__global__ void normalized_iterations_count(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *p_max_iterations, unsigned int *p_image_points)
{
    int max_iterations = *p_max_iterations;
    int image_points = *p_image_points;

    int id = id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id < image_points)
    {
        ComplexNumber c = input[id];
        ComplexNumber z = {0.0, 0.0};
        for (int i = 0; i < max_iterations; i++)
        {
            if ((z.a * z.a + z.b * z.b) > *limit * *limit)
            {
                // four more iterations to smooth it out
                for (int j = 0; j < 4; j++)
                {
                    double newA = z.a * z.a - z.b * z.b + c.a;
                    double newB = 2 * z.a * z.b + c.b;
                    z.a = newA;
                    z.b = newB;
                }
                double modulus = std::sqrt(z.a * z.a + z.b * z.b);
                double smooth_iterations = 5 + i - std::log(std::log(modulus)) / std::log(2);

                output[id] = smooth_iterations;
                return;
            }
            else
            {
                double newA = z.a * z.a - z.b * z.b + c.a;
                double newB = 2 * z.a * z.b + c.b;
                z.a = newA;
                z.b = newB;
            }
        }

        output[id] = 0;
    }
}

void startThreadsGPU(ComplexNumber *input, double *output, unsigned int *limit, unsigned int *max_iterations, unsigned int *image_points)
{
    // Create device pointers
    ComplexNumber *d_input;
    double *d_output;
    unsigned int *d_max_iterations, *d_image_points, *d_limit;

    // Calculate grid
    int threads = 256;
    int blocks = std::ceil(*image_points / threads);

    // Allocate memory to the device pointers on the GPU
    cudaMalloc(&d_input, sizeof(ComplexNumber) * *image_points);
    cudaMalloc(&d_output, sizeof(double) * *image_points);
    cudaMalloc(&d_max_iterations, sizeof(unsigned int));
    cudaMalloc(&d_image_points, sizeof(unsigned int));
    cudaMalloc(&d_limit, sizeof(unsigned int));

    // Copy data into the allocated memory
    cudaMemcpy(d_input, input, sizeof(ComplexNumber) * *image_points, cudaMemcpyHostToDevice);
    cudaMemcpy(d_max_iterations, max_iterations, sizeof(unsigned int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_image_points, image_points, sizeof(unsigned int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_limit, limit, sizeof(unsigned int), cudaMemcpyHostToDevice);

    // Execute kernel
    normalized_iterations_count<<<blocks, threads>>>(d_input, d_output, d_limit, d_max_iterations, d_image_points);

    // Sync ??????????????????????????????
    cudaDeviceSynchronize();

    // Copy the output back to the host
    cudaMemcpy(output, d_output, sizeof(double) * *image_points, cudaMemcpyDeviceToHost);

    // Free memory
    cudaFree(&d_input);
    cudaFree(&d_output);
    cudaFree(&d_max_iterations);
    cudaFree(&d_image_points);

    // Reset the GPU
    cudaDeviceReset();
}