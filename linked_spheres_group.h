#ifndef LINKED_SPHERE_GROUP_H
#define LINKED_SPHERE_GROUP_H

#include "sphere.h"

#include <vector>

using std::vector;
using std::pair;

class linked_spheres_group {
public:
    linked_spheres_group(sphere first_sphere) {
        spheres.push_back(first_sphere);
    }

    int get_number_of_spheres() {
        return spheres.size();
    }

    sphere get_sphere_at(int i) {
        return spheres[i];
    }

    void add_sphere(sphere new_sphere, int linked_to) {
        links.push_back({spheres.size() - 1, linked_to});
        spheres.push_back(new_sphere);
    }

    void delete_sphere(int sphere_id) {
        spheres.erase(spheres.begin() + sphere_id);
        
        int i = 0;
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
        int i = 0;
        while (i < links.size()) {
            if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1))  {
                links.erase(links.begin() + i);
            }
            else {
                i++;
            }
        }    

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

        if (sphere1_unlinked) spheres.erase(spheres.begin() + id1);
        if (sphere2_unlinked) spheres.erase(spheres.begin() + id2);
    }

    bool linked(int id1, int id2) {
        for (int i = 0; i < links.size(); i++) {
            if ((links[i].first == id1 && links[i].second == id2) || (links[i].first == id2 && links[i].second == id1)) 
                return true;
        }
        return false;
    }

private:
    vector<pair<int, int>> links;
    vector<sphere> spheres;
};

#endif