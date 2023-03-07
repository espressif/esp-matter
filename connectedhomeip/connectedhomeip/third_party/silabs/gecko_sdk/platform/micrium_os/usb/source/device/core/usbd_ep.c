/***************************************************************************//**
 * @file
 * @brief USB Device Endpoint Operations
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

#if (defined(RTOS_MODULE_USB_DEV_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/source/device/core/usbd_core_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                           (USBD, EP)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_USBD

#define  USBD_EP_ADDR_CTRL_OUT                          0x00u
#define  USBD_EP_ADDR_CTRL_IN                           0x80u

#define  USBD_URB_FLAG_XFER_END                 DEF_BIT_00      // Flag indicating if xfer requires a ZLP to complete.
#define  USBD_URB_FLAG_EXTRA_URB                DEF_BIT_01      // Flag indicating if the URB is an 'extra' URB.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_EP_RxStartAsyncProcess(USBD_DRV   *p_drv,
                                        USBD_EP    *p_ep,
                                        USBD_URB   *p_urb,
                                        CPU_INT08U *p_buf_cur,
                                        CPU_INT32U len,
                                        RTOS_ERR   *p_err);

static void USBD_EP_TxAsyncProcess(USBD_DRV   *p_drv,
                                   USBD_EP    *p_ep,
                                   USBD_URB   *p_urb,
                                   CPU_INT08U *p_buf_cur,
                                   CPU_INT32U len,
                                   RTOS_ERR   *p_err);

static CPU_INT32U USBD_EP_Rx(USBD_DRV        *p_drv,
                             USBD_EP         *p_ep,
                             void            *p_buf,
                             CPU_INT32U      buf_len,
                             USBD_ASYNC_FNCT async_fnct,
                             void            *p_async_arg,
                             CPU_INT16U      timeout_ms,
                             RTOS_ERR        *p_err);

static CPU_INT32U USBD_EP_Tx(USBD_DRV        *p_drv,
                             USBD_EP         *p_ep,
                             void            *p_buf,
                             CPU_INT32U      buf_len,
                             USBD_ASYNC_FNCT async_fnct,
                             void            *p_async_arg,
                             CPU_INT16U      timeout_ms,
                             CPU_BOOLEAN     end,
                             RTOS_ERR        *p_err);

static USBD_URB *USBD_EP_URB_Abort(USBD_DRV *p_drv,
                                   USBD_EP  *p_ep,
                                   RTOS_ERR *p_err);

static USBD_URB *USBD_URB_AsyncCmpl(USBD_EP  *p_ep,
                                    RTOS_ERR err);

static void USBD_URB_AsyncEnd(CPU_INT08U dev_nbr,
                              USBD_EP    *p_ep,
                              USBD_URB   *p_urb_head);

static void USBD_URB_Free(CPU_INT08U dev_nbr,
                          USBD_EP    *p_ep,
                          USBD_URB   *p_urb);

static USBD_URB *USBD_URB_Get(CPU_INT08U dev_nbr,
                              USBD_EP    *p_ep,
                              RTOS_ERR   *p_err);

static void USBD_URB_Queue(USBD_EP  *p_ep,
                           USBD_URB *p_urb);

static void USBD_URB_Dequeue(USBD_EP *p_ep);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       BULK TRANSFER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_BulkRx()
 *
 * @brief    Receives data on Bulk OUT endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that receives the following returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_FAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors returned.
 *
 * @note     (1) Receive buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/
CPU_INT32U USBD_BulkRx(CPU_INT08U dev_nbr,
                       CPU_INT08U ep_addr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT32U     xfer_len;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, BulkRxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }

  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_OUT)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Rx(p_drv,                                  // Call generic EP rx fnct.
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, BulkRxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_BulkRxAsync()
 *
 * @brief    Receives data on Bulk OUT endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Receive buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/
void USBD_BulkRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, BulkRxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_OUT)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Rx(p_drv,                                       // Call generic EP rx fnct.
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, BulkRxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/****************************************************************************************************//**
 *                                               USBD_BulkTx()
 *
 * @brief    Sends data on a Bulk IN endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted (see Note #2).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #3).
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets transmitted, if no errors are returned.
 *           0,                            if any errors returned.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Transmit buffer must be aligned with a single word (minimum).
 *
 * @note     (3) If end-of-transfer is set and transfer length is multiple of maximum packet sizes,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
CPU_INT32U USBD_BulkTx(CPU_INT08U  dev_nbr,
                       CPU_INT08U  ep_addr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT32U     xfer_len;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, BulkTxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_IN)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Tx(p_drv,                                  // Call generic EP tx fnct.
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        end,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, BulkTxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_BulkTxAsync()
 *
 * @brief    Sends data on the Bulk IN endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Transmit buffer must be aligned with a single word (minimum).
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
void USBD_BulkTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      CPU_BOOLEAN     end,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, BulkTxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_BULK)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_IN)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Tx(p_drv,
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   end,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, BulkTxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERRUPT TRANSFER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_IntrRx()
 *
 * @brief    Receives data on the Interrupt OUT endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data (see Note #2).
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors returned.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Receive buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/
CPU_INT32U USBD_IntrRx(CPU_INT08U dev_nbr,
                       CPU_INT08U ep_addr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT32U     xfer_len;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IntrRxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_OUT)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Rx(p_drv,
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IntrRxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_IntrRxAsync()
 *
 * @brief    Receives data on Interrupt OUT endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Receive buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/
void USBD_IntrRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  CPU_INT08U     ep_phy_nbr;
  USBD_DEV_STATE state;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IntrRxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_OUT)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Rx(p_drv,                                       // Call generic EP rx fnct.
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IntrRxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/****************************************************************************************************//**
 *                                               USBD_EP_IntrTx()
 *
 * @brief    Sends data on Interrupt IN endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the buffer of data that will be transmitted (see Note #2).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #3).
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets transmitted, if no errors are returned.
 *           0,                            if any errors returned.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Transmit buffer must be aligned with a single word (minimum).
 *
 * @note     (3) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
CPU_INT32U USBD_IntrTx(CPU_INT08U  dev_nbr,
                       CPU_INT08U  ep_addr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  CPU_INT08U     ep_phy_nbr;
  CPU_INT32U     xfer_len;
  USBD_DEV_STATE state;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IntrTxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_IN)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Tx(p_drv,
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        end,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IntrTxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_IntrTxAsync()
 *
 * @brief    Sends data on the Interrupt IN endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Transmit buffer must be aligned with a single word (minimum).
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
void USBD_IntrTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      CPU_BOOLEAN     end,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  CPU_INT08U     ep_phy_nbr;
  USBD_DEV_STATE state;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IntrTxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_INTR)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_IN)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Tx(p_drv,
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   end,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IntrTxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/****************************************************************************************************//**
 *                                           USBD_IsocRxAsync()
 *
 * @brief    Receive data on an isochronous OUT endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of a receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Receive buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
void USBD_IsocRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IsocRxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_ISOC)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_OUT)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Rx(p_drv,                                       // Call generic EP rx fnct.
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IsocRxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}
#endif

/****************************************************************************************************//**
 *                                           USBD_IsocTxAsync()
 *
 * @brief    Sends data on an isochronous IN endpoint asynchronously.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_EP_INVALID
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Transmit buffer must be aligned with a single word (minimum).
 *******************************************************************************************************/

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
void USBD_IsocTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err)
{
  USBD_EP        *p_ep;
  USBD_DRV       *p_drv;
  USBD_DEV_STATE state;
  CPU_INT08U     ep_phy_nbr;

  USBD_DBG_STATS_DEV_INC(dev_nbr, IsocTxAsyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (state != USBD_DEV_STATE_CONFIGURED) {                     // EP transfers are ONLY allowed in config'd state.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }
  //                                                               Chk EP attrib.
  if (((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_ISOC)
      || ((ep_addr      & USBD_EP_DIR_MASK) != USBD_EP_DIR_IN)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  (void)USBD_EP_Tx(p_drv,
                   p_ep,
                   p_buf,
                   buf_len,
                   async_fnct,
                   p_async_arg,
                   0u,
                   DEF_NO,
                   p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, IsocTxAsyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}
#endif

/****************************************************************************************************//**
 *                                           USBD_CtrlTxStatus()
 *
 * @brief    Handles status stage from host on control (EP0) IN endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_CtrlTxStatus(CPU_INT08U dev_nbr,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err)
{
  USBD_DEV_STATE state;

  USBD_DBG_STATS_DEV_INC(dev_nbr, CtrlTxStatusExecNbr);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((state != USBD_DEV_STATE_DEFAULT)
      && (state != USBD_DEV_STATE_ADDRESSED)
      && (state != USBD_DEV_STATE_CONFIGURED)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  USBD_EP_TxZLP(dev_nbr,
                USBD_EP_ADDR_CTRL_IN,
                timeout_ms,
                p_err);
  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, CtrlTxStatusSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/****************************************************************************************************//**
 *                                               USBD_CtrlRx()
 *
 * @brief    Receives data on the Control OUT endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data.
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors returned.
 *******************************************************************************************************/
CPU_INT32U USBD_CtrlRx(CPU_INT08U dev_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err)
{
  USBD_DRV       *p_drv;
  USBD_EP        *p_ep;
  CPU_INT08U     ep_phy_nbr;
  USBD_DEV_STATE state;
  CPU_INT32U     xfer_len;

  USBD_DBG_STATS_DEV_INC(dev_nbr, CtrlRxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if ((state != USBD_DEV_STATE_DEFAULT)
      && (state != USBD_DEV_STATE_ADDRESSED)
      && (state != USBD_DEV_STATE_CONFIGURED)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_ADDR_CTRL_OUT);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }
  //                                                               Chk EP attrib.
  if ((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_CTRL) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Rx(p_drv,
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, CtrlRxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                               USBD_CtrlTx()
 *
 * @brief    Sends data on the Control IN endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_buf       Pointer to the buffer of data that will be sent.
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of octets transmitted, if no errors are returned.
 *           0,                            if any errors returned.
 *
 * @note     (1) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
CPU_INT32U USBD_CtrlTx(CPU_INT08U  dev_nbr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err)
{
  USBD_DRV       *p_drv;
  USBD_EP        *p_ep;
  CPU_INT08U     ep_phy_nbr;
  USBD_DEV_STATE state;
  CPU_INT32U     xfer_len;

  USBD_DBG_STATS_DEV_INC(dev_nbr, CtrlTxSyncExecNbr);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if ((state != USBD_DEV_STATE_DEFAULT)
      && (state != USBD_DEV_STATE_ADDRESSED)
      && (state != USBD_DEV_STATE_CONFIGURED)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  if (buf_len == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (0u);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_ADDR_CTRL_IN);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return (0u);
  }
  //                                                               Chk EP attrib.
  if ((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_CTRL) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  xfer_len = USBD_EP_Tx(p_drv,
                        p_ep,
                        p_buf,
                        buf_len,
                        DEF_NULL,
                        DEF_NULL,
                        timeout_ms,
                        end,
                        p_err);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, CtrlTxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBD_CtrlRxStatus()
 *
 * @brief    Handles the status stage from the host on a control (EP0) OUT endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_EP_QUEUING
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_CtrlRxStatus(CPU_INT08U dev_nbr,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err)
{
  USBD_DEV_STATE state;

  USBD_DBG_STATS_DEV_INC(dev_nbr, CtrlRxStatusExecNbr);

  state = USBD_DevStateGet(dev_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((state != USBD_DEV_STATE_DEFAULT)
      && (state != USBD_DEV_STATE_ADDRESSED)
      && (state != USBD_DEV_STATE_CONFIGURED)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  USBD_EP_RxZLP(dev_nbr,
                USBD_EP_ADDR_CTRL_OUT,
                timeout_ms,
                p_err);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, CtrlRxStatusSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CONTROL ENDPOINT FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_CtrlOpen()
 *
 * @brief    Opens control endpoints.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    max_pkt_size    Maximum packet size.
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_CtrlOpen(CPU_INT08U dev_nbr,
                   CPU_INT16U max_pkt_size,
                   RTOS_ERR   *p_err)
{
  USBD_DRV *p_drv;
  RTOS_ERR local_err;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBD_EP_Open(p_drv,
               USBD_EP_ADDR_CTRL_IN,
               max_pkt_size,
               USBD_EP_TYPE_CTRL,
               0u,
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_EP_Open(p_drv,
               USBD_EP_ADDR_CTRL_OUT,
               max_pkt_size,
               USBD_EP_TYPE_CTRL,
               0u,
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBD_EP_Close(p_drv, USBD_EP_ADDR_CTRL_IN, &local_err);
    return;
  }
}

/****************************************************************************************************//**
 *                                               USBD_CtrlClose()
 *
 * @brief    Close control endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_CtrlClose(CPU_INT08U dev_nbr,
                    RTOS_ERR   *p_err)
{
  USBD_DRV *p_drv;
  RTOS_ERR err_in;
  RTOS_ERR err_out;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  USBD_EP_Close(p_drv, USBD_EP_ADDR_CTRL_IN, &err_in);
  USBD_EP_Close(p_drv, USBD_EP_ADDR_CTRL_OUT, &err_out);

  if (RTOS_ERR_CODE_GET(err_in) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, err_in);
  } else {
    RTOS_ERR_COPY(*p_err, err_out);
  }
}

/****************************************************************************************************//**
 *                                               USBD_CtrlStall()
 *
 * @brief    Stall control endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_CtrlStall(CPU_INT08U dev_nbr,
                    RTOS_ERR   *p_err)
{
  USBD_EP      *p_ep_out;
  USBD_EP      *p_ep_in;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CPU_INT08U   ep_phy_nbr;
  CPU_BOOLEAN  stall_in;
  CPU_BOOLEAN  stall_out;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_ADDR_CTRL_OUT);
  p_ep_out = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_ADDR_CTRL_IN);
  p_ep_in = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if ((p_ep_out == DEF_NULL)
      || (p_ep_in == DEF_NULL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep_out->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep_in->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep_out->Ix);
    return;
  }

  if ((p_ep_out->State == USBD_EP_STATE_OPEN)
      && (p_ep_in->State == USBD_EP_STATE_OPEN)) {
    p_drv_api = p_drv->API_Ptr;                                 // Get dev drv API struct.
    stall_in = p_drv_api->EP_Stall(p_drv,
                                   USBD_EP_ADDR_CTRL_IN,
                                   DEF_SET);

    stall_out = p_drv_api->EP_Stall(p_drv,
                                    USBD_EP_ADDR_CTRL_OUT,
                                    DEF_SET);

    if ((stall_in == DEF_FAIL)
        || (stall_out == DEF_FAIL)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
  }

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep_in->Ix);

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep_out->Ix);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       GENERAL ENDPOINT FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_EP_Init()
 *
 * @brief    Initialize endpoint structures.
 *
 * @param    p_mem_seg   Pointer to segment from which to allocate memory. Will be allocated from
 *                       general-purpose heap if null.
 *
 * @param    p_qty_cfg   Pointer to USBD configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function.
 *******************************************************************************************************/
void USBD_EP_Init(MEM_SEG      *p_mem_seg,
                  USBD_QTY_CFG *p_qty_cfg,
                  RTOS_ERR     *p_err)
{
  CPU_INT08U ep_ix;
  CPU_INT08U dev_nbr;
  CPU_INT16U urb_ix;
  USBD_EP    *p_ep;
  USBD_URB   *p_urb;

  USBD_Ptr->EP_Tbl = (USBD_EP **)Mem_SegAlloc("USBD - EP table",
                                              p_mem_seg,
                                              sizeof(USBD_EP *) * p_qty_cfg->DevQty,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_Ptr->EP_TblPtrs = (USBD_EP ***)Mem_SegAlloc("USBD - EP pointers table",
                                                   p_mem_seg,
                                                   sizeof(USBD_EP * *) * p_qty_cfg->DevQty,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_Ptr->EP_OpenCtrTbl = (CPU_INT08U *)Mem_SegAlloc("USBD - EP open ctr table",
                                                       p_mem_seg,
                                                       sizeof(CPU_INT08U) * p_qty_cfg->DevQty,
                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_Ptr->EP_OpenBitMapTbl = (CPU_INT32U *)Mem_SegAlloc("USBD - EP bitmap table",
                                                          p_mem_seg,
                                                          sizeof(CPU_INT32U) * p_qty_cfg->DevQty,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_Ptr->URB_Tbl = (USBD_URB **)Mem_SegAlloc("USBD - URB table",
                                                p_mem_seg,
                                                sizeof(USBD_URB *) * p_qty_cfg->DevQty,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_Ptr->URB_TblPtr = (USBD_URB **)Mem_SegAlloc("USBD - URB pointer table",
                                                   p_mem_seg,
                                                   sizeof(USBD_URB *) * p_qty_cfg->DevQty,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  USBD_Ptr->URB_ExtraAvailCntTbl = (CPU_INT08U *)Mem_SegAlloc("USBD - URB Extra avail cnt tbl",
                                                              p_mem_seg,
                                                              sizeof(CPU_INT08U) * p_qty_cfg->URB_ExtraQty,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  for (dev_nbr = 0u; dev_nbr < p_qty_cfg->DevQty; dev_nbr++) {
    CPU_SIZE_T urb_qty = p_qty_cfg->EP_OpenQty;

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
    urb_qty += p_qty_cfg->URB_ExtraQty;
#endif

    USBD_Ptr->EP_Tbl[dev_nbr] = (USBD_EP *)Mem_SegAlloc("USBD - EP table",
                                                        p_mem_seg,
                                                        sizeof(USBD_EP) * p_qty_cfg->EP_OpenQty,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_Ptr->EP_TblPtrs[dev_nbr] = (USBD_EP **)Mem_SegAlloc("USBD - EP pointers table",
                                                             p_mem_seg,
                                                             sizeof(USBD_EP *) * USBD_EP_MAX_NBR,
                                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_Ptr->URB_Tbl[dev_nbr] = (USBD_URB *)Mem_SegAlloc("USBD - URB table",
                                                          p_mem_seg,
                                                          sizeof(USBD_URB) * urb_qty,
                                                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
    USBD_Ptr->URB_ExtraAvailCntTbl[dev_nbr] = p_qty_cfg->URB_ExtraQty;
#endif

    for (ep_ix = 0u; ep_ix < USBD_EP_MAX_NBR; ep_ix++) {
      if (ep_ix < p_qty_cfg->EP_OpenQty) {
        p_ep = &USBD_Ptr->EP_Tbl[dev_nbr][ep_ix];
        p_ep->Addr = USBD_EP_ADDR_NONE;
        p_ep->Attrib = DEF_BIT_NONE;
        p_ep->MaxPktSize = 0u;
        p_ep->Interval = 0u;
        p_ep->Ix = 0u;
#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
        p_ep->URB_MainAvail = DEF_YES;
#endif
        p_ep->URB_HeadPtr = DEF_NULL;
        p_ep->URB_TailPtr = DEF_NULL;

        USBD_DBG_STATS_EP_RESET(dev_nbr, ep_ix);
      }
      USBD_Ptr->EP_TblPtrs[dev_nbr][ep_ix] = DEF_NULL;
    }

    USBD_Ptr->URB_TblPtr[dev_nbr] = &USBD_Ptr->URB_Tbl[dev_nbr][0u];

    for (urb_ix = 0u; urb_ix < urb_qty; urb_ix++) {
      p_urb = &USBD_Ptr->URB_Tbl[dev_nbr][urb_ix];
      p_urb->BufPtr = DEF_NULL;
      p_urb->BufLen = 0u;
      p_urb->XferLen = 0u;
      p_urb->NextXferLen = 0u;
      p_urb->Flags = 0u;
      p_urb->State = USBD_URB_STATE_IDLE;
      p_urb->AsyncFnct = DEF_NULL;
      p_urb->AsyncFnctArg = DEF_NULL;
      RTOS_ERR_SET(p_urb->Err, RTOS_ERR_NONE);
      if (urb_ix < (urb_qty - 1u)) {
        p_urb->NextPtr = &USBD_Ptr->URB_Tbl[dev_nbr][urb_ix + 1];
      } else {
        p_urb->NextPtr = DEF_NULL;
      }
    }

    USBD_Ptr->EP_OpenCtrTbl[dev_nbr] = 0u;
    USBD_Ptr->EP_OpenBitMapTbl[dev_nbr] = DEF_INT_32_MASK;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_EP_XferAsyncProcess()
 *
 * @brief    Read/write data asynchronously from/to non-control endpoints.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ------      Argument checked by caller.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    xfer_err    Error Code returned by the Core Event.
 *                       Argument changed for 'xfer_err'.
 *
 * @note     (1) A USB device driver can notify the core about the Tx transfer completion using
 *               USBD_EP_TxCmpl() or USBD_EP_TxCmplExt(). The latter function allows to report a
 *               specific error code whereas USBD_EP_TxCmpl() reports only a successful transfer.
 *               In the case of an asynchronous transfer, the error code reported by the USB device
 *               driver must be tested. In case of an error condition, the asynchronous transfer
 *               is marked as completed and the associated callback is called by the core task.
 *
 * @note     (2) This condition covers also the case where the transfer length is multiple of the
 *               maximum packet size. In that case, host sends a zero-length packet considered as
 *               a short packet for the condition.
 *******************************************************************************************************/
void USBD_EP_XferAsyncProcess(USBD_DRV   *p_drv,
                              CPU_INT08U ep_addr,
                              RTOS_ERR   xfer_err)
{
  CPU_INT08U   ep_phy_nbr;
  USBD_EP      *p_ep;
  USBD_DRV_API *p_drv_api;
  CPU_BOOLEAN  ep_dir_in;
  RTOS_ERR     local_err;
  USBD_URB     *p_urb;
  USBD_URB     *p_urb_cmpl;
  CPU_INT08U   *p_buf_cur;
  CPU_INT32U   xfer_len;
  CPU_INT32U   xfer_rem;
  RTOS_ERR     err;

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[p_drv->DevNbr][ep_phy_nbr];
  if (p_ep == DEF_NULL) {
    return;
  }

  p_drv_api = p_drv->API_Ptr;
  ep_dir_in = USBD_EP_IS_IN(p_ep->Addr);

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->XferState == USBD_XFER_STATE_NONE) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    return;
  }

  p_urb = p_ep->URB_HeadPtr;
  if (p_urb == DEF_NULL) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    LOG_DBG(("USBD_EP_Process(): no URB to process for endpoint addr: 0x", (X)ep_addr));
    return;
  }

  if ((p_urb->State == USBD_URB_STATE_IDLE)
      || (p_urb->State == USBD_URB_STATE_XFER_SYNC)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    LOG_ERR(("USBD_EP_Process(): incorrect URB state for endpoint addr: 0x", (X)ep_addr));
    return;
  }

  p_urb_cmpl = DEF_NULL;
  if (RTOS_ERR_CODE_GET(xfer_err) == RTOS_ERR_NONE) {           // See Note #1.
    xfer_rem = p_urb->BufLen - p_urb->XferLen;
    p_buf_cur = &p_urb->BufPtr[p_urb->XferLen];

    if (ep_dir_in == DEF_YES) {                                 // ------------------- IN TRANSFER --------------------
      if (xfer_rem > 0u) {                                      // Another transaction must be done.
        USBD_EP_TxAsyncProcess(p_drv,
                               p_ep,
                               p_urb,
                               p_buf_cur,
                               xfer_rem,
                               &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, local_err);
        }
      } else if ((DEF_BIT_IS_SET(p_urb->Flags, USBD_URB_FLAG_XFER_END) == DEF_YES)
                 && (p_urb->XferLen % p_ep->MaxPktSize == 0u)
                 && (p_urb->XferLen != 0u)) {
        //                                                         $$$$ This case should be tested more thoroughly.
        //                                                         Send ZLP if needed, at end of xfer.
        DEF_BIT_CLR(p_urb->Flags, (CPU_INT08U)USBD_URB_FLAG_XFER_END);

        USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_Nbr);

        p_drv_api->EP_TxZLP(p_drv, p_ep->Addr, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, local_err);
        }
        USBD_DBG_STATS_EP_INC_IF_TRUE(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_Nbr, (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE));
      } else {                                                  // Xfer is completed.
        RTOS_ERR_SET(err, RTOS_ERR_NONE);
        p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, err);
      }
    } else {                                                    // ------------------- OUT TRANSFER -------------------
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxNbr);

      xfer_len = p_drv_api->EP_Rx(p_drv,
                                  p_ep->Addr,
                                  p_buf_cur,
                                  p_urb->NextXferLen,
                                  &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, local_err);
      } else {
        USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxSuccessNbr);

        p_urb->XferLen += xfer_len;

        if ((xfer_len == 0u)                                    // Rx'd a ZLP.
            || (xfer_len < p_urb->NextXferLen)                  // Rx'd a short pkt (see Note #2).
            || (p_urb->XferLen == p_urb->BufLen)) {             // All bytes rx'd.
                                                                // Xfer finished.
          RTOS_ERR_SET(err, RTOS_ERR_NONE);
          p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, err);
        } else {
          p_buf_cur = &p_urb->BufPtr[p_urb->XferLen];           // Xfer not finished.
          xfer_len = p_urb->BufLen - p_urb->XferLen;

          USBD_EP_RxStartAsyncProcess(p_drv,
                                      p_ep,
                                      p_urb,
                                      p_buf_cur,
                                      xfer_len,
                                      &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, local_err);
          }
        }
      }
    }
  } else {
    p_urb_cmpl = USBD_URB_AsyncCmpl(p_ep, xfer_err);
  }

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  if (p_urb_cmpl != DEF_NULL) {
    USBD_URB_AsyncEnd(p_drv->DevNbr, p_ep, p_urb_cmpl);         // Execute callback and free aborted URB(s), if any.
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_Open()
 *
 * @brief    Open non-control endpoint.
 *
 * @param    p_drv           Pointer to device driver structure.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    max_pkt_size    Maximum packet size.
 *
 * @param    attrib          Endpoint attributes.
 *
 * @param    interval        Endpoint polling interval.
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_EP_Open(USBD_DRV   *p_drv,
                  CPU_INT08U ep_addr,
                  CPU_INT16U max_pkt_size,
                  CPU_INT08U attrib,
                  CPU_INT08U interval,
                  RTOS_ERR   *p_err)
{
  USBD_DRV_API *p_drv_api;
  USBD_EP      *p_ep;
  CPU_INT08U   ep_bit;
  CPU_INT08U   ep_ix;
  CPU_INT08U   ep_phy_nbr;
  CPU_INT08U   dev_nbr;
  CPU_INT08U   transaction_frame;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  dev_nbr = p_drv->DevNbr;
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep != DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  CORE_ENTER_ATOMIC();
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (USBD_Ptr->EP_OpenCtrTbl[dev_nbr] >= USBD_Ptr->EP_OpenQty) {
    CORE_EXIT_ATOMIC();
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_EP_NONE_AVAIL,; );
  }
#endif

  ep_bit = (CPU_INT08U)(USBD_EP_MAX_NBR - 1u - CPU_CntLeadZeros32(USBD_Ptr->EP_OpenBitMapTbl[dev_nbr]));
  DEF_BIT_CLR(USBD_Ptr->EP_OpenBitMapTbl[dev_nbr], DEF_BIT32(ep_bit));
  USBD_Ptr->EP_OpenCtrTbl[dev_nbr]++;
  CORE_EXIT_ATOMIC();

  ep_ix = USBD_EP_MAX_NBR - 1u - ep_bit;

  USBD_OS_EP_SignalCreate(p_drv->DevNbr, ep_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_clean;
  }

  USBD_OS_EP_LockCreate(p_drv->DevNbr, ep_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_signal_clean;
  }

  transaction_frame = (max_pkt_size >> 11u) & 0x3;
  transaction_frame += 1u;

  p_drv_api = p_drv->API_Ptr;
  p_drv_api->EP_Open(p_drv,                                     // Open EP in dev drv.
                     ep_addr,
                     attrib & USBD_EP_TYPE_MASK,
                     max_pkt_size & 0x7FF,                      // Mask out transactions per microframe.
                     transaction_frame,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_lock_signal_clean;
  }

  p_ep = &USBD_Ptr->EP_Tbl[dev_nbr][ep_ix];

  CORE_ENTER_ATOMIC();
  p_ep->Addr = ep_addr;
  p_ep->Attrib = attrib;
  p_ep->MaxPktSize = max_pkt_size;
  p_ep->Interval = interval;
  p_ep->State = USBD_EP_STATE_OPEN;
  p_ep->XferState = USBD_XFER_STATE_NONE;
  p_ep->Ix = ep_ix;

  USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr] = p_ep;
  CORE_EXIT_ATOMIC();

#if (USBD_CFG_DBG_STATS_EN == DEF_ENABLED)
  //                                                               Reset stats only if EP address changed.
  if (ep_addr != USBD_DBG_STATS_EP_GET(dev_nbr, ep_ix, Addr)) {
    USBD_DBG_STATS_EP_RESET(dev_nbr, ep_ix);
    USBD_DBG_STATS_EP_SET_ADDR(dev_nbr, ep_ix, ep_addr);
  }
#endif
  USBD_DBG_STATS_EP_INC(dev_nbr, ep_ix, EP_OpenNbr);

  LOG_DBG(("EP Open for endpoint addr: 0x", (X)ep_addr));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return;

end_lock_signal_clean:
  USBD_OS_EP_LockDel(p_drv->DevNbr, ep_ix);

end_signal_clean:
  USBD_OS_EP_SignalDel(p_drv->DevNbr, ep_ix);

end_clean:
  CORE_ENTER_ATOMIC();
  DEF_BIT_SET(USBD_Ptr->EP_OpenBitMapTbl[dev_nbr], DEF_BIT32(ep_bit));
  USBD_Ptr->EP_OpenCtrTbl[dev_nbr] -= 1u;
  CORE_EXIT_ATOMIC();

  LOG_ERR(("EP Open for endpoint addr:", (X)ep_addr, " failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

  return;
}

/****************************************************************************************************//**
 *                                           USBD_EP_MaxPktSizeGet()
 *
 * @brief    Retrieves the endpoint maximum packet size.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_EP_INVALID
 *
 * @return   Maximum packet size, if no errors are returned.
 *           0,                   if any errors returned.
 *******************************************************************************************************/
CPU_INT16U USBD_EP_MaxPktSizeGet(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 RTOS_ERR   *p_err)
{
  USBD_EP    *p_ep;
  USBD_DRV   *p_drv;
  CPU_INT08U ep_phy_nbr;
  CPU_INT16U max_pkt_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);

  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];
  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (0u);
  }

  max_pkt_len = p_ep->MaxPktSize & 0x7FF;                       // Mask out transactions per microframe.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (max_pkt_len);
}

/****************************************************************************************************//**
 *                                           USBD_EP_MaxNbrOpenGet()
 *
 * @brief    Retrieves the maximum number of opened endpoints.
 *
 * @param    dev_nbr     Device number.
 *
 * @return   Maximum number of opened endpoints, if no errors are returned.
 *           0,                                  if any errors returned.
 *******************************************************************************************************/
CPU_INT08U USBD_EP_MaxNbrOpenGet(CPU_INT08U dev_nbr)
{
  USBD_DRV   *p_drv;
  CPU_INT08U nbr_open;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get drv struct.
  if (p_drv == DEF_NULL) {
    return (0u);
  }

  nbr_open = USBD_Ptr->EP_OpenCtrTbl[dev_nbr];

  return (nbr_open);
}

/****************************************************************************************************//**
 *                                               USBD_EP_Abort()
 *
 * @brief    Aborts I/O transfer on the endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NONE_WAITING
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_EP_Abort(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   RTOS_ERR   *p_err)
{
  USBD_EP    *p_ep;
  USBD_DRV   *p_drv;
  CPU_INT08U ep_phy_nbr;
  USBD_URB   *p_urb_head_aborted;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, EP_AbortExecNbr);

  if ((p_ep->State != USBD_EP_STATE_OPEN)
      && (p_ep->State != USBD_EP_STATE_STALL)) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    return;
  }

  p_urb_head_aborted = USBD_EP_URB_Abort(p_drv,                 // Abort xfers in progress, keep ptr to head of list.
                                         p_ep,
                                         p_err);

  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, p_ep->Ix, EP_AbortSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  if (p_urb_head_aborted != DEF_NULL) {
    USBD_URB_AsyncEnd(dev_nbr, p_ep, p_urb_head_aborted);       // Execute callback and free aborted URB(s), if any.
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_Close()
 *
 * @brief    Close non-control endpoint.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBD_EP_Close(USBD_DRV   *p_drv,
                   CPU_INT08U ep_addr,
                   RTOS_ERR   *p_err)
{
  USBD_EP    *p_ep;
  CPU_INT08U dev_nbr;
  CPU_INT08U ep_bit;
  CPU_INT08U ep_phy_nbr;
  USBD_URB   *p_urb_head_aborted;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  dev_nbr = p_drv->DevNbr;
  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ep->State == USBD_EP_STATE_CLOSE) {
    USBD_OS_EP_LockRelease(p_drv->DevNbr,
                           p_ep->Ix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, EP_CloseExecNbr);

  ep_bit = USBD_EP_MAX_NBR - 1u - p_ep->Ix;

  p_urb_head_aborted = USBD_EP_URB_Abort(p_drv,                 // Abort xfers in progress, keep ptr to head of list.
                                         p_ep,
                                         p_err);

  p_ep->State = USBD_EP_STATE_CLOSE;

  CORE_ENTER_ATOMIC();
  DEF_BIT_SET(USBD_Ptr->EP_OpenBitMapTbl[dev_nbr], DEF_BIT32(ep_bit));
  USBD_Ptr->EP_OpenCtrTbl[dev_nbr]--;
  CORE_EXIT_ATOMIC();

  p_drv->API_Ptr->EP_Close(p_drv, ep_addr);

  p_ep->XferState = USBD_XFER_STATE_NONE;
  USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr] = DEF_NULL;

  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, p_ep->Ix, EP_CloseSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  USBD_OS_EP_SignalDel(p_drv->DevNbr, p_ep->Ix);
  USBD_OS_EP_LockDel(p_drv->DevNbr, p_ep->Ix);

  if (p_urb_head_aborted != DEF_NULL) {
    USBD_URB_AsyncEnd(dev_nbr, p_ep, p_urb_head_aborted);       // Execute callback and free aborted URB(s), if any.
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_Stall()
 *
 * @brief    Stalls the non-control endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    state       Endpoint stall state.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NONE_WAITING
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_FAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void USBD_EP_Stall(CPU_INT08U  dev_nbr,
                   CPU_INT08U  ep_addr,
                   CPU_BOOLEAN state,
                   RTOS_ERR    *p_err)
{
  USBD_DRV    *p_drv;
  CPU_INT08U  ep_phy_nbr;
  USBD_EP     *p_ep;
  USBD_URB    *p_urb_head_aborted;
  CPU_BOOLEAN valid;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_urb_head_aborted = DEF_NULL;

  switch (p_ep->State) {
    case USBD_EP_STATE_OPEN:
      if (state == DEF_SET) {
        p_urb_head_aborted = USBD_EP_URB_Abort(p_drv,           // Abort xfers in progress, keep ptr to head of list.
                                               p_ep,
                                               p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          break;
        }
        p_ep->State = USBD_EP_STATE_STALL;
      }

      valid = p_drv->API_Ptr->EP_Stall(p_drv, p_ep->Addr, state);
      if (valid == DEF_FAIL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      }
      break;

    case USBD_EP_STATE_STALL:
      if (state == DEF_CLR) {
        valid = p_drv->API_Ptr->EP_Stall(p_drv, p_ep->Addr, DEF_CLR);
        if (valid == DEF_FAIL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
        } else {
          p_ep->State = USBD_EP_STATE_OPEN;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        }
      }
      break;

    case USBD_EP_STATE_CLOSE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
      break;
  }

  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);

  if (p_urb_head_aborted != DEF_NULL) {
    USBD_URB_AsyncEnd(dev_nbr, p_ep, p_urb_head_aborted);       // Execute callback and free aborted URB(s), if any.
  }
}

/****************************************************************************************************//**
 *                                           USBD_EP_IsStalled()
 *
 * @brief    Gets the stall status of a non-control endpoint.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_EP_INVALID
 *
 * @return   DEF_TRUE,  if endpoint is stalled.
 *           DEF_FALSE, if endpoint is not stalled.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_EP_IsStalled(CPU_INT08U dev_nbr,
                              CPU_INT08U ep_addr,
                              RTOS_ERR   *p_err)
{
  USBD_DRV   *p_drv;
  USBD_EP    *p_ep;
  CPU_INT08U ep_phy_nbr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, DEF_NO);

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return (DEF_NO);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (p_ep->State == USBD_EP_STATE_STALL) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_RxCmpl()
 *
 * @brief    Notify USB stack that packet receive has completed.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *******************************************************************************************************/
void USBD_EP_RxCmpl(USBD_DRV   *p_drv,
                    CPU_INT08U ep_log_nbr)
{
  USBD_EP    *p_ep;
  CPU_INT08U ep_phy_nbr;
  RTOS_ERR   err;

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), err, RTOS_ERR_NULL_PTR,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_LOG_TO_ADDR_OUT(ep_log_nbr));
  p_ep = USBD_Ptr->EP_TblPtrs[p_drv->DevNbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxCmplErrNbr);
    return;
  }

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxCmplNbr);

  if (p_ep->XferState == USBD_XFER_STATE_SYNC) {
    USBD_OS_EP_SignalPost(p_drv->DevNbr, p_ep->Ix, &err);
  } else if ((p_ep->XferState == USBD_XFER_STATE_ASYNC)
             || (p_ep->XferState == USBD_XFER_STATE_ASYNC_PARTIAL)) {
    RTOS_ERR_SET(err, RTOS_ERR_NONE);
    USBD_EventEP(p_drv, p_ep->Addr, err);
  } else {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxCmplErrNbr);
    LOG_ERR(("USBD_EP_RxCmpl(): incorrect XferState for endpoint addr: 0x", (X)p_ep->Addr));
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_TxCmpl()
 *
 * @brief    Notify USB stack that packet transmit has completed.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *******************************************************************************************************/
void USBD_EP_TxCmpl(USBD_DRV   *p_drv,
                    CPU_INT08U ep_log_nbr)
{
  USBD_EP    *p_ep;
  CPU_INT08U ep_phy_nbr;
  RTOS_ERR   err;

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), err, RTOS_ERR_NULL_PTR,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_LOG_TO_ADDR_IN(ep_log_nbr));
  p_ep = USBD_Ptr->EP_TblPtrs[p_drv->DevNbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplErrNbr);
    return;
  }

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplNbr);

  if (p_ep->XferState == USBD_XFER_STATE_SYNC) {
    USBD_OS_EP_SignalPost(p_drv->DevNbr, p_ep->Ix, &err);
  } else if ((p_ep->XferState == USBD_XFER_STATE_ASYNC)
             || (p_ep->XferState == USBD_XFER_STATE_ASYNC_PARTIAL)) {
    RTOS_ERR_SET(err, RTOS_ERR_NONE);
    USBD_EventEP(p_drv, p_ep->Addr, err);
  } else {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplErrNbr);
    LOG_ERR(("USBD_EP_TxCmpl(): incorrect XferState for endpoint addr: 0x", (X)p_ep->Addr));
  }
}

/****************************************************************************************************//**
 *                                           USBD_EP_TxCmplExt()
 *
 * @brief    Notify USB stack that packet transmit has completed (see Note #1).
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *
 * @param    xfer_err    Error code returned by the USB driver.
 *
 * @note     (1) This function is an alternative to the function USBD_EP_TxCmpl() so that a USB device
 *               driver can return to the core an error code upon the Tx transfer completion.
 *******************************************************************************************************/
void USBD_EP_TxCmplExt(USBD_DRV   *p_drv,
                       CPU_INT08U ep_log_nbr,
                       RTOS_ERR   xfer_err)
{
  USBD_EP    *p_ep;
  CPU_INT08U ep_phy_nbr;
  RTOS_ERR   local_err;

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), local_err, RTOS_ERR_NULL_PTR,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(USBD_EP_LOG_TO_ADDR_IN(ep_log_nbr));
  p_ep = USBD_Ptr->EP_TblPtrs[p_drv->DevNbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplErrNbr);
    return;
  }

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplNbr);

  if (p_ep->XferState == USBD_XFER_STATE_SYNC) {
    USBD_OS_EP_SignalAbort(p_drv->DevNbr, p_ep->Ix, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("USBD_EP_TxCmplExt() for endpoint addr: ", (X)p_ep->Addr, " failed with err ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
  } else if ((p_ep->XferState == USBD_XFER_STATE_ASYNC)
             || (p_ep->XferState == USBD_XFER_STATE_ASYNC_PARTIAL)) {
    USBD_EventEP(p_drv, p_ep->Addr, xfer_err);
  } else {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxCmplErrNbr);
    LOG_ERR(("USBD_EP_TxCmplExt(): incorrect XferState for endpoint addr: 0x", (X)p_ep->Addr));
  }
}

/****************************************************************************************************//**
 *                                               USBD_EP_TxZLP()
 *
 * @brief    Send zero-length packet to the host.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) This function should only be called during a synchronous transfer.
 *******************************************************************************************************/
void USBD_EP_TxZLP(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   CPU_INT16U timeout_ms,
                   RTOS_ERR   *p_err)
{
  USBD_EP      *p_ep;
  USBD_URB     *p_urb;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CPU_INT08U   ep_phy_nbr;
  RTOS_ERR     local_err;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, TxZLP_ExecNbr);

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    goto lock_release;
  }
  //                                                               Chk EP attrib.
  if ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    goto lock_release;
  }

  if (p_ep->XferState != USBD_XFER_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto lock_release;
  }

  p_urb = USBD_URB_Get(dev_nbr, p_ep, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto lock_release;
  }

  p_urb->State = USBD_URB_STATE_XFER_SYNC;                      // Only State needs to be set to indicate sync xfer.
  p_ep->XferState = USBD_XFER_STATE_SYNC;                       // Set XferState before submitting xfer.

  USBD_URB_Queue(p_ep, p_urb);

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_Nbr);

  p_drv_api->EP_TxZLP(p_drv, ep_addr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_SuccessNbr);

    USBD_OS_EP_LockRelease(p_drv->DevNbr,                       // Unlock before pending on completion.
                           p_ep->Ix);

    USBD_OS_EP_SignalPend(dev_nbr, p_ep->Ix, timeout_ms, p_err);

    USBD_OS_EP_LockAcquire(p_drv->DevNbr,                       // Re-lock EP after xfer completion.
                           p_ep->Ix,
                           &local_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      p_drv_api->EP_Abort(p_drv, ep_addr);
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxSyncTimeoutErrNbr);
    } else if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      RTOS_ERR_COPY(*p_err, local_err);
    }
  }

  USBD_URB_Dequeue(p_ep);

  USBD_URB_Free(dev_nbr, p_ep, p_urb);

  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, p_ep->Ix, TxZLP_SuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

