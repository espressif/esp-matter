/***************************************************************************//**
 * @file
 * @brief USB Host Hid Class
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

#if (defined(RTOS_MODULE_USB_HOST_HID_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST HID class requires USB Host Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_HID_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/class/usbh_hid_priv.h>
#include  <usb/include/host/usbh_hid.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_config.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_ep.h>

#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (USBH, CLASS, HID)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_USBH

#define  USBH_HID_MAX_DUR                               1020u
#define  USBH_HID_DUR_RESOLUTION                           4u
#define  USBH_HID_LEN_HID_DESC                             9u

/********************************************************************************************************
 *                                           SUBCLASS CODES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 4.2 for more details about subclass codes.
 *******************************************************************************************************/

#define  USBH_HID_SUBCLASS_CODE_NONE                    0x00u
#define  USBH_HID_SUBCLASS_CODE_BOOT_IF                 0x01u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC DESCRIPTORS
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.1 for more details about class-specific descriptors.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_HID_DESC_TYPE_HID                         0x21u
#define  USBH_HID_DESC_TYPE_REPORT                      0x22u
#define  USBH_HID_DESC_TYPE_PHYSICAL                    0x23u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about class-s[ecific requests.
 *
 *           (2) The 'bRequest' field of a class-specific setup request may contain one of these values.
 *******************************************************************************************************/

#define  USBH_HID_REQ_GET_REPORT                        0x01u
#define  USBH_HID_REQ_GET_IDLE                          0x02u
#define  USBH_HID_REQ_GET_PROTOCOL                      0x03u
#define  USBH_HID_REQ_SET_REPORT                        0x09u
#define  USBH_HID_REQ_SET_IDLE                          0x0Au
#define  USBH_HID_REQ_SET_PROTOCOL                      0x0Bu

/********************************************************************************************************
 *                                       GET REPORT REQUEST TYPES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2.1 for more details about get report request types.
 *
 *           (2) The upper byte of the 'wValue' field of a get report setup request may contain one of
 *               these values.
 *******************************************************************************************************/

#define  USBH_HID_REPORT_TYPE_IN                        0x01u
#define  USBH_HID_REPORT_TYPE_OUT                       0x02u
#define  USBH_HID_REPORT_TYPE_FEATURE                   0x03u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_HID_INIT_CFG USBH_HID_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .RxBufQty = 2u,
  .RxBufLen = 10u,
  .ReportDescMaxLen = 128u,
  .UsageMaxNbrPerItem = 5u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 3u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 3u,
                 .ReportDescParseAppCollItemQty = 2u,
                 .ReportDescParseGlobalItemQty = 2u,
                 .ReportDescParseCollItemQty = 2u,
                 .ReportDescParseReportFmtItemQty = 30u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           HID DESCRIPTOR
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.1 for more details about HID descriptor.
 *******************************************************************************************************/

typedef struct usbh_hid_desc {
  CPU_INT08U bLength;
  CPU_INT08U bDescriptorType;
  CPU_INT16U bcdHID;
  CPU_INT08U bCountryCode;
  CPU_INT08U bNbrDescriptors;
  CPU_INT08U bClassDescriptorType;
  CPU_INT16U wClassDescriptorLength;
} USBH_HID_DESC;

