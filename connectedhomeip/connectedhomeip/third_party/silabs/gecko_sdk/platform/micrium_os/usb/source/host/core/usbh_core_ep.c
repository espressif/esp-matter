/***************************************************************************//**
 * @file
 * @brief USB Host Endpoint Operations
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

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>
#include  <usb/source/host/core/usbh_core_ep_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_config.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, EP)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_DBG_EP_STATS_EN                           DEF_DISABLED
#define  USBH_DBG_EP_STATE_MACHINE_TRAP_EN              DEF_DISABLED

#if (USBH_DBG_EP_STATS_EN == DEF_ENABLED)
#define  USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, next)        USBH_DbgEP_StatsStateSwitch((p_ep_mgmt), (next))

#define  USBH_DBG_EP_STATS_RESET()                              Mem_Clr((void *)&USBH_DbgEP_Stats, \
                                                                        (CPU_SIZE_T) sizeof(USBH_DBG_EP_STATS));

#define  USBH_DBG_EP_STATS_INC(ep_handle, stat)                 USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].stat++;
#define  USBH_DBG_EP_STATS_INC_IF_TRUE(ep_handle, stat, bool)   if ((bool) == DEF_TRUE) { \
    USBH_DBG_EP_STATS_INC(ep_handle, stat);                                               \
}

#else
#define  USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, next)

#define  USBH_DBG_EP_STATS_RESET()
#define  USBH_DBG_EP_STATS_INC(ep_handle, stat)
#define  USBH_DBG_EP_STATS_INC_IF_TRUE(ep_handle, stat, bool)
#endif

#if (USBH_DBG_EP_STATE_MACHINE_TRAP_EN == DEF_ENABLED)
#define  USBH_DBG_EP_STATE_MACHINE_TRAP() while (1) {; }
#else
#define  USBH_DBG_EP_STATE_MACHINE_TRAP()
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct usbh_ep_xfer_xync_arg {
  KAL_SEM_HANDLE SemHandle;                                     // Handle to sem used to signal end of synx xfer.
  CPU_INT32U     XferLen;                                       // Length of sync xfer.
  RTOS_ERR       Err;                                           // Err related to sync xfer.
} USBH_EP_XFER_SYNC_ARG;

typedef struct usbh_ep_state_switch_arg {
  USBH_EP_STATE StateToSwitch;                                  // State in which to set the endpoint.
  RTOS_ERR      *ErrPtr;                                        // Ptr to err variable to hold returned err.
} USBH_EP_STATE_SWITCH_ARG;

#if (USBH_DBG_EP_STATS_EN == DEF_ENABLED)
typedef CPU_INT32U USBH_DBG_EP_STATS_TYPE;

typedef struct usbh_dbg_ep_state_switch {
  USBH_DBG_EP_STATS_TYPE FromNone;
  USBH_DBG_EP_STATS_TYPE FromReady;
  USBH_DBG_EP_STATS_TYPE FromPreparing;
  USBH_DBG_EP_STATS_TYPE FromQueued;
  USBH_DBG_EP_STATS_TYPE FromCompleting;
  USBH_DBG_EP_STATS_TYPE FromAbortingClosing;
  USBH_DBG_EP_STATS_TYPE FromAbortCloseCmpl;
  USBH_DBG_EP_STATS_TYPE TotalCnt;
} USBH_DBG_EP_STATE_SWITCH;

typedef struct usbh_dbg_ep_state_switch_stats {
  USBH_DBG_EP_STATE_SWITCH ToNone;
  USBH_DBG_EP_STATE_SWITCH ToReady;
  USBH_DBG_EP_STATE_SWITCH ToPreparing;
  USBH_DBG_EP_STATE_SWITCH ToQueued;
  USBH_DBG_EP_STATE_SWITCH ToCompleting;
  USBH_DBG_EP_STATE_SWITCH ToAbortingClosing;
  USBH_DBG_EP_STATE_SWITCH ToAbortCloseCmpl;
} USBH_DBG_EP_STATE_SWITCH_STATS;

typedef struct usbh_dbg_ep_stats {
  USBH_DBG_EP_STATS_TYPE         OpenCnt;

  USBH_DBG_EP_STATS_TYPE         CloseCnt;
  USBH_DBG_EP_STATS_TYPE         CloseSuccessCnt;

  USBH_DBG_EP_STATS_TYPE         URB_SubmitCnt;
  USBH_DBG_EP_STATS_TYPE         URB_SubmitSuccessCnt;

  USBH_DBG_EP_STATS_TYPE         URB_DoneCnt;
  USBH_DBG_EP_STATS_TYPE         URB_DoneSuccessCnt;

  USBH_DBG_EP_STATS_TYPE         URB_AbortCnt;
  USBH_DBG_EP_STATS_TYPE         URB_AbortEventFound;

  USBH_DBG_EP_STATS_TYPE         URB_AbortedFreeCnt;
  USBH_DBG_EP_STATS_TYPE         URB_AbortedFreeCallbackCnt;

  USBH_DBG_EP_STATS_TYPE         URB_CmplCnt;
  USBH_DBG_EP_STATS_TYPE         URB_CmplCntErrDevHandle;
  USBH_DBG_EP_STATS_TYPE         URB_CmplCntErrDevOther;
  USBH_DBG_EP_STATS_TYPE         URB_CmplCntErrEP;
  USBH_DBG_EP_STATS_TYPE         URB_CmplCntErrURB;
  USBH_DBG_EP_STATS_TYPE         URB_CmplSuccessCnt;
  USBH_DBG_EP_STATS_TYPE         URB_CmplCallbackCnt;

  USBH_DBG_EP_STATE_SWITCH_STATS StateSwitch;
} USBH_DBG_EP_STATS;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_DBG_EP_STATS_EN == DEF_ENABLED)
static USBH_DBG_EP_STATS USBH_DbgEP_Stats[32u];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_EP *USBH_EP_PtrGet(USBH_CONFIG      *p_config,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               CPU_INT08U       ep_addr,
                               RTOS_ERR         *p_err);

static CPU_INT32U USBH_EP_XferSync(USBH_DEV_HANDLE dev_handle,
                                   USBH_EP_HANDLE  ep_handle,
                                   CPU_INT08U      ep_type,
                                   CPU_BOOLEAN     dir_in,
                                   CPU_INT08U      *p_buf,
                                   CPU_INT32U      buf_len,
                                   CPU_INT32U      timeout,
                                   RTOS_ERR        *p_err);

static void USBH_EP_XferSyncPend(USBH_DEV_HANDLE       dev_handle,
                                 USBH_EP_HANDLE        ep_handle,
                                 USBH_EP_XFER_SYNC_ARG *p_xfer_sync_arg,
                                 CPU_INT32U            timeout,
                                 RTOS_ERR              *p_err);

static void USBH_EP_XferSyncCmplCallback(USBH_DEV_HANDLE dev_handle,
                                         USBH_EP_HANDLE  ep_handle,
                                         CPU_INT08U      *p_buf,
                                         CPU_INT32U      buf_len,
                                         CPU_INT32U      xfer_len,
                                         void            *p_arg,
                                         RTOS_ERR        err);

static void USBH_EP_XferAsync(USBH_DEV_HANDLE dev_handle,
                              USBH_EP_HANDLE  ep_handle,
                              CPU_INT08U      ep_type,
                              CPU_BOOLEAN     dir_in,
                              CPU_INT08U      *p_buf,
                              CPU_INT32U      buf_len,
                              USBH_ASYNC_FNCT async_fnct,
                              void            *p_async_arg,
                              RTOS_ERR        *p_err);

static USBH_EP_MGMT *USBH_EP_MgmtGet(USBH_DEV       *p_dev,
                                     USBH_EP_HANDLE ep_handle);

static void USBH_EP_MgmtStateSet(USBH_EP_MGMT  *p_ep_mgmt,
                                 USBH_EP_STATE state,
                                 RTOS_ERR      *p_err);

static USBH_EP_MGMT *USBH_EP_MgmtStateWaitSwitch(USBH_DEV       *p_dev,
                                                 USBH_EP_HANDLE ep_handle,
                                                 USBH_EP_STATE  state,
                                                 CPU_INT32U     timeout_ms,
                                                 RTOS_ERR       *p_err);

static void USBH_EP_MgmtStateSwitch(USBH_EP_MGMT *p_ep_mgmt,
                                    USBH_EP_STATE state,
                                    KAL_MON_RES (*p_eval_fnct)(void *p_mon_data, void *p_eval_op_data, void *p_scan_op_data),
                                    CPU_INT32U timeout_ms,
                                    RTOS_ERR *p_err);

static KAL_MON_RES USBH_EP_MgmtStateSwitchOnEnter(void *p_mon_data,
                                                  void *p_op_data);

static KAL_MON_RES USBH_EP_MgmtStateSwitchOnEval(void *p_mon_data,
                                                 void *p_eval_op_data,
                                                 void *p_scan_op_data);

static USBH_EP_MGMT *USBH_EP_MgmtAlloc(void);

static CPU_BOOLEAN USBH_EP_MgmtAllocCallback(MEM_DYN_POOL *p_pool,
                                             MEM_SEG      *p_seg,
                                             void         *p_blk,
                                             void         *p_arg);

static void USBH_EP_MgmtFree(USBH_EP_MGMT *p_ep_mgmt);

static void USBH_EP_URB_Abort(USBH_EP_MGMT *p_ep_mgmt);

static void USBH_URB_Submit(USBH_DEV        *p_dev,
                            USBH_EP_HANDLE  ep_handle,
                            CPU_INT08U      ep_type,
                            CPU_INT08U      *p_buf,
                            CPU_INT32U      buf_len,
                            USBH_TOKEN      token,
                            CPU_INT32U      timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                            CPU_INT16U      frm_start,
                            CPU_INT08U      frm_cnt,
#endif
                            USBH_ASYNC_FNCT async_fnct,
                            void            *p_async_arg,
                            RTOS_ERR        *p_err);

static CPU_BOOLEAN USBH_URB_MgmtAllocCallback(MEM_DYN_POOL *p_pool,
                                              MEM_SEG      *p_seg,
                                              void         *p_blk,
                                              void         *p_arg);

#if (USBH_DBG_EP_STATS_EN == DEF_ENABLED)
static void USBH_DbgEP_StatsStateSwitch(USBH_EP_MGMT  *p_ep_mgmt,
                                        USBH_EP_STATE next);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_EP_AttribGet()
 *
 * @brief    Gets the endpoint attributes.
 *
 * @param    dev_handle      Handle to the device.
 *
 * @param    fnct_handle     Handle to the function.
 *
 * @param    if_ix           Interface index.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Endpoint attributes, if successful.
 *           0,                   otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bmAttributes' field
 *               of the endpoint descriptor. See 'Universal Serial Bus specification, revision
 *               2.0, section 9.6.6' for more information.
 *******************************************************************************************************/
