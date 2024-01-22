// #include PSP headers
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>

// #include custom headers
#include "gfx.hpp"
#include "player.cpp"
#include "object.hpp"

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

auto main() -> int
{
    setupCallbacks();
    sceCtrlGetSamplingCycle(0);
    pspDebugScreenInit();
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    Player player(0, 0); // Create a player object
    Object floor(0, 230, 20, 480);
    Object roof(0, 0, 22, 480);

    std::vector<Object> platforms;
    platforms.push_back(Object(100, 200, 20, 20));
    platforms.push_back(Object(200, 150, 20, 20));

    GFX::init();

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        // Draw calls
        GFX::clear(0xFFFFCA82);

        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf("VelocityX: %.2f", player.getVelocityX());
        pspDebugScreenSetXY(0, 1);
        pspDebugScreenPrintf("VelocityY: %.2f", player.getVelocityY());
        pspDebugScreenSetXY(0, 2);
        pspDebugScreenPrintf("canJump: %d", player.getCanJump());

        player.update(pad, floor, roof, platforms);
        player.draw(player.getX(), player.getY() + player.height);

        // Environment (make calls between clear and swapBuffers)
        floor.drawRect(floor.getX(), floor.getY() + floor.getHeight(), 0x6AA84F);

        for (const auto &platform : platforms)
        {
            platform.drawRect(platform.getX(), platform.getY() + platform.getHeight(), 0xCC0000);
        }

        // Swap buffers
        GFX::swapBuffers();
        sceDisplayWaitVblankStart();
    }
}