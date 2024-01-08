#ifndef INTERACTIONS
#define INTERACTIONS

#include "ray.h"
#include "camera.h"
#include "hittable_list.h"
#include "linked_spheres_group.h"

class interactions {
    public:
        interactions(linked_spheres_group _spheres_group, hittable_list* _world, camera _cam) :
        spheres_group(_spheres_group), world(_world), cam(_cam) {}

        void add_sphere_at_pos(int screen_pos_x, int screen_pos_y) {
            ray r = cam.get_ray(screen_pos_x, screen_pos_y);
            tuple<int, hit_record> find_sphere = spheres_group.find_hit_sphere(r, interval(0.001, infinity));
            if (std::get<0>(find_sphere) != -1) {
                shared_ptr<sphere> new_sphere = make_shared<sphere>(std::get<1>(find_sphere).p, 0.3, std::get<1>(find_sphere).mat);
                spheres_group.add_sphere(new_sphere, std::get<0>(find_sphere));
            }
        }
    private:
        linked_spheres_group spheres_group;
        hittable_list* world;
        camera cam;
};

#endif