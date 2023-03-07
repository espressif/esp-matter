/***************************************************************************//**
 * @file
 * @brief USB Device Hid Report
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

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device HID class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE

#include  <em_core.h>

#include  <usb/include/device/usbd_core.h>

#include  <usb/source/device/class/usbd_hid_priv.h>
#include  <usb/include/device/usbd_hid.h>

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, HID, REPORT)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 *                                               HID DEFINES
 *******************************************************************************************************/

#define  USBD_HID_REPORT_ITEM_SIZE_MASK                 0x03u
#define  USBD_HID_REPORT_ITEM_TYPE_MASK                 0x0Cu
#define  USBD_HID_REPORT_ITEM_TAG_MASK                  0xF0u

#define  USBD_HID_IDLE_INFINITE                         0x00u
#define  USBD_HID_IDLE_ALL_REPORT                       0x00u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_HID_ReportClr(USBD_HID_REPORT *p_report);

static USBD_HID_REPORT_ID *USBD_HID_ReportID_Alloc(void);

static USBD_HID_REPORT_ID *USBD_HID_ReportID_Get(USBD_HID_REPORT      *p_report,
                                                 USBD_HID_REPORT_TYPE report_type,
                                                 CPU_INT08U           report_id);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_HID_Report_Init()
 *
 * @brief    Initialize HID report module.
 *
 * @param    p_mem_seg   Pointer to memory segment used to allocate internal data.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_Report_Init(MEM_SEG  *p_mem_seg,
                          RTOS_ERR *p_err)
{
  CPU_INT16U ix;

  USBD_HID_Ptr->ReportID_Tbl = (USBD_HID_REPORT_ID *)Mem_SegAlloc("USBD - HID report ID table",
                                                                  p_mem_seg,
                                                                  sizeof(USBD_HID_REPORT_ID) * USBD_HID_InitCfg.ReportID_Qty,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_Ptr->ReportItemTbl = (USBD_HID_REPORT_ITEM *)Mem_SegAlloc("USBD - HID report item table",
                                                                     p_mem_seg,
                                                                     sizeof(USBD_HID_REPORT_ITEM) * USBD_HID_InitCfg.PushPopItemsQty + 1u,
                                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < USBD_HID_InitCfg.ReportID_Qty; ix++) {
    USBD_HID_REPORT_ID *p_report_id = &USBD_HID_Ptr->ReportID_Tbl[ix];

    p_report_id->ID = 0u;
    p_report_id->Size = 0u;
    p_report_id->DataPtr = DEF_NULL;
    p_report_id->NextPtr = DEF_NULL;

    p_report_id->ClassNbr = USBD_CLASS_NBR_NONE;
    p_report_id->IdleCnt = 0u;
    p_report_id->IdleRate = USBD_HID_IDLE_INFINITE;
    p_report_id->Update = DEF_NO;
    p_report_id->TmrNextPtr = DEF_NULL;
  }

  USBD_HID_Ptr->ReportID_Tbl_Ix = USBD_HID_InitCfg.ReportID_Qty;
  USBD_HID_Ptr->ReportID_TmrList = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           USBD_HID_Report_Parse()
 *
 * @brief    Parse HID report.
 *
 * @param    class_nbr           Class instance number.
 *
 * @param    p_report_data       Pointer to HID report descriptor.
 *
 * @param    report_data_len     Length of  HID report descriptor.
 *
 * @param    p_report            _data       Pointer to HID report descriptor.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from
 *                               this function.
 *******************************************************************************************************/
void USBD_HID_Report_Parse(CPU_INT08U       class_nbr,
                           const CPU_INT08U *p_report_data,
                           CPU_INT16U       report_data_len,
                           USBD_HID_REPORT  *p_report,
                           RTOS_ERR         *p_err)
{
  USBD_HID_REPORT_ID   *p_report_id = DEF_NULL;
  USBD_HID_REPORT_ITEM *p_item;
  CPU_INT08U           item_tbl_size = 0u;
  CPU_INT08U           col_nesting = 0u;
  CPU_INT08U           report_type;

  p_item = USBD_HID_Ptr->ReportItemTbl;
  p_item->ReportID = 0u;
  p_item->Size = 0u;
  p_item->Cnt = 0u;

  USBD_HID_ReportClr(p_report);

  while (report_data_len > 0u) {
    CPU_INT08U tag = *p_report_data;
    CPU_INT32U data = 0u;

    p_report_data++;
    report_data_len--;

    switch (tag & USBD_HID_REPORT_ITEM_SIZE_MASK) {
      case 3u:                                                  // Item size: 4 bytes.
        RTOS_ASSERT_DBG_ERR_SET((report_data_len >= 4u), *p_err, RTOS_ERR_INVALID_ARG,; );

        data = MEM_VAL_GET_INT32U_LITTLE(p_report_data);

        p_report_data += 4u;
        report_data_len -= 4u;
        break;

      case 2u:                                                  // Item size: 2 bytes.
        RTOS_ASSERT_DBG_ERR_SET((report_data_len >= 2u), *p_err, RTOS_ERR_INVALID_ARG,; );

        data = MEM_VAL_GET_INT16U_LITTLE(p_report_data);

        p_report_data += 2u;
        report_data_len -= 2u;
        break;

      case 1u:                                                  // Item size: 1 byte.
        RTOS_ASSERT_DBG_ERR_SET((report_data_len >= 1u), *p_err, RTOS_ERR_INVALID_ARG,; );

        data = *p_report_data;

        p_report_data++;
        report_data_len--;
        break;

      case 0u:                                                  // Item size: 0 bytes.
      default:
        break;
    }

    switch (tag & (USBD_HID_REPORT_ITEM_TYPE_MASK
                   | USBD_HID_REPORT_ITEM_TAG_MASK)) {
      case USBD_HID_MAIN_INPUT:
        p_report_id = USBD_HID_ReportID_Get(p_report,
                                            USBD_HID_REPORT_TYPE_INPUT,
                                            p_item->ReportID);
        if (p_report_id == DEF_NULL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
          return;
        }

        p_report_id->Size += p_item->Cnt * p_item->Size;
        p_report_id->ClassNbr = class_nbr;

        if (p_report->MaxInputReportSize < p_report_id->Size) {
          p_report->MaxInputReportSize = p_report_id->Size;
        }
        break;

      case USBD_HID_MAIN_OUTPUT:
        p_report_id = USBD_HID_ReportID_Get(p_report,
                                            USBD_HID_REPORT_TYPE_OUTPUT,
                                            p_item->ReportID);
        if (p_report_id == DEF_NULL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
          return;
        }

        p_report_id->Size += p_item->Cnt * p_item->Size;
        p_report_id->ClassNbr = class_nbr;

        if (p_report->MaxOutputReportSize < p_report_id->Size) {
          p_report->MaxOutputReportSize = p_report_id->Size;
        }
        break;

      case USBD_HID_MAIN_FEATURE:
        p_report_id = USBD_HID_ReportID_Get(p_report,
                                            USBD_HID_REPORT_TYPE_FEATURE,
                                            p_item->ReportID);
        if (p_report_id == DEF_NULL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
          return;
        }

        p_report_id->Size += p_item->Cnt * p_item->Size;
        p_report_id->ClassNbr = class_nbr;

        if (p_report->MaxFeatureReportSize < p_report_id->Size) {
          p_report->MaxFeatureReportSize = p_report_id->Size;
        }
        break;

      case USBD_HID_MAIN_COLLECTION:
        col_nesting++;
        break;

      case USBD_HID_MAIN_ENDCOLLECTION:
        RTOS_ASSERT_DBG_ERR_SET((col_nesting != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

        col_nesting--;
        break;

      case USBD_HID_GLOBAL_REPORT_SIZE:
        p_item->Size = data & DEF_INT_16_MASK;
        break;

      case USBD_HID_GLOBAL_REPORT_COUNT:
        p_item->Cnt = data & DEF_INT_16_MASK;
        break;

      case USBD_HID_GLOBAL_REPORT_ID:
        p_item->ReportID = data & DEF_INT_08_MASK;
        p_report->HasReports = DEF_YES;
        break;

      case USBD_HID_GLOBAL_PUSH:
        if (item_tbl_size >= USBD_HID_InitCfg.PushPopItemsQty) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
          return;
        }

        p_item = &USBD_HID_Ptr->ReportItemTbl[item_tbl_size + 1u];

        p_item->ReportID = USBD_HID_Ptr->ReportItemTbl[item_tbl_size].ReportID;
        p_item->Size = USBD_HID_Ptr->ReportItemTbl[item_tbl_size].Size;
        p_item->Cnt = USBD_HID_Ptr->ReportItemTbl[item_tbl_size].Cnt;

        item_tbl_size++;
        break;

      case USBD_HID_GLOBAL_POP:
        RTOS_ASSERT_DBG_ERR_SET((item_tbl_size != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

        item_tbl_size--;
        p_item = &USBD_HID_Ptr->ReportItemTbl[item_tbl_size];
        break;

      case USBD_HID_LOCAL_USAGE:
      case USBD_HID_LOCAL_USAGE_MIN:
      case USBD_HID_LOCAL_USAGE_MAX:
      case USBD_HID_GLOBAL_USAGE_PAGE:
      case USBD_HID_GLOBAL_LOG_MIN:
      case USBD_HID_GLOBAL_LOG_MAX:
      case USBD_HID_GLOBAL_PHY_MIN:
      case USBD_HID_GLOBAL_PHY_MAX:
      case USBD_HID_GLOBAL_UNIT_EXPONENT:
      case USBD_HID_GLOBAL_UNIT:
      case USBD_HID_LOCAL_DESIGNATOR_INDEX:
      case USBD_HID_LOCAL_DESIGNATOR_MIN:
      case USBD_HID_LOCAL_DESIGNATOR_MAX:
      case USBD_HID_LOCAL_STRING_INDEX:
      case USBD_HID_LOCAL_STRING_MIN:
      case USBD_HID_LOCAL_STRING_MAX:
      case USBD_HID_LOCAL_DELIMITER:
        break;

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }
  }

  RTOS_ASSERT_DBG_ERR_SET((col_nesting == 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               ----------- CONVERT REPORT SIZE TO OCTETS ----------
  p_report->MaxInputReportSize += 7u;
  p_report->MaxInputReportSize /= 8u;

  p_report->MaxOutputReportSize += 7u;
  p_report->MaxOutputReportSize /= 8u;

  p_report->MaxFeatureReportSize += 7u;
  p_report->MaxFeatureReportSize /= 8u;

  if (p_report->HasReports == DEF_YES) {                        // Reserve space for Report ID.
    if (p_report->MaxInputReportSize > 0u) {
      p_report->MaxInputReportSize++;
    }
    if (p_report->MaxOutputReportSize > 0u) {
      p_report->MaxOutputReportSize++;
    }
    if (p_report->MaxFeatureReportSize > 0u) {
      p_report->MaxFeatureReportSize++;
    }
  }

  if (p_report->MaxOutputReportSize > 0u) {
    p_report->MaxOutputReportPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - HID output report",
                                                                DEF_NULL,
                                                                p_report->MaxOutputReportSize,
                                                                USBD_HID_InitCfg.BufAlignOctets,
                                                                DEF_NULL,
                                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  if (p_report->MaxFeatureReportSize > 0u) {
    p_report->MaxFeatureReportPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - HID feature report",
                                                                 DEF_NULL,
                                                                 p_report->MaxFeatureReportSize,
                                                                 USBD_HID_InitCfg.BufAlignOctets,
                                                                 DEF_NULL,
                                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  for (report_type = 0; report_type < 3u; report_type++) {
    p_report_id = p_report->Reports[report_type];
    while (p_report_id != DEF_NULL) {
      p_report_id->Size += 7u;
      p_report_id->Size /= 8u;

      if (p_report_id->Size > 0u) {
        if (p_report->HasReports == DEF_YES) {
          p_report_id->Size++;                                  // Reserve space for report ID.
        }

        if (report_type == 0u) {                                // Input reports use individual buf.
          p_report_id->DataPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - HID report ID",
                                                              DEF_NULL,
                                                              p_report_id->Size,
                                                              USBD_HID_InitCfg.BufAlignOctets,
                                                              DEF_NULL,
                                                              p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return;
          }
          //                                                       The first byte must be the report ID.
          Mem_Clr(&p_report_id->DataPtr[0u], p_report_id->Size);
          p_report_id->DataPtr[0u] = p_report_id->ID;
        }
      }

      p_report_id = p_report_id->NextPtr;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBD_HID_ReportID_InfoGet()
 *
 * @brief    Retrieve HID report length and pointer to its data area.
 *
 * @param    p_report        Pointer to HID report structure.
 *
 * @param    report_type     HID report type.
 *
 * @param    report_id       HID report ID.
 *
 * @param    p_buf           Pointer to variable that will receive the pointer to the HID report data area.
 *
 * @param    p_is_largest    Pointer to variable that will receive the indication that the HID input or
 *                           feature report is the largest report from the list of input or feature
 *                           reports.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Length of HID report, in octets.
 *******************************************************************************************************/
CPU_INT16U USBD_HID_ReportID_InfoGet(const USBD_HID_REPORT *p_report,
                                     USBD_HID_REPORT_TYPE  report_type,
                                     CPU_INT08U            report_id,
                                     CPU_INT08U            **p_buf,
                                     CPU_BOOLEAN           *p_is_largest,
                                     RTOS_ERR              *p_err)
{
  USBD_HID_REPORT_ID *p_report_id;

  switch (report_type) {
    case USBD_HID_REPORT_TYPE_INPUT:
      p_report_id = p_report->Reports[0u];
      break;

    case USBD_HID_REPORT_TYPE_OUTPUT:
      p_report_id = p_report->Reports[1u];
      break;

    case USBD_HID_REPORT_TYPE_FEATURE:
      p_report_id = p_report->Reports[2u];
      break;

    case USBD_HID_REPORT_TYPE_NONE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return (0);
  }

  while (p_report_id != DEF_NULL) {
    if (p_report_id->ID == report_id) {
      switch (report_type) {
        case USBD_HID_REPORT_TYPE_INPUT:
          if (p_buf != DEF_NULL) {
            *p_buf = p_report_id->DataPtr;
          }
          *p_is_largest = (p_report_id->Size == p_report->MaxInputReportSize) ? DEF_YES : DEF_NO;
          break;

        case USBD_HID_REPORT_TYPE_OUTPUT:
          if (p_buf != DEF_NULL) {
            *p_buf = p_report->MaxOutputReportPtr;
          }
          *p_is_largest = DEF_NO;
          break;

        case USBD_HID_REPORT_TYPE_FEATURE:
        default:
          if (p_buf != DEF_NULL) {
            *p_buf = p_report->MaxFeatureReportPtr;
          }
          *p_is_largest = (p_report_id->Size == p_report->MaxFeatureReportSize) ? DEF_YES : DEF_NO;
          break;
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return (p_report_id->Size);
    }

    p_report_id = p_report_id->NextPtr;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);

  return (0);
}

/****************************************************************************************************//**
 *                                       USBD_HID_Report_TmrTaskHandler()
 *
 * @brief    Process all periodic HID input reports.
 *******************************************************************************************************/
void USBD_HID_Report_TmrTaskHandler(void)
{
  USBD_HID_REPORT_ID *p_report_id;
  USBD_HID_REPORT_ID *p_report_id_prev;
  CPU_BOOLEAN        service;
  RTOS_ERR           err;
  CORE_DECLARE_IRQ_STATE;

  p_report_id_prev = (USBD_HID_REPORT_ID *)0;
  CORE_ENTER_ATOMIC();
  p_report_id = USBD_HID_Ptr->ReportID_TmrList;
  CORE_EXIT_ATOMIC();

  while (p_report_id != DEF_NULL) {
    service = DEF_NO;

    CORE_ENTER_ATOMIC();
    if (p_report_id->IdleRate == USBD_HID_IDLE_INFINITE) {
      p_report_id->IdleCnt = 0u;
      p_report_id->Update = DEF_NO;

      if (p_report_id_prev == DEF_NULL) {
        USBD_HID_Ptr->ReportID_TmrList = p_report_id->TmrNextPtr;
      } else {
        p_report_id_prev->TmrNextPtr = p_report_id->TmrNextPtr;
      }

      p_report_id->TmrNextPtr = DEF_NULL;
    } else {
      if (p_report_id->Update == DEF_YES) {
        p_report_id->Update = DEF_NO;

        if (p_report_id->IdleCnt > 1) {
          p_report_id->IdleCnt = p_report_id->IdleRate;
        }
      }

      if (p_report_id->IdleCnt > 1) {
        p_report_id->IdleCnt--;
      } else {
        p_report_id->IdleCnt = p_report_id->IdleRate;
        service = DEF_YES;
      }
    }
    CORE_EXIT_ATOMIC();

    if (service == DEF_YES) {
      USBD_HID_Wr(p_report_id->ClassNbr,
                  p_report_id->DataPtr,
                  p_report_id->Size,
                  100,
                  &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error while submitting HID report to host: ", RTOS_ERR_LOG_ARG_GET(err)));
      }
    }

    p_report_id_prev = p_report_id;
    p_report_id = p_report_id->TmrNextPtr;
  }
}

/****************************************************************************************************//**
 *                                       USBD_HID_ReportID_IdleGet()
 *
 * @brief    Retrieve HID input report idle rate.
 *
 * @param    p_report    Pointer to HID report structure.
 *
 * @param    report_id   HID report ID.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Idle rate.
 *
 * @note     (1) Idle rate is in 4 millisecond units.
 *******************************************************************************************************/
CPU_INT08U USBD_HID_ReportID_IdleGet(const USBD_HID_REPORT *p_report,
                                     CPU_INT08U            report_id,
                                     RTOS_ERR              *p_err)
{
  USBD_HID_REPORT_ID *p_report_id;

  p_report_id = p_report->Reports[0u];

  while (p_report_id != DEF_NULL) {
    if (p_report_id->ID == report_id) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return (p_report_id->IdleRate);
    }

    p_report_id = p_report_id->NextPtr;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);

  return (0);
}

/****************************************************************************************************//**
 *                                       USBD_HID_ReportID_IdleSet()
 *
 * @brief    Set HID input report idle rate.
 *
 * @param    p_report    Pointer to HID report structure.
 *
 * @param    report_id   HID report ID.
 *
 * @param    idle_rate   Report idle rate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Idle rate is in 4 millisecond units.
 *******************************************************************************************************/
void USBD_HID_ReportID_IdleSet(const USBD_HID_REPORT *p_report,
                               CPU_INT08U            report_id,
                               CPU_INT08U            idle_rate,
                               RTOS_ERR              *p_err)
{
  USBD_HID_REPORT_ID *p_report_id;
  CORE_DECLARE_IRQ_STATE;

  p_report_id = p_report->Reports[0u];

  RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);

  while (p_report_id != DEF_NULL) {
    if ((p_report_id->ID == report_id)
        || (report_id == USBD_HID_IDLE_ALL_REPORT)) {
      if (idle_rate != USBD_HID_IDLE_INFINITE) {
        CORE_ENTER_ATOMIC();
        //                                                         Add report ID into timer list.
        if (p_report_id->IdleRate == USBD_HID_IDLE_INFINITE) {
          p_report_id->TmrNextPtr = USBD_HID_Ptr->ReportID_TmrList;
          USBD_HID_Ptr->ReportID_TmrList = p_report_id;
        }

        p_report_id->IdleRate = idle_rate;
        p_report_id->Update = DEF_YES;

        CORE_EXIT_ATOMIC();
      } else {
        CORE_ENTER_ATOMIC();
        p_report_id->IdleRate = USBD_HID_IDLE_INFINITE;
        CORE_EXIT_ATOMIC();
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      if (report_id != USBD_HID_IDLE_ALL_REPORT) {
        return;
      }
    }

    p_report_id = p_report_id->NextPtr;
  }
}

/****************************************************************************************************//**
 *                                       USBD_HID_Report_RemoveAllIdle()
 *
 * @brief    Remove all HID input report from periodic service list.
 *
 * @param    p_report    Pointer to HID report structure.
 *******************************************************************************************************/
void USBD_HID_Report_RemoveAllIdle(const USBD_HID_REPORT *p_report)
{
  USBD_HID_REPORT_ID *p_report_id;
  CORE_DECLARE_IRQ_STATE;

  p_report_id = p_report->Reports[0u];

  while (p_report_id != DEF_NULL) {
    CORE_ENTER_ATOMIC();                                        // Remove only reports present on timer list.
    if (p_report_id->TmrNextPtr != DEF_NULL) {
      p_report_id->IdleRate = USBD_HID_IDLE_INFINITE;
    }
    CORE_EXIT_ATOMIC();

    p_report_id = p_report_id->NextPtr;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_HID_ReportClr()
 *
 * @brief    Initialize HID report structure.
 *
 * @param    p_report    Pointer to HID report structure.
 *******************************************************************************************************/
static void USBD_HID_ReportClr(USBD_HID_REPORT *p_report)
{
  p_report->HasReports = 0;
  p_report->MaxInputReportSize = 0;
  p_report->MaxOutputReportSize = 0;
  p_report->MaxFeatureReportSize = 0;

  p_report->MaxOutputReportPtr = DEF_NULL;
  p_report->MaxFeatureReportPtr = DEF_NULL;

  p_report->Reports[0u] = DEF_NULL;
  p_report->Reports[1u] = DEF_NULL;
  p_report->Reports[2u] = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           USBD_HID_ReportID_Alloc()
 *
 * @brief    Allocate an instance of HID report ID structure.
 *
 * @return   Pointer to HID report ID structure, if NO error(s).
 *
 *           Pointer to NULL,                    otherwise.
 *******************************************************************************************************/
static USBD_HID_REPORT_ID *USBD_HID_ReportID_Alloc(void)
{
  USBD_HID_REPORT_ID *p_report_id;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (USBD_HID_Ptr->ReportID_Tbl_Ix == 0u) {
    CORE_EXIT_ATOMIC();
    return (DEF_NULL);
  }

  USBD_HID_Ptr->ReportID_Tbl_Ix--;

  p_report_id = &USBD_HID_Ptr->ReportID_Tbl[USBD_HID_Ptr->ReportID_Tbl_Ix];
  CORE_EXIT_ATOMIC();

  return (p_report_id);
}

/****************************************************************************************************//**
 *                                           USBD_HID_ReportID_Get()
 *
 * @brief    Retrieve HID report ID structure.
 *
 * @param    p_report        Pointer to HID report structure.
 *
 * @param    report_type     HID report type.
 *
 * @param    report_id       HID report ID.
 *
 * @return   Pointer to HID report ID structure, if NO error(s).
 *
 *           Pointer to NULL,                    otherwise.
 *
 * @note     (1) If HID report ID structure is not available for the specific report type and ID, an
 *               instance of HID report ID structure is allocated and linked into the HID report
 *               structure.
 *******************************************************************************************************/
static USBD_HID_REPORT_ID *USBD_HID_ReportID_Get(USBD_HID_REPORT      *p_report,
                                                 USBD_HID_REPORT_TYPE report_type,
                                                 CPU_INT08U           report_id)
{
  USBD_HID_REPORT_ID *p_report_id;
  USBD_HID_REPORT_ID *p_report_id_prev;
  CPU_INT08U         type;

  switch (report_type) {
    case USBD_HID_REPORT_TYPE_INPUT:
      type = 0u;
      break;

    case USBD_HID_REPORT_TYPE_OUTPUT:
      type = 1u;
      break;

    case USBD_HID_REPORT_TYPE_FEATURE:
      type = 2u;
      break;

    case USBD_HID_REPORT_TYPE_NONE:
    default:
      return (DEF_NULL);
  }

  if (p_report->HasReports == DEF_NO) {
    if (report_id > 0) {
      return (DEF_NULL);
    }

    if (p_report->Reports[type] == DEF_NULL) {
      p_report->Reports[type] = USBD_HID_ReportID_Alloc();
      if (p_report->Reports[type] == DEF_NULL) {
        return (DEF_NULL);
      }
    }

    p_report_id = p_report->Reports[type];

    return (p_report_id);
  }

  p_report_id = p_report->Reports[type];
  p_report_id_prev = DEF_NULL;
  while (p_report_id != DEF_NULL) {
    if (p_report_id->ID == report_id) {
      return (p_report_id);
    }

    p_report_id_prev = p_report_id;
    p_report_id = p_report_id->NextPtr;
  }

  p_report_id = USBD_HID_ReportID_Alloc();
  if (p_report_id == DEF_NULL) {
    return (DEF_NULL);
  }

  p_report_id->ID = report_id;

  if (p_report->Reports[type] == DEF_NULL) {
    p_report->Reports[type] = p_report_id;
  } else {
    p_report_id_prev->NextPtr = p_report_id;
  }

  return (p_report_id);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))
