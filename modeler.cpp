#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

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

class modeler {
    public:
        hittable_list world;
        std::vector<vec3> vectorImage

    void addSphere(int x, int y){

    }
};

int modeler() {
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3( 0.0, 0.25, -2.0),   0.8, material_center));

    world.add(make_shared<cone>(point3(-1.5, 0.25, -2.0), point3(0.75, 0.25, -2.0), 0.2, 0.8, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    

}






NB_MODULE(modelerVrai, m) {
    
    nb::class_<vec3>(m,"vec3")
        .def(nb::init<>())
        .def("x",&vec3::x)
        .def("y",&vec3::y)
        .def("z",&vec3::z)
    ;
    
}










































