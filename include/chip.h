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
// UNDONE: Update the request max length since play sound is different.
#define CHIP_REQUEST_MAX_LEN    (17 + 1)    // Longest request is CHIP_CMD_PLAY_SOUND.
#define CHIP_RESPONSE_MAX_LEN   (10 + 1)    // Longest response is CHIP_CMD_GET_DOG_VERSION.

typedef enum CHiPChargingStatus
{
    CHIP_CHARGING_STATUS_NOT_CHARGING = 0x00,
    CHIP_CHARGING_STATUS_CHARGING = 0x01,
    CHIP_CHARGING_STATUS_CHARGING_FINISHED = 0x02
} CHiPChargingStatus;

typedef enum CHiPChargerType 
{
    // Charging via DC barrel jack port on chest.
    CHIP_CHARGER_TYPE_DC = 0x00,
    // Charging on the base station (SmartBed).
    CHIP_CHARGER_TYPE_BASE  = 0x01
} CHiPChargerType;

typedef enum CHiPSpeed
{
    CHIP_SPEED_ADULT = 0x00,
    CHIP_SPEED_KID = 0x01
} CHiPSpeed;

typedef enum CHiPSoundIndex
{
    CHIP_SOUND_BARK_X1_ANGRY_A34 = 1,
    CHIP_SOUND_BARK_X1_CURIOUS_PLAYFUL_HAPPY_A34 = 2,
    CHIP_SOUND_BARK_X1_NEUTRAL_A34 = 3,
    CHIP_SOUND_BARK_X1_SCARED_A34 = 4,
    CHIP_SOUND_BARK_X2_ANGRY_A34 = 5,
    CHIP_SOUND_BARK_X2_CURIOUS_PLAYFUL_HAPPY_A34 = 6,
    CHIP_SOUND_BARK_X2_NEUTRAL_A34 = 7,
    CHIP_SOUND_BARK_X2_SCARED_A34 = 8,
    CHIP_SOUND_CRY_A34 = 9,
    CHIP_SOUND_GROWL_A_A34 = 10,
    CHIP_SOUND_GROWL_B_A34 = 11,
    CHIP_SOUND_GROWL_C_A34 = 12,
    CHIP_SOUND_HUH_LONG_A34 = 13,
    CHIP_SOUND_HUH_SHORT_A34 = 14,
    CHIP_SOUND_LICK_1_A34 = 15,
    CHIP_SOUND_LICK_2_A34 = 16,
    CHIP_SOUND_PANT_FAST_A34 = 17,
    CHIP_SOUND_PANT_MEDIUM_A34 = 18,
    CHIP_SOUND_PANT_SLOW_A34 = 19,
    CHIP_SOUND_SNIFF_1_A34 = 20,
    CHIP_SOUND_SNIFF_2_A34 = 21,
    CHIP_SOUND_YAWN_A_A34 = 22,
    CHIP_SOUND_YAWN_B_A34 = 23,
    CHIP_SOUND_ONE_A34 = 24,
    CHIP_SOUND_TWO_A34 = 25,
    CHIP_SOUND_THREE_A34 = 26,
    CHIP_SOUND_FOUR_A34 = 27,
    CHIP_SOUND_FIVE_A34 = 28,
    CHIP_SOUND_SIX_A34 = 29,
    CHIP_SOUND_SEVEN_A34 = 30,
    CHIP_SOUND_EIGHT_A34 = 31,
    CHIP_SOUND_NIGHT_A34 = 32,
    CHIP_SOUND_TEN_A34 = 33,
    CHIP_SOUND_ZERO_A34 = 34,
    CHIP_SOUND_CHIP_DOG_COUGH_2_A34 = 35,
    CHIP_SOUND_CHIP_DOG_CRY_2_A34 = 36,
    CHIP_SOUND_CHIP_DOG_CRY_3_A34 = 37,
    CHIP_SOUND_CHIP_DOG_CRY_4_A34 = 38,
    CHIP_SOUND_CHIP_DOG_CRY_5_A34 = 39,
    CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_1_A34 = 40,
    CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_2_A34 = 41,
    CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_3_A34 = 42,
    CHIP_SOUND_CHIP_DOG_EMO_EXCITED_1_A34 = 43,
    CHIP_SOUND_CHIP_DOG_EMO_EXCITED_2_A34 = 44,
    CHIP_SOUND_CHIP_DOG_EMO_EXCITED_3_A34 = 45,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_1_A34 = 46,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_2_A34 = 47,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_3_A34 = 48,
    CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_1_A34 = 49,
    CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_2_A34 = 50,
    CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_3_A34 = 51,
    CHIP_SOUND_CHIP_DOG_EMO_SCARED_YIP_2_A34 = 52,
    CHIP_SOUND_CHIP_DOG_EMO_SCARED_YIP_3_A34 = 53,
    CHIP_SOUND_CHIP_DOG_FART_1_A34 = 54,
    CHIP_SOUND_CHIP_DOG_FART_2_A34 = 55,
    CHIP_SOUND_CHIP_DOG_FART_3_A34 = 56,
    CHIP_SOUND_CHIP_DOG_GROWL_1_A34 = 57,
    CHIP_SOUND_CHIP_DOG_GROWL_2_A34 = 58,
    CHIP_SOUND_CHIP_DOG_GROWL_4_A34 = 59,
    CHIP_SOUND_CHIP_DOG_GROWL_5_A34 = 60,
    CHIP_SOUND_CHIP_DOG_HICCUP_1_A34 = 61,
    CHIP_SOUND_CHIP_DOG_HICCUP_2_A34 = 62,
    CHIP_SOUND_CHIP_DOG_HOWL_1_A34 = 63,
    CHIP_SOUND_CHIP_DOG_HOWL_2_A34 = 64,
    CHIP_SOUND_CHIP_DOG_HOWL_3_A34 = 65,
    CHIP_SOUND_CHIP_DOG_HOWL_4_A34 = 66,
    CHIP_SOUND_CHIP_DOG_HOWL_5_A34 = 67,
    CHIP_SOUND_CHIP_DOG_LICK_2_A34 = 68,
    CHIP_SOUND_CHIP_DOG_LICK_3_A34 = 69,
    CHIP_SOUND_CHIP_DOG_LOWBATTERY_1_A34 = 70,
    CHIP_SOUND_CHIP_DOG_LOWBATTERY_2_A34 = 71,
    CHIP_SOUND_CHIP_DOG_MUFFLE_1_A34 = 72,
    CHIP_SOUND_CHIP_DOG_MUFFLE_2_A34 = 73,
    CHIP_SOUND_CHIP_DOG_MUFFLE_3_A34 = 74,
    CHIP_SOUND_CHIP_DOG_PANT_1_A34 = 75,
    CHIP_SOUND_CHIP_DOG_PANT_2_A34 = 76,
    CHIP_SOUND_CHIP_DOG_PANT_3_A34 = 77,
    CHIP_SOUND_CHIP_DOG_PANT_4_A34 = 78,
    CHIP_SOUND_CHIP_DOG_PANT_5_L_A34 = 79,
    CHIP_SOUND_CHIP_DOG_SMOOCH_1_A34 = 80,
    CHIP_SOUND_CHIP_DOG_SMOOCH_2_A34 = 81,
    CHIP_SOUND_CHIP_DOG_SMOOCH_3_A34 = 82,
    CHIP_SOUND_CHIP_DOG_SNEEZE_1_A34 = 83,
    CHIP_SOUND_CHIP_DOG_SNEEZE_2_A34 = 84,
    CHIP_SOUND_CHIP_DOG_SNEEZE_3_A34 = 85,
    CHIP_SOUND_CHIP_DOG_SNIFF_1_A34 = 86,
    CHIP_SOUND_CHIP_DOG_SNIFF_2_A34 = 87,
    CHIP_SOUND_CHIP_DOG_SNIFF_4_A34 = 88,
    CHIP_SOUND_CHIP_DOG_SNORE_1_A34 = 89,
    CHIP_SOUND_CHIP_DOG_SNORE_2_A34 = 90,
    CHIP_SOUND_CHIP_DOG_SPECIAL_1_A34 = 91,
    CHIP_SOUND_CHIP_SING_DO1_SHORT_A34 = 92,
    CHIP_SOUND_CHIP_SING_DO2_SHORT_A34 = 93,
    CHIP_SOUND_CHIP_SING_FA_SHORT_A34 = 94,
    CHIP_SOUND_CHIP_SING_LA_SHORT_A34 = 95,
    CHIP_SOUND_CHIP_SING_MI_SHORT_A34 = 96,
    CHIP_SOUND_CHIP_SING_RE_SHORT_A34 = 97,
    CHIP_SOUND_CHIP_SING_SO_SHORT_A34 = 98,
    CHIP_SOUND_CHIP_SING_TI_SHORT_A34 = 99,
    CHIP_SOUND_CHIP_DOG_BARK_3_A34 = 100,
    CHIP_SOUND_CHIP_DOG_BARK_4_A34 = 101,
    CHIP_SOUND_CHIP_DOG_BARK_5_A34 = 102,
    CHIP_SOUND_CHIP_DOG_BARK_MULTI_3_A34 = 103,
    CHIP_SOUND_CHIP_DOG_BARK_MULTI_4_A34 = 104,
    CHIP_SOUND_CHIP_DOG_BARK_MULTI_5_A34 = 105,
    CHIP_SOUND_CHIP_DOG_BURP_1_A34 = 106,
    CHIP_SOUND_CHIP_DOG_BURP_2_A34 = 107,
    CHIP_SOUND_CHIP_DOG_COUGH_1_A34 = 108,
    CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_3A = 109,
    CHIP_SOUND_CHIP_DEMO_MUSIC_2 = 110,
    CHIP_SOUND_CHIP_DEMO_MUSIC_3 = 111,
    CHIP_SOUND_CHIP_DOG_BARK_1 = 112,
    CHIP_SOUND_CHIP_DOG_BARK_2 = 113,
    CHIP_SOUND_CHIP_DOG_BARK_MULTI_1 = 114,
    CHIP_SOUND_CHIP_DOG_BARK_MULTI_2 = 115,
    CHIP_SOUND_CHIP_DOG_CRY_1 = 116,
    CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_2A = 117,
    CHIP_SOUND_CHIP_DOG_EMO_EXCITED_3A = 118,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_1A = 119,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_2A = 120,
    CHIP_SOUND_CHIP_DOG_EMO_LAZY_3A = 121,
    CHIP_SOUND_CHIP_DOG_GROWL_3 = 122,
    CHIP_SOUND_CHIP_DOG_HOWL_1A = 123,
    CHIP_SOUND_CHIP_DOG_HOWL_3A = 124,
    CHIP_SOUND_CHIP_DOG_HOWL_4A = 125,
    CHIP_SOUND_CHIP_DOG_HOWL_5A = 126,
    CHIP_SOUND_CHIP_DOG_LICK_1 =127,
    CHIP_SOUND_CHIP_DOG_LOWBATTERY_1A = 128,
    CHIP_SOUND_CHIP_DOG_LOWBATTERY_2A = 129,
    CHIP_SOUND_CHIP_DOG_MUFFLE_1A = 130,
    CHIP_SOUND_CHIP_DOG_SMOOCH_3A = 131,
    CHIP_SOUND_CHIP_DOG_SNEEZE_1A = 132,
    CHIP_SOUND_CHIP_DOG_SNIFF_3 = 133,
    CHIP_SOUND_CHIP_DOG_SNIFF_4A = 134,
    CHIP_SOUND_CHIP_MUSIC_DEMO_1 = 135,
    CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_1A = 136,
    CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_2A = 137,
} CHiPSoundIndex;

