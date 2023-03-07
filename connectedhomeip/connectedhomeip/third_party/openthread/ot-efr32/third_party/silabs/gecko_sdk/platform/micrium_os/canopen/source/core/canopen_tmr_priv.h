/***************************************************************************//**
 * @file
 * @brief CANopen Timer Management
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

#ifndef _CANOPEN_TMR_PRIV_H_
#define _CANOPEN_TMR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <canopen/include/canopen_types.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_TMR_INVALID_ID             -1

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       CANopen timer structure
 *******************************************************************************************************/

typedef struct canopen_tmr {
  CANOPEN_NODE       *NodePtr;                                  ///< Link to parent node
  CPU_INT32U         MaxTmrQty;                                 ///< Num. of elements in action/event pool
  CANOPEN_TMR_ACTION *ActionPoolPtr;                            ///< Timer action pool
  CANOPEN_TMR_TIME   *TmrPoolPtr;                               ///< Timer event pool
  CANOPEN_TMR_ACTION *ActionFreeListHeadPtr;                    ///< Timer action free linked list
  CANOPEN_TMR_TIME   *TmrFreeListHeadPtr;                       ///< Timer event free linked list
  CANOPEN_TMR_TIME   *TmrUsedListHeadPtr;                       ///< Timer event used linked list
  CANOPEN_TMR_TIME   *TmrElapsedListHeadPtr;                    ///< Timer event elapsed linked list
  CPU_INT32U         TimeAbsolute;                              ///< Time ticks of next event since create
  CPU_INT32U         TimeRelative;                              ///< Time ticks of next event from now
} CANOPEN_TMR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void CANopen_TmrInit(CANOPEN_TMR     *p_tmr,
                     CANOPEN_NODE    *p_node,
                     CANOPEN_TMR_MEM *p_tmr_mem,
                     CPU_INT16U      num);

CPU_INT16S CANopen_TmrCreate(CANOPEN_TMR      *p_tmr,
                             CPU_INT32U       start,
                             CPU_INT32U       cycle,
                             CANOPEN_TMR_FNCT fnct,
                             void             *p_arg,
                             RTOS_ERR         *p_err);

void CANopen_TmrDel(CANOPEN_TMR *p_tmr,
                    CPU_INT16S  act_id,
                    RTOS_ERR    *p_err);

void CANopen_TmrReset(CANOPEN_TMR *p_tmr);

void CANopen_TmrClr(CANOPEN_TMR *p_tmr);

CPU_BOOLEAN CANopen_TmrServ(CANOPEN_TMR *p_tmr,
                            RTOS_ERR    *p_err);

void CANopen_TmrProcess(CANOPEN_TMR *p_tmr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_TMR_PRIV_H_
