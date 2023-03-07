/***************************************************************************//**
 * @file
 * @brief USB Host - Android Open Accessory Protocol (Aoap)
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

#if (defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST AOAP class requires USB Host Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_AOAP_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_str.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_aoap.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/core/usbh_core_priv.h>

#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, AOAP)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

//                                                                 ------------------ AOAP VENDOR ID ------------------
#define  USBH_AOAP_VENDOR_ID_GOOGLE_INC                     0x18D1u

//                                                                 ----------------- AOAP PRODUCT IDS -----------------
#define  USBH_AOAP_PRODUCT_ID_ANDROID_ACC                   0x2D00u
#define  USBH_AOAP_PRODUCT_ID_ANDROID_ACC_ADB               0x2D01u
#define  USBH_AOAP_PRODUCT_ID_ANDROID_AUDIO                 0x2D02u
#define  USBH_AOAP_PRODUCT_ID_ANDROID_AUDIO_ADB             0x2D03u
#define  USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO             0x2D04u
#define  USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO_ADB         0x2D05u

//                                                                 ---------------- AOAP REQUEST CODES ----------------
#define  USBH_AOAP_REQ_PROTOCOL_GET                             51u
#define  USBH_AOAP_REQ_STRING_SEND                              52u
#define  USBH_AOAP_REQ_ACCESSORY_START                          53u

#define  USBH_AOAP_PROTOCOL_VER_BUF_LEN                          2u

#define  USBH_AOAP_PROTOCOL_VER_1_00                             1u
#define  USBH_AOAP_PROTOCOL_VER_2_00                             2u

//                                                                 ------------------- AOAP STRINGS -------------------
#define  USBH_AOAP_ACC_STR_QTY                                   6u

#define  USBH_AOAP_ACC_STR_IX_MANUFACTURER                       0u
#define  USBH_AOAP_ACC_STR_IX_MODEL                              1u
#define  USBH_AOAP_ACC_STR_IX_DESC                               2u
#define  USBH_AOAP_ACC_STR_IX_VERSION                            3u
#define  USBH_AOAP_ACC_STR_IX_URI                                4u
#define  USBH_AOAP_ACC_STR_IX_SERIAL                             5u

//                                                                 -------------- AOAP ACCESSORY IF NBR ---------------
#define  USBH_AOAP_ACC_IF_NBR                                    0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       AOAP FUNCTION STRUCT
 *******************************************************************************************************/

typedef struct usbh_aoap_fnct_data {
  USBH_DEV_HANDLE  DevHandle;                                   // Handle to device.
  USBH_FNCT_HANDLE FnctHandle;                                  // Handle to function.

  USBH_EP_HANDLE   EP_HandleBulkIn;                             // Handle to bulk in endpoint.
  USBH_EP_HANDLE   EP_HandleBulkOut;                            // Handle to bulk out endpoint.

  void             *AppFnctsArgPtr;                             // Ptr to app fcnts arg.
} USBH_AOAP_FNCT_DATA;

