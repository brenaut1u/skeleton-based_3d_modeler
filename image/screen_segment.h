#ifndef SCREEN_SEGMENT_H
#define SCREEN_SEGMENT_H

#include <utility>
using std::pair;

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
    screen_point first;
    screen_point second;
    bool first_selected;
    bool second_selected;
    int first_sphere_id;
    int second_sphere_id;
};

#endif