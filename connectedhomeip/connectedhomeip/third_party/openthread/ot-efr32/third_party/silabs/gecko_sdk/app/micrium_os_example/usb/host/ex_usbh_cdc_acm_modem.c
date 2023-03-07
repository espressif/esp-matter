/***************************************************************************//**
 * @file
 * @brief USB Host Example - CDC Abstract Control Model - Modem example
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
 * The CDC ACM class driver is meant to be used with devices such as USB modems and certain USB-to-serial
 * adapters that implements the CDC ACM class.
 *
 * This example application will issue a series of AT Commands to the connected CDC ACM USB modem. It
 * requires a modem that supports AT commands to work properly.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_ACM_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_cdc.h>
#include  <usb/include/host/usbh_acm.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBH_CDC_ACM_MODEM_TASK_PRIO                    21u
#define  EX_USBH_CDC_ACM_MODEM_TASK_STK_SIZE                512u

#define  EX_USBH_CDC_ACM_MODEM_NBR_AT_CMD                   6u

#define  EX_USBH_CDC_ACM_MODEM_CMDS_BUF_LEN                 128u

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
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Table of AT commands that will be issued to modem.
static const CPU_INT08U Ex_USBH_CDC_ACM_ModemAT_CmdsTbl[][10u] = {
  { 'A', 'T', 'Q', '0', 'V', '1', 'E', '0', '\r', '\0' },
  { 'A', 'T', 'I', '0', '\r', '\0' },
  { 'A', 'T', 'I', '1', '\r', '\0' },
  { 'A', 'T', 'I', '2', '\r', '\0' },
  { 'A', 'T', 'I', '3', '\r', '\0' },
  { 'A', 'T', 'I', '7', '\r', '\0' }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT08U *Ex_USBH_CDC_ACM_ModemCmdsBufPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void *Ex_USBH_CDC_ACM_ModemConn(USBH_DEV_HANDLE      dev_handle,
                                       USBH_FNCT_HANDLE     fnct_handle,
                                       USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                       USBH_ACM_FNCT_HANDLE acm_fnct_handle);

static void Ex_USBH_CDC_ACM_ModemDisconn(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                         void                 *p_arg);

static void Ex_USBH_CDC_ACM_ModemNetConn(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                         void                 *p_arg,
                                         CPU_BOOLEAN          is_conn);

static void Ex_USBH_CDC_ACM_ModemRespAvail(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                           void                 *p_arg);

static void Ex_USBH_CDC_ACM_ModemSerialStateChng(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                                 void                 *p_arg,
                                                 CPU_INT08U           serial_state);

static void Ex_USBH_CDC_ACM_ModemTaskHandler(void *p_arg);

static void Ex_USBH_CDC_ACM_ModemTxCmpl(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                        CPU_INT08U           *p_buf,
                                        CPU_INT32U           buf_len,
                                        CPU_INT32U           xfer_len,
                                        void                 *p_arg,
                                        RTOS_ERR             err);

static void Ex_USBH_CDC_ACM_ModemRxCmpl(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                        CPU_INT08U           *p_buf,
                                        CPU_INT32U           buf_len,
                                        CPU_INT32U           xfer_len,
                                        void                 *p_arg,
                                        RTOS_ERR             err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 CDC ACM application functions structure.
static const USBH_ACM_APP_FNCTS Ex_USBH_CDC_ACM_ModemAppFncts = {
  Ex_USBH_CDC_ACM_ModemConn,
  Ex_USBH_CDC_ACM_ModemDisconn,
  Ex_USBH_CDC_ACM_ModemNetConn,
  Ex_USBH_CDC_ACM_ModemRespAvail,
  Ex_USBH_CDC_ACM_ModemSerialStateChng
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Modem task stack and TCB.
static CPU_STK Ex_USBH_CDC_ACM_ModemTaskStk[EX_USBH_CDC_ACM_MODEM_TASK_STK_SIZE];
static OS_TCB  Ex_USBH_CDC_ACM_ModemTaskTCB;

static CPU_INT32U Ex_USBH_CDC_ACM_ModemRxXferLen;               // Receive transfer length.

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_USBH_CDC_ACM_Init()
 *
 * @brief  Example of initialization of the CDC ACM class driver.
 *******************************************************************************************************/
