/*
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */
/*
 *  ======== ServiceMgr.h ========
 */
#ifndef ti_uia_linux_ServiceMgr__include
#define ti_uia_linux_ServiceMgr__include

/*
 *  ======== Return Codes ========
 */
#define ServiceMgr_SUCCESS 0
#define ServiceMgr_FAILED -1

/*
 *  ======== ServiceMgr_Config ========
 *  ServiceMgr configuration structure
 *
 *  maxCtrlPacketSize:         Size of the control packets
 *  numIncomingCtrlPacketBufs: Number packets for receiving ctrl packets
 *                             from the host
 *  sharedRegionId:            SharedRegion to allocate messages from
 *  fileName:                  Instead of sending out Ethernet, write
 *                             compressed events into this file.
 */
 typedef struct ServiceMgr_Config {
    Int maxCtrlPacketSize;
    Int numIncomingCtrlPacketBufs;
    Int sharedRegionId;
    Char fileName[128];
} ServiceMgr_Config;

/*
 *  ======== ServiceMgr_getConfig ========
 *  Get the current module configuration of the ServiceMgr
 *
 *  Thread safety must be provided by the calling application.
 */
Void ServiceMgr_getConfig(ServiceMgr_Config *config);

/*
 *  ======== ServiceMgr_setConfig ========
 *  Set the  module configuration of the ServiceMgr
 *
 *  Must be called before ServiceMgr_start is called (or after
 *  ServiceMgr_stop).
 *
 *  Thread safety must be provided by the calling application.
 *
 *  Returns:
 *    ServiceMgr_SUCCESS:  Succcessfully set configuration
 *    ServiceMgr_FAILED:   Unable to set configuration
 */
Int  ServiceMgr_setConfig(ServiceMgr_Config *config);

/*
 *  ======== ServiceMgr_start ========
 *  Start the UIA ServiceMgr
 *
 *  Thread safety must be provided by the calling application.
 */
Void ServiceMgr_start();

/*
 *  ======== ServiceMgr_stop ========
 *  Stop the UIA ServiceMgr
 *
 *  Thread safety must be provided by the calling application.
 */
Void ServiceMgr_stop();

#endif
