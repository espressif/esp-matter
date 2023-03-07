/***************************************************************************//**
 * @file
 * @brief USB Device Hid Private
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_HID_PRIV_H_
#define  _USBD_HID_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/kal/kal_priv.h>
#include  <usb/include/device/usbd_hid.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum usbd_hid_report_type {
  USBD_HID_REPORT_TYPE_NONE = 0,
  USBD_HID_REPORT_TYPE_INPUT,
  USBD_HID_REPORT_TYPE_OUTPUT,
  USBD_HID_REPORT_TYPE_FEATURE
} USBD_HID_REPORT_TYPE;

typedef struct usbd_hid_report_id USBD_HID_REPORT_ID;

struct usbd_hid_report_id {
  CPU_INT08U         ID;
  CPU_INT16U         Size;
  CPU_INT08U         *DataPtr;
  USBD_HID_REPORT_ID *NextPtr;

  CPU_INT08U         ClassNbr;
  CPU_INT08U         IdleCnt;
  CPU_INT08U         IdleRate;
  CPU_BOOLEAN        Update;
  USBD_HID_REPORT_ID *TmrNextPtr;
};

typedef struct usbd_hid_report {
  CPU_BOOLEAN        HasReports;
  CPU_INT16U         MaxInputReportSize;
  CPU_INT16U         MaxFeatureReportSize;
  CPU_INT08U         *MaxFeatureReportPtr;
  CPU_INT16U         MaxOutputReportSize;
  CPU_INT08U         *MaxOutputReportPtr;
  USBD_HID_REPORT_ID *Reports[3];                               // Index 0: Input Reports; 1: Output; 2: Feature.
} USBD_HID_REPORT;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_hid_ctrl USBD_HID_CTRL;

/********************************************************************************************************
 *                                           HID CLASS STATES
 *******************************************************************************************************/

typedef enum usbd_hid_state {                                   // HID class states.
  USBD_HID_STATE_NONE = 0,
  USBD_HID_STATE_INIT,
  USBD_HID_STATE_CONFIG
} USBD_HID_STATE;

/********************************************************************************************************
 *                                   HID CLASS EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

//                                                                 --------------- HID CLASS COMM INFO ----------------
typedef struct usbd_hid_comm {
  USBD_HID_CTRL *CtrlPtr;                                       // Pointer to control information.
                                                                // Avail EP for comm: Intr
  CPU_INT08U    DataIntrInEpAddr;
  CPU_INT08U    DataIntrOutEpAddr;
  CPU_BOOLEAN   DataIntrOutActiveXfer;
} USBD_HID_COMM;

struct usbd_hid_ctrl {                                          // --------------- HID CLASS CTRL INFO ----------------
  CPU_INT08U            DevNbr;                                 // Dev   nbr.
  CPU_INT08U            ClassNbr;                               // Class nbr.
  USBD_HID_STATE        State;                                  // HID class state.
  USBD_HID_COMM         *CommPtr;                               // HID class comm info ptr.

  USBD_HID_ASYNC_FNCT   IntrRdAsyncFnct;                        // Ptr to async comm callback and arg.
  void                  *IntrRdAsyncArgPtr;
  USBD_HID_ASYNC_FNCT   IntrWrAsyncFnct;
  void                  *IntrWrAsyncArgPtr;
  CPU_INT32U            DataIntrInXferLen;

  CPU_INT08U            SubClassCode;
  CPU_INT08U            ProtocolCode;
  USBD_HID_COUNTRY_CODE CountryCode;
  USBD_HID_REPORT       Report;
  const CPU_INT08U      *ReportDescPtr;
  CPU_INT16U            ReportDescLen;
  const CPU_INT08U      *PhyDescPtr;
  CPU_INT16U            PhyDescLen;
  CPU_INT16U            IntervalIn;
  CPU_INT16U            IntervalOut;
  CPU_BOOLEAN           CtrlRdEn;                               // En rd operations thru ctrl xfer.
  USBD_HID_CALLBACK     *CallbackPtr;                           // Ptr to class-specific desc and req callbacks.
  CPU_INT08U            *RxBufPtr;
  CPU_INT32U            RxBufLen;
  CPU_BOOLEAN           IsRx;

  CPU_INT08U            *CtrlStatusBufPtr;                      // Buf used for ctrl status xfers.
};

typedef struct usbd_hid_report_item {                           // ------------------- REPORT ITEM --------------------
  CPU_INT08U ReportID;
  CPU_INT16U Size;
  CPU_INT16U Cnt;
} USBD_HID_REPORT_ITEM;

typedef struct usbd_hid {                                       // ---------------- HID ROOT STRUCTURE ----------------
                                                                // HID class instance control structures.
  USBD_HID_CTRL        *CtrlTbl;
  CPU_INT08U           CtrlNbrNext;

  //                                                               HID class comm array.
  USBD_HID_COMM        *CommTbl;
  CPU_INT08U           CommNbrNext;

  //                                                               Report item tables.
  USBD_HID_REPORT_ID   *ReportID_Tbl;

  CPU_INT16U           ReportID_Tbl_Ix;
  USBD_HID_REPORT_ID   *ReportID_TmrList;

  USBD_HID_REPORT_ITEM *ReportItemTbl;

#if (DEF_BIT_IS_SET(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, (RTOS_CFG_MODULE_USBD)))
  CPU_INT08U           ClassInstanceQty;
#endif
} USBD_HID;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern USBD_HID_INIT_CFG USBD_HID_InitCfg;
extern USBD_HID          *USBD_HID_Ptr;

/********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *******************************************************************************************************/

void USBD_HID_Report_Init(MEM_SEG  *p_mem_seg,
                          RTOS_ERR *p_err);

void USBD_HID_Report_Parse(CPU_INT08U       class_nbr,
                           const CPU_INT08U *p_report_data,
                           CPU_INT16U       report_data_len,
                           USBD_HID_REPORT  *p_report,
                           RTOS_ERR         *p_err);

CPU_INT16U USBD_HID_ReportID_InfoGet(const USBD_HID_REPORT *p_report,
                                     USBD_HID_REPORT_TYPE  report_type,
                                     CPU_INT08U            report_id,
                                     CPU_INT08U            **p_buf,
                                     CPU_BOOLEAN           *p_is_largest,
                                     RTOS_ERR              *p_err);

CPU_INT08U USBD_HID_ReportID_IdleGet(const USBD_HID_REPORT *p_report,
                                     CPU_INT08U            report_id,
                                     RTOS_ERR              *p_err);

void USBD_HID_ReportID_IdleSet(const USBD_HID_REPORT *p_report,
                               CPU_INT08U            report_id,
                               CPU_INT08U            idle_rate,
                               RTOS_ERR              *p_err);

CPU_BOOLEAN USBD_HID_ReportID_IsLargestIn(const USBD_HID_REPORT *p_report,
                                          CPU_INT08U            report_id,
                                          RTOS_ERR              *p_err);

void USBD_HID_Report_RemoveAllIdle(const USBD_HID_REPORT *p_report);

void USBD_HID_Report_TmrTaskHandler(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