CPU_INT08U USBH_EP_AttribGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             CPU_INT08U       ep_addr,
                             RTOS_ERR         *p_err)
{
  CPU_INT08U ep_attrib = 0u;
  USBH_DEV   *p_dev;
  USBH_EP    *p_ep;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_ep = USBH_EP_PtrGet(&p_dev->ConfigCur,
                        fnct_handle,
                        if_ix,
                        ep_addr,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  ep_attrib = p_ep->Attrib;

end_rel:
  USBH_DevRelShared(p_dev);

  return (ep_attrib);
}

/****************************************************************************************************//**
 *                                           USBH_EP_MaxPktSizeGet()
 *
 * @brief    Gets the endpoint maximum packet size.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    if_ix           Interface index.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Endpoint maximum packet size, if successful.
 *           0,                            otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'wMaxPacketSize' field
 *               of the endpoint descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.6'.
 *******************************************************************************************************/
CPU_INT16U USBH_EP_MaxPktSizeGet(USBH_DEV_HANDLE  dev_handle,
                                 USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       ep_addr,
                                 RTOS_ERR         *p_err)
{
  CPU_INT16U ep_max_pkt_size = 0u;
  USBH_DEV   *p_dev;
  USBH_EP    *p_ep;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (USBH_EP_ADDR_TO_LOG(ep_addr) == 0u) {                     // If ctrl EPs ...
    ep_max_pkt_size = p_dev->CtrlMaxPacketSize;
    goto end_rel;
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_ep = USBH_EP_PtrGet(&p_dev->ConfigCur,
                        fnct_handle,
                        if_ix,
                        ep_addr,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  ep_max_pkt_size = p_ep->MaxPktSize;

end_rel:
  USBH_DevRelShared(p_dev);

  return (ep_max_pkt_size);
}

/****************************************************************************************************//**
 *                                           USBH_EP_IntervalGet()
 *
 * @brief    Gets the endpoint interval.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    if_ix           Interface index.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @return   Endpoint interval, if successful.
 *           0,                 otherwise.
 *
 * @note     (1) The value returned by this function corresponds to the 'bInterval' field
 *               of the endpoint descriptor. For more information, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.6.6' .
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT08U USBH_EP_IntervalGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               CPU_INT08U       ep_addr,
                               RTOS_ERR         *p_err)
{
  CPU_INT08U ep_interval = 0u;
  USBH_DEV   *p_dev;
  USBH_EP    *p_ep;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  p_ep = USBH_EP_PtrGet(&p_dev->ConfigCur,
                        fnct_handle,
                        if_ix,
                        ep_addr,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  ep_interval = p_ep->Interval;

end_rel:
  USBH_DevRelShared(p_dev);

  return (ep_interval);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_EP_CtrlXfer()
 *
 * @brief    Perform the synchronous control transfer on endpoint.
 *
 * @param    dev_handle  Handle to the device.
 *
 * @param    req         bRequest value of the setup packet.
 *
 * @param    req_type    bmRequestType value of the setup packet.
 *
 * @param    val         wValue value of the setup packet.
 *
 * @param    ix          wIndex value of the setup packet.
 *
 * @param    p_buf       Pointer to the data buffer for the data phase.
 *
 * @param    len         wLength value of the setup packet that specifies the number of data bytes in data stage.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *
 * @return   Number of octets transfered in data stage (if any).
 *******************************************************************************************************/
CPU_INT16U USBH_EP_CtrlXfer(USBH_DEV_HANDLE dev_handle,
                            CPU_INT08U      req,
                            CPU_INT08U      req_type,
                            CPU_INT16U      val,
                            CPU_INT16U      ix,
                            CPU_INT08U      *p_buf,
                            CPU_INT16U      len,
                            CPU_INT16U      buf_len,
                            CPU_INT32U      timeout,
                            RTOS_ERR        *p_err)
{
  CPU_INT16U xfer_len;
  USBH_DEV   *p_dev;
  RTOS_ERR   err_disconn;

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  xfer_len = USBH_EP_CtrlXferLocked(p_dev,
                                    req,
                                    req_type,
                                    val,
                                    ix,
                                    p_buf,
                                    len,
                                    buf_len,
                                    timeout,
                                    p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_IO_FATAL) {
    USBH_DevRelShared(p_dev);
  } else {
    CPU_INT08U      port_nbr = p_dev->PortNbr;
    USBH_HUB_FNCT   *p_hub_fnct = p_dev->HubFnctPtr;
    USBH_DEV_HANDLE hub_dev_handle = p_hub_fnct->Handle;

    USBH_DevRelShared(p_dev);

    USBH_HUB_PortDisconnReq(hub_dev_handle,                     // Request to disconnect faulty device.
                            port_nbr,
                            DEF_NULL,
                            DEF_NULL,
                            &err_disconn);
  }

  return (xfer_len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CLASSES AVAILABLE FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_EP_AddrGet()
 *
 * @brief    Gets endpoint address.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Endpoint address, if successful.
 *           0,                otherwise
 *******************************************************************************************************/
CPU_INT08U USBH_EP_AddrGet(USBH_EP_HANDLE ep_handle,
                           RTOS_ERR       *p_err)
{
  CPU_INT08U ep_phy_nbr;
  CPU_INT08U ep_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(ep_handle);
  RTOS_ASSERT_DBG_ERR_SET((ep_phy_nbr < USBH_EP_MAX_NBR), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  ep_addr = USBH_EP_PHY_TO_ADDR(ep_phy_nbr);

  return (ep_addr);
}

/****************************************************************************************************//**
 *                                           USBH_EP_DescExtraGet()
 *
 * @brief    Gets endpoint's extra descriptor(s).
 *
 * @param    fnct_handle         Handle to the function.
 *
 * @param    if_ix               Interface index.
 *
 * @param    ep_addr             Endpoint address.
 *
 * @param    p_desc_extra_len    Pointer to the variable that will receive total extra descriptors
 *                               length in octets.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_INVALID_STATE
 *
 * @return   Pointer to the buffer that contains endpoint's extra descriptor(s).
 *
 * @note     (1) First descriptor returned by this function will always be the endpoint descriptor
 *               itself.
 *******************************************************************************************************/
CPU_INT08U *USBH_EP_DescExtraGet(USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       ep_addr,
                                 CPU_INT16U       *p_desc_extra_len,
                                 RTOS_ERR         *p_err)
{
  CPU_INT08U desc_len;
  CPU_INT08U desc_type;
  CPU_INT08U desc_if_nbr = 0u;
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  CPU_INT08U desc_alt_if_nbr = 0u;
#endif
  CPU_INT08U desc_ep_addr;
  CPU_INT08U *p_desc_buf;
  CPU_INT08U *p_desc_extra_buf = DEF_NULL;
  CPU_INT16U cnt;
  USBH_IF    *p_if;
  USBH_EP    *p_ep;
  CPU_INT16U extra_desc_start_ix = 0u;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET((p_desc_extra_len != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  CORE_ENTER_ATOMIC();
  if (USBH_Ptr->DescDevPtr == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return (DEF_NULL);
  }
  CORE_EXIT_ATOMIC();

  p_if = USBH_IF_PtrGet(&USBH_Ptr->DescDevPtr->ConfigCur,
                        fnct_handle,
                        if_ix,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_ep = USBH_EP_PtrGet(&USBH_Ptr->DescDevPtr->ConfigCur,
                        fnct_handle,
                        if_ix,
                        ep_addr,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  cnt = USBH_DESC_LEN_CONFIG;                                   // Skip config desc.
  p_desc_buf = USBH_Ptr->DescBufPtr;
  *p_desc_extra_len = 0u;

  //                                                               ----------------- RETRIEVE EP DESC -----------------
  while (cnt < USBH_Ptr->DescLen) {
    desc_len = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt]);
    desc_type = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 1u]);

    if (desc_type == USBH_DESC_TYPE_EP) {
      if (extra_desc_start_ix == 0u) {
        desc_ep_addr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 2u]);

        if ((desc_ep_addr == p_ep->Addr)
            && (desc_if_nbr == p_if->Nbr)
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
            && (desc_alt_if_nbr == p_if->AltNbr)
#endif
            ) {
          extra_desc_start_ix = cnt;
        }
      } else {
        break;
      }
    } else if (desc_type == USBH_DESC_TYPE_IF) {
      if (extra_desc_start_ix != 0u) {
        break;
      }
      desc_if_nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 2u]);
            #if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
      desc_alt_if_nbr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[cnt + 3u]);
            #endif
    } else if ((desc_type == USBH_DESC_TYPE_IF_ASSOCIATION)
               && (extra_desc_start_ix != 0u)) {
      break;
    }

    if (extra_desc_start_ix != 0u) {
      *p_desc_extra_len += desc_len;
    }

    cnt += desc_len;
  }

  p_desc_extra_buf = &p_desc_buf[extra_desc_start_ix];

  return (p_desc_extra_buf);
}

/****************************************************************************************************//**
 *                                           USBH_EP_IF_IxGet()
 *
 * @brief    Gets endpoint interface index.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Endpoint interface index, if successful.
 *           0,                        otherwise.
 *******************************************************************************************************/
CPU_INT08U USBH_EP_IF_IxGet(USBH_EP_HANDLE ep_handle,
                            RTOS_ERR       *p_err)
{
  CPU_INT08U if_ix;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  if_ix = USBH_HANDLE_IF_IX_GET(ep_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (if_ix);
}

/****************************************************************************************************//**
 *                                           USBH_EP_StallSet()
 *
 * @brief    Stalls or clears stall on endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    stall_set   Flags that indicate if stall condition should be set or cleared.
 *                       DEF_SET     Set   stall condition on endpoint.
 *                       DEF_CLR     Clear stall condition on endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_EP_StallSet(USBH_DEV_HANDLE dev_handle,
                      USBH_EP_HANDLE  ep_handle,
                      CPU_BOOLEAN     stall_set,
                      RTOS_ERR        *p_err)
{
  CPU_INT08U ep_phy_nbr = USBH_HANDLE_EP_PHY_NBR_GET(ep_handle);
  CPU_INT08U ep_addr;
  CPU_INT08U req;
  USBH_DEV   *p_dev;
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((stall_set == DEF_SET)
                           || (stall_set == DEF_CLR)), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET(((ep_phy_nbr >= 2u)
                           && (ep_phy_nbr < USBH_EP_MAX_NBR)), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  ep_addr = USBH_EP_PHY_TO_ADDR(ep_phy_nbr);

  if (stall_set == DEF_CLR) {
    USBH_HC_DRV   *p_hc_drv = &p_dev->HC_Ptr->Drv;
    USBH_EP_MGMT  *p_ep_mgmt;
    RTOS_ERR      local_err;
    USBH_EP_STATE state;

    p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                            ep_handle,
                                            USBH_EP_STATE_PREPARING,
                                            KAL_TIMEOUT_INFINITE,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }

    p_hc_drv->API_Ptr->EP_HaltClr(p_hc_drv,                     // Clear any halt condition in HCD.
                                  p_ep_mgmt->HCD_EP_DataPtr,
                                  DEF_YES,
                                  &local_err);

    state = (p_ep_mgmt->URB_Cnt == 0u) ? USBH_EP_STATE_READY : USBH_EP_STATE_QUEUED;

    USBH_EP_MgmtStateSet(p_ep_mgmt,
                         state,
                         p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
        && (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE)) {
      RTOS_ERR_COPY(*p_err, local_err);
    }
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }
  }

  req = (stall_set == DEF_SET) ? USBH_DEV_REQ_SET_FEATURE : USBH_DEV_REQ_CLR_FEATURE;

  (void)USBH_EP_CtrlXferLocked(p_dev,
                               req,
                               (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_RECIPIENT_EP),
                               USBH_FEATURE_SEL_EP_HALT,
                               ep_addr,
                               DEF_NULL,
                               0u,
                               0u,
                               std_req_timeout_ms,
                               p_err);

end_rel:
  USBH_DevRelShared(p_dev);
}

/****************************************************************************************************//**
 *                                               USBH_EP_IntrRx()
 *
 * @brief    Receive data on Interrupt IN endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Number of octets received.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT32U USBH_EP_IntrRx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err)
{
  CPU_INT32U xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  xfer_len = USBH_EP_XferSync(dev_handle,
                              ep_handle,
                              USBH_EP_TYPE_INTR,
                              DEF_YES,
                              p_buf,
                              buf_len,
                              timeout,
                              p_err);

  return (xfer_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_EP_IntrRxAsync()
 *
 * @brief    Receive data on Interrupt IN endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
void USBH_EP_IntrRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_INTR,
                    DEF_YES,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_EP_IntrTx()
 *
 * @brief    Send data on Interrupt OUT endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Number of octets transmitted.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT32U USBH_EP_IntrTx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err)
{
  CPU_INT32U xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  xfer_len = USBH_EP_XferSync(dev_handle,
                              ep_handle,
                              USBH_EP_TYPE_INTR,
                              DEF_NO,
                              p_buf,
                              buf_len,
                              timeout,
                              p_err);

  return (xfer_len);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_EP_IntrTxAsync()
 *
 * @brief    Transmit data on Interrupt OUT endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
void USBH_EP_IntrTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_INTR,
                    DEF_NO,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_EP_BulkRx()
 *
 * @brief    Receive data on Bulk IN endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Number of octets received.
 *******************************************************************************************************/
CPU_INT32U USBH_EP_BulkRx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err)
{
  CPU_INT32U xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  xfer_len = USBH_EP_XferSync(dev_handle,
                              ep_handle,
                              USBH_EP_TYPE_BULK,
                              DEF_YES,
                              p_buf,
                              buf_len,
                              timeout,
                              p_err);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_EP_BulkRxAsync()
 *
 * @brief    Receive data on Bulk IN endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void USBH_EP_BulkRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_BULK,
                    DEF_YES,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}

/****************************************************************************************************//**
 *                                               USBH_EP_BulkTx()
 *
 * @brief    Send data on Bulk OUT endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   Number of octets transmitted.
 *******************************************************************************************************/
CPU_INT32U USBH_EP_BulkTx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err)
{
  CPU_INT32U xfer_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  xfer_len = USBH_EP_XferSync(dev_handle,
                              ep_handle,
                              USBH_EP_TYPE_BULK,
                              DEF_NO,
                              p_buf,
                              buf_len,
                              timeout,
                              p_err);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_EP_BulkTxAsync()
 *
 * @brief    Transmit data on Bulk OUT endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void USBH_EP_BulkTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_BULK,
                    DEF_NO,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}

