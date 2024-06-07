#include "draw.h"
#include "camera.h"

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