#include "hittable.h"
#include "vec3.h"
#include "sphere.h"
#include "cone.h"
#include "material.h"
#include "color.h"

inline constexpr double contour_thickness = 0.005;
inline const color contour_color = color(1.0, 1.0, 0.0);

bool cone::hit(const ray& r, interval ray_t, hit_record& rec, bool show_selec) const {
  // show_selec parameter determines whether we show the selection state in the hit_record
  bool hovered = false;
  bool contour = false;
  vec3 ro = r.origin();
  vec3 rd = unit_vector(r.direction());
  vec3 pa = center1;
  vec3 pb = center2;
  auto ra = show_selec && (is_selected(1) || is_hovered(1))
                    ? radius1 + contour_thickness * (center1 - r.origin()).length() : radius1;
  auto rb = show_selec && (is_selected(2) || is_hovered(2))
                    ? radius2 + contour_thickness * (center2 - r.origin()).length() : radius2;

  vec3 outward_normal;
  shared_ptr<material> mat;

  vec3  ba = pb - pa;
  vec3  oa = ro - pa;
  vec3  ob = ro - pb;
  float rr = radius1 - radius2;
  float m0 = dot(ba,ba);
  float m1 = dot(ba,oa);
  float m2 = dot(ba,rd);
  float m3 = dot(rd,oa);
  float m5 = dot(oa,oa);
  float m6 = dot(ob,rd);
  float m7 = dot(ob,ob);

  float h1 = m3*m3 - m5 + ra*ra;
  float h2 = m6*m6 - m7 + rb*rb;

  double t;

    // sphere contours
    if (show_selec && is_selected(2) && h2 > 0.0) {
        double t_tmp = -m6 - sqrt( h2 );

        vec3 r_dir = unit_vector(r.direction());
        point3 p = r.at(t_tmp / r.direction().length());
        vec3 v = p - center2;
        if (rb - (v - r_dir * dot(v, r_dir)).length() < contour_thickness * (center2 - r.origin()).length()) {
            contour = true;
            t = t_tmp;
            outward_normal = (ob + t_tmp * rd) / rb;
            mat = make_shared<unlit>(contour_color);
        }
        hovered = is_hovered(2);
    }
    else if (show_selec && is_selected(1) && h1 > 0.0) {
        double t_tmp = -m3 - sqrt( h1 );

        vec3 r_dir = unit_vector(r.direction());
        point3 p = r.at(t_tmp / r.direction().length());
        vec3 v = p - center1;
        if (ra - (v - r_dir * dot(v, r_dir)).length() < contour_thickness * (center1 - r.origin()).length()) {
            contour = true;
            t = t_tmp;
            outward_normal = (oa + t_tmp * rd) / ra;
            mat = make_shared<unlit>(contour_color);
        }
        hovered = is_hovered(1);
    }
    else if (is_hovered(2) && h2 > 0.0) {
        t = -m6 - sqrt( h2 );
        outward_normal = (ob+t*rd)/rb;
        mat = mat2;
        hovered = is_hovered(2);
    }
    else if ( is_hovered(1) && h1 > 0.0) {
        t = -m3 - sqrt( h1 );
        outward_normal = (oa+t*rd)/ra;
        mat = mat1;
        hovered = is_hovered(1);
    }

    if (!contour) {
        h1 = m3*m3 - m5 + radius1 * radius1;
        h2 = m6*m6 - m7 + radius2 * radius2;

        // if there is a sphere inside the other
        if (ba.length() + ra <= rb && h2 > 0.0) {
            t = -m6 - sqrt(h2);
            outward_normal = (ob + t * rd) / rb;
            mat = mat2;
            hovered = is_hovered(2);
        }
        else if (ba.length() + rb <= ra && h1 > 0.0) {
            t = -m3 - sqrt(h1);
            outward_normal = (oa + t * rd) / ra;
            mat = mat1;
            hovered = is_hovered(1);
        }

        else {
            float d2 = m0 - rr * rr;
            float k2 = d2 - m2 * m2;
            float k1 = d2 * m3 - m1 * m2 + m2 * rr * radius1;
            float k0 = d2 * m5 - m1 * m1 + m1 * rr * radius1 * 2.0 - m0 * radius1 * radius1;

            float h = k1 * k1 - k0 * k2;
            if (h < 0.0) return false;
            t = (-sqrt(h) - k1) / k2;

            float y = m1 - radius1 * rr + t * m2;

            // cone body
            if (y > 0.0 && y < d2) {
                outward_normal = unit_vector(d2 * (oa + t * rd) - ba * y);

                point3 p = r.at(t / r.direction().length());
                double u = lines_intersection(center1, center2 - center1, p, outward_normal);

                mat = blend_materials(mat1, mat2, u);

            }

            // spheres
            else {
                if (h1 < 0.0 && h2 < 0.0) return false;

                t = +infinity;
                if (h1 > 0.0) {
                    t = -m3 - sqrt(h1);
                    outward_normal = (oa + t * rd) / ra;
                    mat = mat1;
                    hovered = is_hovered(1);
                }
                if (h2 > 0.0) {
                    auto tmp_t = -m6 - sqrt(h2);
                    if (tmp_t < t) {
                        t = tmp_t;
                        outward_normal = (ob + t * rd) / rb;
                        mat = mat2;
                        hovered = is_hovered(2);
                    }
                }
            }
        }
    }
    if (t<0.001) return false;
    
    rec.t = t / r.direction().length();
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, unit_vector(outward_normal));
    rec.mat = mat;

    if (show_selec && hovered){
        auto descr = mat->descriptor();
        color new_color = rec.mat->get_material_color();
        new_color = negative(new_color);
        if (descr.first == "lambertian"){
            auto new_mat = make_shared<lambertian>(new_color);
            rec.mat = new_mat;
        }
        else if (descr.first == "metal"){
            auto new_mat = make_shared<metal>(new_color, descr.second[3]);
            rec.mat = new_mat;
        }
    }

    return true;
}