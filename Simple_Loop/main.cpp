#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>

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
    pspDebugScreenInit();
    sceCtrlGetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    int num = 0;
    bool play = false;
    SceCtrlData ctrlData;

    while (true)
    {
        sceCtrlReadBufferPositive(&ctrlData, 1);

        pspDebugScreenSetXY(0, 0);

        if (ctrlData.Buttons & PSP_CTRL_CROSS)
        {
            play = !play;
        }

        if (play)
        {
            pspDebugScreenPrintf("\nNumber: %d", num);
            num++;
        }
    }
}
