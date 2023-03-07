/***************************************************************************//**
 * @file
 * @brief CANopen Process Data Objects (Pdo) Service - Receive Pdo
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

#ifndef _CANOPEN_RPDO_PRIV_H_
#define _CANOPEN_RPDO_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_obj.h>
#include  <canopen/include/canopen_if.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define CANOPEN_RPDO_ASYNC              1                       // Ctrl function code: asynchronous RPDO

#define CANOPEN_RPDO_FLG__E             DEF_BIT_00              // Flag #0 indicates: enabled RPDO
#define CANOPEN_RPDO_FLG_S_             DEF_BIT_01              // Flag #1 indicates: synchronized RPDO
#define CANOPEN_RPDO_FLG_R_             DEF_BIT_02              // Flag #2 indicates: synch + event RPDO

#define CANOPEN_RPDO_MAX_DATA_PAYLOAD   8u

/********************************************************************************************************
 *                                    RPDO COMMUNICATION PARAMETER
 *******************************************************************************************************/

#define CANOPEN_RPDO_COMM_TYPE_SYNC_MAX_LIMIT   240

#define CANOPEN_RPDO_COMM_COB_ID_VALID          (DEF_BIT_31)    // CAN identifier, marked as unused
#define CANOPEN_RPDO_COMM_COB_ID_RTR            (DEF_BIT_30)    // CAN identifier, RTR is not allowed
#define CANOPEN_RPDO_COMM_COB_ID_FRM            (DEF_BIT_29)    // CAN identifier, extended format
#define CANOPEN_RPDO_COMM_COB_ID_CAN_ID_29_BITS_MSK        0x1FFFFFFF

/********************************************************************************************************
 *                                       RPDO MAPPING PARAMETER
 *******************************************************************************************************/

#define CANOPEN_RPDO_MAPPING_OBJ_IX_BIT_SHIFT   16u
#define CANOPEN_RPDO_MAPPING_OBJ_LEN_MSK        0xFF

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                 CANopen RPDO structure
 *
 * @note    (1)  This structure holds all data, which are needed for managing a single RPDO.
 *******************************************************************************************************/

#if (CANOPEN_RPDO_MAX_QTY > 0)
typedef struct canopen_rpdo {
  CANOPEN_NODE *NodePtr;                                        ///< link to parent CANopen node
  CPU_INT32U   MsgId;                                           ///< message identifier
  CANOPEN_OBJ  *MapPtrTbl[CANOPEN_RPDO_MAX_MAP_QTY];            ///< pointer list with mapped objects
  CPU_INT08U   MapObjQty;                                       ///< Number of linked objects
  CPU_INT08U   Flag;                                            ///< Flags attributed of PDO
} CANOPEN_RPDO;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_RPDO_MAX_QTY > 0)
void CANopen_RPdoInit(CANOPEN_RPDO *p_pdo,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err);

void CANopen_RPdoClr(CANOPEN_RPDO *p_pdo,
                     CANOPEN_NODE *p_node);

void CANopen_RPdoReset(CANOPEN_RPDO *p_pdo,
                       CPU_INT16S   nbr,
                       RTOS_ERR     *p_err);

void CANopen_RPdoMapGet(CANOPEN_RPDO *p_pdo,
                        CPU_INT16U   nbr,
                        RTOS_ERR     *p_err);

CPU_INT16S CANopen_RPdoCheck(CANOPEN_RPDO   *p_pdo,
                             CANOPEN_IF_FRM *p_frm);

void CANopen_RPdoRd(CANOPEN_RPDO   *p_pdo,
                    CPU_INT16U     nbr,
                    CANOPEN_IF_FRM *p_frm);

void CANopen_RPdoWr(CANOPEN_RPDO   *p_pdo,
                    CANOPEN_IF_FRM *p_frm,
                    RTOS_ERR       *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_RPDO_PRIV_H_