lock_release:
  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);
}

/****************************************************************************************************//**
 *                                               USBD_EP_RxZLP()
 *
 * @brief    Receive zero-length packet from the host.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) This function should only be called during a synchronous transfer.
 *******************************************************************************************************/
void USBD_EP_RxZLP(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   CPU_INT16U timeout_ms,
                   RTOS_ERR   *p_err)
{
  USBD_EP      *p_ep;
  USBD_URB     *p_urb;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CPU_INT08U   ep_phy_nbr;
  RTOS_ERR     local_err;

  p_drv = USBD_DrvRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
  p_ep = USBD_Ptr->EP_TblPtrs[dev_nbr][ep_phy_nbr];

  if (p_ep == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  USBD_OS_EP_LockAcquire(p_drv->DevNbr,
                         p_ep->Ix,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, RxZLP_ExecNbr);

  if (p_ep->State != USBD_EP_STATE_OPEN) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_EP_STATE);
    goto lock_release;
  }

  if ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    goto lock_release;
  }

  if (p_ep->XferState != USBD_XFER_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto lock_release;
  }

  p_urb = USBD_URB_Get(dev_nbr, p_ep, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto lock_release;
  }

  p_urb->State = USBD_URB_STATE_XFER_SYNC;                      // Only State needs to be set to indicate sync xfer.
  p_ep->XferState = USBD_XFER_STATE_SYNC;                       // Set XferState before submitting xfer.

  USBD_URB_Queue(p_ep, p_urb);

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.
  USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, DrvRxStartNbr);
  (void)p_drv_api->EP_RxStart(p_drv,
                              ep_addr,
                              DEF_NULL,
                              0u,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, DrvRxStartSuccessNbr);

    USBD_OS_EP_LockRelease(p_drv->DevNbr,                       // Unlock before pending on completion.
                           p_ep->Ix);

    USBD_OS_EP_SignalPend(dev_nbr, p_ep->Ix, timeout_ms, p_err);

    USBD_OS_EP_LockAcquire(p_drv->DevNbr,                       // Re-lock EP after xfer completion.
                           p_ep->Ix,
                           &local_err);
    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
        && (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE)) {
      USBD_DBG_STATS_EP_INC(dev_nbr, p_ep->Ix, DrvRxZLP_Nbr);

      p_drv_api->EP_RxZLP(p_drv,
                          ep_addr,
                          p_err);
      USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, p_ep->Ix, DrvRxZLP_SuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));
    } else if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      p_drv_api->EP_Abort(p_drv, ep_addr);
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxSyncTimeoutErrNbr);
    } else if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      RTOS_ERR_COPY(*p_err, local_err);
    }
  }

  USBD_URB_Dequeue(p_ep);

  USBD_URB_Free(dev_nbr, p_ep, p_urb);

  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, p_ep->Ix, RxZLP_SuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

