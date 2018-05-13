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
/* Implementation of CHiP C API. */
#include <assert.h>
#include <stdio.h> // UNDONE: Just here for some temporary debug printf().
#include <stdlib.h>
#include <string.h>
#include "chip.h"
#include "chip-transport.h"


// CHiP Protocol Commands.
// These command codes are placed in the first byte of requests sent to the CHiP and responses sent back from the CHiP.
// See https://github.com/WowWeeLabs/CHiP-BLE-Protocol/blob/master/CHiP-Protocol.md for more information.
#define CHIP_CMD_GET_DOG_VERSION         0x14
#define CHIP_CMD_GET_VOLUME              0x16
#define CHIP_CMD_SET_VOLUME              0x18
#define CHIP_CMD_GET_BATTERY_LEVEL       0x1C
#define CHIP_CMD_GET_CURRENT_DATE_TIME   0x3A
#define CHIP_CMD_SET_CURRENT_DATE_TIME   0x43
#define CHIP_CMD_SET_ALARM_DATE_TIME     0x44
#define CHIP_CMD_GET_ALARM_DATE_TIME     0x4A

#define CHIP_CMD_PLAY_SOUND              0x06
#define CHIP_CMD_SET_POSITION            0x08
#define CHIP_CMD_GET_GESTURE_RESPONSE    0x0A
#define CHIP_CMD_SET_GESTURE_RADAR_MODE  0x0C
#define CHIP_CMD_GET_RADAR_RESPONSE      0x0C
#define CHIP_CMD_GET_GESTURE_RADAR_MODE  0x0D
#define CHIP_CMD_GET_HARDWARE_INFO       0x19
#define CHIP_CMD_SHAKE_RESPONSE          0x1A
#define CHIP_CMD_CLAP_RESPONSE           0x1D
#define CHIP_CMD_ENABLE_CLAP             0x1E
#define CHIP_CMD_GET_CLAP_SETTINGS       0x1F
#define CHIP_CMD_SET_CLAP_DELAY          0x20
#define CHIP_CMD_GET_UP                  0x23
#define CHIP_CMD_DISTANCE_DRIVE          0x70
#define CHIP_CMD_DRIVE_FORWARD           0x71
#define CHIP_CMD_DRIVE_BACKWARD          0x72
#define CHIP_CMD_TURN_LEFT               0x73
#define CHIP_CMD_TURN_RIGHT              0x74
#define CHIP_CMD_STOP                    0x77
#define CHIP_CMD_CONTINUOUS_DRIVE        0x78
#define CHIP_CMD_GET_STATUS              0x79
#define CHIP_CMD_GET_WEIGHT              0x81
#define CHIP_CMD_GET_CHEST_LED           0x83
#define CHIP_CMD_SET_CHEST_LED           0x84
#define CHIP_CMD_READ_ODOMETER           0x85
#define CHIP_CMD_RESET_ODOMETER          0x86
#define CHIP_CMD_FLASH_CHEST_LED         0x89
#define CHIP_CMD_SET_HEAD_LEDS           0x8A
#define CHIP_CMD_GET_HEAD_LEDS           0x8B


// CHiP::flags bits
#define CHIP_FLAG_RADAR_VALID   (1 << 0)
#define CHIP_FLAG_STATUS_VALID  (1 << 1)
#define CHIP_FLAG_GESTURE_VALID (1 << 2)
#define CHIP_FLAG_SHAKE_VALID   (1 << 3)
#define CHIP_FLAG_WEIGHT_VALID  (1 << 4)
#define CHIP_FLAG_CLAP_VALID    (1 << 5)


struct CHiP
{
    CHiPTransport*            pTransport;
    CHiPRadarNotification     lastRadar;
    CHiPGestureNotification   lastGesture;
    CHiPWeight                lastWeight;
    CHiPClap                  lastClap;
    uint32_t                  flags;
};


// Forward Function Declarations.
static int isValidHeadLED(CHiPHeadLED led);
static int parseWeight(CHiP* pCHiP, CHiPWeight* pWeight, const uint8_t* pResponse, size_t responseLength);
static void readNotifications(CHiP* pCHiP);


