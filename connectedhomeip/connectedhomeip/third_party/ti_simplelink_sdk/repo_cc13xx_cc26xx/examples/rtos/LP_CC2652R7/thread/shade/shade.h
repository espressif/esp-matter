/******************************************************************************

 @file shade.h

 @brief shade example application definitions.

 Group: CMCU, LPC
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

#ifndef _SHADE_H_
#define _SHADE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/** Shade uri string */
#define SHADE_URI           "blinds/"
/** Shade state string */
#define SHADE_STATE_URI     "blinds/state"
/** Shade open state string */
#define SHADE_STATE_OPEN    "open"
/** Shade closed state string */
#define SHADE_STATE_CLOSED  "closed"
/** Shade drawn state string */
#define SHADE_STATE_DRAWN   "drawn"


/**
 * Shade events.
 */
typedef enum
{
    Shade_evtOpen,              /* Shade openLock event */
    Shade_evtClosed,            /* Shade closed event */
    Shade_evtDrawn,             /* Shade drawn event */
    Shade_evtNwkSetup,          /* openthread network is setup */
    Shade_evtDevRoleChanged,    /* Events for Device State */
    Shade_evtKeyLeft,           /* Left Key is pressed */
    Shade_evtKeyRight,          /* Right key is pressed */
    Shade_evtNwkJoined,         /* Joined the network */
    Shade_evtNwkJoinFailure,    /* Failed joining network */
#if TIOP_CUI
    Shade_evtProcessMenuUpdate,  /* CUI Menu Event is triggered */
    Shade_evtNwkAttach,          /* CUI Menu Attach option is selected */
    Shade_evtNwkJoin,            /* CUI Menu Join option is selected */
#endif /* TIOP_CUI */
} Shade_evt_t;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Shade task.
 *
 * @param event event to post.
 * @return None
 */
extern void Shade_postEvt(Shade_evt_t event);

#ifdef __cplusplus
}
#endif

#endif /* _SHADE_H_ */