/****************************************************************************************************//**
 *                                           USBH_EP_IsocRxAsync()
 *
 * @brief    Receive data on Isochronous IN endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    frm_start       Frame number of first transfer (for Isochronous endpoint only).
 *
 * @param    frm_cnt         Number of frame of transfer (for Isochronous endpoint only).
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/

#if ((USBH_CFG_ISOC_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED))
void USBH_EP_IsocRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         CPU_INT16U      frm_start,
                         CPU_INT08U      frm_cnt,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  (void)frm_start;
  (void)frm_cnt;

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_ISOC,
                    DEF_YES,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_EP_IsocTxAsync()
 *
 * @brief    Transmit data on Isochronous OUT endpoint asynchronously.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    frm_start       Frame number of first transfer (for Isochronous endpoint only).
 *
 * @param    frm_cnt         Number of frame of transfer (for Isochronous endpoint only).
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_URB_ALLOC
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/

#if ((USBH_CFG_ISOC_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED))
void USBH_EP_IsocTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         CPU_INT16U      frm_start,
                         CPU_INT08U      frm_cnt,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err)
{
  (void)frm_start;
  (void)frm_cnt;

  USBH_EP_XferAsync(dev_handle,
                    ep_handle,
                    USBH_EP_TYPE_ISOC,
                    DEF_NO,
                    p_buf,
                    buf_len,
                    async_fnct,
                    p_async_arg,
                    p_err);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_EP_Abort()
 *
 * @brief    Aborts all transfers and clear halt condition on endpoint.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint. 0 for control endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void USBH_EP_Abort(USBH_DEV_HANDLE dev_handle,
                   USBH_EP_HANDLE  ep_handle,
                   RTOS_ERR        *p_err)
{
  USBH_DEV *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_EP_AbortLocked(p_dev, ep_handle, p_err);

  USBH_DevRelShared(p_dev);
}

/********************************************************************************************************
 *                                           CORE-ONLY FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_EP_Init()
 *
 * @brief    Initializes USB Host endpoint module.
 *
 * @param    ep_qty          Quantity of endpoint.
 *
 * @param    ep_open_qty     Quantity of opened endpoint.
 *
 * @param    urb_qty         Quantity of URB.
 *
 * @param    p_err           Pointer to the variable that will receive the error code from this function.
 *******************************************************************************************************/
void USBH_EP_Init(CPU_INT16U ep_qty,
                  CPU_INT16U ep_open_qty,
                  CPU_INT16U urb_qty,
                  RTOS_ERR   *p_err)
{
  Mem_DynPoolCreate("USBH - EP pool",                           // Create USB EP pool.
                    &USBH_Ptr->EP_Pool,
                    USBH_InitCfg.MemSegPtr,
                    sizeof(USBH_EP),
                    sizeof(CPU_ALIGN),
                    (ep_qty != USBH_OBJ_QTY_UNLIMITED) ? ep_qty : 0u,
                    (ep_qty != USBH_OBJ_QTY_UNLIMITED) ? ep_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreatePersistent("USBH - EP management pool",      // Create USB EP mgmt struct pool.
                              &USBH_Ptr->EP_MgmtPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_EP_MGMT),
                              sizeof(CPU_ALIGN),
                              (ep_open_qty != USBH_OBJ_QTY_UNLIMITED) ? ep_open_qty : 0u,
                              (ep_open_qty != USBH_OBJ_QTY_UNLIMITED) ? ep_open_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_EP_MgmtAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreatePersistent("USBH - URB management pool",     // Create USB URB mgmt struct pool.
                              &USBH_Ptr->URB_MgmtPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_URB_MGMT),
                              sizeof(CPU_ALIGN),
                              (urb_qty != USBH_OBJ_QTY_UNLIMITED) ? urb_qty : 0u,
                              (urb_qty != USBH_OBJ_QTY_UNLIMITED) ? urb_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_URB_MgmtAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_DBG_EP_STATS_RESET();
}

/****************************************************************************************************//**
 *                                               USBH_EP_Open()
 *
 * @brief    Open an endpoint.
 *
 * @param    p_dev   Pointer to the USB device.
 *
 * @param    -----   Argument validated by caller.
 *
 * @param    p_fnct  Pointer to the USB function.
 *
 * @param    p_ep    Pointer to the endpoint.
 *
 * @param    ----    -   Argument validated by caller.
 *
 * @param    if_ix   Interface index.
 *
 * @param    -----   Argument validated by caller.
 *
 * @param    p_err   Pointer to the variable that will receive return error code from this function.
 *
 * @return   Handle to opened endpoint, if no error(s),
 *           USBH_EP_HANDLE_INVALID,    otherwise.
 *
 * @note     (1) The core provides the interval in the unit of measurement of the Host Controller (HC).
 *               If HC is High-Speed, the interval is always in terms of micro frames (125 uS).
 *               Otherwise, the unit will be a frame (1 mS).
 *
 * @note     (2) Interrupt endpoints interval is computed in function of device speed.
 *               - (a) At low-speed,  interrupt endpoints can specify period from 10 ms to 255 ms.
 *               - (b) At full-speed, interrupt endpoints can specify period from  1 ms to 255 ms.
 *               - (c) At High-speed, interrupt endpoints specify period using following formula:
 *                   @verbatim
 *                       2^(bInterval-1) x 125us
 *
 *                   where bInterval is in range [1 .. 16].
 *                   @endverbatim
 * @note     (3) Isochronous endpoints specify period using following formula:
 *               @verbatim
 *                   2^(bInterval-1) x F
 *
 *               where bInterval is in range [1 .. 16] and F is 125us for high-speed and 1ms for
 *               full-speed.
 *               @endverbatim
 * @note     (4) For more information on bInterval computation, see 'Universal Serial Bus
 *               specification, revision 2.0, section 9.6.6'.
 *******************************************************************************************************/
USBH_EP_HANDLE USBH_EP_Open(USBH_DEV   *p_dev,
                            USBH_FNCT  *p_fnct,
                            USBH_EP    *p_ep,
                            CPU_INT08U if_ix,
                            RTOS_ERR   *p_err)
{
  CPU_INT08U         ep_phy_nbr;
  USBH_HC            *p_hc;
  USBH_EP_MGMT       *p_ep_mgmt;
  USBH_HCD_EP_PARAMS ep_params;
  CORE_DECLARE_IRQ_STATE;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_DISABLED)
  if (p_ep != DEF_NULL) {
    if ((USBH_EP_TYPE_GET(p_ep->Attrib) == USBH_EP_TYPE_INTR)
        || (USBH_EP_TYPE_GET(p_ep->Attrib) == USBH_EP_TYPE_ISOC)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return (USBH_EP_HANDLE_INVALID);
    }
  }
#elif (USBH_CFG_ISOC_EN == DEF_DISABLED)
  if (p_ep != DEF_NULL) {
    if (USBH_EP_TYPE_GET(p_ep->Attrib) == USBH_EP_TYPE_ISOC) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return (USBH_EP_HANDLE_INVALID);
    }
  }
#endif

  p_ep_mgmt = USBH_EP_MgmtAlloc();
  if (p_ep_mgmt == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_ALLOC);
    return (USBH_EP_HANDLE_INVALID);
  }

  ep_params.DevHandle = p_dev->Handle;
  ep_params.DevAddr = p_dev->AddrCur;
  ep_params.DevSpd = p_dev->Spd;
  if (p_ep != DEF_NULL) {                                       // Intr, bulk or isoc EP.
    ep_params.Addr = p_ep->Addr;
    ep_params.Type = USBH_EP_TYPE_GET(p_ep->Attrib);
    ep_params.MaxPktSize = p_ep->MaxPktSize;
    ep_phy_nbr = USBH_EP_ADDR_TO_PHY(p_ep->Addr);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    switch (ep_params.Type) {                                   // Interval conversion, see note (1).
      case USBH_EP_TYPE_INTR:                                   // Intr EP, see note (2).
        switch (p_dev->Spd) {
          case USBH_DEV_SPD_HIGH:
            ep_params.Interval = (1u << (p_ep->Interval - 1u));
            break;

          case USBH_DEV_SPD_LOW:
          case USBH_DEV_SPD_FULL:
          default:
            if (p_dev->HC_Ptr->Drv.HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
              ep_params.Interval = (8u * p_ep->Interval);
            } else {
              ep_params.Interval = p_ep->Interval;
            }
            break;
        }
        break;

      case USBH_EP_TYPE_ISOC:
        switch (p_dev->Spd) {                                   // Isoc EP, see note (3).
          case USBH_DEV_SPD_HIGH:
            ep_params.Interval = (1u << (p_ep->Interval - 1u));
            break;

          case USBH_DEV_SPD_FULL:
            ep_params.Interval = (1u << (p_ep->Interval - 1u));
            if (p_dev->HC_Ptr->Drv.HW_InfoPtr->RH_Spd == USBH_DEV_SPD_HIGH) {
              ep_params.Interval = 8u * ep_params.Interval;
            }
            break;

          case USBH_DEV_SPD_LOW:
          default:
            RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
            goto err_end;
        }
        break;

      case USBH_EP_TYPE_BULK:
      default:
        ep_params.Interval = p_ep->Interval;
        break;
    }
#else
    ep_params.Interval = 0u;
#endif
  } else {                                                      // Ctrl EP.
    ep_params.Addr = 0u;
    ep_params.Interval = 0u;
    ep_params.Type = USBH_EP_TYPE_CTRL;
    ep_params.MaxPktSize = p_dev->CtrlMaxPacketSize;
    ep_phy_nbr = 0u;
  }

  USBH_EP_HANDLE_UPDATE(p_ep_mgmt, if_ix, ep_phy_nbr);

  USBH_EP_MgmtStateSet(p_ep_mgmt,
                       USBH_EP_STATE_NONE,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_end;
  }
  p_ep_mgmt->DevPtr = p_dev;                                    // Init EP mgmt struct.
  p_ep_mgmt->FnctPtr = p_fnct;
  p_ep_mgmt->HCD_EP_DataPtr = DEF_NULL;
  p_ep_mgmt->EP_Ptr = p_ep;
  p_ep_mgmt->URB_HeadPtr = DEF_NULL;
  p_ep_mgmt->URB_TailPtr = DEF_NULL;
  p_ep_mgmt->URB_Cnt = 0u;

  p_hc = p_dev->HC_Ptr;
  p_hc->Drv.API_Ptr->EP_Open(&p_hc->Drv,
                             p_dev->Handle,
                             p_ep_mgmt->Handle,
                             &ep_params,
                             &p_ep_mgmt->HCD_EP_DataPtr,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_end;
  }

  CORE_ENTER_ATOMIC();                                          // Add EP mgmt to dev list.
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_dev->EP_MgmtTblPtr[ep_phy_nbr] = p_ep_mgmt;
#else
  p_ep_mgmt->NextPtr = p_dev->EP_MgmtHeadPtr;
  p_dev->EP_MgmtHeadPtr = p_ep_mgmt;
#endif
  CORE_EXIT_ATOMIC();

  USBH_EP_MgmtStateSet(p_ep_mgmt,
                       USBH_EP_STATE_READY,
                       p_err);

  LOG_VRB(("Open endpoint ", (u)ep_phy_nbr, " on device at address ", (u)p_dev->AddrCur));
  USBH_DBG_EP_STATS_INC(p_ep_mgmt->Handle, OpenCnt);
  return (p_ep_mgmt->Handle);

err_end:
  LOG_ERR(("Opening endpoint -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));

  USBH_EP_MgmtFree(p_ep_mgmt);

  return (USBH_EP_HANDLE_INVALID);
}

/****************************************************************************************************//**
 *                                               USBH_EP_Close()
 *
 * @brief    Closes endpoint.
 *
 * @param    p_dev       Pointer to the endpoint management structure.
 *
 * @param    -----       Argument validated by caller.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) Device must be acquired prior to calling this function.
 *******************************************************************************************************/
void USBH_EP_Close(USBH_DEV       *p_dev,
                   USBH_EP_HANDLE ep_handle,
                   RTOS_ERR       *p_err)
{
  USBH_HC_DRV  *p_hc_drv;
  USBH_EP_MGMT *p_ep_mgmt;
  RTOS_ERR     err_local;
  RTOS_ERR     err_kal;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(err_local, RTOS_ERR_NONE);

  KAL_LockAcquire(USBH_Ptr->AsyncEventListLock,                 // Lock async list to prevent modification from Abort.
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                          ep_handle,
                                          USBH_EP_STATE_ABORTING_CLOSING,
                                          KAL_TIMEOUT_INFINITE,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                    &err_kal);
    return;
  }

  USBH_EP_URB_Abort(p_ep_mgmt);

  KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                  &err_kal);
  (void)err_kal;

  p_hc_drv = &p_dev->HC_Ptr->Drv;
  p_hc_drv->API_Ptr->EP_Close(p_hc_drv,
                              p_ep_mgmt->HCD_EP_DataPtr,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(err_local, *p_err);
  }

  //                                                               Remove EP reference in device.
  CORE_ENTER_ATOMIC();
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_dev->EP_MgmtTblPtr[USBH_EP_ADDR_TO_PHY(p_ep_mgmt->EP_Ptr->Addr)] = DEF_NULL;
#else
  if (p_dev->EP_MgmtHeadPtr == p_ep_mgmt) {
    p_dev->EP_MgmtHeadPtr = p_ep_mgmt->NextPtr;
  } else {
    USBH_EP_MGMT *p_ep_mgmt_prev = p_dev->EP_MgmtHeadPtr;

    while ((p_ep_mgmt_prev != DEF_NULL)
           && (p_ep_mgmt_prev->NextPtr != p_ep_mgmt)) {
      p_ep_mgmt_prev = p_ep_mgmt_prev->NextPtr;
    }

    if (p_ep_mgmt_prev != DEF_NULL) {
      p_ep_mgmt_prev->NextPtr = p_ep_mgmt->NextPtr;
    }
  }
