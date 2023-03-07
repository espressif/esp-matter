/******************************************************************************

 @file  icall_jt.h

 @brief Header for Icall proxy for stack's interface to the icall framework.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

#ifndef ICALL_JT_H
#define ICALL_JT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <icall.h>

/*******************************************************************************
 * EXTERNAL VARIABLES
*/

  extern uint32_t *icallServiceTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// Icall proxy index for icall framework API
// If new APIs are being added, the corresponding jump table icallServiceTable
// located in icall_user_config.c needs to be updated.
#define ICALL_SEND                        0
#define ICALL_ENROLLSERVICE               1
#define ICALL_ABORT                       2
#define ICALL_SETTIMERMSECS               3
#define ICALL_SETTIMER                    4
#define ICALL_WAIT                        5
#define ICALL_GETTICKS                    6
#define ICALL_FETCHMSG                    7
#define ICALL_STOPTIMER                   8
#define ICALL_SIGNAL                      9
#define ICALL_PWRDISPENSE                 10
#define ICALL_PWRREQUIRE                  11
#define ICALL_PWRUPACTIVITYCOUNTER        12
#define ICALL_MALLOC                      13
#define ICALL_FREE                        14
#define ICALL_SENDSERVICECOMPLETE         15
#define ICALL_GETHEAPSTATS                16
#define ICALL_MALLOCLIMITED               17

/*
** Icall API Proxy
*/

#define ICALL_TABLE( index )        (*((uint32_t *)((uint32_t)icallServiceTblPtr + (uint32_t)((index)*4))))

#define ICall_send                  ((ICall_Errno (*)(ICall_EntityID, ICall_EntityID, ICall_MSGFormat, void *))                         ICALL_TABLE(ICALL_SEND))
#define ICall_enrollService         ((ICall_Errno (*)(ICall_ServiceEnum , ICall_ServiceFunc , ICall_EntityID *, ICall_SyncHandle *))    ICALL_TABLE(ICALL_ENROLLSERVICE))
#define ICall_abort                 ((ICall_Errno (*)(void))                                                                            ICALL_TABLE(ICALL_ABORT))
#define ICall_setTimerMSecs         ((ICall_Errno (*)(uint_fast32_t, ICall_TimerCback, void *, ICall_TimerID *))                        ICALL_TABLE(ICALL_SETTIMERMSECS))
#define ICall_setTimer              ((ICall_Errno (*)(uint_fast32_t, ICall_TimerCback, void *, ICall_TimerID *))                        ICALL_TABLE(ICALL_SETTIMER))
#define ICall_wait                  ((ICall_Errno (*)(uint_fast32_t))                                                                   ICALL_TABLE(ICALL_WAIT))
#define ICall_getTicks              ((uint_fast32_t (*)(void))                                                                          ICALL_TABLE(ICALL_GETTICKS))
#define ICall_fetchMsg              ((uint_fast32_t (*)(ICall_EntityID *, ICall_EntityID *, void **))                                   ICALL_TABLE(ICALL_FETCHMSG))
#define ICall_stopTimer             ((void (*)(ICall_TimerID))                                                                          ICALL_TABLE(ICALL_STOPTIMER))
#define ICall_signal                ((ICall_Errno (*)(ICall_SyncHandle))                                                                ICALL_TABLE(ICALL_SIGNAL))
#define ICall_pwrDispense           ((ICall_Errno (*)(ICall_PwrBitmap_t))                                                               ICALL_TABLE(ICALL_PWRDISPENSE))
#define ICall_pwrRequire            ((ICall_Errno (*)(ICall_PwrBitmap_t))                                                               ICALL_TABLE(ICALL_PWRREQUIRE))
#define ICall_pwrUpdActivityCounter ((bool (*)(bool))                                                                                   ICALL_TABLE(ICALL_PWRUPACTIVITYCOUNTER))
#define ICall_malloc                ((void* (*)(uint_least16_t))                                                                        ICALL_TABLE(ICALL_MALLOC))
#define ICall_free                  ((void (*)(void*))                                                                                  ICALL_TABLE(ICALL_FREE))
#define ICall_sendServiceComplete   ((ICall_Errno (*)(ICall_EntityID, ICall_EntityID, ICall_MSGFormat, void *))                         ICALL_TABLE(ICALL_SENDSERVICECOMPLETE))
#define ICall_getHeapStats          ((void (*)(ICall_heapStats_t*))                                                                     ICALL_TABLE(ICALL_GETHEAPSTATS))
#define ICall_mallocLimited         ((void* (*)(uint_least16_t))                                                                        ICALL_TABLE(ICALL_MALLOCLIMITED))

#ifdef __cplusplus
}
#endif

#endif /* ICALL_JT_H */
