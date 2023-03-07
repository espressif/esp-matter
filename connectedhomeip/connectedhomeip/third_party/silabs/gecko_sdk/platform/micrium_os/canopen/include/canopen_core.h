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

/****************************************************************************************************//**
 * @defgroup CANOPEN CANopen API
 * @ingroup CAN
 * @brief   CANopen API
 *
 * @defgroup CANOPEN_CORE CANopen Core API
 * @ingroup CANOPEN
 * @brief CANopen Core API
 *
 * @addtogroup CANOPEN_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_CORE_H_
#define _CANOPEN_CORE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen_cfg.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <canopen/include/canopen_emcy.h>
#include  <canopen/include/canopen_if.h>
#include  <canopen/include/canopen_param.h>
#include  <canopen/include/canopen_types.h>
#include  <canopen/include/canopen_obj.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) As per the CANopen CiA-301 specification, the number of SDO servers is limited to 128.
 *               Object indexes relating to SDO range from 0x1200 to 0x127F for the SDO server parameter
 *               (cf. 'Table 74: Standard objects').
 *
 *           (2) RPDO information:
 *
 *               (a) As per the CANopen CiA-301 specification, the number of RPDOs is limited to 512.
 *                   Object indexes relating to RPDO range from 0x1400 to 0x15FF for the RPDO communication
 *                   parameter and from 0x1600 to 0x17FF for the RPDO mapping parameter (cf. 'Table 74:
 *                   Standard objects').
 *
 *               (b) As per the CANopen CiA-301 specification, the number of linked objects for each RPDO
 *                   is limited to 64 (cf. 'Table 69: RPDO mapping values').
 *
 *           (3) TPDO information:
 *
 *               (a) As per the CANopen CiA-301 specification, the number of TPDOs is limited to 512.
 *                   Object indexes relating to RPDO range from 0x1800 to 0x19FF for the TPDO communication
 *                   parameter and from 0x1A00 to 0x1BFF for the TPDO mapping parameter (cf. 'Table 74:
 *                   Standard objects').
 *
 *               (b) As per the CANopen CiA-301 specification, the number of linked objects for each TPDO
 *                   is limited to 64 (cf. 'Table 73: TPDO mapping values').
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_SDO_SPEC_MAX_SERVER_QTY        128u            // See Note #1.

#define  CANOPEN_RPDO_SPEC_MAX_QTY              512u            // See Note #2a.
#define  CANOPEN_RPDO_MAPPING_MAX_VALID_OBJ     64u             // See Note #2b.

#define  CANOPEN_TPDO_SPEC_MAX_QTY              512u            // See Note #3a.
#define  CANOPEN_TPDO_MAPPING_MAX_VALID_OBJ     64u             // See Note #3b.

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                               CANopen events structure callback
 *******************************************************************************************************/

typedef const struct canopen_event_fncts {
  //                                                               This callback is used when a RPDO is received.
  CPU_INT16S (*RpdoOnRx)(CANOPEN_NODE_HANDLE handle,
                         CANOPEN_IF_FRM      *p_frm);
  //                                                               This callback is used before a TPDO is transmitted.
  void (*TpdoOnTx)(CANOPEN_NODE_HANDLE handle,
                   CANOPEN_IF_FRM      *p_frm);
  //                                                               This callback is used when the NMT state is
  //                                                               changed.
  void (*StateOnChange)(CANOPEN_NODE_HANDLE handle,
                        CANOPEN_NODE_STATE  state);
  //                                                               This callback is used when a heartbeat consumer
  //                                                               monitor timer elapses, before receiving the
  //                                                               corresponding heartbeat message.
  void (*HbcOnEvent)(CANOPEN_NODE_HANDLE handle,
                     CPU_INT08U          node_id);
  //                                                               This callback is used when a heartbeat consumer
  //                                                               monitor detects a state change, of a monitored
  //                                                               node.
  void (*HbcOnChange)(CANOPEN_NODE_HANDLE handle,
                      CPU_INT08U          node_id,
                      CANOPEN_NODE_STATE  state);
  //                                                               This callback is used when the NMT slave node
  //                                                               is reseted by the NMT master node.
  CPU_BOOLEAN (*ParamOnLoad)(CANOPEN_NODE_HANDLE handle,
                             CANOPEN_PARAM       *p_pg);
  //                                                               This callback is used when the standard object
  //                                                               "Store" at index 0x1010 is written.
  CPU_BOOLEAN (*ParamOnSave)(CANOPEN_NODE_HANDLE handle,
                             CANOPEN_PARAM       *p_pg);
  //                                                               This callback is used when the standard object
  //                                                               "Restore default parameters" at index 0x1011 is
  //                                                               written.
  CPU_BOOLEAN (*ParamOnDflt)(CANOPEN_NODE_HANDLE handle,
                             CANOPEN_PARAM       *p_pg);
} CANOPEN_EVENT_FNCTS;

