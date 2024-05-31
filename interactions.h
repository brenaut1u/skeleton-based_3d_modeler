#ifndef INTERACTIONS
#define INTERACTIONS

#include <iostream>
#include "linked_spheres_group.h"
#include "save_load.h"

class interactions {
public:
    interactions(){}

    interactions(shared_ptr<linked_spheres_group> _spheres_group, shared_ptr<hittable_list> _world, camera* _cam) :
            spheres_group(_spheres_group), world(_world), cam(_cam) {
        cam_rot_center = point3(0.0, 0.25, -2.0);
    }

    void add_sphere_at_pos(int screen_pos_x, int screen_pos_y);

    void segment_cone_at_pos(int screen_pos_x, int screen_pos_y);

    void delete_sphere(int sphere_id);

    int detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
        return std::get<0>(sphere_at_pos(screen_pos_x, screen_pos_y));
    }

    void change_radius(int sphere_id, double radius){
        spheres_group->change_sphere_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius){
        spheres_group->increase_sphere_radius(sphere_id, radius);
    }

    vec3 get_translation_vector_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void move_spheres_on_screen(const vector<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void rotate_spheres_around_axis(const vector<int>& spheres_id, vec3 axis, point3 axis_point, double angle);

    void rotate_spheres_around_camera_axis(const vector<int>& spheres_id, point3 axis_point, double angle);

    void rotate_camera(double horizontal_angle, double vertical_angle) {
        cam->rotate_camera(horizontal_angle, vertical_angle, cam_rot_center);
    }

    void move_camera_sideways(double delta_pos_x, double delta_pos_y);

    void move_camera_forward(double delta_pos) {
        cam->move_camera_forward(delta_pos);
    }

    void save(string filename) {
        save_in_file(spheres_group.get(), filename);
    }

    static interactions load(string filename,camera& cam);

    shared_ptr<linked_spheres_group> get_spheres_group() const {
        return spheres_group;
    }

    const shared_ptr<hittable_list> & get_world() const {
        return world;
    }

    camera * get_cam() const {
        return cam;
    }

    void select_sphere(int sphere_id) {
        spheres_group->sphere_is_selected(sphere_id);
    }

    void unselect_sphere(int sphere_id) {
        spheres_group->sphere_is_unselected(sphere_id);
    }
    
private:
    shared_ptr<linked_spheres_group> spheres_group;
    shared_ptr<hittable_list> world;
    camera* cam;
    point3 cam_rot_center;

    tuple<int, hit_record> sphere_at_pos(int screen_pos_x, int screen_pos_y);

    tuple<int, hit_record> cone_at_pos(int screen_pos_x, int screen_pos_y);
};

#endif