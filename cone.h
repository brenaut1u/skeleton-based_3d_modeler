#ifndef CONE_H
#define CONE_H

#include "hittable.h"
#include "vec3.h"

class cone : public hittable {
  public:
    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _material)
      : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat(_material) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      vec3 ba = center2 - center1;
      vec3 oa = r.origin() - center1;
      vec3 ob = r.origin() - center2;
      double rr = radius1 - radius2;
      double m0 = dot(ba, ba);
      double m1 = dot(ba, oa);
      double m2 = dot(ba, r.direction());
      double m3 = dot(r.direction(), oa);
      double m5 = dot(oa, oa);
      double m6 = dot(ob, r.direction());
      double m7 = dot(ob, ob);

      double d2 = m0 - rr*rr;
      double k2 = d2 - m2*m2;
      double k1 = d2*m3 - m1*m2 + m2*rr*radius1;
      double k0 = d2*m5 - m1*m1 + m1*rr*radius1*2.0 - m0*radius1*radius1;
      double h = k1*k1 - k0*k2;
      double h1 = m3*m3 - m5 + radius1*radius1;
      double h2 = m6*m6 - m7 + radius2*radius2;
      double t = (-sqrt(h) - k1) / k2;
      double y = m1 - radius1*rr + t*m2;

      vec3 outward_normal;
      double rec_t;

      // body
      if (h < 0.0)
        return false;
      // else if (t<0.0)
      //   return false;
      else if (y > 0.0 && y < d2) {
          rec_t = t;
          outward_normal = unit_vector(d2*(oa + t*r.direction()) - ba*y);
      }

      // caps
      if (h1 < 0.0 && h2 < 0.0)
        return false;
      else
      {
        if (h1 > 0.0) {      
          rec_t = -m3 - sqrt(h1);
          outward_normal = (oa + t*r.direction()) / radius1;
        }
        if (h2 > 0.0) {
          double tmp = -m6 - sqrt(h2);
          if (tmp < rec_t) {
            rec_t = tmp;
            outward_normal = (ob + t*r.direction()) / radius2;
          }
          else
            return false;
        }
      }

      if (!ray_t.surrounds(rec_t))
                return false;

      rec.t = rec_t;
      rec.p = r.at(rec.t);
      rec.set_face_normal(r, outward_normal);
      rec.mat = mat;
      return true;
    }

  private:
    point3 center1;
    point3 center2;
    double radius1;
    double radius2;
    shared_ptr<material> mat;
};

#endif