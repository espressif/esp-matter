/***************************************************************************//**
 * @file
 * @brief USB Host - USB-To-Serial Adapter Class
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
 * @defgroup USBH_USB2SER USB Host USB2SER API
 * @ingroup USBH
 * @brief   USB Host USB2SER API
 *
 * @addtogroup USBH_USB2SER
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_USB2SER_H_
#define  _USBH_USB2SER_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_class.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_USB2SER_STD_REQ_TIMEOUT_DFLT                      5000u

/********************************************************************************************************
 *                                   SOFTWARE FLOW CONTROL PROTOCOL
 *******************************************************************************************************/

#define  USBH_USB2SER_SW_FLOW_CTRL_XON_CHAR_DFLT                0x11u
#define  USBH_USB2SER_SW_FLOW_CTRL_XOFF_CHAR_DFLT               0x13u

/********************************************************************************************************
 *                                               SERIAL STATUS
 *******************************************************************************************************/

//                                                                 ------------------- MODEM STATUS -------------------
#define  USBH_USB2SER_MODEM_STATUS_CTS                      DEF_BIT_00
#define  USBH_USB2SER_MODEM_STATUS_DSR                      DEF_BIT_01
#define  USBH_USB2SER_MODEM_STATUS_RING                     DEF_BIT_02
#define  USBH_USB2SER_MODEM_STATUS_CARRIER                  DEF_BIT_03

//                                                                 ------------------- LINE STATUS --------------------
#define  USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR           DEF_BIT_00
#define  USBH_USB2SER_LINE_STATUS_PARITY_ERR                DEF_BIT_01
#define  USBH_USB2SER_LINE_STATUS_FRAMING_ERR               DEF_BIT_02
#define  USBH_USB2SER_LINE_STATUS_BRK_INT                   DEF_BIT_03

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DATA CHARACTERISTICS
 *******************************************************************************************************/

//                                                                 ---------------------- PARITY ----------------------
typedef enum usbh_usb2ser_parity {
  USBH_USB2SER_PARITY_NONE,
  USBH_USB2SER_PARITY_ODD,
  USBH_USB2SER_PARITY_EVEN,
  USBH_USB2SER_PARITY_MARK,
  USBH_USB2SER_PARITY_SPACE
} USBH_USB2SER_PARITY;

//                                                                 -------------------- STOP BITS ---------------------
typedef enum usbh_usb2ser_stop_bits {
  USBH_USB2SER_STOP_BITS_1,
  USBH_USB2SER_STOP_BITS_1_5,
  USBH_USB2SER_STOP_BITS_2
} USBH_USB2SER_STOP_BITS;

/********************************************************************************************************
 *                                           RESET SELECTION
 *******************************************************************************************************/

typedef enum usbh_usb2ser_reset_sel {
  USBH_USB2SER_RESET_SEL_TX,
  USBH_USB2SER_RESET_SEL_RX,
  USBH_USB2SER_RESET_SEL_ALL
} USBH_USB2SER_RESET_SEL;

/********************************************************************************************************
 *                                   HARDWARE FLOW CONTROL PROTOCOL
 *******************************************************************************************************/

typedef enum usbh_usb2ser_hw_flow_ctrl_protocol {
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS,
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR,
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE
} USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef const struct usbh_usb2ser_adapter_drv USBH_USB2SER_ADAPTER_DRV;
typedef const struct usbh_usb2ser_dev_adapter_drv USBH_USB2SER_DEV_ADAPTER_DRV;

/********************************************************************************************************
 *                                       USB-TO-SERIAL FUNCTION HANDLE
 *******************************************************************************************************/

typedef USBH_CLASS_FNCT_HANDLE USBH_USB2SER_FNCT_HANDLE;

/********************************************************************************************************
 *                                               SERIAL STATUS
 *
 * Note(s) : (1) The serial status is defined as follow:
 *
 *       OFFSET                 MODEM STATUS                                        MASK
 *       --------------------------------------------------------------------------------------------------
 *       Bit 0       Clear to Send (CTS)                         USBH_USB2SER_MODEM_STATUS_CTS
 *       Bit 1       Data Set Ready (DSR)                        USBH_USB2SER_MODEM_STATUS_DSR
 *       Bit 2       Ring Indicator (RI)                         USBH_USB2SER_MODEM_STATUS_RING
 *       Bit 3       Receive Line Signal Detect (RLSD)           USBH_USB2SER_MODEM_STATUS_CARRIER
 *       --------------------------------------------------------------------------------------------------
 *
 *       OFFSET                 LINE STATUS                                         MASK
 *       --------------------------------------------------------------------------------------------------
 *       Bit 0       Overrun Error (OE)                          USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR
 *       Bit 1       Parity Error (PE)                           USBH_USB2SER_LINE_STATUS_PARITY_ERR
 *       Bit 2       Framing Error (FE)                          USBH_USB2SER_LINE_STATUS_FRAMING_ERR
 *       Bit 3       Break Interrupt (BI)                        USBH_USB2SER_LINE_STATUS_BRK_INT
 *       --------------------------------------------------------------------------------------------------
 *******************************************************************************************************/

