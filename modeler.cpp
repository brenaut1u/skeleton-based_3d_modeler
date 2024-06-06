#include <string>
#include <pybind11/pybind11.h>
/*#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>*/
#include <vector>
#include <pybind11/numpy.h>

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

    void initializedWorld()
    {
        world = make_shared<hittable_list>();
        auto mat = make_shared<metal>(color(0.8, 0.6, 0.2), 0.5);

        // CZ You were initializing temporary variable (hiding the attribut of the structure
        // in the current scope) :
        // linked_spheres_group spheres = ...
        // type + identifier => never refer to an existing variable or attributs
        spheres = make_shared<linked_spheres_group>(world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, mat));
        spheres -> add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, mat), 0);

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
        cam = camera(16.0 / 9.0, 400, 1, 1, 50, 5);
        inter = interactions(spheres, world, &cam);

        // std::cout<<cam.image_width/cam.aspect_ratio<<"/n";
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

    void deleteSphere(pyb::array_t<float> sphere_id)
    {
        inter.delete_sphere(numpyViewArray(sphere_id));
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

    void change_color(int sphere_id, int red, int green, int blue) {
        return inter.change_color(sphere_id, color((double) red / 255.0, (double) green / 255.0, (double) blue / 255.0));
    }

    void move_sphere_on_screen(pyb::array_t<float> sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        return inter.move_spheres_on_screen(numpyViewArray(sphere_id), screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    void rotate_camera(double horizontal_angle, double vertical_angle)
    {
        return inter.rotate_camera(horizontal_angle, vertical_angle);
    }

    void move_camera_sideways(double delta_pos_x, double delta_pos_y) {
        return inter.move_camera_sideways(delta_pos_x, delta_pos_y);
    }

    void move_camera_forward(double delta_pos)
    {
        return inter.move_camera_forward(delta_pos);
    }

    // compute the new image (load changes)

    void computeImageSpan(pyb::array_t<float> output, bool draw_skeleton)
    {
        if (draw_skeleton) {
            cam.computePhong(*world, lights, numpyView(output), inter.get_skeleton_screen_coordinates());
        }
        else {
            cam.computePhong(*world, lights, numpyView(output));
        }
    }

    void computeImageSpanBeautifulRender(pyb::array_t<float> output) {
        return inter.start_beautiful_render(numpyView(output));
    }

    bool isBeautifulRenderReady() {
        return inter.is_beautful_render_ready();
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

    std::span<int> numpyViewArray(pyb::array_t<int> array){
    if (!array.ptr())
        throw std::runtime_error("Invalid numpy array!");

    auto buffer_info = array.request();
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Not a 1D numpy array!");
    std::span<int> span = std::span<int>(static_cast<int*>(buffer_info.ptr), buffer_info.size);
    
    return span;
}

    void saveInFile(string fileName){
        inter.save(fileName);
    }

    void load(string fileName){
        inter = inter.load(fileName,cam);
        spheres = inter.get_spheres_group();
        world = inter.get_world();  
    }

    void select(int sphere_id)
    {
        inter.select_sphere(sphere_id);
    }

    void unselect(int sphere_id)
    {
        inter.unselect_sphere(sphere_id);
    }

    void hovered(int sphere_id)
    {
        inter.hovered(sphere_id);
    }

    void addLink(int id1, int id2)
    {
        inter.add_link(id1, id2);
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
        .def("changeColor", &modeler::change_color)
        .def("delete", &modeler::deleteSphere)
        .def("detect", &modeler::detectSphere)
        .def_readwrite("index", &modeler::indexLinkedSphere)
        .def("move_sphere", &modeler::move_sphere_on_screen)
        .def("rotate_camera", &modeler::rotate_camera)
        .def("move_camera_sideways", &modeler::move_camera_sideways)
        .def("move_camera_forward", &modeler::move_camera_forward)
        .def("computeImageSpan", &modeler::computeImageSpan)
        .def("save",&modeler::saveInFile)
        .def("segment_cone",&modeler::segmentCone)
        .def("load",&modeler::load)
        .def("select",&modeler::select)
        .def("unselect",&modeler::unselect)
        .def("addLink",&modeler::addLink)
        .def("hovered",&modeler::hovered);
}