/********************************************************************************************************
 *                                       AOAP CLASS DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_aoap {
  USBH_CLASS_FNCT_POOL FnctPool;                                // Pool of aoap functions.

  USBH_AOAP_APP_FNCTS  *AppFnctsPtr;                            // Ptr to app notification fncts.

  CPU_INT08U           *CtrlBufPtr;                             // Ptr to buffer to use for ctrl data xfers.

  //                                                               Table of aoap accesspry strings.
  const CPU_CHAR       *AccesoryStrTbl[USBH_AOAP_ACC_STR_QTY];
  CPU_INT08U           AccesoryStrLenTbl[USBH_AOAP_ACC_STR_QTY];

  CPU_INT32U           StdReqTimeoutMs;                         // Timeout for standard requests.
} USBH_AOAP;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_AOAP_INIT_CFG USBH_AOAP_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 1u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 1u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_AOAP *USBH_AOAP_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_AOAP_INIT_CFG USBH_AOAP_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u }
#endif
};
#else
extern USBH_AOAP_INIT_CFG USBH_AOAP_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- CLASS DRIVER FUNCTION ---------------
static CPU_BOOLEAN USBH_AOAP_ProbeDev(USBH_DEV_HANDLE dev_handle,
                                      CPU_INT08U      class_code,
                                      void            **pp_class_fnct,
                                      RTOS_ERR        *p_err);

static CPU_BOOLEAN USBH_AOAP_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                       USBH_FNCT_HANDLE fnct_handle,
                                       CPU_INT08U       class_code,
                                       void             **pp_class_fnct,
                                       RTOS_ERR         *p_err);

static CPU_BOOLEAN USBH_AOAP_EP_Probe(void        *p_class_fnct,
                                      CPU_INT08U  if_ix,
                                      CPU_INT08U  ep_type,
                                      CPU_BOOLEAN ep_dir_in);

static void USBH_AOAP_EP_Open(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix,
                              CPU_INT08U     ep_type,
                              CPU_BOOLEAN    ep_dir_in);

static void USBH_AOAP_EP_Close(void           *p_class_fnct,
                               USBH_EP_HANDLE ep_handle,
                               CPU_INT08U     if_ix);

static void USBH_AOAP_Conn(void *p_class_fnct);

static void USBH_AOAP_Disconn(void *p_class_fnct);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_AOAP_UnInit(void);
#endif

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
 *                                           AOAP CLASS DRIVER
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_CLASS_DRV USBH_AOAP_ClassDrv = {
  USBH_AOAP_ProbeDev,
  USBH_AOAP_ProbeFnct,
  USBH_AOAP_EP_Probe,
  USBH_AOAP_EP_Open,
  USBH_AOAP_EP_Close,
  USBH_AOAP_Conn,
  DEF_NULL,
  USBH_AOAP_Disconn,
  DEF_NULL,
  DEF_NULL,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_AOAP_UnInit,
#else
  DEF_NULL,
#endif
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  DEF_NULL,
#endif
  (CPU_CHAR *)"AOAP",
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_AOAP_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the AOAP class is initialized via the
 *               USBH_AOAP_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_AOAP_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_AOAP_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_AOAP_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_AOAP_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the AOAP class is initialized via the
 *               USBH_AOAP_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_AOAP_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                               MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_AOAP_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_AOAP_InitCfg.MemSegPtr = p_mem_seg;
  USBH_AOAP_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_AOAP_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the AOAP class is initialized via the
 *               USBH_AOAP_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_AOAP_ConfigureOptimizeSpdCfg(const USBH_AOAP_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_AOAP_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_AOAP_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_AOAP_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the AOAP class is initialized via the
 *               USBH_AOAP_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_AOAP_ConfigureInitAllocCfg(const USBH_AOAP_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_AOAP_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_AOAP_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_AOAP_Init()
 *
 * @brief    Initializes the AOAP Class.
 *
 * @param    p_str_cfg       Pointer to the configuration structure containing the AOAP strings.
 *                           [Content MUST be persistent]
 *
 * @param    p_app_fncts     Pointer to the callback functions structure.
 *                           [Content MUST be persistent]
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *******************************************************************************************************/
void USBH_AOAP_Init(USBH_AOAP_STR_CFG   *p_str_cfg,
                    USBH_AOAP_APP_FNCTS *p_app_fncts,
                    RTOS_ERR            *p_err)
{
  CPU_INT08U class_fnct_qty;
  CPU_INT08U ctrl_buf_len = USBH_AOAP_PROTOCOL_VER_BUF_LEN;
  USBH_AOAP  *p_aoap;
  CORE_DECLARE_IRQ_STATE;

  if (USBH_AOAP_Ptr != DEF_NULL) {                              // Chk if AOAP already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_str_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Manufacturer and model strings mandatory.
  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrManufacturer != DEF_NULL)
                           && (p_str_cfg->AccStrModel != DEF_NULL)), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrManufacturerLen != 0u)
                           && (p_str_cfg->AccStrModelLen != 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrDescription != DEF_NULL)
                           || (p_str_cfg->AccStrDescriptionLen == 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrVersion != DEF_NULL)
                           || (p_str_cfg->AccStrVersionLen == 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrURI != DEF_NULL)
                           || (p_str_cfg->AccStrURI_Len == 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_str_cfg->AccStrSerial != DEF_NULL)
                           || (p_str_cfg->AccStrSerialLen == 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_AOAP_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_AOAP_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_AOAP_InitCfg.OptimizeSpd.FnctQty == USBH_AOAP_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  //                                                               Alloc AOAP class data.
  p_aoap = (USBH_AOAP *)Mem_SegAlloc("USBH - AOAP Data",
                                     USBH_AOAP_InitCfg.MemSegPtr,
                                     sizeof(USBH_AOAP),
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Ensure ctrl buf can contain longest str.
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrManufacturerLen) ? p_str_cfg->AccStrManufacturerLen : ctrl_buf_len;
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrModelLen) ? p_str_cfg->AccStrModelLen        : ctrl_buf_len;
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrDescriptionLen) ? p_str_cfg->AccStrDescriptionLen  : ctrl_buf_len;
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrVersionLen) ? p_str_cfg->AccStrVersionLen      : ctrl_buf_len;
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrURI_Len) ? p_str_cfg->AccStrURI_Len         : ctrl_buf_len;
  ctrl_buf_len = (ctrl_buf_len < p_str_cfg->AccStrSerialLen) ? p_str_cfg->AccStrSerialLen       : ctrl_buf_len;

  //                                                               Alloc buffer used for ctrl req at enumeration.
  p_aoap->CtrlBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - AOAP ctrl protocol buf",
                                                    USBH_AOAP_InitCfg.MemSegBufPtr,
                                                    ctrl_buf_len,
                                                    USBH_AOAP_InitCfg.BufAlignOctets,
                                                    DEF_NULL,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_aoap->AppFnctsPtr = p_app_fncts;
  p_aoap->StdReqTimeoutMs = 5000u;

  //                                                               Save accessory strings + len.
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_MANUFACTURER] = p_str_cfg->AccStrManufacturer;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_MANUFACTURER] = p_str_cfg->AccStrManufacturerLen;
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_MODEL] = p_str_cfg->AccStrModel;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_MODEL] = p_str_cfg->AccStrModelLen;
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_DESC] = p_str_cfg->AccStrDescription;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_DESC] = p_str_cfg->AccStrDescriptionLen;
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_VERSION] = p_str_cfg->AccStrVersion;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_VERSION] = p_str_cfg->AccStrVersionLen;
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_URI] = p_str_cfg->AccStrURI;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_URI] = p_str_cfg->AccStrURI_Len;
  p_aoap->AccesoryStrTbl[USBH_AOAP_ACC_STR_IX_SERIAL] = p_str_cfg->AccStrSerial;
  p_aoap->AccesoryStrLenTbl[USBH_AOAP_ACC_STR_IX_SERIAL] = p_str_cfg->AccStrSerialLen;

  CORE_ENTER_ATOMIC();
  USBH_AOAP_Ptr = p_aoap;
  CORE_EXIT_ATOMIC();

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  class_fnct_qty = USBH_AOAP_InitCfg.InitAlloc.FnctQty;
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  class_fnct_qty = USBH_AOAP_InitCfg.OptimizeSpd.FnctQty;
#else
  class_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - AOAP class Fnct",
                           &USBH_AOAP_Ptr->FnctPool,
                           USBH_AOAP_InitCfg.MemSegPtr,
                           sizeof(USBH_AOAP_FNCT_DATA),
                           class_fnct_qty,
                           DEF_NULL,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ClassDrvReg(&USBH_AOAP_ClassDrv,
                   p_err);
}

