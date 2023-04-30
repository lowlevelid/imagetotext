#ifndef HEADER_BASE_H
#define HEADER_BASE_H

#include <napi.h>
#include <jpeglib.h>
#include <vector>
#include <iostream>
#include "Type.hpp"

namespace itt 
{ 
namespace base 
{
    
class NBase
{
protected:
    uint32_t width;
    uint32_t height;
    uint32_t channel;
    size_t length;
    itt::type::Byte* data;
    std::vector<itt::type::Color> pixel_data;

public:
    NBase(uint32_t width, uint32_t height, uint32_t channel)
    {
        this->width = width;
        this->height = height;
        this->channel = channel;

        data = nullptr;
    }

    virtual ~NBase() = default;

    itt::type::Byte* Data() 
    {
        return data;
    }

    uint32_t Width() const
    {
        return width;
    }

    uint32_t Height() const
    {
        return height;
    }

    uint32_t Channel() const
    {
        return channel;
    }

    size_t Length() const
    {
        return length;
    }

    void Read(itt::type::Byte** data)
    {
        if (data != nullptr)
            this->data = *data;
    }

    // simple formula to take each 3 flat pixel data and put it to struct
    // https://play.golang.org/p/CntT9L67Dxy
    const std::vector<itt::type::Color>& PixelData()
    {
        if (data == nullptr)
            return pixel_data;

        for (int h = 0; h != height; h++) 
        {
            for (int w = 0; w != width; w++)
            {
                struct itt::type::Color c;
                c.R = data[(h*width*channel)+(w*channel)+0];
                c.G = data[(h*width*channel)+(w*channel)+1];
                c.B = data[(h*width*channel)+(w*channel)+2];

                if (channel == 4)
                    c.A = data[(h*width*channel)+(w*channel)+3];

                pixel_data.push_back(c);
            }

        }
        return pixel_data;
    }

    // http://en.wikipedia.org/wiki/Luma_%28video%29
    // basic grayscale filter
    std::vector<itt::type::Byte> ToGrayPixel()
    {
        auto p_data = PixelData();
        std::vector<itt::type::Byte> flat_pixel;
        
        for (int i = 0; i < p_data.size(); i++)
        {
            auto c = p_data[i];

            float red = (float) c.R * 0.299;
            float green = (float) c.G * 0.587;
            float blue = (float)c.B * 0.114;

            // gray pixel
            itt::type::Byte gray = (itt::type::Byte) (red + green + blue);

            flat_pixel.push_back(gray);
            flat_pixel.push_back(gray);
            flat_pixel.push_back(gray);
            if (channel == 4)
                flat_pixel.push_back((itt::type::Byte) c.A);
        }

        return flat_pixel;
    }
};

} // base
} // itt

#endif