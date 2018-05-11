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
    chipFlashChestLED()
    chipGetChestLED()
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
    int             result = -1;
    const uint8_t   red = 0xff;
    const uint8_t   green = 0x00;
    const uint8_t   blue = 0x00;
    const uint16_t  onTime = 1000;    // 1000 msecs / sec
    const uint16_t  offTime = 1000;   // 1000 msecs / sec
    CHiPChestLED    chestLED;
    CHiP*           pCHiP = chipInit(NULL);

    printf("\tFlashChestLED.c - Use chipFlashChestLED() and chipGetChestLED() functions.\n"
           "\tShould flash chest LED red.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipFlashChestLED(pCHiP, red, green, blue, onTime, offTime);

    sleep(4);

    result = chipGetChestLED(pCHiP, &chestLED);
    printf("chestLED\n");
    printf("red: %u\n", chestLED.red);
    printf("green: %u\n", chestLED.green);
    printf("blue: %u\n", chestLED.blue);
    printf("on time: %u milliseconds\n", chestLED.onTime);
    printf("off time: %u milliseconds\n", chestLED.offTime);

    chipUninit(pCHiP);
}
