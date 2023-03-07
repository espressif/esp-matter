/***************************************************************************//**
 * @file
 * @brief CANopen Process Data Objects (Pdo) Service - Transmit Pdo
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

#ifndef _CANOPEN_TPDO_PRIV_H_
#define _CANOPEN_TPDO_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_def.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_obj.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_TPDO_FLG___E               0x01                // PDO event occured
#define  CANOPEN_TPDO_FLG__I_               0x02                // PDO TX inhibited
#define  CANOPEN_TPDO_FLG__IE               0x03                // PDO event occured + TX inhibited
#define  CANOPEN_TPDO_FLG_S__               0x04                // PDO synced
#define  CANOPEN_TPDO_FLG_S_E               0x05                // PDO synced + event occured
#define  CANOPEN_TPDO_FLG_SI_               0x06                // PDO synced + TX inhibited
#define  CANOPEN_TPDO_FLG_SIE               0x07                // PDO synved + event occured + TX inh.

#define  CANOPEN_TPDO_ASYNC                 1                   // Ctrl function code: asynchronous TPDO
#define  CANOPEN_TPDO_SYNC                  2                   // Ctrl function code: synchronous TPDO

#define  CANOPEN_TPDO_MAX_DATA_PAYLOAD      8u

/********************************************************************************************************
 *                                    TPDO COMMUNICATION PARAMETER
 *******************************************************************************************************/

#define  CANOPEN_TPDO_COMM_PARAM_IX_RANGE                       0x1FFu

#define  CANOPEN_TPDO_MAPPING_OBJ_LEN_MSK                       0xFF

#define  CANOPEN_TPDO_COMM_PARAM_SUB_IX_4_RESERVED              4u

#define  CANOPEN_TPDO_COMM_COB_ID_VALID     (DEF_BIT_31)        // CAN identifier, marked as unused
#define  CANOPEN_TPDO_COMM_COB_ID_RTR       (DEF_BIT_30)        // CAN identifier, RTR is not allowed
#define  CANOPEN_TPDO_COMM_COB_ID_FRM       (DEF_BIT_29)        // CAN identifier, extended format
#define  CANOPEN_TPDO_COMM_COB_ID_CAN_ID_29_BITS_MSK            0x1FFFFFFF

#define  CANOPEN_TPDO_COMM_TYPE_SYNC_ACYCLIC                    0u
#define  CANOPEN_TPDO_COMM_TYPE_SYNC_CYCLIC_MAX_VAL             240u
#define  CANOPEN_TPDO_COMM_TYPE_ASYNC_EVENT_VENDOR_SPECIFIC     254u
#define  CANOPEN_TPDO_COMM_TYPE_ASYNC_EVENT_DEV_APP_SPECIFIC    255u

#define  CANOPEN_TPDO_COMM_INHIBIT_TIME_UNIT_uS                 100u

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                 CANopen TPDO signal link table structure
 *
 * @note    (1)  This structure holds all data, which are needed for managing the links from a signal
 *               to all TPDOs, which has active mapping entries to this signal.
 *******************************************************************************************************/

typedef struct canopen_tpdo_link {
  CANOPEN_OBJ *ObjPtr;                                          ///< pointer to object
  CPU_INT16U  Nbr;                                              ///< currently mapped to TPDO-num (0..511)
} CANOPEN_TPDO_LINK;

/****************************************************************************************************//**
 *                                       CANopen TPDO structure
 *
 * @note    (1)  This structure holds all data, which are needed for managing a single TPDO.
 *******************************************************************************************************/

#if (CANOPEN_TPDO_MAX_QTY > 0)
typedef struct canopen_tpdo {
  CANOPEN_NODE *NodePtr;                                        ///< link to parent CANopen node
  CPU_INT32U   MsgId;                                           ///< message identifier
  CANOPEN_OBJ  *MapPtrTbl[CANOPEN_TPDO_MAX_MAP_QTY];            ///< pointer list with mapped objects
  CPU_INT16S   AsyncEventTmrId;                                 ///< event timer id
  CPU_INT16U   AsyncEventTmrTime;                               ///< event time in timer ticks
  CPU_INT16S   AsyncInhibitTmrId;                               ///< inhibit timer id
  CPU_INT16U   AsyncInhibitTime;                                ///< inhibit time in timer ticks
  CPU_INT08U   Flags;                                           ///< info flags
  CPU_INT08U   MapObjQty;                                       ///< Number of linked objects
} CANOPEN_TPDO;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_TPDO_MAX_QTY > 0)
void CANopen_TPdoInit(CANOPEN_TPDO *p_pdo,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err);

void CANopen_TPdoClr(CANOPEN_TPDO *p_pdo,
                     CANOPEN_NODE *p_node);

void CANopen_TPdoReset(CANOPEN_TPDO *p_pdo,
                       CPU_INT16U   nbr,
                       RTOS_ERR     *p_err);

void CANopen_TPdoMapGet(CANOPEN_TPDO *p_pdo,
                        CPU_INT16U   nbr,
                        RTOS_ERR     *p_err);

void CANopen_TPdoTmrEvent(void *p_arg);

void CANopen_TPdoEndInhibit(void *p_arg);

void CANopen_TPdoWr(CANOPEN_TPDO *p_pdo,
                    RTOS_ERR     *p_err);

void CANopen_TPdoMapClr(CANOPEN_TPDO_LINK *p_map);

void CANopen_TPdoMapNbrDel(CANOPEN_TPDO_LINK *p_map,
                           CPU_INT16U        nbr);

void CANopen_TPdoMapSigDel(CANOPEN_TPDO_LINK *p_map,
                           CANOPEN_OBJ       *p_obj);

void CANopen_TPdoObjTrig(CANOPEN_TPDO *p_tpdo,
                         CANOPEN_OBJ  *p_obj,
                         RTOS_ERR     *p_err);

void CANopen_TPdoPdoTrig(CANOPEN_TPDO *p_tpdo,
                         CPU_INT16U   nbr,
                         RTOS_ERR     *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_TPDO_PRIV_H_
