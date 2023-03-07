/***************************************************************************//**
 * @file
 * @brief USB Device Example - USB CDC Abstract Control Model - Simple serial terminal
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
 * Note(s) : (1) This creates a COM port on your PC. Once connected on the port using a terminal tool
 *               (such as Hyperterminal), a menu is displayed and let's you echo any character you typed
 *               in.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_ACM_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_ascii.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_cdc_acm_serial.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         TERMINAL TASK CFGS
 *******************************************************************************************************/

#define  EX_USBD_CDC_ACM_TERMINAL_TASK_PRIO               21u
#define  EX_USBD_CDC_ACM_TERMINAL_TASK_STK_SIZE           512u

/********************************************************************************************************
 *                                       MENU MESSAGE AND LENGTH
 *******************************************************************************************************/

#define  EX_USBD_CDC_ACM_TERMINAL_BUF_LEN                           512u
#define  EX_USBD_CDC_ACM_TERMINAL_SCREEN_SIZE                        80u

#define  EX_USBD_CDC_ACM_TERMINAL_CURSOR_START                     "\033[H"
#define  EX_USBD_CDC_ACM_TERMINAL_CURSOR_START_SIZE                   3u

#define  EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR                "\033[2J\033[H"
#define  EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE                     7u

#define  EX_USBD_CDC_ACM_TERMINAL_MSG                       "===== USB CDC ACM Serial Emulation Demo ======" \
                                                            "\r\n"                                           \
                                                            "\r\n"                                           \
                                                            "1. Echo 1 demo.\r\n"                            \
                                                            "2. Echo N demo.\r\n"                            \
                                                            "Option: "
#define  EX_USBD_CDC_ACM_TERMINAL_MSG_SIZE                           92u

#define  EX_USBD_CDC_ACM_TERMINAL_MSG1                      "Echo 1 demo... \r\n\r\n>> "
#define  EX_USBD_CDC_ACM_TERMINAL_MSG1_SIZE                          22u

#define  EX_USBD_CDC_ACM_TERMINAL_MSG2                      "Echo N demo. You can send up to 512 characters at once... \r\n\r\n>> "
#define  EX_USBD_CDC_ACM_TERMINAL_MSG2_SIZE                          65u

#define  EX_USBD_CDC_ACM_TERMINAL_NEW_LINE                  "\n\r>> "
#define  EX_USBD_CDC_ACM_TERMINAL_NEW_LINE_SIZE                       5u

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   ACM SERIAL DEMO STATE DATA TYPE
 *******************************************************************************************************/

//                                                                 --------------- TERMINAL MENU STATES ---------------
typedef  enum  ex_usbd_cdc_acm_terminal_state {
  EX_USBD_CDC_ACM_TERMINAL_STATE_MENU = 0u,
  EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_1,
  EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_N
} EX_USBD_CDC_ACM_TERMINAL_STATE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_USBD_CDC_ACM_TerminalTask(void *p_arg);

static void Ex_USBD_CDC_ACM_TerminalLineCtrl(CPU_INT08U subclass_nbr,
                                             CPU_INT08U events,
                                             CPU_INT08U events_chngd,
                                             void       *p_arg);

