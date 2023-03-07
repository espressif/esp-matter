/***************************************************************************//**
 * @file
 * @brief USB Device Example - USB Vendor class - Loopback
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_ascii.h>

#include  <kernel/include/os.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_vendor.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       EXAMPLE CONFIGURATIONS
 *******************************************************************************************************/

#define  EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN               DEF_ENABLED
#define  EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN              DEF_ENABLED

/********************************************************************************************************
 *                                              TIMEOUTS
 *******************************************************************************************************/

#define  EX_USBD_VENDOR_LOOPBACK_RX_TIMEOUT_MS             5000u
#define  EX_USBD_VENDOR_LOOPBACK_TX_TIMEOUT_MS             5000u

/********************************************************************************************************
 *                                              TASK CFGS
 *******************************************************************************************************/

#define  EX_USBD_VENDOR_LOOPBACK_TASK_PRIO                 21u
#define  EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE             256u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static const CPU_INT08U Ex_USBD_Vendor_LoopbackPropertyNameGUID[] = {
  'D', 0u, 'e', 0u, 'v', 0u, 'i', 0u, 'c', 0u, 'e', 0u,
  'I', 0u, 'n', 0u, 't', 0u, 'e', 0u, 'r', 0u, 'f', 0u, 'a', 0u, 'c', 0u, 'e', 0u,
  'G', 0u, 'U', 0u, 'I', 0u, 'D', 0u, 0u, 0u
};

static const CPU_INT08U Ex_USBD_Vendor_LoopbackMS_GUID[] = {
  '{', 0u, '1', 0u, '4', 0u, '3', 0u, 'F', 0u, '2', 0u, '0', 0u, 'B', 0u, 'D', 0u,
  '-', 0u, '7', 0u, 'B', 0u, 'D', 0u, '2', 0u, '-', 0u, '4', 0u, 'C', 0u, 'A', 0u, '6', 0u,
  '-', 0u, '9', 0u, '4', 0u, '6', 0u, '5', 0u,
  '-', 0u, '8', 0u, '8', 0u, '8', 0u, '2', 0u, 'F', 0u, '2', 0u, '1', 0u, '5', 0u, '6', 0u, 'B', 0u, 'D', 0u, '6', 0u, '}', 0u, 0u, 0u
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED)
static OS_TCB Ex_USBD_Vendor_LoopbackEchoSyncTaskTCB;

static CPU_STK    Ex_USBD_Vendor_LoopbackEchoSyncTaskStk[EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE];
static CPU_INT08U Ex_USBD_Vendor_LoopbackBuf[512u];
#endif

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static OS_TCB     Ex_USBD_Vendor_LoopbackAsyncTaskTCB;
static OS_SEM     Ex_USBD_Vendor_LoopbackAsyncSem;
static CPU_STK    Ex_USBD_Vendor_LoopbackAsyncTaskStk[EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE];
static CPU_INT08U Ex_USBD_Vendor_LoopbackPayloadBuf[512u];
static CPU_INT08U Ex_USBD_Vendor_LoopbackHeaderBuf[2u];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN Ex_USBD_Vendor_LoopbackVendReq(CPU_INT08U           class_nbr,
                                                  CPU_INT08U           dev_nbr,
                                                  const USBD_SETUP_REQ *p_setup_req);

static void Ex_USBD_Vendor_LoopbackEchoSyncTask(void *p_arg);

static CPU_BOOLEAN Ex_USBD_Vendor_LoopbackEcho(CPU_INT08U class_nbr);

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static void Ex_USBD_Vendor_LoopbackAsyncTask(void *p_arg);

static void Ex_USBD_Vendor_LoopbackRxHeaderCmpl(CPU_INT08U class_nbr,
                                                void       *p_buf,
                                                CPU_INT32U buf_len,
                                                CPU_INT32U xfer_len,
                                                void       *p_callback_arg,
                                                RTOS_ERR   err);

