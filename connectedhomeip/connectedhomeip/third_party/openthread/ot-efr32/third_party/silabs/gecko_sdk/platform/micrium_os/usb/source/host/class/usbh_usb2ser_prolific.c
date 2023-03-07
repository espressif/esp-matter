/***************************************************************************//**
 * @file
 * @brief USB Host - Prolific Technology Pl2303 Serial Adpter Driver
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_PROLIFIC_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))

#error USB HOST USB2SER Prolific adapter driver requires USB Host USB2SER class. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_USB2SER_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_USB2SER_PROLIFIC_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_usb2ser.h>
#include  <usb/include/host/usbh_usb2ser_prolific.h>
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

#define  LOG_DFLT_CH                                    (USBH, CLASS, USB2SER, PROLIFIC)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_USB2SER_PROLIFIC_FS_MAX_XFER_LEN                      64u
#define  USBH_USB2SER_PROLIFIC_CTRL_LEN                              7u

/********************************************************************************************************
 *                                       PROLIFIC TECHNOLOGY VENDOR ID
 *******************************************************************************************************/

#define  USBH_USB2SER_PROLIFIC_ID_VENDOR                        0x067Bu

/********************************************************************************************************
 *                                   PROLIFIC TECHNOLOGY PL2303 PRODUCT ID
 *******************************************************************************************************/

#define  USBH_USB2SER_PROLIFIC_ID_PRODUCT                       0x2303u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *******************************************************************************************************/

#define  USBH_USB2SER_PROLIFIC_REQ_SET_LINE                       0x20u
#define  USBH_USB2SER_PROLIFIC_REQ_GET_LINE                       0x21u
#define  USBH_USB2SER_PROLIFIC_REQ_SET_CONTROL                    0x22u
#define  USBH_USB2SER_PROLIFIC_REQ_BREAK                          0x23u
#define  USBH_USB2SER_PROLIFIC_REQ_VENDOR                         0x01u

//                                                                 Set line request parity values.
#define  USBH_USB2SER_PROLIFIC_LINE_PARITY_NONE                      0u
#define  USBH_USB2SER_PROLIFIC_LINE_PARITY_ODD                       1u
#define  USBH_USB2SER_PROLIFIC_LINE_PARITY_EVEN                      2u
#define  USBH_USB2SER_PROLIFIC_LINE_PARITY_MARK                      3u
#define  USBH_USB2SER_PROLIFIC_LINE_PARITY_SPACE                     4u

//                                                                 Set line request stop bit values.
#define  USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1                      0u
#define  USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1_5                    1u
#define  USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_2                      2u

//                                                                 Set control request modem state.
#define  USBH_USB2SER_PROLIFIC_CONTROL_DTR                      DEF_BIT_00
#define  USBH_USB2SER_PROLIFIC_CONTROL_RTS                      DEF_BIT_01
//                                                                 Following defs are drv specific.
#define  USBH_USB2SER_PROLIFIC_CONTROL_DTR_EN                   DEF_BIT_02
#define  USBH_USB2SER_PROLIFIC_CONTROL_RTS_EN                   DEF_BIT_03

//                                                                 Set break request values.
#define  USBH_USB2SER_PROLIFIC_BREAK_SET                        0xFFFFu
#define  USBH_USB2SER_PROLIFIC_BREAK_CLR                        0x0000u

//                                                                 Vendor sub-requests.
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL                 0x0000u
#define  USBH_USB2SER_PROLIFIC_VENDOR_PURGE_TX                  0x0008u
#define  USBH_USB2SER_PROLIFIC_VENDOR_PURGE_RX                  0x0009u
#define  USBH_USB2SER_PROLIFIC_VENDOR_START                     0x0404u
#define  USBH_USB2SER_PROLIFIC_VENDOR_XONXOFF_CHAR              0x0505u
//                                                                 Vendor read sub-requests mask.
#define  USBH_USB2SER_PROLIFIC_VENDOR_RD_MSK                    0x8080u

//                                                                 Flow ctrl vendor sub-request values.
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_SET             DEF_BIT_00
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN              DEF_BIT_06
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_DTR_RTS         DEF_BIT_03
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_RTS_CTS         DEF_BIT_05
#define  USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_XON_XOFF        DEF_BIT_07

/********************************************************************************************************
 *                                           PROLIFIC SERIAL STATUS
 *******************************************************************************************************/

#define  USBH_USB2SER_PROLIFIC_STATUS_LEN                           10u
#define  USBH_USB2SER_PROLIFIC_STATUS_IX                             8u

#define  USBH_USB2SER_PROLIFIC_STATUS_CTS                       DEF_BIT_07
#define  USBH_USB2SER_PROLIFIC_STATUS_DSR                       DEF_BIT_01
#define  USBH_USB2SER_PROLIFIC_STATUS_RING                      DEF_BIT_03

#define  USBH_USB2SER_PROLIFIC_STATUS_OVERRUN_ERR               DEF_BIT_06
#define  USBH_USB2SER_PROLIFIC_STATUS_PARITY_ERR                DEF_BIT_05
#define  USBH_USB2SER_PROLIFIC_STATUS_FRAME_ERR                 DEF_BIT_04
#define  USBH_USB2SER_PROLIFIC_STATUS_BREAK_ERR                 DEF_BIT_02

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               PROLIFIC ADAPTER DRIVER FUNCTION STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_prolific_fnct {
  USBH_EP_HANDLE RxEP_Handle;                                   // Handle to rx EP.
  USBH_EP_HANDLE TxEP_Handle;                                   // Handle to tx EP.
#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  USBH_EP_HANDLE StatusEP_Handle;                               // Handle to status (interrupt) EP.
#endif

  CPU_INT08U     CurModemCtrl;                                  // Backup of current modem control.
  CPU_INT16U     XonXoffChar;                                   // Backup of current Xon/Xoff characters.
} USBH_USB2SER_PROLIFIC_FNCT;

