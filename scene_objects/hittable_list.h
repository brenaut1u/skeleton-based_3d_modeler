#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

/**
 * This class represents a list of hittable objects. It basically represents the world.
 * It is used to iterate over all the objects in order to find the closest one intersected by a ray.
 */

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    void remove(shared_ptr<hittable> object);

    bool hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const override;

    int nb_objects() {
        return objects.size();
    }
};

#endif