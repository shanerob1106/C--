#include <pspctrl.h>
#include <pspdebug.h>
#include <chrono>
#include "gfx.hpp"
#include "object.hpp"

enum CollisionType
{
    LEFT,
    RIGHT,
    TOP,
    NONE
};

class Player
{
public:
    Player(int x, int y) : x(x), y(y), canJump(false), jumpHeight(0), jumpSpeed(6), gravity(1), velocityX(0), velocityY(0), maxSpeed(2), friction(0.9), width(20), height(20) {}

    // Player Size
    int width;
    int height;

    // Player Velocity & Movement
    float velocityX;
    float velocityY;
    float maxSpeed;
    float friction;

    void update(const SceCtrlData &pad, const Object &floor, const Object &roof, const Object &platform)
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

        // Only allow jumping if the player is colliding with the floor or platform
        if ((pad.Buttons & PSP_CTRL_CROSS) && (isCollidingWith(floor) || isCollidingWith(platform)) && canJump)
        {
            velocityY = -10;
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
        CollisionType platformCollision = isCollidingWith(platform);
        if (platformCollision != NONE)
        {
            if (platformCollision == TOP)
            {
                pspDebugScreenPrintf("PLAT TOP");
                y = floor.getY() - floor.getHeight();
                velocityY = 0.0f;
                canJump = true;
            }
            else if (platformCollision == RIGHT)
            {
                pspDebugScreenPrintf("PLAT RIGHT");
                x = floor.getX() + floor.getWidth();
                velocityX = 0.0f;
            }
            else if (platformCollision == LEFT)
            {
                pspDebugScreenPrintf("PLAT LEFT");
                x = floor.getX() - width;
                velocityX = 0.0f;
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
        // else if (floorCollision == RIGHT)
        // {
        //     pspDebugScreenPrintf("FLOOR RIGHT");
        //     x = floor.getX() + floor.getWidth();
        //     velocityX = 0.0f;
        // }
        // else if (floorCollision == LEFT)
        // {
        //     pspDebugScreenPrintf("FLOOR LEFT");
        //     x = floor.getX() - width;
        //     velocityX = 0.0f;
        // }
    };

    CollisionType isCollidingWith(const Object &other) const
    {
        if (x < other.getX() + other.getWidth() &&
            x + width > other.getX() &&
            y < other.getY() + other.getHeight() &&
            y + height > other.getY())
        {
            if (velocityY > 0)
            {
                return TOP;
            }
            else if (velocityX > 0)
            {
                return LEFT;
            }
            else if (velocityX < 0)
            {
                return RIGHT;
            }
            return NONE;
        }
        return NONE;
    }

    // Remaining functions...

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
    int jumpHeight;
    int jumpSpeed;
    int gravity;
};