/****************************************************************************************************//**
 *                                       USBH_AOAP_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the AOAP standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_AOAP_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                                RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_AOAP_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_AOAP_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_DevHandleGet()
 *
 * @brief    Retrieves the device handle associated with the AOAP device.
 *
 * @param    aoap_fnct_handle    Handle to the AOAP function.
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
 * @return   Device handle.
 *******************************************************************************************************/
USBH_DEV_HANDLE USBH_AOAP_DevHandleGet(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                                       RTOS_ERR              *p_err)
{
  USBH_DEV_HANDLE     dev_handle;
  USBH_AOAP_FNCT_DATA *p_aoap_fnct_data;

  //                                                               Chk for null err ptr.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                                                                  DEF_NO,
                                                                  KAL_TIMEOUT_INFINITE,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  dev_handle = p_aoap_fnct_data->DevHandle;

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                        DEF_NO);

  return (dev_handle);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_AccDataRx()
 *
 * @brief    Receives the data from android accessory.
 *
 * @param    aoap_fnct_handle    Handle to the AOAP function.
 *
 * @param    p_buf               Pointer to the buffer that will receive the data.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Received length, in octets.
 *******************************************************************************************************/
CPU_INT32U USBH_AOAP_AccDataRx(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                               CPU_INT08U            *p_buf,
                               CPU_INT32U            buf_len,
                               CPU_INT32U            timeout,
                               RTOS_ERR              *p_err)
{
  CPU_INT32U          rx_len;
  USBH_AOAP_FNCT_DATA *p_aoap_fnct_data;

  //                                                               Chk for null err ptr.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                                                                  DEF_NO,
                                                                  KAL_TIMEOUT_INFINITE,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  rx_len = USBH_EP_BulkRx(p_aoap_fnct_data->DevHandle,
                          p_aoap_fnct_data->EP_HandleBulkIn,
                          p_buf,
                          buf_len,
                          timeout,
                          p_err);

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                        DEF_NO);

  return (rx_len);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_AccDataTx()
 *
 * @brief    Send data to the android accessory.
 *
 * @param    aoap_fnct_handle    Handle to the AOAP function.
 *
 * @param    p_buf               Pointer to the buffer that contains the data to send.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Transfer length, in octets.
 *******************************************************************************************************/
