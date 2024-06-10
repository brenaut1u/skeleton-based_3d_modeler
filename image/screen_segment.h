#ifndef SCREEN_SEGMENT_H
#define SCREEN_SEGMENT_H

#include <utility>
using std::pair;

/**
 * These classes are used to represent the skeleton's segments that appear on screen.
 */

class screen_point {
public:
    screen_point(pair<int, int> point) {
        x = point.first;
        y = point.second;
    }

public:
    int x;
    int y;
};

struct screen_segment {
    // positions of the first and second point, respectively
    screen_point first;
    screen_point second;

    // whether the first (resp. second) sphere is selected (affects the point's color)
    bool first_selected;
    bool second_selected;

    // the id of the first (resp. second) sphere in linked_spheres_group (used to select small spheres by clicking on their skeleton)
    int first_sphere_id;
    int second_sphere_id;
};

#endif