CHiP* chipInit(const char* pInitOptions)
{
    CHiP* pCHiP = NULL;

    pCHiP = calloc(1, sizeof(*pCHiP));
    if (!pCHiP)
        goto Error;
    pCHiP->pTransport = chipTransportInit(pInitOptions);
    if (!pCHiP->pTransport)
        goto Error;

    return pCHiP;

Error:
    if (pCHiP)
    {
        chipTransportUninit(pCHiP->pTransport);
        free(pCHiP);
    }
    return NULL;
}

void chipUninit(CHiP* pCHiP)
{
    if (!pCHiP)
        return;
    chipTransportUninit(pCHiP->pTransport);
}

int chipConnectToRobot(CHiP* pCHiP, const char* pRobotName)
{
    assert( pCHiP );
    return chipTransportConnectToRobot(pCHiP->pTransport, pRobotName);
}

int chipDisconnectFromRobot(CHiP* pCHiP)
{
    assert( pCHiP );
    return chipTransportDisconnectFromRobot(pCHiP->pTransport);
}

int chipStartRobotDiscovery(CHiP* pCHiP)
{
    assert( pCHiP );
    return chipTransportStartRobotDiscovery(pCHiP->pTransport);
}

int chipGetDiscoveredRobotCount(CHiP* pCHiP, size_t* pCount)
{
    assert( pCHiP );
    return chipTransportGetDiscoveredRobotCount(pCHiP->pTransport, pCount);
}

int chipGetDiscoveredRobotName(CHiP* pCHiP, size_t robotIndex, const char** ppRobotName)
{
    assert( pCHiP );
    return chipTransportGetDiscoveredRobotName(pCHiP->pTransport, robotIndex, ppRobotName);
}

int chipStopRobotDiscovery(CHiP* pCHiP)
{
    assert( pCHiP );
    return chipTransportStopRobotDiscovery(pCHiP->pTransport);
}

int chipSetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode mode)
{
    uint8_t command[1+1];

    assert( pCHiP );
    assert( mode == CHIP_GESTURE_RADAR_DISABLED || mode == CHIP_GESTURE || mode == CHIP_RADAR );

    command[0] = CHIP_CMD_SET_GESTURE_RADAR_MODE;
    command[1] = mode;
    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode* pMode)
{
    static const uint8_t getGestureRadarMode[1] = { CHIP_CMD_GET_GESTURE_RADAR_MODE };
    uint8_t              response[1+1];
    size_t               responseLength;
    int                  result;

    result = chipRawReceive(pCHiP, getGestureRadarMode, sizeof(getGestureRadarMode), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 2 ||
        response[0] != CHIP_CMD_GET_GESTURE_RADAR_MODE ||
        (response[1] != CHIP_GESTURE_RADAR_DISABLED &&
         response[1] != CHIP_GESTURE &&
         response[1] != CHIP_RADAR))
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    *pMode = response[1];
    return result;
}

int chipSetChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t command[1+3];

    assert( pCHiP );

    command[0] = CHIP_CMD_SET_CHEST_LED;
    command[1] = red;
    command[2] = green;
    command[3] = blue;
    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipFlashChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue, uint16_t onTime, uint16_t offTime)
{
    uint8_t command[1+5];

    assert( pCHiP );
    // on/off time are in units of 20 msecs.
    assert( onTime / 20 <= 255 && offTime / 20 <= 255 );

    command[0] = CHIP_CMD_FLASH_CHEST_LED;
    command[1] = red;
    command[2] = green;
    command[3] = blue;
    command[4] = onTime / 20;
    command[5] = offTime / 20;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetChestLED(CHiP* pCHiP, CHiPChestLED* pChestLED)
{
    static const uint8_t getChestLED[1] = { CHIP_CMD_GET_CHEST_LED };
    uint8_t              response[1+5];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pChestLED );

    result = chipRawReceive(pCHiP, getChestLED, sizeof(getChestLED), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) || response[0] != CHIP_CMD_GET_CHEST_LED )
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    pChestLED->red = response[1];
    pChestLED->green = response[2];
    pChestLED->blue = response[3];
    // on/off time are in units of 20 msecs.
    pChestLED->onTime = (uint16_t)response[4] * 20;
    pChestLED->offTime = (uint16_t)response[5] * 20;
    return result;
}

