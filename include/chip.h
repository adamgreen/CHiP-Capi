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
/* This header file describes the public API that an application can use to communicate with the WowWee CHiP
   robot dog.
*/
#ifndef CHIP_H_
#define CHIP_H_

#include <stdint.h>
#include <stdlib.h>

// Integer error codes that can be returned from most of these CHiP API functions.
#define CHIP_ERROR_NONE          0 // Success
#define CHIP_ERROR_CONNECT       1 // Connection to CHiP failed.
#define CHIP_ERROR_PARAM         2 // Invalid parameter passed to API.
#define CHIP_ERROR_MEMORY        3 // Out of memory.
#define CHIP_ERROR_NOT_CONNECTED 4 // No CHiP robot connected.
#define CHIP_ERROR_NO_REQUEST    5 // Not waiting for a response from a request.
#define CHIP_ERROR_TIMEOUT       6 // Timed out waiting for response.
#define CHIP_ERROR_EMPTY         7 // The queue was empty.
#define CHIP_ERROR_BAD_RESPONSE  8 // Unexpected response from CHiP.

// Maximum length of CHiP request and response buffer lengths.
#define CHIP_REQUEST_MAX_LEN    (17 + 1)    // Longest request is CHIP_CMD_PLAY_SOUND.
#define CHIP_RESPONSE_MAX_LEN   (10 + 1)    // Longest response is CHIP_CMD_GET_DOG_VERSION.

typedef enum CHiPGestureRadarMode
{
    CHIP_GESTURE_RADAR_DISABLED = 0x00,
    CHIP_GESTURE                = 0x02,
    CHIP_RADAR                  = 0x04
} CHiPGestureRadarMode;

typedef enum CHiPRadar
{
    CHIP_RADAR_NONE      = 0x01,
    CHIP_RADAR_10CM_30CM = 0x02,
    CHIP_RADAR_0CM_10CM  = 0x03
} CHiPRadar;

typedef enum CHiPGesture
{
    CHIP_GESTURE_LEFT               = 0x0A,
    CHIP_GESTURE_RIGHT              = 0x0B,
    CHIP_GESTURE_CENTER_SWEEP_LEFT  = 0x0C,
    CHIP_GESTURE_CENTER_SWEEP_RIGHT = 0x0D,
    CHIP_GESTURE_CENTER_HOLD        = 0x0E,
    CHIP_GESTURE_FORWARD            = 0x0F,
    CHIP_GESTURE_BACKWARD           = 0x10
} CHiPGesture;

typedef enum CHiPHeadLED
{
    CHIP_HEAD_LED_OFF        = 0,
    CHIP_HEAD_LED_ON         = 1,
    CHIP_HEAD_LED_BLINK_SLOW = 2,
    CHIP_HEAD_LED_BLINK_FAST = 3
} CHiPHeadLED;

typedef enum CHiPDriveDirection
{
    CHIP_DRIVE_FORWARD  = 0x00,
    CHIP_DRIVE_BACKWARD = 0x01
} CHiPDriveDirection;

typedef enum CHiPTurnDirection
{
    CHIP_TURN_LEFT  = 0x00,
    CHIP_TURN_RIGHT = 0x01
} CHiPTurnDirection;

typedef enum CHiPFallDirection
{
    CHIP_FALL_ON_BACK   = 0x00,
    CHIP_FALL_FACE_DOWN = 0x01
} CHiPFallDirection;

typedef enum CHiPPosition
{
    CHIP_POSITION_ON_BACK                = 0x00,
    CHIP_POSITION_FACE_DOWN              = 0x01,
    CHIP_POSITION_UPRIGHT                = 0x02,
    CHIP_POSITION_PICKED_UP              = 0x03,
    CHIP_POSITION_HAND_STAND             = 0x04,
    CHIP_POSITION_FACE_DOWN_ON_TRAY      = 0x05,
    CHIP_POSITION_ON_BACK_WITH_KICKSTAND = 0x06
} CHiPPosition;

typedef enum CHiPGetUp
{
    CHIP_GETUP_FROM_FRONT  = 0x00,
    CHIP_GETUP_FROM_BACK   = 0x01,
    CHIP_GETUP_FROM_EITHER = 0x02
} CHiPGetUp;

