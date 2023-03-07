/***************************************************************************//**
 * @file
 * @brief IO - Public SD Api
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup IO_SD SD
 * @brief      IO SD API
 * ingroup  IO
 *
 * @addtogroup IO_SD
 * @{
 *******************************************************************************************************/

#ifndef  _SD_H_
#define  _SD_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN
#define  IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN         DEF_DISABLED
#endif

/********************************************************************************************************
 *                                       OCR REGISTER BIT DEFINES
 *
 * Note(s) : (1) See [Ref 1], Table 5-1, & [Ref 2], Section 8.1.
 *******************************************************************************************************/

#define  SD_OCR_LVR                             DEF_BIT_07      // Card is dual-voltage.
#define  SD_OCR_20_21V                          DEF_BIT_08      // Card supports 2.0-2.1V.
#define  SD_OCR_21_22V                          DEF_BIT_09      // Card supports 2.1-2.2V.
#define  SD_OCR_22_33V                          DEF_BIT_10      // Card supports 2.2-2.3V.
#define  SD_OCR_23_24V                          DEF_BIT_11      // Card supports 2.3-2.4V.
#define  SD_OCR_24_25V                          DEF_BIT_12      // Card supports 2.4-2.5V.
#define  SD_OCR_25_26V                          DEF_BIT_13      // Card supports 2.5-2.6V.
#define  SD_OCR_26_27V                          DEF_BIT_14      // Card supports 2.6-2.7V.
#define  SD_OCR_27_28V                          DEF_BIT_15      // Card supports 2.7-2.8V.
#define  SD_OCR_28_29V                          DEF_BIT_16      // Card supports 2.8-2.9V.
#define  SD_OCR_29_30V                          DEF_BIT_17      // Card supports 2.9-3.0V.
#define  SD_OCR_30_31V                          DEF_BIT_18      // Card supports 3.0-3.1V.
#define  SD_OCR_31_32V                          DEF_BIT_19      // Card supports 3.1-3.2V.
#define  SD_OCR_32_33V                          DEF_BIT_20      // Card supports 3.2-3.3V.
#define  SD_OCR_33_34V                          DEF_BIT_21      // Card supports 3.3-3.4V.
#define  SD_OCR_34_35V                          DEF_BIT_22      // Card supports 3.4-3.5V.
#define  SD_OCR_35_36V                          DEF_BIT_23      // Card supports 3.5-3.6V.
#define  SD_OCR_CCS                             DEF_BIT_30      // HC card.
#define  SD_OCR_BUSY                            DEF_BIT_31      // Card has finished power-up routine.

//                                                                 Required OCR for SD v2 card and above.
#define  SD_V2_OCR_VOLTAGE_MASK                (SD_OCR_27_28V | SD_OCR_28_29V | SD_OCR_29_30V   \
                                                | SD_OCR_30_31V | SD_OCR_31_32V | SD_OCR_32_33V \
                                                | SD_OCR_33_34V | SD_OCR_34_35V | SD_OCR_35_36V)

/********************************************************************************************************
 *                                           SD CAPABILITIES
 *******************************************************************************************************/

//                                                                 --------------- VOLTAGE CAPABILITIES ---------------
#define  SD_CAP_BUS_VOLT_3_3               DEF_BIT_00
#define  SD_CAP_BUS_VOLT_3                 DEF_BIT_01
#define  SD_CAP_BUS_VOLT_1_8               DEF_BIT_02

#define  SD_CAP_BUS_SIGNAL_VOLT_3_3        DEF_BIT_06
#define  SD_CAP_BUS_SIGNAL_VOLT_1_8        DEF_BIT_07

//                                                                 -------------- BUS WIDTH CAPABILITIES --------------
#define  SD_CAP_BUS_WIDTH_1_BIT            DEF_BIT_03
#define  SD_CAP_BUS_WIDTH_4_BIT            DEF_BIT_04
#define  SD_CAP_BUS_WIDTH_8_BIT            DEF_BIT_05

//                                                                 --------------- CAPABILITIES' MASKS ----------------
#define  SD_CAP_BUS_SIGNAL_VOLT_MASK      (SD_CAP_BUS_SIGNAL_VOLT_1_8 | SD_CAP_BUS_SIGNAL_VOLT_3_3)

/****************************************************************************************************//**
 *                                           IO_SD_CTRLR_REG()
 *
 * @brief      Registers a IO SD controller to the platform manager.
 *
 * @param      name        Unique name for the IO SD controller. It is recommended to follow the standard
 *                         "sdX",  where X is a digit.
 *
 * @param      p_drv_info  Pointer to the serial driver hardware information structure of type
 *                         SD_CARD_CTRLR_DRV_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

#define  IO_SD_CARD_CTRLR_REG(name, p_drv_info)                  \
  do {                                                           \
    if ((p_drv_info)->DrvAPI_Ptr != DEF_NULL) {                  \
      static const struct  _sd_ctrlr_pm_item _item = {           \
        .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SD_CTRLR, \
        .Item.StrID = (name),                                    \
        .BusDrvAPI_Ptr = &SD_CardBusDrvAPI,                      \
        .BusDrvInfoPtr = (void *)(p_drv_info)                    \
      };                                                         \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item);  \
    }                                                            \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                               SD card types
 *******************************************************************************************************/

