#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "color.h"
#include "../scene_objects/hittable.h"
#include <vector>
#include <memory>

/**
 * Material classes (lambertian, metal and unlit) all derive from a virtual material class.
 * Materials have a base color, and react to rays by creating new ones that follow the normal's direction while deviating
 * a little, randomly.
 * Materials properties can be exported as a descriptor containing first the material type's name, and then the material's
 * properties, depending on the material's type (color, fuzz...)
 */

using std::string;
using std::vector;
using std::pair;

inline constexpr double specular_coefficient = 0.2;

class material {
    // Base class
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

    virtual color get_material_color() const = 0;
    virtual pair<string, vector<double>> descriptor() const = 0;

    //essayer la fonction clone -> ne marchait pas car pb avec shared / unique ou rien
};

class lambertian : public material {
    // An ideal matte material, with no reflexion.
  public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override;

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override;

  private:
    color albedo;
};

class metal : public material {
    // A metallic material, with more or less reflexion depending on the fuzz parameter: 0 means a perfect reflexion,
    // like a mirror, and 1 means the reflexion will be highly blurry

  public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override;

        color get_material_color() const override {return albedo;}

        pair<string, vector<double>> descriptor() const override;

  private:
    color albedo;
    double fuzz;
};

class unlit : public material {
    // This material is not affected by light and shadow: it always appears on screen with the same color.
public:
    unlit(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {return true;}

    color get_material_color() const override {return albedo;}

    pair<string, vector<double>> descriptor() const override {
        return {"unlit", {albedo.x(), albedo.y(), albedo.z()}};
    }

private:
    color albedo;
};

shared_ptr<material> blend_materials(const shared_ptr<material>& mat1, const shared_ptr<material>& mat2, double t);

shared_ptr<material> copy_material(const shared_ptr<material>& mat);

shared_ptr<material> copy_material(const shared_ptr<material>& mat, const color& color);

#endif