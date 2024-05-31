#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
  public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
      : center(_center), radius(_radius), mat(_material) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    point3 get_center() const {
      return center;
    }

    double get_radius() const {
      return radius;
    }

    shared_ptr<material> get_material() const {
      return mat;
    }

    void set_center(point3 new_center) {
        center = new_center;
    }

    void move_center(vec3 delta_center) {
        center += delta_center;
    }

    void set_radius(double new_radius) {
        radius = new_radius;
        radius = max(radius,0.0);
    }

    void increase_radius(double delta_radius) {
        radius += delta_radius;
        radius = max(radius,0.0);
    }

    void set_mat(shared_ptr<material> _mat) {
        mat = _mat;
    }

  private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif