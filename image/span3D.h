#ifndef SPAN3D_H
#define SPAN3D_H

/** 
 * This class is a wrapper for a 3D array of floats, 
 *  we create a 1 dimension array that represent a 3D array.
 * It is used to store the 3 channels of color of an image.
 * We needed to create it because the std::span class does not support 3D arrays,
 * and we needed to pass a 3D array to the save_image function.
 * The binding coudln't transfert a numpy array with dimension (n,m,3) to a span.
 **/

class span3D {
public:
    // We store a pointer to the data, and the size of the 3 dimensions
    span3D(float* ptr, int sizeX_, int sizeY_, int sizeZ_)
        : data(ptr), sizeX(sizeX_), sizeY(sizeY_), sizeZ(sizeZ_) {}

    float& operator()(int x, int y, int z);

    const float& operator()(int x, int y, int z) const;

    // The size of the 3D array is the product of the 3 dimensions
    int size() const { return sizeX * sizeY * sizeZ; }

    // We return the size of each dimension of the 3D array
    // We don't need to return the size of the 3rd dimension, because it is always 3
    int size_X() const {return sizeX;}
    int size_Y() const {return sizeY;}

private:
    // We point to the data
    float* data;
    // We store the size of the 3D array
    int sizeX;
    int sizeY;
    int sizeZ;
};

#endif