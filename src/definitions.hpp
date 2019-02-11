#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

typedef long double Number;

struct Color
{
    int r, g, b;
};

struct PalettePoint
{
    Color color;
    double position;
};

enum class Device { CPU, CPU_MPF, GPU };

#endif