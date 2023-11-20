#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>


struct sphere {
    double rayon;
    std::array<double,2> center;
    

    void setCenter(double x1 , double x2){
        center[0] = x1;
        center[1] = x2;
    }

    double getx(){
        return center[0];
    }

    double gety(){
        return center[1];
    }

};

namespace nb = nanobind;

NB_MODULE(sphere, m) {
    nb::class_<sphere>(m, "sph")
        .def(nb::init<>())
        .def_rw("ray",&sphere::rayon)
        .def("x",&sphere::getx)
        .def("y",&sphere::gety)
        .def("setCenter", &sphere::setCenter)
        ;
}

struct scene {
    std::vector<sphere> listeSpheres;

    double nbSphere(){
        return listeSpheres.size();
    }

    void addSphere(sphere s){
        listeSpheres.insert(listeSpheres.cend(),s);
    }

};

NB_MODULE(scene, m) {
    nb::class_<scene>(m, "sce")
        .def(nb::init<>())
        .def("nbSphere",&scene::nbSphere)
        .def_rw("listeSpheres", &scene::listeSpheres)
        .def("addSphere",&scene::addSphere)
        ;
}

















