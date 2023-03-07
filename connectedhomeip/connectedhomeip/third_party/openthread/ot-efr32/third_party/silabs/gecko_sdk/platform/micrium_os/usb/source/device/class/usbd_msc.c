/***************************************************************************//**
 * @file
 * @brief USB Device Msc Class
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

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device MSC class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    USBD_MSC_MODULE
#include  <em_core.h>

#include  <common/include/lib_str.h>
#include  <common/include/lib_ascii.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/source/device/class/usbd_scsi_storage_priv.h>
#include  <usb/source/device/class/usbd_scsi_priv.h>
#include  <usb/source/device/class/usbd_msc_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, MSD)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

#define  USBD_MSC_SIG_CBW                          0x43425355
#define  USBD_MSC_SIG_CSW                          0x53425355

#define  USBD_MSC_LEN_CBW                                  31
#define  USBD_MSC_LEN_CSW                                  13

#define  USBD_MSC_DEV_STR_LEN                             12u

#define  USBD_MSC_CTRL_REQ_TIMEOUT_mS                   5000u

#define  USBD_MSC_COM_NBR_MAX               (USBD_MSC_CFG_MAX_NBR_DEV \
                                             * USBD_MSC_CFG_MAX_NBR_CFG)

/********************************************************************************************************
 *                                           SUBCLASS CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 2
 *******************************************************************************************************/

#define  USBD_MSC_SUBCLASS_CODE_RBC                      0x01
#define  USBD_MSC_SUBCLASS_CODE_SFF_8020i                0x02
#define  USBD_MSC_SUBCLASS_CODE_MMC_2                    0x02
#define  USBD_MSC_SUBCLASS_CODE_QIC_157                  0x03
#define  USBD_MSC_SUBCLASS_CODE_UFI                      0x04
#define  USBD_MSC_SUBCLASS_CODE_SFF_8070i                0x05
#define  USBD_MSC_SUBCLASS_CODE_SCSI                     0x06

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 3
 *******************************************************************************************************/

#define  USBD_MSC_PROTOCOL_CODE_CTRL_BULK_INTR_CMD_INTR  0x00
#define  USBD_MSC_PROTOCOL_CODE_CTRL_BULK_INTR           0x01
#define  USBD_MSC_PROTOCOL_CODE_BULK_ONLY                0x50

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 3.
 *
 *           (2) The 'bRequest' field of a class-specific setup request may contain one of these values.
 *
 *           (3) The mass storage reset request is "used to reset the mass storage device and its
 *               associated interface".  The setup request packet will consist of :
 *
 *               (a) bmRequestType = 00100001b (class, interface, host-to-device)
 *               (b) bRequest      =     0xFF
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0000
 *
 *           (4) The get max LUN is used to determine the number of LUN's supported by the device.  The
 *               setup request packet will consist of :
 *
 *               (a) bmRequestType = 10100001b (class, interface, device-to-host)
 *               (b) bRequest      =     0xFE
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0001
 *******************************************************************************************************/

#define  USBD_MSC_REQ_MASS_STORAGE_RESET                 0xFF   // See Notes #3.
#define  USBD_MSC_REQ_GET_MAX_LUN                        0xFE   // See Notes #4.

/********************************************************************************************************
 *                                       COMMAND BLOCK FLAG VALUES
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1.
 *
 *           (2) The 'Flags' field of a command block wrapper may contain one of these values.
 *******************************************************************************************************/

#define  USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE          0x00
#define  USBD_MSC_BMCBWFLAGS_DIR_DEVICE_TO_HOST          0x80

/********************************************************************************************************
 *                                       COMMAND BLOCK STATUS VALUES
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.3, Table 5.3.
 *
 *           (2) The 'Status' field of a command status wrapper may contain one of these values.
 *******************************************************************************************************/

#define  USBD_MSC_BCSWSTATUS_CMD_PASSED                  0x00
#define  USBD_MSC_BCSWSTATUS_CMD_FAILED                  0x01
#define  USBD_MSC_BCSWSTATUS_PHASE_ERROR                 0x02

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_MSC_INIT_CFG USBD_MSC_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .DataBufLen = 512u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBD_MSC *USBD_MSC_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_MSC_INIT_CFG USBD_MSC_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .DataBufLen = 512u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#else
extern USBD_MSC_INIT_CFG USBD_MSC_InitCfg;
#endif

/********************************************************************************************************
 *********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_MSC_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg);

static void USBD_MSC_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg);

static void USBD_MSC_EP_StateUpdate(CPU_INT08U dev_nbr,
                                    CPU_INT08U config_nbr,
                                    CPU_INT08U if_nbr,
                                    CPU_INT08U if_alt_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_if_class_arg,
                                    void       *p_if_alt_class_arg);

static CPU_BOOLEAN USBD_MSC_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg);

static void USBD_MSC_RxCBW(USBD_MSC_CTRL *p_ctrl,
                           USBD_MSC_COMM *p_comm);

static void USBD_MSC_CMD_Process(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm);

static void USBD_MSC_TxCSW(USBD_MSC_CTRL *p_ctrl,
                           USBD_MSC_COMM *p_comm);

static CPU_BOOLEAN USBD_MSC_CBW_Verify(const USBD_MSC_CTRL *p_ctrl,
                                       USBD_MSC_COMM       *p_comm,
                                       void                *p_cbw_buf,
                                       CPU_INT32U          cbw_len);

static CPU_BOOLEAN USBD_MSC_RespVerify(USBD_MSC_COMM *p_comm,
                                       CPU_INT32U    scsi_data_len,
                                       CPU_INT08U    scsi_data_dir);

static void USBD_MSC_SCSI_TxData(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm);

static void USBD_MSC_SCSI_Rd(USBD_MSC_CTRL *p_ctrl,
                             USBD_MSC_COMM *p_comm);

static void USBD_MSC_SCSI_Wr(const USBD_MSC_CTRL *p_ctrl,
                             USBD_MSC_COMM       *p_comm,
                             void                *p_buf,
                             CPU_INT32U          xfer_len);

static void USBD_MSC_SCSI_RxData(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm);

static void USBD_MSC_CBW_Parse(USBD_MSC_CBW *p_cbw,
                               void         *p_buf_src);

static void USBD_MSC_CSW_Fmt(const USBD_MSC_CSW *p_csw,
                             void               *p_buf_dest);

/********************************************************************************************************
 *                                               MSC CLASS DRIVER
 *******************************************************************************************************/