typedef struct usbh_usb2ser_serial_status {
  CPU_INT08U Modem;
  CPU_INT08U Line;
} USBH_USB2SER_SERIAL_STATUS;

/********************************************************************************************************
 *                                       APP NOTIFICATION FUNCTIONS
 *******************************************************************************************************/

typedef const struct usbh_usb2ser_app_fncts {
  //                                                               A USB-to-serial function has been connected.
  void *(*Conn)(USBH_DEV_HANDLE dev_handle,
                USBH_FNCT_HANDLE fnct_handle,
                USBH_USB2SER_FNCT_HANDLE handle);

  //                                                               A USB-to-serial function has been disconn'd.
  void (*Disconn)(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                  void                     *p_arg);

  //                                                               USB-to-serial function has received data.
  void (*DataRxd)(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                  void                     *p_arg,
                  CPU_INT08U               *p_buf,
                  CPU_INT32U               buf_len);

  //                                                               USB-to-serial function's status has changed.
  void (*SerialStatusChng)(USBH_USB2SER_FNCT_HANDLE         usb2ser_fnct_handle,
                           void                             *p_arg,
                           const USBH_USB2SER_SERIAL_STATUS serial_status);
} USBH_USB2SER_APP_FNCTS;

/********************************************************************************************************
 *                                               APP IDs
 *******************************************************************************************************/

typedef struct usbh_usb2ser_app_id {
  CPU_INT16U VendorID;
  CPU_INT16U ProductID;
} USBH_USB2SER_APP_ID;

/********************************************************************************************************
 *                                           ADAPTER DRIVER ENTRY
 *******************************************************************************************************/

typedef struct usbh_usb2ser_adapter_drv_entry {
  USBH_USB2SER_ADAPTER_DRV *DrvPtr;

  USBH_USB2SER_APP_ID      *CustomIdTbl;                        // Table assumed to be persistent.
  CPU_INT08U               CustomIdTblLen;
} USBH_USB2SER_ADAPTER_DRV_ENTRY;

/********************************************************************************************************
 *                                   USB2SER ASYNCHRONOUS FUNCTION CALLBACK
 *
 * Note(s) : (1) Application TX function callback.
 *
 *           (2) The arguments are defined as follow:
 *
 *               usb2ser_fnct_handle  Handle to USB-to-serial function.
 *               p_arg                Pointer to argument.
 *               p_buf                Pointer to transmitted data buffer.
 *               buf_len              Buffer length in bytes.
 *               xfer_len             Number of bytes transmitted.
 *               err                  Variable that will receive the return error code.
 *******************************************************************************************************/

typedef void (*USBH_USB2SER_ASYNC_TX_FNCT) (USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                            void                     *p_arg,
                                            CPU_INT08U               *p_buf,
                                            CPU_INT32U               buf_len,
                                            CPU_INT32U               xfer_len,
                                            RTOS_ERR                 err);

/********************************************************************************************************
 *                               USB-TO-SERIAL CONFIGURATION STRUCTURES
 *
 * Note(s) : (1) Can be DEF_NULL if USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED.
 *
 *           (2) Can be DEF_NULL if USBH_CFG_INIT_ALLOC_EN is set to DEF_DISABLED.
 *******************************************************************************************************/

typedef struct usbh_usb2ser_cfg_optimize_spd {                  // ------- USB-TO-SERIAL CLASS OPTIMIZE SPD CFG -------
  CPU_INT08U FnctQty;                                           // Qty of usb-to-serial functions.
} USBH_USB2SER_CFG_OPTIMIZE_SPD;

typedef struct usbh_usb2ser_cfg_init_alloc {                    // -------- USB-TO-SERIAL CLASS INIT ALLOC CFG --------
  CPU_INT08U FnctQty;                                           // Qty of usb-to-serial functions.
  CPU_INT16U TxAsyncXferQty;                                    // Qty of Tx async data.
} USBH_USB2SER_CFG_INIT_ALLOC;

