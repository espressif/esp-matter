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
 *  ======== ServiceMgr.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <ti/uia/runtime/UIAPacket.h>

#include "package/internal/ServiceMgr.xdc.h"

/*
 *************************************************************************
 *                      Module functions
 *************************************************************************
 */

/*
 *  ======== ServiceMgr_freePacket ========
 *  Call the proxy's function
 */
Void ServiceMgr_freePacket(UIAPacket_Hdr *packet)
{
    ServiceMgr_SupportProxy_freePacket(packet);
}

/*
 *  ======== ServiceMgr_getFreePacket ========
 *  Call the proxy's function
 */
UIAPacket_Hdr *ServiceMgr_getFreePacket(UIAPacket_HdrType type, UInt timeout)
{
    return (ServiceMgr_SupportProxy_getFreePacket(type, timeout));
}

/*
 *  ======== ServiceMgr_getNumServices ========
 */
Int ServiceMgr_getNumServices()
{
    return (ServiceMgr_module->numServices);
}

/*
 *  ======== ServiceMgr_processCallback ========
 *  Call the service's callback
 */
Void ServiceMgr_processCallback(ServiceMgr_ServiceId id,
                                ServiceMgr_Reason reason,
                                UIAPacket_Hdr *packet)
{
    /* Make sure the service id is valid */
    Assert_isTrue((id < ServiceMgr_module->numServices),
                   ServiceMgr_A_invalidServiceId);

    Assert_isTrue((ServiceMgr_processCallbackFxn[id] != NULL),
                   ServiceMgr_A_invalidProcessCallbackFxn);

    ServiceMgr_module->runCount++;
    ServiceMgr_processCallbackFxn[id](reason, packet);
}

/*
 *  ======== requestEnergy ========
 *  Call the proxy's function
 */
Void ServiceMgr_requestEnergy(ServiceMgr_ServiceId id)
{
    /* Make sure the service id is valid */
    Assert_isTrue((id < ServiceMgr_module->numServices),
                   ServiceMgr_A_invalidServiceId);

    ServiceMgr_SupportProxy_requestEnergy(id);
}

/*
 *  ======== ServiceMgr_sendPacket ========
 *  Call the proxy's function. It must fill in the srcAddr field.
 */
Bool ServiceMgr_sendPacket(UIAPacket_Hdr *packet)
{
    return (ServiceMgr_SupportProxy_sendPacket(packet));
}

/*
 *  ======== ServiceMgr_setPeriod ========
 *  Call the proxy's function
 */
Void ServiceMgr_setPeriod(ServiceMgr_ServiceId id, UInt32 periodInMs)
{
    /* Make sure the service id is valid */
    Assert_isTrue((id < ServiceMgr_module->numServices),
                   ServiceMgr_A_invalidServiceId);

    ServiceMgr_SupportProxy_setPeriod(id, periodInMs);
}