USBD_CLASS_DRV USBD_MSC_Drv = {
  USBD_MSC_Conn,
  USBD_MSC_Disconn,
  DEF_NULL,                                                     // MSC does NOT use alternate IF(s).
  USBD_MSC_EP_StateUpdate,
  DEF_NULL,                                                     // MSC does NOT use IF functional desc.
  DEF_NULL,
  DEF_NULL,                                                     // MSC does NOT use EP functional desc.
  DEF_NULL,
  DEF_NULL,                                                     // MSC does NOT handle std req with IF recipient.
  USBD_MSC_ClassReq,
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
 *                                   USBD_MSC_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the MSC class is initialized via the
 *               USBD_MSC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_MSC_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBD_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_MSC_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_MSC_ConfigureDataBufLen()
 *
 * @brief    Configures the length, in octets, of the buffer used to exchange MSC data with the USB
 *           host.
 *
 * @param    data_buf_len    Data buffer length, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the MSC class is initialized via the
 *               USBD_MSC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_MSC_ConfigureDataBufLen(CPU_INT32U data_buf_len)
{
  RTOS_ASSERT_CRITICAL((USBD_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_MSC_InitCfg.DataBufLen = data_buf_len;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_MSC_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the MSC class is initialized via the
 *               USBD_MSC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_MSC_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBD_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_MSC_InitCfg.MemSegPtr = p_mem_seg;
  USBD_MSC_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//***
 *                                               USBD_MSC_Init()
 *
 * @brief    Initializes internal structures and variables used by the Mass Storage Class
 *           Bulk Only Transport.
 *
 * @param    p_qty_cfg   Pointer to the MSC configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_MSC_Init(const USBD_MSC_QTY_CFG *p_qty_cfg,
                   RTOS_ERR               *p_err)
{
  CPU_INT08U    ix;
  USBD_MSC      *p_msc;
  USBD_MSC_CTRL *p_ctrl;
  USBD_MSC_COMM *p_comm;
  USBD_SCSI_CFG scsi_cfg;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ClassInstanceQty > 0u)
                           && (p_qty_cfg->ConfigQty > 0u)
                           && (p_qty_cfg->LUN_Qty > 0u)
                           && (USBD_MSC_InitCfg.DataBufLen > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  p_msc = (USBD_MSC *)Mem_SegAlloc("USBD - MSC root struct",
                                   USBD_MSC_InitCfg.MemSegPtr,
                                   sizeof(USBD_MSC),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_msc->CtrlNbrNext = p_qty_cfg->ClassInstanceQty;
  p_msc->CommNbrNext = p_qty_cfg->ClassInstanceQty * p_qty_cfg->ConfigQty;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  p_msc->ClassInstanceQty = p_qty_cfg->ClassInstanceQty;
  p_msc->LUN_Qty = p_qty_cfg->LUN_Qty;
#endif

  USBD_MSC_OS_Init(p_qty_cfg,                                   // Init MSC OS layer.
                   USBD_MSC_InitCfg.MemSegPtr,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBD_MSC_Ptr = p_msc;
  CORE_EXIT_ATOMIC();

  USBD_MSC_Ptr->CtrlTbl = (USBD_MSC_CTRL *)Mem_SegAlloc("USBD - MSC ctrl tbl",
                                                        USBD_MSC_InitCfg.MemSegPtr,
                                                        sizeof(USBD_MSC_CTRL) * p_qty_cfg->ClassInstanceQty,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < p_qty_cfg->ClassInstanceQty; ix++) {       // Init MSC class struct.
    p_ctrl = &USBD_MSC_Ptr->CtrlTbl[ix];
    p_ctrl->State = USBD_MSC_STATE_NONE;
    p_ctrl->CommPtr = DEF_NULL;
    p_ctrl->MaxLun = 0u;

    p_ctrl->CBW_BufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - MSC CBW buffer",
                                                      USBD_MSC_InitCfg.MemSegBufPtr,
                                                      USBD_MSC_LEN_CBW,
                                                      USBD_MSC_InitCfg.BufAlignOctets,
                                                      DEF_NULL,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_ctrl->CBW_BufPtr,
            USBD_MSC_LEN_CBW);

    p_ctrl->CSW_BufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - MSC CSW buffer",
                                                      USBD_MSC_InitCfg.MemSegBufPtr,
                                                      USBD_MSC_LEN_CSW,
                                                      USBD_MSC_InitCfg.BufAlignOctets,
                                                      DEF_NULL,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_ctrl->CSW_BufPtr,
            USBD_MSC_LEN_CSW);

    p_ctrl->DataBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - MSC data buffer",
                                                      USBD_MSC_InitCfg.MemSegBufPtr,
                                                      USBD_MSC_InitCfg.DataBufLen,
                                                      USBD_MSC_InitCfg.BufAlignOctets,
                                                      DEF_NULL,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_ctrl->DataBufPtr,
            USBD_MSC_InitCfg.DataBufLen);

    p_ctrl->CtrlStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - MSC ctrl status buf",
                                                            USBD_MSC_InitCfg.MemSegBufPtr,
                                                            1u,
                                                            USBD_MSC_InitCfg.BufAlignOctets,
                                                            DEF_NULL,
                                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_ctrl->LunDataPtrTbl = (void **)Mem_SegAlloc("USBD - MSC class instance lun tbl",
                                                  USBD_MSC_InitCfg.MemSegPtr,
                                                  sizeof(void *) * p_qty_cfg->LUN_Qty,
                                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  USBD_MSC_Ptr->CommTbl = (USBD_MSC_COMM *)Mem_SegAlloc("USBD - MSC comm tbl",
                                                        USBD_MSC_InitCfg.MemSegPtr,
                                                        sizeof(USBD_MSC_COMM) * p_qty_cfg->ConfigQty,
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < p_qty_cfg->ConfigQty; ix++) {              // Init test class EP tbl.
    p_comm = &USBD_MSC_Ptr->CommTbl[ix];

    p_comm->DataBulkInEpAddr = USBD_EP_ADDR_NONE;
    p_comm->DataBulkOutEpAddr = USBD_EP_ADDR_NONE;
    p_comm->CtrlPtr = DEF_NULL;

    p_comm->CBW.Signature = 0u;
    p_comm->CBW.Tag = 0u;
    p_comm->CBW.DataTransferLength = 0u;
    p_comm->CBW.Flags = 0u;
    p_comm->CBW.LUN = 0u;
    p_comm->CBW.CB_Len = 0u;

    Mem_Clr((void *)p_comm->CBW.CB,
            sizeof(p_comm->CBW.CB));

    p_comm->CSW.Signature = 0u;
    p_comm->CSW.Tag = 0u;
    p_comm->CSW.DataResidue = 0u;
    p_comm->CSW.Status = 0u;

    p_comm->Stall = DEF_NO;
    p_comm->BytesToXfer = 0u;
    p_comm->SCSIWrBufPtr = DEF_NULL;
    p_comm->SCSIWrBuflen = 0u;
  }

  //                                                               Init SCSI layer.
  scsi_cfg.LUN_Qty = p_qty_cfg->ClassInstanceQty * p_qty_cfg->LUN_Qty;
  USBD_SCSI_Init(&scsi_cfg,
                 USBD_MSC_InitCfg.MemSegPtr,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                               USBD_MSC_Add()
 *
 * @brief    Adds a new instance of the Mass Storage Class.
 *
 * @param    p_msc_task_cfg  Pointer to the configuration structure of the task to be created for this MSC
 *                           instance.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Class instance number, if no errors are returned.
 *           USBD_CLASS_NBR_NONE,   if any errors are returned.
 *******************************************************************************************************/
CPU_INT08U USBD_MSC_Add(RTOS_TASK_CFG *p_msc_task_cfg,
                        RTOS_ERR      *p_err)
{
  CPU_INT08U    msc_nbr;
  USBD_MSC_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CLASS_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_msc_task_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_CLASS_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_MSC_Ptr->CtrlNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (USBD_CLASS_NBR_NONE);
  }

  USBD_MSC_Ptr->CtrlNbrNext--;
  msc_nbr = USBD_MSC_Ptr->CtrlNbrNext;                          // Alloc new MSC class.
  CORE_EXIT_ATOMIC();

  USBD_MSC_OS_TaskCreate(msc_nbr,
                         p_msc_task_cfg,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[msc_nbr];

  p_ctrl->ClassNbr = msc_nbr;

  return (msc_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_TaskPrioSet()
 *
 * @brief    Sets priority of the given MSC class instance.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    prio        Priority of the MSC instance's task.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void USBD_MSC_TaskPrioSet(CPU_INT08U     class_nbr,
                          RTOS_TASK_PRIO prio,
                          RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBD_MSC_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  USBD_MSC_OS_TaskPrioSet(class_nbr, prio, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_ConfigAdd()
 *
 * @brief    Adds an existing MSC instance to the specified configuration and device.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    config_nbr  Configuration index to which to add the existing MSC interface.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_IF_ALT_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_CLASS_INSTANCE_ALLOC
 *                           - RTOS_ERR_IF_ALLOC
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_EP_ALLOC
 *                           - RTOS_ERR_EP_NONE_AVAIL
 *
 * @return   DEF_YES, if the MSC instance is added to USB device configuration successfully.
 *           DEF_NO,  if it fails to be added.
 *
 * @note     (1) USBD_MSC_ConfigAdd() adds an Interface descriptor and its associated Endpoint
 *               descriptor(s) to the Configuration descriptor. One call to USBD_MSC_ConfigAdd() builds
 *               the Configuration descriptor corresponding to an MSC device with the following format:
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (MSC)
 *               |-- Endpoint Descriptor (Bulk OUT)
 *               |-- Endpoint Descriptor (Bulk IN)
 *               @endverbatim
 *               If USBD_MSC_ConfigAdd() is called several times from the application, it creates multiple
 *               instances and configurations. For example, the following architecture could be created
 *               for a high-speed device:
 *               @verbatim
 *               High-speed
 *               |-- Configuration 0
 *               |-- Interface 0 (MSC 0)
 *               |-- Configuration 1
 *               |-- Interface 0 (MSC 0)
 *               |-- Interface 1 (MSC 1)
 *               @endverbatim
 *               In this example, there are two instances of MSC: 'MSC 0' and 'MSC 1', and two possible
 *               configurations for the device: 'Configuration 0' and 'Configuration 1'. 'Configuration 1'
 *               is composed of two interfaces.
 *               @n
 *               Each class instance has an association with one of the interfaces. If 'Configuration 1'
 *               is activated by the host, it allows the host to access two different functionalities
 *               offered by the device.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_MSC_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err)
{
  USBD_MSC_CTRL  *p_ctrl;
  USBD_MSC_COMM  *p_comm;
  USBD_DEV_CFG   *p_dev_cfg;
  const CPU_CHAR *p_str;
  CPU_INT16U     str_len;
  CPU_INT08U     if_nbr;
  CPU_INT08U     ep_addr;
  CPU_INT16U     comm_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_dev_cfg = USBD_DevCfgGet(dev_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  if (p_dev_cfg->SerialNbrStrPtr != DEF_NULL) {
    p_str = &p_dev_cfg->SerialNbrStrPtr[0];
    str_len = 0u;
    while ((*p_str != (CPU_CHAR)'\0')                           // Chk for NULL chars ...
           && (p_str != (const CPU_CHAR *)  0)) {               // ...  or NULL ptr found.
      if ((ASCII_IsDigHex(*p_str) == DEF_FALSE)                 // Serial nbr must be a hex string.
          || ((ASCII_IsAlpha(*p_str) == DEF_TRUE)               // Make sure that if A-F values are present they ...
              && (ASCII_IsLower(*p_str) == DEF_TRUE))) {        // ... are lower-case.
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (DEF_NO);
      }
      p_str++;
      str_len++;
    }

    if (str_len < USBD_MSC_DEV_STR_LEN) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return (DEF_NO);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (DEF_NO);
  }

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];

  CORE_ENTER_ATOMIC();
  p_ctrl->DevNbr = dev_nbr;

  if (USBD_MSC_Ptr->CommNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CLASS_INSTANCE_ALLOC);
    return (DEF_NO);
  }

  USBD_MSC_Ptr->CommNbrNext--;
  comm_nbr = USBD_MSC_Ptr->CommNbrNext;
  CORE_EXIT_ATOMIC();

  p_comm = &USBD_MSC_Ptr->CommTbl[comm_nbr];

  if_nbr = USBD_IF_Add(dev_nbr,                                 // Add MSC IF desc to config desc.
                       config_nbr,
                       &USBD_MSC_Drv,
                       (void *)p_comm,
                       DEF_NULL,
                       USBD_CLASS_CODE_MASS_STORAGE,
                       USBD_MSC_SUBCLASS_CODE_SCSI,
                       USBD_MSC_PROTOCOL_CODE_BULK_ONLY,
                       "USB Mass Storage Interface",
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  ep_addr = USBD_BulkAdd(dev_nbr,                               // Add bulk-IN EP desc.
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_YES,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  p_comm->DataBulkInEpAddr = ep_addr;                           // Store bulk-IN EP address.

  ep_addr = USBD_BulkAdd(dev_nbr,                               // Add bulk-OUT EP desc.
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_NO,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  p_comm->DataBulkOutEpAddr = ep_addr;                          // Store bulk-OUT EP address.

  CORE_ENTER_ATOMIC();
  p_ctrl->State = USBD_MSC_STATE_INIT;                          // Set class instance to init state.
  p_ctrl->DevNbr = dev_nbr;
  p_ctrl->CommPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();

  p_comm->CtrlPtr = p_ctrl;

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_SCSI_LunAdd()
 *
 * @brief    Adds a SCSI logical unit to the MSC interface.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    p_lu_info   Pointer to the logical unit information structure.
 *
 * @param    p_lu_fncts  Pointer to the structure of callbacks associated to this logical unit. Can be DEF_NULL.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ALLOC
 *
 * @return   Logical Unit Number.
 *******************************************************************************************************/
CPU_INT08U USBD_MSC_SCSI_LunAdd(CPU_INT08U         class_nbr,
                                USBD_SCSI_LU_INFO  *p_lu_info,
                                USBD_SCSI_LU_FNCTS *p_lu_fncts,
                                RTOS_ERR           *p_err)
{
  CPU_INT08U    max_lun;
  USBD_MSC_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_MSC_LU_NBR_INVALID);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, USBD_MSC_LU_NBR_INVALID);
#endif

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];

  CORE_ENTER_ATOMIC();
  max_lun = p_ctrl->MaxLun;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (max_lun >= USBD_MSC_Ptr->LUN_Qty) {
    CORE_EXIT_ATOMIC();

    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NO_MORE_RSRC, USBD_MSC_LU_NBR_INVALID);
  }
#endif

  p_ctrl->MaxLun++;
  CORE_EXIT_ATOMIC();

  p_ctrl->LunDataPtrTbl[max_lun] = USBD_SCSI_LunAdd(max_lun,
                                                    p_lu_info,
                                                    p_lu_fncts,
                                                    p_err);

  return (max_lun);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_SCSI_LunAttach()
 *
 * @brief    Attach a storage media to the given LUN.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    lu_nbr      Logical unit number.
 *
 * @param    media_name  Name of the storage media to use as registered in the platform manager.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_ALREADY_EXISTS
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @note     (1) As soon as this function is called, the media will be shown as available to the host.
 *               In case you are using a removable media (such as an SD card), you MUST call
 *               USBD_MSC_SCSI_LunDetach() once the removable media is removed to mark it as
 *               unavailable for the host.
 *
 * @note     (2) If the LUN has been ejected from the host, calling USBD_MSC_SCSI_LunDetach() and
 *               USBD_MSC_SCSI_LunAttach() will make it re-appear.
 *
 * @note     (3) Use this function with care as ejecting a logical unit in the middle of a
 *               transfer may corrupt the file system.
 *               The only scenarios where the usage of this function is safe are:
 *                   - (a) Host: Read, Embedded app: Read
 *                   - (b) Host: Read, Embedded app: Write
 *               The following scenarios are not considered safe. Use at your own risk.
 *                   - (a) Host: Write, Embedded app: Read
 *                   - (b) Host: Write, Embedded app: Write
 *******************************************************************************************************/
void USBD_MSC_SCSI_LunAttach(CPU_INT08U class_nbr,
                             CPU_INT08U lu_nbr,
                             CPU_CHAR   *media_name,
                             RTOS_ERR   *p_err)
{
  USBD_MSC_CTRL *p_ctrl;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];

  RTOS_ASSERT_DBG_ERR_SET((lu_nbr < p_ctrl->MaxLun), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBD_SCSI_LunAttach(p_ctrl->LunDataPtrTbl[lu_nbr],
                      media_name,
                      p_err);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_SCSI_LunDetach()
 *
 * @brief    Detach a storage media from a LUN.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    lu_nbr      Logical unit number.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @note     (1) After a call to this function, the media will be available to the embedded application.
 *               The standard file API can be used.
 *
 * @note     (2) Use this function with care as ejecting a logical unit in the middle of a
 *               transfer may corrupt the file system.
 *               The only scenarios where the usage of this function is considered safe are:
 *                   - (a) Host: Read, Embedded app: Read
 *                   - (b) Host: Read, Embedded app: Write
 *               The following scenarios are not considered safe. Use at your own risk.
 *                   - (a) Host: Write, Embedded app: Read
 *                   - (b) Host: Write, Embedded app: Write
 *******************************************************************************************************/
void USBD_MSC_SCSI_LunDetach(CPU_INT08U class_nbr,
                             CPU_INT08U lu_nbr,
                             RTOS_ERR   *p_err)
{
  USBD_MSC_CTRL *p_ctrl;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];

  RTOS_ASSERT_DBG_ERR_SET((lu_nbr < p_ctrl->MaxLun), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBD_SCSI_LunDetach(p_ctrl->LunDataPtrTbl[lu_nbr],
                      p_err);
}

/****************************************************************************************************//**
 *                                               USBD_MSC_IsConn()
 *
 * @brief    Gets the MSC connection state of the device.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @return   DEF_YES, if MSC class is connected.
 *           DEF_NO,  if the MSC class fails to connect.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_MSC_IsConn(CPU_INT08U class_nbr)
{
  USBD_MSC_CTRL  *p_ctrl;
  USBD_DEV_STATE state;
  RTOS_ERR       err;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_MSC_Ptr->ClassInstanceQty), err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];

  if (p_ctrl->CommPtr == DEF_NULL) {
    return (DEF_NO);
  }

  state = USBD_DevStateGet(p_ctrl->DevNbr, &err);               // Get dev state.

  //                                                               Return true if dev state config & MSC state config.
  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (state == USBD_DEV_STATE_CONFIGURED)
      && (p_ctrl->State == USBD_MSC_STATE_CONFIG)) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//***
 *                                           USBD_MSC_TaskHandler()
 *
 * @brief    This function is used to handle MSC transfers.
 *
 * @param    class_nbr   MSC instance number.
 *******************************************************************************************************/
void USBD_MSC_TaskHandler(CPU_INT08U class_nbr)
{
  CPU_BOOLEAN   conn;
  RTOS_ERR      err;
  USBD_MSC_CTRL *p_ctrl = &USBD_MSC_Ptr->CtrlTbl[class_nbr];
  USBD_MSC_COMM *p_comm;

  while (DEF_TRUE) {
    conn = USBD_MSC_IsConn(class_nbr);
    if (conn != DEF_YES) {
      //                                                           Wait till MSC state and dev state is connected.
      USBD_MSC_OS_EnumSignalPend(class_nbr,
                                 0u,
                                 &err);
    }

    p_comm = p_ctrl->CommPtr;
    if (p_comm == DEF_NULL) {
      continue;
    }

    switch (p_comm->NextCommState) {
      case USBD_MSC_COMM_STATE_CBW:                             // ---------------- RECEIVE CBW STATE -----------------
        USBD_MSC_RxCBW(p_ctrl,
                       p_comm);
        break;

      case USBD_MSC_COMM_STATE_DATA:                            // --------------- DATA TRANSPORT STATE ---------------
        USBD_MSC_CMD_Process(p_ctrl,
                             p_comm);
        break;

      case USBD_MSC_COMM_STATE_CSW:                             // ---------------- TRAMSIT CSW STATE -----------------
        USBD_MSC_TxCSW(p_ctrl,
                       p_comm);
        break;

      case USBD_MSC_COMM_STATE_RESET_RECOVERY:                  // --------------- RESET RECOVERY STATE ---------------
      case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:
      case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:
        //                                                         Wait on sem for Reset Recovery to complete.
        USBD_MSC_OS_CommSignalPend(class_nbr,
                                   0u,
                                   &err);
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CBW;

        break;

      case USBD_MSC_COMM_STATE_BULK_IN_STALL:                   // --------------- BULK-IN STALL STATE ----------------
                                                                // Wait on sem for clear bulk-IN stall to complete.
        USBD_MSC_OS_CommSignalPend(class_nbr,
                                   0u,
                                   &err);
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
        break;

      case USBD_MSC_COMM_STATE_BULK_OUT_STALL:                  // --------------- BULK-OUT STALL STATE ---------------
                                                                // Wait on sem for clear bulk-OUT stall to complete.
        USBD_MSC_OS_CommSignalPend(class_nbr,
                                   0u,
                                   &err);
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
        break;

      case USBD_MSC_COMM_STATE_NONE:
      default:
        break;
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_MSC_Conn()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface to.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void USBD_MSC_Conn(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          void       *p_if_class_arg)
{
  USBD_MSC_COMM *p_comm;
  RTOS_ERR      os_err;
  CPU_INT08U    lun;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;
  p_comm = (USBD_MSC_COMM *)p_if_class_arg;

  CORE_ENTER_ATOMIC();
  p_comm->CtrlPtr->CommPtr = p_comm;
  p_comm->CtrlPtr->State = USBD_MSC_STATE_CONFIG;               // Set initial MSC state to config.
  p_comm->NextCommState = USBD_MSC_COMM_STATE_CBW;              // Set initial MSC comm state to rx CBW.
  CORE_EXIT_ATOMIC();

  for (lun = 0; lun < p_comm->CtrlPtr->MaxLun; lun++) {         // Perform some SCSI operations on each logical unit.
    USBD_SCSI_Conn(p_comm->CtrlPtr->LunDataPtrTbl[lun]);
  }

  USBD_MSC_OS_EnumSignalPost(p_comm->CtrlPtr->ClassNbr,
                             &os_err);

  LOG_VRB(("USBD MSC: Conn"));
}

/****************************************************************************************************//**
 *                                           USBD_MSC_Disconn()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void USBD_MSC_Disconn(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             void       *p_if_class_arg)
{
  USBD_MSC_COMM       *p_comm;
  USBD_MSC_COMM_STATE comm_state;
  CPU_BOOLEAN         post_signal;
  RTOS_ERR            os_err;
  CPU_INT08U          lun_ix;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&config_nbr;
  p_comm = (USBD_MSC_COMM *)p_if_class_arg;

  CORE_ENTER_ATOMIC();
  comm_state = p_comm->NextCommState;
  switch (comm_state) {
    case USBD_MSC_COMM_STATE_RESET_RECOVERY:
    case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:
    case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:
    case USBD_MSC_COMM_STATE_BULK_IN_STALL:
    case USBD_MSC_COMM_STATE_BULK_OUT_STALL:
      post_signal = DEF_TRUE;
      break;

    case USBD_MSC_COMM_STATE_NONE:
    case USBD_MSC_COMM_STATE_CBW:
    case USBD_MSC_COMM_STATE_DATA:
    case USBD_MSC_COMM_STATE_CSW:
    default:
      post_signal = DEF_FALSE;
      break;
  }
  p_comm->CtrlPtr->CommPtr = DEF_NULL;
  p_comm->CtrlPtr->State = USBD_MSC_STATE_INIT;                 // Set MSC state to init.
  p_comm->NextCommState = USBD_MSC_COMM_STATE_NONE;             // Set MSC comm state to none.
  CORE_EXIT_ATOMIC();

  if (post_signal == DEF_TRUE) {
    USBD_MSC_OS_CommSignalPost(dev_nbr, &os_err);               // Post sem to notify waiting task if comm ...
  }                                                             // ... is in reset recovery and bulk-IN or bulk-OUT ...
                                                                // ... stall states.
                                                                // Unlock each logical unit added to configuration
  for (lun_ix = 0; lun_ix < p_comm->CtrlPtr->MaxLun; lun_ix++) {
    USBD_SCSI_Disconn(p_comm->CtrlPtr->LunDataPtrTbl[lun_ix]);
  }

  LOG_VRB(("USBD MSC: Disconn"));
}

/****************************************************************************************************//**
 *                                           USBD_MSC_EP_StateUpdate()
 *
 * @brief    Notify class that endpoint state has been updated.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    config_nbr          Configuration ix to add the interface.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    ep_addr             Endpoint address.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * @note     (1) For Reset Recovery, the host shall issue:
 *
 *           - (a) a Bulk-Only Mass Storage Reset
 *           - (b) a Clear Feature HALT to the Bulk-In endpoint or Bulk-Out endpoint.
 *           - (c) a Clear Feature HALT to the complement Bulk-Out endpoint or Bulk-In endpoint.
 *******************************************************************************************************/
static void USBD_MSC_EP_StateUpdate(CPU_INT08U dev_nbr,
                                    CPU_INT08U config_nbr,
                                    CPU_INT08U if_nbr,
                                    CPU_INT08U if_alt_nbr,
                                    CPU_INT08U ep_addr,
                                    void       *p_if_class_arg,
                                    void       *p_if_alt_class_arg)
{
  USBD_MSC_COMM *p_comm = (USBD_MSC_COMM *)p_if_class_arg;
  CPU_BOOLEAN   ep_is_stall;
  CPU_BOOLEAN   ep_in_is_stall;
  CPU_BOOLEAN   ep_out_is_stall;
  CPU_INT08U    class_nbr = p_comm->CtrlPtr->ClassNbr;
  RTOS_ERR      err;
  RTOS_ERR      os_err;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  switch (p_comm->NextCommState) {
    case USBD_MSC_COMM_STATE_BULK_IN_STALL:                     // ---------------- BULK-IN STALL STATE ---------------
      if (ep_addr == p_comm->DataBulkInEpAddr) {                // Verify correct EP addr.
        ep_is_stall = USBD_EP_IsStalled(dev_nbr, ep_addr, &err);
        if (ep_is_stall == DEF_FALSE) {                         // Verify that EP is unstalled.
          LOG_VRB(("USBD MSC: UpdateEP Bulk IN Stall, Signal"));
          //                                                       Post sem to notify waiting task.
          USBD_MSC_OS_CommSignalPost(class_nbr, &os_err);
        } else {
          LOG_VRB(("USBD MSC: UpdateEp Bulk In Stall, EP not stalled"));
        }
      } else {
        LOG_ERR(("USBD MSC: UpdateEP Bulk IN Stall, invalid endpoint"));
      }
      break;

    case USBD_MSC_COMM_STATE_BULK_OUT_STALL:                    // --------------- BULK-OUT STALL STATE ---------------
      if (ep_addr == p_comm->DataBulkOutEpAddr) {               // Verify correct EP addr.
        ep_is_stall = USBD_EP_IsStalled(dev_nbr, ep_addr, &err);
        if (ep_is_stall == DEF_FALSE) {                         // Verify that EP is unstalled.
          LOG_VRB(("USBD MSC: UpdateEP Bulk OUT Stall, Signal"));
          //                                                       Post sem to notify waiting task.
          USBD_MSC_OS_CommSignalPost(class_nbr, &os_err);
        } else {
          LOG_VRB(("USBD MSC: UpdateEP Bulk OUT Stall, EP not stalled"));
        }
      } else {
        LOG_ERR(("USBD MSC: UpdateEP Bulk OUT Stall, invalid endpoint"));
      }
      break;

    case USBD_MSC_COMM_STATE_CBW:                               // -------------- RX CBW / TX CSW STATE ---------------
    case USBD_MSC_COMM_STATE_CSW:
    case USBD_MSC_COMM_STATE_DATA:
      LOG_VRB(("USBD MSC: UpdateEP Rx CBW / Process Data / Tx CSW, skip"));
      break;

    case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:      // ---------- RESET RECOVERY BULK-IN STATE ------------
      if (ep_addr == p_comm->DataBulkInEpAddr) {                // Verify correct EP addr.
        ep_is_stall = USBD_EP_IsStalled(dev_nbr, ep_addr, &err);
        if (ep_is_stall == DEF_FALSE) {                         // Verify that EP is unstalled.
          LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP In Cleared, Stall again"));
          USBD_EP_Stall(dev_nbr,
                        p_comm->DataBulkInEpAddr,
                        DEF_SET,
                        &err);
        } else {
          LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP In Stalled"));
        }
      } else {
        LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, invalid endpoint"));
      }

      p_comm->NextCommState = USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL;
      break;

    case USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:     // ---------- RESET RECOVERY BULK-OUT STATE -----------
      if (ep_addr == p_comm->DataBulkOutEpAddr) {               // Verify correct EP addr.
        ep_is_stall = USBD_EP_IsStalled(dev_nbr, ep_addr, &err);
        if (ep_is_stall == DEF_FALSE) {                         // Verify that EP is unstalled.
          LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP OUT Cleared, Stall again"));
          USBD_EP_Stall(dev_nbr,
                        p_comm->DataBulkOutEpAddr,
                        DEF_SET,
                        &err);
        } else {
          LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP OUT Stalled"));
        }
      } else {
        LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, invalid endpoint"));
      }

      p_comm->NextCommState = USBD_MSC_COMM_STATE_RESET_RECOVERY;
      break;

    case USBD_MSC_COMM_STATE_RESET_RECOVERY:                    // -------- RESET RECOVERY STATE (see Notes #1) -------
      ep_in_is_stall = USBD_EP_IsStalled(dev_nbr, p_comm->DataBulkInEpAddr, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {            // Check stall condition of bulk-IN EP.
        LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, IsStalled failed"));
      }

      ep_out_is_stall = USBD_EP_IsStalled(dev_nbr, p_comm->DataBulkOutEpAddr, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {            // Check stall condition of bulk-OUT EP.
        LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, IsStalled failed"));
      }

      if ((ep_in_is_stall == DEF_FALSE) && (ep_out_is_stall == DEF_FALSE)) {
        LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, Signal"));
        USBD_MSC_OS_CommSignalPost(class_nbr, &os_err);
      } else {
        LOG_DBG(("USBD MSC: UpdateEP Reset Recovery, MSC Reset, Clear Stalled"));
      }
      break;

    case USBD_MSC_COMM_STATE_NONE:
    default:
      LOG_VRB(("USBD MSC: UpdateEP Invalid State, Stall IN/OUT"));
      //                                                           Clr stall unexpected here.
      USBD_EP_Stall(dev_nbr,
                    p_comm->DataBulkInEpAddr,
                    DEF_CLR,
                    &err);

      USBD_EP_Stall(dev_nbr,
                    p_comm->DataBulkOutEpAddr,
                    DEF_CLR,
                    &err);
      break;
  }
}

/****************************************************************************************************//**
 *                                           USBD_MSC_ClassReq()
 *
 * @brief    Process class-specific request.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to SETUP request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument provided when calling USBD_IF_Add().
 *
 * @return   DEF_YES, if class request was successful
 *
 *           DEF_NO,  if the class request was not successful.
 *
 * @note     (1) The Mass Storage Reset class request is used to reset the device and its associated
 *               interface. This request readies the device for the next CBW from the host. The host
 *               sends this request via the control endpoint to the device. The device shall preserve
 *               the value of its bulk data toggle bits and endpoint stall conditions despite the
 *               Bulk-Only Mass Storage Reset. The device shall NAK the status stage of the device
 *               request until the Bulk-Only Mass Storage Reset is complete.
 *
 * @note     (2) The Get Max LUN class request is used to determine the number of logical units supported
 *               by the device. The device shall return one byte of data that contains the maximum LUN
 *               supported by the device.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_MSC_ClassReq(CPU_INT08U           dev_nbr,
                                     const USBD_SETUP_REQ *p_setup_req,
                                     void                 *p_if_class_arg)
{
  CPU_INT08U    request;
  CPU_BOOLEAN   valid;
  USBD_MSC_CTRL *p_ctrl;
  USBD_MSC_COMM *p_comm;
  RTOS_ERR      err;

  (void)&dev_nbr;

  request = p_setup_req->bRequest;
  p_comm = (USBD_MSC_COMM *)p_if_class_arg;
  p_ctrl = p_comm->CtrlPtr;
  valid = DEF_NO;

  switch (request) {
    case USBD_MSC_REQ_MASS_STORAGE_RESET:                       // -------- MASS STORAGE RESET (see Notes #1) ---------
      if ((p_setup_req->wValue == 0u)
          && (p_setup_req->wLength == 0u)) {
        LOG_VRB(("USBD MSC: Class Mass Storage Reset, Stall IN/OUT"));
        USBD_EP_Abort(p_comm->CtrlPtr->DevNbr,
                      p_comm->DataBulkInEpAddr,
                      &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          LOG_ERR(("USBD MSC: Class Mass Storage Reset, EP IN Abort failed"));
        }

        USBD_EP_Abort(p_comm->CtrlPtr->DevNbr,
                      p_comm->DataBulkOutEpAddr,
                      &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          LOG_ERR(("USBD MSC: Class Mass Storage Reset, EP OUT Abort failed"));
        }

        if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
          valid = DEF_YES;
        }
      }
      break;

    case USBD_MSC_REQ_GET_MAX_LUN:                              // ------------ GET MAX LUN (see Notes #2) ------------
      if ((p_setup_req->wValue == 0u)
          && (p_setup_req->wLength == 1u)) {
        LOG_VRB(("USBD MSC: Class Get Max Lun"));
        if (p_comm->CtrlPtr->MaxLun > 0u) {
          //                                                       Store max LUN info.
          p_ctrl->CtrlStatusBufPtr[0u] = p_comm->CtrlPtr->MaxLun - 1;
          (void)USBD_CtrlTx(p_ctrl->DevNbr,                     // Tx max LUN info through ctrl EP.
                            (void *)&p_ctrl->CtrlStatusBufPtr[0u],
                            1u,
                            USBD_MSC_CTRL_REQ_TIMEOUT_mS,
                            DEF_NO,
                            &err);
          if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
              && (p_comm->CtrlPtr->State == USBD_MSC_STATE_CONFIG)) {
            valid = DEF_YES;
          }
        }
      }
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//***
 *                                           USBD_MSC_CBW_Verify()
 *
 * @brief    Parses and verify the CBW sent by the Host.
 *
 * @param    p_ctrl      Pointer to MSC instance control structure.
 *
 * @param    p_comm      Pointer to MSC comm structure.
 *
 * @param    p_cbw_buf   Pointer to the raw cbw buffer.
 *
 * @param    cbw_len     Length of the raw cbw buffer.
 *
 * @return   DEF_OK,     if CBW is valid.
 *           DEF_FAIL,   if CBW is invalid.
 *
 * @note     (1) The device performs two verifications on every CBW received. First is that the CBW
 *               is valid. Second is that the CBW is meaningful.
 *
 *           - (a) The device shall consider a CBW valid when:
 *               - (1) The CBW was received after the device had sent a CSW or after a reset.
 *               - (2) The CBW is 31 (1Fh) bytes in length.
 *               - (3) The Signature is equal to 43425355h.
 *
 *           - (b) The device shall consider a CBW meaningful when:
 *               - (1) No reserve bits are set.
 *               - (2) LUN contains a valid LUN supported by the device.
 *               - (3) both cCBWCBLength and the content of the CB are in accordance with
 *                       bInterfaceSubClass
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_MSC_CBW_Verify(const USBD_MSC_CTRL *p_ctrl,
                                       USBD_MSC_COMM       *p_comm,
                                       void                *p_cbw_buf,
                                       CPU_INT32U          cbw_len)
{
  if (cbw_len != USBD_MSC_LEN_CBW) {                            // See note #1a2.
    LOG_ERR(("USBD MSC: Verify CBW, invalid length"));
    return (DEF_FAIL);
  }

  USBD_MSC_CBW_Parse(&p_comm->CBW, p_cbw_buf);                  // Parse the raw buffer into CBW struct.

  //                                                               See note #1a3.
  if (p_comm->CBW.Signature != USBD_MSC_SIG_CBW) {
    LOG_ERR(("USBD MSC: Verify CBW, invalid signature"));
    return (DEF_FAIL);
  }

  //                                                               See note #2b.
  if (((p_comm->CBW.LUN    & 0xF0u) > 0u)
      || ((p_comm->CBW.CB_Len & 0xE0u) > 0u)
      || (p_comm->CBW.LUN >= p_ctrl->MaxLun)) {
    LOG_ERR(("USBD MSC: Verify CBW, invalid LUN/reserved bits"));
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//***
 *                                           USBD_MSC_RespVerify()
 *
 * @brief    Check the data transfer conditions of host and device and set the CSW status field.
 *
 * @param    p_comm          Pointer to MSC comm structure.
 *
 * @param    scsi_data_len   The length of the response the device intends to transfer.
 *
 * @param    scsi_data_dir   The data transfer direction.
 *
 * @return   DEF_OK,     CBW satisfies one of the thirteen cases.
 *           DEF_FAIL,   Mismatch between direction indicated by CBW and SCSI command.
 *
 * @note     (1) "USB Mass Storage Class - Bulk Only Transport", Revision 1.0, Section 6.7, lists
 *               the thirteen cases of host expectation & device intent with descriptions of the
 *               appropriate action.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_MSC_RespVerify(USBD_MSC_COMM *p_comm,
                                       CPU_INT32U    scsi_data_len,
                                       CPU_INT08U    scsi_data_dir)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_comm->Stall = DEF_FALSE;
  //                                                               Check for host and SCSI dirs.
  if ((scsi_data_len)
      && (p_comm->CBW.Flags != scsi_data_dir)) {
    //                                                             Case  8: Hi <> Do || 10: Hn <> Di.
    p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    CORE_EXIT_ATOMIC();

    return (DEF_FAIL);
  }

  if (p_comm->CBW.DataTransferLength == 0u) {                   // -------------------- Hn CASES: ---------------------
    if (scsi_data_len == 0u) {                                  // Case  1: Hn = Dn.
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_PASSED;
    } else {                                                    // Case  2: Hn < Di || 3: Hn < Do.
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    }
    CORE_EXIT_ATOMIC();

    return (DEF_OK);
  }
  //                                                               -------------------- Hi CASES: ---------------------
  if (p_comm->CBW.Flags == USBD_MSC_BMCBWFLAGS_DIR_DEVICE_TO_HOST) {
    if (scsi_data_len == 0u) {                                  // Case  4: Hi >  Dn.
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_FAILED;
      p_comm->Stall = DEF_TRUE;
      //                                                           Case  5: Hi >  Di.
    } else if (p_comm->CBW.DataTransferLength > scsi_data_len) {
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_PASSED;
      p_comm->Stall = DEF_TRUE;
      //                                                           Case  7: Hi <  Di.
    } else if (p_comm->CBW.DataTransferLength < scsi_data_len) {
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    } else {                                                    // Case  6: Hi == Di.
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_PASSED;
    }
    CORE_EXIT_ATOMIC();

    return (DEF_OK);
  }
  //                                                               -------------------- Ho CASES: ---------------------
  if (p_comm->CBW.Flags == USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
    //                                                             Case 9: Ho > Dn || 11: Ho >  Do.
    if (p_comm->CBW.DataTransferLength > scsi_data_len) {
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_PASSED;
      p_comm->Stall = DEF_TRUE;
      //                                                           Case 13: Ho <  Do.
    } else if (p_comm->CBW.DataTransferLength < scsi_data_len) {
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    } else {                                                    // Case 12: Ho ==  Do.
      p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_PASSED;
    }
    CORE_EXIT_ATOMIC();

    return (DEF_OK);
  }
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//***
 *                                           USBD_MSC_CMD_Process()
 *
 * @brief    Process the CBW sent by the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_CMD_Process(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm)
{
  CPU_BOOLEAN result;
  CPU_INT08U  lun;
  CPU_INT32U  scsi_data_len;
  CPU_INT08U  scsi_data_dir;
  RTOS_ERR    stall_err;
  CORE_DECLARE_IRQ_STATE;

  lun = p_comm->CBW.LUN;

  result = USBD_SCSI_CmdProcess(p_ctrl->LunDataPtrTbl[lun],     // Send the CB to SCSI dev.
                                p_comm->CBW.CB,
                                p_ctrl->DataBufPtr,
                                &scsi_data_len,
                                &scsi_data_dir);
  if (result) {                                                 // Verify data xfer conditions.
    result = USBD_MSC_RespVerify(p_comm,
                                 scsi_data_len,
                                 scsi_data_dir);
  } else {                                                      // Set CSW field in preparation to be returned to host.
    p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_FAILED;
  }

  if (result) {                                                 // SCSI command success.
    p_comm->BytesToXfer = DEF_MIN(p_comm->CBW.DataTransferLength, scsi_data_len);
    if (p_comm->BytesToXfer > 0u) {                             // Host expects data and device has data.
      if (p_comm->CBW.Flags == USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
        USBD_MSC_SCSI_RxData(p_ctrl, p_comm);                   // Rx data from host on bulk-OUT.
      } else {
        USBD_MSC_SCSI_TxData(p_ctrl, p_comm);                   // Tx data to host on bulk-IN.
      }
    } else {
      if (p_comm->Stall) {                                      // Host expects data and but dev has NO data.
        if (p_comm->CBW.Flags == USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
          CORE_ENTER_ATOMIC();
          p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_OUT_STALL;
          CORE_EXIT_ATOMIC();

          USBD_EP_Stall(p_ctrl->DevNbr,
                        p_comm->DataBulkOutEpAddr,
                        DEF_SET,
                        &stall_err);
        } else {                                                // Direction from dev to the host.
          CORE_ENTER_ATOMIC();
          p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_IN_STALL;
          CORE_EXIT_ATOMIC();

          USBD_EP_Stall(p_ctrl->DevNbr,
                        p_comm->DataBulkInEpAddr,
                        DEF_SET,
                        &stall_err);
        }
      } else {                                                  // Host expects NO data.
        CORE_ENTER_ATOMIC();
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
        CORE_EXIT_ATOMIC();
      }
    }
  } else {                                                      // SCSI cmd failed.
    if (p_comm->CBW.DataTransferLength == 0u) {                 // If no data stage send CSW to host.
      CORE_ENTER_ATOMIC();
      p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
      CORE_EXIT_ATOMIC();
      //                                                           If data stage is dost to dev, ...
      //                                                           ... stall OUT pipe and send CSW.
    } else if (p_comm->CBW.Flags == USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
      CORE_ENTER_ATOMIC();
      p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkOutEpAddr,
                    DEF_SET,
                    &stall_err);
    } else {                                                    // If data stage is dev to host, ...
                                                                // ... stall IN pipe and wait for clear stall.
      CORE_ENTER_ATOMIC();
      p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_IN_STALL;
      CORE_EXIT_ATOMIC();

      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkInEpAddr,
                    DEF_SET,
                    &stall_err);
    }
  }
}

/****************************************************************************************************//***
 *                                           USBD_MSC_SCSI_TxData()
 *
 * @brief    Reads data from the SCSI.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_SCSI_TxData(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm)
{
  CPU_INT32U scsi_buf_len;
  RTOS_ERR   stall_err;
  CORE_DECLARE_IRQ_STATE;

  scsi_buf_len = DEF_MIN(p_comm->BytesToXfer, USBD_MSC_InitCfg.DataBufLen);

  while (scsi_buf_len > 0u) {
    USBD_MSC_SCSI_Rd(p_ctrl, p_comm);

    p_comm->BytesToXfer -= scsi_buf_len;                        // Updae remaining bytes to transmit.
    p_comm->CSW.DataResidue -= scsi_buf_len;                    // Update CSW data residue field.
    scsi_buf_len = DEF_MIN(p_comm->BytesToXfer, USBD_MSC_InitCfg.DataBufLen);
  }

  if (p_comm->Stall == DEF_TRUE) {
    p_comm->Stall = DEF_FALSE;

    CORE_ENTER_ATOMIC();                                        // Set the next state to bulk-IN stall.
    p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    USBD_EP_Stall(p_ctrl->DevNbr,
                  p_comm->DataBulkInEpAddr,
                  DEF_SET,
                  &stall_err);
  } else {
    CORE_ENTER_ATOMIC();                                        // Set the next state to tx CSW.
    p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//***
 *                                           USBD_MSC_SCSI_Rd()
 *
 * @brief    Reads data from the SCSI and transmits data to the host. CSW will be transmitted
 *           after the data completion stage.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_SCSI_Rd(USBD_MSC_CTRL *p_ctrl,
                             USBD_MSC_COMM *p_comm)
{
  CPU_BOOLEAN result;
  CPU_INT32U  scsi_buf_len;
  CPU_INT08U  lun;
  RTOS_ERR    err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  scsi_buf_len = DEF_MIN(p_comm->BytesToXfer, USBD_MSC_InitCfg.DataBufLen);
  lun = p_comm->CBW.LUN;
  CORE_EXIT_ATOMIC();

  result = USBD_SCSI_DataRd(p_ctrl->LunDataPtrTbl[lun],         // Rd data from the SCSI.
                            p_comm->CBW.CB[0u],
                            p_ctrl->DataBufPtr,
                            scsi_buf_len);
  if (!result) {
    p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_FAILED;
    goto end_fail;
  }

  (void)USBD_BulkTx(p_ctrl->DevNbr,                             // Tx data to the host.
                    p_comm->DataBulkInEpAddr,
                    p_ctrl->DataBufPtr,
                    scsi_buf_len,
                    0u,
                    DEF_NO,
                    &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  return;

end_fail:
  p_comm->Stall = DEF_YES;
}

/****************************************************************************************************//***
 *                                           USBD_MSC_SCSI_Wr()
 *
 * @brief    Save the buffer and buffer length and write data to the SCSI.
 *
 * @param    p_ctrl      Pointer to MSC instance control structure.
 *
 * @param    p_comm      Pointer to MSC comm structure.
 *
 * @param    p_buf       Pointer to the data buffer.
 *
 * @param    xfer_len    Length of the data buffer.
 *******************************************************************************************************/
static void USBD_MSC_SCSI_Wr(const USBD_MSC_CTRL *p_ctrl,
                             USBD_MSC_COMM       *p_comm,
                             void                *p_buf,
                             CPU_INT32U          xfer_len)
{
  CPU_BOOLEAN result;
  CPU_INT08U  lun;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_comm->SCSIWrBufPtr = p_buf;
  p_comm->SCSIWrBuflen = xfer_len;
  CORE_EXIT_ATOMIC();

  lun = p_comm->CBW.LUN;
  result = USBD_SCSI_DataWr(p_ctrl->LunDataPtrTbl[lun],         // Wr data to SCSI sto.
                            p_comm->CBW.CB[0u],
                            p_comm->SCSIWrBufPtr,
                            p_comm->SCSIWrBuflen);
  if (!result) {
    CORE_ENTER_ATOMIC();                                        // Enter bulk-OUT stall state.
    p_comm->CSW.Status = USBD_MSC_BCSWSTATUS_CMD_FAILED;
    CORE_EXIT_ATOMIC();

    LOG_ERR(("USBD MSC: SCSI Wr, Stall OUT"));

    p_comm->Stall = DEF_TRUE;
  }
}

/****************************************************************************************************//***
 *                                           USBD_MSC_SCSI_RxData()
 *
 * @brief    It receives data from the host. After the data stage is complete, CSW is sent.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_SCSI_RxData(USBD_MSC_CTRL *p_ctrl,
                                 USBD_MSC_COMM *p_comm)
{
  CPU_INT32U scsi_buf_len;
  CPU_INT32U xfer_len;
  RTOS_ERR   err;
  RTOS_ERR   stall_err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  scsi_buf_len = DEF_MIN(p_comm->BytesToXfer, USBD_MSC_InitCfg.DataBufLen);
  CORE_EXIT_ATOMIC();

  while (scsi_buf_len > 0) {
    LOG_VRB(("USBD MSC: Rx Data Len:", (u)scsi_buf_len));
    xfer_len = USBD_BulkRx(p_ctrl->DevNbr,                      // Rx data from host on bulk-OUT pipe
                           p_comm->DataBulkOutEpAddr,
                           p_ctrl->DataBufPtr,
                           scsi_buf_len,
                           0u,
                           &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      CORE_ENTER_ATOMIC();                                      // Enter reset recovery state if err.
      p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      LOG_ERR(("USBD MSC: Rx Data, Stall OUT"));

      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkOutEpAddr,
                    DEF_SET,
                    &stall_err);
      return;
    } else {
      //                                                           Process rx data if no err.
      USBD_MSC_SCSI_Wr(p_ctrl,
                       p_comm,
                       p_ctrl->DataBufPtr,
                       xfer_len);
      p_comm->BytesToXfer -= xfer_len;
      p_comm->CSW.DataResidue -= xfer_len;
      scsi_buf_len = DEF_MIN(p_comm->BytesToXfer, USBD_MSC_InitCfg.DataBufLen);
    }
  }

  if (p_comm->Stall) {
    CORE_ENTER_ATOMIC();
    p_comm->Stall = DEF_FALSE;                                  // Enter bulk-OUT stall state.
    p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_OUT_STALL;
    CORE_EXIT_ATOMIC();

    LOG_ERR(("USBD MSC: Rx Data, Stall OUT"));
    USBD_EP_Stall(p_ctrl->DevNbr,
                  p_comm->DataBulkOutEpAddr,
                  DEF_SET,
                  &stall_err);
    return;
  }

  CORE_ENTER_ATOMIC();                                          // Enter tx CSW state.
  p_comm->NextCommState = USBD_MSC_COMM_STATE_CSW;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//***
 *                                               USBD_MSC_TxCSW()
 *
 * @brief    Send status CSW to the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_TxCSW(USBD_MSC_CTRL *p_ctrl,
                           USBD_MSC_COMM *p_comm)
{
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_comm->CSW.Signature = USBD_MSC_SIG_CSW;                     // Set CSW signature and rcvd tag from CBW.
  p_comm->CSW.Tag = p_comm->CBW.Tag;

  USBD_MSC_CSW_Fmt(&p_comm->CSW,                                // Wr CSW to raw buf.
                   (void *) p_ctrl->CSW_BufPtr);
  CORE_EXIT_ATOMIC();
  //                                                               Tx CSW to host through bulk-IN pipe.
  (void)USBD_BulkTx(p_ctrl->DevNbr,
                    p_comm->DataBulkInEpAddr,
                    p_ctrl->CSW_BufPtr,
                    USBD_MSC_LEN_CSW,
                    0,
                    DEF_NO,
                    &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {                // Enter reset recovery state.
    CORE_ENTER_ATOMIC();
    p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    LOG_ERR(("USBD MSC: TxCSW, Stall IN"));
    USBD_EP_Stall(p_ctrl->DevNbr,
                  p_comm->DataBulkInEpAddr,
                  DEF_SET,
                  &err);
  } else if (p_comm->CSW.Status == USBD_MSC_BCSWSTATUS_PHASE_ERROR) {
    CORE_ENTER_ATOMIC();
    p_comm->NextCommState = USBD_MSC_COMM_STATE_RESET_RECOVERY;
    CORE_EXIT_ATOMIC();
  } else {
    CORE_ENTER_ATOMIC();                                        // Enter rx CBW state.
    p_comm->NextCommState = USBD_MSC_COMM_STATE_CBW;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//***
 *                                               USBD_MSC_RxCBW()
 *
 * @brief    Receive CBW from the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void USBD_MSC_RxCBW(USBD_MSC_CTRL *p_ctrl,
                           USBD_MSC_COMM *p_comm)
{
  CPU_BOOLEAN cbw_ok;
  CPU_INT32U  xfer_len;
  RTOS_ERR    err;
  RTOS_ERR    stall_err;
  CORE_DECLARE_IRQ_STATE;

  xfer_len = USBD_BulkRx(p_ctrl->DevNbr,                        // Rx CBW and returns xfer_len upon success.
                         p_comm->DataBulkOutEpAddr,
                         p_ctrl->CBW_BufPtr,
                         USBD_MSC_LEN_CBW,
                         0,
                         &err);
  LOG_VRB(("USBD MSC: RxCBW with len=", (u)xfer_len));

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_INVALID_DEV_STATE:
    case RTOS_ERR_ABORT:
    case RTOS_ERR_INVALID_EP_STATE:
    case RTOS_ERR_EP_INVALID:
      CORE_ENTER_ATOMIC();
      if (p_ctrl->State == USBD_MSC_STATE_CONFIG) {
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CBW;
      }
      CORE_EXIT_ATOMIC();
      LOG_ERR(("USBD MSC: RxCBW, OS Abort"));
      break;

    case RTOS_ERR_TIMEOUT:
      CORE_ENTER_ATOMIC();
      if (p_ctrl->State == USBD_MSC_STATE_CONFIG) {
        p_comm->NextCommState = USBD_MSC_COMM_STATE_CBW;
      }
      CORE_EXIT_ATOMIC();
      LOG_ERR(("USBD MSC: RxCBW, OS Timeout"));
      break;

    case RTOS_ERR_RX:
    case RTOS_ERR_WOULD_OVF:
      CORE_ENTER_ATOMIC();
      p_comm->NextCommState = USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL;
      CORE_EXIT_ATOMIC();

      LOG_ERR(("USBD MSC: RxCBW, Stall IN/OUT"));
      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkInEpAddr,
                    DEF_SET,
                    &stall_err);

      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkOutEpAddr,
                    DEF_SET,
                    &stall_err);
      break;

    case RTOS_ERR_OS:
    case RTOS_ERR_NULL_PTR:
    case RTOS_ERR_NOT_READY:
    case RTOS_ERR_EP_QUEUING:
    default:
      CORE_ENTER_ATOMIC();
      p_comm->NextCommState = USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      LOG_ERR(("USBD MSC: RxCBW, Stall OUT"));
      USBD_EP_Stall(p_ctrl->DevNbr,
                    p_comm->DataBulkOutEpAddr,
                    DEF_SET,
                    &stall_err);
      break;
  }
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Verify that CBW is valid and meaningful.
  cbw_ok = USBD_MSC_CBW_Verify(p_ctrl,
                               p_comm,
                               p_ctrl->CBW_BufPtr,
                               xfer_len);
  if (cbw_ok != DEF_OK) {                                       // Enter reset recovery state.
    CORE_ENTER_ATOMIC();
    p_comm->NextCommState = USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    LOG_ERR(("USBD MSC: RxCBW, Stall IN/OUT"));

    USBD_EP_Stall(p_ctrl->DevNbr,
                  p_comm->DataBulkInEpAddr,
                  DEF_SET,
                  &stall_err);

    USBD_EP_Stall(p_ctrl->DevNbr,
                  p_comm->DataBulkOutEpAddr,
                  DEF_SET,
                  &stall_err);

    return;
  }

  CORE_ENTER_ATOMIC();
  //                                                               Host expected transfer length.
  p_comm->CSW.DataResidue = p_comm->CBW.DataTransferLength;
  p_comm->BytesToXfer = 0u;
  p_comm->NextCommState = USBD_MSC_COMM_STATE_DATA;             // Enter data transport state.
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_MSC_CBW_Parse()
 *
 * @brief    Parse CBW into CBW structure.
 *
 * @param    p_cbw       Variable that will hold the CBW parsed in this function.
 *
 * @param    p_buf_src   Pointer to buffer that holds CBW.
 *******************************************************************************************************/
static void USBD_MSC_CBW_Parse(USBD_MSC_CBW *p_cbw,
                               void         *p_buf_src)
{
  CPU_INT08U *p_buf_src_08;

  p_buf_src_08 = (CPU_INT08U *)p_buf_src;

  p_cbw->Signature = MEM_VAL_GET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_src_08 +  0u));
  p_cbw->Tag = MEM_VAL_GET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_src_08 +  4u));
  p_cbw->DataTransferLength = MEM_VAL_GET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_src_08 +  8u));
  p_cbw->Flags = p_buf_src_08[12u];
  p_cbw->LUN = p_buf_src_08[13u];
  p_cbw->CB_Len = p_buf_src_08[14u];

  Mem_Copy((void *)&p_cbw->CB[0u],
           (void *)&p_buf_src_08[15u],
           16u);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_CSW_Fmt()
 *
 * @brief    Format CSW from CSW structure.
 *
 * @param    p_csw       Variable holds the CSW information.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold CSW.
 *******************************************************************************************************/
static void USBD_MSC_CSW_Fmt(const USBD_MSC_CSW *p_csw,
                             void               *p_buf_dest)
{
  CPU_INT08U *p_buf_dest_08;

  p_buf_dest_08 = (CPU_INT08U *)p_buf_dest;

  MEM_VAL_SET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_dest_08 + 0u), p_csw->Signature);
  MEM_VAL_SET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_dest_08 + 4u), p_csw->Tag);
  MEM_VAL_SET_INT32U_LITTLE((void *)((CPU_INT08U *)p_buf_dest_08 + 8u), p_csw->DataResidue);

  p_buf_dest_08[12] = p_csw->Status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))