CPU_INT32U USBH_AOAP_AccDataTx(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                               CPU_INT08U            *p_buf,
                               CPU_INT32U            buf_len,
                               CPU_INT32U            timeout,
                               RTOS_ERR              *p_err)
{
  CPU_INT32U          tx_len;
  USBH_AOAP_FNCT_DATA *p_aoap_fnct_data;

  //                                                               Chk for null err ptr.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                                                                  DEF_NO,
                                                                  KAL_TIMEOUT_INFINITE,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  tx_len = USBH_EP_BulkTx(p_aoap_fnct_data->DevHandle,
                          p_aoap_fnct_data->EP_HandleBulkOut,
                          p_buf,
                          buf_len,
                          timeout,
                          p_err);

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)aoap_fnct_handle,
                        DEF_NO);

  return (tx_len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_AOAP_ProbeDev()
 *
 * @brief    Probe device to see if AOAP. If not, attempt to switch to accessory mode if possible.
 *
 * @param    dev_handle      Handle to dev.
 *
 * @param    class           Class code.
 *
 * @param    pp_class_fnct   Pointer to the variable that will receive AOAP's function data.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   DEF_FAIL, AOAP class never accepts at device level. Will only attempt to switch mode if
 *           possible/necessary.
 *
 * @note     (1) At this point we must determine if device is android and if it supports the
 *               accessory mode. If yes, request to switch to accessory mode. The mode switch will
 *               cause the device to disconnect and reconnect in accessory mode. If mode switch is
 *               successful, the device will be accepted but an error will be returned to force the
 *               end of the enumeration.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_AOAP_ProbeDev(USBH_DEV_HANDLE dev_handle,
                                      CPU_INT08U      class_code,
                                      void            **pp_class_fnct,
                                      RTOS_ERR        *p_err)
{
  CPU_INT16U vendor_id;
  CPU_INT16U product_id;

  (void)pp_class_fnct;

  if (class_code != USBH_CLASS_CODE_USE_IF_DESC) {              // If class not defined at IF lvl, definitely not AOAP.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  }

  vendor_id = USBH_DevVendorID_Get(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  product_id = USBH_DevProductID_Get(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if ((vendor_id == USBH_AOAP_VENDOR_ID_GOOGLE_INC)
      && ((product_id == USBH_AOAP_PRODUCT_ID_ANDROID_ACC)
          || (product_id == USBH_AOAP_PRODUCT_ID_ANDROID_ACC_ADB)
          || (product_id == USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO)
          || (product_id == USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO_ADB))) {
    //                                                             Device is Android and already in acc mode. ...
    //                                                             ... Decline here. Will be accepted in ProbeFnct().
    return (DEF_FAIL);
  } else {                                                      // See note (1).
    CPU_INT08U str_cnt;
    CPU_INT16U recv_len;
    CPU_INT32U std_req_timeout;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout = USBH_AOAP_Ptr->StdReqTimeoutMs;
    CORE_EXIT_ATOMIC();

    //                                                             Retrieve protcol version.
    recv_len = USBH_EP_CtrlXfer(dev_handle,
                                USBH_AOAP_REQ_PROTOCOL_GET,
                                USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_RECIPIENT_DEV,
                                0u,
                                0u,
                                USBH_AOAP_Ptr->CtrlBufPtr,
                                USBH_AOAP_PROTOCOL_VER_BUF_LEN,
                                USBH_AOAP_PROTOCOL_VER_BUF_LEN,
                                std_req_timeout,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

    if (recv_len == USBH_AOAP_PROTOCOL_VER_BUF_LEN) {
      CPU_INT16U protocol_ver = MEM_VAL_GET_INT16U_LITTLE(USBH_AOAP_Ptr->CtrlBufPtr);

      switch (protocol_ver) {
        case USBH_AOAP_PROTOCOL_VER_1_00:
        case USBH_AOAP_PROTOCOL_VER_2_00:
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          return (DEF_FAIL);
      }
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return (DEF_FAIL);
    }

    //                                                             Dev supports AOAP. Send accessory strings.
    for (str_cnt = 0u; str_cnt < USBH_AOAP_ACC_STR_QTY; str_cnt++) {
      if (USBH_AOAP_Ptr->AccesoryStrTbl[str_cnt] == DEF_NULL) {
        continue;
      }

      Mem_Copy((void *)USBH_AOAP_Ptr->CtrlBufPtr,
               (void *)USBH_AOAP_Ptr->AccesoryStrTbl[str_cnt],
               USBH_AOAP_Ptr->AccesoryStrLenTbl[str_cnt]);

      (void)USBH_EP_CtrlXfer(dev_handle,
                             USBH_AOAP_REQ_STRING_SEND,
                             USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_RECIPIENT_DEV,
                             0u,
                             str_cnt,
                             USBH_AOAP_Ptr->CtrlBufPtr,
                             USBH_AOAP_Ptr->AccesoryStrLenTbl[str_cnt],
                             USBH_AOAP_Ptr->AccesoryStrLenTbl[str_cnt],
                             std_req_timeout,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_FAIL);
      }
    }

    //                                                             Send accessory start req.
    (void)USBH_EP_CtrlXfer(dev_handle,
                           USBH_AOAP_REQ_ACCESSORY_START,
                           USBH_DEV_REQ_TYPE_VENDOR | USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_RECIPIENT_DEV,
                           0u,
                           0u,
                           DEF_NULL,
                           0u,
                           0u,
                           std_req_timeout,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

    //                                                             Dev will disconnect and reconnect in accessory ...
    //                                                             ... mode. Accept dev and return err to stop enum.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (DEF_OK);
  }
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_ProbeFnct()
 *
 * @brief    Probe class to see if function connected is AOAP.
 *
 * @param    dev_handle      Handle to dev.
 *
 * @param    fnct_handle     Handle to fnct.
 *
 * @param    class           Class code.
 *
 * @param    pp_class_fnct   Pointer to the variable that will receive AOAP's function data.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if fnct has been recognized as AOAP,
 *
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_AOAP_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                       USBH_FNCT_HANDLE fnct_handle,
                                       CPU_INT08U       class_code,
                                       void             **pp_class_fnct,
                                       RTOS_ERR         *p_err)
{
  CPU_INT08U             if_nbr;
  CPU_INT16U             vendor_id;
  CPU_INT16U             product_id;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;
  USBH_AOAP_FNCT_DATA    *p_aoap_fnct;

  PP_UNUSED_PARAM(class_code);

  vendor_id = USBH_DevVendorID_Get(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  product_id = USBH_DevProductID_Get(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if ((vendor_id != USBH_AOAP_VENDOR_ID_GOOGLE_INC)
      || ((product_id != USBH_AOAP_PRODUCT_ID_ANDROID_ACC)
          && (product_id != USBH_AOAP_PRODUCT_ID_ANDROID_ACC_ADB)
          && (product_id != USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO)
          && (product_id != USBH_AOAP_PRODUCT_ID_ANDROID_ACC_AUDIO_ADB))) {
    return (DEF_FAIL);                                          // Dev not android accessory.
  }

  if_nbr = USBH_IF_NbrGet(dev_handle,
                          fnct_handle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if (if_nbr != USBH_AOAP_ACC_IF_NBR) {                         // Accessory IF always has nbr 0.
    return (DEF_FAIL);
  }

  //                                                               At this point, function is AOAP. Alloc a class driver.
  class_fnct_handle = USBH_ClassFnctAlloc(&USBH_AOAP_Ptr->FnctPool,
                                          dev_handle,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_AOAP_FNCT_ALLOC);

    return (DEF_OK);
  }

  p_aoap_fnct = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                             class_fnct_handle,
                                                             DEF_YES,
                                                             OP_LOCK_TIMEOUT_INFINITE,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_aoap_fnct->AppFnctsArgPtr = DEF_NULL;
  p_aoap_fnct->DevHandle = dev_handle;
  p_aoap_fnct->FnctHandle = fnct_handle;
  p_aoap_fnct->EP_HandleBulkIn = USBH_EP_HANDLE_INVALID;
  p_aoap_fnct->EP_HandleBulkOut = USBH_EP_HANDLE_INVALID;

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  *pp_class_fnct = (void *)(CPU_ADDR)class_fnct_handle;

  return (DEF_OK);

end_rel:
  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  USBH_AOAP_Disconn((void *)(CPU_ADDR)class_fnct_handle);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_EP_Probe()
 *
 * @brief    Probes AOAP class driver before opening endpoint to ensure class driver needs it.
 *
 * @param    p_class_fnct    Pointer to the internal AOAP function structure.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *
 * @return   DEF_YES, if endpoint is needed.
 *
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_AOAP_EP_Probe(void        *p_class_fnct,
                                      CPU_INT08U  if_ix,
                                      CPU_INT08U  ep_type,
                                      CPU_BOOLEAN ep_dir_in)
{
  CPU_BOOLEAN            ep_needed = DEF_NO;
  USBH_AOAP_FNCT_DATA    *p_aoap_fnct_data;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  RTOS_ERR               err_usbh;

  (void)&if_ix;

  if (ep_type != USBH_EP_TYPE_BULK) {
    return (ep_needed);
  }

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  class_fnct_handle,
                                                                  DEF_NO,
                                                                  OP_LOCK_TIMEOUT_INFINITE,
                                                                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return (DEF_YES);
  }

  if ((ep_dir_in == DEF_YES)
      && (p_aoap_fnct_data->EP_HandleBulkIn == USBH_EP_HANDLE_INVALID)) {
    ep_needed = DEF_YES;
  } else if ((ep_dir_in == DEF_NO)
             && (p_aoap_fnct_data->EP_HandleBulkOut == USBH_EP_HANDLE_INVALID)) {
    ep_needed = DEF_YES;
  }

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);

  return (ep_needed);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_EP_Open()
 *
 * @brief    Notify AOAP class that endpoint has been opened.
 *
 * @param    p_class_fnct    Pointer to the internal AOAP function structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *******************************************************************************************************/
static void USBH_AOAP_EP_Open(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix,
                              CPU_INT08U     ep_type,
                              CPU_BOOLEAN    ep_dir_in)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_AOAP_FNCT_DATA    *p_aoap_fnct_data;
  RTOS_ERR               err_usbh;

  (void)&ep_type;
  (void)&if_ix;

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  class_fnct_handle,
                                                                  DEF_YES,
                                                                  OP_LOCK_TIMEOUT_INFINITE,
                                                                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return;
  }

  if (ep_dir_in == DEF_YES) {
    p_aoap_fnct_data->EP_HandleBulkIn = ep_handle;
  } else {
    p_aoap_fnct_data->EP_HandleBulkOut = ep_handle;
  }

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_EP_Close()
 *
 * @brief    Notify AOAP class that endpoint has been closed.
 *
 * @param    p_class_fnct    Pointer to the internal AOAP function structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *******************************************************************************************************/
static void USBH_AOAP_EP_Close(void           *p_class_fnct,
                               USBH_EP_HANDLE ep_handle,
                               CPU_INT08U     if_ix)
{
  PP_UNUSED_PARAM(p_class_fnct);
  PP_UNUSED_PARAM(ep_handle);
  PP_UNUSED_PARAM(if_ix);
}

/****************************************************************************************************//**
 *                                               USBH_AOAP_Conn()
 *
 * @brief    Notify AOAP class of a connection.
 *
 * @param    p_class_fnct    Pointer to the internal AOAP function structure.
 *******************************************************************************************************/
static void USBH_AOAP_Conn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_AOAP_FNCT_DATA    *p_aoap_fnct_data;
  USBH_DEV_HANDLE        dev_handle;
  USBH_FNCT_HANDLE       fnct_handle;
  RTOS_ERR               err;

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  class_fnct_handle,
                                                                  DEF_NO,
                                                                  OP_LOCK_TIMEOUT_INFINITE,
                                                                  &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  dev_handle = p_aoap_fnct_data->DevHandle;
  fnct_handle = p_aoap_fnct_data->FnctHandle;

  if ((USBH_AOAP_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_AOAP_Ptr->AppFnctsPtr->Conn != DEF_NULL)) {
    p_aoap_fnct_data->AppFnctsArgPtr = USBH_AOAP_Ptr->AppFnctsPtr->Conn(dev_handle,
                                                                        fnct_handle,
                                                                        (USBH_AOAP_FNCT_HANDLE)class_fnct_handle);
  }

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_Disconn()
 *
 * @brief    Notify AOAP class of a disconnection.
 *
 * @param    p_class_fnct    Pointer to the internal AOAP function structure.
 *******************************************************************************************************/
static void USBH_AOAP_Disconn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_AOAP_FNCT_DATA    *p_aoap_fnct_data;
  RTOS_ERR               err;

  p_aoap_fnct_data = (USBH_AOAP_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_AOAP_Ptr->FnctPool,
                                                                  class_fnct_handle,
                                                                  DEF_YES,
                                                                  OP_LOCK_TIMEOUT_INFINITE,
                                                                  &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  if ((USBH_AOAP_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_AOAP_Ptr->AppFnctsPtr->Disconn != DEF_NULL)) {
    USBH_AOAP_Ptr->AppFnctsPtr->Disconn((USBH_AOAP_FNCT_HANDLE)class_fnct_handle,
                                        p_aoap_fnct_data->AppFnctsArgPtr);
  }

  USBH_ClassFnctRelease(&USBH_AOAP_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  USBH_ClassFnctFree(&USBH_AOAP_Ptr->FnctPool,
                     class_fnct_handle,
                     &err);
}

/****************************************************************************************************//**
 *                                           USBH_AOAP_UnInit()
 *
 * @brief    Un-initializes AOAP class and all its function classes.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_AOAP_UnInit(void)
{
  RTOS_ERR err_usbh;

  USBH_ClassFnctPoolDel(&USBH_AOAP_Ptr->FnctPool,
                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Error while freing function pool -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_AOAP_Ptr = DEF_NULL;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL))
