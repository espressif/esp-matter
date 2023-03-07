/***************************************************************************//**
 * @file
 * @brief USB Device Core Operations
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

#if (defined(RTOS_MODULE_USB_DEV_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    USBD_CORE_MODULE
#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_types.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/source/device/core/usbd_core_priv.h>
#include  <usb/source/usb_ctrlr_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (USBD)
#define  LOG_USBD_BUS_CH                   (USBD, BUS)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 *                                       OBJECTS TOTAL NUMBER DEFINES
 *******************************************************************************************************/

#define  USBD_DEV_NBR_TOT                 (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_CONFIG_NBR_TOT              (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_IF_NBR_TOT                  (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_IF_ALT_NBR_TOT              (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_IF_GRP_NBR_TOT              (DEF_INT_08U_MAX_VAL - 1u)
#define  USBD_EP_NBR_TOT                  (DEF_INT_08U_MAX_VAL - 1u)

/********************************************************************************************************
 *                                           PROTOCOL DEFINES
 *
 * Note(s) : (1) The descriptor buffer is used to send the device, configuration and string descriptors.
 *
 *               (a) The size of the descriptor buffer is set to 64 which is the maximum packet size
 *                   allowed by the USB specification for FS and HS devices.
 *
 *           (2) USB spec 2.0 (section 9.6.3), table 9-10 specify the bitmap for the configuration
 *               attributes.
 *
 *                   D7    Reserved (set to one)
 *                   D6    Self-powered
 *                   D5    Remote Wakeup
 *                   D4..0 Reserved (reset to zero)
 *******************************************************************************************************/

#define  USBD_DESC_BUF_LEN                                64u   // See Note #1a.
#define  USBD_EP_CTRL_ALLOC                       (DEF_BIT_00 | DEF_BIT_01)

#define  USBD_CONFIG_DESC_SELF_POWERED             DEF_BIT_06   // See Note #2.
#define  USBD_CONFIG_DESC_REMOTE_WAKEUP            DEF_BIT_05
#define  USBD_CONFIG_DESC_RSVD_SET                 DEF_BIT_07

//                                                                 -------------- MICROSOFT DESC DEFINES --------------
#define  USBD_MS_OS_DESC_COMPAT_ID_HDR_VER_1_0        0x0010u
#define  USBD_MS_OS_DESC_EXT_PROPERTIES_HDR_VER_1_0   0x000Au
#define  USBD_MS_OS_DESC_VER_1_0                      0x0100u

#define  USBD_STR_MS_OS_LEN                               18u   // Length of MS OS string.
#define  USBD_STR_MS_OS_IX                              0xEEu   // Index  of MS OS string.

#define  USBD_MS_OS_DESC_COMPAT_ID_HDR_LEN                16u
#define  USBD_MS_OS_DESC_COMPAT_ID_SECTION_LEN            24u

#define  USBD_MS_OS_DESC_EXT_PROPERTIES_HDR_LEN           10u
#define  USBD_MS_OS_DESC_EXT_PROPERTIES_SECTION_HDR_LEN    8u

#define  USBD_MS_OS_FEATURE_COMPAT_ID                 0x0004u
#define  USBD_MS_OS_FEATURE_EXT_PROPERTIES            0x0005u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 *
 * Note(s) : (1) For more information, see "Extended Compat ID OS Feature Descriptor Specification",
 *               Appendix A, available at http://msdn.microsoft.com/en-us/windows/hardware/gg463179.aspx.
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_INIT_CFG USBD_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,
};
#endif

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static const CPU_CHAR USBD_StrMS_Signature[] = "MSFT100";       // Signature used in MS OS string desc.

static const CPU_CHAR USBD_MS_CompatID[][8u] = {
  { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
  { 'R', 'N', 'D', 'I', 'S', 0u, 0u, 0u },
  { 'P', 'T', 'P', 0u, 0u, 0u, 0u, 0u },
  { 'M', 'T', 'P', 0u, 0u, 0u, 0u, 0u },
  { 'X', 'U', 'S', 'B', '2', '0', 0u, 0u },
  { 'B', 'L', 'U', 'T', 'U', 'T', 'H', 0u },
  { 'W', 'I', 'N', 'U', 'S', 'B', 0u, 0u },
};

static const CPU_CHAR USBD_MS_SubCompatID[][8u] = {
  { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
  { '1', '1', 0u, 0u, 0u, 0u, 0u, 0u },
  { '1', '2', 0u, 0u, 0u, 0u, 0u, 0u },
  { 'E', 'D', 'R', 0u, 0u, 0u, 0u, 0u },
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Device task default configurations.
const RTOS_TASK_CFG USBD_DevTaskCfgDflt = {
  .Prio = 10u,
  .StkSizeElements = 1024u,
  .StkPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBD *USBD_Ptr = DEF_NULL;

#if (USBD_CFG_DBG_STATS_EN == DEF_ENABLED)
USBD_DBG_STATS_DEV USBD_DbgStatsDevTbl[4u];
USBD_DBG_STATS_EP  USBD_DbgStatsEP_Tbl[4u][32u];
#endif

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_INIT_CFG USBD_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,
};
#else
extern USBD_INIT_CFG USBD_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 ------------- STANDARD REQUEST HANDLERS ------------
static void USBD_StdReqHandler(USBD_DEV *p_dev);

static CPU_BOOLEAN USBD_StdReqDev(USBD_DEV   *p_dev,
                                  CPU_INT08U request);

static CPU_BOOLEAN USBD_StdReqIF(USBD_DEV   *p_dev,
                                 CPU_INT08U request);

static CPU_BOOLEAN USBD_StdReqEP(const USBD_DEV *p_dev,
                                 CPU_INT08U     request);

static CPU_BOOLEAN USBD_StdReqClass(const USBD_DEV *p_dev);

static CPU_BOOLEAN USBD_StdReqVendor(const USBD_DEV *p_dev);

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_BOOLEAN USBD_StdReqDevMS(const USBD_DEV *p_dev);

static CPU_BOOLEAN USBD_StdReqIF_MS(const USBD_DEV *p_dev);

static CPU_BOOLEAN USBD_StdReqMS_ExtPropWr(const USBD_DEV *p_dev,
                                           USBD_CONFIG    *p_config,
                                           CPU_INT08U     if_nbr,
                                           CPU_INT16U     len);
#endif

static CPU_BOOLEAN USBD_StdReqDescGet(USBD_DEV *p_dev);

#if 0
static CPU_BOOLEAN USBD_StdReqDescSet(USBD_DEV *p_dev);
#endif

static void USBD_ConfigClose(USBD_DEV *p_dev);

static void USBD_ConfigOpen(USBD_DEV   *p_dev,
                            CPU_INT08U config_nbr,
                            RTOS_ERR   *p_err);

static void USBD_DevDescSend(USBD_DEV    *p_dev,
                             CPU_BOOLEAN other,
                             CPU_INT16U  req_len,
                             RTOS_ERR    *p_err);

static void USBD_ConfigDescSend(USBD_DEV    *p_dev,
                                CPU_INT08U  config_nbr,
                                CPU_BOOLEAN other,
                                CPU_INT16U  req_len,
                                RTOS_ERR    *p_err);

#if (USBD_CFG_STR_EN == DEF_ENABLED)
static void USBD_StrDescSend(USBD_DEV   *p_dev,
                             CPU_INT08U str_ix,
                             CPU_INT16U req_len,
                             RTOS_ERR   *p_err);

static void USBD_StrDescAdd(USBD_DEV       *p_dev,
                            const CPU_CHAR *p_str,
                            RTOS_ERR       *p_err);

static CPU_INT08U USBD_StrDescIxGet(const USBD_DEV *p_dev,
                                    const CPU_CHAR *p_str);

static const CPU_CHAR *USBD_StrDescGet(const USBD_DEV *p_dev,
                                       CPU_INT08U     str_nbr);
#endif

static void USBD_DescWrStart(USBD_DEV   *p_dev,
                             CPU_INT16U req_len);

static void USBD_DescWrStop(USBD_DEV *p_dev,
                            RTOS_ERR *p_err);

static void USBD_DescWrReq08(USBD_DEV   *p_dev,
                             CPU_INT08U val);

static void USBD_DescWrReq16(USBD_DEV   *p_dev,
                             CPU_INT16U val);

static void USBD_DescWrReq(USBD_DEV         *p_dev,
                           const CPU_INT08U *p_buf,
                           CPU_INT16U       len);

//                                                                 --------------- USB OBJECT FUNCTIONS ---------------
static USBD_DEV *USBD_DevRefGet(CPU_INT08U dev_nbr);

static USBD_CONFIG *USBD_ConfigRefGet(const USBD_DEV *p_dev,
                                      CPU_INT08U     config_nbr);

static USBD_IF *USBD_IF_RefGet(const USBD_CONFIG *p_config,
                               CPU_INT08U        if_nbr);

static USBD_IF_ALT *USBD_IF_AltRefGet(const USBD_IF *p_if,
                                      CPU_INT08U    if_alt_nbr);

static void USBD_IF_AltOpen(USBD_DEV          *p_dev,
                            CPU_INT08U        if_nbr,
                            const USBD_IF_ALT *p_if_alt,
                            RTOS_ERR          *p_err);

static void USBD_IF_AltClose(USBD_DEV          *p_dev,
                             const USBD_IF_ALT *p_if_alt);

static USBD_IF_GRP *USBD_IF_GrpRefGet(const USBD_CONFIG *p_config,
                                      CPU_INT08U        if_grp_nbr);

static void USBD_EventSet(USBD_DRV        *p_drv,
                          USBD_EVENT_CODE event);

static void USBD_EventProcess(USBD_DEV        *p_dev,
                              USBD_EVENT_CODE event);

static CPU_INT08U USBD_EP_Add(CPU_INT08U  dev_nbr,
                              CPU_INT08U  config_nbr,
                              CPU_INT08U  if_nbr,
                              CPU_INT08U  if_alt_nbr,
                              CPU_INT08U  attrib,
                              CPU_BOOLEAN dir_in,
                              CPU_INT16U  max_pkt_len,
                              CPU_INT08U  interval,
                              RTOS_ERR    *p_err);

static CPU_BOOLEAN USBD_EP_Alloc(USBD_DEV     *p_dev,
                                 USBD_DEV_SPD spd,
                                 CPU_INT08U   type,
                                 CPU_BOOLEAN  dir_in,
                                 CPU_INT16U   max_pkt_len,
                                 CPU_INT08U   if_alt_nbr,
                                 USBD_EP_INFO *p_ep,
                                 CPU_INT32U   *p_alloc_bit_map);

static void USBD_CoreEventFree(USBD_CORE_EVENT *p_core_event);

static USBD_CORE_EVENT *USBD_CoreEventGet(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBD_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB device core is initialized via the
 *               USBD_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                           USBD_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg       Pointer to memory segment to use when allocating control data.
 *                           Can be the same segment used for p_mem_seg_buf.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @param    p_mem_seg_buf   Pointer to memory segment to use when allocating data buffers.
 *                           Can be the same segment used for p_mem_seg.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB device core is initialized via the
 *               USBD_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                          MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_InitCfg.MemSegPtr = p_mem_seg;
  USBD_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                               USBD_Init()
 *
 * @brief    Initializes the USB device stack.
 *
 * @param    p_qty_cfg   Pointer to the USBD quantity configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_SEG_OVF
 *
 * @note     (1) USBD_Init() MUST be called ... :
 *               - (a) ONLY ONCE from a product's application;
 *               - (b) With the following conditions:
 *                   - (1) AFTER  the product's OS has been initialized.
 *                   - (2) BEFORE the product's application calls any USB device stack function(s).
 *******************************************************************************************************/
void USBD_Init(USBD_QTY_CFG *p_qty_cfg,
               RTOS_ERR     *p_err)
{
  USBD         *p_usbd;
  USBD_DEV     *p_dev;
  USBD_CONFIG  *p_config;
  USBD_IF      *p_if;
  USBD_IF_ALT  *p_if_alt;
  USBD_IF_GRP  *p_if_grp;
  USBD_EP_INFO *p_ep;
  CPU_INT16U   tbl_ix;
  CPU_INT16U   core_event_qty;
  CPU_INT16U   async_event_qty;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ConfigQty >= 1u)
                           && (p_qty_cfg->DevQty >= 1u)
                           && (p_qty_cfg->DevQty < (DEF_INT_08U_MAX_VAL - 1u))
                           && (p_qty_cfg->EP_DescQty >= 1u)
                           && (p_qty_cfg->IF_Qty >= 1u)
                           && (p_qty_cfg->IF_AltQty >= p_qty_cfg->IF_Qty)
                           && (p_qty_cfg->StrQty <= 100u)
                           && (p_qty_cfg->EP_OpenQty >= 2u)), *p_err, RTOS_ERR_INVALID_CFG,; );

#if (USBD_CFG_URB_EXTRA_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg->URB_ExtraQty != 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  core_event_qty = (USBD_CORE_EVENT_BUS_NBR * p_qty_cfg->DevQty);
  async_event_qty = (p_qty_cfg->EP_OpenQty * p_qty_cfg->DevQty) + p_qty_cfg->URB_ExtraQty;

  USBD_OS_Init(p_qty_cfg,                                       // Initialize OS Interface.
               USBD_InitCfg.MemSegPtr,
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd = (USBD *)Mem_SegAlloc("USBD - Root struct",
                                USBD_InitCfg.MemSegPtr,
                                sizeof(USBD),
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->DevNbrNext = p_qty_cfg->DevQty;
  p_usbd->ConfigNbrNext = p_qty_cfg->ConfigQty;
  p_usbd->IF_NbrNext = p_qty_cfg->IF_Qty;
  p_usbd->IF_AltNbrNext = p_qty_cfg->IF_AltQty;
  p_usbd->IF_GrpNbrNext = p_qty_cfg->IF_GrpQty;
  p_usbd->EP_InfoNbrNext = p_qty_cfg->EP_DescQty;
  p_usbd->StdReqTimeoutMs = 5000u;

  p_usbd->DevTbl = (USBD_DEV *)Mem_SegAlloc("USBD - Device table",
                                            USBD_InitCfg.MemSegPtr,
                                            sizeof(USBD_DEV) * p_qty_cfg->DevQty,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->ConfigTbl = (USBD_CONFIG *)Mem_SegAlloc("USBD - Configuration table",
                                                  USBD_InitCfg.MemSegPtr,
                                                  sizeof(USBD_CONFIG) * p_qty_cfg->ConfigQty,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->IF_Tbl = (USBD_IF *)Mem_SegAlloc("USBD - Interface table",
                                           USBD_InitCfg.MemSegPtr,
                                           sizeof(USBD_IF) * p_qty_cfg->IF_Qty,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->IF_AltTbl = (USBD_IF_ALT *)Mem_SegAlloc("USBD - Alternate Interface table",
                                                  USBD_InitCfg.MemSegPtr,
                                                  sizeof(USBD_IF_ALT) * p_qty_cfg->IF_AltQty,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_qty_cfg->IF_GrpQty > 0u) {
    p_usbd->IF_GrpTbl = (USBD_IF_GRP *)Mem_SegAlloc("USBD - Interface group table",
                                                    USBD_InitCfg.MemSegPtr,
                                                    sizeof(USBD_IF_GRP) * p_qty_cfg->IF_GrpQty,
                                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    p_usbd->IF_GrpTbl = DEF_NULL;
  }

  p_usbd->EP_InfoTbl = (USBD_EP_INFO *)Mem_SegAlloc("USBD - Endpoint info table",
                                                    USBD_InitCfg.MemSegPtr,
                                                    sizeof(USBD_EP_INFO) * p_qty_cfg->EP_DescQty,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->CoreEventPoolData = (USBD_CORE_EVENT *)Mem_SegAlloc("USBD - Core event data table",
                                                              USBD_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CORE_EVENT) * (core_event_qty + async_event_qty),
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbd->CoreEventPoolPtrs = (USBD_CORE_EVENT **)Mem_SegAlloc("USBD - Core event pointers table",
                                                               USBD_InitCfg.MemSegPtr,
                                                               sizeof(USBD_CORE_EVENT *) * (core_event_qty + async_event_qty),
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBD_Ptr = p_usbd;
  CORE_EXIT_ATOMIC();

  //                                                               ------------ DEVICE TABLE INITIALIZATION -----------
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->DevQty; tbl_ix++) {
    p_dev = &USBD_Ptr->DevTbl[tbl_ix];
    p_dev->Addr = 0u;                                           // Dflt dev addr.
    p_dev->State = USBD_DEV_STATE_NONE;
    p_dev->StatePrev = USBD_DEV_STATE_NONE;
    p_dev->ConnStatus = DEF_FALSE;
    p_dev->Spd = USBD_DEV_SPD_INVALID;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Init HS & FS cfg list:
                                                                // array implementation.
    p_dev->ConfigFS_SpdTblPtrs = (USBD_CONFIG **)Mem_SegAlloc("USBD - FS config ptr table",
                                                              USBD_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CONFIG *) * p_qty_cfg->ConfigQty,
                                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_dev->ConfigFS_SpdTblPtrs,
            sizeof(USBD_CONFIG *) * p_qty_cfg->ConfigQty);

#if (USBD_CFG_HS_EN == DEF_ENABLED)
    p_dev->ConfigHS_SpdTblPtrs = (USBD_CONFIG **)Mem_SegAlloc("USBD - HS config ptr table",
                                                              USBD_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CONFIG *) * p_qty_cfg->ConfigQty,
                                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_dev->ConfigHS_SpdTblPtrs,
            sizeof(USBD_CONFIG *) * p_qty_cfg->ConfigQty);
#endif

#else
    p_dev->ConfigFS_HeadPtr = DEF_NULL;                         // linked-list implementation.
    p_dev->ConfigFS_TailPtr = DEF_NULL;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
    p_dev->ConfigHS_HeadPtr = DEF_NULL;
    p_dev->ConfigHS_TailPtr = DEF_NULL;
#endif
#endif

    p_dev->ConfigCurPtr = DEF_NULL;
    p_dev->ConfigCurNbr = USBD_CONFIG_NBR_NONE;
    p_dev->ConfigFS_TotalNbr = 0u;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
    p_dev->ConfigHS_TotalNbr = 0u;
#endif

    //                                                             Alloc desc buf from heap.
    p_dev->DescBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - Desc buffer",
                                                     USBD_InitCfg.MemSegBufPtr,
                                                     USBD_DESC_BUF_LEN,
                                                     USBD_InitCfg.BufAlignOctets,
                                                     DEF_NULL,
                                                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_dev->DescBufPtr,
            USBD_DESC_BUF_LEN);

    //                                                             Alloc ctrl status buf from heap.
    p_dev->CtrlStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - Status buffer",
                                                           USBD_InitCfg.MemSegBufPtr,
                                                           2u,
                                                           USBD_InitCfg.BufAlignOctets,
                                                           DEF_NULL,
                                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_dev->ActualBufPtr = p_dev->DescBufPtr;
    p_dev->DescBufIx = 0u;
    p_dev->DescBufReqLen = 0u;
    p_dev->DescBufMaxLen = USBD_DESC_BUF_LEN;
    p_dev->DescBufErrPtr = DEF_NULL;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
    if (p_qty_cfg->StrQty > 0u) {
      p_dev->StrDesc_Tbl = (CPU_CHAR **)Mem_SegAlloc("USBD - String table",
                                                     USBD_InitCfg.MemSegPtr,
                                                     sizeof(CPU_CHAR *) * p_qty_cfg->StrQty,
                                                     p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      Mem_Clr((void *)p_dev->StrDesc_Tbl,
              p_qty_cfg->StrQty * sizeof(CPU_CHAR *));

      p_dev->StrMaxIx = 0u;
    } else {
      p_dev->StrDesc_Tbl = DEF_NULL;
    }
#endif
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
    p_dev->StrMS_VendorCode = 0u;
#endif
    p_dev->BusFnctsPtr = DEF_NULL;

    Mem_Clr((void *)&p_dev->SetupReq,
            sizeof(USBD_SETUP_REQ));

    Mem_Clr((void *)&p_dev->SetupReqNext,
            sizeof(USBD_SETUP_REQ));

    p_dev->EP_CtrlMaxPktSize = 0u;
    p_dev->EP_MaxPhyNbr = 0u;

    Mem_Set((void *)p_dev->EP_IF_Tbl,
            USBD_IF_NBR_NONE,
            USBD_EP_MAX_NBR);

    p_dev->SelfPwr = DEF_NO;
    p_dev->RemoteWakeup = DEF_DISABLED;
    p_dev->Drv.DevNbr = USBD_DEV_NBR_NONE;
    p_dev->Drv.API_Ptr = DEF_NULL;
    p_dev->Drv.CfgPtr = DEF_NULL;
    p_dev->Drv.DataPtr = DEF_NULL;
    p_dev->Drv.BSP_API_Ptr = DEF_NULL;

    USBD_DBG_STATS_DEV_RESET(tbl_ix);
    USBD_DBG_STATS_DEV_SET_DEV_NBR(tbl_ix);
  }
  //                                                               -------- CONFIGURATION TABLE INITIALIZATION --------
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->ConfigQty; tbl_ix++) {
    p_config = &USBD_Ptr->ConfigTbl[tbl_ix];
    p_config->Attrib = DEF_BIT_NONE;
    p_config->MaxPwr = 0u;
    p_config->DescLen = 0u;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
    p_config->NamePtr = DEF_NULL;
#endif

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Init IF list:
                                                                // array implementation.
    p_config->IF_TblPtrs = (USBD_IF **)Mem_SegAlloc("USBD - Config IF table ptrs",
                                                    USBD_InitCfg.MemSegPtr,
                                                    sizeof(USBD_IF *) * p_qty_cfg->IF_Qty,
                                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_config->IF_TblPtrs,
            sizeof(USBD_IF *) * p_qty_cfg->IF_Qty);

    if (p_qty_cfg->IF_GrpQty > 0u) {
      p_config->IF_GrpTblPtrs = (USBD_IF_GRP **)Mem_SegAlloc("USBD - Config IF group table ptrs",
                                                             USBD_InitCfg.MemSegPtr,
                                                             sizeof(USBD_IF_GRP *) * p_qty_cfg->IF_GrpQty,
                                                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      Mem_Clr((void *)p_config->IF_GrpTblPtrs,
              sizeof(USBD_IF_GRP *) * p_qty_cfg->IF_GrpQty);
    } else {
      p_config->IF_GrpTblPtrs = DEF_NULL;
    }

#else
    p_config->IF_HeadPtr = DEF_NULL;                            // linked-list implementation.
    p_config->IF_TailPtr = DEF_NULL;
    p_config->IF_GrpHeadPtr = DEF_NULL;
    p_config->IF_GrpTailPtr = DEF_NULL;
    p_config->NextPtr = DEF_NULL;
#endif
    p_config->IF_NbrTotal = 0u;
    p_config->IF_GrpNbrTotal = 0u;
    p_config->EP_AllocMap = DEF_BIT_NONE;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
    p_config->ConfigOtherSpd = USBD_CONFIG_NBR_NONE;
#endif
  }
  //                                                               ---------- INTERFACE TABLE INITIALIZATION ----------
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->IF_Qty; tbl_ix++) {
    p_if = &USBD_Ptr->IF_Tbl[tbl_ix];
    p_if->ClassCode = USBD_CLASS_CODE_USE_IF_DESC;
    p_if->ClassSubCode = USBD_SUBCLASS_CODE_USE_IF_DESC;
    p_if->ClassProtocolCode = USBD_PROTOCOL_CODE_USE_IF_DESC;
    p_if->ClassDrvPtr = DEF_NULL;
    p_if->ClassArgPtr = DEF_NULL;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
    p_if->AltTblPtrs = (USBD_IF_ALT **)Mem_SegAlloc("USBD - IF Alternate table ptrs",
                                                    USBD_InitCfg.MemSegPtr,
                                                    sizeof(USBD_IF_ALT *) * p_qty_cfg->IF_AltQty,
                                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr((void *)p_if->AltTblPtrs,
            sizeof(USBD_IF_ALT *) * p_qty_cfg->IF_AltQty);
#else
    p_if->AltHeadPtr = DEF_NULL;
    p_if->AltTailPtr = DEF_NULL;
#endif
    p_if->AltCurPtr = DEF_NULL;
    p_if->AltCur = USBD_IF_ALT_NBR_NONE;
    p_if->AltNbrTotal = 0u;
    p_if->GrpNbr = USBD_IF_GRP_NBR_NONE;
    p_if->EP_AllocMap = DEF_BIT_NONE;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
    p_if->NextPtr = DEF_NULL;
#endif
  }
  //                                                               ------ ALTERNATE SETTINGS TABLE INITIALIZATION -----
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->IF_AltQty; tbl_ix++) {
    p_if_alt = &USBD_Ptr->IF_AltTbl[tbl_ix];
    p_if_alt->ClassArgPtr = DEF_NULL;
    p_if_alt->EP_AllocMap = DEF_BIT_NONE;
    p_if_alt->EP_NbrTotal = 0u;
#if (USBD_CFG_STR_EN == DEF_ENABLED)
    p_if_alt->NamePtr = DEF_NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
    p_if_alt->NextPtr = DEF_NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
    Mem_Clr((void *)p_if_alt->EP_TblPtrs,
            USBD_EP_MAX_NBR * sizeof(USBD_EP_INFO *));
    p_if_alt->EP_TblMap = 0u;
#else
    p_if_alt->EP_HeadPtr = DEF_NULL;
    p_if_alt->EP_TailPtr = DEF_NULL;
#endif
  }
  //                                                               ------- INTERFACE GROUP TABLE INITIALIZATION -------
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->IF_GrpQty; tbl_ix++) {
    p_if_grp = &USBD_Ptr->IF_GrpTbl[tbl_ix];
    p_if_grp->ClassCode = USBD_CLASS_CODE_USE_IF_DESC;
    p_if_grp->ClassSubCode = USBD_SUBCLASS_CODE_USE_IF_DESC;
    p_if_grp->ClassProtocolCode = USBD_PROTOCOL_CODE_USE_IF_DESC;
    p_if_grp->IF_Start = USBD_IF_NBR_NONE;
    p_if_grp->IF_Cnt = 0u;
#if (USBD_CFG_STR_EN == DEF_ENABLED)
    p_if_grp->NamePtr = DEF_NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
    p_if_grp->NextPtr = DEF_NULL;
#endif
  }

  //                                                               ----- ENDPOINT INFORMATION TABLE INITIALIZATION ----
  for (tbl_ix = 0u; tbl_ix < p_qty_cfg->EP_DescQty; tbl_ix++) {
    p_ep = &USBD_Ptr->EP_InfoTbl[tbl_ix];
    p_ep->Addr = USBD_EP_NBR_NONE;
    p_ep->Attrib = DEF_BIT_NONE;
    p_ep->Interval = 0u;
    p_ep->SyncAddr = 0u;                                        // Dflt sync addr is zero.
    p_ep->SyncRefresh = 0u;                                     // Dflt feedback rate exponent is zero.
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
    p_ep->NextPtr = DEF_NULL;
#endif
  }

  //                                                               Init pool of core events.
  USBD_Ptr->CoreEventPoolIx = core_event_qty + async_event_qty;
  for (tbl_ix = 0u; tbl_ix < USBD_Ptr->CoreEventPoolIx; tbl_ix++) {
    USBD_Ptr->CoreEventPoolPtrs[tbl_ix] = &USBD_Ptr->CoreEventPoolData[tbl_ix];
  }

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  USBD_Ptr->StrQtyPerDev = p_qty_cfg->StrQty;
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBD_Ptr->EP_OpenQty = p_qty_cfg->EP_OpenQty;
#endif

  USBD_Ptr->DevQty = p_qty_cfg->DevQty;

  USBD_EP_Init(USBD_InitCfg.MemSegPtr, p_qty_cfg, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the USB device standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBD_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                           RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBD_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBD_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBD_DevAdd()
 *
 * @brief    Adds a device to the stack and creates the default control endpoints.
 *
 * @param    name            Name of the USB controller.
 *
 * @param    p_task_cfg      Pointer to the task configuration structure.
 *
 * @param    p_dev_cfg       Pointer to the specific USB device configuration.
 *
 * @param    p_dev_drv_cfg   Pointer to the device driver configuration structure.
 *
 * @param    p_bus_fnct      Pointer to the specific USB device bus events callback functions.
 *                           [Content MUST be persistent]
 *
 * @param    p_err           Pointer to the variable that will receive the returned error code from the following functions :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_DEV_ALLOC
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_EP_NONE_AVAIL
 *
 * @return   The device number,  if no errors are returned.
 *           USBD_DEV_NBR_NONE, if any errors are returned.
 *
 * @note     (1) Certain driver functions are required for the driver to work correctly with the core.
 *               The pointers to these functions are checked in this function to ensure they are
 *               valid and can be used throughout the core.
 *******************************************************************************************************/
CPU_INT08U USBD_DevAdd(const CPU_CHAR         *name,
                       const RTOS_TASK_CFG    *p_task_cfg,
                       const USBD_DEV_CFG     *p_dev_cfg,
                       const USBD_DEV_DRV_CFG *p_dev_drv_cfg,
                       USBD_BUS_FNCTS         *p_bus_fnct,
                       RTOS_ERR               *p_err)
{
  USBD_DEV_CTRLR_HW_INFO *p_dev_hw_info;
  USBD_DEV               *p_dev;
  CPU_INT08U             dev_nbr;
  CPU_INT08U             ep_phy_nbr;
  CPU_INT16U             event_qty;
  CPU_INT32U             ep_alloc_map;
  USBD_EP_INFO           ep_info;
  CPU_BOOLEAN            alloc;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_DEV_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_dev_drv_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_dev_drv_cfg->EP_OpenQty >= 2u), *p_err, RTOS_ERR_INVALID_CFG, USBD_DEV_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);

  p_dev_hw_info = USB_CtrlrDevHwInfoGet(name,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_DEV_NBR_NONE);
  }

  //                                                               Validate mandatory ptrs.
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvInfoPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->BSP_API_Ptr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);

  //                                                               Validate mandatory fnct ptrs. See Note #1.
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->Init != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->Start != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->Stop != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Open != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Close != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_RxStart != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Rx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_RxZLP != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Tx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_TxStart != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_TxZLP != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Abort != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->EP_Stall != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_hw_info->DrvAPI_Ptr->ISR_Handler != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_DEV_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->DevNbrNext == 0u) {                             // Chk if dev nbr is valid.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_DEV_ALLOC);
    return (USBD_DEV_NBR_NONE);
  }

  dev_nbr = USBD_Ptr->DevNbrNext - 1u;
  USBD_Ptr->DevNbrNext--;
  CORE_EXIT_ATOMIC();

  //                                                               ------------ INITIALIZE DEVICE STRUCTURE -----------
  p_dev = &USBD_Ptr->DevTbl[dev_nbr];
  p_dev->Name = (CPU_CHAR *)name;
  p_dev->Nbr = dev_nbr;
  p_dev->Spd = USBD_DEV_SPD_FULL;                               // Set dflt speed (FS).
  p_dev->BusFnctsPtr = p_bus_fnct;
  p_dev->DevCfg = *p_dev_cfg;
  p_dev->Drv.DevNbr = dev_nbr;
  p_dev->Drv.API_Ptr = p_dev_hw_info->DrvAPI_Ptr;
  p_dev->Drv.CfgPtr = p_dev_hw_info->DrvInfoPtr;
  p_dev->Drv.BSP_API_Ptr = p_dev_hw_info->BSP_API_Ptr;
  p_dev->Drv.DrvCfg = *p_dev_drv_cfg;

  ep_alloc_map = DEF_BIT_NONE;

  alloc = USBD_EP_Alloc(p_dev,                                  // Alloc physical EP for ctrl OUT.
                        p_dev_hw_info->DrvInfoPtr->Spd,
                        USBD_EP_TYPE_CTRL,
                        DEF_NO,
                        0u,
                        0u,
                        &ep_info,
                        &ep_alloc_map);
  if (alloc != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_NONE_AVAIL);
    return (USBD_DEV_NBR_NONE);
  }

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_info.Addr);
  ep_phy_nbr++;

  if (p_dev->EP_MaxPhyNbr < ep_phy_nbr) {
    p_dev->EP_MaxPhyNbr = ep_phy_nbr;
  }

  alloc = USBD_EP_Alloc(p_dev,                                  // Alloc physical EP for ctrl IN.
                        p_dev_hw_info->DrvInfoPtr->Spd,
                        USBD_EP_TYPE_CTRL,
                        DEF_YES,
                        0u,
                        0u,
                        &ep_info,
                        &ep_alloc_map);
  if (alloc != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_NONE_AVAIL);
    return (USBD_DEV_NBR_NONE);
  }

  p_dev->EP_CtrlMaxPktSize = ep_info.MaxPktSize;

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_info.Addr);
  ep_phy_nbr++;

  if (p_dev->EP_MaxPhyNbr < ep_phy_nbr) {
    p_dev->EP_MaxPhyNbr = ep_phy_nbr;
  }

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  //                                                               Add device configuration strings:
  USBD_StrDescAdd(p_dev,                                        // Manufacturer string.
                  p_dev_cfg->ManufacturerStrPtr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_DEV_NBR_NONE);
  }

  USBD_StrDescAdd(p_dev,                                        // Product string.
                  p_dev_cfg->ProductStrPtr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_DEV_NBR_NONE);
  }

  USBD_StrDescAdd(p_dev,                                        // Serial number string.
                  p_dev_cfg->SerialNbrStrPtr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_DEV_NBR_NONE);
  }