lock_release:
  USBD_OS_EP_LockRelease(p_drv->DevNbr,
                         p_ep->Ix);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBD_EP_RxStartAsyncProcess()
 *
 * @brief    Process driver's asynchronous RxStart operation.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    p_ep        Pointer to endpoint on which data will be received.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    p_buf_cur   Pointer to source buffer to receive data.
 *
 * @param    len         Number of octets to receive.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void USBD_EP_RxStartAsyncProcess(USBD_DRV   *p_drv,
                                        USBD_EP    *p_ep,
                                        USBD_URB   *p_urb,
                                        CPU_INT08U *p_buf_cur,
                                        CPU_INT32U len,
                                        RTOS_ERR   *p_err)
{
  USBD_DRV_API *p_drv_api;
  CORE_DECLARE_IRQ_STATE;

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxStartNbr);
  p_urb->NextXferLen = p_drv_api->EP_RxStart(p_drv,
                                             p_ep->Addr,
                                             p_buf_cur,
                                             len,
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxStartSuccessNbr);

  if (p_urb->NextXferLen != len) {
    CORE_ENTER_ATOMIC();
    p_ep->XferState = USBD_XFER_STATE_ASYNC_PARTIAL;            // Xfer will have to be done in many transactions.
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                           USBD_EP_TxAsyncProcess()
 *
 * @brief    Process driver's asynchronous Tx operation.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    p_ep        Pointer to endpoint on which data will be transmitted.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    p_buf_cur   Pointer to source buffer to transmit data.
 *
 * @param    len         Number of octets to transmit.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void USBD_EP_TxAsyncProcess(USBD_DRV   *p_drv,
                                   USBD_EP    *p_ep,
                                   USBD_URB   *p_urb,
                                   CPU_INT08U *p_buf_cur,
                                   CPU_INT32U len,
                                   RTOS_ERR   *p_err)
{
  USBD_DRV_API *p_drv_api;

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxNbr);

  p_urb->NextXferLen = p_drv_api->EP_Tx(p_drv,
                                        p_ep->Addr,
                                        p_buf_cur,
                                        len,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_urb->NextXferLen == len) {                              // Xfer can be done is a single transaction.
    p_ep->XferState = USBD_XFER_STATE_ASYNC;
  } else if ((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_ISOC) {
    p_ep->XferState = USBD_XFER_STATE_ASYNC_PARTIAL;            // Xfer will have to be done in many transactions.
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                          // Cannot split xfer on isoc EP.
    return;
  }

  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxSuccessNbr);
  USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxStartNbr);

  p_drv_api->EP_TxStart(p_drv,
                        p_ep->Addr,
                        p_buf_cur,
                        p_urb->NextXferLen,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_urb->XferLen += p_urb->NextXferLen;                       // Error not accounted on total xfer len.
    p_urb->NextXferLen = 0u;
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxStartSuccessNbr);
  }

  return;
}

