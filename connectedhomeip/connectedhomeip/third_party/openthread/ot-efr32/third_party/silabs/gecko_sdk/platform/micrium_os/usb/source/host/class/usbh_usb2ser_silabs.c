/***************************************************************************//**
 * @file
 * @brief USB Host - Silcon Labs (Cp210X) Serial Class
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
 * @note             (1) The reference document "AN571 CP210x Virtual COM Port Interface" can be
 *                       downloaded from Silicon Labs website.
 *
 * @note             (2) This adapter driver supports the following Silicon Labs chips model:
 *                       - CP2102
 *                       - CP2103
 *                       - CP2104
 *                       - CP2105
 *                       - CP2108
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_SILABS_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))

#error USB HOST USB2SER Silicon Labs adtapter driver requires USB Host USB2SER class. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_USB2SER_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_USB2SER_SILABS_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_usb2ser.h>
#include  <usb/include/host/usbh_usb2ser_silabs.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/class/usbh_usb2ser_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>

#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, USB2SER, SILABS)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_USB2SER_SILABS_FS_MAX_XFER_LEN                      64u

#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
#define  USBH_USB2SER_SILABS_CTRL_BUF_LEN                         76u
#else
#define  USBH_USB2SER_SILABS_CTRL_BUF_LEN                         19u
#endif

//                                                                 Embedded serial events escape character.
#define  USBH_USB2SER_SILABS_SER_EVENTS_ESC_CHAR              0x0001u

/********************************************************************************************************
 *                                       SILICON LABS VENDOR ID
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_ID_VENDOR                        0x10C4u

/********************************************************************************************************
 *                                       SILICON LABS PRODUCT IDs
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_ID_PRODUCT_A                     0xEA60u
#define  USBH_USB2SER_SILABS_ID_PRODUCT_B                     0xEA70u
#define  USBH_USB2SER_SILABS_ID_PRODUCT_C                     0xEA71u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_REQ_IFC_ENABLE                     0x00u
#define  USBH_USB2SER_SILABS_REQ_SETBAUDDIV                     0x01u
#define  USBH_USB2SER_SILABS_REQ_GETBAUDDIV                     0x02u
#define  USBH_USB2SER_SILABS_REQ_SET_LINE_CTL                   0x03u
#define  USBH_USB2SER_SILABS_REQ_GET_LINE_CTL                   0x04u
#define  USBH_USB2SER_SILABS_REQ_SET_BREAK                      0x05u
#define  USBH_USB2SER_SILABS_REQ_IMM_CHAR                       0x06u
#define  USBH_USB2SER_SILABS_REQ_SET_MHS                        0x07u
#define  USBH_USB2SER_SILABS_REQ_GET_MDMSTS                     0x08u
#define  USBH_USB2SER_SILABS_REQ_SET_XON                        0x09u
#define  USBH_USB2SER_SILABS_REQ_SET_XOFF                       0x0Au
#define  USBH_USB2SER_SILABS_REQ_SET_EVENTMASK                  0x0Bu
#define  USBH_USB2SER_SILABS_REQ_GET_EVENTMASK                  0x0Cu
#define  USBH_USB2SER_SILABS_REQ_GET_EVENTSTATE                 0x16u
#define  USBH_USB2SER_SILABS_REQ_SET_CHAR                       0x0Du
#define  USBH_USB2SER_SILABS_REQ_GET_CHARS                      0x0Eu
#define  USBH_USB2SER_SILABS_REQ_GET_PROPS                      0x0Fu
#define  USBH_USB2SER_SILABS_REQ_GET_COMM_STATUS                0x10u
#define  USBH_USB2SER_SILABS_REQ_RESET                          0x11u
#define  USBH_USB2SER_SILABS_REQ_PURGE                          0x12u
#define  USBH_USB2SER_SILABS_REQ_SET_FLOW                       0x13u
#define  USBH_USB2SER_SILABS_REQ_GET_FLOW                       0x14u
#define  USBH_USB2SER_SILABS_REQ_EMBED_EVENTS                   0x15u
#define  USBH_USB2SER_SILABS_REQ_GET_BAUDRATE                   0x1Du
#define  USBH_USB2SER_SILABS_REQ_SET_BAUDRATE                   0x1Eu
#define  USBH_USB2SER_SILABS_REQ_SET_CHARS                      0x19u
#define  USBH_USB2SER_SILABS_REQ_SET_VENDOR_SPECIFIC            0xFFu

//                                                                 Line ctl req stop bits values.
#define  USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_MSK             DEF_BIT_FIELD(4u, 0u)
#define  USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1               0u
#define  USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1_5             1u
#define  USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_2               2u

//                                                                 Line ctl req parity values.
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_MSK                DEF_BIT_FIELD(4u, 4u)
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_NONE               0u
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_ODD                1u
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_EVEN               2u
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_MARK               3u
#define  USBH_USB2SER_SILABS_LINE_CTL_PARITY_SPACE              4u

//                                                                 Line ctl req data length mask.
#define  USBH_USB2SER_SILABS_LINE_CTL_DATA_LEN_MSK              DEF_BIT_FIELD(8u, 8u)

//                                                                 Set MHS req values.
#define  USBH_USB2SER_SILABS_MHS_DTR                            DEF_BIT_00
#define  USBH_USB2SER_SILABS_MHS_RTS                            DEF_BIT_01
#define  USBH_USB2SER_SILABS_MHS_DTR_EN                         DEF_BIT_08
#define  USBH_USB2SER_SILABS_MHS_RTS_EN                         DEF_BIT_09

//                                                                 Set char req selection.
#define  USBH_USB2SER_SILABS_CHAR_MSK                           DEF_BIT_FIELD(8u, 0u)
#define  USBH_USB2SER_SILABS_CHAR_EOF                           0u
#define  USBH_USB2SER_SILABS_CHAR_ERROR                         1u
#define  USBH_USB2SER_SILABS_CHAR_BREAK                         2u
#define  USBH_USB2SER_SILABS_CHAR_EVENT                         3u
#define  USBH_USB2SER_SILABS_CHAR_XON                           4u
#define  USBH_USB2SER_SILABS_CHAR_XOFF                          5u

//                                                                 Set char req value mask.
#define  USBH_USB2SER_SILABS_CHAR_VAL_MSK                       DEF_BIT_FIELD(8u, 8u)

//                                                                 Purge req selectors.
#define  USBH_USB2SER_SILABS_PURGE_SEL_TX                       DEF_BIT_00
#define  USBH_USB2SER_SILABS_PURGE_SEL_RX                       DEF_BIT_01

/********************************************************************************************************
 *                                   SILICON LABS EMBEDDED SERIAL STATUS
 *******************************************************************************************************/

//                                                                 Embedded serial events ctrl char.
#define  USBH_USB2SER_EVENT_CTRL_CHAR_NONE                      0u
#define  USBH_USB2SER_EVENT_CTRL_CHAR_LINE_DATA                 1u
#define  USBH_USB2SER_EVENT_CTRL_CHAR_LINE                      2u
#define  USBH_USB2SER_EVENT_CTRL_CHAR_MODEM                     3u

