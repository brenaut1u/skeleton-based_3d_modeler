#ifndef DRAW_H
#define DRAW_H

#include "../utilities/color.h"
#include "span3D.h"
#include "screen_segment.h"

/**
 * This file contains functions useful to draw on an image of type span3D.
 */

using std::pair;

void color_pixel(span3D image, pair<int, int> pos, color color);

void draw_line(span3D image, screen_point start, screen_point end, int radius, color background_color, color border_color); // Bresenham's line algorithm

// These two functions are only used in draw_line, according to Bresenham's line algorithm
void draw_line_low(span3D image, screen_point start, screen_point end, int radius, color background_color, color border_color);
void draw_line_high(span3D image, screen_point start, screen_point end, int radius, color background_color, color border_color);

void draw_circle(span3D image, screen_point center, int radius, color background_color, color border_color);

#endif