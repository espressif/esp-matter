/***************************************************************************//**
 * @file
 * @brief USB Device Example - USB HID - Mouse
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

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

#include  <kernel/include/os.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_hid.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBD_HID_MOUSE_REPORT_LEN                          4u

#define  EX_USBD_HID_MOUSE_TASK_PRIO                          21u
#define  EX_USBD_HID_MOUSE_TASK_STK_SIZE                     512u

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_USBD_HID_Mouse_TaskHandler(void *p_arg);

//                                                                 ------------------ HID CALLBACKS -------------------
static CPU_BOOLEAN Ex_USBD_HID_Mouse_FeatureReportGet(CPU_INT08U class_nbr,
                                                      CPU_INT08U report_id,
                                                      CPU_INT08U *p_report_buf,
                                                      CPU_INT16U report_len);

static CPU_BOOLEAN Ex_USBD_HID_Mouse_FeatureReportSet(CPU_INT08U class_nbr,
                                                      CPU_INT08U report_id,
                                                      CPU_INT08U *p_report_buf,
                                                      CPU_INT16U report_len);

static CPU_INT08U Ex_USBD_HID_Mouse_ProtocolGet(CPU_INT08U class_nbr,
                                                RTOS_ERR   *p_err);

static void Ex_USBD_HID_Mouse_ProtocolSet(CPU_INT08U class_nbr,
                                          CPU_INT08U protocol,
                                          RTOS_ERR   *p_err);

static void Ex_USBD_HID_Mouse_ReportSet(CPU_INT08U class_nbr,
                                        CPU_INT08U report_id,
                                        CPU_INT08U *p_report_buf,
                                        CPU_INT16U report_len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------- MOUSE TASK TCB AND STACK BUFFER ----------
static OS_TCB  Ex_USBD_HID_Mouse_TaskTCB;
static CPU_STK Ex_USBD_HID_Mouse_TaskStk[EX_USBD_HID_MOUSE_TASK_STK_SIZE];

static CPU_INT08U Ex_USBD_HID_Mouse_Protocol;                   // Current protocol selected by host.

//                                                                 Mouse report buffer.
//                                                                 TODO: Ensure buffer is properly aligned.
static CPU_INT08U Ex_USBD_HID_Mouse_ReportBuf[EX_USBD_HID_MOUSE_REPORT_LEN];

/********************************************************************************************************
 *                                 HID CLASS INSTANCE CALLBACK STRUCT
 *
 * Notes : (1) All callbacks are optional. Can be set to DEF_NULL.
 *******************************************************************************************************/

static USBD_HID_CALLBACK Ex_USBD_HID_Callback = {
  Ex_USBD_HID_Mouse_FeatureReportGet,
  Ex_USBD_HID_Mouse_FeatureReportSet,
  Ex_USBD_HID_Mouse_ProtocolGet,
  Ex_USBD_HID_Mouse_ProtocolSet,
  Ex_USBD_HID_Mouse_ReportSet
};

/********************************************************************************************************
 *                                          HID MOUSE REPORT
 *
 * Notes (1) : The report buffer is sent to the host when it requests the report descriptor. The report
 *             descriptor describes the device itself (axis, quantity of button, scroll wheel, etc) and
 *             how the data for each element is reported in the report.
 *
 *             For more information on how to build your report descriptor, refer to online documentation:
 *             FIXME
 *******************************************************************************************************/