int chipSetHeadLEDs(CHiP* pCHiP, CHiPHeadLED led1, CHiPHeadLED led2, CHiPHeadLED led3, CHiPHeadLED led4)
{
    uint8_t command[1+4];

    assert( pCHiP );

    command[0] = CHIP_CMD_SET_HEAD_LEDS;
    command[1] = led1;
    command[2] = led2;
    command[3] = led3;
    command[4] = led4;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetHeadLEDs(CHiP* pCHiP, CHiPHeadLEDs* pHeadLEDs)
{
    static const uint8_t getHeadLEDs[1] = { CHIP_CMD_GET_HEAD_LEDS };
    uint8_t              response[1+4];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pHeadLEDs );

    result = chipRawReceive(pCHiP, getHeadLEDs, sizeof(getHeadLEDs), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) ||
        response[0] != CHIP_CMD_GET_HEAD_LEDS ||
        !isValidHeadLED(response[1]) ||
        !isValidHeadLED(response[2]) ||
        !isValidHeadLED(response[3]) ||
        !isValidHeadLED(response[4]))
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    pHeadLEDs->led1 = response[1];
    pHeadLEDs->led2 = response[2];
    pHeadLEDs->led3 = response[3];
    pHeadLEDs->led4 = response[4];
    return result;
}

static int isValidHeadLED(CHiPHeadLED led)
{
    return led >= CHIP_HEAD_LED_OFF && led <= CHIP_HEAD_LED_BLINK_FAST;
}

