/******************************************************************************

 @file  oad_app_support.h

 @brief This file contains OAD support functions for the applications

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef OAD_APP_SUPPORT_H
#define OAD_APP_SUPPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef OAD_SUPPORT
// Used for imgHdr_t structure
#include <common/cc26xx/oad/oad_image_header.h>
#include "oad.h"
#if defined(OAD_SUPPORT_ONCHIP)
// Used for OAD Reset Service APIs
#include "oad_reset_service.h"
#include <common/cc26xx/flash_interface/flash_interface.h>
#endif
#endif

#ifdef OAD_SUPPORT
void    OadApp_init();
void    OadApp_cancel();
void    OadApp_processEvents(uint32_t events);
uint8_t OadApp_processL2CAPMsg(l2capSignalEvent_t *pMsg);
#if defined(OAD_SUPPORT_OFFCHIP)
void    OadApp_processOadWriteCB(uint8_t event, uint16_t arg);
#endif // OAD_SUPPORT_OFFCHIP
bool    OadApp_processConnEvt();
#ifdef OAD_SUPPORT_ONCHIP
void    OadApp_addService();
void OadApp_processOadResetEvt(oadResetWrite_t *resetEvt);
void OadApp_processOadResetWriteCB(uint16_t connHandle, uint16_t bim_var);
#endif // OAD_SUPPORT_CHIP
#endif // OAD_SUPPORT

#ifdef __cplusplus
}
#endif

#endif /* OAD_APP_SUPPORT_H */