//                                                                 Embedded serial events modem status fields.
#define  USBH_USB2SER_SILABS_MODEM_STATUS_DELTA_CTS             DEF_BIT_00
#define  USBH_USB2SER_SILABS_MODEM_STATUS_DELTA_DSR             DEF_BIT_01
#define  USBH_USB2SER_SILABS_MODEM_STATUS_TRAIL_EDGE_RI         DEF_BIT_02
#define  USBH_USB2SER_SILABS_MODEM_STATUS_DELTA_DCD             DEF_BIT_03
#define  USBH_USB2SER_SILABS_MODEM_STATUS_CTS                   DEF_BIT_04
#define  USBH_USB2SER_SILABS_MODEM_STATUS_DSR                   DEF_BIT_05
#define  USBH_USB2SER_SILABS_MODEM_STATUS_RI                    DEF_BIT_06
#define  USBH_USB2SER_SILABS_MODEM_STATUS_DCD                   DEF_BIT_07

//                                                                 Embedded serial events line status fields.
#define  USBH_USB2SER_SILABS_LINE_STATUS_DATA_READY             DEF_BIT_00
#define  USBH_USB2SER_SILABS_LINE_STATUS_HW_OVERRUN             DEF_BIT_01
#define  USBH_USB2SER_SILABS_LINE_STATUS_PARITY_ERR             DEF_BIT_02
#define  USBH_USB2SER_SILABS_LINE_STATUS_FRAMING_ERR            DEF_BIT_03
#define  USBH_USB2SER_SILABS_LINE_STATUS_BRK                    DEF_BIT_04

/********************************************************************************************************
 *                                           CONTROL HANDSHAKE
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_DTR_MSK                           (DEF_BIT_00 | DEF_BIT_01)
#define  USBH_USB2SER_SILABS_DTR_INACTIVE                       0u
#define  USBH_USB2SER_SILABS_DTR_ACTIVE                         1u
#define  USBH_USB2SER_SILABS_DTR_CTRL                           2u

#define  USBH_USB2SER_SILABS_CTS_HANDSHAKE                      DEF_BIT_03
#define  USBH_USB2SER_SILABS_DSR_HANDSHAKE                      DEF_BIT_04
#define  USBH_USB2SER_SILABS_DCD_HANDSHAKE                      DEF_BIT_05
#define  USBH_USB2SER_SILABS_DSR_SENSITIVE                      DEF_BIT_06

/********************************************************************************************************
 *                                               FLOW REPLACE
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_AUTO_TRANSMIT_XON_XOFF             DEF_BIT_00
#define  USBH_USB2SER_SILABS_AUTO_RECEIVE_XON_XOFF              DEF_BIT_01
#define  USBH_USB2SER_SILABS_ERROR_CHAR_INSERT                  DEF_BIT_00
#define  USBH_USB2SER_SILABS_NULL_STRIPPING_EN                  DEF_BIT_00
#define  USBH_USB2SER_SILABS_BREAK_CHAR_RESP_EN                 DEF_BIT_00

#define  USBH_USB2SER_SILABS_RTS_MSK                           (DEF_BIT_06 | DEF_BIT_07)
#define  USBH_USB2SER_SILABS_RTS_INACTIVE                       0u
#define  USBH_USB2SER_SILABS_RTS_ACTIVE                         1u
#define  USBH_USB2SER_SILABS_RTS_FLOW_CTRL                      2u
#define  USBH_USB2SER_SILABS_RTS_TRANSMIT_ACTIVE                3u

#define  USBH_USB2SER_SILABS_XOFF_CONTINUE                      DEF_BIT_31

/********************************************************************************************************
 *                                               CAPABILITIES
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_SETTABLE_FLOW_DTR_DSR              DEF_BIT_00
#define  USBH_USB2SER_SILABS_SETTABLE_FLOW_RTS_CTS              DEF_BIT_01
#define  USBH_USB2SER_SILABS_SUPPORT_DCD                        DEF_BIT_02
#define  USBH_USB2SER_SILABS_SUPPORT_PARITY_CHECK               DEF_BIT_03
#define  USBH_USB2SER_SILABS_SETTABLE_XON_XOFF                  DEF_BIT_04
#define  USBH_USB2SER_SILABS_SETTABLE_XON_XOFF_CHARS            DEF_BIT_05

/********************************************************************************************************
 *                                               SETTABLE PARAMS
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_SETTABLE_PARITY_TYPE               DEF_BIT_00
#define  USBH_USB2SER_SILABS_SETTABLE_BAUDRATE                  DEF_BIT_01
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS                 DEF_BIT_02
#define  USBH_USB2SER_SILABS_SETTABLE_STOP_BITS                 DEF_BIT_03
#define  USBH_USB2SER_SILABS_SETTABLE_HANDSHAKE                 DEF_BIT_04
#define  USBH_USB2SER_SILABS_SETTABLE_PARITY_CHECK              DEF_BIT_05
#define  USBH_USB2SER_SILABS_SETTABLE_CARRIER_DETECT            DEF_BIT_05

/********************************************************************************************************
 *                                               SETTABLE BAUDRATE
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_75                   DEF_BIT_00
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_110                  DEF_BIT_01
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_134_5                DEF_BIT_02
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_150                  DEF_BIT_03
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_300                  DEF_BIT_04
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_600                  DEF_BIT_05
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_1200                 DEF_BIT_06
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_1800                 DEF_BIT_07
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_2400                 DEF_BIT_08
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_4800                 DEF_BIT_09
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_7200                 DEF_BIT_10
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_9600                 DEF_BIT_11
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_14400                DEF_BIT_12
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_19200                DEF_BIT_13
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_38400                DEF_BIT_14
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_56000                DEF_BIT_15
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_128000               DEF_BIT_16
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_115200               DEF_BIT_17
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_57600                DEF_BIT_18
#define  USBH_USB2SER_SILABS_SETTABLE_BAUD_OTHER                DEF_BIT_28

/********************************************************************************************************
 *                                           SETTABLE DATA BITS
 *******************************************************************************************************/

#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_5               DEF_BIT_00
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_6               DEF_BIT_01
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_7               DEF_BIT_02
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_8               DEF_BIT_03
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_16              DEF_BIT_04
#define  USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_16_EXT          DEF_BIT_05

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           SILABS FUNCTION STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_silabs_fnct {
  USBH_EP_HANDLE             RxEP_Handle;                       // Handle of rx EP.
  USBH_EP_HANDLE             TxEP_Handle;                       // Handle of tx EP.

  CPU_INT08U                 IF_Nbr;                            // Cur fnct IF nbr.
  CPU_INT08U                 ModemCtrlEn;                       // Backup of modem ctrl value.

#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  CPU_INT32U                 MaxBaudRate;                       // Maximum adapter's supported baudrate.
  CPU_INT08U                 SettableParams;                    // Bitfield of adapter's settable parameters.
  CPU_INT32U                 SettableBaud;                      // Bitfield of adapter's settable baudrates.
  CPU_INT08U                 SettableDataBits;                  // Bitfield of adapter's settabe data bits length.
  CPU_INT08U                 Capabilities;                      // Bitfield of general adapter's cpabilities.
#endif

  USBH_USB2SER_SERIAL_STATUS SerialStatus;                      // Cur serial status.
} USBH_USB2SER_SILABS_FNCT;