/****************************************************************************************************//**
 *                                CANopen node specification structure
 *
 * @note     This data structure holds all configurable components of a complete CANopen node.
 *******************************************************************************************************/

typedef struct canopen_node_spec {
  CPU_INT08U       NodeId;                                      ///< Default Node ID.
  CPU_INT32U       Baudrate;                                    ///< Default baudrate for this node.
  CANOPEN_OBJ      *DictPtr;                                    ///< Pointer to object dictionary associated to node.
  CANOPEN_EMCY_TBL *EmcyCodePtr;                                ///< Pointer to application EMCY information fields.
  CANOPEN_TMR_MEM  *TmrMemPtr;                                  ///< Pointer to timer memory blocks.
  CPU_INT16U       TmrQty;                                      ///< Number of timer memory blocks.
  CPU_INT08U       *SdoBufPtr;                                  ///< Pointer to SDO transfer buffer memory.
} CANOPEN_NODE_SPEC;

/*
 ********************************************************************************************************
 *                               CANopen initialization configuration structure
 *******************************************************************************************************/

typedef struct canopen_init_cfg {
  CPU_INT32U SvcTaskStkSizeElements;                            ///< Service task's stack size, in quantity of elements.
  void       *SvcTaskStkPtr;                                    ///< Pointer to Service stack's stack base.
  MEM_SEG    *MemSegPtr;                                        ///< Pointer to memory segment used for internal data.
  CPU_SIZE_T EventQtyTot;                                       ///< Number of events allocated from the pool.
  CPU_INT32U HwTmrPeriod;                                       ///< Timer base time in microsecond.
} CANOPEN_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CANopen_ConfigureMemSeg(MEM_SEG *p_seg);

void CANopen_ConfigureEventQty(CPU_SIZE_T event_qty);

void CANopen_ConfigureSvcTaskStk(CPU_INT32U stk_size_elements,
                                 void       *p_stk);

void CANopen_ConfigureTmrPeriod(CPU_INT32U tmr_period);

void CANopen_Init(RTOS_ERR *p_err);

CANOPEN_NODE_HANDLE CANopen_NodeAdd(const CPU_CHAR            *p_name,
                                    const CANOPEN_NODE_SPEC   *p_spec,
                                    const CANOPEN_EVENT_FNCTS *p_event_fctns,
                                    RTOS_ERR                  *p_err);

void CANopen_NodeStart(CANOPEN_NODE_HANDLE node_handle,
                       RTOS_ERR            *p_err);

void CANopen_NodeStop(CANOPEN_NODE_HANDLE node_handle,
                      RTOS_ERR            *p_err);
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
void CANopen_NodeParamLoad(CANOPEN_NODE_HANDLE node_handle,
                           CANOPEN_NMT_RESET   reset_type,
                           RTOS_ERR            *p_err);
#endif

void CANopen_NodeLockTimeoutSet(CANOPEN_NODE_HANDLE node_handle,
                                CPU_INT32U          timeout,
                                RTOS_ERR            *p_err);

void CANopen_SvcTaskPrioSet(CPU_INT08U prio,
                            RTOS_ERR   *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         CONFIGURATION ERROR
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_SDO_MAX_SERVER_QTY < 1)
#error "canopen_core.h: CANOPEN_SDO_MAX_SERVER_QTY must be > 0"
#endif