void Ex_USBH_CDC_ACM_Init(void)
{
  RTOS_ERR err;

  //                                                               Initializes CDC base class and ACM subclass.
  USBH_CDC_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  USBH_ACM_Init(&Ex_USBH_CDC_ACM_ModemAppFncts, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  USBH_CDC_PostInit(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Allocate buffer used for AT commands.
  Ex_USBH_CDC_ACM_ModemCmdsBufPtr = (CPU_INT08U *)Mem_SegAllocHW("Ex - USBH CDC ACM Modem cmd buf",
                                                                 DEF_NULL,
                                                                 EX_USBH_CDC_ACM_MODEM_CMDS_BUF_LEN,
                                                                 sizeof(CPU_ALIGN),
                                                                 DEF_NULL,
                                                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create task used to communicate with modem.
  OSTaskCreate(&Ex_USBH_CDC_ACM_ModemTaskTCB,
               "Ex - USBH CDC ACM modem task",
               Ex_USBH_CDC_ACM_ModemTaskHandler,
               DEF_NULL,
               EX_USBH_CDC_ACM_MODEM_TASK_PRIO,
               Ex_USBH_CDC_ACM_ModemTaskStk,
               EX_USBH_CDC_ACM_MODEM_TASK_STK_SIZE / 10u,
               EX_USBH_CDC_ACM_MODEM_TASK_STK_SIZE,
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
 *                                       Ex_USBH_CDC_ACM_ModemConn()
 *
 * @brief  A CDC ACM function has been connected.
 *
 * @param  dev_handle       Handle on USB device.
 *
 * @param  fnct_handle      Handle on USB function.
 *
 * @param  cdc_fnct_handle  Handle on CDC class function handle.
 *
 * @param  acm_fnct_handle  Handle on ACM subclass function handle.
 *
 * @return  Pointer to application specific data associated to CDC ACM function.
 *******************************************************************************************************/
static void *Ex_USBH_CDC_ACM_ModemConn(USBH_DEV_HANDLE      dev_handle,
                                       USBH_FNCT_HANDLE     fnct_handle,
                                       USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                       USBH_ACM_FNCT_HANDLE acm_fnct_handle)
{
  RTOS_ERR err;

  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(fnct_handle);
  PP_UNUSED_PARAM(cdc_fnct_handle);

  EX_TRACE("CDC ACM example: Function connected\r\n");

  //                                                               Post modem task so series of AT cmds will be issued.
  OSTaskQPost(&Ex_USBH_CDC_ACM_ModemTaskTCB,
              (void *)(CPU_ADDR)acm_fnct_handle,                // Pass CDC ACM function handle directly.
              0u,
              OS_OPT_NONE,
              &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                     Ex_USBH_CDC_ACM_ModemDisconn()
 *
 * @brief  A CDC ACM function has been disconnected.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM class function handle.
 *
 * @param  p_arg            Pointer to application specific data associated to CDC ACM
 *                          function returned to Ex_USBH_CDC_ACM_ModemConn().
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemDisconn(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                         void                 *p_arg)
{
  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_arg);

  EX_TRACE("CDC ACM example: Function disconnected\r\n");
}

/****************************************************************************************************//**
 *                                     Ex_USBH_CDC_ACM_ModemNetConn()
 *
 * @brief  A CDC ACM modem network connection state has changed.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM class function handle.
 *
 * @param  p_arg            Pointer to application specific data associated to CDC ACM
 *                          function returned to Ex_USBH_CDC_ACM_ModemConn().
 *
 * @param  is_conn          Indicates if device connected to network or not.
 *                          DEF_YES: Device connected.
 *                          DEF_NO:  Device disconnected.
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemNetConn(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                         void                 *p_arg,
                                         CPU_BOOLEAN          is_conn)
{
  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_arg);
  PP_UNUSED_PARAM(is_conn);

  EX_TRACE("CDC ACM example: Modem network connection notification\r\n");
}

/****************************************************************************************************//**
 *                                    Ex_USBH_CDC_ACM_ModemRespAvail()
 *
 * @brief  A CDC ACM Modem response is available.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM class function handle.
 *
 * @param  p_arg            Pointer to application specific data associated to CDC ACM
 *                          function returned to Ex_USBH_CDC_ACM_ModemConn().
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemRespAvail(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                           void                 *p_arg)
{
  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_arg);

  EX_TRACE("CDC ACM example: Modem response available\r\n");
}

/****************************************************************************************************//**
 *                                 Ex_USBH_CDC_ACM_ModemSerialStateChng()
 *
 * @brief  CDC ACM serial state has changed.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM class function handle.
 *
 * @param  p_arg            Pointer to application specific data associated to USB2SER
 *                          function returned to Ex_USBH_USB2SER_LoopbackConn().
 *
 * @param  serial_state     New serial state bitmap.
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemSerialStateChng(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                                 void                 *p_arg,
                                                 CPU_INT08U           serial_state)
{
  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_arg);

  //                                                               Display serial state.
  EX_TRACE("CDC ACM example: Function serial state changed:\r\n");
  EX_TRACE("+-------- Serial state -------+\r\n");
  EX_TRACE("| RX Carrier              | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_RXCARRIER)  ? "SET" : "CLR"));
  EX_TRACE("| TX Carrier              | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_TXCARRIER)  ? "SET" : "CLR"));
  EX_TRACE("| Break signal            | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_BRK)        ? "SET" : "CLR"));
  EX_TRACE("| Ring signal             | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_RINGSIGNAL) ? "SET" : "CLR"));
  EX_TRACE("| Framing error           | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_FRAMING)    ? "SET" : "CLR"));
  EX_TRACE("| Parity error            | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_PARITY)     ? "SET" : "CLR"));
  EX_TRACE("| Data overrun error      | %s |\r\n",
           (DEF_BIT_IS_SET(serial_state, USBH_CDC_SERIAL_STATE_OVERRUN)    ? "SET" : "CLR"));
  EX_TRACE("+-----------------------------+\r\n\r\n");
}

/****************************************************************************************************//**
 *                                   Ex_USBH_CDC_ACM_ModemTaskHandler()
 *
 * @brief  CDC ACM modem example task handler.
 *
 * @param  p_arg  Task argument.
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemTaskHandler(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    CPU_INT08U           i;
    USBH_ACM_FNCT_HANDLE acm_fnct_handle;
    USBH_CDC_LINECODING  line_coding_set;
    OS_MSG_SIZE          msg_size;
    RTOS_ERR             err;

    //                                                             Wait for a CDC ACM function connection.
    acm_fnct_handle = (USBH_ACM_FNCT_HANDLE)(CPU_ADDR)OSTaskQPend(0u,
                                                                  OS_OPT_NONE,
                                                                  &msg_size,
                                                                  DEF_NULL,
                                                                  &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("CDC ACM example: Unable to receive from Q\r\n");
      continue;
    }

    //                                                             Set proper line coding.
    line_coding_set.Rate = USBH_CDC_LINECODING_RATE_9600;
    line_coding_set.CharFmt = USBH_CDC_LINECODING_STOP_BIT_2;
    line_coding_set.ParityType = USBH_CDC_LINECODING_PARITY_NONE;
    line_coding_set.DataBit = USBH_CDC_LINECODING_DATA_BIT_8;
    USBH_ACM_LineCodingSet(acm_fnct_handle,
                           &line_coding_set,
                           5000u,
                           &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("CDC ACM example: Unable to set line coding\r\n");
      continue;
    }

    //                                                             Activate Carrier and indicates DCE that DTE present.
    USBH_ACM_CtrlLineStateSet(acm_fnct_handle,
                              (USBH_CDC_CTRL_LINE_STATE_CARRIER | USBH_CDC_CTRL_LINE_STATE_DTE),
                              5000u,
                              &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("CDC ACM example: Unable to set line state\r\n");
      continue;
    }

    //                                                             Issue AT cmds and read result.
    for (i = 0u; i < EX_USBH_CDC_ACM_MODEM_NBR_AT_CMD; i++) {
      CPU_SIZE_T at_cmd_len;
      CPU_INT32U ix;

      EX_TRACE("------------------------------------------------\r\n");
      EX_TRACE("               --- AT CMD ---\r\n");
      EX_TRACE("%s\r\n", Ex_USBH_CDC_ACM_ModemAT_CmdsTbl[i]);

      at_cmd_len = Str_Len((CPU_CHAR *)Ex_USBH_CDC_ACM_ModemAT_CmdsTbl[i]);
      at_cmd_len = DEF_MIN(EX_USBH_CDC_ACM_MODEM_CMDS_BUF_LEN, at_cmd_len);

      Mem_Copy((void *)Ex_USBH_CDC_ACM_ModemCmdsBufPtr,
               (void *)Ex_USBH_CDC_ACM_ModemAT_CmdsTbl[i],
               at_cmd_len);

      //                                                           Transmit AT command.
      USBH_ACM_TxAsync(acm_fnct_handle,
                       Ex_USBH_CDC_ACM_ModemCmdsBufPtr,
                       at_cmd_len,
                       Ex_USBH_CDC_ACM_ModemTxCmpl,
                       DEF_NULL,
                       &err);
      if (err.Code != RTOS_ERR_NONE) {
        EX_TRACE("CDC ACM example: Error in USBH_CDC_ACM_DataTx()\r\n");
        continue;
      }

      //                                                           Wait for transmission to complete.
      OSTaskSemPend(0u,
                    OS_OPT_NONE,
                    DEF_NULL,
                    &err);
      if (err.Code != RTOS_ERR_NONE) {
        EX_TRACE("CDC ACM example: Error while waiting for AT cmd transmission to complete\r\n");
        continue;
      }

      //                                                           Receive response from CDC ACM function.
      USBH_ACM_RxAsync(acm_fnct_handle,
                       Ex_USBH_CDC_ACM_ModemCmdsBufPtr,
                       EX_USBH_CDC_ACM_MODEM_CMDS_BUF_LEN,
                       Ex_USBH_CDC_ACM_ModemRxCmpl,
                       DEF_NULL,
                       &err);
      if (err.Code != RTOS_ERR_NONE) {
        EX_TRACE("CDC ACM example: Error in USBH_CDC_ACM_DataRx()");
        continue;
      }

      //                                                           Wait for reception of response.
      OSTaskSemPend(0u,
                    OS_OPT_NONE,
                    DEF_NULL,
                    &err);
      if (err.Code != RTOS_ERR_NONE) {
        EX_TRACE("CDC ACM example: Error while waiting for AT cmd response reception to complete\r\n");
        continue;
      }

      //                                                           Print received data to console.
      EX_TRACE("            --- DATA RECEIVED ---\r\n");
      for (ix = 0u; ix < Ex_USBH_CDC_ACM_ModemRxXferLen; ix++) {
        printf("%c", (CPU_CHAR)Ex_USBH_CDC_ACM_ModemCmdsBufPtr[ix]);
      }
      EX_TRACE("\r\n------------------------------------------------\r\n\r\n");
    }
  }
}

/****************************************************************************************************//**
 *                                      Ex_USBH_CDC_ACM_ModemTxCmpl()
 *
 * @brief  Data transmission to CDC ACM function completed.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM function.
 *
 * @param  p_buf            Pointer to transmitted buffer.
 *
 * @param  buf_len          Length, in octets, of submitted buffer.
 *
 * @param  xfer_len         Length, in octets, of transmitted buffer.
 *
 * @param  p_arg            Pointer to application argument passed to USBH_ACM_TxAsync().
 *
 * @param  err              Error code indicating if transfer was successful.
 *******************************************************************************************************/
static void Ex_USBH_CDC_ACM_ModemTxCmpl(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                        CPU_INT08U           *p_buf,
                                        CPU_INT32U           buf_len,
                                        CPU_INT32U           xfer_len,
                                        void                 *p_arg,
                                        RTOS_ERR             err)
{
  RTOS_ERR err_kernel;

  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(xfer_len);
  PP_UNUSED_PARAM(p_arg);

  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("CDC ACM example: Failed to transmit data\r\n");
  }

  OSTaskSemPost(&Ex_USBH_CDC_ACM_ModemTaskTCB,
                OS_OPT_NONE,
                &err_kernel);
}

/****************************************************************************************************//**
 *                                      Ex_USBH_CDC_ACM_ModemRxCmpl()
 *
 * @brief  Data reception from CDC ACM function completed.
 *
 * @param  acm_fnct_handle  Handle on CDC ACM function.
 *
 * @param  p_buf            Pointer to reception buffer.
 *
 * @param  buf_len          Length, in octets, of submitted buffer.
 *
 * @param  xfer_len         Length, in octets, of received buffer.
 *
 * @param  p_arg            Pointer to application argument passed to USBH_ACM_RxAsync().
 *
 * @param  err              Error code indicating if transfer was successful.
 *******************************************************************************************************/
void Ex_USBH_CDC_ACM_ModemRxCmpl(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                 CPU_INT08U           *p_buf,
                                 CPU_INT32U           buf_len,
                                 CPU_INT32U           xfer_len,
                                 void                 *p_arg,
                                 RTOS_ERR             err)
{
  RTOS_ERR err_kernel;

  PP_UNUSED_PARAM(acm_fnct_handle);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(p_arg);

  if (err.Code == RTOS_ERR_NONE) {
    Ex_USBH_CDC_ACM_ModemRxXferLen = xfer_len;
  } else {
    Ex_USBH_CDC_ACM_ModemRxXferLen = 0u;
    EX_TRACE("CDC ACM example: Failed to receive data\r\n");
  }

  OSTaskSemPost(&Ex_USBH_CDC_ACM_ModemTaskTCB,
                OS_OPT_NONE,
                &err_kernel);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_HOST_ACM_AVAIL