/********************************************************************************************************
 *                                           SILABS ADAPTER DRV DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_silabs {
  USBH_USB2SER_APP_ID *AppID_Tbl;                               // Tbl of app-specific Silabs serial devices.
  CPU_INT08U          AppID_TblLen;                             // Len of tbl of app-specific Silabs serial devices.
} USBH_USB2SER_SILABS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_USB2SER_SILABS *USBH_USB2SER_SILABS_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------- ADAPTER DRIVER FUNCTIONS -------------
static void USBH_USB2SER_SILABS_Init(MEM_SEG             *p_mem_seg,
                                     USBH_USB2SER_APP_ID *id_tbl,
                                     CPU_INT08U          id_tbl_len,
                                     RTOS_ERR            *p_err);

static void USBH_USB2SER_SILABS_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data);

static CPU_INT08U USBH_USB2SER_SILABS_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      USBH_USB2SER_RESET_SEL             sel,
                                      RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                            CPU_INT32U                         baudrate,
                                            RTOS_ERR                           *p_err);

static CPU_INT32U USBH_USB2SER_SILABS_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                  RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         data_size,
                                        USBH_USB2SER_PARITY                parity,
                                        USBH_USB2SER_STOP_BITS             stop_bits,
                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_data_size,
                                        USBH_USB2SER_PARITY                *p_parity,
                                        USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        set,
                                             RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        dtr_en,
                                             CPU_BOOLEAN                        dtr_set,
                                             CPU_BOOLEAN                        rts_en,
                                             CPU_BOOLEAN                        rts_set,
                                             RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        *p_dtr_en,
                                             CPU_BOOLEAN                        *p_dtr_set,
                                             CPU_BOOLEAN                        *p_rts_en,
                                             CPU_BOOLEAN                        *p_rts_set,
                                             RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                               RTOS_ERR                           *p_err);

static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_SILABS_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                             RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        en,
                                               CPU_INT08U                         xon_char,
                                               CPU_INT08U                         xoff_char,
                                               RTOS_ERR                           *p_err);

static CPU_BOOLEAN USBH_USB2SER_SILABS_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                      CPU_INT08U                         *p_xon_char,
                                                      CPU_INT08U                         *p_xoff_char,
                                                      RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT32U                         buf_len,
                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT32U                         buf_len,
                                        void                               *p_arg,
                                        RTOS_ERR                           *p_err);

//                                                                 -------------- CLASS DRIVER FUNCTIONS --------------
static CPU_BOOLEAN USBH_USB2SER_SILABS_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                                 USBH_FNCT_HANDLE fnct_handle,
                                                 CPU_INT08U       class_code,
                                                 void             **pp_class_fnct,
                                                 RTOS_ERR         *p_err);

static void USBH_USB2SER_SILABS_EP_Open(void           *p_class_fnct,
                                        USBH_EP_HANDLE ep_handle,
                                        CPU_INT08U     if_ix,
                                        CPU_INT08U     ep_type,
                                        CPU_BOOLEAN    ep_dir_in);

static void USBH_USB2SER_SILABS_EP_Close(void           *p_class_fnct,
                                         USBH_EP_HANDLE ep_handle,
                                         CPU_INT08U     if_ix);

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static void USBH_USB2SER_SILABS_CtrlReq(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_BOOLEAN                        host_to_dev,
                                        CPU_INT08U                         req,
                                        CPU_INT16U                         val,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT16U                         len,
                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_SILABS_StatusLineUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_INT08U                         status_byte);

static void USBH_USB2SER_SILABS_StatusModemUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                  CPU_INT08U                         status_byte);

static void USBH_USB2SER_SILABS_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
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
 *                                           SILABS ADAPTER DRIVER
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------- SILABS DEVICE ADAPTER DRIVER -----------
const USBH_USB2SER_DEV_ADAPTER_DRV USBH_USB2SER_SILABS_DevAdapterDrv = {
  USBH_USB2SER_SILABS_Init,
  USBH_USB2SER_SILABS_Conn,
  USBH_USB2SER_SILABS_PortNbrGet,
  USBH_USB2SER_SILABS_Reset,
  USBH_USB2SER_SILABS_BaudRateSet,
  USBH_USB2SER_SILABS_BaudRateGet,
  USBH_USB2SER_SILABS_DataSet,
  USBH_USB2SER_SILABS_DataGet,
  USBH_USB2SER_SILABS_BrkSignalSet,
  USBH_USB2SER_SILABS_ModemCtrlSet,
  USBH_USB2SER_SILABS_ModemCtrlGet,
  USBH_USB2SER_SILABS_HW_FlowCtrlSet,
  USBH_USB2SER_SILABS_HW_FlowCtrlGet,
  USBH_USB2SER_SILABS_SW_FlowCtrlSet,
  USBH_USB2SER_SILABS_SW_FlowCtrlGet,
  USBH_USB2SER_SILABS_RxAsync,
  USBH_USB2SER_SILABS_TxAsync,

  USBH_USB2SER_SILABS_CTRL_BUF_LEN,
  0u,                                                           // Serial status embedded in Rx data.
  USBH_USB2SER_SILABS_FS_MAX_XFER_LEN,
  0u,                                                           // No HS support.
  sizeof(USBH_USB2SER_SILABS_FNCT)
};

//                                                                 ----------- SILABS ADAPTER CLASS DRIVER ------------
static USBH_CLASS_DRV USBH_USB2SER_SILABS_ClassDrv = {
  DEF_NULL,
  USBH_USB2SER_SILABS_ProbeFnct,
  DEF_NULL,
  USBH_USB2SER_SILABS_EP_Open,
  USBH_USB2SER_SILABS_EP_Close,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  DEF_NULL,
#endif
  (CPU_CHAR *)"USB2SER - SILABS",
};

//                                                                 -------------- SILABS ADAPTER DRIVER ---------------
const USBH_USB2SER_ADAPTER_DRV USBH_USB2SER_SILABS_Drv = {
  &USBH_USB2SER_SILABS_ClassDrv,
  (USBH_USB2SER_DEV_ADAPTER_DRV *)&USBH_USB2SER_SILABS_DevAdapterDrv
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_Init()
 *
 * @brief    Initialize the Silicon Labs device adapter driver.
 *
 * @param    p_mem_seg   Pointer to memory segment from which adapter driver data will be allocated.
 *                       Data will be allocated from general purpose heap if null.
 *
 * @param    id_tbl      Table containing list of vendor/product IDs that should be considered
 *                       Silicon Labs serial devices. Can be DEF_NULL. See Note #1.
 *
 * @param    id_tlb_len  Length of ID table.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) It is possible to modify the default Vendor and/or Product IDs of the Silicon Labs
 *               chips. The purpose of the ID table that can be passed at initialization is to allow
 *               a customer to add any Vendor/Product ID combination as Silicon Labs serial devices.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_Init(MEM_SEG             *p_mem_seg,
                                     USBH_USB2SER_APP_ID *id_tbl,
                                     CPU_INT08U          id_tbl_len,
                                     RTOS_ERR            *p_err)
{
  if (USBH_USB2SER_SILABS_Ptr != DEF_NULL) {                    // Chk if SILABS adapter driver already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  USBH_USB2SER_SILABS_Ptr = (USBH_USB2SER_SILABS *)Mem_SegAlloc("USBH - SILABS Data",
                                                                p_mem_seg,
                                                                sizeof(USBH_USB2SER_SILABS),
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_SILABS_Ptr->AppID_Tbl = id_tbl;
  USBH_USB2SER_SILABS_Ptr->AppID_TblLen = id_tbl_len;
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_Conn()
 *
 * @brief    Initiates communication with Silicon Labs adapter.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data)
{
  CPU_INT08U                 status_byte;
  CPU_INT08U                 *p_ctrl_buf = p_adapter_drv_data->AdapterCtrlBufPtr;
  RTOS_ERR                   err_usbh;
  USBH_USB2SER_SERIAL_STATUS serial_status = { DEF_BIT_NONE, DEF_BIT_NONE };
  USBH_USB2SER_SILABS_FNCT   *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  p_silabs_fnct->ModemCtrlEn = DEF_BIT_NONE;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Enable interface.
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_IFC_ENABLE,
                              0x0001u,
                              DEF_NULL,
                              0u,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Enabling interface -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Ask to embed serial events in rx data.
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_EMBED_EVENTS,
                              USBH_USB2SER_SILABS_SER_EVENTS_ESC_CHAR,
                              DEF_NULL,
                              0u,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Asking to embed serial events -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

    #if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Retrieve adapter properties.
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_PROPS,
                              0u,
                              p_ctrl_buf,
                              76u,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Requesting modem status -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  //                                                               Parse adapter properties.
  p_silabs_fnct->MaxBaudRate = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[20u]);
  p_silabs_fnct->Capabilities = (CPU_INT08U)MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[28u]);
  p_silabs_fnct->SettableParams = (CPU_INT08U)MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[32u]);
  p_silabs_fnct->SettableBaud = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[36u]);
  p_silabs_fnct->SettableDataBits = (CPU_INT08U)MEM_VAL_GET_INT16U_LITTLE(&p_ctrl_buf[40u]);
    #endif

  //                                                               ---------- RETRIEVE INITIAL SERIAL STATUS ----------
  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Request serial status.
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_COMM_STATUS,
                              0u,
                              p_ctrl_buf,
                              19u,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Requesting comm status -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  //                                                               Parse line status.
  status_byte = p_ctrl_buf[4u];
  serial_status.Line |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_04) ? USBH_USB2SER_LINE_STATUS_PARITY_ERR      : DEF_BIT_NONE);
  serial_status.Line |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_01) ? USBH_USB2SER_LINE_STATUS_FRAMING_ERR     : DEF_BIT_NONE);
  serial_status.Line |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_00) ? USBH_USB2SER_LINE_STATUS_BRK_INT         : DEF_BIT_NONE);
  serial_status.Line |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_02) ? USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR : DEF_BIT_NONE);

  //                                                               Request modem status.
  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_MDMSTS,
                              0u,
                              p_ctrl_buf,
                              1u,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Requesting modem status -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  //                                                               Parse modem status.
  status_byte = p_ctrl_buf[0u];
  serial_status.Modem |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_04) ? USBH_USB2SER_MODEM_STATUS_CTS     : DEF_BIT_NONE);
  serial_status.Modem |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_05) ? USBH_USB2SER_MODEM_STATUS_DSR     : DEF_BIT_NONE);
  serial_status.Modem |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_06) ? USBH_USB2SER_MODEM_STATUS_RING    : DEF_BIT_NONE);
  serial_status.Modem |= (DEF_BIT_IS_SET(status_byte, DEF_BIT_07) ? USBH_USB2SER_MODEM_STATUS_CARRIER : DEF_BIT_NONE);

  USBH_USB2SER_FnctStatusUpdate(p_adapter_drv_data, serial_status);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_PortNbrGet()
 *
 * @brief    Gets port number.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static CPU_INT08U USBH_USB2SER_SILABS_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 RTOS_ERR                           *p_err)
{
  CPU_INT08U               port_nbr;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  port_nbr = p_silabs_fnct->IF_Nbr + 1u;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (port_nbr);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_Reset()
 *
 * @brief    Purges adapter's internal buffers.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *
 * @param    sel                 Purge selection:
 *                                   - USBH_USB2SER_RESET_SEL_TX,    Purge transmit  adapter's buffer.
 *                                   - USBH_USB2SER_RESET_SEL_RX,    Purge reception adapter's buffer.
 *                                   - USBH_USB2SER_RESET_SEL_ALL    Purge all       adapter's buffers.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                      USBH_USB2SER_RESET_SEL             sel,
                                      RTOS_ERR                           *p_err)
{
  CPU_INT16U val;

  switch (sel) {
    case USBH_USB2SER_RESET_SEL_RX:
      val = USBH_USB2SER_SILABS_PURGE_SEL_RX;
      break;

    case USBH_USB2SER_RESET_SEL_TX:
      val = USBH_USB2SER_SILABS_PURGE_SEL_TX;
      break;

    case USBH_USB2SER_RESET_SEL_ALL:
    default:
      val = (USBH_USB2SER_SILABS_PURGE_SEL_RX | USBH_USB2SER_SILABS_PURGE_SEL_TX);
      break;
  }

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Send Reset req.
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_PURGE,
                              val,
                              DEF_NULL,
                              0u,
                              p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_BaudRateSet()
 *
 * @brief    Sets baud rate of the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
 *
 * @param    baudrate            Baud rate to set (in baud/sec). Can be any value supported by
 *                               Silicon Labs chip.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                            CPU_INT32U                         baudrate,
                                            RTOS_ERR                           *p_err)
{
#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  if (DEF_BIT_IS_SET(p_silabs_fnct->SettableParams, USBH_USB2SER_SILABS_SETTABLE_BAUDRATE) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  if (baudrate > p_silabs_fnct->MaxBaudRate) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  switch (baudrate) {
    case 75u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_75) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 110u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_110) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 150u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_150) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 300u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_300) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 600u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_600) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 1200u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_1200) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 1800u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_1800) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 2400u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_2400) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 4800u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_4800) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 7200u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_7200) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 9600u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_9600) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 14400u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_14400) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 19200u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_19200) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 38400u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_38400) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 56000u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_56000) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 57600u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_57600) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 115200u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_115200) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    case 128000u:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_128000) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
      break;

    default:
      if (DEF_BIT_IS_SET(p_silabs_fnct->SettableBaud, USBH_USB2SER_SILABS_SETTABLE_BAUD_OTHER) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }

      LOG_DBG(("!! WARNING !! Selected baudrate may NOT be supported by serial adapter."));
      break;
  }
#endif

  MEM_VAL_SET_INT32U_LITTLE(p_adapter_drv_data->AdapterCtrlBufPtr, baudrate);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,               // Send Reset req.
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_BAUDRATE,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              4u,
                              p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_BaudRateGet()
 *
 * @brief    Gets baud rate of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Baud rate in baud/sec.
 *******************************************************************************************************/