#endif

  USBH_DBG_EP_STATS_INC(p_ep_mgmt->Handle, CloseCnt);

  USBH_EP_HANDLE_INVALIDATE(p_ep_mgmt);                         // Invalidate all handles app and class drivers have.
  CORE_EXIT_ATOMIC();

  USBH_EP_MgmtStateSet(p_ep_mgmt,
                       USBH_EP_STATE_NONE,
                       p_err);

  USBH_EP_MgmtFree(p_ep_mgmt);

  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, err_local);
  }
}

/****************************************************************************************************//**
 *                                           USBH_EP_CtrlReset()
 *
 * @brief    Closes and re-opens default control endpoint.
 *
 * @param    p_dev   Pointer to the device structure
 *
 * @param    -----   Argument validated by caller.
 *
 * @param    p_err   Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_EP_CtrlReset(USBH_DEV *p_dev,
                       RTOS_ERR *p_err)
{
  USBH_HC_DRV        *p_hc_drv;
  USBH_EP_MGMT       *p_ctrl_ep_mgmt;
  USBH_HCD_EP_PARAMS ep_params;
  RTOS_ERR           local_err;

  p_ctrl_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                               p_dev->CtrlEP_Handle,
                                               USBH_EP_STATE_ABORTING_CLOSING,
                                               KAL_TIMEOUT_INFINITE,
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hc_drv = &p_dev->HC_Ptr->Drv;

  p_hc_drv->API_Ptr->EP_Close(p_hc_drv,
                              p_ctrl_ep_mgmt->HCD_EP_DataPtr,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  ep_params.DevHandle = p_dev->Handle;
  ep_params.Addr = 0u;
  ep_params.DevAddr = p_dev->Addr;
  ep_params.DevSpd = p_dev->Spd;
  ep_params.Interval = 0u;
  ep_params.MaxPktSize = p_dev->CtrlMaxPacketSize;
  ep_params.Type = USBH_EP_TYPE_CTRL;

  p_hc_drv->API_Ptr->EP_Open(p_hc_drv,
                             p_dev->Handle,
                             p_dev->CtrlEP_Handle,
                             &ep_params,
                             &p_ctrl_ep_mgmt->HCD_EP_DataPtr,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  USBH_EP_MgmtStateSet(p_ctrl_ep_mgmt,
                       USBH_EP_STATE_READY,
                       p_err);

  return;

end_err:
  USBH_EP_MgmtStateSet(p_ctrl_ep_mgmt,
                       USBH_EP_STATE_NONE,
                       &local_err);
  (void)local_err;
}

/****************************************************************************************************//**
 *                                               USBH_EP_Create()
 *
 * @brief    Create endpoint using information retrieved from device.
 *
 * @param    p_desc_buf  Pointer to the endpoint descriptor sent by device.
 *
 * @param    ----------  Argument validated by caller.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the USBH_EP struct, if successful.
 *           0,                         otherwise.
 *
 * @note     (1) See 'Universal Serial Bus specification, revision 2.0, section 9.6.6' for more
 *               information on endpoint descriptor.
 *******************************************************************************************************/
USBH_EP *USBH_EP_Create(CPU_INT08U *p_desc_buf,
                        RTOS_ERR   *p_err)
{
  USBH_EP *p_ep;

  p_ep = (USBH_EP *)Mem_DynPoolBlkGet(&USBH_Ptr->EP_Pool,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_ep->Addr = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[2u]);
  p_ep->Attrib = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[3u]);
  p_ep->MaxPktSize = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[4u]);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_ep->Interval = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[6u]);
#endif
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  p_ep->NextPtr = DEF_NULL;
#endif

  LOG_VRB(("Create EP ", (u)USBH_EP_ADDR_TO_PHY(p_ep->Addr)));

  return (p_ep);
}

/****************************************************************************************************//**
 *                                               USBH_EP_Del()
 *
 * @brief    Delete endpoint.
 *
 * @param    p_ep    Pointer to the endpoint to delete.
 *
 * @param    ----    Argument validated by caller.
 *******************************************************************************************************/
void USBH_EP_Del(USBH_EP *p_ep)
{
  RTOS_ERR err_lib;

  LOG_VRB(("Delete EP ", (u)USBH_EP_ADDR_TO_PHY(p_ep->Addr)));

  Mem_DynPoolBlkFree(&USBH_Ptr->EP_Pool,
                     (void *)p_ep,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Deleting endpoint -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  (void)&err_lib;
}

/****************************************************************************************************//**
 *                                           USBH_EP_CtrlXferLocked()
 *
 * @brief    Perform synchronous control transfer on endpoint.
 *
 * @param    p_dev       Pointer to the device.
 *
 * @param    req         bRequest value of setup packet.
 *
 * @param    req_type    bmRequestType value of setup packet.
 *
 * @param    val         wValue value of setup packet.
 *
 * @param    ix          wIndex value of setup packet.
 *
 * @param    p_buf       Pointer to the data buffer for data phase.
 *
 * @param    len         wLength value of setup packet that specifies number of data bytes in data
 *                       stage.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @return   Number of octets transfered in data stage (if any).
 *
 * @note     (1) For more information on setup packets, see 'Universal Serial Bus specification,
 *               revision 2.0, section 9.3'.
 *
 * @note     (2) Device MUST be locked by caller.
 *******************************************************************************************************/
CPU_INT16U USBH_EP_CtrlXferLocked(USBH_DEV   *p_dev,
                                  CPU_INT08U req,
                                  CPU_INT08U req_type,
                                  CPU_INT16U val,
                                  CPU_INT16U ix,
                                  CPU_INT08U *p_buf,
                                  CPU_INT16U len,
                                  CPU_INT16U buf_len,
                                  CPU_INT32U timeout,
                                  RTOS_ERR   *p_err)
{
  CPU_INT16U            *p_setup_buf;
  CPU_BOOLEAN           is_in;
  USBH_TOKEN            token;
  CPU_INT16U            req_word;
  CPU_INT16U            xfer_len = 0u;
  USBH_EP_XFER_SYNC_ARG xfer_sync_arg;
  RTOS_ERR              err_kal;

  RTOS_ASSERT_DBG_ERR_SET(((len <= 0u)
                           || (buf_len == 0u)
                           || (p_buf != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, 0u);

  if ((p_dev->State != USBH_DEV_STATE_CONFIG)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_ATTCH)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return (0u);
  }

  p_setup_buf = p_dev->SetupBufPtr;
  is_in = (req_type & USBH_DEV_REQ_DIR_MASK) != 0u ? DEF_TRUE : DEF_FALSE;

  xfer_sync_arg.XferLen = 0u;
  xfer_sync_arg.SemHandle = KAL_SemCreate("USBH EP - Ctrl Xfer Sem",
                                          DEF_NULL,
                                          &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, err_kal);
    return (0u);
  }

  KAL_LockAcquire(p_dev->DfltEP_LockHandle,
                  KAL_OPT_PEND_NONE,
                  timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err_del_sem;
  }
  //                                                               ----------------- SETUP PKT PHASE ------------------
  //                                                               Build setup pkt. See note (1).
  req_word = ((req << 8u) | req_type);
  p_setup_buf[0u] = MEM_VAL_GET_INT16U_LITTLE(&req_word);
  p_setup_buf[1u] = MEM_VAL_GET_INT16U_LITTLE(&val);
  p_setup_buf[2u] = MEM_VAL_GET_INT16U_LITTLE(&ix);
  p_setup_buf[3u] = MEM_VAL_GET_INT16U_LITTLE(&len);

  USBH_URB_Submit(p_dev,
                  p_dev->CtrlEP_Handle,
                  USBH_EP_TYPE_CTRL,
                  (CPU_INT08U *)(CPU_ADDR)p_setup_buf,
                  USBH_SETUP_PKT_LEN,
                  USBH_TOKEN_SETUP,
                  timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                  0u,
                  0u,
#endif
                  USBH_EP_XferSyncCmplCallback,
                  &xfer_sync_arg,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_all;
  }

  USBH_EP_XferSyncPend(p_dev->Handle,
                       p_dev->CtrlEP_Handle,
                       &xfer_sync_arg,
                       timeout,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_all;
  }

  if (xfer_sync_arg.XferLen != USBH_SETUP_PKT_LEN) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    goto end_rel_all;
  }

  LOG_VRB(("Dev addr: ", (u)p_dev->AddrCur, " EP: ", (u)0u, ". Ctrl setup complete, len = ", (u)xfer_sync_arg.XferLen));

  //                                                               -------------------- DATA PHASE --------------------
  if (len > 0u) {
    token = (is_in == DEF_TRUE) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;
    USBH_URB_Submit(p_dev,
                    p_dev->CtrlEP_Handle,
                    USBH_EP_TYPE_CTRL,
                    p_buf,
                    buf_len,
                    token,
                    timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                    0u,
                    0u,
#endif
                    USBH_EP_XferSyncCmplCallback,
                    &xfer_sync_arg,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel_all;
    }

    USBH_EP_XferSyncPend(p_dev->Handle,
                         p_dev->CtrlEP_Handle,
                         &xfer_sync_arg,
                         timeout,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel_all;
    }

    LOG_VRB(("Dev addr: ", (u)p_dev->AddrCur, " EP: ", (u)0u, ". Ctrl data complete, len = ", (u)xfer_sync_arg.XferLen));
  }

  xfer_len = xfer_sync_arg.XferLen;

  //                                                               ------------------- STATUS PHASE -------------------
  if ((is_in == DEF_TRUE)
      && (len > 0u)) {
    token = USBH_TOKEN_OUT;
  } else {
    token = USBH_TOKEN_IN;
  }

  USBH_URB_Submit(p_dev,
                  p_dev->CtrlEP_Handle,
                  USBH_EP_TYPE_CTRL,
                  DEF_NULL,
                  0u,
                  token,
                  timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                  0u,
                  0u,
#endif
                  USBH_EP_XferSyncCmplCallback,
                  &xfer_sync_arg,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_all;
  }

  USBH_EP_XferSyncPend(p_dev->Handle,
                       p_dev->CtrlEP_Handle,
                       &xfer_sync_arg,
                       timeout,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel_all;
  }

  LOG_VRB(("Dev addr: ", (u)p_dev->AddrCur, " EP: ", (u)0u, ". Ctrl status complete."));

end_rel_all:
  KAL_LockRelease(p_dev->DfltEP_LockHandle, &err_kal);

end_err_del_sem:
  KAL_SemDel(xfer_sync_arg.SemHandle);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_EP_AbortLocked()
 *
 * @brief    Aborts all transfers and clear halt condition on endpoint.
 *
 * @param    p_dev       Pointer on device structure.
 *
 * @param    ep_handle   Handle to endpoint. 0 for control endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) Device MUST be acquired by caller.
 *******************************************************************************************************/
void USBH_EP_AbortLocked(USBH_DEV       *p_dev,
                         USBH_EP_HANDLE ep_handle,
                         RTOS_ERR       *p_err)
{
  USBH_EP_MGMT   *p_ep_mgmt;
  USBH_EP_HANDLE corr_ep_handle;
  USBH_HC_DRV    *p_hc_drv;
  RTOS_ERR       err_kal;

  if ((p_dev->State != USBH_DEV_STATE_ATTCH)
      && (p_dev->State != USBH_DEV_STATE_ADDR)
      && (p_dev->State != USBH_DEV_STATE_CONFIG)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  corr_ep_handle = (ep_handle != USBH_EP_HANDLE_CTRL) ? ep_handle : p_dev->CtrlEP_Handle;

  KAL_LockAcquire(USBH_Ptr->AsyncEventListLock,                 // Lock async list to prevent modification from Abort.
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                          corr_ep_handle,
                                          USBH_EP_STATE_ABORTING_CLOSING,
                                          KAL_TIMEOUT_INFINITE,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_rel;
  }

  USBH_EP_URB_Abort(p_ep_mgmt);

  p_hc_drv = &p_ep_mgmt->DevPtr->HC_Ptr->Drv;
  p_hc_drv->API_Ptr->EP_HaltClr(p_hc_drv,
                                p_ep_mgmt->HCD_EP_DataPtr,
                                DEF_NO,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_rel;
  }

  USBH_EP_MgmtStateSet(p_ep_mgmt,                               // Return EP to 'ready' state.
                       USBH_EP_STATE_READY,
                       p_err);

err_rel:
  KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                  &err_kal);
  (void)err_kal;
}

/****************************************************************************************************//**
 *                                           USBH_EP_SuspendLocked()
 *
 * @brief    Suspend/resume all endpoints on given device.
 *
 * @param    p_dev       Pointer on device structure.
 *
 * @param    suspended   Flag indicating if suspending or resuming.
 *                       DEF_YES, must suspend endpoint.
 *                       DEF_NO,  must resume  endpoint.
 *
 * @note     (1) Device must be acquired by caller.
 *******************************************************************************************************/
