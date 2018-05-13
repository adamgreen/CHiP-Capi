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
    chipGetAlarmDateTime()
    chipSetAlarmDateTime()
    chipCancelAlarm()
*/
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

static void printAlarmDateTime(const CHiPAlarmDateTime* pDateTime);

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

    printf("\tAlarm.c - Set an alarm for 2 minutes in the future.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Fetch and display original date/time from CHiP.
    CHiPAlarmDateTime alarm;
    result = chipGetAlarmDateTime(pCHiP, &alarm);
    printf("Original CHiP Alarm\n");
    printAlarmDateTime(&alarm);

    // Set current time to the clock on this computer.
    printf("Updating CHiP to Current Date Time\n");
    time_t currTime = time(NULL);
    struct tm* pTime = localtime(&currTime);
    CHiPCurrentDateTime dateTime;
    dateTime.year = pTime->tm_year + 1900;
    dateTime.month = pTime->tm_mon + 1;
    dateTime.day = pTime->tm_mday;
    dateTime.hour = pTime->tm_hour;
    dateTime.minute = pTime->tm_min;
    dateTime.second = pTime->tm_sec;
    dateTime.dayOfWeek = pTime->tm_wday;
    result = chipSetCurrentDateTime(pCHiP, &dateTime);

    // Set the alarm for 2 minutes in the future.
    alarm.year = pTime->tm_year + 1900;
    alarm.month = pTime->tm_mon + 1;
    alarm.day = pTime->tm_mday;
    alarm.hour = pTime->tm_hour;
    alarm.minute = pTime->tm_min + 2;
    result = chipSetAlarmDateTime(pCHiP, &alarm);

    // Fetch and display updated date/time from CHiP.
    result = chipGetAlarmDateTime(pCHiP, &alarm);
    printf("Updated CHiP Alarm\n");
    printAlarmDateTime(&alarm);

    // Clear the alarm and print it out.
    result = chipCancelAlarm(pCHiP);

    // Fetch and display updated date/time from CHiP.
    result = chipGetAlarmDateTime(pCHiP, &alarm);
    printf("Updated CHiP Alarm after Cancelling\n");
    printAlarmDateTime(&alarm);

    // Set the alarm for 2 minutes in the future again.
    alarm.year = pTime->tm_year + 1900;
    alarm.month = pTime->tm_mon + 1;
    alarm.day = pTime->tm_mday;
    alarm.hour = pTime->tm_hour;
    alarm.minute = pTime->tm_min + 2;
    result = chipSetAlarmDateTime(pCHiP, &alarm);

    // Fetch and display updated date/time from CHiP.
    result = chipGetAlarmDateTime(pCHiP, &alarm);
    printf("Updated CHiP Alarm for 2 minutes in the future\n");
    printAlarmDateTime(&alarm);
    
    printf("The CHiP should start barking in about 2 minutes.\n");
    printf("Tap CHiP's head to turn alarm off.\n");
    sleep(120);

    chipUninit(pCHiP);
}

static void printAlarmDateTime(const CHiPAlarmDateTime* pDateTime)
{
    printf("  %u-%02u-%02u %02u:%02u\n",
           pDateTime->year,
           pDateTime->month,
           pDateTime->day,
           pDateTime->hour,
           pDateTime->minute);
}
