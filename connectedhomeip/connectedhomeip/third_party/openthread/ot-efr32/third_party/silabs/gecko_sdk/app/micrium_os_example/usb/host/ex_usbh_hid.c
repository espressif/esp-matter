/***************************************************************************//**
 * @file
 * @brief USB Host Example - Human Interface Device (HID) - Mouse/keyboard example
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
 * The HID class driver is meant to be used with devices such as mice, keyboards, gamepads, barcode
 * scanners, etc.
 *
 * This example application will simply print the inputs received from either a mouse or a keyboard.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_HID_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   EX_USBH_HID_MODULE

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_hid.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBH_HID_REPORTS_MAX_QTY                      6u

#define  EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP  0x00000001u
#define  EX_USBH_HID_LOG_USAGE_POINTER           0x00000001u
#define  EX_USBH_HID_LOG_USAGE_MOUSE             0x00000002u
#define  EX_USBH_HID_LOG_USAGE_X                 0x00000030u
#define  EX_USBH_HID_LOG_USAGE_Y                 0x00000031u
#define  EX_USBH_HID_LOG_USAGE_KBD               0x00000006u

#define  EX_USBH_HID_MOUSE_BUTTON_QTY                     3u
#define  EX_USBH_HID_MOUSE_BUTTON_1                    0x01u
#define  EX_USBH_HID_MOUSE_BUTTON_2                    0x02u
#define  EX_USBH_HID_MOUSE_BUTTON_3                    0x04u

#define  EX_USBH_HID_KBD_LEFT_CTRL                     0x01u
#define  EX_USBH_HID_KBD_LEFT_SHIFT                    0x02u
#define  EX_USBH_HID_KBD_LEFT_ALT                      0x04u
#define  EX_USBH_HID_KBD_LEFT_GUI                      0x08u
#define  EX_USBH_HID_KBD_RIGHT_CTRL                    0x10u
#define  EX_USBH_HID_KBD_RIGHT_SHIFT                   0x20u
#define  EX_USBH_HID_KBD_RIGHT_ALT                     0x40u
#define  EX_USBH_HID_KBD_RIGHT_GUI                     0x80u
#define  EX_USBH_HID_KBD_MAX_NBR_PRESSED                  6u

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

//                                                                 ------------------- REPORT TYPES -------------------
typedef  enum  ex_usbh_hid_report_type {
  EX_USBH_HID_REPORT_TYPE_NONE,
  EX_USBH_HID_REPORT_TYPE_MOUSE,
  EX_USBH_HID_REPORT_TYPE_KBD,
  EX_USBH_HID_REPORT_TYPE_OTHER
} EX_USBH_HID_REPORT_TYPE;

//                                                                 ------------------- REPORT INFO --------------------
typedef  struct  ex_usbh_hid_report_info {
  EX_USBH_HID_REPORT_TYPE Type;                                 // Report ID type (mouse, keyboard, ...).
  CPU_INT08U              ID;                                   // ID of report.
} EX_USBH_HID_REPORT_INFO;

//                                                                 --------------------- HID FNCT ---------------------
typedef  struct  ex_usbh_hid_fnct {
  CPU_INT08U              ReportIxNext;                         // Ix of next avail report in tbl.
                                                                // Table of report.
  EX_USBH_HID_REPORT_INFO ReportsTbl[EX_USBH_HID_REPORTS_MAX_QTY];
} EX_USBH_HID_FNCT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL Ex_USBH_HID_FnctPool;                       // HID function pool.

//                                                                 ------------------- MOUSE STATUS -------------------
static CPU_INT08U Ex_USBH_HID_MouseButtonStatePrev = 0u;        // Previous button state.
static CPU_INT32S Ex_USBH_HID_MousePosX = 0u;                   // Current X position.
static CPU_INT32S Ex_USBH_HID_MousePosY = 0u;                   // Current Y position.

//                                                                 ----------------- KEYBOARD STATUS ------------------
static CPU_INT08U Ex_USBH_HID_KbdNbrKeysLast = 0u;              // Quantity of pressed keys during last report.
                                                                // Table of keys pressed during last report.
static CPU_INT08U Ex_USBH_HID_KbdKeysLast[EX_USBH_HID_KBD_MAX_NBR_PRESSED];

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void *Ex_USBH_HID_Conn(USBH_DEV_HANDLE      dev_handle,
                              USBH_FNCT_HANDLE     fnct_handle,
                              USBH_HID_FNCT_HANDLE hid_fnct_handle,
                              USBH_HID_APP_COLL    *p_app_coll_head,
                              RTOS_ERR             err);

static void Ex_USBH_HID_OnDataRxd(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                  void                 *p_arg,
                                  CPU_INT08U           report_id,
                                  CPU_INT08U           *p_buf,
                                  CPU_INT32U           buf_len,
                                  RTOS_ERR             err);

static void Ex_USBH_HID_Disconn(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                void                 *p_arg);

static void Ex_USBH_HID_MousePosUpdate(CPU_INT08U *p_buf,
                                       CPU_INT32U buf_len);

static void Ex_USBH_HID_KbdTouchPrint(CPU_INT08U *p_buf,
                                      CPU_INT32U buf_len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 HID application functions structure.
USBH_HID_APP_FNCTS Ex_USBH_HID_AppFncts = {
  Ex_USBH_HID_Conn,
  Ex_USBH_HID_OnDataRxd,
  Ex_USBH_HID_Disconn
};

/********************************************************************************************************
 *                                       KEYBOARD LAYOUT TABLES
 *******************************************************************************************************/