typedef enum CHiPSoundIndex
{
    CHIP_SOUND_ONEKHZ_500MS_8K16BIT = 1,
    CHIP_SOUND_ACTION_BURPING,
    CHIP_SOUND_ACTION_DRINKING,
    CHIP_SOUND_ACTION_EATING,
    CHIP_SOUND_ACTION_FARTING_SHORT,
    CHIP_SOUND_ACTION_OUT_OF_BREATH,
    CHIP_SOUND_BOXING_PUNCHCONNECT_1,
    CHIP_SOUND_BOXING_PUNCHCONNECT_2,
    CHIP_SOUND_BOXING_PUNCHCONNECT_3,
    CHIP_SOUND_FREESTYLE_TRACKING_1,
    CHIP_SOUND_CHIP_1,
    CHIP_SOUND_CHIP_2,
    CHIP_SOUND_CHIP_3,
    CHIP_SOUND_CHIP_APP,
    CHIP_SOUND_CHIP_AWWW,
    CHIP_SOUND_CHIP_BIG_SHOT,
    CHIP_SOUND_CHIP_BLEH,
    CHIP_SOUND_CHIP_BOOM,
    CHIP_SOUND_CHIP_BYE,
    CHIP_SOUND_CHIP_CONVERSE_1,
    CHIP_SOUND_CHIP_CONVERSE_2,
    CHIP_SOUND_CHIP_DROP,
    CHIP_SOUND_CHIP_DUNNO,
    CHIP_SOUND_CHIP_FALL_OVER_1,
    CHIP_SOUND_CHIP_FALL_OVER_2,
    CHIP_SOUND_CHIP_FIGHT,
    CHIP_SOUND_CHIP_GAME,
    CHIP_SOUND_CHIP_GLOAT,
    CHIP_SOUND_CHIP_GO,
    CHIP_SOUND_CHIP_GOGOGO,
    CHIP_SOUND_CHIP_GRUNT_1,
    CHIP_SOUND_CHIP_GRUNT_2,
    CHIP_SOUND_CHIP_GRUNT_3,
    CHIP_SOUND_CHIP_HAHA_GOT_IT,
    CHIP_SOUND_CHIP_HI_CONFIDENT,
    CHIP_SOUND_CHIP_HI_NOT_SURE,
    CHIP_SOUND_CHIP_HI_SCARED,
    CHIP_SOUND_CHIP_HUH,
    CHIP_SOUND_CHIP_HUMMING_1,
    CHIP_SOUND_CHIP_HUMMING_2,
    CHIP_SOUND_CHIP_HURT,
    CHIP_SOUND_CHIP_HUUURGH,
    CHIP_SOUND_CHIP_IN_LOVE,
    CHIP_SOUND_CHIP_IT,
    CHIP_SOUND_CHIP_JOKE,
    CHIP_SOUND_CHIP_K,
    CHIP_SOUND_CHIP_LOOP_1,
    CHIP_SOUND_CHIP_LOOP_2,
    CHIP_SOUND_CHIP_LOW_BATTERY,
    CHIP_SOUND_CHIP_MIPPEE,
    CHIP_SOUND_CHIP_MORE,
    CHIP_SOUND_CHIP_MUAH_HA,
    CHIP_SOUND_CHIP_MUSIC,
    CHIP_SOUND_CHIP_OBSTACLE,
    CHIP_SOUND_CHIP_OHOH,
    CHIP_SOUND_CHIP_OH_YEAH,
    CHIP_SOUND_CHIP_OOPSIE,
    CHIP_SOUND_CHIP_OUCH_1,
    CHIP_SOUND_CHIP_OUCH_2,
    CHIP_SOUND_CHIP_PLAY,
    CHIP_SOUND_CHIP_PUSH,
    CHIP_SOUND_CHIP_RUN,
    CHIP_SOUND_CHIP_SHAKE,
    CHIP_SOUND_CHIP_SIGH,
    CHIP_SOUND_CHIP_SINGING,
    CHIP_SOUND_CHIP_SNEEZE,
    CHIP_SOUND_CHIP_SNORE,
    CHIP_SOUND_CHIP_STACK,
    CHIP_SOUND_CHIP_SWIPE_1,
    CHIP_SOUND_CHIP_SWIPE_2,
    CHIP_SOUND_CHIP_TRICKS,
    CHIP_SOUND_CHIP_TRIIICK,
    CHIP_SOUND_CHIP_TRUMPET,
    CHIP_SOUND_CHIP_WAAAAA,
    CHIP_SOUND_CHIP_WAKEY,
    CHIP_SOUND_CHIP_WHEEE,
    CHIP_SOUND_CHIP_WHISTLING,
    CHIP_SOUND_CHIP_WHOAH,
    CHIP_SOUND_CHIP_WOO,
    CHIP_SOUND_CHIP_YEAH,
    CHIP_SOUND_CHIP_YEEESSS,
    CHIP_SOUND_CHIP_YO,
    CHIP_SOUND_CHIP_YUMMY,
    CHIP_SOUND_MOOD_ACTIVATED,
    CHIP_SOUND_MOOD_ANGRY,
    CHIP_SOUND_MOOD_ANXIOUS,
    CHIP_SOUND_MOOD_BORING,
    CHIP_SOUND_MOOD_CRANKY,
    CHIP_SOUND_MOOD_ENERGETIC,
    CHIP_SOUND_MOOD_EXCITED,
    CHIP_SOUND_MOOD_GIDDY,
    CHIP_SOUND_MOOD_GRUMPY,
    CHIP_SOUND_MOOD_HAPPY,
    CHIP_SOUND_MOOD_IDEA,
    CHIP_SOUND_MOOD_IMPATIENT,
    CHIP_SOUND_MOOD_NICE,
    CHIP_SOUND_MOOD_SAD,
    CHIP_SOUND_MOOD_SHORT,
    CHIP_SOUND_MOOD_SLEEPY,
    CHIP_SOUND_MOOD_TIRED,
    CHIP_SOUND_SOUND_BOOST,
    CHIP_SOUND_SOUND_CAGE,
    CHIP_SOUND_SOUND_GUNS,
    CHIP_SOUND_SOUND_ZINGS,
    CHIP_SOUND_SHORT_MUTE_FOR_STOP,
    CHIP_SOUND_FREESTYLE_TRACKING_2,
    CHIP_SOUND_VOLUME_OFF = 0xF7,
    CHIP_SOUND_VOLUME_1   = 0xF8,
    CHIP_SOUND_VOLUME_2   = 0xF9,
    CHIP_SOUND_VOLUME_3   = 0xFA,
    CHIP_SOUND_VOLUME_4   = 0xFB,
    CHIP_SOUND_VOLUME_5   = 0xFC,
    CHIP_SOUND_VOLUME_6   = 0xFD,
    CHIP_SOUND_VOLUME_7   = 0xFE
} CHiPSoundIndex;

