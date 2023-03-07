/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (Cdc)
 *        Abstract Control Model (Acm) - Serial Emulation
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
 * @note     (1) This implementation is compliant with the PSTN subclass specification revision 1.2
 *               February 9, 2007.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_ACM_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_CDC_AVAIL))

#error USB Device ACM subclass requires USB Device CDC Base Class. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_CDC_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <usb/include/device/usbd_cdc_acm_serial.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                           (USBD, CLASS, CDC, ACM)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_USBD

#define  USBD_ACM_CTRL_REQ_TIMEOUT_mS                  5000u

/********************************************************************************************************
 *                               ACM FUNCTIONAL DESCRIPTORS SIZES DEFINES
 *
 * Note(s) : (1) Table 3 and 4 from the PSTN specification version 1.2 defines the Call management
 *               and ACM management functional descriptors.
 *******************************************************************************************************/

#define  USBD_ACM_DESC_CALL_MGMT_SIZE                     5u
#define  USBD_ACM_DESC_SIZE                               4u
#define  USBD_ACM_DESC_TOT_SIZE                (USBD_ACM_DESC_CALL_MGMT_SIZE \
                                                + USBD_ACM_DESC_SIZE)

/********************************************************************************************************
 *                                   ACM SERIAL NOTIFICATIONS DEFINES
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_REQ_STATE                     0x20u    // Serial state notification code.
#define  USBD_ACM_SERIAL_REQ_STATE_SIZE                   2u    // Serial state notification data size.

#define  USBD_ACM_SERIAL_STATE_BUF_SIZE        (USBD_CDC_NOTIFICATION_HEADER \
                                                + USBD_ACM_SERIAL_REQ_STATE_SIZE)

/********************************************************************************************************
 *                                   ABSTRACT STATE FEATURE DEFINES
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_ABSTRACT_DATA_MUX      DEF_BIT_01
#define  USBD_ACM_SERIAL_ABSTRACT_IDLE          DEF_BIT_00

/********************************************************************************************************
 *                                       LINE STATE SIGNAL DEFINES
 *******************************************************************************************************/
//                                                                 Consistent signals.
#define  USBD_ACM_SERIAL_EVENTS_CONS           (USBD_ACM_SERIAL_STATE_DCD \
                                                | USBD_ACM_SERIAL_STATE_DSR)

//                                                                 Irregular signals.
#define  USBD_ACM_SERIAL_EVENTS_IRRE           (USBD_ACM_SERIAL_STATE_BREAK     \
                                                | USBD_ACM_SERIAL_STATE_RING    \
                                                | USBD_ACM_SERIAL_STATE_PARITY  \
                                                | USBD_ACM_SERIAL_STATE_FRAMING \
                                                | USBD_ACM_SERIAL_STATE_OVERUN)

/********************************************************************************************************
 *                                   SET CONTROL LINE STATE DEFINES
 *
 * Note(s): (1) The PSTN specification version 1.2 defines the 'SetControlLineState' request as:
 *
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *                   | bmRequestType |    bRequestCode   |      wValue      |  wIndex   | wLength | Data |
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *                   | 00100001B     | SET_CONTROL_LINE_ |  Control Signal  | Interface |  Zero   | None |
 *                   |               |       STATE       |     Bitmap       |           |         |      |
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *
 *               (a) Table 18 from the PSTN specification defines the control signal bitmap values for
 *                   the SET_CONTROL_LINE_STATE
 *
 *                   Bit Position
 *                   -------------
 *                       D1          Carrier control for half duplex modems. This signal correspond to
 *                                   V.24 signal 105 and RS-232 signal RTS.
 *
 *                       D0          Indicates to DCE if DTE is present or not. This signal corresponds to
 *                                   V.24 signal 108/2 and RS-232 signal DTR.
 *******************************************************************************************************/

#define USBD_ACM_SERIAL_REQ_DTR                 DEF_BIT_00
#define USBD_ACM_SERIAL_REQ_RTS                 DEF_BIT_01

/********************************************************************************************************
 *                                   COMMUNICATION FEATURE SELECTOR DEFINES
 ** Note(s): (1) The PSTN specification version 1.2 defines the 'GetCommFeature' request as:
 *
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *                   | bmRequestType |    bRequestCode   |      wValue      |  wIndex   |  wLength    |  Data  |
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *                   | 00100001B     | GET_COMM_FEATURE  | Feature Selector | Interface | Length of   | Status |
 *                   |               |                   |     Bitmap       |           | Status Data |        |
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *
 *               (a) Table 14 from the PSTN specification defines the communication feature selector codes:
 *
 *                       Feature             Code     Description
 *                       Selector
 *                   ----------------   --------   ----------------------------------------------------
 *                       ABSTRACT_STATE     01h       Two bytes of data describing multiplexed state and idle
 *                                                   state for this Abstract Model communications device.
 *
 *                       COUNTRY_SETTING    02h       Country code in hexadecimal format as defined in
 *                                                   [ISO3166], release date as specified in offset 3 of
 *                                                   the Country Selection Functional Descriptor. This
 *                                                   selector is only valid for devices that provide a
 *                                                   Country Selection Functional Descriptor, and the value
 *                                                   supplied shall appear as supported country in the
 *                                                   Country Selection Functional Descriptor.
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_ABSTRACT_STATE                0x01u
#define  USBD_ACM_SERIAL_COUNTRY_SETTING               0x02u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_CDC_ACM_SERIAL_INIT_CFG USBD_CDC_ACM_SerialInitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CDC ACM SERIAL CTRL DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_acm_serial_ctrl {                           // --------- ACM SUBCLASS CONTROL INFORMATION ---------
  CPU_INT08U                        Nbr;                        // CDC dev nbr.
  CPU_BOOLEAN                       Idle;
  USBD_ACM_SERIAL_LINE_CODING_CHNGD LineCodingChngdFnct;
  void                              *LineCodingChngdArgPtr;
  USBD_ACM_SERIAL_LINE_CODING       LineCoding;
  USBD_ACM_SERIAL_LINE_CTRL_CHNGD   LineCtrlChngdFnct;
  void                              *LineCtrlChngdArgPtr;
  CPU_INT08U                        LineCtrl;
  CPU_INT08U                        LineState;
  CPU_INT16U                        LineStateInterval;
  CPU_INT08U                        *LineStateBufPtr;
  CPU_BOOLEAN                       LineStateSent;
  CPU_INT08U                        CallMgmtCapabilities;
  CPU_INT08U                        *ReqBufPtr;
} USBD_ACM_SERIAL_CTRL;

/********************************************************************************************************
 *                                   CDC ACM SERIAL SUBCLASS ROOT STRUCT
 *******************************************************************************************************/

