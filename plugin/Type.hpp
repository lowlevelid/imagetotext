#ifndef HEADER_TYPE_H
#define HEADER_TYPE_H

#include <iostream>

namespace itt 
{
namespace type 
{

typedef unsigned char Byte;

struct Color { 
    double R;
    double G;
    double B;
    double A;

    void Debug()
    {
        printf("{ %f, %f, %f, %f }\n", R, G, B, A);
    }

} c_rgb;

struct Vector2 { 
    ssize_t x; 
    ssize_t y; 
} position_t;

struct InspectData {
    size_t sizeKB;
    size_t colorChannelSize;
    size_t totalArrayElement;
};

} // type
} // itt

#endif