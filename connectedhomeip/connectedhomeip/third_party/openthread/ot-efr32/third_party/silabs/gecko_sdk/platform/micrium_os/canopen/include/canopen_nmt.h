/***************************************************************************//**
 * @file
 * @brief CANopen Network Management (NMT) Service
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
 * @addtogroup CANOPEN_COMM_OBJ
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_NMT_H_
#define _CANOPEN_NMT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDES FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_types.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct canopen_hbcons CANOPEN_HBCONS;

/****************************************************************************************************//**
 *                                    Heartbeat consumer structure
 *
 * @note    (1) This structure holds all data, which are needed for the heartbeat consumer handling
 *              within the object dictionary.
 *******************************************************************************************************/

struct canopen_hbcons {
  CANOPEN_HBCONS     *NextPtr;                                  ///< Link to next consumer in active chain.
  CANOPEN_NODE_STATE State;                                     ///< Received Node-State.
  CPU_INT16S         TmrId;                                     ///< Timer Identifier.
  CPU_INT16U         TimeMs;                                    ///< Time (Bit 00-15 when read object).
  CPU_INT08U         NodeId;                                    ///< Node ID (Bit 16-23 when read object).
  CPU_INT08U         MissedEventCnt;                            ///< Event Counter.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                        FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CANopen_NmtReset(CANOPEN_NODE_HANDLE node_handle,
                      CANOPEN_NMT_RESET   type,
                      RTOS_ERR            *p_err);

void CANopen_NmtStateSet(CANOPEN_NODE_HANDLE node_handle,
                         CANOPEN_NODE_STATE  state,
                         RTOS_ERR            *p_err);

CANOPEN_NODE_STATE CANopen_NmtStateGet(CANOPEN_NODE_HANDLE node_handle);

CPU_INT16S CANopen_NmtHbConsEventsGet(CANOPEN_NODE_HANDLE node_handle,
                                      CPU_INT08U          node_id,
                                      RTOS_ERR            *p_err);

CANOPEN_NODE_STATE CANopen_NmtHbConsLastStateGet(CANOPEN_NODE_HANDLE node_handle,
                                                 CPU_INT08U          node_id,
                                                 RTOS_ERR            *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_NMT_H_
