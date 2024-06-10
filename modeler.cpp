// We include every file and class that we need to use in the modeler class.
#include <string>
#include <vector>
#include <memory>
#include <span>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "image/span3D.h"
#include "scene_objects/hittable.h"
#include "scene_objects/hittable_list.h"
#include "scene_objects/sphere.h"
#include "camera/camera.h"
#include "utilities/color.h"
#include "utilities/material.h"
#include "scene_objects/cone.h"
#include "scene_objects/light.h"
#include "structures/interactions.h"

/**
 * This file define le main_modeler that we use in the python script
 * We use the class modeler as an interface between the class interactions and the python script
 **/

namespace pyb = pybind11;

using std::unique_ptr;
using std::shared_ptr;

// We define the modeler class that will be used in the python script.
struct modeler
{
    //  We add the attributes that we will use in the modeler class.
    // Everything related to the world
    shared_ptr<hittable_list> world;
    shared_ptr<linked_spheres_group> spheres;
    unique_ptr<interactions> inter;
    // Everything related to the camera
    std::vector<shared_ptr<light>> lights;
    shared_ptr<phong_camera> phong_cam;
    shared_ptr<beautiful_camera> beautiful_cam;
    // Everything related to the image that we will display
    std::vector<vec3> imageVector;
    
    // We define the default values for the aspect ratio and the image width
    double aspect_ratio = 16.0 / 9.0;
    int phong_image_width = 400;
    int beautiful_image_width = 800;

    int indexLinkedSphere;

    // We initialize the world
    void initializedWorld()
    {
        // We create the initial scene, calling the get_init_scene function from the interactions class
        inter = interactions::get_init_scene(aspect_ratio, phong_image_width, beautiful_image_width);
        world = inter->get_world();
        spheres = inter->get_spheres_group();
        phong_cam = inter->get_phong_cam();
        beautiful_cam = inter->get_beautiful_cam();

        shared_ptr<light> white_light = make_shared<vector_light>(vec3{0.0, 1.0, 0.0});
        lights = std::vector<shared_ptr<light>>{white_light};
    }

    // We add a sphere to the world, calling the interaction function
    void addSphere(int screen_pos_x, int screen_pos_y)
    {
        inter->add_sphere_at_pos(screen_pos_x, screen_pos_y);
    }

    // We segment a cone, calling the interaction function
    void segmentCone(int screen_pos_x, int screen_pos_y)
    {
        inter->segment_cone_at_pos(screen_pos_x, screen_pos_y);
    }

    // We delete a sphere, calling the interaction function
    void deleteSphere(pyb::array_t<float> sphere_id)
    {
        inter->delete_sphere(numpyViewArray(sphere_id));
    }

    // We detect a sphere, calling the interaction function, we return the id of the sphere
    int detectSphere(int screen_pos_x, int screen_pos_y)
    {
        return inter->detect_sphere_at_pos(screen_pos_x, screen_pos_y);
    }

    // The 3 follwing function allow us to get the red, green and blue values of the image
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

    // We set the radius of a sphere, calling the interaction function
    void change_radius(int sphere_id, double radius)
    {
        inter->change_radius(sphere_id, radius);
    }

    // We increase the radius of a sphere, calling the interaction function
    void increase_radius(int sphere_id, double radius)
    {
        inter->increase_radius(sphere_id, radius);
    }

    // We change the color of every sphere selected
    // We need to convert the color values from 0-255 to 0-1 
    // and the numpy array into a span
    void change_color(pyb::array_t<int> sphere_id, int red, int green, int blue) {
        for (int id : numpyViewArray(sphere_id)){
            if (id != -1){
                inter->change_color(id, color((double) red / 255.0, (double) green / 255.0, (double) blue / 255.0));
            }
        }
    }