/********************************************************************************************************
 *                                       PROLIFIC CLASS DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_prolific {
  USBH_USB2SER_APP_ID *AppID_Tbl;                               // Tbl of app-specific Prolific devices.
  CPU_INT08U          AppID_TblLen;                             // Len of tbl of app-specific Prolific devices.
} USBH_USB2SER_PROLIFIC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_USB2SER_PROLIFIC *USBH_USB2SER_PROLIFIC_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------- ADAPTER DRIVER FUNCTIONS -------------
static void USBH_USB2SER_PROLIFIC_Init(MEM_SEG             *p_mem_seg,
                                       USBH_USB2SER_APP_ID *id_tbl,
                                       CPU_INT08U          id_tbl_len,
                                       RTOS_ERR            *p_err);

static void USBH_USB2SER_PROLIFIC_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data);

static CPU_INT08U USBH_USB2SER_PROLIFIC_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                   RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        USBH_USB2SER_RESET_SEL             sel,
                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                              CPU_INT32U                         baudrate,
                                              RTOS_ERR                           *p_err);

static CPU_INT32U USBH_USB2SER_PROLIFIC_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                    RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         data_size,
                                          USBH_USB2SER_PARITY                parity,
                                          USBH_USB2SER_STOP_BITS             stop_bits,
                                          RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_data_size,
                                          USBH_USB2SER_PARITY                *p_parity,
                                          USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                          RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        set,
                                               RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        dtr_en,
                                               CPU_BOOLEAN                        dtr_set,
                                               CPU_BOOLEAN                        rts_en,
                                               CPU_BOOLEAN                        rts_set,
                                               RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        *p_dtr_en,
                                               CPU_BOOLEAN                        *p_dtr_set,
                                               CPU_BOOLEAN                        *p_rts_en,
                                               CPU_BOOLEAN                        *p_rts_set,
                                               RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                                 RTOS_ERR                           *p_err);

static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_PROLIFIC_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                               RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_BOOLEAN                        en,
                                                 CPU_INT08U                         xon_char,
                                                 CPU_INT08U                         xoff_char,
                                                 RTOS_ERR                           *p_err);

static CPU_BOOLEAN USBH_USB2SER_PROLIFIC_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                        CPU_INT08U                         *p_xon_char,
                                                        CPU_INT08U                         *p_xoff_char,
                                                        RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_buf,
                                          CPU_INT32U                         buf_len,
                                          RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_buf,
                                          CPU_INT32U                         buf_len,
                                          void                               *p_arg,
                                          RTOS_ERR                           *p_err);

//                                                                 -------------- CLASS DRIVER FUNCTIONS --------------
static CPU_BOOLEAN USBH_USB2SER_PROLIFIC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                                   USBH_FNCT_HANDLE fnct_handle,
                                                   CPU_INT08U       class_code,
                                                   void             **pp_class_fnct,
                                                   RTOS_ERR         *p_err);

static void USBH_USB2SER_PROLIFIC_EP_Open(void           *p_class_fnct,
                                          USBH_EP_HANDLE ep_handle,
                                          CPU_INT08U     if_ix,
                                          CPU_INT08U     ep_type,
                                          CPU_BOOLEAN    ep_dir_in);

static void USBH_USB2SER_PROLIFIC_EP_Close(void           *p_class_fnct,
                                           USBH_EP_HANDLE ep_handle,
                                           CPU_INT08U     if_ix);

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static void USBH_USB2SER_PROLIFIC_VendorWr(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_INT16U                         val,
                                           CPU_INT16U                         ix,
                                           RTOS_ERR                           *p_err);

static CPU_INT16U USBH_USB2SER_PROLIFIC_VendorRd(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_INT16U                         val,
                                                 RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_LineSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_BOOLEAN                        update_baudrate,
                                          CPU_INT32U                         baudrate,
                                          CPU_INT08U                         data_bits,
                                          CPU_INT08U                         parity,
                                          CPU_INT08U                         stop_bits,
                                          RTOS_ERR                           *p_err);

static void USBH_USB2SER_PROLIFIC_LineGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT32U                         *p_baudrate,
                                          CPU_INT08U                         *p_data_bits,
                                          CPU_INT08U                         *p_parity,
                                          CPU_INT08U                         *p_stop_bits,
                                          RTOS_ERR                           *p_err);

#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
static void USBH_USB2SER_PROLIFIC_StatusRxCmpl(USBH_DEV_HANDLE dev_handle,
                                               USBH_EP_HANDLE  ep_handle,
                                               CPU_INT08U      *p_buf,
                                               CPU_INT32U      buf_len,
                                               CPU_INT32U      xfer_len,
                                               void            *p_arg,
                                               RTOS_ERR        err);
#endif

static void USBH_USB2SER_PROLIFIC_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
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
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN != DEF_ENABLED))
#error  "USBH_CFG_PERIODIC_XFER_EN   illegally #define'd in 'usbh_cfg.h', MUST be set to DEF_ENABLED if USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN is DEF_ENABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       PROLIFIC DRIVER STRUCTURES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------- PROLIFIC DEVICE ADAPTER DRIVER ----------
const USBH_USB2SER_DEV_ADAPTER_DRV USBH_USB2SER_PROLIFIC_DevAdapterDrv = {
  USBH_USB2SER_PROLIFIC_Init,
  USBH_USB2SER_PROLIFIC_Conn,
  USBH_USB2SER_PROLIFIC_PortNbrGet,
  USBH_USB2SER_PROLIFIC_Reset,
  USBH_USB2SER_PROLIFIC_BaudRateSet,
  USBH_USB2SER_PROLIFIC_BaudRateGet,
  USBH_USB2SER_PROLIFIC_DataSet,
  USBH_USB2SER_PROLIFIC_DataGet,
  USBH_USB2SER_PROLIFIC_BrkSignalSet,
  USBH_USB2SER_PROLIFIC_ModemCtrlSet,
  USBH_USB2SER_PROLIFIC_ModemCtrlGet,
  USBH_USB2SER_PROLIFIC_HW_FlowCtrlSet,
  USBH_USB2SER_PROLIFIC_HW_FlowCtrlGet,
  USBH_USB2SER_PROLIFIC_SW_FlowCtrlSet,
  USBH_USB2SER_PROLIFIC_SW_FlowCtrlGet,
  USBH_USB2SER_PROLIFIC_RxAsync,
  USBH_USB2SER_PROLIFIC_TxAsync,

  USBH_USB2SER_PROLIFIC_CTRL_LEN,
  USBH_USB2SER_PROLIFIC_STATUS_LEN,
  USBH_USB2SER_PROLIFIC_FS_MAX_XFER_LEN,
  0u,                                                           // No HS support.
  sizeof(USBH_USB2SER_PROLIFIC_FNCT)
};

//                                                                 ---------- PROLIFIC ADAPTER CLASS DRIVER -----------
static USBH_CLASS_DRV USBH_USB2SER_PROLIFIC_ClassDrv = {
  DEF_NULL,
  USBH_USB2SER_PROLIFIC_ProbeFnct,
  DEF_NULL,
  USBH_USB2SER_PROLIFIC_EP_Open,
  USBH_USB2SER_PROLIFIC_EP_Close,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  DEF_NULL,
#endif
  (CPU_CHAR *)"USB2SER - Prolific",
};

//                                                                 ------------- PROLIFIC ADAPTER DRIVER --------------
const USBH_USB2SER_ADAPTER_DRV USBH_USB2SER_PROLIFIC_Drv = {
  &USBH_USB2SER_PROLIFIC_ClassDrv,
  (USBH_USB2SER_DEV_ADAPTER_DRV *)&USBH_USB2SER_PROLIFIC_DevAdapterDrv
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_Init()
 *
 * @brief    Initializes the Prolific device adapter driver.
 *
 * @param    p_mem_seg   Pointer to memory segment from which Prolific data will be allocated.
 *                       Data will be allocated from global heap if null.
 *
 * @param    id_tbl      Table containing list of vendor/product IDs that should be considered
 *                       Prolific serial devices. Can be DEF_NULL. See Note #1.
 *
 * @param    id_tlb_len  Length of ID table.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) It is possible to modify the default Vendor and/or Product IDs of the Prolific chips.
 *               The purpose of the ID table that can be passed at initialization is to allow a
 *               customer to add any Vendor/Product ID combination as Prolific serial devices.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_Init(MEM_SEG             *p_mem_seg,
                                       USBH_USB2SER_APP_ID *id_tbl,
                                       CPU_INT08U          id_tbl_len,
                                       RTOS_ERR            *p_err)
{
  if (USBH_USB2SER_PROLIFIC_Ptr != DEF_NULL) {                  // Chk if Prolific adapter driver already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  USBH_USB2SER_PROLIFIC_Ptr = (USBH_USB2SER_PROLIFIC *)Mem_SegAlloc("USBH - SILABS Data",
                                                                    p_mem_seg,
                                                                    sizeof(USBH_USB2SER_PROLIFIC),
                                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_PROLIFIC_Ptr->AppID_Tbl = id_tbl;
  USBH_USB2SER_PROLIFIC_Ptr->AppID_TblLen = id_tbl_len;
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_Conn()
 *
 * @brief    Initiates communication with Prolific adapter.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_Conn(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data)
{
  RTOS_ERR                   err_usbh;
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  p_prolific_fnct->CurModemCtrl = DEF_BIT_NONE;
  p_prolific_fnct->XonXoffChar = (USBH_USB2SER_SW_FLOW_CTRL_XOFF_CHAR_DFLT << 8u)
                                 | USBH_USB2SER_SW_FLOW_CTRL_XON_CHAR_DFLT;

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,            // Start sequence.
                                 USBH_USB2SER_PROLIFIC_VENDOR_START,
                                 0u,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Req 0x0404, 0 -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 USBH_USB2SER_PROLIFIC_VENDOR_START,
                                 1u,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Req 0x0404, 1 -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                 USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_SET,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Req 0, 1 -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 1u,
                                 0u,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Req 1, 0 -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 2u,
                                 0x44u,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Req 2, 0x44 -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 USBH_USB2SER_PROLIFIC_VENDOR_XONXOFF_CHAR,
                                 p_prolific_fnct->XonXoffChar,
                                 &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Setting dflt Xon Xoff char -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

    #if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  //                                                               Start reception of serial status.
  USBH_EP_IntrRxAsync(p_adapter_drv_data->DevHandle,
                      p_prolific_fnct->StatusEP_Handle,
                      p_adapter_drv_data->AdapterStatusBufPtr,
                      USBH_USB2SER_PROLIFIC_STATUS_LEN,
                      USBH_USB2SER_PROLIFIC_StatusRxCmpl,
                      (void *)p_adapter_drv_data,
                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Submit status buf -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }
    #endif
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_PortNbrGet()
 *
 * @brief    Retrieves adapter's port number.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static CPU_INT08U USBH_USB2SER_PROLIFIC_PortNbrGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                   RTOS_ERR                           *p_err)
{
  (void)&p_adapter_drv_data;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (1u);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_Reset()
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
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_Reset(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                        USBH_USB2SER_RESET_SEL             sel,
                                        RTOS_ERR                           *p_err)
{
  if ((sel == USBH_USB2SER_RESET_SEL_RX)
      || (sel == USBH_USB2SER_RESET_SEL_ALL)) {
    USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                   USBH_USB2SER_PROLIFIC_VENDOR_PURGE_RX,
                                   0u,
                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  if ((sel == USBH_USB2SER_RESET_SEL_TX)
      || (sel == USBH_USB2SER_RESET_SEL_ALL)) {
    USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                   USBH_USB2SER_PROLIFIC_VENDOR_PURGE_TX,
                                   0u,
                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_BaudRateSet()
 *
 * @brief    Sets baud rate of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    baudrate            Baud rate to set (in baud/sec). Can be any value supported by Prolific
 *                               chip.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_BaudRateSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                              CPU_INT32U                         baudrate,
                                              RTOS_ERR                           *p_err)
{
  USBH_USB2SER_PROLIFIC_LineSet(p_adapter_drv_data,
                                DEF_YES,
                                baudrate,
                                0u,
                                0u,
                                0u,
                                p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_BaudRateGet()
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
static CPU_INT32U USBH_USB2SER_PROLIFIC_BaudRateGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                    RTOS_ERR                           *p_err)
{
  CPU_INT32U baudrate = 0u;

  USBH_USB2SER_PROLIFIC_LineGet(p_adapter_drv_data,
                                &baudrate,
                                DEF_NULL,
                                DEF_NULL,
                                DEF_NULL,
                                p_err);

  return (baudrate);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_DataSet()
 *
 * @brief    Sets data characteristics of communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
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
static void USBH_USB2SER_PROLIFIC_DataSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         data_size,
                                          USBH_USB2SER_PARITY                parity,
                                          USBH_USB2SER_STOP_BITS             stop_bits,
                                          RTOS_ERR                           *p_err)
{
  CPU_INT08U parity_val;
  CPU_INT08U stop_bits_val;

  RTOS_ASSERT_DBG_ERR_SET(((data_size >= 5u)
                           && (data_size <= 8u)), *p_err, RTOS_ERR_INVALID_ARG,; );

  switch (parity) {
    case USBH_USB2SER_PARITY_EVEN:
      parity_val = USBH_USB2SER_PROLIFIC_LINE_PARITY_EVEN;
      break;

    case USBH_USB2SER_PARITY_ODD:
      parity_val = USBH_USB2SER_PROLIFIC_LINE_PARITY_ODD;
      break;

    case USBH_USB2SER_PARITY_SPACE:
      parity_val = USBH_USB2SER_PROLIFIC_LINE_PARITY_SPACE;
      break;

    case USBH_USB2SER_PARITY_MARK:
      parity_val = USBH_USB2SER_PROLIFIC_LINE_PARITY_MARK;
      break;

    case USBH_USB2SER_PARITY_NONE:
    default:
      parity_val = USBH_USB2SER_PROLIFIC_LINE_PARITY_NONE;
      break;
  }

  switch (stop_bits) {
    case USBH_USB2SER_STOP_BITS_1_5:
      stop_bits_val = USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1_5;
      break;

    case USBH_USB2SER_STOP_BITS_2:
      stop_bits_val = USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_2;
      break;

    case USBH_USB2SER_STOP_BITS_1:
    default:
      stop_bits_val = USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1;
      break;
  }

  USBH_USB2SER_PROLIFIC_LineSet(p_adapter_drv_data,
                                DEF_NO,
                                0u,
                                data_size,
                                parity_val,
                                stop_bits_val,
                                p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_DataGet()
 *
 * @brief    Set data characteristics of the communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
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
static void USBH_USB2SER_PROLIFIC_DataGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_data_size,
                                          USBH_USB2SER_PARITY                *p_parity,
                                          USBH_USB2SER_STOP_BITS             *p_stop_bits,
                                          RTOS_ERR                           *p_err)
{
  CPU_INT08U parity_val = USBH_USB2SER_PARITY_NONE;
  CPU_INT08U stop_bits_val = USBH_USB2SER_STOP_BITS_1;

  USBH_USB2SER_PROLIFIC_LineGet(p_adapter_drv_data,
                                DEF_NULL,
                                p_data_size,
                                &parity_val,
                                &stop_bits_val,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_parity != DEF_NULL) {
    switch (parity_val) {
      case USBH_USB2SER_PROLIFIC_LINE_PARITY_EVEN:
        *p_parity = USBH_USB2SER_PARITY_EVEN;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_PARITY_ODD:
        *p_parity = USBH_USB2SER_PARITY_ODD;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_PARITY_SPACE:
        *p_parity = USBH_USB2SER_PARITY_SPACE;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_PARITY_MARK:
        *p_parity = USBH_USB2SER_PARITY_MARK;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_PARITY_NONE:
      default:
        *p_parity = USBH_USB2SER_PARITY_NONE;
        break;
    }
  }

  if (p_stop_bits != DEF_NULL) {
    switch (stop_bits_val) {
      case USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1_5:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_1_5;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_2:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_2;
        break;

      case USBH_USB2SER_PROLIFIC_LINE_STOP_BITS_1:
      default:
        *p_stop_bits = USBH_USB2SER_STOP_BITS_1;
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_BrkSignalSet()
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
static void USBH_USB2SER_PROLIFIC_BrkSignalSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        set,
                                               RTOS_ERR                           *p_err)
{
  CPU_INT16U val;
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  val = (set == DEF_SET) ? USBH_USB2SER_PROLIFIC_BREAK_SET : USBH_USB2SER_PROLIFIC_BREAK_CLR;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_BREAK,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         val,
                         0u,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_ModemCtrlSet()
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
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_ModemCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        dtr_en,
                                               CPU_BOOLEAN                        dtr_set,
                                               CPU_BOOLEAN                        rts_en,
                                               CPU_BOOLEAN                        rts_set,
                                               RTOS_ERR                           *p_err)
{
  CPU_INT16U                 val;
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;
  CPU_INT32U                 std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  val = ((dtr_en == DEF_ENABLED) && (dtr_set == DEF_SET)) ? USBH_USB2SER_PROLIFIC_CONTROL_DTR : DEF_BIT_NONE;
  val |= ((rts_en == DEF_ENABLED) && (rts_set == DEF_SET)) ? USBH_USB2SER_PROLIFIC_CONTROL_RTS : DEF_BIT_NONE;

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_SET_CONTROL,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         val,
                         0u,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_prolific_fnct->CurModemCtrl = val;
    p_prolific_fnct->CurModemCtrl |= (dtr_en == DEF_ENABLED) ? USBH_USB2SER_PROLIFIC_CONTROL_DTR_EN : DEF_BIT_NONE;
    p_prolific_fnct->CurModemCtrl |= (rts_en == DEF_ENABLED) ? USBH_USB2SER_PROLIFIC_CONTROL_RTS_EN : DEF_BIT_NONE;
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_ModemCtrlGet()
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
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_ModemCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                               CPU_BOOLEAN                        *p_dtr_en,
                                               CPU_BOOLEAN                        *p_dtr_set,
                                               CPU_BOOLEAN                        *p_rts_en,
                                               CPU_BOOLEAN                        *p_rts_set,
                                               RTOS_ERR                           *p_err)
{
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_dtr_en != DEF_NULL) {
    *p_dtr_en = (DEF_BIT_IS_SET(p_prolific_fnct->CurModemCtrl, USBH_USB2SER_PROLIFIC_CONTROL_DTR_EN) == DEF_YES) ? DEF_ENABLED : DEF_DISABLED;
  }

  if (p_dtr_set != DEF_NULL) {
    *p_dtr_set = (DEF_BIT_IS_SET(p_prolific_fnct->CurModemCtrl, USBH_USB2SER_PROLIFIC_CONTROL_DTR) == DEF_YES) ? DEF_SET : DEF_CLR;
  }

  if (p_rts_en != DEF_NULL) {
    *p_rts_en = (DEF_BIT_IS_SET(p_prolific_fnct->CurModemCtrl, USBH_USB2SER_PROLIFIC_CONTROL_RTS_EN) == DEF_YES) ? DEF_ENABLED : DEF_DISABLED;
  }

  if (p_rts_set != DEF_NULL) {
    *p_rts_set = (DEF_BIT_IS_SET(p_prolific_fnct->CurModemCtrl, USBH_USB2SER_PROLIFIC_CONTROL_RTS) == DEF_YES) ? DEF_SET : DEF_CLR;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_HW_FlowCtrlSet()
 *
 * @brief    Sets hardware flow control handshaking on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    protocol            Protocol to use.
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS,
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_RTS,
 *                                   - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_HW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                                 RTOS_ERR                           *p_err)
{
  CPU_INT16U ix;

  ix = USBH_USB2SER_PROLIFIC_VendorRd(p_adapter_drv_data,
                                      USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  switch (protocol) {
    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR:
      ix |= (USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN
             | USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_DTR_RTS);
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS:
      ix |= (USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN
             | USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_RTS_CTS);
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE:
    default:
      DEF_BIT_CLR(ix, (CPU_INT16U)USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_DTR_RTS);
      DEF_BIT_CLR(ix, (CPU_INT16U)USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_RTS_CTS);

      if (DEF_BIT_IS_CLR(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_XON_XOFF) == DEF_YES) {
        DEF_BIT_CLR(ix, (CPU_INT16U)USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN);
      }
      break;
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                 ix,
                                 p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_HW_FlowCtrlGet()
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
static USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_PROLIFIC_HW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                                               RTOS_ERR                           *p_err)
{
  CPU_INT16U                         ix;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol;

  ix = USBH_USB2SER_PROLIFIC_VendorRd(p_adapter_drv_data,
                                      USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE);
  }

  if (DEF_BIT_IS_SET(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_RTS_CTS) == DEF_YES) {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS;
  } else if (DEF_BIT_IS_SET(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_DTR_RTS) == DEF_YES) {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR;
  } else {
    protocol = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE;
  }

  return (protocol);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_SW_FlowCtrlSet()
 *
 * @brief    Sets/clears software flow control on communication port.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
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
static void USBH_USB2SER_PROLIFIC_SW_FlowCtrlSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_BOOLEAN                        en,
                                                 CPU_INT08U                         xon_char,
                                                 CPU_INT08U                         xoff_char,
                                                 RTOS_ERR                           *p_err)
{
  CPU_INT16U                 ix;
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  //                                                               ------------- SET XON XOFF CHARACTERS --------------
  ix = (xoff_char << 8u) | xon_char;

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,
                                 USBH_USB2SER_PROLIFIC_VENDOR_XONXOFF_CHAR,
                                 ix,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_prolific_fnct->XonXoffChar = ix;

  //                                                               --------------- SET/CLR SW FLOW CTRL ---------------
  ix = USBH_USB2SER_PROLIFIC_VendorRd(p_adapter_drv_data,
                                      USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (en == DEF_ENABLED) {
    ix |= (USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN
           | USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_XON_XOFF);
  } else {
    DEF_BIT_CLR(ix, (CPU_INT16U)USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_XON_XOFF);

    if ((DEF_BIT_IS_CLR(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_RTS_CTS) == DEF_YES)
        && (DEF_BIT_IS_CLR(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_DTR_RTS) == DEF_YES)) {
      DEF_BIT_CLR(ix, (CPU_INT16U)USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_EN);
    }
  }

  USBH_USB2SER_PROLIFIC_VendorWr(p_adapter_drv_data,            // Set flow control.
                                 USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                 ix,
                                 p_err);
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_SW_FlowCtrlGet()
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
static CPU_BOOLEAN USBH_USB2SER_PROLIFIC_SW_FlowCtrlGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                        CPU_INT08U                         *p_xon_char,
                                                        CPU_INT08U                         *p_xoff_char,
                                                        RTOS_ERR                           *p_err)
{
  CPU_BOOLEAN                en;
  CPU_INT16U                 ix;
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  ix = USBH_USB2SER_PROLIFIC_VendorRd(p_adapter_drv_data,
                                      USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_DISABLED);
  }

  if (DEF_BIT_IS_SET(ix, USBH_USB2SER_PROLIFIC_VENDOR_FLOW_CTRL_XON_XOFF) == DEF_YES) {
    en = DEF_ENABLED;
  } else {
    en = DEF_DISABLED;
  }

  if (p_xon_char != DEF_NULL) {
    *p_xon_char = (CPU_INT08U)(p_prolific_fnct->XonXoffChar & 0x00FFu);
  }

  if (p_xoff_char != DEF_NULL) {
    *p_xoff_char = (CPU_INT08U)(p_prolific_fnct->XonXoffChar >> 8u);
  }

  return (en);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_RxAsync()
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
static void USBH_USB2SER_PROLIFIC_RxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_buf,
                                          CPU_INT32U                         buf_len,
                                          RTOS_ERR                           *p_err)
{
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  if (p_prolific_fnct->RxEP_Handle == USBH_EP_HANDLE_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  USBH_EP_BulkRxAsync(p_adapter_drv_data->DevHandle,
                      p_prolific_fnct->RxEP_Handle,
                      p_buf,
                      buf_len,
                      USBH_USB2SER_PROLIFIC_DataRxCmpl,
                      p_adapter_drv_data,
                      p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_TxAsync()
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
static void USBH_USB2SER_PROLIFIC_TxAsync(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT08U                         *p_buf,
                                          CPU_INT32U                         buf_len,
                                          void                               *p_arg,
                                          RTOS_ERR                           *p_err)
{
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;

  USBH_EP_BulkTxAsync(p_adapter_drv_data->DevHandle,
                      p_prolific_fnct->TxEP_Handle,
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
 *                                       USBH_USB2SER_PROLIFIC_ProbeFnct()
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
 * @return   DEF_OK,   if connected function is a Prolific,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_PROLIFIC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                                   USBH_FNCT_HANDLE fnct_handle,
                                                   CPU_INT08U       class_code,
                                                   void             **pp_class_fnct,
                                                   RTOS_ERR         *p_err)
{
  CPU_BOOLEAN                found = DEF_NO;
  CPU_INT16U                 vendor_id;
  CPU_INT16U                 product_id;
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)*pp_class_fnct;

  PP_UNUSED_PARAM(fnct_handle);
  PP_UNUSED_PARAM(class_code);

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

  if ((vendor_id == USBH_USB2SER_PROLIFIC_ID_VENDOR)            // Chk if standard Prolific IDs.
      && (product_id == USBH_USB2SER_PROLIFIC_ID_PRODUCT)) {
    found = DEF_YES;
  } else if (USBH_USB2SER_PROLIFIC_Ptr->AppID_TblLen > 0u) {    // If not, check if custom IDs.
    CPU_INT08U id_cnt;

    for (id_cnt = 0u; id_cnt < USBH_USB2SER_PROLIFIC_Ptr->AppID_TblLen; id_cnt++) {
      if ((vendor_id == USBH_USB2SER_PROLIFIC_Ptr->AppID_Tbl[id_cnt].VendorID)
          && (product_id == USBH_USB2SER_PROLIFIC_Ptr->AppID_Tbl[id_cnt].ProductID)) {
        found = DEF_YES;
        break;
      }
    }
  }

  if (found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  }

  p_prolific_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
  p_prolific_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;
#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  p_prolific_fnct->StatusEP_Handle = USBH_EP_HANDLE_INVALID;
#endif

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_EP_Open()
 *
 * @brief    Notify adapter drvier that an endpoint was opened.
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
static void USBH_USB2SER_PROLIFIC_EP_Open(void           *p_class_fnct,
                                          USBH_EP_HANDLE ep_handle,
                                          CPU_INT08U     if_ix,
                                          CPU_INT08U     ep_type,
                                          CPU_BOOLEAN    ep_dir_in)
{
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_class_fnct;
  CORE_DECLARE_IRQ_STATE;

  (void)&if_ix;

  CORE_ENTER_ATOMIC();
  switch (ep_type) {
    case USBH_EP_TYPE_BULK:
      if (ep_dir_in == DEF_YES) {
        p_prolific_fnct->RxEP_Handle = ep_handle;
      } else {
        p_prolific_fnct->TxEP_Handle = ep_handle;
      }
      break;

    case USBH_EP_TYPE_INTR:
#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
      if (ep_dir_in == DEF_YES) {
        p_prolific_fnct->StatusEP_Handle = ep_handle;
      }
      break;
#endif

    case USBH_EP_TYPE_CTRL:
    case USBH_EP_TYPE_ISOC:
    default:
      break;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_EP_Close()
 *
 * @brief    Notify adapter driver that endpoint has been closed.
 *
 * @param    p_class_fnct    Pointer to internal adapter fnct structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_EP_Close(void           *p_class_fnct,
                                           USBH_EP_HANDLE ep_handle,
                                           CPU_INT08U     if_ix)
{
  USBH_USB2SER_PROLIFIC_FNCT *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_class_fnct;
  CORE_DECLARE_IRQ_STATE;

  (void)&if_ix;

  CORE_ENTER_ATOMIC();
  if (ep_handle == p_prolific_fnct->RxEP_Handle) {
    p_prolific_fnct->RxEP_Handle = USBH_EP_HANDLE_INVALID;
#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  } else if (ep_handle == p_prolific_fnct->StatusEP_Handle) {
    p_prolific_fnct->StatusEP_Handle = USBH_EP_HANDLE_INVALID;
#endif
  } else if (ep_handle == p_prolific_fnct->TxEP_Handle) {
    p_prolific_fnct->TxEP_Handle = USBH_EP_HANDLE_INVALID;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_VendorWr()
 *
 * @brief    Sends Prolific vendor sub-requests to device.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    val                 Value.
 *
 * @param    ix                  Index.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_VendorWr(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                           CPU_INT16U                         val,
                                           CPU_INT16U                         ix,
                                           RTOS_ERR                           *p_err)
{
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_VENDOR,
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
 *                                       USBH_USB2SER_PROLIFIC_VendorRd()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    val                 Value.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *
 * @return   Read data.
 *******************************************************************************************************/
