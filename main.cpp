#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "cone.h"
#include "linked_spheres_group.h"
#include "interactions.h"

int main() {
    shared_ptr<hittable_list> world = make_shared<hittable_list>();
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // ground
    // world->add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));

    linked_spheres_group spheres(world, make_shared<sphere>(point3(-1.5, 0.25, -2.0), 0.2, material_right));
    spheres.add_sphere(make_shared<sphere>(point3(0.75, 0.25, -2.0), 0.8, material_right), 0);

    light white_light = new_light(point3(-1.0, 0.5, -1.0));
    //light red_light = new_colored_light(point3(0.0, 0.5, -1), point3(0.7, 0.2, 0.2));
    //light blue_light = new_colored_light(point3(1.0, -0.25, -1), point3(0.0, 0.0, 0.8));
    std::vector<light> lights {white_light};
    //std::vector<light> lights {white_light, red_light, blue_light};

    camera cam(16.0 / 9.0, 800, 1, 1);

    interactions inter(&spheres, world, &cam);
    inter.add_sphere_at_pos(500, 100);
    inter.segment_cone_at_pos(300, 200);
    inter.increase_radius(inter.detect_sphere_at_pos(300, 170), -0.2);
    inter.change_radius(inter.detect_sphere_at_pos(500, 67), 0.3);

    point3 center = {0.0, 0.25, -2.0};
    inter.rotate_camera(1.5, 0, center);
    inter.set_sphere_position_on_screen(0, 250, 200);
    inter.rotate_camera(-1.5, 0.0, center);
    inter.rotate_camera(0.5, -1.0, center);
    inter.move_camera_forward(-0.5);

    //auto [x, y, r] = inter.get_selection_circle(inter.detect_sphere_at_pos(55, 255));
    //std::cout << x << " " << y << " " << r;

    //inter.save("save.txt");

    //auto [spheres2, world2] = load_from_file("save.txt");
    //cam.render_phong_file(*world2, lights);

    cam.render_phong_file(*world, lights);
    //cam.render_file(world);
}