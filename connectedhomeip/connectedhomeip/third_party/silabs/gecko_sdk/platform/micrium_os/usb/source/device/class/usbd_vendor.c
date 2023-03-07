/***************************************************************************//**
 * @file
 * @brief USB Device Vendor Class
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

#if (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device Vendor class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    USBD_VENDOR_MODULE
#include  <em_core.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>

#include  <usb/include/device/usbd_vendor.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, VENDOR)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_vendor_ctrl USBD_VENDOR_CTRL;               // Forward declaration.

/********************************************************************************************************
 *                                           VENDOR CLASS STATES
 *******************************************************************************************************/

typedef enum usbd_vendor_state {                                // Vendor class states.
  USBD_VENDOR_STATE_NONE = 0,
  USBD_VENDOR_STATE_INIT,
  USBD_VENDOR_STATE_CONFIG
} USBD_VENDOR_STATE;

/********************************************************************************************************
 *                               VENDOR CLASS EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

//                                                                 -------------- VENDOR CLASS COMM INFO --------------
typedef struct usbd_vendor_comm {
  USBD_VENDOR_CTRL *CtrlPtr;                                    // Ptr to ctrl info.
                                                                // Avail EP for comm: Bulk (and Intr)
  CPU_INT08U       DataBulkInEpAddr;
  CPU_INT08U       DataBulkOutEpAddr;
  CPU_INT08U       IntrInEpAddr;
  CPU_INT08U       IntrOutEpAddr;

  CPU_BOOLEAN      DataBulkInActiveXfer;
  CPU_BOOLEAN      DataBulkOutActiveXfer;
  CPU_BOOLEAN      IntrInActiveXfer;
  CPU_BOOLEAN      IntrOutActiveXfer;
} USBD_VENDOR_COMM;

struct usbd_vendor_ctrl {                                       // -------------- VENDOR CLASS CTRL INFO --------------
  CPU_INT08U              DevNbr;                               ///< Vendor class dev nbr.
  USBD_VENDOR_STATE       State;                                ///< Vendor class state.
  CPU_INT08U              ClassNbr;                             ///< Vendor class instance nbr.
  USBD_VENDOR_COMM        *CommPtr;                             ///< Vendor class comm info ptr.
  CPU_BOOLEAN             IntrEn;                               ///< Intr IN & OUT EPs en/dis flag.
  CPU_INT16U              IntrInterval;                         ///< Polling interval for intr IN & OUT EPs.
  USBD_VENDOR_REQ_FNCT    VendorReqCallbackPtr;                 ///< Ptr to app callback for vendor-specific req.
  USBD_VENDOR_ASYNC_FNCT  BulkRdAsyncFnct;                      ///< Ptr to callback  used for async comm.
  void                    *BulkRdAsyncArgPtr;                   ///< Ptr to extra arg used for async comm.
  USBD_VENDOR_ASYNC_FNCT  BulkWrAsyncFnct;                      ///< Ptr to callback  used for async comm.
  void                    *BulkWrAsyncArgPtr;                   ///< Ptr to extra arg used for async comm.
  USBD_VENDOR_ASYNC_FNCT  IntrRdAsyncFnct;                      ///< Ptr to callback  used for async comm.
  void                    *IntrRdAsyncArgPtr;                   ///< Ptr to extra arg used for async comm.
  USBD_VENDOR_ASYNC_FNCT  IntrWrAsyncFnct;                      ///< Ptr to callback  used for async comm.
  void                    *IntrWrAsyncArgPtr;                   ///< Ptr to extra arg used for async comm.

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)                     // Microsoft ext properties.
  USBD_MS_OS_EXT_PROPERTY *MS_ExtPropertyTbl;
  CPU_INT08U              MS_ExtPropertyNext;
#endif
};

typedef struct usbd_vendor {                                    // ------------- VENDOR CLASS ROOT STRUCT -------------
  USBD_VENDOR_CTRL *CtrlTbl;                                    // Vendor class ctrl array.
  CPU_INT08U       CtrlNbrNext;
  //                                                               Vendor class comm array.
  USBD_VENDOR_COMM *CommTbl;
  CPU_INT08U       CommNbrNext;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_INT08U       ClassInstanceQty;
#endif
} USBD_VENDOR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_VENDOR_INIT_CFG USBD_Vendor_InitCfgDflt = {
  .MsExtPropertiesQty = 1u,
  .MemSegPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_VENDOR *USBD_VendorPtr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_VENDOR_INIT_CFG USBD_Vendor_InitCfg = {
  .MsExtPropertiesQty = 1u,
  .MemSegPtr = DEF_NULL
};
#else
extern USBD_VENDOR_INIT_CFG USBD_Vendor_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_Vendor_Conn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg);

static void USBD_Vendor_Disconn(CPU_INT08U dev_nbr,
                                CPU_INT08U config_nbr,
                                void       *p_if_class_arg);

static CPU_BOOLEAN USBD_Vendor_VendorReq(CPU_INT08U           dev_nbr,
                                         const USBD_SETUP_REQ *p_setup_req,
                                         void                 *p_if_class_arg);

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_INT08U USBD_Vendor_MS_GetCompatID(CPU_INT08U dev_nbr,
                                             CPU_INT08U *p_sub_compat_id_ix);

static CPU_INT08U USBD_Vendor_MS_GetExtPropertyTbl(CPU_INT08U              dev_nbr,
                                                   USBD_MS_OS_EXT_PROPERTY **pp_ext_property_tbl);
#endif

static void USBD_Vendor_RdAsyncCmpl(CPU_INT08U dev_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err);

static void USBD_Vendor_WrAsyncCmpl(CPU_INT08U dev_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err);

static void USBD_Vendor_IntrRdAsyncCmpl(CPU_INT08U dev_nbr,
                                        CPU_INT08U ep_addr,
                                        void       *p_buf,
                                        CPU_INT32U buf_len,
                                        CPU_INT32U xfer_len,
                                        void       *p_arg,
                                        RTOS_ERR   err);

static void USBD_Vendor_IntrWrAsyncCmpl(CPU_INT08U dev_nbr,
                                        CPU_INT08U ep_addr,
                                        void       *p_buf,
                                        CPU_INT32U buf_len,
                                        CPU_INT32U xfer_len,
                                        void       *p_arg,
                                        RTOS_ERR   err);

/********************************************************************************************************
 *                                           VENDOR CLASS DRIVER
 *******************************************************************************************************/

