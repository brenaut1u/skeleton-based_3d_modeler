#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "light.h"
#include "save_load.h"

int main() {
    light white_light = new_light(point3(-1.0, 0.5, -1.0));
    std::vector<light> lights {white_light};

    camera cam(16.0 / 9.0, 800, 1, 1);

    auto [spheres, world] = load_from_file("oiseau.txt");
    cam.move_camera_forward(-7.0);
    cam.rotate_camera(-1.2, 0.3, point3(0.0, -1.0, 0.0));

    cam.render_phong_file(*world, lights);
    //cam.render_file(world);
}