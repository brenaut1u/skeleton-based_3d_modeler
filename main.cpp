#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "light.h"
#include "save_load.h"
#include "interactions.h"

int main() {
    light white_light = new_light(point3(-1.0, 0.5, -1.0));
    std::vector<light> lights {white_light};

    camera cam(16.0 / 9.0, 800, 50, 10);

//    shared_ptr<hittable_list> world = make_shared<hittable_list>();
//    auto material = make_shared<lambertian>(color(0.7, 0.3, 0.3));
//
//    linked_spheres_group spheres(world, make_shared<sphere>(point3(0.2, 0.0, -2.0), 0.1, material));
//    spheres.add_sphere(make_shared<sphere>(point3(0.1, 0.0, -2.0), 0.5, material), 0);

    auto [spheres, world] = load_from_file("oiseau2.txt");
    interactions inter = interactions(spheres, world, &cam);
    inter.move_camera_forward(-5);
    inter.rotate_camera(-0.9, 0.0);
    inter.move_camera_sideways(0.4, 0.2);

    cam.render_phong_file(*world, lights);
    //cam.render_file(*world);
}