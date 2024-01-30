#ifndef CONE_H
#define CONE_H

#include "hittable.h"
#include "vec3.h"
#include "sphere.h"

class cone : public hittable {
  public:
    cone(point3 _center1, point3 _center2, double _radius1, double _radius2, shared_ptr<material> _material)
      : center1(_center1), center2(_center2), radius1(_radius1), radius2(_radius2), mat(_material) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
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
      
      float d2 = m0-rr*rr;
      
      float k2 = d2    - m2*m2;
      float k1 = d2*m3 - m1*m2 + m2*rr*ra;
      float k0 = d2*m5 - m1*m1 + m1*rr*ra*2.0 - m0*ra*ra;
      
      float h = k1*k1 - k0*k2;
      if(h < 0.0) return false;
      float t = (-sqrt(h)-k1)/k2;

      float y = m1 - ra*rr + t*m2;
      if( y>0.0 && y<d2 ) 
      {
          outward_normal = unit_vector( d2*(oa + t*rd)-ba*y);
      }

      else {
          float h1 = m3*m3 - m5 + ra*ra;
          float h2 = m6*m6 - m7 + rb*rb;
          if( h1 < 0.0 && h2 < 0.0 ) return false;
          
          t = +infinity;
          if( h1>0.0 )
          {        
            t = -m3 - sqrt( h1 );
            outward_normal = (oa+t*rd)/ra;
          }
          if( h2>0.0 )
          {
            auto tmp_t = -m6 - sqrt( h2 );
              if( tmp_t<t ) {
                t = tmp_t;
                outward_normal = (ob+t*rd)/rb;
                }
          }
      }
      
      if( t < 0.001 ) return false;
      
      rec.t = t / r.direction().length();
      rec.p = r.at(rec.t);
      rec.set_face_normal(r, unit_vector(outward_normal));
      rec.mat = mat;

      return true;
    }

    void set_center1(point3 new_center) {
        center1 = new_center;
    }

    void move_center1(vec3 delta_center) {
        center1 += delta_center;
    }

    void set_center2(point3 new_center) {
        center2 = new_center;
    }

    void move_center2(vec3 delta_center) {
        center2 += delta_center;
    }

    void set_radius1(double new_radius) {
        radius1 = new_radius;
    }

    void set_radius2(double new_radius) {
        radius2 = new_radius;
    }

    void increase_radius1(double delta_radius) {
        radius1 += delta_radius;
    }

    void increase_radius2(double delta_radius) {
        radius2 += delta_radius;
    }

  private:
    point3 center1;
    point3 center2;
    double radius1;
    double radius2;
    shared_ptr<material> mat;
};

static shared_ptr<cone> cone_from_spheres(shared_ptr<sphere> sphere1, shared_ptr<sphere> sphere2, shared_ptr<material> material) {
  return make_shared<cone>(sphere1->get_center(), sphere2->get_center(), sphere1->get_radius(), sphere2->get_radius(), material);
}

#endif