int chipContinuousDrive(CHiP* pCHiP, int8_t velocity, int8_t turnRate)
{
    uint8_t command[1+2];

    assert( pCHiP );
    assert( velocity >= -32 && velocity <= 32 );
    assert( turnRate >= -32 && turnRate <= 32 );

    command[0] = CHIP_CMD_CONTINUOUS_DRIVE;

    if (velocity == 0)
        command[1] = 0x00;
    else if (velocity < 0)
        command[1] = 0x20 + (-velocity);
    else
        command[1] = velocity;

    if (turnRate == 0)
        command[2] = 0x00;
    else if (turnRate < 0)
        command[2] = 0x60 + (-turnRate);
    else
        command[2] = 0x40 + turnRate;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipDistanceDrive(CHiP* pCHiP, CHiPDriveDirection driveDirection, uint8_t cm,
                                CHiPTurnDirection turnDirection, uint16_t degrees)
{
    uint8_t command[1+5];

    assert( pCHiP );
    assert( degrees <= 360 );

    command[0] = CHIP_CMD_DISTANCE_DRIVE;
    command[1] = driveDirection;
    command[2] = cm;
    command[3] = turnDirection;
    command[4] = degrees >> 8;
    command[5] = degrees & 0xFF;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipTurnLeft(CHiP* pCHiP, uint16_t degrees, uint8_t speed)
{
    // The turn command is in units of 5 degrees.
    uint8_t angle = degrees / 5;
    uint8_t command[1+2];

    assert( pCHiP );
    assert( degrees <= 255 * 5 );
    assert( speed <= 24 );

    command[0] = CHIP_CMD_TURN_LEFT;
    command[1] = angle;
    command[2] = speed;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipTurnRight(CHiP* pCHiP, uint16_t degrees, uint8_t speed)
{
    // The turn command is in units of 5 degrees.
    uint8_t angle = degrees / 5;
    uint8_t command[1+2];

    assert( pCHiP );
    assert( degrees <= 255 * 5 );
    assert( speed <= 24 );

    command[0] = CHIP_CMD_TURN_RIGHT;
    command[1] = angle;
    command[2] = speed;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipDriveForward(CHiP* pCHiP, uint8_t speed, uint16_t time)
{
    // The time parameters is in units of 7 milliseconds.
    uint8_t command[1+2];

    assert( pCHiP );
    assert( speed <= 30 );
    assert( time <= 255 * 7 );

    command[0] = CHIP_CMD_DRIVE_FORWARD;
    command[1] = speed;
    command[2] = time / 7;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipDriveBackward(CHiP* pCHiP, uint8_t speed, uint16_t time)
{
    // The time parameters is in units of 7 milliseconds.
    uint8_t command[1+2];

    assert( pCHiP );
    assert( speed <= 30 );
    assert( time <= 255 * 7 );

    command[0] = CHIP_CMD_DRIVE_BACKWARD;
    command[1] = speed;
    command[2] = time / 7;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipStop(CHiP* pCHiP)
{
    uint8_t command[1];

    assert( pCHiP );

    command[0] = CHIP_CMD_STOP;
    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipFallDown(CHiP* pCHiP, CHiPFallDirection direction)
{
    uint8_t command[1+1];

    assert( pCHiP );

    command[0] = CHIP_CMD_SET_POSITION;
    command[1] = direction;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetUp(CHiP* pCHiP, CHiPGetUp getup)
{
    uint8_t command[1+1];

    assert( pCHiP );

    command[0] = CHIP_CMD_GET_UP;
    command[1] = getup;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipPlaySound(CHiP* pCHiP, const CHiPSound* pSounds, size_t soundCount, uint8_t repeatCount)
{
    size_t  i;
    uint8_t command[1+17];

    assert( pCHiP );
    assert( pSounds );
    assert( soundCount <= 8 );

    command[0] = CHIP_CMD_PLAY_SOUND;
    for (i = 0 ; i < 8 ; i++)
    {
        if (i < soundCount)
        {
            // Delay is in units of 30 msecs.
            assert( pSounds[i].delay <= 255 * 30 );
            command[1 + i*2] = pSounds[i].sound;
            command[1 + i*2 +1 ] = pSounds[i].delay / 30;
        }
        else
        {
            command[1 + i*2] = CHIP_SOUND_SHORT_MUTE_FOR_STOP;
            command[1 + i*2 + 1] = 0;
        }
    }
    command[17] = repeatCount;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipSetVolume(CHiP* pCHiP, uint8_t volume)
{
    uint8_t command[1+1];

    assert( pCHiP );
    assert( volume >= 1 && volume <= 11 );

    command[0] = CHIP_CMD_SET_VOLUME;
    command[1] = volume;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetVolume(CHiP* pCHiP, uint8_t* pVolume)
{
    static const uint8_t getVolume[1] = { CHIP_CMD_GET_VOLUME };
    uint8_t              response[1+1];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pVolume );

    result = chipRawReceive(pCHiP, getVolume, sizeof(getVolume), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) ||
        response[0] != CHIP_CMD_GET_VOLUME ||
        response[1] == 0 || response[1] > 11)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    *pVolume = response[1];
    return result;
}

int chipReadOdometer(CHiP* pCHiP, float* pDistanceInCm)
{
    static const uint8_t readOdometer[1] = { CHIP_CMD_READ_ODOMETER };
    uint8_t              response[1+4];
    size_t               responseLength;
    uint32_t             ticks;
    int                  result;

    assert( pCHiP );
    assert( pDistanceInCm );

    result = chipRawReceive(pCHiP, readOdometer, sizeof(readOdometer), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) ||
        response[0] != CHIP_CMD_READ_ODOMETER)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    // Tick count is store as big-endian in response buffer.
    ticks = response[1] << 24 | response[2] << 16 | response[3] << 8 | response[4];
    // Odometer has 48.5 ticks / cm.
    *pDistanceInCm = (float)((double)ticks / 48.5);
    return result;
}

int chipResetOdometer(CHiP* pCHiP)
{
    uint8_t command[1];

    assert( pCHiP );

    command[0] = CHIP_CMD_RESET_ODOMETER;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetBatteryLevel(CHiP* pCHiP, CHiPBatteryLevel* pBatteryLevel)
{
    static const uint8_t getBatteryLevel[1] = { CHIP_CMD_GET_BATTERY_LEVEL };
    uint8_t              response[1+3];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pBatteryLevel );

    result = chipRawReceive(pCHiP, getBatteryLevel, sizeof(getBatteryLevel), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 4 ||
        response[0] != CHIP_CMD_GET_BATTERY_LEVEL ||
        response[1] > CHIP_CHARGING_STATUS_CHARGING_FINISHED ||
        response[2] > CHIP_CHARGER_TYPE_BASE)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    // Convert battery integer value to floating point percentage value between 0.0f and 1.0f.
    pBatteryLevel->chargingStatus = response[1];
    pBatteryLevel->chargerType = response[2];
    pBatteryLevel->batteryLevel = (float)(response[3] - 0x7D) / 34.0f;
    return CHIP_ERROR_NONE;
}

int chipGetCurrentDateTime(CHiP* pCHiP, CHiPCurrentDateTime* pDateTime)
{
    static const uint8_t getCurrentDateTime[1] = { CHIP_CMD_GET_CURRENT_DATE_TIME };
    uint8_t              response[1+8];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pDateTime );

    result = chipRawReceive(pCHiP, getCurrentDateTime, sizeof(getCurrentDateTime), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 9 ||
        response[0] != CHIP_CMD_GET_CURRENT_DATE_TIME ||
        response[3] > 12 || // Month
        response[4] > 31 || // Day
        response[5] > 23 || // Hour
        response[6] > 59 || // Minute
        response[7] > 59 || // Second
        response[8] > 7)    // Day of Week
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    // Year is stored in 2 bytes, big endian.
    pDateTime->year = ((uint16_t)response[1] << 8) | (uint16_t)response[2];
    pDateTime->month = response[3];
    pDateTime->day = response[4];
    pDateTime->hour = response[5];
    pDateTime->minute = response[6];
    pDateTime->second = response[7];
    pDateTime->dayOfWeek = response[8];
    
    return CHIP_ERROR_NONE;
}

int chipSetCurrentDateTime(CHiP* pCHiP, const CHiPCurrentDateTime* pDateTime)
{
    uint8_t command[1+8];

    assert( pCHiP );
    assert( pDateTime->month >= 1 && pDateTime->month <= 12 );
    assert( pDateTime->day >= 1 && pDateTime->day <= 31 );
    assert( pDateTime->hour < 24 );
    assert( pDateTime->minute < 60 );
    assert( pDateTime->second < 60 );
    assert( pDateTime->dayOfWeek < 7 );

    command[0] = CHIP_CMD_SET_CURRENT_DATE_TIME;
    command[1] = (pDateTime->year >> 8) & 0xFF;
    command[2] = pDateTime->year & 0xFF;
    command[3] = pDateTime->month;
    command[4] = pDateTime->day;
    command[5] = pDateTime->hour;
    command[6] = pDateTime->minute;
    command[7] = pDateTime->second;
    command[8] = pDateTime->dayOfWeek;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetAlarmDateTime(CHiP* pCHiP, CHiPAlarmDateTime* pDateTime)
{
    static const uint8_t getAlarmDateTime[1] = { CHIP_CMD_GET_ALARM_DATE_TIME };
    uint8_t              response[1+6];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pDateTime );

    result = chipRawReceive(pCHiP, getAlarmDateTime, sizeof(getAlarmDateTime), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 7 ||
        response[0] != CHIP_CMD_GET_ALARM_DATE_TIME ||
        response[3] > 12 || // Month
        response[4] > 31 || // Day
        response[5] > 23 || // Hour
        response[6] > 59)   // Minute
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    // Year is stored in 2 bytes, big endian.
    pDateTime->year = ((uint16_t)response[1] << 8) | (uint16_t)response[2];
    pDateTime->month = response[3];
    pDateTime->day = response[4];
    pDateTime->hour = response[5];
    pDateTime->minute = response[6];
    
    return CHIP_ERROR_NONE;
}

int chipSetAlarmDateTime(CHiP* pCHiP, const CHiPAlarmDateTime* pDateTime)
{
    uint8_t command[1+6];

    assert( pCHiP );
    assert( pDateTime->month >= 1 && pDateTime->month <= 12 );
    assert( pDateTime->day >= 1 && pDateTime->day <= 31 );
    assert( pDateTime->hour < 24 );
    assert( pDateTime->minute < 60 );

    command[0] = CHIP_CMD_SET_ALARM_DATE_TIME;
    command[1] = (pDateTime->year >> 8) & 0xFF;
    command[2] = pDateTime->year & 0xFF;
    command[3] = pDateTime->month;
    command[4] = pDateTime->day;
    command[5] = pDateTime->hour;
    command[6] = pDateTime->minute;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipCancelAlarm(CHiP* pCHiP)
{
    uint8_t command[1+6];

    assert( pCHiP );

    memset(command, 0, sizeof(command));
    command[0] = CHIP_CMD_SET_ALARM_DATE_TIME;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetWeight(CHiP* pCHiP, CHiPWeight* pWeight)
{
    static const uint8_t getWeight[1] = { CHIP_CMD_GET_WEIGHT };
    uint8_t              response[1+1];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pWeight );

    result = chipRawReceive(pCHiP, getWeight, sizeof(getWeight), response, sizeof(response), &responseLength);
    if (result)
        return result;
    return parseWeight(pCHiP, pWeight, response, responseLength);
}

static int parseWeight(CHiP* pCHiP, CHiPWeight* pWeight, const uint8_t* pResponse, size_t responseLength)
{
    if (responseLength != 2 ||
        pResponse[0] != CHIP_CMD_GET_WEIGHT)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    pWeight->millisec = chipTransportGetMilliseconds(pCHiP->pTransport);
    pWeight->weight = pResponse[1];
    return CHIP_ERROR_NONE;
}

int chipGetClapSettings(CHiP* pCHiP, CHiPClapSettings* pSettings)
{
    static const uint8_t getClapSettings[1] = { CHIP_CMD_GET_CLAP_SETTINGS };
    uint8_t              response[1+3];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pSettings );

    result = chipRawReceive(pCHiP, getClapSettings, sizeof(getClapSettings), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) ||
        response[0] != CHIP_CMD_GET_CLAP_SETTINGS ||
        (response[1] != CHIP_CLAP_DISABLED && response[1] != CHIP_CLAP_ENABLED))
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    pSettings->enabled = response[1];
    pSettings->delay = response[2] << 8 | response[3];
    return CHIP_ERROR_NONE;
}

int chipEnableClap(CHiP* pCHiP, CHiPClapEnabled enabled)
{
    uint8_t command[1+1];

    assert( pCHiP );

    command[0] = CHIP_CMD_ENABLE_CLAP;
    command[1] = enabled;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipSetClapDelay(CHiP* pCHiP, uint16_t delay)
{
    uint8_t command[1+2];

    assert( pCHiP );

    command[0] = CHIP_CMD_SET_CLAP_DELAY;
    command[1] = delay >> 8;
    command[2] = delay & 0xFF;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetLatestRadarNotification(CHiP* pCHiP, CHiPRadarNotification* pNotification)
{
    CHiPRadar radar;

    readNotifications(pCHiP);

    if ((pCHiP->flags & CHIP_FLAG_RADAR_VALID) == 0)
        return CHIP_ERROR_EMPTY;
    radar = pCHiP->lastRadar.radar;
    if (radar != CHIP_RADAR_NONE && radar != CHIP_RADAR_10CM_30CM && radar != CHIP_RADAR_0CM_10CM)
        return CHIP_ERROR_BAD_RESPONSE;

    *pNotification = pCHiP->lastRadar;
    return CHIP_ERROR_NONE;
}

static void readNotifications(CHiP* pCHiP)
{
    int     result = -1;
    size_t  responseLength = 0;
    uint8_t response[CHIP_RESPONSE_MAX_LEN];

    while (CHIP_ERROR_NONE == chipRawReceiveNotification(pCHiP, response, sizeof(response), &responseLength))
    {
        // Must have at least one byte to indicate which response is being given.
        if (responseLength < 1)
            continue;
        switch (response[0])
        {
        case CHIP_CMD_GET_RADAR_RESPONSE:
            if (responseLength == 2)
            {
                pCHiP->lastRadar.millisec = chipTransportGetMilliseconds(pCHiP->pTransport);
                pCHiP->lastRadar.radar = response[1];
                pCHiP->flags |= CHIP_FLAG_RADAR_VALID;
            }
            break;
        case CHIP_CMD_GET_GESTURE_RESPONSE:
            if (responseLength == 2)
            {
                pCHiP->lastGesture.millisec = chipTransportGetMilliseconds(pCHiP->pTransport);
                pCHiP->lastGesture.gesture = response[1];
                pCHiP->flags |= CHIP_FLAG_GESTURE_VALID;
            }
            break;
        case CHIP_CMD_SHAKE_RESPONSE:
            if (responseLength == 1)
            {
                pCHiP->flags |= CHIP_FLAG_SHAKE_VALID;
            }
            break;
        case CHIP_CMD_GET_WEIGHT:
            result = parseWeight(pCHiP, &pCHiP->lastWeight, response, responseLength);
            if (result == CHIP_ERROR_NONE)
                pCHiP->flags |= CHIP_FLAG_WEIGHT_VALID;
            break;
        case CHIP_CMD_CLAP_RESPONSE:
            if (responseLength == 2)
            {
                pCHiP->lastClap.millisec = chipTransportGetMilliseconds(pCHiP->pTransport);
                pCHiP->lastClap.count = response[1];
                pCHiP->flags |= CHIP_FLAG_CLAP_VALID;
            }
            break;
        default:
            printf("notification -> ");
            for (int i = 0 ; i < responseLength ; i++)
                printf("%02X", response[i]);
            printf("\n");
            break;
        }
    }
}

int chipGetLatestGestureNotification(CHiP* pCHiP, CHiPGestureNotification* pNotification)
{
    CHiPGesture gesture;

    readNotifications(pCHiP);

    if ((pCHiP->flags & CHIP_FLAG_GESTURE_VALID) == 0)
        return CHIP_ERROR_EMPTY;
    gesture = pCHiP->lastGesture.gesture;
    if (gesture < CHIP_GESTURE_LEFT || gesture > CHIP_GESTURE_BACKWARD)
        return CHIP_ERROR_BAD_RESPONSE;

    *pNotification = pCHiP->lastGesture;
    return CHIP_ERROR_NONE;
}

int chipGetLatestShakeNotification(CHiP* pCHiP)
{
    readNotifications(pCHiP);

    if ((pCHiP->flags & CHIP_FLAG_SHAKE_VALID) == 0)
        return CHIP_ERROR_EMPTY;
    pCHiP->flags &= ~CHIP_FLAG_SHAKE_VALID;
    return CHIP_ERROR_NONE;
}

int chipGetLatestWeightNotification(CHiP* pCHiP, CHiPWeight* pWeight)
{
    readNotifications(pCHiP);

    if ((pCHiP->flags & CHIP_FLAG_WEIGHT_VALID) == 0)
        return CHIP_ERROR_EMPTY;

    *pWeight = pCHiP->lastWeight;
    return CHIP_ERROR_NONE;
}

int chipGetLatestClapNotification(CHiP* pCHiP, CHiPClap* pClap)
{
    readNotifications(pCHiP);

    if ((pCHiP->flags & CHIP_FLAG_CLAP_VALID) == 0)
        return CHIP_ERROR_EMPTY;

    *pClap = pCHiP->lastClap;
    return CHIP_ERROR_NONE;
}

int chipGetDogVersion(CHiP* pCHiP, CHiPDogVersion* pVersion)
{
    static const uint8_t getDogVersion[1] = { CHIP_CMD_GET_DOG_VERSION };
    uint8_t              response[1+10];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pVersion );

    result = chipRawReceive(pCHiP, getDogVersion, sizeof(getDogVersion), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != sizeof(response) || response[0] != CHIP_CMD_GET_DOG_VERSION)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    pVersion->bodyHardware = response[1];
    pVersion->headHardware = response[2];
    pVersion->mechanic = response[3];
    pVersion->bleSpiFlash = response[4];
    pVersion->nuvotonSpiFlash = response[5];
    pVersion->bleBootloader = response[6];
    pVersion->bleApromFirmware = response[7];
    pVersion->nuvotonBootloaderFirmware = response[8];
    pVersion->nuvotonApromFirmware = response[9];
    pVersion->nuvoton = response[10];
    return result;
}

int chipRawSend(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength)
{
    assert( pCHiP );
    return chipTransportSendRequest(pCHiP->pTransport, pRequest, requestLength, CHIP_EXPECT_NO_RESPONSE);
}

int chipRawReceive(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength,
                   uint8_t* pResponseBuffer, size_t responseBufferSize, size_t* pResponseLength)
{
    int result = -1;

    assert( pCHiP );

    result = chipTransportSendRequest(pCHiP->pTransport, pRequest, requestLength, CHIP_EXPECT_RESPONSE);
    if (result)
        return result;
    return chipTransportGetResponse(pCHiP->pTransport, pResponseBuffer, responseBufferSize, pResponseLength);
}

int chipRawReceiveNotification(CHiP* pCHiP, uint8_t* pNotifyBuffer, size_t notifyBufferSize, size_t* pNotifyLength)
{
    assert( pCHiP );
    return chipTransportGetOutOfBandResponse(pCHiP->pTransport, pNotifyBuffer, notifyBufferSize, pNotifyLength);
}
