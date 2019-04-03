/* Copyright (C) 2019  Adam Green (https://github.com/adamgreen)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
// Example to let a user directly control the WowWee CHiP robotic toy dog from a Mac over Bluetooth Low Energy, BLE.
//  It demonstrates the use of the CHiP C API.
#include <curses.h>
#include <stdio.h>
#include <string.h>
#include "chip.h"
#include "osxble.h"



// CHiP navigation commands are comprised of 3 components.
typedef struct DirectionCommand
{
    int forwardReverse;
    int leftRight;
    int spin;
} DirectionCommand;



// Function Prototypes
static void showCommands();
static void showBatteryLevel(CHiP* pCHiP);



int main(int argc, char *argv[])
{
    // Initialize the Core Bluetooth stack on this the main thread and start the worker robot thread to run the
    // code found in robotMain() below.
    osxCHiPInitAndRun();
    return 0;
}

void robotMain(void)
{
    // Connect to first CHiP robot discovered.
    CHiP* pCHiP = chipInit(NULL);
    chipConnectToRobot(pCHiP, NULL);

    // Initialize curses to get direct keyboard input for controlling CHiP.
    // Use halfdelay(1) to disable line buffering and only block 1/10th of a second waiting for next keypress.
    // Use noecho() to not echo each character to the console and keypad(*, TRUE) to enable arrow keys.
    WINDOW* pWindow = initscr();
    halfdelay(1);
    noecho();
    keypad(pWindow, TRUE);

    showCommands();
    showBatteryLevel(pCHiP);

    // Enter main loop where curses is used to get the most recent key press and then send the appropriate command,
    // if any, to the CHiP robot.
    bool quit = false;
    DirectionCommand prevDir = {0, 0, 0};
    while (!quit)
    {
        DirectionCommand currDir = {0, 0, 0};

        int key = getch();
        switch (key)
        {
        case 'q':
        case 'Q':
            quit = true;
            break;
        case KEY_UP:
            currDir.forwardReverse = prevDir.forwardReverse + 1;
            if (currDir.forwardReverse > 32)
            {
                currDir.forwardReverse = 32;
            }
            break;
        case KEY_DOWN:
            currDir.forwardReverse = prevDir.forwardReverse - 1;
            if (currDir.forwardReverse < -32)
            {
                currDir.forwardReverse = -32;
            }
            break;
        case KEY_LEFT:
            currDir.leftRight = prevDir.leftRight - 1;
            if (currDir.leftRight < -32)
            {
                currDir.leftRight = -32;
            }
            break;
        case KEY_RIGHT:
            currDir.leftRight = prevDir.leftRight + 1;
            if (currDir.leftRight > 32)
            {
                currDir.leftRight = 32;
            }
            break;
        case 'a':
        case 'A':
            currDir.spin = prevDir.spin - 1;
            if (currDir.spin < -32)
            {
                currDir.spin = -32;
            }
            break;
        case 'f':
        case 'F':
            currDir.spin = prevDir.spin + 1;
            if (currDir.spin > 32)
            {
                currDir.spin = 32;
            }
            break;
        case 's':
        case 'S':
            chipAction(pCHiP, CHIP_ACTION_SIT);
            break;
        case 'l':
        case 'L':
            chipAction(pCHiP, CHIP_ACTION_LIE_DOWN);
            break;
        case 'r':
        case 'R':
            chipAction(pCHiP, CHIP_ACTION_RESET);
            break;
        case 'b':
        case 'B':
            chipPlaySound(pCHiP, CHIP_SOUND_BARK_X1_CURIOUS_PLAYFUL_HAPPY_A34);
            break;
        case '?':
            showBatteryLevel(pCHiP);
            break;
        default:
            break;
        }

        if (currDir.forwardReverse != 0 || currDir.leftRight != 0 || currDir.spin != 0)
        {
            chipDrive(pCHiP, currDir.forwardReverse, currDir.leftRight, currDir.spin);
            prevDir = currDir;
        }
        else
        {
            memset(&prevDir, 0, sizeof(prevDir));
        }
    }

    // Cleanup on exit.
    endwin();
    chipUninit(pCHiP);
}


static void showCommands()
{
    printw("                     CHiP C API Demo\n\n");
    printw("                   Supported Commands\n");
    printw("-----------------------------------------------------------\n");
    printw("  Up Arrow - Drive Forward      Down Arrow - Drive Backward\n");
    printw("Left Arrow - Strafe to Left    Right Arrow - Strafe Right\n");
    printw("         A - Turn Left                   F - Turn Right\n");
    printw("S - Sit Down         L - Lie Down    R - Reset Pose    B - Bark\n");
    printw("? - Battery Level    Q - Quit\n\n");
}

static void showBatteryLevel(CHiP* pCHiP)
{
    CHiPBatteryLevel batteryLevel;
    chipGetBatteryLevel(pCHiP, &batteryLevel);

    printw("  Battery level: %.1f%%\n", batteryLevel.batteryLevel * 100.0f);

    const char* pChargingStatus = "<invalid>";
    switch (batteryLevel.chargingStatus)
    {
    case CHIP_CHARGING_STATUS_NOT_CHARGING:
        pChargingStatus = "Not Charging";
        break;
    case CHIP_CHARGING_STATUS_CHARGING:
        pChargingStatus = "Charging in Progress";
        break;
    case CHIP_CHARGING_STATUS_CHARGING_FINISHED:
        pChargingStatus = "Charging Completed";
        break;
    }
    printw("Charging status: %s\n", pChargingStatus);

    // No need to print charger type if not charging so just return.
    if (batteryLevel.chargingStatus == CHIP_CHARGING_STATUS_NOT_CHARGING)
        return;

    const char* pChargerType = "<invalid>";
    switch (batteryLevel.chargerType)
    {
    case CHIP_CHARGER_TYPE_DC:
        pChargerType = "DC Power Jack";
        break;
    case CHIP_CHARGER_TYPE_BASE:
        pChargerType = "Base Station";
        break;
    }
    printw("Charger type: %s\n", pChargerType);
}
