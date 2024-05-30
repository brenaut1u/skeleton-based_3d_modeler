#ifndef CONE_H
#define CONE_H

#include "vec3.h"
#include "sphere.h"
#include "color.h"
#include "material.h"

class cone : public hittable {
  public:
    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _material)
      : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat1(_material), mat2(_material) {}

    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _mat1, shared_ptr<material> _mat2)
            : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat1(_mat1), mat2(_mat2) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    void set_center1(point3 new_center) {
        center1 = new_center;
    }

    void move_center1(vec3 delta_center) {
        center1 += delta_center;
    }

    void set_center2(point3 new_center) {
        center2 = new_center;
    }

    void move_center2(vec3 delta_center) {
        center2 += delta_center;
    }

    void set_radius1(double new_radius) {
        radius1 = new_radius;
        radius1 = max(radius1,0.0);
    }

    void set_radius2(double new_radius) {
        radius2 = new_radius;
        radius2 = max(radius2,0.0);
    }

    void increase_radius1(double delta_radius) {
        radius1 += delta_radius;
        radius1 = max(radius1,0.0);
    }

    void increase_radius2(double delta_radius) {
        radius2 += delta_radius;
        radius2 = max(radius2,0.0);
    }

    void set_mat1(shared_ptr<material> mat) {
        mat1 = mat;
    }

    void set_mat2(shared_ptr<material> mat) {
        mat2 = mat;
    }

  private:
    point3 center1;
    point3 center2;
    double radius1;
    double radius2;
    shared_ptr<material> mat1;
    shared_ptr<material> mat2;
};

static shared_ptr<cone> cone_from_spheres(shared_ptr<sphere> sphere1, shared_ptr<sphere> sphere2, shared_ptr<material> mat1, shared_ptr<material> mat2) {
  return make_shared<cone>(sphere1->get_center(), sphere2->get_center(), sphere1->get_radius(), sphere2->get_radius(), mat1, mat2);
}

#endif