typedef enum CHiPClapEnabled
{
    CHIP_CLAP_DISABLED = 0x00,
    CHIP_CLAP_ENABLED  = 0x01
} CHiPClapEnabled;

typedef struct CHiPRadarNotification
{
    uint32_t millisec;
    CHiPRadar radar;
} CHiPRadarNotification;

typedef struct CHiPGestureNotification
{
    uint32_t   millisec;
    CHiPGesture gesture;
} CHiPGestureNotification;

typedef struct CHiPStatus
{
    uint32_t    millisec;
    float       battery;
    CHiPPosition position;
} CHiPStatus;

typedef struct CHiPWeight
{
    uint32_t millisec;
    int8_t   weight;
} CHiPWeight;

typedef struct CHiPClap
{
    uint32_t millisec;
    uint8_t  count;
} CHiPClap;

typedef struct CHiPChestLED
{
    uint16_t onTime;
    uint16_t offTime;
    uint8_t  red;
    uint8_t  green;
    uint8_t  blue;
} CHiPChestLED;

typedef struct CHiPHeadLEDs
{
    CHiPHeadLED led1;
    CHiPHeadLED led2;
    CHiPHeadLED led3;
    CHiPHeadLED led4;
} CHiPHeadLEDs;

typedef struct CHiPDogVersion
{
    uint8_t bodyHardware;
    uint8_t headHardware;
    uint8_t mechanic;
    uint8_t bleSpiFlash;
    uint8_t nuvotonSpiFlash;
    uint8_t bleBootloader;
    uint8_t bleApromFirmware;
    uint8_t nuvotonBootloaderFirmware;
    uint8_t nuvotonApromFirmware;
    uint8_t nuvoton;
} CHiPDogVersion;

typedef struct CHiPSound
{
    CHiPSoundIndex sound;
    uint16_t      delay;
} CHiPSound;

