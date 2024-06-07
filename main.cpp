#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "light.h"
#include "save_load.h"
#include "interactions.h"

int main() {
    unique_ptr<interactions> inter = interactions::get_init_scene();
    shared_ptr<camera> cam = inter->get_phong_cam();
    shared_ptr<hittable_list> world = inter->get_world();

    //cam->render_phong_file(*world, lights);
    cam->render_file(*world);
}