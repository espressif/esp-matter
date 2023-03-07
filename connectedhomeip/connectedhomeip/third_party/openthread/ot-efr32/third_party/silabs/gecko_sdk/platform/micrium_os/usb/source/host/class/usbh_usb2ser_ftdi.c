/***************************************************************************//**
 * @file
 * @brief USB Host - Future Technology Devices International (FTDI)
 *        Serial Adapter Driver
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
 * @note             (1) The reference document "API for FTxxxx Devices Application Note AN_115" can be
 *                       requested from Future Technology Devices International Ltd. after signing a
 *                       Non-Disclosure Agreement (NDA).
 * @note             (2) This adapter driver supports the following FTDI devices:
 *                       - FT232B
 *                       - FT232R
 *                       - FT232H
 *                       - FT2232F
 *                       - FT2232H
 *                       - FT4232H
 *                       - FT8U232AM
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_FTDI_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))

#error USB HOST USB2SER FTDI adapter driver requires USB Host USB2SER class. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_USB2SER_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_USB2SER_FTDI_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/lib_str.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>
#include  <usb/include/host/usbh_usb2ser.h>
#include  <usb/include/host/usbh_usb2ser_ftdi.h>

#include  <usb/source/host/class/usbh_usb2ser_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, USB2SER, FTDI)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_USB2SER_FTDI_FS_MAX_XFER_LEN                                 64u
#define  USBH_USB2SER_FTDI_HS_MAX_XFER_LEN                                512u

/********************************************************************************************************
 *                                           FTDI VENDOR ID
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 2.
 *
 *           (2) The 'idVendor' field of the device descriptor may contain this value.
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_ID_VENDOR                                   0x0403u

/********************************************************************************************************
 *                                           FTDI PRODUCT IDs
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 2.
 *
 *           (2) The 'idProduct' field of the device descriptor may contain one of these values.
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT8U232AM                        0x6001u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT232B                           0x6001u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT2232D                          0x6010u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT232R                           0x6001u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT2232H                          0x6010u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT4232H                          0x6011u
#define  USBH_USB2SER_FTDI_ID_PRODUCT_FT232H                           0x6014u

#define  USBH_USB2SER_FTDI_ID_TBL_LEN                                       7u

/********************************************************************************************************
 *                                           FTDI bcd Device Values
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 2.
 *
 *           (2) The 'bcdDevice' field of the device descriptor may contain one of these values. It is the
 *               only field that is not alterable via an EEPROM.
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT8U232AM                        0x0200u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT232B_WITHOUT_EEPROM            0x0200u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT232B_WITH_EEPROM               0x0400u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT2232D                          0x0500u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT232R                           0x0600u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT2232H                          0x0700u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT4232H                          0x0800u
#define  USBH_USB2SER_FTDI_BCD_DEVICE_FT232H                           0x0900u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 3.
 *
 *           (2) The 'bRequest' field of a class-specific setup request may contain one of these values.
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_REQ_RESET                                     0x00u
#define  USBH_USB2SER_FTDI_REQ_SET_MODEM_CTRL                            0x01u
#define  USBH_USB2SER_FTDI_REQ_SET_FLOW_CTRL                             0x02u
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE                             0x03u
#define  USBH_USB2SER_FTDI_REQ_SET_DATA                                  0x04u
#define  USBH_USB2SER_FTDI_REQ_GET_MODEM_STATUS                          0x05u

#define  USBH_USB2SER_FTDI_FS_MAX_BAUD_RATE                           3000000u
#define  USBH_USB2SER_FTDI_HS_MAX_BAUD_RATE                          12000000u

#define  USBH_USB2SER_FTDI_BAUD_RATE_2_MHz                            2000000u

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_INT_DIV_MIN                    2u
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_INT_DIV_MAX                16383u

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_HIGH_FS_DEV               DEF_BIT_00

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_HIGH_HS_DEV               DEF_BIT_08
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_CLK_DIV                   DEF_BIT_09

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_INT_DIV_MSK            DEF_BIT_FIELD(14u, 0u)
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_FRACT_DIV_MSK         (DEF_BIT_15 | DEF_BIT_14)

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_1_OR_7_8TH        (DEF_BIT_15 | DEF_BIT_14)
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_2_OR_6_8TH         DEF_BIT_15
#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_4_OR_5_8TH         DEF_BIT_14

#define  USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_TOLERANCE_PERCENT              3u

/********************************************************************************************************
 *                                           FTDI_RESET CONTROL
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 3.1.
 *
 *           (2) The control value is defined as follow:
 *
 *               USBH_USB2SER_FTDI_RESET_CTRL_SIO                Reset SIO (Resets both RX and TX buffer)
 *               USBH_USB2SER_FTDI_RESET_CTRL_RX                 Purge RX buffer
 *               USBH_USB2SER_FTDI_RESET_CTRL_TX                 Purge TX buffer
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_RESET_CTRL_SIO                                   DEF_BIT_NONE
#define  USBH_USB2SER_FTDI_RESET_CTRL_RX                                    DEF_BIT_00
#define  USBH_USB2SER_FTDI_RESET_CTRL_TX                                    DEF_BIT_01

/********************************************************************************************************
 *                                       FTDI_SET_DATA CHARACTERISTICS
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 3.5.
 *
 *           (2) The control value is defined as follow:
 *
 *               USBH_USB2SER_FTDI_DATA_PARITY_NONE              Do not use the parity bit
 *               USBH_USB2SER_FTDI_DATA_PARITY_ODD               Use odd        parity bit
 *               USBH_USB2SER_FTDI_DATA_PARITY_EVEN              Use even       parity bit
 *               USBH_USB2SER_FTDI_DATA_PARITY_MARK              Use mark       parity bit
 *               USBH_USB2SER_FTDI_DATA_PARITY_SPACE             Use space      parity bit
 *
 *               USBH_USB2SER_FTDI_DATA_STOP_BITS_1              Use 1 stop bit
 *               USBH_USB2SER_FTDI_DATA_STOP_BITS_2              Use 2 stop bit
 *
 *               USBH_USB2SER_FTDI_DATA_BREAK_ENABLED            Send break
 *               USBH_USB2SER_FTDI_DATA_BREAK_DISABLED           Stop break
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_DATA_DATA_BITS_MSK                               DEF_BIT_FIELD(8u, 0u)

#define  USBH_USB2SER_FTDI_DATA_PARITY_MSK                                  DEF_BIT_FIELD(3u, 8u)
#define  USBH_USB2SER_FTDI_DATA_PARITY_NONE                                 0u
#define  USBH_USB2SER_FTDI_DATA_PARITY_ODD                                  1u
#define  USBH_USB2SER_FTDI_DATA_PARITY_EVEN                                 2u
#define  USBH_USB2SER_FTDI_DATA_PARITY_MARK                                 3u
#define  USBH_USB2SER_FTDI_DATA_PARITY_SPACE                                4u

#define  USBH_USB2SER_FTDI_DATA_STOP_BITS_MSK                               DEF_BIT_FIELD(3u, 11u)
#define  USBH_USB2SER_FTDI_DATA_STOP_BITS_1                                 0u
#define  USBH_USB2SER_FTDI_DATA_STOP_BITS_2                                 1u

#define  USBH_USB2SER_FTDI_DATA_BREAK_MSK                                   DEF_BIT_14
#define  USBH_USB2SER_FTDI_DATA_BREAK_DISABLED                              0u
#define  USBH_USB2SER_FTDI_DATA_BREAK_ENABLED                               1u

/********************************************************************************************************
 *                                           FTDI_SET_FLOW PROTOCOL
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 3.3.
 *
 *           (2) The control value is defined as follow:
 *
 *               USBH_USB2SER_FTDI_PROTOCOL_RTS_CTS              Enable output handshaking using RTS/CTS
 *               USBH_USB2SER_FTDI_PROTOCOL_DTR_DSR              Enable output handshaking using DTR/DSR
 *               USBH_USB2SER_FTDI_PROTOCOL_XON_XOFF             Enable Xon/Xoff handshaking
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_PROTOCOL_RTS_CTS                                 DEF_BIT_00
#define  USBH_USB2SER_FTDI_PROTOCOL_DTR_DSR                                 DEF_BIT_01
#define  USBH_USB2SER_FTDI_PROTOCOL_XON_XOFF                                DEF_BIT_02

/********************************************************************************************************
 *                                           FTDI_MODEM CONTROL
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 3.2.
 *
 *           (2) The control value is defined as follow:
 *
 *               USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET            Set     DTR state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_DTR_RESET          Reset   DTR state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED        Use     DTR state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_DTR_DISABLED       Disable DTR state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET            Set     RTS state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_RTS_RESET          Reset   RTS state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED        Use     RTS state
 *               USBH_USB2SER_FTDI_MODEM_CTRL_RTS_DISABLED       Disable RTS state
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET                               DEF_BIT_00
#define  USBH_USB2SER_FTDI_MODEM_CTRL_DTR_RESET                             DEF_BIT_NONE
#define  USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED                           DEF_BIT_08
#define  USBH_USB2SER_FTDI_MODEM_CTRL_DTR_DISABLED                          DEF_BIT_NONE
#define  USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET                               DEF_BIT_01
#define  USBH_USB2SER_FTDI_MODEM_CTRL_RTS_RESET                             DEF_BIT_NONE
#define  USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED                           DEF_BIT_09
#define  USBH_USB2SER_FTDI_MODEM_CTRL_RTS_DISABLED                          DEF_BIT_NONE

/********************************************************************************************************
 *                                           FTDI SERIAL STATUS
 *
 * Note(s) : (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 4.
 *
 *           (2) These defines may be used as a mask to determine whether a field of the modem status or
 *               the line status is enabled or not.
 *******************************************************************************************************/

