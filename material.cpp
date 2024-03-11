#include "material.h"

bool lambertian::scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
}

pair<string, vector<double>> lambertian::descriptor() const {
    string mat_type = "lambertian";
    vector<double> data = {albedo.x(), albedo.y(), albedo.z()};
    return {mat_type, data};
}

bool metal::scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected);
    scattered = ray(rec.p, reflected + fuzz*random_unit_vector());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
}

pair<string, vector<double>> metal::descriptor() const {
    string mat_type = "metal";
    vector<double> data = {albedo.x(), albedo.y(), albedo.z(), fuzz};
    return {mat_type, data};
}