void USBH_EP_SuspendLocked(USBH_DEV    *p_dev,
                           CPU_BOOLEAN suspended)
{
  USBH_HC_DRV   *p_hc_drv = &p_dev->HC_Ptr->Drv;
  USBH_EP_MGMT  *p_ep_mgmt;
  USBH_EP_STATE state;
  RTOS_ERR      err;

  //                                                               Resume each endpoint.
  USBH_OBJ_ITERATOR_INIT_EXT(p_ep_mgmt, p_dev, USBH_EP_MAX_NBR, EP_Mgmt) {
    if (p_ep_mgmt != DEF_NULL) {
      p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                              p_ep_mgmt->Handle,
                                              USBH_EP_STATE_PREPARING,
                                              KAL_TIMEOUT_INFINITE,
                                              &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error locking EP for Suspend/Resume. -> ", RTOS_ERR_LOG_ARG_GET(err)));
      } else {
        p_hc_drv->API_Ptr->EP_Suspend(p_hc_drv,
                                      p_ep_mgmt->HCD_EP_DataPtr,
                                      suspended,
                                      &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error Suspending/Resuming EP. -> ", RTOS_ERR_LOG_ARG_GET(err)));
        }

        state = (p_ep_mgmt->URB_Cnt == 0u) ? USBH_EP_STATE_READY : USBH_EP_STATE_QUEUED;

        USBH_EP_MgmtStateSet(p_ep_mgmt,
                             state,
                             &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error re-setting EP state after Suspend/Resume. -> ", RTOS_ERR_LOG_ARG_GET(err)));
        }
      }
    }

    USBH_OBJ_ITERATOR_NEXT_GET(p_ep_mgmt);
  }
}

/****************************************************************************************************//**
 *                                       USBH_EP_URB_ListAbortedFree()
 *
 * @brief    Execute callback and frees every URB in list.
 *
 * @param    dev_handle                  Handle to device.
 *
 * @param    ----------                  Argument validated by caller.
 *
 * @param    ep_handle                   Handle to endpoint.
 *
 * @param    ---------                   Argument validated by caller.
 *
 * @param    p_urb_mgmt_aborted_head     Pointer to the head of URB list.
 *
 * @note     (1) Device and Endpoint MUST be released by caller before calling this function.
 *******************************************************************************************************/
void USBH_EP_URB_ListAbortedFree(USBH_DEV_HANDLE dev_handle,
                                 USBH_EP_HANDLE  ep_handle,
                                 USBH_URB_MGMT   *p_urb_mgmt_aborted_head)
{
  USBH_URB_MGMT *p_urb_mgmt = p_urb_mgmt_aborted_head;

  while (p_urb_mgmt != DEF_NULL) {
    CPU_INT08U      *p_buf = p_urb_mgmt->BufPtr;
    CPU_INT32U      buf_len = p_urb_mgmt->BufLen;
    void            *p_arg = p_urb_mgmt->AsyncFnctArgPtr;
    USBH_URB_MGMT   *p_urb_mgmt_nxt = p_urb_mgmt->NextPtr;
    USBH_ASYNC_FNCT async_fnct = p_urb_mgmt->AsyncFnct;
    RTOS_ERR        err;

    USBH_DBG_EP_STATS_INC(ep_handle, URB_AbortedFreeCnt);

    Mem_DynPoolBlkFree(&USBH_Ptr->URB_MgmtPool,
                       (void *)p_urb_mgmt,
                       &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing URB mgmt struct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }

    if (async_fnct != DEF_NULL) {
      USBH_DBG_EP_STATS_INC(ep_handle, URB_AbortedFreeCallbackCnt);

      RTOS_ERR_SET(err, RTOS_ERR_ABORT);

      async_fnct(dev_handle,
                 ep_handle,
                 p_buf,
                 buf_len,
                 0u,
                 p_arg,
                 err);
    }

    p_urb_mgmt = p_urb_mgmt_nxt;
  }
}

/****************************************************************************************************//**
 *                                           USBH_URB_Complete()
 *
 * @brief    Handles a URB after transfer has been done'd or aborted.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *******************************************************************************************************/
