/***************************************************************************//**
 * @file
 * @brief CANopen Synchronization (Sync) Object Service
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

#ifndef _CANOPEN_SYNC_PRIV_H_
#define _CANOPEN_SYNC_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_if.h>
#include  <canopen/include/canopen_types.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define CANOPEN_SYNC_FLG_TX    0x01                             // message type indication  TPDO
#define CANOPEN_SYNC_FLG_RX    0x02                             // message type indication: RPDO

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct canopen_sync {
  //                                                               Link to parent node
  CANOPEN_NODE   *NodePtr;
  //                                                               SYNC message identifier
  CPU_INT32U     CobId;
  //                                                               SYNC time (num of received SYNC)
  CPU_INT32U     Time;
#if (CANOPEN_RPDO_MAX_QTY > 0)
  //                                                               synchronous RPDO CAN message frame
  CANOPEN_IF_FRM RPdoFrmTbl[CANOPEN_RPDO_MAX_QTY];
  //                                                               Pointer to synchronous RPDO
  CANOPEN_RPDO   *RPdoTblPtr[CANOPEN_RPDO_MAX_QTY];
#endif
#if (CANOPEN_TPDO_MAX_QTY > 0)
  //                                                               Pointer to synchronous TPDO
  CANOPEN_TPDO *TPdoTblPtr[CANOPEN_TPDO_MAX_QTY];
  //                                                               Number of SYNC until PDO shall be sent
  CPU_INT08U   TPdoTransmissionTypeTbl[CANOPEN_TPDO_MAX_QTY];
  //                                                               SYNC time when transmission must occur
  CPU_INT08U   TPdoSyncCntTbl[CANOPEN_TPDO_MAX_QTY];
#endif
} CANOPEN_SYNC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_SYNC_EN == DEF_ENABLED)
void CANopen_SyncInit(CANOPEN_SYNC *p_sync,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err);

void CANopen_SyncHandler(CANOPEN_SYNC *p_sync);

void CANopen_SyncAdd(CANOPEN_SYNC *p_sync,
                     CPU_INT16U   num,
                     CPU_INT08U   msg_type,
                     CPU_INT08U   tx_type);

void CANopen_SyncRemove(CANOPEN_SYNC *p_sync,
                        CPU_INT16U   num,
                        CPU_INT08U   msg_type);
#endif

#if ((CANOPEN_SYNC_EN == DEF_ENABLED) && (CANOPEN_RPDO_MAX_QTY > 0))
void CANopen_SyncRx(CANOPEN_SYNC   *p_sync,
                    CANOPEN_IF_FRM *p_frm);
#endif

CPU_BOOLEAN CANopen_SyncUpdate(CANOPEN_SYNC   *p_sync,
                               CANOPEN_IF_FRM *p_frm);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_SYNC_PRIV_H_