typedef enum CHiPAction
{
    CHIP_ACTION_RESET = 0x01,
    CHIP_ACTION_SIT = 0x02,
    CHIP_ACTION_LIE_DOWN = 0x03,
    CHIP_ACTION_ALL_IDLE_MODE = 0x04,
    CHIP_ACTION_DANCE = 0x05,
    CHIP_ACTION_VR_TRAINING1 = 0x06,
    CHIP_ACTION_VR_TRAINING2 = 0x07,
    CHIP_ACTION_RESET2 = 0x08,
    CHIP_ACTION_JUMP = 0x09,
    CHIP_ACTION_YOGA = 0x0a,
    CHIP_ACTION_WATCH_COME = 0x0b,
    CHIP_ACTION_WATCH_FOLLOW = 0x0c,
    CHIP_ACTION_WATCH_FETCH = 0x0d,
    CHIP_ACTION_BALL_TRACKING = 0x0e,
    CHIP_ACTION_BALL_SOCCER = 0x0f,
    CHIP_ACTION_BASE = 0x10,
    CHIP_ACTION_DANCE_BASE = 0x11,
    CHIP_ACTION_STOP_OR_STAND_FROM_BASE = 0x12,
    CHIP_ACTION_GUARD_MODE = 0x13,
    CHIP_ACTION_FREE_ROAM = 0x14,
    CHIP_ACTION_FACE_DOWN_FOR_CONTROLLING_CHIPPIES = 0x15,
} CHiPAction;


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

