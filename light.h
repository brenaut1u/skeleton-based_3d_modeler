#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"
#include "color.h"

struct light {
    vec3 pos;
    color ray_color;
};
typedef struct light light;

inline light new_light(vec3 pos) {
    light l;
    l.pos = pos;
    l.ray_color = {1.0, 1.0, 1.0};
    return l;
}

inline light new_colored_light(vec3 pos, color ray_color) {
    light l;
    l.pos = pos;
    l.ray_color = ray_color;
    return l;
}
#endif