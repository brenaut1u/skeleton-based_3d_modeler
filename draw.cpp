#include <math.h>
#include "draw.h"

using std::abs;
using std::sqrt;
using std::ceil;
using std::round;

void color_pixel(span3D image, pair<int, int> pos, color color) {
    int image_height = image.size_Y();
    image(pos.first, image_height - pos.second -1, 0) = color.x();
    image(pos.first, image_height - pos.second - 1, 1) = color.y();
    image(pos.first, image_height - pos.second - 1, 2) = color.z();
}

void draw_line(span3D image, pair<int, int> start, pair<int, int> end, int radius, color background_color, color border_color) {
    int line_width = abs(end.first - start.first);
    int line_height = abs(end.second - start.second);

    if (line_width > line_height) {
        if (start.first > end.first) {
            pair<int, int> tmp = start;
            start = end;
            end = tmp;
        }

        if (line_height > 0) {
            int pix_steps = ceil(line_width / line_height);

            for (int x = start.first; x <= end.first; x++) {
                int y;
                if (end.second > start.second) {
                    y = start.second + (x - start.first) / pix_steps;
                }
                else {
                    y = start.second - (x - start.first) / pix_steps;
                }


                // draw the outine
                color_pixel(image, {x, y - radius / 2 - 1}, border_color);
                color_pixel(image, {x, y + radius / 2 + 1}, border_color);

                // draw the inside
                for (int i = -radius / 2; i <= radius / 2; i++) {
                    color_pixel(image, {x, y + i}, background_color);
                }
            }
        }
        else {
            // if the line is horizontal
            for (int x = start.first; x <= end.first; x++) {
                // draw the outline
                color_pixel(image, {x, start.second - radius / 2 - 1}, border_color);
                color_pixel(image, {x, start.second + radius / 2 + 1}, border_color);

                // draw the inside
                for (int i = -radius / 2; i <= radius / 2; i++) {
                    color_pixel(image, {x, start.second + i}, background_color);
                }
            }
        }

    }
    else {
        if (start.second > end.second) {
            pair<int, int> tmp = start;
            start = end;
            end = tmp;
        }

        if (line_width > 0) {
            int pix_steps = ceil(line_height / line_width);

            for (int y = start.second; y <= end.second; y++) {
                int x;
                if (end.first > start.first) {
                    x = start.first + (y - start.second) / pix_steps;
                }
                else {
                    x = start.first - (y - start.second) / pix_steps;
                }

                // draw the outline
                color_pixel(image, {x - radius / 2 - 1, y}, border_color);
                color_pixel(image, {x + radius / 2 + 1, y}, border_color);

                // draw the inside
                for (int i = -radius / 2; i <= radius / 2; i++) {
                    color_pixel(image, {x + i, y}, background_color);
                }
            }
        }
        else {
            // if the line is vertical
            for (int y = start.second; y <= end.second; y++) {
                // draw the outline
                color_pixel(image, {start.first - radius / 2 - 1, y}, border_color);
                color_pixel(image, {start.first + radius / 2 + 1, y}, border_color);

                // draw the inside
                for (int i = -radius / 2; i <= radius / 2; i++) {
                    color_pixel(image, {start.first + i, y}, background_color);
                }
            }
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