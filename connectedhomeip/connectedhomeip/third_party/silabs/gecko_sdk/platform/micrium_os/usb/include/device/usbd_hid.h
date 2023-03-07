/***************************************************************************//**
 * @file
 * @brief USB Device HID Class
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
 * @defgroup USBD_HID USB Device HID API
 * @ingroup USBD
 * @brief   USB Device HID API
 *
 * @addtogroup USBD_HID
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_HID_H_
#define  _USBD_HID_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_hid_report.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  USBD_HID_MODULE
#define  USBD_HID_EXT
#else
#define  USBD_HID_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.1 for more details about HID descriptor country code.
 *
 *               (a) The country code identifies which country the hardware is localized for. Most
 *                   hardware is not localized and thus this value would be zero (0). However, keyboards
 *                   may use the field to indicate the language of the key caps.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ COUNTRY CODES (see Note #1) -----------
typedef enum usbd_hid_country_code {
  USBD_HID_COUNTRY_CODE_NOT_SUPPORTED = 0u,                     // See Note #1a.
  USBD_HID_COUNTRY_CODE_ARABIC = 1u,
  USBD_HID_COUNTRY_CODE_BELGIAN = 2u,
  USBD_HID_COUNTRY_CODE_CANADIAN_BILINGUAL = 3u,
  USBD_HID_COUNTRY_CODE_CANADIAN_FRENCH = 4u,
  USBD_HID_COUNTRY_CODE_CZECH_REPUBLIC = 5u,
  USBD_HID_COUNTRY_CODE_DANISH = 6u,
  USBD_HID_COUNTRY_CODE_FINNISH = 7u,
  USBD_HID_COUNTRY_CODE_FRENCH = 8u,
  USBD_HID_COUNTRY_CODE_GERMAN = 9u,
  USBD_HID_COUNTRY_CODE_GREEK = 10u,
  USBD_HID_COUNTRY_CODE_HEBREW = 11u,
  USBD_HID_COUNTRY_CODE_HUNGARY = 12u,
  USBD_HID_COUNTRY_CODE_INTERNATIONAL = 13u,
  USBD_HID_COUNTRY_CODE_ITALIAN = 14u,
  USBD_HID_COUNTRY_CODE_JAPAN_KATAKANA = 15u,
  USBD_HID_COUNTRY_CODE_KOREAN = 16u,
  USBD_HID_COUNTRY_CODE_LATIN_AMERICAN = 17u,
  USBD_HID_COUNTRY_CODE_NETHERLANDS_DUTCH = 18u,
  USBD_HID_COUNTRY_CODE_NORWEGIAN = 19u,
  USBD_HID_COUNTRY_CODE_PERSIAN_FARSI = 20u,
  USBD_HID_COUNTRY_CODE_POLAND = 21u,
  USBD_HID_COUNTRY_CODE_PORTUGUESE = 22u,
  USBD_HID_COUNTRY_CODE_RUSSIA = 23u,
  USBD_HID_COUNTRY_CODE_SLOVAKIA = 24u,
  USBD_HID_COUNTRY_CODE_SPANISH = 25u,
  USBD_HID_COUNTRY_CODE_SWEDISH = 26u,
  USBD_HID_COUNTRY_CODE_SWISS_FRENCH = 27u,
  USBD_HID_COUNTRY_CODE_SWISS_GERMAN = 28u,
  USBD_HID_COUNTRY_CODE_SWITZERLAND = 29u,
  USBD_HID_COUNTRY_CODE_TAIWAN = 30u,
  USBD_HID_COUNTRY_CODE_TURKISH_Q = 31u,
  USBD_HID_COUNTRY_CODE_UK = 32u,
  USBD_HID_COUNTRY_CODE_US = 33u,
  USBD_HID_COUNTRY_CODE_YUGOSLAVIA = 34u,
  USBD_HID_COUNTRY_CODE_TURKISH_F = 35u
} USBD_HID_COUNTRY_CODE;

/********************************************************************************************************
 *                           HUMAN INTERFACE DEVICE CLASS SUBCLASS CODES DEFINES
 *
 * Note(s) : (1) Human interface device class subclass codes are defined in section 4.2 of HID
 *               specification revision 1.11.
 *******************************************************************************************************/

#define  USBD_HID_SUBCLASS_NONE                        0x00u    // No subclass.
#define  USBD_HID_SUBCLASS_BOOT                        0x01u    // Boot interface.

/********************************************************************************************************
 *                           HUMAN INTERFACE DEVICE CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Human interface device class protocol codes are defined in section 4.3 of HID
 *               specification revision 1.11.
 *******************************************************************************************************/

