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
}; typedef struct sphere_ref sphere_ref;

struct cone_ref {
    shared_ptr<cone> cone;
    int sphere_id1;
    int sphere_id2;
}; typedef struct cone_ref cone_ref;

struct material_ref {
    shared_ptr<material> mat;
    int nb_users;
}; typedef struct material_ref material_ref;

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

    shared_ptr<sphere> get_sphere_at(int i) {
        return spheres[i].sphere;
    }

    void add_sphere(shared_ptr<sphere> new_sphere) {
        int mat_id = -1;
        for (int i = 0; i < materials.size() && mat_id == -1; i++) {
            if (materials[i].mat == new_sphere->get_material()) {
                mat_id = i;
                materials[i].nb_users++;
            }
        }
        if (mat_id == -1) {
            materials.push_back({new_sphere->get_material(), 1});
            mat_id = materials.size() - 1;
        }
        spheres.push_back({new_sphere, mat_id});
    }

    void add_sphere(shared_ptr<sphere> new_sphere, int linked_to) {
        add_sphere(new_sphere);
        add_link(spheres.size() - 1, linked_to);
    }

    void add_sphere_split_cone(int cone_id, point3 p, vec3 n, shared_ptr<material> mat) {
        point3 c1 = spheres[cones[cone_id].sphere_id1].sphere->get_center();
        point3 c2 = spheres[cones[cone_id].sphere_id2].sphere->get_center();
        vec3 v = c2 - c1;
        double t = (p.y() - c1.y() + (n.y() / n.x()) * (c1.x() - p.x())) / (v.y() - v.x() * n.y() / n.x());
        point3 center = c1 + t * v;
        double radius = (p - center).length();
        shared_ptr<sphere> new_sphere = make_shared<sphere>(center, radius, mat);
        add_sphere(new_sphere, cones[cone_id].sphere_id1);
        add_link(spheres.size() - 1, cones[cone_id].sphere_id2);
        delete_link(cones[cone_id].sphere_id1, cones[cone_id].sphere_id2);
    }

    void delete_sphere(int sphere_id) {
        // delete cones
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

        // delete links
        i = 0;
        while (i < links.size()) {
            if (links[i].first == sphere_id || links[i].second == sphere_id) {
                links.erase(links.begin() + i);
            }
            else {
                i++;
            }
        }

        // material
        if (materials[spheres[sphere_id].material_id].nb_users == 1) {
            // delete the material if the sphere was its last user
            materials.erase(materials.begin() + spheres[sphere_id].material_id);
            for (auto s : spheres) {
                if (s.material_id >= spheres[sphere_id].material_id) {
                    s.material_id--;
                }
            }
        } else {
            materials[spheres[sphere_id].material_id].nb_users--;
        }

        spheres.erase(spheres.begin() + sphere_id);
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
        shared_ptr<cone> new_cone = cone_from_spheres(spheres[id1].sphere, spheres[id2].sphere, spheres[id1].sphere->get_material());
        cones.push_back(cone_ref {new_cone, id1, id2});
        world->add(new_cone);
        world->remove(spheres[id1].sphere); //to avoid having the old sphere (if it exists) at the same position as the new cone overlapping each other
        world->remove(spheres[id2].sphere);
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
            if (spheres[i].sphere->hit(r, ray_t, tmp_rec)) {
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

    tuple<int, hit_record> find_hit_cone(const ray& r, interval ray_t) {
        int index = -1;
        hit_record rec;

        for (int i = 0; i < cones.size(); i++) {
            hit_record tmp_rec;
            if (cones[i].cone->hit(r, ray_t, tmp_rec)) {
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

    void change_sphere_radius(int id, double radius) {
        spheres[id].sphere->set_radius(radius);
        for (const cone_ref &c : cones) {
            if (c.sphere_id1 == id) {
                c.cone->set_radius1(radius);
            }
            if (c.sphere_id2 == id) {
                c.cone->set_radius2(radius);
            }
        }
    }

    void increase_sphere_radius(int id, double radius) {
        spheres[id].sphere->increase_radius(radius);
        for (const cone_ref &c : cones) {
            if (c.sphere_id1 == id) {
                c.cone->increase_radius1(radius);
            }
            if (c.sphere_id2 == id) {
                c.cone->increase_radius2(radius);
            }
        }
    }

    void set_sphere_position(int id, point3 pos) {
        spheres[id].sphere->set_center(pos);
        for (const cone_ref &c : cones) {
            if (c.sphere_id1 == id) {
                c.cone->set_center1(pos);
            }
            if (c.sphere_id2 == id) {
                c.cone->set_center2(pos);
            }
        }
    }

    string save() {
        string txt = "";
        txt += "materials\n";
        for (const material_ref &mat : materials) {
            auto mat_descr = mat.mat->descriptor();
            txt += mat_descr.first;
            for (double i : mat_descr.second) {
                txt += " " + to_string(i);
            }
            txt += "\n";
        }

        txt += "\nspheres\n";
        for (const sphere_ref &s : spheres) {
            txt += to_string(s.sphere->get_center().x()) + " " + to_string(s.sphere->get_center().y()) + " " + to_string(s.sphere->get_center().z()) + " ";
            txt += to_string(s.sphere->get_radius()) + " ";
            txt += to_string(s.material_id) + "\n";
        }

        txt += "\nlinks\n";
        for (const pair<int, int> &l : links) {
            txt += to_string(l.first) + " " + to_string(l.second) + "\n";
        }

        return txt;
    }

private:
    vector<pair<int, int>> links;
    vector<sphere_ref> spheres;
    vector<cone_ref> cones;
    shared_ptr<hittable_list> world;
    vector<material_ref> materials;

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