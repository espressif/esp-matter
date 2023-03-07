/***************************************************************************//**
 * @file
 * @brief USB Host Example - USB-To-Serial loopback
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
 * The USB2SER class driver is meant to be used with USB-to-serial adapters. It currently supports
 * adapters from FTDI Ltd, Silicon Labs inc, and Prolific Technology inc.
 *
 * This example application, by default, configures the serial port to communicate at 115200 bps, 8 data
 * bits, No parity, and 1 stop bit (no software/hardware handshake). The adapter can be connected via a
 * serial port to a PC and you can use a serial terminal to send data to the serial port. This application
 * will send back all the received data.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_usb2ser.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN          100u

#define  EX_USBH_USB2SER_LOOPBACK_TASK_PRIO                 21u
#define  EX_USBH_USB2SER_LOOPBACK_TASK_STK_SIZE             256u

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                          USB2SER FNCT DATA
 *
 * Note(s) : (1) This is a structure that is allocated for each USB2SER adapter that is connected to the
 *               host. It contains information that is pecific to each USB2SER function.
 *******************************************************************************************************/
typedef  struct  ex_usbh_usb2ser_loopback_fnct {
  USBH_USB2SER_FNCT_HANDLE Handle;                              // Handle on USB2SER fnct.

  //                                                               Transmit data circular buffer.
  CPU_INT08U               CircularBuf[EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN];
  CPU_INT08U               CircularBufWrIx;                     // Circular buffer write index.
  CPU_INT08U               CircularBufRdIx;                     // Circular buffer read index.
} EX_USBH_USB2SER_LOOPBACK_FNCT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void *Ex_USBH_USB2SER_LoopbackConn(USBH_DEV_HANDLE          dev_handle,
                                          USBH_FNCT_HANDLE         fnct_handle,
                                          USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle);

static void Ex_USBH_USB2SER_LoopbackDisconn(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                            void                     *p_arg);

static void Ex_USBH_USB2SER_LoopbackDataRxd(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                            void                     *p_arg,
                                            CPU_INT08U               *p_buf,
                                            CPU_INT32U               buf_len);

static void Ex_USBH_USB2SER_LoopbackSerialStatusChng(USBH_USB2SER_FNCT_HANDLE         usb2ser_fnct_handle,
                                                     void                             *p_arg,
                                                     const USBH_USB2SER_SERIAL_STATUS serial_status);

static void Ex_USBH_USB2SER_LoopbackTxCmpl(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                           void                     *p_arg,
                                           CPU_INT08U               *p_buf,
                                           CPU_INT32U               buf_len,
                                           CPU_INT32U               xfer_len,
                                           RTOS_ERR                 err);

