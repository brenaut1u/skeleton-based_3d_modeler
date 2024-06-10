#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include "span3D.h"

/** 
 * This class is a wrapper for a 3D array of floats. 
 * It is used to store the 3 channels of color of an image.
 * We needed to create it because the std::span class does not support 3D arrays,
 * and we needed to pass a 3D array to the save_image function.
 * The binding coudln't transfert a numpy array with dimension (n,m,3) to a span.
 **/

// We return the data at the position (x,y,z)
float& span3D::operator()(int x, int y, int z) {
    if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
        throw std::runtime_error("");

    };
    // We need to calculta precisly wich element of the 1D array we want to access
    // We use the formula (x * sizeY + y ) * sizeZ + z
    // x is the first dimension, so we need to multiplicate it by the size of the 2nd and 3rd dimension
    // y is the second dimension, so we need to multiplicate it by the size of the 3rd dimension
    // z is the third dimension, we do not need to multiplicate it by anything
    return data[ (x * sizeY + y ) * sizeZ + z ];
}

const float& span3D::operator()(int x, int y, int z) const {
    if (!((x * sizeY + y ) * sizeZ + z < sizeX * sizeY * sizeZ)){
        throw std::runtime_error("");
    };
    return data[ ( x * sizeY + y ) * sizeZ + z ];
}