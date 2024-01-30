#ifndef CAMERA_H
#define CAMERA_H

#include "interval.h"
#include "color.h"
#include "hittable.h"
#include "material.h"
#include "light.h"

#include <iostream>
#include <vector>

#define MODE " "
#define AMBIENT_OCCLUSION 0.1

class camera {
  public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth         = 10;   // Maximum number of ray bounces into scene

    camera(){}
    
    camera(double _aspect_ratio, int _image_width, int _samples_per_pixel, int _max_depth) :
                                    aspect_ratio(_aspect_ratio), image_width(_image_width),
                                    samples_per_pixel(_samples_per_pixel), max_depth(_max_depth) {
        initialize();
    }

    void render_file(const hittable& world) {
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
    

    std::vector<vec3> render(const hittable& world) {
        std::vector<vec3> rendered_image;

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                rendered_image.push_back(pixel_color);              
            }
        }
        return rendered_image;
    }

    void render_phong_file(const hittable& world, const std::vector<light>& lights) {
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    ray_color_with_point_lights(r, max_depth, world, lights);
                    pixel_color += ray_color_with_point_lights(r, max_depth, world, lights);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);                
            }
        }

        std::clog << "\rDone.                 \n";        
    }

    std::vector<vec3> render_phong(const hittable& world, const std::vector<light>& lights) {
        std::vector<vec3> rendered_image;

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color_with_point_lights(r, max_depth, world, lights);
                }
                rendered_image.push_back(pixel_color);              
            }
        }  
        return rendered_image;    
    }   

    ray get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    void rotate_camera(double horizontal_angle, double vertical_angle, point3 rot_center) {
        // horizontal rotation
        vec3 h_axis = vec3(0.0, 1.0, 0.0);
        vec3 pc = center - rot_center;
        vec3 pl = pixel00_loc - rot_center;
        vec3 pc_rot = cos(horizontal_angle) * pc + sin(horizontal_angle) * cross(h_axis, pc) + (1 - cos(horizontal_angle)) * dot(h_axis, pc) * h_axis;
        center = rot_center + pc_rot;
        vec3 pl_rot = cos(horizontal_angle) * pl + sin(horizontal_angle) * cross(h_axis, pl) + (1 - cos(horizontal_angle)) * dot(h_axis, pl) * h_axis;
        pixel00_loc = rot_center + pl_rot;
        viewport_u = cos(horizontal_angle) * viewport_u + sin(horizontal_angle) * cross(h_axis, viewport_u) + (1 - cos(horizontal_angle)) * dot(h_axis, viewport_u) * h_axis;
        pixel_delta_u = viewport_u / image_width;

        // vertical rotation
        vec3 v_axis = unit_vector(viewport_u);
        pc = center - rot_center;
        pl = pixel00_loc - rot_center;
        pc_rot = cos(vertical_angle) * pc + sin(vertical_angle) * cross(v_axis, pc) + (1 - cos(vertical_angle)) * dot(v_axis, pc) * v_axis;
        center = rot_center + pc_rot;
        pl_rot = cos(vertical_angle) * pl + sin(vertical_angle) * cross(v_axis, pl) + (1 - cos(vertical_angle)) * dot(v_axis, pl) * v_axis;
        pixel00_loc = rot_center + pl_rot;
        viewport_v = cos(vertical_angle) * viewport_v + sin(vertical_angle) * cross(v_axis, viewport_v) + (1 - cos(vertical_angle)) * dot(v_axis, viewport_v) * v_axis;
        pixel_delta_v = viewport_v / image_height;
    }

    point3 get_center() const {
        return center;
    }

    point3 get_pixel00_loc() const {
        return pixel00_loc;
    }

    vec3 get_viewport_u() const {
        return viewport_u;
    }

    vec3 get_viewport_v() const {
        return viewport_v;
    }

  private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0 (top left corner)

    double focal_length;
    double viewport_height;
    double viewport_width;

    // the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u;
    vec3 viewport_v;

    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Determine viewport dimensions.
        focal_length = 1.0;
        viewport_height = 2.0;
        viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        viewport_u = vec3(viewport_width, 0, 0);
        viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = (samples_per_pixel > 1) * (-0.5 + random_double());
        auto py = (samples_per_pixel > 1) * (-0.5 + random_double());
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                if (MODE == "normals") {
                    vec3 N = rec.normal;
                    return 0.5*color(N.x()+1, N.y()+1, N.z()+1);
                }
                else if (MODE == "distances") {
                    float dist = (rec.p.length() - 1) / 2;
                    return color(dist, dist, dist);
                }
                else {
                    return attenuation * ray_color(scattered, depth-1, world);
                }
            return color(0,0,0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

    
    color ray_color_with_point_lights(const ray& r, int depth, const hittable& world, const std::vector<light>& lights) const {
        hit_record rec;
        vec3 intensity = {0, 0, 0};

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        if (world.hit(r, interval(0.001, infinity), rec)) {
            for (int i = 0; i < lights.size(); i++) {
                vec3 intensity_i = {1, 1, 1};

                light l = lights.at(i);
                vec3 light_ray = unit_vector(l.pos - rec.p);
                vec3 reflected_ray = 2.0 * dot(rec.normal, light_ray) * rec.normal - light_ray;

                double diffuse = max(0, dot(rec.normal, light_ray));
                double specular = SPECULAR_COEFFICIENT * max(0, dot(reflected_ray, unit_vector(center - rec.p)));
                intensity_i *=  diffuse + specular + AMBIENT_OCCLUSION;
                intensity_i = term_to_term_product(intensity_i, l.ray_color);

                intensity += intensity_i;
            }
            return term_to_term_product(intensity, rec.mat->get_material_color());
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);       
    }
};

#endif