typedef  enum  sd_cardtype {
  SD_CARDTYPE_NONE,                                             ///< Card type unspecified.
  SD_CARDTYPE_SD_V1_X,                                          ///< v1.x SD card.
  SD_CARDTYPE_SD_V2_0,                                          ///< v2.0 standard-capacity SD card.
  SD_CARDTYPE_SD_V2_0_HC,                                       ///< v2.0 high-capacity SD card.
  SD_CARDTYPE_MMC,                                              ///< Standard-capacity MMC (< 2GB).
  SD_CARDTYPE_MMC_HC                                            ///< High-capacity MMC (>= 2GB).
} SD_CARDTYPE;

/*
 ********************************************************************************************************
 *                                           Handle to SD bus
 *******************************************************************************************************/

typedef  struct  sd_bus_handle *  SD_BUS_HANDLE;

/*
 ********************************************************************************************************
 *                                           Handle to SD function
 *******************************************************************************************************/

typedef  CPU_INT08U SD_FNCT_HANDLE;

/****************************************************************************************************//**
 *                                           SD EVENT FUNCTIONS
 *
 * @note     (1) This structure contains a set of application generic callbacks.
 *******************************************************************************************************/

typedef  const struct  sd_event_fncts {
  //                                                               Indicates card was connected and is ready for
  //                                                               communication.
  void (*CardConn)(SD_BUS_HANDLE bus_handle);

  //                                                               Indicates card was disconnected.
  void (*CardDisconn)(SD_BUS_HANDLE bus_handle);

  //                                                               Indicates card initialization has failed.
  void (*CardConnFail)(SD_BUS_HANDLE bus_handle,
                       RTOS_ERR      err);

  //                                                               Indicates initialization of an IO function failed.
  void (*IO_FnctConnFail)(SD_BUS_HANDLE bus_handle,
                          CPU_INT08U    fnct_nbr,
                          RTOS_ERR      err);
} SD_EVENT_FNCTS;

/****************************************************************************************************//**
 *                       SD IO asynchronous transfer complete function callback
 *
 * @note     bus_handle      Handle to SD bus.
 *
 * @note     fnct_handle     Handle to SD IO function.
 *
 * @note     p_buf           Pointer to transferred data buffer.
 *
 * @note     blk_qty         Quantity of transferred blocks.
 *
 * @note     blk_len         Length of blocks, in octets.
 *
 * @note     p_async_data    Pointer to function driver's specific data associated to transfer.
 *
 * @note     err             Error variable.
 *******************************************************************************************************/

typedef  void (*SD_ASYNC_FNCT)  (SD_BUS_HANDLE  bus_handle,
                                 SD_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U     *p_buf,
                                 CPU_INT32U     blk_qty,
                                 CPU_INT32U     blk_len,
                                 void           *p_async_data,
                                 RTOS_ERR       err);

/*
 ********************************************************************************************************
 *                               SD initialization configuration structure
 *******************************************************************************************************/

typedef  struct  sd_init_cfg {
  CPU_INT32U     CoreTaskStkSizeElements;                       ///< Core task's stack size, in quantity of elements.
  void           *CoreTaskStkPtr;                               ///< Pointer to core stack's stack base.
  CPU_INT32U     AsyncTaskStkSizeElements;                      ///< Async task's stack size, in quantity of elements.
  void           *AsyncTaskStkPtr;                              ///< Pointer to async stack's stack base.

  CPU_SIZE_T     IO_FnctQtyTot;                                 ///< Total quantity of SDIO functions.
  CPU_SIZE_T     EventQtyTot;                                   ///< Total quantity of SD events.
  CPU_SIZE_T     XferQtyTot;                                    ///< Total quantity of simultaneous transfers.

  MEM_SEG        *MemSegPtr;                                    ///< Pointer to memory segment used for internal data.
  MEM_SEG        *MemSegBufPtr;                                 ///< Pointer to memory segment used for data buffer.

  SD_EVENT_FNCTS *EventFnctsPtr;                                ///< Pointer to event functions structure.
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  CPU_INT32U     CoreCardPollingPeriodMs;                       ///< Card Polling Period, in milliseconds.
#endif
} SD_INIT_CFG;

/*
 ********************************************************************************************************
 *                                       SD bus driver API structure
 *******************************************************************************************************/

typedef  struct  sd_bus_drv_api SD_BUS_DRV_API;

/****************************************************************************************************//**
 *                                   SD controller platform manager item
 *
 * @note     (1) This structure should ONLY be instantiated via the macro IO_SERIAL_CTRLR_REG().
 *******************************************************************************************************/

struct  _sd_ctrlr_pm_item {
  PLATFORM_MGR_ITEM    Item;                                    ///< Platform manager item.
  const SD_BUS_DRV_API *BusDrvAPI_Ptr;                          ///< Pointer to bus driver API structure.
  void                 *BusDrvInfoPtr;                          ///< Pointer to bus driver information structure.
};

