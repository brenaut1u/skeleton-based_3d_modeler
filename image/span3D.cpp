#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include "span3D.h"

float& span3D::operator()(int x, int y, int z) {
    if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
        //std::cout<<x<<"   "<<y<<"   "<<z<<"/n";
        throw std::runtime_error("");

    };
    //assert((x * sizeY + y ) * sizeX + z < sizeX * sizeY * sizeZ);
    return data[ (x * sizeY + y ) * sizeZ + z ];
}

const float& span3D::operator()(int x, int y, int z) const {
    if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
        //std::cout<<x<<"   "<<y<<"   "<<z<<"/n";
        throw std::runtime_error("");
    };
    return data[ ( x * sizeY + y ) * sizeZ + z ];
}