#endif

  event_qty = USBD_CORE_EVENT_BUS_NBR + p_dev_drv_cfg->EP_OpenQty + p_dev_drv_cfg->URB_ExtraQty;

  if (p_task_cfg == DEF_NULL) {
    p_task_cfg = &USBD_DevTaskCfgDflt;
  }

  USBD_OS_TaskCreate((RTOS_TASK_CFG *)p_task_cfg,
                     dev_nbr,
                     event_qty,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_DEV_NBR_NONE);
  }

  return (dev_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_DevTaskPrioSet()
 *
 * @brief    Sets priority of the given devcie's task.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    prio        Priority of the device's task.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void USBD_DevTaskPrioSet(CPU_INT08U     dev_nbr,
                         RTOS_TASK_PRIO prio,
                         RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBD_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG((dev_nbr < USBD_Ptr->DevQty), RTOS_ERR_INVALID_ARG,; );
#endif

  USBD_OS_DevTaskPrioSet(dev_nbr, prio, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_DevNbrGetFromName()
 *
 * @brief    Get device number from the USB controller name.
 *
 * @param    name    Name of the USB controller.
 *
 * @return   Device number.
 *******************************************************************************************************/
CPU_INT08U USBD_DevNbrGetFromName(const CPU_CHAR *name)
{
  CPU_INT08U dev_qty;
  CPU_INT08U dev_ix;
  CPU_INT08U dev_nbr = USBD_DEV_NBR_NONE;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  dev_qty = USBD_Ptr->DevQty;
  CORE_EXIT_ATOMIC();

  for (dev_ix = 0u; dev_ix < dev_qty; dev_ix++) {
    CPU_INT16S cmp_result;
    USBD_DEV   *p_dev = &USBD_Ptr->DevTbl[dev_ix];

    cmp_result = Str_Cmp(name, p_dev->Name);
    if (cmp_result == 0) {
      dev_nbr = p_dev->Nbr;
      break;
    }
  }

  return (dev_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_DevStart()
 *
 * @brief    Starts the device stack.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that receives one of the following returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_EP_NONE_AVAIL
 *                           - RTOS_ERR_FAIL
 *
 * @note     (1) Device stack can be only started if the device is in one of the following states:
 *               - USBD_DEV_STATE_NONE    Device controller has not been initialized.
 *               - USBD_DEV_STATE_INIT    Device controller is already   initialized.
 *******************************************************************************************************/
void USBD_DevStart(CPU_INT08U dev_nbr,
                   RTOS_ERR   *p_err)
{
  USBD_DEV     *p_dev;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CPU_BOOLEAN  init;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  p_drv = &p_dev->Drv;
  p_drv_api = p_drv->API_Ptr;
  init = DEF_NO;

  if (p_dev->State == USBD_DEV_STATE_NONE) {                    // If dev not initialized ...
    p_drv_api->Init(p_drv, p_err);                              // ... call dev drv 'Init()' function.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    init = DEF_YES;
  }

  p_drv_api->Start(p_drv, p_err);

  if (init == DEF_YES) {
    CORE_ENTER_ATOMIC();
    p_dev->State = USBD_DEV_STATE_INIT;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                               USBD_DevStop()
 *
 * @brief    Stops the device stack.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *******************************************************************************************************/
void USBD_DevStop(CPU_INT08U dev_nbr,
                  RTOS_ERR   *p_err)
{
  USBD_DEV     *p_dev;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (p_dev->State == USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  USBD_ConfigClose(p_dev);                                      // Close curr cfg.

  p_drv = &p_dev->Drv;
  p_drv_api = p_drv->API_Ptr;

  p_drv_api->Stop(p_drv);

  CORE_ENTER_ATOMIC();
  p_dev->State = USBD_DEV_STATE_INIT;                           // Re-init dev stack to 'INIT' state.
  p_dev->StatePrev = USBD_DEV_STATE_INIT;
  p_dev->ConnStatus = DEF_FALSE;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               USBD_ConfigAdd()
 *
 * @brief    Adds a configuration attribute to the device.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    attrib      Available configuration attributes.
 *                           - USBD_DEV_ATTRIB_SELF_POWERED      Power does not come from VBUS.
 *                           - USBD_DEV_ATTRIB_REMOTE_WAKEUP     Remote wakeup feature enabled.
 *
 * @param    max_pwr     Bus power required for this device (see Note #1).
 *
 * @param    spd         Available configuration speeds.
 *                           - USBD_DEV_SPD_FULL   Configuration is added in the full-speed configuration set.
 *                           - USBD_DEV_SPD_HIGH   Configuration is added in the high-speed configuration set.
 *
 * @param    p_name      Pointer to string describing the configuration (see Note #2).
 *
 * @param    p_err       Pointers to the variable that receives one of the following returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_CONFIG_ALLOC
 *                           - RTOS_ERR_INVALID_ARG
 *
 * @return   Configuration number, if no errors are returned.
 *           USBD_CONFIG_NBR_NONE,    if any errors are returned.
 *
 * @note     (1) USB spec 2.0 (section 7.2.1.3/4) defines power constraints for bus-powered devices:
 *               "A low-power function draws up to one unit load from the USB cable when operational"
 *               "A function is high-power if, when fully powered, draws over one, but no more than
 *               five, unit loads from the USB cable."
 *               A unit load is defined as 100mA, so 'max_pwr' argument should be between 0 mA
 *               and 500mA
 *
 * @note     (2) String support is optional: 'p_name' can be a NULL string pointer.
 *
 * @note     (3) Configuration can only be added when the device is in the following states:
 *                   - USBD_DEV_STATE_NONE    Device controller has not been initialized.
 *                   - USBD_DEV_STATE_INIT    Device controller is already   initialized.
 *
 * @note     (4) A high-speed configuration can only be added if the device controller is high-speed.
 *******************************************************************************************************/
CPU_INT08U USBD_ConfigAdd(CPU_INT08U     dev_nbr,
                          CPU_INT08U     attrib,
                          CPU_INT16U     max_pwr,
                          USBD_DEV_SPD   spd,
                          const CPU_CHAR *p_name,
                          RTOS_ERR       *p_err)
{
  USBD_DEV    *p_dev;
  USBD_CONFIG *p_config;
  CPU_INT08U  config_tbl_ix;
  CPU_INT08U  config_nbr;
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_HS_EN == DEF_DISABLED)
  (void)&spd;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CONFIG_NBR_NONE);

  //                                                               Chk max pwr (see Note #1).
  RTOS_ASSERT_DBG_ERR_SET((max_pwr <= USBD_MAX_BUS_PWR_LIMIT_mA), *p_err, RTOS_ERR_INVALID_ARG, USBD_CONFIG_NBR_NONE);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_CONFIG_NBR_NONE);

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (USBD_CONFIG_NBR_NONE);
  }

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if ((p_dev->Drv.CfgPtr->Spd != USBD_DEV_SPD_HIGH)             // Chk if dev supports high spd.
      && (spd == USBD_DEV_SPD_HIGH)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_CONFIG_NBR_NONE);
  }
#endif

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->ConfigNbrNext == 0u) {                          // Chk if cfg is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CONFIG_ALLOC);
    return (USBD_CONFIG_NBR_NONE);
  }
  config_tbl_ix = USBD_Ptr->ConfigNbrNext - 1u;

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  //                                                               Add cfg to dev HS cfg.
  if (spd == USBD_DEV_SPD_HIGH) {
    config_nbr = p_dev->ConfigHS_TotalNbr;
    if (config_nbr > USBD_CONFIG_NBR_TOT) {                     // Chk cfg limit.
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_CONFIG_ALLOC);
      return (USBD_CONFIG_NBR_NONE);
    }
    p_dev->ConfigHS_TotalNbr++;
  } else {
#endif
  //                                                               ... FS cfg.
  config_nbr = p_dev->ConfigFS_TotalNbr;
  if (config_nbr > USBD_CONFIG_NBR_TOT) {                       // Chk cfg limit.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_CONFIG_ALLOC);
    return (USBD_CONFIG_NBR_NONE);
  }
  p_dev->ConfigFS_TotalNbr++;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif
  USBD_Ptr->ConfigNbrNext--;

  //                                                               ------ CONFIGURATION STRUCTURE INITIALIZATION ------
  p_config = &USBD_Ptr->ConfigTbl[config_tbl_ix];
  //                                                               Link cfg into dev struct.
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CORE_EXIT_ATOMIC();

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (spd == USBD_DEV_SPD_HIGH) {
    p_dev->ConfigHS_SpdTblPtrs[config_nbr] = p_config;
    DEF_BIT_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT);           // Set spd bit in cfg nbr.
  } else {
#endif
  p_dev->ConfigFS_SpdTblPtrs[config_nbr] = p_config;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif

#else
  p_config->NextPtr = DEF_NULL;

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (spd == USBD_DEV_SPD_HIGH) {
    if (p_dev->ConfigHS_HeadPtr == DEF_NULL) {                  // Link cfg in HS list.
      p_dev->ConfigHS_HeadPtr = p_config;
      p_dev->ConfigHS_TailPtr = p_config;
    } else {
      p_dev->ConfigHS_TailPtr->NextPtr = p_config;
      p_dev->ConfigHS_TailPtr = p_config;
    }
    DEF_BIT_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT);           // Set spd bit in cfg nbr.
  } else {
#endif
  if (p_dev->ConfigFS_HeadPtr == DEF_NULL) {                    // Link cfg in FS list.
    p_dev->ConfigFS_HeadPtr = p_config;
    p_dev->ConfigFS_TailPtr = p_config;
  } else {
    p_dev->ConfigFS_TailPtr->NextPtr = p_config;
    p_dev->ConfigFS_TailPtr = p_config;
  }
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif
  CORE_EXIT_ATOMIC();
#endif

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  p_config->NamePtr = p_name;
#endif

  p_config->Attrib = attrib;
  p_config->EP_AllocMap = USBD_EP_CTRL_ALLOC;                   // Init EP alloc bitmap.
  p_config->MaxPwr = max_pwr;
  p_config->DescLen = 0u;                                       // Init cfg desc len.

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  USBD_StrDescAdd(p_dev, p_name, p_err);                        // Add cfg string to dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CONFIG_NBR_NONE);
  }
#else
  (void)&p_name;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#endif

  return (config_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_ConfigOtherSpeed()
 *
 * @brief    Associate a configuration with its alternative-speed counterpart.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    config_other    Other-speed configuration number.
 *
 * @param    p_err           Pointer to the variable that receives one of the following returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *
 * @note     (1) Configurations from high- and full-speed can be associated with each other to provide
 *               comparable functionality regardless of speed.
 *
 * @note     (2) Configuration can ONLY be associated when the device is in the following states:
 *                   - USBD_DEV_STATE_NONE    Device controller has not been initialized.
 *                   - USBD_DEV_STATE_INIT    Device controller is already   initialized.
 *******************************************************************************************************/

#if (USBD_CFG_HS_EN == DEF_ENABLED)
void USBD_ConfigOtherSpeed(CPU_INT08U dev_nbr,
                           CPU_INT08U config_nbr,
                           CPU_INT08U config_other,
                           RTOS_ERR   *p_err)
{
  USBD_DEV    *p_dev;
  USBD_CONFIG *p_config;
  USBD_CONFIG *p_config_other;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  //                                                               Chk if both cfg are from same spd.
  RTOS_ASSERT_DBG_ERR_SET(!(((config_nbr   & USBD_CONFIG_NBR_SPD_BIT)
                             ^ (config_other & USBD_CONFIG_NBR_SPD_BIT)) != USBD_CONFIG_NBR_SPD_BIT), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);
  p_config_other = USBD_ConfigRefGet(p_dev, config_other);
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_config_other != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Chk if cfg already associated.
  RTOS_ASSERT_DBG_ERR_SET(((p_config->ConfigOtherSpd == USBD_CONFIG_NBR_NONE)
                           && (p_config_other->ConfigOtherSpd == USBD_CONFIG_NBR_NONE)), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_config->ConfigOtherSpd = config_other;
  p_config_other->ConfigOtherSpd = config_nbr;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_DevStateGet()
 *
 * @brief    Gets the current device state.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that receives this returned error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Current device state, if no errors are returned.
 *           USBD_DEV_STATE_NONE,  if any errors are returned.
 *******************************************************************************************************/
USBD_DEV_STATE USBD_DevStateGet(CPU_INT08U dev_nbr,
                                RTOS_ERR   *p_err)
{
  USBD_DEV *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_DEV_STATE_NONE);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_DEV_STATE_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (p_dev->State);
}

/****************************************************************************************************//**
 *                                               USBD_DevSpdGet()
 *
 * @brief    Get device speed.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *
 * @return   The current device speed, if successful.
 *           USBD_DEV_SPD_INVALID,     if unsuccessful.
 *******************************************************************************************************/
USBD_DEV_SPD USBD_DevSpdGet(CPU_INT08U dev_nbr,
                            RTOS_ERR   *p_err)
{
  USBD_DEV *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_DEV_SPD_INVALID);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_DEV_SPD_INVALID);

  if (p_dev->State == USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (USBD_DEV_SPD_INVALID);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_dev->Spd);
}

/****************************************************************************************************//**
 *                                           USBD_DevSelfPwrSet()
 *
 * @brief    Sets the device's current power source.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    self_pwr    The power source of the device :
 *                           - DEF_TRUE  device is self-powered.
 *                           - DEF_FALSE device is  bus-powered.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBD_DevSelfPwrSet(CPU_INT08U  dev_nbr,
                        CPU_BOOLEAN self_pwr,
                        RTOS_ERR    *p_err)
{
  USBD_DEV *p_dev;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  p_dev->SelfPwr = self_pwr;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       USBD_DevSetMS_VendorCode()
 *
 * @brief    Set the device's Microsoft vendor code.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    vendor_code     Microsoft vendor code.
 *
 * @param    p_err           Pointer to the variable that receives returned error code from this function :
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) The vendor code used MUST be different from any vendor bRequest value.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
void USBD_DevSetMS_VendorCode(CPU_INT08U dev_nbr,
                              CPU_INT08U vendor_code,
                              RTOS_ERR   *p_err)
{
  USBD_DEV *p_dev;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  p_dev->StrMS_VendorCode = vendor_code;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_DevGetCfg()
 *
 * @brief    Get device configuration.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Pointer to device configuration, if no errors are returned.
 *           Pointer to NULL,                 if any errors are returned.
 *******************************************************************************************************/
USBD_DEV_CFG *USBD_DevCfgGet(CPU_INT08U dev_nbr,
                             RTOS_ERR   *p_err)
{
  USBD_DEV *p_dev;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (&p_dev->DevCfg);
}

/****************************************************************************************************//**
 *                                           USBD_DevFrameNbrGet()
 *
 * @brief    Gets the last frame number from the driver.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NOT_SUPPORTED
 *
 * @return   The current frame number.
 *
 * @note     (1) The frame number will always be in the range of 0-2047 (11 bits).
 *
 * @note     (2) Frame number returned to the caller contains the frame and microframe numbers. It is
 *               encoded following this 16-bit format:
 *               @verbatim
 *               | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *               |  0    0 |  microframe  |                  frame                     |
 *               @endverbatim
 *               Caller must use the macros USBD_FRAME_NBR_GET() or USBD_MICROFRAME_NBR_GET() to get
 *               the frame or microframe number only.
 *******************************************************************************************************/
CPU_INT16U USBD_DevFrameNbrGet(CPU_INT08U dev_nbr,
                               RTOS_ERR   *p_err)
{
  USBD_DEV     *p_dev;
  USBD_DRV     *p_drv;
  USBD_DRV_API *p_drv_api;
  CPU_INT16U   frame_nbr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  if (p_dev->State == USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  p_drv = &p_dev->Drv;
  p_drv_api = p_drv->API_Ptr;
  if (p_drv_api->FrameNbrGet == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return (0u);
  }

  frame_nbr = p_drv_api->FrameNbrGet(p_drv);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (frame_nbr);                                           // See Note #2.
}

/****************************************************************************************************//**
 *                                               USBD_IF_Add()
 *
 * @brief    Adds an interface to a specific configuration.
 *
 * @param    dev_nbr                 Device number.
 *
 * @param    cfg_nbr                 Configuration index to add the interface.
 *
 * @param    p_class_drv             Pointer to interface driver.
 *
 * @param    p_if_class_arg          Pointer to interface driver argument.
 *
 * @param    p_if_alt_class_arg      Pointer to alternate interface argument.
 *
 * @param    class_code              Class code assigned by the USB-IF.
 *
 * @param    class_sub_code          Subclass code assigned by the USB-IF.
 *
 * @param    class_protocol_code     Protocol code assigned by the USB-IF.
 *
 * @param    p_name                  Pointer to string describing the Interface.
 *
 * @param    p_err                   Pointer to the variable that will receive the return error code from these functions :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_INVALID_DEV_STATE
 *                                       - RTOS_ERR_IF_ALT_ALLOC
 *                                       - RTOS_ERR_ALLOC
 *                                       - RTOS_ERR_IF_ALLOC
 *
 * @return   Interface number, if no errors are returned.
 *           USBD_IF_NBR_NONE, if any errors are returned.
 *
 * @note     (1) USB Spec 2.0 Interface (section 9.6.5) states: "An interface may include alternate
 *               settings that allow the endpoints and/or their characteristics to be varied after
 *               the device has been configured. The default setting for an interface is always
 *               an alternate setting of zero."
 *******************************************************************************************************/
CPU_INT08U USBD_IF_Add(CPU_INT08U     dev_nbr,
                       CPU_INT08U     cfg_nbr,
                       USBD_CLASS_DRV *p_class_drv,
                       void           *p_if_class_arg,
                       void           *p_if_alt_class_arg,
                       CPU_INT08U     class_code,
                       CPU_INT08U     class_sub_code,
                       CPU_INT08U     class_protocol_code,
                       const CPU_CHAR *p_name,
                       RTOS_ERR       *p_err)
{
  CPU_INT08U  if_tbl_ix;
  CPU_INT08U  if_nbr;
  CPU_INT08U  if_alt_nbr;
  USBD_DEV    *p_dev;
  USBD_CONFIG *p_config;
  USBD_IF     *p_if;
  USBD_IF_ALT *p_if_alt;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_IF_NBR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_class_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, USBD_IF_NBR_NONE);

  //                                                               Chk if IF_Desc() & IF_DescSizeGet() are either ...
  //                                                               ... present or not.
  RTOS_ASSERT_DBG_ERR_SET((((p_class_drv->IF_Desc == DEF_NULL)
                            && (p_class_drv->IF_DescSizeGet == DEF_NULL))
                           || ((p_class_drv->IF_Desc != DEF_NULL)
                               && (p_class_drv->IF_DescSizeGet != DEF_NULL))), *p_err, RTOS_ERR_NULL_PTR, USBD_IF_NBR_NONE);

  //                                                               Chk if EP_Desc() & EP_DescSizeGet() are either ...
  //                                                               ... present or not.
  RTOS_ASSERT_DBG_ERR_SET((((p_class_drv->EP_Desc == DEF_NULL)
                            && (p_class_drv->EP_DescSizeGet == DEF_NULL))
                           || ((p_class_drv->EP_Desc != DEF_NULL)
                               && (p_class_drv->EP_DescSizeGet != DEF_NULL))), *p_err, RTOS_ERR_NULL_PTR, USBD_IF_NBR_NONE);

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_NBR_NONE);

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (USBD_IF_NBR_NONE);
  }

  p_config = USBD_ConfigRefGet(p_dev, cfg_nbr);                 // Get cfg struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->IF_NbrNext == 0u) {                             // Chk if IF struct is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_ALLOC);
    return (USBD_IF_NBR_NONE);
  }
  if_tbl_ix = USBD_Ptr->IF_NbrNext - 1u;

  if (USBD_Ptr->IF_AltNbrNext == 0u) {                          // Chk if IF alt struct is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_ALT_ALLOC);
    return (USBD_IF_NBR_NONE);
  }
  if_alt_nbr = USBD_Ptr->IF_AltNbrNext - 1u;

  if_nbr = p_config->IF_NbrTotal;                               // Get next IF nbr in cfg.
  if (if_nbr > USBD_IF_NBR_TOT) {                               // Chk IF limit.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_ALLOC);
    return (USBD_IF_NBR_NONE);
  }

  USBD_Ptr->IF_NbrNext--;
  USBD_Ptr->IF_AltNbrNext--;
  p_config->IF_NbrTotal++;

  p_if = &USBD_Ptr->IF_Tbl[if_tbl_ix];
  p_if_alt = &USBD_Ptr->IF_AltTbl[if_alt_nbr];                  // Get IF alt struct (see Note #1).

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Link IF and alt setting.
  CORE_EXIT_ATOMIC();

  p_config->IF_TblPtrs[if_nbr] = p_if;
  p_if->AltTblPtrs[0u] = p_if_alt;
#else
  p_if->NextPtr = DEF_NULL;
  p_if_alt->NextPtr = DEF_NULL;
  p_if->AltHeadPtr = p_if_alt;
  p_if->AltTailPtr = p_if_alt;

  if (p_config->IF_HeadPtr == DEF_NULL) {
    p_config->IF_HeadPtr = p_if;
    p_config->IF_TailPtr = p_if;
  } else {
    p_config->IF_TailPtr->NextPtr = p_if;
    p_config->IF_TailPtr = p_if;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_if->ClassCode = class_code;
  p_if->ClassSubCode = class_sub_code;
  p_if->ClassProtocolCode = class_protocol_code;
  p_if->ClassDrvPtr = p_class_drv;
  p_if->ClassArgPtr = p_if_class_arg;
  p_if->EP_AllocMap = USBD_EP_CTRL_ALLOC;
  p_if->AltCurPtr = p_if_alt;                                   // Set curr alt setting.
  p_if->AltCur = 0u;
  p_if->AltNbrTotal = 1u;
  p_if_alt->EP_AllocMap = p_if->EP_AllocMap;
  p_if_alt->ClassArgPtr = p_if_alt_class_arg;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  p_if_alt->NamePtr = p_name;
#endif

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  USBD_StrDescAdd(p_dev, p_name, p_err);                        // Add IF string to dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_IF_NBR_NONE);
  }
#else
  (void)&p_name;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#endif

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_IF_AltAdd()
 *
 * @brief    Adds an alternate setting to a specific interface.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    p_class_arg     Pointer to alternate interface argument.
 *
 * @param    p_name          Pointer to alternate setting name.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from these functions :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IF_ALT_ALLOC
 *                               - RTOS_ERR_ALLOC
 *
 * @return   Interface alternate setting number, if no error(s) are returned.
 *           USBD_IF_ALT_NBR_NONE, if any errors are returned.
 *******************************************************************************************************/
CPU_INT08U USBD_IF_AltAdd(CPU_INT08U     dev_nbr,
                          CPU_INT08U     config_nbr,
                          CPU_INT08U     if_nbr,
                          void           *p_class_arg,
                          const CPU_CHAR *p_name,
                          RTOS_ERR       *p_err)
{
  USBD_DEV    *p_dev;
  USBD_CONFIG *p_config;
  USBD_IF     *p_if;
  USBD_IF_ALT *p_if_alt;
  CPU_INT08U  if_alt_tbl_ix;
  CPU_INT08U  if_alt_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  //                                                               -------------- GET OBJECT REFERENCES ---------------
  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_ALT_NBR_NONE);

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);              // Get config struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_ALT_NBR_NONE);

  p_if = USBD_IF_RefGet(p_config, if_nbr);                      // Get IF struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_ALT_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->IF_AltNbrNext == 0u) {                          // Chk if next alt setting is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_ALT_ALLOC);
    return (USBD_IF_ALT_NBR_NONE);
  }
  if_alt_tbl_ix = USBD_Ptr->IF_AltNbrNext - 1u;

  if_alt_nbr = p_if->AltNbrTotal;
  if (if_alt_nbr > USBD_IF_ALT_NBR_TOT) {                       // Chk if alt setting is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_ALT_ALLOC);
    return (USBD_IF_ALT_NBR_NONE);
  }

  USBD_Ptr->IF_AltNbrNext--;
  p_if->AltNbrTotal++;

  p_if_alt = &USBD_Ptr->IF_AltTbl[if_alt_tbl_ix];

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Add alt setting to IF.
  CORE_EXIT_ATOMIC();

  p_if->AltTblPtrs[if_alt_nbr] = p_if_alt;
#else
  p_if_alt->NextPtr = DEF_NULL;

  p_if->AltTailPtr->NextPtr = p_if_alt;
  p_if->AltTailPtr = p_if_alt;
  CORE_EXIT_ATOMIC();
#endif

  p_if_alt->ClassArgPtr = p_class_arg;
  p_if_alt->EP_AllocMap = USBD_EP_CTRL_ALLOC;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  p_if_alt->NamePtr = p_name;
#endif

  DEF_BIT_CLR(p_if_alt->EP_AllocMap, p_if->EP_AllocMap);
  DEF_BIT_SET(p_if_alt->EP_AllocMap, USBD_EP_CTRL_ALLOC);

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  USBD_StrDescAdd(p_dev, p_name, p_err);                        // Add alt setting string to dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_IF_ALT_NBR_NONE);
  }
