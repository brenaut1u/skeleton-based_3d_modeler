#ifndef LINKED_SPHERE_GROUP_H
#define LINKED_SPHERE_GROUP_H

#include "sphere.h"
#include "cone.h"
#include "hittable_list.h"
#include "material.h"

#include <string>
#include <vector>

using std::vector;
using std::pair;
using std::tuple;
using std::string;
using std::to_string;

struct sphere_ref {
    shared_ptr<sphere> sphere;
    int material_id;
};

struct cone_ref {
    shared_ptr<cone> cone;
    int sphere_id1;
    int sphere_id2;
};

struct material_ref {
    shared_ptr<material> mat;
    int nb_users;
};

class linked_spheres_group {
public:
    linked_spheres_group() {}

    linked_spheres_group(const linked_spheres_group &other) = default;
    linked_spheres_group& operator=(const linked_spheres_group &other) = default;

    linked_spheres_group(shared_ptr<hittable_list> _world, shared_ptr<sphere> first_sphere) : world(_world) {
        materials.push_back({first_sphere->get_material(), 1});
        spheres.push_back({first_sphere, 0});
        world->add(first_sphere);
    }

    int get_number_of_spheres() {
        return spheres.size();
    }
    int get_number_of_links() {
        return links.size();
    }

    shared_ptr<sphere> get_sphere_at(int i) {
        if (i >= 0 && i < spheres.size()) {
            return spheres[i].sphere;
        }
        else {
            return nullptr;
        }
    }

    vector<pair<int, int>> get_links() const {
        return links;
    }

    void sphere_is_selected(int id_selected) ;

    void sphere_is_unselected(int id_selected) ;

    void sphere_is_hovered(int id_selected) ;

    void add_sphere(shared_ptr<sphere> new_sphere);

    void add_sphere(shared_ptr<sphere> new_sphere, int linked_to);

    void add_sphere_split_cone(int cone_id, point3 p, vec3 n, shared_ptr<material> mat);

    void delete_sphere(int sphere_id);

    void unlink(int id1, int id2);

    void add_link(int id1, int id2);

    bool linked(int id1, int id2);

    tuple<int, hit_record> find_hit_sphere(const ray& r, interval ray_t);

    tuple<int, hit_record> find_hit_cone(const ray& r, interval ray_t);

    void change_sphere_radius(int id, double radius);

    void increase_sphere_radius(int id, double radius);

    void set_sphere_position(int id, point3 pos);

    void set_sphere_color(int id, color c);

    void delete_isolated_spheres();

    string save();

private:
    vector<pair<int, int>> links;
    vector<sphere_ref> spheres;
    vector<cone_ref> cones;
    shared_ptr<hittable_list> world;
    vector<material_ref> materials;

    bool is_sphere_isolated(int sphere_id);
    int nb_sphere_links(int sphere_id);
};

#endif