/****************************************************************************************************//**
 *                                               USBD_EP_Rx()
 *
 * @brief    Receive data on OUT endpoint.This function should not be called from Interrupt Context.
 *
 * @param    p_drv           Pointer to device driver structure.
 *
 * @param    -----           Argument checked by caller.
 *
 * @param    p_ep            Pointer to endpoint on which data will be received.
 *
 * @param    ----            Argument checked by caller.
 *
 * @param    p_buf           Pointer to destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    timeout_ms      Timeout in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this function.
 *
 * @return   Number of octets received, if no errors are returned.
 *           0,                         if any errors returned.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Endpoint must be locked when calling this function.
 *
 * @note     (3) During a synchronous transfer, endpoint is unlocked before pending on transfer
 *               completion to be able to abort. Since the endpoint is already locked when this
 *               function is called (see callers functions), it releases the lock before pending and
 *               re-locks once the transfer completes.
 *
 * @note     (4) This condition covers also the case where the transfer length is multiple of the
 *               maximum packet size. In that case, host sends a zero-length packet considered as
 *               a short packet for the condition.
 *******************************************************************************************************/
static CPU_INT32U USBD_EP_Rx(USBD_DRV        *p_drv,
                             USBD_EP         *p_ep,
                             void            *p_buf,
                             CPU_INT32U      buf_len,
                             USBD_ASYNC_FNCT async_fnct,
                             void            *p_async_arg,
                             CPU_INT16U      timeout_ms,
                             RTOS_ERR        *p_err)
{
  USBD_URB        *p_urb;
  USBD_DRV_API    *p_drv_api;
  USBD_XFER_STATE prev_xfer_state;
  CPU_INT08U      *p_buf_cur;
  CPU_INT32U      xfer_len;
  CPU_INT32U      xfer_tot;
  CPU_INT32U      prev_xfer_len;
  RTOS_ERR        local_err;

  if ((buf_len != 0u)
      && (p_buf == DEF_NULL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return (0u);
  }

  if (async_fnct == DEF_NULL) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxSyncExecNbr);
    if (p_ep->XferState != USBD_XFER_STATE_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return (0u);
    }
  } else {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxAsyncExecNbr);
    if ((p_ep->XferState != USBD_XFER_STATE_NONE)
        && (p_ep->XferState != USBD_XFER_STATE_ASYNC)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return (0u);
    }
  }

  p_urb = USBD_URB_Get(p_drv->DevNbr, p_ep, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_urb->BufPtr = (CPU_INT08U *)p_buf;                          // Init 'p_urb' fields.
  p_urb->BufLen = buf_len;
  p_urb->XferLen = 0u;
  p_urb->NextXferLen = 0u;
  p_urb->AsyncFnct = async_fnct;
  p_urb->AsyncFnctArg = p_async_arg;
  p_urb->NextPtr = DEF_NULL;
  RTOS_ERR_SET(p_urb->Err, RTOS_ERR_NONE);

  if (async_fnct != DEF_NULL) {                                 // -------------------- ASYNC XFER --------------------
    p_urb->State = USBD_URB_STATE_XFER_ASYNC;
    prev_xfer_state = p_ep->XferState;                          // Keep prev XferState, to restore in case of err.
    p_ep->XferState = USBD_XFER_STATE_ASYNC;                    // Set XferState before submitting the xfer.

    USBD_EP_RxStartAsyncProcess(p_drv,
                                p_ep,
                                p_urb,
                                p_urb->BufPtr,
                                p_urb->BufLen,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      USBD_URB_Queue(p_ep, p_urb);                              // If no err, queue URB.
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxAsyncSuccessNbr);
    } else {
      p_ep->XferState = prev_xfer_state;                        // If an err occured, restore prev XferState.
      USBD_URB_Free(p_drv->DevNbr, p_ep, p_urb);                // Free URB.
    }

    return (0u);
  }

  p_ep->XferState = USBD_XFER_STATE_SYNC;                       // -------------------- SYNC XFER ---------------------
  p_urb->State = USBD_URB_STATE_XFER_SYNC;

  USBD_URB_Queue(p_ep, p_urb);

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.
  p_urb->NextXferLen = p_urb->BufLen;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  while ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
         && (p_urb->NextXferLen > 0u)) {
    p_buf_cur = &p_urb->BufPtr[p_urb->XferLen];

    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxStartNbr);
    p_urb->NextXferLen = p_drv_api->EP_RxStart(p_drv,
                                               p_ep->Addr,
                                               p_buf_cur,
                                               p_urb->NextXferLen,
                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxStartSuccessNbr);

    USBD_OS_EP_LockRelease(p_drv->DevNbr,                       // Unlock before pending on completion. See Note #3.
                           p_ep->Ix);

    USBD_OS_EP_SignalPend(p_drv->DevNbr,
                          p_ep->Ix,
                          timeout_ms,
                          p_err);

    USBD_OS_EP_LockAcquire(p_drv->DevNbr,                       // Re-lock EP after xfer completion. See Note #3.
                           p_ep->Ix,
                           &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        RTOS_ERR_COPY(*p_err, local_err);
      }
      break;
    }

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      p_drv_api->EP_Abort(p_drv, p_ep->Addr);
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, RxSyncTimeoutErrNbr);
      break;
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }

    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxNbr);
    xfer_len = p_drv_api->EP_Rx(p_drv,
                                p_ep->Addr,
                                p_buf_cur,
                                p_urb->NextXferLen,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvRxSuccessNbr);

    if (xfer_len > p_urb->NextXferLen) {                        // Rx'd more data than what was expected.
      p_urb->XferLen += p_urb->NextXferLen;
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    } else {
      p_urb->XferLen += xfer_len;
      prev_xfer_len = p_urb->NextXferLen;
      p_urb->NextXferLen = p_urb->BufLen - p_urb->XferLen;
      if ((xfer_len == 0u)                                      // Rx'd a ZLP.
          || (xfer_len < prev_xfer_len)) {                      // Rx'd a short pkt (see Note #4).
        p_urb->NextXferLen = 0u;
      }
    }
  }

  xfer_tot = p_urb->XferLen;

  USBD_URB_Dequeue(p_ep);

  USBD_URB_Free(p_drv->DevNbr, p_ep, p_urb);

  USBD_DBG_STATS_EP_INC_IF_TRUE(p_drv->DevNbr, p_ep->Ix, RxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_tot);
}

