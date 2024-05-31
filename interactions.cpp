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

pair<int, int> interactions::world_to_screen_pos(point3 p) {
    point3 la = cam->get_center();
    point3 lb = p;
    vec3 lab = lb - la;
    point3 p0 = cam->get_pixel00_loc();
    vec3 p01 = cam->get_viewport_u();
    vec3 p02 = cam->get_viewport_v();

    double t = dot(cross(p01, p02), la - p0) / dot(-lab, cross(p01, p02));
    point3 point_on_screen = la + t * lab;

    int screen_pos_x = cam->image_width * dot(point_on_screen - cam->get_pixel00_loc(), unit_vector(cam->get_viewport_u())) / cam->get_viewport_u().length();
    int screen_pos_y = cam->image_width / cam->aspect_ratio * dot(point_on_screen - cam->get_pixel00_loc(), unit_vector(cam->get_viewport_v())) / cam->get_viewport_v().length();

    return {screen_pos_x, screen_pos_y};
}

segment_list interactions::get_skeleton_screen_coordinates() {
    segment_list skeleton_screen_coordinates;
    for (const pair<int, int>& link : spheres_group->get_links()) {
        skeleton_screen_coordinates.push_back({ world_to_screen_pos(spheres_group->get_sphere_at(link.first)->get_center()),
                                                world_to_screen_pos(spheres_group->get_sphere_at(link.second)->get_center())});
    }
    return skeleton_screen_coordinates;
}

vec3 interactions::get_translation_vector_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
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

            return new_pos - lb;
        }
    }
    return {0.0, 0.0, 0.0};
}

void interactions::move_spheres_on_screen(const vector<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    //the move is related to the last sphere of the list
    if (!spheres_id.empty()) {
        vec3 v = get_translation_vector_on_screen(spheres_id[0], screen_pos_x, screen_pos_y, new_screen_pos_x,
                                                  new_screen_pos_y);
        for (int id: spheres_id) {
            shared_ptr<sphere> sph = spheres_group->get_sphere_at(id);
            spheres_group->set_sphere_position(id, sph->get_center() + v);
        }
    }
}

void interactions::rotate_spheres_around_axis(const vector<int>& spheres_id, vec3 axis, point3 axis_point, double angle) {
    for (int sphere_id : spheres_id) {
        shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id);
        point3 sph_pos = sph->get_center();
        sph->set_center(point_rotation(sph_pos, axis_point, axis, angle));
    }
}

void interactions::rotate_spheres_around_camera_axis(const vector<int>& spheres_id, point3 axis_point, double angle) {
    vec3 axis = axis_point - cam->get_center();
    rotate_spheres_around_axis(spheres_id, axis, axis_point, angle);
}

void interactions::move_camera_sideways(double delta_pos_x, double delta_pos_y) {
    cam->move_camera_sideways(delta_pos_x, delta_pos_y);
    cam_rot_center += delta_pos_x * cam->get_viewport_u() + delta_pos_y * cam->get_viewport_v();
}

interactions interactions::load(string filename,camera& cam) {
    try {
        auto [spheres, world] = load_from_file(filename);
        return interactions(spheres, world, &cam);
    }
    catch (const std::exception e) {
        std::cout<<"Error loading file\n";
        return *this;
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
