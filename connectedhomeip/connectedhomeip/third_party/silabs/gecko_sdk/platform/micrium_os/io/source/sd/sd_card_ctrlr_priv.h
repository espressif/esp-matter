/***************************************************************************//**
 * @file
 * @brief Sd Controller Declarations
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

#ifndef  _SD_CARD_CTRLR_PRIV_H_
#define  _SD_CARD_CTRLR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CMD FLAG DEFINES
 *
 * Note(s) : (a) If SD_CARD_CMD_FLAG_INIT is set, then the controller should send the 80-clock
 *               initialization sequence before transmitting the command.
 *
 *           (b) If SD_CARD_CMD_FLAG_OPEN_DRAIN is set, then the command should be transmitted
 *               with the command line in an open drain state.
 *
 *           (c) If SD_CARD_CMD_FLAG_BUSY is set, then the controller should check for busy after
 *               the response before transmitting any data.
 *
 *           (d) If SD_CARD_CMD_FLAG_CRC_VALID is set, then CRC check should be enabled.
 *
 *           (e) If SD_CARD_CMD_FLAG_IX_VALID is set, then index check should be enabled.
 *
 *           (f) If SD_CARD_CMD_FLAG_START_DATA_TX is set, then this command will either start
 *               transmitting data, or expect to start receiving data.
 *
 *           (g) If SD_CARD_CMD_FLAG_STOP_DATA_TX is set, then this command is attempting to stop
 *               (abort) data transmission/reception.
 *
 *           (h) If SD_CARD_CMD_FLAG_RESP is set, then a response is expected for this command.
 *
 *           (i) If SD_CARD_CMD_FLAG_RESP_LONG is set, then a long response is expected for this
 *               command.
 *******************************************************************************************************/

#define  SD_CARD_CMD_FLAG_NONE             DEF_BIT_NONE
#define  SD_CARD_CMD_FLAG_BUSY             DEF_BIT_00           // Busy signal expected after command.
#define  SD_CARD_CMD_FLAG_CRC_VALID        DEF_BIT_01           // CRC valid after command.
#define  SD_CARD_CMD_FLAG_IX_VALID         DEF_BIT_02           // Index valid after command.
#define  SD_CARD_CMD_FLAG_RESP             DEF_BIT_03           // Response expected.
#define  SD_CARD_CMD_FLAG_RESP_LONG        DEF_BIT_04           // Long response expected.
#define  SD_CARD_CMD_FLAG_DATA_ABORT       DEF_BIT_05           // Data abort command (CMD12, I/O abort in CCCR).
#define  SD_CARD_CMD_FLAG_DATA_RESUME      DEF_BIT_06           // Data stop  command (Fnct sel in CCCR).
#define  SD_CARD_CMD_FLAG_DATA_SUSPEND     DEF_BIT_07           // Data stop  command (Bus suspend in CCCR).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                           Response types
 *******************************************************************************************************/

typedef enum sd_card_resp_type {
  SD_CARD_RESP_TYPE_UNKNOWN,
  SD_CARD_RESP_TYPE_NONE,
  SD_CARD_RESP_TYPE_R1,
  SD_CARD_RESP_TYPE_R1B,
  SD_CARD_RESP_TYPE_R2,
  SD_CARD_RESP_TYPE_R3,
  SD_CARD_RESP_TYPE_R4,
  SD_CARD_RESP_TYPE_R5,
  SD_CARD_RESP_TYPE_R5B,
  SD_CARD_RESP_TYPE_R6,
  SD_CARD_RESP_TYPE_R7
} SD_CARD_RESP_TYPE;

/*
 ********************************************************************************************************
 *                                           SD card bus handle
 *******************************************************************************************************/

typedef struct sd_card_bus *  SD_CARD_BUS_HANDLE;

/*
 ********************************************************************************************************
 *                                           Data direction
 *******************************************************************************************************/

typedef enum sd_card_data_dir {
  SD_CARD_DATA_DIR_NONE,
  SD_CARD_DATA_DIR_HOST_TO_CARD,
  SD_CARD_DATA_DIR_CARD_TO_HOST
} SD_CARD_DATA_DIR;

/*
 ********************************************************************************************************
 *                                               Data type
 *******************************************************************************************************/

typedef enum sd_card_data_type {
  SD_CARD_DATA_TYPE_NONE,
  SD_CARD_DATA_TYPE_SINGLE_BLOCK,
  SD_CARD_DATA_TYPE_MULTI_BLOCK,
  SD_CARD_DATA_TYPE_STREAM
} SD_CARD_DATA_TYPE;

/*
 ********************************************************************************************************
 *                                           SD card bus voltage
 *******************************************************************************************************/

typedef enum sd_card_bus_volt {
  SD_CARD_BUS_VOLT_NONE,
  SD_CARD_BUS_VOLT_3_3,                                         ///< 3.3 volts.
  SD_CARD_BUS_VOLT_3,                                           ///< 3 volts.
  SD_CARD_BUS_VOLT_1_8                                          ///< 1.8 volts.
} SD_CARD_BUS_VOLT;

/*
 ********************************************************************************************************
 *                                       SD card controller reset level
 *******************************************************************************************************/

typedef enum sd_card_ctrlr_reset_lvl {
  SD_CARD_CTRLR_RESET_LVL_CMD,                                  ///< Reset command line.
  SD_CARD_CTRLR_RESET_LVL_DATA,                                 ///< Reset data line(s).
  SD_CARD_CTRLR_RESET_LVL_ALL                                   ///< Reset entire host controller except card detection.
} SD_CARD_CTRLR_RESET_LVL;

/*
 ********************************************************************************************************
 *                                   SD command description structure
 *******************************************************************************************************/