void USBH_URB_Complete(USBH_DEV_HANDLE dev_handle,
                       USBH_EP_HANDLE  ep_handle)
{
  CPU_INT32U          xfer_len = 0u;
  USBH_HC_DRV         *p_hc_drv;
  USBH_DEV            *p_dev;
  USBH_EP_MGMT        *p_ep_mgmt;
  USBH_URB_MGMT       *p_urb_mgmt = DEF_NULL;
  USBH_HCD_URB_PARAMS hcd_urb_params;
  USBH_EP_STATE       end_state;
  RTOS_ERR            err;
  RTOS_ERR            local_err;

  USBH_DBG_EP_STATS_INC(ep_handle, URB_CmplCnt);

  p_dev = USBH_DevAcquireCompleting(dev_handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_INVALID_HANDLE) {
      USBH_DBG_EP_STATS_INC(ep_handle, URB_CmplCntErrDevHandle);
      LOG_DBG(("WARNING - Invalid dev Handle to URB completion"));
    } else {
      USBH_DBG_EP_STATS_INC(ep_handle, URB_CmplCntErrDevOther);
      LOG_ERR(("Acquiring dev in AsyncComplete -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
    return;
  }

  p_hc_drv = &p_dev->HC_Ptr->Drv;
  p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,                // Switch EP to 'completing' state.
                                          ep_handle,
                                          USBH_EP_STATE_COMPLETING,
                                          KAL_TIMEOUT_INFINITE,
                                          &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    USBH_DBG_EP_STATS_INC(ep_handle, URB_CmplCntErrEP);
    LOG_ERR(("Failed to acquire EP in async complete -> ", RTOS_ERR_LOG_ARG_GET(err)));
    goto end_dev_rel;
  }

  p_urb_mgmt = p_ep_mgmt->URB_HeadPtr;
  if (p_urb_mgmt == DEF_NULL) {                                 // Chk if URB aborted.
    USBH_DBG_EP_STATS_INC(ep_handle, URB_CmplCntErrURB);
    LOG_DBG(("WARNING: URB was aborted on EP ", (u)USBH_EP_ADDR_TO_PHY(p_ep_mgmt->EP_Ptr->Addr)));
    goto end_rel;
  }

  hcd_urb_params.BufPtr = p_urb_mgmt->BufPtr;
  hcd_urb_params.BufLen = p_urb_mgmt->BufLen;
  hcd_urb_params.Token = p_urb_mgmt->Token;

  xfer_len = p_hc_drv->API_Ptr->URB_Complete(p_hc_drv,
                                             p_ep_mgmt->HCD_EP_DataPtr,
                                             p_urb_mgmt->HCD_URB_DataPtr,
                                             &hcd_urb_params,
                                             &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_DBG(("WARNING: Driver failed to complete URB."));
  }

  p_ep_mgmt->URB_HeadPtr = p_urb_mgmt->NextPtr;
  p_ep_mgmt->URB_Cnt--;

end_rel:
  if (p_ep_mgmt->URB_Cnt == 0u) {
    end_state = USBH_EP_STATE_READY;
    p_ep_mgmt->URB_TailPtr = DEF_NULL;
  } else {
    end_state = USBH_EP_STATE_QUEUED;
  }

  USBH_EP_MgmtStateSet(p_ep_mgmt,
                       end_state,
                       &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to set EP in correct state after completing async URB -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }

end_dev_rel:
  USBH_DevRelShared(p_dev);

  USBH_DBG_EP_STATS_INC_IF_TRUE(p_ep_mgmt->Handle, URB_CmplSuccessCnt, (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  if (p_urb_mgmt != DEF_NULL) {
    CPU_INT08U      *p_buf = p_urb_mgmt->BufPtr;
    CPU_INT32U      buf_len = p_urb_mgmt->BufLen;
    USBH_ASYNC_FNCT callback_fnct = p_urb_mgmt->AsyncFnct;
    void            *p_arg = p_urb_mgmt->AsyncFnctArgPtr;

    Mem_DynPoolBlkFree(&USBH_Ptr->URB_MgmtPool,
                       (void *)p_urb_mgmt,
                       &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing URB mgmt struct -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }

    if (callback_fnct != DEF_NULL) {
      USBH_DBG_EP_STATS_INC(p_ep_mgmt->Handle, URB_CmplCallbackCnt);

      callback_fnct(p_dev->Handle,
                    ep_handle,
                    p_buf,
                    buf_len,
                    xfer_len,
                    p_arg,
                    err);
    }
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_IO_FATAL) {          // In case of fatal err, disconn dev. See note 1.
      USBH_HUB_PortDisconnReq(p_dev->HubFnctPtr->Handle,
                              p_dev->PortNbr,
                              DEF_NULL,
                              DEF_NULL,
                              &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Disconnecting problematic device -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
      }
    }
  }
}

/********************************************************************************************************
 *                                           HCD-RELATED FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_URB_Done()
 *
 * @brief    Signal to core that URB processing is done, in the HCD.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *******************************************************************************************************/
void USBH_URB_Done(USBH_DEV_HANDLE dev_handle,
                   USBH_EP_HANDLE  ep_handle)
{
  USBH_DBG_EP_STATS_INC(ep_handle, URB_DoneCnt);

  USBH_AsyncEventPost(USBH_ASYNC_EVENT_TYPE_URB_COMPLETE,
                      dev_handle,
                      ep_handle,
                      DEF_NULL);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_EP_PtrGet()
 *
 * @brief    Gets a pointer to endpoint.
 *
 * @param    p_config        Pointer to the configuration structure.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    if_ix           Interface index.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the endpoint structure, if successful.
 *           0,                             otherwise.
 *******************************************************************************************************/
static USBH_EP *USBH_EP_PtrGet(USBH_CONFIG      *p_config,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               CPU_INT08U       ep_addr,
                               RTOS_ERR         *p_err)
{
  USBH_IF *p_if;
  USBH_EP *p_ep;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(((USBH_EP_ADDR_TO_PHY(ep_addr) >= 2u)
                           && (USBH_EP_ADDR_TO_PHY(ep_addr) < USBH_EP_MAX_NBR)), *p_err, RTOS_ERR_INVALID_HANDLE, DEF_NULL);

  p_if = USBH_IF_PtrGet(p_config, fnct_handle, if_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_ep = p_if->EPTblPtr[USBH_EP_ADDR_TO_PHY(ep_addr) - 2u];
#else
  p_ep = p_if->EPHeadPtr;
  while (p_ep != DEF_NULL) {
    if (p_ep->Addr == ep_addr) {
      break;
    }

    p_ep = p_ep->NextPtr;
  }
#endif
  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
  }

  return (p_ep);
}

/****************************************************************************************************//**
 *                                           USBH_EP_XferSync()
 *
 * @brief    Transmit data on Bulk or Interrupt endpoint. This function is blocking.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    ep_type     Endpoint type.
 *                       USBH_EP_TYPE_INTR
 *                       USBH_EP_TYPE_BULK
 *
 * @param    ep_dir_in   Endpoint direction.
 *                       DEF_YES     Direction IN
 *                       DEF_NO      Direction OUT
 *
 * @param    p_buf       Pointer to the destination buffer to receive data.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @return   Number of octets transferred.
 *
 * @note     (1) A Bulk or Interrupt Endpoint's Address goes from 0x01 to 0x0F and from 0x81 to 0x8F
 *               inclusively.
 *
 * @note     (2) A fatal error occurs when a has a behaviour that can compromise the entire bus. This
 *               kind of issues include babble and collision.
 *******************************************************************************************************/
static CPU_INT32U USBH_EP_XferSync(USBH_DEV_HANDLE dev_handle,
                                   USBH_EP_HANDLE  ep_handle,
                                   CPU_INT08U      ep_type,
                                   CPU_BOOLEAN     dir_in,
                                   CPU_INT08U      *p_buf,
                                   CPU_INT32U      buf_len,
                                   CPU_INT32U      timeout,
                                   RTOS_ERR        *p_err)
{
  USBH_DEV              *p_dev;
  USBH_DEV_HANDLE       hub_dev_handle;
  USBH_HUB_FNCT         *p_hub_fnct;
  USBH_TOKEN            token;
  CPU_INT08U            port_nbr;
  USBH_EP_XFER_SYNC_ARG xfer_sync_arg;

  //                                                               See Note #1.
  RTOS_ASSERT_DBG_ERR_SET(((USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) >= 2u)
                           && (USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) < USBH_EP_MAX_NBR)), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  xfer_sync_arg.XferLen = 0u;

  if (p_dev->State != USBH_DEV_STATE_CONFIG) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  xfer_sync_arg.SemHandle = KAL_SemCreate("USBH EP - Sync Sem",
                                          DEF_NULL,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  token = (dir_in == DEF_YES) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;

  USBH_URB_Submit(p_dev,
                  ep_handle,
                  ep_type,
                  p_buf,
                  buf_len,
                  token,
                  timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                  0u,
                  0u,
#endif
                  USBH_EP_XferSyncCmplCallback,
                  &xfer_sync_arg,
                  p_err);

  p_hub_fnct = p_dev->HubFnctPtr;
  hub_dev_handle = p_hub_fnct->Handle;
  port_nbr = p_dev->PortNbr;

  USBH_DevRelShared(p_dev);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBH_EP_XferSyncPend(dev_handle,
                         ep_handle,
                         &xfer_sync_arg,
                         timeout,
                         p_err);
  }

  KAL_SemDel(xfer_sync_arg.SemHandle);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO_FATAL) {         // In case of fatal err, disconn dev. See note 2.
    RTOS_ERR err_disconn;

    USBH_HUB_PortDisconnReq(hub_dev_handle,
                            port_nbr,
                            DEF_NULL,
                            DEF_NULL,
                            &err_disconn);
    if (RTOS_ERR_CODE_GET(err_disconn) != RTOS_ERR_NONE) {
      LOG_ERR(("Disconnecting problematic device -> ", RTOS_ERR_LOG_ARG_GET(err_disconn)));
    }
  }

  return (xfer_sync_arg.XferLen);

end_rel:
  USBH_DevRelShared(p_dev);

  return (xfer_sync_arg.XferLen);
}

/****************************************************************************************************//**
 *                                           USBH_EP_XferSyncPend()
 *
 * @brief    Pends on semaphore until transfer has completed.
 *
 * @param    dev_handle          Handle to device.
 *
 * @param    ep_handle           Handle to endpoint on which the transfer is made.
 *
 * @param    p_sync_xfer_arg     Pointer to the sync xfer arg structure.
 *
 * @param    timeout             Timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive return error code from this function
 *                               or from the transfer completion.
 *******************************************************************************************************/
static void USBH_EP_XferSyncPend(USBH_DEV_HANDLE       dev_handle,
                                 USBH_EP_HANDLE        ep_handle,
                                 USBH_EP_XFER_SYNC_ARG *p_xfer_sync_arg,
                                 CPU_INT32U            timeout,
                                 RTOS_ERR              *p_err)
{
  RTOS_ERR err_kal;

  KAL_SemPend(p_xfer_sync_arg->SemHandle,
              KAL_OPT_PEND_NONE,
              timeout,
              &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) == RTOS_ERR_TIMEOUT) {
    RTOS_ERR local_err;

    USBH_EP_Abort(dev_handle,
                  ep_handle,
                  &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      KAL_SemPend(p_xfer_sync_arg->SemHandle,                   // Wait for callback to be called following Abort().
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  &err_kal);
      if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
        LOG_ERR(("Error when pending on transfer abort completion -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
      }
    } else {
      LOG_ERR(("Error when aborting timed out transfer -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
  } else {
    RTOS_ERR_COPY(*p_err, p_xfer_sync_arg->Err);
  }
}

/****************************************************************************************************//**
 *                                       USBH_EP_XferSyncCmplCallback()
 *
 * @brief    Callback used to post the semaphore of a synchronous transfer.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint on which the transfer has completed.
 *
 * @param    p_buf       Pointer to the buffer.
 *
 * @param    buf_len     Length of buffer.
 *
 * @param    xfer_len    Actual number of bytes transferred.
 *
 * @param    p_arg       Pointer to the argument passed to async task, in this case a pointer to a sync
 *                       xfer arg structure.
 *
 * @param    err         Variable containing error code from the transfer.
 *******************************************************************************************************/
static void USBH_EP_XferSyncCmplCallback(USBH_DEV_HANDLE dev_handle,
                                         USBH_EP_HANDLE  ep_handle,
                                         CPU_INT08U      *p_buf,
                                         CPU_INT32U      buf_len,
                                         CPU_INT32U      xfer_len,
                                         void            *p_arg,
                                         RTOS_ERR        err)
{
  USBH_EP_XFER_SYNC_ARG *p_sync_xfer_arg = (USBH_EP_XFER_SYNC_ARG *)p_arg;
  RTOS_ERR              err_kal;

  (void)dev_handle;
  (void)ep_handle;
  (void)p_buf;
  (void)buf_len;

  p_sync_xfer_arg->XferLen = xfer_len;
  RTOS_ERR_COPY(p_sync_xfer_arg->Err, err);

  KAL_SemPost(p_sync_xfer_arg->SemHandle,
              KAL_OPT_POST_NONE,
              &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
    LOG_ERR(("Error when posting transfer completion -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_EP_XferAsync()
 *
 * @brief    Transmit data on non-control endpoint. This function is non-blocking. It returns immediately
 *           after transfer preparation. Upon completion, a callback provided by the application will be
 *           called to finalize the transfer.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    ep_type         Endpoint type.
 *                           USBH_EP_TYPE_INTR
 *                           USBH_EP_TYPE_BULK
 *                           USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                           DEF_YES     Direction IN
 *                           DEF_NO      Direction OUT
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) A Non-Control Endpoint's Address goes from 0x01 to 0x0F and from 0x81 to 0x8F inclusively.
 *******************************************************************************************************/
static void USBH_EP_XferAsync(USBH_DEV_HANDLE dev_handle,
                              USBH_EP_HANDLE  ep_handle,
                              CPU_INT08U      ep_type,
                              CPU_BOOLEAN     dir_in,
                              CPU_INT08U      *p_buf,
                              CPU_INT32U      buf_len,
                              USBH_ASYNC_FNCT async_fnct,
                              void            *p_async_arg,
                              RTOS_ERR        *p_err)
{
  USBH_DEV   *p_dev;
  USBH_TOKEN token = (dir_in == DEF_YES) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;

  RTOS_ASSERT_DBG_ERR_SET(((USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) >= 2u)
                           && (USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) < USBH_EP_MAX_NBR)), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  p_dev = USBH_DevAcquireShared(dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_URB_Submit(p_dev,
                  ep_handle,
                  ep_type,
                  p_buf,
                  buf_len,
                  token,
                  KAL_TIMEOUT_INFINITE,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                  0u,
                  0u,
#endif
                  async_fnct,
                  p_async_arg,
                  p_err);

  USBH_DevRelShared(p_dev);
}

/****************************************************************************************************//**
 *                                               USBH_EP_MgmtGet()
 *
 * @brief    Gets endpoint management associated to handle. Using this function does not guarantee
 *           exclusive access to endpoint management.
 *
 * @param    p_dev       Pointer to the device structure.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @return   Pointer to the endpoint management structure, if successful.
 *           0,                                        otherwise.
 *
 * @note     (1) Device MUST be acquired by caller.
 *
 * @note     (2) WARNING: This function will return the endpoint management structure at index
 *                       indicated in handle. It WILL NOT lock it nor validate handle.
 *******************************************************************************************************/
static USBH_EP_MGMT *USBH_EP_MgmtGet(USBH_DEV       *p_dev,
                                     USBH_EP_HANDLE ep_handle)
{
  USBH_EP_MGMT *p_ep_mgmt;
  CORE_DECLARE_IRQ_STATE;

  if (ep_handle != p_dev->CtrlEP_Handle) {                      // Validate handle.
    RTOS_ASSERT_DBG(((USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) >= 2u)
                     && (USBH_HANDLE_EP_PHY_NBR_GET(ep_handle) < USBH_EP_MAX_NBR)), RTOS_ERR_INVALID_HANDLE, DEF_NULL);
  }

  CORE_ENTER_ATOMIC();                                          // Retrieve EP mgmt structure.
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_ep_mgmt = p_dev->EP_MgmtTblPtr[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)];
#else
  p_ep_mgmt = p_dev->EP_MgmtHeadPtr;

  while ((p_ep_mgmt != DEF_NULL)
         && (USBH_HANDLE_EP_PHY_NBR_GET(p_ep_mgmt->Handle) != USBH_HANDLE_EP_PHY_NBR_GET(ep_handle))) {
    p_ep_mgmt = p_ep_mgmt->NextPtr;
  }
#endif
  CORE_EXIT_ATOMIC();

  return (p_ep_mgmt);
}

/****************************************************************************************************//**
 *                                           USBH_EP_MgmtStateSet()
 *
 * @brief    Set endpoint in a given state. State must be a non-blocking, non-locked one. This function
 *           unlocks the p_ep_mgmt.
 *
 * @param    p_ep_mgmt   Pointer to the USBH_EP_MGMT structure of the endpoint on which to set the state.
 *
 * @param    state       State to which to set the endpoint.
 *
 * @param    p_err       Pointer to the variable that will receive the error code of this function.
 *
 * @note     (1) This function must be called only for states that cannot block. It is guaranteed to
 *               be non-blocking, it will return immediately.
 *
 * @note     (2) After setting the state, any previously acquired reference to p_ep_mgmt MUST NOT be
 *               used before being re-obtained again.
 *******************************************************************************************************/
static void USBH_EP_MgmtStateSet(USBH_EP_MGMT  *p_ep_mgmt,
                                 USBH_EP_STATE state,
                                 RTOS_ERR      *p_err)
{
  //                                                               Confirm state will not block, see Note #1.
  RTOS_ASSERT_DBG_ERR_SET(((state == USBH_EP_STATE_NONE)
                           || (state == USBH_EP_STATE_READY)
                           || (state == USBH_EP_STATE_QUEUED)
                           || (state == USBH_EP_STATE_ABORT_CLOSE_CMPL)), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBH_EP_MgmtStateSwitch(p_ep_mgmt,
                          state,
                          DEF_NULL,
                          0u,
                          p_err);
}

/****************************************************************************************************//**
 *                                       USBH_EP_MgmtStateWaitSwitch()
 *
 * @brief    Wait for endpoint to be switched into a particular state. Endpoint is locked afterwards.
 *
 * @param    p_dev       Pointer to the USBH_DEV structure to which the endpoint belongs.
 *
 * @param    ep_handle   Handle to endpoint on which to switch state.
 *
 * @param    state       State to which to set the endpoint.
 *
 * @param    timeout_ms  Timeout, in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the error code of this function.
 *
 * @return   Pointer to the USBH_EP_MGMT structure associated to endpoint, if NO error(s),
 *           DEF_NULL,                                                     otherwise.
 *
 * @note     (1) This function MAY pend, if not possible to set the EP in the given state immediately,
 *               it MUST NOT be called from an ISR or a critical section.
 *
 * @note     (2) This function MUST NOT be called to set the EP in a state that does not require to
 *               block.
 *
 * @note     (3) If 'p_err' is not RTOS_ERR_NONE and/or the returned p_ep_mgmt is NULL, p_ep_mgmt
 *               MUST NOT be used, the state is not correct and the endpoint is not locked.
 *******************************************************************************************************/
static USBH_EP_MGMT *USBH_EP_MgmtStateWaitSwitch(USBH_DEV       *p_dev,
                                                 USBH_EP_HANDLE ep_handle,
                                                 USBH_EP_STATE  state,
                                                 CPU_INT32U     timeout_ms,
                                                 RTOS_ERR       *p_err)
{
  USBH_EP_MGMT *p_ep_mgmt;

  //                                                               Make sure state is set with this fnct. See Note #2.
  RTOS_ASSERT_DBG_ERR_SET(((state == USBH_EP_STATE_PREPARING)
                           || (state == USBH_EP_STATE_COMPLETING)
                           || (state == USBH_EP_STATE_ABORTING_CLOSING)), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  p_ep_mgmt = USBH_EP_MgmtGet(p_dev, ep_handle);                // Get EP mgmt ptr.
  if (p_ep_mgmt == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  //                                                               Make sure handle is still valid for this EP mgmt.
  if (USBH_EP_HANDLE_VALIDATE(p_ep_mgmt, ep_handle) != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NULL);
  }

  USBH_EP_MgmtStateSwitch(p_ep_mgmt,
                          state,
                          USBH_EP_MgmtStateSwitchOnEval,
                          timeout_ms,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_ep_mgmt);
}

/****************************************************************************************************//**
 *                                           USBH_EP_MgmtStateSwitch()
 *
 * @brief    Generic function that processes the call to KAL_MonOp() for endpoint state switch.
 *
 * @param    p_ep_mgmt       Pointer to the USBH_EP_MGMT structure of the endpoint on which to change state.
 *
 * @param    state           State to which to set the endpoint.
 *
 * @param    p_eval_fnct     Function that will be used as the 'Evaluation' function for the monitor.
 *
 * @param    timeout_ms      Timeout, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive the error code of this function.
 *
 * @note     (1) The error may be set in two ways: first during the call to KAL_MonOp by the enter and
 *               evaluation function. If no error is returned this way, p_err will be set based on the
 *               error value returned by the call to KAL_MonOp().
 *******************************************************************************************************/
static void USBH_EP_MgmtStateSwitch(USBH_EP_MGMT  *p_ep_mgmt,
                                    USBH_EP_STATE state,
                                    KAL_MON_RES (*p_eval_fnct)(void *p_mon_data,
                                                               void *p_eval_op_data,
                                                               void *p_scan_op_data),
                                    CPU_INT32U timeout_ms,
                                    RTOS_ERR   *p_err)
{
  USBH_EP_STATE_SWITCH_ARG switch_arg;
  RTOS_ERR                 err_kal;

  switch_arg.StateToSwitch = state;
  switch_arg.ErrPtr = p_err;

  KAL_MonOp(p_ep_mgmt->MonHandle,
            &switch_arg,
            USBH_EP_MgmtStateSwitchOnEnter,
            p_eval_fnct,
            KAL_OPT_MON_NONE,
            timeout_ms,
            &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {            // Err handling, if err not already set. See Note #1.
    RTOS_ERR_COPY(*p_err, err_kal);
  }

  return;
}

/****************************************************************************************************//**
 *                                       USBH_EP_MgmtStateSwitchOnEnter()
 *
 * @brief    Enter function used by monitor for endpoints state.
 *
 * @param    p_mon_data  Pointer to the data passed at monitor creation. In this case, a pointer to the
 *                       USBH_EP_MGMT structure of the endpoint evaluated.
 *
 * @param    p_op_data   Pointer to the data passed to KAL_MonOp(). In this case, the
 *                       USBH_EP_STATE_SWITCH_ARG on the caller's stack.
 *
 * @return   KAL monitor result, which consists of a combination of the following flags:
 *               KAL_MON_RES_BLOCK or KAL_MON_RES_ALLOW  To block or allow task being evaluated to run.
 *               KAL_MON_RES_STOP_EVAL                   To stop evaluation other tasks.
 *******************************************************************************************************/
static KAL_MON_RES USBH_EP_MgmtStateSwitchOnEnter(void *p_mon_data,
                                                  void *p_op_data)
{
  USBH_EP_MGMT             *p_ep_mgmt = (USBH_EP_MGMT *)p_mon_data;
  USBH_EP_STATE_SWITCH_ARG *p_switch_arg = (USBH_EP_STATE_SWITCH_ARG *)p_op_data;
  KAL_MON_RES              res = KAL_MON_RES_ALLOW;

  RTOS_ERR_SET(*p_switch_arg->ErrPtr, RTOS_ERR_NONE);

  switch (p_switch_arg->StateToSwitch) {
    case USBH_EP_STATE_NONE:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_NONE:
        case USBH_EP_STATE_READY:
        case USBH_EP_STATE_ABORT_CLOSE_CMPL:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_NONE);
          p_ep_mgmt->State = USBH_EP_STATE_NONE;
          break;

        default:                                                // Problem detected in state machine.
          res |= KAL_MON_RES_STOP_EVAL;
          RTOS_ERR_SET(*p_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    case USBH_EP_STATE_READY:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_READY:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_READY);
          res |= KAL_MON_RES_STOP_EVAL;
          break;

        case USBH_EP_STATE_NONE:
        case USBH_EP_STATE_PREPARING:
        case USBH_EP_STATE_COMPLETING:
        case USBH_EP_STATE_ABORT_CLOSE_CMPL:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_READY);
          p_ep_mgmt->State = USBH_EP_STATE_READY;
          break;

        case USBH_EP_STATE_ABORTING_CLOSING:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_ABORT_CLOSE_CMPL);
          p_ep_mgmt->State = USBH_EP_STATE_ABORT_CLOSE_CMPL;
          break;

        default:                                                // Problem in EP state machine.
          res |= KAL_MON_RES_STOP_EVAL;
          RTOS_ERR_SET(*p_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    case USBH_EP_STATE_QUEUED:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_PREPARING:                           // Switch state to 'Queued', if in correct state.
        case USBH_EP_STATE_COMPLETING:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_QUEUED);
          p_ep_mgmt->State = USBH_EP_STATE_QUEUED;
          break;

        case USBH_EP_STATE_ABORTING_CLOSING:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_ABORT_CLOSE_CMPL);
          p_ep_mgmt->State = USBH_EP_STATE_ABORT_CLOSE_CMPL;
          break;

        default:                                                // Problem in EP state machine.
          res |= KAL_MON_RES_STOP_EVAL;
          RTOS_ERR_SET(*p_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    case USBH_EP_STATE_ABORTING_CLOSING:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_READY:                               // No need to block if no xfer or xfer is queued.
        case USBH_EP_STATE_QUEUED:
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_ABORT_CLOSE_CMPL);
          res |= KAL_MON_RES_STOP_EVAL;
          p_ep_mgmt->State = USBH_EP_STATE_ABORT_CLOSE_CMPL;
          break;

        case USBH_EP_STATE_PREPARING:
        case USBH_EP_STATE_COMPLETING:                          // Indicate an abort is req'd.
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_ABORTING_CLOSING);
          res = KAL_MON_RES_BLOCK;
          p_ep_mgmt->State = USBH_EP_STATE_ABORTING_CLOSING;
          break;

        default:                                                // Abort cannot be called when EP is in another state.
          res |= KAL_MON_RES_STOP_EVAL;
          RTOS_ERR_SET(*p_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    default:                                                    // Other states switch will be checked in OnEval.
      res = KAL_MON_RES_BLOCK;
      break;
  }

  return (res);
}

/****************************************************************************************************//**
 *                                       USBH_EP_MgmtStateSwitchOnEval()
 *
 * @brief    Evaluation function used by monitor for endpoints state.
 *
 * @param    p_mon_data      Pointer to the data passed at monitor creation. In this case, a pointer to
 *                           the USBH_EP_MGMT structure of the endpoint evaluated.
 *
 * @param    p_eval_op_data  Pointer to the data passed to KAL_MonOp(). In this case, the
 *                           USBH_EP_STATE_SWITCH_ARG on one of the previous caller's stack.
 *
 * @param    p_scan_op_data  Pointer to the data passed to KAL_MonOp(). In this case, the
 *                           USBH_EP_STATE_SWITCH_ARG on the caller's stack.
 *
 * @return   KAL monitor result, which consists of a combination of the following flags:
 *               KAL_MON_RES_BLOCK or KAL_MON_RES_ALLOW  To block or allow task being evaluated to run.
 *               KAL_MON_RES_STOP_EVAL                   To stop evaluation other tasks.
 *******************************************************************************************************/
static KAL_MON_RES USBH_EP_MgmtStateSwitchOnEval(void *p_mon_data,
                                                 void *p_eval_op_data,
                                                 void *p_scan_op_data)
{
  USBH_EP_MGMT             *p_ep_mgmt = (USBH_EP_MGMT *)p_mon_data;
  USBH_EP_STATE_SWITCH_ARG *p_eval_switch_arg = (USBH_EP_STATE_SWITCH_ARG *)p_eval_op_data;
  KAL_MON_RES              res = KAL_MON_RES_BLOCK;

  (void)&p_scan_op_data;

  switch (p_eval_switch_arg->StateToSwitch) {
    case USBH_EP_STATE_PREPARING:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_READY:                               // Switch state and allow task to continue.
        case USBH_EP_STATE_QUEUED:
          if (p_ep_mgmt->URB_Cnt < DEF_INT_08U_MAX_VAL) {
            USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_PREPARING);
            res = (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
            p_ep_mgmt->State = USBH_EP_STATE_PREPARING;
          }
          //                                                       Else, do nothing, wait for URB_Cnt to dec.
          break;

        case USBH_EP_STATE_PREPARING:                           // Do nothing, wait for EP to be in an 'avail' state.
        case USBH_EP_STATE_COMPLETING:
          break;

        default:                                                // Invalid state transition. EP is being aborted/closed.
          res = KAL_MON_RES_ALLOW;
          RTOS_ERR_SET(*p_eval_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    case USBH_EP_STATE_COMPLETING:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_QUEUED:                              // Allow task to complete xfer.
          USBH_DBG_EP_STATS_STATE_SWITCH(p_ep_mgmt, USBH_EP_STATE_COMPLETING);
          res = (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
          p_ep_mgmt->State = USBH_EP_STATE_COMPLETING;
          break;

        case USBH_EP_STATE_PREPARING:                           // Wait for EP to be avail.
          break;

        default:                                                // Invalid state transition.
          RTOS_ERR_SET(*p_eval_switch_arg->ErrPtr, RTOS_ERR_INVALID_STATE);
          res = KAL_MON_RES_ALLOW;
          USBH_DBG_EP_STATE_MACHINE_TRAP();
          break;
      }
      break;

    case USBH_EP_STATE_ABORTING_CLOSING:
      switch (p_ep_mgmt->State) {
        case USBH_EP_STATE_ABORT_CLOSE_CMPL:                    // Task can be resumed.
          res = (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
          break;

        default:                                                // Do nothing, wait for state to change.
          break;
      }
      break;

    default:                                                    // Other cases never happen. Return err if they happen.
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, KAL_MON_RES_BLOCK);
  }

  return (res);
}

/****************************************************************************************************//**
 *                                           USBH_EP_MgmtAlloc()
 *
 * @brief    Allocates endpoint management structure.
 *
 * @return   Pointer to the endpoint management, if successful.
 *           Null pointer,                       otherwise.
 *******************************************************************************************************/
static USBH_EP_MGMT *USBH_EP_MgmtAlloc(void)
{
  RTOS_ERR     err_lib;
  USBH_EP_MGMT *p_ep_mgmt;

  p_ep_mgmt = (USBH_EP_MGMT *)Mem_DynPoolBlkGet(&USBH_Ptr->EP_MgmtPool,
                                                &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating endpoint management structure -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_EP_ALLOC)));
  }

  (void)&err_lib;

  return (p_ep_mgmt);
}

/****************************************************************************************************//**
 *                                       USBH_EP_MgmtAllocCallback()
 *
 * @brief    Function called when a new Endpoint management is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @param    p_arg   Pointer to the argument passed at pool creation.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_EP_MgmtAllocCallback(MEM_DYN_POOL *p_pool,
                                             MEM_SEG      *p_seg,
                                             void         *p_blk,
                                             void         *p_arg)
{
  RTOS_ERR     err_kal;
  USBH_EP_MGMT *p_ep_mgmt;

  (void)&p_pool;
  (void)&p_seg;
  (void)&p_arg;

  p_ep_mgmt = (USBH_EP_MGMT *)p_blk;

  p_ep_mgmt->MonHandle = KAL_MonCreate("USBH - EP Mgmt Mon",
                                       DEF_NULL,
                                       p_ep_mgmt,
                                       DEF_NULL,
                                       &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
    LOG_ERR(("Creating monitor from USBH_EP_MgmtAllocCallback() -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
    return (DEF_FAIL);
  }

  p_ep_mgmt->Handle = USBH_EP_HANDLE_INVALID;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_EP_MgmtFree()
 *
 * @brief    Frees endpoint management structure.
 *
 * @param    p_ep_mgmt   Pointer to the endpoint management structure.
 *******************************************************************************************************/
static void USBH_EP_MgmtFree(USBH_EP_MGMT *p_ep_mgmt)
{
  RTOS_ERR err_lib;

  Mem_DynPoolBlkFree(&USBH_Ptr->EP_MgmtPool,
                     p_ep_mgmt,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing endpoint management structure -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  (void)&err_lib;
}

/****************************************************************************************************//**
 *                                           USBH_EP_URB_Abort()
 *
 * @brief    Aborts all transfer(s) queued on this endpoint.
 *
 * @param    p_ep_mgmt   Pointer to the endpoint management.
 *
 * @param    ---------   Argument validated by caller.
 *
 * @note     (1) Endpoint must be acquired by caller.
 *
 * @note     (2) This function will abort all the transfers (URB) currently Q-ed on this endpoint.
 *               This function will NOT free the aborted URB or complete sync/async transfers. The
 *               USBH_EP_URB_ListAbortedFree() must be called once the endpoint is unlocked.
 *
 * @note     (3) The URBs are aborted in a Last In First Out (LIFO) order. Aborting URBs in a First In
 *               First Out (FIFO) order could cause the HCD to schedule the next URB in the queue
 *               between each call to URB_Abort().
 *******************************************************************************************************/
static void USBH_EP_URB_Abort(USBH_EP_MGMT *p_ep_mgmt)
{
  if (p_ep_mgmt->URB_Cnt != 0u) {
    USBH_HC_DRV         *p_hc_drv = &p_ep_mgmt->DevPtr->HC_Ptr->Drv;
    USBH_URB_MGMT       *p_urb_mgmt_aborted_head = p_ep_mgmt->URB_HeadPtr;
    USBH_URB_MGMT       *p_urb_mgmt_to_abort = p_ep_mgmt->URB_TailPtr;
    USBH_URB_MGMT       *p_urb_mgmt_prev = DEF_NULL;
    USBH_HCD_URB_PARAMS hcd_urb_params;

    if (p_ep_mgmt->URB_HeadPtr != p_ep_mgmt->URB_TailPtr) {
      p_urb_mgmt_prev = p_ep_mgmt->URB_HeadPtr;

      while ((p_urb_mgmt_prev->NextPtr != DEF_NULL)
             && (p_urb_mgmt_prev->NextPtr != p_ep_mgmt->URB_TailPtr)) {
        p_urb_mgmt_prev = p_urb_mgmt_prev->NextPtr;
      }
    }

    while (p_urb_mgmt_to_abort != DEF_NULL) {                   // Abort URBs in LIFO order. See note (3).
      USBH_DBG_EP_STATS_INC(p_ep_mgmt->Handle, URB_AbortCnt);

      hcd_urb_params.BufPtr = p_urb_mgmt_to_abort->BufPtr;
      hcd_urb_params.BufLen = p_urb_mgmt_to_abort->BufLen;
      hcd_urb_params.Token = p_urb_mgmt_to_abort->Token;

      p_hc_drv->API_Ptr->URB_Abort(p_hc_drv,                    // Abort every URB in the HCD.
                                   p_ep_mgmt->HCD_EP_DataPtr,
                                   p_urb_mgmt_to_abort->HCD_URB_DataPtr,
                                   &hcd_urb_params);

      p_urb_mgmt_to_abort = p_ep_mgmt->URB_HeadPtr;             // Retrieve previous URB. See note (3).

      if (p_urb_mgmt_prev == DEF_NULL) {
        break;
      } else if (p_urb_mgmt_prev == p_ep_mgmt->URB_HeadPtr) {
        p_urb_mgmt_to_abort = p_urb_mgmt_prev;
        p_urb_mgmt_prev = DEF_NULL;
      } else {
        while (p_urb_mgmt_to_abort->NextPtr != p_urb_mgmt_prev) {
          p_urb_mgmt_to_abort = p_urb_mgmt_to_abort->NextPtr;
        }
        p_urb_mgmt_prev = p_urb_mgmt_to_abort;
        p_urb_mgmt_to_abort = p_urb_mgmt_to_abort->NextPtr;
      }
    }

    p_ep_mgmt->URB_Cnt = 0u;
    p_ep_mgmt->URB_HeadPtr = DEF_NULL;
    p_ep_mgmt->URB_TailPtr = DEF_NULL;

    if (p_urb_mgmt_aborted_head != DEF_NULL) {
      USBH_DEV_HANDLE  dev_handle = p_ep_mgmt->DevPtr->Handle;
      USBH_EP_HANDLE   ep_handle = p_ep_mgmt->Handle;
      USBH_ASYNC_EVENT *p_async_event;
      USBH_ASYNC_EVENT *p_async_event_prev = DEF_NULL;
      CORE_DECLARE_IRQ_STATE;

      CORE_ENTER_ATOMIC();
      p_async_event = USBH_Ptr->AsyncEventQHeadPtr;

      while (p_async_event != DEF_NULL) {                       // Browse async event list.
                                                                // If EP already had a xfer done but not cmpl, ...
                                                                // remove it from async list.
        if ((p_async_event->Type == USBH_ASYNC_EVENT_TYPE_URB_COMPLETE)
            && (p_async_event->DevHandle == dev_handle)
            && (p_async_event->EP_Handle == ep_handle)) {
          USBH_ASYNC_EVENT *p_async_event_next = p_async_event->NextPtr;

          USBH_DBG_EP_STATS_INC(p_ep_mgmt->Handle, URB_AbortEventFound);
          if (p_async_event != USBH_Ptr->AsyncEventQHeadPtr) {
            p_async_event_prev->NextPtr = p_async_event_next;
            if (p_async_event == USBH_Ptr->AsyncEventQTailPtr) {
              USBH_Ptr->AsyncEventQTailPtr = p_async_event_prev;
            }
          } else {                                              // Currently checking head event.
            USBH_Ptr->AsyncEventQHeadPtr = p_async_event_next;
          }
          USBH_AsyncEventFree(p_async_event);

          p_async_event = p_async_event_next;
        } else {
          p_async_event_prev = p_async_event;
          p_async_event = p_async_event->NextPtr;
        }
      }
      CORE_EXIT_ATOMIC();

      //                                                           Post event to async task asking to free aborted URBs.
      USBH_AsyncEventPost(USBH_ASYNC_EVENT_TYPE_URB_LIST_FREE,
                          p_ep_mgmt->DevPtr->Handle,
                          p_ep_mgmt->Handle,
                          (void *)p_urb_mgmt_aborted_head);
    }
  }
}

/****************************************************************************************************//**
 *                                               USBH_URB_Submit()
 *
 * @brief    Perform asynchronous transfer on endpoint.
 *
 * @param    p_dev           Pointer to the device.
 *
 * @param    ep_handle       Handle to endpoint on which to transfer.
 *
 * @param    ep_type         Type of the endpoint to use.
 *
 * @param    p_buf           Pointer to the buffer that contains data to transmit/will receive data.
 *
 * @param    buf_len         Buffer length in octets.
 *
 * @param    token           USB token to issue.
 *
 * @param    timeout         Timeout, in milliseconds.
 *
 * @param    frm_start       Frame number of first transfer (for Isochronous endpoint only).
 *
 * @param    frm_cnt         Number of frame of transfer (for Isochronous endpoint only).
 *
 * @param    async_fnct      Function that will be called upon completion of transfer.
 *
 * @param    p_async_arg     Parameter that will be passed to async_fnct.
 *
 * @param    p_err           Variable that will receive the return error code from this function.
 *
 * @note     (1) Device MUST be acquired by the caller of this function.
 *******************************************************************************************************/
static void USBH_URB_Submit(USBH_DEV        *p_dev,
                            USBH_EP_HANDLE  ep_handle,
                            CPU_INT08U      ep_type,
                            CPU_INT08U      *p_buf,
                            CPU_INT32U      buf_len,
                            USBH_TOKEN      token,
                            CPU_INT32U      timeout,
#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
                            CPU_INT16U      frm_start,
                            CPU_INT08U      frm_cnt,
#endif
                            USBH_ASYNC_FNCT async_fnct,
                            void            *p_async_arg,
                            RTOS_ERR        *p_err)
{
  USBH_HC_DRV         *p_hc_drv;
  USBH_URB_MGMT       *p_urb_mgmt;
  USBH_EP_MGMT        *p_ep_mgmt;
  USBH_HCD_URB_PARAMS hcd_urb_params;
  RTOS_ERR            local_err;

  USBH_DBG_EP_STATS_INC(ep_handle, URB_SubmitCnt);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL)
                           || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

  p_urb_mgmt = (USBH_URB_MGMT *)Mem_DynPoolBlkGet(&USBH_Ptr->URB_MgmtPool,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_URB_ALLOC);
    return;
  }

  p_ep_mgmt = USBH_EP_MgmtStateWaitSwitch(p_dev,
                                          ep_handle,
                                          USBH_EP_STATE_PREPARING,
                                          timeout,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_free_urb;
  }

  //                                                               For ctrl EP, this test must be done after locking ...
  //                                                               the EP, if an abort/close has happened between two stages.
  if (ep_type == USBH_EP_TYPE_CTRL) {
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();                                        // CS needed because this flag is set in mon call.
    if (USBH_DEV_HANDLE_IS_CLOSING(p_dev->Handle) == DEF_YES) {
      //                                                           Return err of dev is being closed.
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      goto err_restore;
    }
    CORE_EXIT_ATOMIC();
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    USBH_EP *p_ep = p_ep_mgmt->EP_Ptr;

    if (p_ep != DEF_NULL) {                                     // Check attributes if EP is not ctrl EP.
      if (USBH_EP_TYPE_GET(p_ep->Attrib) != ep_type) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
        goto err_restore;
      }

      if ((token == USBH_TOKEN_IN)
          && (USBH_EP_IS_IN(p_ep->Addr) == DEF_NO)
          && (USBH_EP_TYPE_GET(p_ep->Attrib) != USBH_EP_TYPE_CTRL)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
        goto err_restore;
      }

      if ((token == USBH_TOKEN_OUT)
          && (USBH_EP_IS_IN(p_ep->Addr) == DEF_YES)
          && (USBH_EP_TYPE_GET(p_ep->Attrib) != USBH_EP_TYPE_CTRL)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
        goto err_restore;
      }
    }
  }
#else
  (void)&ep_type;
#endif

  p_urb_mgmt->AsyncFnct = async_fnct;
  p_urb_mgmt->AsyncFnctArgPtr = p_async_arg;
  p_urb_mgmt->BufPtr = p_buf;
  p_urb_mgmt->BufLen = buf_len;
  p_urb_mgmt->Token = token;
  p_urb_mgmt->NextPtr = DEF_NULL;
  p_urb_mgmt->HCD_URB_DataPtr = DEF_NULL;

  hcd_urb_params.BufPtr = p_buf;
  hcd_urb_params.BufLen = buf_len;
  hcd_urb_params.Token = token;

#if (USBH_CFG_ISOC_EN == DEF_ENABLED)
  hcd_urb_params.FrmCnt = frm_cnt;
  hcd_urb_params.FrmStart = frm_start;
  p_urb_mgmt->FrmCnt = frm_cnt;
  p_urb_mgmt->FrmStart = frm_start;
#endif

  p_hc_drv = &p_dev->HC_Ptr->Drv;
  p_hc_drv->API_Ptr->URB_Submit(p_hc_drv,
                                p_ep_mgmt->HCD_EP_DataPtr,
                                &p_urb_mgmt->HCD_URB_DataPtr,
                                &hcd_urb_params,
                                &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, local_err);
    goto err_restore;
  }

  if (p_ep_mgmt->URB_Cnt == 0u) {                               // Insert URB to EP's list.
    p_ep_mgmt->URB_HeadPtr = p_urb_mgmt;
  } else {
    p_ep_mgmt->URB_TailPtr->NextPtr = p_urb_mgmt;
  }
  p_ep_mgmt->URB_TailPtr = p_urb_mgmt;
  p_ep_mgmt->URB_Cnt++;

  USBH_EP_MgmtStateSet(p_ep_mgmt,
                       USBH_EP_STATE_QUEUED,
                       p_err);

  USBH_DBG_EP_STATS_INC(ep_handle, URB_SubmitSuccessCnt);

  return;

