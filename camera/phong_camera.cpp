#include <future>
#include "../image/draw.h"
#include "camera.h"
#include "phong_camera.h"

void phong_camera::render_file(const hittable_list& world, const std::vector<light>& lights) {
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth, world, lights);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::clog << "\rDone.                 \n";
}

void phong_camera::render(const hittable_list& world, const std::vector<light>& lights, span3D image, float progress_bar_status) {
    std::future<void> task1 = std::async(&phong_camera::render_partial, this, world, lights, image, 0, image_width / 3, 0, image_height);
    std::future<void> task2 = std::async(&phong_camera::render_partial, this, world, lights, image, image_width / 3, 2 * image_width / 3, 0, image_height);
    std::future<void> task3 = std::async(&phong_camera::render_partial, this, world, lights, image, 2 * image_width / 3, image_width, 0, image_height);

    task1.get();
    task2.get();
    task3.get();
    draw_progress_bar(image, progress_bar_status);
}

void phong_camera::render(const hittable_list& world, const std::vector<light>& lights, span3D image, float progress_bar_status, const vector<screen_segment>& skeleton) {
    render(world, lights, image, progress_bar_status);

    for (const auto& segment : skeleton) {
        draw_line(image, segment.first, segment.second, skeleton_line_radius, skeleton_background_color, skeleton_border_color);

        color background_color = segment.first_selected ? skeleton_selected_background_color : skeleton_background_color;
        color border_color = segment.first_selected ? skeleton_selected_border_color : skeleton_border_color;
        draw_circle(image, segment.first, skeleton_circle_radius, background_color, border_color);

        background_color = segment.second_selected ? skeleton_selected_background_color : skeleton_background_color;
        border_color = segment.second_selected ? skeleton_selected_border_color : skeleton_border_color;
        draw_circle(image, segment.second, skeleton_circle_radius, background_color, border_color);
    }

    draw_progress_bar(image, progress_bar_status);
}

void phong_camera::render_partial(const hittable_list& world, const std::vector<light>& lights, span3D image, int start_x, int end_x, int start_y, int end_y) {
    for (int j = start_y; j < end_y; ++j) {
        for (int i = start_x; i < end_x; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth, world, lights);
            }
            color_pixel(image, {i, j}, pixel_color / samples_per_pixel);
        }
    }
}

color phong_camera::ray_color(const ray& r, int depth, const hittable& world, const std::vector<light>& lights) const {
    hit_record rec;
    vec3 intensity = {0, 0, 0};

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, interval(0.001, infinity), rec, true)) {
        if (rec.mat->descriptor().first == "unlit") {
            return rec.mat->get_material_color();
        }
        else {
            for (int i = 0; i < lights.size(); i++) {
                vec3 intensity_i = {1, 1, 1};

                light l = lights.at(i);
                vec3 light_ray = {0.0, 1.0, 0.0};//unit_vector(l.pos - rec.p);
                vec3 reflected_ray = 2.0 * dot(rec.normal, light_ray) * rec.normal - light_ray;

                double diffuse = max(0, dot(rec.normal, light_ray));
                double specular = specular_coefficient * max(0, dot(reflected_ray, unit_vector(center - rec.p)));
                intensity_i *= diffuse + specular + ambient_occlusion;
                intensity_i = term_to_term_product(intensity_i, l.ray_color);

                intensity += intensity_i;
            }
            return term_to_term_product(intensity, rec.mat->get_material_color());
        }
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

void phong_camera::draw_progress_bar(span3D image, float progress_bar_status) {
    for (int i = 0; i < image.size_X(); i++) {
        if (i < progress_bar_status * image.size_X()) {
            for (int j = 1; j <= progress_bar_thickness; j++) {
                color_pixel(image, {i, image.size_Y() - j}, progress_bar_color_left);
            }
        }
        else {
            for (int j = 1; j <= progress_bar_thickness; j++) {
                color_pixel(image, {i, image.size_Y() - j}, progress_bar_color_right);
            }
        }
    }
}