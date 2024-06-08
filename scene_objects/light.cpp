#include "light.h"

light new_light(vec3 pos) {
    light l;
    l.pos = pos;
    l.ray_color = {1.0, 1.0, 1.0};
    return l;
}

light new_colored_light(vec3 pos, color ray_color) {
    light l;
    l.pos = pos;
    l.ray_color = ray_color;
    return l;
}