#if (CANOPEN_SDO_MAX_SERVER_QTY > CANOPEN_SDO_SPEC_MAX_SERVER_QTY)
#error "canopen_core.h: CANOPEN_SDO_MAX_SERVER_QTY must be <= 128 (maximum SDO servers allowed by the specification)."
#endif

#if ((CANOPEN_SDO_MAX_SERVER_QTY == 1) && (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_SDO_MAX_SERVER_QTY  must be > 1 when setting CANOPEN_SDO_DYN_ID_EN to DEF_ENABLED"
#endif

#if ((CANOPEN_EMCY_MAX_ERR_QTY == 0) && (CANOPEN_EMCY_HIST_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_EMCY_MAX_ERR_QTY must be > 0 when setting CANOPEN_EMGY_HIST_EN to DEF_ENABLED"
#endif

#if (CANOPEN_EMCY_MAX_ERR_QTY > CANOPEN_EMCY_MAX_ERR_CODE)
#error "canopen_core.h: CANOPEN_EMCY_MAX_ERR_QTY must be <= 256."
#endif

#if ((CANOPEN_EMCY_HIST_EN != DEF_ENABLED) && (CANOPEN_EMCY_HIST_MAN_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_EMCY_HIST_EN must be DEF_ENABLED when setting CANOPEN_EMCY_HIST_MAN_EN to DEF_ENABLED"
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN != DEF_ENABLED) && (CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_RPDO_DYN_COM_EN must be DEF_ENABLED when setting CANOPEN_RPDO_DYN_MAP_EN to DEF_ENABLED"
#endif

#if (CANOPEN_RPDO_MAX_QTY > CANOPEN_RPDO_SPEC_MAX_QTY)
#error "canopen_core.h: CANOPEN_RPDO_MAX_QTY must be <= 512 (maximum RPDOs allowed by the specification)."
#endif

#if (CANOPEN_RPDO_MAX_QTY > 0u) && (CANOPEN_RPDO_MAX_MAP_QTY == 0u)
#error "canopen_core.h: CANOPEN_RPDO_MAX_MAP_QTY must be > 0 when CANOPEN_RPDO_MAX_QTY is > 0."
#endif

#if (CANOPEN_RPDO_MAX_MAP_QTY > CANOPEN_RPDO_MAPPING_MAX_VALID_OBJ)
#error "canopen_core.h: CANOPEN_RPDO_MAX_MAP_QTY must be <= 64"
#endif

#if (CANOPEN_TPDO_MAX_QTY > CANOPEN_TPDO_SPEC_MAX_QTY)
#error "canopen_core.h: CANOPEN_TPDO_MAX_QTY cannot be greater than 512 (maximum TPDOs allowed by the specification)."
#endif

#if (CANOPEN_TPDO_MAX_QTY > 0u) && (CANOPEN_TPDO_MAX_MAP_QTY == 0u)
#error "canopen_core.h: CANOPEN_TPDO_MAX_MAP_QTY must be > 0 when CANOPEN_TPDO_MAX_QTY is > 0."
#endif

#if ((CANOPEN_TPDO_MAX_QTY == 0) && (CANOPEN_RPDO_MAX_QTY == 0) && (CANOPEN_SYNC_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_TPDO_MAX_QTY or CANOPEN_RPDO_MAX_QTY must be > 0 when setting CANOPEN_SYNC_EN to DEF_ENABLED"
#endif

#if (CANOPEN_TPDO_MAX_MAP_QTY > CANOPEN_TPDO_MAPPING_MAX_VALID_OBJ)
#error "canopen_core.h: CANOPEN_TPDO_MAX_MAP_QTY must be <= 64"
#endif

#if ((CANOPEN_TPDO_DYN_COM_EN != DEF_ENABLED) && (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
#error "canopen_core.h: CANOPEN_TPDO_DYN_COM_EN must be DEF_ENABLED when setting CANOPEN_TPDO_DYN_MAP_EN to DEF_ENABLED"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_CORE_H_
