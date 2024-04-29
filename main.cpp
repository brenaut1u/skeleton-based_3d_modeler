#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "light.h"
#include "save_load.h"

int main() {
    light white_light = new_light(point3(-1.0, 0.5, -1.0));
    std::vector<light> lights {white_light};

    camera cam(16.0 / 9.0, 800, 1, 1);

    shared_ptr<hittable_list> world = make_shared<hittable_list>();
    auto material = make_shared<lambertian>(color(0.7, 0.3, 0.3));

    linked_spheres_group spheres(world, make_shared<sphere>(point3(0.2, 0.0, -2.0), 0.1, material));
    spheres.add_sphere(make_shared<sphere>(point3(0.1, 0.0, -2.0), 0.5, material), 0);

    cam.render_phong_file(*world, lights);
    //cam.render_file(world);
}