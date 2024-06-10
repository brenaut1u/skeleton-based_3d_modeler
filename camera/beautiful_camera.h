#ifndef BEAUTIFUL_CAMERA_H
#define BEAUTIFUL_CAMERA_H

#include "camera.h"

/**
 * This class represents a camera that uses a normal, complete raytracing method, with recursive rays.
 * The render is more realistic than phong_camera but takes more time.
 * The render function can be called in an independent thread, and the class can communicate the render status.
 */

class beautiful_camera : public camera {
public:
    beautiful_camera(){}

    beautiful_camera(double _aspect_ratio, int _image_width, int _samples_per_pixel, int _max_depth) :
            camera(_aspect_ratio, _image_width, _samples_per_pixel, _max_depth) {
        initialize();
    }

    void render_file(const hittable_list& world);

    void render(const hittable_list& world, span3D image);

    void stop_beautiful_render() {continue_beautiful_render = false;} // Allows to stop the render from a different thread

    bool is_beautiful_render_ready() const {
        return beautiful_render_ready;}

    float get_render_status() const {return render_status;}

private:
    bool continue_beautiful_render = true; // used to stop the render
    bool beautiful_render_ready = false;
    float render_status; // between 0.0 and 1.0

    color ray_color(const ray& r, int depth, const hittable& world) const;
};


#endif
