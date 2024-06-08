#include <future>
#include "draw.h"
#include "camera.h"
#include "beautiful_camera.h"

void beautiful_camera::render_file(const hittable_list& world) {
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth, world);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::clog << "\rDone.                 \n";
}

void beautiful_camera::render(const hittable_list& world, span3D image) {
    beautiful_render_ready = false;
    continue_beautiful_render = true;
    float render_status;
    for (int j = 0; j < image_height; ++j) {
        render_status = 100 * (float) j / image_height;
        if (int(render_status) == render_status) std::cout << render_status << "%"<< std::endl;

        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                if (!continue_beautiful_render) {
                    return;
                }
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth, world);
            }
            color_pixel(image, {i, j}, pixel_color / samples_per_pixel);
        }
    }
    beautiful_render_ready = true;
    std::cout << "Done." << std::endl;
}

color beautiful_camera::ray_color(const ray& r, int depth, const hittable& world) const {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, interval(0.001, infinity), rec, false)) {
        if (rec.mat->descriptor().first == "unlit") {
            return rec.mat->get_material_color();
        }
        else {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                if (MODE == "normals") {
                    vec3 N = rec.normal;
                    return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
                } else if (MODE == "distances") {
                    float dist = (rec.p.length() - 1) / 2;
                    return color(dist, dist, dist);
                } else {
                    return attenuation * ray_color(scattered, depth - 1, world);
                }
            return color(0, 0, 0);
        }
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}