/****************************************************************************************************//**
 *                                               USBD_EP_Tx()
 *
 * @brief    Send data on IN endpoints.
 *
 * @param    p_drv           Pointer to device driver structure.
 *
 * @param    -----           Argument checked by caller.
 *
 * @param    p_ep            Pointer to endpoint on which data will be sent.
 *
 * @param    ----            Argument checked by caller.
 *
 * @param    p_buf           Pointer to buffer of data that will be sent.
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    timeout_ms      Timeout in milliseconds.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @param    p_err           Pointer to the variable that will receive return error code from this function
 *
 * @return   Number of octets transmitted, if no errors are returned.
 *           0,                            if any errors returned.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *
 * @note     (3) Endpoint must be locked when calling this function.
 *
 * @note     (4) During a synchronous transfer, endpoint is unlocked before pending on transfer
 *               completion to be able to abort. Since the endpoint is already locked when this
 *               function is called (see callers functions), it releases the lock before pending and
 *               re-locks once the transfer completes.
 *******************************************************************************************************/
static CPU_INT32U USBD_EP_Tx(USBD_DRV        *p_drv,
                             USBD_EP         *p_ep,
                             void            *p_buf,
                             CPU_INT32U      buf_len,
                             USBD_ASYNC_FNCT async_fnct,
                             void            *p_async_arg,
                             CPU_INT16U      timeout_ms,
                             CPU_BOOLEAN     end,
                             RTOS_ERR        *p_err)
{
  USBD_URB        *p_urb;
  USBD_DRV_API    *p_drv_api;
  USBD_XFER_STATE prev_xfer_state;
  CPU_INT08U      *p_buf_cur;
  CPU_INT32U      xfer_rem;
  CPU_INT32U      xfer_tot;
  RTOS_ERR        local_err;
  CPU_BOOLEAN     zlp_flag = DEF_NO;

  if ((buf_len != 0u)
      && (p_buf == DEF_NULL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return (0u);
  }

  if (async_fnct == DEF_NULL) {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxSyncExecNbr);
    if (p_ep->XferState != USBD_XFER_STATE_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return (0u);
    }
  } else {
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxAsyncExecNbr);
    if ((p_ep->XferState != USBD_XFER_STATE_NONE)
        && (p_ep->XferState != USBD_XFER_STATE_ASYNC)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return (0u);
    }
  }

  if (buf_len == 0u) {
    zlp_flag = DEF_YES;
  }

  p_urb = USBD_URB_Get(p_drv->DevNbr, p_ep, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_urb->BufPtr = (CPU_INT08U *)p_buf;                          // Init 'p_urb' fields.
  p_urb->BufLen = buf_len;
  p_urb->XferLen = 0u;
  p_urb->NextXferLen = 0u;
  p_urb->AsyncFnct = async_fnct;
  p_urb->AsyncFnctArg = p_async_arg;
  p_urb->NextPtr = DEF_NULL;
  RTOS_ERR_SET(p_urb->Err, RTOS_ERR_NONE);
  if (end == DEF_YES) {
    DEF_BIT_SET(p_urb->Flags, USBD_URB_FLAG_XFER_END);
  }

  if (async_fnct != DEF_NULL) {                                 // -------------------- ASYNC XFER --------------------
    p_urb->State = USBD_URB_STATE_XFER_ASYNC;
    prev_xfer_state = p_ep->XferState;                          // Keep prev XferState, to restore in case of err.
    p_ep->XferState = USBD_XFER_STATE_ASYNC;                    // Set XferState before submitting the xfer.

    USBD_EP_TxAsyncProcess(p_drv,
                           p_ep,
                           p_urb,
                           p_urb->BufPtr,
                           p_urb->BufLen,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      USBD_URB_Queue(p_ep, p_urb);                              // If no err, queue URB.
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxAsyncSuccessNbr);
    } else {
      p_ep->XferState = prev_xfer_state;                        // If an err occured, restore prev XferState.
      USBD_URB_Free(p_drv->DevNbr, p_ep, p_urb);                // Free URB.
    }

    return (0u);
  }

  p_ep->XferState = USBD_XFER_STATE_SYNC;                       // -------------------- SYNC XFER ---------------------
  p_urb->State = USBD_URB_STATE_XFER_SYNC;

  USBD_URB_Queue(p_ep, p_urb);

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.
  xfer_rem = p_urb->BufLen;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  while ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
         && ((xfer_rem > 0u)
             || (zlp_flag == DEF_YES))) {
    if (zlp_flag == DEF_NO) {
      p_buf_cur = &p_urb->BufPtr[p_urb->XferLen];
    } else {
      p_buf_cur = DEF_NULL;
      zlp_flag = DEF_NO;                                        // If Tx ZLP, loop done only once.
    }

    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxNbr);

    p_urb->NextXferLen = p_drv_api->EP_Tx(p_drv,
                                          p_ep->Addr,
                                          p_buf_cur,
                                          xfer_rem,
                                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }

    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxSuccessNbr);
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxStartNbr);

    p_drv_api->EP_TxStart(p_drv,
                          p_ep->Addr,
                          p_buf_cur,
                          p_urb->NextXferLen,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }

    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxStartSuccessNbr);

    USBD_OS_EP_LockRelease(p_drv->DevNbr,                       // Unlock before pending on completion. See Note #4.
                           p_ep->Ix);

    USBD_OS_EP_SignalPend(p_drv->DevNbr,
                          p_ep->Ix,
                          timeout_ms,
                          p_err);

    USBD_OS_EP_LockAcquire(p_drv->DevNbr,                       // Re-lock EP after xfer completion. See Note #4.
                           p_ep->Ix,
                           &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        RTOS_ERR_COPY(*p_err, local_err);
      }
      break;
    }

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT) {
      p_drv_api->EP_Abort(p_drv, p_ep->Addr);
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, TxSyncTimeoutErrNbr);
      break;
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }

    p_urb->XferLen += p_urb->NextXferLen;
    xfer_rem -= p_urb->NextXferLen;
  }

  xfer_tot = p_urb->XferLen;

  if ((end == DEF_YES)
      && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
      && ((p_urb->BufLen % p_ep->MaxPktSize) == 0u)
      && (p_urb->BufLen != 0u)) {
    //                                                             $$$$ This case should be tested more thoroughly.
    USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_Nbr);

    p_drv_api->EP_TxZLP(p_drv, p_ep->Addr, p_err);

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      USBD_DBG_STATS_EP_INC(p_drv->DevNbr, p_ep->Ix, DrvTxZLP_SuccessNbr);

      USBD_OS_EP_LockRelease(p_drv->DevNbr,                     // Unlock before pending on completion. See Note #4.
                             p_ep->Ix);

      USBD_OS_EP_SignalPend(p_drv->DevNbr, p_ep->Ix, timeout_ms, p_err);

      USBD_OS_EP_LockAcquire(p_drv->DevNbr,                     // Re-lock EP after xfer completion. See Note #4.
                             p_ep->Ix,
                             &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        RTOS_ERR_COPY(*p_err, local_err);
      }
    }
  }

  USBD_URB_Dequeue(p_ep);

  USBD_URB_Free(p_drv->DevNbr, p_ep, p_urb);

  USBD_DBG_STATS_EP_INC_IF_TRUE(p_drv->DevNbr, p_ep->Ix, TxSyncSuccessNbr, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_tot);
}

