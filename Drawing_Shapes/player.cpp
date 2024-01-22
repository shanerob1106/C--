// PSP headers
#include <pspctrl.h>
#include <pspdebug.h>

// #include custom headers
#include "gfx.hpp"
#include "object.hpp"

// STL includes
#include <algorithm>
#include <vector>

enum CollisionType
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    NONE
};

class Player
{
public:
    Player(int x, int y) : x(x), y(y), canJump(false), gravity(0.5), velocityX(0), velocityY(0), maxSpeed(2), friction(0.9), width(20), height(20) {}

    // Player Size
    int width;
    int height;

    // Player Velocity & Movement
    float velocityX;
    float velocityY;
    float maxSpeed;
    float friction;

    void update(const SceCtrlData &pad, const Object &floor, const Object &roof, const std::vector<Object> &platforms)
    {
        // Player controller
        if (pad.Buttons & PSP_CTRL_LEFT)
        {
            velocityX = -maxSpeed;
        }
        else if (pad.Buttons & PSP_CTRL_RIGHT)
        {
            velocityX = maxSpeed;
        }
        else
        {
            velocityX = 0;
        }

        velocityY += gravity;

        bool onPlatform = false;
        for (const auto &platform : platforms)
        {
            if (isCollidingWith(platform))
            {
                onPlatform = true;
                break;
            }
        }

        // Only allow jumping if the player is colliding with the floor or platform
        if ((pad.Buttons & PSP_CTRL_CROSS) && ((isCollidingWith(floor) || onPlatform)) && canJump)
        {
            velocityY = -6;
            canJump = false;
        }

        // // Reset jumpPressed when the cross button is released
        if (!(pad.Buttons & PSP_CTRL_CROSS))
        {
            canJump = false;
        }

        x += velocityX;
        y += velocityY;

        // Collision detection (platform)
        for (const auto &platform : platforms)
        {
            CollisionType platformCollision = isCollidingWith(platform);
            if (platformCollision != NONE)
            {
                if (platformCollision == TOP)
                {
                    pspDebugScreenPrintf("PLAT TOP");
                    y = platform.getY() - platform.getHeight();
                    velocityY = 0.0f;
                    canJump = true;
                }
                else if (platformCollision == RIGHT)
                {
                    pspDebugScreenPrintf("PLAT RIGHT");
                    x = platform.getX() + platform.getWidth();
                    velocityX = 0.0f;
                }
                else if (platformCollision == LEFT)
                {
                    pspDebugScreenPrintf("PLAT LEFT");
                    x = platform.getX() - platform.getWidth();
                    velocityX = 0.0f;
                }
                else if (platformCollision == BOTTOM)
                {
                    pspDebugScreenPrintf("PLAT BOTTOM");
                    y = platform.getY() + platform.getHeight();
                    velocityY = 0.0f;
                }
            }
        }

        CollisionType floorCollision = isCollidingWith(floor);

        if (floorCollision == TOP)
        {
            pspDebugScreenPrintf("FLOOR TOP");
            y = floor.getY() - floor.getHeight();
            velocityY = 0.0f;
            canJump = true;
        }
    };

    CollisionType isCollidingWith(const Object &other) const
    {
        float xCenter = x + 0.5f * width;
        float yCenter = y + 0.5f * height;

        float otherXCenter = other.getX() + 0.5f * other.getWidth();
        float otherYCenter = other.getY() + 0.5f * other.getHeight();

        float xDist = std::abs(xCenter - otherXCenter);
        float yDist = std::abs(yCenter - otherYCenter);

        float combinedHalfWidths = 0.5f * (width + other.getWidth());
        float combinedHalfHeights = 0.5f * (height + other.getHeight());

        if (xDist < combinedHalfWidths && yDist < combinedHalfHeights)
        {
            float overlapX = combinedHalfWidths - xDist;
            float overlapY = combinedHalfHeights - yDist;

            if (overlapX < overlapY)
            {
                // Collision on the left or right side
                return xCenter < otherXCenter ? LEFT : RIGHT;
            }
            else
            {
                // Collision on the top or bottom side
                return yCenter < otherYCenter ? TOP : BOTTOM;
            }
        }

        return NONE;
    }

    int getY()
    {
        return y;
    }

    int getX()
    {
        return x;
    }

    float getVelocityX() const
    {
        return velocityX;
    }

    float getVelocityY() const
    {
        return velocityY;
    }

    void setVelocityX(float newVelocityX)
    {
        velocityX = newVelocityX;
    }

    void setVelocityY(float newVelocityY)
    {
        velocityY = newVelocityY;
    }

    bool getCanJump() const
    {
        return canJump;
    }

    void setY(int newY);

    void setX(int newX);

    void draw(int x, int y)
    {
        GFX::drawRect(x, y, 20, 20, 0xFF0000); // Draw the head
    }

private:
    int x;
    int y;
    bool canJump;
    float gravity;
};