#else
  (void)&p_name;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#endif

  return (if_alt_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_IF_Grp()
 *
 * @brief    Creates an interface group.
 *
 * @param    dev_nbr                 Device number
 *
 * @param    config_nbr              Configuration number.
 *
 * @param    class_code              Class code assigned by the USB-IF.
 *
 * @param    class_sub_code          Subclass code assigned by the USB-IF.
 *
 * @param    class_protocol_code     Protocol code assigned by the USB-IF.
 *
 * @param    if_start                Interface number of the first interface that is associated with this group.
 *
 * @param    if_cnt                  Number of consecutive interfaces that are associated with this group.
 *
 * @param    p_name                  Pointer to the string that describes the interface group.
 *
 * @param    p_err                   Pointer to the variable that will receive the return error code from these functions :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ALLOC
 *                                       - RTOS_ERR_IF_GRP_ALLOC
 *                                       - RTOS_ERR_ALREADY_EXISTS
 *
 * @return   Interface group number, if no errors are returned.
 *           USBD_IF_GRP_NBR_NONE,   if any errors are returned.
 *******************************************************************************************************/
CPU_INT08U USBD_IF_Grp(CPU_INT08U     dev_nbr,
                       CPU_INT08U     config_nbr,
                       CPU_INT08U     class_code,
                       CPU_INT08U     class_sub_code,
                       CPU_INT08U     class_protocol_code,
                       CPU_INT08U     if_start,
                       CPU_INT08U     if_cnt,
                       const CPU_CHAR *p_name,
                       RTOS_ERR       *p_err)
{
  USBD_DEV    *p_dev;
  USBD_CONFIG *p_config;
  USBD_IF     *p_if;
  USBD_IF_GRP *p_if_grp;
  CPU_INT08U  if_grp_tbl_ix;
  CPU_INT08U  if_grp_nbr;
  CPU_INT08U  if_nbr;
  CPU_INT08U  if_end;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((((CPU_INT16U)(if_start) + (CPU_INT16U)(if_cnt)) <= (CPU_INT16U)USBD_IF_NBR_TOT), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_GRP_NBR_NONE);

  //                                                               --------------- GET OBJECT REFERENCES --------------
  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_GRP_NBR_NONE);

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);              // Get cfg struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_GRP_NBR_NONE);

  //                                                               Verify that IFs do NOT belong to another group.
  for (if_nbr = 0u; if_nbr < if_cnt; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr + if_start);
    RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_GRP_NBR_NONE);

    if (p_if->GrpNbr != USBD_IF_GRP_NBR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
      return (USBD_IF_GRP_NBR_NONE);
    }
  }

  CORE_ENTER_ATOMIC();

  if (USBD_Ptr->IF_GrpNbrNext == 0u) {                          // Chk if IF grp is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IF_GRP_ALLOC);
    return (USBD_IF_GRP_NBR_NONE);
  }
  USBD_Ptr->IF_GrpNbrNext--;
  if_grp_tbl_ix = USBD_Ptr->IF_GrpNbrNext;

  p_if_grp = &USBD_Ptr->IF_GrpTbl[if_grp_tbl_ix];

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  if_grp_nbr = p_config->IF_GrpNbrTotal;
  p_config->IF_GrpNbrTotal++;
  CORE_EXIT_ATOMIC();

  p_config->IF_GrpTblPtrs[if_grp_nbr] = p_if_grp;
#else
  p_if_grp->NextPtr = DEF_NULL;

  if_grp_nbr = p_config->IF_GrpNbrTotal;
  p_config->IF_GrpNbrTotal++;

  if (if_grp_nbr == 0u) {
    p_config->IF_GrpHeadPtr = p_if_grp;
    p_config->IF_GrpTailPtr = p_if_grp;
  } else {
    p_config->IF_GrpTailPtr->NextPtr = p_if_grp;
    p_config->IF_GrpTailPtr = p_if_grp;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_if_grp->ClassCode = class_code;
  p_if_grp->ClassSubCode = class_sub_code;
  p_if_grp->ClassProtocolCode = class_protocol_code;
  p_if_grp->IF_Start = if_start;
  p_if_grp->IF_Cnt = if_cnt;

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  p_if_grp->NamePtr = p_name;
#endif

  if_end = if_cnt + if_start;
  for (if_nbr = if_start; if_nbr < if_end; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_IF_GRP_NBR_NONE);

    if (p_if->GrpNbr != USBD_IF_GRP_NBR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
      return (USBD_IF_GRP_NBR_NONE);
    }

    CORE_ENTER_ATOMIC();
    p_if->GrpNbr = if_grp_nbr;
    CORE_EXIT_ATOMIC();
  }

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  USBD_StrDescAdd(p_dev, p_name, p_err);                        // Add IF grp string to dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_IF_GRP_NBR_NONE);
  }
