#include "sphere.h"

/**
 * This class represents spheres. While spheres do not appear dirrectly on screen (even single spheres are actually
 * cones with two identical spheres), the hit function is still used to detect a sphere at a given position on screen,
 * for selection for example.
 */

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const {
    vec3 ro = r.origin();
    vec3 rd = unit_vector(r.direction());
    vec3 ce = center;
    auto ra = radius;

    vec3 oc = ro - ce;
    float b = dot( oc, rd );
    vec3 qc = oc - b*rd;
    float h = ra*ra - dot( qc, qc );
    if( h<0.0 ) return false; // no intersection
    h = sqrt( h );
    auto root = -b-h;
    if (!ray_t.surrounds(root / r.direction().length())) {
         root = -b+h;
         if (!ray_t.surrounds(root / r.direction().length()))
             return false;
    }

    rec.t = root / r.direction().length();
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;

    return true;
}