static CPU_INT32U USBH_USB2SER_SILABS_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                  RTOS_ERR                           *p_err)
{
  CPU_INT32U baudrate;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_BAUDRATE,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              4u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  baudrate = MEM_VAL_GET_INT32U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[0u]);

  return (baudrate);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_DataSet()
 *
 * @brief    Sets data characteristics of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
 *
 * @param    data_size           Number of data bits.
 *
 * @param    parity              Parity to use.
 *                                   - USBH_USB2SER_PARITY_NONE,     No          parity bit.
 *                                   - USBH_USB2SER_PARITY_ODD,      Set on odd  parity bit.
 *                                   - USBH_USB2SER_PARITY_EVEN,     Set on even parity bit.
 *                                   - USBH_USB2SER_PARITY_MARK,     Set         parity bit.
 *                                   - USBH_USB2SER_PARITY_SPACE     Cleared     parity bit.
 *
 * @param    stop_bits           Number of stop bits.
 *                                   - USBH_USB2SER_STOP_BITS_1,     Use 1 stop bit.
 *                                   - USBH_USB2SER_STOP_BITS_2      Use 2 stop bit.
 *                                   - USBH_USB2SER_STOP_BITS_1_5    1.5 Stop bit.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         data_size,
                                        USBH_USB2SER_PARITY                parity,
                                        USBH_USB2SER_STOP_BITS             stop_bits,
                                        RTOS_ERR                           *p_err)
{
  CPU_INT08U val_parity;
  CPU_INT08U val_stop_bits;
  CPU_INT16U val = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_SET(((data_size >= 5u)
                           && (data_size <= 8u)), *p_err, RTOS_ERR_INVALID_ARG,; );

#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  {
    USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

    if ((parity != USBH_USB2SER_PARITY_NONE)
        && (DEF_BIT_IS_SET(p_silabs_fnct->SettableParams, USBH_USB2SER_SILABS_SETTABLE_PARITY_CHECK) == DEF_NO)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
    }

    switch (data_size) {
      case 5u:
        if (DEF_BIT_IS_SET(p_silabs_fnct->SettableDataBits, USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_5) == DEF_NO) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
          return;
        }
        break;

      case 6u:
        if (DEF_BIT_IS_SET(p_silabs_fnct->SettableDataBits, USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_6) == DEF_NO) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
          return;
        }
        break;

      case 7u:
        if (DEF_BIT_IS_SET(p_silabs_fnct->SettableDataBits, USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_7) == DEF_NO) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
          return;
        }
        break;

      case 8u:
        if (DEF_BIT_IS_SET(p_silabs_fnct->SettableDataBits, USBH_USB2SER_SILABS_SETTABLE_DATA_BITS_8) == DEF_NO) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
          return;
        }
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
    }
  }
