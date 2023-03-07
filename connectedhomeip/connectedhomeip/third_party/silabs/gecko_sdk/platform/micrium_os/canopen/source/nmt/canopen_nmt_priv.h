/***************************************************************************//**
 * @file
 * @brief CANopen Network Management (Nmt) Service
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_NMT_PRIV_H_
#define _CANOPEN_NMT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_types.h>
#include  <canopen/include/canopen_nmt.h>
#include  <canopen/include/canopen_if.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define CANOPEN_NMT_ALLOWED             0x01                    // Indication of NMT transfers allowed
#define CANOPEN_BOOT_ALLOWED            0x02                    // Indication of BOOT transfers allowed
#define CANOPEN_EMCY_ALLOWED            0x04                    // Indication of EMCY transfers allowed
#define CANOPEN_TIME_ALLOWED            0x08                    // Indication of TIME transfers allowed
#define CANOPEN_SYNC_ALLOWED            0x10                    // Indication of SYNC transfers allowed
#define CANOPEN_SDO_ALLOWED             0x20                    // Indication of SDO transfers allowed
#define CANOPEN_PDO_ALLOWED             0x40                    // Indication of PDO transfers allowed

#define CANOPEN_NMT_STATE_INIT_BOOT_UP  0u                      // NMT initialization state.
#define CANOPEN_NMT_STATE_STOP          4u                      // NMT stopped state.
#define CANOPEN_NMT_STATE_OP            5u                      // NMT operational state.
#define CANOPEN_NMT_STATE_PRE_OP        127u                    // NMT pre-operational state.
#define CANOPEN_NMT_STATE_INVALID       255u                    // NMT invalid state.

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     NMT state machine structure
 *
 * @note    (1) This structure holds all data, which are needed for the NMT state machine management.
 *******************************************************************************************************/

typedef struct canopen_nmt {
  CANOPEN_NODE       *NodePtr;                                  ///< Pointer to parent CANopen node info
  CANOPEN_HBCONS     *HbConsPtr;                                ///< The used heartbeat consumer chain
  CANOPEN_NODE_STATE State;                                     ///< NMT state of this node
  CPU_INT16S         TmrId;                                     ///< Heartbeat timer identifier
  CPU_INT08U         CurStateObjAllowed;                        ///< Encoding of allowed CAN objects
} CANOPEN_NMT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void CANopen_NmtInit(CANOPEN_NMT  *p_nmt,
                     CANOPEN_NODE *p_node,
                     RTOS_ERR     *p_err);

void CANopen_NmtBootup(CANOPEN_NMT *p_nmt,
                       RTOS_ERR    *p_err);

CPU_INT16S CANopen_NmtCheck(CANOPEN_NMT    *p_nmt,
                            CANOPEN_IF_FRM *p_frm,
                            RTOS_ERR       *p_err);

void CANopen_NmtNodeIdSet(CANOPEN_NMT *p_nmt,
                          CPU_INT08U  node_id,
                          RTOS_ERR    *p_err);

CPU_INT08U CANopen_NmtNodeIdGet(CANOPEN_NMT *p_nmt,
                                RTOS_ERR    *p_err);

CANOPEN_NODE_STATE CANopen_NmtStateDecode(CPU_INT08U code);

CPU_INT08U CANopen_NmtStateEncode(CANOPEN_NODE_STATE state);

void CANopen_NmtHbConsInit(CANOPEN_NMT *p_nmt,
                           RTOS_ERR    *p_err);

void CANopen_NmtHbConsActivate(CANOPEN_NMT    *p_nmt,
                               CANOPEN_HBCONS *p_hbc,
                               CPU_INT16U     time,
                               CPU_INT08U     nodeid,
                               RTOS_ERR       *p_err);

CPU_INT16S CANopen_NmtHbConsCheck(CANOPEN_NMT    *p_nmt,
                                  CANOPEN_IF_FRM *p_frm,
                                  RTOS_ERR       *p_err);

void CANopen_NmtHbProdInit(CANOPEN_NMT *p_nmt,
                           RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_NMT_PRIV_H_
