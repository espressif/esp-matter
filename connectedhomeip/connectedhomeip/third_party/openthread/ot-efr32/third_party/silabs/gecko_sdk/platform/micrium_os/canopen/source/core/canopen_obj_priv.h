/***************************************************************************//**
 * @file
 * @brief CANopen Object Directory Management
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

#ifndef _CANOPEN_OBJ_PRIV_H_
#define _CANOPEN_OBJ_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_dict.h>
#include  <canopen/include/canopen_obj.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_CAN_ID_MASK                    0x7FFu
#define  CANOPEN_CAN_ID_FNCT_CODE_MASK          0x780u
#define  CANOPEN_CAN_ID_NODE_ID_MASK            0x07Fu

#define  CANOPEN_CAN_ID_TX_FILTER               1u
#define  CANOPEN_CAN_ID_RX_FILTER_NMT           2u
#define  CANOPEN_CAN_ID_RX_FILTER_SYNC          3u
#define  CANOPEN_CAN_ID_RX_FILTER_PDO1          4u
#define  CANOPEN_CAN_ID_RX_FILTER_PDO2          5u
#define  CANOPEN_CAN_ID_RX_FILTER_PDO3          6u
#define  CANOPEN_CAN_ID_RX_FILTER_PDO4          7u
#define  CANOPEN_CAN_ID_RX_FILTER_SDO1          8u
#define  CANOPEN_CAN_ID_RX_FILTER_HBC           9u
#define  CANOPEN_CAN_ID_RX_FILTER_NODE          10u

/********************************************************************************************************
 *                                    COB-ID FOR BROADCAST OBJECTS
 *
 * Note(s) : (1) More details about COB-ID can be found in section 7.3.3 of specification CiA-301 "CANopen
 *               application layer and communication profile, Version: 4.2.0, 21 February 2011"
 *******************************************************************************************************/

#define  CANOPEN_COB_ID_NMT                     0x00u
#define  CANOPEN_COB_ID_SYNC                    0x80u
#define  CANOPEN_COB_ID_TIMESTAMP               0x100u

/********************************************************************************************************
 *                                    COB-ID FOR PEER-TO-PEER OBJECTS
 *
 * Note(s) : (1) More details about COB-ID can be found in section 7.3.3 of specification CiA-301 "CANopen
 *               application layer and communication profile, Version: 4.2.0, 21 February 2011"
 *******************************************************************************************************/

#define  CANOPEN_COB_ID_EMCY_START              0x80u
#define  CANOPEN_COB_ID_EMCY_RANGE              0x7Fu

#define  CANOPEN_COB_ID_PDO1_TX_START           0x180u
#define  CANOPEN_COB_ID_PDO1_TX_RANGE           0x7Fu
#define  CANOPEN_COB_ID_PDO1_RX_START           0x200u
#define  CANOPEN_COB_ID_PDO1_RX_RANGE           0x7Fu

#define  CANOPEN_COB_ID_PDO2_TX_START           0x280u
#define  CANOPEN_COB_ID_PDO2_TX_RANGE           0x7Fu
#define  CANOPEN_COB_ID_PDO2_RX_START           0x300u
#define  CANOPEN_COB_ID_PDO2_RX_RANGE           0x7Fu

#define  CANOPEN_COB_ID_PDO3_TX_START           0x380u
#define  CANOPEN_COB_ID_PDO3_TX_RANGE           0x7Fu
#define  CANOPEN_COB_ID_PDO3_RX_START           0x400u
#define  CANOPEN_COB_ID_PDO3_RX_RANGE           0x7Fu

#define  CANOPEN_COB_ID_PDO4_TX_START           0x480u
#define  CANOPEN_COB_ID_PDO4_TX_RANGE           0x7Fu
#define  CANOPEN_COB_ID_PDO4_RX_START           0x500u
#define  CANOPEN_COB_ID_PDO4_RX_RANGE           0x7Fu

#define  CANOPEN_COB_ID_SDO_TX_START            0x580u
#define  CANOPEN_COB_ID_SDO_TX_RANGE            0x7Fu
#define  CANOPEN_COB_ID_SDO_RX_START            0x600u
#define  CANOPEN_COB_ID_SDO_RX_RANGE            0x7Fu

#define  CANOPEN_COB_ID_NMT_ERR_CTRL_START      0x700u
#define  CANOPEN_COB_ID_NMT_ERR_CTRL_RANGE      0x7Fu

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void CANopen_ObjInit(CANOPEN_OBJ *p_obj);

CPU_BOOLEAN CANopen_ObjCmp(CANOPEN_OBJ *p_obj,
                           void        *p_val,
                           RTOS_ERR    *p_err);

CPU_INT32U CANopen_ObjSizeGet(CANOPEN_OBJ *p_obj,
                              CPU_INT32U  width,
                              RTOS_ERR    *p_err);

void CANopen_ObjValRd(CANOPEN_NODE *p_node,
                      CANOPEN_OBJ  *p_obj,
                      void         *p_val,
                      CPU_INT08U   width,
                      CPU_INT08U   node_id,
                      RTOS_ERR     *p_err);

void CANopen_ObjValWr(CANOPEN_NODE *p_node,
                      CANOPEN_OBJ  *p_obj,
                      void         *p_val,
                      CPU_INT08U   width,
                      CPU_INT08U   node_id,
                      RTOS_ERR     *p_err);

void CANopen_ObjBufStartRd(CANOPEN_NODE *p_node,
                           CANOPEN_OBJ  *p_obj,
                           CPU_INT08U   *p_buf,
                           CPU_INT32U   len,
                           RTOS_ERR     *p_err);

void CANopen_ObjBufContRd(CANOPEN_NODE *p_node,
                          CANOPEN_OBJ  *p_obj,
                          CPU_INT08U   *p_buf,
                          CPU_INT32U   len,
                          RTOS_ERR     *p_err);

void CANopen_ObjBufStartWr(CANOPEN_NODE *p_node,
                           CANOPEN_OBJ  *p_obj,
                           CPU_INT08U   *p_buf,
                           CPU_INT32U   len,
                           RTOS_ERR     *p_err);

void CANopen_ObjBufContWr(CANOPEN_NODE *p_node,
                          CANOPEN_OBJ  *p_obj,
                          CPU_INT08U   *p_buf,
                          CPU_INT32U   len,
                          RTOS_ERR     *p_err);

void CANopen_ObjDirectRd(CANOPEN_OBJ *p_obj,
                         void        *p_val,
                         CPU_INT32U  len,
                         RTOS_ERR    *p_err);

void CANopen_ObjDirectWr(CANOPEN_OBJ *p_obj,
                         void        *p_val,
                         CPU_INT32U  len,
                         RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_OBJ_PRIV_H_