static const CPU_INT08U Ex_USBH_HID_KbdHID_ToKBD[] = {
  0u, 0u, 0u, 0u, 31u, 50u, 48u, 33u, 19u, 34u, 35u, 36u, 24u, 37u, 38u, 39u,                                            // 0x00 - 0x0F
  52u, 51u, 25u, 26u, 17u, 20u, 32u, 21u, 23u, 49u, 18u, 47u, 22u, 46u, 2u, 3u,                                          // 0x10 - 0x1F
  4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 43u, 110u, 15u, 16u, 61u, 12u, 13u, 27u,                                             // 0x20 - 0x2F
  28u, 29u, 42u, 40u, 41u, 1u, 53u, 54u, 55u, 30u, 112u, 113u, 114u, 115u, 116u, 117u,                                   // 0x30 - 0x3F
  118u, 119u, 120u, 121u, 122u, 123u, 124u, 125u, 126u, 75u, 80u, 85u, 76u, 81u, 86u, 89u,                               // 0x40 - 0x4F
  79u, 84u, 83u, 90u, 95u, 100u, 105u, 106u, 108u, 93u, 98u, 103u, 92u, 97u, 102u, 91u,                                  // 0x50 - 0x5F
  96u, 101u, 99u, 104u, 45u, 129u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                               // 0x60 - 0x6F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0x70 - 0x7F
  0u, 0u, 0u, 0u, 0u, 107u, 0u, 56u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                     // 0x80 - 0x8F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0x90 - 0x9F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0xA0 - 0xAF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0xB0 - 0xBF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0xC0 - 0xCF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,                                                        // 0xD0 - 0xDF
  58u, 44u, 60u, 127u, 64u, 57u, 62u, 128u                                                                               // 0xE0 - 0xE7
};

