#include <iostream>
#include "ray.h"
#include "camera.h"
#include "hittable_list.h"
#include "linked_spheres_group.h"
#include "sphere.h"
#include "hittable.h"
#include "save_load.h"
#include "interactions.h"

void interactions::add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    auto [sphere_id, rec] = sphere_at_pos(screen_pos_x, screen_pos_y);
    if (sphere_id != -1) {
        shared_ptr<sphere> new_sphere = make_shared<sphere>(rec.p, 0.3, rec.mat);
        spheres_group->add_sphere(new_sphere, sphere_id);
    }
}

void interactions::segment_cone_at_pos(int screen_pos_x, int screen_pos_y) {
    auto [cone_id, rec] = cone_at_pos(screen_pos_x, screen_pos_y);
    if (cone_id != -1) {
        spheres_group->add_sphere_split_cone(cone_id, rec.p, rec.normal, rec.mat);
    }
}

void interactions::delete_sphere(int sphere_id) {
    spheres_group->delete_sphere(sphere_id);
    spheres_group->delete_isolated_spheres();
}

void interactions::move_sphere_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id);
    if (sph) {
        hit_record rec;
        ray r = cam->get_ray(screen_pos_x, screen_pos_y);
        bool hit = sph->hit(r, interval(0.001, infinity), rec);

        if (hit) {
            point3 la = cam->get_center();
            point3 lb = rec.p;
            vec3 lab = lb - la;
            point3 p0 = cam->get_pixel00_loc();
            vec3 p01 = cam->get_viewport_u();
            vec3 p02 = cam->get_viewport_v();

            double t = dot(cross(p01, p02), la - p0) / dot(-lab, cross(p01, p02));
            point3 pos_on_screen = p0 + ((double) new_screen_pos_x / cam->image_width) * p01
                                   +
                                   ((double) new_screen_pos_y /
                                    (static_cast<int>(cam->image_width / cam->aspect_ratio))) *
                                   p02;
            point3 new_pos = (pos_on_screen - la * (1 - t)) / t;

            spheres_group->set_sphere_position(sphere_id, sph->get_center() + new_pos - lb);
            //return new_pos - lb; //TODO: move the calculation of the translation vector to another function
        }
    }
}



interactions interactions::load(string filename,camera& cam) {
    try {
        auto [spheres, world] = load_from_file(filename);
        return interactions(spheres, world, &cam);
    }
    catch (const std::exception e) {
        std::cout<<"Error loading file\n";
        return interactions(make_shared<linked_spheres_group>(), make_shared<hittable_list>(), &cam);
    }
}

tuple<int, hit_record> interactions::sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    ray r = cam->get_ray(screen_pos_x, screen_pos_y);
    tuple<int, hit_record> find_sphere = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
    return find_sphere;
}

tuple<int, hit_record> interactions::cone_at_pos(int screen_pos_x, int screen_pos_y) {
    ray r = cam->get_ray(screen_pos_x, screen_pos_y);
    tuple<int, hit_record> find_cone = spheres_group->find_hit_cone(r, interval(0.001, infinity));
    return find_cone;
}
