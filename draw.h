#ifndef DRAW_H
#define DRAW_H

#include "color.h"
#include "span3D.h"

using std::pair;

void color_pixel(span3D image, pair<int, int> pos, color color);

void draw_line(span3D image, pair<int, int> start, pair<int, int> end, int radius, color background_color, color border_color);

void draw_circle(span3D image, pair<int, int> center, int radius, color background_color, color border_color);

#endif