err_restore:
  {
    USBH_EP_STATE state;

    //                                                             In case of err, reset EP in correct state.
    state = (p_ep_mgmt->URB_Cnt == 0u) ? USBH_EP_STATE_READY : USBH_EP_STATE_QUEUED;

    USBH_EP_MgmtStateSet(p_ep_mgmt,
                         state,
                         &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error resetting state to previous state after failed URB_Submit() -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
  }

err_free_urb:
  Mem_DynPoolBlkFree(&USBH_Ptr->URB_MgmtPool,
                     (void *)p_urb_mgmt,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing URB mgmt struct -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }

  return;
}

/****************************************************************************************************//**
 *                                       USBH_URB_MgmtAllocCallback()
 *
 * @brief    Function called when a new USB Request Block (URB) is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @param    p_arg   Pointer to the argument passed at pool creation. Unused in this case.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_URB_MgmtAllocCallback(MEM_DYN_POOL *p_pool,
                                              MEM_SEG      *p_seg,
                                              void         *p_blk,
                                              void         *p_arg)
{
  RTOS_ERR         err_lib;
  USBH_ASYNC_EVENT *p_async_event;
  CORE_DECLARE_IRQ_STATE;

  (void)&p_pool;
  (void)&p_blk;
  (void)&p_arg;

  //                                                               Alloc new async event.
  p_async_event = (USBH_ASYNC_EVENT *)Mem_SegAlloc("USBH - Async event",
                                                   p_seg,
                                                   sizeof(USBH_ASYNC_EVENT),
                                                   &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating async event from USBH_URB_MgmtAllocCallback() -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    return (DEF_FAIL);
  }

  //                                                               Add async event to free list.
  CORE_ENTER_ATOMIC();
  USBH_AsyncEventFree(p_async_event);
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_DbgEP_StatsStateSwitch()
 *
 * @brief    Function used to keep track of stats of changes in endpoints states.
 *
 * @param    p_ep_mgmt   Pointer to the USBH_EP_MGMT structure of endpoint to track.
 *
 * @param    next        Next state of the endpoint.
 *******************************************************************************************************/

