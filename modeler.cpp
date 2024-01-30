#include <string>
#include <pybind11/pybind11.h>
/*#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>*/
#include <vector>

namespace pyb = pybind11;

#include "hittable_list.h"
#include "sphere.h"
#include "hittable.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "cone.h"
#include "interactions.h"


struct modeler {
    hittable_list world;
    linked_spheres_group spheres;
    camera cam;
    std::vector<vec3> imageVector;
    std::vector<light> lights;
    interactions inter;
    int indexLinkedSphere;
    point3 gravity_center;

    void initializedWorld(){
      auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
      auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
      auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
      auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

      // CZ You were initializing temporary variable (hiding the attribut of the structure
      // in the current scope) :
      // linked_spheres_group spheres = ...
      // type + identifier => never refer to an existing variable or attributs
      spheres = linked_spheres_group(&world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, material_right));
      spheres.add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, material_right), 0);

      // CZ : no real reason to change the lights during updates,
      // however, it could be in a dedicated function
      light white_light = new_light(point3(-1.0, 0.5, -1.0));
      light red_light = new_colored_light(point3(0.0, 0.5, -1), point3(0.7, 0.2, 0.2));
      light blue_light = new_colored_light(point3(1.0, -0.25, -1), point3(0.0, 0.0, 0.8));
      lights = std::vector<light>{white_light};
      //std::vector<light> lights {white_light, red_light, blue_light};

      // CZ : warning, requires "interactions" copies the camera,
      // it should be initialized before initializing interactions
      // a set camera in interactions will be useful later (move viewpoint
      // in the scene : translate and rotate)
      cam = camera(16.0 / 9.0, 400, 1, 1);
      inter = interactions(&spheres, &world, &cam);
      gravity_center = point3(0.5, 0.25, -1);
    }

    void initializedCam()
    {
      //CZ : set resolution of camera,
      // should be taken as parameter to be coherent with python
      // also when the camera is updated, the new camera should be provided
      // to 'inter'
    }

    void computeImage()
    {
        imageVector = cam.render_phong(world, lights);
    }

    void addSphere(int screen_pos_x, int screen_pos_y){
        inter.add_sphere_at_pos(screen_pos_x,screen_pos_y);
    }

    void deleteSphere(int sphere_id){
        inter.delete_sphere(sphere_id);
    }

    int detectSphere(int screen_pos_x, int screen_pos_y){
        return inter.detect_sphere_at_pos(screen_pos_x,screen_pos_y);
    }

    double getRed(int i){
        return imageVector[i].x();
    }

    double getGreen(int i){
        return imageVector[i].y();
    }

    double getBlue(int i){
        return imageVector[i].z();
    }

    void change_radius(int sphere_id, double radius) {
        return inter.change_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius) {
        return inter.increase_radius(sphere_id, radius);
    }

    void set_sphere_position_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y) {
        return inter.set_sphere_position_on_screen(sphere_id, screen_pos_x, screen_pos_y);
    }

    void rotate_camera(double horizontal_angle,double vertical_angle){
        return inter.rotate_camera(horizontal_angle,vertical_angle,gravity_center);
    }

    void move_camera_forward(double delta_pos) {
        return inter.move_camera_forward(delta_pos);
    }
};


PYBIND11_MODULE(modelerVrai, m) {
    pyb::class_<modeler>(m,"modeler")
        .def(pyb::init<>())
        .def("initializedWorld", &modeler::initializedWorld)
        .def("initializedCam", &modeler::initializedCam)
        .def("computeImage",&modeler::computeImage)
        .def("getRed",&modeler::getRed)
        .def("getGreen",&modeler::getGreen)
        .def("getBlue",&modeler::getBlue)
        .def("add",&modeler::addSphere)
        .def("changeRadius",&modeler::change_radius)
        .def("increaseRadius", &modeler::increase_radius)
        .def("delete",&modeler::deleteSphere)
        .def("detect",&modeler::detectSphere)
        .def_readwrite("index",&modeler::indexLinkedSphere)
        .def("set_sphere_position", &modeler::set_sphere_position_on_screen)
        .def("rotate_camera",&modeler::rotate_camera)
        .def("move_camera_forward", &modeler::move_camera_forward)
    ;
}










































