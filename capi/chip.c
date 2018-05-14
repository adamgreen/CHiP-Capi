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
#include <stdlib.h>
#include <string.h>
#include "chip.h"
#include "chip-transport.h"


// CHiP Protocol Commands.
// These command codes are placed in the first byte of requests sent to the CHiP and responses sent back from the CHiP.
// See https://github.com/WowWeeLabs/CHiP-BLE-Protocol/blob/master/CHiP-Protocol.md for more information.
#define CHIP_CMD_PLAY_SOUND              0x06
#define CHIP_CMD_ACTION                  0x07
#define CHIP_CMD_GET_DOG_VERSION         0x14
#define CHIP_CMD_GET_VOLUME              0x16
#define CHIP_CMD_SET_VOLUME              0x18
#define CHIP_CMD_GET_BATTERY_LEVEL       0x1C
#define CHIP_CMD_GET_CURRENT_DATE_TIME   0x3A
#define CHIP_CMD_SET_CURRENT_DATE_TIME   0x43
#define CHIP_CMD_SET_ALARM_DATE_TIME     0x44
#define CHIP_CMD_SET_SPEED               0x45
#define CHIP_CMD_GET_SPEED               0x46
#define CHIP_CMD_SET_EYE_BRIGHTNESS      0x48
#define CHIP_CMD_GET_EYE_BRIGHTNESS      0x49
#define CHIP_CMD_GET_ALARM_DATE_TIME     0x4A
#define CHIP_CMD_DRIVE                   0x78
#define CHIP_CMD_FORCE_SLEEP             0xFA

// Special sound index used to stop any current playing sound.
#define CHIP_SOUND_SHORT_MUTE_FOR_STOP   138


struct CHiP
{
    CHiPTransport*            pTransport;
};


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

int chipDrive(CHiP* pCHiP, int8_t forwardReverse, int8_t leftRight, int8_t spin)
{
    uint8_t command[1+3];

    assert( pCHiP );
    assert( forwardReverse >= -32 && forwardReverse <= 32 );
    assert( leftRight >= -32 && leftRight <= 32 );
    assert( spin >= -32 && spin <= 32 );

    command[0] = CHIP_CMD_DRIVE;

    if (forwardReverse == 0)
        command[1] = 0x00;
    else if (forwardReverse < 0)
        command[1] = 0x20 + (-forwardReverse);
    else
        command[1] = forwardReverse;

    if (spin == 0)
        command[2] = 0x00;
    else if (spin < 0)
        command[2] = 0x60 + (-spin);
    else
        command[2] = 0x40 + spin;

    if (leftRight == 0)
        command[3] = 0x00;
    else if (leftRight < 0)
        command[3] = 0xA0 + (-leftRight);
    else
        command[3] = 0x80 + leftRight;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipAction(CHiP* pCHiP, CHiPAction action)
{
    uint8_t command[1+1];

    assert( pCHiP );
    assert( action >= CHIP_ACTION_RESET && action <= CHIP_ACTION_FACE_DOWN_FOR_CONTROLLING_CHIPPIES );

    command[0] = CHIP_CMD_ACTION;
    command[1] = action;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipPlaySound(CHiP* pCHiP, CHiPSoundIndex sound)
{
    uint8_t command[1+2];

    assert( pCHiP );
    assert( sound >= CHIP_SOUND_BARK_X1_ANGRY_A34 && sound <= CHIP_SOUND_SHORT_MUTE_FOR_STOP );

    command[0] = CHIP_CMD_PLAY_SOUND;
    command[1] = sound;
    command[2] = 0;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipStopSound(CHiP* pCHiP)
{
    return chipPlaySound(pCHiP, CHIP_SOUND_SHORT_MUTE_FOR_STOP);
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

int chipGetSpeed(CHiP* pCHiP, CHiPSpeed* pSpeed)
{
    static const uint8_t getSpeed[1] = { CHIP_CMD_GET_SPEED };
    uint8_t              response[1+1];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pSpeed );

    result = chipRawReceive(pCHiP, getSpeed, sizeof(getSpeed), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 2 ||
        response[0] != CHIP_CMD_GET_SPEED ||
        response[1] > CHIP_SPEED_KID)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    *pSpeed = response[1];

    return CHIP_ERROR_NONE;
}

int chipSetSpeed(CHiP* pCHiP, CHiPSpeed speed)
{
    uint8_t command[1+1];

    assert( pCHiP );
    assert ( speed <= CHIP_SPEED_KID );

    command[0] = CHIP_CMD_SET_SPEED;
    command[1] = speed;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipGetEyeBrightness(CHiP* pCHiP, uint8_t* pBrightness)
{
    static const uint8_t getBrightness[1] = { CHIP_CMD_GET_EYE_BRIGHTNESS };
    uint8_t              response[1+1];
    size_t               responseLength;
    int                  result;

    assert( pCHiP );
    assert( pBrightness );

    result = chipRawReceive(pCHiP, getBrightness, sizeof(getBrightness), response, sizeof(response), &responseLength);
    if (result)
        return result;
    if (responseLength != 2 ||
        response[0] != CHIP_CMD_GET_EYE_BRIGHTNESS)
    {
        return CHIP_ERROR_BAD_RESPONSE;
    }

    *pBrightness = response[1];

    return CHIP_ERROR_NONE;
}

int chipSetEyeBrightness(CHiP* pCHiP, uint8_t brightness)
{
    uint8_t command[1+1];

    assert( pCHiP );

    command[0] = CHIP_CMD_SET_EYE_BRIGHTNESS;
    command[1] = brightness;

    return chipRawSend(pCHiP, command, sizeof(command));
}

int chipForceSleep(CHiP* pCHiP)
{
    uint8_t command[1+2];

    assert( pCHiP );

    command[0] = CHIP_CMD_FORCE_SLEEP;
    command[1] = 0x12;
    command[2] = 0x34;

    return chipRawSend(pCHiP, command, sizeof(command));
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
