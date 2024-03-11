#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"
#include "color.h"

struct light {
    vec3 pos;
    color ray_color;
};

light new_light(vec3 pos);

light new_colored_light(vec3 pos, color ray_color);
#endif