static USBD_CLASS_DRV USBD_Vendor_Drv = {
  USBD_Vendor_Conn,
  USBD_Vendor_Disconn,
  DEF_NULL,                                                     // Vendor does NOT use alternate interface(s).
  DEF_NULL,
  DEF_NULL,                                                     // Vendor does NOT use functional EP desc.
  DEF_NULL,
  DEF_NULL,                                                     // Vendor does NOT use functional IF desc.
  DEF_NULL,
  DEF_NULL,                                                     // Vendor does NOT handle std req with IF recipient.
  DEF_NULL,                                                     // Vendor does NOT define class-specific req.
  USBD_Vendor_VendorReq,

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  USBD_Vendor_MS_GetCompatID,
  USBD_Vendor_MS_GetExtPropertyTbl,
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBD_Vendor_ConfigureMsExtPropertiesQty()
 *
 * @brief    Configures the quantity of Microsoft extended properties. Ignored when
 *           USBD_CFG_MS_OS_DESC_EN is set do DEF_DISABLED.
 *
 * @param    ms_ext_properties_qty   Quantity of Microsoft extended properties.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the Vendor class is initialized via the
 *               USBD_Vendor_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_Vendor_ConfigureMsExtPropertiesQty(CPU_INT08U ms_ext_properties_qty)
{
  RTOS_ASSERT_CRITICAL((USBD_VendorPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_Vendor_InitCfg.MsExtPropertiesQty = ms_ext_properties_qty;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_Vendor_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data.
 *
 * @param    p_mem_seg   Pointer to memory segment to use when allocating control data.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the Vendor class is initialized via the
 *               USBD_Vendor_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_Vendor_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((USBD_VendorPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_Vendor_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           USBD_Vendor_Init()
 *
 * @brief    Initializes the internal structures and variables used by the Vendor class.
 *
 * @param    p_qty_cfg   Pointer to the vendor class configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_Vendor_Init(USBD_VENDOR_QTY_CFG *p_qty_cfg,
                      RTOS_ERR            *p_err)
{
  CPU_INT08U       ix;
  USBD_VENDOR_CTRL *p_ctrl;
  USBD_VENDOR_COMM *p_comm;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ClassInstanceQty > 0u)
                           && (p_qty_cfg->ConfigQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  USBD_VendorPtr = (USBD_VENDOR *)Mem_SegAlloc("USBD - Vendor class root struct",
                                               USBD_Vendor_InitCfg.MemSegPtr,
                                               sizeof(USBD_VENDOR),
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_VendorPtr->CtrlNbrNext = p_qty_cfg->ClassInstanceQty;
  USBD_VendorPtr->CommNbrNext = p_qty_cfg->ClassInstanceQty * p_qty_cfg->ConfigQty;

  USBD_VendorPtr->CtrlTbl = (USBD_VENDOR_CTRL *)Mem_SegAlloc("USBD - Vendor ctrl table",
                                                             USBD_Vendor_InitCfg.MemSegPtr,
                                                             sizeof(USBD_VENDOR_CTRL) * USBD_VendorPtr->CtrlNbrNext,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_VendorPtr->CommTbl = (USBD_VENDOR_COMM *)Mem_SegAlloc("USBD - Vendor comm table",
                                                             USBD_Vendor_InitCfg.MemSegPtr,
                                                             sizeof(USBD_VENDOR_COMM) * USBD_VendorPtr->CommNbrNext,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < USBD_VendorPtr->CtrlNbrNext; ix++) {       // Init vendor class struct.
    p_ctrl = &USBD_VendorPtr->CtrlTbl[ix];
    p_ctrl->DevNbr = USBD_DEV_NBR_NONE;
    p_ctrl->State = USBD_VENDOR_STATE_NONE;
    p_ctrl->ClassNbr = USBD_CLASS_NBR_NONE;
    p_ctrl->CommPtr = DEF_NULL;
    p_ctrl->IntrEn = DEF_FALSE;
    p_ctrl->IntrInterval = 0u;
    p_ctrl->VendorReqCallbackPtr = DEF_NULL;

    p_ctrl->BulkRdAsyncFnct = DEF_NULL;
    p_ctrl->BulkRdAsyncArgPtr = DEF_NULL;
    p_ctrl->BulkWrAsyncFnct = DEF_NULL;
    p_ctrl->BulkWrAsyncArgPtr = DEF_NULL;
    p_ctrl->IntrRdAsyncFnct = DEF_NULL;
    p_ctrl->IntrRdAsyncArgPtr = DEF_NULL;
    p_ctrl->IntrWrAsyncFnct = DEF_NULL;
    p_ctrl->IntrWrAsyncArgPtr = DEF_NULL;

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
    p_ctrl->MS_ExtPropertyNext = 0u;

    p_ctrl->MS_ExtPropertyTbl = (USBD_MS_OS_EXT_PROPERTY *)Mem_SegAlloc("USBD - Vendor MS ext prop tbl",
                                                                        USBD_Vendor_InitCfg.MemSegPtr,
                                                                        sizeof(USBD_MS_OS_EXT_PROPERTY) * USBD_Vendor_InitCfg.MsExtPropertiesQty,
                                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr(p_ctrl->MS_ExtPropertyTbl,
            sizeof(USBD_MS_OS_EXT_PROPERTY) * USBD_Vendor_InitCfg.MsExtPropertiesQty);
#endif
  }

  //                                                               Init vendor EP tbl.
  for (ix = 0u; ix < USBD_VendorPtr->CommNbrNext; ix++) {
    p_comm = &USBD_VendorPtr->CommTbl[ix];
    p_comm->CtrlPtr = DEF_NULL;
    p_comm->DataBulkInEpAddr = USBD_EP_ADDR_NONE;
    p_comm->DataBulkOutEpAddr = USBD_EP_ADDR_NONE;
    p_comm->IntrInEpAddr = USBD_EP_ADDR_NONE;
    p_comm->IntrOutEpAddr = USBD_EP_ADDR_NONE;

    p_comm->DataBulkInActiveXfer = DEF_NO;
    p_comm->DataBulkOutActiveXfer = DEF_NO;
    p_comm->IntrInActiveXfer = DEF_NO;
    p_comm->IntrOutActiveXfer = DEF_NO;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBD_VendorPtr->ClassInstanceQty = p_qty_cfg->ClassInstanceQty;
#endif
}

/****************************************************************************************************//**
 *                                               USBD_Vendor_Add()
 *
 * @brief    Adds a new instance of the Vendor class.
 *
 * @param    intr_en         Interrupt endpoints IN and OUT flag:
 *                               - DEF_TRUE    Pair of interrupt endpoints added to interface.
 *                               - DEF_FALSE   Pair of interrupt endpoints not added to interface.
 *
 * @param    interval        Endpoint interval in milliseconds (must be a power of 2).
 *
 * @param    req_callback    Vendor-specific request callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this
 *                           function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *
 * @return   Class instance number, if no errors are returned.
 *           USBD_CLASS_NBR_NONE,   otherwise.
 *******************************************************************************************************/
CPU_INT08U USBD_Vendor_Add(CPU_BOOLEAN          intr_en,
                           CPU_INT16U           interval,
                           USBD_VENDOR_REQ_FNCT req_callback,
                           RTOS_ERR             *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_INT08U       vendor_class_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CLASS_NBR_NONE);

  if (intr_en == DEF_TRUE) {
    //                                                             Interval must be a power of 2.
    RTOS_ASSERT_DBG_ERR_SET((MATH_IS_PWR2(interval) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG, USBD_CLASS_NBR_NONE);
  }

  CORE_ENTER_ATOMIC();
  if (USBD_VendorPtr->CtrlNbrNext == 0u) {                      // Chk if max nbr of instances reached.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (USBD_CLASS_NBR_NONE);
  }

  USBD_VendorPtr->CtrlNbrNext--;                                // Next avail vendor class instance nbr.
  vendor_class_nbr = USBD_VendorPtr->CtrlNbrNext;               // Alloc new vendor class instance nbr.
  CORE_EXIT_ATOMIC();

  p_ctrl = &USBD_VendorPtr->CtrlTbl[vendor_class_nbr];          // Get vendor class instance.
                                                                // Store vendor class instance info.
  p_ctrl->IntrEn = intr_en;                                     // Intr EPs en/dis.
  p_ctrl->IntrInterval = interval;                              // Polling interval for intr EPs.
  p_ctrl->VendorReqCallbackPtr = req_callback;                  // App callback for vendor-specific req.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (vendor_class_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_ConfigAdd()
 *
 * @brief    Adds the Vendor class instance into the specified configuration (see Note #1).
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    config_nbr  Configuration index to which to add the Vendor class instance.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_USB_IF_ALT_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_USB_EP_NONE_AVAIL
 *
 * @note     (1) Called several times, it creates multiple instances and configurations.
 *               For instance, the following architecture could be created :
 *               @verbatim
 *               HS
 *               |-- Configuration 0
 *                   |-- Interface 0 (Vendor 0)
 *               |-- Configuration 1
 *                   |-- Interface 0 (Vendor 0)
 *                   |-- Interface 1 (Vendor 1)
 *               @endverbatim
 *               In that example, there are two instances of Vendor class: 'Vendor 0' and '1', and two
 *               possible configurations: 'Configuration 0' and '1'. 'Configuration 1' is composed
 *               of two interfaces. Each class instance has an association with one of the interfaces.
 *               If 'Configuration 1' is activated by the host, it allows the host to access two
 *               different functionalities offered by the device.
 *
 * @note     (2) Configuration Descriptor corresponding to a Vendor-specific device has the following
 *               format :
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (Vendor class)
 *                   |-- Endpoint Descriptor (Bulk OUT)
 *                   |-- Endpoint Descriptor (Bulk IN)
 *                   |-- Endpoint Descriptor (Interrupt OUT) - optional
 *                   |-- Endpoint Descriptor (Interrupt IN)  - optional
 *               @endverbatim
 *******************************************************************************************************/
void USBD_Vendor_ConfigAdd(CPU_INT08U class_nbr,
                           CPU_INT08U dev_nbr,
                           CPU_INT08U config_nbr,
                           RTOS_ERR   *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  USBD_VENDOR_COMM *p_comm;
  CPU_INT08U       if_nbr;
  CPU_INT08U       ep_addr;
  CPU_INT16U       comm_nbr;
  CPU_INT16U       intr_interval;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get vendor class instance.
  CORE_ENTER_ATOMIC();
  if ((p_ctrl->DevNbr != USBD_DEV_NBR_NONE)                     // Chk if class is associated with a different dev.
      && (p_ctrl->DevNbr != dev_nbr)) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  p_ctrl->DevNbr = dev_nbr;

  if (USBD_VendorPtr->CommNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return;
  }

  comm_nbr = USBD_VendorPtr->CommNbrNext - 1u;                  // Alloc new vendor class comm info nbr.
  USBD_VendorPtr->CommNbrNext--;                                // Next avail vendor class comm info nbr.
  CORE_EXIT_ATOMIC();

  p_comm = &USBD_VendorPtr->CommTbl[comm_nbr];                  // Get vendor class comm info.

  //                                                               ------------- CONFIG DESC CONSTRUCTION -------------
  //                                                               See Note #2.
  //                                                               Add vendor IF desc to config desc.
  if_nbr = USBD_IF_Add(dev_nbr,
                       config_nbr,
                       &USBD_Vendor_Drv,
                       (void *)p_comm,                          // EP comm struct associated to Vendor IF.
                       DEF_NULL,
                       USBD_CLASS_CODE_VENDOR_SPECIFIC,
                       USBD_SUBCLASS_CODE_VENDOR_SPECIFIC,
                       USBD_PROTOCOL_CODE_VENDOR_SPECIFIC,
                       "Vendor-specific class",
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               Add bulk IN EP desc.
  ep_addr = USBD_BulkAdd(dev_nbr,
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_YES,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_comm->DataBulkInEpAddr = ep_addr;                           // Store bulk IN EP addr.

  //                                                               Add bulk OUT EP desc.
  ep_addr = USBD_BulkAdd(dev_nbr,
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_NO,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_comm->DataBulkOutEpAddr = ep_addr;                          // Store bulk OUT EP addr.

  if (p_ctrl->IntrEn == DEF_TRUE) {
    if (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
      intr_interval = p_ctrl->IntrInterval;                     // In FS, bInterval in frames.
    } else {
      intr_interval = p_ctrl->IntrInterval * 8u;                // In HS, bInterval in microframes.
    }
    //                                                             Add intr IN EP desc.
    ep_addr = USBD_IntrAdd(dev_nbr,
                           config_nbr,
                           if_nbr,
                           0u,
                           DEF_YES,
                           0u,
                           intr_interval,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_comm->IntrInEpAddr = ep_addr;                             // Store intr IN EP addr.

    //                                                             Add intr OUT EP desc.
    ep_addr = USBD_IntrAdd(dev_nbr,
                           config_nbr,
                           if_nbr,
                           0u,
                           DEF_NO,
                           0u,
                           intr_interval,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_comm->IntrOutEpAddr = ep_addr;                            // Store intr OUT EP addr.
  }
  //                                                               Store vendor class instance info.
  CORE_ENTER_ATOMIC();
  p_ctrl->State = USBD_VENDOR_STATE_INIT;                       // Set class instance to init state.
  p_ctrl->ClassNbr = class_nbr;
  p_ctrl->CommPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();

  p_comm->CtrlPtr = p_ctrl;                                     // Save ref to vendor class instance ctrl struct.
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_IsConn()
 *
 * @brief    Gets the vendor class connection state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   DEF_YES, if the Vendor class is connected.
 *           DEF_NO,  if the Vendor class is NOT connected.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_Vendor_IsConn(CPU_INT08U class_nbr)
{
  USBD_VENDOR_CTRL *p_ctrl;
  USBD_DEV_STATE   state;
  RTOS_ERR         err;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.
  state = USBD_DevStateGet(p_ctrl->DevNbr, &err);               // Get dev state.

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (state == USBD_DEV_STATE_CONFIGURED)
      && (p_ctrl->State == USBD_VENDOR_STATE_CONFIG)) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                       USBD_Vendor_MS_ExtPropertyAdd()
 *
 * @brief    Adds a Microsoft OS extended property to this vendor class instance.
 *
 * @param    class_nbr           Class instance number.
 *
 * @param    property_type       Property type (see Note #2).
 *                                   - OS_PROPERTY_TYPE_REG_SZ
 *                                   - OS_PROPERTY_TYPE_REG_EXPAND_SZ
 *                                   - OS_PROPERTY_TYPE_REG_BINARY
 *                                   - OS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN
 *                                   - OS_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN
 *                                   - OS_PROPERTY_TYPE_REG_LINK
 *                                   - OS_PROPERTY_TYPE_REG_MULTI_SZ
 *
 * @param    p_property_name     Pointer to the buffer that contains the property name.
 *                               ---- Buffer assumed to be persistent ----
 *
 * @param    property_name_len   Length of the property name in octets.
 *
 * @param    p_property          _name     Pointer to the buffer that contains the property name.
 *                               ---- Buffer assumed to be persistent ----
 *
 * @param    property_len        Length of the property in octets.
 *
 * @param    p_err               Pointer to the variable that will receive this return error code from this
 *                               function :
 *                                   - RTOS_ERR_NONE
 *
 * @note     (1) For more information on Microsoft OS descriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) For more information on property types, refer to "Table 3. Property Data Types" of
 *               "Extended Properties OS Feature Descriptor Specification" document provided by
 *               Microsoft available at
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
void USBD_Vendor_MS_ExtPropertyAdd(CPU_INT08U       class_nbr,
                                   CPU_INT08U       property_type,
                                   const CPU_INT08U *p_property_name,
                                   CPU_INT16U       property_name_len,
                                   const CPU_INT08U *p_property,
                                   CPU_INT32U       property_len,
                                   RTOS_ERR         *p_err)
{
  CPU_INT08U              ext_property_nbr;
  USBD_VENDOR_CTRL        *p_ctrl;
  USBD_MS_OS_EXT_PROPERTY *p_ext_property;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  RTOS_ASSERT_DBG_ERR_SET(((property_type == USBD_MS_OS_PROPERTY_TYPE_REG_SZ)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_EXPAND_SZ)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_BINARY)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_LINK)
                           || (property_type == USBD_MS_OS_PROPERTY_TYPE_REG_MULTI_SZ)), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_property_name != DEF_NULL)
                           || (property_name_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_property != DEF_NULL)
                           || (property_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  CORE_ENTER_ATOMIC();
  ext_property_nbr = p_ctrl->MS_ExtPropertyNext;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (ext_property_nbr >= USBD_Vendor_InitCfg.MsExtPropertiesQty) {
    CORE_EXIT_ATOMIC();

    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_ALLOC,; );
  }
#endif
  p_ctrl->MS_ExtPropertyNext++;

  p_ext_property = &p_ctrl->MS_ExtPropertyTbl[ext_property_nbr];

  p_ext_property->PropertyType = property_type;
  p_ext_property->PropertyNamePtr = p_property_name;
  p_ext_property->PropertyNameLen = property_name_len;
  p_ext_property->PropertyPtr = p_property;
  p_ext_property->PropertyLen = property_len;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_Vendor_Rd()
 *
 * @brief    Receive the data from the host through the Bulk OUT endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive the buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_CLASS_STATE
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors are returned.
 *******************************************************************************************************/
CPU_INT32U USBD_Vendor_Rd(CPU_INT08U class_nbr,
                          void       *p_buf,
                          CPU_INT32U buf_len,
                          CPU_INT16U timeout,
                          RTOS_ERR   *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_INT32U       xfer_len;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  xfer_len = USBD_BulkRx(p_ctrl->DevNbr,
                         p_ctrl->CommPtr->DataBulkOutEpAddr,
                         p_buf,
                         buf_len,
                         timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                               USBD_Vendor_Wr()
 *
 * @brief    Sends the data to host through Bulk IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit the buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_CLASS_STATE
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets sent, if no errors are returned.
 *           0,                     if any errors are returned.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
CPU_INT32U USBD_Vendor_Wr(CPU_INT08U  class_nbr,
                          void        *p_buf,
                          CPU_INT32U  buf_len,
                          CPU_INT16U  timeout,
                          CPU_BOOLEAN end,
                          RTOS_ERR    *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_INT32U       xfer_len;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  xfer_len = USBD_BulkTx(p_ctrl->DevNbr,
                         p_ctrl->CommPtr->DataBulkInEpAddr,
                         p_buf,
                         buf_len,
                         timeout,
                         end,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_RdAsync()
 *
 * @brief    Receive the data from the host through the Bulk OUT endpoint. This function is non-blocking
 *           are returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the receive buffer.
 *
 * @param    buf_len         Receive buffer length in octets.
 *
 * @param    async_fnct      Receive the the callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the receive callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_Vendor_RdAsync(CPU_INT08U             class_nbr,
                         void                   *p_buf,
                         CPU_INT32U             buf_len,
                         USBD_VENDOR_ASYNC_FNCT async_fnct,
                         void                   *p_async_arg,
                         RTOS_ERR               *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  p_ctrl->BulkRdAsyncFnct = async_fnct;
  p_ctrl->BulkRdAsyncArgPtr = p_async_arg;

  if (p_ctrl->CommPtr->DataBulkOutActiveXfer == DEF_NO) {       // Check if another xfer is already in progress.
    p_ctrl->CommPtr->DataBulkOutActiveXfer = DEF_YES;           // Indicate that a xfer is in progres.
    USBD_BulkRxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->DataBulkOutEpAddr,
                     p_buf,
                     buf_len,
                     USBD_Vendor_RdAsyncCmpl,
                     (void *)p_ctrl,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_ctrl->CommPtr->DataBulkOutActiveXfer = DEF_NO;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
  }
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_WrAsync()
 *
 * @brief    Sends the data to host through Bulk IN endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a
 *           callback provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the transmit buffer.
 *
 * @param    buf_len         Transmit buffer length in octets.
 *
 * @param    async_fnct      Transmit the callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the transmit callback.
 *
 * @param    end             End-of-transfer flag (see Note #1).
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
void USBD_Vendor_WrAsync(CPU_INT08U             class_nbr,
                         void                   *p_buf,
                         CPU_INT32U             buf_len,
                         USBD_VENDOR_ASYNC_FNCT async_fnct,
                         void                   *p_async_arg,
                         CPU_BOOLEAN            end,
                         RTOS_ERR               *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  p_ctrl->BulkWrAsyncFnct = async_fnct;
  p_ctrl->BulkWrAsyncArgPtr = p_async_arg;

  if (p_ctrl->CommPtr->DataBulkInActiveXfer == DEF_NO) {        // Check if another xfer is already in progress.
    p_ctrl->CommPtr->DataBulkInActiveXfer = DEF_YES;            // Indicate that a xfer is in progres.
    USBD_BulkTxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->DataBulkInEpAddr,
                     p_buf,
                     buf_len,
                     USBD_Vendor_WrAsyncCmpl,
                     (void *)p_ctrl,
                     end,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_ctrl->CommPtr->DataBulkInActiveXfer = DEF_NO;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
  }
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_IntrRd()
 *
 * @brief    Receives the data from the host through the Interrupt OUT endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_CLASS_STATE
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors are returned.
 *******************************************************************************************************/
CPU_INT32U USBD_Vendor_IntrRd(CPU_INT08U class_nbr,
                              void       *p_buf,
                              CPU_INT32U buf_len,
                              CPU_INT16U timeout,
                              RTOS_ERR   *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_INT32U       xfer_len;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  xfer_len = USBD_IntrRx(p_ctrl->DevNbr,
                         p_ctrl->CommPtr->IntrOutEpAddr,
                         p_buf,
                         buf_len,
                         timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_IntrWr()
 *
 * @brief    Sends data to the host through the Interrupt IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_CLASS_STATE
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets sent, if no errors are returned.
 *           0,                     if any errors are returned.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
CPU_INT32U USBD_Vendor_IntrWr(CPU_INT08U  class_nbr,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT16U  timeout,
                              CPU_BOOLEAN end,
                              RTOS_ERR    *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_INT32U       xfer_len;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  xfer_len = USBD_IntrTx(p_ctrl->DevNbr,
                         p_ctrl->CommPtr->IntrInEpAddr,
                         p_buf,
                         buf_len,
                         timeout,
                         end,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_IntrRdAsync()
 *
 * @brief    Receives the data from the host through Interrupt OUT endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the receive buffer.
 *
 * @param    buf_len         Receive the buffer length in octets.
 *
 * @param    async_fnct      Receive the callback.
 *
 * @param    p_async_arg     Additional argument provided by application for the receive callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_Vendor_IntrRdAsync(CPU_INT08U             class_nbr,
                             void                   *p_buf,
                             CPU_INT32U             buf_len,
                             USBD_VENDOR_ASYNC_FNCT async_fnct,
                             void                   *p_async_arg,
                             RTOS_ERR               *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  p_ctrl->IntrRdAsyncFnct = async_fnct;
  p_ctrl->IntrRdAsyncArgPtr = p_async_arg;

  if (p_ctrl->CommPtr->IntrOutActiveXfer == DEF_NO) {           // Check if another xfer is already in progress.
    p_ctrl->CommPtr->IntrOutActiveXfer = DEF_YES;               // Indicate that a xfer is in progres.
    USBD_IntrRxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->IntrOutEpAddr,
                     p_buf,
                     buf_len,
                     USBD_Vendor_IntrRdAsyncCmpl,
                     (void *)p_ctrl,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_ctrl->CommPtr->IntrOutActiveXfer = DEF_NO;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
  }
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_IntrWrAsync()
 *
 * @brief    Send the data to the host through the Interrupt IN endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the transmit buffer.
 *
 * @param    buf_len         Transmit buffer length in octets.
 *
 * @param    async_fnct      Transmit callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the transmit callback.
 *
 * @param    end             End-of-transfer flag (see Note #1).
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
void USBD_Vendor_IntrWrAsync(CPU_INT08U             class_nbr,
                             void                   *p_buf,
                             CPU_INT32U             buf_len,
                             USBD_VENDOR_ASYNC_FNCT async_fnct,
                             void                   *p_async_arg,
                             CPU_BOOLEAN            end,
                             RTOS_ERR               *p_err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  CPU_BOOLEAN      conn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_VendorPtr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  conn = USBD_Vendor_IsConn(class_nbr);
  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  p_ctrl = &USBD_VendorPtr->CtrlTbl[class_nbr];                 // Get Vendor class instance ctrl struct.

  p_ctrl->IntrWrAsyncFnct = async_fnct;
  p_ctrl->IntrWrAsyncArgPtr = p_async_arg;

  if (p_ctrl->CommPtr->IntrInActiveXfer == DEF_NO) {            // Check if another xfer is already in progress.
    p_ctrl->CommPtr->IntrInActiveXfer = DEF_YES;                // Indicate that a xfer is in progres.
    USBD_IntrTxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->IntrInEpAddr,
                     p_buf,
                     buf_len,
                     USBD_Vendor_IntrWrAsyncCmpl,
                     (void *)p_ctrl,
                     end,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_ctrl->CommPtr->IntrInActiveXfer = DEF_NO;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_Vendor_Conn()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface to.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void USBD_Vendor_Conn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg)
{
  USBD_VENDOR_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;

  p_comm = (USBD_VENDOR_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = p_comm;
  p_comm->CtrlPtr->State = USBD_VENDOR_STATE_CONFIG;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_Disconn()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void USBD_Vendor_Disconn(CPU_INT08U dev_nbr,
                                CPU_INT08U config_nbr,
                                void       *p_if_class_arg)
{
  USBD_VENDOR_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;

  p_comm = (USBD_VENDOR_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->State = USBD_VENDOR_STATE_INIT;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_VendorReq()
 *
 * @brief    Process vendor-specific request.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument passed to USBD_IF_Add().
 *
 * @return   DEF_OK,   if vendor-specific request successfully processed.
 *
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_Vendor_VendorReq(CPU_INT08U           dev_nbr,
                                         const USBD_SETUP_REQ *p_setup_req,
                                         void                 *p_if_class_arg)
{
  USBD_VENDOR_COMM *p_comm;
  CPU_BOOLEAN      valid;

  (void)&dev_nbr;

  p_comm = (USBD_VENDOR_COMM *)p_if_class_arg;
  //                                                               Process req if callback avail.
  if (p_comm->CtrlPtr->VendorReqCallbackPtr != DEF_NULL) {
    valid = p_comm->CtrlPtr->VendorReqCallbackPtr(p_comm->CtrlPtr->ClassNbr,
                                                  p_comm->CtrlPtr->DevNbr,
                                                  p_setup_req);
  } else {
    valid = DEF_FAIL;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                       USBD_Vendor_MS_GetCompatID()
 *
 * @brief    Returns Microsoft descriptor compatible ID.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    p_sub_compat_id_ix  Pointer to the variable that will receive subcompatible ID.
 *
 * @return   Compatible ID.
 *
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_INT08U USBD_Vendor_MS_GetCompatID(CPU_INT08U dev_nbr,
                                             CPU_INT08U *p_sub_compat_id_ix)
{
  (void)&dev_nbr;

  *p_sub_compat_id_ix = USBD_MS_OS_SUBCOMPAT_ID_NULL;

  return (USBD_MS_OS_COMPAT_ID_WINUSB);
}
#endif

/****************************************************************************************************//**
 *                                   USBD_Vendor_MS_GetExtPropertyTbl()
 *
 * @brief    Returns Microsoft descriptor extended properties table.
 *
 * @param    dev_nbr                 Device number.
 *
 * @param    pp_ext_property_tbl     Pointer to the variable that will receive the Microsoft extended
 *                                   properties table.
 *
 * @return   Number of Microsoft extended properties in table.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_INT08U USBD_Vendor_MS_GetExtPropertyTbl(CPU_INT08U              dev_nbr,
                                                   USBD_MS_OS_EXT_PROPERTY **pp_ext_property_tbl)
{
  USBD_VENDOR_CTRL *p_ctrl;

  p_ctrl = &USBD_VendorPtr->CtrlTbl[dev_nbr];                   // Get Vendor class instance ctrl struct.
  *pp_ext_property_tbl = p_ctrl->MS_ExtPropertyTbl;

  return (p_ctrl->MS_ExtPropertyNext);                          // Only one extended property (GUID) supported.
}
#endif

/****************************************************************************************************//**
 *                                           USBD_Vendor_RdAsyncCmpl()
 *
 * @brief    Inform the application about the Bulk OUT transfer completion.
 *
 * @param    dev_nbr     Device number
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_Vendor_RdAsyncCmpl(CPU_INT08U dev_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  void             *p_callback_arg;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_ctrl = (USBD_VENDOR_CTRL *)p_arg;                           // Get Vendor class instance ctrl struct.
  p_callback_arg = p_ctrl->BulkRdAsyncArgPtr;

  p_ctrl->CommPtr->DataBulkOutActiveXfer = DEF_NO;              // Xfer finished, no more active xfer.
  p_ctrl->BulkRdAsyncFnct(p_ctrl->ClassNbr,                     // Call app callback to inform about xfer completion.
                          p_buf,
                          buf_len,
                          xfer_len,
                          p_callback_arg,
                          err);
}

/****************************************************************************************************//**
 *                                           USBD_Vendor_WrAsyncCmpl()
 *
 * @brief    Inform the application about the Bulk IN transfer completion.
 *
 * @param    dev_nbr     Device number
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length.
 *
 * @param    xfer_len    Number of octets sent.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_Vendor_WrAsyncCmpl(CPU_INT08U dev_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  void             *p_callback_arg;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_ctrl = (USBD_VENDOR_CTRL *)p_arg;                           // Get Vendor class instance ctrl struct.
  p_callback_arg = p_ctrl->BulkWrAsyncArgPtr;

  p_ctrl->CommPtr->DataBulkInActiveXfer = DEF_NO;               // Xfer finished, no more active xfer.
  p_ctrl->BulkWrAsyncFnct(p_ctrl->ClassNbr,                     // Call app callback to inform about xfer completion.
                          p_buf,
                          buf_len,
                          xfer_len,
                          p_callback_arg,
                          err);
}

/****************************************************************************************************//**
 *                                       USBD_Vendor_IntrRdAsyncCmpl()
 *
 * @brief    Inform the application about the Interrupt OUT transfer completion.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_Vendor_IntrRdAsyncCmpl(CPU_INT08U dev_nbr,
                                        CPU_INT08U ep_addr,
                                        void       *p_buf,
                                        CPU_INT32U buf_len,
                                        CPU_INT32U xfer_len,
                                        void       *p_arg,
                                        RTOS_ERR   err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  void             *p_callback_arg;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_ctrl = (USBD_VENDOR_CTRL *)p_arg;                           // Get Vendor class instance ctrl struct.
  p_callback_arg = p_ctrl->IntrRdAsyncArgPtr;

  p_ctrl->CommPtr->IntrOutActiveXfer = DEF_NO;                  // Xfer finished, no more active xfer.
  p_ctrl->IntrRdAsyncFnct(p_ctrl->ClassNbr,                     // Call app callback to inform about xfer completion.
                          p_buf,
                          buf_len,
                          xfer_len,
                          p_callback_arg,
                          err);
}

/****************************************************************************************************//**
 *                                       USBD_Vendor_IntrWrAsyncCmpl()
 *
 * @brief    Inform the application about the Interrupt IN transfer completion.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length.
 *
 * @param    xfer_len    Number of octets sent.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_Vendor_IntrWrAsyncCmpl(CPU_INT08U dev_nbr,
                                        CPU_INT08U ep_addr,
                                        void       *p_buf,
                                        CPU_INT32U buf_len,
                                        CPU_INT32U xfer_len,
                                        void       *p_arg,
                                        RTOS_ERR   err)
{
  USBD_VENDOR_CTRL *p_ctrl;
  void             *p_callback_arg;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_ctrl = (USBD_VENDOR_CTRL *)p_arg;                           // Get Vendor class instance ctrl struct.
  p_callback_arg = p_ctrl->IntrWrAsyncArgPtr;

  p_ctrl->CommPtr->IntrInActiveXfer = DEF_NO;                   // Xfer finished, no more active xfer.
  p_ctrl->IntrWrAsyncFnct(p_ctrl->ClassNbr,                     // Call app callback to inform about xfer completion.
                          p_buf,
                          buf_len,
                          xfer_len,
                          p_callback_arg,
                          err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL))
