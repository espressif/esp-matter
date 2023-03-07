/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (Cdc)
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
 * @note   (1) This implementation is compliant with the CDC specification revision 1.2
 *             errata 1. November 3, 2010.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_CDC_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device CDC class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#include  <em_core.h>
#include  <usb/include/device/usbd_cdc.h>
#include  <common/include/lib_math.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (USBD, CLASS, CDC)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 *                                   CDC FUNCTIONAL DESCRIPTOR SIZE
 *******************************************************************************************************/

#define  USBD_CDC_DESC_SIZE_HEADER                       5u     // Header functional desc size.
#define  USBD_CDC_DESC_SIZE_UNION_MIN                    4u     // Min size of union functional desc.

/********************************************************************************************************
 *                                       CDC TOTAL NUMBER DEFINES
 *******************************************************************************************************/

#define  USBD_CDC_NBR_TOTAL                     (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_CDC_DATA_IF_NBR_TOTAL             (DEF_INT_08U_MAX_VAL - 1u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CDC STATE DATA TYPE
 *******************************************************************************************************/

typedef enum usbd_cdc_state {                                   // CDC device state.
  USBD_CDC_STATE_NONE = 0,
  USBD_CDC_STATE_INIT,
  USBD_CDC_STATE_CONFIG
} USBD_CDC_STATE;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_cdc_ctrl USBD_CDC_CTRL;
typedef struct usbd_cdc_data_if USBD_CDC_DATA_IF;

/********************************************************************************************************
 *                                       CDC DATA IF CLASS DATA TYPE
 *
 * Note(s) : (1) USB CDC specification specifies that the type of endpoints belonging to a Data IF are
 *               restricted to being either isochronous or bulk, and are expected to exist in pairs of
 *               the same type (one IN and one OUT).
 *******************************************************************************************************/

typedef struct usbd_cdc_data_if_ep {
  CPU_INT08U DataIn;
  CPU_INT08U DataOut;
} USBD_CDC_DATA_IF_EP;

struct usbd_cdc_data_if {
  CPU_INT08U       IF_Nbr;                                      ///< Data IF nbr.
  CPU_INT08U       Protocol;                                    ///< Data IF protocol.
  CPU_BOOLEAN      IsocEn;                                      ///< EP isochronous enable.
  USBD_CDC_DATA_IF *NextPtr;                                    ///< Next data IF.
};

/****************************************************************************************************//**
 *                                   CDC COMMUNICATION IF DATA TYPE
 *
 * @note     (1) A CDC device consists in one communication IF, and multiple data IFs (optional).
 *                   @verbatim
 *                       +-----IFs----+-------EPs-------+
 *                       |  COMM_IF   |  CTRL           | <--------  Mgmt   Requests.
 *                       |            |  INTR (Optional)| ---------> Events Notifications.
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (0)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (0)
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (1)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (1)
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (2)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (2)
 *                       +------------+-----------------+
 *                                   .
 *                                   .
 *                                   .
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (n - 1)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (n - 1)
 *                       +------------+-----------------+
 *                   @endverbatim
 *               - (a)  The communication IF may have an optional notification element. Notifications are
 *                       sent using a interrupt endpoint.
 *               - (b)  The communication IF structure contains a link list of Data IFs.
 * @{
 *******************************************************************************************************/
//                                                                 ------ CDC DATA CLASS INTERFACE COMMUNICATION ------
typedef struct usbd_cdc_comm {
  CPU_INT08U    DevNbr;                                         ///< Dev nbr.
  CPU_INT08U    CCI_IF_Nbr;                                     ///< Comm Class IF nbr.
  USBD_CDC_CTRL *CtrlPtr;                                       ///< Ptr to ctrl info.
  CPU_INT08U    NotifyIn;                                       ///< Notification EP (see note #1a).
  CPU_INT16U    DataIF_EP_Ix;                                   ///< Start ix of data IFs EP information.
  CPU_BOOLEAN   NotifyInActiveXfer;
} USBD_CDC_COMM;

struct usbd_cdc_ctrl {                                          // ----------- CDC CLASS CONTROL INFORMATION ----------
  USBD_CDC_STATE        State;                                  ///< CDC state.
  CPU_BOOLEAN           NotifyEn;                               ///< CDC mgmt element notifications enable.
  CPU_INT16U            NotifyInterval;                         ///< CDC mgmt element notifications interval.
  CPU_INT08U            DataIF_Nbr;                             ///< Number of data IFs.
  USBD_CDC_DATA_IF      *DataIF_HeadPtr;                        ///< Data IFs list head ptr. (see note #1b)
  USBD_CDC_DATA_IF      *DataIF_TailPtr;                        ///< Data IFs list tail ptr.
  CPU_INT08U            SubClassCode;                           ///< CDC subclass code.
  CPU_INT08U            SubClassProtocol;                       ///< CDC subclass protocol.
  USBD_CDC_SUBCLASS_DRV *SubClassDrvPtr;                        ///< CDC subclass drv.
  void                  *SubClassArg;                           ///< CDC subclass drv argument.
  USBD_CDC_COMM         *CommPtr;                               ///< CDC comm information ptr.
};