static CPU_BOOLEAN Ex_USBD_CDC_ACM_TerminalLineCoding(CPU_INT08U                  subclass_nbr,
                                                      USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                                      void                        *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Terminal task's tcb and stack.
static OS_TCB  Ex_USBD_CDC_ACM_TerminalTaskTCB;
static CPU_STK Ex_USBD_CDC_ACM_TerminalTaskStk[EX_USBD_CDC_ACM_TERMINAL_TASK_STK_SIZE];

//                                                                 Universal buf used to transmit and recevie data.
//                                                                 TODO: Ensure buf aligned for USB ctrlr needs.
static CPU_INT08U Ex_USBD_CDC_ACM_TerminalBuf[EX_USBD_CDC_ACM_TERMINAL_BUF_LEN];

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_USBD_ACM_SerialInit()
 *
 * @brief  Adds a CDC ACM interface to the device and creates a task to handle the terminal
 *         functionnalities.
 *
 * @param  dev_nbr        Device number.
 *
 * @param  config_nbr_fs  Full-Speed configuration number.
 *
 * @param  config_nbr_hs  High-Speed configuration number.
 *******************************************************************************************************/
void Ex_USBD_ACM_SerialInit(CPU_INT08U dev_nbr,
                            CPU_INT08U config_nbr_fs,
                            CPU_INT08U config_nbr_hs)
{
  CPU_INT08U       cdc_acm_nbr;
  USBD_CDC_QTY_CFG cfg_qty_cdc;
  RTOS_ERR         err;

  //                                                               --------------- INIT CDC BASE CLASS ----------------
  cfg_qty_cdc.ClassInstanceQty = 1u;
  cfg_qty_cdc.ConfigQty = 2u;
  cfg_qty_cdc.DataIF_Qty = 2u;

  USBD_CDC_Init(&cfg_qty_cdc, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- INIT ACM SUBCLASS -----------------
  USBD_ACM_SerialInit(1u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------- CREATE CDC ACM CLASS INSTANCE -----------
  cdc_acm_nbr = USBD_ACM_SerialAdd(64u,
                                   (USBD_ACM_SERIAL_CALL_MGMT_DATA_CCI_DCI | USBD_ACM_SERIAL_CALL_MGMT_DEV),
                                   &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------- ADD CLASS INSTANCE TO CONFIGS -----------
  USBD_ACM_SerialConfigAdd(cdc_acm_nbr,
                           dev_nbr,
                           config_nbr_fs,
                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (config_nbr_hs != USBD_CONFIG_NBR_NONE) {                  // Add instance to HS config if available.
    USBD_ACM_SerialConfigAdd(cdc_acm_nbr,
                             dev_nbr,
                             config_nbr_hs,
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

  //                                                               Register line coding and ctrl line change callbacks.
  //                                                               These 2 calls are optinnal.
  USBD_ACM_SerialLineCodingReg(cdc_acm_nbr,
                               Ex_USBD_CDC_ACM_TerminalLineCoding,
                               DEF_NULL,
                               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  USBD_ACM_SerialLineCtrlReg(cdc_acm_nbr,
                             Ex_USBD_CDC_ACM_TerminalLineCtrl,
                             DEF_NULL,
                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  OSTaskCreate(&Ex_USBD_CDC_ACM_TerminalTaskTCB,
               "USB Device CDC ACM Terminal",
               Ex_USBD_CDC_ACM_TerminalTask,
               (void *)(CPU_ADDR)cdc_acm_nbr,
               EX_USBD_CDC_ACM_TERMINAL_TASK_PRIO,
               Ex_USBD_CDC_ACM_TerminalTaskStk,
               EX_USBD_CDC_ACM_TERMINAL_TASK_STK_SIZE / 10u,
               EX_USBD_CDC_ACM_TERMINAL_TASK_STK_SIZE,
               0u,
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
 *                                     Ex_USBD_CDC_ACM_TerminalTask()
 *
 * @brief  USB CDC ACM terminal emulation demo task.
 *
 * @param  p_arg  Task argument pointer.
 *
 * @note   (1) This task manages the display of the terminal in according to the user's inputs.
 *******************************************************************************************************/
static void Ex_USBD_CDC_ACM_TerminalTask(void *p_arg)
{
  CPU_INT08U                     ch;
  CPU_BOOLEAN                    conn;
  EX_USBD_CDC_ACM_TERMINAL_STATE state = EX_USBD_CDC_ACM_TERMINAL_STATE_MENU;
  CPU_INT08U                     cdc_acm_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;
  CPU_INT08U                     line_ctr = 0u;
  CPU_INT08U                     line_state;
  CPU_INT32U                     xfer_len;
  RTOS_ERR                       err;

  while (DEF_ON) {
    //                                                             Wait until device is in cfg'd state.
    conn = USBD_ACM_SerialIsConn(cdc_acm_nbr);
    line_state = USBD_ACM_SerialLineCtrlGet(cdc_acm_nbr, &err);

    while ((conn != DEF_YES)
           || (DEF_BIT_IS_CLR(line_state, USBD_ACM_SERIAL_CTRL_DTR))
           || (err.Code != RTOS_ERR_NONE)) {
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);

      conn = USBD_ACM_SerialIsConn(cdc_acm_nbr);
      line_state = USBD_ACM_SerialLineCtrlGet(cdc_acm_nbr, &err);
    }

    switch (state) {
      case EX_USBD_CDC_ACM_TERMINAL_STATE_MENU:
        //                                                         Display start cursor.
        Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                 EX_USBD_CDC_ACM_TERMINAL_CURSOR_START,
                 EX_USBD_CDC_ACM_TERMINAL_CURSOR_START_SIZE);

        (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                Ex_USBD_CDC_ACM_TerminalBuf,
                                EX_USBD_CDC_ACM_TERMINAL_CURSOR_START_SIZE,
                                0u,
                                &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        //                                                         Display main menu.
        Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                 EX_USBD_CDC_ACM_TERMINAL_MSG,
                 EX_USBD_CDC_ACM_TERMINAL_MSG_SIZE);

        (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                Ex_USBD_CDC_ACM_TerminalBuf,
                                EX_USBD_CDC_ACM_TERMINAL_MSG_SIZE,
                                0u,
                                &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        //                                                         Wait for character.
        (void)USBD_ACM_SerialRx(cdc_acm_nbr,
                                Ex_USBD_CDC_ACM_TerminalBuf,
                                1u,
                                0u,
                                &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        ch = Ex_USBD_CDC_ACM_TerminalBuf[0u];

        //                                                         Echo back character.
        (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                Ex_USBD_CDC_ACM_TerminalBuf,
                                1u,
                                0u,
                                &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        switch (ch) {                                           // Select demo options.
          case '1':                                             // Echo one character.

            //                                                     Clear screen.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR,
                     EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            //                                                     Display option 1 instructions.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_MSG1,
                     EX_USBD_CDC_ACM_TERMINAL_MSG1_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_MSG1_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            state = EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_1;
            line_ctr = 0u;
            break;

          case '2':                                             // Echon 'N' charachters.

            //                                                     Clear screen.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR,
                     EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            //                                                     Display option 2 instructions.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_MSG2,
                     EX_USBD_CDC_ACM_TERMINAL_MSG2_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_MSG2_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            state = EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_N;
            line_ctr = 0u;
            break;

          default:
            break;
        }
        break;

      case EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_1:               // 'Echo 1' state.
        (void)USBD_ACM_SerialRx(cdc_acm_nbr,                    // Wait for character.
                                Ex_USBD_CDC_ACM_TerminalBuf,
                                1u,
                                0u,
                                &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        ch = Ex_USBD_CDC_ACM_TerminalBuf[0u];

        if (ch == ASCII_CHAR_END_OF_TEXT) {                     // If 'Ctrl-c' character is received.
                                                                // ... return to 'menu' state.
          state = EX_USBD_CDC_ACM_TERMINAL_STATE_MENU;

          //                                                       Clear screen.
          Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                   EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR,
                   EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE);

          (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                  Ex_USBD_CDC_ACM_TerminalBuf,
                                  EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE,
                                  0u,
                                  &err);
          if (err.Code != RTOS_ERR_NONE) {
            break;
          }
        } else {
          //                                                       Echo back character.
          (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                  Ex_USBD_CDC_ACM_TerminalBuf,
                                  1u,
                                  0u,
                                  &err);
          if (err.Code != RTOS_ERR_NONE) {
            break;
          }

          line_ctr++;

          if (line_ctr == EX_USBD_CDC_ACM_TERMINAL_SCREEN_SIZE - 3u) {
            //                                                     Move to next line.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_NEW_LINE,
                     EX_USBD_CDC_ACM_TERMINAL_NEW_LINE_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_NEW_LINE_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            line_ctr = 0u;
          }
        }
        break;

      case EX_USBD_CDC_ACM_TERMINAL_STATE_ECHO_N:               // 'Echo N' state.
        xfer_len = USBD_ACM_SerialRx(cdc_acm_nbr,               // Wait for N characters.
                                     Ex_USBD_CDC_ACM_TerminalBuf,
                                     EX_USBD_CDC_ACM_TERMINAL_BUF_LEN,
                                     0u,
                                     &err);
        if (err.Code != RTOS_ERR_NONE) {
          break;
        }

        if ((xfer_len == 1u)                                    // If 'Ctrl-c' character is received.
            && (Ex_USBD_CDC_ACM_TerminalBuf[0] == ASCII_CHAR_END_OF_TEXT)) {
          //                                                       ... return to 'menu' state.
          state = EX_USBD_CDC_ACM_TERMINAL_STATE_MENU;

          //                                                       Clear screen.
          Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                   EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR,
                   EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE);

          (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                  Ex_USBD_CDC_ACM_TerminalBuf,
                                  EX_USBD_CDC_ACM_TERMINAL_SCREEN_CLR_SIZE,
                                  0u,
                                  &err);
          if (err.Code != RTOS_ERR_NONE) {
            break;
          }
        } else {
          //                                                       Echo back characters.
          (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                  &Ex_USBD_CDC_ACM_TerminalBuf[0],
                                  xfer_len,
                                  0u,
                                  &err);
          if (err.Code != RTOS_ERR_NONE) {
            break;
          }

          line_ctr += xfer_len;

          if (line_ctr == EX_USBD_CDC_ACM_TERMINAL_SCREEN_SIZE - 3u) {
            //                                                     Move to next line.
            Mem_Copy(Ex_USBD_CDC_ACM_TerminalBuf,
                     EX_USBD_CDC_ACM_TERMINAL_NEW_LINE,
                     EX_USBD_CDC_ACM_TERMINAL_NEW_LINE_SIZE);

            (void)USBD_ACM_SerialTx(cdc_acm_nbr,
                                    Ex_USBD_CDC_ACM_TerminalBuf,
                                    EX_USBD_CDC_ACM_TERMINAL_NEW_LINE_SIZE,
                                    0u,
                                    &err);
            if (err.Code != RTOS_ERR_NONE) {
              break;
            }

            line_ctr = 0u;
          }
        }
        break;

      default:
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                   Ex_USBD_CDC_ACM_TerminalLineCtrl()
 *
 * @brief  Serial control line change notification callback.
 *
 * @param  nbr           CDC ACM serial emulation subclass instance number.
 *
 * @param  events        Current line state. The line state is a OR'ed of the following flags :
 *                           - USBD_ACM_SERIAL_CTRL_BREAK
 *                           - USBD_ACM_SERIAL_CTRL_RTS
 *                           - USBD_ACM_SERIAL_CTRL_DTR
 *
 * @param  events_chngd  Line state flags that have changed.
 *
 * @param  p_arg         Callback argument.
 *
 * @note   (1) This callback is optional.
 *******************************************************************************************************/
static void Ex_USBD_CDC_ACM_TerminalLineCtrl(CPU_INT08U nbr,
                                             CPU_INT08U events,
                                             CPU_INT08U events_chngd,
                                             void       *p_arg)
{
  (void)&nbr;
  (void)&events;
  (void)&events_chngd;
  (void)&p_arg;

  //                                                               TODO: Handle line control event.
}

/****************************************************************************************************//**
 *                                  Ex_USBD_CDC_ACM_TerminalLineCoding()
 *
 * @brief  Serial line coding line change notification callback.
 *
 * @param  nbr            CDC ACM serial emulation subclass instance number.
 *
 * @param  p_line_coding  Pointer to line coding structure.
 *
 * @param  p_arg          Callback argument.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_USBD_CDC_ACM_TerminalLineCoding(CPU_INT08U                  nbr,
                                                      USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                                      void                        *p_arg)
{
  (void)&nbr;
  (void)&p_line_coding;
  (void)&p_arg;

  //                                                               TODO: Handle line coding change.

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_ACM_AVAIL
