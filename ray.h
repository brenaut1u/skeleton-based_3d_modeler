#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "rtweekend.h"

class ray {
  public:
    ray() {}

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    point3 origin() const  { return orig; }
    vec3 direction() const { return dir; }

    point3 at(double t) const {
        return orig + t*dir;
    }

  private:
    point3 orig = {0.0, 0.0, 0.0};
    vec3 dir = {1.0, 0.0, 0.0};
};

#endif