#ifndef INTERACTIONS
#define INTERACTIONS

#include <iostream>
#include "linked_spheres_group.h"
#include "save_load.h"

class interactions {
public:
    interactions(){}

    interactions(shared_ptr<linked_spheres_group> _spheres_group, shared_ptr<hittable_list> _world, camera* _cam) :
            spheres_group(_spheres_group), world(_world), cam(_cam) {}

    void add_sphere_at_pos(int screen_pos_x, int screen_pos_y);

    void segment_cone_at_pos(int screen_pos_x, int screen_pos_y);

    void delete_sphere(int sphere_id) {
        spheres_group->delete_sphere(sphere_id);
    }

    int detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
        return std::get<0>(sphere_at_pos(screen_pos_x, screen_pos_y));
    }

    void change_radius(int sphere_id, double radius){
        spheres_group->change_sphere_radius(sphere_id, radius);
    }

    void increase_radius(int sphere_id, double radius){
        spheres_group->increase_sphere_radius(sphere_id, radius);
    }

    void move_sphere_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y);

    void rotate_camera(double horizontal_angle, double vertical_angle, point3 rot_center) {
        cam->rotate_camera(horizontal_angle, vertical_angle, rot_center);
    }

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

private:
    shared_ptr<linked_spheres_group> spheres_group;
    shared_ptr<hittable_list> world;
    camera* cam;

    tuple<int, hit_record> sphere_at_pos(int screen_pos_x, int screen_pos_y);

    tuple<int, hit_record> cone_at_pos(int screen_pos_x, int screen_pos_y);
};

#endif