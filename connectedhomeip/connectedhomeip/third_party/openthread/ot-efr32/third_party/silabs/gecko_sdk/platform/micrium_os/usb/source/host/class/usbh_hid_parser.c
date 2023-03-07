/***************************************************************************//**
 * @file
 * @brief USB Host Hid Class Parser
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

#if (defined(RTOS_MODULE_USB_HOST_HID_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST HID class requires USB Host core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_HIDPARSER_MODULE
#define   MICRIUM_SOURCE

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_hid.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_hid_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                                   GENERIC
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBH, CLASS, HID, PARSER)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 *                                               ITEM DATA
 *******************************************************************************************************/

#define  USBH_HID_ITEM_SIZE(x)                   (((x) & 0x03u) == 3u  ? 4u : ((x) & 0x03u))

#define  USBH_HID_ITEM_TYPE(x)                  ((((x) & 0x0Cu) >> 2u) & 0x03u)

#define  USBH_HID_ITEM_TAG(x)                   ((((x) & 0xF0u) >> 4u) & 0x0Fu)

#define  USBH_HID_SIGNED_DATA(p_parse_info)      (((p_parse_info)->Size == 1u) ? (p_parse_info)->Data.S08   \
                                                  : ((p_parse_info)->Size == 2u) ? (p_parse_info)->Data.S16 \
                                                  : ((p_parse_info)->Size == 4u) ? (p_parse_info)->Data.S32 : 0)

#define  USBH_HID_UNSIGNED_DATA(p_parse_info)    (((p_parse_info)->Size == 1u) ? (p_parse_info)->Data.U08   \
                                                  : ((p_parse_info)->Size == 2u) ? (p_parse_info)->Data.U16 \
                                                  : ((p_parse_info)->Size == 4u) ? (p_parse_info)->Data.U32 : 0u)

/********************************************************************************************************
 *                                           UNDEFINED VALUES
 *******************************************************************************************************/

#define  USBH_HID_USAGEPAGE_UNDEFINED             0x00000000u
#define  USBH_HID_LOG_MIN_UNDEFINED               DEF_INT_32S_MIN_VAL
#define  USBH_HID_LOG_MAX_UNDEFINED               DEF_INT_32S_MAX_VAL
#define  USBH_HID_PHY_MIN_UNDEFINED               DEF_INT_32S_MIN_VAL
#define  USBH_HID_PHY_MAX_UNDEFINED               DEF_INT_32S_MAX_VAL
#define  USBH_HID_UNIT_EXP_UNDEFINED              DEF_INT_32S_MIN_VAL
#define  USBH_HID_UNIT_UNDEFINED                  0xFFFFFFFF
#define  USBH_HID_USAGE_MIN_UNDEFINED             0xFFFFFFFFu
#define  USBH_HID_USAGE_MAX_UNDEFINED             0xFFFFFFFFu

#define  USBH_HID_REPORT_SIZE_UNDEFINED                    0u
#define  USBH_HID_REPORT_CNT_UNDEFINED                     0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           HID COLLECTION
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2.6 for more details about HID collection.
 *******************************************************************************************************/

typedef struct usbh_hid_coll USBH_HID_COLL;
struct usbh_hid_coll {
  CPU_INT32U    Usage;                                          // Coll usage.
  CPU_INT08U    Type;                                           // Coll type (App / Physical / Logical).
  USBH_HID_COLL *NextPtr;                                       // Ptr to next HID collection.
};

/********************************************************************************************************
 *                                               HID GLOBAL
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2.7 for more details about HID Global Items.
 *******************************************************************************************************/

typedef struct usbh_hid_global USBH_HID_GLOBAL;
struct usbh_hid_global {
  CPU_INT16U      UsagePage;                                    // Usage Page.
  CPU_INT32S      LogMin;                                       // Logical Minimun.
  CPU_INT32S      LogMax;                                       // Logical Maximum.
  CPU_INT32S      PhyMin;                                       // Physical Minimum.
  CPU_INT32S      PhyMax;                                       // Physical Maximum.
  CPU_INT32S      UnitExp;                                      // Unit exponent.
  CPU_INT32S      Unit;                                         // Unit.
  CPU_INT32U      ReportSize;                                   // Report Size (bits).
  CPU_INT08U      ReportID;                                     // Report ID.
  CPU_INT32U      ReportCnt;                                    // Report Count.
  USBH_HID_GLOBAL *NextPtr;                                     // Pointer to next global item.
};

/********************************************************************************************************
 *                                               HID LOCAL
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2.8 for more details about HID Local Items.
 *******************************************************************************************************/

typedef struct usbh_hid_local {
  CPU_INT32U *UsageTbl;                                         // Usage tbl.
  CPU_INT08U NbrUsage;                                          // Nbr of usage in tbl.
  CPU_INT32U UsageMin;                                          // Usage Minimum.
  CPU_INT32U UsageMax;                                          // Usage Maximum.
} USBH_HID_LOCAL;

/********************************************************************************************************
 *                                               HID PARSE INFO
 *******************************************************************************************************/

