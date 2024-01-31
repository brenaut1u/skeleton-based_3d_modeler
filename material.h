#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "color.h"
#include "hittable.h"
#include <vector>

using std::string;
using std::vector;
using std::pair;

#define SPECULAR_COEFFICIENT 0.2

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

    virtual color get_material_color() const = 0;
    virtual pair<string, vector<double>> descriptor() const = 0;
};

class lambertian : public material {
  public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override {
        string mat_type = "lambertian";
        vector<double> data = {albedo.x(), albedo.y(), albedo.z()};
        return {mat_type, data};
    }

  private:
    color albedo;
};

class metal : public material {
  public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        scattered = ray(rec.p, reflected + fuzz*random_unit_vector());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override {
        string mat_type = "metal";
        vector<double> data = {albedo.x(), albedo.y(), albedo.z(), fuzz};
        return {mat_type, data};
    }

  private:
    color albedo;
    double fuzz;
};

#endif