/********************************************************************************************************
 *                                           HID CLASS DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_hid {
  CPU_INT08U           *ReportDescBufPtr;                       // Ptr to report descriptor buffer.

  CPU_INT32U           StdReqTimeoutMs;                         // Timeout for standard requests.

  USBH_CLASS_FNCT_POOL FnctPool;                                // Pool of hid functions.

  USBH_HID_APP_FNCTS   *AppFnctsPtr;                            // Ptr to app notification fncts.
} USBH_HID;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_HID *USBH_HID_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_HID_INIT_CFG USBH_HID_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .RxBufQty = 2u,
  .RxBufLen = 10u,
  .ReportDescMaxLen = 128u,
  .UsageMaxNbrPerItem = 5u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u,
                 .ReportDescParseAppCollItemQty = 0u,
                 .ReportDescParseGlobalItemQty = 0u,
                 .ReportDescParseCollItemQty = 0u,
                 .ReportDescParseReportFmtItemQty = 0u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- CLASS DRIVER FUNCTION ---------------
static CPU_BOOLEAN USBH_HID_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err);

static void USBH_HID_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_ix,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in);

static void USBH_HID_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix);

static void USBH_HID_Conn(void *p_class_fnct);

static void USBH_HID_Disconn(void *p_class_fnct);

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static CPU_INT16U USBH_HID_Tx(USBH_HID_FNCT *p_hid_fnct,
                              CPU_INT08U    report_id,
                              void          *p_buf,
                              CPU_INT16U    buf_len,
                              CPU_INT32U    timeout_ms,
                              RTOS_ERR      *p_err);

static void USBH_HID_ReportRxAsyncCallback(USBH_DEV_HANDLE dev_handle,
                                           USBH_EP_HANDLE  ep_handle,
                                           CPU_INT08U      *p_buf,
                                           CPU_INT32U      buf_len,
                                           CPU_INT32U      xfer_len,
                                           void            *p_arg,
                                           RTOS_ERR        err);

static USBH_HID_APP_COLL *USBH_HID_HID_DescProcess(USBH_HID_FNCT *p_hid_fnct,
                                                   RTOS_ERR      *p_err);

static void USBH_HID_ReportRxAsync(USBH_HID_FNCT          *p_hid_fnct,
                                   CPU_INT08U             *p_buf,
                                   USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                                   RTOS_ERR               *p_err);

static CPU_BOOLEAN USBH_HID_FnctAllocCallback(void *p_class_fnct_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN != DEF_ENABLED)
#error  "USBH_CFG_PERIODIC_XFER_EN Must be set to DEF_ENABLED when using HID Class"
#endif

/********************************************************************************************************
 *                                           HID CLASS DRIVER
 *******************************************************************************************************/

