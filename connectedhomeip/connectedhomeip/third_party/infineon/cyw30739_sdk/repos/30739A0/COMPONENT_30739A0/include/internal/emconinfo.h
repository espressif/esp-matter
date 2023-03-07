/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#ifndef _EMCONINFO_H_
#define _EMCONINFO_H_

//////////////////////////////////////////////////////////////////////////////
//    this file defined the embedded controller info
//    this data structure contains all the information about the device.
#include "brcm_fw_types.h"


#ifdef __cplusplus
extern "C" {
#endif


#define EMCONINFO_FLAG_BONDED_DEVICE                  0x1
#define EMCONINFO_FLAG_LINK_ENCRYPTED                 0x2

#define BD_ADDRESS_SIZE     6
//
typedef struct
{
    // 4 bytes aligned.
    UINT16          connHandle;
    UINT8           flag;

    UINT8           peerAddressType;

    // 4 bytes aligned
    UINT8           peerAddress[BD_ADDRESS_SIZE]; // this is 6 bytes peer address.
    UINT16          connInterval; // connection interval in frames.

    // 4 bytes aligned
    UINT16          connLatency;
    UINT16          supervisionTimeout;

} EMCONINFO_DEVINFO;

// all the macros need this variable to be exported.
extern EMCONINFO_DEVINFO emConInfo_devInfo;

//////////////////////////////////////////////////////////////////////////////
//                          public interfaces.
void emconinfo_init(EMCONINFO_DEVINFO *pdevInfo);

void emconinfo_setPtr(EMCONINFO_DEVINFO *pdevInfo);
EMCONINFO_DEVINFO *emconinfo_getPtr(void);

int  emconinfo_getConnHandle(void);
void emconinfo_setConnHandle(UINT16 connHandle);

UINT8 *emconninfo_getPeerAddr(void);

void emconninfo_setPeerAddrType(int type);
int  emconninfo_getPeerAddrType(void);

int emconninfo_getConnInterval(void);
void emconninfo_setConnInterval(int interval);

int  emconninfo_getPeripheralLatency(void);
void emconninfo_setPeripheralLatency(int latency);

int emconninfo_getSupervisionTimeout(void);
void emconninfo_setSupervisionTimeout(int timeout);


int emconninfo_deviceBonded(void);
void emconninfo_setDeviceBondedFlag(void);
void emconninfo_clearDeviceBondedFlag(void);


int emconninfo_islinkEncrypted(void);
void emconninfo_clearLinkEncryptedFlag(void);
void emconninfo_setLinkEncryptedFlag(void);

// This is for C++ compiler. WIN32 uses C++
#ifdef __cplusplus
}
#endif

#endif // end of #ifndef _EMCONINFO_H_
