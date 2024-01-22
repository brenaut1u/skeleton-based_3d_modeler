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
        
        interactions(linked_spheres_group _spheres_group, hittable_list* _world, camera* _cam) :
        spheres_group(_spheres_group), world(_world), cam(_cam) {}

        void add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            ray r = cam->get_ray(screen_pos_x, screen_pos_y);
            tuple<int, hit_record> find_sphere = spheres_group.find_hit_sphere(r, interval(0.001, infinity));
            if (std::get<0>(find_sphere) != -1) {
                shared_ptr<sphere> new_sphere = make_shared<sphere>(std::get<1>(find_sphere).p, 0.3, std::get<1>(find_sphere).mat);
                spheres_group.add_sphere(new_sphere, std::get<0>(find_sphere));
            }
        }

        void delete_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            ray r = cam->get_ray(screen_pos_x, screen_pos_y);
            tuple<int, hit_record> find_sphere = spheres_group.find_hit_sphere(r, interval(0.001, infinity));
            if (std::get<0>(find_sphere) != -1) {
                spheres_group.delete_sphere(std::get<0>(find_sphere));
            }
        }

        int detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            ray r = cam->get_ray(screen_pos_x, screen_pos_y);
            tuple<int, hit_record> find_sphere = spheres_group.find_hit_sphere(r, interval(0.001, infinity));
            return std::get<0>(find_sphere);
        }

        void change_radius(double radius, int id_sphere){
            shared_ptr<sphere> old_sphere = spheres_group.get_sphere_at(id_sphere);
            vec3 center = old_sphere->get_center();
            //double new_radius = 0.9;
            //double new_radius = std::sqrt(std::pow(screen_pos_x - center.x(), 2) + std::pow(screen_pos_y - center.x(), 2))/200;
            shared_ptr<sphere> new_sphere = make_shared<sphere>(center, radius, old_sphere->get_material());  
            spheres_group.change_sphere_at(new_sphere,id_sphere);
        }

        void rotate_camera(double horizontal_angle, double vertical_angle, point3 rot_center) {
            cam->rotate_camera(horizontal_angle, vertical_angle, rot_center);
        }

    private:
        linked_spheres_group spheres_group;
        hittable_list* world;
        camera* cam;
};

#endif