typedef struct usbd_cdc_acm_serial {
  USBD_ACM_SERIAL_CTRL *CtrlTbl;                                // Table of CDC ACM subclass instace structure.
  CPU_INT08U           CtrlNbrNext;                             // Index to next CDC ACM subclass instance to allocate.

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_INT08U           SublassInstanceQty;                      // Quantity of subclass instances.
#endif
} USBD_CDC_ACM_SERIAL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_CDC_ACM_SERIAL *USBD_CDC_ACM_SerialPtr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_CDC_ACM_SERIAL_INIT_CFG USBD_CDC_ACM_SerialInitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#else
extern USBD_CDC_ACM_SERIAL_INIT_CFG USBD_CDC_ACM_SerialInitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN USBD_ACM_SerialMgmtReq(CPU_INT08U           dev_nbr,
                                          const USBD_SETUP_REQ *p_setup_req,
                                          void                 *p_subclass_arg);

static void USBD_ACM_SerialNotifyCmpl(CPU_INT08U dev_nbr,
                                      void       *p_subclass_arg);

static void USBD_ACM_SerialFnctDesc(CPU_INT08U dev_nbr,
                                    void       *p_subclass_arg,
                                    CPU_INT08U first_dci_if_nbr);

static CPU_INT16U USBD_ACM_SerialFnctDescSizeGet(CPU_INT08U dev_nbr,
                                                 void       *p_subclass_arg);

/********************************************************************************************************
 *                                           CDC ACM CLASS DRIVER
 *******************************************************************************************************/

