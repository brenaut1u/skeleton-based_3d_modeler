#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "color.h"
#include "hittable.h"
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::pair;

inline constexpr double specular_coefficient = 0.2;

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

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override;

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override;

  private:
    color albedo;
};

class metal : public material {
  public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override;

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override;

  private:
    color albedo;
    double fuzz;
};

shared_ptr<material> blend_materials(const shared_ptr<material>& mat1, const shared_ptr<material>& mat2, double t);

#endif