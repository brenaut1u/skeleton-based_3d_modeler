#ifndef INTERACTIONS
#define INTERACTIONS

#include <iostream>
#include <span>
#include <future>
#include <memory>
#include "linked_spheres_group.h"
#include "save_load.h"
#include "../camera/phong_camera.h"
#include "../camera/beautiful_camera.h"
#include "../image/screen_segment.h"

using std::unique_ptr;

/*
 * This class is used to interact with the scene. 
 * It contains the linked_spheres_group, the world, the phong_camera and the beautiful_camera.
 * Each function of this class is used to interact with the scene after the user input had been analyzed.
*/

class interactions {
public:
    interactions(){}

    interactions(shared_ptr<linked_spheres_group> _spheres_group, shared_ptr<hittable_list> _world,
                 shared_ptr<phong_camera> _phong_cam, shared_ptr<beautiful_camera> _beautiful_cam) :
            spheres_group(_spheres_group), world(_world), phong_cam(_phong_cam), beautiful_cam(_beautiful_cam) {
        cam_rot_center = point3(0.0, 0.25, -2.0);
        update_skeleton_screen_coordinates();
    }

    static unique_ptr<interactions> get_init_scene(double aspect_ratio, int phong_image_width, int beautiful_image_width);

    void add_sphere_at_pos(int screen_pos_x, int screen_pos_y);

    void segment_cone_at_pos(int screen_pos_x, int screen_pos_y);

    void delete_sphere(const std::span<int>& spheres_id);

    int detect_sphere_at_pos(int screen_pos_x, int screen_pos_y);

    pair<int, int> world_to_screen_pos(point3 p);

    void update_skeleton_screen_coordinates();

    vector<screen_segment> get_skeleton_screen_coordinates() const {
        return skeleton_screen_coordinates;
    }

    void change_radius(int sphere_id, double radius){
        spheres_group->change_sphere_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius){
        spheres_group->increase_sphere_radius(sphere_id, radius);
    }

    void change_color(int sphere_id, color c) {
        spheres_group->set_sphere_color(sphere_id, c);
    }

    vec3 get_translation_vector_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void move_spheres_on_screen(const std::span<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void move_spheres_ik(const std::span<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void rotate_spheres_around_axis(const std::span<int>& spheres_id, vec3 axis, point3 axis_point, double angle);

    void rotate_spheres_around_camera_axis(const std::span<int>& spheres_id, point3 axis_point, double angle);

    void rotate_camera(double horizontal_angle, double vertical_angle);

    void move_camera_sideways(double delta_pos_x, double delta_pos_y);

    void move_camera_forward(double delta_pos);

    void save(string filename) {
        save_in_file(spheres_group.get(), filename);
    }

    static unique_ptr<interactions> load(string filename, shared_ptr<phong_camera> phong_cam, shared_ptr<beautiful_camera> beautiful_cam);

    shared_ptr<linked_spheres_group> get_spheres_group() const {
        return spheres_group;
    }

    const shared_ptr<hittable_list> & get_world() const {
        return world;
    }

    shared_ptr<phong_camera> get_phong_cam() const {
        return phong_cam;
    }

    shared_ptr<beautiful_camera> get_beautiful_cam() const {
        return beautiful_cam;
    }

    void select_sphere(int sphere_id) {
        spheres_group->select_sphere(sphere_id);
    }

    void hovered(int sphere_id) {
        spheres_group->hover_sphere(sphere_id);
    }

    void unselect_sphere(int sphere_id) {
        spheres_group->unselect_sphere(sphere_id);
    }
    
    void add_link(int id1, int id2);

    void start_beautiful_render(span3D beautiful_image);

    bool is_beautiful_render_ready() {
        return beautiful_cam->is_beautiful_render_ready();
    }
    
private:
    shared_ptr<linked_spheres_group> spheres_group;
    shared_ptr<hittable_list> world;
    shared_ptr<phong_camera> phong_cam;
    shared_ptr<beautiful_camera> beautiful_cam;
    point3 cam_rot_center; // the point around which the camera rotates
    vector<screen_segment> skeleton_screen_coordinates; // the screen coordinates of the centers of the spheres
    std::future<void> beautiful_render_task;

    tuple<int, hit_record> cone_at_pos(int screen_pos_x, int screen_pos_y);
};

#endif