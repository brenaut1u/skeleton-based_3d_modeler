#ifndef LIGHT_H
#define LIGHT_H

#include "../utilities/vec3.h"
#include "../utilities/color.h"

/**
 * These classes represent a light that can be used in Phong's shading.
 */

class light {
public:
    virtual ~light() = default;
    virtual vec3 get_light_ray(point3 p) = 0; // returns a ray going from the light's origin to point p
    virtual color get_color() const = 0;
};


/**
 * A light represented by a point
 */

class point_light : public light {
public:
    point_light(point3 _pos) : pos(_pos) {
        color = {1.0, 1.0, 1.0};
    }

    point_light(point3 _pos, color _color) : pos(_pos), color(_color) {}

    vec3 get_light_ray(point3 p) override {
        return pos - p;
    }

    color get_color() const override {
        return color;
    }

private:
    point3 pos;
    color color;
};


/**
 * A light represented by a vector. The light could be described as a point light located at an infinite distance from
 * the objects
 */

class vector_light : public light {
public:
    vector_light(vec3 _dir) : dir(_dir) {
        color = {1.0, 1.0, 1.0};
    }

    vector_light(vec3 _dir, color _color) : dir(_dir), color(_color) {}

    vec3 get_light_ray(point3 p) override {
        return dir;
    }

    color get_color() const override {
        return color;
    }

private:
    vec3 dir;
    color color;
};

#endif