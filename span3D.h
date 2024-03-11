#ifndef SPAN3D_H
#define SPAN3D_H

class span3D {
public:
    span3D(float* ptr, int sizeX_, int sizeY_, int sizeZ_)
        : data(ptr), sizeX(sizeX_), sizeY(sizeY_), sizeZ(sizeZ_) {}

    float& operator()(int x, int y, int z);

    const float& operator()(int x, int y, int z) const;

    int size() const { return sizeX * sizeY * sizeZ; }

private:
    float* data;
    int sizeX;
    int sizeY;
    int sizeZ;
};

#endif