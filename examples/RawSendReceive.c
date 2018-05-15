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
    chipRawSend()
    chipRawReceive()
*/
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

#define CHIP_CMD_SET_EYE_BRIGHTNESS      0x48
#define CHIP_CMD_GET_EYE_BRIGHTNESS      0x49

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

    printf("\tRawSendReceive.c - Use chipRaw*() functions to blink CHiP's eyes.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Manually request the current eye brightness and restore to it later.
    static const uint8_t getBrightness[1] = { CHIP_CMD_GET_EYE_BRIGHTNESS };
    uint8_t              response[1+1];
    size_t               responseLength;

    result = chipRawReceive(pCHiP, getBrightness, sizeof(getBrightness), response, sizeof(response), &responseLength);
    uint8_t originalBrightness = response[1];
    printf("Original brighness = %u\n", originalBrightness);

    // Blink the eyes by dimming and brightening the eyes a few times.
    uint8_t command[1+1];
    command[0] = CHIP_CMD_SET_EYE_BRIGHTNESS;
    for (int i = 0 ; i < 5 ; i++)
    {
        command[1] = 0x40;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);

        command[1] = 0xFF;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);
    }

    // Restore the brightness to original setting (probably 0, default).
    command[1] = originalBrightness;
    result = chipRawSend(pCHiP, command, sizeof(command));
    usleep(250000);

    chipUninit(pCHiP);
}
