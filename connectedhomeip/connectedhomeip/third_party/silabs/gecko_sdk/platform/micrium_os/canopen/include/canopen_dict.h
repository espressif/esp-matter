/***************************************************************************//**
 * @file
 * @brief CANopen Object Dictionary Management
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @addtogroup CANOPEN_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_DICT_H_
#define _CANOPEN_DICT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_types.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CANopen_DictByteRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT08U          *p_val,
                        RTOS_ERR            *p_err);

void CANopen_DictWordRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT16U          *p_val,
                        RTOS_ERR            *p_err);

void CANopen_DictLongRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT32U          *p_val,
                        RTOS_ERR            *p_err);

void CANopen_DictByteWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT08U          val,
                        RTOS_ERR            *p_err);

void CANopen_DictWordWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT16U          val,
                        RTOS_ERR            *p_err);

void CANopen_DictLongWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT32U          val,
                        RTOS_ERR            *p_err);

void CANopen_DictBufRd(CANOPEN_NODE_HANDLE node_handle,
                       CPU_INT32U          key,
                       CPU_INT08U          *p_buf,
                       CPU_INT32U          len,
                       RTOS_ERR            *p_err);

void CANopen_DictBufWr(CANOPEN_NODE_HANDLE node_handle,
                       CPU_INT32U          key,
                       CPU_INT08U          *p_buf,
                       CPU_INT32U          len,
                       RTOS_ERR            *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 *@}                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_DICT_H_
