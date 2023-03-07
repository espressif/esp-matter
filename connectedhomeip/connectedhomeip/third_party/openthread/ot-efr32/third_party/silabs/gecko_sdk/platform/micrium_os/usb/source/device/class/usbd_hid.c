/***************************************************************************//**
 * @file
 * @brief USB Device Hid Class
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

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device HID class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    USBD_HID_MODULE
#include  <em_core.h>

#include  <usb/include/device/usbd_hid.h>
#include  <usb/source/device/class/usbd_hid_os_priv.h>
#include  <usb/source/device/class/usbd_hid_priv.h>
#include  <common/include/lib_math.h>

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, HID)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

#define  USBD_HID_CTRL_REQ_TIMEOUT_mS                  5000u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC LOCAL DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.1 for more details about class-descriptor types.
 *
 *               (a) The 'wValue' field of Get_Descriptor Request specifies the Descriptor Type in the
 *                   high byte.
 *
 *           (2) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about class-specific requests.
 *
 *               (a) The 'bRequest' field of SETUP packet may contain one of these values.
 *******************************************************************************************************/

#define  USBD_HID_DESC_LEN                                 6u

//                                                                 --------- CLASS-SPECIFIC DESC (see Note #1) --------
#define  USBD_HID_DESC_TYPE_HID                         0x21
#define  USBD_HID_DESC_TYPE_REPORT                      0x22
#define  USBD_HID_DESC_TYPE_PHYSICAL                    0x23
//                                                                 --------- CLASS-SPECIFIC REQ (see Note #2) ---------
#define  USBD_HID_REQ_GET_REPORT                        0x01
#define  USBD_HID_REQ_GET_IDLE                          0x02
#define  USBD_HID_REQ_GET_PROTOCOL                      0x03
#define  USBD_HID_REQ_SET_REPORT                        0x09
#define  USBD_HID_REQ_SET_IDLE                          0x0A
#define  USBD_HID_REQ_SET_PROTOCOL                      0x0B

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_HID_INIT_CFG USBD_HID_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .ReportID_Qty = 2u,
  .PushPopItemsQty = 0u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,
  .TmrTaskStkSizeElements = 512u,
  .TmrTaskStkPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBD_HID *USBD_HID_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_HID_INIT_CFG USBD_HID_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .ReportID_Qty = 2u,
  .PushPopItemsQty = 0u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,
  .TmrTaskStkSizeElements = 512u,
  .TmrTaskStkPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_HID_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg);

static void USBD_HID_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg);

static void USBD_HID_AltSettingUpdate(CPU_INT08U dev_nbr,
                                      CPU_INT08U config_nbr,
                                      CPU_INT08U if_nbr,
                                      CPU_INT08U if_alt_nbr,
                                      void       *p_if_class_arg,
                                      void       *p_if_alt_class_arg);

static void USBD_HID_EP_StateUpdate(CPU_INT08U dev_nbr,
                                    CPU_INT08U config_nbr,
                                    CPU_INT08U if_nbr,
                                    CPU_INT08U if_alt_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_if_class_arg,
                                    void       *p_if_alt_class_arg);

static void USBD_HID_IF_Desc(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             CPU_INT08U if_nbr,
                             CPU_INT08U if_alt_nbr,
                             void       *p_if_class_arg,
                             void       *p_if_alt_class_arg);

static void USBD_HID_IF_DescHandler(CPU_INT08U dev_nbr,
                                    void       *p_if_class_arg);

static CPU_INT16U USBD_HID_IF_DescSizeGet(CPU_INT08U dev_nbr,
                                          CPU_INT08U config_nbr,
                                          CPU_INT08U if_nbr,
                                          CPU_INT08U if_alt_nbr,
                                          void       *p_if_class_arg,
                                          void       *p_if_alt_class_arg);

static CPU_BOOLEAN USBD_HID_IF_Req(CPU_INT08U           dev_nbr,
                                   const USBD_SETUP_REQ *p_setup_req,
                                   void                 *p_if_class_arg);

static CPU_BOOLEAN USBD_HID_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg);

static void USBD_HID_WrAsyncCmpl(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 void       *p_buf,
                                 CPU_INT32U buf_len,
                                 CPU_INT32U xfer_len,
                                 void       *p_arg,
                                 RTOS_ERR   err);

static void USBD_HID_WrSyncCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err);

static void USBD_HID_RdAsyncCmpl(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 void       *p_buf,
                                 CPU_INT32U buf_len,
                                 CPU_INT32U xfer_len,
                                 void       *p_arg,
                                 RTOS_ERR   err);

static void USBD_HID_OutputDataCmpl(CPU_INT08U class_nbr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err);

/********************************************************************************************************
 *                                           HID CLASS DRIVER
 *******************************************************************************************************/

