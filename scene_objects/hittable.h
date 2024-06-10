#ifndef HITTABLE_H
#define HITTABLE_H

#include "../utilities/ray.h"
#include "../utilities/interval.h"

class material;


/**
 * This class is used to store information about intersections between rays and objects' surfaces: surface normal,
 * intersection point, material, etc.
 */

class hit_record {
  public:
    point3 p; // intersection point
    vec3 normal;
    shared_ptr<material> mat; // material at p
    double t; // such as p = ray.origin + t * ray.direction
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal);
};


/**
 * This class is the base class for every world object that has a rendered surface.
 */

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const = 0;
    // The show_selec parameter determines whether the selection state affects the hit_record
};

#endif