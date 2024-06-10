#include <future>
#include "../image/draw.h"
#include "camera.h"
#include "beautiful_camera.h"

/**
 * This class represents a camera that uses a normal, complete raytracing method, with recursive rays.
 * The render is more realistic than phong_camera but takes more time.
 * The render function can be called in an independent thread, and the class can communicate the render status.
 */

void beautiful_camera::render_file(const hittable_list& world) {
    // This function outputs the computed image directly to the console in PPM format, and can therefore
    // be used to store the image data in a file.

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
    // This function computes the render and stores it in the given span3D image.
    // Once the render is launched, the beautiful_render_ready and render_status attributes are
    // regularly updated, which allows to know the render status from a different thread.
    // The render is stopped when continue_beautiful_render becomes false, which allows
    // to interrupt the render from a different thread using the stop_beautiful_render() method.

    beautiful_render_ready = false;
    continue_beautiful_render = true;
    for (int j = 0; j < image_height; ++j) {
        render_status = (float) j / image_height;

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
}

color beautiful_camera::ray_color(const ray& r, int depth, const hittable& world) const {
    // This function returns the color found by the ray when intersecting an object.
    // The recursion allows the ray to bounce over the object's surface, creating shadows and reflection.

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
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}