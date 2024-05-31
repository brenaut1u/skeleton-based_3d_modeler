#include <math.h>
#include "draw.h"

using std::abs;
using std::sqrt;
using std::ceil;
using std::round;
using std::min;
using std::max;

void color_pixel(span3D image, pair<int, int> pos, color color) {
    int image_height = image.size_Y();
    image(pos.first, image_height - pos.second -1, 0) = color.x();
    image(pos.first, image_height - pos.second - 1, 1) = color.y();
    image(pos.first, image_height - pos.second - 1, 2) = color.z();
}

void draw_line(span3D image, pair<int, int> start, pair<int, int> end, int radius, color background_color, color border_color) {
    // Bresenham's line algorithm
    if (abs(end.second - start.second) < abs(end.first - start.first)) {
        if (start.first > end.first) {
            draw_line_low(image, end, start, radius, background_color, border_color);
        }
        else {
            draw_line_low(image, start, end, radius, background_color, border_color);
        }
    }
    else {
        if (start.second > end.second) {
            draw_line_high(image, end, start, radius, background_color, border_color);
        }
        else {
            draw_line_high(image, start, end, radius, background_color, border_color);
        }
    }
}

void draw_line_low(span3D image, pair<int, int> start, pair<int, int> end, int radius, color background_color, color border_color) {
    int dx = end.first - start.first;
    int dy = end.second - start.second;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = start.second;

    for (int x = start.first; x <= end.first; x++) {
        // draw the outline
        color_pixel(image, {x, y - radius / 2 - 1}, border_color);
        color_pixel(image, {x, y + radius / 2 + 1}, border_color);

        // draw the inside
        for (int i = -radius / 2; i <= radius / 2; i++) {
            color_pixel(image, {x, y + i}, background_color);
        }
        if (D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else {
            D = D + 2 * dy;
        }
    }
}

void draw_line_high(span3D image, pair<int, int> start, pair<int, int> end, int radius, color background_color, color border_color) {
    int dx = end.first - start.first;
    int dy = end.second - start.second;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = start.first;

    for (int y = start.second; y <= end.second; y++) {
        // draw the outline
        color_pixel(image, {x - radius / 2 - 1, y}, border_color);
        color_pixel(image, {x + radius / 2 + 1, y}, border_color);

        // draw the inside
        for (int i = -radius / 2; i <= radius / 2; i++) {
            color_pixel(image, {x + i, y}, background_color);
        }
        if (D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else {
            D = D + 2 * dx;
        }
    }
}

void draw_circle(span3D image, pair<int, int> center, int radius, color background_color, color border_color) {
    for (int x = - radius; x <= radius; x++) {
        int thickness = round(sqrt(radius * radius - x * x));

        // this is needed because otherwise, the circle's outline wouldn't be continuous
        int thickness2;
        if (x < 0) {
            thickness2 = round(sqrt(radius * radius - (x + 1) * (x + 1)));
        }
        else {
            thickness2 = round(sqrt(radius * radius - (x - 1) * (x - 1)));
        }

        // draw the outline
        if (thickness == thickness2) {
            color_pixel(image, {center.first + x, center.second - thickness}, border_color);
            color_pixel(image, {center.first + x, center.second + thickness}, border_color);
        }
        else {
            for (int i = thickness; i < thickness2; i++) {
                color_pixel(image, {center.first + x, center.second - i}, border_color);
                color_pixel(image, {center.first + x, center.second + i}, border_color);
            }
        }

        // draw the inside
        for (int y = -thickness + 1; y <= thickness - 1; y++) {
            color_pixel(image, {center.first + x, center.second + y}, background_color);
        }
    }
}