#if (USBH_DBG_EP_STATS_EN == DEF_ENABLED)
static void USBH_DbgEP_StatsStateSwitch(USBH_EP_MGMT  *p_ep_mgmt,
                                        USBH_EP_STATE next)
{
  USBH_DBG_EP_STATE_SWITCH *p_switch;
  USBH_EP_HANDLE           ep_handle = p_ep_mgmt->Handle;
  USBH_EP_STATE            prev = p_ep_mgmt->State;

  if (USBH_EP_HANDLE_VALIDATE(p_ep_mgmt, ep_handle) == DEF_FAIL) {
    //                                                             Early return in case handle used is invalid.
    return;
  }

  switch (next) {
    case USBH_EP_STATE_NONE:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToNone;
      break;
    case USBH_EP_STATE_READY:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToReady;
      break;
    case USBH_EP_STATE_PREPARING:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToPreparing;
      break;
    case USBH_EP_STATE_QUEUED:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToQueued;
      break;
    case USBH_EP_STATE_COMPLETING:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToCompleting;
      break;
    case USBH_EP_STATE_ABORTING_CLOSING:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToAbortingClosing;
      break;
    case USBH_EP_STATE_ABORT_CLOSE_CMPL:
      p_switch = &USBH_DbgEP_Stats[USBH_HANDLE_EP_PHY_NBR_GET(ep_handle)].StateSwitch.ToAbortCloseCmpl;
      break;
    default:
      USBH_DBG_EP_STATE_MACHINE_TRAP();
  }

  p_switch->TotalCnt++;

  switch (prev) {
    case USBH_EP_STATE_NONE:
      p_switch->FromNone++;
      break;
    case USBH_EP_STATE_READY:
      p_switch->FromReady++;
      break;
    case USBH_EP_STATE_PREPARING:
      p_switch->FromPreparing++;
      break;
    case USBH_EP_STATE_QUEUED:
      p_switch->FromQueued++;
      break;
    case USBH_EP_STATE_COMPLETING:
      p_switch->FromCompleting++;
      break;
    case USBH_EP_STATE_ABORTING_CLOSING:
      p_switch->FromAbortingClosing++;
      break;
    case USBH_EP_STATE_ABORT_CLOSE_CMPL:
      p_switch->FromAbortCloseCmpl++;
      break;
    default:
      USBH_DBG_EP_STATE_MACHINE_TRAP();
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
