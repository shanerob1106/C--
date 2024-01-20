#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include "gfx.hpp"

PSP_MODULE_INFO("Hello World", 0, 1, 1);

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int callbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

void setupCallbacks()
{
    int thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, NULL);
    if (thid >= 0)
    {
        sceKernelStartThread(thid, 0, NULL);
    }
}

class Object
{
public:
    Object(int x, int y, int height, int width) : x(x), y(y), width(width), height(height) {}

    int x, y, width, height;

    // Draw a rectangle
    void drawRect(int x, int y, int color)
    {
        // Adjust the position by the offset
        GFX::drawRect(x, y, width, height, color);
    }

    // Get the position of the object
    int getY()
    {
        return y;
    }

    int getX()
    {
        return x;
    }
};

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
    Player(int x, int y) : x(x), y(y), isJumping(false), jumpPressed(false), jumpHeight(40), jumpSpeed(6), gravity(4) {}

    int width = 20;
    int height = 20;

    void update(const SceCtrlData &pad, const Object &floor, const Object &roof, const Object &platform)
    {
        if (isJumping)
        {
            y -= jumpSpeed;          // Move the person upwards
            jumpHeight -= jumpSpeed; // Decrease the jump height

            if (jumpHeight <= 0)
            {
                isJumping = false; // Stop jumping when the jump height reaches 0
            }
        }
        else
        {
            y += gravity; // Apply gravity to the y-coordinate
        }

        // Player controller
        if (pad.Buttons & PSP_CTRL_LEFT)
        {
            x--;
        }
        if (pad.Buttons & PSP_CTRL_RIGHT)
        {
            x++;
        }
        if (pad.Buttons & PSP_CTRL_CROSS && !jumpPressed)
        {
            // Only allow jumping if the player is colliding with the floor or platform
            if ((isCollidingWith(floor) || isCollidingWith(platform)) && !isJumping)
            {
                isJumping = true;
                jumpHeight = 40;
                jumpPressed = true;
            }
            else
            {
                jumpPressed = false;
            }
        }

        // Reset jumpPressed when the cross button is released
        if (!(pad.Buttons & PSP_CTRL_CROSS))
        {
            jumpPressed = false;
        }
    }

    bool isCollidingWith(const Object &other) const
    {

        if (isColliding)
        {
            if (y > 0)
            {
                return TOP;
            }
            else if (movingLeft)
            {
                return LEFT;
            }
            else if (movingRight)
            {
                return RIGHT;
            }
            return NONE;
        }

        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }

    int getY()
    {
        return y;
    }

    int getX()
    {
        return x;
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
    bool isJumping;
    bool jumpPressed;
    bool isColliding;

    bool movingRight;
    bool movingLeft;

    int jumpHeight;
    int jumpSpeed;
    int gravity;
};

bool isTouchingFloor(Player player, int floorY)
{
    // Check if the player's y-coordinate is equal to the floor's y-coordinate
    if (player.getY() >= floorY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isTouchingRoof(Player player, int roofY)
{
    // Check if the player's y-coordinate is equal to the roof's y-coordinate
    if (player.getY() <= roofY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Player::setY(int newY)
{
    y = newY;
}

void Player::setX(int newX)
{
    x = newX;
}

auto main() -> int
{
    setupCallbacks();
    sceCtrlGetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    Player player(0, 0); // Create a player object
    Object floor(0, 230, 22, 480);
    Object roof(0, 0, 22, 480);

    Object platform(100, 214, 20, 20);

    GFX::init();

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        // Draw calls
        GFX::clear(0xFFFFCA82);

        // Environment (make calls between clear and swapBuffers)
        floor.drawRect(floor.x, floor.getY() + floor.height, 0x6AA84F);
        platform.drawRect(platform.x, platform.getY() + platform.height, 0xCC0000);

        // Player
        player.update(pad, floor, roof, platform);
        player.draw(player.getX(), player.getY() + player.height);

        // Collision detection (floor)
        if (player.isCollidingWith(floor))
        {
            player.setY(floor.getY() - player.height);
        }

        // Collision detection (platform)
        if (player.isCollidingWith(platform))
        {
            player.setY(platform.getY() - player.height);
        }
        // else if (player.isCollidingWith(platform) == LEFT)
        // {
        //     player.setX(platform.getX() - player.width);
        // }
        // else if (player.isCollidingWith(platform) == RIGHT)
        // {
        //     player.setX(platform.getX() + platform.width);
        // }

        // Collision detection (roof)
        if (player.isCollidingWith(roof))
        {
            player.setY(roof.getY() + roof.height);
        }

        // Swap buffers
        GFX::swapBuffers();
        sceDisplayWaitVblankStart();
    }
}