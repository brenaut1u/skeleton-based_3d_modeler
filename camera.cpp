#include <future>
//#include <thread>
#include "draw.h"
#include "camera.h"

//const auto processor_count = std::thread::hardware_concurrency();

void camera::render_file(const hittable_list& world) {
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

std::vector<vec3> camera::render(const hittable_list& world) {
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

void camera::render_phong_file(const hittable_list& world, const std::vector<light>& lights) {
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

std::vector<vec3> camera::render_phong(const hittable_list& world, const std::vector<light>& lights) {
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

void camera::computePhong_partial(const hittable_list& world, const std::vector<light>& lights, span3D image, int start_x, int end_x, int start_y, int end_y) {
    for (int j = start_y; j < end_y; ++j) {
        for (int i = start_x; i < end_x; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color_with_point_lights(r, max_depth, world, lights);
            }
            color_pixel(image, {i, j}, pixel_color);
        }
    }
}

void camera::computePhong(const hittable_list& world, const std::vector<light>& lights, span3D image) {
//    std::cout << processor_count << std::endl;
//
//    for (int i = 0; i < processor_count; i++) {
//        std::future<void> task = std::async(&camera::computePhong_partial, this, world, lights, image, 0, (int) (((float) i / (processor_count - 1)) * image_width), 0, image_height);
//    }
// First attempt to create as many threads as there are cores, but then the render is VERY slow...

//    std::future<void> task1 = std::async(&camera::computePhong_partial, this, world, lights, image, 0, image_width / 2, 0, image_height / 2);
//    std::future<void> task2 = std::async(&camera::computePhong_partial, this, world, lights, image, image_width / 2, image_width, 0, image_height / 2);
//    std::future<void> task3 = std::async(&camera::computePhong_partial, this, world, lights, image, image_width / 2, image_width, image_height / 2, image_height);
//    std::future<void> task4 = std::async(&camera::computePhong_partial, this, world, lights, image, 0, image_width / 2, image_height / 2, image_height);

    std::future<void> task1 = std::async(&camera::computePhong_partial, this, world, lights, image, 0, image_width, 0, image_height);

}

void camera::computePhong(const hittable_list& world, const std::vector<light>& lights, span3D image, const vector<screen_segment>& skeleton) {
    computePhong(world, lights, image);

    for (const auto& segment : skeleton) {
        draw_line(image, segment.first, segment.second, skeleton_line_radius, skeleton_background_color, skeleton_border_color);

        color background_color = segment.first_selected ? skeleton_selected_background_color : skeleton_background_color;
        color border_color = segment.first_selected ? skeleton_selected_border_color : skeleton_border_color;
        draw_circle(image, segment.first, skeleton_circle_radius, background_color, border_color);

        background_color = segment.second_selected ? skeleton_selected_background_color : skeleton_background_color;
        border_color = segment.second_selected ? skeleton_selected_border_color : skeleton_border_color;
        draw_circle(image, segment.second, skeleton_circle_radius, background_color, border_color);
    }
}

ray camera::get_ray(int i, int j) const {
    // Get a randomly sampled camera ray for the pixel at location i,j.

    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
}

void camera::rotate_camera(double horizontal_angle, double vertical_angle, point3 rot_center) {
    // horizontal rotation
    vec3 h_axis = vec3(0.0, 1.0, 0.0);
    center = point_rotation(center, rot_center, h_axis, horizontal_angle);
    pixel00_loc = point_rotation(pixel00_loc, rot_center, h_axis, horizontal_angle);
    viewport_u = vector_rotation(viewport_u, h_axis, horizontal_angle);
    viewport_v = vector_rotation(viewport_v, h_axis, horizontal_angle);
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // vertical rotation
    vec3 v_axis = unit_vector(viewport_u);
    center = point_rotation(center, rot_center, v_axis, vertical_angle);
    pixel00_loc = point_rotation(pixel00_loc, rot_center, v_axis, vertical_angle);
    viewport_v = vector_rotation(viewport_v, v_axis, vertical_angle);
    pixel_delta_v = viewport_v / image_height;
}

void camera::move_camera_sideways(double delta_pos_x, double delta_pos_y) {
    center += delta_pos_x * viewport_u + delta_pos_y * viewport_v;
    pixel00_loc += delta_pos_x * viewport_u + delta_pos_y * viewport_v;
}

void camera::move_camera_forward(double delta_pos) {
    point3 view_center = pixel00_loc + 0.5 * viewport_u + 0.5 * viewport_v;
    vec3 v = unit_vector(view_center - center);
    center += delta_pos * v;
    pixel00_loc += delta_pos * v;
}

void camera::start_beautiful_render(const hittable_list& world, span3D beautiful_image) {
    beautiful_render_ready = false;
    continue_beautiful_render = true;
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            std::cout << 100 * ((float) j * (float) image_width + (float) i) / ((float) image_height * image_width) << "%" << std::endl;
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel_beautiful_render; ++sample) {
                if (!continue_beautiful_render) {
                    return;
                }
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth_beautiful_render, world);
            }
            color_pixel(beautiful_image, {i, j}, pixel_color);
        }
    }
    beautiful_render_ready = true;
    std::cout << "Done." << std::endl;
}

void camera::initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = point3(0, 0, 0);

    // Determine viewport dimensions.
    focal_length = 1.5;
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

vec3 camera::pixel_sample_square() const {
    // Returns a random point in the square surrounding a pixel at the origin.
    auto px = (samples_per_pixel > 1) * (-0.5 + random_double());
    auto py = (samples_per_pixel > 1) * (-0.5 + random_double());
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

color camera::ray_color(const ray& r, int depth, const hittable& world) const {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, interval(0.001, infinity), rec)) {
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

color camera::ray_color_with_point_lights(const ray& r, int depth, const hittable& world, const std::vector<light>& lights) const {
    hit_record rec;
    vec3 intensity = {0, 0, 0};

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, interval(0.001, infinity), rec)) {
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