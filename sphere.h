#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
  public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
      : center(_center), radius(_radius), mat(_material) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {        
        vec3 ro = r.origin();
        vec3 rd = unit_vector(r.direction());
        vec3 ce = center;
        auto ra = radius;

        vec3 oc = ro - ce;
        float b = dot( oc, rd );
        vec3 qc = oc - b*rd;
        float h = ra*ra - dot( qc, qc );
        if( h<0.0 ) return false; // no intersection
        h = sqrt( h );
        auto root = -b-h;
        if (!ray_t.surrounds(root / r.direction().length())) {
             root = -b+h;
             if (!ray_t.surrounds(root / r.direction().length()))
                 return false;
        }

        rec.t = root / r.direction().length();
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

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
    }

    void increase_radius(double delta_radius) {
        radius += delta_radius;
    }

  private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif