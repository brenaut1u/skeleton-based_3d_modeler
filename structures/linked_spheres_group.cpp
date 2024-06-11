#include "linked_spheres_group.h"

void linked_spheres_group::add_sphere(shared_ptr<sphere> new_sphere) {
    // Adds a sphere and updates the materials list
    int mat_id = add_material(new_sphere->get_material());
    spheres.push_back({new_sphere, mat_id, false});
}

void linked_spheres_group::add_sphere(shared_ptr<sphere> new_sphere, int linked_to) {
    // Adds a sphere to be linked to the already existing sphere whose id is given by linked_to parameter
    add_sphere(new_sphere);
    add_link(spheres.size() - 1, linked_to);
    add_link(spheres.size() - 1, spheres.size() - 1);
}

void linked_spheres_group::add_sphere_split_cone(int cone_id, point3 p, vec3 n, shared_ptr<material> mat) {
    // Adds a sphere that splits a cone. The cone is split in such a way that its shape remains unaffected.
    // This is performed by adding setting the new sphere's center at the interception point between the normal at point p
    // and the cone's central axis.

    point3 c1 = spheres[cones[cone_id].sphere_id1].sphere->get_center();
    point3 c2 = spheres[cones[cone_id].sphere_id2].sphere->get_center();
    vec3 v = c2 - c1;
    double t = lines_intersection(c1, v, p, n);
    point3 center = c1 + t * v;
    double radius = (p - center).length();
    shared_ptr<sphere> new_sphere = make_shared<sphere>(center, radius, mat);
    add_sphere(new_sphere, cones[cone_id].sphere_id1);
    add_link(spheres.size() - 1, cones[cone_id].sphere_id2);
    unlink(cones[cone_id].sphere_id1, cones[cone_id].sphere_id2);
}

void linked_spheres_group::delete_sphere(const std::span<int>& spheres_id) {
    // We need to delete the spheres in reverse order to avoid changing the id of the spheres that we want to delete.
    // When we delete a sphere, we also delete the cones that are linked to it.
    // If the material of the sphere is not used by any other sphere, we delete it.
    // We also update the ids of the spheres that are linked to the spheres that we delete.
    // If there are only two spheres in the scene, the deletion is not done because it would cause the deletion of
    // all objects in the scene.

    for (auto it = spheres_id.rbegin(); it != spheres_id.rend(); ++it) {
        int id = *it; // id of the sphere to delete
        if (spheres.size() > 2) { // we need at least 2 spheres
            int i = 0;

            // we need to check for all the cones if they are linked to the sphere to delete
            // or if they have a sphere with a greater id than the id of the sphere to delete
            while (i < cones.size()) { 
                // delete the cones linked to the sphere
                if (cones[i].sphere_id1 == id || cones[i].sphere_id2 == id) {
                    world->remove(cones[i].cone);
                    cones.erase(cones.begin() + i);
                } 
                // update the id of the remaining spheres, those who have an id greater than the id of the sphere to delete
                else {
                    if (cones[i].sphere_id1 > id) {
                        cones[i].sphere_id1--;
                    }
                    if (cones[i].sphere_id2 > id) {
                        cones[i].sphere_id2--;
                    }
                    i++;
                }
            }

            // We need to check if the sphere is linked to another sphere
            // if it is the case, we need to delete the link
            i = 0;
            // We need to do the same for each link
            while (i < links.size()) {
                // If the link is linked to the sphere to delete, we delete it
                if (links[i].first == id || links[i].second == id) {
                    links.erase(links.begin() + i);
                } 
                // If the link is linked to a sphere with a greater id than the id of the sphere to delete
                else {
                    if (links[i].first > id) {
                        links[i].first--;
                    }
                    if (links[i].second > id) {
                        links[i].second--;
                    }
                    i++;
                }
            }

            // We need to delete the material of the sphere if it is not used by any other sphere
            if (materials[spheres[id].material_id].nb_users == 1) {
                materials.erase(materials.begin() + spheres[id].material_id);
                // We need to update the id of the material of the spheres 
                // that have a material with a greater id than the id of the material of the sphere to delete
                for (int s = 0; s < spheres.size(); s++) {
                    if (s != id && spheres[s].material_id >= spheres[id].material_id) {
                        spheres[s].material_id--;
                    }
                }
            } 
            // If the material is used by another sphere, we just decrement the number of users
            else {
                materials[spheres[id].material_id].nb_users--;
            }
            // Lastly, we delete the sphere from the spheres vector
            spheres.erase(spheres.begin() + id);
        }
    }
}

void linked_spheres_group::add_link(int id1, int id2) {
    // Adds a link between sphere id1 and sphere id2.

    links.push_back({id1, id2});
    shared_ptr<cone> new_cone = cone_from_spheres(spheres[id1].sphere, spheres[id2].sphere, spheres[id1].sphere->get_material(), spheres[id2].sphere->get_material());
    cones.push_back(cone_ref {new_cone, id1, id2});

    // Update the world
    world->add(new_cone);
    world->remove(spheres[id1].sphere); //to avoid having the old sphere (if it exists) at the same position as the new cone overlapping each other
    world->remove(spheres[id2].sphere);
}

void linked_spheres_group::unlink(int id1, int id2) {
    // Deletes the link between sphere id1 and sphere id2

    // Delete the link
    int i = 0;
    while (i < links.size()) {
        if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1))  {
            links.erase(links.begin() + i);
        }
        else {
            i++;
        }
    }

    // Delete the cone
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

bool linked_spheres_group::are_linked(int id1, int id2) {
    // Tests if sphere id1 and sphere id2 are linked
    for (int i = 0; i < links.size(); i++) {
        if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1))
            return true;
    }
    return false;
}