static void Ex_USBD_Vendor_LoopbackRxPayloadCmpl(CPU_INT08U class_nbr,
                                                 void       *p_buf,
                                                 CPU_INT32U buf_len,
                                                 CPU_INT32U xfer_len,
                                                 void       *p_callback_arg,
                                                 RTOS_ERR   err);

static void Ex_USBD_Vendor_LoopbackTxPayloadCmpl(CPU_INT08U class_nbr,
                                                 void       *p_buf,
                                                 CPU_INT32U buf_len,
                                                 CPU_INT32U xfer_len,
                                                 void       *p_callback_arg,
                                                 RTOS_ERR   err);
#endif
/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_USBD_Vendor_LoopbackInit()
 *
 * @brief  Adds a vendor interface to the device and creates a task to handle the loopback
 *         functionnalities.
 *
 * @param  dev_nbr        Device number.
 *
 * @param  config_nbr_fs  Full-Speed configuration number.
 *
 * @param  config_nbr_hs  High-Speed configuration number.
 *******************************************************************************************************/
void Ex_USBD_Vendor_Init(CPU_INT08U dev_nbr,
                         CPU_INT08U config_nbr_fs,
                         CPU_INT08U config_nbr_hs)
{
  CPU_INT08U          vendor_nbr_0;
  USBD_VENDOR_QTY_CFG cfg_vendor;
  RTOS_ERR            err;
#if ((EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED) \
  && (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED))
  CPU_INT08U vendor_nbr_1;
#endif

  //                                                               ---------------- INIT VENDOR CLASS -----------------
  cfg_vendor.ClassInstanceQty = 1u;
  cfg_vendor.ConfigQty = 2u;

#if ((EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED) \
  && (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED))
  cfg_vendor.ClassInstanceQty++;                                // Add a class instance.