typedef struct CHiPBatteryLevel
{
    // Floating point value between 0.0 (depleted battery) and 1.0 (full battery).
    float              batteryLevel;
    CHiPChargingStatus chargingStatus;
    CHiPChargerType    chargerType;
} CHiPBatteryLevel;

typedef struct CHiPCurrentDateTime
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  dayOfWeek;
} CHiPCurrentDateTime;

typedef struct CHiPAlarmDateTime
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
} CHiPAlarmDateTime;




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

int chipDrive(CHiP* pCHiP, int8_t forwardReverse, int8_t leftRight, int8_t spin);

int chipAction(CHiP* pCHiP, CHiPAction action);

int chipSetVolume(CHiP* pCHiP, uint8_t volume);
int chipGetVolume(CHiP* pCHiP, uint8_t* pVolume);

int chipGetBatteryLevel(CHiP* pCHiP, CHiPBatteryLevel* pBatteryLevel);

int chipGetDogVersion(CHiP* pCHiP, CHiPDogVersion* pVersion);

// Current & Alarm Date/Time are cleared on power cycle.
int chipGetCurrentDateTime(CHiP* pCHiP, CHiPCurrentDateTime* pDateTime);
int chipSetCurrentDateTime(CHiP* pCHiP, const CHiPCurrentDateTime* pDateTime);