#define  USBH_USB2SER_FTDI_MODEM_STATUS_FS                                  DEF_BIT_00
#define  USBH_USB2SER_FTDI_MODEM_STATUS_HS                                  DEF_BIT_01
#define  USBH_USB2SER_FTDI_MODEM_STATUS_CTS                                 DEF_BIT_04
#define  USBH_USB2SER_FTDI_MODEM_STATUS_DSR                                 DEF_BIT_05
#define  USBH_USB2SER_FTDI_MODEM_STATUS_RING                                DEF_BIT_06
#define  USBH_USB2SER_FTDI_MODEM_STATUS_CARRIER                             DEF_BIT_07

#define  USBH_USB2SER_FTDI_LINE_STATUS_RX_OVERFLOW_ERR                      DEF_BIT_01
#define  USBH_USB2SER_FTDI_LINE_STATUS_PARITY_ERR                           DEF_BIT_02
#define  USBH_USB2SER_FTDI_LINE_STATUS_FRAMING_ERR                          DEF_BIT_03
#define  USBH_USB2SER_FTDI_LINE_STATUS_BREAK_INT                            DEF_BIT_04
#define  USBH_USB2SER_FTDI_LINE_STATUS_TX_REG_EMPTY                         DEF_BIT_05
#define  USBH_USB2SER_FTDI_LINE_STATUS_TX_EMPTY                             DEF_BIT_06

#define  USBH_USB2SER_FTDI_STATUS_LEN                                       2u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FTDI FUNCTION TYPES
 *******************************************************************************************************/

typedef enum usbh_ftdi_serial_fnct_type {
  USBH_USB2SER_FTDI_FNCT_TYPE_UNDETERMINED,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT8U232AM,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT232B,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT2232D,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT232R,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT2232H,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT4232H,
  USBH_USB2SER_FTDI_FNCT_TYPE_FT232H
} USBH_USB2SER_FTDI_FNCT_TYPE;

