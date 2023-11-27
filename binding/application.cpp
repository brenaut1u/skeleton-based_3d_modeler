#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

namespace nb = nanobind;


struct sphere {
    int rayon;
    int x;
    int y;
    

    void setCenter(int x1 , int x2){
        x = x1;
        y = x2;
    }

    int getx(){
        return x;
    }

    int gety(){
        return y;
    }

};


struct scene {
    std::vector<sphere> listeSpheres;

    int nbSphere(){
        return listeSpheres.size();
    }

    void addSphere(sphere s){
        listeSpheres.push_back(s);
    }

    sphere getsphere(int i){
        return listeSpheres[i];
    }
    
};


NB_MODULE(modeler, m) {
    nb::class_<sphere>(m,"sphere")
        .def(nb::init<>())
        .def_rw("ray",&sphere::rayon)
        .def("x",&sphere::getx)
        .def("y",&sphere::gety)
        .def("setCenter", &sphere::setCenter)
        ;
    nb::class_<scene>(m, "sce")
        .def(nb::init<>())
        .def("nbSphere",&scene::nbSphere)
        .def_rw("listeSpheres", &scene::listeSpheres)
        .def("addSphere",&scene::addSphere)
        .def("getSphere",&scene::getsphere)
        ;
}













