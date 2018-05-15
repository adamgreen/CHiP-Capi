# CHiP C API
<img src="https://raw.githubusercontent.com/adamgreen/CHiP/master/images/20180507-01.jpg" alt="WowWee CHiP" width="320" height="240" />

[WowWee](https://wowwee.com) supports a few different ways for you to take control of your [CHiP Robotic Dog](https://wowwee.com/CHiP):
* [Official WowWee CHiP App from the Apple App Store](https://itunes.apple.com/us/app/chip-your-new-best-friend/id1137040079?mt=8)
* [Official WowWee iOS CHiP SDK](https://github.com/WowWeeLabs/CHIP-iOS-SDK)
* [Official WowWee CHiP App from Google Play Store](https://play.google.com/store/apps/details?id=com.wowwee.chip&hl=en)
* [Official WowWee Android CHiP SDK](https://github.com/WowWeeLabs/CHIP-Android-SDK)

All of the above make use of a Bluetooth Low Energy (BLE) based protocol for communicating with the CHiP, similar to their [MiP self-balancing robot](https://github.com/WowWeeLabs/MiP-BLE-Protocol). The library in this repository lets you take control of the CHiP robot from you Macintosh computer using that same BLE based protocol.

On a platform like macOS it would be typical to write a Cocoa UI application that responds to events.  The [Official WowWee iOS SDK](https://github.com/WowWeeLabs/CHiP-iOS-SDK#readme) is an example of a framework that works in such an environment. This **CHiP C API** attempts to hide that event driven behavior from the developer.  It accomplishes this by having the main thread call **osxCHiPInitAndRun()** which runs the main NSApplication event loop and starts up a worker thread to execute the developer's code in a more sequential manner.  The developer just needs to implement **robotMain()** to provide the code to be run in that worker thread.  It is expected that the developer's macOS applications are written to be console applications.  For macOS GUI based applications, it would be better to create a macOS port of the [Official WowWee iOS SDK](https://github.com/WowWeeLabs/CHiP-iOS-SDK#readme) and use it instead.

### Example
```c
#include <stdio.h>
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
    int   result = -1;
    CHiP* pCHiP = chipInit(NULL);

    printf("\tConnect.c - Connect and disconnect from default CHiP.\n");

    // Connect to default CHiP robot.
    result = chipConnectToRobot(pCHiP, NULL);

    // Do stuff with the CHiP robot.

    // Disconnect and shutdown the application.
    result = chipDisconnectFromRobot(pCHiP);
    chipUninit(pCHiP);
}
```


## Reference
### Error Codes
| Error                     | Value    | Description
|---------------------------|----------|---------------
| CHIP_ERROR_NONE           | 0        | Success
| CHIP_ERROR_CONNECT        | 1        | Connection to CHiP failed
| CHIP_ERROR_PARAM          | 2        | Invalid parameter passed to API
| CHIP_ERROR_MEMORY         | 3        | Out of memory
| CHIP_ERROR_NOT_CONNECTED  | 4        | No CHiP robot connected
| CHIP_ERROR_NO_REQUEST     | 5        | Not waiting for a response from a request
| CHIP_ERROR_TIMEOUT        | 6        | Timed out waiting for response
| CHIP_ERROR_EMPTY          | 7        | The queue was empty
| CHIP_ERROR_BAD_RESPONSE   | 8        | Unexpected response from CHiP


### API by Function
| Function          | API
|-------------------|---------------------------------------------------------------------------------------------------
| Initialization    | [chipInit](#chipinit)
| <br>              | [chipUninit](#chipuninit)
| Connection        | [chipConnectToRobot](#chipconnecttorobot)
| <br>              | [chipDisconnectFromRobot](#chipdisconnectfromrobot)
| Discovery         | [chipStartRobotDiscovery](#chipstartrobotdiscovery)
| <br>              | [chipGetDiscoveredRobotCount](#chipgetdiscoveredrobotcount)
| <br>              | [chipGetDiscoveredRobotName](#chipgetdiscoveredrobotname)
| <br>              | [chipStopRobotDiscovery](#chipstoprobotdiscovery)
| Motion            | [chipDrive](#chipdrive)
| <br>              | [chipAction](#chipaction)
| <br>              | [chipGetSpeed](#chipgetspeed)
| <br>              | [chipSetSpeed](#chipsetspeed)
| LEDs              | [chipGetEyeBrightness](#chipgeteyebrightness)
| <br>              | [chipSetEyeBrightness](#chipseteyebrightness)
| Sound             | [chipPlaySound](#chipplaysound)
| <br>              | [chipStopSound](#chipstopsound)
| <br>              | [chipGetVolume](#chipgetvolume)
| <br>              | [chipSetVolume](#chipsetvolume)
| Battery / Charge  | [chipGetBatteryLevel](#chipgetbatterylevel)
| Time / Alarm      | [chipGetCurrentDateTime](#chipgetcurrentdatetime)
| <br>              | [chipSetCurrentDateTime](#chipsetcurrentdatetime)
| <br>              | [chipGetAlarmDateTime](#chipgetalarmdatetime)
| <br>              | [chipSetAlarmDateTime](#chipsetalarmdatetime)
| <br>              | [chipCancelAlarm](#chipcancelalarm)
| Version Info      | [chipGetDogVersion](#chipgetdogversion)
| Sleep             | [chipForceSleep](#chipforcesleep)
| Raw               | [chipRawSend](#chiprawsend)
| <br>              | [chipRawReceive](#chiprawreceive)
| <br>              | [chipRawReceiveNotification](#chiprawreceivenotification)


---
### chipInit
```CHiP* chipInit(const char* pInitOptions)```
#### Description
Is the first chip*() function that should be called by the developer.  It allocates and returns the CHiP* pointer used as the first parameter in all subsequent chip*() function calls.

#### Parameters
* **pInitOptions** is a character string which originates with the user.  It is transport specific.  At this point all supported transports ignore the parameter so it can be set to NULL.

#### Returns
* NULL on error.
* A valid pointer to a CHiP object otherwise.  This pointer is used as the first parameter in all subsequent chip*() function calls.


---
### chipUninit
```void chipUninit(CHiP* pCHiP)```
#### Description
Is the last chip*() function that should be called by the developer.  It is used to cleanly shutdown the transport and any other resources used by the CHiP API.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
Nothing


---
### chipConnectToRobot
```int chipConnectToRobot(CHiP* pCHiP, const char* pRobotName)```
#### Description
Called to connect to the desired CHiP robot.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pRobotName** is the name of the robot to which a connection should be made.  This parameter can be NULL to indicate the default robot should be used.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* If **pRobotName** is set to NULL then connect to the first CHiP robot discovered by BLE.
* A list of valid names for **pRobotName** can be found through the use of the [chipStartRobotDiscovery()](#chipstartrobotdiscovery), [chipGetDiscoveredRobotCount()](#chipgetdiscoveredrobotcount), [chipGetDiscoveredRobotName()](#chipgetdiscoveredrobotname), and [chipStopRobotDiscovery()](#chipstoprobotdiscovery) functions.

#### Example
```c
#include <stdio.h>
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
    int   result = -1;
    CHiP* pCHiP = chipInit(NULL);

    printf("\tConnect.c - Connect and disconnect from default CHiP.\n");

    // Connect to default CHiP robot.
    result = chipConnectToRobot(pCHiP, NULL);

    // Do stuff with the CHiP robot.

    // Disconnect and shutdown the application.
    result = chipDisconnectFromRobot(pCHiP);
    chipUninit(pCHiP);
}
```


---
### chipDisconnectFromRobot
```int chipDisconnectFromRobot(CHiP* pCHiP)```
#### Description
Called to disconnect from the current CHiP robot.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* Doesn't need to be called for a clean shutdown as [chipUninit()](#chipuninit) will take care of disconnecting from any active robots as part of the transport layer cleanup.
* This API exists incase the developer wants to explicitly disconnect from a CHiP during execution and connect to another.

#### Example
```c
#include <stdio.h>
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
    int   result = -1;
    CHiP* pCHiP = chipInit(NULL);

    printf("\tConnect.c - Connect and disconnect from default CHiP.\n");

    // Connect to default CHiP robot.
    result = chipConnectToRobot(pCHiP, NULL);

    // Do stuff with the CHiP robot.

    // Disconnect and shutdown the application.
    result = chipDisconnectFromRobot(pCHiP);
    chipUninit(pCHiP);
}
```


---
### chipStartRobotDiscovery
```int chipStartRobotDiscovery(CHiP* pCHiP)```
#### Description
Starts the process of discovering CHiP robots to which a connection can be made. This discovery process will continue until [chipStopRobotDiscovery()](#chipstoprobotdiscovery) is called.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* Once the discovery process has started, the [chipGetDiscoveredRobotCount()](#chipgetdiscoveredrobotcount) and [chipGetDiscoveredRobotName()](#chipgetdiscoveredrobotname) functions can be called to query the current list of robots.  Those functions can still be called after calling [chipStopRobotDiscovery()](#chipstoprobotdiscovery) but no new robots will be added to the list.

#### Example
```c
#include <stdio.h>
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
    int         result = -1;
    size_t      robotCount = 0;
    const char* pRobotName = NULL;
    CHiP*        pCHiP = chipInit(NULL);

    printf("\tDiscovery.c - Discover CHiP robots to which a connection can be made.\n"
           "\tShould see first CHiP robot be listed when it is powered up near\n"
           "\tthe test machine.\n");
    result = chipStartRobotDiscovery(pCHiP);
    do
    {
        result = chipGetDiscoveredRobotCount(pCHiP, &robotCount);
    } while (robotCount == 0);
    result = chipGetDiscoveredRobotName(pCHiP, 0, &pRobotName);
    printf("\tDiscovered %s\n", pRobotName);
    result = chipStopRobotDiscovery(pCHiP);

    chipUninit(pCHiP);
}
```

---
### chipGetDiscoveredRobotCount
```int chipGetDiscoveredRobotCount(CHiP* pCHiP, size_t* pCount)```
#### Description
Query how many CHiP robots the discovery process has found so far.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pCount** is pointer to where the current count of robots should be placed.  Shouldn't be NULL.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* The discovery process should be started by calling [chipStartRobotDiscovery()](#chipstartrobotdiscovery) before calling this function.
* The count returned by this function can increase (if more and more robots are discovered over time) until [chipStopRobotDiscovery()](#chipstoprobotdiscovery) is called.

#### Example
```c
#include <stdio.h>
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
    int         result = -1;
    size_t      robotCount = 0;
    const char* pRobotName = NULL;
    CHiP*        pCHiP = chipInit(NULL);

    printf("\tDiscovery.c - Discover CHiP robots to which a connection can be made.\n"
           "\tShould see first CHiP robot be listed when it is powered up near\n"
           "\tthe test machine.\n");
    result = chipStartRobotDiscovery(pCHiP);
    do
    {
        result = chipGetDiscoveredRobotCount(pCHiP, &robotCount);
    } while (robotCount == 0);
    result = chipGetDiscoveredRobotName(pCHiP, 0, &pRobotName);
    printf("\tDiscovered %s\n", pRobotName);
    result = chipStopRobotDiscovery(pCHiP);

    chipUninit(pCHiP);
}
```


---
### chipGetDiscoveredRobotName
```int chipGetDiscoveredRobotName(CHiP* pCHiP, size_t robotIndex, const char** ppRobotName)```
#### Description
Query the name of a specific CHiP robot which the discovery process has found.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **robotIndex** is the index of the robot for which the name should be obtained.  It must be >= 0 and < the count returned by [chipGetDiscoveredRobotCount()](#chipgetdiscoveredrobotcount).
* **ppRobotName** is a pointer to where the robot name should be placed.  Shouldn't be NULL.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* The discovery process should be started by calling [chipStartRobotDiscovery()](#chipstartrobotdiscovery) before calling this function.
* This function is used to index into the list of discovered robots to obtain its name.  This name can be later used as the **pRobotName** parameter of the [chipConnectToRobot()](#chipconnecttorobot) function.

#### Example
```c
#include <stdio.h>
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
    int         result = -1;
    size_t      robotCount = 0;
    const char* pRobotName = NULL;
    CHiP*        pCHiP = chipInit(NULL);

    printf("\tDiscovery.c - Discover CHiP robots to which a connection can be made.\n"
           "\tShould see first CHiP robot be listed when it is powered up near\n"
           "\tthe test machine.\n");
    result = chipStartRobotDiscovery(pCHiP);
    do
    {
        result = chipGetDiscoveredRobotCount(pCHiP, &robotCount);
    } while (robotCount == 0);
    result = chipGetDiscoveredRobotName(pCHiP, 0, &pRobotName);
    printf("\tDiscovered %s\n", pRobotName);
    result = chipStopRobotDiscovery(pCHiP);

    chipUninit(pCHiP);
}
```


---
### chipStopRobotDiscovery
```int chipStopRobotDiscovery(CHiP* pCHiP)```
#### Description
Stops the process of discovering CHiP robots to which a connection can be made.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* The discovery process is started with a call to [chipStartRobotDiscovery()](#chipstartrobotdiscovery) and stops when this function is called.  
* CHiP robots which were found between these two calls can be listed through the use of the [chipGetDiscoveredRobotCount()](#chipgetdiscoveredrobotcount) and [chipGetDiscoveredRobotName()](#chipgetdiscoveredrobotname) functions.

#### Example
```c
#include <stdio.h>
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
    int         result = -1;
    size_t      robotCount = 0;
    const char* pRobotName = NULL;
    CHiP*        pCHiP = chipInit(NULL);

    printf("\tDiscovery.c - Discover CHiP robots to which a connection can be made.\n"
           "\tShould see first CHiP robot be listed when it is powered up near\n"
           "\tthe test machine.\n");
    result = chipStartRobotDiscovery(pCHiP);
    do
    {
        result = chipGetDiscoveredRobotCount(pCHiP, &robotCount);
    } while (robotCount == 0);
    result = chipGetDiscoveredRobotName(pCHiP, 0, &pRobotName);
    printf("\tDiscovered %s\n", pRobotName);
    result = chipStopRobotDiscovery(pCHiP);

    chipUninit(pCHiP);
}
```


---
### chipDrive
```int chipDrive(CHiP* pCHiP, int8_t forwardReverse, int8_t leftRight, int8_t spin)```
#### Description
Instruct the CHiP to drive in a particular direction. The cool thing about the CHiP is that it is capable of omnidirectional motion where it can travel forward and reverse, strafe side to side, and spin in place. It can even be doing all 3 at once and this function allows you to control such motion.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **forwardReverse** is the forward/reverse velocity desired.  This parameter can have a value between -32 and 32, where -32 is maximum reverse velocity and 32 is maximum forward velocity.  A velocity of 0 indicates that no forward/reverse motion is desired at this time.
* **leftRight** is the left/right strafe desired. This parameter can have a value between -32 and 32, where -32 commands a maximum strafe to the left and 32 commands a maximum strafe to the right.  A value of 0 indicates that no strafing is desired at this time.
* **spin** is the amount of spin desired.  This parameter can have a value between -32 and 32, where -32 commands a maximum spin to the left and 32 commands a maximum spin to the right.  A value of 0 indicates that no left/right turning is desired at this time.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* This command must be sent at regular intervals to keep the CHiP robot moving in the desired direction.  This interval
  should be ~50 milliseconds.
* When sent at longer intervals the CHiP's motion will become more jerky as it thinks that there will not be another
  motion command coming so it starts to stop all motion and then starts moving again once the next command does finally
  arrive.

#### Example
```c
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

    printf("\tContinuousDrive.c - Use chipDrive() function.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    printf("Drive forward at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, 8, 0, 0);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);

    printf("Drive backward at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, -8, 0, 0);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);


    printf("Strafe right at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, 0, 8, 0);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);

    printf("Strafe left at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, 0, -8, 0);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);


    printf("Spin to the right at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, 0, 0, 8);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);
    printf("Spin to the left at 25%%\n");
    for (i = 0 ; i < 20 ; i++)
    {
        result = chipDrive(pCHiP, 0, 0, -8);
        // Pace out the drive commands by a bit.
        usleep(50000);
    }
    usleep(500000);
    
    chipUninit(pCHiP);
}
```

---
### chipAction
```int chipAction(CHiP* pCHiP, CHiPAction action)```
#### Description
Commands the CHiP to start performing one of its built-in actions such as sit, lie down, etc.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **action** is used to specify which of the built-in actions to start from the following list:

| Action |
|--------|
| CHIP_ACTION_RESET |
| CHIP_ACTION_SIT |
| CHIP_ACTION_LIE_DOWN |
| CHIP_ACTION_ALL_IDLE_MODE |
| CHIP_ACTION_DANCE |
| CHIP_ACTION_VR_TRAINING1 |
| CHIP_ACTION_VR_TRAINING2 |
| CHIP_ACTION_RESET2 |
| CHIP_ACTION_JUMP |
| CHIP_ACTION_YOGA |
| CHIP_ACTION_WATCH_COME |
| CHIP_ACTION_WATCH_FOLLOW |
| CHIP_ACTION_WATCH_FETCH |
| CHIP_ACTION_BALL_TRACKING |
| CHIP_ACTION_BALL_SOCCER |
| CHIP_ACTION_BASE |
| CHIP_ACTION_DANCE_BASE |
| CHIP_ACTION_STOP_OR_STAND_FROM_BASE |
| CHIP_ACTION_GUARD_MODE |
| CHIP_ACTION_FREE_ROAM |
| CHIP_ACTION_FACE_DOWN_FOR_CONTROLLING_CHIPPIES |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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

    printf("\tAction.c - Use chipAction().\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    printf("Sit!\n");
    result = chipAction(pCHiP, CHIP_ACTION_SIT);
    sleep(2);

    printf("Lie Down!\n");
    result = chipAction(pCHiP, CHIP_ACTION_LIE_DOWN);
    sleep(2);
    
    chipUninit(pCHiP);
}
```


---
### chipGetSpeed
```int chipGetSpeed(CHiP* pCHiP, CHiPSpeed* pSpeed)```
#### Description
Retrieves the current speed setting, **CHIP_SPEED_ADULT** or **CHIP_SPEED_KID**, of the CHiP. It can be changed with the [chipSetSpeed()](#chipsetspeed) function. The **CHIP_SPEED_KID** setting is used to slow down the CHiP and make it easier for young children to control.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pSpeed** is a pointer to an object to be filled in with the current speed setting of the CHiP. It will be set to one of the following:

| CHiPSpeed        | Description |
|------------------|-------------|
| CHIP_SPEED_ADULT | Full speed option |
| CHIP_SPEED_KID   | Reduced speed option, more suitable for young children |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
#include <stdio.h>
#include "chip.h"
#include "osxble.h"

static void printSpeed(CHiPSpeed speed);

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

    printf("\tSpeed.c - Use chipGetSpeed() and chipSetSpeed() functions to flip the speed setting.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read the current speed setting.
    CHiPSpeed speed;
    result = chipGetSpeed(pCHiP, &speed);
    printf("Original speed is ");
    printSpeed(speed);
    printf("\n");

    // Switch the speed.
    result = chipSetSpeed(pCHiP, speed == CHIP_SPEED_ADULT ? CHIP_SPEED_KID : CHIP_SPEED_ADULT);

    // Read the current speed setting.
    result = chipGetSpeed(pCHiP, &speed);
    printf("New speed is ");
    printSpeed(speed);
    printf("\n");

    chipUninit(pCHiP);
}

static void printSpeed(CHiPSpeed speed)
{
    switch (speed)
    {
    case CHIP_SPEED_ADULT:
        printf("Adult");
        break;
    case CHIP_SPEED_KID:
        printf("Kid");
        break;
    }
}
```


---
### chipSetSpeed
```int chipSetSpeed(CHiP* pCHiP, CHiPSpeed speed)```
#### Description
Sets the current speed setting, **CHIP_SPEED_ADULT** or **CHIP_SPEED_KID**, of the CHiP. The current setting can be retrieved by calling the [chipGetSpeed()](#chipgetspeed) function. The **CHIP_SPEED_KID** setting is used to slow down the CHiP and make it easier for young children to control.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **speed** is the speed to which the CHiP should now be set. It can be one of the following:

| CHiPSpeed        | Description |
|------------------|-------------|
| CHIP_SPEED_ADULT | Full speed option |
| CHIP_SPEED_KID   | Reduced speed option, more suitable for young children |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
#include <stdio.h>
#include "chip.h"
#include "osxble.h"

static void printSpeed(CHiPSpeed speed);

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

    printf("\tSpeed.c - Use chipGetSpeed() and chipSetSpeed() functions to flip the speed setting.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read the current speed setting.
    CHiPSpeed speed;
    result = chipGetSpeed(pCHiP, &speed);
    printf("Original speed is ");
    printSpeed(speed);
    printf("\n");

    // Switch the speed.
    result = chipSetSpeed(pCHiP, speed == CHIP_SPEED_ADULT ? CHIP_SPEED_KID : CHIP_SPEED_ADULT);

    // Read the current speed setting.
    result = chipGetSpeed(pCHiP, &speed);
    printf("New speed is ");
    printSpeed(speed);
    printf("\n");

    chipUninit(pCHiP);
}

static void printSpeed(CHiPSpeed speed)
{
    switch (speed)
    {
    case CHIP_SPEED_ADULT:
        printf("Adult");
        break;
    case CHIP_SPEED_KID:
        printf("Kid");
        break;
    }
}
```


---
### chipGetEyeBrightness
```int chipGetEyeBrightness(CHiP* pCHiP, uint8_t* pBrightness)```
#### Description
Requests the current setting for the brightness of CHiP's eye LEDs.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pBrightness** is a pointer to be filled in with the current eye brightness setting. The brightness values can vary from **0x00** to **0xFF** with the following values having special meanings:

| Brightness | Description |
|------------|-------------|
| 0x00       | Default brightness |
| 0x01       | Minimum brightness |
| 0xFF       | Maximum brightness |


#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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

    printf("\tSpeed.c - Use chipGetEyeBrightness() and chipSetEyeBrightness() functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read and display the current eye brightness setting.
    uint8_t originalBrightness;
    result = chipGetEyeBrightness(pCHiP, &originalBrightness);
    printf("Original eye brightness is 0x%02X\n", originalBrightness);

    // Set the current eye brightness to medium.
    result = chipSetEyeBrightness(pCHiP, 0x80);
    uint8_t currentBrightness;
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Updated eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x80) ? "pass" : "fail");
    sleep(2);

    // Set the current eye brightness to default (0x00).
    result = chipSetEyeBrightness(pCHiP, 0x00);
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Default eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x00) ? "pass" : "fail");
    sleep(2);

    // Set back to original brightness and exit.
    result = chipSetEyeBrightness(pCHiP, originalBrightness);

    chipUninit(pCHiP);
}
```


---
### chipSetEyeBrightness
```int chipSetEyeBrightness(CHiP* pCHiP, uint8_t brightness)```
#### Description
Sets the brightness to be used for CHiP's eye LEDs.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **brightness** indciates how bright CHiP's eye LEDs should be illuminated. The brightness values can vary from **0x00** to **0xFF** with the following values having special meanings:

| Brightness | Description |
|------------|-------------|
| 0x00       | Default brightness |
| 0x01       | Minimum brightness |
| 0xFF       | Maximum brightness |


#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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

    printf("\tSpeed.c - Use chipGetEyeBrightness() and chipSetEyeBrightness() functions.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Read and display the current eye brightness setting.
    uint8_t originalBrightness;
    result = chipGetEyeBrightness(pCHiP, &originalBrightness);
    printf("Original eye brightness is 0x%02X\n", originalBrightness);

    // Set the current eye brightness to medium.
    result = chipSetEyeBrightness(pCHiP, 0x80);
    uint8_t currentBrightness;
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Updated eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x80) ? "pass" : "fail");
    sleep(2);

    // Set the current eye brightness to default (0x00).
    result = chipSetEyeBrightness(pCHiP, 0x00);
    result = chipGetEyeBrightness(pCHiP, &currentBrightness);
    printf("Default eye brightness is 0x%02X - %s\n", currentBrightness, (currentBrightness == 0x00) ? "pass" : "fail");
    sleep(2);

    // Set back to original brightness and exit.
    result = chipSetEyeBrightness(pCHiP, originalBrightness);

    chipUninit(pCHiP);
}
```


---
### chipPlaySound
```int chipPlaySound(CHiP* pCHiP, CHiPSoundIndex sound)```
#### Description
Requests the CHiP to start playing one of its built-in sounds.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **sound** is the index of the built-in sound to start playing. It can be one of the following:

| CHiPSoundIndex |
|----------------|
| CHIP_SOUND_BARK_X1_ANGRY_A34 |
| CHIP_SOUND_BARK_X1_CURIOUS_PLAYFUL_HAPPY_A34 |
| CHIP_SOUND_BARK_X1_NEUTRAL_A34 |
| CHIP_SOUND_BARK_X1_SCARED_A34 |
| CHIP_SOUND_BARK_X2_ANGRY_A34 |
| CHIP_SOUND_BARK_X2_CURIOUS_PLAYFUL_HAPPY_A34 |
| CHIP_SOUND_BARK_X2_NEUTRAL_A34 |
| CHIP_SOUND_BARK_X2_SCARED_A34 |
| CHIP_SOUND_CRY_A34 |
| CHIP_SOUND_GROWL_A_A34 |
| CHIP_SOUND_GROWL_B_A34 |
| CHIP_SOUND_GROWL_C_A34 |
| CHIP_SOUND_HUH_LONG_A34 |
| CHIP_SOUND_HUH_SHORT_A34 |
| CHIP_SOUND_LICK_1_A34 |
| CHIP_SOUND_LICK_2_A34 |
| CHIP_SOUND_PANT_FAST_A34 |
| CHIP_SOUND_PANT_MEDIUM_A34 |
| CHIP_SOUND_PANT_SLOW_A34 |
| CHIP_SOUND_SNIFF_1_A34 |
| CHIP_SOUND_SNIFF_2_A34 |
| CHIP_SOUND_YAWN_A_A34 |
| CHIP_SOUND_YAWN_B_A34 |
| CHIP_SOUND_ONE_A34 |
| CHIP_SOUND_TWO_A34 |
| CHIP_SOUND_THREE_A34 |
| CHIP_SOUND_FOUR_A34 |
| CHIP_SOUND_FIVE_A34 |
| CHIP_SOUND_SIX_A34 |
| CHIP_SOUND_SEVEN_A34 |
| CHIP_SOUND_EIGHT_A34 |
| CHIP_SOUND_NIGHT_A34 |
| CHIP_SOUND_TEN_A34 |
| CHIP_SOUND_ZERO_A34 |
| CHIP_SOUND_CHIP_DOG_COUGH_2_A34 |
| CHIP_SOUND_CHIP_DOG_CRY_2_A34 |
| CHIP_SOUND_CHIP_DOG_CRY_3_A34 |
| CHIP_SOUND_CHIP_DOG_CRY_4_A34 |
| CHIP_SOUND_CHIP_DOG_CRY_5_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_1_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_2_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_3_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_EXCITED_1_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_EXCITED_2_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_EXCITED_3_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_1_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_2_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_3_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_1_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_2_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_RESPONSE_3_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_SCARED_YIP_2_A34 |
| CHIP_SOUND_CHIP_DOG_EMO_SCARED_YIP_3_A34 |
| CHIP_SOUND_CHIP_DOG_FART_1_A34 |
| CHIP_SOUND_CHIP_DOG_FART_2_A34 |
| CHIP_SOUND_CHIP_DOG_FART_3_A34 |
| CHIP_SOUND_CHIP_DOG_GROWL_1_A34 |
| CHIP_SOUND_CHIP_DOG_GROWL_2_A34 |
| CHIP_SOUND_CHIP_DOG_GROWL_4_A34 |
| CHIP_SOUND_CHIP_DOG_GROWL_5_A34 |
| CHIP_SOUND_CHIP_DOG_HICCUP_1_A34 |
| CHIP_SOUND_CHIP_DOG_HICCUP_2_A34 |
| CHIP_SOUND_CHIP_DOG_HOWL_1_A34 |
| CHIP_SOUND_CHIP_DOG_HOWL_2_A34 |
| CHIP_SOUND_CHIP_DOG_HOWL_3_A34 |
| CHIP_SOUND_CHIP_DOG_HOWL_4_A34 |
| CHIP_SOUND_CHIP_DOG_HOWL_5_A34 |
| CHIP_SOUND_CHIP_DOG_LICK_2_A34 |
| CHIP_SOUND_CHIP_DOG_LICK_3_A34 |
| CHIP_SOUND_CHIP_DOG_LOWBATTERY_1_A34 |
| CHIP_SOUND_CHIP_DOG_LOWBATTERY_2_A34 |
| CHIP_SOUND_CHIP_DOG_MUFFLE_1_A34 |
| CHIP_SOUND_CHIP_DOG_MUFFLE_2_A34 |
| CHIP_SOUND_CHIP_DOG_MUFFLE_3_A34 |
| CHIP_SOUND_CHIP_DOG_PANT_1_A34 |
| CHIP_SOUND_CHIP_DOG_PANT_2_A34 |
| CHIP_SOUND_CHIP_DOG_PANT_3_A34 |
| CHIP_SOUND_CHIP_DOG_PANT_4_A34 |
| CHIP_SOUND_CHIP_DOG_PANT_5_L_A34 |
| CHIP_SOUND_CHIP_DOG_SMOOCH_1_A34 |
| CHIP_SOUND_CHIP_DOG_SMOOCH_2_A34 |
| CHIP_SOUND_CHIP_DOG_SMOOCH_3_A34 |
| CHIP_SOUND_CHIP_DOG_SNEEZE_1_A34 |
| CHIP_SOUND_CHIP_DOG_SNEEZE_2_A34 |
| CHIP_SOUND_CHIP_DOG_SNEEZE_3_A34 |
| CHIP_SOUND_CHIP_DOG_SNIFF_1_A34 |
| CHIP_SOUND_CHIP_DOG_SNIFF_2_A34 |
| CHIP_SOUND_CHIP_DOG_SNIFF_4_A34 |
| CHIP_SOUND_CHIP_DOG_SNORE_1_A34 |
| CHIP_SOUND_CHIP_DOG_SNORE_2_A34 |
| CHIP_SOUND_CHIP_DOG_SPECIAL_1_A34 |
| CHIP_SOUND_CHIP_SING_DO1_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_DO2_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_FA_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_LA_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_MI_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_RE_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_SO_SHORT_A34 |
| CHIP_SOUND_CHIP_SING_TI_SHORT_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_3_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_4_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_5_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_MULTI_3_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_MULTI_4_A34 |
| CHIP_SOUND_CHIP_DOG_BARK_MULTI_5_A34 |
| CHIP_SOUND_CHIP_DOG_BURP_1_A34 |
| CHIP_SOUND_CHIP_DOG_BURP_2_A34 |
| CHIP_SOUND_CHIP_DOG_COUGH_1_A34 |
| CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_3A |
| CHIP_SOUND_CHIP_DEMO_MUSIC_2 |
| CHIP_SOUND_CHIP_DEMO_MUSIC_3 |
| CHIP_SOUND_CHIP_DOG_BARK_1 |
| CHIP_SOUND_CHIP_DOG_BARK_2 |
| CHIP_SOUND_CHIP_DOG_BARK_MULTI_1 |
| CHIP_SOUND_CHIP_DOG_BARK_MULTI_2 |
| CHIP_SOUND_CHIP_DOG_CRY_1 |
| CHIP_SOUND_CHIP_DOG_EMO_CURIOUS_2A |
| CHIP_SOUND_CHIP_DOG_EMO_EXCITED_3A |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_1A |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_2A |
| CHIP_SOUND_CHIP_DOG_EMO_LAZY_3A |
| CHIP_SOUND_CHIP_DOG_GROWL_3 |
| CHIP_SOUND_CHIP_DOG_HOWL_1A |
| CHIP_SOUND_CHIP_DOG_HOWL_3A |
| CHIP_SOUND_CHIP_DOG_HOWL_4A |
| CHIP_SOUND_CHIP_DOG_HOWL_5A |
| CHIP_SOUND_CHIP_DOG_LICK_1 |
| CHIP_SOUND_CHIP_DOG_LOWBATTERY_1A |
| CHIP_SOUND_CHIP_DOG_LOWBATTERY_2A |
| CHIP_SOUND_CHIP_DOG_MUFFLE_1A |
| CHIP_SOUND_CHIP_DOG_SMOOCH_3A |
| CHIP_SOUND_CHIP_DOG_SNEEZE_1A |
| CHIP_SOUND_CHIP_DOG_SNIFF_3 |
| CHIP_SOUND_CHIP_DOG_SNIFF_4A |
| CHIP_SOUND_CHIP_MUSIC_DEMO_1 |
| CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_1A |
| CHIP_SOUND_CHIO_DOG_EMO_RESPONSE_2A |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
    printf("Growling...\n");
    result = chipPlaySound(pCHiP, CHIP_SOUND_GROWL_A_A34);

    sleep(1);

    printf("Stop growling!\n");
    result = chipStopSound(pCHiP);

    sleep(2);

    printf("Licking...\n");
    result = chipPlaySound(pCHiP, CHIP_SOUND_LICK_1_A34);

    sleep(5);

    chipUninit(pCHiP);
}
```


---
### chipStopSound
```int chipStopSound(CHiP* pCHiP)```
#### Description
Requests that the CHiP immediately stop whatever sound it is currently playing.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
    printf("Growling...\n");
    result = chipPlaySound(pCHiP, CHIP_SOUND_GROWL_A_A34);

    sleep(1);

    printf("Stop growling!\n");
    result = chipStopSound(pCHiP);

    sleep(2);

    printf("Licking...\n");
    result = chipPlaySound(pCHiP, CHIP_SOUND_LICK_1_A34);

    sleep(5);

    chipUninit(pCHiP);
}
```


---
### chipGetVolume
```int chipGetVolume(CHiP* pCHiP, uint8_t* pVolume)```
#### Description
Retrieves the current volume level setting of the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pVolume** is a pointer to be filled in with the current volume setting. It will range from **1** (mute) to **11** (full volume).

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
    uint8_t volume = 0;
    CHiP*   pCHiP = chipInit(NULL);

    printf("\tVolume.c - Use chipSet/GetVolume().\n"
           "\tSet volume level to 2 and read out afterwards.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipSetVolume(pCHiP, 2);
    result = chipGetVolume(pCHiP, &volume);

    printf("Volume = %u\n", volume);

    chipUninit(pCHiP);
}
```


---
### chipSetVolume
```int chipSetVolume(CHiP* pCHiP, uint8_t volume)```
#### Description
Sets the current volume level of the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **volume** is the new volume level to which the CHiP should be set. It should range from **1** (mute) to **11** (full volume).

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
    uint8_t volume = 0;
    CHiP*   pCHiP = chipInit(NULL);

    printf("\tVolume.c - Use chipSet/GetVolume().\n"
           "\tSet volume level to 2 and read out afterwards.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipSetVolume(pCHiP, 2);
    result = chipGetVolume(pCHiP, &volume);

    printf("Volume = %u\n", volume);

    chipUninit(pCHiP);
}
```


---
### chipGetBatteryLevel
```int chipGetBatteryLevel(CHiP* pCHiP, CHiPBatteryLevel* pBatteryLevel)```
#### Description
Retrieves the current battery level and charging state from the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pBatteryLevel** is a pointer to an object to be filled in with the CHiP's current battery state. This object includes the following properties:

| Property       | Value                                  | Description |
|----------------|----------------------------------------|-------------|
| batteryLevel   | 0.0                                    | Battery is empty|
| <br>           | 1.0                                    | Battery is fully charged |
| chargingStatus | CHIP_CHARGING_STATUS_NOT_CHARGING      | CHiP isn't currently being charged |
| <br>           | CHIP_CHARGING_STATUS_CHARGING          | CHiP is currently being charged. See chargerType property to know which charger is being used |
| <br>           | CHIP_CHARGING_STATUS_CHARGING_FINISHED | CHiP is still on charger but is now fully charged |
| chargerType    | CHIP_CHARGER_TYPE_DC                   | CHiP is being charged from DC barrel jack in chest |
| <br>           | CHIP_CHARGER_TYPE_BASE                 | CHiP is being charged in base station (SmartBed) |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
#include <stdio.h>
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

    printf("\tBatteryLevel.c - Use chipGetBatteryLevel().\n");

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
```


---
### chipGetCurrentDateTime
```int chipGetCurrentDateTime(CHiP* pCHiP, CHiPCurrentDateTime* pDateTime)```
#### Description
Requests the current date and time from the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pDateTime** is a pointer to an object to be filled in with the current date and time. This object has the following properties:

| Field     | Description |
|-----------|-------------|
| year      | The current year |
| month     | The current month (1-January to 12-December) |
| day       | The current day of month (1 to 31) |
| hour      | The current hour (0 - 23) |
| minute    | The current minute (0 - 59) |
| second    | The current second (0 - 59) |
| dayOfWeek | The current day of week (0-Sunday to 6-Saturday) |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* The current date and time is only maintained while the CHiP is powered on. Powering off the CHiP will cause the date and time to be reset.

#### Example
```c
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
```


---
### chipSetCurrentDateTime
```int chipSetCurrentDateTime(CHiP* pCHiP, const CHiPCurrentDateTime* pDateTime);```
#### Description
Sets the current date and time on the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pDateTime** is a pointer to an object containing the new current date and time. This object has the following properties:

| Field     | Description |
|-----------|-------------|
| year      | The current year |
| month     | The current month (1-January to 12-December) |
| day       | The current day of month (1 to 31) |
| hour      | The current hour (0 - 23) |
| minute    | The current minute (0 - 59) |
| second    | The current second (0 - 59) |
| dayOfWeek | The current day of week (0-Sunday to 6-Saturday) |

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* The current date and time is only maintained while the CHiP is powered on. Powering off the CHiP will cause the date and time to be reset.
* See the following example for how to set CHiP's current date & time to match the host.

#### Example
```c
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
```


---
### chipGetAlarmDateTime
```int chipGetAlarmDateTime(CHiP* pCHiP, CHiPAlarmDateTime* pDateTime)```
#### Description
Retrieves the current alarm setting from the CHiP. When the alarm date/time arrives, the MiP will start barking and not stop until the user pets its head.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pDateTime** is an object to be filled in with the CHiP's current alarm setting. This object has the following fields:

| Field     | Description |
|-----------|-------------|
| year      | The current year |
| month     | The current month (1-January to 12-December) |
| day       | The current day of month (1 to 31) |
| hour      | The current hour (0 - 23) |
| minute    | The current minute (0 - 59) |

#### Notes
* The alarm is only maintained while the CHiP is powered on. Powering off the CHiP will cause the alarm to be cleared.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
```


---
### chipSetAlarmDateTime
```int chipSetAlarmDateTime(CHiP* pCHiP, const CHiPAlarmDateTime* pDateTime)```
#### Description
Sets the current alarm on the CHiP. When this alarm date/time arrives, the MiP will start barking and not stop until the user pets its head.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pDateTime** is an object with the desired date/time for the CHiP's alarm to activate. This object has the following fields:

| Field     | Description |
|-----------|-------------|
| year      | The current year |
| month     | The current month (1-January to 12-December) |
| day       | The current day of month (1 to 31) |
| hour      | The current hour (0 - 23) |
| minute    | The current minute (0 - 59) |

#### Notes
* The alarm is only maintained while the CHiP is powered on. Powering off the CHiP will cause the alarm to be cleared.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
```


---
### chipCancelAlarm
```int chipCancelAlarm(CHiP* pCHiP)```
#### Description
Clears the current alarm on the CHiP.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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
```


---
### chipGetDogVersion
```int chipGetDogVersion(CHiP* pCHiP, CHiPDogVersion* pVersion)```
#### Description
Requests the CHiP's version information.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pVersion** is a pointer to an object to be filled in with the CHiP's version information.

```c
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
```

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
#include <stdio.h>
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
    int            result = -1;
    CHiPDogVersion version;
    CHiP*          pCHiP = chipInit(NULL);

    printf("\tDogVersion.c - Use chipGetDogVersion() function.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipGetDogVersion(pCHiP, &version);

    printf("bodyHardware: %u\n"
           "headHardware: %u\n"
           "mechanic: %u\n"
           "bleSpiFlash: %u\n"
           "nuvotonSpiFlash: %u\n"
           "bleBootloader: %u\n"
           "bleApromFirmware: %u\n"
           "nuvotonBootloaderFirmware: %u\n"
           "nuvotonApromFirmware: %u\n"
           "nuvoton: %u\n",
           version.bodyHardware,
           version.headHardware,
           version.mechanic,
           version.bleSpiFlash,
           version.nuvotonSpiFlash,
           version.bleBootloader,
           version.bleApromFirmware,
           version.nuvotonBootloaderFirmware,
           version.nuvotonApromFirmware,
           version.nuvoton);

    chipUninit(pCHiP);
}
```


---
### chipForceSleep
```int chipForceSleep(CHiP* pCHiP)```
#### Description
Requests that the CHiP switch into sleep mode.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Example
```c
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

    printf("\tSleep.c - Putting CHiP to sleep.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    result = chipForceSleep(pCHiP);

    // Leave some time for command to make it through before disconnecting.
    sleep(2);
    
    chipUninit(pCHiP);
}
```


---
### chipRawSend
```int chipRawSend(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength)```
#### Description
Send a raw command to the CHiP robot.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pRequest** is a pointer to the array of the command bytes to be sent to the robot.
* **requestLength** is the number of bytes in the pRequest buffer to be sent to the robot.

#### Returns
* **CHIP_ERROR_NONE** on success.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* There are typically higher level APIs that can be used to send commands to the CHiP but in cases where the functionality you need isn't already implemented in the CHiP C API, you can use this function to send the raw command bytes directly to the CHiP.

#### Example
```c
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

#define CHIP_CMD_SET_EYE_BRIGHTNESS      0x48
#define CHIP_CMD_GET_EYE_BRIGHTNESS      0x49

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

    printf("\tRawSendReceive.c - Use chipRaw*() functions to blink CHiP's eyes.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Manually request the current eye brightness and restore to it later.
    static const uint8_t getBrightness[1] = { CHIP_CMD_GET_EYE_BRIGHTNESS };
    uint8_t              response[1+1];
    size_t               responseLength;

    result = chipRawReceive(pCHiP, getBrightness, sizeof(getBrightness), response, sizeof(response), &responseLength);
    uint8_t originalBrightness = response[1];
    printf("Original brighness = %u\n", originalBrightness);

    // Blink the eyes by dimming and brightening the eyes a few times.
    uint8_t command[1+1];
    command[0] = CHIP_CMD_SET_EYE_BRIGHTNESS;
    for (int i = 0 ; i < 5 ; i++)
    {
        command[1] = 0x40;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);

        command[1] = 0xFF;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);
    }

    // Restore the brightness to original setting (probably 0, default).
    command[1] = originalBrightness;
    result = chipRawSend(pCHiP, command, sizeof(command));
    usleep(250000);

    chipUninit(pCHiP);
}
```


---
### chipRawReceive
```int chipRawReceive(CHiP* pCHiP, const uint8_t* pRequest, size_t requestLength, uint8_t* pResponseBuffer, size_t responseBufferSize, size_t* pResponseLength)```
#### Description
Send a raw request to the CHiP and receive its raw response.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pRequest** is a pointer to the array of the command bytes to be sent to the robot.
* **requestLength** is the number of bytes in the pRequest buffer to be sent to the robot.
* **pResponseBuffer** is a pointer to the array of bytes into which the response should be copied.
* **responseBufferSize** is the number of bytes in the pResponseBuffer.
* **pResponseLength** is a pointer to where the actual number of bytes in the response should be placed.  This value may be truncated to responseBufferSize if the actual response was > responseBufferSize.

#### Returns
* **CHIP_ERROR_NONE** on success.
* **CHIP_ERROR_TIMEOUT** if CHiP doesn't respond to request after multiple retries.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* There are typically higher level APIs that can be used to send requests to the CHiP but in cases where the functionality you need isn't already implemented in the CHiP C API, you can use this function to send the raw request to the CHiP and receive the raw resulting bytes.

#### Example
```c
#include <stdio.h>
#include <unistd.h>
#include "chip.h"
#include "osxble.h"

#define CHIP_CMD_SET_EYE_BRIGHTNESS      0x48
#define CHIP_CMD_GET_EYE_BRIGHTNESS      0x49

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

    printf("\tRawSendReceive.c - Use chipRaw*() functions to blink CHiP's eyes.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Manually request the current eye brightness and restore to it later.
    static const uint8_t getBrightness[1] = { CHIP_CMD_GET_EYE_BRIGHTNESS };
    uint8_t              response[1+1];
    size_t               responseLength;

    result = chipRawReceive(pCHiP, getBrightness, sizeof(getBrightness), response, sizeof(response), &responseLength);
    uint8_t originalBrightness = response[1];
    printf("Original brighness = %u\n", originalBrightness);

    // Blink the eyes by dimming and brightening the eyes a few times.
    uint8_t command[1+1];
    command[0] = CHIP_CMD_SET_EYE_BRIGHTNESS;
    for (int i = 0 ; i < 5 ; i++)
    {
        command[1] = 0x40;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);

        command[1] = 0xFF;
        result = chipRawSend(pCHiP, command, sizeof(command));
        usleep(250000);
    }

    // Restore the brightness to original setting (probably 0, default).
    command[1] = originalBrightness;
    result = chipRawSend(pCHiP, command, sizeof(command));
    usleep(250000);

    chipUninit(pCHiP);
}
```


---
### chipRawReceiveNotification
```int chipRawReceiveNotification(CHiP* pCHiP, uint8_t* pNotifyBuffer, size_t notifyBufferSize, size_t* pNotifyLength)```
#### Description
Get an out of band notification sent by the CHiP robot.

#### Parameters
* **pCHiP** is an object that was previously returned from the [chipInit()](#chipinit) call.
* **pNotifyBuffer** is a pointer to the array of bytes into which the notification should be copied.
* **notifyBufferSize** is the number of bytes in the pNotifyBuffer.
* **pNotifyLength** is a pointer to where the actual number of bytes in the notification should be placed.  This value may be truncated to notifyBufferSize if the actual response was > notifyBufferSize.

#### Returns
* **CHIP_ERROR_NONE** on success.
* **CHIP_ERROR_EMPTY** if there are currently no notifications to retrieve.
* Non-zero CHIP_ERROR_* code otherwise.

#### Notes
* Sometimes the CHiP robot sends notifications which aren't in direct response to the last request made.  This function will return one of these responses/notifications.
* _I don't currently know how to interpret these out of band notification events from the CHiP. If you figure them out, enter an Issue here on GitHub and let me know._

#### Example
```c
#include <stdio.h>
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
    size_t  responseLength = 0;
    uint8_t response[CHIP_RESPONSE_MAX_LEN];
    CHiP*   pCHiP = chipInit(NULL);

    printf("\tRawReceiveNotification.c - Use chipRawReceiveNotification() functions.\n"
           "\tDisplay notifications as they arrive. Press CTRL+C to terminate.\n");

    // Connect to first CHiP robot discovered.
    result = chipConnectToRobot(pCHiP, NULL);

    // Wait for first out of band notification to arrive.
    while (1)
    {
        if (CHIP_ERROR_EMPTY != chipRawReceiveNotification(pCHiP, response, sizeof(response), &responseLength))
        {
            // Display notification contents.
            printf("notification -> ");
            for (int i = 0 ; i < responseLength ; i++)
            {
                printf("%02X", response[i]);
            }
            printf("\n");
        }
    }

    chipUninit(pCHiP);
}
```