typedef struct usbh_hid_parse_info {
  USBH_HID_LOCAL    Local;                                      // Cur local  item.
  USBH_HID_GLOBAL   Global;                                     // Cur global item.
  USBH_HID_GLOBAL   *GlobalStkPtr;                              // Global items stack for push and pop.
  USBH_HID_COLL     *CollStkPtr;                                // Collection stack.

  USBH_HID_APP_COLL *AppCollHeadPtr;                            // Ptr to head of app colls list.

  CPU_INT08U        Type;                                       // Item Type.
  CPU_INT08U        Tag;                                        // Item Tag.
  CPU_INT08U        Size;                                       // Item Size.
  union {
    CPU_INT08U U08;
    CPU_INT08S S08;
    CPU_INT16U U16;
    CPU_INT16S S16;
    CPU_INT32U U32;
    CPU_INT32S S32;
  } Data;                                                       // Item Data.
} USBH_HID_PARSE_INFO;

/********************************************************************************************************
 *                                       HID PARSER DATA STRUCTURE
 *******************************************************************************************************/

typedef struct usbh_hid_parser {
  MEM_DYN_POOL        AppCollItemsPool;                         // App collection items pool.
  MEM_DYN_POOL        CollItemsPool;                            // Collection items pool.
  MEM_DYN_POOL        GlobalItemsPool;                          // Global     items pool.
  MEM_DYN_POOL        ReportFmtItemPool;                        // Report fmt items pool.

  USBH_HID_PARSE_INFO *ParseInfoPtr;                            // Ptr to parse info struct.
} USBH_HID_PARSER;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_HID_PARSER *USBH_HID_ParserPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_HID_InitItemParser(USBH_HID_PARSE_INFO *p_parse_info);

static void USBH_HID_FreeItemParser(USBH_HID_PARSE_INFO *p_parse_info);

static void USBH_HID_InitGlobalItem(USBH_HID_GLOBAL *p_global);

static void USBH_HID_InitLocalItem(USBH_HID_LOCAL *p_local);

static void USBH_HID_ParseMain(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err);

static void USBH_HID_ParseGlobal(USBH_HID_PARSE_INFO *p_parse_info,
                                 RTOS_ERR            *p_err);

static void USBH_HID_ParseLocal(USBH_HID_PARSE_INFO *p_parse_info,
                                RTOS_ERR            *p_err);

static void USBH_HID_OpenColl(USBH_HID_PARSE_INFO *p_parse_info,
                              RTOS_ERR            *p_err);

static void USBH_HID_CloseColl(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err);

static void USBH_HID_AddReport(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err);

static void USBH_HID_ValidateReport(USBH_HID_REPORT_FMT *p_report_fmt,
                                    RTOS_ERR            *p_err);

static void USBH_HID_InitReport(USBH_HID_PARSE_INFO *p_parse_info,
                                USBH_HID_REPORT_FMT *p_report_fmt);

static void USBH_HID_MaxReportLenSet(USBH_HID_FNCT       *p_hid_fnct,
                                     USBH_HID_PARSE_INFO *p_parse_info);

static void USBH_HID_SetData(USBH_HID_PARSE_INFO *p_parse_info,
                             void                *p_buf);

static CPU_BOOLEAN USBH_HID_ParserReportFmtItemAllocCallback(MEM_DYN_POOL *p_pool,
                                                             MEM_SEG      *p_seg,
                                                             void         *p_blk,
                                                             void         *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HID_ParserInit()
 *
 * @brief    Initialize HID parser.
 *
 * @param    p_err   Pointer to variable that will receive return error code from this function.
 *******************************************************************************************************/
void USBH_HID_ParserInit(RTOS_ERR *p_err)
{
  //                                                               ---------- ALLOC HID PARSER INTERNAL DATA ----------
  USBH_HID_ParserPtr = (USBH_HID_PARSER *)Mem_SegAlloc("USBH - HID Parser root struct",
                                                       USBH_HID_InitCfg.MemSegPtr,
                                                       sizeof(USBH_HID_PARSER),
                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_HID_ParserPtr->ParseInfoPtr = (USBH_HID_PARSE_INFO *)Mem_SegAlloc("USBH - HID Parse Info struct",
                                                                         USBH_HID_InitCfg.MemSegPtr,
                                                                         sizeof(USBH_HID_PARSE_INFO),
                                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Local item usage tbl.
  USBH_HID_ParserPtr->ParseInfoPtr->Local.UsageTbl = (CPU_INT32U *)Mem_SegAlloc("USBH - HID Parser local item usages",
                                                                                USBH_HID_InitCfg.MemSegPtr,
                                                                                (sizeof(CPU_INT32U) * USBH_HID_InitCfg.UsageMaxNbrPerItem),
                                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("USBH HID Parser - App Coll Items",
                    &USBH_HID_ParserPtr->AppCollItemsPool,
                    USBH_HID_InitCfg.MemSegPtr,
                    sizeof(USBH_HID_APP_COLL),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseAppCollItemQty,
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseAppCollItemQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("HID_ParserGlobalInit() - Missing octets for collections pool."));
    return;
  }

  Mem_DynPoolCreate("USBH HID Parser - Coll Items",
                    &USBH_HID_ParserPtr->CollItemsPool,
                    USBH_HID_InitCfg.MemSegPtr,
                    sizeof(USBH_HID_COLL),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseCollItemQty,
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseCollItemQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("HID_ParserGlobalInit() - Missing octets for collections pool."));
    return;
  }

  Mem_DynPoolCreate("USBH HID Parser - Global Items",
                    &USBH_HID_ParserPtr->GlobalItemsPool,
                    USBH_HID_InitCfg.MemSegPtr,
                    sizeof(USBH_HID_GLOBAL),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseGlobalItemQty,
                    USBH_HID_InitCfg.InitAlloc.ReportDescParseGlobalItemQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("HID_ParserGlobalInit() - Missing octets for global items pool."));
    return;
  }

  Mem_DynPoolCreatePersistent("USBH HID Parser - Report Format Items",
                              &USBH_HID_ParserPtr->ReportFmtItemPool,
                              USBH_HID_InitCfg.MemSegPtr,
                              sizeof(USBH_HID_REPORT_FMT_ITEM),
                              sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                              USBH_HID_InitCfg.InitAlloc.ReportDescParseReportFmtItemQty,
                              USBH_HID_InitCfg.InitAlloc.ReportDescParseReportFmtItemQty,
#else
                              0u,
                              LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                              USBH_HID_ParserReportFmtItemAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("HID_ParserGlobalInit() - Missing octets for report format items pool."));
    return;
  }

  return;
}