static USBD_CLASS_DRV USBD_HID_Drv = {
  USBD_HID_Conn,
  USBD_HID_Disconn,
  USBD_HID_AltSettingUpdate,
  USBD_HID_EP_StateUpdate,
  USBD_HID_IF_Desc,
  USBD_HID_IF_DescSizeGet,
  DEF_NULL,
  DEF_NULL,
  USBD_HID_IF_Req,
  USBD_HID_ClassReq,
  DEF_NULL,

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  DEF_NULL,
  DEF_NULL
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBD_HID_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBD_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_HID_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_HID_ConfigureReportID_Qty()
 *
 * @brief    Configures the quantity of report IDs.
 *
 * @param    report_id_qty   Quantity of report IDs.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBD_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigureReportID_Qty(CPU_INT08U report_id_qty)
{
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_HID_InitCfg.ReportID_Qty = report_id_qty;
}
#endif

/****************************************************************************************************//**
 *                                   USBD_HID_ConfigurePushPopItemsQty()
 *
 * @brief    Configures the quantity of push/pop items.
 *
 * @param    push_pop_items_qty  Quantity of Push/pop items.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBD_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigurePushPopItemsQty(CPU_INT08U push_pop_items_qty)
{
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_HID_InitCfg.PushPopItemsQty = push_pop_items_qty;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_HID_ConfigureTmrTaskStk()
 *
 * @brief    Configures the timer task stack.
 *
 * @param    stk_size_elements   Size of the stack, in stack elements.
 *
 * @param    p_stk               Pointer to base of the stack.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the HID class is initialized via the
 *               USBD_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigureTmrTaskStk(CPU_INT32U stk_size_elements,
                                  void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_HID_InitCfg.TmrTaskStkSizeElements = stk_size_elements;
  USBD_HID_InitCfg.TmrTaskStkPtr = p_stk;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_HID_ConfigureMemSeg()
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
 *               USBD_HID_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_HID_InitCfg.MemSegPtr = p_mem_seg;
  USBD_HID_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                               USBD_HID_Init()
 *
 * @brief    Initialize HID class.
 *
 * @param    p_qty_cfg   Pointer to HID class configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_INVALID_CFG
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_HID_Init(const USBD_HID_QTY_CFG *p_qty_cfg,
                   RTOS_ERR               *p_err)
{
  CPU_INT08U    ix;
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ClassInstanceQty > 0u)
                           && (p_qty_cfg->ConfigQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  USBD_HID_Ptr = (USBD_HID *)Mem_SegAlloc("USBD - HID root struct",
                                          USBD_HID_InitCfg.MemSegPtr,
                                          sizeof(USBD_HID),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_Ptr->CtrlTbl = (USBD_HID_CTRL *)Mem_SegAlloc("USBD - HID control structure table",
                                                        USBD_HID_InitCfg.MemSegPtr,
                                                        sizeof(USBD_HID_CTRL) * p_qty_cfg->ClassInstanceQty,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_Ptr->CommTbl = (USBD_HID_COMM *)Mem_SegAlloc("USBD - HID comm structure table",
                                                        USBD_HID_InitCfg.MemSegPtr,
                                                        sizeof(USBD_HID_COMM) * p_qty_cfg->ConfigQty,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_OS_Init(USBD_HID_InitCfg.MemSegPtr,
                   (USBD_HID_QTY_CFG *)p_qty_cfg,
                   USBD_HID_InitCfg.TmrTaskStkSizeElements,
                   USBD_HID_InitCfg.TmrTaskStkPtr,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_Report_Init(USBD_HID_InitCfg.MemSegPtr,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < p_qty_cfg->ClassInstanceQty; ix++) {       // Init HID class struct.
    p_ctrl = &USBD_HID_Ptr->CtrlTbl[ix];
    p_ctrl->DevNbr = USBD_DEV_NBR_NONE;
    p_ctrl->ClassNbr = USBD_CLASS_NBR_NONE;
    p_ctrl->State = USBD_HID_STATE_NONE;
    p_ctrl->CommPtr = DEF_NULL;
    p_ctrl->SubClassCode = USBD_HID_SUBCLASS_NONE;
    p_ctrl->ProtocolCode = USBD_HID_PROTOCOL_NONE;
    p_ctrl->CountryCode = USBD_HID_COUNTRY_CODE_NOT_SUPPORTED;
    p_ctrl->ReportDescPtr = DEF_NULL;
    p_ctrl->ReportDescLen = 0u;
    p_ctrl->PhyDescPtr = DEF_NULL;
    p_ctrl->PhyDescLen = 0u;
    p_ctrl->IntervalIn = 0u;
    p_ctrl->IntervalOut = 0u;
    p_ctrl->CtrlRdEn = DEF_TRUE;
    p_ctrl->CallbackPtr = DEF_NULL;
    p_ctrl->IntrWrAsyncFnct = DEF_NULL;
    p_ctrl->IntrWrAsyncArgPtr = DEF_NULL;
    p_ctrl->IntrRdAsyncFnct = DEF_NULL;
    p_ctrl->IntrRdAsyncArgPtr = DEF_NULL;

    p_ctrl->CtrlStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - HID Ctrl status buffer",
                                                            USBD_HID_InitCfg.MemSegBufPtr,
                                                            sizeof(CPU_INT08U),
                                                            USBD_HID_InitCfg.BufAlignOctets,
                                                            DEF_NULL,
                                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  for (ix = 0u; ix < p_qty_cfg->ConfigQty; ix++) {              // Init HID EP tbl.
    p_comm = &USBD_HID_Ptr->CommTbl[ix];

    p_comm->CtrlPtr = DEF_NULL;
    p_comm->DataIntrInEpAddr = USBD_EP_ADDR_NONE;
    p_comm->DataIntrOutEpAddr = USBD_EP_ADDR_NONE;
    p_comm->DataIntrOutActiveXfer = DEF_NO;
  }

  USBD_HID_Ptr->CtrlNbrNext = p_qty_cfg->ClassInstanceQty;
  USBD_HID_Ptr->CommNbrNext = p_qty_cfg->ConfigQty;

#if (RTOS_ARG_CHK_EXT_EN)
  USBD_HID_Ptr->ClassInstanceQty = p_qty_cfg->ClassInstanceQty;
#endif
}

/****************************************************************************************************//**
 *                                           USBD_HID_TmrTaskPrioSet()
 *
 * @brief    Sets priority of the HID timer task.
 *
 * @param    prio    Priority of the HID timer task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void USBD_HID_TmrTaskPrioSet(RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBD_HID_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  USBD_HID_OS_TmrTaskPrioSet(prio, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_Add()
 *
 * @brief   Adds a new instance of the HID class.
 *
 * @param   subclass            Subclass code.
 *
 * @param   protocol            Protocol code.
 *
 * @param   country_code        Country code ID.
 *
 * @param   p_report_desc       Pointer to the report descriptor structure
 *                              [Content MUST be persistent]
 *
 * @param   report_desc_len     Report descriptor length.
 *
 * @param   p_phy_desc          Pointer to the physical descriptor structure.
 *                              [Content MUST be persistent]
 *
 * @param   phy_desc_len        Physical descriptor length.
 *
 * @param   interval_in         Polling interval for input transfers, in milliseconds.
 *                              It must be a power of 2.
 *
 * @param   interval_out        Polling interval for output transfers, in milliseconds.
 *                              It must be a power of 2. Used only when read operations are not
 *                              through control transfers.
 *
 * @param   ctrl_rd_en          Enable read operations through the control transfers.
 *
 * @param   p_hid_callback      Pointer to HID descriptor and request callback structure.
 *                              [Content MUST be persistent]
 *
 * @param   p_err               Pointer to the variable that will receive one of these returned error codes from this function :
 *                                  - RTOS_ERR_NONE
 *                                  - RTOS_ERR_ALLOC
 *                                  - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *                                  - RTOS_ERR_INVALID_ARG
 *                                  - RTOS_ERR_SEG_OVF
 *
 * @return  Class instance number, if no errors are returned.
 *          USBD_CLASS_NBR_NONE,   if any errors are returned.
 *
 *******************************************************************************************************/
CPU_INT08U USBD_HID_Add(CPU_INT08U            subclass,
                        CPU_INT08U            protocol,
                        USBD_HID_COUNTRY_CODE country_code,
                        const CPU_INT08U      *p_report_desc,
                        CPU_INT16U            report_desc_len,
                        const CPU_INT08U      *p_phy_desc,
                        CPU_INT16U            phy_desc_len,
                        CPU_INT16U            interval_in,
                        CPU_INT16U            interval_out,
                        CPU_BOOLEAN           ctrl_rd_en,
                        USBD_HID_CALLBACK     *p_hid_callback,
                        RTOS_ERR              *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  CPU_INT08U    class_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CLASS_NBR_NONE);

  //                                                               Interval for intr IN must be a power of 2.
  RTOS_ASSERT_DBG_ERR_SET((MATH_IS_PWR2(interval_in)), *p_err, RTOS_ERR_INVALID_ARG, USBD_CLASS_NBR_NONE);

  //                                                               Interval for intr OUT must be a power of 2.
  RTOS_ASSERT_DBG_ERR_SET(((ctrl_rd_en == DEF_YES)
                           || (MATH_IS_PWR2(interval_out) == DEF_YES)), *p_err, RTOS_ERR_INVALID_ARG, USBD_CLASS_NBR_NONE);

  if (((p_report_desc == DEF_NULL)
       && (report_desc_len > 0u))
      || ((p_report_desc != DEF_NULL)
          && (report_desc_len == 0u))) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_CLASS_NBR_NONE);
  }

  if (((p_phy_desc == DEF_NULL)
       && (phy_desc_len > 0u))
      || ((p_phy_desc != DEF_NULL)
          && (phy_desc_len < 3u))) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_CLASS_NBR_NONE);
  }

  CORE_ENTER_ATOMIC();
  if (USBD_HID_Ptr->CtrlNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (USBD_CLASS_NBR_NONE);
  }

  USBD_HID_Ptr->CtrlNbrNext--;
  class_nbr = USBD_HID_Ptr->CtrlNbrNext;                        // Alloc new HID class instance.
  CORE_EXIT_ATOMIC();

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];

  p_ctrl->SubClassCode = subclass;
  p_ctrl->ProtocolCode = protocol;
  p_ctrl->CountryCode = country_code;
  p_ctrl->ReportDescPtr = p_report_desc;
  p_ctrl->ReportDescLen = report_desc_len;
  p_ctrl->PhyDescPtr = p_phy_desc;
  p_ctrl->PhyDescLen = phy_desc_len;
  p_ctrl->IntervalIn = interval_in;
  p_ctrl->IntervalOut = interval_out;
  p_ctrl->CtrlRdEn = ctrl_rd_en;
  p_ctrl->CallbackPtr = p_hid_callback;

  USBD_HID_Report_Parse(class_nbr,
                        p_ctrl->ReportDescPtr,
                        p_ctrl->ReportDescLen,
                        &p_ctrl->Report,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  } else {
    return (class_nbr);
  }
}

