/***************************************************************************//**
 * @file
 * @brief CANopen Data Types
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

#ifndef _CANOPEN_TYPES_H_
#define _CANOPEN_TYPES_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct canopen_node_handle {
  void *NodePtr;
} CANOPEN_NODE_HANDLE;

typedef struct canopen_node CANOPEN_NODE;

typedef void (*CANOPEN_TMR_FNCT)(void *p_arg);

/****************************************************************************************************//**
 *                                             Timer action
 *
 * @note     This structure holds all data, which are needed for managing a timer timed action.
 *******************************************************************************************************/

typedef struct canopen_tmr_action CANOPEN_TMR_ACTION;

struct canopen_tmr_action {
  CPU_INT16U         Id;                                        ///< Unique action identifier.
  CANOPEN_TMR_ACTION *NextActionPtr;                            ///< Link to next action.
  CANOPEN_TMR_FNCT   Fnct;                                      ///< Pointer to callback function.
  void               *ParamPtr;                                 ///< Callback function parameter.
  CPU_INT32U         CycleTime;                                 ///< Action cycle time in ticks.
};

/****************************************************************************************************//**
 *                                             Timer event
 *
 * @note     This structure holds all data, which are needed for managing a timer event.
 *******************************************************************************************************/

typedef struct canopen_tmr_time CANOPEN_TMR_TIME;

struct canopen_tmr_time {
  CANOPEN_TMR_TIME   *NextPtr;                                  ///< Link to next timer.
  CANOPEN_TMR_ACTION *ActionPtr;                                ///< Root of linked action list.
  CANOPEN_TMR_ACTION *ActionEndPtr;                             ///< Last element in linked action list.
  CPU_INT32U         Delta;                                     ///< Delta ticks from previous timer event.
};

/****************************************************************************************************//**
 *                                             Timer memory
 *
 * @note     This structure is intended to simplify the memory allocation in the application. The
 *           number of actions and timer structures are always the same, therefore we can reduce the
 *           configuration effort to the memory array, and the length of this memory array.
 *******************************************************************************************************/

typedef struct canopen_tmr_mem {
  CANOPEN_TMR_ACTION Action;                                    ///< Memory portion for action info.
  CANOPEN_TMR_TIME   Tmr;                                       ///< Memory portion for timer info.
} CANOPEN_TMR_MEM;

/*
 ********************************************************************************************************
 *                        This enumeration holds all possible CANopen device states.
 *******************************************************************************************************/

typedef enum canopen_node_state {
  CANOPEN_INVALID = 0,                                          ///< Device in INVALID state.
  CANOPEN_INIT,                                                 ///< Device in INIT state.
  CANOPEN_PREOP,                                                ///< Device in PRE-OPERATIONAL state.
  CANOPEN_OPERATIONAL,                                          ///< Device in OPERATIONAL state.
  CANOPEN_STOP,                                                 ///< Device in STOP state.
  CANOPEN_STATE_QTY                                             ///< Number of device states.
} CANOPEN_NODE_STATE;

/*
 ********************************************************************************************************
 *                            This enumeration holds all possible reset types.
 *******************************************************************************************************/

typedef enum canopen_nmt_reset {
  CANOPEN_RESET_NODE = 0,                                       ///< Reset application (and communication).
  CANOPEN_RESET_COMM,                                           ///< Reset communication.
  CANOPEN_RESET_QTY                                             ///< Number of reset types.
} CANOPEN_NMT_RESET;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const CANOPEN_NODE_HANDLE CANopen_NodeHandleNull;

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_NODE_HANDLE_NULL   { DEF_NULL; }

#define  CANOPEN_NODE_HANDLE_IS_NULL(node_handle)       (((node_handle).NodePtr == DEF_NULL) ? DEF_YES : DEF_NO)
#define  CANOPEN_NODE_HANDLE_IS_VALID(node_handle)       (!CANOPEN_NODE_HANDLE_IS_NULL(node_handle))

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_TYPES_H_
