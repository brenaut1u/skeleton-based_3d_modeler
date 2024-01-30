#ifndef INTERACTIONS
#define INTERACTIONS

#include <iostream>
#include "ray.h"
#include "camera.h"
#include "hittable_list.h"
#include "linked_spheres_group.h"
#include "sphere.h"
#include "hittable.h"

class interactions {
    public:
        interactions(){}
        
        interactions(linked_spheres_group* _spheres_group, hittable_list* _world, camera* _cam) :
        spheres_group(_spheres_group), world(_world), cam(_cam) {}

        void add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            tuple<int, hit_record> find_sphere = sphere_at_pos(screen_pos_x, screen_pos_y);
            int sphere_id = std::get<0>(find_sphere);
            hit_record rec = std::get<1>(find_sphere);
            if (sphere_id != -1) {
                shared_ptr<sphere> new_sphere = make_shared<sphere>(rec.p, 0.3, rec.mat);
                spheres_group->add_sphere(new_sphere, std::get<0>(find_sphere));
            }
        }

        void segment_cone_at_pos(int screen_pos_x, int screen_pos_y) {
            tuple<int, hit_record> find_cone = cone_at_pos(screen_pos_x, screen_pos_y);
            int cone_id = std::get<0>(find_cone);
            hit_record rec = std::get<1>(find_cone);
            if (cone_id != -1) {
                spheres_group->add_sphere_split_cone(cone_id, rec.p, rec.normal, rec.mat);
            }
        }

        void delete_sphere(int sphere_id) {
            spheres_group->delete_sphere(sphere_id);
        }

        int detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            ray r = cam->get_ray(screen_pos_x, screen_pos_y);
            tuple<int, hit_record> find_sphere = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
            return std::get<0>(find_sphere);
        }

        void change_radius(int sphere_id, double radius){
            spheres_group->change_sphere_radius(sphere_id, radius);
        }

        void increase_radius(int sphere_id, double radius){
            spheres_group->increase_sphere_radius(sphere_id, radius);
        }

//        void change_radius(double radius, int id_sphere){
//            shared_ptr<sphere> old_sphere = spheres_group->get_sphere_at(id_sphere);
//            vec3 center = old_sphere->get_center();
//            //double new_radius = 0.9;
//            //double new_radius = std::sqrt(std::pow(screen_pos_x - center.x(), 2) + std::pow(screen_pos_y - center.x(), 2))/200;
//            shared_ptr<sphere> new_sphere = make_shared<sphere>(center, radius, old_sphere->get_material());
//            spheres_group->change_sphere_at(new_sphere,id_sphere);
//        }

        void set_sphere_position_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y) {
            shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id);
            point3 la = cam->get_center();
            point3 lb = sph->get_center();
            vec3 lab = lb - la;
            point3 p0 = cam->get_pixel00_loc();
            vec3 p1 = cam->get_viewport_u();
            vec3 p2 = cam->get_viewport_v();

            vec3 p01 = p1 - p0;
            vec3 p02 = p2 - p0;

            double t = dot(cross(p01, p02), la - p0) / dot(-lab, cross(p01, p02));
            point3 pos_on_screen = p0 + ((double) screen_pos_x / cam->image_width) * p1
                                      + ((double) screen_pos_y / (static_cast<int>(cam->image_width / cam->aspect_ratio))) * p2;
            point3 new_pos = (pos_on_screen - la * (1 - t)) / t;
            spheres_group->set_sphere_position(sphere_id, new_pos);
        }

        void rotate_camera(double horizontal_angle, double vertical_angle, point3 rot_center) {
            cam->rotate_camera(horizontal_angle, vertical_angle, rot_center);
        }

        void move_camera_forward(double delta_pos) {
            cam->move_camera_forward(delta_pos);
        }

    private:
        linked_spheres_group* spheres_group;
        hittable_list* world;
        camera* cam;

    tuple<int, hit_record> sphere_at_pos(int screen_pos_x, int screen_pos_y) {
        ray r = cam->get_ray(screen_pos_x, screen_pos_y);
        tuple<int, hit_record> find_sphere = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
        return find_sphere;
    }

    tuple<int, hit_record> cone_at_pos(int screen_pos_x, int screen_pos_y) {
        ray r = cam->get_ray(screen_pos_x, screen_pos_y);
        tuple<int, hit_record> find_cone = spheres_group->find_hit_cone(r, interval(0.001, infinity));
        return find_cone;
    }
};

#endif