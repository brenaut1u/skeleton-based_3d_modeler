#ifndef COMMAND_H
#define COMMAND_H


#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "cone.h"







class command {
    
    void create_sphere(hittable_list world,point3 center , double radius){
        auto mat = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        world.add(make_shared<sphere>(center, 100.0, mat));
    }

    void apply_command(){
        


    }




}





#endif