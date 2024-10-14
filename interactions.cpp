#include <iostream>
#include "ray.h"
#include "camera.h"
#include "hittable_list.h"
#include "linked_spheres_group.h"
#include "sphere.h"
#include "hittable.h"
#include "save_load.h"
#include "interactions.h"

inline constexpr double camera_move_factor = 0.5; // The attenuation factor for camera displacement, to make it smaller

unique_ptr<interactions> interactions::get_init_scene(double aspect_ratio, int phong_image_width, int beautiful_image_width) {
    shared_ptr<hittable_list> world = make_shared<hittable_list>();
    auto mat = make_shared<metal>(color(0.8, 0.6, 0.2), 0.5);

    shared_ptr<linked_spheres_group> spheres = make_shared<linked_spheres_group>(world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, mat));
    spheres -> add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, mat), 0);

    shared_ptr<phong_camera> phong_cam = make_shared<phong_camera>(aspect_ratio, phong_image_width, 1, 1);
    shared_ptr<beautiful_camera> beautiful_cam = make_shared<beautiful_camera>(aspect_ratio, beautiful_image_width, 25, 5);

    return make_unique<interactions>(spheres, world, phong_cam, beautiful_cam);
}

void interactions::add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
    auto [sphere_id, rec] = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
    if (sphere_id != -1) {
        shared_ptr<sphere> new_sphere = make_shared<sphere>(rec.p, 0.3, rec.mat);
        spheres_group->add_sphere(new_sphere, sphere_id);
    }
    else {
        segment_cone_at_pos(screen_pos_x, screen_pos_y);
    }
    update_skeleton_screen_coordinates();
}

void interactions::segment_cone_at_pos(int screen_pos_x, int screen_pos_y) {
    auto [cone_id, rec] = cone_at_pos(screen_pos_x, screen_pos_y);
    if (cone_id != -1) {
        spheres_group->add_sphere_split_cone(cone_id, rec.p, rec.normal, rec.mat);
    }
}

void interactions::delete_sphere(const std::span<int>& spheres_id) {
    spheres_group->delete_sphere(spheres_id);
    update_skeleton_screen_coordinates();
}

int interactions::detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
    auto [sphere_id, rec] = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
    if (sphere_id != -1) return sphere_id;

    for (screen_segment seg : skeleton_screen_coordinates) {
        if ((screen_pos_x - seg.first.x) * (screen_pos_x - seg.first.x)
                + (screen_pos_y - seg.first.y) * (screen_pos_y - seg.first.y)
                <= skeleton_circle_radius * skeleton_circle_radius) {
            return seg.first_sphere_id;
        }
        if ((screen_pos_x - seg.second.x) * (screen_pos_x - seg.second.x)
                + (screen_pos_y - seg.second.y) * (screen_pos_y - seg.second.y)
                <= skeleton_circle_radius * skeleton_circle_radius) {
            return seg.second_sphere_id;
        }
    }
    return -1;
}

pair<int, int> interactions::world_to_screen_pos(point3 p) {
    point3 cam_center = phong_cam->get_center();

    double t = line_plane_intersection(cam_center, p - cam_center,
                                       phong_cam->get_pixel00_loc(), phong_cam->get_viewport_u(), phong_cam->get_viewport_v());
    point3 point_on_screen = cam_center + t * (p - cam_center);

    int screen_pos_x =
            phong_cam->image_width * dot(point_on_screen - phong_cam->get_pixel00_loc(), unit_vector(phong_cam->get_viewport_u())) /
            phong_cam->get_viewport_u().length();
    int screen_pos_y = phong_cam->image_width / phong_cam->aspect_ratio *
                       dot(point_on_screen - phong_cam->get_pixel00_loc(), unit_vector(phong_cam->get_viewport_v())) /
                       phong_cam->get_viewport_v().length();

    return {screen_pos_x, screen_pos_y};
}

