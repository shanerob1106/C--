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

bool isJumping = false;
int jumpHeight = 50;
int jumpSpeed = 5;

void person(int x, int& y)
{
    static int gravity = 4; // Define the gravity value

    if (isJumping)
    {
        y -= jumpSpeed; // Move the person upwards
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

    GFX::drawRect(x + 10, y + 10, 20, 20, 0xFF0000); // Draw the head
}

auto main() -> int
{
    setupCallbacks();
    sceCtrlGetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;
    GFX::init();

    int x = 0;
    int y = 0;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if(y > 232){
            y= 232;
        }

        if (pad.Buttons & PSP_CTRL_LEFT)
        {
            x--;
        }
        else if (pad.Buttons & PSP_CTRL_RIGHT)
        {
            x++;
        }
        else if (pad.Buttons & PSP_CTRL_CROSS)
        {
            isJumping = true;
        }

        GFX::clear(0xFFFFCA82);
        GFX::drawRect(0, 252, 480, 20, 0x6AA84F);
        person(x, y);
        GFX::swapBuffers();
        
        sceDisplayWaitVblankStart();
    }
}