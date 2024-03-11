#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "linked_spheres_group.h"

using std::stod;
using std::stoi;

void save_in_file(linked_spheres_group* spheres_group, string filename);

vector<string> split(string txt);

pair<linked_spheres_group, shared_ptr<hittable_list>> load_from_file(string filename);

#endif
