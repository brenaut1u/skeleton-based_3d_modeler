#include <iostream>
#include "../utilities/ray.h"
#include "../camera/camera.h"
#include "../scene_objects/hittable_list.h"
#include "linked_spheres_group.h"
#include "../scene_objects/sphere.h"
#include "../scene_objects/hittable.h"
#include "save_load.h"
#include "interactions.h"

/**
 * This class is used to interact with the scene. 
 * It contains the linked_spheres_group, the world, the phong_camera and the beautiful_camera.
 * Each function of this class is used to interact with the scene after the user input had been analyzed.
*/

inline constexpr double camera_move_factor = 0.5; // The attenuation factor for camera displacement, to make it smaller

unique_ptr<interactions> interactions::get_init_scene(double aspect_ratio, int phong_image_width, int beautiful_image_width) {
    // Creates the initial scene, with only one cone
    shared_ptr<hittable_list> world = make_shared<hittable_list>();
    auto mat = make_shared<metal>(color(0.8, 0.6, 0.2), 0.5);

    shared_ptr<linked_spheres_group> spheres = make_shared<linked_spheres_group>(world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, mat));
    spheres -> add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, mat), 0);

    shared_ptr<phong_camera> phong_cam = make_shared<phong_camera>(aspect_ratio, phong_image_width, 1, 1);
    shared_ptr<beautiful_camera> beautiful_cam = make_shared<beautiful_camera>(aspect_ratio, beautiful_image_width, 50, 5);

    return make_unique<interactions>(spheres, world, phong_cam, beautiful_cam);
}

void interactions::add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    // Adds a sphere at screen pos.
    // If the position is on a sphere, the new sphere will be created on its surface.
    // If the position is on a cone, the new sphere will be created in such a way that the cone will be segmented
    // without changing its shape.
    // If there is nothing, nothing is done.

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
    // Adds a sphere that segments the cone at screen pos, if it exists. The sphere is added in such a way that
    // the cone's shape isn't altered.

    auto [cone_id, rec] = cone_at_pos(screen_pos_x, screen_pos_y);
    if (cone_id != -1) {
        spheres_group->add_sphere_split_cone(cone_id, rec.p, rec.normal, rec.mat);
    }
}

void interactions::delete_sphere(const std::span<int>& spheres_id) {
    // Delete the selected spheres and the links between them in the linked_spheres_group
    spheres_group->delete_sphere(spheres_id);
    update_skeleton_screen_coordinates();
}

int interactions::detect_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
    // Returns the id in spheres_group of the nearest sphere at the given screen position.

    // If a sphere is found directly
    ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
    auto [sphere_id, rec] = spheres_group->find_hit_sphere(r, interval(0.001, infinity));
    if (sphere_id != -1) return sphere_id;

    // If no sphere is found, we iterate over the skeleton to find a sphere which distance on screen of its center
    // position to the clicked pixel is smaller than the skeleton's circles radius.
    // This allows to select very small spheres thanks to their skeleton.
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

screen_point interactions::world_to_screen_pos(point3 p) {
    // Returns the screen coordinates of point p
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

    return {{screen_pos_x, screen_pos_y}};
}

void interactions::update_skeleton_screen_coordinates() {
    // Calculates the screen coordinates of all visible spheres in order to update skeleton_screen_coordinates
    skeleton_screen_coordinates = vector<screen_segment>();
    for (const pair<int, int>& link : spheres_group->get_links()) {
        point3 c1 = spheres_group->get_sphere_at(link.first)->get_center();
        point3 c2 = spheres_group->get_sphere_at(link.second)->get_center();

        // If the two spheres are not behind the camera:
        if (dot(c1 - phong_cam->get_center(), cross(phong_cam->get_viewport_u(), phong_cam->get_viewport_v())) > 0 &&
            dot(c2 - phong_cam->get_center(), cross(phong_cam->get_viewport_u(), phong_cam->get_viewport_v())) > 0)
        {
            skeleton_screen_coordinates.push_back(screen_segment{world_to_screen_pos(c1),
                                                                 world_to_screen_pos(c2),
                                                                 spheres_group->is_sphere_selected(link.first),
                                                                 spheres_group->is_sphere_selected(link.second),
                                                                 link.first,
                                                                 link.second});
        }
    }
}

vec3 interactions::get_translation_vector_on_screen(int sphere_id, int screen_pos_x, int screen_pos_y, int new_screen_pos_x, int new_screen_pos_y) {
    // Finds the world translation vector that would bring the sphere from given start screen position to given
    // new screen position, while maintaining its distance to screen plane constant.

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
    // Moves a group of spheres. The translation vector is calculated in relation to the last sphere of the list,
    // so this sphere will follow the mouse and the other spheres will follow the same movement.

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
    // Moves spheres using inverse kinematics.
    // The first sphere remains immobile (like the shoulder for instance), the last sphere is moved to new_screen_pos
    // (like the hand) and the second sphere's position is calculated by inverse kinematics (like the elbow).
    // The remaining spheres follow the last one, which allow, for example, to move a complete hand with fingers.
    // Uses the algorithm found on Inigo Quilez's webpage : https://iquilezles.org/articles/simpleik/

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
    // Rotate the selected spheres around the axis defined by axis_point and axis

    // We apply the rotation to each selected sphere
    for (int sphere_id : spheres_id) {
        shared_ptr<sphere> sph = spheres_group->get_sphere_at(sphere_id); // get the sphere
        point3 sph_pos = sph->get_center(); // get the sphere position
        if (sph_pos != axis_point) {
            sph->set_center(point_rotation(sph_pos, axis_point, axis, angle));  // apply the rotation
            spheres_group->set_sphere_position(sphere_id, sph->get_center()); // update the sphere position in the sphere group
        }
    }
    update_skeleton_screen_coordinates();
}

void interactions::rotate_spheres_around_camera_axis(const std::span<int>& spheres_id, point3 axis_point, double angle) {
    // Rotate the selected spheres around the axis defined by the camera center and axis_point
    vec3 axis = axis_point - phong_cam->get_center(); // axis is the vector from the camera center to axis_point
    rotate_spheres_around_axis(spheres_id, axis, axis_point, angle); // call the general rotation function
    update_skeleton_screen_coordinates();
}

void interactions::rotate_camera(double horizontal_angle, double vertical_angle) {
    // Rotates the camera around the scene's center of rotation (cam_rot_center)
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
    // Loads a scene from file. If there is an error reading the file (file nor found, unreadable...), the modeler
    // creates a new scene similar to the start scene (with only two spheres).

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
    // Finds the nearest cone at the given screen position, and returns its id in spheres_group and the information
    // of its surface at the clicked point (the hit_record).
    ray r = phong_cam->get_ray(screen_pos_x, screen_pos_y);
    tuple<int, hit_record> find_cone = spheres_group->find_hit_cone(r, interval(0.001, infinity));
    return find_cone;
}

void interactions::add_link(int id1, int id2) {
    // Adds a link (a cone) between sphere id1 and sphere id2
    spheres_group->add_link(id1, id2);
    update_skeleton_screen_coordinates();
}

void interactions::start_beautiful_render(span3D beautiful_image) {
    // Launches (or restarts) the beautiful render in a separate thread

    // If there is already a render in progress, we stop it
    beautiful_cam->stop_beautiful_render();
    try {
        beautiful_render_task.get();
    }
    catch(const std::exception e) {}

    beautiful_render_task = std::async(&beautiful_camera::render, beautiful_cam, *world, beautiful_image);
}