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
/* This header file describes the API that the CHiP C API uses to communicate with the transport specific layer. */
#ifndef CHIP_TRANSPORT_H_
#define CHIP_TRANSPORT_H_

#include <stdint.h>
#include <stdlib.h>

// expectResponse parameter values for chipTransportSendRequest() parameter.
#define CHIP_EXPECT_NO_RESPONSE 0
#define CHIP_EXPECT_RESPONSE    1

// An abstract object type used by the CHiP API to provide transport specific information to each transport function.
// It will be initially created by a call to chipTransportInit() and then passed in as the first parameter to each of the
// other chipTransport*() functions.  It can be freed at the end with a call to chipTransportUninit;
typedef struct CHiPTransport CHiPTransport;

// Initialize a CHiPTransport object.
// Will be the first chipTransport*() function called so it can be used for any setup that the transport needs to take
// care of.  Transport specific data can be stored in the returned in the object pointed to by the returned pointer.
//
//   pInitOptions: A character string which originates with the user.  It could be used for things like serial port,
//                 etc.
//   Returns: NULL on error.
//            A valid pointer to a transport specific object otherwise.
CHiPTransport* chipTransportInit(const char* pInitOptions);

// Cleanup a CHiPTransport object.
// Will be the last chipTransport*() function called so it can be used to cleanly shutdown any transport resources.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
void          chipTransportUninit(CHiPTransport* pTransport);

// Connect to a CHiP robot.
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   pRobotName: The name of the robot to which a connection should be made.  This parameter can be NULL to indicate
//               that the first robot discovered.  A list of valid names can be found through the use of the
//               chipTransportStartRobotDiscovery(), chipTransportGetDiscoveredRobotCount(),
//               chipTransportGetDiscoveredRobotName(), and chipTransportStopRobotDiscovery() functions.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportConnectToRobot(CHiPTransport* pTransport, const char* pRobotName);

// Disconnect from CHiP robot.
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportDisconnectFromRobot(CHiPTransport* pTransport);

// Start the process of discovering CHiP robots to which a connection can be made.
// This discovery process will continue until chipTransportStopRobotDiscovery() is called.  Once the discovery process
// has started, the chipTransportGetDiscoveredRobotCount() and chipTransportGetDiscoveredRobotName() functions can be
// called to query the current list of robots.  Those functions can still be called after calling
// chipTransportStopRobotDiscovery() but no new robots will be added to the list.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportStartRobotDiscovery(CHiPTransport* pTransport);

// Query how many CHiP robots the discovery process has found so far.
// The discovery process is started by calling chipTransportStartRobotDiscovery().  The count returned by this function
// can increase (if more and more robots are discovered over time) until chipTransportStopRobotDiscovery() is called.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   pCount: A pointer to where the current count of robots should be placed.  Shouldn't be NULL.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportGetDiscoveredRobotCount(CHiPTransport* pTransport, size_t* pCount);

// Query the name of a specific CHiP robot which the discovery process has found.
// The discovery process is started by calling chipTransportStartRobotDiscovery().  This function is used to index into
// the list of discovered robots to obtain its name.  This name can be later used as the pRobotName parameter of the
// chipTransportConnectToRobot() function.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   robotIndex: The index of the robot for which the name should be obtained.  It must be >= 0 and < the count returned
//               by chipTransportGetDiscoveredRobotCount().
//   ppRobotName: A pointer to where the robot name should be placed.  Shouldn't be NULL.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportGetDiscoveredRobotName(CHiPTransport* pTransport, size_t robotIndex, const char** ppRobotName);

// Stops the process of discovering CHiP robots to which a connection can be made.
// The discovery process is started with a call to chipTransportStartRobotDiscovery() and stops when this function is
// called.  CHiP robots which were found between these two calls can be listed through the use of the
// chipTransportGetDiscoveredRobotCount() and chipTransportGetDiscoveredRobotName() functions.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportStopRobotDiscovery(CHiPTransport* pTransport);

// Send a request to the CHiP robot.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   pRequest: Is a pointer to the array of bytes to be sent to the robot.
//   requestLength: Is the number of bytes in the pRequest buffer to be sent to the robot.
//   expectResponse: Set to 0 if the robot is not expected to send a response to this request.  Set to non-zero if the
//                   robot will send a response to this request - a response which can be read by a subsequent call to
//                   chipTransportGetResponse().
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportSendRequest(CHiPTransport* pTransport, const uint8_t* pRequest, size_t requestLength, int expectResponse);

// Retrieve the response from the CHiP robot for the last request made.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   pResponseBuffer: Is a pointer to the array of bytes into which the response should be copied.
//   responseBufferSize: Is the number of bytes in the pResponseBuffer.
//   pResponseLength: Is a pointer to where the actual number of bytes in the response should be placed.  This value
//                    may be truncated to responseBufferSize if the actual response was > responseBufferSize.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportGetResponse(CHiPTransport* pTransport,
                            uint8_t* pResponseBuffer,
                            size_t responseBufferSize,
                            size_t* pResponseLength);

// Has the robot yet responded to the last request made?
//
//   Returns: 0 if still waiting for the response which means that a call to chipTransportGetResponse() would block
//                waiting for the response to arrive.
//            non-zero if the response has been received.
int chipTransportIsResponseAvailable(CHiPTransport* pTransport);


// Get an out of band response sent by the CHiP robot.
// Sometimes the CHiP robot sends notifications which aren't in direct response to the last request made.  This
// function will return one of these responses/notifications.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   pResponseBuffer: Is a pointer to the array of bytes into which the response should be copied.
//   responseBufferSize: Is the number of bytes in the pResponseBuffer.
//   pResponseLength: Is a pointer to where the actual number of bytes in the response should be placed.  This value
//                    may be truncated to responseBufferSize if the actual response was > responseBufferSize.
//   Returns: CHIP_ERROR_NONE on success and a non-zero CHIP_ERROR_* code otherwise.
int chipTransportGetOutOfBandResponse(CHiPTransport* pTransport,
                                     uint8_t* pResponseBuffer,
                                     size_t responseBufferSize,
                                     size_t* pResponseLength);

// Get the number of milliseconds the computer has been up and running using transport / platform specific
// functionality.
//
//   pTransport: An object that was previously returned from the chipTransportInit() call.
//   Returns: Millisecond count.
uint32_t chipTransportGetMilliseconds(CHiPTransport* pTransport);

#endif // CHIP_TRANSPORT_H_
