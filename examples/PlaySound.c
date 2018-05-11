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
    chipPlaySound()
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

    printf("\tPlaySound.c - Use chipPlaySound().\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Play 1 sound.
    const CHiPSound sounds1[] = {{CHIP_SOUND_ONEKHZ_500MS_8K16BIT, 0}};
    result = chipPlaySound(pCHiP, sounds1, sizeof(sounds1)/sizeof(sounds1[0]), 0);

    sleep(2);

    // Play 2 sounds with 1 second delay between them, repeating them a second time.
    // Play the first at a lower volume than the second.
    const CHiPSound sounds2[] = {{CHIP_SOUND_VOLUME_4, 0},            // Play eating sound at half volume.
                                {CHIP_SOUND_ACTION_EATING, 1000},
                                {CHIP_SOUND_VOLUME_7, 0},            // Play burping sound at full volume.
                                {CHIP_SOUND_ACTION_BURPING, 0},
                                {CHIP_SOUND_VOLUME_1, 0}};           // Finish by setting volume low.
    result = chipPlaySound(pCHiP, sounds2, sizeof(sounds2)/sizeof(sounds2[0]), 1);

    sleep(7);

    chipUninit(pCHiP);
}