void interactions::update_skeleton_screen_coordinates() {
    skeleton_screen_coordinates = vector<screen_segment>();
    for (const pair<int, int>& link : spheres_group->get_links()) {
        point3 c1 = spheres_group->get_sphere_at(link.first)->get_center();
        point3 c2 = spheres_group->get_sphere_at(link.second)->get_center();
        if (dot(c1 - phong_cam->get_center(), cross(phong_cam->get_viewport_u(), phong_cam->get_viewport_v())) > 0 &&
            dot(c2 - phong_cam->get_center(), cross(phong_cam->get_viewport_u(), phong_cam->get_viewport_v())) > 0)
        {
            skeleton_screen_coordinates.push_back(screen_segment{screen_point{world_to_screen_pos(c1)},
                                                                 screen_point{world_to_screen_pos(c2)},
                                                                 spheres_group->is_sphere_selected(link.first),
                                                                 spheres_group->is_sphere_selected(link.second),
                                                                 link.first,
                                                                 link.second});
        }
    }
}

vec3 interactions::get_translation_vector_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id);
    if (sph) {
        hit_record rec;
        ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
        bool hit = sph->hit(r, interval(0.001, infinity), rec, false);

        point3 la = phong_cam->get_center();
        point3 lb = hit ? rec.p : sph->get_center();
        point3 p0 = phong_cam->get_pixel00_loc();
        vec3 p01 = phong_cam->get_viewport_u();
        vec3 p02 = phong_cam->get_viewport_v();

        double t = line_plane_intersection(la, lb - la, p0, p01, p02);
        point3 pos_on_screen = p0 + ((double) new_screen_pos_x / phong_cam->image_width) * p01
                               +
                               ((double) new_screen_pos_y /
                                (static_cast<int>(phong_cam->image_width / phong_cam->aspect_ratio))) *
                               p02;
        point3 new_pos = (pos_on_screen - la * (1 - t)) / t;

        return new_pos - lb;
    }
    return {0.0, 0.0, 0.0};
}

void interactions::move_spheres_on_screen(const std::span<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    // The move is related to the last sphere of the list
    if (!spheres_id.empty()) {
        vec3 v = get_translation_vector_on_screen(spheres_id[spheres_id.size()-1], screen_pos_x, screen_pos_y,
                                                  new_screen_pos_x, new_screen_pos_y);
        for (int id: spheres_id) {
            shared_ptr<sphere> sph = spheres_group->get_sphere_at(id);
            spheres_group->set_sphere_position(id, sph->get_center() + v);
        }
    }
    update_skeleton_screen_coordinates();
}

void interactions::move_spheres_ik(const std::span<int>& spheres_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    // The move is related to the first sphere. The last sphere is moved to new_screen_pos and
    // the second sphere's position is calculated by inverse kinematics. The remaining spheres move like the last one,
    // which allow, for example, to move a complete hand with fingers.

    int nb_spheres = spheres_id.size();
    if (nb_spheres >= 3) {
        shared_ptr<sphere> sph1 = spheres_group->get_sphere_at(spheres_id[0]);
        shared_ptr<sphere> sph2 = spheres_group->get_sphere_at(spheres_id[1]);
        shared_ptr<sphere> sph3 = spheres_group->get_sphere_at(spheres_id[nb_spheres - 1]);

        vec3 v12 = sph1->get_center() - sph2->get_center();
        vec3 v23 = sph2->get_center() - sph3->get_center();

        double r12 = v12.length();
        double r23 = v23.length();

        vec3 sph3_move = get_translation_vector_on_screen(spheres_id[nb_spheres - 1],screen_pos_x, screen_pos_y,new_screen_pos_x, new_screen_pos_y);
        point3 sph3_new_pos = sph3->get_center() + sph3_move;

        vec3 new_v13 = sph3_new_pos - sph1->get_center();

        if ((sph3_new_pos - sph1->get_center()).length() <= r12 + r23) {
            // if sph3 is not too far away from sph1 for a solution to exist
            vec3 u = unit_vector(new_v13);
            vec3 v = unit_vector(v12 - u * dot(u, v12));

            if (v == u) {
                // in case the three spheres are aligned
                u = u != vec3{0.0, 1.0, 0.0} ? vec3{0.0, 1.0, 0.0} : vec3{1.0, 0.0, 0.0};
            }

            double h = dot(new_v13, new_v13);
            double w = h + r12 * r12 - r23 * r23;
            double s = max(4.0 * r12 * r12 * h - w * w, 0.0);

            double du = (w * dot(new_v13, u) - dot(new_v13, v) * sqrt(s)) * 0.5 / h;
            double dv = (w * dot(new_v13, v) + dot(new_v13, u) * sqrt(s)) * 0.5 / h;

            // moving second sphere (eg: the elbow)
            spheres_group->set_sphere_position(spheres_id[1], sph1->get_center() + du * u - dv * v);

            // moving last sphere and all the others (eg: the hand and the fingers)
            for (int i = 2; i < spheres_id.size(); i++) {
                shared_ptr<sphere> sph = spheres_group->get_sphere_at(spheres_id[i]);
                spheres_group->set_sphere_position(spheres_id[i], sph->get_center() + sph3_move);
            }

            update_skeleton_screen_coordinates();
        }
    }
}