static const CPU_INT08U Ex_USBD_HID_Mouse_ReportDesc[] = {
  USBD_HID_GLOBAL_USAGE_PAGE           + 1, USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  USBD_HID_LOCAL_USAGE                 + 1, USBD_HID_CA_MOUSE,
  USBD_HID_MAIN_COLLECTION             + 1, USBD_HID_COLLECTION_APPLICATION,
  USBD_HID_LOCAL_USAGE             + 1, USBD_HID_CP_POINTER,
  USBD_HID_MAIN_COLLECTION         + 1, USBD_HID_COLLECTION_PHYSICAL,
  USBD_HID_GLOBAL_USAGE_PAGE   + 1, USBD_HID_USAGE_PAGE_BUTTON,
  USBD_HID_LOCAL_USAGE_MIN     + 1, 0x01,
  USBD_HID_LOCAL_USAGE_MAX     + 1, 0x03,
  USBD_HID_GLOBAL_LOG_MIN      + 1, 0x00,
  USBD_HID_GLOBAL_LOG_MAX      + 1, 0x01,
  USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x03,
  USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x01,
  USBD_HID_MAIN_INPUT          + 1, USBD_HID_MAIN_DATA | USBD_HID_MAIN_VARIABLE | USBD_HID_MAIN_ABSOLUTE,
  USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x01,
  USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x0D,
  USBD_HID_MAIN_INPUT          + 1, USBD_HID_MAIN_CONSTANT,
  USBD_HID_GLOBAL_USAGE_PAGE   + 1, USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  USBD_HID_LOCAL_USAGE         + 1, USBD_HID_DV_X,
  USBD_HID_LOCAL_USAGE         + 1, USBD_HID_DV_Y,
  USBD_HID_GLOBAL_LOG_MIN      + 1, 0x81,
  USBD_HID_GLOBAL_LOG_MAX      + 1, 0x7F,
  USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x08,
  USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x02,
  USBD_HID_MAIN_INPUT          + 1, USBD_HID_MAIN_DATA | USBD_HID_MAIN_VARIABLE | USBD_HID_MAIN_RELATIVE,
  USBD_HID_MAIN_ENDCOLLECTION,
  USBD_HID_MAIN_ENDCOLLECTION
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_USBD_HID_Mouse_Init()
 *
 * @brief  Adds an HID interface that represents a mouse to the device.
 *
 * @param  dev_nbr     Device number.
 *
 * @param  cfg_nbr_fs  Full-Speed configuration number.
 *
 * @param  cfg_nbr_hs  High-Speed configuration number.
 *
 * @note   (1) The examle creates a task that will send periodic reports that emulates a mous that
 *             moves back and fourth diagonally.
 *******************************************************************************************************/
void Ex_USBD_HID_Init(CPU_INT08U dev_nbr,
                      CPU_INT08U cfg_nbr_fs,
                      CPU_INT08U cfg_nbr_hs)
{
  CPU_INT08U       class_nbr;
  USBD_HID_QTY_CFG cfg_qty_hid;
  RTOS_ERR         err;

  cfg_qty_hid.ClassInstanceQty = 1u;
  cfg_qty_hid.ConfigQty = 2u;

  USBD_HID_Init(&cfg_qty_hid,
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- CREATE AN HID CLASS INSTANCE -----------
  class_nbr = USBD_HID_Add(USBD_HID_SUBCLASS_BOOT,              // Using boot protocol.
                           USBD_HID_PROTOCOL_MOUSE,             // Mouse pre-defined protocol.
                           USBD_HID_COUNTRY_CODE_NOT_SUPPORTED,
                           Ex_USBD_HID_Mouse_ReportDesc,        // Provide report desc linked to this instance.
                           sizeof(Ex_USBD_HID_Mouse_ReportDesc),
                           DEF_NULL,
                           0u,
                           2u,
                           2u,
                           DEF_YES,
                           &Ex_USBD_HID_Callback,               // Optionnal callback struct. Can be DEF_NULL.
                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               -------- ADD HID CLASS INSTANCE TO CONFIGS ---------
  USBD_HID_ConfigAdd(class_nbr,                                 // Add HID class to FS dflt cfg.
                     dev_nbr,
                     cfg_nbr_fs,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (cfg_nbr_hs != USBD_CONFIG_NBR_NONE) {
    USBD_HID_ConfigAdd(class_nbr,                               // Add HID class to HS dflt cfg.
                       dev_nbr,
                       cfg_nbr_hs,
                       &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

  //                                                               -------- CREATE HID MOUSE TASK (See note 1) --------
  OSTaskCreate(&Ex_USBD_HID_Mouse_TaskTCB,
               "USB Device HID Mouse",
               Ex_USBD_HID_Mouse_TaskHandler,
               (void *)(CPU_ADDR)class_nbr,
               EX_USBD_HID_MOUSE_TASK_PRIO,
               &Ex_USBD_HID_Mouse_TaskStk[0],
               EX_USBD_HID_MOUSE_TASK_STK_SIZE / 10u,
               EX_USBD_HID_MOUSE_TASK_STK_SIZE,
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
 *                                     Ex_USBD_HID_Mouse_TaskHandler()
 *
 * @brief  Perform HID writes to host. The HID writes simulate the movement of a mouse.
 *
 * @param  p_arg  Task argument. Class number in this case.
 *******************************************************************************************************/
static void Ex_USBD_HID_Mouse_TaskHandler(void *p_arg)
{
  CPU_INT08U  class_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;
  CPU_BOOLEAN x_is_pos = DEF_YES;
  CPU_BOOLEAN y_is_pos = DEF_YES;
  CPU_BOOLEAN conn;
  RTOS_ERR    err;

  Ex_USBD_HID_Mouse_ReportBuf[0u] = 0u;
  Ex_USBD_HID_Mouse_ReportBuf[1u] = 0u;

  while (DEF_TRUE) {
    conn = USBD_HID_IsConn(class_nbr);                          // Wait for dev connection.
    while (conn != DEF_YES) {
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);

      conn = USBD_HID_IsConn(class_nbr);
    }

    //                                                             Emulates back and fourth movement.
    ((CPU_INT08S *)Ex_USBD_HID_Mouse_ReportBuf)[2u] = (x_is_pos) ? 50 : -50;
    ((CPU_INT08S *)Ex_USBD_HID_Mouse_ReportBuf)[3u] = (y_is_pos) ? 50 : -50;

    x_is_pos = !x_is_pos;
    y_is_pos = !y_is_pos;

    //                                                             Send report.
    (void)USBD_HID_Wr(class_nbr,
                      Ex_USBD_HID_Mouse_ReportBuf,
                      EX_USBD_HID_MOUSE_REPORT_LEN,
                      0u,
                      &err);

    OSTimeDlyHMSM(0u, 0u, 0u, 100u,
                  OS_OPT_TIME_HMSM_NON_STRICT,
                  &err);
  }
}

/********************************************************************************************************
 *                                          USB HID CALLBACKS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                  Ex_USBD_HID_Mouse_FeatureReportGet()
 *
 * @brief  Get HID feature report corresponding to report ID.
 *
 * @param  class_nbr     Class instance number.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to feature report buffer.
 *
 * @param  report_len    Length of report, in octets.
 *
 * @return  DEF_OK,   if NO error(s) occurred and report ID is supported.
 *          DEF_FAIL, otherwise.
 *
 * @note   (1) Report ID must not be written into the feature report buffer.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_USBD_HID_Mouse_FeatureReportGet(CPU_INT08U class_nbr,
                                                      CPU_INT08U report_id,
                                                      CPU_INT08U *p_report_buf,
                                                      CPU_INT16U report_len)
{
  /* This function is called when host issues a GetReport(feature) request. The application
   * can provide the report to send by copying it in p_report_buf.
   */

  PP_UNUSED_PARAM(class_nbr);
  PP_UNUSED_PARAM(report_id);

  Mem_Clr(p_report_buf, report_len);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                  Ex_USBD_HID_Mouse_FeatureReportSet()
 *
 * @brief  Set HID feature report corresponding to report ID.
 *
 * @param  class_nbr     Class instance number.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to feature report buffer.
 *
 * @param  report_len    Length of report, in octets.
 *
 * @return  DEF_OK,   if NO error(s) occurred and report ID is supported.
 *          DEF_FAIL, otherwise.
 *
 * @note   (1) Report ID is not present in the feature report buffer.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_USBD_HID_Mouse_FeatureReportSet(CPU_INT08U class_nbr,
                                                      CPU_INT08U report_id,
                                                      CPU_INT08U *p_report_buf,
                                                      CPU_INT16U report_len)
{
  /* This function is called when host issues a SetReport(Feature) request. The application
   * can take action in function of the provided report in p_report_buf.
   */

  PP_UNUSED_PARAM(class_nbr);
  PP_UNUSED_PARAM(report_id);
  PP_UNUSED_PARAM(p_report_buf);
  PP_UNUSED_PARAM(report_len);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                     Ex_USBD_HID_Mouse_ProtocolGet()
 *
 * @brief  Retrieve active protocol: BOOT or REPORT protocol.
 *
 * @param  class_nbr  Class instance number.
 *
 * @param  p_err      Pointer to variable that will receive the return error code from this function :
 *                      - RTOS_ERR_NONE   Protocol successfully retrieved.
 *
 * @return  Protocol.
 *******************************************************************************************************/
static CPU_INT08U Ex_USBD_HID_Mouse_ProtocolGet(CPU_INT08U class_nbr,
                                                RTOS_ERR   *p_err)
{
  /* This function is called when host issues a GetProtocol request. The application
   * should return the current protocol.
   */

  PP_UNUSED_PARAM(class_nbr);

  p_err->Code = RTOS_ERR_NONE;

  return (Ex_USBD_HID_Mouse_Protocol);
}

/****************************************************************************************************//**
 *                                     Ex_USBD_HID_Mouse_ProtocolSet()
 *
 * @brief  Store active protocol: BOOT or REPORT protocol.
 *
 * @param  class_nbr  Class instance number.
 *
 * @param  protocol   Protocol.
 *
 * @param  p_err      Pointer to variable that will receive the return error code from this function :
 *                      - RTOS_ERR_NONE   Protocol successfully set.
 *******************************************************************************************************/
static void Ex_USBD_HID_Mouse_ProtocolSet(CPU_INT08U class_nbr,
                                          CPU_INT08U protocol,
                                          RTOS_ERR   *p_err)
{
  /* This function is called when host issues a SetProtocol request. The application
   * should apply the new protocol.
   */

  PP_UNUSED_PARAM(class_nbr);

  Ex_USBD_HID_Mouse_Protocol = protocol;

  p_err->Code = RTOS_ERR_NONE;
}

/****************************************************************************************************//**
 *                                      Ex_USBD_HID_Mouse_ReportSet()
 *
 * @brief  Notification of a new set report received on control endpoint.
 *
 * @param  class_nbr     Class instance number.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to report buffer.
 *
 * @param  report_len    Length of report, in octets.
 *******************************************************************************************************/
static void Ex_USBD_HID_Mouse_ReportSet(CPU_INT08U class_nbr,
                                        CPU_INT08U report_id,
                                        CPU_INT08U *p_report_buf,
                                        CPU_INT16U report_len)
{
  /* This function is called when host issues a SetReport request. The application
   * can take action in function of the report content.
   */

  PP_UNUSED_PARAM(class_nbr);
  PP_UNUSED_PARAM(report_id);
  PP_UNUSED_PARAM(p_report_buf);
  PP_UNUSED_PARAM(report_len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_HID_AVAIL