static const CPU_INT08S Ex_USBH_HID_KbdKey[] = {
  '\0', '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
  '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\0', '\n',
  '\0', '\0', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0',
  '\0', '\0', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\r', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '7', '4', '1',
  '\0', '/', '8', '5', '2',
  '0', '*', '9', '6', '3',
  '.', '-', '+', '\0', '\n', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static const CPU_INT08S Ex_USBH_HID_KbdShiftKey[] = {
  '\0', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
  '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
  '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '\0', '\n',
  '\0', '\0', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_USBH_HID_Init()
 *
 * @brief  Example of initialization of the HID class driver.
 *******************************************************************************************************/
void Ex_USBH_HID_Init(void)
{
  RTOS_ERR err;

  USBH_HID_Init(&Ex_USBH_HID_AppFncts, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create pool of HID functions.
  Mem_DynPoolCreate("Ex USBH HID - Fnct pool",
                    &Ex_USBH_HID_FnctPool,
                    DEF_NULL,
                    sizeof(EX_USBH_HID_FNCT),
                    sizeof(CPU_ALIGN),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_USBH_HID_DevConn()
 *
 * @brief  This function is called when HID device is connected.
 *
 * @param  p_hid_dev  Pointer to HID device.
 *******************************************************************************************************/
static void *Ex_USBH_HID_Conn(USBH_DEV_HANDLE      dev_handle,
                              USBH_FNCT_HANDLE     fnct_handle,
                              USBH_HID_FNCT_HANDLE hid_fnct_handle,
                              USBH_HID_APP_COLL    *p_app_coll_head,
                              RTOS_ERR             err)
{
  CPU_INT08U        report_tbl_ix;
  RTOS_ERR          err_local;
  USBH_HID_APP_COLL *p_app_coll_item;
  EX_USBH_HID_FNCT  *p_fnct;

  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(fnct_handle);
  PP_UNUSED_PARAM(err);

  EX_TRACE("HID example: Function connected\r\n");

  p_fnct = (EX_USBH_HID_FNCT *)Mem_DynPoolBlkGet(&Ex_USBH_HID_FnctPool,
                                                 &err_local);
  if (err_local.Code != RTOS_ERR_NONE) {
    EX_TRACE("HID example: Failed to allocate function\r\n");
    return (DEF_NULL);
  }

  p_fnct->ReportIxNext = 0u;
  for (report_tbl_ix = 0u; report_tbl_ix < EX_USBH_HID_REPORTS_MAX_QTY; report_tbl_ix++) {
    p_fnct->ReportsTbl[report_tbl_ix].Type = EX_USBH_HID_REPORT_TYPE_NONE;
  }

  //                                                               Set infinite Idle Time.
  USBH_HID_IdleSet(hid_fnct_handle,
                   0u,
                   0u,
                   &err_local);
  if ((err_local.Code != RTOS_ERR_NONE)
      && (err_local.Code != RTOS_ERR_EP_STALL)) {
    EX_TRACE("HID example: Failed to set IDLE time\r\n");
    return (DEF_NULL);
  }

  p_app_coll_item = p_app_coll_head;
  while ((p_app_coll_item != DEF_NULL)
         && (p_fnct->ReportIxNext < EX_USBH_HID_REPORTS_MAX_QTY)) {
    CPU_BOOLEAN              already_in_tbl = DEF_NO;
    USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item = p_app_coll_item->ReportFmtItemHeadPtr;
    EX_USBH_HID_REPORT_TYPE  report_type;

    while ((p_report_fmt_item != DEF_NULL)
           && (p_fnct->ReportIxNext < EX_USBH_HID_REPORTS_MAX_QTY)) {
      //                                                           If app usage is mouse and phy usage is pointer, ...
      //                                                           ... report is considered to belong to a mouse.
      if ((p_app_coll_item->Usage == ((CPU_INT32U)(EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP << 16u) | (EX_USBH_HID_LOG_USAGE_MOUSE)))
          | (p_report_fmt_item->ReportFmt.AppUsage == ((CPU_INT32U)(EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP << 16u) | (EX_USBH_HID_LOG_USAGE_MOUSE))
             && (p_report_fmt_item->ReportFmt.PhyUsage == ((CPU_INT32U)(EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP << 16u) | (EX_USBH_HID_LOG_USAGE_POINTER))))) {
        report_type = EX_USBH_HID_REPORT_TYPE_MOUSE;
      } else if ((p_app_coll_item->Usage == ((CPU_INT32U)(EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP << 16u) | (EX_USBH_HID_LOG_USAGE_KBD)))
                 | (p_report_fmt_item->ReportFmt.AppUsage == ((CPU_INT32U)(EX_USBH_HID_LOG_USAGE_PAGE_GEN_DESKTOP << 16u) | (EX_USBH_HID_LOG_USAGE_KBD)))) {
        report_type = EX_USBH_HID_REPORT_TYPE_KBD;
      } else {
        report_type = EX_USBH_HID_REPORT_TYPE_OTHER;
      }

      //                                                           Check if report already in table.
      for (report_tbl_ix = 0u; report_tbl_ix < p_fnct->ReportIxNext; report_tbl_ix++) {
        if ((p_fnct->ReportsTbl[report_tbl_ix].Type != EX_USBH_HID_REPORT_TYPE_NONE)
            && (p_fnct->ReportsTbl[report_tbl_ix].ID == p_report_fmt_item->ReportFmt.ReportID)) {
          already_in_tbl = DEF_YES;
          break;
        }
      }

      //                                                           Add it if not part of table.
      if (already_in_tbl == DEF_NO) {
        p_fnct->ReportsTbl[p_fnct->ReportIxNext].ID = p_report_fmt_item->ReportFmt.ReportID;
        p_fnct->ReportsTbl[p_fnct->ReportIxNext].Type = report_type;

        switch (report_type) {
          case EX_USBH_HID_REPORT_TYPE_MOUSE:
            EX_TRACE("HID example: Mouse connected\r\n");
            break;

          case EX_USBH_HID_REPORT_TYPE_KBD:
            EX_TRACE("HID example: Keyboard connected\r\n");
            break;

          case EX_USBH_HID_REPORT_TYPE_OTHER:
            EX_TRACE("HID example: Vendor-specific report format\r\n");
            break;

          case EX_USBH_HID_REPORT_TYPE_NONE:
          default:
            break;
        }

        p_fnct->ReportIxNext++;
      }

      p_report_fmt_item = p_report_fmt_item->NextPtr;
    }

    p_app_coll_item = p_app_coll_item->NextPtr;
  }

  return (p_fnct);
}

/****************************************************************************************************//**
 *                                         Ex_USBH_HID_OnDataRxd()
 *
 * @brief  This function is called when a report is received from an HID function.
 *
 * @param  hid_fnct_handle  Handle on HID function.
 *
 * @param  p_arg            Argument associated to HID function returned to function
 *                          Ex_USBH_HID_Conn().
 *
 * @param  report_id        ID of received report.
 *
 * @param  p_buf            Pointer to buffer containing report.
 *
 * @param  buf_len          Length of buffer, in octets.
 *
 * @param  err              Error code.
 *******************************************************************************************************/
static void Ex_USBH_HID_OnDataRxd(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                  void                 *p_arg,
                                  CPU_INT08U           report_id,
                                  CPU_INT08U           *p_buf,
                                  CPU_INT32U           buf_len,
                                  RTOS_ERR             err)
{
  CPU_INT08U       report_tbl_ix;
  EX_USBH_HID_FNCT *p_fnct = (EX_USBH_HID_FNCT *)p_arg;

  PP_UNUSED_PARAM(hid_fnct_handle);

  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("HID example: Error on data reception\r\n");
    return;
  }

  if (p_fnct == DEF_NULL) {                                     // Function not supported.
    return;
  }

  for (report_tbl_ix = 0u; report_tbl_ix < p_fnct->ReportIxNext; report_tbl_ix++) {
    EX_USBH_HID_REPORT_INFO *p_report_id = &p_fnct->ReportsTbl[report_tbl_ix];

    if (p_report_id->ID == report_id) {
      switch (p_report_id->Type) {
        case EX_USBH_HID_REPORT_TYPE_MOUSE:
          Ex_USBH_HID_MousePosUpdate(p_buf, buf_len);
          break;

        case EX_USBH_HID_REPORT_TYPE_KBD:
          Ex_USBH_HID_KbdTouchPrint(p_buf, buf_len);
          break;

        case EX_USBH_HID_REPORT_TYPE_OTHER:
        case EX_USBH_HID_REPORT_TYPE_NONE:
        default:
          break;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                        Ex_USBH_HID_DevDisconn()
 *
 * @brief  This function is called when an HID function is disconnected.
 *
 * @param  hid_fnct_handle  Handle on HID function.
 *
 * @param  p_arg            Argument associated to HID function returned to function
 *                          Ex_USBH_HID_Conn().
 *******************************************************************************************************/
static void Ex_USBH_HID_Disconn(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                void                 *p_arg)
{
  RTOS_ERR         err;
  EX_USBH_HID_FNCT *p_fnct = (EX_USBH_HID_FNCT *)p_arg;

  PP_UNUSED_PARAM(hid_fnct_handle);

  EX_TRACE("HID example: Function disconnected\r\n");

  if (p_fnct == DEF_NULL) {                                     // Function was not supported.
    return;
  }

  Mem_DynPoolBlkFree(&Ex_USBH_HID_FnctPool,
                     (void *)p_fnct,
                     &err);
  if (err.Code != RTOS_ERR_NONE) {
    EX_TRACE("HID example: Failed to free function\r\n");
    return;
  }
}

/****************************************************************************************************//**
 *                                      Ex_USBH_HID_MousePosUpdate()
 *
 * @brief  New mouse position received. Update absolute position.
 *
 * @param  p_buf    Pointer to buffer.
 *
 * @param  buf_len  Length of buffer in octets.
 *******************************************************************************************************/
static void Ex_USBH_HID_MousePosUpdate(CPU_INT08U *p_buf,
                                       CPU_INT32U buf_len)
{
  CPU_INT08U button_state;
  CPU_INT08U button_cur_ix;

  if (buf_len < 3u) {
    EX_TRACE("HID example: received less data than expected\r\n");
    return;
  }

  //                                                               ------------ PRINT MOUSE BUTTONS STATE -------------
  button_state = p_buf[0u];

  for (button_cur_ix = 0u; button_cur_ix < EX_USBH_HID_MOUSE_BUTTON_QTY; button_cur_ix++) {
    CPU_BOOLEAN pressed = (button_state & DEF_BIT(button_cur_ix)) == DEF_BIT(button_cur_ix) ? DEF_TRUE : DEF_FALSE;
    CPU_BOOLEAN pressed_prev = (Ex_USBH_HID_MouseButtonStatePrev & DEF_BIT(button_cur_ix)) == DEF_BIT(button_cur_ix) ? DEF_TRUE : DEF_FALSE;

    if ((pressed)
        && (!pressed_prev)) {
      EX_TRACE("HID Example: Mouse button #%d pressed\r\n", button_cur_ix);
    } else if ((!pressed)
               && (pressed_prev)) {
      EX_TRACE("HID Example: Mouse button #%d released\r\n", button_cur_ix);
    }
  }

  Ex_USBH_HID_MouseButtonStatePrev = button_state;

  //                                                               --------- UPDATE/PRINT NEW MOUSE POSITION ----------
  if ((p_buf[1u] != 0u)
      || (p_buf[2u] != 0u)) {
    Ex_USBH_HID_MousePosX += (CPU_INT08S)p_buf[1u];
    Ex_USBH_HID_MousePosY += (CPU_INT08S)p_buf[2u];

    EX_TRACE("HID Example: Mouse pointer at (x, y) = (%d, %d)\n",
             Ex_USBH_HID_MousePosX,
             Ex_USBH_HID_MousePosY);
  }
}

/****************************************************************************************************//**
 *                                       Ex_USBH_HID_KbdTouchPrint()
 *
 * @brief  Print pressed keyboard keys.
 *
 * @param  p_buf    Pointer to keyboard report buffer.
 *
 * @param  buf_len  Length of report buffer in octets.
 *******************************************************************************************************/
static void Ex_USBH_HID_KbdTouchPrint(CPU_INT08U *p_buf,
                                      CPU_INT32U buf_len)
{
  CPU_BOOLEAN shift;
  CPU_INT08U  ix;
  CPU_INT08U  nbr_keys = 0u;
  CPU_INT08U  key_newest = 0u;
  CPU_INT08U  nbr_keys_new = 0u;
  CPU_INT08U  kbd_keys_tbl[EX_USBH_HID_KBD_MAX_NBR_PRESSED];
  CPU_INT08U  kbd_keys_new_tbl[EX_USBH_HID_KBD_MAX_NBR_PRESSED];

  if (buf_len < 8u) {
    EX_TRACE("HID example: received less data than expected\r\n");
    return;
  }

  //                                                               ----------- DETERMINE SHIFT KEY PRESSED ------------
  if ((p_buf[0u] == EX_USBH_HID_KBD_LEFT_SHIFT)
      || (p_buf[0u] == EX_USBH_HID_KBD_RIGHT_SHIFT)) {
    shift = DEF_TRUE;
  } else {
    shift = DEF_FALSE;
  }

  //                                                               ---------- DETERMINE IF ERROR ENCOUNTERED ----------
  for (ix = 2u; ix < (2u + EX_USBH_HID_KBD_MAX_NBR_PRESSED); ix++) {
    if ((p_buf[ix] == 0x01u)
        || (p_buf[ix] == 0x02u)
        || (p_buf[ix] == 0x03u)) {
      EX_TRACE("HID example: error encountered\r\n");
      return;
    }
  }

  //                                                               -------------- DETERMINE PRESSED KEYS --------------
  for (ix = 2u; ix < (2u + EX_USBH_HID_KBD_MAX_NBR_PRESSED); ix++) {
    if (p_buf[ix] != 0u) {                                      // If pressed, add to array.
      CPU_INT08U jx;

      kbd_keys_tbl[nbr_keys] = p_buf[ix];
      nbr_keys++;

      //                                                           Determine if key was already pressed.
      for (jx = 0u; jx < Ex_USBH_HID_KbdNbrKeysLast; jx++) {
        if (p_buf[ix] == Ex_USBH_HID_KbdKeysLast[jx]) {
          break;
        }
      }

      //                                                           If key was not already pressed, add to array.
      if (jx == Ex_USBH_HID_KbdNbrKeysLast) {
        kbd_keys_new_tbl[nbr_keys_new] = p_buf[ix];
        nbr_keys_new++;
      }
    }
  }

  //                                                               ---------------- PRINT PRESSED KEYS ----------------
  if (nbr_keys_new > 0u) {
    EX_TRACE("HID example: keyboard keys pressed: ");

    for (ix = 0u; ix < nbr_keys_new; ix++) {
      key_newest = kbd_keys_new_tbl[ix];

      if (shift == DEF_TRUE) {
        EX_TRACE("%c", Ex_USBH_HID_KbdShiftKey[Ex_USBH_HID_KbdHID_ToKBD[key_newest]]);
      } else {
        EX_TRACE("%c", Ex_USBH_HID_KbdKey[Ex_USBH_HID_KbdHID_ToKBD[key_newest]]);
      }
    }

    EX_TRACE("\r\n");
  }

  //                                                               -------------- PREPARE FOR NEXT LOOP ---------------
  Ex_USBH_HID_KbdNbrKeysLast = nbr_keys;
  for (ix = 0u; ix < EX_USBH_HID_KBD_MAX_NBR_PRESSED; ix++) {
    Ex_USBH_HID_KbdKeysLast[ix] = kbd_keys_tbl[ix];
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_HOST_HID_AVAIL
