#include "gfx.hpp"

#ifndef OBJECT_HPP
#define OBJECT_HPP

class Object
{
public:
    Object(int x, int y, int height, int width) : x(x), y(y), width(width), height(height) {}

    int x, y, width, height;

    // Draw a rectangle
    void drawRect(int x, int y, int color) const
    {
        // Adjust the position by the offset
        GFX::drawRect(x, y, width, height, color);
    }

    // Get the position of the object
    int getY() const
    {
        return y;
    }

    int getX() const
    {
        return x;
    }

    // Get the size of the object
    int getHeight() const
    {
        return height;
    }

    int getWidth() const
    {
        return width;
    }
};

#endif // OBJECT_HPP