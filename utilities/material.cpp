#include "material.h"

bool lambertian::scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
    // Creates a new scattered ray that follows the normal, while deviating a little bit, randomly.
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
}

pair<string, vector<double>> lambertian::descriptor() const {
    // Returns the material's type name ("lambertian"), then the material's color components (red, green, blue)
    string mat_type = "lambertian";
    vector<double> data = {albedo.x(), albedo.y(), albedo.z()};
    return {mat_type, data};
}

bool metal::scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
    // Creates a new scattered ray that follows the normal, while deviating a little bit, randomly.
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = ray(rec.p, reflected);
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
}

pair<string, vector<double>> metal::descriptor() const {
    // Returns the material's type name ("metal"), then the material's color components and fuzz (red, green, blue, fuzz)
    string mat_type = "metal";
    vector<double> data = {albedo.x(), albedo.y(), albedo.z(), fuzz};
    return {mat_type, data};
}

shared_ptr<material> blend_materials(const shared_ptr<material>& mat1, const shared_ptr<material>& mat2, double t) {
    // Blends two materials together, with proportions depending on t.
    // t = 0 means the resulting material will be mat1, and t = 1 means mat2.
    // If the two materials have different types (metal and lambertian), the resulting material will be a metal
    // with a blended fuzz between 1 and the metallic material's fuzz.
    // This function doesn't consider unlit materials, as they are used only in specific cases that do not involve this
    // function.

    shared_ptr<material> mat;

    auto mat1_desc = mat1->descriptor();
    auto mat2_desc = mat2->descriptor();

    color c = color((1 - t) * mat1_desc.second[0] + t * mat2_desc.second[0],
                    (1 - t) * mat1_desc.second[1] + t * mat2_desc.second[1],
                    (1 - t) * mat1_desc.second[2] + t * mat2_desc.second[2]);

    if (mat1_desc.first == "metal") {
        if (mat2_desc.first == "metal") {
            mat = make_shared<metal>(c, (1 - t) * mat1_desc.second[3] + t * mat2_desc.second[3]);
        }
        else {
            mat = make_shared<metal>(c, (1 - t) * mat1_desc.second[3] + t);
        }
    }
    else {
        if (mat2_desc.first == "metal") {
            mat = make_shared<metal>(c, (1 - t) + t * mat2_desc.second[3]);
        }
        else {
            mat = make_shared<lambertian>(c);
        }
    }

    return mat;
}

shared_ptr<material> copy_material(const shared_ptr<material>& mat) {
    // Returns a copy of the material
    auto mat_descr = mat->descriptor();
    if (mat_descr.first == "lambertian") {
        return make_shared<lambertian>(mat->get_material_color());
    }
    else if (mat_descr.first == "metal") {
        return make_shared<metal>(mat->get_material_color(), mat_descr.second[3]);
    }
    else if (mat_descr.first == "unlit") {
        return make_shared<unlit>(mat->get_material_color());
    }
    return nullptr;
}

shared_ptr<material> copy_material(const shared_ptr<material>& mat, const color& color) {
    // Returns a copy of the material while assigning it the desired color
    auto mat_descr = mat->descriptor();
    if (mat_descr.first == "lambertian") {
        return make_shared<lambertian>(color);
    }
    else if (mat_descr.first == "metal") {
        return make_shared<metal>(color, mat_descr.second[3]);
    }
    else if (mat_descr.first == "unlit") {
        return make_shared<unlit>(color);
    }
    return nullptr;
}