static USBD_CDC_SUBCLASS_DRV USBD_ACM_SerialDrv = {
  USBD_ACM_SerialMgmtReq,
  USBD_ACM_SerialNotifyCmpl,
  USBD_ACM_SerialFnctDesc,
  USBD_ACM_SerialFnctDescSizeGet,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           APPLICATION FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBD_ACM_SerialConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC ACM class is initialized via the
 *               USBD_ACM_SerialInit() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_ACM_SerialConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_ACM_SerialPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_ACM_SerialInitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the CDC ACM class is initialized via the
 *               USBD_ACM_SerialInit() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_ACM_SerialConfigureMemSeg(MEM_SEG *p_mem_seg,
                                    MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_ACM_SerialPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_ACM_SerialInitCfg.MemSegPtr = p_mem_seg;
  USBD_CDC_ACM_SerialInitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialInit()
 *
 * @brief    Initializes the CDC ACM serial emulation subclass.
 *
 * @param    subclass_instance_qty   Quantity of CDC ACM subclass instances.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these returned
 *                                   error codes from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_ACM_SerialInit(CPU_INT08U subclass_instance_qty,
                         RTOS_ERR   *p_err)
{
  CPU_INT08U           ix;
  USBD_ACM_SERIAL_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((subclass_instance_qty != 0u), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               Allocate CDC ACM root struct.
  USBD_CDC_ACM_SerialPtr = (USBD_CDC_ACM_SERIAL *)Mem_SegAlloc("USBD - CDC ACM Serial root struct",
                                                               USBD_CDC_ACM_SerialInitCfg.MemSegPtr,
                                                               sizeof(USBD_CDC_ACM_SERIAL),
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBD_CDC_ACM_SerialPtr->SublassInstanceQty = subclass_instance_qty;
#endif

  USBD_CDC_ACM_SerialPtr->CtrlNbrNext = subclass_instance_qty;
  USBD_CDC_ACM_SerialPtr->CtrlTbl = (USBD_ACM_SERIAL_CTRL *)Mem_SegAlloc("USBD - CDC ACM Serial ctrl struct table",
                                                                         USBD_CDC_ACM_SerialInitCfg.MemSegPtr,
                                                                         sizeof(USBD_ACM_SERIAL_CTRL) * USBD_CDC_ACM_SerialPtr->CtrlNbrNext,
                                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Init ACM serial ctrl.
  for (ix = 0u; ix < USBD_CDC_ACM_SerialPtr->CtrlNbrNext; ix++) {
    p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[ix];
    p_ctrl->Nbr = USBD_CDC_NBR_NONE;
    p_ctrl->Idle = DEF_NO;
    p_ctrl->LineCodingChngdFnct = DEF_NULL;
    p_ctrl->LineCodingChngdArgPtr = DEF_NULL;
    p_ctrl->LineCoding.BaudRate = 9600u;
    p_ctrl->LineCoding.Parity = USBD_ACM_SERIAL_PARITY_NONE;
    p_ctrl->LineCoding.StopBits = USBD_ACM_SERIAL_STOP_BIT_1;
    p_ctrl->LineCoding.DataBits = 8u;

    p_ctrl->LineCtrlChngdFnct = DEF_NULL;
    p_ctrl->LineCtrlChngdArgPtr = DEF_NULL;
    p_ctrl->LineCtrl = DEF_BIT_NONE;

    p_ctrl->LineStateSent = DEF_NO;
    p_ctrl->LineStateInterval = 0u;
    p_ctrl->LineState = 0u;
    p_ctrl->CallMgmtCapabilities = 0u;

    //                                                             Alloc control buffers.
    p_ctrl->ReqBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - CDC ACM request buffer",
                                                     USBD_CDC_ACM_SerialInitCfg.MemSegBufPtr,
                                                     7u,
                                                     USBD_CDC_ACM_SerialInitCfg.BufAlignOctets,
                                                     DEF_NULL,
                                                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_ctrl->LineStateBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - CDC ACM line state buffer",
                                                           USBD_CDC_ACM_SerialInitCfg.MemSegBufPtr,
                                                           USBD_ACM_SERIAL_STATE_BUF_SIZE,
                                                           USBD_CDC_ACM_SerialInitCfg.BufAlignOctets,
                                                           DEF_NULL,
                                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_ctrl->LineStateBufPtr,
            USBD_ACM_SERIAL_STATE_BUF_SIZE);
  }
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialAdd()
 *
 * @brief    Adds a new instance of the CDC ACM serial emulation subclass.
 *
 * @param    line_state_interval     Line state notification interval in milliseconds (value must
 *                                   be a power of 2).
 *
 * @param    call_mgmt_capabilities  Call Management Capabilities bitmap. OR'ed of the following
 *                                   flags:
 *                                       -  IAL_CALL_MGMT_DEV           Device handles call management itself.
 *                                       -  IAL_CALL_MGMT_DATA_CCI_DCI  Device can send/receive call management
 *                                   information over a Data Class interface.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these returned error codes from
 *                                   this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ALLOC
 *                                       - RTOS_ERR_SUBCLASS_INSTANCE_ALLOC
 *                                       - RTOS_ERR_USB_CLASS_INSTANCE_ALLOC
 *
 * @return   CDC ACM serial emulation subclass instance number.
 *
 * @internal
 * @note     (1) [INTERNAL] See Note #2 of the function USBD_ACM_SerialFnctDesc() for more details
 *               about the Call Management capabilities.
 *
 * @note     (2) [INTERNAL] Depending on the operating system (Windows, Linux or Mac OS X), not all the possible
 *               flags combinations are supported. Windows and Linux support all combinations, whereas
 *               Mac OS X supports all except the combination (USBD_ACM_SERIAL_CALL_MGMT_DEV).
 * @endinternal
 *******************************************************************************************************/
CPU_INT08U USBD_ACM_SerialAdd(CPU_INT16U line_state_interval,
                              CPU_INT16U call_mgmt_capabilities,
                              RTOS_ERR   *p_err)

{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_INT08U           subclass_nbr;
  CPU_INT08U           class_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_ACM_SERIAL_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_CDC_ACM_SerialPtr->CtrlNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_SUBCLASS_INSTANCE_ALLOC);
    return (USBD_ACM_SERIAL_NBR_NONE);
  }

  USBD_CDC_ACM_SerialPtr->CtrlNbrNext--;
  subclass_nbr = USBD_CDC_ACM_SerialPtr->CtrlNbrNext;           // Alloc new CDC ACM serial emulation subclass.
  CORE_EXIT_ATOMIC();
  //                                                               Init control struct.
  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  //                                                               Create new CDC device.
  class_nbr = USBD_CDC_Add(USBD_CDC_SUBCLASS_ACM,
                           &USBD_ACM_SerialDrv,
                           (void *)p_ctrl,
                           USBD_CDC_COMM_PROTOCOL_AT_V250,
                           DEF_ENABLED,
                           line_state_interval,
                           p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_ACM_SERIAL_NBR_NONE);
  }
  //                                                               Add data IF class to CDC device.
  (void)USBD_CDC_DataIF_Add(class_nbr,
                            DEF_DISABLED,
                            USBD_CDC_DATA_PROTOCOL_NONE,
                            p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_ACM_SERIAL_NBR_NONE);
  }

  p_ctrl->LineStateInterval = line_state_interval;
  p_ctrl->CallMgmtCapabilities = call_mgmt_capabilities;        // See Note #2.
  p_ctrl->Nbr = class_nbr;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (subclass_nbr);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialConfigAdd()
 *
 * @brief    Adds a CDC ACM subclass class instance into USB device configuration.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add new test class interface to.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_USB_IF_ALT_ALLOC
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_USB_IF_GRP_ALLOC
 *                               - RTOS_ERR_USB_CLASS_INSTANCE_ALLOC
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_USB_EP_NONE_AVAIL
 *
 * @return   DEF_YES, if the CDC ACM serial subclass instance was added to USB device configuration
 *                    successfully.
 *           DEF_NO,  if the instance was not added.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_ACM_SerialConfigAdd(CPU_INT08U subclass_nbr,
                                     CPU_INT08U dev_nbr,
                                     CPU_INT08U config_nbr,
                                     RTOS_ERR   *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  (void)USBD_CDC_ConfigAdd(p_ctrl->Nbr,
                           dev_nbr,
                           config_nbr,
                           p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialIsConn()
 *
 * @brief    Gets the CDC ACM serial emulation subclass connection state.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @return   DEF_YES, CDC ACM serial emulation is connected.
 *           DEF_NO,  CDC ACM serial emulation is not connected.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_ACM_SerialIsConn(CPU_INT08U subclass_nbr)

{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_BOOLEAN          conn;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  conn = USBD_CDC_IsConn(p_ctrl->Nbr);

  return (conn);
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialRx()
 *
 * @brief    Receives data on the CDC ACM serial emulation subclass.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    timeout         Timeout, in milliseconds.
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
 *           0,                         if any errors are returned.
 *******************************************************************************************************/
CPU_INT32U USBD_ACM_SerialRx(CPU_INT08U subclass_nbr,
                             CPU_INT08U *p_buf,
                             CPU_INT32U buf_len,
                             CPU_INT16U timeout,
                             RTOS_ERR   *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_BOOLEAN          conn;
  CPU_INT32U           xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  conn = USBD_CDC_IsConn(p_ctrl->Nbr);

  if ((conn == DEF_NO)
      || (p_ctrl->Idle == DEF_TRUE)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  xfer_len = USBD_CDC_DataRx(p_ctrl->Nbr,
                             0u,
                             p_buf,
                             buf_len,
                             timeout,
                             p_err);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialTx()
 *
 * @brief    Sends data on the CDC ACM serial emulation subclass.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
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
 *           0,                            otherwise.
 *******************************************************************************************************/
CPU_INT32U USBD_ACM_SerialTx(CPU_INT08U subclass_nbr,
                             CPU_INT08U *p_buf,
                             CPU_INT32U buf_len,
                             CPU_INT16U timeout,
                             RTOS_ERR   *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_BOOLEAN          conn;
  CPU_INT32U           xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  conn = USBD_CDC_IsConn(p_ctrl->Nbr);

  if ((conn == DEF_NO)
      || (p_ctrl->Idle == DEF_TRUE)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return (0u);
  }

  xfer_len = USBD_CDC_DataTx(p_ctrl->Nbr,
                             0u,
                             p_buf,
                             buf_len,
                             timeout,
                             p_err);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineCtrlGet()
 *
 * @brief    Returns the state of control lines.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *
 * @return   Bit-field with the state of the control line.
 *******************************************************************************************************/
CPU_INT08U USBD_ACM_SerialLineCtrlGet(CPU_INT08U subclass_nbr,
                                      RTOS_ERR   *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_INT08U           line_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  line_ctrl = p_ctrl->LineCtrl;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (line_ctrl);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineCtrlReg()
 *
 * @brief    Sets the line control change notification callback.
 *
 * @param    subclass_nbr        CDC ACM serial emulation subclass instance number.
 *
 * @param    line_ctrl_chngd     Line control change notification callback (see Note #1).
 *
 * @param    p_arg               Pointer to the callback argument.
 *
 * @param    p_err               Pointer to the variable that will receive one of these returned error codes from this function :
 *                                   - RTOS_ERR_NONE
 *
 * @return   ACM serial emulation subclass device number.
 *
 * @note     (1) The callback specified by 'line_ctrl_chngd' argument notifies of changes in the
 *               control signals to the application.
 *               @n
 *               The line control notification function uses the following prototype:
 *                   @verbatim
 *                   void  AppLineCtrlChngd  (CPU_INT08U   subclass_nbr,
 *                                           CPU_INT08U   events,
 *                                           CPU_INT08U   events_chngd,
 *                                           void        *p_arg);
 *                   @endverbatim
 *******************************************************************************************************/
void USBD_ACM_SerialLineCtrlReg(CPU_INT08U                      subclass_nbr,
                                USBD_ACM_SERIAL_LINE_CTRL_CHNGD line_ctrl_chngd,
                                void                            *p_arg,
                                RTOS_ERR                        *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_ctrl->LineCtrlChngdFnct = line_ctrl_chngd;
  p_ctrl->LineCtrlChngdArgPtr = p_arg;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineCodingGet()
 *
 * @brief    Gets the current state of the line coding.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_line_coding   Pointer to the structure where the current line coding will be stored.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBD_ACM_SerialLineCodingGet(CPU_INT08U                  subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                  RTOS_ERR                    *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_line_coding != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_line_coding->BaudRate = p_ctrl->LineCoding.BaudRate;
  p_line_coding->DataBits = p_ctrl->LineCoding.DataBits;
  p_line_coding->StopBits = p_ctrl->LineCoding.StopBits;
  p_line_coding->Parity = p_ctrl->LineCoding.Parity;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineCodingSet()
 *
 * @brief    Sets a new line coding.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_line_coding   Pointer to the structure that contains the new line coding.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void USBD_ACM_SerialLineCodingSet(CPU_INT08U                  subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                  RTOS_ERR                    *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_line_coding != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  if ((p_line_coding->DataBits != 5u)
      && (p_line_coding->DataBits != 6u)
      && (p_line_coding->DataBits != 7u)
      && (p_line_coding->DataBits != 8u)
      && (p_line_coding->DataBits != 16u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  if ((p_line_coding->StopBits != USBD_ACM_SERIAL_STOP_BIT_1)
      && (p_line_coding->StopBits != USBD_ACM_SERIAL_STOP_BIT_1_5)
      && (p_line_coding->StopBits != USBD_ACM_SERIAL_STOP_BIT_2)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  if ((p_line_coding->Parity != USBD_ACM_SERIAL_PARITY_NONE)
      && (p_line_coding->Parity != USBD_ACM_SERIAL_PARITY_ODD)
      && (p_line_coding->Parity != USBD_ACM_SERIAL_PARITY_EVEN)
      && (p_line_coding->Parity != USBD_ACM_SERIAL_PARITY_MARK)
      && (p_line_coding->Parity != USBD_ACM_SERIAL_PARITY_SPACE)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_ctrl->LineCoding.BaudRate = p_line_coding->BaudRate;
  p_ctrl->LineCoding.DataBits = p_line_coding->DataBits;
  p_ctrl->LineCoding.StopBits = p_line_coding->StopBits;
  p_ctrl->LineCoding.Parity = p_line_coding->Parity;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineCodingReg()
 *
 * @brief    Sets the line coding change notification callback.
 *
 * @param    subclass_nbr        CDC ACM serial emulation subclass instance number.
 *
 * @param    line_coding_chngd   Line coding change notification callback (see Note #1).
 *
 * @param    p_arg               Pointer to the callback argument.
 *
 * @param    p_err               Pointer to the variable that will receive one of these returned error codes from this function :
 *                                   - RTOS_ERR_NONE
 *
 * @note     (1) This callback notifies of changes in the line coding to the application.
 *               @n
 *               The line coding change notification function has the following prototype:
 *                   @verbatim
 *                   CPU_BOOLEAN  AppLineCodingChngd (CPU_INT08U                    subclass_nbr,
 *                                                   USBD_ACM_SERIAL_LINE_CODING  *p_line_coding,
 *                                                   void                         *p_arg);
 *                   @endverbatim
 *******************************************************************************************************/
void USBD_ACM_SerialLineCodingReg(CPU_INT08U                        subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING_CHNGD line_coding_chngd,
                                  void                              *p_arg,
                                  RTOS_ERR                          *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_ctrl->LineCodingChngdFnct = line_coding_chngd;
  p_ctrl->LineCodingChngdArgPtr = p_arg;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineStateSet()
 *
 * @brief    Sets a line state event(s).
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    events          Line state event(s) to set.  OR'ed of the following flags:
 *                               - USBD_ACM_SERIAL_STATE_DCD
 *                               - USBD_ACM_SERIAL_STATE_DSR
 *                               - USBD_ACM_SERIAL_STATE_BREAK
 *                               - USBD_ACM_SERIAL_STATE_RING
 *                               - USBD_ACM_SERIAL_STATE_FRAMING
 *                               - USBD_ACM_SERIAL_STATE_PARITY
 *                               - USBD_ACM_SERIAL_STATE_OVERUN
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
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
 *******************************************************************************************************/
void USBD_ACM_SerialLineStateSet(CPU_INT08U subclass_nbr,
                                 CPU_INT08U events,
                                 RTOS_ERR   *p_err)

{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_INT08U           line_state_chngd;
  CPU_BOOLEAN          conn;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  conn = USBD_CDC_IsConn(p_ctrl->Nbr);

  if (conn == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  events &= (USBD_ACM_SERIAL_EVENTS_CONS | USBD_ACM_SERIAL_EVENTS_IRRE);

  CORE_ENTER_ATOMIC();
  line_state_chngd = events ^ p_ctrl->LineState;

  if (line_state_chngd != DEF_BIT_NONE) {
    DEF_BIT_SET(p_ctrl->LineState, events & USBD_ACM_SERIAL_EVENTS_CONS);

    if (p_ctrl->LineStateSent == DEF_NO) {
      p_ctrl->LineStateSent = DEF_YES;
      p_ctrl->LineStateBufPtr[USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->LineState
                                                              | (events & USBD_ACM_SERIAL_EVENTS_IRRE);
      CORE_EXIT_ATOMIC();

      (void)USBD_CDC_Notify(p_ctrl->Nbr,
                            USBD_ACM_SERIAL_REQ_STATE,
                            0u,
                            &p_ctrl->LineStateBufPtr[0],
                            USBD_ACM_SERIAL_REQ_STATE_SIZE,
                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      DEF_BIT_SET(p_ctrl->LineState, events & USBD_ACM_SERIAL_EVENTS_IRRE);
      CORE_EXIT_ATOMIC();
    }
  } else {
    CORE_EXIT_ATOMIC();
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialLineStateClr()
 *
 * @brief          Clears a line state event(s).
 *
 * @param          subclass_nbr  CDC ACM serial emulation subclass instance number.
 *
 * @param          events        Line state event(s) set to be cleared. OR'ed of the following
 *                               flags (see Note #1) :
 *                                   - USBD_ACM_SERIAL_STATE_DCD    Set DCD signal (Rx carrier).
 *                                   - USBD_ACM_SERIAL_STATE_DSR    Set DSR signal (Tx carrier).
 *
 * @param          p_err         Pointer to the variable that will receive return error code from
 *                               this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_USB_INVALID_DEV_STATE
 *                                   - RTOS_ERR_NULL_PTR
 *                                   - RTOS_ERR_USB_EP_QUEUING
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_CLASS_STATE
 *                                   - RTOS_ERR_USB_INVALID_EP_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *
 * @return   None.
 *
 * @note     (1) USB PSTN spec ver 1.20 states: "For the irregular signals like break, the
 *                       incoming ring signal, or the overrun error state, this will reset their values
 *                       to zero and again will not send another notification until their state changes."
 *                       The irregular events are automatically cleared by the ACM serial emulation subclass.
 *******************************************************************************************************/
void USBD_ACM_SerialLineStateClr(CPU_INT08U subclass_nbr,
                                 CPU_INT08U events,
                                 RTOS_ERR   *p_err)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_INT08U           line_state_clr;
  CPU_BOOLEAN          conn;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((subclass_nbr < USBD_CDC_ACM_SerialPtr->SublassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_ACM_SerialPtr->CtrlTbl[subclass_nbr];
  conn = USBD_CDC_IsConn(p_ctrl->Nbr);

  if (conn == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  events &= USBD_ACM_SERIAL_EVENTS_CONS;
  CORE_ENTER_ATOMIC();
  line_state_clr = events & p_ctrl->LineState;

  if (line_state_clr != DEF_BIT_NONE) {
    DEF_BIT_CLR(p_ctrl->LineState, (CPU_INT32U)events);

    if (p_ctrl->LineStateSent == DEF_NO) {
      p_ctrl->LineStateSent = DEF_YES;
      p_ctrl->LineStateBufPtr[USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->LineState;
      CORE_EXIT_ATOMIC();

      (void)USBD_CDC_Notify(p_ctrl->Nbr,
                            USBD_ACM_SERIAL_REQ_STATE,
                            0u,
                            p_ctrl->LineStateBufPtr,
                            USBD_ACM_SERIAL_REQ_STATE_SIZE,
                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      CORE_EXIT_ATOMIC();
    }
  } else {
    CORE_EXIT_ATOMIC();
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialMgmtReq()
 *
 * @brief    CDC ACM serial emulation class management request.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *
 * @return   DEF_OK,   if NO error(s) occurred and request is supported.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) CDC ACM defines the following requests :
 *               - (a) SET_COMM_FEATURE        This request controls the settings for a particular communications
 *                                             feature of a particular target.
 *               - (b) GET_COMM_FEATURE        This request returns the current settings for the communications
 *                                             feature as selected
 *               - (c) CLEAR_COMM_FEATURE      This request controls the settings for a particular communications
 *                                             feature of a particular target, setting the selected feature
 *                                             to its default state.
 *               - (d) SET_LINE_CODING         This request allows the host to specify typical asynchronous
 *                                             line-character formatting properties, which may be required by
 *                                             some applications.
 *               - (e) GET_LINE_CODING         This request allows the host to find out the currently configured
 *                                             line coding.
 *               - (f) SET_CONTROL_LINE_STATE  This request generates RS-232/V.24 style control signals.
 *               - (g) SEND_BREAK              This request sends special carrier modulation that generates an
 *                                             RS-232 style break.
 *               See 'Universal Serial Bus Communications Class Subclass Specification for PSTN Devices
 *               02/09/2007, Version 1.2', section 6.2.2 for more details about ACM requests.
 *
 * @note     (2) 'SEND_BREAK' with variable length is not implemented in most USB host stacks.
 *               This feature may be implemented in the future.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_ACM_SerialMgmtReq(CPU_INT08U           dev_nbr,
                                          const USBD_SETUP_REQ *p_setup_req,
                                          void                 *p_subclass_arg)
{
  USBD_ACM_SERIAL_CTRL        *p_ctrl;
  CPU_INT08U                  request_code;
  USBD_ACM_SERIAL_LINE_CODING line_coding;
  CPU_INT08U                  event_chngd;
  CPU_INT08U                  event_state;
  CPU_BOOLEAN                 valid;
  RTOS_ERR                    err;
  CORE_DECLARE_IRQ_STATE;

  p_ctrl = (USBD_ACM_SERIAL_CTRL *)p_subclass_arg;
  request_code = p_setup_req->bRequest;
  valid = DEF_FAIL;

  switch (request_code) {
    case USBD_CDC_REQ_SET_COMM_FEATURE:                         // ---------- SET_COMM_FEATURE (see Note #1a) ---------
                                                                // Only 'ABSTRACT_STATE' feature is supported.
      if (p_setup_req->wValue == USBD_ACM_SERIAL_ABSTRACT_STATE) {
        (void)USBD_CtrlRx(dev_nbr,
                          (void *)p_ctrl->ReqBufPtr,
                          2u,
                          USBD_ACM_CTRL_REQ_TIMEOUT_mS,
                          &err);
        //                                                         Multiplexing call management command on data is ...
        //                                                         ... not supported
        if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
            && (DEF_BIT_IS_CLR(p_ctrl->ReqBufPtr[0], USBD_ACM_SERIAL_ABSTRACT_DATA_MUX))) {
          CORE_ENTER_ATOMIC();
          p_ctrl->Idle = DEF_BIT_IS_SET(p_ctrl->ReqBufPtr[0], USBD_ACM_SERIAL_ABSTRACT_IDLE);
          CORE_EXIT_ATOMIC();
          valid = DEF_OK;
        }
      }
      break;

    case USBD_CDC_REQ_GET_COMM_FEATURE:                         // ---------- GET_COMM_FEATURE (see Note #1b) ---------
      if (p_setup_req->wValue == USBD_ACM_SERIAL_ABSTRACT_STATE) {
        p_ctrl->ReqBufPtr[0] = (p_ctrl->Idle == DEF_NO) ? USBD_ACM_SERIAL_ABSTRACT_IDLE : DEF_BIT_NONE;
        p_ctrl->ReqBufPtr[1] = DEF_BIT_NONE;

        (void)USBD_CtrlTx(dev_nbr,
                          (void *)&p_ctrl->ReqBufPtr[0],
                          2u,
                          USBD_ACM_CTRL_REQ_TIMEOUT_mS,
                          DEF_NO,
                          &err);
        if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
          valid = DEF_OK;
        }
      }
      break;

    case USBD_CDC_REQ_CLR_COMM_FEATURE:                         // --------- CLEAR_COMM_FEATURE (see Note #1c) --------
      if (p_setup_req->wValue == USBD_ACM_SERIAL_ABSTRACT_STATE) {
        CORE_ENTER_ATOMIC();
        p_ctrl->Idle = DEF_NO;
        CORE_EXIT_ATOMIC();
        valid = DEF_OK;
      }
      break;

    case USBD_CDC_REQ_SET_LINE_CODING:                          // ---------- SET_LINE_CODING (see Note #1d) ----------
      (void)USBD_CtrlRx(dev_nbr,
                        (void *)&p_ctrl->ReqBufPtr[0],
                        7u,
                        USBD_ACM_CTRL_REQ_TIMEOUT_mS,
                        &err);

      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        line_coding.BaudRate = MEM_VAL_GET_INT32U_LITTLE(&p_ctrl->ReqBufPtr[0]);
        line_coding.StopBits = p_ctrl->ReqBufPtr[4];
        line_coding.Parity = p_ctrl->ReqBufPtr[5];
        line_coding.DataBits = p_ctrl->ReqBufPtr[6];

        valid = DEF_OK;
        if (p_ctrl->LineCodingChngdFnct != DEF_NULL) {
          valid = p_ctrl->LineCodingChngdFnct(p_ctrl->Nbr,
                                              &line_coding,
                                              p_ctrl->LineCtrlChngdArgPtr);
        }

        if (valid == DEF_OK) {
          CORE_ENTER_ATOMIC();
          p_ctrl->LineCoding.BaudRate = line_coding.BaudRate;
          p_ctrl->LineCoding.DataBits = line_coding.DataBits;
          p_ctrl->LineCoding.StopBits = line_coding.StopBits;
          p_ctrl->LineCoding.Parity = line_coding.Parity;
          CORE_EXIT_ATOMIC();
        }
      }
      break;

    case USBD_CDC_REQ_GET_LINE_CODING:                          // ---------- GET_LINE_CODING (see Note #1e) ----------
      MEM_VAL_SET_INT32U_LITTLE(&p_ctrl->ReqBufPtr[0], p_ctrl->LineCoding.BaudRate);
      p_ctrl->ReqBufPtr[4] = p_ctrl->LineCoding.StopBits;
      p_ctrl->ReqBufPtr[5] = p_ctrl->LineCoding.Parity;
      p_ctrl->ReqBufPtr[6] = p_ctrl->LineCoding.DataBits;
      (void)USBD_CtrlTx(dev_nbr,
                        (void *)&p_ctrl->ReqBufPtr[0],
                        7u,
                        USBD_ACM_CTRL_REQ_TIMEOUT_mS,
                        DEF_NO,
                        &err);

      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    case USBD_CDC_REQ_SET_CTRL_LINE_STATE:                      // ------ SET_CONTROL_LINE_STATE (see Note #1f) -------
      event_state = (DEF_BIT_IS_SET(p_setup_req->wValue, USBD_ACM_SERIAL_REQ_RTS) == DEF_YES) ? USBD_ACM_SERIAL_CTRL_RTS
                    : DEF_BIT_NONE;
      event_state |= (DEF_BIT_IS_SET(p_setup_req->wValue, USBD_ACM_SERIAL_REQ_DTR) == DEF_YES) ? USBD_ACM_SERIAL_CTRL_DTR
                     : DEF_BIT_NONE;
      event_chngd = p_ctrl->LineCtrl
                    ^  event_state;

      if ((DEF_BIT_IS_CLR(p_ctrl->LineCtrl, USBD_ACM_SERIAL_CTRL_DTR) == DEF_YES)
          && (DEF_BIT_IS_SET(p_setup_req->wValue, USBD_ACM_SERIAL_REQ_DTR) == DEF_YES)) {
        CORE_ENTER_ATOMIC();

        if (p_ctrl->LineStateSent == DEF_NO) {
          p_ctrl->LineStateSent = DEF_YES;
          p_ctrl->LineStateBufPtr[USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->LineState;
          CORE_EXIT_ATOMIC();

          (void)USBD_CDC_Notify(p_ctrl->Nbr,
                                USBD_ACM_SERIAL_REQ_STATE,
                                0u,
                                &p_ctrl->LineStateBufPtr[0],
                                USBD_ACM_SERIAL_REQ_STATE_SIZE,
                                &err);
        } else {
          CORE_EXIT_ATOMIC();
        }
      }

      if (event_chngd != DEF_BIT_NONE) {
        CORE_ENTER_ATOMIC();
        DEF_BIT_SET(p_ctrl->LineCtrl, (event_state & event_chngd));
        DEF_BIT_CLR(p_ctrl->LineCtrl, (~(CPU_INT32U)event_state & event_chngd));
        CORE_EXIT_ATOMIC();

        if (p_ctrl->LineCtrlChngdFnct != (USBD_ACM_SERIAL_LINE_CTRL_CHNGD)0) {
          p_ctrl->LineCtrlChngdFnct(p_ctrl->Nbr,
                                    event_state,
                                    event_chngd,
                                    p_ctrl->LineCtrlChngdArgPtr);
        }
      }
      valid = DEF_OK;
      break;

    case USBD_CDC_REQ_SEND_BREAK:                               // ------------- SEND_BREAK (see Note #1g) ------------
      if ((p_setup_req->wValue == 0x0000u)
          && (DEF_BIT_IS_SET(p_ctrl->LineCtrl, USBD_ACM_SERIAL_CTRL_BREAK))) {
        CORE_ENTER_ATOMIC();
        DEF_BIT_CLR(p_ctrl->LineCtrl, (CPU_INT08U)USBD_ACM_SERIAL_CTRL_BREAK);
        CORE_EXIT_ATOMIC();

        if (p_ctrl->LineCtrlChngdFnct != DEF_NULL) {
          p_ctrl->LineCtrlChngdFnct(p_ctrl->Nbr,
                                    DEF_BIT_NONE,
                                    USBD_ACM_SERIAL_CTRL_BREAK,
                                    p_ctrl->LineCtrlChngdArgPtr);
        }
      } else if ((p_setup_req->wValue != 0x0000u)
                 && (DEF_BIT_IS_CLR(p_ctrl->LineCtrl, USBD_ACM_SERIAL_CTRL_BREAK))) {
#if 0                                                           // Feature not implemented (see Note #2).
        if (p_setup_req->wValue != 0xFFFFu) {
          valid = USBD_ACM_SerialOS_BreakTimeOutSet(p_steup_req->wValue);
          if (valid == DEF_FAIL) {
            break;
          }
        }
#endif

        CORE_ENTER_ATOMIC();
        DEF_BIT_SET(p_ctrl->LineCtrl, USBD_ACM_SERIAL_CTRL_BREAK);
        CORE_EXIT_ATOMIC();

        if (p_ctrl->LineCtrlChngdFnct != DEF_NULL) {
          p_ctrl->LineCtrlChngdFnct(p_ctrl->Nbr,
                                    USBD_ACM_SERIAL_CTRL_BREAK,
                                    USBD_ACM_SERIAL_CTRL_BREAK,
                                    p_ctrl->LineCtrlChngdArgPtr);
        }
      }
      valid = DEF_OK;
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialNotifyCmpl()
 *
 * @brief    ACM subclass notification complete callback.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_subclass_arg  Pointer to ACM subclass notification complete callback argument.
 *******************************************************************************************************/
static void USBD_ACM_SerialNotifyCmpl(CPU_INT08U dev_nbr,
                                      void       *p_subclass_arg)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl;
  CPU_INT08U           line_state_prev;
  CPU_INT08U           event_chngd;
  RTOS_ERR             err;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  p_ctrl = (USBD_ACM_SERIAL_CTRL *)p_subclass_arg;

  CORE_ENTER_ATOMIC();
  //                                                               Get prev state.
  line_state_prev = p_ctrl->LineStateBufPtr[USBD_CDC_NOTIFICATION_HEADER];

  if (DEF_BIT_IS_SET_ANY(line_state_prev, USBD_ACM_SERIAL_EVENTS_IRRE)) {
    //                                                             Clr irregular events.
    DEF_BIT_CLR(line_state_prev, line_state_prev & USBD_ACM_SERIAL_EVENTS_IRRE);
  }

  event_chngd = line_state_prev ^ p_ctrl->LineState;

  if (event_chngd != DEF_BIT_NONE) {
    p_ctrl->LineStateBufPtr[USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->LineState;
    DEF_BIT_CLR(p_ctrl->LineState, p_ctrl->LineState & USBD_ACM_SERIAL_EVENTS_IRRE);
    CORE_EXIT_ATOMIC();

    (void)USBD_CDC_Notify(p_ctrl->Nbr,
                          USBD_ACM_SERIAL_REQ_STATE,
                          0u,
                          p_ctrl->LineStateBufPtr,
                          USBD_ACM_SERIAL_REQ_STATE_SIZE,
                          &err);
  } else {
    p_ctrl->LineStateSent = DEF_NO;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                           USBD_ACM_SerialFnctDesc()
 *
 * @brief    CDC Subclass interface descriptor callback.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    p_subclass_arg      Pointer to subclass argument.
 *
 * @param    first_dci_if_nbr    Interface number of the first Data Class Interface following a
 *                               Communication Class Interface (see Note #1).
 *
 * @note     (1) The Call Management Functional Descriptor contains the field 'bDataInterface' which
 *               represents the interface number of Data Class interface (DCI) optionally used for call
 *               management. In case the Communication Class Interface is followed by several DCIs,
 *               the interface number set for 'bDataInterface' will be the first DCI. It is NOT
 *               possible to use another DCI for handling the call management.
 *
 * @internal
 * @note     (2) [INTERNAL] Table 4 of the PSTN specification describes the abstract control management
 *               functional descriptor. The 'bmCapabilities' (offset 3) field describes the requests
 *               supported by the device.
 *               @verbatim
 *                   BIT       STATE     DESCRIPTION
 *                   -------   -------    -------------
 *                   D0       '1'       Device supports the request combination of SET_COMM_FEATURE,
 *                                       CLR_COMM_FEATURE, and GET_COMM_FEATURE.
 *
 *                   D1       '1'       Device supports the request combination of SET_LINE_CODING,
 *                                       SET_CTRL_LINE_STATE, GET_LINE_CODING and the notification state.
 *
 *                   D2       '1'       Device supports the request SEND_BREAK.
 *
 *                   D3       '1'       Device supports the notification NETWORK_CONNECTION.
 *                                       Not required in ACM serial emulation subclass.
 *               @endverbatim
 *
 * @note     (3) [INTERNAL] Table 3 of the PSTN specification describes the call management functional
 *               descriptor. The 'bmCapabilities' (offset 3) field describes how call management is
 *               handled by the device.
 *               @verbatim
 *                   BIT       STATE     DESCRIPTION
 *                   -------   -------    -------------
 *                   D0       '0'       Device does not handle call management itself.
 *
 *                           '1'       Device handles call management itself.
 *
 *                   D1       '0'       Device send/receives call management information only over the
 *                                       communication class interface.
 *
 *                           '1'       Device can send/receive call management information over a data
 *                                       class interface.
 *               @endverbatim
 * @endinternal
 *******************************************************************************************************/
static void USBD_ACM_SerialFnctDesc(CPU_INT08U dev_nbr,
                                    void       *p_subclass_arg,
                                    CPU_INT08U first_dci_if_nbr)
{
  USBD_ACM_SERIAL_CTRL *p_ctrl = (USBD_ACM_SERIAL_CTRL *)p_subclass_arg;

  //                                                               --------- BUILD ABSTRACT CONTROL MGMT DESC ---------
  USBD_DescWr08(dev_nbr, 4u);                                   // Desc size.
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_TYPE_CS_IF);             // Desc type.
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_SUBTYPE_ACM);            // Desc subtype.
                                                                // Dev request capabilities (see Note #2).
  USBD_DescWr08(dev_nbr, DEF_BIT_00
                | DEF_BIT_01
                | DEF_BIT_02);

  //                                                               --------------- BUILD CALL MGMT DESC ---------------
  USBD_DescWr08(dev_nbr, 5u);                                   // Desc size.
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_TYPE_CS_IF);             // Desc type.
  USBD_DescWr08(dev_nbr, USBD_CDC_DESC_SUBTYPE_CALL_MGMT);      // Desc subtype.
  USBD_DescWr08(dev_nbr, p_ctrl->CallMgmtCapabilities);         // Dev call mgmt capabilities (see Note #3).
                                                                // IF nbr of data class IF optionally used for call ...
                                                                // ... mgmt.
  if (p_ctrl->CallMgmtCapabilities == USBD_ACM_SERIAL_CALL_MGMT_DATA_OVER_DCI) {
    USBD_DescWr08(dev_nbr, first_dci_if_nbr);                   // See Note #1.
  } else {
    USBD_DescWr08(dev_nbr, 0u);
  }
}

/****************************************************************************************************//**
 *                                       USBD_ACM_SerialFnctDescSizeGet()
 *
 * @brief    Retrieve the size of the CDC subclass interface descriptor.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *******************************************************************************************************/
static CPU_INT16U USBD_ACM_SerialFnctDescSizeGet(CPU_INT08U dev_nbr,
                                                 void       *p_subclass_arg)
{
  (void)&dev_nbr;
  (void)&p_subclass_arg;

  return (USBD_ACM_DESC_TOT_SIZE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_ACM_AVAIL))