typedef struct CHiPClapSettings
{
    CHiPClapEnabled enabled;
    uint16_t       delay;
} CHiPClapSettings;

// Abstraction of the pointer type returned by chipInit() and subsequently passed into all other chip*() functions.
typedef struct CHiP CHiP;


// The documentation for these functions can be found at the following link:
//  https://github.com/adamgreen/CHiP-Capi#readme
CHiP* chipInit(const char* pInitOptions);
void chipUninit(CHiP* pCHiP);

int chipConnectToRobot(CHiP* pCHiP, const char* pRobotName);
int chipDisconnectFromRobot(CHiP* pCHiP);

int chipStartRobotDiscovery(CHiP* pCHiP);
int chipGetDiscoveredRobotCount(CHiP* pCHiP, size_t* pCount);
int chipGetDiscoveredRobotName(CHiP* pCHiP, size_t robotIndex, const char** ppRobotName);
int chipStopRobotDiscovery(CHiP* pCHiP);


int chipGetDogVersion(CHiP* pCHiP, CHiPDogVersion* pVersion);




int chipSetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode mode);
int chipGetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode* pMode);

int chipSetChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue);
int chipFlashChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue, uint16_t onTime, uint16_t offTime);
int chipGetChestLED(CHiP* pCHiP, CHiPChestLED* pChestLED);
int chipSetHeadLEDs(CHiP* pCHiP, CHiPHeadLED led1, CHiPHeadLED led2, CHiPHeadLED led3, CHiPHeadLED led4);
int chipGetHeadLEDs(CHiP* pCHiP, CHiPHeadLEDs* pHeadLEDs);

int chipContinuousDrive(CHiP* pCHiP, int8_t velocity, int8_t turnRate);
int chipDistanceDrive(CHiP* pCHiP, CHiPDriveDirection driveDirection, uint8_t cm,
                      CHiPTurnDirection turnDirection, uint16_t degrees);
int chipTurnLeft(CHiP* pCHiP, uint16_t degrees, uint8_t speed);
int chipTurnRight(CHiP* pCHiP, uint16_t degrees, uint8_t speed);
int chipDriveForward(CHiP* pCHiP, uint8_t speed, uint16_t time);
int chipDriveBackward(CHiP* pCHiP, uint8_t speed, uint16_t time);
int chipStop(CHiP* pCHiP);
int chipFallDown(CHiP* pCHiP, CHiPFallDirection direction);
int chipGetUp(CHiP* pCHiP, CHiPGetUp getup);

int chipPlaySound(CHiP* pCHiP, const CHiPSound* pSounds, size_t soundCount, uint8_t repeatCount);
int chipSetVolume(CHiP* pCHiP, uint8_t volume);
int chipGetVolume(CHiP* pCHiP, uint8_t* pVolume);

int chipReadOdometer(CHiP* pCHiP, float* pDistanceInCm);
int chipResetOdometer(CHiP* pCHiP);

int chipGetStatus(CHiP* pCHiP, CHiPStatus* pStatus);

int chipGetWeight(CHiP* pCHiP, CHiPWeight* pWeight);

int chipGetClapSettings(CHiP* pCHiP, CHiPClapSettings* pSettings);
int chipEnableClap(CHiP* pCHiP, CHiPClapEnabled enabled);
int chipSetClapDelay(CHiP* pCHiP, uint16_t delay);

int chipGetLatestRadarNotification(CHiP* pCHiP, CHiPRadarNotification* pNotification);
int chipGetLatestGestureNotification(CHiP* pCHiP, CHiPGestureNotification* pNotification);
int chipGetLatestStatusNotification(CHiP* pCHiP, CHiPStatus* pStatus);
int chipGetLatestShakeNotification(CHiP* pCHiP);
int chipGetLatestWeightNotification(CHiP* pCHiP, CHiPWeight* pWeight);
int chipGetLatestClapNotification(CHiP* pCHiP, CHiPClap* pClap);

int chipRawSend(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength);
int chipRawReceive(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength,
                   uint8_t* pResponseBuffer, size_t responseBufferSize, size_t* pResponseLength);
int chipRawReceiveNotification(CHiP* pCHiP, uint8_t* pNotifyBuffer, size_t notifyBufferSize, size_t* pNotifyLength);


#endif // CHIP_H_
