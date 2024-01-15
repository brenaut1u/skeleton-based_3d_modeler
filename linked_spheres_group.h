#ifndef LINKED_SPHERE_GROUP_H
#define LINKED_SPHERE_GROUP_H

#include "sphere.h"
#include "cone.h"

#include <vector>

using std::vector;
using std::pair;
using std::tuple;

struct cone_ref {
    shared_ptr<cone> cone;
    int sphere_id1;
    int sphere_id2;
}; typedef struct cone_ref cone_ref;

class linked_spheres_group {
public:
    linked_spheres_group() {}

    linked_spheres_group(hittable_list* _world, shared_ptr<sphere> first_sphere) : world(_world) {
        spheres.push_back(first_sphere);
        world->add(first_sphere);
    }

    int get_number_of_spheres() {
        return spheres.size();
    }

    shared_ptr<sphere> get_sphere_at(int i) {
        return spheres[i];
    }

    void add_sphere(shared_ptr<sphere> new_sphere, int linked_to) {
        spheres.push_back(new_sphere);
        add_link(spheres.size() - 1, linked_to);
    }

    void add_sphere_split_cone(shared_ptr<sphere> new_sphere, int linked_to1, int linked_to2) {
        if (linked(linked_to1, linked_to2)) {
            delete_link(linked_to1, linked_to2);   

            spheres.push_back(new_sphere);
            add_link(spheres.size() - 1, linked_to1);
            add_link(spheres.size() - 1, linked_to2);
        }
    }

    void delete_sphere(int sphere_id) {
        spheres.erase(spheres.begin() + sphere_id);

        int i = 0;
        while (i < cones.size()) {
            if (cones[i].sphere_id1 == sphere_id || cones[i].sphere_id2 == sphere_id) {
                world->remove(cones[i].cone);
                cones.erase(cones.begin() + i);
            }
            else {
                i++;
            }
        }
        
        i = 0;
        while (i < links.size()) {
            if (links[i].first == sphere_id || links[i].second == sphere_id) {
                links.erase(links.begin() + i);
            }
            else {
                i++;
            }
        }
    }

    void unlink(int id1, int id2) {
        delete_link(id1, id2); 

        // delete isolated spheres
        bool sphere1_unlinked = true;
        bool sphere2_unlinked = true;
        
        for(int i = 0; i < links.size(); i++) {
            if (links[i].first == id1 || links[i].second == id1) {
                sphere1_unlinked = false;
            }
            if (links[i].first == id2 || links[i].second == id2) {
                sphere2_unlinked = false;
            }
        }

        if (sphere1_unlinked) {
            delete_sphere(id1);
        }
        if (sphere2_unlinked) {
            delete_sphere(id2);
        }
    }

    void add_link(int id1, int id2) {
        links.push_back({id1, id2});
        shared_ptr<cone> new_cone = cone_from_spheres(spheres[id1], spheres[id2], spheres[id1]->get_material());
        cones.push_back(cone_ref {new_cone, id1, id2});
        world->add(new_cone);
        world->remove(spheres[id1]); //to avoid having the old sphere (if it exists) at the same position as the new cone overlapping each other
        world->remove(spheres[id2]);
    }

    bool linked(int id1, int id2) {
        for (int i = 0; i < links.size(); i++) {
            if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1)) 
                return true;
        }
        return false;
    }

    tuple<int, hit_record> find_hit_sphere(const ray& r, interval ray_t) {
        int index = -1;
        hit_record rec;

        for (int i = 0; i < spheres.size(); i++) {
            hit_record tmp_rec;
            if (spheres[i]->hit(r, ray_t, tmp_rec)) {
                if (index == - 1 || tmp_rec.t < rec.t) {
                    rec = tmp_rec;
                    index = i;
                }
            }
        }

        if (index != -1) {
            return tuple<int, hit_record>{index, rec};
        }
        else {
            return tuple<int, hit_record>{-1, rec};
        }
    }

    void change_sphere_at(shared_ptr<sphere> new_sphere, int id_sphere){
        spheres[id_sphere] = new_sphere;
        for (const pair<int, int> &val : links){
            if (std::get<0>(val) == id_sphere || std::get<1>(val) == id_sphere){
                delete_link(std::get<0>(val),std::get<1>(val));
                add_link(std::get<0>(val),std::get<1>(val));
            }
        }
    }


private:
    vector<pair<int, int>> links;
    vector<shared_ptr<sphere>> spheres;
    vector<cone_ref> cones;
    hittable_list* world;

    void delete_link(int id1, int id2) {
        // delete the link
        int i = 0;
        while (i < links.size()) {
            if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1))  {
                links.erase(links.begin() + i);
            }
            else {
                i++;
            }
        }

        // delete the cone
        i = 0;
        while (i < cones.size()) {
            if ((cones[i].sphere_id1 == id1 && cones[i].sphere_id2 == id2) ||
                        (cones[i].sphere_id1 == id2 && cones[i].sphere_id2 == id1)) {
                world->remove(cones[i].cone);
                cones.erase(cones.begin() + i);
            }
            else {
                i++;
            }
        }
    }
};

#endif