/****************************************************************************************************//**
 *                                           USBD_EP_URB_Abort()
 *
 * @brief    Aborts endpoint's URB(s). Does not free the URB(s), see Note #1.
 *
 * @param    p_drv   Pointer to device driver structure.
 *
 * @param    -----   Argument checked by caller.
 *
 * @param    p_ep    Pointer to endpoint on which data will be sent.
 *
 * @param    ----    Argument checked by caller.
 *
 * @param    p_err   Pointer to the variable that will receive return error code from this function :
 *
 * @param    -----   Argument checked by caller.
 *
 * @return   Pointer to head of completed URB list, if asynchronous transfer in progress.
 *           Pointer to NULL,                       if the transfer is not in progress.
 *
 * @note     (1) If a synchronous transfer was in progress, a single URB was aborted and must be
 *               freed by calling  USBD_URB_Free(). If an asynchronous transfer was in progress, it
 *               is possible that multiple URBs were aborted and must be freed by calling
 *               USBD_URB_AsyncEnd() with the pointer to the head of the aborted URB(s) list returned
 *               by this function as a parameter.
 *
 * @note     (2) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static USBD_URB *USBD_EP_URB_Abort(USBD_DRV *p_drv,
                                   USBD_EP  *p_ep,
                                   RTOS_ERR *p_err)
{
  USBD_DRV_API *p_drv_api;
  CPU_BOOLEAN  abort_ok;
  RTOS_ERR     err;
  USBD_URB     *p_urb_head;
  USBD_URB     *p_urb;
  USBD_URB     *p_urb_cur;

  p_drv_api = p_drv->API_Ptr;                                   // Get dev drv API struct.
  p_urb_head = DEF_NULL;
  p_urb_cur = DEF_NULL;

  switch (p_ep->XferState) {
    case USBD_XFER_STATE_NONE:
      abort_ok = DEF_OK;
      break;

    case USBD_XFER_STATE_ASYNC:
    case USBD_XFER_STATE_ASYNC_PARTIAL:
      p_urb = p_ep->URB_HeadPtr;
      RTOS_ERR_SET(err, RTOS_ERR_ABORT);
      while (p_urb != DEF_NULL) {
        p_urb = USBD_URB_AsyncCmpl(p_ep, err);
        if (p_urb_head == DEF_NULL) {
          p_urb_head = p_urb;
        } else {
          p_urb_cur->NextPtr = p_urb;
        }
        p_urb_cur = p_urb;
      }
      abort_ok = p_drv->API_Ptr->EP_Abort(p_drv, p_ep->Addr);   // Call drv's abort fnct.
      break;

    case USBD_XFER_STATE_SYNC:
      USBD_OS_EP_SignalAbort(p_drv->DevNbr, p_ep->Ix, p_err);

      abort_ok = p_drv_api->EP_Abort(p_drv, p_ep->Addr);
      break;

    default:
      abort_ok = DEF_FAIL;
      break;
  }

  if (abort_ok == DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
  }

  return (p_urb_head);                                          // See Note #1.
}

/****************************************************************************************************//**
 *                                           USBD_URB_AsyncCmpl()
 *
 * @brief    Notify URB completion to asynchronous callback.
 *
 * @param    p_ep    Pointer to endpoint on which transfer has completed.
 *
 * @param    ----  Argument checked by caller.
 *
 * @param    err     Error associated with transfer.
 *
 * @return   Pointer to completed URB, if any.
 *
 *           Pointer to NULL,          otherwise.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *
 * @note     (2) Endpoint must have an asynchronous transfer in progress.
 *******************************************************************************************************/
