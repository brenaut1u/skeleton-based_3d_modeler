#ifndef PHONG_CAMERA_H
#define PHONG_CAMERA_H

#include "camera.h"

inline const color skeleton_background_color = {0.5, 0.9, 1.0};
inline const color skeleton_border_color = {0.2, 0.3, 0.5};
inline const color skeleton_selected_background_color = {1.0, 1.0, 0.0};
inline const color skeleton_selected_border_color = {1.0, 1.0, 1.0};
inline constexpr float skeleton_line_radius = 2;
inline constexpr float skeleton_circle_radius = 4;
inline const color progress_bar_color_left = {0.0, 0.5, 1.0};
inline const color progress_bar_color_right = {0.3, 0.3, 0.3};
inline constexpr int progress_bar_thickness = 3;

class phong_camera : public camera {
public:
    phong_camera(){}

    phong_camera(double _aspect_ratio, int _image_width, int _samples_per_pixel, int _max_depth) :
            camera(_aspect_ratio, _image_width, _samples_per_pixel, _max_depth) {
        initialize();
    }

    void render_file(const hittable_list& world, const std::vector<light>& lights);

    void render(const hittable_list& world, const std::vector<light>& lights, span3D image, float progress_bar_status);

    void render(const hittable_list& world, const std::vector<light>& lights, span3D image, float progress_bar_status, const vector<screen_segment>& skeleton);

private:
    color ray_color(const ray& r, int depth, const hittable& world, const std::vector<light>& lights) const;

    void render_partial(const hittable_list& world, const std::vector<light>& lights, span3D image, int start_x, int end_x, int start_y, int end_y);

    void draw_progress_bar(span3D image, float progress_bar_status);
};

#endif
