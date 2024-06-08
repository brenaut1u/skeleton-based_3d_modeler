#ifndef LIGHT_H
#define LIGHT_H

#include "../utilities/vec3.h"
#include "../utilities/color.h"

struct light {
    vec3 pos;
    color ray_color;
};

light new_light(vec3 pos);

light new_colored_light(vec3 pos, color ray_color);
#endif