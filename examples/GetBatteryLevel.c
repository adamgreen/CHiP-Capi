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
    chipGetBatteryLevel()
*/
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

static void printBatteryLevel(const CHiPBatteryLevel* pBatteryLevel);

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

    printf("\tGetBatteryLevel.c - Use chipGetBatteryLevel().\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    printf("Calling chipGetBatteryLevel()\n");
    CHiPBatteryLevel batteryLevel;
    result = chipGetBatteryLevel(pCHiP, &batteryLevel);
    printBatteryLevel(&batteryLevel);

    chipUninit(pCHiP);
}

static void printBatteryLevel(const CHiPBatteryLevel* pBatteryLevel)
{
    printf("  Battery level: %.1f%%\n", pBatteryLevel->batteryLevel * 100.0f);

    const char* pChargingStatus = "<invalid>";
    switch (pBatteryLevel->chargingStatus)
    {
    case CHIP_CHARGING_STATUS_NOT_CHARGING:
        pChargingStatus = "Not Charging";
        break;
    case CHIP_CHARGING_STATUS_CHARGING:
        pChargingStatus = "Charging in Progress";
        break;
    case CHIP_CHARGING_STATUS_CHARGING_FINISHED:
        pChargingStatus = "Charging Completed";
        break;
    }
    printf("Charging status: %s\n", pChargingStatus);

    // No need to print charger type if not charging so just return.
    if (pBatteryLevel->chargingStatus == CHIP_CHARGING_STATUS_NOT_CHARGING)
        return;
        
    const char* pChargerType = "<invalid>";
    switch (pBatteryLevel->chargerType)
    {
    case CHIP_CHARGER_TYPE_DC:
        pChargerType = "DC Power Jack";
        break;
    case CHIP_CHARGER_TYPE_BASE:
        pChargerType = "Base Station";
        break;
    }
    printf("Charger type: %s\n", pChargerType);
}
