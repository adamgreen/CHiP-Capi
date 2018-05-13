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
    chipGetCurrentDateTime()
    chipSetCurrentDateTime()
*/
#include <stdio.h>
#include <time.h>
#include "chip.h"
#include "osxble.h"

static void printCurrentDateTime(const CHiPCurrentDateTime* pDateTime);

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

    printf("\tCurrentDateTime.c - Use chipGetCurrentDateTime() and chipSetCurrentDateTime() functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Fetch and display original date/time from CHiP.
    CHiPCurrentDateTime dateTime;
    result = chipGetCurrentDateTime(pCHiP, &dateTime);
    printf("Original CHiP Date Time\n");
    printCurrentDateTime(&dateTime);

    // Now set it according to the clock on this computer.
    printf("\nUpdating CHiP to Current Date Time\n\n");
    time_t currTime = time(NULL);
    struct tm* pTime = localtime(&currTime);
    dateTime.year = pTime->tm_year + 1900;
    dateTime.month = pTime->tm_mon + 1;
    dateTime.day = pTime->tm_mday;
    dateTime.hour = pTime->tm_hour;
    dateTime.minute = pTime->tm_min;
    dateTime.second = pTime->tm_sec;
    dateTime.dayOfWeek = pTime->tm_wday;
    result = chipSetCurrentDateTime(pCHiP, &dateTime);

    // Fetch and display date/time again after set.
    result = chipGetCurrentDateTime(pCHiP, &dateTime);
    printf("Updated CHiP Date Time\n");
    printCurrentDateTime(&dateTime);

    chipUninit(pCHiP);
}

static void printCurrentDateTime(const CHiPCurrentDateTime* pDateTime)
{
    const char* daysOfWeek[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    
    printf("  %u-%02u-%02u (%s) %02u:%02u:%02u\n",
           pDateTime->year,
           pDateTime->month,
           pDateTime->day,
           daysOfWeek[pDateTime->dayOfWeek],
           pDateTime->hour,
           pDateTime->minute,
           pDateTime->second);
}
