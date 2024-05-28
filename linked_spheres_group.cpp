#include "linked_spheres_group.h"

void linked_spheres_group::add_sphere(shared_ptr<sphere> new_sphere) {
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

void linked_spheres_group::add_sphere(shared_ptr<sphere> new_sphere, int linked_to) {
    add_sphere(new_sphere);
    add_link(spheres.size() - 1, linked_to);
}

void linked_spheres_group::add_sphere_split_cone(int cone_id, point3 p, vec3 n, shared_ptr<material> mat) {
    point3 c1 = spheres[cones[cone_id].sphere_id1].sphere->get_center();
    point3 c2 = spheres[cones[cone_id].sphere_id2].sphere->get_center();
    vec3 v = c2 - c1;
    double t = (p.y() - c1.y() + (n.y() / n.x()) * (c1.x() - p.x())) / (v.y() - v.x() * n.y() / n.x());
    point3 center = c1 + t * v;
    double radius = (p - center).length();
    shared_ptr<sphere> new_sphere = make_shared<sphere>(center, radius, mat);
    add_sphere(new_sphere, cones[cone_id].sphere_id1);
    add_link(spheres.size() - 1, cones[cone_id].sphere_id2);
    unlink(cones[cone_id].sphere_id1, cones[cone_id].sphere_id2);
}

void linked_spheres_group::delete_sphere(int sphere_id) {
    if (spheres.size() > 2) {
        if (spheres.size() == 3 && nb_sphere_links(sphere_id) == 2) {
            //prevents deletion of all the spheres if there are three spheres and we are rying to delete the middle
            //sphere (as the isolated spheres are deleted too)
            return;
        }

        // update cones
        int i = 0;
        while (i < cones.size()) {
            if (cones[i].sphere_id1 == sphere_id || cones[i].sphere_id2 == sphere_id) {
                world->remove(cones[i].cone);
                cones.erase(cones.begin() + i);
            } else {
                if (cones[i].sphere_id1 > sphere_id) {
                    cones[i].sphere_id1--;
                }
                if (cones[i].sphere_id2 > sphere_id) {
                    cones[i].sphere_id2--;
                }
                i++;
            }
        }

        // update links
        i = 0;
        while (i < links.size()) {
            if (links[i].first == sphere_id || links[i].second == sphere_id) {
                links.erase(links.begin() + i);
            } else {
                if (links[i].first > sphere_id) {
                    links[i].first--;
                }
                if (links[i].second > sphere_id) {
                    links[i].second--;
                }
                i++;
            }
        }

        // material
        if (materials[spheres[sphere_id].material_id].nb_users == 1) {
            // delete the material if the sphere was its last user
            materials.erase(materials.begin() + spheres[sphere_id].material_id);
            for (auto s: spheres) {
                if (s.material_id >= spheres[sphere_id].material_id) {
                    s.material_id--;
                }
            }
        } else {
            materials[spheres[sphere_id].material_id].nb_users--;
        }

        spheres.erase(spheres.begin() + sphere_id);
    }
}

void linked_spheres_group::add_link(int id1, int id2) {
    links.push_back({id1, id2});
    shared_ptr<cone> new_cone = cone_from_spheres(spheres[id1].sphere, spheres[id2].sphere, spheres[id1].sphere->get_material());
    cones.push_back(cone_ref {new_cone, id1, id2});
    world->add(new_cone);
    world->remove(spheres[id1].sphere); //to avoid having the old sphere (if it exists) at the same position as the new cone overlapping each other
    world->remove(spheres[id2].sphere);
}

void linked_spheres_group::unlink(int id1, int id2) {
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

bool linked_spheres_group::linked(int id1, int id2) {
    for (int i = 0; i < links.size(); i++) {
        if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1))
            return true;
    }
    return false;
}

tuple<int, hit_record> linked_spheres_group::find_hit_sphere(const ray& r, interval ray_t) {
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

tuple<int, hit_record> linked_spheres_group::find_hit_cone(const ray& r, interval ray_t) {
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

void linked_spheres_group::change_sphere_radius(int id, double radius) {
    spheres.at(id).sphere->set_radius(radius);
    for (const cone_ref &c : cones) {
        if (c.sphere_id1 == id) {
            c.cone->set_radius1(radius);
        }
        if (c.sphere_id2 == id) {
            c.cone->set_radius2(radius);
        }
    }
}

void linked_spheres_group::increase_sphere_radius(int id, double radius) {
    spheres.at(id).sphere->increase_radius(radius);
    for (const cone_ref &c : cones) {
        if (c.sphere_id1 == id) {
            c.cone->increase_radius1(radius);
        }
        if (c.sphere_id2 == id) {
            c.cone->increase_radius2(radius);
        }
    }
}

void linked_spheres_group::set_sphere_position(int id, point3 pos) {
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

string linked_spheres_group::save() {
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

void linked_spheres_group::delete_isolated_spheres() {
    int sphere_id = 0;
    while (sphere_id < spheres.size()) {
        if (is_sphere_isolated(sphere_id)) {
            delete_sphere(sphere_id);
        }
        else {
            sphere_id++;
        }
    }
}

bool linked_spheres_group::is_sphere_isolated(int sphere_id) {
    for (const pair<int, int> link : links) {
        if (link.first == sphere_id || link.second == sphere_id) {
            return false;
        }
    }
    return true;
}

int linked_spheres_group::nb_sphere_links(int sphere_id) {
    int nb_links = 0;
    for (const pair<int, int> link : links) {
        if (link.first == sphere_id || link.second == sphere_id) {
            nb_links++;
        }
    }
    return nb_links;
}

void linked_spheres_group::sphere_is_selected(int id_selected) {
    int i = 0;
    while (i < cones.size()) {
        if (cones[i].sphere_id1 == id_selected) {
            // cones[i].cone->set_selected(1);
            if (cones[i].cone->is_selected(2)) {
                cones[i].cone->set_selected(3);
            }
            else if (not cones[i].cone->is_selected(3)) { 
                cones[i].cone->set_selected(1);
            }
        }
        else if (cones[i].sphere_id2 == id_selected) {
            // cones[i].cone->set_selected(2);
            if (cones[i].cone->is_selected(1)) {
                cones[i].cone->set_selected(3);
            }
            else if (not cones[i].cone->is_selected(3)){ 
                cones[i].cone->set_selected(2);
            }
        }
        else {
            cones[i].cone->set_selected(0);
        }
        i++;
    }
}