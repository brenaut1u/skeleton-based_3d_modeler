#include <string>
#include <pybind11/pybind11.h>
#include <vector>
#include <memory>
#include <pybind11/numpy.h>

using std::unique_ptr;

namespace pyb = pybind11;

#include <memory>
#include "span3D.h"
#include "hittable_list.h"
#include "sphere.h"
#include "hittable.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "cone.h"
#include "interactions.h"

using std::unique_ptr;
using std::shared_ptr;

#include <pybind11/numpy.h>

#include <span>

struct modeler
{
    shared_ptr<hittable_list> world;
    shared_ptr<linked_spheres_group> spheres;
    shared_ptr<phong_camera> phong_cam;
    shared_ptr<beautiful_camera> beautiful_cam;

    double aspect_ratio = 16.0 / 9.0;
    int phong_image_width = 400;
    int beautiful_image_width = 800;

    std::vector<vec3> imageVector;
    std::vector<light> lights;
    unique_ptr<interactions> inter;
    int indexLinkedSphere;

    void initializedWorld()
    {
        inter = interactions::get_init_scene(aspect_ratio, phong_image_width, beautiful_image_width);
        world = inter->get_world();
        spheres = inter->get_spheres_group();
        phong_cam = inter->get_phong_cam();
        beautiful_cam = inter->get_beautiful_cam();

        light white_light = new_light(point3(-1.0, 0.5, -1.0));
        lights = std::vector<light>{white_light};
    }

    void addSphere(int screen_pos_x, int screen_pos_y)
    {
        inter->add_sphere_at_pos(screen_pos_x, screen_pos_y);
    }

    void segmentCone(int screen_pos_x, int screen_pos_y)
    {
        inter->segment_cone_at_pos(screen_pos_x, screen_pos_y);
    }

    void deleteSphere(pyb::array_t<float> sphere_id)
    {
        inter->delete_sphere(numpyViewArray(sphere_id));
    }

    int detectSphere(int screen_pos_x, int screen_pos_y)
    {
        return inter->detect_sphere_at_pos(screen_pos_x, screen_pos_y);
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
        return inter->change_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius)
    {
        return inter->increase_radius(sphere_id, radius);
    }

    void change_color(pyb::array_t<int> sphere_id, int red, int green, int blue) {
        for (int id : numpyViewArray(sphere_id)){
            if (id != -1){
                inter->change_color(id, color((double) red / 255.0, (double) green / 255.0, (double) blue / 255.0));
            }
        }
    }

    void move_spheres_on_screen(pyb::array_t<float> sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        return inter->move_spheres_on_screen(numpyViewArray(sphere_id), screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    void move_spheres_ik(pyb::array_t<float> sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        return inter->move_spheres_ik(numpyViewArray(sphere_id), screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    void rotate_camera(double horizontal_angle, double vertical_angle)
    {
        return inter->rotate_camera(horizontal_angle, vertical_angle);
    }

    void move_camera_sideways(double delta_pos_x, double delta_pos_y) {
        return inter->move_camera_sideways(delta_pos_x, delta_pos_y);
    }

    void move_camera_forward(double delta_pos)
    {
        return inter->move_camera_forward(delta_pos);
    }

    // compute the new image (load changes)

    void computePhongRender(pyb::array_t<float> output, bool draw_skeleton)
    {
        if (draw_skeleton) {
            phong_cam->render(*world, lights, numpyView(output), inter->get_skeleton_screen_coordinates());
        }
        else {
            phong_cam->render(*world, lights, numpyView(output));
        }
    }

    void computeBeautifulRender(pyb::array_t<float> output) {
        return inter->start_beautiful_render(numpyView(output));
    }

    bool isBeautifulRenderReady() {
        return inter->is_beautiful_render_ready();
    }

    //transform a numpy array into a span3D
    span3D numpyView(pyb::array_t<float> array)
    {
        // Accessing the underlying pointer to the data
        float *data = static_cast<float *>(array.request().ptr);

        // Accessing the shape and strides of the array
        auto shape = array.shape();
        auto strides = array.strides();
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
        inter->save(fileName);
    }

    void load(string fileName){
        inter = inter->load(fileName, phong_cam, beautiful_cam);
        spheres = inter->get_spheres_group();
        world = inter->get_world();
        phong_cam = inter->get_phong_cam();
        beautiful_cam = inter->get_beautiful_cam();
    }

    void select(pyb::array_t<float> sphere_id)
    {
        for (int id : numpyViewArray(sphere_id))
        {
            if (id != -1){
                inter->select_sphere(id);
            }
        }
        inter->update_skeleton_screen_coordinates();
    }

    void unselect(pyb::array_t<float> sphere_id)
    {
        for (int id : numpyViewArray(sphere_id))
        {
            if (id != -1){
                inter->unselect_sphere(id);
            }
        }
        inter->update_skeleton_screen_coordinates();
    }

    void hovered(int sphere_id)
    {
        inter->hovered(sphere_id);
    }

    void addLink(int id1, int id2)
    {
        inter->add_link(id1, id2);
    }

    void rotateSphereCamera(pyb::array_t<int> sphere_id, double angle)
    {
        auto ids = numpyViewArray(sphere_id);
        inter->rotate_spheres_around_camera_axis(ids, spheres->get_sphere_at(ids[0])->get_center(), angle);
    }

    void rotateSphereAxis(pyb::array_t<int> sphere_id, double angle)
    {
        auto ids = numpyViewArray(sphere_id);
        auto A = spheres->get_sphere_at(ids[0])->get_center();
        auto B = spheres->get_sphere_at(ids[1])->get_center();
        auto axis = B - A;
        inter->rotate_spheres_around_axis(ids, axis , spheres->get_sphere_at(ids[0])->get_center(), angle);
    }
};

PYBIND11_MODULE(main_modeler, m)
{
    pyb::class_<modeler>(m, "modeler")
        .def(pyb::init<>())
        .def("initializedWorld", &modeler::initializedWorld)
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
        .def("move_spheres", &modeler::move_spheres_on_screen)
        .def("move_spheres_ik", &modeler::move_spheres_ik)
        .def("rotate_camera", &modeler::rotate_camera)
        .def("move_camera_sideways", &modeler::move_camera_sideways)
        .def("move_camera_forward", &modeler::move_camera_forward)
        .def("computePhongRender", &modeler::computePhongRender)
        .def("computeBeautifulRender", &modeler::computeBeautifulRender)
        .def("isBeautifulRenderReady", &modeler::isBeautifulRenderReady)
        .def("save",&modeler::saveInFile)
        .def("segment_cone",&modeler::segmentCone)
        .def("load",&modeler::load)
        .def("select",&modeler::select)
        .def("unselect",&modeler::unselect)
        .def("addLink",&modeler::addLink)
        .def("hovered",&modeler::hovered)
        .def("rotateSphereCamera",&modeler::rotateSphereCamera)
        .def("rotateSphereAxis",&modeler::rotateSphereAxis);
}
