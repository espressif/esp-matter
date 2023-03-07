/***************************************************************************//**
 * @file
 * @brief CANopen Protocol Core Operations
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

#ifndef _CANOPEN_CORE_PRIV_H_
#define _CANOPEN_CORE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_emcy.h>
#include  <canopen/source/core/canopen_ctr_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_tmr_priv.h>
#include  <canopen/source/nmt/canopen_nmt_priv.h>
#include  <canopen/source/objects/emcy/canopen_emcy_priv.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/sdo/canopen_sdo_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_SDO_BLK_EN                 DEF_DISABLED
#define  CANOPEN_SDO_MAX_SEG_QTY            1

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                         CANopen event type
 *******************************************************************************************************/

typedef enum canopen_event_type {
  CANOPEN_EVENT_ERR_SET,
  CANOPEN_EVENT_ERR_CLR,
  CANOPEN_EVENT_TMR,
  CANOPEN_EVENT_RX
} CANOPEN_EVENT_TYPE;

/*
 ********************************************************************************************************
 *                                       CANopen event structure
 *******************************************************************************************************/

typedef struct canopen_event {
  SLIST_MEMBER         EventList;                               ///< CANopen event list.
  struct canopen_event *NextEventPtr;                           ///< CANopen next event pointer.
  CANOPEN_EVENT_TYPE   EventType;                               ///< CANopen event type.
  CANOPEN_NODE         *NodePtr;                                ///< CANopen node structure.
  CPU_INT08U           IntNbr;                                  ///< CAN Bus interrupt number.
  CPU_INT08U           ErrCodeIx;                               ///< CANopen error code index in user emergency table.
  CANOPEN_EMCY_USR     *VendorErrCodePtr;                       ///< CANopen vendor error code.
} CANOPEN_EVENT;

/*
 ********************************************************************************************************
 *                                    CANopen node handle structure
 *******************************************************************************************************/

typedef struct canopen_handle {
  SLIST_MEMBER    *NodeListPtr;                                 ///< Pointer to a CANopen node list.
  SLIST_MEMBER    *FreeEventListPtr;                            ///< Pointer to a free event list.
  CANOPEN_EVENT   *EventHeadPtr;                                ///< Head of CANopen event list.
  CANOPEN_EVENT   *EventTailPtr;                                ///< Tail of CANopen event list.
  CPU_BOOLEAN     IsInit;                                       ///< Init flag.
  KAL_TASK_HANDLE SvcTaskHandle;                                ///< Handle to a service task structure.
  KAL_SEM_HANDLE  EventSemHandle;                               ///< Receive semaphore handle.
  CPU_INT32U      LockTimeoutMs;                                ///< Lock timeout.
} CANOPEN_HANDLE;

/****************************************************************************************************//**
 *                                       CANopen node structure
 *
 * @note    This data structure holds all information, which represents a complete CANopen node.
 *******************************************************************************************************/

struct canopen_node {
  SLIST_MEMBER        NodeList;
  CPU_INT32U          TmrPeriod;                                ///< Hardware timer period in microsecond.
  CANOPEN_DICT        Dict;                                     ///< The object dictionary.
  CANOPEN_IF          If;                                       ///< The bus interface.
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  CANOPEN_EMCY        Emcy;                                     ///< The node error state.
#endif
  CANOPEN_NMT         Nmt;                                      ///< The Network management.
  CANOPEN_TMR         Tmr;                                      ///< The Highspeed timer manager.
  CANOPEN_SDO         Sdo[CANOPEN_SDO_MAX_SERVER_QTY];          ///< The SDO Server Array.
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
  CPU_INT08U          *SdoBufPtr;                               ///< Pointer to SDO Transfer Buffer Start.
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
  CANOPEN_RPDO        RpdoTbl[CANOPEN_RPDO_MAX_QTY];            ///< The Receive PDO Array.
#endif
#if (CANOPEN_TPDO_MAX_QTY > 0)
  CANOPEN_TPDO        TpdoTbl[CANOPEN_TPDO_MAX_QTY];            ///< The Transmit PDO Array.
  CANOPEN_TPDO_LINK   TpdoMappingTbl[CANOPEN_TPDO_MAX_MAP_QTY]; ///< The Transmit PDO mapping link Array.
#endif
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  CANOPEN_SYNC        Sync;                                     ///< The SYNC management tables.
#endif
  CPU_INT32U          Baudrate;                                 ///< The default CAN Baudrate.
  CPU_INT08U          NodeId;                                   ///< The CANopen node id.
  CPU_CHAR            *NamePtr;                                 ///< CAN controller name associated to node.
  KAL_LOCK_HANDLE     ObjLock;                                  ///< Object dictionary lock.
  CPU_INT32U          LockTimeoutMs;                            ///< Lock timeout.
  CANOPEN_EVENT_FNCTS *EventFnctsPtr;                           ///< Pointer to events callback structure.
#if (CANOPEN_DBG_CTR_ERR_EN == DEF_ENABLED)
  CANOPEN_CTR_ERRS    *CntErrPtr;                               ///< Pointer to error counters.
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_NODE_HANDLE_SET(handle, p_node)         (handle).NodePtr = (void *)(p_node)

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void CANopen_SvcTaskWakeUp(CANOPEN_EVENT *p_event);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_CORE_PRIV_H_
