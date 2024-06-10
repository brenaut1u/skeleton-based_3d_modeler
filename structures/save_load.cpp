#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "linked_spheres_group.h"
#include "../scene_objects/hittable_list.h"
#include "../utilities/color.h"
#include "../utilities/material.h"
#include "save_load.h"

/**
 * These functions are used to save and load linked_spheres_groups from files
 */

using std::stod;
using std::stoi;

void save_in_file(linked_spheres_group* spheres_group, string filename) {
    // Saves in file. If filename is incorrect, does nothing.
    std::ofstream file (filename);
    if (file.is_open())
    {
        file << spheres_group->save();
        file.close();
    }
}

vector<string> split(string txt) {
    // This function splits a string by removing whitespaces, and returns a vector of words
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

pair<shared_ptr<linked_spheres_group>, shared_ptr<hittable_list>> load_from_file(string filename) {
    // Loads from file. If filename is incorrect or if data in file doesn't match to a linked_spheres_group description,
    // throws an error.

    std::ifstream file (filename);
    vector<shared_ptr<material>> materials;
    auto spheres_group = shared_ptr<linked_spheres_group>();
    shared_ptr<hittable_list> world = make_shared<hittable_list>();

    string line = "";
    string category = "";
    bool first_sphere = true;

    if (!file.is_open()) {
        std::cerr<<"Filename incorrect" << std::endl;
        throw std::exception();
    }

    while (getline (file,line))
    {
        line.erase(std::remove(line.begin(), line.end(), '\r' ), line.end());

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
                    materials.push_back(make_shared<metal>(col, stof(l[4])));
                }
                else {
                    std::cerr<<"error loading file" << std::endl;
                    throw std::exception();
                }
            }
            else if (category == "spheres") {
                vector<string> l = split(line);
                point3 center = {stod(l[0]), stod(l[1]), stod(l[2])};
                double radius = stod(l[3]);
                int mat_id = stoi(l[4]);
                shared_ptr<sphere> sph = make_shared<sphere>(center, radius, materials[mat_id]);
                if (first_sphere) {
                    //std::cout<<"first sphere" << std::endl;
                    spheres_group = make_shared<linked_spheres_group>(world, sph);
                    first_sphere = false;
                }
                else {
                    spheres_group -> add_sphere(sph);
                }
            }
            else if (category == "links") {
                vector<string> l = split(line);
                spheres_group -> add_link(stoi(l[0]), stoi(l[1]));
            }
            else {
                std::cerr<<"error loading file" << std::endl;
                throw std::exception();
            }
        }
    }
    return {spheres_group, world};
}