/******************************************************************************
 *  @file  rf_mac_api.h
 *
 *  @brief      Header for RF proxy for stack's interface to the RF driver with 
 *              Dynamic Multi-protocol Manager RF API remapping
 *
 *  The dmm_rfmap header file remaps select functions in the RF driver API the
 *  DMM equivalents that are defined in the dmm_schedular module so that
 *  priorities and timing can be set according to the dual mode manager policy
 Group: WCS, BTS
 Target Device: cc13xx_cc26xx
 ******************************************************************************
 
 Copyright (c) 2015-2021, Texas Instruments Incorporated
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
#ifndef RF_API_H
#define RF_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <ti/drivers/rf/RF.h>
#include <dmm/dmm_scheduler.h>

extern uint32_t *macRfDrvTblPtr;
extern void rfSpinlock( void );

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */


#define RF_open              ((RF_Handle    (*)(RF_Object *, RF_Mode *, RF_RadioSetup *, RF_Params *))       DMMSch_rfOpen)
#define RF_close             ((void         (*)(RF_Handle))                                                  RF_close)
#define RF_postCmd           ((RF_CmdHandle (*)(RF_Handle, RF_Op *, RF_Priority, RF_Callback, RF_EventMask)) DMMSch_rfPostCmd)
#define RF_pendCmd           ((RF_EventMask (*)(RF_Handle, RF_CmdHandle, RF_EventMask))                      RF_pendCmd)
#define RF_runCmd            ((RF_EventMask (*)(RF_Handle, RF_Op *, RF_Priority, RF_Callback, RF_EventMask)) DMMSch_rfRunCmd)
#define RF_cancelCmd         ((RF_Stat      (*)(RF_Handle, RF_CmdHandle, uint8_t))                           DMMSch_rfCancelCmd)
#define RF_flushCmd          ((RF_Stat      (*)(RF_Handle, RF_CmdHandle, uint8_t))                           DMMSch_rfFlushCmd)
#define RF_yield             ((void         (*)(RF_Handle))                                                  RF_yield)
#define RF_Params_init       ((void         (*)(RF_Params *))                                                RF_Params_init)
#define RF_runImmediateCmd   ((RF_Stat      (*)(RF_Handle, uint32_t *))                                      DMMSch_rfRunImmediateCmd)
#define RF_runDirectCmd      ((RF_Stat      (*)(RF_Handle, uint32_t))                                        DMMSch_rfRunDirectCmd)
#define RF_ratCompare        ((RF_RatHandle       (*)(RF_Handle, RF_RatConfigCompare *, RF_RatConfigOutput *))     RF_ratCompare)
#define RF_ratCapture        ((RF_RatHandle       (*)(RF_Handle, RF_RatConfigCapture *, RF_RatConfigOutput *))     RF_ratCapture)
#define RF_RatConfigOutput_init       ((void      (*)(RF_RatConfigOutput *))                                 RF_RatConfigOutput_init)
#define RF_ratDisableChannel ((RF_Stat      (*)(RF_Handle, RF_RatHandle))                                    RF_ratDisableChannel)
#define RF_getCurrentTime    ((uint32_t     (*)(void))                                                       RF_getCurrentTime)
#define RF_getRssi           ((int8_t       (*)(RF_Handle))                                                  RF_getRssi)
#define RF_getInfo           ((RF_Stat      (*)(RF_Handle, RF_InfoType, RF_InfoVal *))                       RF_getInfo)
#define RF_getCmdOp          ((RF_Op *      (*)(RF_Handle, RF_CmdHandle))                                    RF_getCmdOp)
#define RF_control           ((RF_Stat      (*)(RF_Handle, int8_t, void *))                                  RF_control)
#define RF_scheduleCmd       ((RF_CmdHandle (*)(RF_Handle, RF_Op *, RF_ScheduleCmdParams *, RF_Callback, RF_EventMask))  DMMSch_rfScheduleCmd)
#define RF_getTxPower        ((RF_TxPowerTable_Value      (*)(RF_Handle))                                    RF_getTxPower)
#define RF_setTxPower        ((RF_Stat      (*)(RF_Handle, RF_TxPowerTable_Value))                           RF_setTxPower)
#define RF_TxPowerTable_findPowerLevel        ((int8_t      (*)(RF_TxPowerTable_Entry *, RF_TxPowerTable_Value))  RF_TxPowerTable_findPowerLevel)
#define RF_TxPowerTable_findValue        ((RF_TxPowerTable_Value      (*)(RF_TxPowerTable_Entry *, int8_t))  RF_TxPowerTable_findValue)
#define RF_ScheduleCmdParams_init        ((void      (*)(RF_ScheduleCmdParams *))                            RF_ScheduleCmdParams_init)
#define RF_requestAccess     ((RF_Stat (*)(RF_Handle h, RF_AccessParams *pParams))                            DMMSch_rfRequestAccess)
#define RF_runScheduleCmd    ((RF_EventMask (*) (RF_Handle, RF_Op*, RF_ScheduleCmdParams*, RF_Callback, RF_EventMask)) DMMSch_rfRunScheduleCmd)


#ifdef __cplusplus
}
#endif

#endif /* RF_API_H */