static CPU_INT16U USBH_USB2SER_PROLIFIC_VendorRd(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                                 CPU_INT16U                         val,
                                                 RTOS_ERR                           *p_err)
{
  CPU_INT08U *p_ctrl_buf = p_adapter_drv_data->AdapterCtrlBufPtr;
  CPU_INT16U ix = 0u;
  CPU_INT16U rd_val = (val | 0x0080u);
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  if ((rd_val & 0x0F00u) != 0u) {
    rd_val = (rd_val | 0x8000);
  }

  p_ctrl_buf[0u] = 0u;
  p_ctrl_buf[1u] = 0u;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_VENDOR,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_RECIPIENT_DEV),
                         rd_val,
                         0u,
                         p_ctrl_buf,
                         2u,
                         2u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    ix = MEM_VAL_GET_INT16U_LITTLE(&p_ctrl_buf[0u]);
  }

  return (ix);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_LineSet()
 *
 * @brief    Sets prolific device line characteristics.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    update_baudrate     Flag indicating if baudrate needs to be updated.
 *
 * @param    baudrate            Baudrate in baud/sec.
 *
 * @param    data_bits           Number of data bits.
 *
 * @param    parity              Parity value.
 *
 * @param    stop_bits           Stop bits value.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_LineSet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_BOOLEAN                        update_baudrate,
                                          CPU_INT32U                         baudrate,
                                          CPU_INT08U                         data_bits,
                                          CPU_INT08U                         parity,
                                          CPU_INT08U                         stop_bits,
                                          RTOS_ERR                           *p_err)
{
  CPU_INT08U *p_ctrl_buf = p_adapter_drv_data->AdapterCtrlBufPtr;
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  //                                                               Retrieve current line params.
  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_GET_LINE,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         0u,
                         p_ctrl_buf,
                         7u,
                         7u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (update_baudrate == DEF_YES) {
    MEM_VAL_SET_INT32U_LITTLE(&p_ctrl_buf[0u], baudrate);
  } else {
    p_ctrl_buf[4u] = stop_bits;
    p_ctrl_buf[5u] = parity;
    p_ctrl_buf[6u] = data_bits;
  }

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_SET_LINE,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         0u,
                         p_ctrl_buf,
                         7u,
                         7u,
                         std_req_timeout,
                         p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_PROLIFIC_LineGet()
 *
 * @brief    Retrieves current line characteristics from device.
 *
 * @param    p_adapter_drv_data  Pointer to adapter driver data structure.
 *
 * @param    p_baudrate          Pointer to variable that will receive the current baud rate.
 *
 * @param    p_data_bits         Pointer to variable that will receive the current data bits.
 *
 * @param    p_parity            Pointer to variable that will receive the current parity.
 *
 * @param    p_stop_bits         Pointer to variable that will receive the current stop bits.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_LineGet(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                          CPU_INT32U                         *p_baudrate,
                                          CPU_INT08U                         *p_data_bits,
                                          CPU_INT08U                         *p_parity,
                                          CPU_INT08U                         *p_stop_bits,
                                          RTOS_ERR                           *p_err)
{
  CPU_INT08U *p_ctrl_buf = p_adapter_drv_data->AdapterCtrlBufPtr;
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_adapter_drv_data->DevHandle,
                         USBH_USB2SER_PROLIFIC_REQ_GET_LINE,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         0u,
                         p_ctrl_buf,
                         7u,
                         7u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_baudrate != DEF_NULL) {
    *p_baudrate = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl_buf[0u]);
  }

  if (p_stop_bits != DEF_NULL) {
    *p_stop_bits = p_ctrl_buf[4u];
  }

  if (p_parity != DEF_NULL) {
    *p_parity = p_ctrl_buf[5u];
  }

  if (p_data_bits != DEF_NULL) {
    *p_data_bits = p_ctrl_buf[6u];
  }
}

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_StatusRxCmpl()
 *
 * @brief    Asynchronous status receive completion function.
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
 *******************************************************************************************************/

