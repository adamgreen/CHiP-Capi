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
    chipGetEyeBrightness()
    chipSetEyeBrightness()
*/
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

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

    printf("\tSpeed.c - Use chipGetEyeBrightness() and chipSetEyeBrightness() functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read and display the current eye brightness setting.
    uint8_t originalBrightness;
    result = chipGetEyeBrightness(pCHiP, &originalBrightness);
    printf("Original eye brightness is 0x%02X\n", originalBrightness);

    // Set the current eye brightness to medium.
    result = chipSetEyeBrightness(pCHiP, 0x80);
    uint8_t currentBrightness;
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Updated eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x80) ? "pass" : "fail");
    sleep(2);

    // Set the current eye brightness to default (0x00).
    result = chipSetEyeBrightness(pCHiP, 0x00);
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Default eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x00) ? "pass" : "fail");
    sleep(2);

    // Set back to original brightness and exit.
    result = chipSetEyeBrightness(pCHiP, originalBrightness);

    chipUninit(pCHiP);
}
