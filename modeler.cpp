#include <string>
#include <pybind11/pybind11.h>
/*#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>*/
#include <vector>

namespace pyb = pybind11;

#include "span3D.h"
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


#include <pybind11/numpy.h>

#include <span>

struct modeler
{
    shared_ptr<hittable_list> world;
    shared_ptr<linked_spheres_group> spheres;
    camera cam;
    std::vector<vec3> imageVector;
    std::vector<light> lights;
    interactions inter;
    int indexLinkedSphere;
    point3 gravity_center;

    void initializedWorld()
    {
        world = make_shared<hittable_list>();
        auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
        auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
        auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

        // CZ You were initializing temporary variable (hiding the attribut of the structure
        // in the current scope) :
        // linked_spheres_group spheres = ...
        // type + identifier => never refer to an existing variable or attributs
        spheres = make_shared<linked_spheres_group>(world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, material_right));
        spheres -> add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, material_right), 0);

        // CZ : no real reason to change the lights during updates,
        // however, it could be in a dedicated function
        light white_light = new_light(point3(-1.0, 0.5, -1.0));
        light red_light = new_colored_light(point3(0.0, 0.5, -1), point3(0.7, 0.2, 0.2));
        light blue_light = new_colored_light(point3(1.0, -0.25, -1), point3(0.0, 0.0, 0.8));
        lights = std::vector<light>{white_light};
        // std::vector<light> lights {white_light, red_light, blue_light};

        // CZ : warning, requires "interactions" copies the camera,
        // it should be initialized before initializing interactions
        // a set camera in interactions will be useful later (move viewpoint
        // in the scene : translate and rotate)
        cam = camera(16.0 / 9.0, 800, 1, 1);
        inter = interactions(spheres, world, &cam);
        gravity_center = point3(0.0, 0.25, -2.0);

        std::cout<<cam.image_width/cam.aspect_ratio<<"/n";
        //computeImageSpan(output);
    }

    void computeImage()
    {
        imageVector = cam.render_phong(*world, lights);
    }

    void addSphere(int screen_pos_x, int screen_pos_y)
    {
        inter.add_sphere_at_pos(screen_pos_x, screen_pos_y);
    }

    void segmentCone(int screen_pos_x, int screen_pos_y)
    {
        inter.segment_cone_at_pos(screen_pos_x, screen_pos_y);
    }

    void deleteSphere(int sphere_id)
    {
        inter.delete_sphere(sphere_id);
    }

    int detectSphere(int screen_pos_x, int screen_pos_y)
    {
        return inter.detect_sphere_at_pos(screen_pos_x, screen_pos_y);
    }

    double getRed(int i)
    {
        return imageVector[i].x();
    }

    double getGreen(int i)
    {
        return imageVector[i].y();
    }

    double getBlue(int i)
    {
        return imageVector[i].z();
    }

    void change_radius(int sphere_id, double radius)
    {
        return inter.change_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius)
    {
        return inter.increase_radius(sphere_id, radius);
    }

    void move_sphere_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        return inter.move_sphere_on_screen(sphere_id, screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    void rotate_camera(double horizontal_angle, double vertical_angle)
    {
        return inter.rotate_camera(horizontal_angle, vertical_angle, gravity_center);
    }

    void move_camera_forward(double delta_pos)
    {
        return inter.move_camera_forward(delta_pos);
    }

    // compute the new image (load changes)
    void computeImageSpan(pyb::array_t<float> output)
    {
        cam.computePhong(*world, lights, numpyView(output));
    }

    //transform a numpy array into a span3D
    span3D numpyView(pyb::array_t<float> array)
    {
        // Accessing the underlying pointer to the data
        float *data = static_cast<float *>(array.request().ptr);

        // Accessing the shape and strides of the array
        auto shape = array.shape();
        auto strides = array.strides();

        // std::cout<<shape[0]<<"   "<<shape[1]<<"   "<<shape[2]<<"/n";
        // std::cout<<strides[0]<<"   "<<strides[1]<<"   "<<strides[2]<<"/n";

        return span3D(data, shape[0], shape[1], shape[2]);
    }

    void saveInFile(string fileName){
        inter.save(fileName);
    }

    void load(string fileName){
        inter = inter.load(fileName,cam);
        spheres = inter.get_spheres_group();
        world = inter.get_world();  
    }

};

void compute(float *res, int n_x, int n_y)
{
    // write first pixel
    res[0 + n_y * (n_x - 1) * 3] = 1.0; // n*m*c + n*y +x
    res[1 + n_y * (n_x - 1) * 3] = 2.0; // n*m*c + n*y +x
    res[2 + n_y * (n_x - 1) * 3] = 3.0; // n*m*c + n*y +x

    //    for (size_t idx = 0; idx < buf1.shape[0]; idx++)
    //        ptr3[idx] = ptr1[idx] + ptr2[idx];
    // ecrire une classe qui fait l'équivalent de ndspan
    // prend le py array et
    // class multi dimesntionnal view
    //     take pointer to data and dimensions
    //     operator()(int x, int y, int z) -> double&

    // span3d numpyView(pyb::array_t<float> output)
}


void add_arrays(pyb::array_t<float> output)
{
    pyb::buffer_info buf1 = output.request();
    std::cout << buf1.ndim << std::endl;
    std::cout << buf1.size << std::endl;
    std::cout << buf1.shape[0] << std::endl;
    std::cout << buf1.shape[1] << std::endl;
    std::cout << buf1.shape[2] << std::endl;
    std::flush(std::cout);
    float *ptr1 = static_cast<float *>(buf1.ptr);

    if (buf1.ndim != 3)
        throw std::runtime_error("Number of color must be three");

    compute(ptr1, buf1.shape[0], buf1.shape[1]);
};

PYBIND11_MODULE(main_modeler, m)
{
    pyb::class_<modeler>(m, "modeler")
        .def(pyb::init<>())
        .def("initializedWorld", &modeler::initializedWorld)
        .def("computeImage", &modeler::computeImage)
        .def("getRed", &modeler::getRed)
        .def("getGreen", &modeler::getGreen)
        .def("getBlue", &modeler::getBlue)
        .def("add", &modeler::addSphere)
        .def("changeRadius", &modeler::change_radius)
        .def("increaseRadius", &modeler::increase_radius)
        .def("delete", &modeler::deleteSphere)
        .def("detect", &modeler::detectSphere)
        .def_readwrite("index", &modeler::indexLinkedSphere)
        .def("move_sphere", &modeler::move_sphere_on_screen)
        .def("rotate_camera", &modeler::rotate_camera)
        .def("move_camera_forward", &modeler::move_camera_forward)
        .def("computeImageSpan", &modeler::computeImageSpan)
        .def("save",&modeler::saveInFile)
        .def("segment_cone",&modeler::segmentCone)
        .def("load",&modeler::load);
    m.def("add_arrays", &add_arrays, "Add two NumPy arrays");
}