///< @}

/*
 ********************************************************************************************************
 *                                       CDC BASE CLASS ROOT STRUCT
 *******************************************************************************************************/

typedef struct usbd_cdc {
  USBD_CDC_CTRL       *CtrlTbl;                                 ///< Ctrl struct table.
  CPU_INT08U          CtrlNbrNext;                              ///< Next ctrl struct index.

  USBD_CDC_COMM       *CommTbl;                                 ///< Comm struct table.
  CPU_INT16U          CommNbrNext;                              ///< Next comm struct index.

  USBD_CDC_DATA_IF    *DataIF_Tbl;                              ///< Data interface struct table.
  CPU_INT08U          DataIF_NbrNext;                           ///< Next data interface struct index.

  USBD_CDC_DATA_IF_EP *DataIF_EP_Tbl;                           ///< Data interface endpoint struct table.
  CPU_INT16U          DataIF_EP_NbrNext;                        ///< Next data interface endpoint struct index.
  CPU_INT16U          DataIF_EP_Qty;                            ///< Quantity of data interface endpoint.

#if (RTOS_ARG_CHK_EXT_EN)
  CPU_INT08U          ClassInstanceQty;                         ///< Quantity of class instance.
#endif
} USBD_CDC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_CDC_INIT_CFG USBD_CDC_InitCfgDflt = {
  .MemSegPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBD_CDC *USBD_CDC_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_CDC_INIT_CFG USBD_CDC_InitCfg = {
  .MemSegPtr = DEF_NULL
};
#else
extern USBD_CDC_INIT_CFG USBD_CDC_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_CDC_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg);

static void USBD_CDC_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg);

static void USBD_CDC_CommIF_Desc(CPU_INT08U dev_nbr,
                                 CPU_INT08U config_nbr,
                                 CPU_INT08U if_nbr,
                                 CPU_INT08U if_alt_nbr,
                                 void       *p_if_class_arg,
                                 void       *p_if_alt_class_arg);

static CPU_INT16U USBD_CDC_CommIF_DescSizeGet(CPU_INT08U dev_nbr,
                                              CPU_INT08U config_nbr,
                                              CPU_INT08U if_nbr,
                                              CPU_INT08U if_alt_nbr,
                                              void       *p_if_class_arg,
                                              void       *p_if_alt_class_arg);

static CPU_BOOLEAN USBD_CDC_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg);

static void USBD_CDC_NotifyCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err);

/********************************************************************************************************
 *                                           CDC CLASS DRIVERS
 *******************************************************************************************************/
//                                                                 Comm IF class drv.
static USBD_CLASS_DRV USBD_CDC_CommDrv = {
  USBD_CDC_Conn,
  USBD_CDC_Disconn,
  DEF_NULL,
  DEF_NULL,
  USBD_CDC_CommIF_Desc,
  USBD_CDC_CommIF_DescSizeGet,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  USBD_CDC_ClassReq,
  DEF_NULL,

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  DEF_NULL,
  DEF_NULL
#endif
};