static USBD_URB *USBD_URB_AsyncCmpl(USBD_EP  *p_ep,
                                    RTOS_ERR err)
{
  USBD_URB *p_urb;

  p_urb = p_ep->URB_HeadPtr;                                    // Get head URB for EP.
  if (p_urb == DEF_NULL) {
    return (p_urb);
  }

  USBD_URB_Dequeue(p_ep);                                       // Dequeue first URB from EP.

  RTOS_ERR_COPY(p_urb->Err, err);                               // Set err for curr URB.
  p_urb->NextPtr = DEF_NULL;                                    // Remove links with 'p_ep' URB linked list.

  return (p_urb);
}

/****************************************************************************************************//**
 *                                           USBD_URB_AsyncEnd()
 *
 * @brief    Execute callback associated with each USB request block in the linked list and free them.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument checked by caller.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb_head  Pointer to head of USB request block linked list.
 *
 * @param    -----       Argument checked by caller.
 *
 * @note     (1) Endpoint must NOT be locked when calling this function.
 *******************************************************************************************************/
static void USBD_URB_AsyncEnd(CPU_INT08U dev_nbr,
                              USBD_EP    *p_ep,
                              USBD_URB   *p_urb_head)
{
  USBD_URB        *p_urb_cur;
  USBD_URB        *p_urb_next;
  void            *p_buf;
  CPU_INT32U      buf_len;
  CPU_INT32U      xfer_len;
  USBD_ASYNC_FNCT async_fnct;
  void            *p_async_arg;
  RTOS_ERR        err;

  p_urb_cur = p_urb_head;
  while (p_urb_cur != DEF_NULL) {                               // Iterate through linked list.
    p_buf = (void *)p_urb_cur->BufPtr;
    buf_len = p_urb_cur->BufLen;
    xfer_len = p_urb_cur->XferLen;
    async_fnct = p_urb_cur->AsyncFnct;
    p_async_arg = p_urb_cur->AsyncFnctArg;
    p_urb_next = p_urb_cur->NextPtr;
    RTOS_ERR_COPY(err, p_urb_cur->Err);

    USBD_URB_Free(dev_nbr, p_ep, p_urb_cur);                    // Free URB to pool.

    async_fnct(dev_nbr,                                         // Execute callback fnct.
               p_ep->Addr,
               p_buf,
               buf_len,
               xfer_len,
               p_async_arg,
               err);

    p_urb_cur = p_urb_next;
  }
}

