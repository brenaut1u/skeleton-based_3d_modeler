#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel);

#endif