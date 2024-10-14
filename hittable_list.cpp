#include "hittable_list.h"

void hittable_list::remove(shared_ptr<hittable> object) {
    int index = -1;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i] == object) {
            index = i;
        }
    }

    if (index != -1) objects.erase(objects.begin() + index);
}

bool hittable_list::hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const {
    // show_selec parameter determines whether we show the selection state in the hit_record
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec, show_selec)) {
            hit_anything = true;
            if (temp_rec.t < closest_so_far) {
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
    }

    return hit_anything;
}