/****************************************************************************************************//**
 *                                           USBD_HID_ConfigAdd()
 *
 * @brief    Adds the HID class instance to the USB device configuration (see Note #1).
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    config_nbr  Configuration index to add HID class instance to.
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
 * @return   DEF_YES, if the HID class instance was added to USB device configuration successfully.
 *           DEF_NO,  if it fails to be added.
 *
 * @note     (1) Called several times, it creates multiple instances and configurations.
 *               For instance, the following architecture could be created :
 *               @verbatim
 *               HS
 *               |-- Configuration 0 (HID class 0)
 *                                   (HID class 1)
 *                                   (HID class 2)
 *                   |-- Interface 0
 *               |-- Configuration 1 (HID class 0)
 *                   |-- Interface 0
 *               @endverbatim
 *
 * @note     (2) Configuration Descriptor corresponding to a HID device uses the following format :
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (HID class)
 *                   |-- Endpoint Descriptor (Interrupt IN)
 *                   |-- Endpoint Descriptor (Interrupt OUT) - optional
 *               @endverbatim
 *******************************************************************************************************/
CPU_BOOLEAN USBD_HID_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT08U    if_nbr;
  CPU_INT08U    ep_addr;
  CPU_INT16U    comm_nbr;
  CPU_INT16U    interval_in;
  CPU_INT16U    interval_out;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), *p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.
  CORE_ENTER_ATOMIC();
  if ((p_ctrl->DevNbr != USBD_DEV_NBR_NONE)                     // Chk if class is associated with a different dev.
      && (p_ctrl->DevNbr != dev_nbr)) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (DEF_NO);
  }

  p_ctrl->DevNbr = dev_nbr;

  if (USBD_HID_Ptr->CommNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (DEF_NO);
  }

  USBD_HID_Ptr->CommNbrNext--;
  comm_nbr = USBD_HID_Ptr->CommNbrNext;                         // Alloc new HID class comm info.
  CORE_EXIT_ATOMIC();

  p_comm = &USBD_HID_Ptr->CommTbl[comm_nbr];

  //                                                               ------------- CONFIG DESC CONSTRUCTION -------------
  //                                                               See Note #2.

  //                                                               Add HID IF desc to config desc.
  if_nbr = USBD_IF_Add(dev_nbr,
                       config_nbr,
                       &USBD_HID_Drv,
                       (void *)p_comm,                          // Comm struct associated to HID IF.
                       (void *)0,
                       USBD_CLASS_CODE_HID,
                       p_ctrl->SubClassCode,
                       p_ctrl->ProtocolCode,
                       "HID Class",
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  if (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
    //                                                             In FS, bInterval in frames.
    interval_in = p_ctrl->IntervalIn;
    interval_out = p_ctrl->IntervalOut;
  } else {                                                      // In HS, bInterval in microframes.
    interval_in = p_ctrl->IntervalIn * 8u;
    interval_out = p_ctrl->IntervalOut * 8u;
  }

  ep_addr = USBD_IntrAdd(dev_nbr,                               // Add intr IN EP desc.
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_YES,
                         0u,
                         interval_in,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  p_comm->DataIntrInEpAddr = ep_addr;                           // Store intr IN EP addr.

  if (p_ctrl->CtrlRdEn == DEF_FALSE) {
    ep_addr = USBD_IntrAdd(dev_nbr,                             // Add intr OUT EP desc.
                           config_nbr,
                           if_nbr,
                           0u,
                           DEF_NO,
                           0u,
                           interval_out,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NO);
    }
  } else {
    ep_addr = USBD_EP_ADDR_NONE;
  }

  p_comm->DataIntrOutEpAddr = ep_addr;                          // Store intr OUT EP addr.

  //                                                               Store HID class instance info.
  CORE_ENTER_ATOMIC();
  p_ctrl->ClassNbr = class_nbr;
  p_ctrl->State = USBD_HID_STATE_INIT;
  CORE_EXIT_ATOMIC();

  p_comm->CtrlPtr = p_ctrl;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                               USBD_HID_IsConn()
 *
 * @brief    Gets the HID class connection state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   DEF_YES, if HID class is connected.
 *           DEF_NO,  it fails to connect.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_HID_IsConn(CPU_INT08U class_nbr)
{
  USBD_HID_CTRL  *p_ctrl;
  USBD_DEV_STATE state;
  RTOS_ERR       err;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.
  state = USBD_DevStateGet(p_ctrl->DevNbr, &err);               // Get dev state.

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (state == USBD_DEV_STATE_CONFIGURED)
      && (p_ctrl->State == USBD_HID_STATE_CONFIG)) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                               USBD_HID_Rd()
 *
 * @brief          Receives  data from the host through the Interrupt OUT endpoint. This function is blocking.
 *
 * @param          class_nbr   Class instance number.
 *
 * @param          p_buf       Pointer to the receive buffer.
 *
 * @param          buf_len     Receive the buffer length, in octets.
 *
 * @param          timeout     Timeout, in milliseconds.
 *
 * @param          p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                                 - RTOS_ERR_NONE
 *                                 - RTOS_ERR_USB_INVALID_DEV_STATE
 *                                 - RTOS_ERR_NULL_PTR
 *                                 - RTOS_ERR_USB_EP_QUEUING
 *                                 - RTOS_ERR_RX
 *                                 - RTOS_ERR_NOT_READY
 *                                 - RTOS_ERR_USB_INVALID_EP
 *                                 - RTOS_ERR_OS_SCHED_LOCKED
 *                                 - RTOS_ERR_NOT_AVAIL
 *                                 - RTOS_ERR_FAIL
 *                                 - RTOS_ERR_WOULD_OVF
 *                                 - RTOS_ERR_OS_OBJ_DEL
 *                                 - RTOS_ERR_INVALID_HANDLE
 *                                 - RTOS_ERR_WOULD_BLOCK
 *                                 - RTOS_ERR_IS_OWNER
 *                                 - RTOS_ERR_INVALID_CLASS_STATE
 *                                 - RTOS_ERR_USB_INVALID_EP_STATE
 *                                 - RTOS_ERR_ABORT
 *                                 - RTOS_ERR_TIMEOUT
 *
 * @return        Number of octets received, if no errors are returned.
 *               0,                        if any errors are returned.
 ***********************************************************************************************/
CPU_INT32U USBD_HID_Rd(CPU_INT08U class_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout,
                       RTOS_ERR   *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT32U    xfer_len;
  CPU_BOOLEAN   conn;
  RTOS_ERR      err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->CommPtr;
  conn = USBD_HID_IsConn(class_nbr);
  CORE_EXIT_ATOMIC();

  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  if (p_ctrl->CtrlRdEn == DEF_TRUE) {                           // Use SET_REPORT to rx data instead.
    USBD_HID_OS_OutputLock(class_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    if (p_ctrl->IsRx == DEF_TRUE) {
      USBD_HID_OS_OutputUnlock(class_nbr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return (0u);
    }
    p_ctrl->IsRx = DEF_TRUE;
    p_ctrl->RxBufLen = buf_len;
    p_ctrl->RxBufPtr = (CPU_INT08U *)p_buf;
    //                                                             Save app rx callback.
    p_ctrl->IntrRdAsyncFnct = USBD_HID_OutputDataCmpl;
    p_ctrl->IntrRdAsyncArgPtr = &xfer_len;

    USBD_HID_OS_OutputUnlock(class_nbr);

    USBD_HID_OS_OutputDataPend(class_nbr, timeout, &err);

    USBD_HID_OS_OutputLock(class_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    RTOS_ERR_COPY(*p_err, err);

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      p_ctrl->RxBufLen = 0u;
      p_ctrl->RxBufPtr = (CPU_INT08U *)0;

      p_ctrl->IntrRdAsyncFnct = 0;
      p_ctrl->IntrRdAsyncArgPtr = 0;

      p_ctrl->IsRx = DEF_FALSE;
      USBD_HID_OS_OutputUnlock(class_nbr);
      return (0u);
    }

    USBD_HID_OS_OutputUnlock(class_nbr);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    return (xfer_len);
  }

  //                                                               ------------------ INTR OUT COMM -------------------
  xfer_len = USBD_IntrRx(p_ctrl->DevNbr,
                         p_comm->DataIntrOutEpAddr,
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
 *                                           USBD_HID_RdAsync()
 *
 * @brief    Receives data from the host through the Interrupt OUT endpoint. This function is non-blocking and
 *           returns immediately after transfer preparation. Upon transfer completion, the provided callback
 *           is called to notify the application.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to receive buffer.
 *
 * @param    buf_len         Receive buffer length, in octets.
 *
 * @param    async_fnct      Receive callback.
 *
 * @param    p_async_arg     Additional argument provided by application for receive callback.
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
 *                               - RTOS_ERR_FAIL
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
void USBD_HID_RdAsync(CPU_INT08U          class_nbr,
                      void                *p_buf,
                      CPU_INT32U          buf_len,
                      USBD_HID_ASYNC_FNCT async_fnct,
                      void                *p_async_arg,
                      RTOS_ERR            *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_BOOLEAN   conn;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->CommPtr;
  conn = USBD_HID_IsConn(class_nbr);
  CORE_EXIT_ATOMIC();

  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  if (p_ctrl->CtrlRdEn == DEF_TRUE) {                           // Use SET_REPORT to rx data instead.
    USBD_HID_OS_OutputLock(class_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (p_ctrl->IsRx == DEF_TRUE) {
      USBD_HID_OS_OutputUnlock(class_nbr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
    }
    p_ctrl->IsRx = DEF_TRUE;
    p_ctrl->RxBufLen = buf_len;
    p_ctrl->RxBufPtr = (CPU_INT08U *)p_buf;
    //                                                             Save app rx callback.
    p_ctrl->IntrRdAsyncFnct = async_fnct;
    p_ctrl->IntrRdAsyncArgPtr = p_async_arg;
    USBD_HID_OS_OutputUnlock(class_nbr);
  } else {
    //                                                             Save app rx callback.
    CORE_ENTER_ATOMIC();
    p_ctrl->IntrRdAsyncFnct = async_fnct;
    p_ctrl->IntrRdAsyncArgPtr = p_async_arg;
    //                                                             ------------------ INTR OUT COMM -------------------
    if (p_comm->DataIntrOutActiveXfer == DEF_NO) {              // Check if another xfer is already in progress.
      p_comm->DataIntrOutActiveXfer = DEF_YES;                  // Indicate that a xfer is in progres.
      CORE_EXIT_ATOMIC();

      USBD_IntrRxAsync(p_ctrl->DevNbr,
                       p_comm->DataIntrOutEpAddr,
                       p_buf,
                       buf_len,
                       USBD_HID_RdAsyncCmpl,
                       (void *)p_comm,
                       p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        CORE_ENTER_ATOMIC();
        p_comm->DataIntrOutActiveXfer = DEF_NO;
        CORE_EXIT_ATOMIC();
      }
    } else {
      CORE_ENTER_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                               USBD_HID_Wr()
 *
 * @brief    Sends data to the host through the Interrupt IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer. If more than one input report exists, the first
 *                       byte must represent the Report ID.
 *
 * @param    buf_len     Transmit buffer length, in octets.
 *
 * @param    timeout     Timeout in milliseconds.
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
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_CLASS_STATE
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets sent, if no errors are returned.
 *           0,                    if any errors are returned.
 *******************************************************************************************************/
CPU_INT32U USBD_HID_Wr(CPU_INT08U class_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout,
                       RTOS_ERR   *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT32U    xfer_len;
  CPU_INT08U    report_id;
  CPU_INT08U    *p_buf_data;
  CPU_INT08U    *p_buf_report;
  CPU_INT16U    report_len;
  CPU_BOOLEAN   is_largest;
  CPU_BOOLEAN   conn;
  CPU_BOOLEAN   eot;
  RTOS_ERR      err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0)), *p_err, RTOS_ERR_NULL_PTR, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->CommPtr;
  conn = USBD_HID_IsConn(class_nbr);
  CORE_EXIT_ATOMIC();

  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  //                                                               ----------------- STORE REPORT DATA ----------------
  if (buf_len > 0) {
    p_buf_data = (CPU_INT08U *)p_buf;
    if (p_ctrl->Report.HasReports == DEF_YES) {
      report_id = p_buf_data[0];
    } else {
      report_id = 0;
    }

    report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                           USBD_HID_REPORT_TYPE_INPUT,
                                           report_id,
                                           &p_buf_report,
                                           &is_largest,
                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    if (report_len > buf_len) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return (0u);
    }

    eot = (is_largest == DEF_YES) ? DEF_NO : DEF_YES;
  } else {
    p_buf_data = (CPU_INT08U *)0;
    p_buf_report = (CPU_INT08U *)0;
    report_len = 0;
    eot = DEF_YES;
  }

  USBD_HID_OS_TxLock(class_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  //                                                               ------------------- INTR IN COMM -------------------
  USBD_IntrTxAsync(p_ctrl->DevNbr,
                   p_comm->DataIntrInEpAddr,
                   p_buf,
                   buf_len,
                   USBD_HID_WrSyncCmpl,
                   (void *)p_comm,
                   eot,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBD_HID_OS_TxUnlock(class_nbr);
    return (0u);
  }

  if (buf_len > 0) {                                            // Defer copy while transmitting.
    USBD_HID_OS_InputLock(class_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      USBD_HID_OS_TxUnlock(class_nbr);
      return (0u);
    }
    Mem_Copy(&p_buf_report[0], &p_buf_data[0], report_len);
    USBD_HID_OS_InputUnlock(class_nbr);
  }

  USBD_HID_OS_InputDataPend(class_nbr, timeout, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      USBD_EP_Abort(p_ctrl->DevNbr,
                    p_comm->DataIntrInEpAddr,
                    &err);
    }
    USBD_HID_OS_TxUnlock(class_nbr);
    return (0u);
  }

  xfer_len = p_ctrl->DataIntrInXferLen;

  USBD_HID_OS_TxUnlock(class_nbr);
  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_HID_WrAsync()
 *
 * @brief    Sends data to the host through the Interrupt IN endpoint. This function is non-blocking,
 *           and returns immediately after transfer preparation. Upon transfer completion, the provided
 *           callback is called to notify the application.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to transmit buffer. If more than one input report exists, the first
 *                           byte must represent the Report ID.
 *
 * @param    buf_len         Transmit buffer length, in octets.
 *
 * @param    async_fnct      Transmit callback.
 *
 * @param    p_async_arg     Additional argument provided by application for transmit callback.
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
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_HID_WrAsync(CPU_INT08U          class_nbr,
                      void                *p_buf,
                      CPU_INT32U          buf_len,
                      USBD_HID_ASYNC_FNCT async_fnct,
                      void                *p_async_arg,
                      RTOS_ERR            *p_err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT08U    report_id;
  CPU_INT08U    *p_buf_data;
  CPU_INT08U    *p_buf_report;
  CPU_INT16U    report_len;
  CPU_BOOLEAN   is_largest;
  CPU_BOOLEAN   conn;
  CPU_BOOLEAN   eot;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET(((class_nbr < USBD_HID_Ptr->ClassInstanceQty)
                           && (class_nbr >= USBD_HID_Ptr->CtrlNbrNext)), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_HID_Ptr->CtrlTbl[class_nbr];                   // Get HID class instance.

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->CommPtr;
  conn = USBD_HID_IsConn(class_nbr);
  CORE_EXIT_ATOMIC();

  if (conn != DEF_YES) {                                        // Chk class state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  //                                                               ----------------- STORE REPORT DATA ----------------
  if (buf_len > 0) {
    p_buf_data = (CPU_INT08U *)p_buf;
    if (p_ctrl->Report.HasReports == DEF_YES) {
      report_id = p_buf_data[0];
    } else {
      report_id = 0;
    }
    report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                           USBD_HID_REPORT_TYPE_INPUT,
                                           report_id,
                                           &p_buf_report,
                                           &is_largest,
                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (report_len > buf_len) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    eot = (is_largest == DEF_YES) ? DEF_NO : DEF_YES;
  } else {
    p_buf_data = (CPU_INT08U *)0;
    p_buf_report = (CPU_INT08U *)0;
    report_len = 0;
    eot = DEF_YES;
  }

  USBD_HID_OS_TxLock(class_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Save app rx callback.
  p_ctrl->IntrWrAsyncFnct = async_fnct;
  p_ctrl->IntrWrAsyncArgPtr = p_async_arg;

  //                                                               ------------------- INTR IN COMM -------------------
  USBD_IntrTxAsync(p_ctrl->DevNbr,
                   p_comm->DataIntrInEpAddr,
                   p_buf,
                   buf_len,
                   USBD_HID_WrAsyncCmpl,
                   (void *)p_comm,
                   eot,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBD_HID_OS_TxUnlock(class_nbr);
    return;
  }

  if (buf_len > 0) {                                            // Defer copy while transmitting.
    USBD_HID_OS_InputLock(class_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      USBD_HID_OS_TxUnlock(class_nbr);
      return;
    }
    Mem_Copy(&p_buf_report[0], &p_buf_data[0], report_len);
    USBD_HID_OS_InputUnlock(class_nbr);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_HID_Conn()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void USBD_HID_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg)
{
  USBD_HID_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = p_comm;
  p_comm->CtrlPtr->State = USBD_HID_STATE_CONFIG;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_HID_Disconn()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void USBD_HID_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg)
{
  USBD_HID_COMM *p_comm;
  USBD_HID_CTRL *p_ctrl;
  RTOS_ERR      err;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;

  USBD_HID_Report_RemoveAllIdle(&p_ctrl->Report);

  CORE_ENTER_ATOMIC();
  p_ctrl->CommPtr = (USBD_HID_COMM *)0;
  p_ctrl->State = USBD_HID_STATE_INIT;
  if (p_ctrl->IsRx == DEF_TRUE) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(err, RTOS_ERR_ABORT);
    p_ctrl->IntrRdAsyncFnct(p_ctrl->ClassNbr,
                            p_ctrl->RxBufPtr,
                            p_ctrl->RxBufLen,
                            0,
                            (void *)p_ctrl->IntrRdAsyncArgPtr,
                            err);

    CORE_ENTER_ATOMIC();
    p_ctrl->IsRx = DEF_FALSE;
    p_ctrl->RxBufLen = 0;
    p_ctrl->RxBufPtr = (CPU_INT08U *)0;

    p_ctrl->IntrRdAsyncFnct = 0;
    p_ctrl->IntrRdAsyncArgPtr = 0;
    CORE_EXIT_ATOMIC();
  } else {
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                       USBD_HID_AltSettingUpdate()
 *
 * @brief    Notify class that interface alternate setting has been updated.
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
static void USBD_HID_AltSettingUpdate(CPU_INT08U dev_nbr,
                                      CPU_INT08U config_nbr,
                                      CPU_INT08U if_nbr,
                                      CPU_INT08U if_alt_nbr,
                                      void       *p_if_class_arg,
                                      void       *p_if_alt_class_arg)
{
  USBD_HID_COMM *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = p_comm;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 *                                       USBD_HID_EP_StateUpdate()
 *
 * Description : Notify class that endpoint state has been updated.
 *
 * Argument(s) : dev_nbr             Device number.
 *
 *               config_nbr          Configuration number.
 *
 *               if_nbr              Interface number.
 *
 *               if_alt_nbr          Interface alternate setting number.
 *
 *               ep_addr             Endpoint address.
 *
 *               p_if_class_arg      Pointer to class argument specific to interface.
 *
 *               p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) EP state may have changed, it can be checked through USBD_EP_IsStalled().
 *******************************************************************************************************/
static void USBD_HID_EP_StateUpdate(CPU_INT08U dev_nbr,
                                    CPU_INT08U config_nbr,
                                    CPU_INT08U if_nbr,
                                    CPU_INT08U if_alt_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_if_class_arg,
                                    void       *p_if_alt_class_arg)

{
  (void)&dev_nbr;
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&ep_addr;
  (void)&p_if_class_arg;
  (void)&p_if_alt_class_arg;
}

/****************************************************************************************************//**
 *                                           USBD_HID_IF_Desc()
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
static void USBD_HID_IF_Desc(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             CPU_INT08U if_nbr,
                             CPU_INT08U if_alt_nbr,
                             void       *p_if_class_arg,
                             void       *p_if_alt_class_arg)

{
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  USBD_HID_IF_DescHandler(dev_nbr, p_if_class_arg);
}

/****************************************************************************************************//**
 *                                           USBD_HID_IF_DescHandler()
 *
 * @brief    Class interface descriptor callback handler.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void USBD_HID_IF_DescHandler(CPU_INT08U dev_nbr,
                                    void       *p_if_class_arg)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT08U    nbr_desc;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;

  nbr_desc = 0u;
  if (p_ctrl->ReportDescLen > 0u) {
    nbr_desc++;
  }
  if (p_ctrl->PhyDescLen > 0u) {
    nbr_desc++;
  }

  USBD_DescWr08(dev_nbr, USBD_HID_DESC_LEN + nbr_desc * 3u);
  USBD_DescWr08(dev_nbr, USBD_HID_DESC_TYPE_HID);
  USBD_DescWr16(dev_nbr, 0x0111);
  USBD_DescWr08(dev_nbr, (CPU_INT08U)p_ctrl->CountryCode);
  USBD_DescWr08(dev_nbr, nbr_desc);

  if (p_ctrl->ReportDescLen > 0u) {
    USBD_DescWr08(dev_nbr, USBD_HID_DESC_TYPE_REPORT);
    USBD_DescWr16(dev_nbr, p_ctrl->ReportDescLen);
  }
  if (p_ctrl->PhyDescLen > 0u) {
    USBD_DescWr08(dev_nbr, USBD_HID_DESC_TYPE_PHYSICAL);
    USBD_DescWr16(dev_nbr, p_ctrl->PhyDescLen);
  }
}

/****************************************************************************************************//**
 *                                           USBD_HID_IF_DescSizeGet()
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
static CPU_INT16U USBD_HID_IF_DescSizeGet(CPU_INT08U dev_nbr,
                                          CPU_INT08U config_nbr,
                                          CPU_INT08U if_nbr,
                                          CPU_INT08U if_alt_nbr,
                                          void       *p_if_class_arg,
                                          void       *p_if_alt_class_arg)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT08U    nbr_desc;

  (void)&dev_nbr;
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;

  nbr_desc = 0u;
  if (p_ctrl->ReportDescLen > 0u) {
    nbr_desc++;
  }
  if (p_ctrl->PhyDescLen > 0u) {
    nbr_desc++;
  }

  return (USBD_HID_DESC_LEN + nbr_desc * 3u);
}

/****************************************************************************************************//**
 *                                               USBD_HID_IF_Req()
 *
 * @brief    Process interface requests.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   DEF_OK,   if NO error(s) occurred and request is supported.
 *
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) HID class supports 3 class-specific descriptors:
 *
 *           - (a) HID      descriptor (mandatory)
 *
 *           - (a) Report   descriptor (mandatory)
 *
 *           - (a) Physical descriptor (optional)
 *
 *               HID descriptor is sent to Host as part of the Configuration descriptor. Report and
 *               Physical descriptors are retrieved by the host using a GET_DESCRIPTOR standard request
 *               with the recipient being the interface. The way to get Report and Physical descriptors
 *               is specific to the HID class. The HID class specification indicates in the section
 *               '7.1 Standard Requests':
 *
 *               "The HID class uses the standard request Get_Descriptor as described in the USB Specification.
 *               When a Get_Descriptor(Configuration) request is issued, it returns the Configuration descriptor,
 *               all Interface descriptors, all Endpoint descriptors, and the HID descriptor for each interface.
 *               It shall not return the String descriptor, HID Report descriptor or any of the
 *               optional HID class descriptors."
 *
 * @note     (2) The HID descriptor identifies the length and type of subordinate descriptors for a
 *               device.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.1 for more details about HID descriptor.
 *
 * @note     (3) The Report descriptor is made up of items that provide information about the device.
 *               The first part of an item contains three fields: item type, item tag, and item size.
 *               Together these fields identify the kind of information the item provides.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2 for more details about Report descriptor.
 *
 * @note     (4) A Physical Descriptor is a data structure that provides information about the
 *               specific part or parts of the human body that are activating a control or controls.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.3 for more details about Physical descriptor.
 *
 *           - (a) Descriptor set 0 is a special descriptor set that specifies the number of additional
 *                   descriptor sets, and also the number of Physical Descriptors in each set.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_HID_IF_Req(CPU_INT08U           dev_nbr,
                                   const USBD_SETUP_REQ *p_setup_req,
                                   void                 *p_if_class_arg)
{
  USBD_HID_COMM *p_comm;
  CPU_BOOLEAN   valid;
  CPU_BOOLEAN   dev_to_host;
  CPU_INT08U    desc_type;
  CPU_INT08U    w_value;
  CPU_INT16U    desc_len;
  CPU_INT32U    desc_offset;

  (void)&dev_nbr;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  valid = DEF_FAIL;

  dev_to_host = DEF_BIT_IS_SET(p_setup_req->bmRequestType, USBD_REQ_DIR_BIT);

  if (p_setup_req->bRequest == USBD_REQ_GET_DESCRIPTOR) {       // ---------- GET DESCRIPTOR (see Note #1) ------------
    if (dev_to_host != DEF_YES) {
      return (valid);
    }
    //                                                             Get desc type.
    desc_type = (CPU_INT08U)((p_setup_req->wValue >> 8u) & DEF_INT_08_MASK);
    w_value = (CPU_INT08U) (p_setup_req->wValue        & DEF_INT_08_MASK);

    switch (desc_type) {
      case USBD_HID_DESC_TYPE_HID:                              // ------------- HID DESC (see Note #2) ---------------
        if (w_value != 0) {
          break;
        }

        USBD_HID_IF_DescHandler(dev_nbr, p_if_class_arg);
        valid = DEF_OK;
        break;

      case USBD_HID_DESC_TYPE_REPORT:                           // ------------ REPORT DESC (see Note #3) -------------
        if (w_value != 0) {
          break;
        }

        if (p_comm->CtrlPtr->ReportDescLen > 0) {
          USBD_DescWr(dev_nbr,
                      p_comm->CtrlPtr->ReportDescPtr,
                      p_comm->CtrlPtr->ReportDescLen);
          valid = DEF_OK;
        }
        break;

      case USBD_HID_DESC_TYPE_PHYSICAL:                         // ------------ PHYSICAL DESC (see Note #4) -----------
        if (p_comm->CtrlPtr->PhyDescLen < 3) {
          break;
        }

        if (w_value > 0) {
          desc_len = MEM_VAL_GET_INT16U_LITTLE(p_comm->CtrlPtr->PhyDescPtr + 1);
          desc_offset = desc_len * (w_value - 1) + 3;

          if (p_comm->CtrlPtr->PhyDescLen < (desc_offset + desc_len)) {
            break;
          }

          USBD_DescWr(dev_nbr,
                      &p_comm->CtrlPtr->PhyDescPtr[desc_offset],
                      desc_len);
        } else {
          USBD_DescWr(dev_nbr,                                  // See Note #4a.
                      p_comm->CtrlPtr->PhyDescPtr,
                      3);
        }

        valid = DEF_OK;
        break;

      default:                                                  // Other desc type not supported.
        break;
    }
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_HID_ClassReq()
 *
 * @brief    Process class-specific requests.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   DEF_OK,   if NO error(s) occurred and request is supported.
 *
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) HID defines the following class-specific requests :
 *
 *           - (a) GET_REPORT   allows the host to receive a report.
 *
 *           - (b) SET_REPORT   allows the host to send a report to the device, possibly setting
 *                               the state of input, output, or feature controls.
 *
 *           - (c) GET_IDLE     reads the current idle rate for a particular Input report.
 *
 *           - (d) SET_IDLE     silences a particular report on the Interrupt In pipe until a
 *                               new event occurs or the specified amount of time passes.
 *
 *           - (e) GET_PROTOCOL reads which protocol is currently active (either the boot
 *                               protocol or the report protocol).
 *
 *               - (1) Protocol is either BOOT or REPORT.
 *
 *           - (f) SET_PROTOCOL switches between the boot protocol and the report protocol (or
 *                               vice versa).
 *
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about Class-Specific Requests.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_HID_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg)
{
  USBD_HID_COMM     *p_comm;
  USBD_HID_CTRL     *p_ctrl;
  CPU_BOOLEAN       valid;
  CPU_BOOLEAN       dev_to_host;
  RTOS_ERR          err;
  CPU_INT08U        idle_rate;
  CPU_INT08U        report_id;
  CPU_INT08U        report_type;
  CPU_INT16U        report_len;
  CPU_INT08U        protocol;
  CPU_INT08U        *p_buf;
  CPU_INT32U        buf_len;
  CPU_INT16U        req_len;
  CPU_INT08U        w_value;
  CPU_BOOLEAN       is_largest;
  USBD_HID_CALLBACK *p_callback;

  (void)&dev_nbr;

  p_comm = (USBD_HID_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;
  valid = DEF_FAIL;
  p_callback = p_ctrl->CallbackPtr;

  dev_to_host = DEF_BIT_IS_SET(p_setup_req->bmRequestType, USBD_REQ_DIR_BIT);

  switch (p_setup_req->bRequest) {                              // See Note #1.
    case USBD_HID_REQ_GET_REPORT:                               // ------------- GET_REPORT (see Note #1a) ------------
      if (dev_to_host != DEF_YES) {
        break;
      }

      report_type = (CPU_INT08U)((p_setup_req->wValue >> 8u) & DEF_INT_08_MASK);
      report_id = (CPU_INT08U)(p_setup_req->wValue        & DEF_INT_08_MASK);

      switch (report_type) {
        case 1:                                                 // Input report.
          report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                                 USBD_HID_REPORT_TYPE_INPUT,
                                                 report_id,
                                                 &p_buf,
                                                 &is_largest,
                                                 &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          //                                                       Get min between req len & report size.
          buf_len = DEF_MIN(p_setup_req->wLength, report_len);

          USBD_HID_OS_InputLock(p_ctrl->ClassNbr, &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }
          //                                                       Send Report to host.
          USBD_CtrlTx(p_ctrl->DevNbr,
                      (void *)p_buf,
                      buf_len,
                      USBD_HID_CTRL_REQ_TIMEOUT_mS,
                      DEF_NO,
                      &err);
          if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
            valid = DEF_OK;
          }

          USBD_HID_OS_InputUnlock(p_ctrl->ClassNbr);
          break;

        case 3:                                                 // Feature report.
          if ((p_callback == (USBD_HID_CALLBACK *)0)
              || (p_callback->FeatureReportGet == DEF_NULL)) {
            break;                                              // Stall request.
          }

          report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                                 USBD_HID_REPORT_TYPE_FEATURE,
                                                 report_id,
                                                 &p_buf,
                                                 &is_largest,
                                                 &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          //                                                       Get min between req len & report size.
          req_len = DEF_MIN(p_setup_req->wLength, report_len);
          valid = p_callback->FeatureReportGet(p_ctrl->ClassNbr,
                                               report_id,
                                               p_buf,
                                               req_len);
          if (valid != DEF_OK) {
            break;
          }
          //                                                       Send Report to host.
          USBD_CtrlTx(p_ctrl->DevNbr,
                      (void *)p_buf,
                      req_len,
                      USBD_HID_CTRL_REQ_TIMEOUT_mS,
                      DEF_NO,
                      &err);
          if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
            valid = DEF_OK;
          }
          break;

        case 0:
        default:
          break;
      }
      break;

    case USBD_HID_REQ_SET_REPORT:                               // ------------- SET_REPORT (see Note #1b) ------------
      if (dev_to_host != DEF_NO) {
        break;
      }

      report_type = (CPU_INT08U)((p_setup_req->wValue >> 8u) & DEF_INT_08_MASK);
      report_id = (CPU_INT08U)(p_setup_req->wValue        & DEF_INT_08_MASK);

      switch (report_type) {
        case 2:                                                 // Output report.
          if ((p_callback == (USBD_HID_CALLBACK *)0)
              || (p_callback->ReportSet == DEF_NULL)) {
            break;                                              // Stall request.
          }

          report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                                 USBD_HID_REPORT_TYPE_OUTPUT,
                                                 report_id,
                                                 &p_buf,
                                                 &is_largest,
                                                 &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          USBD_HID_OS_OutputLock(p_ctrl->ClassNbr, &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          if (p_setup_req->wLength > report_len) {
            USBD_HID_OS_OutputUnlock(p_ctrl->ClassNbr);
            break;
          }

          //                                                       Receive report from host.
          USBD_CtrlRx(p_ctrl->DevNbr,
                      (void *)p_buf,
                      report_len,
                      USBD_HID_CTRL_REQ_TIMEOUT_mS,
                      &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            USBD_HID_OS_OutputUnlock(p_ctrl->ClassNbr);
            break;
          }

          USBD_HID_OS_OutputUnlock(p_ctrl->ClassNbr);

          p_callback->ReportSet(p_ctrl->ClassNbr,
                                report_id,
                                p_buf,
                                report_len);

          valid = DEF_OK;
          break;

        case 3:                                                 // Feature report.
          if ((p_callback == (USBD_HID_CALLBACK *)0)
              || (p_callback->FeatureReportSet == DEF_NULL)) {
            break;                                              // Stall request.
          }

          report_len = USBD_HID_ReportID_InfoGet(&p_ctrl->Report,
                                                 USBD_HID_REPORT_TYPE_FEATURE,
                                                 report_id,
                                                 &p_buf,
                                                 &is_largest,
                                                 &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          if (p_setup_req->wLength != report_len) {
            break;
          }

          //                                                       Receive report from host.
          USBD_CtrlRx(p_ctrl->DevNbr,
                      (void *)p_buf,
                      report_len,
                      0,
                      &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = p_callback->FeatureReportSet(p_ctrl->ClassNbr,
                                               report_id,
                                               p_buf,
                                               report_len);
          break;

        case 0:
        default:
          break;
      }
      break;

    case USBD_HID_REQ_GET_IDLE:                                 // -------------- GET_IDLE (see Note #1c) -------------
      if (dev_to_host != DEF_YES) {
        break;
      }

      w_value = (CPU_INT08U)((p_setup_req->wValue >> 8u) & DEF_INT_08_MASK);

      if ((p_setup_req->wLength == 1)                           // Chk if setup req is valid.
          && (w_value == 0)) {
        report_id = (CPU_INT08U)(p_setup_req->wValue & DEF_INT_08_MASK);
        //                                                         Get idle rate (duration).
        p_ctrl->CtrlStatusBufPtr[0] = USBD_HID_ReportID_IdleGet(&p_ctrl->Report,
                                                                report_id,
                                                                &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          break;
        }

        USBD_CtrlTx(p_ctrl->DevNbr,
                    (void *)&p_ctrl->CtrlStatusBufPtr[0],
                    1u,
                    USBD_HID_CTRL_REQ_TIMEOUT_mS,
                    DEF_NO,
                    &err);
        if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
          valid = DEF_OK;
        }
      }
      break;

    case USBD_HID_REQ_SET_IDLE:                                 // -------------- SET_IDLE (see Note #1d) -------------
      if (dev_to_host != DEF_NO) {
        break;
      }

      if (p_setup_req->wLength > 0) {                           // Chk if setup req is valid.
        break;
      }

      idle_rate = (CPU_INT08U)((p_setup_req->wValue >> 8u) & DEF_INT_08_MASK);
      report_id = (CPU_INT08U)(p_setup_req->wValue        & DEF_INT_08_MASK);

      USBD_HID_ReportID_IdleSet(&p_ctrl->Report,
                                report_id,
                                idle_rate,
                                &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    case USBD_HID_REQ_GET_PROTOCOL:                             // ------------ GET_PROTOCOL (see Note #1e) -----------
      if ((p_callback == (USBD_HID_CALLBACK *)0)
          || (p_callback->ProtocolGet == DEF_NULL)) {
        break;                                                  // Stall request.
      }

      if (dev_to_host != DEF_YES) {
        break;
      }

      if ((p_setup_req->wLength != 1)                           // Chk if setup req is valid.
          || (p_setup_req->wValue != 0)) {
        break;
      }
      //                                                           Get currently active protocol from app.
      p_ctrl->CtrlStatusBufPtr[0] = p_callback->ProtocolGet(p_ctrl->ClassNbr,
                                                            &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        break;
      }
      //                                                           Send active protocol to host (see Note #1e1).
      USBD_CtrlTx(p_ctrl->DevNbr,
                  (void *)&p_ctrl->CtrlStatusBufPtr[0],
                  1u,
                  USBD_HID_CTRL_REQ_TIMEOUT_mS,
                  DEF_NO,
                  &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    case USBD_HID_REQ_SET_PROTOCOL:                             // ------------ SET_PROTOCOL (see Note #1f) -----------
      if ((p_callback == (USBD_HID_CALLBACK *)0)
          || (p_callback->ProtocolSet == DEF_NULL)) {
        break;                                                  // Stall request.
      }

      if (dev_to_host != DEF_NO) {
        break;
      }

      if ((p_setup_req->wLength != 0)                           // Chk if setup req is valid.
          || ((p_setup_req->wValue != 0)
              && (p_setup_req->wValue != 1))) {
        break;
      }

      protocol = (CPU_INT08U)p_setup_req->wValue;
      //                                                           Set new protocol.
      p_callback->ProtocolSet(p_ctrl->ClassNbr,
                              protocol,
                              &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_HID_OutputDataCmpl()
 *
 * @brief    Inform the class about the set report transfer completion on control endpoint.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status.
 *******************************************************************************************************/
static void USBD_HID_OutputDataCmpl(CPU_INT08U class_nbr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_arg,
                                    RTOS_ERR   err)
{
  CPU_INT32U *p_xfer_len;

  (void)&class_nbr;
  (void)&p_buf;
  (void)&buf_len;

  p_xfer_len = (CPU_INT32U *)p_arg;

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    *p_xfer_len = xfer_len;
    USBD_HID_OS_OutputDataPost(class_nbr);
  } else {
    *p_xfer_len = 0;
    USBD_HID_OS_OutputDataPendAbort(class_nbr);
  }
}

/****************************************************************************************************//**
 *                                           USBD_HID_RdAsyncCmpl()
 *
 * @brief    Inform the application about the Bulk OUT transfer completion.
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
static void USBD_HID_RdAsyncCmpl(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 void       *p_buf,
                                 CPU_INT32U buf_len,
                                 CPU_INT32U xfer_len,
                                 void       *p_arg,
                                 RTOS_ERR   err)
{
  USBD_HID_COMM       *p_comm;
  USBD_HID_CTRL       *p_ctrl;
  USBD_HID_ASYNC_FNCT fnct;
  void                *p_fnct_arg;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_comm = (USBD_HID_COMM *)p_arg;
  p_ctrl = p_comm->CtrlPtr;
  fnct = p_ctrl->IntrRdAsyncFnct;
  p_fnct_arg = p_ctrl->IntrRdAsyncArgPtr;

  p_comm->DataIntrOutActiveXfer = DEF_NO;                       // Xfer finished, no more active xfer.

  fnct(p_ctrl->ClassNbr,                                        // Notify app about xfer completion.
       p_buf,
       buf_len,
       xfer_len,
       p_fnct_arg,
       err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_WrAsyncCmpl()
 *
 * @brief    Inform the application about the Bulk IN transfer completion.
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
static void USBD_HID_WrAsyncCmpl(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 void       *p_buf,
                                 CPU_INT32U buf_len,
                                 CPU_INT32U xfer_len,
                                 void       *p_arg,
                                 RTOS_ERR   err)
{
  USBD_HID_COMM *p_comm;
  USBD_HID_CTRL *p_ctrl;

  (void)&dev_nbr;
  (void)&ep_addr;

  p_comm = (USBD_HID_COMM *)p_arg;
  p_ctrl = (USBD_HID_CTRL *)p_comm->CtrlPtr;

  USBD_HID_OS_TxUnlock(p_ctrl->ClassNbr);

  p_ctrl->IntrWrAsyncFnct(p_ctrl->ClassNbr,                     // Notify app about xfer completion.
                          p_buf,
                          buf_len,
                          xfer_len,
                          p_ctrl->IntrWrAsyncArgPtr,
                          err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_WrSyncCmpl()
 *
 * @brief    Inform the class about the Bulk IN transfer completion.
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
static void USBD_HID_WrSyncCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err)
{
  USBD_HID_CTRL *p_ctrl;
  USBD_HID_COMM *p_comm;
  CPU_INT08U    class_nbr;

  (void)&dev_nbr;
  (void)&ep_addr;
  (void)&p_buf;
  (void)&buf_len;

  p_comm = (USBD_HID_COMM *)p_arg;
  p_ctrl = p_comm->CtrlPtr;
  class_nbr = p_ctrl->ClassNbr;

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    p_ctrl->DataIntrInXferLen = xfer_len;
    USBD_HID_OS_InputDataPost(class_nbr);
  } else {
    p_ctrl->DataIntrInXferLen = 0;
    USBD_HID_OS_InputDataPendAbort(class_nbr);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))