/*
 ********************************************************************************************************
 *                                           SD capabilities
 *******************************************************************************************************/

//                                                                 Bitmap representing various capabilities of SD host
//                                                                 controller or SD Card. Can be a bitmap of the
//                                                                 following values:
//                                                                 - SD_CAP_BUS_VOLT_3_3
//                                                                 - SD_CAP_BUS_VOLT_3
//                                                                 - SD_CAP_BUS_VOLT_1_8
//                                                                 - SD_CAP_BUS_SIGNAL_VOLT_3_3
//                                                                 - SD_CAP_BUS_SIGNAL_VOLT_1_8
//                                                                 - SD_CAP_BUS_WIDTH_1_BIT
//                                                                 - SD_CAP_BUS_WIDTH_4_BIT
//                                                                 - SD_CAP_BUS_WIDTH_8_BIT
typedef  CPU_INT08U SD_CAP_BITMAP;

/*
 ********************************************************************************************************
 *                                           SD host capabilities
 *******************************************************************************************************/

typedef  struct  sd_host_capabilities {
  //                                                               Bitmap representing various capabilities of SD host
  //                                                               controller.
  SD_CAP_BITMAP Capabilities;

  //                                                               Bitmap representing the supported operation
  //                                                               conditions (VDD supply) by SD host controller.
  //                                                               Format is the same as OCR register described in
  //                                                               section 5.1 of "SD Specifications: Physical Layer
  //                                                               Specification, version 4.20".
  CPU_INT32U OCR;
} SD_HOST_CAPABILITIES;

/*
 ********************************************************************************************************
 *                                           SD card capabilities
 *******************************************************************************************************/

typedef  struct  sd_card_capabilities {
  SD_CAP_BITMAP Capabilities;
} SD_CARD_CAPABILITIES;

/*
 ********************************************************************************************************
 *                                           SD capabilities
 *******************************************************************************************************/

typedef  struct  sd_capabilities {
  SD_HOST_CAPABILITIES Host;
  SD_CARD_CAPABILITIES Card;
} SD_CAPABILITIES;

/*
 ********************************************************************************************************
 *                                         SD card detect mode
 *******************************************************************************************************/

typedef  enum  sd_card_detect_mode {
  SD_CARD_DETECT_MODE_NONE,
  SD_CARD_DETECT_MODE_INTERRUPT,                                ///< Card detected by SDHC or SPI controller interrupt.
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  SD_CARD_DETECT_MODE_POLLING,                                  ///< Card detected by Core task polling.
#endif
  SD_CARD_DETECT_MODE_BSP_EVENT,                                ///< Card detected by BSP IO interrupt or polling.
  SD_CARD_DETECT_MODE_WIRED                                     ///< Card detected by bus Start() function.
} SD_CARD_DETECT_MODE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const SD_BUS_DRV_API SD_CardBusDrvAPI;

extern const SD_BUS_HANDLE  SD_BusHandleNull;
extern const SD_FNCT_HANDLE SD_FnctHandleNull;

extern const SD_INIT_CFG SD_InitCfgDflt;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureMemSeg(MEM_SEG *p_seg,
                        MEM_SEG *p_seg_buf);

void SD_ConfigureIO_FnctHandleQty(CPU_SIZE_T fnct_handle_qty);

void SD_ConfigureEventQty(CPU_SIZE_T event_qty);

void SD_ConfigureXferQty(CPU_SIZE_T xfer_qty);

void SD_ConfigureEventFncts(const SD_EVENT_FNCTS *p_event_fncts);

void SD_ConfigureCoreTaskStk(CPU_INT32U stk_size_elements,
                             void       *p_stk);

void SD_ConfigureAsyncTaskStk(CPU_INT32U stk_size_elements,
                              void       *p_stk);

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
void SD_ConfigureCoreCardPollingPeriod(CPU_INT32U period_ms);
#endif
#endif

void SD_CoreTaskPrioSet(CPU_INT08U prio,
                        RTOS_ERR   *p_err);

void SD_AsyncTaskPrioSet(CPU_INT08U prio,
                         RTOS_ERR   *p_err);

void SD_OperTimeoutSet(CPU_INT32U timeout_ms,
                       RTOS_ERR   *p_err);

SD_BUS_HANDLE SD_BusAdd(const CPU_CHAR *name,
                        RTOS_ERR       *p_err);

SD_BUS_HANDLE SD_BusHandleGetFromName(const CPU_CHAR *name);

void SD_BusStart(SD_BUS_HANDLE bus_handle,
                 RTOS_ERR      *p_err);

void SD_BusStop(SD_BUS_HANDLE bus_handle,
                RTOS_ERR      *p_err);

CPU_SIZE_T SD_AlignReqGet(SD_BUS_HANDLE bus_handle,
                          RTOS_ERR      *p_err);

SD_CARDTYPE SD_CardTypeGet(SD_BUS_HANDLE bus_handle,
                           RTOS_ERR      *p_err);

void SD_BSP_BusCardDetectEvent(SD_BUS_HANDLE bus_handle);

void SD_BSP_BusCardRemoveEvent(SD_BUS_HANDLE bus_handle);

#ifdef  __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
