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
    chipEnableClap()
    chipSetClapDelay()
    chipGetClapSettings()
    chipGetLatestClapNotification()
*/
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"


static void printClapSettings(const CHiPClapSettings* pSettings);


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

    printf("\tClap.c - Use clap related functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    CHiPClapSettings settings = {0, 0};
    result = chipGetClapSettings(pCHiP, &settings);
    printf("Initial clap settings.\n");
    printClapSettings(&settings);

    // Modify clap settings.
    // NOTE: Need some delay between settings or second one will be dropped.
    result = chipEnableClap(pCHiP, CHIP_CLAP_ENABLED);
    sleep(1);
    result = chipSetClapDelay(pCHiP, 501);

    result = chipGetClapSettings(pCHiP, &settings);
    printf("Updated clap settings.\n");
    printClapSettings(&settings);

    printf("Waiting for user to clap.\n");
    CHiPClap clap = {0, 0};
    while (CHIP_ERROR_NONE != chipGetLatestClapNotification(pCHiP, &clap))
    {
    }
    printf("Detected %u claps\n", clap.count);

    chipUninit(pCHiP);
}

static void printClapSettings(const CHiPClapSettings* pSettings)
{
    printf("  Enabled = %s\n", pSettings->enabled ? "ON" : "OFF");
    printf("    Delay = %u\n", pSettings->delay);
}
