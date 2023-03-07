/******************************************************************************

 @file  watchdog.c

 @brief Watchdog driver interface

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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
/*
 *  ======== Watchdog.c ========
 */

#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/Watchdog.h>

/* Externs */
extern const Watchdog_Config Watchdog_config[];

/* Also used to check status for initialization */
static int Watchdog_count = -1;

/* Default Watchdog parameters structure */
const Watchdog_Params Watchdog_defaultParams = {
    NULL,                    /* callbackFxn */
    Watchdog_RESET_ON,       /* resetMode */
    Watchdog_DEBUG_STALL_ON, /* debugStallMode */
    NULL                     /* custom */
};

/*
 *  ======== Watchdog_clear ========
 */
void Watchdog_clear(Watchdog_Handle handle)
{
    handle->fxnTablePtr->watchdogClear(handle);
}

/*
 *  ======== Watchdog_close ========
 */
void Watchdog_close(Watchdog_Handle handle)
{
    handle->fxnTablePtr->watchdogClose(handle);
}

/*
 *  ======== Watchdog_control ========
 */
int Watchdog_control(Watchdog_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->watchdogControl(handle, cmd, arg));
}

/*
 *  ======== Watchdog_init ========
 */
void Watchdog_init(void)
{
    /* Prevent multiple calls for Watchdog_init */
    if (Watchdog_count >= 0) {
        return;
    }

    /* Call each driver's init function */
    for (Watchdog_count = 0;
            Watchdog_config[Watchdog_count].fxnTablePtr != NULL;
            Watchdog_count++) {
        Watchdog_config[Watchdog_count].fxnTablePtr->watchdogInit(
                (Watchdog_Handle)&(Watchdog_config[Watchdog_count]));
    }
}

/*
 *  ======== Watchdog_open ========
 */
Watchdog_Handle Watchdog_open(unsigned int index, Watchdog_Params *params)
{
    Watchdog_Handle handle;

    handle = (Watchdog_Handle)&(Watchdog_config[index]);
    return (handle->fxnTablePtr->watchdogOpen(handle, params));
}

/*
 *  ======== Watchdog_Params_init ========
 */
void Watchdog_Params_init(Watchdog_Params *params)
{
    *params = Watchdog_defaultParams;
}


/*
 *  ======== Watchdog_setReload ========
 */
void Watchdog_setReload(Watchdog_Handle handle, uint32_t value)
{
    handle->fxnTablePtr->watchdogSetReload(handle, value);
}
