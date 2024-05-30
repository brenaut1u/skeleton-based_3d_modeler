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

shared_ptr<material> blend_materials(const shared_ptr<material>& mat1, const shared_ptr<material>& mat2, double t) {
    shared_ptr<material> mat;

    auto mat1_desc = mat1->descriptor();
    auto mat2_desc = mat2->descriptor();

    color c = color((1 - t) * mat1_desc.second[0] + t * mat2_desc.second[0],
                    (1 - t) * mat1_desc.second[1] + t * mat2_desc.second[1],
                    (1 - t) * mat1_desc.second[2] + t * mat2_desc.second[2]);

    if (mat1_desc.first == "metal") {
        if (mat2_desc.first == "metal") {
            mat = make_shared<metal>(c, (1 - t) * mat1_desc.second[4] + t * mat2_desc.second[4]);
        }
        else {
            mat = make_shared<metal>(c, (1 - t) * mat1_desc.second[4]);
        }
    }
    else {
        if (mat2_desc.first == "metal") {
            mat = make_shared<metal>(c, t * mat2_desc.second[4]);
        }
        else {
            mat = make_shared<lambertian>(c);
        }
    }

    return mat;
}