/****************************************************************************************************//**
 *                                       USBH_HID_ReportDescParse()
 *
 * @brief    Parse report descriptor.
 *
 * @param    p_hid_fnct      Pointer to HID function.
 *
 * @param    p_report_desc   Pointer to report descriptor buffer.
 *
 * @param    desc_len        Length of report descriptor buffer in octets.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *
 * @return   Pointer to head of app collection linked list.
 *******************************************************************************************************/
USBH_HID_APP_COLL *USBH_HID_ReportDescParse(USBH_HID_FNCT *p_hid_fnct,
                                            CPU_INT08U    *p_report_desc,
                                            CPU_INT32U    desc_len,
                                            RTOS_ERR      *p_err)
{
  USBH_HID_PARSE_INFO *p_parse_info = USBH_HID_ParserPtr->ParseInfoPtr;
  CPU_INT32U          pos;
  CPU_INT08U          item;

  //                                                               ----------------- INITIALIZE DATA ------------------
  USBH_HID_InitItemParser(p_parse_info);
  pos = 0u;

  RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);

  //                                                               ------------- PARSE REPORT DESCRIPTOR --------------
  while (pos < desc_len) {
    item = p_report_desc[pos++];
    p_parse_info->Size = USBH_HID_ITEM_SIZE(item);
    p_parse_info->Type = USBH_HID_ITEM_TYPE(item);
    p_parse_info->Tag = USBH_HID_ITEM_TAG(item);

    USBH_HID_SetData(p_parse_info, (void *)&p_report_desc[pos]);
    pos += p_parse_info->Size;

    if (item == USBH_HID_ITEM_LONG) {                           // If item is a long item ...
      LOG_ERR(("HID_ItemParser() - Long item found"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);             // ... rtn err.
      break;
    }

    switch (p_parse_info->Type) {
      case USBH_HID_ITEM_TYPE_MAIN:                             // Parse main item.
        USBH_HID_ParseMain(p_parse_info, p_err);
        break;

      case USBH_HID_ITEM_TYPE_GLOBAL:                           // Parse global item.
        USBH_HID_ParseGlobal(p_parse_info, p_err);
        break;

      case USBH_HID_ITEM_TYPE_LOCAL:                            // Parse local item.
        USBH_HID_ParseLocal(p_parse_info, p_err);

        if (pos <= 6u) {                                        // Store device usage.
          p_hid_fnct->Usage = p_parse_info->Local.UsageTbl[0u];
        }
        break;

      default:
        LOG_ERR(("HID_ItemParser() - Unknown item type"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        break;
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBH_HID_MaxReportLenSet(p_hid_fnct, p_parse_info);
  }

  //                                                               --------------- HANDLE PARSER ERRORS ---------------
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // Parsing err.
    USBH_HID_FreeItemParser(p_parse_info);
  } else if (p_parse_info->CollStkPtr != DEF_NULL) {            // Collection stack err.
    LOG_ERR(("HID_ItemParser() - Mismatch in collections descriptor length: ", (u)desc_len));
    USBH_HID_FreeItemParser(p_parse_info);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
  } else if (p_parse_info->GlobalStkPtr != DEF_NULL) {          // Global stack err.
    LOG_ERR(("HID_ItemParser() - Mismatch in push and pop items"));
    USBH_HID_FreeItemParser(p_parse_info);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    return (USBH_HID_ParserPtr->ParseInfoPtr->AppCollHeadPtr);
  } else {
    return (DEF_NULL);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HID_InitItemParser()
 *
 * @brief    Initialize HID parser.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *******************************************************************************************************/
static void USBH_HID_InitItemParser(USBH_HID_PARSE_INFO *p_parse_info)
{
  USBH_HID_APP_COLL *p_app_coll_item = p_parse_info->AppCollHeadPtr;
  RTOS_ERR          err_lib;

  while (p_app_coll_item != DEF_NULL) {
    USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item = p_app_coll_item->ReportFmtItemHeadPtr;

    while (p_report_fmt_item != DEF_NULL) {
      //                                                           Free report fmt item.
      p_app_coll_item->ReportFmtItemHeadPtr = p_report_fmt_item->NextPtr;

      Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->ReportFmtItemPool,
                         (void *)p_report_fmt_item,
                         &err_lib);
      PP_UNUSED_PARAM(err_lib);

      p_report_fmt_item = p_app_coll_item->ReportFmtItemHeadPtr;
    }

    p_parse_info->AppCollHeadPtr = p_app_coll_item->NextPtr;

    Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->AppCollItemsPool,
                       (void *)p_app_coll_item,
                       &err_lib);
    PP_UNUSED_PARAM(err_lib);

    p_app_coll_item = p_parse_info->AppCollHeadPtr;
  }

  p_parse_info->GlobalStkPtr = DEF_NULL;
  p_parse_info->CollStkPtr = DEF_NULL;
  p_parse_info->Data.U32 = 0u;
  p_parse_info->Type = 0u;
  p_parse_info->Tag = 0u;
  p_parse_info->Size = 0u;

  USBH_HID_InitGlobalItem(&p_parse_info->Global);
  USBH_HID_InitLocalItem(&p_parse_info->Local);
}

/********************************************************************************************************
 *                                       USBH_HID_FreeItemParser()
 *
 * Description : Free memory allocated for parser structure.
 *
 * Argument(s) : p_parse_info    Pointer to parse info structure.
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void USBH_HID_FreeItemParser(USBH_HID_PARSE_INFO *p_parse_info)
{
  USBH_HID_GLOBAL *p_global;
  USBH_HID_GLOBAL *p_global_free;
  USBH_HID_COLL   *p_coll;
  USBH_HID_COLL   *p_coll_free;
  RTOS_ERR        err_lib;

  //                                                               --------------- FREE THE GLOBAL STACK --------------
  p_global = p_parse_info->GlobalStkPtr;
  while (p_global != DEF_NULL) {
    p_global_free = p_global;
    p_global = p_global->NextPtr;

    Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->GlobalItemsPool, (void *)p_global_free, &err_lib);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  //                                                               ------------- FREE THE COLLECTION STACK ------------
  p_coll = p_parse_info->CollStkPtr;
  while (p_coll != DEF_NULL) {
    p_coll_free = p_coll;
    p_coll = p_coll->NextPtr;

    Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->CollItemsPool, (void *)p_coll_free, &err_lib);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           USBH_HID_InitGlobalItem()
 *
 * @brief    Initialize global item.
 *
 * @param    p_global    Pointer to global item.
 *******************************************************************************************************/
static void USBH_HID_InitGlobalItem(USBH_HID_GLOBAL *p_global)
{
  p_global->UsagePage = 0u;
  p_global->LogMin = USBH_HID_LOG_MIN_UNDEFINED;
  p_global->LogMax = (CPU_INT32S)USBH_HID_LOG_MAX_UNDEFINED;
  p_global->PhyMin = USBH_HID_PHY_MIN_UNDEFINED;
  p_global->PhyMax = (CPU_INT32S)USBH_HID_PHY_MAX_UNDEFINED;
  p_global->UnitExp = USBH_HID_UNIT_EXP_UNDEFINED;
  p_global->Unit = (CPU_INT32S)USBH_HID_UNIT_UNDEFINED;
  p_global->ReportSize = 0u;
  p_global->ReportID = 0u;
  p_global->ReportCnt = 0u;
  p_global->NextPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           USBH_HID_InitLocalItem()
 *
 * @brief    Initialize the local item.
 *
 * @param    p_local     Pointer to the local item.
 *******************************************************************************************************/
static void USBH_HID_InitLocalItem(USBH_HID_LOCAL *p_local)
{
  p_local->NbrUsage = 0u;
  p_local->UsageMin = USBH_HID_USAGE_MIN_UNDEFINED;
  p_local->UsageMax = USBH_HID_USAGE_MAX_UNDEFINED;
}

/****************************************************************************************************//**
 *                                           USBH_HID_ParseMain()
 *
 * @brief    Parse a main item.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_ParseMain(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err)
{
  //                                                               -------------------- PARSE ITEM --------------------
  switch (p_parse_info->Tag) {
    case USBH_HID_MAIN_ITEM_TAG_FEATURE:
    case USBH_HID_MAIN_ITEM_TAG_IN:
    case USBH_HID_MAIN_ITEM_TAG_OUT:
      USBH_HID_AddReport(p_parse_info, p_err);                  // Add report to report list.
      break;

    case USBH_HID_MAIN_ITEM_TAG_COLL:
      USBH_HID_OpenColl(p_parse_info, p_err);                   // Open new collection.
      break;

    case USBH_HID_MAIN_ITEM_TAG_ENDCOLL:
      USBH_HID_CloseColl(p_parse_info, p_err);                  // Close collection.
      break;

    default:
      LOG_ERR(("HID_ParseMain() - Unknown main tag"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      break;
  }
  //                                                               ---------------- RE-INIT LOCAL ITEM ----------------
  USBH_HID_InitLocalItem(&p_parse_info->Local);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_ParseGlobal()
 *
 * @brief    Parse a global item.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_ParseGlobal(USBH_HID_PARSE_INFO *p_parse_info,
                                 RTOS_ERR            *p_err)
{
  USBH_HID_GLOBAL *p_global_new;
  USBH_HID_GLOBAL *p_global_free;
  RTOS_ERR        err_lib;

  switch (p_parse_info->Tag) {
    case USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
      p_parse_info->Global.UsagePage = USBH_HID_UNSIGNED_DATA(p_parse_info);

      if (p_parse_info->Size > 2u) {                            // Rtn err if parser size is too large.
        LOG_ERR(("HID_ParseGlobal() - Global item usage page: out of range"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }

      if ((p_parse_info->Global.UsagePage >= 0x0092u)           // Warn if usage page is a reserved value.
          && (p_parse_info->Global.UsagePage < 0xFEFFu)) {
        LOG_ERR(("HID_ParseGlobal() - Global item usage page: reserved value"));
      }
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_LOG_MIN:
      p_parse_info->Global.LogMin = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_LOG_MAX:
      p_parse_info->Global.LogMax = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_PHY_MIN:
      p_parse_info->Global.PhyMin = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_PHY_MAX:
      p_parse_info->Global.PhyMax = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT:
      p_parse_info->Global.UnitExp = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_UNIT:
      p_parse_info->Global.Unit = USBH_HID_SIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
      p_parse_info->Global.ReportSize = USBH_HID_UNSIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID:
      p_parse_info->Global.ReportID = USBH_HID_UNSIGNED_DATA(p_parse_info);

      if (p_parse_info->Global.ReportID == 0u) {                // Report ID should not be zero.
        LOG_ERR(("HID_ParseGlobal() - Global item report ID: null value"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }

      if (p_parse_info->Size > 1u) {                            // Report ID should be 1-byte value.
        LOG_ERR(("HID_ParseGlobal() - Global item report ID: value > 255"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
      p_parse_info->Global.ReportCnt = USBH_HID_UNSIGNED_DATA(p_parse_info);

      if (p_parse_info->Global.ReportCnt == 0u) {               // Report cnt should not be zero.
        LOG_ERR(("HID_ParseGlobal() - Global item report count: null value"));
      }
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_PUSH:                         // Push item to stack.
      if (p_parse_info->Size != 0u) {
        LOG_ERR(("HID_ParseGlobal() - Global item push: size 0"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }
      //                                                           Allocate memory for new entry.
      p_global_new = (USBH_HID_GLOBAL *)Mem_DynPoolBlkGet(&USBH_HID_ParserPtr->GlobalItemsPool,
                                                          &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("HID_ParseGlobal() - Global item push: out of memory"));
        return;
      }

      *p_global_new = p_parse_info->Global;                     // Copy global to new entry.
      p_global_new->NextPtr = p_parse_info->GlobalStkPtr;
      p_parse_info->GlobalStkPtr = p_global_new;
      break;

    case USBH_HID_GLOBAL_ITEM_TAG_POP:                          // Pop item from stack.
      if (p_parse_info->Size != 0u) {
        LOG_ERR(("HID_ParseGlobal() - Global item pop: size 0"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }
      //                                                           Global stack should be not empty.
      if (p_parse_info->GlobalStkPtr == DEF_NULL) {
        LOG_ERR(("HID_ParseGlobal() - Global item pop: mismatch push/pop"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return;
      }
      //                                                           Copy top element to global.
      p_parse_info->Global = *(p_parse_info->GlobalStkPtr);
      p_global_free = p_parse_info->GlobalStkPtr;
      p_parse_info->GlobalStkPtr = p_global_free->NextPtr;

      //                                                           Free top item.
      Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->GlobalItemsPool, (void *)p_global_free, &err_lib);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      break;

    default:
      LOG_ERR(("HID_ParseGlobal() - Global item unknown"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_ParseLocal()
 *
 * @brief    Parse a local item.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_ParseLocal(USBH_HID_PARSE_INFO *p_parse_info,
                                RTOS_ERR            *p_err)
{
  switch (p_parse_info->Tag) {
    case USBH_HID_LOCAL_ITEM_TAG_USAGE:

      p_parse_info->Local.NbrUsage++;

      if (p_parse_info->Local.NbrUsage > USBH_HID_InitCfg.UsageMaxNbrPerItem) {
        LOG_ERR(("HID_ParseLocal() - Local item usage failed to allocate"));
        return;
      }

      p_parse_info->Local.UsageTbl[p_parse_info->Local.NbrUsage - 1u] = USBH_HID_UNSIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN:
      p_parse_info->Local.UsageMin = USBH_HID_UNSIGNED_DATA(p_parse_info);
      break;

    case USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX:
      p_parse_info->Local.UsageMax = USBH_HID_UNSIGNED_DATA(p_parse_info);
      break;

    default:
      break;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_OpenColl()
 *
 * @brief    Add new collection item to collection stack.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_OpenColl(USBH_HID_PARSE_INFO *p_parse_info,
                              RTOS_ERR            *p_err)
{
  USBH_HID_APP_COLL *p_app_coll;
  USBH_HID_COLL     *p_coll_new;

  //                                                               --------------- INIT NEW COLLECTION ----------------
  p_coll_new = (USBH_HID_COLL *)Mem_DynPoolBlkGet(&USBH_HID_ParserPtr->CollItemsPool,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               Initialize collection parameters.
  p_coll_new->Usage = (((CPU_INT32U)p_parse_info->Global.UsagePage << 16u) & 0xFFFF0000u) | (CPU_INT32U)p_parse_info->Local.UsageTbl[0u];
  p_coll_new->Type = USBH_HID_UNSIGNED_DATA(p_parse_info);

  if ((p_parse_info->CollStkPtr == DEF_NULL)                    // Check if first collection is app.
      && (p_coll_new->Type != USBH_HID_COLL_APP)) {
    LOG_ERR(("HID_OpenColl() - 1st collection NOT an App collection"));

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    goto end_err;
  }

  //                                                               --------- PUT COLLECTION IN APP USAGE ARAY ---------
  if (p_coll_new->Type == USBH_HID_COLL_APP) {
    if (p_parse_info->CollStkPtr != DEF_NULL) {
      LOG_ERR(("HID_OpenColl() - Collection nested"));

      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      goto end_err;
    }

    //                                                             Max app collections exceeded.
    p_app_coll = (USBH_HID_APP_COLL *)Mem_DynPoolBlkGet(&USBH_HID_ParserPtr->AppCollItemsPool,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("HID_OpenColl() - Max App collection exceeded"));
      goto end_err;
    }

    p_app_coll->Usage = p_coll_new->Usage;
    p_app_coll->Type = p_coll_new->Type;
    p_app_coll->NextPtr = p_parse_info->AppCollHeadPtr;         // Current Application Collection
    p_parse_info->AppCollHeadPtr = p_app_coll;
  }

  //                                                               ------------- PUT COLLECTION IN STACK --------------
  p_coll_new->NextPtr = p_parse_info->CollStkPtr;
  p_parse_info->CollStkPtr = p_coll_new;

  return;

end_err:
  {
    RTOS_ERR local_err;

    Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->CollItemsPool, (void *)p_coll_new, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_CloseColl()
 *
 * @brief    Remove top collection from collection stack.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_CloseColl(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err)
{
  USBH_HID_COLL *p_coll_free;

  if (p_parse_info->CollStkPtr == DEF_NULL) {                   // Collection stack should not be empty.
    LOG_ERR(("HID_CloseColl() - Collection mismatch"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  }

  p_coll_free = p_parse_info->CollStkPtr;                       // Remove top element from collection stack.
  p_parse_info->CollStkPtr = p_coll_free->NextPtr;

  Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->CollItemsPool, (void *)p_coll_free, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_AddReport()
 *
 * @brief    Add new report to report list in application collection.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_AddReport(USBH_HID_PARSE_INFO *p_parse_info,
                               RTOS_ERR            *p_err)
{
  USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item;
  USBH_HID_APP_COLL        *p_app_coll;

  p_app_coll = p_parse_info->AppCollHeadPtr;                    // Current Application Collection
  if (p_app_coll == DEF_NULL) {                                 // Rtn error if no app collections exist.
    LOG_ERR(("HID_AddReport() - NO App collection exists"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  }

  //                                                               Get new Report Format
  p_report_fmt_item = (USBH_HID_REPORT_FMT_ITEM *)Mem_DynPoolBlkGet(&USBH_HID_ParserPtr->ReportFmtItemPool,
                                                                    p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_report_fmt_item->NextPtr = DEF_NULL;

  //                                                               ----------------- INITIALIZE REPORT ----------------
  USBH_HID_InitReport(p_parse_info, &p_report_fmt_item->ReportFmt);

  //                                                               ------------------ VALIDATE REPORT -----------------
  USBH_HID_ValidateReport(&p_report_fmt_item->ReportFmt, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_lib;

    LOG_ERR(("HID_AddReport() - Device report validation failed ", RTOS_ERR_LOG_ARG_GET(*p_err)));

    Mem_DynPoolBlkFree(&USBH_HID_ParserPtr->ReportFmtItemPool, (void *)p_report_fmt_item, &err_lib);
    PP_UNUSED_PARAM(err_lib);

    return;
  }

  //                                                               Add report fmt item to list in app coll.
  p_report_fmt_item->NextPtr = p_app_coll->ReportFmtItemHeadPtr;
  p_app_coll->ReportFmtItemHeadPtr = p_report_fmt_item;

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_ValidateReport()
 *
 * @brief    Validate report by checking report values.
 *
 * @param    p_report_fmt    Pointer to report.
 *
 * @param    p_err           Pointer to variable that will receive return error code from this
 *                           function.
 *******************************************************************************************************/
static void USBH_HID_ValidateReport(USBH_HID_REPORT_FMT *p_report_fmt,
                                    RTOS_ERR            *p_err)
{
  CPU_BOOLEAN x_def;
  CPU_BOOLEAN y_def;

  //                                                               --------------- CHK MANDATORY FIELDS ---------------
  if ((p_report_fmt->UsagePage == USBH_HID_USAGE_PAGE_UNDEFINED)
      || (p_report_fmt->LogMax == USBH_HID_LOG_MIN_UNDEFINED)
      || (p_report_fmt->LogMin == USBH_HID_LOG_MAX_UNDEFINED)
      || (p_report_fmt->ReportCnt == USBH_HID_REPORT_CNT_UNDEFINED)
      || (p_report_fmt->ReportSize == USBH_HID_REPORT_SIZE_UNDEFINED)) {
    LOG_ERR(("HID_ValidateReport() - Mandatory field NOT defined"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  }

  //                                                               -------------- CHK LOGICAL MIN & MAX ---------------
  if (p_report_fmt->LogMin >= p_report_fmt->LogMax) {
    LOG_ERR(("HID_ValidateReport() - Logical Min > Logical Max"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  }
  //                                                               Chk presence/value of physical min & max.
  x_def = (p_report_fmt->PhyMax == USBH_HID_PHY_MAX_UNDEFINED) ? DEF_FALSE : DEF_TRUE;
  y_def = (p_report_fmt->PhyMin == USBH_HID_PHY_MIN_UNDEFINED) ? DEF_FALSE : DEF_TRUE;

  if (((x_def == DEF_TRUE) && (y_def == DEF_FALSE))
      || ((y_def == DEF_TRUE) && (x_def == DEF_FALSE))) {
    LOG_ERR(("HID_ValidateReport() - Only one of physical min & max defined"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  } else if ((x_def == DEF_FALSE) && (y_def == DEF_FALSE)) {
    p_report_fmt->PhyMin = p_report_fmt->LogMin;
    p_report_fmt->PhyMax = p_report_fmt->LogMax;
  } else if (p_report_fmt->PhyMax < p_report_fmt->PhyMin) {
    LOG_ERR(("HID_ValidateReport() - Physical Min > Physical Max"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
    return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_InitReport()
 *
 * @brief    Initialize report.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_report_fmt    Pointer to report.
 *******************************************************************************************************/
static void USBH_HID_InitReport(USBH_HID_PARSE_INFO *p_parse_info,
                                USBH_HID_REPORT_FMT *p_report_fmt)
{
  USBH_HID_COLL *p_coll_tmp;
  CPU_INT08U    usage_ix;

  //                                                               Copy usages.
  for (usage_ix = 0u; usage_ix < p_parse_info->Local.NbrUsage; usage_ix++) {
    p_report_fmt->UsageTbl[usage_ix] = ((CPU_INT32U)p_parse_info->Global.UsagePage << 16u)
                                       | (CPU_INT32U)p_parse_info->Local.UsageTbl[usage_ix];
  }

  p_report_fmt->UsagePage = p_parse_info->Global.UsagePage;
  p_report_fmt->NbrUsage = p_parse_info->Local.NbrUsage;
  p_report_fmt->ReportType = p_parse_info->Tag;
  p_report_fmt->Flag = USBH_HID_UNSIGNED_DATA(p_parse_info);
  p_report_fmt->LogMin = p_parse_info->Global.LogMin;
  p_report_fmt->LogMax = p_parse_info->Global.LogMax;
  p_report_fmt->PhyMin = p_parse_info->Global.PhyMin;
  p_report_fmt->PhyMax = p_parse_info->Global.PhyMax;
  p_report_fmt->ReportSize = p_parse_info->Global.ReportSize;
  p_report_fmt->ReportCnt = p_parse_info->Global.ReportCnt;
  p_report_fmt->UnitExp = p_parse_info->Global.UnitExp;
  p_report_fmt->Unit = p_parse_info->Global.Unit;
  p_report_fmt->ReportID = p_parse_info->Global.ReportID;
  p_report_fmt->UsageMin = p_parse_info->Local.UsageMin;
  p_report_fmt->UsageMax = p_parse_info->Local.UsageMax;
  p_report_fmt->PhyUsage = USBH_HID_USAGE_PAGE_UNDEFINED;       // Init collection usage to undefined values.
  p_report_fmt->AppUsage = USBH_HID_USAGE_PAGE_UNDEFINED;
  p_report_fmt->LogUsage = USBH_HID_USAGE_PAGE_UNDEFINED;

  p_coll_tmp = p_parse_info->CollStkPtr;                        // Find values for collection usage.
  while (p_coll_tmp != DEF_NULL) {
    switch (p_coll_tmp->Type) {
      case USBH_HID_COLL_PHYSICAL:
        p_report_fmt->PhyUsage = p_coll_tmp->Usage;
        break;

      case USBH_HID_COLL_APP:
        p_report_fmt->AppUsage = p_coll_tmp->Usage;
        break;

      case USBH_HID_COLL_LOGICAL:
        p_report_fmt->LogUsage = p_coll_tmp->Usage;
        break;

      default:
        break;
    }
    p_coll_tmp = p_coll_tmp->NextPtr;
  }
}

/****************************************************************************************************//**
 *                                       USBH_HID_MaxReportLenSet()
 *
 * @brief    Find INPUT report that has the greates size and set the max len and the flag indicating
 *           if multiple reports are present in the HID fnct struct.
 *
 * @param    p_hid_fnct      Pointer to HID function.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *******************************************************************************************************/
static void USBH_HID_MaxReportLenSet(USBH_HID_FNCT       *p_hid_fnct,
                                     USBH_HID_PARSE_INFO *p_parse_info)
{
  USBH_HID_APP_COLL *p_app_coll = p_parse_info->AppCollHeadPtr;
  CPU_INT32U        max_len = 0u;
  CPU_INT32U        len;
  CPU_INT08U        cur_report_id;

  p_hid_fnct->HasMultipleInputReports = DEF_NO;

  while (p_app_coll != DEF_NULL) {                              // For each app collection.
    USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item = p_app_coll->ReportFmtItemHeadPtr;

    while (p_report_fmt_item != DEF_NULL) {                     // For each report fmt in this collection.
      USBH_HID_APP_COLL *p_app_coll_iter = p_app_coll;

      cur_report_id = p_report_fmt_item->ReportFmt.ReportID;

      if (p_report_fmt_item->ReportFmt.ReportType == USBH_HID_MAIN_ITEM_TAG_IN) {
        if (cur_report_id != 0u) {
          p_hid_fnct->HasMultipleInputReports = DEF_YES;
        }

        len = 0u;

        while (p_app_coll_iter != DEF_NULL) {                   // Iterate through collections not yet browsed.
          USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item_iter = p_app_coll_iter->ReportFmtItemHeadPtr;

          //                                                       For each report fmt in this collection.
          while (p_report_fmt_item_iter != DEF_NULL) {
            USBH_HID_REPORT_FMT *p_report_fmt_iter = &p_report_fmt_item_iter->ReportFmt;

            //                                                     If report ID/type match, increment size.
            if ((p_report_fmt_iter->ReportID == cur_report_id)
                && (p_report_fmt_iter->ReportType == USBH_HID_MAIN_ITEM_TAG_IN)) {
              len += (p_report_fmt_iter->ReportCnt * p_report_fmt_iter->ReportSize);
            }

            p_report_fmt_item_iter = p_report_fmt_item_iter->NextPtr;
          }

          p_app_coll_iter = p_app_coll_iter->NextPtr;
        }

        len = (len + 7u) / 8u;

        if (len > max_len) {
          max_len = len;
        }
      }

      p_report_fmt_item = p_report_fmt_item->NextPtr;
    }

    p_app_coll = p_app_coll->NextPtr;
  }

  if (p_hid_fnct->HasMultipleInputReports) {                    // If more than one report, report ID will be present.
    max_len++;
  }
  p_hid_fnct->MaxReportLen = max_len;

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HID_SetData()
 *
 * @brief    Set data in parser.
 *
 * @param    p_parse_info    Pointer to parse info structure.
 *
 * @param    p_buf           Pointer to data buffer.
 *******************************************************************************************************/
static void USBH_HID_SetData(USBH_HID_PARSE_INFO *p_parse_info,
                             void                *p_buf)
{
  switch (p_parse_info->Size) {
    case 1:
      p_parse_info->Data.U08 = MEM_VAL_GET_INT08U_LITTLE(p_buf);
      p_parse_info->Data.S08 = (CPU_INT08S)MEM_VAL_GET_INT08U_LITTLE(p_buf);
      break;

    case 2:
      p_parse_info->Data.U16 = MEM_VAL_GET_INT16U_LITTLE(p_buf);
      p_parse_info->Data.S16 = (CPU_INT16S)MEM_VAL_GET_INT16U_LITTLE(p_buf);
      break;

    case 4:
      p_parse_info->Data.U32 = MEM_VAL_GET_INT32U_LITTLE(p_buf);
      p_parse_info->Data.S32 = (CPU_INT32S)MEM_VAL_GET_INT32U_LITTLE(p_buf);
      break;

    default:
      break;
  }
}

/****************************************************************************************************//**
 *                               USBH_HID_ParserReportFmtItemAllocCallback()
 *
 * @brief    Callback called when USBH_HID_REPORT_FMT_ITEM block is allocated. Allocates usage tbl.
 *
 * @param    p_pool  Pointer to MEM_DYN_POOL structure from which block was allocated.
 *
 * @param    p_seg   Pointer to memory segment from which block was allocated.
 *
 * @param    p_blk   Pointer to allocated block.
 *
 * @param    p_arg   Pointer to argument.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HID_ParserReportFmtItemAllocCallback(MEM_DYN_POOL *p_pool,
                                                             MEM_SEG      *p_seg,
                                                             void         *p_blk,
                                                             void         *p_arg)
{
  USBH_HID_REPORT_FMT_ITEM *p_report_fmt_item;
  RTOS_ERR                 err_lib;

  (void)&p_pool;
  (void)&p_arg;

  p_report_fmt_item = (USBH_HID_REPORT_FMT_ITEM *)p_blk;

  p_report_fmt_item->ReportFmt.UsageTbl = (CPU_INT32U *)Mem_SegAlloc("USBH - HID Parser Report Usage Tbl",
                                                                     p_seg,
                                                                     (sizeof(CPU_INT32U) * USBH_HID_InitCfg.UsageMaxNbrPerItem),
                                                                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_HID_AVAIL))