    // We move the selected spheres on the screen
    // We need to convert the numpy array into a span
    void move_spheres_on_screen(pyb::array_t<float> sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        inter->move_spheres_on_screen(numpyViewArray(sphere_id), screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    // We move the selected spheres using the inverse kinematics
    void move_spheres_ik(pyb::array_t<float> sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y)
    {
        inter->move_spheres_ik(numpyViewArray(sphere_id), screen_pos_x, screen_pos_y, new_screen_pos_x, new_screen_pos_y);
    }

    // We rotate the camera
    void rotate_camera(double horizontal_angle, double vertical_angle)
    {
        inter->rotate_camera(horizontal_angle, vertical_angle);
    }

    // We move the camera sideways
    void move_camera_sideways(double delta_pos_x, double delta_pos_y) {
        inter->move_camera_sideways(delta_pos_x, delta_pos_y);
    }

    // We move the camera forward
    void move_camera_forward(double delta_pos)
    {
        inter->move_camera_forward(delta_pos);
    }

    // Compute the new image
    void computePhongRender(pyb::array_t<float> output, bool draw_skeleton)
    {
        // We check if we need to display the skeleton
        if (draw_skeleton) {
            phong_cam->render(*world, lights, numpyView(output), beautiful_cam->get_render_status(), inter->get_skeleton_screen_coordinates());
        }
        else {
            phong_cam->render(*world, lights, numpyView(output), beautiful_cam->get_render_status());
        }
    }

    // Compute the new image with the beautiful method
    void computeBeautifulRender(pyb::array_t<float> output) {
        inter->start_beautiful_render(numpyView(output));
    }

    // Check if the beautiful render is ready, allows the python code to display it if ready
    bool isBeautifulRenderReady() {
        return inter->is_beautiful_render_ready();
    }

    // Transform a numpy array into a span3D
    span3D numpyView(pyb::array_t<float> array)
    {
        // Accessing the underlying pointer to the data
        float *data = static_cast<float *>(array.request().ptr);

        // Accessing the shape and strides of the array
        auto shape = array.shape();
        auto strides = array.strides();
        return span3D(data, shape[0], shape[1], shape[2]);
    }

    // Transform a numpy array into a span
    std::span<int> numpyViewArray(pyb::array_t<int> array){
        // Accessing the underlying pointer to the data
        if (!array.ptr())
            throw std::runtime_error("Invalid numpy array!");

        auto buffer_info = array.request();
        if (buffer_info.ndim != 1)
            throw std::runtime_error("Not a 1D numpy array!");
        std::span<int> span = std::span<int>(static_cast<int*>(buffer_info.ptr), buffer_info.size);      
        return span;    
    }

    // Save the world in a file
    void saveInFile(string fileName){
        inter->save(fileName);
    }

    // Load the world from a file
    void load(string fileName){
        inter = inter->load(fileName, phong_cam, beautiful_cam);
        spheres = inter->get_spheres_group();
        world = inter->get_world();
        phong_cam = inter->get_phong_cam();
        beautiful_cam = inter->get_beautiful_cam();
    }

    // Select a sphere
    void select(pyb::array_t<float> sphere_id)
    {
        for (int id : numpyViewArray(sphere_id))
        {
            if (id != -1){
                inter->select_sphere(id);
            }
        }
        // We update the skeleton screen coordinates
        inter->update_skeleton_screen_coordinates();
    }

    // Unselect a sphere
    void unselect(pyb::array_t<float> sphere_id)
    {
        for (int id : numpyViewArray(sphere_id))
        {
            if (id != -1){
                inter->unselect_sphere(id);
            }
        }
        // We update the skeleton screen coordinates
        inter->update_skeleton_screen_coordinates();
    }

    // Change the attribute of the sphere in the cone that is hovered
    void hovered(int sphere_id)
    {
        inter->hovered(sphere_id);
    }

    // Add a link between two spheres
    void addLink(int id1, int id2)
    {
        inter->add_link(id1, id2);
    }

    // Rotate the sphere around the camera axis
    void rotateSphereCamera(pyb::array_t<int> sphere_id, double angle)
    {
        auto ids = numpyViewArray(sphere_id);
        inter->rotate_spheres_around_camera_axis(ids, spheres->get_sphere_at(ids[0])->get_center(), angle);
    }

    // Rotate the sphere around an axis
    void rotateSphereAxis(pyb::array_t<int> sphere_id, double angle)
    {
        auto ids = numpyViewArray(sphere_id);
        auto A = spheres->get_sphere_at(ids[0])->get_center(); // the center of the first sphere
        auto B = spheres->get_sphere_at(ids[1])->get_center(); // the center of the second sphere
        auto axis = B - A; // the axis of rotation
        inter->rotate_spheres_around_axis(ids, axis , spheres->get_sphere_at(ids[0])->get_center(), angle);
    }
};


// We create the module that will be used in the python script, we call it main_modeler
// We add the functions that we will use in the python script
// In orange, it is the name we will use in the python script
// In blue and yellow, it is the function in the modeler class
PYBIND11_MODULE(main_modeler, m)
{
    pyb::class_<modeler>(m, "modeler")
        .def(pyb::init<>()) // We initialize the modeler class
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
