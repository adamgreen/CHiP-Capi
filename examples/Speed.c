/* Copyright (C) 2018  Adam Green (https://github.com/adamgreen)

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
/* Example used in following API documentation:
    chipGetSpeed()
    chipSetSpeed()
*/
#include <stdio.h>
#include <time.h>
#include "chip.h"
#include "osxble.h"

static void printSpeed(CHiPSpeed speed);

int main(int argc, char *argv[])
{
    // Initialize the Core Bluetooth stack on this the main thread and start the worker robot thread to run the
    // code found in robotMain() below.
    osxCHiPInitAndRun();
    return 0;
}

void robotMain(void)
{
    int     result = -1;
    CHiP*   pCHiP = chipInit(NULL);

    printf("\tSpeed.c - Use chipGetSpeed() and chipSetSpeed() functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read the current speed setting.
    CHiPSpeed speed;
    result = chipGetSpeed(pCHiP, &speed);
    printf("Original speed is ");
    printSpeed(speed);
    printf("\n");

    // Switch the speed.
    result = chipSetSpeed(pCHiP, speed == CHIP_SPEED_ADULT ? CHIP_SPEED_KID : CHIP_SPEED_ADULT);

    // Read the current speed setting.
    result = chipGetSpeed(pCHiP, &speed);
    printf("New speed is ");
    printSpeed(speed);
    printf("\n");
}

static void printSpeed(CHiPSpeed speed)
{
    switch (speed)
    {
    case CHIP_SPEED_ADULT:
        printf("Adult");
        break;
    case CHIP_SPEED_KID:
        printf("Kid");
        break;
    }
}