void interactions::rotate_spheres_around_axis(const std::span<int>& spheres_id, vec3 axis, point3 axis_point, double angle) {
    for (int sphere_id : spheres_id) {
        shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id);
        point3 sph_pos = sph->get_center();
        if (sph_pos != axis_point) {
            sph->set_center(point_rotation(sph_pos, axis_point, axis, angle));
            spheres_group->set_sphere_position(sphere_id, sph->get_center());
        }
    }
    update_skeleton_screen_coordinates();
}

void interactions::rotate_spheres_around_camera_axis(const std::span<int>& spheres_id, point3 axis_point, double angle) {
    vec3 axis = axis_point - phong_cam->get_center();
    rotate_spheres_around_axis(spheres_id, axis, axis_point, angle);
    update_skeleton_screen_coordinates();
}

void interactions::rotate_camera(double horizontal_angle, double vertical_angle) {
    phong_cam->rotate_camera(horizontal_angle, vertical_angle, cam_rot_center);
    beautiful_cam->rotate_camera(horizontal_angle, vertical_angle, cam_rot_center);
    update_skeleton_screen_coordinates();
}

void interactions::move_camera_sideways(double delta_pos_x, double delta_pos_y) {
    // Move the camera laterally or upwards/downwards.
    // The move is adapted to the distance of the camera to the rotation center:
    // the smaller the distance, the smaller the displacement.

    double dist_to_rot_center = (phong_cam->get_center() - cam_rot_center).length() * camera_move_factor;
    phong_cam->move_camera_sideways(delta_pos_x * dist_to_rot_center, delta_pos_y * dist_to_rot_center);
    beautiful_cam->move_camera_sideways(delta_pos_x * dist_to_rot_center, delta_pos_y * dist_to_rot_center);
    cam_rot_center += delta_pos_x * dist_to_rot_center * phong_cam->get_viewport_u()
                    + delta_pos_y * dist_to_rot_center * phong_cam->get_viewport_v();
    update_skeleton_screen_coordinates();
}

void interactions::move_camera_forward(double delta_pos) {
    // Move the camera forward or backward.
    // The move is adapted to the distance of the camera to the rotation center:
    // the smaller the distance, the smaller the displacement.

    double dist_to_rot_center = (phong_cam->get_center() - cam_rot_center).length() * camera_move_factor;
    phong_cam->move_camera_forward(delta_pos * dist_to_rot_center);
    beautiful_cam->move_camera_forward(delta_pos * dist_to_rot_center);
    update_skeleton_screen_coordinates();
}

unique_ptr<interactions> interactions::load(string filename, shared_ptr<phong_camera> phong_cam, shared_ptr<beautiful_camera> beautiful_cam) {
    try {
        auto [spheres, world] = load_from_file(filename);
        return make_unique<interactions>(spheres, world, phong_cam, beautiful_cam);
    }
    catch (const std::exception e) {
        std::cout<<"Error loading file\n";
        return interactions::get_init_scene(phong_cam->aspect_ratio, phong_cam->image_width, beautiful_cam->image_width);
    }
}

tuple<int, hit_record> interactions::cone_at_pos(int screen_pos_x, int screen_pos_y) {
    ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
    tuple<int, hit_record> find_cone = spheres_group->find_hit_cone(r, interval(0.001, infinity));
    return find_cone;
}

void interactions::add_link(int id1, int id2) {
    spheres_group->add_link(id1, id2);
    update_skeleton_screen_coordinates();
}

void interactions::start_beautiful_render(span3D beautiful_image) {
    beautiful_cam->stop_beautiful_render();
    try {
        beautiful_render_task.get();
    }
    catch(const std::exception e) {}
    beautiful_render_task = std::async(&beautiful_camera::render, beautiful_cam, *world, beautiful_image);
}