/********************************************************************************************************
 *                                           FTDI FUNCTION STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_ftdi_fnct {
  USBH_EP_HANDLE              RxEP_Handle;                      // Handle to rx EP.
  USBH_EP_HANDLE              TxEP_Handle;                      // Handle to tx EP.

  USBH_USB2SER_FTDI_FNCT_TYPE Type;                             // Type of FTDI chip.

  CPU_INT08U                  PortNbr;                          // Cur fnct port nbr.

  //                                                               FTDI adapter offer not getter req. Must store values.
  CPU_INT32U                  BaudRate;                         // Current baud rate.
  CPU_INT16U                  SetDataVal;                       // Current data params.
  CPU_INT16U                  FlowCtrlProtocol;                 // Current flow ctrl.
  CPU_INT16U                  XonXoffChar;                      // Current Xon/Xoff characters.
  CPU_INT16U                  ModemCtrlVal;                     // Current modem ctrl value.
} USBH_USB2SER_FTDI_FNCT;

/********************************************************************************************************
 *                                           FTDI CLASS DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_ftdi {
  USBH_USB2SER_APP_ID *AppID_Tbl;                               // Tbl of app-specific FTDI serial devices.
  CPU_INT08U          AppID_TblLen;                             // Len of tbl of app-specific FTDI serial devices.
} USBH_USB2SER_FTDI;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Table of dflt vendor/product ids.
static USBH_USB2SER_APP_ID USBH_FTDI_SerialDfltID_Tbl[] =
{
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT8U232AM },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT232B },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT2232D },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT232R },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT2232H },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT4232H },
  { USBH_USB2SER_FTDI_ID_VENDOR, USBH_USB2SER_FTDI_ID_PRODUCT_FT232H }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_USB2SER_FTDI *USBH_USB2SER_FTDI_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------- ADAPTER DRIVER FUNCTIONS -------------
static void USBH_USB2SER_FTDI_Init(MEM_SEG             *p_mem_seg,
                                   USBH_USB2SER_APP_ID *id_tbl,
                                   CPU_INT08U          id_tbl_len,
                                   RTOS_ERR            *p_err);

static void USBH_USB2SER_FTDI_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data);

static CPU_INT08U USBH_USB2SER_FTDI_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                    USBH_USB2SER_RESET_SEL             sel,
                                    RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT32U                         baudrate,
                                          RTOS_ERR                           *p_err);

static CPU_INT32U USBH_USB2SER_FTDI_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         data_size,
                                      USBH_USB2SER_PARITY                parity,
                                      USBH_USB2SER_STOP_BITS             stop_bits,
                                      RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_data_size,
                                      USBH_USB2SER_PARITY                *p_parity,
                                      USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                      RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        set,
                                           RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        dtr_en,
                                           CPU_BOOLEAN                        dtr_set,
                                           CPU_BOOLEAN                        rts_en,
                                           CPU_BOOLEAN                        rts_set,
                                           RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        *p_dtr_en,
                                           CPU_BOOLEAN                        *p_dtr_set,
                                           CPU_BOOLEAN                        *p_rts_en,
                                           CPU_BOOLEAN                        *p_rts_set,
                                           RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                             RTOS_ERR                           *p_err);

static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_FTDI_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                           RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        en,
                                             CPU_INT08U                         xon_char,
                                             CPU_INT08U                         xoff_char,
                                             RTOS_ERR                           *p_err);

static CPU_BOOLEAN USBH_USB2SER_FTDI_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                    CPU_INT08U                         *p_xon_char,
                                                    CPU_INT08U                         *p_xoff_char,
                                                    RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_buf,
                                      CPU_INT32U                         buf_len,
                                      RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_buf,
                                      CPU_INT32U                         buf_len,
                                      void                               *p_arg,
                                      RTOS_ERR                           *p_err);

//                                                                 -------------- CLASS DRIVER FUNCTIONS --------------
static CPU_BOOLEAN USBH_USB2SER_FTDI_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                               USBH_FNCT_HANDLE fnct_handle,
                                               CPU_INT08U       class_code,
                                               void             **pp_class_fnct,
                                               RTOS_ERR         *p_err);

static void USBH_USB2SER_FTDI_EP_Open(void           *p_class_fnct,
                                      USBH_EP_HANDLE ep_handle,
                                      CPU_INT08U     if_ix,
                                      CPU_INT08U     ep_type,
                                      CPU_BOOLEAN    ep_dir_in);

static void USBH_USB2SER_FTDI_EP_Close(void           *p_class_fnct,
                                       USBH_EP_HANDLE ep_handle,
                                       CPU_INT08U     if_ix);

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_USB2SER_FTDI_TraceDump(void                *p_class_fnct,
                                        CPU_INT32U          opt,
                                        USBH_CMD_TRACE_FNCT trace_fnct);
#endif

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static void USBH_USB2SER_FTDI_StdReq(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                     CPU_INT08U                         req,
                                     CPU_INT16U                         val,
                                     CPU_INT08U                         hi_ix,
                                     RTOS_ERR                           *p_err);

static void USBH_USB2SER_FTDI_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
                                         USBH_EP_HANDLE  ep_handle,
                                         CPU_INT08U      *p_buf,
                                         CPU_INT32U      buf_len,
                                         CPU_INT32U      xfer_len,
                                         void            *p_arg,
                                         RTOS_ERR        err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_DEV_VENDOR_ID  Must be enabled"
#endif

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_DEV_PRODUCT_ID  Must be enabled"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FTDI CLASS DRIVER
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ FTDI DEVICE ADAPTER DRIVER ------------
static USBH_USB2SER_DEV_ADAPTER_DRV USBH_USB2SER_FTDI_DevAdapterDrv = {
  USBH_USB2SER_FTDI_Init,
  USBH_USB2SER_FTDI_Conn,
  USBH_USB2SER_FTDI_PortNbrGet,
  USBH_USB2SER_FTDI_Reset,
  USBH_USB2SER_FTDI_BaudRateSet,
  USBH_USB2SER_FTDI_BaudRateGet,
  USBH_USB2SER_FTDI_DataSet,
  USBH_USB2SER_FTDI_DataGet,
  USBH_USB2SER_FTDI_BrkSignalSet,
  USBH_USB2SER_FTDI_ModemCtrlSet,
  USBH_USB2SER_FTDI_ModemCtrlGet,
  USBH_USB2SER_FTDI_HW_FlowCtrlSet,
  USBH_USB2SER_FTDI_HW_FlowCtrlGet,
  USBH_USB2SER_FTDI_SW_FlowCtrlSet,
  USBH_USB2SER_FTDI_SW_FlowCtrlGet,
  USBH_USB2SER_FTDI_RxAsync,
  USBH_USB2SER_FTDI_TxAsync,

  0u,                                                           // FTDI does not need a ctrl buffer.
  0u,                                                           // Serial status embedded in Rx data.
  USBH_USB2SER_FTDI_FS_MAX_XFER_LEN,
  USBH_USB2SER_FTDI_HS_MAX_XFER_LEN,
  sizeof(USBH_USB2SER_FTDI_FNCT)
};

//                                                                 ------------ FTDI ADAPTER CLASS DRIVER -------------
static USBH_CLASS_DRV USBH_USB2SER_FTDI_ClassDrv = {
  DEF_NULL,
  USBH_USB2SER_FTDI_ProbeFnct,
  DEF_NULL,
  USBH_USB2SER_FTDI_EP_Open,
  USBH_USB2SER_FTDI_EP_Close,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_USB2SER_FTDI_TraceDump,
#endif
  (CPU_CHAR *)"USB2SER - FTDI",
};

//                                                                 --------------- FTDI ADAPTER DRIVER ----------------
const USBH_USB2SER_ADAPTER_DRV USBH_USB2SER_FTDI_Drv = {
  &USBH_USB2SER_FTDI_ClassDrv,
  &USBH_USB2SER_FTDI_DevAdapterDrv
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_USB2SER_FTDI_Init()
 *
 * @brief    Initializes the FTDI device adapter driver.
 *
 * @param    p_mem_seg   Pointer to memory segment from which FTDI data will be allocated.
 *                       Data will be allocated from global heap if null.
 *
 * @param    id_tbl      Table containing list of vendor/product IDs that should be considered
 *                       FTDI serial devices. Can be DEF_NULL. See Note #1.
 *
 * @param    id_tlb_len  Length of ID table.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this
 *                       function.
 *
 * @note     (1) It is possible to modify the default Vendor and/or Product IDs of the FTDI chips. The
 *               purpose of the ID table that can be passed at initialization is to allow a customer
 *               to add any Vendor/Product ID combination as FTDI serial devices.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_Init(MEM_SEG             *p_mem_seg,
                                   USBH_USB2SER_APP_ID *id_tbl,
                                   CPU_INT08U          id_tbl_len,
                                   RTOS_ERR            *p_err)
{
  if (USBH_USB2SER_FTDI_Ptr != DEF_NULL) {                      // Chk if FTDI adapter driver already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  USBH_USB2SER_FTDI_Ptr = (USBH_USB2SER_FTDI *)Mem_SegAlloc("USBH - FTDI Data",
                                                            p_mem_seg,
                                                            sizeof(USBH_USB2SER_FTDI),
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_FTDI_Ptr->AppID_Tbl = id_tbl;
  USBH_USB2SER_FTDI_Ptr->AppID_TblLen = id_tbl_len;
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_FTDI_Conn()
 *
 * @brief    Initiates communication with FTDI adapter.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *
 * @note     (1) FTDI devices do not offer getter requests. Hence, the adapter must be initialized
 *               with known parameters at connection.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data)
{
  RTOS_ERR err_usbh;

  USBH_USB2SER_FTDI_BaudRateSet(p_adapter_drv_data,             // Set default baud rate.
                                9600u,
                                &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt baudrate -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_FTDI_DataSet(p_adapter_drv_data,                 // Set default data parameters.
                            8u,
                            USBH_USB2SER_PARITY_NONE,
                            USBH_USB2SER_STOP_BITS_1,
                            &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt data params -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_FTDI_HW_FlowCtrlSet(p_adapter_drv_data,          // Disable hardware flow control.
                                   USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE,
                                   &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt hw flow ctrl protocol -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_FTDI_SW_FlowCtrlSet(p_adapter_drv_data,          // Disable software flow control.
                                   DEF_DISABLED,
                                   USBH_USB2SER_SW_FLOW_CTRL_XON_CHAR_DFLT,
                                   USBH_USB2SER_SW_FLOW_CTRL_XOFF_CHAR_DFLT,
                                   &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt sw flow ctrl -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_FTDI_ModemCtrlSet(p_adapter_drv_data,            // Ensure DTR and RTS pins are not set.
                                 DEF_DISABLED,
                                 DEF_CLR,
                                 DEF_DISABLED,
                                 DEF_CLR,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt modem ctrl value -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_PortNbrGet()
 *
 * @brief    Retrieves adapter's port number.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) Port number is determined using the interface number (interface number + 1).
 *******************************************************************************************************/