/****************************************************************************************************//**
 *                                               USBD_URB_Free()
 *
 * @brief    Free URB to URB pool.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument checked by caller.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    -----       Argument checked by caller.
 *******************************************************************************************************/
static void USBD_URB_Free(CPU_INT08U dev_nbr,
                          USBD_EP    *p_ep,
                          USBD_URB   *p_urb)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_urb->State = USBD_URB_STATE_IDLE;
  p_urb->NextPtr = USBD_Ptr->URB_TblPtr[dev_nbr];
  USBD_Ptr->URB_TblPtr[dev_nbr] = p_urb;

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET(p_urb->Flags, USBD_URB_FLAG_EXTRA_URB)) {
    //                                                             If the URB freed is an 'extra' URB, dec ctr.
    //                                                            USBD_URB_ExtraCtr[dev_nbr]--;
    USBD_Ptr->URB_ExtraAvailCntTbl[dev_nbr]++;
  } else {
    p_ep->URB_MainAvail = DEF_YES;
  }
#else
  (void)&p_ep;
#endif
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               USBD_URB_Get()
 *
 * @brief    Get URB from URB pool.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument checked by caller.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_err       Pointer to the variable that will receive return error code from this function.
 *
 * @param    -----       Argument checked by caller.
 *
 * @return   Pointer to USB request block, if no errors are returned.
 *           Pointer to NULL,              if any errors returned.
 *******************************************************************************************************/
static USBD_URB *USBD_URB_Get(CPU_INT08U dev_nbr,
                              USBD_EP    *p_ep,
                              RTOS_ERR   *p_err)
{
  CPU_BOOLEAN ep_empty;
  USBD_URB    *p_urb;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  ep_empty = ((p_ep->URB_HeadPtr == DEF_NULL) && (p_ep->URB_TailPtr == DEF_NULL)) ? DEF_YES : DEF_NO;

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  //                                                               Check if EP is empty, if enough URB rem or if main...
  //                                                               ...URB avail.
  if ((ep_empty == DEF_YES)
      || (USBD_Ptr->URB_ExtraAvailCntTbl[dev_nbr] > 0u)
      || (p_ep->URB_MainAvail == DEF_YES)) {
#else
  if (ep_empty == DEF_YES) {                                    // Check if EP is empty.
#endif

    p_urb = USBD_Ptr->URB_TblPtr[dev_nbr];
    if (p_urb != DEF_NULL) {
      USBD_Ptr->URB_TblPtr[dev_nbr] = p_urb->NextPtr;

      p_urb->NextPtr = DEF_NULL;
      p_urb->Flags = 0u;
#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
      if ((ep_empty == DEF_NO)
          && (p_ep->URB_MainAvail == DEF_NO)) {
        //                                                         If the EP already has an URB in progress, inc ctr and mark the URB as an 'extra' URB.
        USBD_Ptr->URB_ExtraAvailCntTbl[dev_nbr]--;
        DEF_BIT_SET(p_urb->Flags, USBD_URB_FLAG_EXTRA_URB);
      } else if (p_ep->URB_MainAvail == DEF_YES) {
        p_ep->URB_MainAvail = DEF_NO;
      }
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_QUEUING);
      p_urb = DEF_NULL;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_QUEUING);
    p_urb = DEF_NULL;
  }
  CORE_EXIT_ATOMIC();

  return (p_urb);
}

/****************************************************************************************************//**
 *                                               USBD_URB_Queue()
 *
 * @brief    Queue USB request block into endpoint.
 *
 * @param    p_ep    Pointer to endpoint structure.
 *
 * @param    ----    Argument checked by caller.
 *
 * @param    p_urb   Pointer to USB request block.
 *
 * @param    -----   Argument checked by caller.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void USBD_URB_Queue(USBD_EP  *p_ep,
                           USBD_URB *p_urb)
{
  p_urb->NextPtr = DEF_NULL;

  if (p_ep->URB_TailPtr == DEF_NULL) {                          // Q is empty.
    p_ep->URB_HeadPtr = p_urb;
    p_ep->URB_TailPtr = p_urb;
  } else {                                                      // Q is not empty.
    p_ep->URB_TailPtr->NextPtr = p_urb;
    p_ep->URB_TailPtr = p_urb;
  }

  return;
}

/****************************************************************************************************//**
 *                                           USBD_URB_Dequeue()
 *
 * @brief    Dequeue head USB request block from endpoint.
 *
 * @param    p_ep    Pointer to endpoint structure.
 *
 * @param    ----    Argument checked by caller.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void USBD_URB_Dequeue(USBD_EP *p_ep)
{
  USBD_URB *p_urb;

  p_urb = p_ep->URB_HeadPtr;
  if (p_urb == DEF_NULL) {
    return;
  }

  if (p_urb->NextPtr == DEF_NULL) {                             // Only one URB is queued.
    p_ep->URB_HeadPtr = DEF_NULL;
    p_ep->URB_TailPtr = DEF_NULL;
    p_ep->XferState = USBD_XFER_STATE_NONE;
  } else {
    p_ep->URB_HeadPtr = p_urb->NextPtr;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_AVAIL))
