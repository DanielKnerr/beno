#include <iostream>
#include <png++/png.hpp>
#include <math.h>

struct Color
{
    int r, g, b;
};

struct PalettePoint
{
    Color color;
    double position;
};

void linear_interpolation(PalettePoint *palettePoints, int numPoints, Color *palette, int resolution)
{
    for (int i = 0; i < numPoints; i++)
    {
        int startPixel = palettePoints[i].position * resolution;
        int endPixel = palettePoints[i + 1].position * resolution;

        Color c1 = palettePoints[i].color;
        Color c2 = palettePoints[i + 1].color;

        for (int x = startPixel; x < endPixel; x++)
        {
            double position = (double)(x - startPixel) / (endPixel - startPixel);

            double nR = (1.0f - position) * c1.r + position * c2.r;
            double nG = (1.0f - position) * c1.g + position * c2.g;
            double nB = (1.0f - position) * c1.b + position * c2.b;

            palette[x] = Color{(int)nR, (int)nG, (int)nB};
        }
    }
}