typedef struct usbh_usb2ser_init_cfg {                          // ----------- USB2SER INIT CONFIGURATIONS ------------
  CPU_SIZE_T                     BufAlignOctets;                // Indicates desired mem alignment for internal buf.
  CPU_BOOLEAN                    HS_En;                         // Enable support for HS usb-to-serial adapters.
  CPU_INT08U                     RxBufQty;                      // Quantity of rx buffers per function.

  USBH_USB2SER_ADAPTER_DRV_ENTRY *DevDrvEntryTbl;               // Table containing the adapter driver entries.

  MEM_SEG                        *MemSegPtr;                    // Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG                        *MemSegBufPtr;                 // Ptr to mem segment to use when allocating data buf.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_USB2SER_CFG_OPTIMIZE_SPD  OptimizeSpd;                   // Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_USB2SER_CFG_INIT_ALLOC InitAlloc;                        // Configurations for allocations at init mode.
#endif
} USBH_USB2SER_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 USBH USB2SER class dflt configurations.
extern const USBH_USB2SER_INIT_CFG USBH_USB2SER_InitCfgDflt;

//                                                                 USBH USB2SER dflt adapter driver table .
extern const USBH_USB2SER_ADAPTER_DRV_ENTRY USBH_USB2SER_DrvEntryTblDfltCfg[];

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBH_USB2SER_ConfigureHS_En(CPU_BOOLEAN hs_en);

void USBH_USB2SER_ConfigureRxBufQty(CPU_INT08U rx_buf_qty);

void USBH_USB2SER_ConfigureDrvEntryTbl(USBH_USB2SER_ADAPTER_DRV_ENTRY *p_tbl);

void USBH_USB2SER_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                                  MEM_SEG *p_mem_seg_buf);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_USB2SER_ConfigureOptimizeSpdCfg(const USBH_USB2SER_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_USB2SER_ConfigureInitAllocCfg(const USBH_USB2SER_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                           USB2SER FUNCTIONS
 *******************************************************************************************************/

void USBH_USB2SER_Init(USBH_USB2SER_APP_FNCTS *p_app_fncts,
                       RTOS_ERR               *p_err);

void USBH_USB2SER_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                                   RTOS_ERR   *p_err);

USBH_DEV_HANDLE USBH_USB2SER_DevHandleGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                          RTOS_ERR                 *p_err);

CPU_INT08U USBH_USB2SER_PortNbrGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                   RTOS_ERR                 *p_err);

void USBH_USB2SER_Reset(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                        USBH_USB2SER_RESET_SEL   sel,
                        RTOS_ERR                 *p_err);

void USBH_USB2SER_BaudRateSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                              CPU_INT32U               baudrate,
                              RTOS_ERR                 *p_err);

CPU_INT32U USBH_USB2SER_BaudRateGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                    RTOS_ERR                 *p_err);

void USBH_USB2SER_DataSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                          CPU_INT08U               data_size,
                          USBH_USB2SER_PARITY      parity,
                          USBH_USB2SER_STOP_BITS   stop_bits,
                          RTOS_ERR                 *p_err);

void USBH_USB2SER_DataGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                          CPU_INT08U               *p_data_size,
                          USBH_USB2SER_PARITY      *p_parity,
                          USBH_USB2SER_STOP_BITS   *p_stop_bits,
                          RTOS_ERR                 *p_err);

void USBH_USB2SER_BreakSignalSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                 CPU_BOOLEAN              set,
                                 RTOS_ERR                 *p_err);

void USBH_USB2SER_ModemDTR_Set(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                               CPU_BOOLEAN              set,
                               RTOS_ERR                 *p_err);

CPU_BOOLEAN USBH_USB2SER_ModemDTR_Get(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                      RTOS_ERR                 *p_err);

void USBH_USB2SER_ModemRTS_Set(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                               CPU_BOOLEAN              set,
                               RTOS_ERR                 *p_err);

CPU_BOOLEAN USBH_USB2SER_ModemRTS_Get(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                      RTOS_ERR                 *p_err);

void USBH_USB2SER_HW_FlowCtrlSet(USBH_USB2SER_FNCT_HANDLE           usb2ser_fnct_handle,
                                 USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                 RTOS_ERR                           *p_err);

USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_HW_FlowCtrlGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                                               RTOS_ERR                 *p_err);

void USBH_USB2SER_SW_FlowCtrlSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                 CPU_BOOLEAN              en,
                                 CPU_INT08U               xon_char,
                                 CPU_INT08U               xoff_char,
                                 RTOS_ERR                 *p_err);

CPU_BOOLEAN USBH_USB2SER_SW_FlowCtrlGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                        CPU_INT08U               *p_xon_char,
                                        CPU_INT08U               *p_xoff_char,
                                        RTOS_ERR                 *p_err);

USBH_USB2SER_SERIAL_STATUS USBH_USB2SER_StatusGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                                  RTOS_ERR                 *p_err);

void USBH_USB2SER_TxAsync(USBH_USB2SER_FNCT_HANDLE   usb2ser_fnct_handle,
                          CPU_INT08U                 *p_buf,
                          CPU_INT32U                 buf_len,
                          USBH_USB2SER_ASYNC_TX_FNCT tx_cmpl_notify,
                          void                       *p_arg,
                          RTOS_ERR                   *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
