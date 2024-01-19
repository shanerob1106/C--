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

void floor(int x, int y)
{
    GFX::drawRect(x, y, 480, 20, 0x6AA84F);
}

class Player
{
public:
    Player(int x, int y) : x(x), y(y), isJumping(false), jumpPressed(false), jumpHeight(40), jumpSpeed(6), gravity(4) {}

    void update(const SceCtrlData &pad)
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
        else if (pad.Buttons & PSP_CTRL_RIGHT)
        {
            x++;
        }
        else if (pad.Buttons & PSP_CTRL_CROSS && !jumpPressed)
        {
            // Jumping
            if (!jumpPressed)
            {
                isJumping = true;
                jumpHeight = 40;
                jumpPressed = true;
            }
            else{
                jumpPressed = false;
            }
        }

        // Reset jumpPressed when the cross button is released
        if (!(pad.Buttons & PSP_CTRL_CROSS))
        {
            jumpPressed = false;
        }
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

    void draw()
    {
        GFX::drawRect(x + 10, y + 10, 20, 20, 0xFF0000); // Draw the head
    }

private:
    int x;
    int y;
    bool isJumping;
    bool jumpPressed;
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

    Player player(0, 0);
    int floorY = 220;
    GFX::init();

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        // Draw calls
        GFX::clear(0xFFFFCA82);

        // Environment (make calls between clear and swapBuffers)
        floor(0, 252);

        // Player
        player.update(pad);
        player.draw();

        if (isTouchingFloor(player, floorY))
        {
            player.setY(floorY);
        }

        if (isTouchingRoof(player, 0))
        {
            player.setY(0);
        }

        // Swap buffers
        GFX::swapBuffers();
        sceDisplayWaitVblankStart();
    }
}