typedef struct sd_card_cmd {
  CPU_INT08U        Cmd;                                        ///< Command number.
  CPU_INT32U        Arg;                                        ///< Command argument.
  CPU_INT32U        Flags;                                      ///< Command flags.
  SD_CARD_RESP_TYPE RespType;                                   ///< Response type.
  SD_CARD_DATA_DIR  DataDir;                                    ///< Data transfer direction.
  SD_CARD_DATA_TYPE DataType;                                   ///< Data transfer type.
} SD_CARD_CMD;

/*
 ********************************************************************************************************
 *                                       SD card driver structure
 *******************************************************************************************************/

struct sd_card_drv {
  void                        *DataPtr;                         ///< Pointer to driver's data structure.

  const SD_CARD_CTRLR_HW_INFO *HW_InfoPtr;                      ///< Pointer to hardware information strcture.
  const SD_CARD_CTRLR_DRV_API *DrvAPI_Ptr;                      ///< Pointer to driver API structure.
  const SD_CARD_CTRLR_BSP_API *BSP_API_Ptr;                     ///< Pointer to BSP API structure.
};

/*
 ********************************************************************************************************
 *                               SD card controller driver API structure
 *******************************************************************************************************/

struct sd_card_ctrlr_drv_api {
  //                                                               Initializes driver.
  void (*Init)                (SD_CARD_DRV        *p_drv,
                               SD_CARD_BUS_HANDLE handle,
                               MEM_SEG            *p_mem_seg,
                               RTOS_ERR           *p_err);

  //                                                               Initializes SD controller.
  void (*InitHW)               (SD_CARD_DRV *p_drv,
                                RTOS_ERR    *p_err);

  //                                                               Starts driver.
  void (*Start)                (SD_CARD_DRV *p_drv,
                                RTOS_ERR    *p_err);

  //                                                               Stops driver.
  void (*Stop)                 (SD_CARD_DRV *p_drv,
                                RTOS_ERR    *p_err);

  //                                                               Sets SD clock frequency.
  void (*ClkFreqSet)           (SD_CARD_DRV *p_drv,
                                CPU_INT32U  freq,
                                RTOS_ERR    *p_err);

  //                                                               Sets bus voltage.
  void (*BusSupplyVoltSet)     (SD_CARD_DRV      *p_drv,
                                SD_CARD_BUS_VOLT volt,
                                RTOS_ERR         *p_err);

  //                                                               Updates bus voltage.
  void (*BusSignalVoltSet)     (SD_CARD_DRV             *p_drv,
                                SD_CARD_BUS_SIGNAL_VOLT volt,
                                RTOS_ERR                *p_err);

  SD_CARD_BUS_SIGNAL_VOLT (*BusSignalVoltGet)     (SD_CARD_DRV *p_drv,
                                                   RTOS_ERR    *p_err);

  //                                                               Sets bus data width.
  void (*BusWidthSet)          (SD_CARD_DRV *p_drv,
                                CPU_INT08U  width,
                                RTOS_ERR    *p_err);

  //                                                               Gets the data lines state.
  CPU_INT08U (*DataLinesLvlGet)      (SD_CARD_DRV *p_drv,
                                      RTOS_ERR    *p_err);

  //                                                               Starts SD command.
  void (*CmdStart)             (SD_CARD_DRV *p_drv,
                                SD_CARD_CMD *p_cmd,
                                CPU_INT08U  *p_resp,
                                RTOS_ERR    *p_err);

  //                                                               Gets command response.
  void (*CmdRespGet)           (SD_CARD_DRV *p_drv,
                                CPU_INT08U  *p_resp,
                                RTOS_ERR    *p_err);

  //                                                               Submits buffer for data transfer.
  void (*DataBufSubmit)        (SD_CARD_DRV *p_drv,
                                CPU_INT08U  *p_buf,
                                CPU_INT32U  blk_cnt,
                                CPU_INT32U  blk_size,
                                CPU_BOOLEAN dir_is_rd,
                                RTOS_ERR    *p_err);

  //                                                               Completes a data transfer.
  void (*DataXferCmpl)         (SD_CARD_DRV *p_drv,
                                CPU_INT08U  *p_buf,
                                CPU_INT32U  blk_cnt,
                                CPU_INT32U  blk_size,
                                CPU_BOOLEAN dir_is_rd,
                                RTOS_ERR    *p_err);

  //                                                               Enables/Disables the Card Interrupt.
  void (*CardIntEnDis)         (SD_CARD_DRV *p_drv,
                                CPU_BOOLEAN enable);

  //                                                               Resets part of or entire host controller.
  void (*CtrlrReset)           (SD_CARD_DRV             *p_drv,
                                SD_CARD_CTRLR_RESET_LVL lvl,
                                RTOS_ERR                *p_err);

  //                                                               Gets controller capabilities.
  SD_HOST_CAPABILITIES (*CapabilitiesGet)      (SD_CARD_DRV *p_drv);

  //                                                               Retrieves required buffer alignment.
  CPU_SIZE_T           (*ReqAlignGet)          (SD_CARD_DRV *p_drv);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const SD_CARD_BUS_HANDLE SD_CardBusHandleNull;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void SD_CardEventCardDetect(SD_CARD_BUS_HANDLE bus_handle);

void SD_CardEventCardRemove(SD_CARD_BUS_HANDLE bus_handle);

void SD_CardEventCmdCmpl(SD_CARD_BUS_HANDLE bus_handle,
                         RTOS_ERR           err);

void SD_CardEventDataXferCmpl(SD_CARD_BUS_HANDLE bus_handle,
                              RTOS_ERR           err);

void SD_CardEventCardInt(SD_CARD_BUS_HANDLE bus_handle);
#endif
