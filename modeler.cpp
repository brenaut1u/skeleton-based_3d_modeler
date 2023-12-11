#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <vector>

namespace nb = nanobind;

#include "hittable_list.h"
#include "sphere.h"
#include "hittable.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "cone.h"


struct modeler {
    hittable_list world;
    camera cam;
    std::vector<vec3> imageVector;
    light white_light = new_light(point3(-1.0, 0.5, -1.0));
    light red_light = new_colored_light(point3(0.0, 0.5, -1), point3(0.7, 0.2, 0.2));
    light blue_light = new_colored_light(point3(1.0, -0.25, -1), point3(0.0, 0.0, 0.8));
    std::vector<light> lights {white_light};

    void initializedWorld(hittable_list world){
        auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
        auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
        auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

        world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
        //world.add(make_shared<sphere>(point3( 0.0, 0.25, -2.0),   0.8, material_center));

        world.add(make_shared<cone>(point3(-1.5, 0.25, -2.0), point3(0.75, 0.25, -2.0), 0.2, 0.8, material_right));

    }
    

    void intializedCam(camera cam){
        light white_light = new_light(point3(-1.0, 0.5, -1.0));
        light red_light = new_colored_light(point3(0.0, 0.5, -1), point3(0.7, 0.2, 0.2));
        light blue_light = new_colored_light(point3(1.0, -0.25, -1), point3(0.0, 0.0, 0.8));
        std::vector<light> lights {white_light};
        cam.aspect_ratio = 16.0 / 9.0;
        cam.image_width  = 400;
        cam.samples_per_pixel = 100;
        cam.max_depth = 50;
    }

    std::vector<vec3> getVector(hittable_list wolrd, camera cam){
        return cam.render_phong(world, lights);
    }



};



NB_MODULE(modelerVrai, m) {
    nb::class_<modeler>(m,"modeler")
        .def(nb::init<>())
        .def("initializedWorld", &modeler::initializedWorld)
        .def("initializedCam",&modeler::intializedCam)
        .def("getVector",&modeler::getVector)
    ;
}










































