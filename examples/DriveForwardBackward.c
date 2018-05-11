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
    chipDriveForward()
    chipDriveBackward()
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
    CHiP*  pCHiP = chipInit(NULL);

    printf("\tDriveForwardBackward.c - Use chipDriveForward() & chipDriveBackward() functions.\n"
           "\tDrive ahead and back, 1 second in each direction.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipDriveForward(pCHiP, 15, 1000);
    sleep(2);
    result = chipDriveBackward(pCHiP, 15, 1000);
    sleep(2);

    chipUninit(pCHiP);
}