int linked_spheres_group::add_material(shared_ptr<material> mat) {
    // Adds a material to the materials list. If the material is already in the list,
    // the matching material's number of users is incremented.
    // Returns the id of the added material.
    // In the current state of the project, we could disable the loop and just add the new material to the list,
    // no matter if there is already another similar material in the list. However, this could be useful
    // if future features were to be added, such as the modification of a material (eg: we have a red object made of
    // multiple spheres, and we want everything to be blue. Then we only have to modify the shared material.)

    for (int i = 0; i < materials.size(); i++) {
        if (are_same_materials(materials[i].mat, mat)) {
            materials[i].nb_users++;
            return i;
        }
    }

    materials.push_back({mat, 1});
    return materials.size() - 1;
}

tuple<int, hit_record> linked_spheres_group::find_hit_sphere(const ray& r, interval ray_t) {
    // Finds the sphere hit by the ray.
    // If such a sphere exists, its id is returned with the intersection information (the hit_record).
    // Else, return -1 and an empty hit_record.

    int index = -1;
    hit_record rec;

    for (int i = 0; i < spheres.size(); i++) {
        hit_record tmp_rec;
        if (spheres[i].sphere->hit(r, ray_t, tmp_rec, false)) {
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
    // Finds the cone hit by the ray.
    // If such a cone exists, its id is returned with the intersection information (the hit_record).
    // Else, return -1 and an empty hit_record.

    int index = -1;
    hit_record rec;

    for (int i = 0; i < cones.size(); i++) {
        hit_record tmp_rec;
        if (cones[i].cone->hit(r, ray_t, tmp_rec, false)) {
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

void linked_spheres_group::set_sphere_radius(int id, double radius) {
    // Sets the sphere's radius at exact given value

    spheres.at(id).sphere->set_radius(radius);

    // Updates the cones
    for (const cone_ref &c : cones) {
        if (c.sphere_id1 == id) {
            c.cone->set_radius1(radius);
        }
        if (c.sphere_id2 == id) {
            c.cone->set_radius2(radius);
        }
    }
}

void linked_spheres_group::increase_sphere_radius(int id, double delta_radius) {
    // Increases or decreases the sphere's radius by delta_radius

    spheres.at(id).sphere->increase_radius(delta_radius);

    // Updates the cones
    for (const cone_ref &c : cones) {
        if (c.sphere_id1 == id) {
            c.cone->increase_radius1(delta_radius);
        }
        if (c.sphere_id2 == id) {
            c.cone->increase_radius2(delta_radius);
        }
    }
}

void linked_spheres_group::set_sphere_position(int id, point3 pos) {
    // Moves the sphere at the exact given position

    spheres[id].sphere->set_center(pos);

    //Updates the cones
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
    // Returns a string containing the scene's information.
    // First, the string contains the list of the materials, then the list of the spheres, and finally the list of the links.

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

void linked_spheres_group::set_sphere_color(int id, color c) {
    // Changes the color of the spheres and updates the materials list

    // Creates the new material with the given color
    int mat_id = spheres[id].material_id;
    shared_ptr<material> mat = copy_material_change_color(materials[mat_id].mat, c);

    // Updates the materials list
    spheres[id].material_id = add_material(mat);
    materials[mat_id].nb_users--;
    if (materials[mat_id].nb_users == 0) {
        materials.erase(materials.begin() + mat_id);
        for (int s = 0; s < spheres.size(); s++) {
            if (spheres[s].material_id > mat_id) {
                spheres[s].material_id--;
            }
        }
    }

    spheres[id].sphere->set_mat(mat);

    // Updates the cones
    for (const cone_ref &cone : cones) {
        if (cone.sphere_id1 == id) {
            cone.cone->set_mat1(mat);
        }
        if (cone.sphere_id2 == id) {
            cone.cone->set_mat2(mat);
        }
    }
}

void linked_spheres_group::select_sphere(int id_selected) {
    // Marks a sphere as selected

    spheres[id_selected].is_selected = true;

    // Updates the cones
    for (const auto& c : cones) {
        if (c.sphere_id1 == id_selected) {
            if (c.cone->is_selected(2)) {
                c.cone->set_selected(3);
            }
            else if (not c.cone->is_selected(3)) {
                c.cone->set_selected(1);
            }
        }
        if (c.sphere_id2 == id_selected) {
            if (c.cone->is_selected(1)) {
                c.cone->set_selected(3);
            }
            else if (not c.cone->is_selected(3)){
                c.cone->set_selected(2);
            }
        }
        if (!c.cone->is_selected(1) && !c.cone->is_selected(2) && c.cone->is_selected(3)) {
            c.cone->set_selected(0);
        }
    }
}

void linked_spheres_group::unselect_sphere(int id_selected){
    // Marks a sphere as unselected

    spheres[id_selected].is_selected = false;

    // Updates the cones
    for (const auto& c : cones) {
        if (c.sphere_id1 == id_selected) {
            if (c.cone->is_selected(1)) {
                c.cone->set_selected(0);
            }
            else if (c.cone->is_selected(3)) {
                c.cone->set_selected(2);
            }
        }
        else if (c.sphere_id2 == id_selected) {
            if (c.cone->is_selected(2)) {
                c.cone->set_selected(0);
            }
            else if (c.cone->is_selected(3)) {
                c.cone->set_selected(1);
            }
        }
    }
}

void linked_spheres_group::hover_sphere(int id_hovered){
    // Marks all the spheres as not hovered except sphere id_hovered
    for (const auto& c : cones) {
        c.cone->set_hovered(0);
        if (c.sphere_id1 == id_hovered) {
            c.cone->set_hovered(1);
        }
        if (c.sphere_id2 == id_hovered) {
            c.cone->set_hovered(2);
        }
    }
}