#if (USBH_USB2SER_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
static void USBH_USB2SER_PROLIFIC_StatusRxCmpl(USBH_DEV_HANDLE dev_handle,
                                               USBH_EP_HANDLE  ep_handle,
                                               CPU_INT08U      *p_buf,
                                               CPU_INT32U      buf_len,
                                               CPU_INT32U      xfer_len,
                                               void            *p_arg,
                                               RTOS_ERR        err)
{
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data = (USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *)p_arg;
  USBH_USB2SER_PROLIFIC_FNCT         *p_prolific_fnct = (USBH_USB2SER_PROLIFIC_FNCT *)p_adapter_drv_data->DataPtr;
  USBH_EP_HANDLE                     status_ep_handle;
  CORE_DECLARE_IRQ_STATE;

  (void)&ep_handle;

  CORE_ENTER_ATOMIC();
  status_ep_handle = p_prolific_fnct->StatusEP_Handle;
  CORE_EXIT_ATOMIC();

  if ((xfer_len >= USBH_USB2SER_PROLIFIC_STATUS_IX)
      && (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)) {
    CPU_INT08U                 status = p_buf[USBH_USB2SER_PROLIFIC_STATUS_IX];
    USBH_USB2SER_SERIAL_STATUS serial_status = { DEF_BIT_NONE, DEF_BIT_NONE };

    serial_status.Line |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_PARITY_ERR)  ? USBH_USB2SER_LINE_STATUS_PARITY_ERR      : DEF_BIT_NONE);
    serial_status.Line |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_BREAK_ERR)   ? USBH_USB2SER_LINE_STATUS_BRK_INT         : DEF_BIT_NONE);
    serial_status.Line |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_FRAME_ERR)   ? USBH_USB2SER_LINE_STATUS_FRAMING_ERR     : DEF_BIT_NONE);
    serial_status.Line |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_OVERRUN_ERR) ? USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR : DEF_BIT_NONE);

    serial_status.Modem |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_CTS)  ? USBH_USB2SER_MODEM_STATUS_CTS  : DEF_BIT_NONE);
    serial_status.Modem |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_DSR)  ? USBH_USB2SER_MODEM_STATUS_DSR  : DEF_BIT_NONE);
    serial_status.Modem |= (DEF_BIT_IS_SET(status, USBH_USB2SER_PROLIFIC_STATUS_RING) ? USBH_USB2SER_MODEM_STATUS_RING : DEF_BIT_NONE);

    USBH_USB2SER_FnctStatusUpdate(p_adapter_drv_data,
                                  serial_status);
  } else if ((RTOS_ERR_CODE_GET(err) != RTOS_ERR_ABORT)
             && (status_ep_handle != USBH_EP_HANDLE_INVALID)) {
    LOG_ERR(("Receiving status -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  if (status_ep_handle != USBH_EP_HANDLE_INVALID) {
    RTOS_ERR err_rx;

    //                                                             Re-submit on intr IN ep.
    USBH_EP_IntrRxAsync(dev_handle,
                        status_ep_handle,
                        p_buf,
                        buf_len,
                        USBH_USB2SER_PROLIFIC_StatusRxCmpl,
                        (void *)p_adapter_drv_data,
                        &err_rx);
    if ((RTOS_ERR_CODE_GET(err_rx) != RTOS_ERR_NONE)
        && (RTOS_ERR_CODE_GET(err_rx) != RTOS_ERR_INVALID_HANDLE)) {
      LOG_ERR(("Re-submitting status buf -> ", RTOS_ERR_LOG_ARG_GET(err_rx)));
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                   USBH_USB2SER_PROLIFIC_DataRxCmpl()
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
 *******************************************************************************************************/
static void USBH_USB2SER_PROLIFIC_DataRxCmpl(USBH_DEV_HANDLE dev_handle,
                                             USBH_EP_HANDLE  ep_handle,
                                             CPU_INT08U      *p_buf,
                                             CPU_INT32U      buf_len,
                                             CPU_INT32U      xfer_len,
                                             void            *p_arg,
                                             RTOS_ERR        err)
{
  (void)&dev_handle;
  (void)&ep_handle;

  USBH_USB2SER_FnctAsyncRxCmpl((USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *)p_arg,
                               p_buf,
                               p_buf,
                               buf_len,
                               xfer_len,
                               err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_USB2SER_PROLIFIC_AVAIL))
