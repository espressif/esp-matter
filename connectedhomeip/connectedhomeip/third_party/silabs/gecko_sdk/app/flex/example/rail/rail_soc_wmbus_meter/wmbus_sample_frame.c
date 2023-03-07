/***************************************************************************//**
 * @file
 * @brief wmbus_sample_frame.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "wmbus_sample_frame.h"
#include <string.h>

//address consts
static const char manuf[3] = { 'S', 'I', 'L' }; //FLAG assigned for Silicon Labs
static const WMBUS_deviceType_t deviceType = WMBUS_DEVICE_WATER_METER;
static const uint8_t version = 0x01;
static const uint32_t devId = 0x01; //BCD

static void setupDLLHeader(void *buffer, bool isFrameB, uint16_t frameLength, WMBUS_functionCode_t functionCode, bool primary, bool fcvDcf, bool fcbAcd)
{
  WMBUS_dll_header_t *dllHeader = (WMBUS_dll_header_t*)buffer;
  memset(dllHeader, 0, sizeof(WMBUS_dll_header_t));//clear the header
  dllHeader->cField.detailed.primary = primary;
  dllHeader->cField.detailed.fcbAcd = fcbAcd;
  dllHeader->cField.detailed.fcvDcf = fcvDcf;
  dllHeader->cField.detailed.functionCode = functionCode;
  dllHeader->address.detailed.manufacturer = WMBUSframe_Chars2MField(manuf);
  dllHeader->address.detailed.id = devId;
  dllHeader->address.detailed.version = version;
  dllHeader->address.detailed.deviceType = deviceType;

  frameLength -= 1; //l field shouldn't be counted
  if ( isFrameB ) {
    if ( frameLength < 125 ) {
      dllHeader->lField = (uint8_t)(frameLength + 2); //2block, 1 crc
    } else {
      dllHeader->lField = (uint8_t)(frameLength + 4); //3block, 2 crc
    }
  } else {
    dllHeader->lField = (uint8_t)frameLength;
  }
}

static uint16_t setupSTLHeader(void *buffer, uint8_t mode, uint8_t ciField, uint8_t accessNumber, WMBUS_accessibility_t accessibility, bool synchronized, void *payload, uint16_t payloadLength)
{
  (void) ciField;
  WMBUS_stl_header_t *stlHeader = (WMBUS_stl_header_t*)buffer;
  memset(stlHeader, 0, sizeof(WMBUS_stl_header_t));//clear the header
  stlHeader->ciField = WMBUS_CI_EN13757_3_APPLICATION_SHORT;
  stlHeader->accessNumber = accessNumber;
  stlHeader->confWord.mode_0_5.mode = mode;
  stlHeader->confWord.mode_0_5.accessibility = accessibility;
  stlHeader->confWord.mode_0_5.synchronized = synchronized;
  if ( mode == 5 ) {
    uint8_t iv[16];
    uint16_t mf = WMBUSframe_Chars2MField(manuf);
    //with long transport layer header, the address from the header should be used
    memcpy(iv, &mf, 2);
    memcpy(iv + 2, &devId, 4);
    memcpy(iv + 6, &version, 1);
    memcpy(iv + 7, &deviceType, 1);
    memset(iv + 8, accessNumber, 8);
    stlHeader->confWord.mode_0_5.numOfEncBlocks = WMBUSframe_crypto5encrypt(payload, payload, iv, payloadLength, 0);
    return 16 * stlHeader->confWord.mode_0_5.numOfEncBlocks; //payloadlength probably changed due to padding
  } else {
    return payloadLength;
  }
}

uint16_t WMBUS_SAMPLE_setupFrame(uint8_t *buffer, uint8_t accessNumber, WMBUS_accessibility_t accessibility, int32_t volume, int16_t volumeFlow, bool periodic, bool encrypt)
{
  uint8_t *payloadPtr = buffer + sizeof(WMBUS_dll_header_t) + sizeof(WMBUS_stl_header_t);
  uint8_t *stlPtr = buffer + sizeof(WMBUS_dll_header_t);
  uint8_t *dllPtr = buffer;

  //setup the payload before the headers, so we'll know the length
  uint16_t frameLength = 0;
  if ( encrypt ) { //2B AES check
    frameLength += WMBUSframe_addIdleFiller(payloadPtr + frameLength, 2);
  }
  //VIF 0x13 is volume in m^3
  frameLength += WMBUSframe_addPayloadI32(payloadPtr + frameLength, WMBUS_DIB_FUNCTION_INSTANTANEOUS, 0x13, volume);
  //VIF 0x3B is volume flow in m^3
  frameLength += WMBUSframe_addPayloadI16(payloadPtr + frameLength, WMBUS_DIB_FUNCTION_INSTANTANEOUS, 0x3B, volumeFlow);

  //continue with STL header, since encryption could change the length
  frameLength = setupSTLHeader(stlPtr, encrypt ? 5 : 0, WMBUS_CI_EN13757_3_APPLICATION_SHORT, accessNumber,
                               accessibility, periodic, payloadPtr, frameLength);

  //add the header lengths to frameLnegth
  frameLength += sizeof(WMBUS_dll_header_t) + sizeof(WMBUS_stl_header_t);
  //finally, set up the DLL header
  setupDLLHeader(dllPtr, false, frameLength, WMBUS_FUNCTION_SND_NR, true, false, false);

  return frameLength;
}
