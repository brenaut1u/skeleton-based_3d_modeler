#include "hittable.h"
#include "vec3.h"
#include "sphere.h"
#include "cone.h"
#include "material.h"
#include "color.h"

bool cone::hit(const ray& r, interval ray_t, hit_record& rec) const {
  bool selected = false;
  vec3 ro = r.origin();
  vec3 rd = unit_vector(r.direction());
  vec3 pa = center1;
  vec3 pb = center2;
  auto ra = radius1;
  auto rb = radius2;

  vec3 outward_normal;

  vec3  ba = pb - pa;
  vec3  oa = ro - pa;
  vec3  ob = ro - pb;
  float rr = ra - rb;
  float m0 = dot(ba,ba);
  float m1 = dot(ba,oa);
  float m2 = dot(ba,rd);
  float m3 = dot(rd,oa);
  float m5 = dot(oa,oa);
  float m6 = dot(ob,rd);
  float m7 = dot(ob,ob);

  float h1 = m3*m3 - m5 + ra*ra;
  float h2 = m6*m6 - m7 + rb*rb;

  float t;

    if ((ba.length() + ra <= rb || is_selected(2)) && h2 > 0.0) {
        t = -m6 - sqrt( h2 );
        outward_normal = (ob+t*rd)/rb;
        selected = is_selected(2);
    }
    else if ((ba.length() + rb <= ra  || is_selected(1)) && h1 > 0.0) {
        t = -m3 - sqrt( h1 );
        outward_normal = (oa+t*rd)/ra;
        selected = is_selected(1);
    }
    else {
        float d2 = m0 - rr * rr;

        float k2 = d2 - m2 * m2;
        float k1 = d2 * m3 - m1 * m2 + m2 * rr * ra;
        float k0 = d2 * m5 - m1 * m1 + m1 * rr * ra * 2.0 - m0 * ra * ra;

        float h = k1 * k1 - k0 * k2;
        if (h < 0.0) return false;
        t = (-sqrt(h) - k1) / k2;

        float y = m1 - ra * rr + t * m2;
        if (y > 0.0 && y < d2) {
            outward_normal = unit_vector(d2 * (oa + t * rd) - ba * y);
        } else {
            if (h1 < 0.0 && h2 < 0.0) return false;

            t = +infinity;
            if (h1 > 0.0) {
                t = -m3 - sqrt(h1);
                outward_normal = (oa + t * rd) / ra;
                selected = is_selected(1) || is_selected(3);
            }
            if (h2 > 0.0) {
                auto tmp_t = -m6 - sqrt(h2);
                if (tmp_t < t) {
                    t = tmp_t;
                    outward_normal = (ob + t * rd) / rb;
                    selected = is_selected(2) || is_selected(3);
                }
            }
        }
    }


    rec.t = t / r.direction().length();
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, unit_vector(outward_normal));
    rec.mat = mat;

    if (selected){
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

