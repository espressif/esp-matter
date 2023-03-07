/******************************************************************************

 @file  rf_mac_api.h

 @brief Header for RF proxy for stack's interface to the RF driver.

 Group: WCS, BTS
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

extern uint32_t *macRfDrvTblPtr;
extern void rfSpinlock( void );

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// RF proxy index for RF driver API
#define RF_OPEN                        0
#define RF_CLOSE                       1
#define RF_POST_CMD                    2
#define RF_PEND_CMD                    3
#define RF_RUN_CMD                     4
#define RF_CANCEL_CMD                  5
#define RF_FLUSH_CMD                   6
#define RF_YIELD                       7
#define RF_PARAMS_INIT                 8
#define RF_RUN_IMMED_CMD               9
#define RF_RUN_DIRECT_CMD              10
#define RF_RAT_COMPARE                 11
#define RF_RAT_CAPTURE                 12
#define RF_RAT_CONFIG_OUTPUT_INIT      13
#define RF_RAT_DISABLE_CHAN            14
#define RF_GET_CURRENT_TIME            15
#define RF_GET_RSSI                    16
#define RF_GET_INFO                    17
#define RF_GET_CMD_OP                  18
#define RF_CONTROL                     19
#define RF_SCHEDULE_CMD                20
#define RF_GET_TX_POWER                21
#define RF_SET_TX_POWER                22
#define RF_TX_FIND_LEVEL               23
#define RF_TX_FIND_VALUE               24
#define RF_SCHEDULE_PARAMS_INIT        25
/*
** RF API Proxy
*/

#define RF_TABLE( index )   (*((uint32_t *)((uint32_t)macRfDrvTblPtr + (uint32_t)((index)*4))))
//#define RF_TABLE( index )  ((macRfDrvTblPtr==NULL)?(uint32_t)rfSpinlock:(*((uint32_t *)((uint32_t)macRfDrvTblPtr + (uint32_t)((index)*4)))))

#define RF_open              ((RF_Handle    (*)(RF_Object *, RF_Mode *, RF_RadioSetup *, RF_Params *))       RF_TABLE(RF_OPEN))
#define RF_close             ((void         (*)(RF_Handle))                                                  RF_TABLE(RF_CLOSE))
#define RF_postCmd           ((RF_CmdHandle (*)(RF_Handle, RF_Op *, RF_Priority, RF_Callback, RF_EventMask)) RF_TABLE(RF_POST_CMD))
#define RF_pendCmd           ((RF_EventMask (*)(RF_Handle, RF_CmdHandle, RF_EventMask))                      RF_TABLE(RF_PEND_CMD))
#define RF_runCmd            ((RF_EventMask (*)(RF_Handle, RF_Op *, RF_Priority, RF_Callback, RF_EventMask)) RF_TABLE(RF_RUN_CMD))
#define RF_cancelCmd         ((RF_Stat      (*)(RF_Handle, RF_CmdHandle, uint8_t))                           RF_TABLE(RF_CANCEL_CMD))
#define RF_flushCmd          ((RF_Stat      (*)(RF_Handle, RF_CmdHandle, uint8_t))                           RF_TABLE(RF_FLUSH_CMD))
#define RF_yield             ((void         (*)(RF_Handle))                                                  RF_TABLE(RF_YIELD))
#define RF_Params_init       ((void         (*)(RF_Params *))                                                RF_TABLE(RF_PARAMS_INIT))
#define RF_runImmediateCmd   ((RF_Stat      (*)(RF_Handle, uint32_t *))                                      RF_TABLE(RF_RUN_IMMED_CMD))
#define RF_runDirectCmd      ((RF_Stat      (*)(RF_Handle, uint32_t))                                        RF_TABLE(RF_RUN_DIRECT_CMD))
#define RF_ratCompare        ((RF_RatHandle       (*)(RF_Handle, RF_RatConfigCompare *, RF_RatConfigOutput *))     RF_TABLE(RF_RAT_COMPARE))
#define RF_ratCapture        ((RF_RatHandle       (*)(RF_Handle, RF_RatConfigCapture *, RF_RatConfigOutput *))     RF_TABLE(RF_RAT_CAPTURE))
#define RF_RatConfigOutput_init       ((void      (*)(RF_RatConfigOutput *))                                 RF_TABLE(RF_RAT_CONFIG_OUTPUT_INIT))
#define RF_ratDisableChannel ((RF_Stat      (*)(RF_Handle, RF_RatHandle))                                    RF_TABLE(RF_RAT_DISABLE_CHAN))
#define RF_getCurrentTime    ((uint32_t     (*)(void))                                                       RF_TABLE(RF_GET_CURRENT_TIME))
#define RF_getRssi           ((int8_t       (*)(RF_Handle))                                                  RF_TABLE(RF_GET_RSSI))
#define RF_getInfo           ((RF_Stat      (*)(RF_Handle, RF_InfoType, RF_InfoVal *))                       RF_TABLE(RF_GET_INFO))
#define RF_getCmdOp          ((RF_Op *      (*)(RF_Handle, RF_CmdHandle))                                    RF_TABLE(RF_GET_CMD_OP))
#define RF_control           ((RF_Stat      (*)(RF_Handle, int8_t, void *))                                  RF_TABLE(RF_CONTROL))
#define RF_scheduleCmd       ((RF_CmdHandle (*)(RF_Handle, RF_Op *, RF_ScheduleCmdParams *, RF_Callback, RF_EventMask))  RF_TABLE(RF_SCHEDULE_CMD))
#define RF_getTxPower        ((RF_TxPowerTable_Value      (*)(RF_Handle))                                    RF_TABLE(RF_GET_TX_POWER))
#define RF_setTxPower        ((RF_Stat      (*)(RF_Handle, RF_TxPowerTable_Value))                           RF_TABLE(RF_SET_TX_POWER))
#define RF_TxPowerTable_findPowerLevel        ((int8_t      (*)(RF_TxPowerTable_Entry *, RF_TxPowerTable_Value))  RF_TABLE(RF_TX_FIND_LEVEL))
#define RF_TxPowerTable_findValue        ((RF_TxPowerTable_Value      (*)(RF_TxPowerTable_Entry *, int8_t))  RF_TABLE(RF_TX_FIND_VALUE))
#define RF_ScheduleCmdParams_init        ((void      (*)(RF_ScheduleCmdParams *))                            RF_TABLE(RF_SCHEDULE_PARAMS_INIT))

#ifdef __cplusplus
}
#endif

#endif /* RF_API_H */
