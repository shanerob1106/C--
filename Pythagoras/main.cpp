// PSP Include
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <cmath>

// C++ Include
#include <cstdlib>

#define sqrt(x) __builtin_sqrt(x)

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

float mySqrt(float x) {
    if (x < 0.0f) {
        // Handle negative input, if needed
        return 0.0f; // You may want to return an error code or NaN
    }

    // Initial guess
    float guess = x / 2.0f;

    // Newton's method for improving the guess
    for (int i = 0; i < 10; ++i) {
        guess = 0.5f * (guess + x / guess);
    }

    return guess;
}

auto main() -> int
{
    setupCallbacks();
    pspDebugScreenInit();
    sceCtrlGetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SceCtrlData pad;
    int a = 0;
    bool selectedA = false;

    int b = 0;
    bool selectedB = false;

    pspDebugScreenPrintf("Use the D-Pad to select a value. \n");
    pspDebugScreenPrintf("Press X to select A. \n");
    pspDebugScreenPrintf("Press O to select B. \n");

    while (selectedA == false)
    {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_CROSS)
        {
            selectedA = true;
            pspDebugScreenClear();
            pspDebugScreenPrintf("Selected %d as A. \n", a);
        }
        if (pad.Buttons & PSP_CTRL_UP)
        {
            a++;
            pspDebugScreenClear();
            pspDebugScreenPrintf("A: %d\n", a);
        }
        if (pad.Buttons & PSP_CTRL_DOWN)
        {
            a--;
            pspDebugScreenClear();
            pspDebugScreenPrintf("A: %d\n", a);
        }
    }

    pspDebugScreenClear();
    pspDebugScreenPrintf("B: %d\n", b);

    while (selectedB == false)
    {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_CIRCLE)
        {
            selectedB = true;
            pspDebugScreenClear();
            pspDebugScreenPrintf("Selected %d as B. \n", b);
        }
        if (pad.Buttons & PSP_CTRL_UP)
        {
            b++;
            pspDebugScreenClear();
            pspDebugScreenPrintf("B: %d\n", b);
        }
        if (pad.Buttons & PSP_CTRL_DOWN)
        {
            b--;
            pspDebugScreenClear();
            pspDebugScreenPrintf("B: %d\n", b);
        }
    }

    if (selectedA == true && selectedB == true)
    {
        float c = (a * a) + (b * b);
        pspDebugScreenClear();
        pspDebugScreenPrintf("Square root a*a + b*b is C: %f", mySqrt(c));
    }
}