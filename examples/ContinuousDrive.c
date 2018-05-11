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
    chipContinuousDrive()
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
    int    result = -1;
    int    i = 0;
    CHiP*  pCHiP = chipInit(NULL);

    printf("\tContinuousDrive.c - Use chipContinuousDrive() function.\n"
           "\tDrive forward with right turn and then backward with left turn.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    for (i = 0 ; i < 40 ; i++)
    {
        // Drive forward at half-speed and turn right at quarter rate.
        result = chipContinuousDrive(pCHiP, 16, 8);
        // Pace out the continuous drive commands by 50 msec.
        usleep(50000);
    }
    for (i = 0 ; i < 40 ; i++)
    {
        // Drive backward at half-speed and turn left at quarter rate.
        result = chipContinuousDrive(pCHiP, -16, -8);
        // Pace out the continuous drive commands by 50 msec.
        usleep(50000);
    }

    chipUninit(pCHiP);
}