#define  USBD_HID_PROTOCOL_NONE                        0x00u    // No class specific protocol.
#define  USBD_HID_PROTOCOL_KBD                         0x01u    // Keyboard protocol.
#define  USBD_HID_PROTOCOL_MOUSE                       0x02u    // Mouse protocol.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- ASYNC COMM CALLBACK ----------------
typedef void (*USBD_HID_ASYNC_FNCT)(CPU_INT08U class_nbr,
                                    void       *p_buf,
                                    CPU_INT32U buf_len,
                                    CPU_INT32U xfer_len,
                                    void       *p_callback_arg,
                                    RTOS_ERR   err);

typedef const struct usbd_hid_callback {                        // ------------ HID DESC AND REQ CALLBACKS ------------
  CPU_BOOLEAN (*FeatureReportGet)(CPU_INT08U class_nbr,
                                  CPU_INT08U report_id,
                                  CPU_INT08U *p_report_buf,
                                  CPU_INT16U report_len);

  CPU_BOOLEAN (*FeatureReportSet)(CPU_INT08U class_nbr,
                                  CPU_INT08U report_id,
                                  CPU_INT08U *p_report_buf,
                                  CPU_INT16U report_len);

  CPU_INT08U (*ProtocolGet)(CPU_INT08U class_nbr,
                            RTOS_ERR   *p_err);

  void (*ProtocolSet)(CPU_INT08U class_nbr,
                      CPU_INT08U protocol,
                      RTOS_ERR   *p_err);

  void (*ReportSet)(CPU_INT08U class_nbr,
                    CPU_INT08U report_id,
                    CPU_INT08U *p_report_buf,
                    CPU_INT16U report_len);
} USBD_HID_CALLBACK;

//                                                                 -------- HID CLASS CONFIGURATION STRUCTURE ---------
typedef struct usbd_hid_qty_cfg {
  CPU_INT08U ClassInstanceQty;                                  // Quantity of class instances.
  CPU_INT08U ConfigQty;                                         // Quantity of configurations.
} USBD_HID_QTY_CFG;

//                                                                 ------------- HID INIT CONFIGURATIONS --------------
typedef struct usbd_hid_init_cfg {
  CPU_SIZE_T BufAlignOctets;                                    // Indicates desired mem alignment for internal buf.
  CPU_INT08U ReportID_Qty;                                      // Quantity of report ids.
  CPU_INT08U PushPopItemsQty;                                   // Quantity of push/pop items.

  MEM_SEG    *MemSegPtr;                                        // Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG    *MemSegBufPtr;                                     // Ptr to mem segment to use when allocating data buf.

  CPU_INT32U TmrTaskStkSizeElements;                            // Tmr task's stack size in qty of elements.
  void       *TmrTaskStkPtr;                                    // Ptr to tmr stack's stack base.
} USBD_HID_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBD_HID_INIT_CFG USBD_HID_InitCfgDflt;            // USBD HID class dflt configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_HID_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBD_HID_ConfigureReportID_Qty(CPU_INT08U report_id_qty);

void USBD_HID_ConfigurePushPopItemsQty(CPU_INT08U push_pop_items_qty);

void USBD_HID_ConfigureTmrTaskStk(CPU_INT32U stk_size_elements,
                                  void       *p_stk);

void USBD_HID_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf);
#endif

/********************************************************************************************************
 *                                               HID FUNCTIONS
 *******************************************************************************************************/

//                                                                 HID class initialization.
void USBD_HID_Init(const USBD_HID_QTY_CFG *p_qty_cfg,
                   RTOS_ERR               *p_err);

void USBD_HID_TmrTaskPrioSet(RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err);

//                                                                 Add new instance of the HID class.
CPU_INT08U USBD_HID_Add(CPU_INT08U            subclass,
                        CPU_INT08U            protocol,
                        USBD_HID_COUNTRY_CODE country_code,
                        const CPU_INT08U      *p_report_desc,
                        CPU_INT16U            report_desc_len,
                        const CPU_INT08U      *p_phy_desc,
                        CPU_INT16U            phy_desc_len,
                        CPU_INT16U            interval_in,
                        CPU_INT16U            interval_out,
                        CPU_BOOLEAN           ctrl_rd_en,
                        USBD_HID_CALLBACK     *p_hid_callback,
                        RTOS_ERR              *p_err);

CPU_BOOLEAN USBD_HID_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_HID_IsConn(CPU_INT08U class_nbr);

CPU_INT32U USBD_HID_Wr(CPU_INT08U class_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout,
                       RTOS_ERR   *p_err);

CPU_INT32U USBD_HID_Rd(CPU_INT08U class_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout,
                       RTOS_ERR   *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