#endif

  USBD_Vendor_Init(&cfg_vendor, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- CREATE VENDOR CLASS INSTANCE -----------
  vendor_nbr_0 = USBD_Vendor_Add(DEF_NO,
                                 0u,
                                 Ex_USBD_Vendor_LoopbackVendReq,   // Optional fnct that is called on vendor specific req.
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------- ADD CLASS INSTANCE TO CONFIGS -----------
  USBD_Vendor_ConfigAdd(vendor_nbr_0,
                        dev_nbr,
                        config_nbr_fs,
                        &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (config_nbr_hs != USBD_CONFIG_NBR_NONE) {                  // Add instance to HS config if available.
    USBD_Vendor_ConfigAdd(vendor_nbr_0,
                          dev_nbr,
                          config_nbr_hs,
                          &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)                     // If MS OS descriptors enabled, add ext properties...
  USBD_DevSetMS_VendorCode(dev_nbr, 1u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Add GUID properties. Must match Windows application.
  USBD_Vendor_MS_ExtPropertyAdd(vendor_nbr_0,
                                USBD_MS_OS_PROPERTY_TYPE_REG_SZ,
                                Ex_USBD_Vendor_LoopbackPropertyNameGUID,
                                sizeof(Ex_USBD_Vendor_LoopbackPropertyNameGUID),
                                Ex_USBD_Vendor_LoopbackMS_GUID,
                                sizeof(Ex_USBD_Vendor_LoopbackMS_GUID),
                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               Add second vendor class instance, if needed.
#if ((EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED) \
  && (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED))

  //                                                               ----------- CREATE VENDOR CLASS INSTANCE -----------
  vendor_nbr_1 = USBD_Vendor_Add(DEF_NO,
                                 0u,
                                 Ex_USBD_Vendor_LoopbackVendReq,// Optional fnct that is called on vendor specific req.
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------- ADD CLASS INSTANCE TO CONFIGS -----------
  USBD_Vendor_ConfigAdd(vendor_nbr_1,
                        dev_nbr,
                        config_nbr_fs,
                        &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (config_nbr_hs != USBD_CONFIG_NBR_NONE) {                  // Add instance to HS config if available.
    USBD_Vendor_ConfigAdd(vendor_nbr_1,
                          dev_nbr,
                          config_nbr_hs,
                          &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  //                                                               Add GUID properties. Must match Windows application.
  USBD_Vendor_MS_ExtPropertyAdd(vendor_nbr_1,
                                USBD_MS_OS_PROPERTY_TYPE_REG_SZ,
                                Ex_USBD_Vendor_LoopbackPropertyNameGUID,
                                sizeof(Ex_USBD_Vendor_LoopbackPropertyNameGUID),
                                Ex_USBD_Vendor_LoopbackMS_GUID,
                                sizeof(Ex_USBD_Vendor_LoopbackMS_GUID),
                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif
#endif

#if (EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED)
  OSTaskCreate(&Ex_USBD_Vendor_LoopbackEchoSyncTaskTCB,
               "USB Device Vendor Echo Sync",
               Ex_USBD_Vendor_LoopbackEchoSyncTask,
               (void *)(CPU_ADDR)vendor_nbr_0,
               EX_USBD_VENDOR_LOOPBACK_TASK_PRIO,
               &Ex_USBD_Vendor_LoopbackEchoSyncTaskStk[0],
               EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE / 10u,
               EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
  //                                                               Create sem to signal re(starting) of test sequence.
  OSSemCreate(&Ex_USBD_Vendor_LoopbackAsyncSem,
              "Async task sem",
              1u,                                               // Sem initially available (see Note #1).
              &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create Echo Async task.
  OSTaskCreate(&Ex_USBD_Vendor_LoopbackAsyncTaskTCB,
               "USB Device Vendor Echo Async",
               Ex_USBD_Vendor_LoopbackAsyncTask,
#if ((EX_USBD_VENDOR_LOOPBACK_CFG_SYNC_EN == DEF_ENABLED) \
               && (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED))
               (void *)(CPU_ADDR)vendor_nbr_1,
#else
               (void *)(CPU_ADDR)vendor_nbr_0,
#endif
               EX_USBD_VENDOR_LOOPBACK_TASK_PRIO,
               &Ex_USBD_Vendor_LoopbackAsyncTaskStk[0],
               EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE / 10u,
               EX_USBD_VENDOR_LOOPBACK_TASK_STK_SIZE,
               0u,
               0u,
               (void *)          0,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    Ex_USBD_Vendor_LoopbackClassReq()
 *
 * @brief  This function is called when a vendor specific request is received from the host. This
 *         function can be ommited if no vendor specific request is used.
 *
 * @param  class_nbr    Class number.
 *
 * @param  dev_nbr      Device number.
 *
 * @param  p_setup_req  Pointer to setup request buffer.
 *
 * @return  DEF_OK:   Operation succeeded.
 *          DEF_FAIL: Otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_USBD_Vendor_LoopbackVendReq(CPU_INT08U           class_nbr,
                                                  CPU_INT08U           dev_nbr,
                                                  const USBD_SETUP_REQ *p_setup_req)
{
  //                                                               TODO: Process vendor specific request.

  PP_UNUSED_PARAM(class_nbr);
  PP_UNUSED_PARAM(dev_nbr);
  PP_UNUSED_PARAM(p_setup_req);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                  Ex_USBD_Vendor_LoopbackEchoSyncTask()
 *
 * @brief  Perform Echo demo with synchronous communication:
 *           - (a) Read header.
 *           - (b) Read payload of a certain size.
 *           - (c) Write payload previously received.
 *
 * @param  p_arg  Argument passed to the task.
 *******************************************************************************************************/
static void Ex_USBD_Vendor_LoopbackEchoSyncTask(void *p_arg)
{
  CPU_INT08U  class_nbr;
  CPU_BOOLEAN conn;
  CPU_BOOLEAN ok;
  RTOS_ERR    err;

  class_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;
  ok = DEF_OK;

  while (DEF_TRUE) {
    //                                                             Wait for cfg activated by host.
    conn = USBD_Vendor_IsConn(class_nbr);
    while (conn != DEF_YES) {
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);

      conn = USBD_Vendor_IsConn(class_nbr);
    }

    ok = Ex_USBD_Vendor_LoopbackEcho(class_nbr);

    if (!ok) {
      //                                                           Delay 250 ms if an error occurred.
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);
    }
  }
}

/****************************************************************************************************//**
 *                                      Ex_USBD_Vendor_LoopbackEcho()
 *
 * @brief  Read and write transfers from different size.
 *
 * @param  class_nbr  Vendor class instance number.
 *
 * @return  DEF_OK,   if NO error(s).
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_USBD_Vendor_LoopbackEcho(CPU_INT08U class_nbr)
{
  RTOS_ERR   err;
  CPU_INT08U msg_hdr[2];
  CPU_INT32U nbr_octets_recv;
  CPU_INT32U payload_len;
  CPU_INT32U rem_payload_len;
  CPU_INT32U xfer_len;

  //                                                               ----------------- HEADER RECEPTION -----------------
  nbr_octets_recv = USBD_Vendor_Rd(class_nbr,
                                   (void *)&msg_hdr[0],
                                   2,
                                   0,
                                   &err);
  if ((err.Code != RTOS_ERR_NONE)
      || (nbr_octets_recv != 2)) {                              // Verify that hdr is 2 octets.
    return (DEF_FAIL);
  }

  //                                                               ---------------- PAYLOAD PROCESSING ----------------
  payload_len = DEF_BIT_MASK(msg_hdr[0], 8);                    // Get nbr of octets from the message header.
  payload_len |= msg_hdr[1];
  rem_payload_len = payload_len;

  while (rem_payload_len > 0) {
    if (rem_payload_len >= 512) {
      xfer_len = 512;
    } else {
      xfer_len = rem_payload_len;
    }
    //                                                             Receive payload chunk from host.
    nbr_octets_recv = USBD_Vendor_Rd(class_nbr,
                                     (void *)&Ex_USBD_Vendor_LoopbackBuf[0],
                                     xfer_len,
                                     EX_USBD_VENDOR_LOOPBACK_RX_TIMEOUT_MS,
                                     &err);
    if (err.Code != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

    rem_payload_len -= nbr_octets_recv;                         // Remaining nbr of octets to be received.

    //                                                             Send back received payload chunk to host.
    (void)USBD_Vendor_Wr(class_nbr,
                         (void *)&Ex_USBD_Vendor_LoopbackBuf[0],
                         nbr_octets_recv,
                         EX_USBD_VENDOR_LOOPBACK_TX_TIMEOUT_MS,
                         DEF_FALSE,
                         &err);
    if (err.Code != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                   Ex_USBD_Vendor_LoopbackAsyncTask()
 *
 * @brief  Perform loopback demo with asynchronous communication (see Note #1):
 *           - (a) Read header.
 *           - (b) Read payload of a certain size.
 *           - (c) Write payload previously received.
 *
 * @param  p_arg  Argument passed to the task.
 *
 * @note   (1) The Echo Async task will prepare the first read to initiate the test sequence with
 *             the host. The other steps of the test sequence will be managed in the different
 *             asynchronous callback.
 *******************************************************************************************************/

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static void Ex_USBD_Vendor_LoopbackAsyncTask(void *p_arg)
{
  CPU_INT08U  class_nbr;
  CPU_BOOLEAN conn;
  CPU_INT32U  nbr_octets_expected;
  RTOS_ERR    err;

  class_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;

  while (DEF_TRUE) {
    //                                                             Wait to (re)start the test sequence.
    (void)OSSemPend(&Ex_USBD_Vendor_LoopbackAsyncSem,
                    0u,
                    OS_OPT_PEND_BLOCKING,
                    DEF_NULL,
                    &err);

    //                                                             Wait for cfg activated by host.
    conn = USBD_Vendor_IsConn(class_nbr);
    while (conn != DEF_YES) {
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

      conn = USBD_Vendor_IsConn(class_nbr);
    }

    nbr_octets_expected = 0;
    //                                                             ----------------- HEADER RECEPTION -----------------
    //                                                             Prepare async Bulk OUT xfer.
    USBD_Vendor_RdAsync(class_nbr,
                        (void *)&Ex_USBD_Vendor_LoopbackHeaderBuf[0],
                        2u,
                        Ex_USBD_Vendor_LoopbackRxHeaderCmpl,
                        (void *)&nbr_octets_expected,
                        &err);
    if (err.Code != RTOS_ERR_NONE) {
      OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
                OS_OPT_POST_ALL,
                &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                  Ex_USBD_Vendor_LoopbackRxHeaderCmpl()
 *
 * @brief  Callback called upon Bulk OUT transfer completion.
 *
 * @param  p_buf           Pointer to receive buffer.
 *
 * @param  buf_len         Receive buffer length.
 *
 * @param  xfer_len        Number of octets received.
 *
 * @param  p_callback_arg  Additional argument provided by application.
 *
 * @param  err             Transfer error status.
 *******************************************************************************************************/

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static void Ex_USBD_Vendor_LoopbackRxHeaderCmpl(CPU_INT08U class_nbr,
                                                void       *p_buf,
                                                CPU_INT32U buf_len,
                                                CPU_INT32U xfer_len,
                                                void       *p_callback_arg,
                                                RTOS_ERR   err)
{
  CPU_INT08U *p_header_buf;
  CPU_INT32U payload_len;
  CPU_INT32U *p_nbr_octets_expected;
  RTOS_ERR   err_usb;

  PP_UNUSED_PARAM(buf_len);

  p_header_buf = (CPU_INT08U *)p_buf;
  p_nbr_octets_expected = (CPU_INT32U *)p_callback_arg;

  if (err.Code == RTOS_ERR_NONE) {
    if (xfer_len != 2) {                                        // Verify that hdr is 2 octets.
      return;
    }
    //                                                             ---------------- PAYLOAD PROCESSING ----------------
    //                                                             Get nbr of octets from the message header.
    payload_len = DEF_BIT_MASK(p_header_buf[0], 8);
    payload_len |= p_header_buf[1];
    *p_nbr_octets_expected = payload_len;

    if (*p_nbr_octets_expected >= 512) {
      xfer_len = 512;
    } else {
      xfer_len = *p_nbr_octets_expected;
    }

    //                                                             Receive payload chunk from host.
    USBD_Vendor_RdAsync(class_nbr,
                        (void *)&Ex_USBD_Vendor_LoopbackPayloadBuf[0],
                        xfer_len,
                        Ex_USBD_Vendor_LoopbackRxPayloadCmpl,
                        (void *) p_nbr_octets_expected,
                        &err_usb);
    if (err_usb.Code != RTOS_ERR_NONE) {                        // Restart header reception.
      OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
                OS_OPT_POST_ALL,
                &err_usb);
      APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
    }
  } else {                                                      // Restart header reception.
    OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
              OS_OPT_POST_ALL,
              &err_usb);
    APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                 Ex_USBD_Vendor_LoopbackRxPayloadCmpl()
 *
 * @brief  Callback called upon Bulk OUT transfer completion.
 *
 * @param  p_buf           Pointer to receive buffer.
 *
 * @param  buf_len         Receive buffer length.
 *
 * @param  xfer_len        Number of octets received.
 *
 * @param  p_callback_arg  Additional argument provided by application.
 *
 * @param  err             Transfer error status.
 *******************************************************************************************************/

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static void Ex_USBD_Vendor_LoopbackRxPayloadCmpl(CPU_INT08U class_nbr,
                                                 void       *p_buf,
                                                 CPU_INT32U buf_len,
                                                 CPU_INT32U xfer_len,
                                                 void       *p_callback_arg,
                                                 RTOS_ERR   err)
{
  RTOS_ERR err_usb;

  PP_UNUSED_PARAM(buf_len);

  if (err.Code == RTOS_ERR_NONE) {
    //                                                             ---------------- PAYLOAD PROCESSING ----------------
    USBD_Vendor_WrAsync(class_nbr,
                        p_buf,                                  // Rx buf becomes a Tx buf.
                        xfer_len,
                        Ex_USBD_Vendor_LoopbackTxPayloadCmpl,
                        p_callback_arg,                         // Nbr of octets expected gotten from header msg.
                        DEF_FALSE,
                        &err_usb);
    if (err_usb.Code != RTOS_ERR_NONE) {                             // Restart test sequence with header reception.
      OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
                OS_OPT_POST_ALL,
                &err_usb);
      APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
    }
  } else {                                                      // Restart test sequence with header reception.
    OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
              OS_OPT_POST_ALL,
              &err_usb);
    APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                 Ex_USBD_Vendor_LoopbackTxPayloadCmpl()
 *
 * @brief  Callback called upon Bulk IN transfer completion.
 *
 * @param  p_buf           Pointer to transmit buffer.
 *
 * @param  buf_len         Transmit buffer length.
 *
 * @param  xfer_len        Number of octets transmitted.
 *
 * @param  p_callback_arg  Additional argument provided by application.
 *
 * @param  err             Transfer error status.
 *******************************************************************************************************/

#if (EX_USBD_VENDOR_LOOPBACK_CFG_ASYNC_EN == DEF_ENABLED)
static void Ex_USBD_Vendor_LoopbackTxPayloadCmpl(CPU_INT08U class_nbr,
                                                 void       *p_buf,
                                                 CPU_INT32U buf_len,
                                                 CPU_INT32U xfer_len,
                                                 void       *p_callback_arg,
                                                 RTOS_ERR   err)
{
  CPU_INT32U *p_nbr_octets_expected;
  RTOS_ERR   err_usb;

  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(xfer_len);

  p_nbr_octets_expected = (CPU_INT32U *)p_callback_arg;         // Nbr of octets expected gotten from header msg.

  if (err.Code == RTOS_ERR_NONE) {
    *p_nbr_octets_expected -= xfer_len;                         // Update nbr of octets expected from host.

    if (*p_nbr_octets_expected == 0) {                          // All the msg payload received from host.
                                                                // ----------------- HEADER RECEPTION -----------------
                                                                // Prepare async Bulk OUT xfer.
      USBD_Vendor_RdAsync(class_nbr,
                          (void *)&Ex_USBD_Vendor_LoopbackHeaderBuf[0],
                          2u,
                          Ex_USBD_Vendor_LoopbackRxHeaderCmpl,
                          (void *) p_nbr_octets_expected,
                          &err_usb);
      if (err_usb.Code != RTOS_ERR_NONE) {                      // Restart test sequence with header reception.
        OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
                  OS_OPT_POST_ALL,
                  &err_usb);
        APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
      }
    } else {
      //                                                           ---------------- PAYLOAD PROCESSING ----------------
      if (*p_nbr_octets_expected >= 512) {
        xfer_len = 512;
      } else {
        xfer_len = *p_nbr_octets_expected;
      }
      //                                                           Receive payload chunk from host.
      USBD_Vendor_RdAsync(class_nbr,
                          (void *)&Ex_USBD_Vendor_LoopbackPayloadBuf[0],
                          xfer_len,
                          Ex_USBD_Vendor_LoopbackRxPayloadCmpl,
                          (void *) p_nbr_octets_expected,
                          &err_usb);
      if (err_usb.Code != RTOS_ERR_NONE) {                      // Restart test sequence with header reception.
        OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
                  OS_OPT_POST_ALL,
                  &err_usb);
        APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
      }
    }
  } else {                                                      // Restart test sequence with header reception.
    OSSemPost(&Ex_USBD_Vendor_LoopbackAsyncSem,
              OS_OPT_POST_ALL,
              &err_usb);
    APP_RTOS_ASSERT_CRITICAL(err_usb.Code == RTOS_ERR_NONE,; );
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_VENDOR_AVAIL
