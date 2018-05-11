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
    size_t  responseLength = 0;
    uint8_t response[CHIP_RESPONSE_MAX_LEN];
    CHiP*   pCHiP = chipInit(NULL);

    printf("\tRawSendReceive.c - Use chipRaw*() functions.\n"
           "\tShould set chest LED to purple and display CHiP firmware revision\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Send 4-byte CHiP command to set Chest LED to Purple.
    static const uint8_t setChestPurple[] = "\x84\xFF\x01\xFF";
    result = chipRawSend(pCHiP, setChestPurple, sizeof(setChestPurple)-1);

    // Request the CHiP firmware revision information and display it.
    static const uint8_t getCHiPSoftwareVersion[] = "\x14";
    result = chipRawReceive(pCHiP, getCHiPSoftwareVersion, sizeof(getCHiPSoftwareVersion)-1,
                                 response, sizeof(response), &responseLength);
    if (result == CHIP_ERROR_NONE && responseLength == 5 && response[0] == 0x14)
    {
        printf("\tCHiP Software Version: %d-%d-%d (build #%d)\n",
               response[1] + 2000,
               response[2],
               response[3],
               response[4]);
    }

    chipUninit(pCHiP);
}