USBH_CLASS_DRV USBH_HID_Drv = {
  DEF_NULL,
  USBH_HID_ProbeFnct,
  DEF_NULL,
  USBH_HID_EP_Open,
  USBH_HID_EP_Close,
  USBH_HID_Conn,
  DEF_NULL,
  USBH_HID_Disconn,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  DEF_NULL,
#endif
  (CPU_CHAR *)"HID",
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_HID_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_HID_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HID_ConfigureRxBuf()
 *
 * @brief    Configures the receive buffers.
 *
 * @param    rx_buf_qty  Quantity of buffers available for report reception.
 *
 * @param    rx_buf_len  Len of buffers, in octets, used  for report reception.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_HID_ConfigureRxBuf(CPU_INT08U rx_buf_qty,
                             CPU_INT08U rx_buf_len)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.RxBufQty = rx_buf_qty;
  USBH_HID_InitCfg.RxBufLen = rx_buf_len;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_HID_ConfigureReportDescMaxLen()
 *
 * @brief    Configures the report descriptor buffer.
 *
 * @param    report_desc_max_len     Maximum length, in octets, of report desc.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_HID_ConfigureReportDescMaxLen(CPU_INT16U report_desc_max_len)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.ReportDescMaxLen = report_desc_max_len;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_HID_ConfigureUsageMaxNbrPerItem()
 *
 * @brief    Configures the maximum quantity of usage per item.
 *
 * @param    usage_max_nbr_per_item  Maximum number of usage associated with a given item.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_HID_ConfigureUsageMaxNbrPerItem(CPU_INT08U usage_max_nbr_per_item)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.UsageMaxNbrPerItem = usage_max_nbr_per_item;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HID_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg       Pointer to memory segment to use when allocating control data.
 *                           Can be the same segment used for p_mem_seg_buf.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @param    p_mem_seg_buf   Pointer to memory segment to use when allocating data buffers.
 *                           Can be the same segment used for p_mem_seg.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_HID_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.MemSegPtr = p_mem_seg;
  USBH_HID_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_HID_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_HID_ConfigureOptimizeSpdCfg(const USBH_HID_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HID_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the HID class is initialized via the
 *               USBH_HID_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_HID_ConfigureInitAllocCfg(const USBH_HID_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_HID_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HID_Init()
 *
 * @brief    Initializes the HID class.
 *
 * @param    p_hid_app_fncts     Pointer to the HID application callback functions structure.
 *                               [Content MUST be persistent]
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_INVALID_CFG
 *******************************************************************************************************/
void USBH_HID_Init(USBH_HID_APP_FNCTS *p_hid_app_fncts,
                   RTOS_ERR           *p_err)
{
  CPU_INT08U class_fnct_qty;
  USBH_HID   *p_hid;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  if (USBH_HID_Ptr != DEF_NULL) {                               // Chk if HID already initialized.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_HID_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_HID_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_HID_InitCfg.OptimizeSpd.FnctQty == USBH_HID_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif // ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) &&
  //                                                               (USBH_CFG_INIT_ALLOC_EN   == DEF_ENABLED))

  //                                                               ---------- ALLOC HID CLASS INTERNAL DATA -----------
  //                                                               Alloc cdc class root struct.
  p_hid = (USBH_HID *)Mem_SegAlloc("USBH - HID root struct",
                                   USBH_HID_InitCfg.MemSegPtr,
                                   sizeof(USBH_HID),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hid->ReportDescBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - HID Report Desc Buf",
                                                         USBH_HID_InitCfg.MemSegBufPtr,
                                                         USBH_HID_InitCfg.ReportDescMaxLen,
                                                         USBH_HID_InitCfg.BufAlignOctets,
                                                         DEF_NULL,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hid->AppFnctsPtr = p_hid_app_fncts;
  p_hid->StdReqTimeoutMs = USBH_HID_STD_REQ_TIMEOUT_DFLT;

  CORE_ENTER_ATOMIC();
  USBH_HID_Ptr = p_hid;
  CORE_EXIT_ATOMIC();

  //                                                               Init HID parser.
  USBH_HID_ParserInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  class_fnct_qty = USBH_HID_InitCfg.InitAlloc.FnctQty;
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  class_fnct_qty = USBH_HID_InitCfg.OptimizeSpd.FnctQty;
#else
  class_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - HID class Fnct",
                           &USBH_HID_Ptr->FnctPool,
                           USBH_HID_InitCfg.MemSegPtr,
                           sizeof(USBH_HID_FNCT),
                           class_fnct_qty,
                           USBH_HID_FnctAllocCallback,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ClassDrvReg(&USBH_HID_Drv, p_err);
}

/****************************************************************************************************//**
 *                                       USBH_HID_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the HID standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_HID_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_HID_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_HID_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HID_UsageGet()
 *
 * @brief    Gets the global usage associated to HID function.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Usage.
 *******************************************************************************************************/
CPU_INT32U USBH_HID_UsageGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                             RTOS_ERR             *p_err)
{
  CPU_INT32U    usage = 0u;
  USBH_HID_FNCT *p_hid_fnct;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      KAL_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (usage);
  }

  usage = p_hid_fnct->Usage;

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);

  return (usage);
}

/****************************************************************************************************//**
 *                                           USBH_HID_IsBootCapable()
 *
 * @brief    Tests whether HID interface belongs to boot subclass.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_INVALID_STATE
 *
 * @return   DEF_YES, if the function belongs to the boot subclass,
 *           DEF_NO,  if the function does not belong to the boot subclass.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_HID_IsBootCapable(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                   RTOS_ERR             *p_err)
{
  CPU_BOOLEAN   is_boot = DEF_NO;
  CPU_INT08U    subclass;
  USBH_HID_FNCT *p_hid_fnct;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      KAL_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (is_boot);
  }

  subclass = USBH_FnctSubclassGet(p_hid_fnct->DevHandle, p_hid_fnct->FnctHandle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_release;
  }

  is_boot = (subclass == USBH_HID_SUBCLASS_CODE_BOOT_IF) ? DEF_YES : DEF_NO;

end_release:
  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);

  return (is_boot);
}

/****************************************************************************************************//**
 *                                           USBH_HID_ReportTx()
 *
 * @brief    Sends the report to the device.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    report_id           ID of the Report.
 *
 * @param    p_buf               Pointer to the the buffer that contains the report.
 *
 * @param    buf_len             Buffer length, in octets.
 *
 * @param    timeout_ms          Timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NULL_PTR
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Number of octets sent.
 *
 * @note     (1) Do not add the report id to p_buf, it will be added automatically.
 *******************************************************************************************************/
CPU_INT16U USBH_HID_ReportTx(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                             CPU_INT08U           report_id,
                             void                 *p_buf,
                             CPU_INT16U           buf_len,
                             CPU_INT32U           timeout_ms,
                             RTOS_ERR             *p_err)
{
  CPU_INT16U    xfer_len = 0u;
  USBH_HID_FNCT *p_hid_fnct;

  if ((p_buf == DEF_NULL)
      || (buf_len == 0u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return (0u);
  }

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (xfer_len);
  }

  xfer_len = USBH_HID_Tx(p_hid_fnct,                            // Tx output report.
                         report_id,
                         p_buf,
                         buf_len,
                         timeout_ms,
                         p_err);

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_HID_ProtocolSet()
 *
 * @brief    Sets the protocol (boot/report descriptor) of HID function.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    protocol            Protocol to set.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_HID_ProtocolSet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                          CPU_INT16U           protocol,
                          RTOS_ERR             *p_err)
{
  CPU_INT32U    std_req_timeout;
  USBH_HID_FNCT *p_hid_fnct;
  CORE_DECLARE_IRQ_STATE;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_HID_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                         USBH_HID_REQ_SET_PROTOCOL,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         protocol,
                         p_hid_fnct->IF_Nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_ProtocolGet()
 *
 * @brief    Gets the protocol (boot/report) of the HID function.
 *
 * @param    hid_fnct_handle     Handle the HID function.
 *
 * @param    p_protocol          Variable that receives protocol of the device.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_HID_ProtocolGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                          CPU_INT16U           *p_protocol,
                          RTOS_ERR             *p_err)
{
  CPU_INT32U    std_req_timeout;
  USBH_HID_FNCT *p_hid_fnct;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((p_protocol != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_HID_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                         USBH_HID_REQ_GET_PROTOCOL,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         p_hid_fnct->IF_Nbr,
                         (CPU_INT08U *)p_protocol,
                         1u,
                         1u,
                         std_req_timeout,
                         p_err);

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_IdleSet()
 *
 * @brief    Sets the idle duration for the given report ID.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    report_id           ID of the Report.
 *
 * @param    dur                 Idle duration, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_HID_IdleSet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                      CPU_INT08U           report_id,
                      CPU_INT32U           dur,
                      RTOS_ERR             *p_err)
{
  CPU_INT32U    std_req_timeout;
  USBH_HID_FNCT *p_hid_fnct;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((dur <= USBH_HID_MAX_DUR), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  dur = dur / USBH_HID_DUR_RESOLUTION;                          // Convert into resolution units.

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_HID_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  //                                                               Send class-specific request.
  (void)USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                         USBH_HID_REQ_SET_IDLE,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         (((dur << 8u) & 0xFF00u) | report_id),
                         p_hid_fnct->IF_Nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_EP_STALL) {
    RTOS_ERR err_abort;

    USBH_EP_Abort(p_hid_fnct->DevHandle,
                  USBH_EP_HANDLE_CTRL,
                  &err_abort);
    PP_UNUSED_PARAM(err_abort);
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_NO);
  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_IdleGet()
 *
 * @brief    Gets the idle duration for the given report ID.
 *
 * @param    hid_fnct_handle     Handle to the HID function.
 *
 * @param    report_id           ID of the Report.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Idle duration in milliseconds.
 *******************************************************************************************************/
CPU_INT32U USBH_HID_IdleGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                            CPU_INT08U           report_id,
                            RTOS_ERR             *p_err)
{
  CPU_INT32U    dur = 0u;
  CPU_INT32U    std_req_timeout;
  USBH_HID_FNCT *p_hid_fnct;
  CORE_DECLARE_IRQ_STATE;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                                                      DEF_YES,
                                                      KAL_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (dur);
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_HID_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                         USBH_HID_REQ_GET_IDLE,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         report_id,
                         p_hid_fnct->IF_Nbr,
                         (CPU_INT08U *)&dur,
                         1u,
                         1u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    dur = (dur) * USBH_HID_DUR_RESOLUTION;                      // Convert to ms.
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)hid_fnct_handle,
                        DEF_YES);
  return (dur);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HID_ProbeFnct()
 *
 * @brief    Callback to probe class function to see if it is an HID class function.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    class_code      Class code.
 *
 * @param    pp_class_fnct   Pointer to the variable that will receive class-specific argument, if any.
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this
 *                           function.
 *
 * @return   DEF_OK,     if connected function is a HID,
 *           DEF_FAIL,   otherwise
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HID_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err)
{
  USBH_HID_FNCT          *p_hid_fnct;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;

  if (class_code != USBH_CLASS_CODE_HID) {                      // Check for HID class code.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  }

  //                                                               Alloc a fnct from HID fnct pool.
  class_fnct_handle = USBH_ClassFnctAlloc(&USBH_HID_Ptr->FnctPool,
                                          dev_handle,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_HID_FNCT_ALLOC);
    return (DEF_OK);
  }

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBH_HID_Disconn((void *)(CPU_ADDR)class_fnct_handle);
    return (DEF_OK);
  }

  p_hid_fnct->DevHandle = dev_handle;
  p_hid_fnct->FnctHandle = fnct_handle;
  p_hid_fnct->InEP_Handle = USBH_EP_HANDLE_INVALID;
  p_hid_fnct->OutEP_Handle = USBH_EP_HANDLE_INVALID;
  p_hid_fnct->IF_Nbr = USBH_IF_NbrGet(dev_handle, fnct_handle, 0u, p_err);

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  *pp_class_fnct = (void *)(CPU_ADDR)class_fnct_handle;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HID_EP_Open()
 *
 * @brief    Endpoint open callback. Indicates class EPs as open.
 *
 * @param    p_class_fnct    Class function handle.
 *
 * @param    ep_handle       Handle of opened endpoint.
 *
 * @param    if_ix           Interface index of opened endpoint.
 *
 * @param    ep_type         Type of endpoint opened.
 *
 * @param    ep_dir_in       Direction of endpoint. DEF_YES if endpoint is IN.
 *******************************************************************************************************/
static void USBH_HID_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_ix,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_HID_FNCT          *p_hid_fnct;
  RTOS_ERR               err;

  PP_UNUSED_PARAM(if_ix);

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  if (ep_type == USBH_EP_TYPE_INTR) {
    if (ep_dir_in == DEF_YES) {
      p_hid_fnct->InEP_Handle = ep_handle;
    } else {
      p_hid_fnct->OutEP_Handle = ep_handle;
    }
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_HID_EP_Close()
 *
 * @brief    Endpoint close callback. Indicates class EPs as closed.
 *
 * @param    p_class_fnct    Class function handle.
 *
 * @param    ep_handle       Handle of closed endpoint.
 *
 * @param    if_ix           Interface index of closed endpoint.
 *******************************************************************************************************/
static void USBH_HID_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_HID_FNCT          *p_hid_fnct;
  RTOS_ERR               err;

  PP_UNUSED_PARAM(if_ix);

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Mark EP as invalid.
  if (p_hid_fnct->InEP_Handle == ep_handle) {
    p_hid_fnct->InEP_Handle = USBH_EP_HANDLE_INVALID;
  } else if (p_hid_fnct->OutEP_Handle == ep_handle) {
    p_hid_fnct->OutEP_Handle = USBH_EP_HANDLE_INVALID;
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                               USBH_HID_Conn()
 *
 * @brief    Connection callback. Processes descriptors, calls application callback, start receiving
 *           reports from HID function.
 *
 * @param    p_class_fnct    Class function handle.
 *
 * @note     (1) The locking scheme used (lock as non-exclusive, then exclusive to set AppFnctsArgPtr
 *               and then non-exclusive) is safe because USBH_HID_Conn and Disconn can only be called
 *               by a single task, the HUB task. Therefore, no need to protect from a Disconn here
 *               and it's also safe from the Async task since we re-lock before queuing the async
 *               xfers on the intr EP.
 *******************************************************************************************************/
static void USBH_HID_Conn(void *p_class_fnct)
{
  USBH_HID_APP_COLL      *p_app_coll_head;
  USBH_HID_FNCT          *p_hid_fnct;
  void                   *p_arg = DEF_NULL;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  CPU_INT08U             rx_buf_ix;
  RTOS_ERR               err;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Read and parse report desc.
  p_app_coll_head = USBH_HID_HID_DescProcess(p_hid_fnct, &err);
  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (p_hid_fnct->MaxReportLen > USBH_HID_InitCfg.RxBufLen)) {
    RTOS_ERR_SET(err, RTOS_ERR_OVERFLOW_DESC);
  }

  if ((USBH_HID_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_HID_Ptr->AppFnctsPtr->Conn != DEF_NULL)) {
    p_arg = USBH_HID_Ptr->AppFnctsPtr->Conn(p_hid_fnct->DevHandle,
                                            p_hid_fnct->FnctHandle,
                                            (USBH_HID_FNCT_HANDLE)class_fnct_handle,
                                            p_app_coll_head,
                                            err);
  }

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    p_hid_fnct->DevHandle = USBH_DEV_HANDLE_INVALID;
    p_hid_fnct->FnctHandle = USBH_FNCT_HANDLE_INVALID;
    p_hid_fnct->IF_Nbr = 0u;
    goto end_err;
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring HID class fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_hid_fnct->AppFnctsArgPtr = p_arg;

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring HID class fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  if (p_hid_fnct->MaxReportLen != 0u) {
    for (rx_buf_ix = 0u; rx_buf_ix < USBH_HID_InitCfg.RxBufQty; rx_buf_ix++) {
      USBH_HID_ReportRxAsync(p_hid_fnct,
                             p_hid_fnct->RxBufPtrTbl[rx_buf_ix],
                             class_fnct_handle,
                             &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Submitting HID xfers -> ", RTOS_ERR_LOG_ARG_GET(err)));
      }
    }
  }

end_err:
  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_Disconn()
 *
 * @brief    Disconnnection callback. Calls disconn app callback and frees resources.
 *
 * @param    p_class_fnct    Class function handle.
 *******************************************************************************************************/
static void USBH_HID_Disconn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_HID_FNCT          *p_hid_fnct;
  RTOS_ERR               err;

  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  if (USBH_HID_Ptr->AppFnctsPtr != DEF_NULL && USBH_HID_Ptr->AppFnctsPtr->Disconn != DEF_NULL) {
    USBH_HID_Ptr->AppFnctsPtr->Disconn((USBH_HID_FNCT_HANDLE)class_fnct_handle,
                                       p_hid_fnct->AppFnctsArgPtr);
  }

  p_hid_fnct->DevHandle = USBH_DEV_HANDLE_INVALID;
  p_hid_fnct->FnctHandle = USBH_FNCT_HANDLE_INVALID;
  p_hid_fnct->IF_Nbr = 0u;

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  USBH_ClassFnctFree(&USBH_HID_Ptr->FnctPool,
                     class_fnct_handle,
                     &err);
}

/****************************************************************************************************//**
 *                                               USBH_HID_Tx()
 *
 * @brief    Send report to HID function.
 *
 * @param    p_hid_fnct  Pointer to the HID function.
 *
 * @param    report_id   Report Id.
 *
 * @param    p_buf       Pointer to the buffer that contains the report.
 *
 * @param    buf_len     Buffer length, in octets.
 *
 * @param    timeout_ms  Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_USB_URB_ALLOC
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_TX
 *
 * @return   Number of octets sent.
 *******************************************************************************************************/
static CPU_INT16U USBH_HID_Tx(USBH_HID_FNCT *p_hid_fnct,
                              CPU_INT08U    report_id,
                              void          *p_buf,
                              CPU_INT16U    buf_len,
                              CPU_INT32U    timeout_ms,
                              RTOS_ERR      *p_err)
{
  CPU_INT16U xfer_len;

  //                                                               Use optional intr OUT EP if available.
  if (p_hid_fnct->OutEP_Handle != USBH_EP_HANDLE_INVALID) {
    xfer_len = (CPU_INT16U)USBH_EP_IntrTx(p_hid_fnct->DevHandle,
                                          p_hid_fnct->OutEP_Handle,
                                          (CPU_INT08U *)p_buf,
                                          buf_len,
                                          timeout_ms,
                                          p_err);
  } else {
    //                                                             Send report via ctrl EP if intr OUT not available.
    xfer_len = USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                                USBH_HID_REQ_SET_REPORT,
                                (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                                (((USBH_HID_REPORT_TYPE_OUT << 8u) & 0xFF00u) | report_id),
                                p_hid_fnct->IF_Nbr,
                                (CPU_INT08U *)p_buf,
                                buf_len,
                                buf_len,
                                timeout_ms,
                                p_err);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                       USBH_HID_ReportRxAsyncCallback()
 *
 * @brief    Interrupt callback function, checks status of transfer, calls callback and re-submits
 *           another transfer.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Length of received buffer in octets.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Context variable, in this case, the class function handle.
 *
 * @param    err         Error from transfer.
 *
 * @note     (1) The bInterval period associated to periodic transfer can be managed by hardware
 *               (i.e. host controller) or by sofware (i.e. host stack).
 *               - (a) Hardware: host controller will emit the periodic IN request every bInterval
 *                     written into a register. Essentially, a list-based host controller will use this
 *                     method (e.g. OHCI host controller).
 *               - (b) Software: host controller doesn't provide a hardware management of the bInterval
 *                     period. In this case, the stack will wait bInterval frame before re-submiting an
 *                     IN request.
 *
 * @note     (2) If endpoint has no interrupt data to transmit when accessed by the host, it
 *               responds with NAK. Next polling from the Host will take place at the next period
 *               (i.e. bInterval of the endpoint).
 *******************************************************************************************************/
static void USBH_HID_ReportRxAsyncCallback(USBH_DEV_HANDLE dev_handle,
                                           USBH_EP_HANDLE  ep_handle,
                                           CPU_INT08U      *p_buf,
                                           CPU_INT32U      buf_len,
                                           CPU_INT32U      xfer_len,
                                           void            *p_arg,
                                           RTOS_ERR        err)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_arg;
  USBH_HID_FNCT          *p_hid_fnct;
  RTOS_ERR               local_err;

  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(ep_handle);
  PP_UNUSED_PARAM(buf_len);

  //                                                               Get HID fnct.
  p_hid_fnct = (USBH_HID_FNCT *)USBH_ClassFnctAcquire(&USBH_HID_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring HID class fnct -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    return;
  }

  if ((RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)
      && (p_hid_fnct->InEP_Handle != USBH_EP_HANDLE_INVALID)) {
    USBH_EP_Abort(p_hid_fnct->DevHandle,
                  p_hid_fnct->InEP_Handle,
                  &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Aborting failed HID xfer -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
  }

  if ((USBH_HID_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_HID_Ptr->AppFnctsPtr->DataRxd != DEF_NULL)) {
    CPU_INT08U report_id = 0u;
    CPU_INT32U buf_ix_start = 0u;

    //                                                             If fnct has multiple report input reports.
    if (p_hid_fnct->HasMultipleInputReports == DEF_YES) {
      //                                                           If yes, report ID is specified at beginning of buf.
      report_id = p_buf[0u];
      buf_ix_start = 1u;
    }
    USBH_HID_Ptr->AppFnctsPtr->DataRxd(class_fnct_handle,
                                       p_hid_fnct->AppFnctsArgPtr,
                                       report_id,
                                       &p_buf[buf_ix_start],
                                       (xfer_len - buf_ix_start),
                                       err);
  }

  USBH_HID_ReportRxAsync(p_hid_fnct,
                         p_buf,
                         class_fnct_handle,
                         &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Submitting HID xfers -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }

  USBH_ClassFnctRelease(&USBH_HID_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_HID_HID_DescProcess()
 *
 * @brief    Parse HID descriptor, then request and parse report descriptor.
 *
 * @param    p_hid_fnct  Pointer to the HID function.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @return   Pointer to the head of app collection linked list.
 *******************************************************************************************************/
static USBH_HID_APP_COLL *USBH_HID_HID_DescProcess(USBH_HID_FNCT *p_hid_fnct,
                                                   RTOS_ERR      *p_err)
{
  USBH_HID_DESC     hid_desc;                                   // HID desc content.
  CPU_INT16U        len;
  CPU_INT16U        extra_desc_len;
  CPU_INT08U        *p_mem;
  CPU_INT32U        std_req_timeout;
  USBH_HID_APP_COLL *p_app_coll_head = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Get HID desc from extra desc.
  p_mem = USBH_IF_DescExtraGet(p_hid_fnct->FnctHandle,
                               0u,
                               &extra_desc_len,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_mem += USBH_DESC_LEN_IF;                                    // Skip IF desc.

  //                                                               Get desc len.
  hid_desc.bLength = MEM_VAL_GET_INT08U_LITTLE((void *)p_mem);
  len = hid_desc.bLength;
  if (len < USBH_HID_LEN_HID_DESC) {                            // Chk len.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return (DEF_NULL);
  }
  p_mem += sizeof(CPU_INT08U);
  len -= sizeof(CPU_INT08U);

  //                                                               Get desc type.
  hid_desc.bDescriptorType = MEM_VAL_GET_INT08U_LITTLE((void *)p_mem);
  //                                                               Type should be USBH_HID_DESC_TYPE_HID.
  if (hid_desc.bDescriptorType != USBH_HID_DESC_TYPE_HID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return (DEF_NULL);
  }
  p_mem += sizeof(CPU_INT08U);
  len -= sizeof(CPU_INT08U);

  //                                                               -------------------- GET bcdHID --------------------
  hid_desc.bcdHID = MEM_VAL_GET_INT16U_LITTLE((void *)p_mem);
  p_mem += sizeof(CPU_INT16U);
  len -= sizeof(CPU_INT16U);
  //                                                               ----------------- GET COUNTRY CODE -----------------
  hid_desc.bCountryCode = MEM_VAL_GET_INT08U_LITTLE((void *)p_mem);
  p_mem += sizeof(CPU_INT08U);
  len -= sizeof(CPU_INT08U);
  //                                                               -------------- GET NBR OF CLASS DESC ---------------
  hid_desc.bNbrDescriptors = MEM_VAL_GET_INT08U_LITTLE((void *)p_mem);
  p_mem += sizeof(CPU_INT08U);
  len -= sizeof(CPU_INT08U);

  if (hid_desc.bNbrDescriptors < 1u) {                          // Nbr of desc should be at least one.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return (DEF_NULL);
  }

  hid_desc.bDescriptorType = 0u;
  //                                                               ----------------- GET REPORT DESC ------------------
  while (len > 0) {
    CPU_INT08U b_type;
    CPU_INT16U w_len;

    b_type = MEM_VAL_GET_INT08U_LITTLE((void *)p_mem);
    p_mem += sizeof(CPU_INT08U);
    len -= sizeof(CPU_INT08U);

    w_len = MEM_VAL_GET_INT16U_LITTLE((void *)p_mem);
    p_mem += sizeof(CPU_INT16U);
    len -= sizeof(CPU_INT16U);
    //                                                             If this is a report desc ...
    if ((b_type == USBH_HID_DESC_TYPE_REPORT)
        && (w_len > 0u)) {
      hid_desc.bDescriptorType = b_type;
      hid_desc.wClassDescriptorLength = w_len;
      break;
    }
  }

  if (hid_desc.bDescriptorType == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return (DEF_NULL);
  }

  if (hid_desc.wClassDescriptorLength > USBH_HID_InitCfg.ReportDescMaxLen) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OVERFLOW_DESC);
    return (DEF_NULL);
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_HID_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  //                                                               Read report desc.
  len = USBH_EP_CtrlXfer(p_hid_fnct->DevHandle,
                         USBH_DEV_REQ_GET_DESC,
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_RECIPIENT_IF),
                         ((USBH_HID_DESC_TYPE_REPORT << 8u) & 0xFF00u),
                         p_hid_fnct->IF_Nbr,
                         USBH_HID_Ptr->ReportDescBufPtr,
                         hid_desc.wClassDescriptorLength,
                         hid_desc.wClassDescriptorLength,
                         std_req_timeout,
                         p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
      && (len == hid_desc.wClassDescriptorLength)) {
    //                                                             Parse report desc.
    p_app_coll_head = USBH_HID_ReportDescParse(p_hid_fnct,
                                               USBH_HID_Ptr->ReportDescBufPtr,
                                               len,
                                               p_err);
  }

  return (p_app_coll_head);
}

/****************************************************************************************************//**
 *                                           USBH_HID_ReportRxAsync()
 *
 * @brief    Receive INPUT report for given HID function.
 *
 * @param    p_hid_fnct          Pointer to the HID function.
 *
 * @param    p_buf               Pointer to the destination buffer to receive data.
 *
 * @param    class_fnct_handle   Handle of class function.
 *
 * @param    p_err               Pointer to the variable that will receive return error code from this
 *                               function.
 *******************************************************************************************************/
static void USBH_HID_ReportRxAsync(USBH_HID_FNCT          *p_hid_fnct,
                                   CPU_INT08U             *p_buf,
                                   USBH_CLASS_FNCT_HANDLE class_fnct_handle,
                                   RTOS_ERR               *p_err)
{
  //                                                               Start receiving in data.
  if (p_hid_fnct->InEP_Handle != USBH_EP_HANDLE_INVALID) {
    USBH_EP_IntrRxAsync(p_hid_fnct->DevHandle,
                        p_hid_fnct->InEP_Handle,
                        p_buf,
                        p_hid_fnct->MaxReportLen,
                        USBH_HID_ReportRxAsyncCallback,
                        (void *)(CPU_ADDR)class_fnct_handle,
                        p_err);
  }

  return;
}

/****************************************************************************************************//**
 *                                       USBH_HID_FnctAllocCallback()
 *
 * @brief    Function called when a USBH_HID_FNCT block is obtained from pool. Allocates rx buffers
 *           for that HID function.
 *
 * @param    p_class_fnct_data   Pointer to the USBH_HID_FNCT structure allocated.
 *
 * @return   DEF_OK,   if allocation of buffers was successful,
 *           DEF_FAIL, if not.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HID_FnctAllocCallback(void *p_class_fnct_data)
{
  USBH_HID_FNCT *p_hid_fnct = (USBH_HID_FNCT *)p_class_fnct_data;
  CPU_INT08U    i;
  RTOS_ERR      err_lib;

  //                                                               -------------- ALLOC RX BUFFERS TABLE --------------
  p_hid_fnct->RxBufPtrTbl = (CPU_INT08U **)Mem_SegAlloc("USBH - HID Fnct Rx Buf tbl",
                                                        USBH_HID_InitCfg.MemSegPtr,
                                                        (sizeof(CPU_INT08U *) * USBH_HID_InitCfg.RxBufQty),
                                                        &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  for (i = 0u; i < USBH_HID_InitCfg.RxBufQty; i++) {            // Alloc rx buf(s).
    p_hid_fnct->RxBufPtrTbl[i] = (CPU_INT08U *)Mem_SegAllocHW("USBH - HID Fnct Rx Bufs",
                                                              USBH_HID_InitCfg.MemSegBufPtr,
                                                              USBH_HID_InitCfg.RxBufLen,
                                                              USBH_HID_InitCfg.BufAlignOctets,
                                                              DEF_NULL,
                                                              &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_HID_AVAIL))