#endif

  switch (parity) {
    case USBH_USB2SER_PARITY_EVEN:
      val_parity = USBH_USB2SER_SILABS_LINE_CTL_PARITY_EVEN;
      break;

    case USBH_USB2SER_PARITY_ODD:
      val_parity = USBH_USB2SER_SILABS_LINE_CTL_PARITY_ODD;
      break;

    case USBH_USB2SER_PARITY_SPACE:
      val_parity = USBH_USB2SER_SILABS_LINE_CTL_PARITY_SPACE;
      break;

    case USBH_USB2SER_PARITY_MARK:
      val_parity = USBH_USB2SER_SILABS_LINE_CTL_PARITY_MARK;
      break;

    case USBH_USB2SER_PARITY_NONE:
    default:
      val_parity = USBH_USB2SER_SILABS_LINE_CTL_PARITY_NONE;
      break;
  }

  switch (stop_bits) {
    case USBH_USB2SER_STOP_BITS_1_5:
      val_stop_bits = USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1_5;
      break;

    case USBH_USB2SER_STOP_BITS_2:
      val_stop_bits = USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_2;
      break;

    case USBH_USB2SER_STOP_BITS_1:
    default:
      val_stop_bits = USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1;
      break;
  }

  DEF_BIT_FIELD_WR(val,
                   val_parity,
                   (CPU_INT16U)USBH_USB2SER_SILABS_LINE_CTL_PARITY_MSK);

  DEF_BIT_FIELD_WR(val,
                   val_stop_bits,
                   (CPU_INT16U)USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_MSK);

  DEF_BIT_FIELD_WR(val,
                   data_size,
                   (CPU_INT16U)USBH_USB2SER_SILABS_LINE_CTL_DATA_LEN_MSK);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_LINE_CTL,
                              val,
                              DEF_NULL,
                              0u,
                              p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_DataGet()
 *
 * @brief    Set data characteristics of the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
 *
 * @param    p_data_size         Pointer to variable that will receive the number of data bits.
 *
 * @param    p_parity            Pointer to variable that will receive the parity currently used.
 *                                   - USBH_USB2SER_PARITY_NONE,     No          parity bit.
 *                                   - USBH_USB2SER_PARITY_ODD,      Set on odd  parity bit.
 *                                   - USBH_USB2SER_PARITY_EVEN,     Set on even parity bit.
 *                                   - USBH_USB2SER_PARITY_MARK,     Set         parity bit.
 *                                   - USBH_USB2SER_PARITY_SPACE     Cleared     parity bit.
 *
 * @param    p_stop_bits         Pointer to variable that will receive the number of stop bits.
 *                                   - USBH_USB2SER_STOP_BITS_1,     Use 1 stop bit.
 *                                   - USBH_USB2SER_STOP_BITS_2      Use 2 stop bit.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_data_size,
                                        USBH_USB2SER_PARITY                *p_parity,
                                        USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                        RTOS_ERR                           *p_err)
{
  CPU_INT16U val;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_LINE_CTL,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              2u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  val = MEM_VAL_GET_INT16U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[0u]);

  if (p_data_size != DEF_NULL) {
    *p_data_size = DEF_BIT_FIELD_RD(val, USBH_USB2SER_SILABS_LINE_CTL_DATA_LEN_MSK);
  }

  if (p_parity != DEF_NULL) {
    CPU_INT08U val_parity = DEF_BIT_FIELD_RD(val, USBH_USB2SER_SILABS_LINE_CTL_PARITY_MSK);

    switch (val_parity) {
      case USBH_USB2SER_SILABS_LINE_CTL_PARITY_EVEN:
        *p_parity = USBH_USB2SER_PARITY_EVEN;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_PARITY_ODD:
        *p_parity = USBH_USB2SER_PARITY_ODD;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_PARITY_SPACE:
        *p_parity = USBH_USB2SER_PARITY_SPACE;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_PARITY_MARK:
        *p_parity = USBH_USB2SER_PARITY_MARK;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_PARITY_NONE:
      default:
        *p_parity = USBH_USB2SER_PARITY_NONE;
        break;
    }
  }

  if (p_stop_bits != DEF_NULL) {
    CPU_INT08U val_stop_bits = DEF_BIT_FIELD_RD(val, USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_MSK);

    switch (val_stop_bits) {
      case USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1_5:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_1_5;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_2:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_2;
        break;

      case USBH_USB2SER_SILABS_LINE_CTL_STOP_BITS_1:
      default:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_1;
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_BrkSignalSet()
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
static void USBH_USB2SER_SILABS_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        set,
                                             RTOS_ERR                           *p_err)
{
  CPU_INT16U               val;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_silabs_fnct->IF_Nbr != 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  val = (set == DEF_SET) ? 0x0001u : 0x0000u;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_BREAK,
                              val,
                              DEF_NULL,
                              0u,
                              p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_ModemCtrlSet()
 *
 * @brief    Set modem control register of the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
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
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        dtr_en,
                                             CPU_BOOLEAN                        dtr_set,
                                             CPU_BOOLEAN                        rts_en,
                                             CPU_BOOLEAN                        rts_set,
                                             RTOS_ERR                           *p_err)
{
  CPU_INT16U               modem_ctrl_val = DEF_BIT_NONE;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  if (dtr_en == DEF_ENABLED) {
    DEF_BIT_SET(modem_ctrl_val,
                USBH_USB2SER_SILABS_MHS_DTR_EN);

    DEF_BIT_SET(p_silabs_fnct->ModemCtrlEn,
                DEF_BIT_00);

    if (dtr_set == DEF_SET) {
      DEF_BIT_SET(modem_ctrl_val,
                  USBH_USB2SER_SILABS_MHS_DTR);
    }
  }

  if (rts_en == DEF_ENABLED) {
    DEF_BIT_SET(modem_ctrl_val,
                USBH_USB2SER_SILABS_MHS_RTS_EN);

    DEF_BIT_SET(p_silabs_fnct->ModemCtrlEn,
                DEF_BIT_01);

    if (rts_set == DEF_SET) {
      DEF_BIT_SET(modem_ctrl_val,
                  USBH_USB2SER_SILABS_MHS_RTS);
    }
  }

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_MHS,
                              modem_ctrl_val,
                              DEF_NULL,
                              0u,
                              p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_ModemCtrlGet()
 *
 * @brief    Gets modem control state of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
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
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                             CPU_BOOLEAN                        *p_dtr_en,
                                             CPU_BOOLEAN                        *p_dtr_set,
                                             CPU_BOOLEAN                        *p_rts_en,
                                             CPU_BOOLEAN                        *p_rts_set,
                                             RTOS_ERR                           *p_err)
{
  CPU_INT16U               modem_ctrl_val;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_MDMSTS,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              1u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  modem_ctrl_val = MEM_VAL_GET_INT08U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[0u]);

  if (p_dtr_en != DEF_NULL) {
    *p_dtr_en = (DEF_BIT_IS_SET(p_silabs_fnct->ModemCtrlEn, DEF_BIT_00) == DEF_YES) ? DEF_SET : DEF_CLR;
  }

  if (p_dtr_set != DEF_NULL) {
    *p_dtr_set = (DEF_BIT_IS_SET(modem_ctrl_val, DEF_BIT_00) == DEF_YES) ? DEF_SET : DEF_CLR;
  }

  if (p_rts_en != DEF_NULL) {
    *p_rts_en = (DEF_BIT_IS_SET(p_silabs_fnct->ModemCtrlEn, DEF_BIT_01) == DEF_YES) ? DEF_SET : DEF_CLR;
  }

  if (p_rts_set != DEF_NULL) {
    *p_rts_set = (DEF_BIT_IS_SET(modem_ctrl_val, DEF_BIT_01) == DEF_YES) ? DEF_SET : DEF_CLR;
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_HW_FlowCtrlSet()
 *
 * @brief    Sets hardware flow control handshaking on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *
 * @param    protocol            Protocol to use.
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS,
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_RTS,
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                               RTOS_ERR                           *p_err)
{
  CPU_INT32U ctrl_handshake;
  CPU_INT32U flow_replace;
  CPU_INT32U *p_ctrl_buf = (CPU_INT32U *)p_adapter_drv_data->AdapterCtrlBufPtr;
#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;
#endif

#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  if ((protocol != USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE)
      && (DEF_BIT_IS_SET(p_silabs_fnct->SettableParams, USBH_USB2SER_SILABS_SETTABLE_HANDSHAKE) == DEF_NO)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }
#endif

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_FLOW,
                              0u,
                              (CPU_INT08U *)p_ctrl_buf,
                              16u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  ctrl_handshake = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[0u]);
  flow_replace = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[1u]);

  switch (protocol) {
    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR:
#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
      if (DEF_BIT_IS_SET(p_silabs_fnct->Capabilities, USBH_USB2SER_SILABS_SETTABLE_FLOW_DTR_DSR) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
#endif

      DEF_BIT_FIELD_WR(ctrl_handshake,
                       USBH_USB2SER_SILABS_DTR_CTRL,
                       USBH_USB2SER_SILABS_DTR_MSK);

      DEF_BIT_SET(ctrl_handshake, USBH_USB2SER_SILABS_DSR_HANDSHAKE);
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS:
#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
      if (DEF_BIT_IS_SET(p_silabs_fnct->Capabilities, USBH_USB2SER_SILABS_SETTABLE_FLOW_RTS_CTS) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        return;
      }
#endif

      DEF_BIT_SET(ctrl_handshake, USBH_USB2SER_SILABS_CTS_HANDSHAKE);

      DEF_BIT_FIELD_WR(flow_replace,
                       USBH_USB2SER_SILABS_RTS_FLOW_CTRL,
                       USBH_USB2SER_SILABS_RTS_MSK);
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE:
    default:
      DEF_BIT_CLR(ctrl_handshake, USBH_USB2SER_SILABS_CTS_HANDSHAKE);

      DEF_BIT_FIELD_WR(flow_replace,
                       USBH_USB2SER_SILABS_RTS_INACTIVE,
                       USBH_USB2SER_SILABS_RTS_MSK);

      DEF_BIT_FIELD_WR(ctrl_handshake,
                       USBH_USB2SER_SILABS_DTR_INACTIVE,
                       USBH_USB2SER_SILABS_DTR_MSK);

      DEF_BIT_CLR(ctrl_handshake, USBH_USB2SER_SILABS_DSR_HANDSHAKE);
      break;
  }

  MEM_VAL_SET_INT32U_LITTLE(&p_ctrl_buf[0u], ctrl_handshake);
  MEM_VAL_SET_INT32U_LITTLE(&p_ctrl_buf[1u], flow_replace);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_FLOW,
                              0u,
                              (CPU_INT08U *)p_ctrl_buf,
                              16u,
                              p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_HW_FlowCtrlGet()
 *
 * @brief    Gets hardware flow control handshaking on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Current hardware flow control protocol.
 *******************************************************************************************************/
static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_SILABS_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                             RTOS_ERR                           *p_err)
{
  CPU_INT32U                         ctrl_handshake;
  CPU_INT32U                         flow_replace;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol_cur;

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_FLOW,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              16u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE);
  }

  ctrl_handshake = MEM_VAL_GET_INT32U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[0u]);
  flow_replace = MEM_VAL_GET_INT32U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[4u]);

  if ((DEF_BIT_FIELD_RD(ctrl_handshake, USBH_USB2SER_SILABS_DTR_MSK) == USBH_USB2SER_SILABS_DTR_CTRL)
      && (DEF_BIT_IS_SET(ctrl_handshake, USBH_USB2SER_SILABS_DSR_HANDSHAKE) == DEF_YES)) {
    protocol_cur = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR;
  } else if ((DEF_BIT_FIELD_RD(flow_replace, USBH_USB2SER_SILABS_RTS_MSK) == USBH_USB2SER_SILABS_RTS_FLOW_CTRL)
             && (DEF_BIT_IS_SET(ctrl_handshake, USBH_USB2SER_SILABS_CTS_HANDSHAKE) == DEF_YES)) {
    protocol_cur = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS;
  } else {
    protocol_cur = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE;
  }

  return (protocol_cur);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_SW_FlowCtrlSet()
 *
 * @brief    Set flow control handshaking for the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data..
 *
 * @param    en                  Flag that indicates if software flow control must be enabled or
 *                               disabled.
 *
 * @param    xon_char            Xon character.
 *
 * @param    xoff_char           Xoff character.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        en,
                                               CPU_INT08U                         xon_char,
                                               CPU_INT08U                         xoff_char,
                                               RTOS_ERR                           *p_err)
{
  CPU_INT16U val = 0u;
  CPU_INT32U flow_replace;
  CPU_INT32U *p_ctrl_buf = (CPU_INT32U *)p_adapter_drv_data->AdapterCtrlBufPtr;
#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;
#endif

#if (USBH_USB2SER_CFG_ADAPTER_CAPABILITIES_CHK_EN == DEF_ENABLED)
  if ((en == DEF_ENABLED)
      && ((DEF_BIT_IS_SET(p_silabs_fnct->SettableParams, USBH_USB2SER_SILABS_SETTABLE_HANDSHAKE) == DEF_NO)
          || (DEF_BIT_IS_SET(p_silabs_fnct->Capabilities, USBH_USB2SER_SILABS_SETTABLE_XON_XOFF) == DEF_NO))) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }
