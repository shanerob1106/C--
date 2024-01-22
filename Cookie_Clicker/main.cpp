// PSP Include
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>

// C++ STL Include
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

PSP_MODULE_INFO("Cookie Clicker", 0, 1, 1);

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

class Upgrade
{
public:
    int numBought;
    double baseCost;
    double costMultiplier;
    double effect;

    Upgrade(double _baseCost, double _costMultiplier, double _effect)
        : numBought(0), baseCost(_baseCost), costMultiplier(_costMultiplier), effect(_effect) {}

    double getCurrentCost() const
    {
        return baseCost * power(costMultiplier, numBought);
    }

    // Simple power function
    double power(double base, int exponent) const
    {
        double result = 1.0;
        for (int i = 0; i < exponent; ++i)
        {
            result *= base;
        }
        return result;
    }

    bool canPurchase(int &cookies) const
    {
        return cookies >= getCurrentCost();
    }

    void purchase(int &cookies)
    {
        if (canPurchase(cookies))
        {
            cookies -= getCurrentCost();
            numBought++;
        }
    }
};

// Upgrades
Upgrade clickerUpgrade(10.0, 1.2, 1.0);           // Initial cost: 10.0, Multiplier: 1.2, Effect: 1.0
Upgrade autoClickerUpgrade(50.0, 1.5, 0.5);       // Initial cost: 50.0, Multiplier: 1.5, Effect: 0.5
Upgrade randomMultiplierUpgrade(100.0, 2.0, 0.0); // Initial cost: 100.0, Multiplier: 2.0, Effect: 0.0

// Function to handle the upgrade clicker logic
void upgradeClicker(int &cookies)
{
    clickerUpgrade.purchase(cookies);
}

// Function to handle the auto-clicker logic
void upgradeAutoClicker(int &cookies)
{
    autoClickerUpgrade.purchase(cookies);
}

// Function to handle the random 5-second multiplier chance upgrade logic
void upgradeRandomMultiplier(int &cookies)
{
    randomMultiplierUpgrade.purchase(cookies);
}

// Handles auto-clicker logic
void handleAutoClicker(int &cookies)
{
    static const int autoClickerBaseRate = 1; // Cookies per second per auto-clicker
    static double autoClickerTimeElapsed = 0.0;

    autoClickerTimeElapsed += 0.1;

    if (autoClickerTimeElapsed >= 1.0)
    {
        cookies += (autoClickerUpgrade.numBought * autoClickerBaseRate);
        autoClickerTimeElapsed -= 1.0; // Deduct the elapsed time
    }
}

void handleHeader(int &cookies)
{
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("                                                                       ");
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("Cookies: %d", cookies);
    pspDebugScreenPrintf("\nAuto-clickers: %d", autoClickerUpgrade.numBought);
}

// Save game state to a file
void saveGame(const int &cookies)
{
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open())
    {
        saveFile << cookies << "\n";
        saveFile << clickerUpgrade.numBought << "\n";
        saveFile << autoClickerUpgrade.numBought << "\n";
        saveFile << randomMultiplierUpgrade.numBought << "\n";
        saveFile.close();
    }
}

// Load game state from a file
void loadGame(int &cookies)
{
    std::ifstream saveFile("savegame.txt");
    if (saveFile.is_open())
    {
        saveFile >> cookies;
        saveFile >> clickerUpgrade.numBought;
        saveFile >> autoClickerUpgrade.numBought;
        saveFile >> randomMultiplierUpgrade.numBought;
        saveFile.close();
    }
}

auto main() -> int
{
    // DONT TOUCH THIS
    setupCallbacks();
    pspDebugScreenInit();
    sceCtrlGetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SceCtrlData pad;

    // Cookie variables
    int cookies = 0;
    bool crossPressedLastFrame = false;
    bool menuPressedLastFrame = false;
    bool menuOpen = false;

    loadGame(cookies);

    // Main loop
    while (true)
    {
        sceCtrlReadBufferPositive(&pad, 1);
        handleAutoClicker(cookies);

        // Add cookies
        if (pad.Buttons & PSP_CTRL_CROSS && !crossPressedLastFrame)
        {
            // Increment the cookies based on the clickerUpgrade effect
            cookies += static_cast<int>(clickerUpgrade.effect * (1 + clickerUpgrade.numBought));

            // Apply upgrade effects (double, triple, quadruple)
            if (clickerUpgrade.numBought > 0)
            {
                cookies += clickerUpgrade.numBought;
            }
        }

        crossPressedLastFrame = (pad.Buttons & PSP_CTRL_CROSS) != 0;

        // Print cookies
        handleHeader(cookies);

        // Open/Close menu
        if (pad.Buttons & PSP_CTRL_START && !menuPressedLastFrame)
        {
            menuPressedLastFrame = true;

            if (!menuOpen)
            {
                menuOpen = true;
                pspDebugScreenClear();
            }
            else
            {
                menuOpen = false;
                pspDebugScreenClear();
            }
        }
        else if (!(pad.Buttons & PSP_CTRL_START))
        {
            menuPressedLastFrame = false;
        }

        if (menuOpen)
        {
            pspDebugScreenClear();
            pspDebugScreenSetXY(0, 3);

            handleHeader(cookies);

            pspDebugScreenPrintf("\n\nShop Open!\n");

            pspDebugScreenPrintf("\nPress SQUARE to buy clicker upgrade!");
            pspDebugScreenPrintf("\nPress TRIANGLE to buy auto-clicker upgrade!");
            pspDebugScreenPrintf("\nPress CIRCLE to buy random multiplier upgrade!\n");
            pspDebugScreenPrintf("\nSave game by pressing L!");
            pspDebugScreenPrintf("\nOpen game by pressing R!");

            pspDebugScreenPrintf("\nUpgrade clicker cost: %.0f", clickerUpgrade.getCurrentCost());
            pspDebugScreenPrintf("\nUpgrade auto-clicker cost: %.0f", autoClickerUpgrade.getCurrentCost());
            pspDebugScreenPrintf("\nUpgrade random multiplier cost: %.0f", randomMultiplierUpgrade.getCurrentCost());

            if (pad.Buttons & PSP_CTRL_SQUARE)
            {
                // Buy clicker logic
                upgradeClicker(cookies);
            }

            if (pad.Buttons & PSP_CTRL_TRIANGLE)
            {
                // Buy auto-clicker logic
                upgradeAutoClicker(cookies);
            }

            if (pad.Buttons & PSP_CTRL_CIRCLE)
            {
                // Buy random multiplier logic
                upgradeRandomMultiplier(cookies);
            }

            if (pad.Buttons & PSP_CTRL_LTRIGGER)
            {
                saveGame(cookies);
            }

            // Load game logic
            if (pad.Buttons & PSP_CTRL_RTRIGGER)
            {
                loadGame(cookies);
            }
        }
    }
}