static void Ex_USBH_USB2SER_LoopbackTaskHandler(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 USB2SER application functions structure.
static const USBH_USB2SER_APP_FNCTS Ex_USBH_USB2SER_LoopbackAppFncts = {
  Ex_USBH_USB2SER_LoopbackConn,
  Ex_USBH_USB2SER_LoopbackDisconn,
  Ex_USBH_USB2SER_LoopbackDataRxd,
  Ex_USBH_USB2SER_LoopbackSerialStatusChng
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL Ex_USBH_USB2SER_LoopbackFnctPool;

static CPU_STK Ex_USBH_USB2SER_LoopbackTaskStk[EX_USBH_USB2SER_LOOPBACK_TASK_STK_SIZE];
static OS_TCB  Ex_USBH_USB2SER_LoopbackTaskTCB;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_USBH_USB2SER_Init()
 *
 * @brief  Example of initialization of the USB2SER class driver. Also creates the required tasks
 *         and dynamic memory pool of USB2SER function.
 *******************************************************************************************************/
void Ex_USBH_USB2SER_Init(void)
{
  RTOS_ERR err;

  USBH_USB2SER_Init(&Ex_USBH_USB2SER_LoopbackAppFncts, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create dynamic memory pool of USB2SER function.
  Mem_DynPoolCreate("Ex - USBH USB2SER loopback fnct pool",
                    &Ex_USBH_USB2SER_LoopbackFnctPool,
                    DEF_NULL,
                    sizeof(EX_USBH_USB2SER_LOOPBACK_FNCT),
                    sizeof(CPU_ALIGN),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,                  // Unlimited pool.
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  OSTaskCreate(&Ex_USBH_USB2SER_LoopbackTaskTCB,
               "Ex - USBH USB2SER loopback task",
               Ex_USBH_USB2SER_LoopbackTaskHandler,
               DEF_NULL,
               EX_USBH_USB2SER_LOOPBACK_TASK_PRIO,
               Ex_USBH_USB2SER_LoopbackTaskStk,
               EX_USBH_USB2SER_LOOPBACK_TASK_STK_SIZE / 10u,
               EX_USBH_USB2SER_LOOPBACK_TASK_STK_SIZE,
               50u,
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     Ex_USBH_USB2SER_LoopbackConn()
 *
 * @brief  A USB2SER function has been connected.
 *
 * @param  dev_handle           Handle on USB device.
 *
 * @param  fnct_handle          Handle on USB function.
 *
 * @param  usb2ser_fnct_handle  Handle on USB2SER class function handle.
 *
 * @return  Pointer to application specific data associated to USB2SER function.
 *******************************************************************************************************/
static void *Ex_USBH_USB2SER_LoopbackConn(USBH_DEV_HANDLE          dev_handle,
                                          USBH_FNCT_HANDLE         fnct_handle,
                                          USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle)
{
  RTOS_ERR                      err;
  EX_USBH_USB2SER_LOOPBACK_FNCT *p_fnct;

  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(fnct_handle);

  EX_TRACE("USB2SER example: Function connected\r\n");

  //                                                               ----------------- ALLOC FNCT DATA ------------------
  //                                                               Allocate data structure associated to USB2SER fnct.
  p_fnct = (EX_USBH_USB2SER_LOOPBACK_FNCT *)Mem_DynPoolBlkGet(&Ex_USBH_USB2SER_LoopbackFnctPool,
                                                              &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to allocate fnct data.\r\n");
    return (DEF_NULL);
  }

  p_fnct->CircularBufWrIx = 0u;
  p_fnct->CircularBufRdIx = 0u;
  p_fnct->Handle = usb2ser_fnct_handle;

  //                                                               --------------- SET SERIAL SETTINGS ----------------
  //                                                               Set USB2SER function's baudrate.
  USBH_USB2SER_BaudRateSet(usb2ser_fnct_handle,
                           115200u,                             // Use 115200 baud/sec by default.
                           &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to set baudrate\r\n");
  }

  //                                                               Set USB2SER function's data characteristics.
  USBH_USB2SER_DataSet(usb2ser_fnct_handle,
                       8u,                                      // 8 data bits.
                       USBH_USB2SER_PARITY_NONE,                // No parity.
                       USBH_USB2SER_STOP_BITS_1,                // 1 Stop bit.
                       &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to set data characteristics\r\n");
  }

  //                                                               Disable hardware (RTS/CTS, DTR/DSR) flow control.
  USBH_USB2SER_HW_FlowCtrlSet(usb2ser_fnct_handle,
                              USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE,
                              &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to disable hardware flow control\r\n");
  }

  //                                                               Disable software (xon/xoff) flow control.
  USBH_USB2SER_SW_FlowCtrlSet(usb2ser_fnct_handle,
                              DEF_DISABLED,
                              USBH_USB2SER_SW_FLOW_CTRL_XON_CHAR_DFLT,
                              USBH_USB2SER_SW_FLOW_CTRL_XOFF_CHAR_DFLT,
                              &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to disable software flow control\r\n");
  }

  return (p_fnct);
}

/****************************************************************************************************//**
 *                                    Ex_USBH_USB2SER_LoopbackDisconn()
 *
 * @brief  A USB2SER function has been disconnected.
 *
 * @param  usb2ser_fnct_handle  Handle on USB2SER class function handle.
 *
 * @param  p_arg                Pointer to application specific data associated to USB2SER
 *                              function returned to Ex_USBH_USB2SER_LoopbackConn().
 *******************************************************************************************************/
static void Ex_USBH_USB2SER_LoopbackDisconn(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                            void                     *p_arg)
{
  RTOS_ERR                      err;
  EX_USBH_USB2SER_LOOPBACK_FNCT *p_fnct = (EX_USBH_USB2SER_LOOPBACK_FNCT *)p_arg;

  PP_UNUSED_PARAM(usb2ser_fnct_handle);

  EX_TRACE("USB2SER example: Function disconnected\r\n");

  //                                                               ----------------- FREE FNCT DATA ------------------
  Mem_DynPoolBlkFree(&Ex_USBH_USB2SER_LoopbackFnctPool,
                     (void *)p_fnct,
                     &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to free fnct data.\r\n");
  }
}

/****************************************************************************************************//**
 *                                    Ex_USBH_USB2SER_LoopbackDataRxd()
 *
 * @brief  A USB2SER function has received new data.
 *
 * @param  usb2ser_fnct_handle  Handle on USB2SER class function handle.
 *
 * @param  p_arg                Pointer to application specific data associated to USB2SER
 *                              function returned to Ex_USBH_USB2SER_LoopbackConn().
 *
 * @param  p_buf                Pointer to buffer containing the received data.
 *
 * @param  buf_len              Buffer length in octets.
 *
 * @note   (1) It is not a good practice to attempt communication in this callback. Processing done
 *             here should be quick and not pend on anything (i.e. post another task).
 *******************************************************************************************************/
static void Ex_USBH_USB2SER_LoopbackDataRxd(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                            void                     *p_arg,
                                            CPU_INT08U               *p_buf,
                                            CPU_INT32U               buf_len)
{
  CPU_INT08U                    part1_len;
  CPU_INT08U                    part2_len;
  CPU_INT08U                    last_char;
  RTOS_ERR                      err;
  EX_USBH_USB2SER_LOOPBACK_FNCT *p_fnct = (EX_USBH_USB2SER_LOOPBACK_FNCT *)p_arg;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(usb2ser_fnct_handle);

  //                                                               Add received data to fnct's circular buf.
  CORE_ENTER_ATOMIC();
  if (p_fnct->CircularBufWrIx >= p_fnct->CircularBufRdIx) {
    part1_len = EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN - p_fnct->CircularBufWrIx;
    part2_len = p_fnct->CircularBufRdIx;
  } else {
    part1_len = p_fnct->CircularBufRdIx - p_fnct->CircularBufWrIx;
    part2_len = 0u;
  }

  if (buf_len > (CPU_INT32U)(part1_len + part2_len)) {
    CORE_EXIT_ATOMIC();
    EX_TRACE("USB2SER example: Unable to transmit received data, circular buf full\r\n");
    return;
  }
  CORE_EXIT_ATOMIC();

  Mem_Copy(&p_fnct->CircularBuf[p_fnct->CircularBufWrIx],
           p_buf,
           DEF_MAX(buf_len, part1_len));
  if (buf_len > part1_len) {
    Mem_Copy(&p_fnct->CircularBuf[0u],
             &p_buf[part1_len],
             buf_len - part1_len);
    CORE_ENTER_ATOMIC();
    p_fnct->CircularBufWrIx = buf_len - part1_len;
    CORE_EXIT_ATOMIC();
  } else {
    CORE_ENTER_ATOMIC();
    p_fnct->CircularBufWrIx += buf_len;
    p_fnct->CircularBufWrIx %= EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN;
    CORE_EXIT_ATOMIC();
  }

  //                                                               Post loopback task to send back received data.
  OSTaskQPost(&Ex_USBH_USB2SER_LoopbackTaskTCB,
              (void *)p_fnct,
              sizeof(void *),
              OS_OPT_NONE,
              &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to post to Q.\r\n");
  }

  //                                                               Print received data to console.
  last_char = p_buf[buf_len - 1u];
  p_buf[buf_len - 1u] = 0u;
  EX_TRACE("USB2SER example: Function data received: %s", p_buf);
  EX_TRACE("%c\r\n", last_char);
}

/****************************************************************************************************//**
 *                               Ex_USBH_USB2SER_LoopbackSerialStatusChng()
 *
 * @brief  A USB2SER function's serial status has changed.
 *
 * @param  usb2ser_fnct_handle  Handle on USB2SER class function handle.
 *
 * @param  p_arg                Pointer to application specific data associated to USB2SER
 *                              function returned to Ex_USBH_USB2SER_LoopbackConn().
 *
 * @param  serial_status        New serial status.
 *******************************************************************************************************/
static void Ex_USBH_USB2SER_LoopbackSerialStatusChng(USBH_USB2SER_FNCT_HANDLE         usb2ser_fnct_handle,
                                                     void                             *p_arg,
                                                     const USBH_USB2SER_SERIAL_STATUS serial_status)
{
  PP_UNUSED_PARAM(usb2ser_fnct_handle);
  PP_UNUSED_PARAM(p_arg);

  //                                                               Print modem status.
  EX_TRACE("USB2SER example: Function serial status changed:\r\n");
  EX_TRACE("+-------------- Modem status -------------+\r\n");
  EX_TRACE("| Clear To Send (CTS)               | %s |\r\n", DEF_BIT_IS_SET(serial_status.Modem, USBH_USB2SER_MODEM_STATUS_CTS)     ? "SET" : "CLR");
  EX_TRACE("| Data Set Ready (DSR)              | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Modem, USBH_USB2SER_MODEM_STATUS_DSR)     ? "SET" : "CLR"));
  EX_TRACE("| Ring Indicator (RI)               | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Modem, USBH_USB2SER_MODEM_STATUS_RING)    ? "SET" : "CLR"));
  EX_TRACE("| Receive Line Signal Detect (RLSD) | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Modem, USBH_USB2SER_MODEM_STATUS_CARRIER) ? "SET" : "CLR"));
  EX_TRACE("+-----------------------------------------+\r\n\r\n");

  //                                                               Print line status.
  EX_TRACE("+-------- Line status -------+\r\n");
  EX_TRACE("| Overrun Error (OE)   | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Line, USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR) ? "SET" : "CLR"));
  EX_TRACE("| Parity Error (PE)    | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Line, USBH_USB2SER_LINE_STATUS_PARITY_ERR)      ? "SET" : "CLR"));
  EX_TRACE("| Framing Error (FE)   | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Line, USBH_USB2SER_LINE_STATUS_FRAMING_ERR)     ? "SET" : "CLR"));
  EX_TRACE("| Break Interrupt (BI) | %s |\r\n",
           (DEF_BIT_IS_SET(serial_status.Line, USBH_USB2SER_LINE_STATUS_BRK_INT)         ? "SET" : "CLR"));
  EX_TRACE("+----------------------------+\r\n");
}

/****************************************************************************************************//**
 *                                    Ex_USBH_USB2SER_LoopbackTxCmpl()
 *
 * @brief  This function is called once data transmission is completed.
 *
 * @param  usb2ser_fnct_handle  Handle on USB2SER function.
 *
 * @param  p_arg                Pointer to argument passed to USBH_USB2SER_TxAsync().
 *
 * @param  p_buf                Pointer to transmitted buffer.
 *
 * @param  buf_len              Length, in octets, of submitted buffer.
 *
 * @param  xfer_len             Length, in octets, of transmitted buffer.
 *
 * @param  err                  Error code indicating if transfer was successful.
 *******************************************************************************************************/
static void Ex_USBH_USB2SER_LoopbackTxCmpl(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                           void                     *p_arg,
                                           CPU_INT08U               *p_buf,
                                           CPU_INT32U               buf_len,
                                           CPU_INT32U               xfer_len,
                                           RTOS_ERR                 err)
{
  RTOS_ERR err_kernel;

  PP_UNUSED_PARAM(usb2ser_fnct_handle);
  PP_UNUSED_PARAM(p_arg);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(xfer_len);

  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to complete transmission of data.\r\n");
  }

  OSTaskSemPost(&Ex_USBH_USB2SER_LoopbackTaskTCB,
                OS_OPT_NONE,
                &err_kernel);
  if (err_kernel.Code != RTOS_ERR_NONE) {
    EX_TRACE("USB2SER example: Unable to post loopback task semaphore.\r\n");
  }
}

/****************************************************************************************************//**
 *                                  Ex_USBH_USB2SER_LoopbackTaskHandler()
 *
 * @brief  Loopback task handler.
 *
 * @param  p_arg  Pointer to task's argument.
 *******************************************************************************************************/
static void Ex_USBH_USB2SER_LoopbackTaskHandler(void *p_arg)
{
  while (DEF_ON) {
    CPU_INT08U                    part1_len;
    CPU_INT08U                    part2_len;
    OS_MSG_SIZE                   msg_size;
    EX_USBH_USB2SER_LOOPBACK_FNCT *p_fnct;
    RTOS_ERR                      err;
    CORE_DECLARE_IRQ_STATE;

    PP_UNUSED_PARAM(p_arg);

    //                                                             Wait for data reception on a USB2SER function.
    p_fnct = (EX_USBH_USB2SER_LOOPBACK_FNCT *)OSTaskQPend(0u,
                                                          OS_OPT_NONE,
                                                          &msg_size,
                                                          DEF_NULL,
                                                          &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("USB2SER example: Unable to receive from Q\r\n");
      continue;
    }

    CORE_ENTER_ATOMIC();
    if (p_fnct->CircularBufRdIx >= p_fnct->CircularBufWrIx) {
      part1_len = EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN - p_fnct->CircularBufRdIx;
      part2_len = p_fnct->CircularBufWrIx;
    } else {
      part1_len = p_fnct->CircularBufWrIx - p_fnct->CircularBufRdIx;
      part2_len = 0u;
    }
    CORE_EXIT_ATOMIC();

    //                                                             Send back received data.
    if (part1_len > 0u) {
      USBH_USB2SER_TxAsync(p_fnct->Handle,
                           &p_fnct->CircularBuf[p_fnct->CircularBufRdIx],
                           part1_len,
                           Ex_USBH_USB2SER_LoopbackTxCmpl,
                           DEF_NULL,
                           &err);
      if (err.Code == RTOS_ERR_NONE) {
        //                                                         Wait for transfer completion.
        OSTaskSemPend(0u,
                      OS_OPT_NONE,
                      DEF_NULL,
                      &err);
        if (err.Code != RTOS_ERR_NONE) {
          EX_TRACE("USB2SER example: Failed to pend on loopback task semaphore\r\n");
        }
      } else {
        EX_TRACE("USB2SER example: Unable to transmit received data\r\n");
        continue;
      }
    }

    if (part2_len > 0u) {
      USBH_USB2SER_TxAsync(p_fnct->Handle,
                           &p_fnct->CircularBuf[0u],
                           part2_len,
                           Ex_USBH_USB2SER_LoopbackTxCmpl,
                           DEF_NULL,
                           &err);
      if (err.Code == RTOS_ERR_NONE) {
        //                                                         Wait for transfer completion.
        OSTaskSemPend(0u,
                      OS_OPT_NONE,
                      DEF_NULL,
                      &err);
        if (err.Code != RTOS_ERR_NONE) {
          EX_TRACE("USB2SER example: Failed to pend on loopback task semaphore\r\n");
        }
      } else {
        EX_TRACE("USB2SER example: Unable to transmit received data\r\n");
      }

      CORE_ENTER_ATOMIC();
      p_fnct->CircularBufRdIx = part2_len;
      CORE_EXIT_ATOMIC();
    } else {
      CORE_ENTER_ATOMIC();
      p_fnct->CircularBufRdIx += part1_len;
      p_fnct->CircularBufRdIx %= EX_USBH_USB2SER_LOOPBACK_CIRCULAR_BUF_LEN;
      CORE_EXIT_ATOMIC();
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_HOST_USB2SER_AVAIL
