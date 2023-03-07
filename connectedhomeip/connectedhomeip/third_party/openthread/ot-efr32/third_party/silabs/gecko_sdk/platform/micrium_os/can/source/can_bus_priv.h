/***************************************************************************//**
 * @file
 * @brief Internal CAN Api
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

#ifndef  _CAN_BUS_PRIV_H_
#define  _CAN_BUS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                              INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/collections/slist_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <can/include/can_bus.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                        CAN Bus operational mode
 *******************************************************************************************************/

typedef enum can_bus_mode {
  CAN_BUS_MODE_NORMAL,
  CAN_BUS_MODE_BASIC,
  CAN_BUS_MODE_LOOPBACK,
  CAN_BUS_MODE_SILENTLOOPBACK,
  CAN_BUS_MODE_SILENT
} CAN_BUS_MODE;

/*
 ********************************************************************************************************
 *                                    CAN Bus RX callback function pointer
 *******************************************************************************************************/

typedef void (*CAN_BUS_ON_RX) (void       *p_data,
                               CPU_INT08U int_nbr);

/*
 ********************************************************************************************************
 *                                    CAN Bus TX callback function pointer
 *******************************************************************************************************/

typedef void (*CAN_BUS_ON_TX) (CAN_BUS_HANDLE bus_handle);

/*
 ********************************************************************************************************
 *                                  CAN Bus timer overflow function pointer
 *******************************************************************************************************/

typedef void (*CAN_BUS_ON_TMR_OVF) (void);

/****************************************************************************************************//**
 *                                      CAN Bus RX filter structure
 *
 * @brief    This structure is used to initialize a RX filter.
 *******************************************************************************************************/

typedef struct can_bus_rx_filter {
  CPU_INT32U  MsgNum;                                           ///< CAN message number.
  CPU_INT32U  Id;                                               ///< CAN message ID.
  CPU_INT32U  Mask;                                             ///< CAN message mask filter.
  CPU_BOOLEAN EndOfBuffer;
} CAN_BUS_RX_FILTER;

/****************************************************************************************************//**
 *                                  CAN Bus TX message object structure
 *
 * @brief    This structure is used to initialize a TX message object.
 *******************************************************************************************************/

typedef struct can_bus_tx_msg_obj {
  CPU_INT08U MsgNum;                                            ///< CAN message number.
  CPU_INT08U RemoteTransfer;                                    ///< Allow remote transfer.
  CPU_INT08U EndOfBuffer;                                       ///< End of message buffer.
} CAN_BUS_TX_MSG_OBJ;

/*
 ********************************************************************************************************
 *                                      CAN module root structure
 *******************************************************************************************************/

typedef struct can_bus_data {
  SLIST_MEMBER       *BusHandleListHeadPtr;                     ///< Head pointer of bus handle list.
  CAN_BUS_ON_TMR_OVF TmrOvfEvent;                               ///< On timer overflow callback.
} CAN_BUS_DATA;

/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                            CAN bus handle
 ********************************************************************************************************
 *******************************************************************************************************/

struct can_bus_handle {
  SLIST_MEMBER            ListNode;                             ///< Node for CAN bus handle list.
  const CPU_CHAR          *NamePtr;                             ///< Name of CAN bus controller.
  KAL_SEM_HANDLE          TxRdySemHandle;                       ///< CAN TX RdySemaphore handle.
  CPU_ADDR                RegBaseAddr;                          ///< CAN register address.
  const CAN_CTRLR_BSP_API *BusBspApiPtr;                        ///< BSP API pointer.
  CAN_BUS_ON_RX           OnRx;                                 ///< RX callback.
  CAN_BUS_ON_TX           OnTx;                                 ///< TX callback.
  CPU_INT08U              IF_Rx;                                ///< BSP Rx interface.
  CPU_INT08U              IF_Tx;                                ///< BSP Tx interface.
  void                    *BusDrvDataPtr;                       ///< Pointer to bus driver data.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const CAN_BUS_HANDLE CAN_BusHandleNull;

/****************************************************************************************************
 ****************************************************************************************************
 *                                       FUNCTION PROTOTYPES
 ****************************************************************************************************
 ***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CanBus_Init(RTOS_ERR *p_err);

CAN_BUS_HANDLE CanBus_Add(const CPU_CHAR *name,
                          void           *p_data,
                          RTOS_ERR       *p_err);

void CanBus_Start(CAN_BUS_HANDLE bus_handle,
                  CPU_INT32U     tmr_period,
                  CPU_INT32U     baudrate,
                  RTOS_ERR       *p_err);

void CanBus_BitTiming(CAN_BUS_HANDLE bus_handle,
                      CPU_INT32U     bitrate,
                      CPU_INT16U     propagation_time_segment,
                      CPU_INT16U     phase_buffer_segment1,
                      CPU_INT16U     phase_buffer_segment2,
                      CPU_INT16U     synchronisation_jump_width,
                      RTOS_ERR       *p_err);

void CanBus_TmrSubscribe(CAN_BUS_ON_TMR_OVF tim_fnct,
                         RTOS_ERR           *p_err);

CAN_BUS_HANDLE CanBus_HandleGetFromName(const CPU_CHAR *name);

void CanBus_Stop(CAN_BUS_HANDLE bus_handle,
                 RTOS_ERR       *p_err);

void CanBus_FilterSet(CAN_BUS_HANDLE    bus_handle,
                      CAN_BUS_RX_FILTER filter,
                      RTOS_ERR          *p_err);

void CanBus_RxSubscribe(CAN_BUS_HANDLE bus_handle,
                        CAN_BUS_ON_RX  rx_fnct,
                        RTOS_ERR       *p_err);

void CanBus_MsgObjSet(CAN_BUS_HANDLE     bus_handle,
                      CAN_BUS_TX_MSG_OBJ msg_obj,
                      RTOS_ERR           *p_err);

void CanBus_Wr(CAN_BUS_HANDLE bus_handle,
               CAN_BUS_ON_TX  tx_fnct,
               CPU_INT08U     msg_nbr,
               CPU_INT32U     can_id,
               CPU_INT08U     *p_buf,
               CPU_INT16U     size,
               RTOS_ERR       *p_err);

void CanBus_Rd(CAN_BUS_HANDLE bus_handle,
               CPU_INT08U     *p_msg_nbr,
               CPU_INT32U     *p_can_id,
               CPU_INT08U     *p_buf,
               CPU_INT08U     *p_size,
               RTOS_ERR       *p_err);

void CanBus_MsgReset(CAN_BUS_HANDLE bus_handle,
                     RTOS_ERR       *p_err);

void CanBus_ModeSet(CAN_BUS_HANDLE bus_handle,
                    CAN_BUS_MODE   mode,
                    RTOS_ERR       *p_err);

void CanBus_BitTimingSet(CAN_BUS_HANDLE bus_handle,
                         CPU_INT32U     bitrate,
                         CPU_INT16U     propagation_time_segment,
                         CPU_INT16U     phase_buffer_segment1,
                         CPU_INT16U     phase_buffer_segment2,
                         CPU_INT16U     sync_jump_width,
                         RTOS_ERR       *p_err);

void CanBus_ISRHandler(CAN_BUS_HANDLE bus_handle);

void CanBus_TmrISRHandler(void);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CAN_BUS_PRIV_H_
