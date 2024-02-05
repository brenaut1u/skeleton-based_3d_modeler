#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "linked_spheres_group.h"

using std::stod;
using std::stoi;

void save_in_file(linked_spheres_group spheres_group, string filename) {
    std::ofstream file (filename);
    if (file.is_open())
    {
        file << spheres_group.save();
        file.close();
    }
}

vector<string> split(string txt) {
    vector<string> res;
    string curr;
    for (char c : txt) {
        if (c == ' ') {
            res.push_back(curr);
            curr = "";
        }
        else {
            curr += c;
        }
    }
    if (!curr.empty()) res.push_back(curr);
    return res;
}

pair<linked_spheres_group, shared_ptr<hittable_list>> load_from_file(string filename) {
    vector<shared_ptr<material>> materials;
    linked_spheres_group spheres_group;
    shared_ptr<hittable_list> world = make_shared<hittable_list>();

    string line;
    string category = "";
    bool first_sphere = true;
    std::ifstream file (filename);
    if (file.is_open())
    {
        while (getline (file,line))
        {
            if (line == "materials" || line == "spheres" || line == "links") {
                category = line;
            }
            else if (!line.empty()){
                if (category == "materials") {
                    vector<string> l = split(line);
                    string mat_type = l[0];
                    color col = {stod(l[1]), stod(l[2]), stod(l[3])};
                    if (mat_type == "lambertian") {
                        materials.push_back(make_shared<lambertian>(col));
                    }
                    else if (mat_type == "metal") {
                        materials.push_back(make_shared<metal>(col, stod(l[4])));
                    }
                }
                else if (category == "spheres") {
                    vector<string> l = split(line);
                    point3 center = {stod(l[0]), stod(l[1]), stod(l[2])};
                    double radius = stod(l[3]);
                    int mat_id = stoi(l[4]);
                    shared_ptr<sphere> sph = make_shared<sphere>(center, radius, materials[mat_id]);
                    if (first_sphere) {
                        spheres_group = linked_spheres_group(world, sph);
                        first_sphere = false;
                    }
                    else {
                        spheres_group.add_sphere(sph);
                    }
                }
                else if (category == "links") {
                    vector<string> l = split(line);
                    spheres_group.add_link(stoi(l[0]), stoi(l[1]));
                }
            }
        }
        file.close();
    }
    return {spheres_group, world};
}

#endif
