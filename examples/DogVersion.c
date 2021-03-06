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
    chipGetDogVersion()
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
    int            result = -1;
    CHiPDogVersion version;
    CHiP*          pCHiP = chipInit(NULL);

    printf("\tDogVersion.c - Use chipGetDogVersion() function.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipGetDogVersion(pCHiP, &version);

    printf("bodyHardware: %u\n"
           "headHardware: %u\n"
           "mechanic: %u\n"
           "bleSpiFlash: %u\n"
           "nuvotonSpiFlash: %u\n"
           "bleBootloader: %u\n"
           "bleApromFirmware: %u\n"
           "nuvotonBootloaderFirmware: %u\n"
           "nuvotonApromFirmware: %u\n"
           "nuvoton: %u\n",
           version.bodyHardware,
           version.headHardware,
           version.mechanic,
           version.bleSpiFlash,
           version.nuvotonSpiFlash,
           version.bleBootloader,
           version.bleApromFirmware,
           version.nuvotonBootloaderFirmware,
           version.nuvotonApromFirmware,
           version.nuvoton);

    chipUninit(pCHiP);
}