static CPU_INT08U USBH_USB2SER_FTDI_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               RTOS_ERR                           *p_err)
{
  CPU_INT08U             port_nbr;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  port_nbr = p_ftdi_fnct->PortNbr;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (port_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_FTDI_Reset()
 *
 * @brief    Purges adapter's internal buffers.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    sel                 Purge selection:
 *                                   - USBH_USB2SER_RESET_SEL_TX,    Purge transmit  adapter's buffer.
 *                                   - USBH_USB2SER_RESET_SEL_RX,    Purge reception adapter's buffer.
 *                                   - USBH_USB2SER_RESET_SEL_ALL    Purge all       adapter's buffers.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'Reset' request, see 'API for FTxxxx Devices Application
 *               Note AN_115, April 04 2011, Version 1.1, section 3.1'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                    USBH_USB2SER_RESET_SEL             sel,
                                    RTOS_ERR                           *p_err)
{
  CPU_INT16U reset_ctrl;

  switch (sel) {
    case USBH_USB2SER_RESET_SEL_RX:
      reset_ctrl = USBH_USB2SER_FTDI_RESET_CTRL_RX;
      break;

    case USBH_USB2SER_RESET_SEL_TX:
      reset_ctrl = USBH_USB2SER_FTDI_RESET_CTRL_TX;
      break;

    case USBH_USB2SER_RESET_SEL_ALL:
    default:
      reset_ctrl = USBH_USB2SER_FTDI_RESET_CTRL_SIO;
      break;
  }

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send Reset req.
                           USBH_USB2SER_FTDI_REQ_RESET,
                           reset_ctrl,
                           0u,
                           p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_BaudRateSet()
 *
 * @brief    Sets baud rate of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    baudrate            Baud rate to set (in baud/sec). Can be any value supported by FTDI
 *                               chip.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'SetBaudRate' request, see 'API for FTxxxx Devices
 *               Application Note AN_115, April 04 2011, Version 1.1, section 3.4' and 'Aliasing VCP
 *               Baud Rates Application Note AN_120, June 11 2009, Version 1.0, sections 3 and 4.4'.
 *
 * @note     (2) The table below details the values of the various FTDI devices and how to set their
 *               baud rates:
 *           |==================================================================================================|
 *           |   |    bcdDevice    |Default VID|Default PID|         lIndex         |          hIndex           |
 *           |==================================================================================================|
 *           |AM | 0x0200          |           |           | 0x00                   | 0x00                      |
 *           |---+-----------------|           |           |------------------------+---------------------------|
 *           |   | 0x0200 or 0x0400|           |  0x6001   | Bit 0 - Baud High      | 0x00                      |
 *           |---+-----------------|           |           |------------------------+---------------------------|
 *           |   | 0x0600          |           |           | Bit 0 - Baud High      | 0x00                      |
 *           |   |                 |           |           |                        |                           |
 *           |---+-----------------|           |-----------+------------------------+---------------------------|
 *           |   | 0x0500          |  0x0403   |           | 0x00/0x01 -> PortNbr A | Bit 0 - Baud High         |
 *           |   |                 |           |  0x6010   | 0x02      -> PortNbr B |                           |
 *           |---+-----------------|           |           |------------------------+---------------------------|
 *           |   | 0x0700          |           |           | 0x00/0x01 -> PortNbr A | Bit 0 - Baud High         |
 *           |---+-----------------|           |-----------| 0x02      -> PortNbr B | Bit 1 - Baud Clock Divide |
 *           |   | 0x0800          |           |  0x6011   | 0x03      -> PortNbr C |                           |
 *           |---+-----------------|           |-----------| 0x04      -> PortNbr D |                           |
 *           |   | 0x0900          |           |  0x6014   |                        |                           |
 *           |==================================================================================================|
 *
 * @note     (3) To ensure compatibility with older chips, the Baud High bit will not be used.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT32U                         baudrate,
                                          RTOS_ERR                           *p_err)
{
  CPU_INT32U             base_baud_rate;
  CPU_INT16U             req_ix = 0u;
  CPU_INT32U             req_val = 0u;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Baud rate must be lower than max val allowed for spd.
  RTOS_ASSERT_DBG_ERR_SET((baudrate <= USBH_USB2SER_FTDI_HS_MAX_BAUD_RATE), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET((baudrate <= USBH_USB2SER_FTDI_HS_MAX_BAUD_RATE), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (baudrate > USBH_USB2SER_FTDI_FS_MAX_BAUD_RATE) {
    RTOS_ASSERT_DBG_ERR_SET(((p_ftdi_fnct->Type != USBH_USB2SER_FTDI_FNCT_TYPE_FT8U232AM)
                             && (p_ftdi_fnct->Type != USBH_USB2SER_FTDI_FNCT_TYPE_FT232B)
                             && (p_ftdi_fnct->Type != USBH_USB2SER_FTDI_FNCT_TYPE_FT2232D)
                             && (p_ftdi_fnct->Type != USBH_USB2SER_FTDI_FNCT_TYPE_FT232R)), *p_err, RTOS_ERR_INVALID_ARG,; );
  }

  req_ix = p_ftdi_fnct->PortNbr;                                // Set req ix depending on port number. See Note #2.

  if (baudrate == USBH_USB2SER_FTDI_FS_MAX_BAUD_RATE) {         // These 2 cases are handled in a particular way.
    req_val = 0u;
  } else if (baudrate == USBH_USB2SER_FTDI_BAUD_RATE_2_MHz) {
    req_val = 1u;
  } else {                                                      // Generic cases using divider.
    CPU_INT32U fraction_divider;

    if (((p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT2232H)
         || (p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT4232H)
         || (p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT232H))
        && (baudrate > 732u)) {
      //                                                           Use Baud Clock Divide bit, if possible and needed.
      DEF_BIT_SET(req_ix, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_CLK_DIV);
      base_baud_rate = USBH_USB2SER_FTDI_HS_MAX_BAUD_RATE;

      req_val = base_baud_rate / baudrate;                      // Calculate integer portion of divider.
      if ((req_val > USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_INT_DIV_MAX)
          || (req_val == 0u)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return;
      }
    } else {
      base_baud_rate = USBH_USB2SER_FTDI_FS_MAX_BAUD_RATE;

      req_val = base_baud_rate / baudrate;                      // Calculate integer portion of divider.
      if ((req_val > USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_INT_DIV_MAX)
          || (req_val < USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_INT_DIV_MIN)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return;
      }
    }

    //                                                             fraction_divider is expresssed in 16th, to be ...
    //                                                             able to have the precision req'd to make a decision.
    fraction_divider = ((base_baud_rate % baudrate) << 4u);

    if ((p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT8U232AM)
        || (p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_UNDETERMINED)) {
      //                                                           The FT8U232AM is the only chip supporting only a ...
      //                                                           subset of divider. In doubt, use only subset.
      fraction_divider = (fraction_divider / baudrate);

      if (fraction_divider >= 12u) {                            // If fract div must be >=  3/4, round up   to   1.
        fraction_divider = 8u;
      } else if (fraction_divider >= 6u) {                      // If fract div must be >=  3/8, round up   to 1/2.
        fraction_divider = 4u;
      } else if (fraction_divider >= 3u) {                      // If fract div must be >= 3/16, round up   to 1/4.
        fraction_divider = 2u;
      } else if (fraction_divider >= 1u) {                      // If fract div must be >= 1/16, round up   to 1/8.
        fraction_divider = 1u;
      } else {                                                  // If fract div must be <  1/16, round down to   0.
        fraction_divider = 0u;
      }
      //                                                           fraction_divider is now expressed in 8th.
    } else {
      fraction_divider = (fraction_divider + (baudrate - 1u));
      fraction_divider = ((fraction_divider / baudrate) >> 1u);
      //                                                           fraction_divider is now expressed in 8th.
    }

    switch (fraction_divider) {                                 // fraction_divider must be expressed in 8th.
      case 0u:
      case 3u:
        break;

      case 1u:
      case 7u:
        DEF_BIT_SET(req_val, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_1_OR_7_8TH);
        break;

      case 2u:
      case 6u:
        DEF_BIT_SET(req_val, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_2_OR_6_8TH);
        break;

      case 4u:
      case 5u:
        DEF_BIT_SET(req_val, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_ADD_4_OR_5_8TH);
        break;

      case 8u:                                                  // If a full div (8/8th) must be used, inc int div.
        req_val += 1u;
        fraction_divider = 0u;
        break;

      default:
        CPU_SW_EXCEPTION(; );
        break;
    }

    if ((fraction_divider == 3u)                                // This case may not happen with the FT8U232AM.
        || (fraction_divider == 5u)
        || (fraction_divider == 6u)
        || (fraction_divider == 7u)) {
      if ((p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT232B)
          || (p_ftdi_fnct->Type == USBH_USB2SER_FTDI_FNCT_TYPE_FT232R)) {
        DEF_BIT_SET(req_ix, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_HIGH_FS_DEV);
      } else {
        DEF_BIT_SET(req_ix, USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_BAUD_HIGH_HS_DEV);
      }
    }

        #if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                // Confirm obtained baud rate is within defined limit.
    {
      CPU_INT32U obtained_baud_rate;
      CPU_INT32U lower_limit;
      CPU_INT32U higher_limit;

      obtained_baud_rate = (base_baud_rate * 8u) / (((req_val & USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_REQ_VAL_INT_DIV_MSK) * 8u) + fraction_divider);

      lower_limit = baudrate - ((baudrate * USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_TOLERANCE_PERCENT) / 100u);
      higher_limit = baudrate + ((baudrate * USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE_TOLERANCE_PERCENT) / 100u);

      RTOS_ASSERT_DBG_ERR_SET(((obtained_baud_rate <= higher_limit)
                               && (obtained_baud_rate >= lower_limit)), *p_err, RTOS_ERR_INVALID_ARG,; );
    }
        #endif
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_FTDI_REQ_SET_BAUD_RATE,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_RECIPIENT_DEV),
                         req_val,
                         req_ix,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->BaudRate = baudrate;
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_BaudRateGet()
 *
 * @brief    Retrieves baud rate of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Baud rate in baud/sec.
 *******************************************************************************************************/
static CPU_INT32U USBH_USB2SER_FTDI_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                RTOS_ERR                           *p_err)
{
  CPU_INT32U             baudrate;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  baudrate = p_ftdi_fnct->BaudRate;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (baudrate);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_DataSet()
 *
 * @brief    Sets data characteristics of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    data_size           Number of data bits (Can be 6, 7 or 8).
 *
 * @param    parity              Parity to use.
 *                               USBH_USB2SER_PARITY_NONE,      No          parity bit.
 *                               USBH_USB2SER_PARITY_ODD,       Set on odd  parity bit.
 *                               USBH_USB2SER_PARITY_EVEN,      Set on even parity bit.
 *                               USBH_USB2SER_PARITY_MARK,      Set         parity bit.
 *                               USBH_USB2SER_PARITY_SPACE      Cleared     parity bit.
 *
 * @param    stop_bits           Number of stop bits.
 *                               USBH_USB2SER_STOP_BITS_1,       Use 1 stop bit.
 *                               USBH_USB2SER_STOP_BITS_2        Use 2 stop bit.
 *                               USBH_USB2SER_STOP_BITS_1_5      1.5 Stop bit (not supported on FTDI).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'SetData' request, see 'API for FTxxxx Devices Application
 *               Note AN_115, April 04 2011, Version 1.1, section 3.5'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         data_size,
                                      USBH_USB2SER_PARITY                parity,
                                      USBH_USB2SER_STOP_BITS             stop_bits,
                                      RTOS_ERR                           *p_err)
{
  CPU_INT08U             val_parity;
  CPU_INT08U             val_stop_bits;
  CPU_INT16U             set_data_val;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  switch (parity) {
    case USBH_USB2SER_PARITY_EVEN:
      val_parity = USBH_USB2SER_FTDI_DATA_PARITY_EVEN;
      break;

    case USBH_USB2SER_PARITY_ODD:
      val_parity = USBH_USB2SER_FTDI_DATA_PARITY_ODD;
      break;

    case USBH_USB2SER_PARITY_SPACE:
      val_parity = USBH_USB2SER_FTDI_DATA_PARITY_SPACE;
      break;

    case USBH_USB2SER_PARITY_MARK:
      val_parity = USBH_USB2SER_FTDI_DATA_PARITY_MARK;
      break;

    case USBH_USB2SER_PARITY_NONE:
    default:
      val_parity = USBH_USB2SER_FTDI_DATA_PARITY_NONE;
      break;
  }

  switch (stop_bits) {
    case USBH_USB2SER_STOP_BITS_1_5:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;

    case USBH_USB2SER_STOP_BITS_2:
      val_stop_bits = USBH_USB2SER_FTDI_DATA_STOP_BITS_2;
      break;

    case USBH_USB2SER_STOP_BITS_1:
    default:
      val_stop_bits = USBH_USB2SER_FTDI_DATA_STOP_BITS_1;
      break;
  }

  set_data_val = p_ftdi_fnct->SetDataVal;

  DEF_BIT_FIELD_WR(set_data_val,
                   val_parity,
                   (CPU_INT16U)USBH_USB2SER_FTDI_DATA_PARITY_MSK);

  DEF_BIT_FIELD_WR(set_data_val,
                   val_stop_bits,
                   (CPU_INT16U)USBH_USB2SER_FTDI_DATA_STOP_BITS_MSK);

  DEF_BIT_FIELD_WR(set_data_val,
                   data_size,
                   (CPU_INT16U)USBH_USB2SER_FTDI_DATA_DATA_BITS_MSK);

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send SetData req.
                           USBH_USB2SER_FTDI_REQ_SET_DATA,
                           set_data_val,
                           0u,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->SetDataVal = set_data_val;
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_DataGet()
 *
 * @brief    Gets data characteristics of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_data_size         Pointer to variable that will receive the number of data bits.
 *
 * @param    p_parity            Pointer to variable that will receive the parity currently used.
 *                                   - USBH_USB2SER_PARITY_NONE,    No          parity bit.
 *                                   - USBH_USB2SER_PARITY_ODD,     Set on odd  parity bit.
 *                                   - USBH_USB2SER_PARITY_EVEN,    Set on even parity bit.
 *                                   - USBH_USB2SER_PARITY_MARK,    Set         parity bit.
 *                                   - USBH_USB2SER_PARITY_SPACE    Cleared     parity bit.
 *
 * @param    p_stop_bits         Pointer to variable that will receive the number of stop bits.
 *                                      - USBH_USB2SER_STOP_BITS_1,     Use 1 stop bit.
 *                                      - USBH_USB2SER_STOP_BITS_2      Use 2 stop bit.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_data_size,
                                      USBH_USB2SER_PARITY                *p_parity,
                                      USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                      RTOS_ERR                           *p_err)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_data_size != DEF_NULL) {
    *p_data_size = DEF_BIT_FIELD_RD(p_ftdi_fnct->SetDataVal,
                                    USBH_USB2SER_FTDI_DATA_DATA_BITS_MSK);
  }

  if (p_parity != DEF_NULL) {
    CPU_INT08U val_parity = DEF_BIT_FIELD_RD(p_ftdi_fnct->SetDataVal,
                                             USBH_USB2SER_FTDI_DATA_PARITY_MSK);

    switch (val_parity) {
      case USBH_USB2SER_FTDI_DATA_PARITY_EVEN:
        *p_parity = USBH_USB2SER_PARITY_EVEN;
        break;

      case USBH_USB2SER_FTDI_DATA_PARITY_ODD:
        *p_parity = USBH_USB2SER_PARITY_ODD;
        break;

      case USBH_USB2SER_FTDI_DATA_PARITY_SPACE:
        *p_parity = USBH_USB2SER_PARITY_SPACE;
        break;

      case USBH_USB2SER_FTDI_DATA_PARITY_MARK:
        *p_parity = USBH_USB2SER_PARITY_MARK;
        break;

      case USBH_USB2SER_FTDI_DATA_PARITY_NONE:
      default:
        *p_parity = USBH_USB2SER_PARITY_NONE;
        break;
    }
  }

  if (p_stop_bits != DEF_NULL) {
    CPU_INT08U val_stop_bits = DEF_BIT_FIELD_RD(p_ftdi_fnct->SetDataVal,
                                                USBH_USB2SER_FTDI_DATA_STOP_BITS_MSK);

    switch (val_stop_bits) {
      case USBH_USB2SER_FTDI_DATA_STOP_BITS_2:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_2;
        break;

      case USBH_USB2SER_FTDI_DATA_STOP_BITS_1:
      default:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_1;
        break;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_BrkSignalSet()
 *
 * @brief    Sets/Clears break signal on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    set                 Flag that indicates if break signal should be set or clear.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        set,
                                           RTOS_ERR                           *p_err)
{
  CPU_INT08U             val_brk;
  CPU_INT16U             set_data_val;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  if (set == DEF_SET) {
    val_brk = USBH_USB2SER_FTDI_DATA_BREAK_ENABLED;
  } else {
    val_brk = USBH_USB2SER_FTDI_DATA_BREAK_DISABLED;
  }

  set_data_val = p_ftdi_fnct->SetDataVal;

  DEF_BIT_FIELD_WR(set_data_val,
                   val_brk,
                   (CPU_INT16U)USBH_USB2SER_FTDI_DATA_BREAK_MSK);

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send SetData req.
                           USBH_USB2SER_FTDI_REQ_SET_DATA,
                           set_data_val,
                           0u,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->SetDataVal = set_data_val;
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_ModemCtrlSet()
 *
 * @brief    Sets modem control of the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    dtr_en              Flag that indicates if Data Terminal Ready (DTR) pin manual control
 *                               is enabled.
 *
 * @param    dtr_set             Flag that indicates, when DTR pin control is enabled, if the pin state
 *                               is set or not.
 *
 * @param    rts_en              Flag that indicates if Ready To Send (RTS) pin manual  control is
 *                               enabled.
 *
 * @param    rts_set             Flag that indicates, when RTS pin control is enabled, if the pin state
 *                               is set or not.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'ModemCtrlVal' request, see 'API for FTxxxx Devices
 *               Application Note AN_115, April 04 2011, Version 1.1, section 3.2'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        dtr_en,
                                           CPU_BOOLEAN                        dtr_set,
                                           CPU_BOOLEAN                        rts_en,
                                           CPU_BOOLEAN                        rts_set,
                                           RTOS_ERR                           *p_err)
{
  CPU_INT16U             modem_ctrl_val;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  modem_ctrl_val = p_ftdi_fnct->ModemCtrlVal;

  if (dtr_en == DEF_ENABLED) {
    DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED);
  } else {
    DEF_BIT_CLR(modem_ctrl_val, (CPU_INT16U)USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED);
  }

  if (dtr_set == DEF_SET) {
    DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET);
  } else {
    DEF_BIT_CLR(modem_ctrl_val, (CPU_INT16U)USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET);
  }

  if (rts_en == DEF_ENABLED) {
    DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED);
  } else {
    DEF_BIT_CLR(modem_ctrl_val, (CPU_INT16U)USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED);
  }

  if (rts_set == DEF_SET) {
    DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET);
  } else {
    DEF_BIT_CLR(modem_ctrl_val, (CPU_INT16U)USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET);
  }

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send ModemCtrlVal req.
                           USBH_USB2SER_FTDI_REQ_SET_MODEM_CTRL,
                           modem_ctrl_val,
                           0u,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->ModemCtrlVal = modem_ctrl_val;
  }
}