#endif

  //                                                               ---------------- SET XON CHARACTER -----------------
  DEF_BIT_FIELD_WR(val,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_XON,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_MSK);

  DEF_BIT_FIELD_WR(val,
                   xon_char,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_VAL_MSK);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_CHAR,
                              val,
                              DEF_NULL,
                              0u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ---------------- SET XOFF CHARACTER ----------------
  DEF_BIT_FIELD_WR(val,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_XOFF,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_MSK);

  DEF_BIT_FIELD_WR(val,
                   xoff_char,
                   (CPU_INT16U)USBH_USB2SER_SILABS_CHAR_VAL_MSK);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_CHAR,
                              val,
                              DEF_NULL,
                              0u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_FLOW,
                              0u,
                              (CPU_INT08U *)p_ctrl_buf,
                              16u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  flow_replace = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[1u]);

  if (en == DEF_ENABLED) {
    flow_replace |= (USBH_USB2SER_SILABS_AUTO_TRANSMIT_XON_XOFF
                     | USBH_USB2SER_SILABS_AUTO_RECEIVE_XON_XOFF
                     | USBH_USB2SER_SILABS_XOFF_CONTINUE);
  } else {
    flow_replace &= ~(USBH_USB2SER_SILABS_AUTO_TRANSMIT_XON_XOFF
                      | USBH_USB2SER_SILABS_AUTO_RECEIVE_XON_XOFF
                      | USBH_USB2SER_SILABS_XOFF_CONTINUE);
  }

  MEM_VAL_SET_INT32U_LITTLE(&p_ctrl_buf[1u], flow_replace);

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_YES,
                              USBH_USB2SER_SILABS_REQ_SET_FLOW,
                              0u,
                              (CPU_INT08U *)p_ctrl_buf,
                              16u,
                              p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_SW_FlowCtrlGet()
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
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_SILABS_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                      CPU_BOOLEAN                        *p_xon_char,
                                                      CPU_BOOLEAN                        *p_xoff_char,
                                                      RTOS_ERR                           *p_err)
{
  CPU_BOOLEAN en;
  CPU_INT32U  flow_replace;

  if ((p_xon_char != DEF_NULL)                                  // Retrieve xon/xoff characters.
      || (p_xoff_char != DEF_NULL)) {
    USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                                DEF_NO,
                                USBH_USB2SER_SILABS_REQ_GET_CHARS,
                                0u,
                                p_adapter_drv_data->AdapterCtrlBufPtr,
                                6u,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_DISABLED);
    }

    if (p_xon_char != DEF_NULL) {
      *p_xon_char = p_adapter_drv_data->AdapterCtrlBufPtr[4u];
    }

    if (p_xoff_char != DEF_NULL) {
      *p_xoff_char = p_adapter_drv_data->AdapterCtrlBufPtr[5u];
    }
  }

  USBH_USB2SER_SILABS_CtrlReq(p_adapter_drv_data,
                              DEF_NO,
                              USBH_USB2SER_SILABS_REQ_GET_FLOW,
                              0u,
                              p_adapter_drv_data->AdapterCtrlBufPtr,
                              16u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_DISABLED);
  }

  flow_replace = MEM_VAL_GET_INT32U_LITTLE(&p_adapter_drv_data->AdapterCtrlBufPtr[4u]);

  if ((DEF_BIT_IS_SET(flow_replace, USBH_USB2SER_SILABS_AUTO_TRANSMIT_XON_XOFF) == DEF_YES)
      && (DEF_BIT_IS_SET(flow_replace, USBH_USB2SER_SILABS_AUTO_RECEIVE_XON_XOFF) == DEF_YES)) {
    en = DEF_ENABLED;
  } else {
    en = DEF_DISABLED;
  }

  return (en);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_RxAsync()
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
static void USBH_USB2SER_SILABS_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT32U                         buf_len,
                                        RTOS_ERR                           *p_err)
{
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_silabs_fnct->RxEP_Handle == USBH_EP_HANDLE_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  USBH_EP_BulkRxAsync(p_adapter_drv_data->DevHandle,
                      p_silabs_fnct->RxEP_Handle,
                      p_buf,
                      buf_len,
                      USBH_USB2SER_SILABS_DataRxCmpl,
                      (void *)p_adapter_drv_data,
                      p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_TxAsync()
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
static void USBH_USB2SER_SILABS_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT32U                         buf_len,
                                        void                               *p_arg,
                                        RTOS_ERR                           *p_err)
{
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  USBH_EP_BulkTxAsync(p_adapter_drv_data->DevHandle,
                      p_silabs_fnct->TxEP_Handle,
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
 *                                       USBH_USB2SER_SILABS_ProbeFnct()
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
 * @return   DEF_OK,   if connected function is a Silicon Labs,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_SILABS_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                                 USBH_FNCT_HANDLE fnct_handle,
                                                 CPU_INT08U       class_code,
                                                 void             **pp_class_fnct,
                                                 RTOS_ERR         *p_err)
{
  CPU_BOOLEAN              found = DEF_NO;
  CPU_INT08U               if_nbr;
  CPU_INT16U               vendor_id;
  CPU_INT16U               product_id;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct;

  if (class_code != USBH_CLASS_CODE_VENDOR_SPECIFIC) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  }

  vendor_id = USBH_DevVendorID_Get(dev_handle,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  product_id = USBH_DevProductID_Get(dev_handle,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if ((vendor_id == USBH_USB2SER_SILABS_ID_VENDOR)              // Chk if standard Silicon Labs IDs.
      && ((product_id == USBH_USB2SER_SILABS_ID_PRODUCT_A)
          || (product_id == USBH_USB2SER_SILABS_ID_PRODUCT_B)
          || (product_id == USBH_USB2SER_SILABS_ID_PRODUCT_C))) {
    found = DEF_YES;
  } else if (USBH_USB2SER_SILABS_Ptr->AppID_TblLen > 0u) {      // If not, check if custom IDs.
    CPU_INT08U id_cnt;

    for (id_cnt = 0u; id_cnt < USBH_USB2SER_SILABS_Ptr->AppID_TblLen; id_cnt++) {
      if ((vendor_id == USBH_USB2SER_SILABS_Ptr->AppID_Tbl[id_cnt].VendorID)
          && (product_id == USBH_USB2SER_SILABS_Ptr->AppID_Tbl[id_cnt].ProductID)) {
        found = DEF_YES;
        break;
      }
    }
  }

  if (found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  }

  p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)*pp_class_fnct;
  p_silabs_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
  p_silabs_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;

  p_silabs_fnct->SerialStatus.Modem = DEF_BIT_NONE;
  p_silabs_fnct->SerialStatus.Line = DEF_BIT_NONE;

  if_nbr = USBH_IF_NbrGet(dev_handle,
                          fnct_handle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  p_silabs_fnct->IF_Nbr = if_nbr;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_EP_Open()
 *
 * @brief    Notify adapter driver that an endpoint was opened.
 *
 * @param    p_class_fnct    Pointer to internal adapter driver fnct structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_EP_Open(void           *p_class_fnct,
                                        USBH_EP_HANDLE ep_handle,
                                        CPU_INT08U     if_ix,
                                        CPU_INT08U     ep_type,
                                        CPU_BOOLEAN    ep_dir_in)
{
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_class_fnct;

  (void)&if_ix;

  if (ep_type == USBH_EP_TYPE_BULK) {
    if (ep_dir_in == DEF_YES) {
      p_silabs_fnct->RxEP_Handle = ep_handle;
    } else {
      p_silabs_fnct->TxEP_Handle = ep_handle;
    }
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_EP_Close()
 *
 * @brief    Notify adapter driver that endpoint has been closed.
 *
 * @param    p_class_fnct    Pointer to internal adapter fnct structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_EP_Close(void           *p_class_fnct,
                                         USBH_EP_HANDLE ep_handle,
                                         CPU_INT08U     if_ix)
{
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_class_fnct;

  (void)&if_ix;

  if (ep_handle == p_silabs_fnct->RxEP_Handle) {
    p_silabs_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
  } else if (ep_handle == p_silabs_fnct->TxEP_Handle) {
    p_silabs_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_CtrlReq()
 *
 * @brief    Sends Silicon Labs control request.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    host_to_dev         Falg that indicates if request is host-to-device or device-to-host.
 *
 * @param    req                 Request.
 *
 * @param    val                 Value of the wValue field.
 *
 * @param    p_buf               Pointer to data buffer.
 *
 * @param    len                 Data buffer length in bytes.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_CtrlReq(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        CPU_BOOLEAN                        host_to_dev,
                                        CPU_INT08U                         req,
                                        CPU_INT16U                         val,
                                        CPU_INT08U                         *p_buf,
                                        CPU_INT16U                         len,
                                        RTOS_ERR                           *p_err)
{
  CPU_INT08U               dir = (host_to_dev == DEF_YES) ? USBH_DEV_REQ_DIR_HOST_TO_DEV : USBH_DEV_REQ_DIR_DEV_TO_HOST;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;
  CPU_INT32U               std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         req,
                         (dir | USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_RECIPIENT_IF),
                         val,
                         p_silabs_fnct->IF_Nbr,
                         p_buf,
                         len,
                         len,
                         std_req_timeout,
                         p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_StatusLineUpdate()
 *
 * @brief    Parses and update new line status.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    status_byte         Bitmap that contains line status.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_StatusLineUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_INT08U                         status_byte)
{
  CPU_INT08U               status_line = DEF_BIT_NONE;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  status_line |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_LINE_STATUS_PARITY_ERR)  ? USBH_USB2SER_LINE_STATUS_PARITY_ERR      : DEF_BIT_NONE);
  status_line |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_LINE_STATUS_FRAMING_ERR) ? USBH_USB2SER_LINE_STATUS_FRAMING_ERR     : DEF_BIT_NONE);
  status_line |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_LINE_STATUS_BRK)         ? USBH_USB2SER_LINE_STATUS_BRK_INT         : DEF_BIT_NONE);
  status_line |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_LINE_STATUS_HW_OVERRUN)  ? USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR : DEF_BIT_NONE);

  p_silabs_fnct->SerialStatus.Line = status_line;

  USBH_USB2SER_FnctStatusUpdate(p_adapter_drv_data,
                                p_silabs_fnct->SerialStatus);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_SILABS_StatusModemUpdate()
 *
 * @brief    Parses and update new modem status.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    status_byte         Bitmap that contains modem status.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_StatusModemUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                  CPU_INT08U                         status_byte)
{
  CPU_INT08U               status_modem = DEF_BIT_NONE;
  USBH_USB2SER_SILABS_FNCT *p_silabs_fnct = (USBH_USB2SER_SILABS_FNCT *)p_adapter_drv_data->DataPtr;

  status_modem |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_MODEM_STATUS_CTS) ? USBH_USB2SER_MODEM_STATUS_CTS     : DEF_BIT_NONE);
  status_modem |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_MODEM_STATUS_DSR) ? USBH_USB2SER_MODEM_STATUS_DSR     : DEF_BIT_NONE);
  status_modem |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_MODEM_STATUS_RI)  ? USBH_USB2SER_MODEM_STATUS_RING    : DEF_BIT_NONE);
  status_modem |= (DEF_BIT_IS_SET(status_byte, USBH_USB2SER_SILABS_MODEM_STATUS_DCD) ? USBH_USB2SER_MODEM_STATUS_CARRIER : DEF_BIT_NONE);

  p_silabs_fnct->SerialStatus.Modem = status_modem;

  USBH_USB2SER_FnctStatusUpdate(p_adapter_drv_data,
                                p_silabs_fnct->SerialStatus);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SILABS_DataRxCmpl()
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
 * @note     (1) The received data may contain embedded serial status anywhere in the data. The entire
 *               buffer must be parsed.
 *******************************************************************************************************/
