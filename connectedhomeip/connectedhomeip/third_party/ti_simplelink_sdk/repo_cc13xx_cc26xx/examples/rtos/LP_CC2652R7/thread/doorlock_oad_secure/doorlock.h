/******************************************************************************

 @file doorlock.h

 @brief Door lock example application

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

#ifndef _DOORLOCK_H_
#define _DOORLOCK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* Door lock uri string */
#define DOORLOCK_URI           "doorlock/"
/* Door lock state string */
#define DOORLOCK_STATE_URI     "doorlock/lockstate"
/* Door unlock state string */
#define DOORLOCK_STATE_LOCK    "lock"
/* Door locked state string */
#define DOORLOCK_STATE_UNLOCK  "unlock"

/**
 * Door lock events.
 */
typedef enum
{
    DoorLock_evtLock,               /* Lock event received */
    DoorLock_evtUnlock,             /* unlock event received */
    DoorLock_evtNwkSetup,           /* open thread network is setup */
    DoorLock_evtKeyLeft,            /* Left Key is pressed */
    DoorLock_evtKeyRight,           /* Right key is pressed */
    DoorLock_evtNwkJoined,          /* Joined the network */
    DoorLock_evtNwkJoinFailure,     /* Failed joining network */
    DoorLock_evtDevRoleChanged,     /* Events for Device State */
    OAD_queueEvt,                   /* Events for OAD */
    OAD_CtrlRegEvt,                 /* Events for OAD registration*/
#if TIOP_CUI
    DoorLock_evtProcessMenuUpdate,  /* CUI Menu Event is triggered */
    DoorLock_evtNwkAttach,          /* CUI Menu Attach option is selected */
    DoorLock_evtNwkJoin,            /* CUI Menu Join option is selected */
#endif /* TIOP_CUI */
} appEvent_e;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Door Lock task.
 *
 * @param event event to post.
 * @return None
 */
extern void app_postEvt(appEvent_e event);

#ifdef __cplusplus
}
#endif

#endif /* _DOORLOCK_H_ */