int chipGetAlarmDateTime(CHiP* pCHiP, CHiPAlarmDateTime* pDateTime);
int chipSetAlarmDateTime(CHiP* pCHiP, const CHiPAlarmDateTime* pDateTime);
int chipCancelAlarm(CHiP* pCHiP);

int chipGetSpeed(CHiP* pCHiP, CHiPSpeed* pSpeed);
int chipSetSpeed(CHiP* pCHiP, CHiPSpeed speed);

int chipGetEyeBrightness(CHiP* pCHiP, uint8_t* pBrightness);
int chipSetEyeBrightness(CHiP* pCHiP, uint8_t brightness);

int chipForceSleep(CHiP* pCHiP);

int chipPlaySound(CHiP* pCHiP, CHiPSoundIndex sound);
int chipStopSound(CHiP* pCHiP);




int chipSetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode mode);
int chipGetGestureRadarMode(CHiP* pCHiP, CHiPGestureRadarMode* pMode);

int chipSetChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue);
int chipFlashChestLED(CHiP* pCHiP, uint8_t red, uint8_t green, uint8_t blue, uint16_t onTime, uint16_t offTime);
int chipGetChestLED(CHiP* pCHiP, CHiPChestLED* pChestLED);
int chipSetHeadLEDs(CHiP* pCHiP, CHiPHeadLED led1, CHiPHeadLED led2, CHiPHeadLED led3, CHiPHeadLED led4);
int chipGetHeadLEDs(CHiP* pCHiP, CHiPHeadLEDs* pHeadLEDs);

int chipDistanceDrive(CHiP* pCHiP, CHiPDriveDirection driveDirection, uint8_t cm,
                      CHiPTurnDirection turnDirection, uint16_t degrees);
int chipTurnLeft(CHiP* pCHiP, uint16_t degrees, uint8_t speed);
int chipTurnRight(CHiP* pCHiP, uint16_t degrees, uint8_t speed);
int chipDriveForward(CHiP* pCHiP, uint8_t speed, uint16_t time);
int chipDriveBackward(CHiP* pCHiP, uint8_t speed, uint16_t time);
int chipStop(CHiP* pCHiP);
int chipFallDown(CHiP* pCHiP, CHiPFallDirection direction);
int chipGetUp(CHiP* pCHiP, CHiPGetUp getup);

int chipReadOdometer(CHiP* pCHiP, float* pDistanceInCm);
int chipResetOdometer(CHiP* pCHiP);

int chipGetWeight(CHiP* pCHiP, CHiPWeight* pWeight);

int chipGetClapSettings(CHiP* pCHiP, CHiPClapSettings* pSettings);
int chipEnableClap(CHiP* pCHiP, CHiPClapEnabled enabled);
int chipSetClapDelay(CHiP* pCHiP, uint16_t delay);

int chipGetLatestRadarNotification(CHiP* pCHiP, CHiPRadarNotification* pNotification);
int chipGetLatestGestureNotification(CHiP* pCHiP, CHiPGestureNotification* pNotification);
int chipGetLatestShakeNotification(CHiP* pCHiP);
int chipGetLatestWeightNotification(CHiP* pCHiP, CHiPWeight* pWeight);
int chipGetLatestClapNotification(CHiP* pCHiP, CHiPClap* pClap);

int chipRawSend(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength);
int chipRawReceive(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength,
                   uint8_t* pResponseBuffer, size_t responseBufferSize, size_t* pResponseLength);
int chipRawReceiveNotification(CHiP* pCHiP, uint8_t* pNotifyBuffer, size_t notifyBufferSize, size_t* pNotifyLength);


#endif // CHIP_H_
