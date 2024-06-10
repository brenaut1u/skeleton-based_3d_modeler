#ifndef CONE_H
#define CONE_H

#include "../utilities/vec3.h"
#include "sphere.h"
#include "../utilities/color.h"
#include "../utilities/material.h"

/**
 * This class represents a sphere-cone, that is a cone with rounded extremities made of two spheres.
 * The cone is defined by the two sphere's center position, and their radius.
 * The two spheres can have different materials. The material at each point in-between is a blend of the two materials.
 * The spheres can be show their hover and selection status.
 * If a sphere is hovered, its color becomes the negative of the base color.
 * If a sphere is selected, a thin yellow contour appears around it.
 */

class cone : public hittable {
  public:
    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _material)
      : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat1(_material), mat2(_material) {}

    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _mat1, shared_ptr<material> _mat2)
            : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat1(_mat1), mat2(_mat2) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const override;

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

    void set_selected(int new_selected) {
        selected = new_selected;
    }

    bool is_selected(int id_selected) const {
        return selected == id_selected || selected == 3;
    }

    void set_hovered(int new_hovered) {
        hovered = new_hovered;
    }

    bool is_hovered(int id_hovered) const {
        return hovered == id_hovered;
    }

    void set_radius1(double new_radius) {
        radius1 = max(new_radius,0.0);
    }

    void set_radius2(double new_radius) {
        radius2 = max(new_radius,0.0);
    }

    void increase_radius1(double delta_radius) {
        radius1 = max(radius1 + delta_radius,0.0);
    }

    void increase_radius2(double delta_radius) {
        radius2 = max(radius2 + delta_radius,0.0);
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
    int selected = 0;
    int hovered = 0;
};

static shared_ptr<cone> cone_from_spheres(shared_ptr<sphere> sphere1, shared_ptr<sphere> sphere2, shared_ptr<material> mat1, shared_ptr<material> mat2) {
    // Creates a new cone from two spheres and two materials
    return make_shared<cone>(sphere1->get_center(), sphere2->get_center(), sphere1->get_radius(), sphere2->get_radius(), mat1, mat2);
}

#endif