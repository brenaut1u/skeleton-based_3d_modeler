#ifndef SPAN3D_H
#define SPAN3D_H

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <iostream>


class span3D {
public:
    span3D(float* ptr, int sizeX_, int sizeY_, int sizeZ_)
        : data(ptr), sizeX(sizeX_), sizeY(sizeY_), sizeZ(sizeZ_) {}

    float& operator()(int x, int y, int z) {
        if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
            //std::cout<<x<<"   "<<y<<"   "<<z<<"/n";
            throw std::runtime_error("");

        };
        //assert((x * sizeY + y ) * sizeX + z < sizeX * sizeY * sizeZ);
        return data[ (x * sizeY + y ) * sizeZ + z ];
    }

    const float& operator()(int x, int y, int z) const {
        if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
            //std::cout<<x<<"   "<<y<<"   "<<z<<"/n";
            throw std::runtime_error("");
        };
        return data[ ( x * sizeY + y ) * sizeZ + z ];
    }

    int size() const { return sizeX * sizeY * sizeZ; }

private:
    float* data;
    int sizeX;
    int sizeY;
    int sizeZ;
};

#endif