#else
  (void)&p_name;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#endif

  return (if_grp_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_DescDevGet()
 *
 * @brief    Gets the device descriptor.
 *
 * @param    p_drv       Pointer to the device driver structure.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in destination buffer.
 *
 * @param    p_err       Pointer to the variable that receives one of the returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of bytes actually in the descriptor, if no errors are returned.
 *           0,                                          if any errors are returned.
 *
 * @note     (1) This function should be used by drivers that supporting the standard requests auto-reply,
 *               (during the initialization process).
 *******************************************************************************************************/
CPU_INT08U USBD_DescDevGet(USBD_DRV   *p_drv,
                           CPU_INT08U *p_buf,
                           CPU_INT08U max_len,
                           RTOS_ERR   *p_err)
{
  USBD_DEV   *p_dev;
  CPU_INT08U desc_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((max_len != 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  p_dev = USBD_DevRefGet(p_drv->DevNbr);                        // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  if (p_dev->State != USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  p_dev->ActualBufPtr = p_buf;
  p_dev->DescBufMaxLen = max_len;
  p_dev->DescBufErrPtr = p_err;

  USBD_DevDescSend(p_dev,
                   DEF_NO,
                   max_len,
                   p_err);

  desc_len = p_dev->DescBufIx;
  p_dev->DescBufErrPtr = DEF_NULL;

  return (desc_len);
}

/****************************************************************************************************//**
 *                                           USBD_DescConfigGet()
 *
 * @brief    Gets a configuration descriptor.
 *
 * @param    p_drv       Pointer to device driver structure.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in the destination buffer.
 *
 * @param    config_ix   Index of the desired configuration descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of bytes actually in the descriptor, if no errors are returned.
 *           0,                                          if any errors are returned.
 *
 * @note     (1) This function should be used by drivers supporting a standard request's auto-reply,
 *               during the initialization process.
 *******************************************************************************************************/
CPU_INT16U USBD_DescConfigGet(USBD_DRV   *p_drv,
                              CPU_INT08U *p_buf,
                              CPU_INT16U max_len,
                              CPU_INT08U config_ix,
                              RTOS_ERR   *p_err)
{
  USBD_DEV   *p_dev;
  CPU_INT08U desc_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((max_len != 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  p_dev = USBD_DevRefGet(p_drv->DevNbr);                        // Get dev struct.
  if (p_dev == DEF_NULL) {
    return (0u);
  }

  if (p_dev->State != USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  p_dev->ActualBufPtr = p_buf;
  p_dev->DescBufMaxLen = max_len;
  p_dev->DescBufErrPtr = p_err;

  USBD_ConfigDescSend(p_dev,
                      config_ix,
                      DEF_NO,
                      max_len,
                      p_err);

  desc_len = p_dev->DescBufIx;
  p_dev->DescBufErrPtr = DEF_NULL;

  return (desc_len);
}

/****************************************************************************************************//**
 *                                               USBD_DescStrGet()
 *
 * @brief    Get a string descriptor.
 *
 * @param    p_drv       Pointer to the device driver structure.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in destination buffer.
 *
 * @param    str_ix      Index of the desired string descriptor.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_EP_INVALID
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Number of bytes actually in the descriptor, if no errors are returned.
 *           0,                                          if any errors are returned.
 *
 * @note     (1) This function should be used by drivers supporting a standard request's auto-reply,
 *               during the initialization process.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBD_DescStrGet(USBD_DRV   *p_drv,
                           CPU_INT08U *p_buf,
                           CPU_INT08U max_len,
                           CPU_INT08U str_ix,
                           RTOS_ERR   *p_err)
{
  USBD_DEV   *p_dev;
  CPU_INT08U desc_len;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((max_len != 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  p_dev = USBD_DevRefGet(p_drv->DevNbr);                        // Get dev struct.
  if (p_dev == DEF_NULL) {
    return (0u);
  }

  if (p_dev->State != USBD_DEV_STATE_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return (0u);
  }

  p_dev->ActualBufPtr = p_buf;
  p_dev->DescBufMaxLen = max_len;
  p_dev->DescBufErrPtr = p_err;

  USBD_StrDescSend(p_dev,
                   str_ix,
                   max_len,
                   p_err);

  desc_len = p_dev->DescBufIx;
  p_dev->DescBufErrPtr = DEF_NULL;

  return (desc_len);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_StrAdd()
 *
 * @brief    Add string to USB device.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_str       Pointer to string to add (see Note #1).
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from these functions :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_DEV_STATE
 *                           - RTOS_ERR_ALLOC
 *
 * @note     (1) USB spec 2.0 chapter 9.5 states: "Where appropriate, descriptors contain references
 *               to string descriptors that provide displayable information describing a descriptor
 *               in human-readable form. The inclusion of string descriptors is optional.  However,
 *               the reference fields within descriptors are mandatory. If a device does not support
 *               string descriptors, string reference fields must be reset to zero to indicate no
 *               string descriptor is available".
 *               Since string descriptors are optional, 'p_str' could be a NULL pointer.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
void USBD_StrAdd(CPU_INT08U     dev_nbr,
                 const CPU_CHAR *p_str,
                 RTOS_ERR       *p_err)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  USBD_StrDescAdd(p_dev, p_str, p_err);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_StrIxGet()
 *
 * @brief    Get string index corresponding to a given string.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_str       Pointer to string.
 *
 * @return   String index.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBD_StrIxGet(CPU_INT08U     dev_nbr,
                         const CPU_CHAR *p_str)
{
  USBD_DEV   *p_dev;
  CPU_INT08U str_ix;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return (0u);
  }

  str_ix = USBD_StrDescIxGet(p_dev, p_str);

  return (str_ix);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_DescWr08()
 *
 * @brief    Writes an 8-bit value to the descriptor buffer.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    val         8-bit value to write in the descriptor buffer.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *******************************************************************************************************/
void USBD_DescWr08(CPU_INT08U dev_nbr,
                   CPU_INT08U val)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    USBD_DescWrReq(p_dev, &val, 1u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWr16()
 *
 * @brief    Write 16-bit value in the descriptor buffer.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----  Argument validated by the caller(s)
 *
 * @param    val     16-bit value.
 *******************************************************************************************************/
void USBD_DescWr16(CPU_INT08U dev_nbr,
                   CPU_INT16U val)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    CPU_INT08U buf[2u];

    buf[0u] = (CPU_INT08U)(val        & DEF_INT_08_MASK);
    buf[1u] = (CPU_INT08U)((val >> 8u) & DEF_INT_08_MASK);

    USBD_DescWrReq(p_dev, &buf[0u], 2u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWr24()
 *
 * @brief    Writes a 24-bit value to the descriptor buffer.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    val         32-bit value containing 24 useful bits to write in the descriptor buffer.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *
 * @note     (2) USB descriptors are in little-endian format.
 *******************************************************************************************************/
void USBD_DescWr24(CPU_INT08U dev_nbr,
                   CPU_INT32U val)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    CPU_INT08U buf[3u];

    buf[0u] = (CPU_INT08U)(val         & DEF_INT_08_MASK);
    buf[1u] = (CPU_INT08U)((val >> 8u)  & DEF_INT_08_MASK);
    buf[2u] = (CPU_INT08U)((val >> 16u) & DEF_INT_08_MASK);

    USBD_DescWrReq(p_dev, &buf[0u], 3u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWr32()
 *
 * @brief    Writes a 32-bit value to the descriptor buffer.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    val         32-bit value to write in the descriptor buffer.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *
 * @note     (2) USB descriptors are in little-endian format.
 *******************************************************************************************************/
void USBD_DescWr32(CPU_INT08U dev_nbr,
                   CPU_INT32U val)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    CPU_INT08U buf[4u];

    buf[0u] = (CPU_INT08U)(val         & DEF_INT_08_MASK);
    buf[1u] = (CPU_INT08U)((val >> 8u) & DEF_INT_08_MASK);
    buf[2u] = (CPU_INT08U)((val >> 16u) & DEF_INT_08_MASK);
    buf[3u] = (CPU_INT08U)((val >> 24u) & DEF_INT_08_MASK);

    USBD_DescWrReq(p_dev, &buf[0u], 4u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWr()
 *
 * @brief    Writes a buffer into the descriptor buffer.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    p_buf       Pointer to the buffer to write into the descriptor buffer.
 *
 * @param    len         Length of the buffer.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *******************************************************************************************************/
void USBD_DescWr(CPU_INT08U       dev_nbr,
                 const CPU_INT08U *p_buf,
                 CPU_INT16U       len)
{
  USBD_DEV *p_dev;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  if ((p_buf == DEF_NULL)
      || (len == 0u)) {
    return;
  }

  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    USBD_DescWrReq(p_dev, p_buf, len);
  }
}

/****************************************************************************************************//**
 *                                               USBD_BulkAdd()
 *
 * @brief    Add a bulk endpoint to alternate setting interface.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    dir_in          Endpoint direction.
 *                               - DEF_YES    IN   direction.
 *                               - DEF_NO     OUT  direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet length (see Note #1)
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_EP_ALLOC
 *                               - RTOS_ERR_EP_NONE_AVAIL
 *
 * @return   Endpoint address,  if no errors are returned.
 *           USBD_EP_ADDR_NONE, if any errors are returned.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack will allocate the first available
 *               BULK endpoint, regardless its maximum packet size.
 *******************************************************************************************************/
CPU_INT08U USBD_BulkAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT16U  max_pkt_len,
                        RTOS_ERR    *p_err)
{
  CPU_INT08U ep_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (USBD_CFG_HS_EN == DEF_ENABLED)                             // USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  if (((max_pkt_len != 0u)                                      // Chk EP size.
       && (max_pkt_len != 512u))
      && (DEF_BIT_IS_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT))) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_ADDR_NONE);
  }
#endif

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (((max_pkt_len != 0u)
       && (max_pkt_len != 8u)
       && (max_pkt_len != 16u)
       && (max_pkt_len != 32u)
       && (max_pkt_len != 64u))
      && (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES)) {
#else
  if ( (max_pkt_len != 0u)
       && (max_pkt_len != 8u)
       && (max_pkt_len != 16u)
       && (max_pkt_len != 32u)
       && (max_pkt_len != 64u)) {
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_ADDR_NONE);
  }

  ep_addr = USBD_EP_Add(dev_nbr,
                        config_nbr,
                        if_nbr,
                        if_alt_nbr,
                        USBD_EP_TYPE_BULK,
                        dir_in,
                        max_pkt_len,
                        0u,
                        p_err);
  return (ep_addr);
}

/****************************************************************************************************//**
 *                                               USBD_IntrAdd()
 *
 * @brief    Adds an interrupt endpoint to an alternate setting interface.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    dir_in          Endpoint Direction.
 *                               - DEF_YES    IN   direction.
 *                               - DEF_NO     OUT  direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet length. (see Note #1)
 *
 * @param    interval        Endpoint interval in frames or microframes.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from these functions :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_EP_ALLOC
 *                               - RTOS_ERR_EP_NONE_AVAIL
 *
 * @return   Endpoint address,  if no errors are returned.
 *           USBD_EP_ADDR_NONE, if any errors are returned.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack will allocate the first available
 *               INTERRUPT endpoint, regardless its maximum packet size.
 *
 * @note     (2) For high-speed interrupt endpoints, bInterval value must be in the range
 *               from 1 to 16. The bInterval value is used as the exponent for a 2^(bInterval-1)
 *               value. Maximum polling interval value is 2^(16-1) = 32768 32768 microframes
 *               (i.e. 4096 frames) in high-speed.
 *******************************************************************************************************/
CPU_INT08U USBD_IntrAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT16U  max_pkt_len,
                        CPU_INT16U  interval,
                        RTOS_ERR    *p_err)
{
  CPU_INT08U ep_addr;
  CPU_INT08U interval_code;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((interval != 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);

#if (USBD_CFG_HS_EN == DEF_ENABLED)                             // USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
                                                                // Full spd validation.
  if (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
#endif
  if (max_pkt_len > 64u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (interval < 255u) {
    interval_code = (CPU_INT08U)interval;
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }
#if (USBD_CFG_HS_EN == DEF_ENABLED)
} else {                                                        // High spd validation.
  if (((if_alt_nbr == 0u)
       && (max_pkt_len > 64u))
      || (max_pkt_len > 1024u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (interval > USBD_EP_MAX_INTERVAL_VAL) {                    // See Note #2.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (MATH_IS_PWR2(interval) == DEF_NO) {                       // Interval must be a power of 2.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }
  //                                                               Compute bInterval exponent in 2^(bInterval-1).
  interval_code = (CPU_INT08U)(32u - CPU_CntLeadZeros32(interval));

  if (interval_code > 16u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }
}
#endif

  ep_addr = USBD_EP_Add(dev_nbr,
                        config_nbr,
                        if_nbr,
                        if_alt_nbr,
                        USBD_EP_TYPE_INTR,
                        dir_in,
                        max_pkt_len,
                        interval_code,
                        p_err);
  return (ep_addr);
}

/****************************************************************************************************//**
 *                                               USBD_IsocAdd()
 *
 * @brief    Add an isochronous endpoint to alternate setting interface.
 *
 * @param    dev_nbr             Device number.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    dir_in              Endpoint Direction :
 *                                   - DEF_YES,    IN  direction.
 *                                   - DEF_NO,     OUT direction.
 *
 * @param    attrib              Isochronous endpoint synchronization and usage type attributes.
 *
 * @param    max_pkt_len         Endpoint maximum packet length (see Note #1).
 *
 * @param    transaction_frame   Endpoint transactions per (micro)frame (see Note #2).
 *
 * @param    interval            Endpoint interval in frames or microframes.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from these functions :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_EP_ALLOC
 *                                   - RTOS_ERR_EP_NONE_AVAIL
 *
 * @return   Endpoint address,  if no errors are returned.
 *           USBD_EP_ADDR_NONE, if any errors are returned.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack allocates the first available
 *               ISOCHRONOUS endpoint regardless of its maximum packet size.
 *
 * @note     (2) For full-speed endpoints, 'transaction_frame' must be set to 1 since there is no
 *               support for high-bandwidth endpoints.
 *
 * @note     (3) For full-/high-speed isochronous endpoints, bInterval value must be in the range
 *               from 1 to 16. The bInterval value is used as the exponent for a 2^(bInterval-1)
 *               value. Maximum polling interval value is 2^(16-1) = 32768 frames in full-speed and
 *               32768 microframes (i.e. 4096 frames) in high-speed.
 *******************************************************************************************************/

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
CPU_INT08U USBD_IsocAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT08U  attrib,
                        CPU_INT16U  max_pkt_len,
                        CPU_INT08U  transaction_frame,
                        CPU_INT16U  interval,
                        RTOS_ERR    *p_err)
{
  CPU_INT08U ep_addr;
  CPU_INT16U pkt_len;
  CPU_INT08U interval_code;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  if ((if_alt_nbr == 0u)                                        // Chk if dflt IF setting with isoc EP max_pkt_len > 0.
      && (max_pkt_len > 0u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  //                                                               Chk if sync & usage bits are used.
  if ((attrib & (CPU_INT08U)(~(USBD_EP_TYPE_SYNC_MASK | USBD_EP_TYPE_USAGE_MASK))) != 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

#if (USBD_CFG_HS_EN == DEF_ENABLED)                             // USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
                                                                // Full spd validation.
  if (DEF_BIT_IS_CLR(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
#endif
  if (max_pkt_len > 1023u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (transaction_frame != 1u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }
#if (USBD_CFG_HS_EN == DEF_ENABLED)
} else {                                                        // High spd validation.
  switch (transaction_frame) {
    case 1u:
      if (max_pkt_len > 1024u) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (USBD_EP_NBR_NONE);
      }
      break;

    case 2u:
      if ((max_pkt_len < 513u)
          || (max_pkt_len > 1024u)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (USBD_EP_NBR_NONE);
      }
      break;

    case 3u:
      if ((max_pkt_len < 683u)
          || (max_pkt_len > 1024u)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (USBD_EP_NBR_NONE);
      }
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return (USBD_EP_NBR_NONE);
  }
}
#endif

  //                                                               Explicit feedback EP must be set to no sync.
  if (((attrib & USBD_EP_TYPE_USAGE_MASK) == USBD_EP_TYPE_USAGE_FEEDBACK)
      && ((attrib & USBD_EP_TYPE_SYNC_MASK) != USBD_EP_TYPE_SYNC_NONE)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (interval > USBD_EP_MAX_INTERVAL_VAL) {                    // See Note #3.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  if (MATH_IS_PWR2(interval) == DEF_NO) {                       // Interval must be a power of 2.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }
  //                                                               Compute bInterval exponent in 2^(bInterval-1).
  interval_code = (CPU_INT08U)(32u - CPU_CntLeadZeros32(interval));

  if (interval_code > 16u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (USBD_EP_NBR_NONE);
  }

  pkt_len = (transaction_frame - 1u) << 11u
            | max_pkt_len;

  ep_addr = USBD_EP_Add(dev_nbr,
                        config_nbr,
                        if_nbr,
                        if_alt_nbr,
                        USBD_EP_TYPE_ISOC | attrib,
                        dir_in,
                        pkt_len,
                        interval_code,
                        p_err);

  return (ep_addr);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_IsocSyncRefreshSet()
 *
 * @brief    Set synchronization feedback rate on synchronization isochronous endpoint.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    synch_ep_addr   Synchronization endpoint address.
 *
 * @param    sync_refresh    Exponent of synchronization feedback rate (see Note #3).
 *
 * @param    p_err           Pointer to the variable that receives the return error code from these functions :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_EP_INVALID
 *
 * @note     (1) Synchronization endpoints can ONLY be associated when the device is in the following
 *               states:
 *                   - USBD_DEV_STATE_NONE    Device controller has not been initialized.
 *                   - USBD_DEV_STATE_INIT    Device controller is already      initialized.
 *
 * @note     (2) For audio class 1.0, the interface class code must be USBD_CLASS_CODE_AUDIO and
 *               protocol 'zero'
 *
 * @note     (3) If explicit synchronization mechanism is needed to maintain synchronization during
 *               transfers, the information carried over the synchronization path must be available
 *               every 2 ^ (10 - P) frames, with P ranging from 1 to 9 (512 ms down to 2 ms).
 *
 * @note     (4) Table 4-22 "Standard AS Isochronous Synch Endpoint Descriptor" of Audio 1.0
 *               specification indicates for bmAttributes field no usage type for bits 5..4. But
 *               USB 2.0 specification, Table 9-13 "Standard Endpoint Descriptor" indicates several
 *               types of usage. When an explicit feedback is defined for a asynchronous isochronous
 *               endpoint, the associated synch feedback should use the Usage type 'Feedback endpoint'.
 *******************************************************************************************************/

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
void USBD_IsocSyncRefreshSet(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             CPU_INT08U if_nbr,
                             CPU_INT08U if_alt_nbr,
                             CPU_INT08U synch_ep_addr,
                             CPU_INT08U sync_refresh,
                             RTOS_ERR   *p_err)
{
  USBD_DEV     *p_dev;
  USBD_CONFIG  *p_config;
  USBD_IF      *p_if;
  USBD_IF_ALT  *p_if_alt;
  USBD_EP_INFO *p_ep;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CPU_INT32U ep_alloc_map;
#endif
  CPU_INT08U  ep_nbr;
  CPU_BOOLEAN found;
  CORE_DECLARE_IRQ_STATE;

  if ((sync_refresh < 1u)                                       // See Note #3.
      || (sync_refresh > 9u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  //                                                               --------------- GET OBJECT REFERENCES --------------
  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);              // Get cfg struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_if = USBD_IF_RefGet(p_config, if_nbr);                      // Get IF struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Chk if audio class.
  RTOS_ASSERT_DBG_ERR_SET((p_if->ClassCode == USBD_CLASS_CODE_AUDIO), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Chk if audio class, version 1.0.
  RTOS_ASSERT_DBG_ERR_SET((p_if->ClassProtocolCode == 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);               // Get IF alt setting struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if_alt != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  found = DEF_NO;
  p_ep = DEF_NULL;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  ep_alloc_map = p_if_alt->EP_TblMap;
  while ((ep_alloc_map != DEF_BIT_NONE)
         && (found != DEF_YES)) {
    ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
    p_ep = p_if_alt->EP_TblPtrs[ep_nbr];

    if (p_ep->Addr == synch_ep_addr) {
      found = DEF_YES;
    }

    DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
  }
#else
  p_ep = p_if_alt->EP_HeadPtr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
    if (p_ep->Addr == synch_ep_addr) {
      found = DEF_YES;
      break;
    }

    p_ep = p_ep->NextPtr;
  }
#endif

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }
  //                                                               Chk EP type attrib.
  if ((p_ep->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_ISOC) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  switch (p_ep->Attrib & USBD_EP_TYPE_SYNC_MASK) {              // Chk EP sync type attrib.
    case USBD_EP_TYPE_SYNC_NONE:
      break;

    case USBD_EP_TYPE_SYNC_ASYNC:
    case USBD_EP_TYPE_SYNC_ADAPTIVE:
    case USBD_EP_TYPE_SYNC_SYNC:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return;
  }

  switch (p_ep->Attrib & USBD_EP_TYPE_USAGE_MASK) {             // Chk EP usage type attrib.
    case USBD_EP_TYPE_USAGE_FEEDBACK:                           // See Note #4.
      break;

    case USBD_EP_TYPE_USAGE_DATA:
    case USBD_EP_TYPE_USAGE_IMPLICIT_FEEDBACK:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return;
  }

  if (p_ep->SyncAddr != 0u) {                                   // Chk associated sync EP addr.
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  CORE_ENTER_ATOMIC();
  p_ep->SyncRefresh = sync_refresh;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_IsocSyncAddrSet()
 *
 * @brief    Associates synchronization endpoint to isochronous endpoint.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    data_ep_addr    Data endpoint address.
 *
 * @param    sync_addr       Associated synchronization endpoint.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from these functions :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_DEV_STATE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_EP_INVALID
 *
 * @note     (1) Synchronization endpoints can ONLY be associated when the device is in the following
 *               states:
 *                   - USBD_DEV_STATE_NONE    Device controller has not been initialized.
 *                   - USBD_DEV_STATE_INIT    Device controller is already      initialized.
 *
 * @note     (2) For audio class 1.0, interface class code must be USBD_CLASS_CODE_AUDIO and protocol
 *               'zero'.
 *******************************************************************************************************/

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
void USBD_IsocSyncAddrSet(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          CPU_INT08U if_nbr,
                          CPU_INT08U if_alt_nbr,
                          CPU_INT08U data_ep_addr,
                          CPU_INT08U sync_addr,
                          RTOS_ERR   *p_err)
{
  USBD_DEV     *p_dev;
  USBD_CONFIG  *p_config;
  USBD_IF      *p_if;
  USBD_IF_ALT  *p_if_alt;
  USBD_EP_INFO *p_ep;
  USBD_EP_INFO *p_ep_isoc;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CPU_INT32U ep_alloc_map;
#endif
  CPU_INT08U  ep_nbr;
  CPU_BOOLEAN found_ep;
  CPU_BOOLEAN found_sync;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- GET OBJECT REFERENCES --------------
  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_dev->State != USBD_DEV_STATE_NONE)                     // Chk curr dev state.
      && (p_dev->State != USBD_DEV_STATE_INIT)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DEV_STATE);
    return;
  }

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);              // Get cfg struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_if = USBD_IF_RefGet(p_config, if_nbr);                      // Get IF struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Chk if audio class.
  RTOS_ASSERT_DBG_ERR_SET((p_if->ClassCode == USBD_CLASS_CODE_AUDIO), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Chk if audio class, version 1.0.
  RTOS_ASSERT_DBG_ERR_SET((p_if->ClassProtocolCode == 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);               // Get IF alt setting struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if_alt != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  found_ep = DEF_NO;
  found_sync = DEF_NO;
  p_ep_isoc = DEF_NULL;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  ep_alloc_map = p_if_alt->EP_TblMap;
  while ((ep_alloc_map != DEF_BIT_NONE)
         && ((found_ep != DEF_YES)
             || (found_sync != DEF_YES))) {
    ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
    p_ep = p_if_alt->EP_TblPtrs[ep_nbr];

    if (p_ep->Addr == data_ep_addr) {
      found_ep = DEF_YES;
      p_ep_isoc = p_ep;
    }

    if (p_ep->Addr == sync_addr) {
      found_sync = DEF_YES;
    }

    DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
  }
#else
  p_ep = p_if_alt->EP_HeadPtr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
    if (p_ep->Addr == data_ep_addr) {
      found_ep = DEF_YES;
      p_ep_isoc = p_ep;
    }

    if (p_ep->Addr == sync_addr) {
      found_sync = DEF_YES;
    }

    if ((found_ep == DEF_YES)
        && (found_sync == DEF_YES)) {
      break;
    }

    p_ep = p_ep->NextPtr;
  }
#endif

  if (found_ep != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  if (found_sync != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  //                                                               Chk EP type attrib.
  if ((p_ep_isoc->Attrib & USBD_EP_TYPE_MASK) != USBD_EP_TYPE_ISOC) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  switch (p_ep_isoc->Attrib & USBD_EP_TYPE_SYNC_MASK) {         // Chk EP sync type attrib.
    case USBD_EP_TYPE_SYNC_ASYNC:
      if (USBD_EP_IS_IN(p_ep_isoc->Addr) == DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
        return;
      }
      break;

    case USBD_EP_TYPE_SYNC_ADAPTIVE:
      if (USBD_EP_IS_IN(p_ep_isoc->Addr) == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
        return;
      }
      break;

    case USBD_EP_TYPE_SYNC_NONE:
    case USBD_EP_TYPE_SYNC_SYNC:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return;
  }

  switch (p_ep_isoc->Attrib & USBD_EP_TYPE_USAGE_MASK) {        // Chk EP usage type attrib.
    case USBD_EP_TYPE_USAGE_DATA:
      break;

    case USBD_EP_TYPE_USAGE_FEEDBACK:
    case USBD_EP_TYPE_USAGE_IMPLICIT_FEEDBACK:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
      return;
  }

  if (p_ep_isoc->SyncRefresh != 0u) {                           // Refresh interval must be set to zero.
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  CORE_ENTER_ATOMIC();
  p_ep_isoc->SyncAddr = sync_addr;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_EP_Add()
 *
 * @brief    Add an endpoint to alternate setting interface.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    attrib          Endpoint's attributes.
 *
 * @param    dir_in          Endpoint Direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet size.
 *
 * @param    -----------     Argument validated by the caller.
 *
 * @param    interval        Interval for polling data transfers.
 *
 * @param    --------        Argument validated by the caller.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Endpoint number,  if no errors are returned.
 *           USBD_EP_NBR_NONE, if any errors are returned.
 *******************************************************************************************************/
static CPU_INT08U USBD_EP_Add(CPU_INT08U  dev_nbr,
                              CPU_INT08U  config_nbr,
                              CPU_INT08U  if_nbr,
                              CPU_INT08U  if_alt_nbr,
                              CPU_INT08U  attrib,
                              CPU_BOOLEAN dir_in,
                              CPU_INT16U  max_pkt_len,
                              CPU_INT08U  interval,
                              RTOS_ERR    *p_err)

{
  USBD_DEV     *p_dev;
  USBD_CONFIG  *p_config;
  USBD_IF      *p_if;
  USBD_IF_ALT  *p_if_alt;
  USBD_EP_INFO *p_ep;
  CPU_INT08U   ep_type;
  CPU_INT32U   ep_alloc_map;
  CPU_INT32U   ep_alloc_map_clr;
  CPU_INT08U   ep_nbr;
  CPU_INT08U   ep_phy_nbr;
  USBD_DEV_SPD dev_spd;
  CPU_BOOLEAN  alloc;
  CORE_DECLARE_IRQ_STATE;

  //                                                               -------------- GET OBJECT REFERENCES ---------------
  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  RTOS_ASSERT_DBG_ERR_SET((p_dev != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_EP_NBR_NONE);

  p_config = USBD_ConfigRefGet(p_dev, config_nbr);              // Get cfg struct.
  RTOS_ASSERT_DBG_ERR_SET((p_config != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_EP_NBR_NONE);

  p_if = USBD_IF_RefGet(p_config, if_nbr);                      // Get IF struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_EP_NBR_NONE);

  p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);               // Get IF alt setting struct.
  RTOS_ASSERT_DBG_ERR_SET((p_if_alt != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, USBD_EP_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->EP_InfoNbrNext == 0u) {                         // Chk if EP is avail.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_ALLOC);
    return (USBD_EP_NBR_NONE);
  }
  USBD_Ptr->EP_InfoNbrNext--;
  ep_nbr = USBD_Ptr->EP_InfoNbrNext;
  CORE_EXIT_ATOMIC();

  ep_type = attrib & USBD_EP_TYPE_MASK;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
    dev_spd = USBD_DEV_SPD_HIGH;
  } else {
#endif
  dev_spd = USBD_DEV_SPD_FULL;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif

  p_ep = &USBD_Ptr->EP_InfoTbl[ep_nbr];
  p_ep->Interval = interval;
  p_ep->Attrib = attrib;
  p_ep->SyncAddr = 0u;                                          // Dflt sync addr is zero.
  p_ep->SyncRefresh = 0u;                                       // Dflt feedback rate exponent is zero.

  CORE_ENTER_ATOMIC();
  ep_alloc_map = p_config->EP_AllocMap;                         // Get cfg EP alloc bit map.
  DEF_BIT_CLR(ep_alloc_map, p_if->EP_AllocMap);                 // Clr EP already alloc'd in the IF.
  DEF_BIT_SET(ep_alloc_map, p_if_alt->EP_AllocMap);

  ep_alloc_map_clr = ep_alloc_map;

  alloc = USBD_EP_Alloc(p_dev,                                  // Alloc physical EP.
                        dev_spd,
                        ep_type,
                        dir_in,
                        max_pkt_len & 0x7FF,                    // Mask out transactions per microframe.
                        if_alt_nbr,
                        p_ep,
                        &ep_alloc_map);
  if (alloc != DEF_OK) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_NONE_AVAIL);
    return (USBD_EP_NBR_NONE);
  }

  p_ep->MaxPktSize |= max_pkt_len & 0x1800;                     // Set transactions per microframe.

  ep_phy_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);
  ep_phy_nbr++;

  if (p_dev->EP_MaxPhyNbr < ep_phy_nbr) {
    p_dev->EP_MaxPhyNbr = ep_phy_nbr;
  }

  p_if_alt->EP_AllocMap |= ep_alloc_map & ~ep_alloc_map_clr;
  p_if->EP_AllocMap |= p_if_alt->EP_AllocMap;
  p_config->EP_AllocMap |= p_if->EP_AllocMap;

  p_if_alt->EP_NbrTotal++;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  ep_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);
  p_if_alt->EP_TblPtrs[ep_nbr] = p_ep;
  DEF_BIT_SET(p_if_alt->EP_TblMap, DEF_BIT32(ep_nbr));
#else
  p_ep->NextPtr = DEF_NULL;
  if (p_if_alt->EP_HeadPtr == DEF_NULL) {
    p_if_alt->EP_HeadPtr = p_ep;
    p_if_alt->EP_TailPtr = p_ep;
  } else {
    p_if_alt->EP_TailPtr->NextPtr = p_ep;
    p_if_alt->EP_TailPtr = p_ep;
  }
#endif
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_ep->Addr);
}

/****************************************************************************************************//**
 *                                               USBD_EP_Alloc()
 *
 * @brief    Allocate a physical endpoint from the device controller.
 *
 * @param    p_dev               Pointer to USB device.
 *
 * @param    -----            Argument validated in 'USBD_DevAdd()' & 'USBD_EP_Add()'
 *
 * @param    spd                 Endpoint speed.
 *                                   - USBD_DEV_SPD_FULL   Endpoint is full-speed.
 *                                   - USBD_DEV_SPD_HIGH   Endpoint is high-speed.
 *
 * @param    type                Endpoint type.
 *                                   - USBD_EP_TYPE_CTRL  Control endpoint.
 *                                   - USBD_EP_TYPE_ISOC  Isochronous endpoint.
 *                                   - USBD_EP_TYPE_BULK  Bulk endpoint.
 *                                   - USBD_EP_TYPE_INTR  Interrupt endpoint.
 *
 * @param    dir_in              Endpoint direction.
 *                                   - DEF_YES  IN  endpoint.
 *                                   - DEF_NO   OUT endpoint.
 *
 * @param    max_pkt_len         Endpoint maximum packet size length.
 *
 * @param    if_alt_nbr          Alternate interface number containing the endpoint.
 *
 * @param    p_ep                Pointer to the variable that will receive the endpoint parameters.
 *
 * @param    ----                Argument validated in 'USBD_DevAdd()' & 'USBD_EP_Add()'
 *
 * @param    p_alloc_bit_map     Pointer to allocation table bit-map.
 *
 * @param    ---------------     Argument validated in 'USBD_DevAdd()' & 'USBD_EP_Add()'
 *
 * @note     (1) 'Universal Serial Bus Specification, Revision 2.0, April 27, 2000' Section 5.5.3
 *               @n
 *               "An endpoint for control transfers specifies the maximum data payload size that
 *               the endpoint can accept from or transmit to the bus. The allowable maximum control
 *               transfer data payload sizes for full-speed devices is 8, 16, 32, or 64 bytes; for
 *               high-speed devices, it is 64 bytes and for low-speed devices, it is 8 bytes."
 *               @n
 *               "All Host Controllers are required to have support for 8-, 16-, 32-, and 64-byte
 *               maximum data payload sizes for full-speed control endpoints, only 8-byte maximum
 *               data payload sizes for low-speed control endpoints, and only 64-byte maximum data
 *               payload size for high-speed control endpoints"
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_EP_Alloc(USBD_DEV     *p_dev,
                                 USBD_DEV_SPD spd,
                                 CPU_INT08U   type,
                                 CPU_BOOLEAN  dir_in,
                                 CPU_INT16U   max_pkt_len,
                                 CPU_INT08U   if_alt_nbr,
                                 USBD_EP_INFO *p_ep,
                                 CPU_INT32U   *p_alloc_bit_map)
{
  USBD_DRV_EP_INFO *p_ep_tbl;
  USBD_DRV         *p_drv;
  CPU_INT08U       ep_tbl_ix;
  CPU_INT08U       ep_attrib;
  CPU_INT08U       ep_attrib_srch;
  CPU_INT08U       ep_max_pkt_bits;
  CPU_INT16U       ep_max_pkt;
  CPU_BOOLEAN      ep_found;

#if (USBD_CFG_HS_EN == DEF_DISABLED)
  (void)&spd;
  (void)&if_alt_nbr;
#endif

  if (dir_in == DEF_YES) {
    ep_attrib_srch = USBD_EP_INFO_DIR_IN;
  } else {
    ep_attrib_srch = USBD_EP_INFO_DIR_OUT;
  }

  switch (type) {
    case USBD_EP_TYPE_CTRL:
      DEF_BIT_SET(ep_attrib_srch, USBD_EP_INFO_TYPE_CTRL);
      break;

    case USBD_EP_TYPE_ISOC:
      DEF_BIT_SET(ep_attrib_srch, USBD_EP_INFO_TYPE_ISOC);
      break;

    case USBD_EP_TYPE_BULK:
      DEF_BIT_SET(ep_attrib_srch, USBD_EP_INFO_TYPE_BULK);
      break;

    case USBD_EP_TYPE_INTR:
      DEF_BIT_SET(ep_attrib_srch, USBD_EP_INFO_TYPE_INTR);
      break;

    default:
      return (DEF_FAIL);
  }

  p_drv = &p_dev->Drv;
  p_ep_tbl = p_drv->CfgPtr->EP_InfoTbl;                         // Get ctrl EP info tbl.
  ep_attrib = p_ep_tbl->Attrib;                                 // Get attrib for first entry.
  ep_tbl_ix = 0u;
  ep_found = DEF_NO;

  while ((ep_attrib != DEF_BIT_NONE)                            // Search until last entry or EP found.
         && (ep_found == DEF_NO)) {
    //                                                             Chk if EP not alloc'd and EP attrib match req'd ...
    //                                                             ... attrib.
    if ((DEF_BIT_IS_CLR(*p_alloc_bit_map, DEF_BIT32(ep_tbl_ix)) == DEF_YES)
        && (DEF_BIT_IS_SET(ep_attrib, ep_attrib_srch) == DEF_YES)) {
      ep_max_pkt = p_ep_tbl[ep_tbl_ix].MaxPktSize;

      switch (type) {
        case USBD_EP_TYPE_CTRL:                                 // Chk ctrl transfer pkt size constrains.
          ep_max_pkt = DEF_MIN(ep_max_pkt, 64u);
          //                                                       Get next power of 2.
          ep_max_pkt_bits = (CPU_INT08U)(31u - CPU_CntLeadZeros32(ep_max_pkt));
          ep_max_pkt = DEF_BIT16(ep_max_pkt_bits);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
          if ((spd == USBD_DEV_SPD_HIGH)
              && (ep_max_pkt != 64u)) {
            break;
          }

          if ((spd == USBD_DEV_SPD_HIGH)
              && (ep_max_pkt < 8u)) {
            break;
          }
#endif
          ep_found = DEF_YES;
          break;

        case USBD_EP_TYPE_BULK:
#if (USBD_CFG_HS_EN == DEF_ENABLED)
          //                                                       Max pkt size is 512 for bulk EP in HS.
          ep_max_pkt = DEF_MIN(ep_max_pkt, 512u);
          if ((spd == USBD_DEV_SPD_HIGH)
              && (ep_max_pkt == 512u)) {
            ep_found = DEF_YES;
            break;
          }
#endif
          //                                                       Max pkt size is 64 for bulk EP in FS.
          ep_max_pkt = DEF_MIN(ep_max_pkt, 64u);
          ep_max_pkt_bits = (CPU_INT08U)(31u - CPU_CntLeadZeros32(ep_max_pkt));
          ep_max_pkt = DEF_BIT16(ep_max_pkt_bits);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
          if ((spd == USBD_DEV_SPD_HIGH)
              && (ep_max_pkt >= 8u)) {
            break;
          }
#endif
          ep_found = DEF_YES;
          break;

        case USBD_EP_TYPE_ISOC:
#if (USBD_CFG_HS_EN == DEF_ENABLED)
          if (spd == USBD_DEV_SPD_HIGH) {
            ep_max_pkt = DEF_MIN(ep_max_pkt, 1024u);
          } else {
#endif
          ep_max_pkt = DEF_MIN(ep_max_pkt, 1023u);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
      }
#endif

          if (max_pkt_len > 0u) {
            ep_max_pkt = DEF_MIN(ep_max_pkt, max_pkt_len);
          }

          ep_found = DEF_YES;
          break;

        case USBD_EP_TYPE_INTR:
#if (USBD_CFG_HS_EN == DEF_ENABLED)
          if ((spd == USBD_DEV_SPD_HIGH)
              && (if_alt_nbr != 0u)) {                          // Dflt IF intr EP max pkt size limited to 64.
            ep_max_pkt = DEF_MIN(ep_max_pkt, 1024u);
          } else {
#endif
          ep_max_pkt = DEF_MIN(ep_max_pkt, 64u);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
      }
#endif
          if (max_pkt_len > 0u) {
            ep_max_pkt = DEF_MIN(ep_max_pkt, max_pkt_len);
          }

          ep_found = DEF_YES;
          break;

        default:
          return (DEF_FAIL);
      }

      if ((ep_found == DEF_YES)
          && ((max_pkt_len == ep_max_pkt)
              || (max_pkt_len == 0u))) {
        p_ep->MaxPktSize = ep_max_pkt;
        DEF_BIT_SET(*p_alloc_bit_map, DEF_BIT32(ep_tbl_ix));
        p_ep->Addr = p_ep_tbl[ep_tbl_ix].Nbr;
        if (dir_in == DEF_TRUE) {
          p_ep->Addr |= USBD_EP_DIR_IN;                         // Add dir bit (IN EP).
        }
      } else {
        ep_found = DEF_NO;
        ep_tbl_ix++;
        ep_attrib = p_ep_tbl[ep_tbl_ix].Attrib;
      }
    } else {
      ep_tbl_ix++;
      ep_attrib = p_ep_tbl[ep_tbl_ix].Attrib;
    }
  }

  if (ep_found == DEF_NO) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBD_EP_MaxPhyNbrGet()
 *
 * @brief    Gets the maximum physical endpoint number.
 *
 * @param    dev_nbr     Device number.
 *
 * @return   Maximum physical endpoint number, if no errors are returned.
 *           USBD_EP_PHY_NONE,                 if any errors are returned.
 *******************************************************************************************************/
CPU_INT08U USBD_EP_MaxPhyNbrGet(CPU_INT08U dev_nbr)
{
  USBD_DEV *p_dev;
  CPU_INT08U ep_phy_nbr;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.

  if (p_dev == DEF_NULL) {
    return (USBD_EP_PHY_NONE);
  }

  if (p_dev->EP_MaxPhyNbr == 0u) {
    ep_phy_nbr = USBD_EP_PHY_NONE;
  } else {
    ep_phy_nbr = p_dev->EP_MaxPhyNbr - 1u;
  }

  return (ep_phy_nbr);
}

/****************************************************************************************************//**
 *                                               USBD_EventConn()
 *
 * @brief    Notifies the USB connection bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventConn(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_CONN);
}

/****************************************************************************************************//**
 *                                           USBD_EventDisconn()
 *
 * @brief    Notifies the USB disconnection bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventDisconn(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_DISCONN);
}

/****************************************************************************************************//**
 *                                               USBD_EventHS()
 *
 * @brief    Notifies the USB High-Speed bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventHS(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_HS);
}

/****************************************************************************************************//**
 *                                               USBD_EventReset()
 *
 * @brief    Notifies the USB reset bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventReset(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_RESET);
}

/****************************************************************************************************//**
 *                                           USBD_EventSuspend()
 *
 * @brief    Notifies the USB suspend bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventSuspend(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_SUSPEND);
}

/****************************************************************************************************//**
 *                                           USBD_EventResume()
 *
 * @brief    Notifies the USB resume bus events to the device stack.
 *
 * @param    p_drv   Pointer to device driver.
 *******************************************************************************************************/
void USBD_EventResume(USBD_DRV *p_drv)
{
  USBD_EventSet(p_drv, USBD_EVENT_BUS_RESUME);
}

/****************************************************************************************************//**
 *                                               USBD_EventSetup()
 *
 * @brief    Sends a USB setup event to the core task.
 *
 * @param    p_drv   Pointer to device driver.
 *
 * @param    p_buf   Pointer to the setup packet.
 *******************************************************************************************************/
void USBD_EventSetup(USBD_DRV *p_drv,
                     void     *p_buf)
{
  USBD_DEV *p_dev;
  USBD_CORE_EVENT *p_core_event;
  CPU_INT08U *p_buf_08;

  RTOS_ASSERT_DBG((p_buf != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_dev = USBD_DevRefGet(p_drv->DevNbr);                        // Get dev struct.
  if (p_dev == DEF_NULL) {
    return;
  }

  p_core_event = USBD_CoreEventGet();                           // Get core event struct.
  if (p_core_event == DEF_NULL) {
    return;
  }

  LOG_VRB(("USBD: Setup Pkt"));

  p_buf_08 = (CPU_INT08U *)p_buf;
  p_dev->SetupReqNext.bmRequestType = p_buf_08[0u];
  p_dev->SetupReqNext.bRequest = p_buf_08[1u];
  p_dev->SetupReqNext.wValue = MEM_VAL_GET_INT16U_LITTLE(p_buf_08 + 2u);
  p_dev->SetupReqNext.wIndex = MEM_VAL_GET_INT16U_LITTLE(p_buf_08 + 4u);
  p_dev->SetupReqNext.wLength = MEM_VAL_GET_INT16U_LITTLE(p_buf_08 + 6u);

  p_core_event->Type = USBD_EVENT_SETUP;
  p_core_event->DrvPtr = p_drv;
  RTOS_ERR_SET(p_core_event->Err, RTOS_ERR_NONE);

  USBD_OS_CoreEventPut(p_drv->DevNbr,
                       p_core_event);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_EventEP()
 *
 * @brief    Send a USB endpoint event to the core task.
 *
 * @param    p_drv       Pointer to device driver.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    err         Error code returned by the USB device driver.
 *******************************************************************************************************/
void USBD_EventEP(USBD_DRV   *p_drv,
                  CPU_INT08U ep_addr,
                  RTOS_ERR   err)
{
  USBD_CORE_EVENT *p_core_event;

  RTOS_ASSERT_DBG((p_drv != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_core_event = USBD_CoreEventGet();                           // Get core event struct.
  if (p_core_event == DEF_NULL) {
    return;
  }

  p_core_event->Type = USBD_EVENT_EP;
  p_core_event->DrvPtr = p_drv;
  p_core_event->EP_Addr = ep_addr;
  RTOS_ERR_COPY(p_core_event->Err, err);

  USBD_OS_CoreEventPut(p_drv->DevNbr, p_core_event);            // Queue core event.
}

/****************************************************************************************************//**
 *                                               USBD_DrvRefGet()
 *
 * @brief    Get a reference to the device driver structure.
 *
 * @param    dev_nbr     Device number
 *
 * @return   Pointer to device driver structure, if no errors are returned.
 *           Pointer to NULL                   , if any errors are returned.
 *******************************************************************************************************/
USBD_DRV *USBD_DrvRefGet(CPU_INT08U dev_nbr)
{
  USBD_DEV *p_dev;
  USBD_DRV *p_drv;

  p_dev = USBD_DevRefGet(dev_nbr);                              // Get dev struct.
  if (p_dev == DEF_NULL) {
    return (DEF_NULL);
  }

  p_drv = &p_dev->Drv;

  return (p_drv);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_StdReqHandler()
 *
 * @brief    Standard request process.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *******************************************************************************************************/
static void USBD_StdReqHandler(USBD_DEV *p_dev)
{
  CPU_INT08U recipient;
  CPU_INT08U type;
  CPU_INT08U request;
  CPU_BOOLEAN valid;
  CPU_BOOLEAN dev_to_host;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();                                          // Copy setup request.
  p_dev->SetupReq.bmRequestType = p_dev->SetupReqNext.bmRequestType;
  p_dev->SetupReq.bRequest = p_dev->SetupReqNext.bRequest;
  p_dev->SetupReq.wValue = p_dev->SetupReqNext.wValue;
  p_dev->SetupReq.wIndex = p_dev->SetupReqNext.wIndex;
  p_dev->SetupReq.wLength = p_dev->SetupReqNext.wLength;
  CORE_EXIT_ATOMIC();

  recipient = p_dev->SetupReq.bmRequestType & USBD_REQ_RECIPIENT_MASK;
  type = p_dev->SetupReq.bmRequestType & USBD_REQ_TYPE_MASK;
  request = p_dev->SetupReq.bRequest;
  dev_to_host = DEF_BIT_IS_SET(p_dev->SetupReq.bmRequestType, USBD_REQ_DIR_BIT);
  valid = DEF_FAIL;

  switch (type) {
    case USBD_REQ_TYPE_STANDARD:
      switch (recipient) {                                      // Select req recipient:
        case USBD_REQ_RECIPIENT_DEVICE:                         // Device.
          valid = USBD_StdReqDev(p_dev, request);
          break;

        case USBD_REQ_RECIPIENT_INTERFACE:                      // Interface.
          valid = USBD_StdReqIF(p_dev, request);
          break;

        case USBD_REQ_RECIPIENT_ENDPOINT:                       // Endpoint.
          valid = USBD_StdReqEP(p_dev, request);
          break;

        case USBD_REQ_RECIPIENT_OTHER:                          // Not supported.
        default:
          break;
      }
      break;

    case USBD_REQ_TYPE_CLASS:                                   // Class-specific req.
      switch (recipient) {
        case USBD_REQ_RECIPIENT_INTERFACE:
        case USBD_REQ_RECIPIENT_ENDPOINT:
          valid = USBD_StdReqClass(p_dev);                      // Class-specific req.
          break;

        case USBD_REQ_RECIPIENT_DEVICE:
        case USBD_REQ_RECIPIENT_OTHER:
        default:
          break;
      }
      break;

    case USBD_REQ_TYPE_VENDOR:
      switch (recipient) {
        case USBD_REQ_RECIPIENT_INTERFACE:
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
          if (request == p_dev->StrMS_VendorCode) {
            RTOS_ERR_SET(err, RTOS_ERR_NONE);
            p_dev->DescBufErrPtr = &err;
            valid = USBD_StdReqIF_MS(p_dev);                    // Microsoft OS descriptor req.
            p_dev->DescBufErrPtr = DEF_NULL;
          } else {
            valid = USBD_StdReqVendor(p_dev);                   // Vendor-specific req.
          }
          break;
#endif

        case USBD_REQ_RECIPIENT_ENDPOINT:
          valid = USBD_StdReqVendor(p_dev);                     // Vendor-specific req.
          break;

        case USBD_REQ_RECIPIENT_DEVICE:
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
          if (request == p_dev->StrMS_VendorCode) {
            RTOS_ERR_SET(err, RTOS_ERR_NONE);
            p_dev->DescBufErrPtr = &err;
            valid = USBD_StdReqDevMS(p_dev);                    // Microsoft OS descriptor req.
            p_dev->DescBufErrPtr = DEF_NULL;
          }
#endif
          break;

        case USBD_REQ_RECIPIENT_OTHER:
        default:
          break;
      }
      break;

    case USBD_REQ_TYPE_RESERVED:
    default:
      break;
  }

  if (valid == DEF_FAIL) {
    LOG_DBG(("USBD: Request Error"));
    USBD_CtrlStall(p_dev->Nbr, &err);
  } else {
    CPU_INT32U std_req_timeout;

    CORE_ENTER_ATOMIC();
    std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
    CORE_EXIT_ATOMIC();

    if (dev_to_host == DEF_YES) {
      LOG_VRB(("USBD: Rx Status"));
      USBD_CtrlRxStatus(p_dev->Nbr, std_req_timeout, &err);
    } else {
      LOG_VRB(("USBD: Tx Status"));
      USBD_CtrlTxStatus(p_dev->Nbr, std_req_timeout, &err);

      if ((type == USBD_REQ_TYPE_STANDARD)
          && (recipient == USBD_REQ_RECIPIENT_DEVICE)
          && (request == USBD_REQ_SET_ADDRESS)
          && (p_dev->Drv.API_Ptr->AddrEn != DEF_NULL)) {
        p_dev->Drv.API_Ptr->AddrEn(&p_dev->Drv, p_dev->Addr);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                               USBD_StdReqDev()
 *
 * @brief    Process device standard request.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----    Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) USB Spec 2.0, section 9.4.6 specifies the format of the SET_ADDRESS request. The
 *               SET_ADDRESS sets the device address for all future device access.
 *           - (a) The 'wValue' filed specify the device address to use for all subsequent accesses.
 *           - (b) If the specified device address is greater than 127 or if 'wIndex' or 'wLength'
 *                   are non-zero, the behavior of the device is not specified.
 *           - (c) If the device is in the default state and the address specified is non-zero,
 *                   the device shall enter the device address, otherwise the device remains in the
 *                   default state' (this is not an error condition).
 *           - (d) If the device is in the address state and the address specified is zero, then
 *                   the device shall enter the default state otherwise, the device remains in
 *                   the address state but uses the newly-specified address.
 *           - (e) Device behavior when the SET_ADDRESS request is received while the device is not
 *                   in the default or address state is not specified.
 *           - (f) USB Spec 2.0, section 9.2.6.3 specifies the maximum timeout for the SET_ADDRESS
 *                   request:
 *                   "After the reset/resume recovery interval, if a device receives a SetAddress()
 *                   request, the device must be able to complete processing of the request and be
 *                   able to successfully complete the Status stage of the request within 50 ms. In
 *                   the case of the SetAddress() request, the Status stage successfully completes
 *                   when the device sends the zero-length Status packet or when the device sees
 *                   the ACK in response to the Status stage data packet."
 *
 * @note     (2) USB Spec 2.0, section 9.4.7 specifies the format of the SET_CONFIGURATION request.
 *           - (a) The lower byte of 'wValue' field specifies the desired configuration.
 *           - (b) If 'wIndex', 'wLength', or the upper byte of wValue is non-zero, then the behavior
 *                   of this request is not specified.
 *           - (c) The configuration value must be zero or match a configuration value from a
 *                   configuration value from a configuration descriptor. If the configuration value
 *                   is zero, the device is place in its address state.
 *           - (d) Device behavior when this request is received while the device is in the Default
 *                   state is not specified.
 *           - (e) If device is in address state and the specified configuration value is zero,
 *                   then the device remains in the Address state. If the specified configuration value
 *                   matches the configuration value from a configuration descriptor, then that
 *                   configuration is selected and the device enters the Configured state. Otherwise,
 *                   the device responds with a Request Error.
 *           - (f) If the specified configuration value is zero, then the device enters the Address
 *                   state. If the specified configuration value matches the configuration value from a
 *                   configuration descriptor, then that configuration is selected and the device
 *                   remains in the Configured state. Otherwise, the device responds with a Request
 *                   Error.
 *
 * @note     (3) USB Spec 2.0, section 9.4.2 specifies the format of the GET_CONFIGURATION request.
 *           - (a) If 'wValue' or 'wIndex' are non-zero or 'wLength' is not '1', then the device
 *                   behavior is not specified.
 *           - (b) If the device is in default state, the device behavior is not specified.
 *           - (c) In address state a value of zero MUST be returned.
 *           - (d) In configured state, the non-zero bConfigurationValue of the current configuration
 *                   must be returned.
 *
 * @note     (4) USB Spec 2.0, section 9.4.5 specifies the format of the GET_STATUS request.
 *           - (a) If 'wValue' is non-zero or 'wLength is not equal to '2', or if wIndex is non-zero
 *                   then the behavior of the device is not specified.
 *           - (b) USB Spec 2, 0, figure 9-4 shows the format of information returned by the device
 *                   for a GET_STATUS request.
 *                   @verbatim
 *                   +====|====|====|====|====|====|====|========|=========+
 *                   | D0 | D1 | D2 | D3 | D4 | D3 | D2 |   D1   |    D0   |
 *                   |----------------------------------|--------|---------|
 *                   |     RESERVED (RESET TO ZERO)     | Remote |   Self  |
 *                   |                                  | Wakeup | Powered |
 *                   +==================================|========|=========+
 *                   @endverbatim
 *               - (1) The Self Powered field indicates whether the device is currently self-powered.
 *                       If D0 is reset to zero, the device is bus-powered. If D0 is set to one, the
 *                       device is self-powered. The Self Powered field may not be changed by the
 *                       SetFeature() or ClearFeature() requests.
 *               - (2) The Remote Wakeup field indicates whether the device is currently enabled to
 *                       request remote wakeup. The default mode for devices that support remote wakeup
 *                       is disabled. If D1 is reset to zero, the ability of the device to signal
 *                       remote wakeup is disabled. If D1 is set to one, the ability of the device to
 *                       signal remote wakeup is enabled. The Remote Wakeup field can be modified by
 *                       the SetFeature() and ClearFeature() requests using the DEVICE_REMOTE_WAKEUP
 *                       feature selector. This field is reset to zero when the device is reset.
 *
 * @note     (5) USB Spec 2.0, section 9.4.1/9.4.9 specifies the format of the CLEAR_FEATURE/SET_FEATURE
 *               request.
 *           - (a) If 'wLength' or 'wIndex' are non-zero, then the device behavior is not specified.
 *           - (b) The device CLEAR_FEATURE request is only valid when the device is in the
 *                   configured state.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqDev(USBD_DEV   *p_dev,
                                  CPU_INT08U request)
{
  CPU_BOOLEAN valid;
  CPU_BOOLEAN addr_set;
  CPU_BOOLEAN dev_to_host;
  CPU_INT08U dev_addr;
  CPU_INT08U cfg_nbr;
  CPU_INT32U std_req_timeout;
  USBD_DRV *p_drv;
  USBD_DRV_API *p_drv_api;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqDevNbr);

  dev_to_host = DEF_BIT_IS_SET(p_dev->SetupReq.bmRequestType, USBD_REQ_DIR_BIT);
  valid = DEF_FAIL;

  switch (request) {
    case USBD_REQ_GET_DESCRIPTOR:                               // ------------------ GET DESCRIPTOR ------------------
      if (dev_to_host != DEF_YES) {
        break;
      }

      valid = USBD_StdReqDescGet(p_dev);
      break;

    case USBD_REQ_SET_ADDRESS:                                  // -------------------- SET ADDRESS -------------------
      if (dev_to_host != DEF_NO) {
        break;
      }

      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqSetAddrNbr);

      dev_addr = (CPU_INT08U)(p_dev->SetupReq.wValue            // Get dev addr (see Note #1a).
                              & DEF_INT_08_MASK);

      LOG_VRB(("USBD: Set Address to addr #", (u)dev_addr));

      if ((dev_addr > 127u)                                     // Validate request values. (see Note #1b).
          || (p_dev->SetupReq.wIndex != 0u)
          || (p_dev->SetupReq.wLength != 0u)) {
        break;
      }

      p_drv = &p_dev->Drv;
      p_drv_api = p_dev->Drv.API_Ptr;
      switch (p_dev->State) {
        case USBD_DEV_STATE_DEFAULT:
          if (dev_addr > 0u) {                                  // See Note #1c.
            if (p_drv_api->AddrSet != DEF_NULL) {
              addr_set = p_drv_api->AddrSet(p_drv, dev_addr);
              if (addr_set == DEF_FAIL) {
                LOG_ERR(("USBD Set Address Failed"));
                break;
              }
            }

            CORE_ENTER_ATOMIC();                                // Set dev in addressed state.
            p_dev->Addr = dev_addr;
            p_dev->State = USBD_DEV_STATE_ADDRESSED;
            CORE_EXIT_ATOMIC();

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_ADDRESSED:                          // See Note #1c.
          if (dev_addr == 0u) {                                 // If dev addr is zero ...
                                                                // ... set addr in dev drv.
            if (p_drv_api->AddrSet != DEF_NULL) {
              addr_set = p_drv_api->AddrSet(p_drv, 0u);
              if (addr_set == DEF_FAIL) {
                LOG_ERR(("USBD Set Address Failed"));
                break;
              }
            }

            CORE_ENTER_ATOMIC();                                // Dev enters default state.
            p_dev->Addr = 0u;
            p_dev->State = USBD_DEV_STATE_DEFAULT;
            CORE_EXIT_ATOMIC();

            valid = DEF_OK;
          } else {
            //                                                     ... remains in addressed state and set new addr.
            if (p_drv_api->AddrSet != DEF_NULL) {
              addr_set = p_drv_api->AddrSet(p_drv, dev_addr);
              if (addr_set == DEF_FAIL) {
                LOG_ERR(("USBD Set Address Failed"));
                break;
              }
            }

            CORE_ENTER_ATOMIC();
            p_dev->Addr = dev_addr;
            CORE_EXIT_ATOMIC();

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_CONFIGURED:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          LOG_ERR(("USBD Set Address Failed due to invalid device state."));
          break;
      }
      break;

    case USBD_REQ_SET_CONFIGURATION:                            // ----------------- SET CONFIGURATION ----------------
      if (dev_to_host != DEF_NO) {
        break;
      }

      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqSetCfgNbr);

      if (((p_dev->SetupReq.wValue & 0xFF00u) != 0u)            // Validate request values (see Note #2b).
          && (p_dev->SetupReq.wIndex != 0u)
          && (p_dev->SetupReq.wLength != 0u)) {
        break;
      }
      //                                                           Get cfg value.
      cfg_nbr = (CPU_INT08U)(p_dev->SetupReq.wValue & DEF_INT_08_MASK);
      LOG_VRB(("USBD: Set Configuration #", (u)cfg_nbr));

#if (USBD_CFG_HS_EN == DEF_ENABLED)
      //                                                           Cfg value MUST exists.
      if ((cfg_nbr > p_dev->ConfigHS_TotalNbr)
          && (p_dev->Spd == USBD_DEV_SPD_HIGH)) {
        LOG_ERR(("  Set Configuration Cfg Invalid Nbr"));
        break;
      }
#endif

      if ((cfg_nbr > p_dev->ConfigFS_TotalNbr)
          && (p_dev->Spd == USBD_DEV_SPD_FULL)) {
        LOG_ERR(("  Set Configuration Cfg Invalid Nbr"));
        break;
      }

      switch (p_dev->State) {
        case USBD_DEV_STATE_ADDRESSED:                          // See Note #2e.
          if (cfg_nbr > 0u) {
            USBD_ConfigOpen(p_dev,                              // Open cfg.
                            (cfg_nbr - 1u),
                            &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
              LOG_ERR(("USBD Set Configuration Failed. Err = ", RTOS_ERR_LOG_ARG_GET(err)));
              break;
            }

            valid = DEF_OK;
          } else {
            valid = DEF_OK;                                     // Remain in addressed state.
          }
          break;

        case USBD_DEV_STATE_CONFIGURED:                         // See Note #2f.
          if (cfg_nbr > 0u) {
            if (p_dev->ConfigCurNbr == (cfg_nbr - 1u)) {
              valid = DEF_OK;
              break;
            }

            USBD_ConfigClose(p_dev);                            // Close curr  cfg.

            USBD_ConfigOpen(p_dev,                              // Open cfg.
                            (cfg_nbr - 1u),
                            &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
              LOG_ERR(("USBD Set Configuration Failed. Err = ", RTOS_ERR_LOG_ARG_GET(err)));
              break;
            }

            valid = DEF_OK;
          } else {
            USBD_ConfigClose(p_dev);                            // Close curr cfg.

            CORE_ENTER_ATOMIC();
            p_dev->State = USBD_DEV_STATE_ADDRESSED;
            CORE_EXIT_ATOMIC();

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          LOG_ERR(("USBD Set Configuration Failed due to invalid device state."));
          break;
      }
      break;

    case USBD_REQ_GET_CONFIGURATION:                            // ---------------- GET CONFIGURATION -----------------
      if (dev_to_host != DEF_YES) {
        break;
      }

      if ((p_dev->SetupReq.wLength != 1u)                       // Validate request values (see Note #3a).
          && (p_dev->SetupReq.wIndex != 0u)
          && (p_dev->SetupReq.wValue != 0u)) {
        break;
      }

      switch (p_dev->State) {
        case USBD_DEV_STATE_ADDRESSED:                          // See Note #3b.
          cfg_nbr = 0u;
          LOG_VRB(("USBD: Get Configuration #", (u)cfg_nbr));

          p_dev->CtrlStatusBufPtr[0u] = cfg_nbr;                // Uses Ctrl status buf to follow USB mem alignment.

          (void)USBD_CtrlTx(p_dev->Nbr,
                            (void *)&p_dev->CtrlStatusBufPtr[0u],
                            1u,
                            std_req_timeout,
                            DEF_NO,
                            &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_CONFIGURED:                         // See Note #3c.
          if (p_dev->ConfigCurPtr == DEF_NULL) {
            break;
          }

          cfg_nbr = p_dev->ConfigCurNbr + 1u;
          LOG_VRB(("USBD: Get Configuration #", (u)cfg_nbr));

          p_dev->CtrlStatusBufPtr[0u] = cfg_nbr;                // Uses Ctrl status buf to follow USB mem alignment.

          (void)USBD_CtrlTx(p_dev->Nbr,
                            (void *)&p_dev->CtrlStatusBufPtr[0u],
                            1u,
                            std_req_timeout,
                            DEF_NO,
                            &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          LOG_ERR(("USBD Get Configuration Failed due to invalid device state."));
          break;
      }
      break;

    case USBD_REQ_GET_STATUS:                                   // -------------------- GET STATUS --------------------
      if (dev_to_host != DEF_YES) {
        break;
      }

      if ((p_dev->SetupReq.wLength != 2u)                       // Validate request values (see Note #4a).
          && (p_dev->SetupReq.wIndex != 0u)
          && (p_dev->SetupReq.wValue != 0u)) {
        break;
      }

      LOG_VRB(("USBD: Get Status (Device)"));
      p_dev->CtrlStatusBufPtr[0u] = DEF_BIT_NONE;
      p_dev->CtrlStatusBufPtr[1u] = DEF_BIT_NONE;

      switch (p_dev->State) {
        case USBD_DEV_STATE_ADDRESSED:                          // See Note #4b.
          if (p_dev->SelfPwr == DEF_YES) {
            p_dev->CtrlStatusBufPtr[0u] |= DEF_BIT_00;
          }
          if (p_dev->RemoteWakeup == DEF_ENABLED) {
            p_dev->CtrlStatusBufPtr[0u] |= DEF_BIT_01;
          }

          (void)USBD_CtrlTx(p_dev->Nbr,
                            (void *)&p_dev->CtrlStatusBufPtr[0u],
                            2u,
                            std_req_timeout,
                            DEF_NO,
                            &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_CONFIGURED:
          if (p_dev->ConfigCurPtr != DEF_NULL) {
            if (DEF_BIT_IS_SET(p_dev->ConfigCurPtr->Attrib, USBD_DEV_ATTRIB_SELF_POWERED)) {
              p_dev->CtrlStatusBufPtr[0u] |= DEF_BIT_00;
            }
            if (DEF_BIT_IS_SET(p_dev->ConfigCurPtr->Attrib, USBD_DEV_ATTRIB_REMOTE_WAKEUP)) {
              p_dev->CtrlStatusBufPtr[0u] |= DEF_BIT_01;
            }
          }

          (void)USBD_CtrlTx(p_dev->Nbr,
                            (void *)&p_dev->CtrlStatusBufPtr[0u],
                            2u,
                            std_req_timeout,
                            DEF_NO,
                            &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          LOG_ERR(("USBD Get Status (Device) Failed due to invalid device state."));
          break;
      }
      break;

    case USBD_REQ_CLEAR_FEATURE:                                // ----------------- SET/CLEAR FEATURE ----------------
    case USBD_REQ_SET_FEATURE:
      if (dev_to_host != DEF_NO) {
        break;
      }

      if ((p_dev->SetupReq.wLength != 0u)                       // Validate request values.
          && (p_dev->SetupReq.wIndex != 0u)) {
        break;
      }

      if (request == USBD_REQ_CLEAR_FEATURE) {
        LOG_VRB(("USBD: Clear Feature (Device)"));
      } else {
        LOG_VRB(("USBD: Set Feature (Device)"));
      }

      switch (p_dev->State) {
        case USBD_DEV_STATE_CONFIGURED:
          if (p_dev->ConfigCurPtr == DEF_NULL) {
            break;
          }

          if ((p_dev->SetupReq.wValue == USBD_FEATURE_SEL_DEVICE_REMOTE_WAKEUP)
              && (DEF_BIT_IS_SET(p_dev->ConfigCurPtr->Attrib, USBD_DEV_ATTRIB_REMOTE_WAKEUP))) {
            p_dev->RemoteWakeup = (request == USBD_REQ_CLEAR_FEATURE) ? DEF_DISABLED : DEF_ENABLED;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_ADDRESSED:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          if (request == USBD_REQ_CLEAR_FEATURE) {
            LOG_ERR(("USBD Clear Feature (Device) Failed due to invalid device state."));
          } else {
            LOG_ERR(("USBD Set Feature (Device) Failed due to invalid device state."));
          }
          break;
      }
      break;

    default:
      break;
  }

  USBD_DBG_STATS_DEV_INC_IF_TRUE(p_dev->Nbr, StdReqDevStallNbr, (valid == DEF_FAIL));

  return (valid);
}

/****************************************************************************************************//**
 *                                               USBD_StdReqIF()
 *
 * @brief    Process device standard request (Interface).
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) USB Spec 2.0, section 9.4.10 specifies the format of the SET_INTERFACE request.
 *               This request allows the host to select an alternate setting for the specified
 *               interface:
 *           - (a) Some USB devices have configurations with interfaces that have mutually
 *                   exclusive settings.  This request allows the host to select the desired
 *                   alternate setting.  If a device only supports a default setting for the
 *                   specified interface, then a STALL may be returned in the Status stage of
 *                   the request. This request cannot be used to change the set of configured
 *                   interfaces (the SetConfiguration() request must be used instead).
 *           - (2) USB Spec 2.0, section 9.4.4 specifies the format of the GET_INTERFACE request.
 *               This request returns the selected alternate setting for the specified interface.
 *           - (a) If 'wValue' is non-zero or 'wLength' is not '1', then the device behavior is
 *                   not specified.
 *           - (b) The GET_INTERFACE request is only valid when the device is in the configured
 *                   state.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqIF(USBD_DEV   *p_dev,
                                 CPU_INT08U request)
{
  CPU_INT32U std_req_timeout;
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_IF_ALT *p_if_alt;
  USBD_CLASS_DRV *p_class_drv;
  CPU_INT08U if_nbr;
  CPU_INT08U if_alt_nbr;
  CPU_BOOLEAN valid;
  CPU_BOOLEAN dev_to_host;
  CPU_INT16U req_len;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqIF_Nbr);

  p_config = p_dev->ConfigCurPtr;
  if (p_config == DEF_NULL) {
    USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqIF_StallNbr);
    return (DEF_FAIL);
  }

  if_nbr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
  p_if = USBD_IF_RefGet(p_config, if_nbr);
  if (p_if == DEF_NULL) {
    USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqIF_StallNbr);
    return (DEF_FAIL);
  }

  dev_to_host = DEF_BIT_IS_SET(p_dev->SetupReq.bmRequestType, USBD_REQ_DIR_BIT);
  valid = DEF_FAIL;

  switch (request) {
    case USBD_REQ_GET_STATUS:
      if (dev_to_host != DEF_YES) {
        break;
      }

      LOG_VRB(("USBD: Get Status (Interface) IF ", (u)if_nbr));

      if ((p_dev->State != USBD_DEV_STATE_ADDRESSED)
          && (p_dev->State != USBD_DEV_STATE_CONFIGURED)) {
        break;
      }

      if ((p_dev->State == USBD_DEV_STATE_ADDRESSED)
          && (if_nbr != 0u)) {
        break;
      }

      p_dev->CtrlStatusBufPtr[0u] = DEF_BIT_NONE;

      (void)USBD_CtrlTx(p_dev->Nbr,
                        (void *)&p_dev->CtrlStatusBufPtr[0u],
                        1u,
                        std_req_timeout,
                        DEF_NO,
                        &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        break;
      }

      valid = DEF_OK;
      break;

    case USBD_REQ_CLEAR_FEATURE:
    case USBD_REQ_SET_FEATURE:
      if (dev_to_host != DEF_NO) {
        break;
      }

      if (request == USBD_REQ_CLEAR_FEATURE) {
        LOG_VRB(("USBD: Clear Feature (Interface) IF ", (u)if_nbr));
      } else {
        LOG_VRB(("USBD: Set Feature (Interface) IF ", (u)if_nbr));
      }

      if ((p_dev->State != USBD_DEV_STATE_ADDRESSED)
          && (p_dev->State != USBD_DEV_STATE_CONFIGURED)) {
        break;
      }

      if ((p_dev->State == USBD_DEV_STATE_ADDRESSED)
          && (if_nbr != 0u)) {
        break;
      }

      valid = DEF_OK;
      break;

    case USBD_REQ_GET_DESCRIPTOR:
      if (dev_to_host != DEF_YES) {
        break;
      }

      LOG_VRB(("USBD: Get Descriptor (Interface) IF ", (u)if_nbr));

      p_class_drv = p_if->ClassDrvPtr;
      if (p_class_drv->IF_Req == DEF_NULL) {
        break;
      }

      req_len = p_dev->SetupReq.wLength;
      USBD_DescWrStart(p_dev, req_len);

      RTOS_ERR_SET(err, RTOS_ERR_NONE);
      p_dev->DescBufErrPtr = &err;

      valid = p_class_drv->IF_Req(p_dev->Nbr,
                                  &p_dev->SetupReq,
                                  p_if->ClassArgPtr);
      if (valid == DEF_OK) {
        USBD_DescWrStop(p_dev, &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          valid = DEF_FAIL;
        }
      }
      p_dev->DescBufErrPtr = DEF_NULL;
      break;

    case USBD_REQ_GET_INTERFACE:
      if (dev_to_host != DEF_YES) {
        break;
      }

      if (p_dev->State != USBD_DEV_STATE_CONFIGURED) {
        break;
      }

      p_dev->CtrlStatusBufPtr[0u] = p_if->AltCur;

      LOG_VRB(("USBD: Get Interface IF ", (u)if_nbr, " Alt ", (u)p_if->AltCur));

      (void)USBD_CtrlTx(p_dev->Nbr,
                        (void *)&p_dev->CtrlStatusBufPtr[0u],
                        1u,
                        std_req_timeout,
                        DEF_NO,
                        &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        break;
      }

      valid = DEF_OK;
      break;

    case USBD_REQ_SET_INTERFACE:
      if (dev_to_host != DEF_NO) {
        break;
      }

      if (p_dev->State != USBD_DEV_STATE_CONFIGURED) {
        break;
      }
      //                                                           Get IF alt setting nbr.
      if_alt_nbr = (CPU_INT08U)(p_dev->SetupReq.wValue  & DEF_INT_08_MASK);
      p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);

      LOG_VRB(("USBD: Set Interface IF ", (u)if_nbr, " Alt ", (u)if_alt_nbr));

      if (p_if_alt == DEF_NULL) {
        LOG_ERR(("USBD: Set Interface Invalid Alt IF"));
        break;
      }

      if (p_if_alt == p_if->AltCurPtr) {                        // If alt setting is the same as the cur one,...
        valid = DEF_OK;                                         // ...no further processing is needed.
        break;
      }

      USBD_IF_AltClose(p_dev, p_if->AltCurPtr);                 // Close the cur alt setting.

      USBD_IF_AltOpen(p_dev, if_nbr, p_if_alt, &err);           // Open the new alt setting.
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {            // Re-open curr IF alt setting, in case it fails.
        USBD_IF_AltOpen(p_dev, p_if->AltCur, p_if->AltCurPtr, &err);
        break;
      }

      CORE_ENTER_ATOMIC();                                      // Set IF alt setting.
      p_if->AltCurPtr = p_if_alt;
      p_if->AltCur = if_alt_nbr;
      CORE_EXIT_ATOMIC();
      //                                                           Notify class that IF or alt IF has been updated.
      if (p_if->ClassDrvPtr->AltSettingUpdate != DEF_NULL) {
        p_if->ClassDrvPtr->AltSettingUpdate(p_dev->Nbr,
                                            p_dev->ConfigCurNbr,
                                            if_nbr,
                                            if_alt_nbr,
                                            p_if->ClassArgPtr,
                                            p_if_alt->ClassArgPtr);
      }

      valid = DEF_OK;
      break;

    default:
      p_class_drv = p_if->ClassDrvPtr;
      if (p_class_drv->IF_Req == DEF_NULL) {
        break;
      }

      valid = p_class_drv->IF_Req(p_dev->Nbr,
                                  &p_dev->SetupReq,
                                  p_if->ClassArgPtr);
      break;
  }

  USBD_DBG_STATS_DEV_INC_IF_TRUE(p_dev->Nbr, StdReqIF_StallNbr, (valid == DEF_FAIL));

  return (valid);
}

/****************************************************************************************************//**
 *                                               USBD_StdReqEP()
 *
 * @brief    Process device standard request (Endpoint).
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqEP(const USBD_DEV *p_dev,
                                 CPU_INT08U     request)
{
  CPU_INT32U std_req_timeout;
  USBD_IF *p_if;
  USBD_IF_ALT *p_alt_if;
  CPU_BOOLEAN ep_is_stall;
  CPU_INT08U if_nbr;
  CPU_INT08U ep_addr;
  CPU_INT08U ep_phy_nbr;
  CPU_BOOLEAN valid;
  CPU_BOOLEAN dev_to_host;
  CPU_INT08U feature;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqEP_Nbr);

  ep_addr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
  feature = (CPU_INT08U)(p_dev->SetupReq.wValue & DEF_INT_08_MASK);
  dev_to_host = DEF_BIT_IS_SET(p_dev->SetupReq.bmRequestType, USBD_REQ_DIR_BIT);
  valid = DEF_FAIL;

  switch (request) {
    case USBD_REQ_CLEAR_FEATURE:
    case USBD_REQ_SET_FEATURE:
      if (dev_to_host != DEF_NO) {
        break;
      }

      switch (p_dev->State) {
        case USBD_DEV_STATE_ADDRESSED:
          if (((ep_addr == 0x80u)
               || (ep_addr == 0x00u))
              && (feature == USBD_FEATURE_SEL_ENDPOINT_HALT)) {
            if (request == USBD_REQ_CLEAR_FEATURE) {
              LOG_VRB(("USBD: Clear Feature (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));

              USBD_EP_Stall(p_dev->Nbr,
                            ep_addr,
                            DEF_CLR,
                            &err);
              if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                break;
              }
            } else {
              LOG_VRB(("USBD: Set Feature (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));

              USBD_EP_Stall(p_dev->Nbr,
                            ep_addr,
                            DEF_SET,
                            &err);
              if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                break;
              }
            }

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_CONFIGURED:
          if (feature == USBD_FEATURE_SEL_ENDPOINT_HALT) {
            if (request == USBD_REQ_CLEAR_FEATURE) {
              LOG_VRB(("USBD: Clear Feature (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));

              USBD_EP_Stall(p_dev->Nbr,
                            ep_addr,
                            DEF_CLR,
                            &err);
              if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                break;
              }
            } else {
              LOG_VRB(("USBD: Set Feature (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));

              USBD_EP_Stall(p_dev->Nbr,
                            ep_addr,
                            DEF_SET,
                            &err);
              if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                break;
              }
            }

            ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
            if_nbr = p_dev->EP_IF_Tbl[ep_phy_nbr];
            p_if = USBD_IF_RefGet(p_dev->ConfigCurPtr, if_nbr);
            p_alt_if = p_if->AltCurPtr;

            //                                                     Notify class that EP state has been updated.
            if (p_if->ClassDrvPtr->EP_StateUpdate != DEF_NULL) {
              p_if->ClassDrvPtr->EP_StateUpdate(p_dev->Nbr,
                                                p_dev->ConfigCurNbr,
                                                if_nbr,
                                                p_if->AltCur,
                                                ep_addr,
                                                p_if->ClassArgPtr,
                                                p_alt_if->ClassArgPtr);
            }

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          break;
      }
      break;

    case USBD_REQ_GET_STATUS:
      if (dev_to_host != DEF_YES) {
        break;
      }

      p_dev->CtrlStatusBufPtr[0u] = DEF_BIT_NONE;
      p_dev->CtrlStatusBufPtr[1u] = DEF_BIT_NONE;

      switch (p_dev->State) {
        case USBD_DEV_STATE_ADDRESSED:
          if ((ep_addr == 0x80u)
              || (ep_addr == 0x00u)) {
            LOG_VRB(("USBD: Get Status (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));
            ep_is_stall = USBD_EP_IsStalled(p_dev->Nbr, ep_addr, &err);
            if (ep_is_stall == DEF_TRUE) {
              p_dev->CtrlStatusBufPtr[0u] = DEF_BIT_00;
              p_dev->CtrlStatusBufPtr[1u] = DEF_BIT_NONE;
            }

            (void)USBD_CtrlTx(p_dev->Nbr,
                              (void *)&p_dev->CtrlStatusBufPtr[0u],
                              2u,
                              std_req_timeout,
                              DEF_NO,
                              &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
              break;
            }

            valid = DEF_OK;
          }
          break;

        case USBD_DEV_STATE_CONFIGURED:
          LOG_VRB(("USBD: Get Status (EP)(STALL) for EP 0x", (X)USBD_EP_ADDR_TO_LOG(ep_addr)));
          ep_is_stall = USBD_EP_IsStalled(p_dev->Nbr, ep_addr, &err);
          if (ep_is_stall == DEF_TRUE) {
            p_dev->CtrlStatusBufPtr[0u] = DEF_BIT_00;
            p_dev->CtrlStatusBufPtr[1u] = DEF_BIT_NONE;
          }

          (void)USBD_CtrlTx(p_dev->Nbr,
                            (void *)&p_dev->CtrlStatusBufPtr[0],
                            2u,
                            std_req_timeout,
                            DEF_NO,
                            &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          valid = DEF_OK;
          break;

        case USBD_DEV_STATE_NONE:
        case USBD_DEV_STATE_INIT:
        case USBD_DEV_STATE_ATTACHED:
        case USBD_DEV_STATE_DEFAULT:
        case USBD_DEV_STATE_SUSPENDED:
        default:
          break;
      }
      break;

    default:
      break;
  }

  USBD_DBG_STATS_DEV_INC_IF_TRUE(p_dev->Nbr, StdReqEP_StallNbr, (valid == DEF_FAIL));

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_StdReqClass()
 *
 * @brief    Class standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqClass(const USBD_DEV *p_dev)
{
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_CLASS_DRV *p_class_drv;
  CPU_INT08U recipient;
  CPU_INT08U if_nbr;
  CPU_INT08U ep_addr;
  CPU_INT08U ep_phy_nbr;
  CPU_BOOLEAN valid;

  USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqClassNbr);

  p_config = p_dev->ConfigCurPtr;
  if (p_config == DEF_NULL) {
    return (DEF_FAIL);
  }

  recipient = p_dev->SetupReq.bmRequestType & USBD_REQ_RECIPIENT_MASK;

  if (recipient == USBD_REQ_RECIPIENT_INTERFACE) {
    if_nbr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
  } else {
    ep_addr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
    if_nbr = p_dev->EP_IF_Tbl[ep_phy_nbr];
  }

  p_if = USBD_IF_RefGet(p_config, if_nbr);
  if (p_if == DEF_NULL) {
    USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqClassStallNbr);
    return (DEF_FAIL);
  }

  p_class_drv = p_if->ClassDrvPtr;
  if (p_class_drv->ClassReq == DEF_NULL) {
    USBD_DBG_STATS_DEV_INC(p_dev->Nbr, StdReqClassStallNbr);
    return (DEF_FAIL);
  }

  valid = p_class_drv->ClassReq(p_dev->Nbr,
                                &p_dev->SetupReq,
                                p_if->ClassArgPtr);

  USBD_DBG_STATS_DEV_INC_IF_TRUE(p_dev->Nbr, StdReqClassStallNbr, (valid == DEF_FAIL));

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_StdReqVendor()
 *
 * @brief    Vendor standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqVendor(const USBD_DEV *p_dev)
{
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_CLASS_DRV *p_class_drv;
  CPU_INT08U recipient;
  CPU_INT08U if_nbr;
  CPU_INT08U ep_addr;
  CPU_INT08U ep_phy_nbr;
  CPU_BOOLEAN valid;

  p_config = p_dev->ConfigCurPtr;
  if (p_config == DEF_NULL) {
    return (DEF_FAIL);
  }

  recipient = p_dev->SetupReq.bmRequestType & USBD_REQ_RECIPIENT_MASK;

  if (recipient == USBD_REQ_RECIPIENT_INTERFACE) {
    if_nbr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
  } else {
    ep_addr = (CPU_INT08U)(p_dev->SetupReq.wIndex & DEF_INT_08_MASK);
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(ep_addr);
    if_nbr = p_dev->EP_IF_Tbl[ep_phy_nbr];
  }

  p_if = USBD_IF_RefGet(p_config, if_nbr);
  if (p_if == DEF_NULL) {
    return (DEF_FAIL);
  }

  p_class_drv = p_if->ClassDrvPtr;
  if (p_class_drv->VendorReq == DEF_NULL) {
    return (DEF_FAIL);
  }

  valid = p_class_drv->VendorReq(p_dev->Nbr,
                                 &p_dev->SetupReq,
                                 p_if->ClassArgPtr);

  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_StdReqDevMS()
 *
 * @brief    Microsoft descriptor request handler (when recipient is device).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_BOOLEAN USBD_StdReqDevMS(const USBD_DEV *p_dev)
{
  CPU_BOOLEAN valid;
  CPU_INT08U if_nbr;
  CPU_INT08U max_if;
  CPU_INT08U if_ix;
  CPU_INT08U compat_id_ix;
  CPU_INT08U subcompat_id_ix;
  CPU_INT08U section_cnt;
  CPU_INT16U feature;
  CPU_INT16U len;
  CPU_INT08U cfg_nbr = 0u;
  CPU_INT32U desc_len;
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_CLASS_DRV *p_class_drv;

  valid = DEF_FAIL;
  feature = p_dev->SetupReq.wIndex;
  if_nbr = (CPU_INT08U)(p_dev->SetupReq.wValue & DEF_INT_08_MASK);
  len = p_dev->SetupReq.wLength;

  //                                                               Use 1st cfg as Microsoft doesn't specify cfg in ...
  //                                                               ... setup pkt.
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (p_dev->Spd == USBD_DEV_SPD_HIGH) {
    p_config = USBD_ConfigRefGet(p_dev, cfg_nbr | USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = USBD_ConfigRefGet(p_dev, cfg_nbr);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif
  if (p_config == DEF_NULL) {
    return (DEF_FAIL);
  }

  switch (feature) {
    case USBD_MS_OS_FEATURE_COMPAT_ID:                          // See note (1).
                                                                // ----------------- SEND DESC HEADER -----------------
                                                                // Compute length of descriptor.
      desc_len = USBD_MS_OS_DESC_COMPAT_ID_HDR_LEN;
      section_cnt = 0u;
      if (if_nbr == 0u) {                                       // If req IF == 0, sends all dev compat IDs.
        max_if = p_config->IF_NbrTotal;
      } else {
        max_if = if_nbr + 1u;
      }

      for (if_ix = if_nbr; if_ix < max_if; if_ix++) {
        p_if = USBD_IF_RefGet(p_config, if_ix);
        p_class_drv = p_if->ClassDrvPtr;
        if (p_class_drv->MS_GetCompatID != DEF_NULL) {
          compat_id_ix = p_class_drv->MS_GetCompatID(p_dev->Nbr, &subcompat_id_ix);
          if (compat_id_ix != USBD_MS_OS_COMPAT_ID_NONE) {
            desc_len += USBD_MS_OS_DESC_COMPAT_ID_SECTION_LEN;
            section_cnt++;
          }
        }
      }

      USBD_DescWrStart((USBD_DEV *)p_dev, desc_len);            // Wr desc hdr.

      USBD_DescWr32(p_dev->Nbr, desc_len);
      USBD_DescWr16(p_dev->Nbr, USBD_MS_OS_DESC_VER_1_0);
      USBD_DescWr16(p_dev->Nbr, feature);
      USBD_DescWr08(p_dev->Nbr, section_cnt);
      USBD_DescWr32(p_dev->Nbr, 0u);                            // Add 7 null bytes (reserved).
      USBD_DescWr16(p_dev->Nbr, 0u);
      USBD_DescWr08(p_dev->Nbr, 0u);

      //                                                           ---------------- SEND DESC SECTIONS ----------------
      if (len != USBD_MS_OS_DESC_COMPAT_ID_HDR_VER_1_0) {       // If req len = version, only send desc hdr.
        for (if_ix = if_nbr; if_ix < max_if; if_ix++) {
          p_if = USBD_IF_RefGet(p_config, if_ix);
          if (p_if->ClassDrvPtr->MS_GetCompatID != DEF_NULL) {
            compat_id_ix = p_if->ClassDrvPtr->MS_GetCompatID(p_dev->Nbr, &subcompat_id_ix);
            if (compat_id_ix != USBD_MS_OS_COMPAT_ID_NONE) {
              USBD_DescWr08(p_dev->Nbr, if_ix);
              USBD_DescWr08(p_dev->Nbr, 0x01u);

              USBD_DescWr(p_dev->Nbr,
                          (CPU_INT08U *)USBD_MS_CompatID[compat_id_ix],
                          8u);

              USBD_DescWr(p_dev->Nbr,
                          (CPU_INT08U *)USBD_MS_SubCompatID[subcompat_id_ix],
                          8u);

              USBD_DescWr32(p_dev->Nbr, 0u);                    // Add 6 null bytes (reserved).
              USBD_DescWr16(p_dev->Nbr, 0u);
            }
          }
        }
      }

      USBD_DescWrStop((USBD_DEV *)p_dev, p_dev->DescBufErrPtr);
      if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    case USBD_MS_OS_FEATURE_EXT_PROPERTIES:
      valid = USBD_StdReqMS_ExtPropWr(p_dev,
                                      p_config,
                                      if_nbr,
                                      len);
      break;

    default:
      break;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_StdReqIF_MS()
 *
 * @brief    Microsoft descriptor request handler (when recipient is interface).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_BOOLEAN USBD_StdReqIF_MS(const USBD_DEV *p_dev)
{
  CPU_BOOLEAN valid = DEF_FAIL;
  CPU_INT08U if_nbr = (CPU_INT08U)(p_dev->SetupReq.wValue & DEF_INT_08_MASK);
  CPU_INT16U feature = p_dev->SetupReq.wIndex;
  CPU_INT16U len = p_dev->SetupReq.wLength;
  USBD_CONFIG *p_config;

  //                                                               Use 1st cfg as Microsoft doesn't specify cfg in ...
  //                                                               ... setup pkt.
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (p_dev->Spd == USBD_DEV_SPD_HIGH) {
    p_config = USBD_ConfigRefGet(p_dev, 0u | USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = USBD_ConfigRefGet(p_dev, 0u);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif
  if (p_config == DEF_NULL) {
    return (DEF_FAIL);
  }

  switch (feature) {
    case USBD_MS_OS_FEATURE_EXT_PROPERTIES:
      valid = USBD_StdReqMS_ExtPropWr(p_dev,
                                      p_config,
                                      if_nbr,
                                      len);
      break;

    default:
      break;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_StdReqMS_ExtPropWr()
 *
 * @brief    Microsoft descriptor request handler (when recipient is interface).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    if_nbr  Interface number.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    len     Length of descriptor as requested by host.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
static CPU_BOOLEAN USBD_StdReqMS_ExtPropWr(const USBD_DEV *p_dev,
                                           USBD_CONFIG    *p_config,
                                           CPU_INT08U     if_nbr,
                                           CPU_INT16U     len)
{
  CPU_BOOLEAN valid = DEF_FAIL;
  CPU_INT08U section_cnt = 0u;
  CPU_INT08U ext_property_cnt;
  CPU_INT08U ext_property_ix;
  CPU_INT32U desc_len = USBD_MS_OS_DESC_EXT_PROPERTIES_HDR_LEN;
  USBD_IF *p_if;
  USBD_CLASS_DRV *p_class_drv;
  USBD_MS_OS_EXT_PROPERTY *p_ext_property;

  //                                                               ----------------- SEND DESC HEADER -----------------
  //                                                               Compute length of descriptor.
  p_if = USBD_IF_RefGet(p_config, if_nbr);
  p_class_drv = p_if->ClassDrvPtr;

  if (p_class_drv->MS_GetExtPropertyTbl != DEF_NULL) {
    ext_property_cnt = p_class_drv->MS_GetExtPropertyTbl(p_dev->Nbr, &p_ext_property);
    for (ext_property_ix = 0u; ext_property_ix < ext_property_cnt; ext_property_ix++) {
      desc_len += USBD_MS_OS_DESC_EXT_PROPERTIES_SECTION_HDR_LEN;
      desc_len += p_ext_property->PropertyNameLen;
      desc_len += p_ext_property->PropertyLen;
      desc_len += 6u;

      section_cnt++;
      p_ext_property++;
    }
  }

  USBD_DescWrStart((USBD_DEV *)p_dev, desc_len);

  USBD_DescWr32(p_dev->Nbr, desc_len);
  USBD_DescWr16(p_dev->Nbr, USBD_MS_OS_DESC_VER_1_0);
  USBD_DescWr16(p_dev->Nbr, USBD_MS_OS_FEATURE_EXT_PROPERTIES);
  USBD_DescWr16(p_dev->Nbr, section_cnt);

  //                                                               ---------------- SEND DESC SECTIONS ----------------
  //                                                               If req len = version, only send desc hdr.
  if ((len != USBD_MS_OS_DESC_EXT_PROPERTIES_HDR_VER_1_0)
      && (p_class_drv->MS_GetExtPropertyTbl != DEF_NULL)) {
    ext_property_cnt = p_class_drv->MS_GetExtPropertyTbl(p_dev->Nbr, &p_ext_property);
    for (ext_property_ix = 0u; ext_property_ix < ext_property_cnt; ext_property_ix++) {
      //                                                           Compute desc section len.
      desc_len = USBD_MS_OS_DESC_EXT_PROPERTIES_SECTION_HDR_LEN;
      desc_len += p_ext_property->PropertyNameLen;
      desc_len += p_ext_property->PropertyLen;
      desc_len += 6u;

      //                                                           Wr desc section.
      USBD_DescWr32(p_dev->Nbr, desc_len);
      USBD_DescWr32(p_dev->Nbr, p_ext_property->PropertyType);

      USBD_DescWr16(p_dev->Nbr, p_ext_property->PropertyNameLen);
      USBD_DescWr(p_dev->Nbr,
                  (CPU_INT08U *)p_ext_property->PropertyNamePtr,
                  p_ext_property->PropertyNameLen);

      USBD_DescWr32(p_dev->Nbr, p_ext_property->PropertyLen);
      USBD_DescWr(p_dev->Nbr,
                  (CPU_INT08U *)p_ext_property->PropertyPtr,
                  p_ext_property->PropertyLen);

      p_ext_property++;
    }
  }

  USBD_DescWrStop((USBD_DEV *)p_dev, p_dev->DescBufErrPtr);
  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    valid = DEF_OK;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_StdReqDescGet()
 *
 * @brief    GET_DESCRIPTOR standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_StdReqDescGet(USBD_DEV *p_dev)
{
  CPU_INT08U desc_type;
  CPU_INT08U desc_ix;
  CPU_INT16U req_len;
  CPU_BOOLEAN valid;
  RTOS_ERR err;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  USBD_DRV *p_drv;
#endif

  desc_type = (CPU_INT08U)((p_dev->SetupReq.wValue >> 8u) & DEF_INT_08_MASK);
  desc_ix = (CPU_INT08U)(p_dev->SetupReq.wValue        & DEF_INT_08_MASK);
  valid = DEF_FAIL;
  req_len = p_dev->SetupReq.wLength;
  p_dev->ActualBufPtr = p_dev->DescBufPtr;                      // Set the desc buf as the current buf.
  p_dev->DescBufMaxLen = USBD_DESC_BUF_LEN;                     // Set the max len for the desc buf.
  p_dev->DescBufErrPtr = &err;

  switch (desc_type) {
    case USBD_DESC_TYPE_DEVICE:                                 // ----------------- DEVICE DESCRIPTOR ----------------
      LOG_VRB(("USBD: Get Descriptor (Device)"));
      USBD_DevDescSend(p_dev,
                       DEF_NO,
                       req_len,
                       &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
      break;

    case USBD_DESC_TYPE_CONFIGURATION:                          // ------------- CONFIGURATION DESCRIPTOR -------------
      LOG_VRB(("USBD: Get Descriptor (Configuration) ix #", (u)desc_ix));
      USBD_ConfigDescSend(p_dev,
                          desc_ix,
                          DEF_NO,
                          req_len,
                          &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("USBD Get Descriptor (Configuration) Failed. Err = ", RTOS_ERR_LOG_ARG_GET(err)));
      } else {
        valid = DEF_OK;
      }
      break;

    case USBD_DESC_TYPE_STRING:                                 // ---------------- STRING DESCRIPTOR -----------------
      LOG_VRB(("USBD: Get Descriptor (String) ix #", (u)desc_ix));
#if (USBD_CFG_STR_EN == DEF_ENABLED)
      USBD_StrDescSend(p_dev,
                       desc_ix,
                       req_len,
                       &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
#endif
      break;

    case USBD_DESC_TYPE_DEVICE_QUALIFIER:                       // ----------- DEVICE QUALIFIER DESCRIPTOR ------------
      LOG_VRB(("USBD: Get Descriptor (Device Qualifier)"));
#if (USBD_CFG_HS_EN == DEF_ENABLED)
      p_drv = &p_dev->Drv;

      if (p_drv->CfgPtr->Spd == USBD_DEV_SPD_FULL) {            // Chk if dev only supports FS.
        break;
      }

      USBD_DevDescSend(p_dev,
                       DEF_YES,
                       req_len,
                       &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
#endif
      break;

    case USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION:              // ------- OTHER-SPEED CONFIGURATION DESCRIPTOR -------
      LOG_VRB(("USBD: Get Descriptor (Other Speed)"));
#if (USBD_CFG_HS_EN == DEF_ENABLED)
      p_drv = &p_dev->Drv;

      if (p_drv->CfgPtr->Spd == USBD_DEV_SPD_FULL) {
        break;
      }

      USBD_ConfigDescSend(p_dev,
                          desc_ix,
                          DEF_YES,
                          req_len,
                          &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        valid = DEF_OK;
      }
#endif
      break;

    default:
      break;
  }

  p_dev->DescBufErrPtr = DEF_NULL;
  return (valid);
}

/****************************************************************************************************//**
 *                                           USBD_StdReqDescSet()
 *
 * @brief    SET_DESCRIPTOR standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   DEF_OK,   if no error(s) occurred and request is supported.
 *           DEF_FAIL, if any errors are returned.
 *
 * @note     (1) $$$$ SET_DESCRIPTOR MAY be implemented in future versions.
 *******************************************************************************************************/

#if 0
static CPU_BOOLEAN USBD_StdReqDescSet(USBD_DEV *p_dev)
{
  (void)&p_dev;

  return (DEF_FAIL);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_CfgClose()
 *
 * @brief    Close current device configuration.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue and
 *                   'USBD_DevStop()' function.
 *******************************************************************************************************/
static void USBD_ConfigClose(USBD_DEV *p_dev)
{
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_IF_ALT *p_if_alt;
  USBD_DRV_API *p_drv_api;
  CPU_INT08U if_nbr;
  CORE_DECLARE_IRQ_STATE;

  p_config = p_dev->ConfigCurPtr;
  if (p_config == DEF_NULL) {
    return;
  }

  if ((p_dev->BusFnctsPtr != DEF_NULL)
      && (p_dev->BusFnctsPtr->CfgClr != DEF_NULL)) {
    //                                                             Notify app about clr cfg.
    p_dev->BusFnctsPtr->CfgClr(p_dev->Nbr, p_dev->ConfigCurNbr);
  }

  for (if_nbr = 0u; if_nbr < p_config->IF_NbrTotal; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    if (p_if == DEF_NULL) {
      return;
    }

    p_if_alt = p_if->AltCurPtr;
    if (p_if_alt == DEF_NULL) {
      return;
    }

    if (p_if->ClassDrvPtr->Disconn != DEF_NULL) {
      p_if->ClassDrvPtr->Disconn(p_dev->Nbr,                    // Notify class that cfg is not active.
                                 p_dev->ConfigCurNbr,
                                 p_if->ClassArgPtr);
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->State = USBD_DEV_STATE_ADDRESSED;
  CORE_EXIT_ATOMIC();

  for (if_nbr = 0u; if_nbr < p_config->IF_NbrTotal; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    if (p_if == DEF_NULL) {
      return;
    }

    p_if_alt = p_if->AltCurPtr;
    if (p_if_alt == DEF_NULL) {
      return;
    }

    USBD_IF_AltClose(p_dev, p_if_alt);

    p_if_alt = USBD_IF_AltRefGet(p_if, 0u);

    CORE_ENTER_ATOMIC();
    p_if->AltCurPtr = p_if_alt;
    p_if->AltCur = 0u;
    CORE_EXIT_ATOMIC();
  }

  p_drv_api = p_dev->Drv.API_Ptr;
  if (p_drv_api->CfgClr != DEF_NULL) {
    p_drv_api->CfgClr(&p_dev->Drv, p_dev->ConfigCurNbr);        // Clr cfg in the driver.
  }

  CORE_ENTER_ATOMIC();
  p_dev->ConfigCurPtr = DEF_NULL;
  p_dev->ConfigCurNbr = USBD_CONFIG_NBR_NONE;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               USBD_CfgOpen()
 *
 * @brief    Open specified configuration.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    config_nbr  Configuration number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBD_ConfigOpen(USBD_DEV   *p_dev,
                            CPU_INT08U config_nbr,
                            RTOS_ERR   *p_err)

{
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_IF_ALT *p_if_alt;
  USBD_DRV_API *p_drv_api;
  CPU_INT08U if_nbr;
  CPU_BOOLEAN cfg_set;
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (p_dev->Spd == USBD_DEV_SPD_HIGH) {
    p_config = USBD_ConfigRefGet(p_dev, config_nbr | USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = USBD_ConfigRefGet(p_dev, config_nbr);
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif

  if (p_config == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  for (if_nbr = 0u; if_nbr < p_config->IF_NbrTotal; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    if (p_if == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    p_if_alt = p_if->AltCurPtr;
    if (p_if_alt == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    USBD_IF_AltOpen(p_dev,
                    if_nbr,
                    p_if_alt,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->ConfigCurPtr = p_config;
  p_dev->ConfigCurNbr = config_nbr;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_drv_api = p_dev->Drv.API_Ptr;
  if (p_drv_api->CfgSet != DEF_NULL) {
    cfg_set = p_drv_api->CfgSet(&p_dev->Drv, config_nbr);       // Set cfg in the drv.
    if (cfg_set == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->State = USBD_DEV_STATE_CONFIGURED;
  CORE_EXIT_ATOMIC();

  for (if_nbr = 0u; if_nbr < p_config->IF_NbrTotal; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    if (p_if == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    } else {
      if (p_if->ClassDrvPtr->Conn != DEF_NULL) {
        p_if->ClassDrvPtr->Conn(p_dev->Nbr,                     // Notify class that cfg is active.
                                config_nbr,
                                p_if->ClassArgPtr);
      }
    }
  }

  if ((p_dev->BusFnctsPtr != DEF_NULL)
      && (p_dev->BusFnctsPtr->CfgSet != DEF_NULL)) {
    p_dev->BusFnctsPtr->CfgSet(p_dev->Nbr, config_nbr);         // Notify app about set cfg.
  }
}

/****************************************************************************************************//**
 *                                           USBD_DevDescSend()
 *
 * @brief    Send device configuration descriptor.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    other       Other speed configuration :
 *                           - DEF_YES     Current speed.
 *                           - DEF_NO      Other operational speed.
 *
 * @param    req_len     Requested length by the host.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function,
 *
 * @note     (1) USB Spec 2.0 table 9-8 describes the standard device descriptor.
 *
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               | Offset |        Field       |  Size |   Value  |            Description            |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    0   | bLength            |   1   | Number   | Size of this descriptor           |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    1   | bDescriptorType    |   1   | Const    | DEVICE Descriptor Type            |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    2   | bcdUSB             |   2   | BCD USB  | Specification release number      |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    4   | bDeviceClass       |   1   | Class    | Class code.                       |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    5   | bDeviceSubClass    |   1   | SubClass | Subclass code.                    |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    6   | bDeviceProtocol    |   1   | Protocol | Protocol code.                    |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    7   | bMaxPacketSize0    |   1   | Number   | Max packet size for EP zero       |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    8   | idVendor           |   2   | ID       | Vendor  ID                        |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   10   | idProduct          |   2   | ID       | Product ID                        |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   12   | bcdDevice          |   2   | BCD      | Dev release number in BCD format  |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   14   | iManufacturer      |   1   | Index    | Index of manufacturer string      |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   15   | iProduct           |   1   | Index    | Index of product string           |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *               |   16   | iSerialNumber      |   1   | Index    | Index of serial number string     |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *               |   17   | bNumConfigurations |   1   |  Number  | Number of possible configurations |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *
 *           ) To enable host to identify devices that use the Interface Association descriptor the
 *           device descriptor should contain the following values.
 *******************************************************************************************************/
static void USBD_DevDescSend(USBD_DEV    *p_dev,
                             CPU_BOOLEAN other,
                             CPU_INT16U  req_len,
                             RTOS_ERR    *p_err)
{
  USBD_CONFIG *p_config;
  CPU_BOOLEAN if_grp_en;
  CPU_INT08U cfg_nbr;
  CPU_INT08U cfg_nbr_spd;
  CPU_INT08U cfg_nbr_total;
#if (USBD_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U str_ix;
#endif

#if (USBD_CFG_HS_EN == DEF_DISABLED)
  (void)&other;
#endif

  if_grp_en = DEF_NO;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (other == DEF_NO) {
#endif
  USBD_DescWrStart(p_dev, req_len);
  USBD_DescWrReq08(p_dev, USBD_DESC_LEN_DEV);                   // Desc len.
  USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_DEVICE);               // Dev desc type.
  USBD_DescWrReq16(p_dev, 0x200u);                              // USB spec release nbr in BCD fmt (2.00).

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (p_dev->Spd == USBD_DEV_SPD_FULL) {
#endif
  cfg_nbr_spd = DEF_BIT_NONE;
  cfg_nbr_total = p_dev->ConfigFS_TotalNbr;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
} else {
  cfg_nbr_spd = USBD_CONFIG_NBR_SPD_BIT;
  cfg_nbr_total = p_dev->ConfigHS_TotalNbr;
}
#endif

  cfg_nbr = 0u;
  while ((cfg_nbr < cfg_nbr_total)
         && (if_grp_en == DEF_NO)) {
    p_config = USBD_ConfigRefGet(p_dev, cfg_nbr | cfg_nbr_spd);
    if (p_config != DEF_NULL) {
      if (p_config->IF_GrpNbrTotal > 0u) {
        if_grp_en = DEF_YES;
      }
    }

    cfg_nbr++;
  }

  if (if_grp_en == DEF_NO) {
    //                                                             Dev class is specified in IF desc.
    USBD_DescWrReq08(p_dev, USBD_CLASS_CODE_USE_IF_DESC);
    USBD_DescWrReq08(p_dev, USBD_SUBCLASS_CODE_USE_IF_DESC);
    USBD_DescWrReq08(p_dev, USBD_PROTOCOL_CODE_USE_IF_DESC);
  } else {
    //                                                             Multi-Interface function dev class.
    USBD_DescWrReq08(p_dev, USBD_CLASS_CODE_MISCELLANEOUS);
    USBD_DescWrReq08(p_dev, USBD_SUBCLASS_CODE_USE_COMMON_CLASS);
    USBD_DescWrReq08(p_dev, USBD_PROTOCOL_CODE_USE_IAD);
  }
  //                                                               Set max pkt size for ctrl EP.
  USBD_DescWrReq08(p_dev, (CPU_INT08U)p_dev->EP_CtrlMaxPktSize);
  //                                                               Set vendor id, product id and dev id.
  USBD_DescWrReq16(p_dev, p_dev->DevCfg.VendorID);
  USBD_DescWrReq16(p_dev, p_dev->DevCfg.ProductID);
  USBD_DescWrReq16(p_dev, p_dev->DevCfg.DeviceBCD);

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  str_ix = USBD_StrDescIxGet(p_dev, p_dev->DevCfg.ManufacturerStrPtr);
  USBD_DescWrReq08(p_dev, str_ix);
  str_ix = USBD_StrDescIxGet(p_dev, p_dev->DevCfg.ProductStrPtr);
  USBD_DescWrReq08(p_dev, str_ix);
  str_ix = USBD_StrDescIxGet(p_dev, p_dev->DevCfg.SerialNbrStrPtr);
  USBD_DescWrReq08(p_dev, str_ix);
#else
  USBD_DescWrReq08(p_dev, 0u);
  USBD_DescWrReq08(p_dev, 0u);
  USBD_DescWrReq08(p_dev, 0u);
#endif
  USBD_DescWrReq08(p_dev, cfg_nbr_total);

#if (USBD_CFG_HS_EN == DEF_ENABLED)
} else {
  if (p_dev->Drv.CfgPtr->Spd != USBD_DEV_SPD_HIGH) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  USBD_DescWrStart(p_dev, req_len);
  USBD_DescWrReq08(p_dev, USBD_DESC_LEN_DEV_QUAL);              // Desc len.
  USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_DEVICE_QUALIFIER);
  USBD_DescWrReq16(p_dev, 0x200u);                              // USB spec release nbr in BCD fmt (2.00).

  if (p_dev->Spd == USBD_DEV_SPD_HIGH) {
    cfg_nbr_spd = DEF_BIT_NONE;
    cfg_nbr_total = p_dev->ConfigFS_TotalNbr;
  } else {
    cfg_nbr_spd = USBD_CONFIG_NBR_SPD_BIT;
    cfg_nbr_total = p_dev->ConfigHS_TotalNbr;
  }

  cfg_nbr = 0u;
  while ((cfg_nbr < cfg_nbr_total)
         && (if_grp_en == DEF_NO)) {
    p_config = USBD_ConfigRefGet(p_dev, cfg_nbr | cfg_nbr_spd);
    if (p_config != DEF_NULL) {
      if (p_config->IF_GrpNbrTotal > 0u) {
        if_grp_en = DEF_YES;
      }
      cfg_nbr++;
    }
  }
  if (if_grp_en == DEF_NO) {
    //                                                             Dev class is specified in IF desc.
    USBD_DescWrReq08(p_dev, USBD_CLASS_CODE_USE_IF_DESC);
    USBD_DescWrReq08(p_dev, USBD_SUBCLASS_CODE_USE_IF_DESC);
    USBD_DescWrReq08(p_dev, USBD_PROTOCOL_CODE_USE_IF_DESC);
  } else {
    //                                                             Multi-Interface function dev class.
    USBD_DescWrReq08(p_dev, USBD_CLASS_CODE_MISCELLANEOUS);
    USBD_DescWrReq08(p_dev, USBD_SUBCLASS_CODE_USE_COMMON_CLASS);
    USBD_DescWrReq08(p_dev, USBD_PROTOCOL_CODE_USE_IAD);
  }
  //                                                               Set max pkt size for ctrl EP.
  USBD_DescWrReq08(p_dev, (CPU_INT08U)p_dev->EP_CtrlMaxPktSize);
  USBD_DescWrReq08(p_dev, cfg_nbr_total);
  USBD_DescWrReq08(p_dev, 0u);
}
#endif

  USBD_DescWrStop(p_dev, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_CfgDescSend()
 *
 * @brief    Send configuration descriptor.
 *
 * @param    p_dev       Pointer to device struct.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    cfg_nbr     Configuration number.
 *
 * @param    other       Other speed configuration :
 *                       DEF_NO      Descriptor is build for the current speed.
 *                       DEF_YES     Descriptor is build for the  other  speed.
 *
 * @param    req_len     Requested length by the host.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBD_ConfigDescSend(USBD_DEV    *p_dev,
                                CPU_INT08U  config_nbr,
                                CPU_BOOLEAN other,
                                CPU_INT16U  req_len,
                                RTOS_ERR    *p_err)
{
  USBD_CONFIG *p_config;
  USBD_IF *p_if;
  USBD_EP_INFO *p_ep;
  USBD_IF_ALT *p_if_alt;
  USBD_IF_GRP *p_if_grp;
  USBD_CLASS_DRV *p_if_drv;
  CPU_INT08U cfg_nbr_cur;
  CPU_INT08U ep_nbr;
  CPU_INT08U if_nbr;
  CPU_INT08U if_total;
  CPU_INT08U if_grp_cur;
  CPU_INT08U if_alt_nbr;
  CPU_INT08U attrib;
#if (USBD_CFG_STR_EN == DEF_ENABLED)
  CPU_INT08U str_ix;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CPU_INT32U ep_alloc_map;
#endif

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (p_dev->Spd == USBD_DEV_SPD_HIGH) {
    cfg_nbr_cur = config_nbr | USBD_CONFIG_NBR_SPD_BIT;
  } else {
#endif
  cfg_nbr_cur = config_nbr;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_config = USBD_ConfigRefGet(p_dev, cfg_nbr_cur);
  if (p_config == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

#if (USBD_CFG_HS_EN == DEF_ENABLED)                             // other will always be DEF_NO when HS is disabled.
  if (other == DEF_YES) {
    if (p_config->ConfigOtherSpd == USBD_CONFIG_NBR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }

    cfg_nbr_cur = p_config->ConfigOtherSpd;

    p_config = USBD_ConfigRefGet(p_dev, cfg_nbr_cur);           // Retrieve cfg struct for other spd.
    if (p_config == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  }
#endif

  p_config->DescLen = USBD_DESC_LEN_CFG;                        // Init cfg desc len.

  USBD_DescWrStart(p_dev, req_len);
  //                                                               ---------- BUILD CONFIGURATION DESCRIPTOR ----------
  USBD_DescWrReq08(p_dev, USBD_DESC_LEN_CFG);                   // Desc len.
  if (other == DEF_YES) {
    USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION);
  } else {
    USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_CONFIGURATION);      // Desc type.
  }

  if_total = p_config->IF_NbrTotal;
  if_grp_cur = USBD_IF_GRP_NBR_NONE;

  for (if_nbr = 0u; if_nbr < if_total; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    p_if_drv = p_if->ClassDrvPtr;

    if ((p_if->GrpNbr != if_grp_cur)
        && (p_if->GrpNbr != USBD_IF_GRP_NBR_NONE)) {
      //                                                           Add IF assoc desc len.
      p_config->DescLen += USBD_DESC_LEN_IF_ASSOCIATION;
      if_grp_cur = p_if->GrpNbr;
    }

    p_config->DescLen += (USBD_DESC_LEN_IF * p_if->AltNbrTotal);

    for (if_alt_nbr = 0u; if_alt_nbr < p_if->AltNbrTotal; if_alt_nbr++) {
      p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);
      p_config->DescLen += (USBD_DESC_LEN_EP * p_if_alt->EP_NbrTotal);

      if (p_if_drv->IF_DescSizeGet != DEF_NULL) {               // Add IF functional desc len.
        p_config->DescLen += p_if_drv->IF_DescSizeGet(p_dev->Nbr,
                                                      cfg_nbr_cur,
                                                      if_nbr,
                                                      if_alt_nbr,
                                                      p_if->ClassArgPtr,
                                                      p_if_alt->ClassArgPtr);
      }

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
      ep_alloc_map = p_if_alt->EP_TblMap;
      while (ep_alloc_map != DEF_BIT_NONE) {
        ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
        p_ep = p_if_alt->EP_TblPtrs[ep_nbr];

        if (p_if_drv->EP_DescSizeGet != DEF_NULL) {             // Add EP functional desc len.
          p_config->DescLen += p_if_drv->EP_DescSizeGet(p_dev->Nbr,
                                                        cfg_nbr_cur,
                                                        if_nbr,
                                                        if_alt_nbr,
                                                        p_ep->Addr,
                                                        p_if->ClassArgPtr,
                                                        p_if_alt->ClassArgPtr);
        }

        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          p_config->DescLen += 2u;                              // EP desc on audio class v1.0 has 2 additional fields.
        }

        DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
      }
#else
      p_ep = p_if_alt->EP_HeadPtr;

      for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
        if (p_if_drv->EP_DescSizeGet != DEF_NULL) {
          p_config->DescLen += p_if_drv->EP_DescSizeGet(p_dev->Nbr,
                                                        cfg_nbr_cur,
                                                        if_nbr,
                                                        if_alt_nbr,
                                                        p_ep->Addr,
                                                        p_if->ClassArgPtr,
                                                        p_if_alt->ClassArgPtr);
        }

        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          p_config->DescLen += 2u;                              // EP desc on audio class v1.0 has 2 additional fields.
        }

        p_ep = p_ep->NextPtr;
      }
#endif
    }
  }
  //                                                               ------------------ BUILD CFG DESC ------------------
  USBD_DescWrReq16(p_dev, p_config->DescLen);                   // Desc len.
  USBD_DescWrReq08(p_dev, p_config->IF_NbrTotal);               // Nbr of IF.
  USBD_DescWrReq08(p_dev, config_nbr + 1u);                     // Cfg ix.

#if (USBD_CFG_STR_EN == DEF_ENABLED)
  str_ix = USBD_StrDescIxGet(p_dev, p_config->NamePtr);         // Add str ix.
  USBD_DescWrReq08(p_dev, str_ix);
#else
  USBD_DescWrReq08(p_dev, 0u);
#endif

  attrib = USBD_CONFIG_DESC_RSVD_SET;
  if (DEF_BIT_IS_SET(p_config->Attrib, USBD_DEV_ATTRIB_SELF_POWERED)) {
    DEF_BIT_SET(attrib, USBD_CONFIG_DESC_SELF_POWERED);
  }
  if (DEF_BIT_IS_SET(p_config->Attrib, USBD_DEV_ATTRIB_REMOTE_WAKEUP)) {
    DEF_BIT_SET(attrib, USBD_CONFIG_DESC_REMOTE_WAKEUP);
  }
  USBD_DescWrReq08(p_dev, attrib);
  USBD_DescWrReq08(p_dev, (CPU_INT08U)((p_config->MaxPwr + 1u) / 2u));

  //                                                               ------------ BUILD INTERFACE DESCRIPTOR ------------
  if_total = p_config->IF_NbrTotal;
  if_grp_cur = USBD_IF_GRP_NBR_NONE;

  for (if_nbr = 0u; if_nbr < if_total; if_nbr++) {
    p_if = USBD_IF_RefGet(p_config, if_nbr);
    p_if_drv = p_if->ClassDrvPtr;

    if ((p_if->GrpNbr != if_grp_cur)
        && (p_if->GrpNbr != USBD_IF_GRP_NBR_NONE)) {
      //                                                           Add IF assoc desc (IAD).
      p_if_grp = USBD_IF_GrpRefGet(p_config, p_if->GrpNbr);

      USBD_DescWrReq08(p_dev, USBD_DESC_LEN_IF_ASSOCIATION);
      USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_IAD);
      USBD_DescWrReq08(p_dev, p_if_grp->IF_Start);
      USBD_DescWrReq08(p_dev, p_if_grp->IF_Cnt);
      USBD_DescWrReq08(p_dev, p_if_grp->ClassCode);
      USBD_DescWrReq08(p_dev, p_if_grp->ClassSubCode);
      USBD_DescWrReq08(p_dev, p_if_grp->ClassProtocolCode);

#if (USBD_CFG_STR_EN == DEF_ENABLED)
      str_ix = USBD_StrDescIxGet(p_dev, p_if_grp->NamePtr);
      USBD_DescWrReq08(p_dev, str_ix);
#else
      USBD_DescWrReq08(p_dev, 0u);
#endif

      if_grp_cur = p_if->GrpNbr;
    }
    //                                                             Add IF/alt settings desc.
    for (if_alt_nbr = 0u; if_alt_nbr < p_if->AltNbrTotal; if_alt_nbr++) {
      p_if_alt = USBD_IF_AltRefGet(p_if, if_alt_nbr);

      USBD_DescWrReq08(p_dev, USBD_DESC_LEN_IF);
      USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_INTERFACE);
      USBD_DescWrReq08(p_dev, if_nbr);
      USBD_DescWrReq08(p_dev, if_alt_nbr);
      USBD_DescWrReq08(p_dev, p_if_alt->EP_NbrTotal);
      USBD_DescWrReq08(p_dev, p_if->ClassCode);
      USBD_DescWrReq08(p_dev, p_if->ClassSubCode);
      USBD_DescWrReq08(p_dev, p_if->ClassProtocolCode);

#if (USBD_CFG_STR_EN == DEF_ENABLED)
      str_ix = USBD_StrDescIxGet(p_dev, p_if_alt->NamePtr);
      USBD_DescWrReq08(p_dev, str_ix);
#else
      USBD_DescWrReq08(p_dev, 0u);
#endif

      if (p_if_drv->IF_Desc != DEF_NULL) {                      // Add class specific IF desc.
        p_if_drv->IF_Desc(p_dev->Nbr,
                          cfg_nbr_cur,
                          if_nbr,
                          if_alt_nbr,
                          p_if->ClassArgPtr,
                          p_if_alt->ClassArgPtr);
      }
      //                                                           ------------------- BUILD EP DESC ------------------
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
      ep_alloc_map = p_if_alt->EP_TblMap;
      while (ep_alloc_map != DEF_BIT_NONE) {
        ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
        p_ep = p_if_alt->EP_TblPtrs[ep_nbr];

        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          //                                                       EP desc on audio class v1.0 has 2 additional fields.
          USBD_DescWrReq08(p_dev, USBD_DESC_LEN_EP + 2u);
        } else {
          USBD_DescWrReq08(p_dev, USBD_DESC_LEN_EP);
        }

        USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_ENDPOINT);
        USBD_DescWrReq08(p_dev, p_ep->Addr);
        USBD_DescWrReq08(p_dev, p_ep->Attrib);
        USBD_DescWrReq16(p_dev, p_ep->MaxPktSize);
        USBD_DescWrReq08(p_dev, p_ep->Interval);

        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          //                                                       EP desc on audio class v1.0 has 2 additional fields.
          USBD_DescWrReq08(p_dev, p_ep->SyncRefresh);
          USBD_DescWrReq08(p_dev, p_ep->SyncAddr);
        }

        if (p_if_drv->EP_Desc != DEF_NULL) {
          p_if_drv->EP_Desc(p_dev->Nbr,                         // Add class specific EP desc.
                            cfg_nbr_cur,
                            if_nbr,
                            if_alt_nbr,
                            p_ep->Addr,
                            p_if->ClassArgPtr,
                            p_if_alt->ClassArgPtr);
        }

        DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
      }
#else
      p_ep = p_if_alt->EP_HeadPtr;

      for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          //                                                       EP desc on audio class v1.0 has 2 additional fields.
          USBD_DescWrReq08(p_dev, USBD_DESC_LEN_EP + 2u);
        } else {
          USBD_DescWrReq08(p_dev, USBD_DESC_LEN_EP);
        }

        USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_ENDPOINT);
        USBD_DescWrReq08(p_dev, p_ep->Addr);
        USBD_DescWrReq08(p_dev, p_ep->Attrib);
        USBD_DescWrReq16(p_dev, p_ep->MaxPktSize);
        USBD_DescWrReq08(p_dev, p_ep->Interval);

        if ((p_if->ClassCode == USBD_CLASS_CODE_AUDIO)
            && (p_if->ClassProtocolCode == 0u)
            && (((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_ISOC)
                || ((p_ep->Attrib & USBD_EP_TYPE_MASK) == USBD_EP_TYPE_INTR))) {
          //                                                       EP desc on audio class v1.0 has 2 additional fields.
          USBD_DescWrReq08(p_dev, p_ep->SyncRefresh);
          USBD_DescWrReq08(p_dev, p_ep->SyncAddr);
        }

        if (p_if_drv->EP_Desc != DEF_NULL) {
          p_if_drv->EP_Desc(p_dev->Nbr,                         // Add class specific EP desc.
                            cfg_nbr_cur,
                            if_nbr,
                            if_alt_nbr,
                            p_ep->Addr,
                            p_if->ClassArgPtr,
                            p_if_alt->ClassArgPtr);
        }

        p_ep = p_ep->NextPtr;
      }
#endif
    }
  }

  USBD_DescWrStop(p_dev, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_StrDescSend()
 *
 * @brief    Send string descriptor.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    str_ix      String index.
 *
 * @param    req_len     Requested length by the host.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
static void USBD_StrDescSend(USBD_DEV   *p_dev,
                             CPU_INT08U str_ix,
                             CPU_INT16U req_len,
                             RTOS_ERR   *p_err)
{
  const CPU_CHAR *p_str;
  CPU_SIZE_T len;
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  CPU_INT08U ix;
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  USBD_DescWrStart(p_dev, req_len);

  switch (str_ix) {
    case 0u:
      USBD_DescWrReq08(p_dev, 4u);
      USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_STRING);
      USBD_DescWrReq16(p_dev, p_dev->DevCfg.LangId);
      break;

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
    case USBD_STR_MS_OS_IX:
      USBD_DescWrReq08(p_dev, USBD_STR_MS_OS_LEN);
      USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_STRING);

      for (ix = 0u; ix < 7u; ix++) {
        USBD_DescWrReq08(p_dev, (CPU_INT08U)USBD_StrMS_Signature[ix]);
        USBD_DescWrReq08(p_dev, 0u);
      }

      USBD_DescWrReq08(p_dev, p_dev->StrMS_VendorCode);
      USBD_DescWrReq08(p_dev, 0u);
      break;
#endif

    default:
      p_str = USBD_StrDescGet(p_dev, str_ix - 1u);
      if (p_str != DEF_NULL) {
        len = Str_Len(p_str);
        len = (2u * len) + 2u;
        len = DEF_MIN(len, DEF_INT_08U_MAX_VAL);
        len = len - (len % 2u);

        USBD_DescWrReq08(p_dev, (CPU_INT08U)len);
        USBD_DescWrReq08(p_dev, USBD_DESC_TYPE_STRING);

        while (*p_str != '\0') {
          USBD_DescWrReq08(p_dev, (CPU_INT08U)*p_str);
          USBD_DescWrReq08(p_dev, 0u);

          p_str++;
        }
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
        return;
      }
      break;
  }

  USBD_DescWrStop(p_dev, p_err);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_DescWrStart()
 *
 * @brief    Start write operation in the descriptor buffer.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    req_len     Requested length by the host.
 *******************************************************************************************************/
static void USBD_DescWrStart(USBD_DEV   *p_dev,
                             CPU_INT16U req_len)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_dev->DescBufIx = 0u;
  p_dev->DescBufReqLen = req_len;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               USBD_DescWrStop()
 *
 * @brief    Stop write operation in the descriptor buffer.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) This function might be called in two contexts: when a Get Descriptor standard request
 *               is received, or when a driver supporting standard request auto-reply queries the
 *               device, a configuration or a string descriptor. The descriptor needs to be sent on
 *               control endpoint 0 only if this function is called for a Get Descriptor standard
 *               request. If the function is called when a driver needs the descriptor, nothing has to
 *               be done.
 *******************************************************************************************************/
static void USBD_DescWrStop(USBD_DEV *p_dev,
                            RTOS_ERR *p_err)
{
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    if (p_dev->ActualBufPtr == p_dev->DescBufPtr) {             // See Note #1.
      if (p_dev->DescBufIx > 0u) {
        CPU_INT32U std_req_timeout;
        CORE_DECLARE_IRQ_STATE;

        CORE_ENTER_ATOMIC();
        std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
        CORE_EXIT_ATOMIC();

        (void)USBD_CtrlTx(p_dev->Nbr,
                          &p_dev->DescBufPtr[0u],
                          (CPU_INT32U)p_dev->DescBufIx,
                          std_req_timeout,
                          (p_dev->DescBufReqLen > 0u) ? DEF_YES : DEF_NO,
                          p_err);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           USBD_DescWrReq08()
 *
 * @brief    Write 8-bit value in the descriptor buffer.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----   Argument validated by the caller(s).
 *
 * @param    val     8-bit value.
 *******************************************************************************************************/
static void USBD_DescWrReq08(USBD_DEV   *p_dev,
                             CPU_INT08U val)
{
  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    USBD_DescWrReq(p_dev, &val, 1u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWr16()
 *
 * @brief    Write 16-bit value in the descriptor buffer.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----   Argument validated by the caller(s)
 *
 * @param    val     16-bit value.
 *******************************************************************************************************/
static void USBD_DescWrReq16(USBD_DEV   *p_dev,
                             CPU_INT16U val)
{
  if (RTOS_ERR_CODE_GET(*(p_dev->DescBufErrPtr)) == RTOS_ERR_NONE) {
    CPU_INT08U buf[2u];

    buf[0u] = (CPU_INT08U)(val        & DEF_INT_08_MASK);
    buf[1u] = (CPU_INT08U)((val >> 8u) & DEF_INT_08_MASK);

    USBD_DescWrReq(p_dev, &buf[0u], 2u);
  }
}

/****************************************************************************************************//**
 *                                               USBD_DescWrReq()
 *
 * @brief    USB device configuration write request.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----   Argument validated by the caller(s)
 *
 * @param    p_buf   Pointer to data buffer.
 *
 * @param    -----   Argument validated by the caller(s)
 *
 * @param    len     Buffer length.
 *
 * @note     (1) This function might be called in two contexts: when a Get Descriptor standard request
 *               is received, or when a driver supporting standard request auto-reply queries the
 *               device, a configuration or a string descriptor. In the Get Descriptor standard
 *               request case, if the buffer is full, a transfer on control endpoint 0 is done, before
 *               resuming to fill the buffer. In the case of a driver supporting standard request
 *               auto-reply, if the buffer is full, an error is set and the function exits.
 *
 * @note     (2) If an error is reported by USBD_CtrlTx() during the construction of the descriptor,
 *               this pointer will store the error code, stop the rest of the data phase, skip the
 *               status phase and ensure that the control endpoint 0 is stalled to notify the host
 *               that an error has occurred.
 *******************************************************************************************************/
static void USBD_DescWrReq(USBD_DEV         *p_dev,
                           const CPU_INT08U *p_buf,
                           CPU_INT16U       len)
{
  CPU_INT08U *p_desc;
  CPU_INT08U buf_cur_ix;
  CPU_INT16U len_req;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  p_desc = p_dev->ActualBufPtr;
  buf_cur_ix = p_dev->DescBufIx;
  len_req = p_dev->DescBufReqLen;
  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  while ((len_req != 0u)
         && (len != 0u)) {
    if (buf_cur_ix >= p_dev->DescBufMaxLen) {
      if (p_dev->ActualBufPtr == p_dev->DescBufPtr) {           // Send data in response to std req. See Note #1.
        CPU_INT32U std_req_timeout;

        CORE_ENTER_ATOMIC();
        std_req_timeout = USBD_Ptr->StdReqTimeoutMs;
        CORE_EXIT_ATOMIC();

        buf_cur_ix = 0u;
        (void)USBD_CtrlTx(p_dev->Nbr,
                          &p_dev->DescBufPtr[0u],
                          USBD_DESC_BUF_LEN,
                          std_req_timeout,
                          DEF_NO,
                          &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          break;
        }
      } else {                                                  // Buf provided by driver is too small. See Note #1.
        len_req = 0u;
        RTOS_ERR_SET(err, RTOS_ERR_ALLOC);
      }
    } else {
      p_desc[buf_cur_ix] = *p_buf;
      p_buf++;
      len--;
      len_req--;
      buf_cur_ix++;
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->DescBufIx = buf_cur_ix;
  p_dev->DescBufReqLen = len_req;
  if (p_dev->DescBufErrPtr != DEF_NULL) {
    *(p_dev->DescBufErrPtr) = err;                              // See Note #2.
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               USBD_DevRefGet()
 *
 * @brief    Get device structure.
 *
 * @param    dev_nbr     Device number.
 *
 * @return   Pointer to device structure, if no errors are returned.
 *
 *           Pointer to NULL,             if any errors are returned.
 *******************************************************************************************************/
static USBD_DEV *USBD_DevRefGet(CPU_INT08U dev_nbr)
{
  USBD_DEV *p_dev;

  //                                                               Chk if dev nbr is valid.
#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG((dev_nbr < USBD_Ptr->DevQty), RTOS_ERR_INVALID_ARG, DEF_NULL);
#endif

  p_dev = &USBD_Ptr->DevTbl[dev_nbr];                           // Get dev struct.
  return (p_dev);
}

/****************************************************************************************************//**
 *                                           USBD_ConfigRefGet()
 *
 * @brief    Get configuration structure.
 *
 * @param    p_dev       Pointer to device struct.
 *
 * @param    config_nbr  Configuration number.
 *
 * @return   Pointer to configuration structure, if no errors are returned.
 *
 *           Pointer to NULL,                    if any errors are returned.
 *******************************************************************************************************/
static USBD_CONFIG *USBD_ConfigRefGet(const USBD_DEV *p_dev,
                                      CPU_INT08U     config_nbr)
{
  USBD_CONFIG *p_config;
  CPU_INT08U cfg_val;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  CPU_INT08U config_ix;
#endif

#if (USBD_CFG_HS_EN == DEF_ENABLED)                             // USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  cfg_val = config_nbr & (CPU_INT08U)(~USBD_CONFIG_NBR_SPD_BIT);
#else
  cfg_val = config_nbr;
#endif

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Array implementation.
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT) == DEF_YES) {
    if (cfg_val >= p_dev->ConfigHS_TotalNbr) {                  // Chk if cfg nbr is valid.
      return (DEF_NULL);
    }
    p_config = p_dev->ConfigHS_SpdTblPtrs[cfg_val];             // Get HS cfg struct.
  } else {
#endif
  if (cfg_val >= p_dev->ConfigFS_TotalNbr) {                    // Chk if cfg nbr is valid.
    return (DEF_NULL);
  }
  p_config = p_dev->ConfigFS_SpdTblPtrs[cfg_val];               // Get FS cfg struct.
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif
#else
#if (USBD_CFG_HS_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET(config_nbr, USBD_CONFIG_NBR_SPD_BIT)) {    // Linked-list implementation.
    if (cfg_val >= p_dev->ConfigHS_TotalNbr) {                  // Chk if cfg nbr is valid.
      return (DEF_NULL);
    }
    p_config = p_dev->ConfigHS_HeadPtr;
  } else {
#endif
  if (cfg_val >= p_dev->ConfigFS_TotalNbr) {                    // Chk if cfg nbr is valid.
    return (DEF_NULL);
  }
  p_config = p_dev->ConfigFS_HeadPtr;
#if (USBD_CFG_HS_EN == DEF_ENABLED)
}
#endif

  for (config_ix = 0u; config_ix < cfg_val; config_ix++) {      // Iterate thru list until to get cfg struct.
    p_config = p_config->NextPtr;
  }
#endif

  return (p_config);
}

/****************************************************************************************************//**
 *                                               USBD_EventSet()
 *
 * @brief    Send an event to the core task.
 *
 * @param    p_drv   Pointer to device driver.
 *
 * @param    event   Event code :
 *                   USBD_EVENT_BUS_RESET    Reset.
 *                   USBD_EVENT_BUS_SUSPEND  Suspend.
 *                   USBD_EVENT_BUS_RESUME   Resume.
 *                   USBD_EVENT_BUS_CONN     Connect.
 *                   USBD_EVENT_BUS_DISCONN  Disconnect.
 *                   USBD_EVENT_BUS_HS       High speed.
 *                   USBD_EVENT_EP           Endpoint.
 *                   USBD_EVENT_SETUP        Setup.
 *******************************************************************************************************/
static void USBD_EventSet(USBD_DRV        *p_drv,
                          USBD_EVENT_CODE event)
{
  USBD_CORE_EVENT *p_core_event;

  RTOS_ASSERT_DBG((p_drv != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_core_event = USBD_CoreEventGet();
  if (p_core_event == DEF_NULL) {
    return;
  }

  p_core_event->Type = event;
  p_core_event->DrvPtr = p_drv;
  RTOS_ERR_SET(p_core_event->Err, RTOS_ERR_NONE);

  USBD_OS_CoreEventPut(p_drv->DevNbr,
                       p_core_event);
}

/****************************************************************************************************//**
 *                                           USBD_CoreTaskHandler()
 *
 * @brief    Processes all core events and core operations.
 *
 * @param    dev_nbr     Device number associated to task if one task per device is used, 0 otherwise.
 *******************************************************************************************************/
void USBD_CoreTaskHandler(CPU_INT08U dev_nbr)
{
  USBD_CORE_EVENT *p_core_event;
  USBD_DEV *p_dev;
  USBD_DRV *p_drv;
  CPU_INT08U ep_addr;
  USBD_EVENT_CODE event;
  RTOS_ERR err;

  while (DEF_TRUE) {
    //                                                             Wait for an event.
    p_core_event = (USBD_CORE_EVENT *)USBD_OS_CoreEventGet(dev_nbr, &err);
    if (p_core_event != DEF_NULL) {
      event = p_core_event->Type;
      p_drv = p_core_event->DrvPtr;
      p_dev = USBD_DevRefGet(p_drv->DevNbr);

      if (p_dev != DEF_NULL) {
        switch (event) {                                        // Decode event.
          case USBD_EVENT_BUS_RESET:                            // -------------------- BUS EVENTS --------------------
          case USBD_EVENT_BUS_RESUME:
          case USBD_EVENT_BUS_CONN:
          case USBD_EVENT_BUS_HS:
          case USBD_EVENT_BUS_SUSPEND:
          case USBD_EVENT_BUS_DISCONN:
            USBD_EventProcess(p_dev, event);
            break;

          case USBD_EVENT_EP:                                   // ------------------ ENDPOINT EVENTS -----------------
            if (p_dev->State == USBD_DEV_STATE_SUSPENDED) {
              p_dev->State = p_dev->StatePrev;
            }
            ep_addr = p_core_event->EP_Addr;
            USBD_EP_XferAsyncProcess(p_drv, ep_addr, p_core_event->Err);
            break;

          case USBD_EVENT_SETUP:                                // ------------------- SETUP EVENTS -------------------
            USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevSetupEventNbr);
            if (p_dev->State == USBD_DEV_STATE_SUSPENDED) {
              p_dev->State = p_dev->StatePrev;
            }
            USBD_StdReqHandler(p_dev);
            break;

          default:
            break;
        }
      }

      USBD_CoreEventFree(p_core_event);                         // Return event to free pool.
    }
  }
}

/****************************************************************************************************//**
 *                                           USBD_EventProcess()
 *
 * @brief    Processes bus related events.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    event   Bus related events :
 *                   USBD_EVENT_BUS_RESET    Reset.
 *                   USBD_EVENT_BUS_SUSPEND  Suspend.
 *                   USBD_EVENT_BUS_RESUME   Resume.
 *                   USBD_EVENT_BUS_CONN     Connect.
 *                   USBD_EVENT_BUS_DISCONN  Disconnect.
 *                   USBD_EVENT_BUS_HS       High speed.
 *
 * @note     (1) This prevents a suspend event to overwrite the internal status with a suspend state in
 *               the case of multiple suspend events in a row.
 *
 * @note     (2) USB Spec 2.0 section 9.1.1.6 states "When suspended, the USB device maintains any
 *               internal status, including its address and configuration."
 *
 * @note     (3) A suspend event is usually followed by a resume event when the bus activity comes back.
 *               But in some cases, after a suspend event, a reset event can be notified to the Core
 *               before a resume event. Thus, the internal state of the device should not be changed
 *               to the previous one.
 *******************************************************************************************************/
static void USBD_EventProcess(USBD_DEV        *p_dev,
                              USBD_EVENT_CODE event)
{
  USBD_BUS_FNCTS *p_bus_fnct;
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  p_bus_fnct = p_dev->BusFnctsPtr;

  switch (event) {
    case USBD_EVENT_BUS_RESET:                                  // -------------------- RESET EVENT -------------------
      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevResetEventNbr);
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: Reset"));

      CORE_ENTER_ATOMIC();
      if (p_dev->ConnStatus == DEF_FALSE) {
        p_dev->ConnStatus = DEF_TRUE;
        CORE_EXIT_ATOMIC();

        if ((p_bus_fnct != DEF_NULL)
            && (p_bus_fnct->Conn != DEF_NULL)) {
          p_bus_fnct->Conn(p_dev->Nbr);                         // Call application connect callback.
        }
      } else {
        CORE_EXIT_ATOMIC();
      }

      USBD_CtrlClose(p_dev->Nbr, &err);                         // Close ctrl EP.

      if (p_dev->ConfigCurNbr != USBD_CONFIG_NBR_NONE) {
        USBD_ConfigClose(p_dev);                                // Close curr cfg.
      }

      USBD_CtrlOpen(p_dev->Nbr,                                 // Open ctrl EP.
                    p_dev->EP_CtrlMaxPktSize,
                    &err);

      CORE_ENTER_ATOMIC();                                      // Set dev in default state, reset dev speed.
      p_dev->Addr = 0u;
      p_dev->State = USBD_DEV_STATE_DEFAULT;
      p_dev->Spd = USBD_DEV_SPD_FULL;
      CORE_EXIT_ATOMIC();

      if ((p_bus_fnct != DEF_NULL)
          && (p_bus_fnct->Reset != DEF_NULL)) {
        p_bus_fnct->Reset(p_dev->Nbr);                          // Call application reset callback.
      }
      break;

    case USBD_EVENT_BUS_SUSPEND:                                // ------------------- SUSPEND EVENT ------------------
      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevSuspendEventNbr);
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: Suspend"));

      CORE_ENTER_ATOMIC();
      if (p_dev->State != USBD_DEV_STATE_SUSPENDED) {           // See Note #1.
        p_dev->StatePrev = p_dev->State;                        // Save cur       state (see Note #2).
      }
      p_dev->State = USBD_DEV_STATE_SUSPENDED;                  // Set  suspended state.
      CORE_EXIT_ATOMIC();

      if ((p_bus_fnct != DEF_NULL)
          && (p_bus_fnct->Suspend != DEF_NULL)) {
        p_bus_fnct->Suspend(p_dev->Nbr);                        // Call application suspend callback.
      }
      break;

    case USBD_EVENT_BUS_RESUME:                                 // ------------------- RESUME EVENT -------------------
      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevResumeEventNbr);
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: Resume"));

      CORE_ENTER_ATOMIC();
      if (p_dev->State == USBD_DEV_STATE_SUSPENDED) {           // See Note #3.
        p_dev->State = p_dev->StatePrev;                        // Restore prev state.
      }
      CORE_EXIT_ATOMIC();

      if ((p_bus_fnct != DEF_NULL)
          && (p_bus_fnct->Resume != DEF_NULL)) {
        p_bus_fnct->Resume(p_dev->Nbr);                         // Call application resume callback.
      }
      break;

    case USBD_EVENT_BUS_CONN:                                   // ------------------- CONNECT EVENT ------------------
      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevConnEventNbr);
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: Connect"));

      CORE_ENTER_ATOMIC();
      p_dev->State = USBD_DEV_STATE_ATTACHED;                   // Set attached state.
      p_dev->ConnStatus = DEF_TRUE;
      CORE_EXIT_ATOMIC();

      if ((p_bus_fnct != DEF_NULL)
          && (p_bus_fnct->Conn != DEF_NULL)) {
        p_bus_fnct->Conn(p_dev->Nbr);                           // Call application connect callback.
      }
      break;

    case USBD_EVENT_BUS_DISCONN:                                // ----------------- DISCONNECT EVENT -----------------
      USBD_DBG_STATS_DEV_INC(p_dev->Nbr, DevDisconnEventNbr);
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: Disconnect"));

      USBD_CtrlClose(p_dev->Nbr, &err);                         // Close ctrl EP.

      if (p_dev->ConfigCurNbr != USBD_CONFIG_NBR_NONE) {
        USBD_ConfigClose(p_dev);                                // Close curr cfg.
      }

      CORE_ENTER_ATOMIC();
      p_dev->Addr = 0u;                                         // Set default address.
      p_dev->State = USBD_DEV_STATE_INIT;                       // Dev is not attached.
      p_dev->ConfigCurNbr = USBD_CONFIG_NBR_NONE;               // No active cfg.
      p_dev->ConnStatus = DEF_FALSE;
      CORE_EXIT_ATOMIC();

      if ((p_bus_fnct != DEF_NULL)
          && (p_bus_fnct->Disconn != DEF_NULL)) {
        p_bus_fnct->Disconn(p_dev->Nbr);                        // Call application disconnect callback.
      }
      break;

    case USBD_EVENT_BUS_HS:                                     // ------------ HIGH-SPEED HANDSHAKE EVENT ------------
      LOG_VRB_TO(LOG_USBD_BUS_CH, ("USBD Bus: High Speed detection"));
#if (USBD_CFG_HS_EN == DEF_ENABLED)
      CORE_ENTER_ATOMIC();
      p_dev->Spd = USBD_DEV_SPD_HIGH;
      if (p_dev->State == USBD_DEV_STATE_SUSPENDED) {
        p_dev->State = p_dev->StatePrev;
      }
      CORE_EXIT_ATOMIC();
#endif
      break;

    case USBD_EVENT_EP:
    case USBD_EVENT_SETUP:
    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                           USBD_CoreEventGet()
 *
 * @brief    Gets a new core event from the pool.
 *
 * @return   Pointer to core event, if no errors are returned.
 *
 *           Pointer to NULL,       if any errors are returned.
 *******************************************************************************************************/
static USBD_CORE_EVENT *USBD_CoreEventGet(void)
{
  USBD_CORE_EVENT *p_core_event;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (USBD_Ptr->CoreEventPoolIx < 1u) {                         // Chk if core event is avail.
    CORE_EXIT_ATOMIC();
    return (DEF_NULL);
  }

  USBD_Ptr->CoreEventPoolIx--;
  p_core_event = USBD_Ptr->CoreEventPoolPtrs[USBD_Ptr->CoreEventPoolIx];
  CORE_EXIT_ATOMIC();

  return (p_core_event);
}

/****************************************************************************************************//**
 *                                           USBD_CoreEventFree()
 *
 * @brief    Returns a core event to the pool.
 *
 * @param    p_core_event    Pointer to core event.
 *******************************************************************************************************/
static void USBD_CoreEventFree(USBD_CORE_EVENT *p_core_event)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  USBD_Ptr->CoreEventPoolPtrs[USBD_Ptr->CoreEventPoolIx] = p_core_event;
  USBD_Ptr->CoreEventPoolIx++;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               USBD_IF_RefGet()
 *
 * @brief    Gets the interface structure.
 *
 * @param    p_cfg   Pointer to configuration structure.
 *
 * @param    if_nbr  Interface number.
 *
 * @return   Pointer to interface structure, if no errors are returned.
 *
 *           Pointer to NULL,                if any errors are returned.
 *******************************************************************************************************/
static USBD_IF *USBD_IF_RefGet(const USBD_CONFIG *p_config,
                               CPU_INT08U        if_nbr)
{
  USBD_IF *p_if;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  CPU_INT08U if_ix;
#endif

  if (if_nbr >= p_config->IF_NbrTotal) {                        // Chk if IF nbr is valid.
    return (DEF_NULL);
  }

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Get IF struct.
  p_if = p_config->IF_TblPtrs[if_nbr];
#else
  p_if = p_config->IF_HeadPtr;

  for (if_ix = 0u; if_ix < if_nbr; if_ix++) {
    p_if = p_if->NextPtr;
  }
#endif

  return (p_if);
}

/****************************************************************************************************//**
 *                                           USBD_IF_AltRefGet()
 *
 * @brief    Gets the alternate setting interface structure.
 *
 * @param    p_if        Pointer to interface structure.
 *
 * @param    if_alt_nbr  Alternate setting interface number.
 *
 * @return   Pointer to alternate setting interface structure, if no errors are returned.
 *
 *           Pointer to NULL,                                  if any errors are returned.
 *******************************************************************************************************/
static USBD_IF_ALT *USBD_IF_AltRefGet(const USBD_IF *p_if,
                                      CPU_INT08U    if_alt_nbr)
{
  USBD_IF_ALT *p_if_alt;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  CPU_INT08U if_alt_ix;
#endif

  if (if_alt_nbr >= p_if->AltNbrTotal) {                        // Chk alt setting nbr.
    return (DEF_NULL);
  }

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)                      // Get alt IF struct.
  p_if_alt = p_if->AltTblPtrs[if_alt_nbr];
#else
  p_if_alt = p_if->AltHeadPtr;

  for (if_alt_ix = 0u; if_alt_ix < if_alt_nbr; if_alt_ix++) {
    p_if_alt = p_if_alt->NextPtr;
  }
#endif

  return (p_if_alt);
}

/****************************************************************************************************//**
 *                                               USBD_IF_AltOpen()
 *
 * @brief    Opens all endpoints from the specified alternate setting.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----     Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    if_nbr      Interface number.
 *
 * @param    p_if_alt    Pointer to alternate setting interface.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBD_IF_AltOpen(USBD_DEV          *p_dev,
                            CPU_INT08U        if_nbr,
                            const USBD_IF_ALT *p_if_alt,
                            RTOS_ERR          *p_err)
{
  CPU_INT08U ep_nbr;
  CPU_INT08U ep_phy_nbr;
  CPU_BOOLEAN valid;
  USBD_EP_INFO *p_ep;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CPU_INT32U ep_alloc_map;
#endif
  CORE_DECLARE_IRQ_STATE;

  valid = DEF_OK;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  ep_alloc_map = p_if_alt->EP_TblMap;
  while (ep_alloc_map != DEF_BIT_NONE) {
    ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
    p_ep = p_if_alt->EP_TblPtrs[ep_nbr];
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);

    CORE_ENTER_ATOMIC();
    p_dev->EP_IF_Tbl[ep_phy_nbr] = if_nbr;
    CORE_EXIT_ATOMIC();

    USBD_EP_Open(&p_dev->Drv,
                 p_ep->Addr,
                 p_ep->MaxPktSize,
                 p_ep->Attrib,
                 p_ep->Interval,
                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      valid = DEF_FAIL;
      break;
    }

    DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
  }
#else
  p_ep = p_if_alt->EP_HeadPtr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);

    CORE_ENTER_ATOMIC();
    p_dev->EP_IF_Tbl[ep_phy_nbr] = if_nbr;
    CORE_EXIT_ATOMIC();

    USBD_EP_Open(&p_dev->Drv,
                 p_ep->Addr,
                 p_ep->MaxPktSize,
                 p_ep->Attrib,
                 p_ep->Interval,
                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      valid = DEF_FAIL;
      break;
    }

    p_ep = p_ep->NextPtr;
  }
#endif

  if (valid == DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    USBD_IF_AltClose(p_dev, p_if_alt);
  }
}

/****************************************************************************************************//**
 *                                           USBD_IF_AltClose()
 *
 * @brief    Closes all endpoints from the specified alternate setting.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    p_if_alt    Pointer to alternate setting interface.
 *******************************************************************************************************/
static void USBD_IF_AltClose(USBD_DEV          *p_dev,
                             const USBD_IF_ALT *p_if_alt)
{
  CPU_INT08U ep_nbr;
  CPU_INT08U ep_phy_nbr;
  USBD_EP_INFO *p_ep;
  RTOS_ERR err;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  CPU_INT32U ep_alloc_map;
#endif
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  ep_alloc_map = p_if_alt->EP_TblMap;
  while (ep_alloc_map != DEF_BIT_NONE) {
    ep_nbr = (CPU_INT08U)CPU_CntTrailZeros32(ep_alloc_map);
    p_ep = p_if_alt->EP_TblPtrs[ep_nbr];
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);

    CORE_ENTER_ATOMIC();
    p_dev->EP_IF_Tbl[ep_phy_nbr] = USBD_IF_NBR_NONE;
    CORE_EXIT_ATOMIC();

    USBD_EP_Close(&p_dev->Drv,
                  p_ep->Addr,
                  &err);

    DEF_BIT_CLR(ep_alloc_map, DEF_BIT32(ep_nbr));
  }
#else
  p_ep = p_if_alt->EP_HeadPtr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->EP_NbrTotal; ep_nbr++) {
    ep_phy_nbr = USBD_EP_ADDR_TO_PHY(p_ep->Addr);

    CORE_ENTER_ATOMIC();
    p_dev->EP_IF_Tbl[ep_phy_nbr] = USBD_IF_NBR_NONE;
    CORE_EXIT_ATOMIC();

    USBD_EP_Close(&p_dev->Drv,
                  p_ep->Addr,
                  &err);

    p_ep = p_ep->NextPtr;
  }
#endif
}

/****************************************************************************************************//**
 *                                           USBD_IF_GrpRefGet()
 *
 * @brief    Gets the interface group structure.
 *
 * @param    p_cfg       Pointer to configuration structure.
 *
 * @param    if_grp_nbr  Interface number.
 *
 * @return   Pointer to interface group structure, if no errors are returned.
 *
 *           Pointer to NULL,                      if any errors are returned.
 *******************************************************************************************************/
static USBD_IF_GRP *USBD_IF_GrpRefGet(const USBD_CONFIG *p_config,
                                      CPU_INT08U        if_grp_nbr)
{
  USBD_IF_GRP *p_if_grp;
#if (USBD_CFG_OPTIMIZE_SPD == DEF_DISABLED)
  CPU_INT08U if_grp_ix;
#endif

  RTOS_ASSERT_DBG((if_grp_nbr < p_config->IF_GrpNbrTotal), RTOS_ERR_NULL_PTR, DEF_NULL);

#if (USBD_CFG_OPTIMIZE_SPD == DEF_ENABLED)
  p_if_grp = p_config->IF_GrpTblPtrs[if_grp_nbr];
#else
  p_if_grp = p_config->IF_GrpHeadPtr;

  for (if_grp_ix = 0u; if_grp_ix < if_grp_nbr; if_grp_ix++) {
    p_if_grp = p_if_grp->NextPtr;
  }
#endif

  return (p_if_grp);
}

/****************************************************************************************************//**
 *                                               USBD_StrDescAdd()
 *
 * @brief    Adds the string to the USB device.
 *
 * @param    p_dev   Pointer to device structure.
 *
 * @param    -----   Argument validated in the caller(s).
 *
 * @param    p_str   Pointer to string to add (see Note #1).
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) USB spec 2.0 chapter 9.5 states "Where appropriate, descriptors contain references
 *               to string descriptors that provide displayable information describing a descriptor
 *               in human-readable form. The inclusion of string descriptors is optional.  However,
 *               the reference fields within descriptors are mandatory. If a device does not support
 *               string descriptors, string reference fields must be reset to zero to indicate no
 *               string descriptor is available.
 *
 *               Since string descriptors are optional, 'p_str' could be a NULL pointer.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
static void USBD_StrDescAdd(USBD_DEV       *p_dev,
                            const CPU_CHAR *p_str,
                            RTOS_ERR       *p_err)
{
  CPU_INT08U str_ix;
  CORE_DECLARE_IRQ_STATE;

  if (p_str == DEF_NULL) {                                      // Return if NULL ptr.
    return;
  }

  for (str_ix = 0u; str_ix < p_dev->StrMaxIx; str_ix++) {
    if (p_str == p_dev->StrDesc_Tbl[str_ix]) {                  // Str already stored in tbl.
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return;
    }
  }

  CORE_ENTER_ATOMIC();
  str_ix = p_dev->StrMaxIx;                                     // Get curr str tbl ix.

  if (str_ix >= USBD_Ptr->StrQtyPerDev) {                       // Chk if str can be stored in tbl.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_dev->StrDesc_Tbl[str_ix] = (CPU_CHAR *)p_str;
  p_dev->StrMaxIx++;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               USBD_StrDescGet()
 *
 * @brief    Gets the string pointer.
 *
 * @param    p_dev       Pointer to device.
 *
 * @param    -----    Argument validate by the caller(s).
 *
 * @param    str_nbr     Number of the string to obtain.
 *
 * @return   Pointer to requested string, if no errors are returned.
 *
 *           Pointer to NULL,             if any errors are returned.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
static const CPU_CHAR *USBD_StrDescGet(const USBD_DEV *p_dev,
                                       CPU_INT08U     str_nbr)
{
  const CPU_CHAR *p_str;

  if (str_nbr > p_dev->StrMaxIx) {
    return (DEF_NULL);
  }

  p_str = p_dev->StrDesc_Tbl[str_nbr];
  return (p_str);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_StrDescIxGet()
 *
 * @brief    Get string index.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    p_str   Pointer to string.
 *
 * @return   String index.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == DEF_ENABLED)
static CPU_INT08U USBD_StrDescIxGet(const USBD_DEV *p_dev,
                                    const CPU_CHAR *p_str)
{
  CPU_INT08U str_ix;

  if (p_str == DEF_NULL) {                                      // Return if a NULL pointer.
    return (0u);
  }

  for (str_ix = 0u; str_ix < p_dev->StrMaxIx; str_ix++) {
    if (p_str == p_dev->StrDesc_Tbl[str_ix]) {                  // Str already stored in tbl.
      return (str_ix + 1u);
    }
  }

  return (0u);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_AVAIL))