static void USBH_USB2SER_SILABS_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
                                           USBH_EP_HANDLE  ep_handle,
                                           CPU_INT08U      *p_buf,
                                           CPU_INT32U      buf_len,
                                           CPU_INT32U      xfer_len,
                                           void            *p_arg,
                                           RTOS_ERR        err)
{
  CPU_INT32U                         cnt;
  CPU_INT32U                         rx_len = xfer_len;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data = (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *)p_arg;

  (void)&dev_handle;
  (void)&ep_handle;

  //                                                               Find embedded serial event(s) if any.
  cnt = 0u;
  while (cnt < rx_len) {
    CPU_INT08U ctrl_char;
    CPU_INT08U status;

    if (p_buf[cnt] != USBH_USB2SER_SILABS_SER_EVENTS_ESC_CHAR) {
      cnt++;
      continue;
    }

    ctrl_char = p_buf[cnt + 1u];
    switch (ctrl_char) {
      case USBH_USB2SER_EVENT_CTRL_CHAR_NONE:                   // An escape char was present in rxd data. Remove it.
        Mem_Move(&p_buf[cnt + 1u],
                 &p_buf[cnt + 2u],
                 rx_len - cnt - 1u);

        rx_len -= 1u;
        cnt++;
        break;

      case USBH_USB2SER_EVENT_CTRL_CHAR_LINE_DATA:              // Line or modem status event.
      case USBH_USB2SER_EVENT_CTRL_CHAR_LINE:
      case USBH_USB2SER_EVENT_CTRL_CHAR_MODEM:
        status = p_buf[cnt + 2u];

        Mem_Move(&p_buf[cnt],
                 &p_buf[cnt + 3u],
                 rx_len - cnt - 3u);

        rx_len -= 3u;

        if (ctrl_char == USBH_USB2SER_EVENT_CTRL_CHAR_MODEM) {
          USBH_USB2SER_SILABS_StatusModemUpdate(p_adapter_drv_data,
                                                status);
        } else {
          USBH_USB2SER_SILABS_StatusLineUpdate(p_adapter_drv_data,
                                               status);
        }
        break;

      default:
        cnt++;
        break;
    }
  }

  USBH_USB2SER_FnctAsyncRxCmpl(p_adapter_drv_data,
                               p_buf,
                               p_buf,
                               buf_len,
                               rx_len,
                               err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_USB2SER_SILABS_AVAIL))