/****************************************************************************************************//**
 *                                       USBH_FTDI_SerialModemCtrlSet()
 *
 * @brief    Gets modem control state of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_dtr_en            Pointer to variable that will receive current Data Terminal Ready
 *                               (DTR) pin manual control.
 *
 * @param    p_dtr_set           Pointer to variable that will receive, when DTR pin control is enabled,
 *                               the pin state.
 *
 * @param    p_rts_en            Pointer to variable that will receive current Data Terminal Ready
 *                               (RTS) pin manual control.
 *
 * @param    p_dtr_set           Pointer to variable that will receive, when DTR pin control is enabled,
 *                               the pin state.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'ModemCtrlVal' request, see 'API for FTxxxx Devices
 *               Application Note AN_115, April 04 2011, Version 1.1, section 3.2'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_BOOLEAN                        *p_dtr_en,
                                           CPU_BOOLEAN                        *p_dtr_set,
                                           CPU_BOOLEAN                        *p_rts_en,
                                           CPU_BOOLEAN                        *p_rts_set,
                                           RTOS_ERR                           *p_err)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_dtr_en != DEF_NULL) {
    *p_dtr_en = DEF_BIT_IS_SET(p_ftdi_fnct->ModemCtrlVal, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED);
  }

  if (p_dtr_set != DEF_NULL) {
    *p_dtr_set = DEF_BIT_IS_SET(p_ftdi_fnct->ModemCtrlVal, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET);
  }

  if (p_rts_en != DEF_NULL) {
    *p_rts_en = DEF_BIT_IS_SET(p_ftdi_fnct->ModemCtrlVal, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED);
  }

  if (p_rts_set != DEF_NULL) {
    *p_rts_set = DEF_BIT_IS_SET(p_ftdi_fnct->ModemCtrlVal, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_FTDI_HW_FlowCtrlSet()
 *
 * @brief    Sets hardware flow control handshaking on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    protocol            Protocol to use.
 *                                              - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS,
 *                                              - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_RTS,
 *                                              - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'SetFlowCtrl' request, see 'API for FTxxxx Devices
 *               Application Note AN_115, April 04 2011, Version 1.1, section 3.3'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                             RTOS_ERR                           *p_err)
{
  CPU_INT16U             protocol_val;
  CPU_INT16U             modem_ctrl_val;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  protocol_val = p_ftdi_fnct->FlowCtrlProtocol;
  modem_ctrl_val = p_ftdi_fnct->ModemCtrlVal;

  switch (protocol) {
    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR:
      DEF_BIT_SET(protocol_val, USBH_USB2SER_FTDI_PROTOCOL_DTR_DSR);

      DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_ENABLED);
      DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_DTR_SET);

      USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,              // Send ModemCtrlVal req.
                               USBH_USB2SER_FTDI_REQ_SET_MODEM_CTRL,
                               modem_ctrl_val,
                               0u,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        p_ftdi_fnct->ModemCtrlVal = modem_ctrl_val;
      }
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS:
      DEF_BIT_SET(protocol_val, USBH_USB2SER_FTDI_PROTOCOL_RTS_CTS);

      DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_ENABLED);
      DEF_BIT_SET(modem_ctrl_val, USBH_USB2SER_FTDI_MODEM_CTRL_RTS_SET);

      USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,              // Send ModemCtrlVal req.
                               USBH_USB2SER_FTDI_REQ_SET_MODEM_CTRL,
                               modem_ctrl_val,
                               0u,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        p_ftdi_fnct->ModemCtrlVal = modem_ctrl_val;
      }
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE:
    default:
      DEF_BIT_CLR(protocol_val, (CPU_INT16U)USBH_USB2SER_FTDI_PROTOCOL_DTR_DSR);
      DEF_BIT_CLR(protocol_val, (CPU_INT16U)USBH_USB2SER_FTDI_PROTOCOL_RTS_CTS);
      break;
  }

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send SetFlowCtrl req.
                           USBH_USB2SER_FTDI_REQ_SET_FLOW_CTRL,
                           p_ftdi_fnct->XonXoffChar,
                           protocol_val,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->FlowCtrlProtocol = protocol_val;
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_FTDI_HW_FlowCtrlGet()
 *
 * @brief    Gets hardware flow control handshaking on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Current hardware flow control protocol.
 *******************************************************************************************************/