//                                                                 Data IF class drv.
static USBD_CLASS_DRV USBD_CDC_DataDrv = {
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  DEF_NULL,
  DEF_NULL
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           APPLICATION FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBD_CDC_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data.
 *
 * @param    p_mem_seg   Pointer to memory segment to use when allocating control data.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC class is initialized via the
 *               USBD_CDC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               USBD_CDC_Init()
 *
 * @brief    Initializes CDC class.
 *
 * @param    p_qty_cfg   Pointer to CDC bas class configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_CDC_Init(USBD_CDC_QTY_CFG *p_qty_cfg,
                   RTOS_ERR         *p_err)
{
  CPU_INT08U          ix;
  USBD_CDC_CTRL       *p_ctrl;
  USBD_CDC_COMM       *p_comm;
  USBD_CDC_DATA_IF    *p_data_if;
  USBD_CDC_DATA_IF_EP *p_data_ep;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ClassInstanceQty > 0u)
                           && (p_qty_cfg->ConfigQty >= p_qty_cfg->ClassInstanceQty)
                           && (p_qty_cfg->DataIF_Qty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  USBD_CDC_Ptr = (USBD_CDC *)Mem_SegAlloc("USBD - CDC root struct",
                                          USBD_CDC_InitCfg.MemSegPtr,
                                          sizeof(USBD_CDC),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_Ptr->CtrlNbrNext = p_qty_cfg->ClassInstanceQty;
  USBD_CDC_Ptr->CommNbrNext = p_qty_cfg->ClassInstanceQty * p_qty_cfg->ConfigQty;
  USBD_CDC_Ptr->DataIF_NbrNext = p_qty_cfg->DataIF_Qty;
  USBD_CDC_Ptr->DataIF_EP_NbrNext = 0u;
  USBD_CDC_Ptr->DataIF_EP_Qty = p_qty_cfg->ClassInstanceQty * p_qty_cfg->ConfigQty * p_qty_cfg->DataIF_Qty;

#if (RTOS_ARG_CHK_EXT_EN)
  USBD_CDC_Ptr->ClassInstanceQty = p_qty_cfg->ClassInstanceQty;
#endif

  USBD_CDC_Ptr->CtrlTbl = (USBD_CDC_CTRL *)Mem_SegAlloc("USBD - CDC ctrl table",
                                                        USBD_CDC_InitCfg.MemSegPtr,
                                                        sizeof(USBD_CDC_CTRL) * USBD_CDC_Ptr->CtrlNbrNext,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_Ptr->CommTbl = (USBD_CDC_COMM *)Mem_SegAlloc("USBD - CDC comm table",
                                                        USBD_CDC_InitCfg.MemSegPtr,
                                                        sizeof(USBD_CDC_COMM) * USBD_CDC_Ptr->CommNbrNext,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_Ptr->DataIF_Tbl = (USBD_CDC_DATA_IF *)Mem_SegAlloc("USBD - CDC data IF table",
                                                              USBD_CDC_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CDC_DATA_IF) * USBD_CDC_Ptr->DataIF_NbrNext,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_Ptr->DataIF_EP_Tbl = (USBD_CDC_DATA_IF_EP *)Mem_SegAlloc("USBD - CDC data IF EP table",
                                                                    USBD_CDC_InitCfg.MemSegPtr,
                                                                    sizeof(USBD_CDC_DATA_IF_EP) * USBD_CDC_Ptr->DataIF_EP_Qty,
                                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < USBD_CDC_Ptr->CtrlNbrNext; ix++) {         // Init CDC ctrl tbl.
    p_ctrl = &USBD_CDC_Ptr->CtrlTbl[ix];
    p_ctrl->State = USBD_CDC_STATE_NONE;
    p_ctrl->NotifyEn = DEF_DISABLED;
    p_ctrl->NotifyInterval = 0u;
    p_ctrl->DataIF_Nbr = 0u;
    p_ctrl->DataIF_HeadPtr = DEF_NULL;
    p_ctrl->DataIF_TailPtr = DEF_NULL;
    p_ctrl->SubClassCode = USBD_CDC_SUBCLASS_RSVD;
    p_ctrl->SubClassProtocol = USBD_CDC_COMM_PROTOCOL_NONE;
    p_ctrl->SubClassDrvPtr = DEF_NULL;
    p_ctrl->SubClassArg = DEF_NULL;
    p_ctrl->CommPtr = DEF_NULL;
  }

  for (ix = 0u; ix < USBD_CDC_Ptr->CommNbrNext; ix++) {         // Init CDC comm tbl.
    p_comm = &USBD_CDC_Ptr->CommTbl[ix];
    p_comm->CtrlPtr = DEF_NULL;
    p_comm->NotifyIn = USBD_EP_ADDR_NONE;
    p_comm->DataIF_EP_Ix = 0u;
    p_comm->CCI_IF_Nbr = USBD_IF_NBR_NONE;
    p_comm->NotifyInActiveXfer = DEF_NO;
  }

  for (ix = 0u; ix < USBD_CDC_Ptr->DataIF_NbrNext; ix++) {      // Init CDC data IF tbl.
    p_data_if = &USBD_CDC_Ptr->DataIF_Tbl[ix];
    p_data_if->Protocol = USBD_CDC_DATA_PROTOCOL_NONE;
    p_data_if->IsocEn = DEF_DISABLED;
    p_data_if->NextPtr = DEF_NULL;
    p_data_if->IF_Nbr = USBD_IF_NBR_NONE;
  }

  for (ix = 0u; ix < USBD_CDC_Ptr->DataIF_EP_NbrNext; ix++) {   // Init CDC data IF EP tbl.
    p_data_ep = &USBD_CDC_Ptr->DataIF_EP_Tbl[ix];
    p_data_ep->DataIn = USBD_EP_ADDR_NONE;
    p_data_ep->DataOut = USBD_EP_ADDR_NONE;
  }
}

/****************************************************************************************************//**
 *                                               USBD_CDC_Add()
 *
 * @brief    Adds a new instance of the CDC class.
 *
 * @param    subclass            Communication class subclass subcode (see Note #1).
 *
 * @param    p_subclass_drv      Pointer to the CDC subclass driver.
 *
 * @param    p_subclass_arg      Pointer to the CDC subclass driver argument.
 *
 * @param    protocol            Communication class protocol code.
 *
 * @param    notify_en           Notification enabled :
 *                               DEF_ENABLED   Enable  CDC class notifications.
 *                               DEF_DISABLED  Disable CDC class notifications.
 *
 * @param    notify_interval     Notification interval in milliseconds (must be a power of 2).
 *
 * @param    p_err               Pointer to the variable that will receive one of these returned error codes from this
 *                               function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *
 * @return   CDC class instance number, if no errors are returned.
 *           USBD_CDC_NBR_NONE,         if any errors returned.
 *
 * @note     (1) Communication class subclass codes are defined in 'usbd_cdc.h'
 *               'USBD_CDC_SUBCLASS_XXXX'.
 *******************************************************************************************************/
CPU_INT08U USBD_CDC_Add(CPU_INT08U            subclass,
                        USBD_CDC_SUBCLASS_DRV *p_subclass_drv,
                        void                  *p_subclass_arg,
                        CPU_INT08U            protocol,
                        CPU_BOOLEAN           notify_en,
                        CPU_INT16U            notify_interval,
                        RTOS_ERR              *p_err)
{
  CPU_INT08U    cdc_nbr;
  USBD_CDC_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CDC_NBR_NONE);

  //                                                               Interval must be a power of 2.
  RTOS_ASSERT_DBG_ERR_SET(((notify_en != DEF_ENABLED)
                           || (MATH_IS_PWR2(notify_interval) != DEF_NO)), *p_err, RTOS_ERR_INVALID_ARG, USBD_CDC_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_CDC_Ptr->CtrlNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (USBD_CDC_NBR_NONE);
  }

  USBD_CDC_Ptr->CtrlNbrNext--;
  cdc_nbr = USBD_CDC_Ptr->CtrlNbrNext;                          // Alloc new CDC class.
  CORE_EXIT_ATOMIC();

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[cdc_nbr];                     // Get & init CDC struct.

  p_ctrl->SubClassCode = subclass;
  p_ctrl->SubClassProtocol = protocol;
  p_ctrl->NotifyEn = notify_en;
  p_ctrl->NotifyInterval = notify_interval;
  p_ctrl->SubClassDrvPtr = p_subclass_drv;
  p_ctrl->SubClassArg = p_subclass_arg;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cdc_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_ConfigAdd()
 *
 * @brief    Adds a CDC instance into the USB device configuration.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    config_nbr  Configuration index to which to add the new CDC class interface.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_USB_IF_ALT_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_USB_IF_GRP_ALLOC
 *                           - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *                           - RTOS_ERR_ALREADY_EXISTS
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_USB_EP_NONE_AVAIL
 *
 * @return   DEF_YES, if the CDC class instance was added to USB device configuration successfully.
 *           DEF_NO,  if the instance was not added successfully.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_CDC_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err)
{
  USBD_CDC_CTRL       *p_ctrl;
  USBD_CDC_COMM       *p_comm;
  USBD_CDC_DATA_IF_EP *p_data_ep;
  USBD_CDC_DATA_IF    *p_data_if;
  CPU_INT08U          if_nbr;
  CPU_INT08U          ep_addr;
  CPU_INT16U          comm_nbr;
  CPU_INT16U          data_if_nbr_cur = 0u;
  CPU_INT16U          data_if_nbr_end = 0u;
  CPU_INT16U          data_if_ix;
  CPU_INT16U          notify_interval;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];

  CORE_ENTER_ATOMIC();
  if (USBD_CDC_Ptr->CommNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (DEF_NO);
  }

  USBD_CDC_Ptr->CommNbrNext--;
  comm_nbr = USBD_CDC_Ptr->CommNbrNext;                         // Alloc CDC class comm info.

  p_comm = &USBD_CDC_Ptr->CommTbl[comm_nbr];

  if (p_ctrl->DataIF_Nbr > 0u) {
    data_if_nbr_cur = USBD_CDC_Ptr->DataIF_EP_NbrNext;          // Alloc data IFs EP struct.
    data_if_nbr_end = data_if_nbr_cur + p_ctrl->DataIF_Nbr;
    if (data_if_nbr_end > USBD_CDC_Ptr->DataIF_EP_Qty) {
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
      return (DEF_NO);
    }

    USBD_CDC_Ptr->DataIF_EP_NbrNext = data_if_nbr_end;
  }
  CORE_EXIT_ATOMIC();

  if_nbr = USBD_IF_Add(dev_nbr,                                 // Add CDC comm IF to config.
                       config_nbr,
                       &USBD_CDC_CommDrv,
                       (void *)p_comm,
                       DEF_NULL,
                       USBD_CLASS_CODE_CDC_CONTROL,
                       p_ctrl->SubClassCode,
                       p_ctrl->SubClassProtocol,
                       "CDC Comm Interface",
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  p_comm->CCI_IF_Nbr = if_nbr;

  if (p_ctrl->NotifyEn == DEF_TRUE) {
    if (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
      notify_interval = p_ctrl->NotifyInterval;                 // In FS, bInterval in frames.
    } else {
      notify_interval = p_ctrl->NotifyInterval * 8u;            // In HS, bInterval in microframes.
    }

    ep_addr = USBD_IntrAdd(dev_nbr,                             // Add interrupt (IN) EP for notifications.
                           config_nbr,
                           if_nbr,
                           0u,
                           DEF_YES,
                           0u,
                           notify_interval,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NO);
    }

    p_comm->NotifyIn = ep_addr;
  }

  if (p_ctrl->DataIF_Nbr > 0u) {                                // Add CDC data IFs to config.
    p_comm->DataIF_EP_Ix = data_if_nbr_cur;
    p_data_if = p_ctrl->DataIF_HeadPtr;

    for (data_if_ix = data_if_nbr_cur; data_if_ix < data_if_nbr_end; data_if_ix++) {
      p_data_ep = &USBD_CDC_Ptr->DataIF_EP_Tbl[data_if_ix];

      //                                                           Add CDC data IF to config.
      if_nbr = USBD_IF_Add(dev_nbr,
                           config_nbr,
                           &USBD_CDC_DataDrv,
                           (void *)p_comm,
                           DEF_NULL,
                           USBD_CLASS_CODE_CDC_DATA,
                           USBD_SUBCLASS_CODE_USE_IF_DESC,
                           p_data_if->Protocol,
                           "CDC Data Interface",
                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_NO);
      }

      p_data_if->IF_Nbr = if_nbr;

      if (p_data_if->IsocEn == DEF_DISABLED) {
        //                                                         Add Bulk IN EP.
        ep_addr = USBD_BulkAdd(dev_nbr,
                               config_nbr,
                               if_nbr,
                               0u,
                               DEF_YES,
                               0u,
                               p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (DEF_NO);
        }

        p_data_ep->DataIn = ep_addr;
        //                                                         Add Bulk OUT EP.
        ep_addr = USBD_BulkAdd(dev_nbr,
                               config_nbr,
                               if_nbr,
                               0u,
                               DEF_NO,
                               0u,
                               p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (DEF_NO);
        }

        p_data_ep->DataOut = ep_addr;
        p_data_if = p_data_if->NextPtr;
        //                                                         Group comm IF with data IFs.
        (void)USBD_IF_Grp(dev_nbr,
                          config_nbr,
                          USBD_CLASS_CODE_CDC_CONTROL,
                          p_ctrl->SubClassCode,
                          p_ctrl->SubClassProtocol,
                          p_comm->CCI_IF_Nbr,
                          p_ctrl->DataIF_Nbr + 1u,
                          "CDC Device",
                          p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (DEF_NO);
        }
      }
    }
  }

  p_comm->DevNbr = dev_nbr;
  p_comm->CtrlPtr = p_ctrl;

  CORE_ENTER_ATOMIC();
  p_ctrl->State = USBD_CDC_STATE_INIT;
  p_ctrl->CommPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                               USBD_CDC_IsConn()
 *
 * @brief    Gets the CDC class connection state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   DEF_YES, if CDC class is connected.
 *           DEF_NO,  if CDC class is not connected.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_CDC_IsConn(CPU_INT08U class_nbr)
{
  USBD_CDC_CTRL  *p_ctrl;
  USBD_CDC_COMM  *p_comm;
  USBD_DEV_STATE state;
  RTOS_ERR       err;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];

  if (p_ctrl->CommPtr == DEF_NULL) {
    return (DEF_NO);
  }

  p_comm = p_ctrl->CommPtr;
  state = USBD_DevStateGet(p_comm->DevNbr, &err);               // Get dev state.

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (state == USBD_DEV_STATE_CONFIGURED)
      && (p_ctrl->State == USBD_CDC_STATE_CONFIG)) {
    return (DEF_YES);
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_DataIF_Add()
 *
 * @brief    Adds a data interface class to the CDC communication interface class.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    isoc_en     Data interface isochronous enable (see Note #1) :
 *                           - DEF_ENABLED   Data interface uses isochronous EPs.
 *                           - DEF_DISABLED  Data interface uses bulk        EPs.
 *
 * @param    protocol    Data interface protocol code.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ALLOC
 *
 * @return   Data interface number.
 *
 * @note     (1) The value of 'isoc_en' must be DEF_DISABLED. Isochronous EPs are not supported.
 *******************************************************************************************************/
CPU_INT08U USBD_CDC_DataIF_Add(CPU_INT08U  class_nbr,
                               CPU_BOOLEAN isoc_en,
                               CPU_INT08U  protocol,
                               RTOS_ERR    *p_err)
{
  USBD_CDC_CTRL    *p_ctrl;
  USBD_CDC_DATA_IF *p_data_if;
  CPU_INT16U       data_if_ix;
  CPU_INT08U       data_if_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CDC_DATA_IF_NBR_NONE);
  //                                                               Check 'isoc_en' argument (see Note #1) .
  RTOS_ASSERT_DBG_ERR_SET((isoc_en == DEF_DISABLED), *p_err, RTOS_ERR_INVALID_ARG, USBD_CDC_DATA_IF_NBR_NONE);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, USBD_CDC_DATA_IF_NBR_NONE);
#endif

  CORE_ENTER_ATOMIC();
  if (USBD_CDC_Ptr->DataIF_NbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (USBD_CDC_DATA_IF_NBR_NONE);
  }

  USBD_CDC_Ptr->DataIF_NbrNext--;
  data_if_ix = USBD_CDC_Ptr->DataIF_NbrNext;
  CORE_EXIT_ATOMIC();

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];
  p_data_if = &USBD_CDC_Ptr->DataIF_Tbl[data_if_ix];
  data_if_nbr = p_ctrl->DataIF_Nbr;

  if (data_if_nbr == USBD_CDC_DATA_IF_NBR_TOTAL) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (USBD_CDC_DATA_IF_NBR_NONE);
  }

  if (p_ctrl->DataIF_HeadPtr == DEF_NULL) {                     // Add data IF in the list.
    p_ctrl->DataIF_HeadPtr = p_data_if;
  } else {
    p_ctrl->DataIF_TailPtr->NextPtr = p_data_if;
  }
  p_ctrl->DataIF_TailPtr = p_data_if;
  p_ctrl->DataIF_Nbr++;

  CORE_EXIT_ATOMIC();

  p_data_if->Protocol = protocol;
  p_data_if->IsocEn = isoc_en;
  p_data_if->NextPtr = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (data_if_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_CDC_DataRx()
 *
 * @brief    Receives data on the CDC data interface.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    data_if_nbr     CDC data interface number.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    timeout         Timeout in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors returned.
 *******************************************************************************************************/
CPU_INT32U USBD_CDC_DataRx(CPU_INT08U class_nbr,
                           CPU_INT08U data_if_nbr,
                           CPU_INT08U *p_buf,
                           CPU_INT32U buf_len,
                           CPU_INT16U timeout,
                           RTOS_ERR   *p_err)
{
  USBD_CDC_CTRL       *p_ctrl;
  USBD_CDC_COMM       *p_comm;
  USBD_CDC_DATA_IF    *p_data_if;
  USBD_CDC_DATA_IF_EP *p_data_ep;
  CPU_INT32U          xfer_len;
  CPU_INT16U          data_if_ix;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];

  if (p_ctrl->State != USBD_CDC_STATE_CONFIG) {                 // Transfers are only valid in config state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  if (data_if_nbr >= p_ctrl->DataIF_Nbr) {                      // Check 'data_if_nbr' is valid.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return(0u);
  }

  p_comm = p_ctrl->CommPtr;
  p_data_if = p_ctrl->DataIF_HeadPtr;
  //                                                               Find data IF struct.
  for (data_if_ix = 0u; data_if_ix < data_if_nbr; data_if_ix++) {
    p_data_if = p_data_if->NextPtr;
  }

  data_if_ix = p_comm->DataIF_EP_Ix + data_if_nbr;
  p_data_ep = &USBD_CDC_Ptr->DataIF_EP_Tbl[data_if_ix];
  xfer_len = 0u;

  if (p_data_if->IsocEn == DEF_DISABLED) {
    xfer_len = USBD_BulkRx(p_comm->DevNbr,
                           p_data_ep->DataOut,
                           p_buf,
                           buf_len,
                           timeout,
                           p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);               // $$$$ Isoc transfer not supported.
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                               USBD_CDC_DataTx()
 *
 * @brief    Sends data on the CDC data interface.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    data_if_nbr     CDC data interface number.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    timeout         Timeout in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets transmitted, if no errors are returned.
 *           0,                            if any errors returned.
 *******************************************************************************************************/
CPU_INT32U USBD_CDC_DataTx(CPU_INT08U class_nbr,
                           CPU_INT08U data_if_nbr,
                           CPU_INT08U *p_buf,
                           CPU_INT32U buf_len,
                           CPU_INT16U timeout,
                           RTOS_ERR   *p_err)
{
  USBD_CDC_CTRL       *p_ctrl;
  USBD_CDC_COMM       *p_comm;
  USBD_CDC_DATA_IF    *p_data_if;
  USBD_CDC_DATA_IF_EP *p_data_ep;
  CPU_INT32U          xfer_len;
  CPU_INT16U          data_if_ix;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];

  if (p_ctrl->State != USBD_CDC_STATE_CONFIG) {                 // Transfers are only valid in config state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  if (data_if_nbr >= p_ctrl->DataIF_Nbr) {                      // Check 'data_if_nbr' is valid.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return(0u);
  }

  p_comm = p_ctrl->CommPtr;
  p_data_if = p_ctrl->DataIF_HeadPtr;
  //                                                               Find data IF struct.
  for (data_if_ix = 0u; data_if_ix < data_if_nbr; data_if_ix++) {
    p_data_if = p_data_if->NextPtr;
  }

  data_if_ix = p_comm->DataIF_EP_Ix + data_if_nbr;
  p_data_ep = &USBD_CDC_Ptr->DataIF_EP_Tbl[data_if_ix];
  xfer_len = 0u;

  if (p_data_if->IsocEn == DEF_DISABLED) {
    xfer_len = USBD_BulkTx(p_comm->DevNbr,
                           p_data_ep->DataIn,
                           p_buf,
                           buf_len,
                           timeout,
                           DEF_YES,
                           p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);               // $$$$ Isoc transfer not supported.
    return (0u);
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                               USBD_CDC_Notify()
 *
 * @brief    Sends a communication interface class notification to the host.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    notification    Notification code.
 *
 * @param    value           Notification value.
 *
 * @param    p_buf           Pointer to the notification buffer (see Note #1).
 *
 * @param    data_len        Length of the data portion of the notification.
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
 * @note     (1) The notification buffer MUST contain space for the notification header
 *               'USBD_CDC_NOTIFICATION_HEADER' plus the variable-length data portion.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_CDC_Notify(CPU_INT08U class_nbr,
                            CPU_INT08U notification,
                            CPU_INT16U value,
                            CPU_INT08U *p_buf,
                            CPU_INT16U data_len,
                            RTOS_ERR   *p_err)
{
  USBD_CDC_COMM *p_comm;
  USBD_CDC_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
#endif

  p_ctrl = &USBD_CDC_Ptr->CtrlTbl[class_nbr];

  if (p_ctrl->State != USBD_CDC_STATE_CONFIG) {                 // Transfers are only valid in config state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (DEF_FAIL);
  }
  p_comm = p_ctrl->CommPtr;

  p_buf[0] = (1u)                                               // Recipient : Interface.
             | (1u << 5u)                                       // Type      : Class.
             |  DEF_BIT_07;                                     // Direction : Device to host.
  p_buf[1] = notification;
  p_buf[4] = p_comm->CCI_IF_Nbr;
  p_buf[5] = 0u;
  MEM_VAL_SET_INT16U_LITTLE(&p_buf[2], value);
  MEM_VAL_SET_INT16U_LITTLE(&p_buf[6], data_len);

  CORE_ENTER_ATOMIC();
  if (p_comm->NotifyInActiveXfer == DEF_NO) {                   // Check if another xfer is already in progress.
    p_comm->NotifyInActiveXfer = DEF_YES;                       // Indicate that a xfer is in progres.
    CORE_EXIT_ATOMIC();
    USBD_IntrTxAsync(p_comm->DevNbr,
                     p_comm->NotifyIn,
                     p_buf,
                     (CPU_INT32U)data_len + USBD_CDC_NOTIFICATION_HEADER,
                     USBD_CDC_NotifyCmpl,
                     (void *)p_comm,
                     DEF_YES,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CORE_ENTER_ATOMIC();
      p_comm->NotifyInActiveXfer = DEF_NO;
      CORE_EXIT_ATOMIC();
      return (DEF_FAIL);
    }
  } else {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_CDC_Conn()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration ix to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void USBD_CDC_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg)
{
  USBD_CDC_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;
  (void)&dev_nbr;

  p_comm = (USBD_CDC_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = p_comm;
  p_comm->CtrlPtr->State = USBD_CDC_STATE_CONFIG;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_CDC_Disconn()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration ix to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void USBD_CDC_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg)
{
  USBD_CDC_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;
  (void)&dev_nbr;

  p_comm = (USBD_CDC_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = (USBD_CDC_COMM *)0;
  p_comm->CtrlPtr->State = USBD_CDC_STATE_INIT;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_CDC_ClassReq()
 *
 * @brief    Class request handler.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *
 *           DEF_FAIL, if any errors returned.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_CDC_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg)
{
  USBD_CDC_CTRL         *p_ctrl;
  USBD_CDC_COMM         *p_comm;
  USBD_CDC_SUBCLASS_DRV *p_drv;
  CPU_BOOLEAN           valid;

  p_comm = (USBD_CDC_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;
  p_drv = p_ctrl->SubClassDrvPtr;
  valid = DEF_OK;

  if (p_drv->MngmtReq != DEF_NULL) {
    //                                                             Call subclass-specific management request handler.
    valid = p_drv->MngmtReq(dev_nbr,
                            p_setup_req,
                            p_ctrl->SubClassArg);
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_NotifyCmpl()
 *
 * @brief    CDC notification complete callback.
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
static void USBD_CDC_NotifyCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err)
{
  USBD_CDC_CTRL         *p_ctrl;
  USBD_CDC_COMM         *p_comm;
  USBD_CDC_SUBCLASS_DRV *p_drv;

  (void)&ep_addr;
  (void)&p_buf;
  (void)&buf_len;
  (void)&xfer_len;
  (void)&err;

  p_comm = (USBD_CDC_COMM *)p_arg;
  p_ctrl = p_comm->CtrlPtr;
  p_drv = p_ctrl->SubClassDrvPtr;

  p_comm->NotifyInActiveXfer = DEF_NO;                          // Xfer finished, no more active xfer.
  if (p_drv->NotifyCmpl != DEF_NULL) {
    p_drv->NotifyCmpl(dev_nbr, p_ctrl->SubClassArg);
  }
}

/****************************************************************************************************//**
 *                                           USBD_CDC_CommIF_Desc()
 *
 * @brief    Class interface descriptor callback.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *******************************************************************************************************/
static void USBD_CDC_CommIF_Desc(CPU_INT08U dev_nbr,
                                 CPU_INT08U config_nbr,
                                 CPU_INT08U if_nbr,
                                 CPU_INT08U if_alt_nbr,
                                 void       *p_if_class_arg,
                                 void       *p_if_alt_class_arg)
{
  USBD_CDC_CTRL         *p_ctrl;
  USBD_CDC_COMM         *p_comm;
  USBD_CDC_SUBCLASS_DRV *p_drv;
  USBD_CDC_DATA_IF      *p_data_if;
  CPU_INT08U            desc_size;
  CPU_INT08U            data_if_nbr;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (USBD_CDC_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;
  //                                                               ------------- BUILD HEADER DESCRIPTOR --------------
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_SIZE_HEADER);
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_TYPE_CS_IF);
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_SUBTYPE_HEADER);
  USBD_DescWr16(dev_nbr, 0x0120u);                              // CDC release number (1.20) in BCD fmt.

  //                                                               ------------- BUILD UNION IF DESCRIPTOR ------------
  if (p_ctrl->DataIF_Nbr > 0u) {
    desc_size = USBD_CDC_DESC_SIZE_UNION_MIN + p_ctrl->DataIF_Nbr;

    USBD_DescWr08(dev_nbr, desc_size);
    USBD_DescWr08(dev_nbr, USBD_CDC_DESC_TYPE_CS_IF);
    USBD_DescWr08(dev_nbr, USBD_CDC_DESC_SUBTYPE_UNION);
    USBD_DescWr08(dev_nbr, p_comm->CCI_IF_Nbr);

    p_data_if = p_ctrl->DataIF_HeadPtr;                         // Add all subordinate data IFs.

    for (data_if_nbr = 0u; data_if_nbr < p_ctrl->DataIF_Nbr; data_if_nbr++) {
      USBD_DescWr08(dev_nbr, p_data_if->IF_Nbr);
      p_data_if = p_data_if->NextPtr;
    }
  }

  p_drv = p_ctrl->SubClassDrvPtr;

  if (p_drv->FnctDesc != DEF_NULL) {
    //                                                             Call subclass-specific functional descriptor.;
    p_drv->FnctDesc(dev_nbr,
                    p_ctrl->SubClassArg,
                    p_ctrl->DataIF_HeadPtr->IF_Nbr);
  }
}

/****************************************************************************************************//**
 *                                       USBD_CDC_CommIF_DescSizeGet()
 *
 * @brief    Retrieve the size of the class interface descriptor.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * @return   Size of the class interface descriptor.
 *******************************************************************************************************/
static CPU_INT16U USBD_CDC_CommIF_DescSizeGet(CPU_INT08U dev_nbr,
                                              CPU_INT08U config_nbr,
                                              CPU_INT08U if_nbr,
                                              CPU_INT08U if_alt_nbr,
                                              void       *p_if_class_arg,
                                              void       *p_if_alt_class_arg)
{
  USBD_CDC_CTRL         *p_ctrl;
  USBD_CDC_COMM         *p_comm;
  USBD_CDC_SUBCLASS_DRV *p_drv;
  CPU_INT16U            desc_size;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (USBD_CDC_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;

  desc_size = USBD_CDC_DESC_SIZE_HEADER;

  if (p_ctrl->DataIF_Nbr > 0u) {
    desc_size += USBD_CDC_DESC_SIZE_UNION_MIN;
    desc_size += p_ctrl->DataIF_Nbr;
  }

  p_drv = p_ctrl->SubClassDrvPtr;

  if (p_drv->FnctDescSizeGet != DEF_NULL) {
    desc_size += p_drv->FnctDescSizeGet(dev_nbr, p_ctrl->SubClassArg);
  }

  return (desc_size);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_CDC_AVAIL))
