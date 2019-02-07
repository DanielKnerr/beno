#include <iostream>
#include <png++/png.hpp>
#include <math.h>
#include "definitions.hpp"

void linear_interpolation(PalettePoint *palettePoints, int numPoints, Color *palette, int resolution)
{
    for (int i = 0; i < numPoints; i++)
    {
        int start = palettePoints[i].position * resolution;
        int end = palettePoints[i + 1].position * resolution;

        Color c1 = palettePoints[i].color;
        Color c2 = palettePoints[i + 1].color;

        for (int x = start; x < end; x++)
        {
            double position = (double)(x - start) / (end - start);

            double nR = (1.0f - position) * c1.r + position * c2.r;
            double nG = (1.0f - position) * c1.g + position * c2.g;
            double nB = (1.0f - position) * c1.b + position * c2.b;

            palette[x] = Color{(int)nR, (int)nG, (int)nB};
        }
    }
}