static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_FTDI_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                           RTOS_ERR                           *p_err)
{
  CPU_INT16U                         protocol_val;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol;
  USBH_USB2SER_FTDI_FNCT             *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  protocol_val = p_ftdi_fnct->FlowCtrlProtocol;

  if (DEF_BIT_IS_SET(protocol_val, USBH_USB2SER_FTDI_PROTOCOL_DTR_DSR) == DEF_YES) {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR;
  } else if (DEF_BIT_IS_SET(protocol_val, USBH_USB2SER_FTDI_PROTOCOL_RTS_CTS) == DEF_YES) {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS;
  } else {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (protocol);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_FTDI_SW_FlowCtrlSet()
 *
 * @brief    Sets/clears software flow control on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    en                  Flag that indicates if software flow control must be enabled or
 *                               disabled.
 *
 * @param    xon_char            Xon  character.
 *
 * @param    xoff_char           Xoff character.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) For more information on the 'SetFlowCtrl' request, see 'API for FTxxxx Devices
 *               Application Note AN_115, April 04 2011, Version 1.1, section 3.3'.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        en,
                                             CPU_INT08U                         xon_char,
                                             CPU_INT08U                         xoff_char,
                                             RTOS_ERR                           *p_err)
{
  CPU_INT16U             protocol_val;
  CPU_INT16U             sw_flow_ctrl_chars;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  protocol_val = p_ftdi_fnct->FlowCtrlProtocol;

  if (en == DEF_ENABLED) {
    DEF_BIT_SET(protocol_val, USBH_USB2SER_FTDI_PROTOCOL_XON_XOFF);
  } else {
    DEF_BIT_CLR(protocol_val, (CPU_INT16U)USBH_USB2SER_FTDI_PROTOCOL_XON_XOFF);
  }

  sw_flow_ctrl_chars = (CPU_INT16U)(((CPU_INT16U)xoff_char << 8u) | xon_char);

  USBH_USB2SER_FTDI_StdReq(p_adapter_drv_data,                  // Send SetFlowCtrl req.
                           USBH_USB2SER_FTDI_REQ_SET_FLOW_CTRL,
                           sw_flow_ctrl_chars,
                           protocol_val,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_ftdi_fnct->FlowCtrlProtocol = protocol_val;
    p_ftdi_fnct->XonXoffChar = sw_flow_ctrl_chars;
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_FTDI_SW_FlowCtrlGet()
 *
 * @brief    Gets software flow control handshaking status on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_xon_char          Pointer to variable that will receive the current Xon character.
 *
 * @param    p_xoff_char         Pointer to variable that will receive the current Xoff character.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Current status of software handshaking (enabled or disabled).
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_FTDI_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                    CPU_INT08U                         *p_xon_char,
                                                    CPU_INT08U                         *p_xoff_char,
                                                    RTOS_ERR                           *p_err)
{
  CPU_BOOLEAN            sw_flow_ctrl_en;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  sw_flow_ctrl_en = DEF_BIT_IS_SET(p_ftdi_fnct->FlowCtrlProtocol, USBH_USB2SER_FTDI_PROTOCOL_XON_XOFF) ? DEF_ENABLED : DEF_DISABLED;

  if (p_xon_char != DEF_NULL) {
    *p_xon_char = (CPU_INT08U)(p_ftdi_fnct->XonXoffChar & 0x00FFu);
  }

  if (p_xoff_char != DEF_NULL) {
    *p_xoff_char = (CPU_INT08U)((p_ftdi_fnct->XonXoffChar >> 8u) & 0x00FFu);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (sw_flow_ctrl_en);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_RxAsync()
 *
 * @brief    Initiates data reception on communication port. This function is non blocking.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_buf               Pointer to buffer that contains the data to transfer.
 *
 * @param    buf_len             Buffer length, in octets.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_buf,
                                      CPU_INT32U                         buf_len,
                                      RTOS_ERR                           *p_err)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_ftdi_fnct->RxEP_Handle == USBH_EP_HANDLE_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  USBH_EP_BulkRxAsync(p_adapter_drv_data->DevHandle,
                      p_ftdi_fnct->RxEP_Handle,
                      p_buf,
                      buf_len,
                      USBH_USB2SER_FTDI_DataRxCmpl,
                      p_adapter_drv_data,
                      p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_TxAsync()
 *
 * @brief    Sends data on communication port. This function is non blocking.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_buf               Pointer to buffer of data that will be sent.
 *
 * @param    buf_len             Buffer length in bytes.
 *
 * @param    p_arg               Pointer to argument that will be passed as parameter of
 *                               USBH_USB2SER_FnctAsyncTxCmpl().
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      CPU_INT08U                         *p_buf,
                                      CPU_INT32U                         buf_len,
                                      void                               *p_arg,
                                      RTOS_ERR                           *p_err)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;

  USBH_EP_BulkTxAsync(p_adapter_drv_data->DevHandle,
                      p_ftdi_fnct->TxEP_Handle,
                      p_buf,
                      buf_len,
                      USBH_USB2SER_FnctAsyncTxCmpl,
                      p_arg,
                      p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_ProbeFnct()
 *
 * @brief    Probe to see if function connected is supported by this adapter driver.
 *
 * @param    dev_handle      Handle to dev.
 *
 * @param    fnct_handle     Handle to fnct.
 *
 * @param    class_code      Class code.
 *
 * @param    pp_class_fnct   Pointer to variable that points to adapter driver's function data.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if connected function is a FTDI,
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) See "API for FTxxxx Devices Application Note AN_115", April 04 2011, Version 1.1,
 *               section 2 for more details.
 *
 * @note     (2) The serial nbr string is the only way to differentiate between the FT8U232AM and the
 *               FT232B chips when they have the same bcdDevice.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_FTDI_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                               USBH_FNCT_HANDLE fnct_handle,
                                               CPU_INT08U       class_code,
                                               void             **pp_class_fnct,
                                               RTOS_ERR         *p_err)
{
  USBH_USB2SER_FTDI_FNCT_TYPE local_type = USBH_USB2SER_FTDI_FNCT_TYPE_UNDETERMINED;
  CPU_BOOLEAN                 ret_val = DEF_FAIL;
  CPU_INT16U                  vendor_id;
  CPU_INT16U                  product_id;

  vendor_id = USBH_DevVendorID_Get(dev_handle,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (ret_val);
  }

  product_id = USBH_DevProductID_Get(dev_handle,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (ret_val);
  }

  //                                                               See if possible to determine which type of FTDI ...
  //                                                               chip is used, based on dflt vendor and product IDs.
  if (vendor_id == USBH_USB2SER_FTDI_ID_VENDOR) {
    switch (product_id) {
      case USBH_USB2SER_FTDI_ID_PRODUCT_FT4232H:
        local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT4232H;
        break;

      case USBH_USB2SER_FTDI_ID_PRODUCT_FT232H:
        local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT232H;
        break;

      default:
        break;
    }
  }
  //                                                               If type has not been determined, try finding it ...
  //                                                               using the bcdDevice field. See Note #1.
  if (local_type == USBH_USB2SER_FTDI_FNCT_TYPE_UNDETERMINED) {
        #if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
    {
      CPU_INT16U bcd_dev;

      bcd_dev = USBH_DevRelNbrGet(dev_handle,
                                  p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (ret_val);
      }

      if (bcd_dev == USBH_USB2SER_FTDI_BCD_DEVICE_FT8U232AM) {
                #if (USBH_CFG_STR_EN == DEF_ENABLED)            // Check serial nbr string, see Note #2.
        (void)USBH_DevSerNbrStrGet(dev_handle,
                                   DEF_NULL,
                                   0u,
                                   p_err);
        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NOT_FOUND) {
          local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT232B;
        } else {
          local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT8U232AM;
        }
                #endif
      } else {
        switch (bcd_dev) {
          case USBH_USB2SER_FTDI_BCD_DEVICE_FT232B_WITH_EEPROM:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT232B;
            break;

          case USBH_USB2SER_FTDI_BCD_DEVICE_FT2232D:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT2232D;
            break;

          case USBH_USB2SER_FTDI_BCD_DEVICE_FT232R:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT232R;
            break;

          case USBH_USB2SER_FTDI_BCD_DEVICE_FT2232H:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT2232H;
            break;

          case USBH_USB2SER_FTDI_BCD_DEVICE_FT4232H:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT4232H;
            break;

          case USBH_USB2SER_FTDI_BCD_DEVICE_FT232H:
            local_type = USBH_USB2SER_FTDI_FNCT_TYPE_FT232H;
            break;

          default:
            RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
            return (ret_val);
        }
      }
    }
        #endif
  }

  if (class_code == USBH_CLASS_CODE_VENDOR_SPECIFIC) {          // Class must be Vendor Specific.
    CPU_BOOLEAN found = DEF_NO;
    CPU_INT08U  ix;

    //                                                             Chk known Vendor and Product IDs, to see if matches.
    for (ix = 0u; ix < USBH_USB2SER_FTDI_ID_TBL_LEN; ++ix) {
      if ((vendor_id == USBH_FTDI_SerialDfltID_Tbl[ix].VendorID)
          && (product_id == USBH_FTDI_SerialDfltID_Tbl[ix].ProductID)) {
        found = DEF_YES;                                        // Valid combination found. Early exit.
        break;
      }
    }

    if (found == DEF_NO) {                                      // If no valid combination was found.
                                                                // Chk app-provided Vendor and Product IDs.
                                                                // See USBH_FTDI_Init() Note #1.
      for (ix = 0u; ix < USBH_USB2SER_FTDI_Ptr->AppID_TblLen; ++ix) {
        if ((vendor_id == USBH_USB2SER_FTDI_Ptr->AppID_Tbl[ix].VendorID)
            && (product_id == USBH_USB2SER_FTDI_Ptr->AppID_Tbl[ix].ProductID)) {
          found = DEF_YES;                                      // Valid combination found. Early exit.
          break;
        }
      }
    }

    if (found == DEF_YES) {
      CPU_INT08U             if_nbr;
      USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct;

      p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)*pp_class_fnct;

      p_ftdi_fnct->Type = local_type;
      p_ftdi_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
      p_ftdi_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;
      p_ftdi_fnct->SetDataVal = DEF_BIT_NONE;
      p_ftdi_fnct->BaudRate = 0u;
      p_ftdi_fnct->FlowCtrlProtocol = DEF_BIT_NONE;
      p_ftdi_fnct->ModemCtrlVal = DEF_BIT_NONE;
      p_ftdi_fnct->XonXoffChar = 0u;

      if_nbr = USBH_IF_NbrGet(dev_handle,
                              fnct_handle,
                              0u,
                              p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_FAIL);
      }

      p_ftdi_fnct->PortNbr = if_nbr + 1u;

      ret_val = DEF_OK;
    }
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_EP_Open()
 *
 * @brief    Notify adapter drvier that an endpoint was opened.
 *
 * @param    p_class_fnct    Pointer to internal FTDI serial fnct structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_EP_Open(void           *p_class_fnct,
                                      USBH_EP_HANDLE ep_handle,
                                      CPU_INT08U     if_ix,
                                      CPU_INT08U     ep_type,
                                      CPU_BOOLEAN    ep_dir_in)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_class_fnct;

  (void)&if_ix;

  if (ep_type == USBH_EP_TYPE_BULK) {
    if (ep_dir_in == DEF_YES) {
      p_ftdi_fnct->RxEP_Handle = ep_handle;
    } else {
      p_ftdi_fnct->TxEP_Handle = ep_handle;
    }
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_EP_Close()
 *
 * @brief    Notify adapter driver that endpoint has been closed.
 *
 * @param    p_class_fnct    Pointer to internal adapter fnct structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_EP_Close(void           *p_class_fnct,
                                       USBH_EP_HANDLE ep_handle,
                                       CPU_INT08U     if_ix)
{
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_class_fnct;

  (void)&if_ix;

  if (ep_handle == p_ftdi_fnct->RxEP_Handle) {
    p_ftdi_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
  } else if (ep_handle == p_ftdi_fnct->TxEP_Handle) {
    p_ftdi_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_TraceDump()
 *
 * @brief    Output adapter driver specific debug information on USB-to-serial function.
 *
 * @param    p_class_fnct    Pointer to internal structure of the function.
 *
 * @param    opt             Trace options.
 *
 * @param    trace_fnct      Trace output function.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_USB2SER_FTDI_TraceDump(void                *p_class_fnct,
                                        CPU_INT32U          opt,
                                        USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR               str[2u];
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_class_fnct;

  (void)&opt;

  trace_fnct("      --- USB Host (USB-To-Serial FTDI specific debug information) ---\r\n");
  trace_fnct("      | Type         | Port #  |                                        |\r\n");
  trace_fnct("      | ");

  //                                                               Output state.
  switch (p_ftdi_fnct->Type) {
    case USBH_USB2SER_FTDI_FNCT_TYPE_FT2232D:
      trace_fnct("FT2232D      | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT2232H:
      trace_fnct("FT2232H      | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT232B:
      trace_fnct("FT232B       | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT232H:
      trace_fnct("FT232H       | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT232R:
      trace_fnct("FT232R       | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT4232H:
      trace_fnct("FT4232H      | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_FT8U232AM:
      trace_fnct("FT8U232AM    | ");
      break;

    case USBH_USB2SER_FTDI_FNCT_TYPE_UNDETERMINED:
      trace_fnct("Undetermined | ");
      break;

    default:
      trace_fnct("???????      | ");
      break;
  }

  //                                                               Output port #.
  (void)Str_FmtNbr_Int32U(p_ftdi_fnct->PortNbr,
                          2u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("      |\r\n");
}
#endif

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_StdReq()
 *
 * @brief    Sends standard FTDI control request to device.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    req                 FTDI control request type.
 *
 * @param    val                 Value of the wValue field.
 *
 * @param    hi_ix               High-Byte value of the wIndex field.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_StdReq(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                     CPU_INT08U                         req,
                                     CPU_INT16U                         val,
                                     CPU_INT08U                         hi_ix,
                                     RTOS_ERR                           *p_err)
{
  CPU_INT16U             ix;
  USBH_USB2SER_FTDI_FNCT *p_ftdi_fnct = (USBH_USB2SER_FTDI_FNCT *)p_adapter_drv_data->DataPtr;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  ix = (CPU_INT16U)(((CPU_INT16U)hi_ix << 8u) | p_ftdi_fnct->PortNbr);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         req,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_RECIPIENT_DEV),
                         val,
                         ix,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FTDI_DataRxCmpl()
 *
 * @brief    Asynchronous data receive completion function.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to reception buffer.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    xfer_len    Number of octets transferred.
 *
 * @param    p_arg       Asynchronous function argument.
 *
 * @param    err         Status of transaction.
 *
 * @note     (1) The received data in 'p_buf' is in little-endian.
 *
 * @note     (2) The FTDI device always add a serial status of two bytes at the beginning of every
 *               packet. This function extracts the serial status from those two bytes and adjusts the
 *               buffer pointer to point on actual data and not status bytes.
 *******************************************************************************************************/
static void USBH_USB2SER_FTDI_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
                                         USBH_EP_HANDLE  ep_handle,
                                         CPU_INT08U      *p_buf,
                                         CPU_INT32U      buf_len,
                                         CPU_INT32U      xfer_len,
                                         void            *p_arg,
                                         RTOS_ERR        err)
{
  CPU_INT32U                         rx_len = xfer_len;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data = (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *)p_arg;

  (void)&dev_handle;
  (void)&ep_handle;

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (rx_len >= USBH_USB2SER_FTDI_STATUS_LEN)) {
    USBH_USB2SER_SERIAL_STATUS status = { DEF_BIT_NONE, DEF_BIT_NONE };

    status.Modem |= (DEF_BIT_IS_SET(p_buf[0u], USBH_USB2SER_FTDI_MODEM_STATUS_CTS)     ? USBH_USB2SER_MODEM_STATUS_CTS     : DEF_BIT_NONE);
    status.Modem |= (DEF_BIT_IS_SET(p_buf[0u], USBH_USB2SER_FTDI_MODEM_STATUS_DSR)     ? USBH_USB2SER_MODEM_STATUS_DSR     : DEF_BIT_NONE);
    status.Modem |= (DEF_BIT_IS_SET(p_buf[0u], USBH_USB2SER_FTDI_MODEM_STATUS_RING)    ? USBH_USB2SER_MODEM_STATUS_RING    : DEF_BIT_NONE);
    status.Modem |= (DEF_BIT_IS_SET(p_buf[0u], USBH_USB2SER_FTDI_MODEM_STATUS_CARRIER) ? USBH_USB2SER_MODEM_STATUS_CARRIER : DEF_BIT_NONE);

    status.Line |= (DEF_BIT_IS_SET(p_buf[1u], USBH_USB2SER_FTDI_LINE_STATUS_PARITY_ERR)      ? USBH_USB2SER_LINE_STATUS_PARITY_ERR      : DEF_BIT_NONE);
    status.Line |= (DEF_BIT_IS_SET(p_buf[1u], USBH_USB2SER_FTDI_LINE_STATUS_FRAMING_ERR)     ? USBH_USB2SER_LINE_STATUS_FRAMING_ERR     : DEF_BIT_NONE);
    status.Line |= (DEF_BIT_IS_SET(p_buf[1u], USBH_USB2SER_FTDI_LINE_STATUS_BREAK_INT)       ? USBH_USB2SER_LINE_STATUS_BRK_INT         : DEF_BIT_NONE);
    status.Line |= (DEF_BIT_IS_SET(p_buf[1u], USBH_USB2SER_FTDI_LINE_STATUS_RX_OVERFLOW_ERR) ? USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR : DEF_BIT_NONE);

    rx_len -= USBH_USB2SER_FTDI_STATUS_LEN;

    USBH_USB2SER_FnctStatusUpdate(p_adapter_drv_data, status);
  }

  USBH_USB2SER_FnctAsyncRxCmpl(p_adapter_drv_data,
                               p_buf,
                               &p_buf[USBH_USB2SER_FTDI_STATUS_LEN],
                               buf_len,
                               rx_len,
                               err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_USB2SER_FTDI_AVAIL))
