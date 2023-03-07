/***************************************************************************//**
 * @file
 * @brief USB Host - Mass Storage Class (Msc)
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

#if (defined(RTOS_MODULE_USB_HOST_MSC_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST MSC class requires USB Host Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_MSC_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/kal.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_msc.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (USBH, CLASS, MSD)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 *                                               DEBUG STATS
 *******************************************************************************************************/

#define  USBH_DBG_MSC_STATS_EN                          DEF_DISABLED

#if (USBH_DBG_MSC_STATS_EN == DEF_ENABLED)
#define  USBH_DBG_MSC_DEV_QTY                          10u
#define  USBH_DBG_MSC_DEV_LUN_QTY                       5u

#define  USBH_DBG_MSC_STATS_RESET()                     Mem_Clr((void *)&USBH_DbgMscStats, \
                                                                (CPU_SIZE_T) sizeof(USBH_DBG_MSC_STATS));

#define  USBH_DBG_MSC_STATS_LU_RESET(msc_nbr, lun)      Mem_Clr((void *)&USBH_DbgMscStats.LU_Tbl[msc_nbr][lun], \
                                                                (CPU_SIZE_T) sizeof(USBH_DBG_MSC_LUN_STATS));

#define  USBH_DBG_MSC_STATS_INC(stat)                   USBH_DbgMscStats.stat++;

#define  USBH_DBG_MSC_STATS_LU_INC(msc_nbr, lun, stat)  { \
    if (lun < USBH_DBG_MSC_DEV_LUN_QTY) {                 \
      USBH_DbgMscStats.LU_Tbl[msc_nbr][lun].stat++;       \
    }                                                     \
}
#else
#define  USBH_DBG_MSC_STATS_RESET()
#define  USBH_DBG_MSC_STATS_LU_RESET(msc_nbr, lun)
#define  USBH_DBG_MSC_STATS_INC(stat)
#define  USBH_DBG_MSC_STATS_LU_INC(ep_phy_nbr, dev_addr, stat)
#endif

/********************************************************************************************************
 *                                               MISCELLANEOUS
 *******************************************************************************************************/

#define  USBH_MSC_STD_REQ_TIMEOUT_DFLT                      5000u
#define  USBH_MSC_MAX_RETRY                                    3u

/********************************************************************************************************
 *                                           SUBCLASS CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 2.
 *******************************************************************************************************/

#define  USBH_MSC_SUBCLASS_CODE_RBC                         0x01u
#define  USBH_MSC_SUBCLASS_CODE_SFF_8020i                   0x02u
#define  USBH_MSC_SUBCLASS_CODE_MMC_5                       0x02u
#define  USBH_MSC_SUBCLASS_CODE_QIC_157                     0x03u
#define  USBH_MSC_SUBCLASS_CODE_UFI                         0x04u
#define  USBH_MSC_SUBCLASS_CODE_SFF_8070i                   0x05u
#define  USBH_MSC_SUBCLASS_CODE_SCSI                        0x06u
#define  USBH_MSC_SUBCLASS_CODE_LSDFS                       0x07u
#define  USBH_MSC_SUBCLASS_CODE_IEEE_1667                   0x08u

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 3.
 *******************************************************************************************************/

#define  USBH_MSC_PROTOCOL_CODE_CTRL_BULK_INTR_CMD_INTR     0x00u
#define  USBH_MSC_PROTOCOL_CODE_CTRL_BULK_INTR              0x01u
#define  USBH_MSC_PROTOCOL_CODE_BULK_ONLY                   0x50u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 3 for more details.
 *
 *           (2) The mass storage reset request is "used to reset the mass storage device and its
 *               associated interface".  The setup request packet will consist of :
 *
 *               (a) bmRequestType = 00100001b (class, interface, host-to-device)
 *               (b) bRequest      =     0xFF
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0000
 *
 *           (3) The get max LUN is used to determine the number of LUN's supported by the device.  The
 *               setup request packet will consist of :
 *
 *               (a) bmRequestType = 10100001b (class, interface, device-to-host)
 *               (b) bRequest      =     0xFE
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0001
 *******************************************************************************************************/

#define  USBH_MSC_REQ_MASS_STORAGE_RESET                    0xFFu // See Note #2.
#define  USBH_MSC_REQ_GET_MAX_LUN                           0xFEu // See Note #3.

#define  USBH_MSC_REQ_GET_MAX_LUN_INVALID                   0xFFu

/********************************************************************************************************
 *                                       COMMAND BLOCK WRAPPER (CBW)
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1 for more details about
 *               the Command Block Wrapper structure definition.
 *******************************************************************************************************/

#define  USBH_MSC_CBW_SIG                             0x43425355u
#define  USBH_MSC_CBW_LEN                                     31u
#define  USBH_MSC_CSW_LEN                                     13u

#define  USBH_MSC_CBW_CMD_BLK_MAX_LEN                         16u

#define  USBH_MSC_CBW_DATA_DIR_NONE                         0x01u
#define  USBH_MSC_CBW_DATA_DIR_OUT                          0x00u
#define  USBH_MSC_CBW_DATA_DIR_IN                      DEF_BIT_07

/********************************************************************************************************
 *                                       COMMAND STATUS WRAPPER (CSW)
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.2 for more details about
 *               the Command Status Wrapper structure definition.
 *
 *           (2) The 'bCSWStatus' field of a command status wrapper may contain one of these values.
 *               See 'USB Mass Storage Class - Bulk Only Transport', Table 5.3.
 *******************************************************************************************************/

#define  USBH_MSC_CSW_SIG                             0x53425355u
#define  USBH_MSC_CSW_LEN                                     13u
//                                                                 See Note #2.
#define  USBH_MSC_CSW_STATUS_CMD_PASSED                     0x00u
#define  USBH_MSC_CSW_STATUS_CMD_FAILED                     0x01u
#define  USBH_MSC_CSW_STATUS_PHASE_ERR                      0x02u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_MSC_INIT_CFG USBH_MSC_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 1u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 1u, .RespBufQty = 2u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           MSC TRANSPORT API
 *******************************************************************************************************/

typedef struct usbh_msc_transport_api {
  void (*Init)(RTOS_ERR *p_err);

  void *(*RespBufGet)(CPU_INT16U required_buf_len,
                      void       *p_arg,
                      RTOS_ERR   *p_err);

  void (*RespBufFree)(void     *p_resp_buf,
                      void     *p_arg,
                      RTOS_ERR *p_err);

  CPU_INT32U (*Rx)(CPU_INT08U  lun,
                   void        *p_cmd_blk,
                   CPU_INT08U  cmd_blk_len,
                   void        *p_buf,
                   CPU_INT32U  buf_len,
                   CPU_INT32U  timeout,
                   void        *p_arg,
                   CPU_BOOLEAN *p_cmd_status,
                   RTOS_ERR    *p_err);

  CPU_INT32U (*Tx)(CPU_INT08U  lun,
                   void        *p_cmd_blk,
                   CPU_INT08U  cmd_blk_len,
                   void        *p_buf,
                   CPU_INT32U  buf_len,
                   CPU_INT32U  timeout,
                   void        *p_arg,
                   CPU_BOOLEAN *p_cmd_status,
                   RTOS_ERR    *p_err);
} USBH_MSC_TRANSPORT_API;

/********************************************************************************************************
 *                                       MSC FUNCTION STRUCTURE
 *
 * Note(s) : (1) A shared buffer is used for the Command Block Wrapper (CBW) and the Command Status
 *               Wrapper (CSW). Since the Bulk-Only Transport (BOT) protocol is a sequential protocol,
 *               it is safe to use a shared buffer. It allows to safe some RAM space.
 *******************************************************************************************************/

typedef struct usbh_msc_fnct_data {
  USBH_DEV_HANDLE           DevHandle;                          // Handle to device.
  USBH_FNCT_HANDLE          FnctHandle;                         // Handle to function.
  USBH_EP_HANDLE            EP_InHandle;                        // Handle to EP IN used for comm.
  USBH_EP_HANDLE            EP_OutHandle;                       // Handle to EP OUT used for comm.

  CPU_INT08U                MaxLUN;                             // Max nbr of logical units for a given MSC function.
  CPU_INT16U                ID;                                 // Unique ID assigned by MSC layer.

  CPU_INT08U                *CmdStatusBufPtr;                   // Ptr to buf used for CBW and CSW (see Note #1).
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  struct usbh_msc_fnct_data *NextPtr;                           // Ptr to next MSC fnct struct.
#endif
} USBH_MSC_FNCT_DATA;

/********************************************************************************************************
 *                                           MSC DATA STRUCTURE
 *******************************************************************************************************/

typedef struct usbh_msc {
  CPU_INT32U             StdReqTimeoutMs;                       // Timeout, in milliseconds, for std requests.
  USBH_CLASS_FNCT_POOL   FnctPool;                              // Pool of MSC functions.
  USBH_MSC_CMD_BLK_FNCTS *CmdBlkFnctsPtr;                       // Ptr to cmd blk notification fncts.
  MEM_DYN_POOL           RespBufPool;                           // Pool of data IN buf for some Cmd Blocks.
} USBH_MSC;

/********************************************************************************************************
 *                                           COMMAND BLOCK WRAPPER
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1 for more details about
 *               the Command Block Wrapper (CBW)
 *******************************************************************************************************/

typedef struct usbh_msc_cbw {
  CPU_INT32U dCBWSignature;                                     // Signature to identify this data pkt as CBW.
  CPU_INT32U dCBWTag;                                           // Command block tag sent by host.
  CPU_INT32U dCBWDataTransferLength;                            // Number of bytes of data that host expects to xfer.
  CPU_INT08U bmCBWFlags;                                        // Bitmap encoding mainly xfer dir.
  CPU_INT08U bCBWLUN;                                           // LUN to which the command block is being sent.
  CPU_INT08U bCBWCBLength;                                      // Length of CBWCB in bytes.
  CPU_INT08U CBWCB[USBH_MSC_CBW_CMD_BLK_MAX_LEN];               // Command block to be executed by device.
} USBH_MSC_CBW;

/********************************************************************************************************
 *                                       COMMAND STATUS WRAPPER
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.2 for more details about
 *               the Command Status Wrapper (CSW)
 *******************************************************************************************************/

typedef struct usbh_msc_csw {
  CPU_INT32U dCSWSignature;                                     // Signature to identify this data pkt as CSW.
  CPU_INT32U dCSWTag;                                           // Device shall set this to value in CBW's dCBWTag.
  CPU_INT32U dCSWDataResidue;                                   // Difference between expected & actual nbr data bytes.
  CPU_INT08U bCSWStatus;                                        // Indicates success or failure of command.
} USBH_MSC_CSW;

/********************************************************************************************************
 *                                           STATISTICS DATA TYPE
 *******************************************************************************************************/

#if (USBH_DBG_MSC_STATS_EN == DEF_ENABLED)
typedef struct usbh_dbg_msc_lun_stats {
  CPU_INT32U CBW_Cnt;
  CPU_INT32U DataPhaseCnt;
  CPU_INT32U NoDataPhaseCnt;
  CPU_INT32U CSW_Cnt;
  CPU_INT32U CmdSuccessCnt;
  CPU_INT32U CmdErrCnt;
} USBH_DBG_MSC_LUN_STATS;

typedef struct usbh_dbg_msc_stats {
  CPU_INT32U             ProbeFnctCnt;
  CPU_INT32U             EP_ProbeCnt;
  CPU_INT32U             EP_OpenCnt;
  CPU_INT32U             EP_CloseCnt;
  CPU_INT32U             ConnCnt;
  CPU_INT32U             DisconnCnt;
  CPU_INT32U             SuspendCnt;
  CPU_INT32U             ResumeCnt;
  USBH_DBG_MSC_LUN_STATS LU_Tbl[USBH_DBG_MSC_DEV_QTY][USBH_DBG_MSC_DEV_LUN_QTY];
} USBH_DBG_MSC_STATS;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_MSC *USBH_MSC_Ptr = DEF_NULL;

static CPU_INT16U USBH_MSC_DevID_Gen = 0u;                      // Generator of unique MSC dev IDs.

#if (USBH_DBG_MSC_STATS_EN == DEF_ENABLED)
USBH_DBG_MSC_STATS USBH_DbgMscStats;
#endif

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_MSC_INIT_CFG USBH_MSC_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u, .RespBufQty = 0u }
#endif
};
#else
extern USBH_MSC_INIT_CFG USBH_MSC_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- CLASS DRIVER FUNCTION ---------------
static CPU_BOOLEAN USBH_MSC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err);

static CPU_BOOLEAN USBH_MSC_EP_Probe(void        *p_class_fnct,
                                     CPU_INT08U  if_ix,
                                     CPU_INT08U  ep_type,
                                     CPU_BOOLEAN ep_dir_in);

static void USBH_MSC_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_nbr,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in);

static void USBH_MSC_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_nbr);

static void USBH_MSC_Conn(void *p_class_fnct);

static void USBH_MSC_Disconn(void *p_class_fnct);

static void USBH_MSC_Suspend(void *p_class_fnct);

static void USBH_MSC_Resume(void *p_class_fnct);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_MSC_UnInit(void);
#endif

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_MSC_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct);
#endif
//                                                                 ---------------- MSC TRANSPORT API -----------------
static void *USBH_MSC_RespBufGet(CPU_INT16U required_buf_len,
                                 void       *p_arg,
                                 RTOS_ERR   *p_err);

static void USBH_MSC_RespBufFree(void     *p_resp_buf,
                                 void     *p_arg,
                                 RTOS_ERR *p_err);

static CPU_INT32U USBH_MSC_Rx(CPU_INT08U  lun,
                              void        *p_cmd_blk,
                              CPU_INT08U  cmd_blk_len,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT32U  timeout,
                              void        *p_arg,
                              CPU_BOOLEAN *p_cmd_status,
                              RTOS_ERR    *p_err);

static CPU_INT32U USBH_MSC_Tx(CPU_INT08U  lun,
                              void        *p_cmd_blk,
                              CPU_INT08U  cmd_blk_len,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT32U  timeout,
                              void        *p_arg,
                              CPU_BOOLEAN *p_cmd_status,
                              RTOS_ERR    *p_err);

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static CPU_BOOLEAN USBH_MSC_FnctAllocCallback(void *p_class_fnct_data);

static CPU_INT08U USBH_MSC_MaxLUN_Get(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                      RTOS_ERR           *p_err);

static CPU_INT32U USBH_MSC_CmdBlkExe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                     CPU_INT08U         lun,
                                     CPU_INT08U         dir,
                                     void               *p_cmd_blk,
                                     CPU_INT08U         cmd_blk_len,
                                     CPU_INT08U         *p_buf,
                                     CPU_INT32U         buf_len,
                                     CPU_INT32U         timeout,
                                     CPU_BOOLEAN        *p_cmd_status,
                                     RTOS_ERR           *p_err);

static void USBH_MSC_CBW_Exe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             CPU_INT08U         *p_cbw_buf,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err);

static void USBH_MSC_CSW_Exe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             USBH_MSC_CSW       *p_msc_csw,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err);

static void USBH_MSC_DataExe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             CPU_INT08U         dir,
                             CPU_INT08U         *p_buf,
                             CPU_INT32U         buf_len,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err);

static void USBH_MSC_ResetRecovery(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                   RTOS_ERR           *p_err);

static void USBH_MSC_BulkOnlyReset(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                   RTOS_ERR           *p_err);

static void USBH_MSC_CSW_Parse(USBH_MSC_CSW *p_csw,
                               void         *p_buf_src);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_FNCT_SUBCLASS  Must be enabled"
#endif

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_FNCT_PROTOCOL  Must be enabled"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                   INITIALIZED GLOBAL VARIABLES ACCESSES BY OTHER MODULES/OBJECTS
 *******************************************************************************************************/

USBH_CLASS_DRV USBH_MSC_ClassDrv = {
  DEF_NULL,
  USBH_MSC_ProbeFnct,
  USBH_MSC_EP_Probe,
  USBH_MSC_EP_Open,
  USBH_MSC_EP_Close,
  USBH_MSC_Conn,
  DEF_NULL,
  USBH_MSC_Disconn,
  USBH_MSC_Suspend,
  USBH_MSC_Resume,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_MSC_UnInit,
#else
  DEF_NULL,
#endif
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_MSC_TraceDump,
#endif
  (CPU_CHAR *)"MSC",
};

USBH_MSC_TRANSPORT_API USBH_MSC_TransportApi = {
  DEF_NULL,
  USBH_MSC_RespBufGet,
  USBH_MSC_RespBufFree,
  USBH_MSC_Rx,
  USBH_MSC_Tx
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_MSC_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the MSC class is initialized via the
 *               USBH_MSC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_MSC_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_MSC_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_MSC_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the MSC class is initialized via the
 *               USBH_MSC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_MSC_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_MSC_InitCfg.MemSegPtr = p_mem_seg;
  USBH_MSC_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_MSC_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the MSC class is initialized via the
 *               USBH_MSC_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_MSC_ConfigureOptimizeSpdCfg(const USBH_MSC_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_MSC_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_MSC_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the MSC class is initialized via the
 *               USBH_MSC_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_MSC_ConfigureInitAllocCfg(const USBH_MSC_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_MSC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_MSC_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_MSC_Init()
 *
 * @brief    Initializes The Mass Storage Class (MSC) driver.
 *
 * @param    p_cmd_blk_fncts     Pointer to the Command Block layer API structure.
 *                               [Content MUST be persistent]
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *
 * @note     (1) MSC layer queries the Command Block layer for the maximum response buffer length that
 *               may be needed during the Data IN phase processing of the Bulk-Only Transport protocol.
 *******************************************************************************************************/
void USBH_MSC_Init(const USBH_MSC_CMD_BLK_FNCTS *p_cmd_blk_fncts,
                   RTOS_ERR                     *p_err)
{
  CPU_INT08U class_fnct_qty;
  CPU_INT16U max_resp_buf_len;
  CPU_SIZE_T resp_buf_qty_init;
  CPU_SIZE_T resp_buf_qty_max;
  USBH_MSC   *p_msc;
  CORE_DECLARE_IRQ_STATE;

  if (USBH_MSC_Ptr != DEF_NULL) {                               // Chk if MSC already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_cmd_blk_fncts != DEF_NULL)
                           || (p_cmd_blk_fncts->Conn != DEF_NULL)
                           || (p_cmd_blk_fncts->Disconn != DEF_NULL)
                           || (p_cmd_blk_fncts->MaxRespBufLenGet != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_MSC_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_MSC_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_MSC_InitCfg.OptimizeSpd.FnctQty == USBH_MSC_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  //                                                               Alloc MSC data.
  p_msc = (USBH_MSC *)Mem_SegAlloc("USBH - MSC Data",
                                   USBH_MSC_InitCfg.MemSegPtr,
                                   sizeof(USBH_MSC),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_msc->CmdBlkFnctsPtr = (USBH_MSC_CMD_BLK_FNCTS *)p_cmd_blk_fncts;
  p_msc->StdReqTimeoutMs = USBH_MSC_STD_REQ_TIMEOUT_DFLT;

  CORE_ENTER_ATOMIC();
  USBH_MSC_Ptr = p_msc;
  CORE_EXIT_ATOMIC();

  //                                                               Create pool of MSC functions.
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  class_fnct_qty = USBH_MSC_InitCfg.InitAlloc.FnctQty;
  resp_buf_qty_init = USBH_MSC_InitCfg.InitAlloc.RespBufQty;
  resp_buf_qty_max = USBH_MSC_InitCfg.InitAlloc.RespBufQty;
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  class_fnct_qty = USBH_MSC_InitCfg.OptimizeSpd.FnctQty;
  resp_buf_qty_init = 0u;
  resp_buf_qty_max = LIB_MEM_BLK_QTY_UNLIMITED;
#else
  class_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
  resp_buf_qty_init = 0u;
  resp_buf_qty_max = LIB_MEM_BLK_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - MSC Fnct",
                           &USBH_MSC_Ptr->FnctPool,
                           USBH_MSC_InitCfg.MemSegPtr,
                           sizeof(USBH_MSC_FNCT_DATA),
                           class_fnct_qty,
                           USBH_MSC_FnctAllocCallback,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               Alloc buffers used for cmd blks that need a resp...
  //                                                               ...buf during the Data IN phase.
  //                                                               See Note #1.
  max_resp_buf_len = USBH_MSC_Ptr->CmdBlkFnctsPtr->MaxRespBufLenGet();

  Mem_DynPoolCreateHW("USBH - Response buf pool for Data IN",
                      &USBH_MSC_Ptr->RespBufPool,
                      USBH_MSC_InitCfg.MemSegBufPtr,
                      max_resp_buf_len,
                      USBH_MSC_InitCfg.BufAlignOctets,
                      resp_buf_qty_init,
                      resp_buf_qty_max,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ClassDrvReg(&USBH_MSC_ClassDrv, p_err);                  // Reg MSC driver to the host core.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_DBG_MSC_STATS_RESET();
}

/****************************************************************************************************//**
 *                                       USBH_MSC_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the MSC standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_MSC_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_MSC_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_MSC_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_MSC_FnctAllocCallback()
 *
 * @brief    Callback called each time a new MSC function is allocated.
 *
 * @param    p_class_fnct_data   Pointer to the class function data.
 *
 * @return   DEF_OK,   if NO error(s),
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_MSC_FnctAllocCallback(void *p_class_fnct_data)
{
  USBH_MSC_FNCT_DATA *p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)p_class_fnct_data;
  RTOS_ERR           err_lib;

  //                                                               Alloc buffer used for CBW/CSW.
  p_msc_fnct_data->CmdStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - CBW/CSW buf",
                                                                  USBH_MSC_InitCfg.MemSegBufPtr,
                                                                  USBH_MSC_CBW_LEN,
                                                                  USBH_MSC_InitCfg.BufAlignOctets,
                                                                  DEF_NULL,
                                                                  &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating cmd-status buffer failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_MaxLUN_Get()
 *
 * @brief    Gets maximum logical unit number (LUN) supported by MSC device.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @return   Maximum number of logical unit.
 *
 * @note     (1) See specification "USB MSC Bulk-Only Transport, Revision 1.0", section 3.2 for more
 *               details about the class request "GET MAX LUN".
 *******************************************************************************************************/
static CPU_INT08U USBH_MSC_MaxLUN_Get(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                      RTOS_ERR           *p_err)
{
  CPU_INT08U if_nbr;
  CPU_INT08U max_lun;
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  if_nbr = USBH_IF_NbrGet(p_msc_fnct_data->DevHandle,
                          p_msc_fnct_data->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_MSC_REQ_GET_MAX_LUN_INVALID);
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_MSC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_msc_fnct_data->DevHandle,
                         USBH_MSC_REQ_GET_MAX_LUN,              // See Note #1.
                         (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         if_nbr,
                         p_msc_fnct_data->CmdStatusBufPtr,
                         1u,
                         1u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR local_err;

    max_lun = USBH_MSC_REQ_GET_MAX_LUN_INVALID;

    USBH_EP_Abort(p_msc_fnct_data->DevHandle,
                  USBH_EP_HANDLE_CTRL,
                  &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      RTOS_ERR_COPY(*p_err, local_err);
    } else if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_EP_STALL) {
      //                                                           Device may stall if no multiple LUNs support...
      max_lun = 0u;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // ...That case is a normal situation.
    }
  } else {
    max_lun = p_msc_fnct_data->CmdStatusBufPtr[0];
  }

  return (max_lun);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_RespBufGet()
 *
 * @brief    Provides a response buffer to the Command Block upper layer. This buffer will receive
 *           data send by the MSC device during the Data IN phase of Bulk-Only Transport protocol.
 *
 * @param    required_buf_len    Buffer length required for the response buffer.
 *
 * @param    p_arg               Pointer to the MSC private data.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @return   Pointer to the response buffer, if NO error(s).
 *           Null pointer,               otherwise.
 *******************************************************************************************************/
static void *USBH_MSC_RespBufGet(CPU_INT16U required_buf_len,
                                 void       *p_arg,
                                 RTOS_ERR   *p_err)
{
  void *p_resp_buf;

  PP_UNUSED_PARAM(required_buf_len);
  PP_UNUSED_PARAM(p_arg);

  p_resp_buf = Mem_DynPoolBlkGet(&USBH_MSC_Ptr->RespBufPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Getting response buffer failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (DEF_NULL);
  }

  return (p_resp_buf);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_RespBufFree()
 *
 * @brief    Frees the response buffer that was used by the Command Block upper layer.
 *
 * @param    p_resp_buf  Pointer to the response buffer to free.
 *
 * @param    p_arg       Pointer to the MSC private data.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *******************************************************************************************************/
static void USBH_MSC_RespBufFree(void     *p_resp_buf,
                                 void     *p_arg,
                                 RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_arg);

  Mem_DynPoolBlkFree(&USBH_MSC_Ptr->RespBufPool,
                     p_resp_buf,
                     p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               USBH_MSC_Rx()
 *
 * @brief    Receives data from MSC device.
 *
 * @param    lun             Logical unit number.
 *
 * @param    p_cmd_blk       Pointer to the command block.
 *
 * @param    cmd_blk_len     Command block length.
 *
 * @param    p_buf           Pointer to the receive data buffer.
 *
 * @param    buf_len         Receive data buffer length.
 *
 * @param    timeout         Timeout in ms.
 *
 * @param    p_arg           Pointer to the MSC private data.
 *
 * @param    p_cmd_status    Pointer to the variable that will receive the status of the command execution.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   Number of octets received.
 *******************************************************************************************************/
static CPU_INT32U USBH_MSC_Rx(CPU_INT08U  lun,
                              void        *p_cmd_blk,
                              CPU_INT08U  cmd_blk_len,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT32U  timeout,
                              void        *p_arg,
                              CPU_BOOLEAN *p_cmd_status,
                              RTOS_ERR    *p_err)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_arg;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  CPU_INT32U             nbr_octets_rxd;

  RTOS_ASSERT_DBG_ERR_SET((cmd_blk_len <= USBH_MSC_CBW_CMD_BLK_MAX_LEN), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                timeout,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (0u);
  }
  //                                                               Execute cmd blk.
  nbr_octets_rxd = USBH_MSC_CmdBlkExe(p_msc_fnct_data,
                                      lun,
                                      USBH_MSC_CBW_DATA_DIR_IN,
                                      p_cmd_blk,
                                      cmd_blk_len,
                                      (CPU_INT08U *)p_buf,
                                      buf_len,
                                      timeout,
                                      p_cmd_status,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Executing READ command block failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  return (nbr_octets_rxd);
}

/****************************************************************************************************//**
 *                                               USBH_MSC_Tx()
 *
 * @brief    Sends data to MSC device.
 *
 * @param    lun             Logical unit number.
 *
 * @param    p_cmd_blk       Pointer to the command block.
 *
 * @param    cmd_blk_len     Command block length.
 *
 * @param    p_buf           Pointer to the transmit data buffer.
 *
 * @param    buf_len         Transmit data buffer length.
 *
 * @param    timeout         Timeout in ms.
 *
 * @param    p_arg           Pointer to the MSC private data.
 *
 * @param    p_cmd_status    Pointer to the variable that will receive the status of the command execution.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   Number of octets transmitted.
 *******************************************************************************************************/
static CPU_INT32U USBH_MSC_Tx(CPU_INT08U  lun,
                              void        *p_cmd_blk,
                              CPU_INT08U  cmd_blk_len,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT32U  timeout,
                              void        *p_arg,
                              CPU_BOOLEAN *p_cmd_status,
                              RTOS_ERR    *p_err)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_arg;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  CPU_INT32U             nbr_octets_txd;
  CPU_INT08U             dir;

  RTOS_ASSERT_DBG_ERR_SET((cmd_blk_len <= USBH_MSC_CBW_CMD_BLK_MAX_LEN), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                timeout,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (0u);
  }
  //                                                               Execute cmd blk.
  dir = ((buf_len == 0u) ? USBH_MSC_CBW_DATA_DIR_NONE : USBH_MSC_CBW_DATA_DIR_OUT);
  nbr_octets_txd = USBH_MSC_CmdBlkExe(p_msc_fnct_data,
                                      lun,
                                      dir,
                                      p_cmd_blk,
                                      cmd_blk_len,
                                      (CPU_INT08U *)p_buf,
                                      buf_len,
                                      timeout,
                                      p_cmd_status,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Executing WRITE command block failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  return (nbr_octets_txd);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_ProbeFnct()
 *
 * @brief    Determines if class driver can handle connected USB function.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    class_code      Function class code
 *
 * @param    pp_class_fnct   Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if function should     be handled by this class driver.
 *           DEF_FAIL, if function should not be handled by this class driver.
 *
 * @note     (1) This function is called by the core to determine if a USB device can be supported
 *               by this class driver. At the moment of this call, the device is not ready for
 *               communication.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_MSC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err)
{
  CPU_INT08U             subclass;
  CPU_INT08U             protocol;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data = DEF_NULL;

  USBH_DBG_MSC_STATS_INC(ProbeFnctCnt);

  if (class_code != USBH_CLASS_CODE_MASS_STORAGE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  } else {
    //                                                             Retrieve MSC subclass code.
    subclass = USBH_FnctSubclassGet(dev_handle,
                                    fnct_handle,
                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_OK);
    }

    if ((subclass != USBH_MSC_SUBCLASS_CODE_SCSI)
        && (subclass != USBH_MSC_SUBCLASS_CODE_SFF_8070i)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      return (DEF_OK);
    }
    //                                                             Retrieve MSC protocol code.
    protocol = USBH_FnctProtocolGet(dev_handle,
                                    fnct_handle,
                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_OK);
    }

    if (protocol != USBH_MSC_PROTOCOL_CODE_BULK_ONLY) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      return (DEF_OK);
    }

    //                                                             Alloc a MSC function.
    class_fnct_handle = USBH_ClassFnctAlloc(&USBH_MSC_Ptr->FnctPool,
                                            dev_handle,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_MSC_FNCT_ALLOC);
      return (DEF_OK);
    }
    //                                                             Get MSC function data.
    p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                  class_fnct_handle,
                                                                  DEF_YES,
                                                                  OP_LOCK_TIMEOUT_INFINITE,
                                                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      goto end_rel;
    }

    *pp_class_fnct = (void *)(CPU_ADDR)class_fnct_handle;       // Return specific MSC fnct handle to core.

    //                                                             Store some info for internal usage.
    p_msc_fnct_data->DevHandle = dev_handle;
    p_msc_fnct_data->FnctHandle = fnct_handle;
    p_msc_fnct_data->EP_InHandle = USBH_EP_HANDLE_INVALID;
    p_msc_fnct_data->EP_OutHandle = USBH_EP_HANDLE_INVALID;
    p_msc_fnct_data->ID = USBH_MSC_DevID_Gen;                   // Assigned a unique ID to this MSC dev.
    USBH_MSC_DevID_Gen++;                                       // Nxt avail MSC dev ID.

    USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_YES);
    return (DEF_OK);
  }

end_rel:
  if (class_fnct_handle != USBH_CLASS_FNCT_HANDLE_INVALID) {
    RTOS_ERR err_free;

    USBH_ClassFnctFree(&USBH_MSC_Ptr->FnctPool,
                       class_fnct_handle,
                       &err_free);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_free) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_EP_Probe()
 *
 * @brief    Probes class driver before opening endpoint to ensure class driver needs it.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Endpoint type.
 *                               - USBH_EP_TYPE_INTR
 *                               - USBH_EP_TYPE_BULK
 *                               - USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                               - DEF_YES     Direction IN
 *                               - DEF_NO      Direction OUT
 *
 * @return   DEF_YES, if endpoint is needed.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) Some rare memory sticks may present a MSC interface with an interrupt endpoint
 *               besides the required bulk endpoints' pair. In that case, the host core should NOT
 *               open this interrupt endpoint as the Bulk-Only Transport protocol does not use it.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_MSC_EP_Probe(void        *p_class_fnct,
                                     CPU_INT08U  if_ix,
                                     CPU_INT08U  ep_type,
                                     CPU_BOOLEAN ep_dir_in)
{
  CPU_BOOLEAN            ep_needed = DEF_YES;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  RTOS_ERR               err_usbh;

  PP_UNUSED_PARAM(ep_dir_in);
  PP_UNUSED_PARAM(if_ix);

  USBH_DBG_MSC_STATS_INC(EP_ProbeCnt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                OP_LOCK_TIMEOUT_INFINITE,
                                                                &err_usbh);
  PP_UNUSED_PARAM(p_msc_fnct_data);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return (DEF_NO);
  }

  if (ep_type != USBH_EP_TYPE_BULK) {                           // See Note #1.
    ep_needed = DEF_NO;
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
  return (ep_needed);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_EP_Open()
 *
 * @brief    Stores possible information about the endpoint opened by core on given function's
 *           interface.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Endpoint type.
 *                               - USBH_EP_TYPE_INTR
 *                               - USBH_EP_TYPE_BULK
 *                               - USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                               - DEF_YES     Direction IN
 *                               - DEF_NO      Direction OUT
 *
 * @note     (1) This function will be called by the core for each endpoint declared by the function
 *               via endpoint descriptors and that were successfully opened.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_ix,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  RTOS_ERR               err_usbh;

  PP_UNUSED_PARAM(if_ix);
  USBH_DBG_MSC_STATS_INC(EP_OpenCnt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                OP_LOCK_TIMEOUT_INFINITE,
                                                                &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  if (ep_type == USBH_EP_TYPE_BULK) {                           // Store bulk EPs handles for BOT comm.
    if (ep_dir_in == DEF_YES) {
      p_msc_fnct_data->EP_InHandle = ep_handle;
    } else {
      p_msc_fnct_data->EP_OutHandle = ep_handle;
    }
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_EP_Close()
 *
 * @brief    Performs any specific operation needed when the endpoint has been closed by core on given
 *           function's interface.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @note     (1) This function will be called by the core when a endpoint is closed.
 *               - (a) When a new alternate interface is selected.
 *
 * @note     (2) This function will not be called if an endpoint is closed in the following circumstances.
 *               The Disconn() function will be called instead.
 *               - (a) When the device is disconnected.
 *               - (b) When a new configuration is selected.
 *
 * @note     (3) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  RTOS_ERR               err_usbh;

  PP_UNUSED_PARAM(if_ix);
  USBH_DBG_MSC_STATS_INC(EP_CloseCnt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                OP_LOCK_TIMEOUT_INFINITE,
                                                                &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }
  //                                                               Invalid bulk EPs handles used for BOT comm.
  if (p_msc_fnct_data->EP_InHandle == ep_handle) {
    p_msc_fnct_data->EP_InHandle = USBH_EP_HANDLE_INVALID;
  } else if (p_msc_fnct_data->EP_OutHandle == ep_handle) {
    p_msc_fnct_data->EP_OutHandle = USBH_EP_HANDLE_INVALID;
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                               USBH_MSC_Conn()
 *
 * @brief    Initializes MSC communication.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core when all the endpoints have been successfully
 *               opened for given function and after it issued the SetConfiguration request. The
 *               function is hence ready for communication.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_Conn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  CPU_INT08U             max_nbr_lu;
  CPU_INT08U             i;
  RTOS_ERR               err_usbh;

  USBH_DBG_MSC_STATS_INC(ConnCnt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctGet(&USBH_MSC_Ptr->FnctPool,
                                                            class_fnct_handle,
                                                            &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Getting MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }
  //                                                               Get nbr of logical units supported by dev.
  max_nbr_lu = USBH_MSC_MaxLUN_Get(p_msc_fnct_data, &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Getting max LUN failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }
  max_nbr_lu += 1u;                                             // +1 because max LUN is zero-based.

  //                                                               Call cmd blk Conn() callback for each LU.
  for (i = 0; i < max_nbr_lu; i++) {
    USBH_MSC_Ptr->CmdBlkFnctsPtr->Conn(i,
                                       p_msc_fnct_data->ID,
                                       (void *)&USBH_MSC_TransportApi,
                                       (void *) p_class_fnct);
  }

  p_msc_fnct_data->MaxLUN = max_nbr_lu;
}

/****************************************************************************************************//**
 *                                           USBH_MSC_Disconn()
 *
 * @brief    Disconnects MSC function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core in the following circumstances.
 *               - (a) The core is unable to open an endpoint for this function.
 *               - (b) An error occurs in the core before the SetConfiguration request is issued.
 *               - (c) The device has been disconnected.
 *               - (d) A new configuration has been selected.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_Disconn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  CPU_INT08U             i;
  RTOS_ERR               err_usbh;

  USBH_DBG_MSC_STATS_INC(DisconnCnt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                OP_LOCK_TIMEOUT_INFINITE,
                                                                &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  //                                                               Call cmd blk Disconn() callback for each LU.
  for (i = 0; i < p_msc_fnct_data->MaxLUN; i++) {
    USBH_MSC_Ptr->CmdBlkFnctsPtr->Disconn(i,
                                          p_msc_fnct_data->ID,
                                          (void *)p_class_fnct);

    USBH_DBG_MSC_STATS_LU_RESET((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), i);
  }

  USBH_ClassFnctFree(&USBH_MSC_Ptr->FnctPool,
                     class_fnct_handle,
                     &err_usbh);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_usbh) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           USBH_MSC_Suspend()
 *
 * @brief    Puts MSC function in suspend state.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_Suspend(void *p_class_fnct)
{
  PP_UNUSED_PARAM(p_class_fnct);
  USBH_DBG_MSC_STATS_INC(SuspendCnt);
}

/****************************************************************************************************//**
 *                                               USBH_MSC_Resume()
 *
 * @brief    Resumes CDC function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_MSC_Resume(void *p_class_fnct)
{
  PP_UNUSED_PARAM(p_class_fnct);
  USBH_DBG_MSC_STATS_INC(ResumeCnt);
}

/****************************************************************************************************//**
 *                                               USBH_MSC_UnInit()
 *
 * @brief    Un-initializes MSC.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_MSC_UnInit(void)
{
  RTOS_ERR err_usbh;

  USBH_ClassFnctPoolDel(&USBH_MSC_Ptr->FnctPool,
                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Deleting MSC fnct pool failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_MSC_Ptr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_MSC_TraceDump()
 *
 * @brief    Output class specific debug information on function.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    opt             Trace dump options.
 *
 * @param    trace_fnct      Function to call to output traces.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_MSC_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct)
{
  USBH_MSC_FNCT_DATA     *p_msc_fnct_data;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  CPU_CHAR               str[5u];
  RTOS_ERR               err_usbh;
  CPU_INT08U             subclass;
  CPU_INT08U             protocol;

  PP_UNUSED_PARAM(opt);

  p_msc_fnct_data = (USBH_MSC_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_MSC_Ptr->FnctPool,
                                                                class_fnct_handle,
                                                                DEF_YES,
                                                                USBH_MSC_Ptr->StdReqTimeoutMs,
                                                                &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring MSC class fnct failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  trace_fnct("    ---------- USB Host (MSC specific debug information) -----------\r\n");
  trace_fnct("    | Dev ID# | Nbr of LUs | Subclass  | Protocol  |               |\r\n");
  trace_fnct("    | ");

  //                                                               Output MSC dev ID.
  (void)Str_FmtNbr_Int32U(p_msc_fnct_data->ID,
                          5u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("   | ");
  //                                                               Output LU qty composing this dev.
  (void)Str_FmtNbr_Int32U(p_msc_fnct_data->MaxLUN,
                          2u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("         | ");
  //                                                               Output MSC subclass code.
  subclass = USBH_FnctSubclassGet(p_msc_fnct_data->DevHandle,
                                  p_msc_fnct_data->FnctHandle,
                                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Getting MSC subclass failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  switch (subclass) {
    case USBH_MSC_SUBCLASS_CODE_SCSI:
      trace_fnct("SCSI      | ");
      break;

    case USBH_MSC_SUBCLASS_CODE_SFF_8070i:
      trace_fnct("SFF 8070i | ");
      break;

    default:
      trace_fnct("????      | ");
      break;
  }
  //                                                               Output MSC protocol code.
  protocol = USBH_FnctProtocolGet(p_msc_fnct_data->DevHandle,
                                  p_msc_fnct_data->FnctHandle,
                                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Getting MSC subclass protocol w/ err = ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  switch (protocol) {
    case USBH_MSC_PROTOCOL_CODE_BULK_ONLY:
      trace_fnct("Bulk Only |               |");
      break;
    default:
      trace_fnct("????      |               |");
      break;
  }
  trace_fnct("\r\n");

  USBH_ClassFnctRelease(&USBH_MSC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_MSC_CmdBlkExe()
 *
 * @brief    Executes MSC command cycle. Sends The command (CBW), followed by data stage (if present),
 *           and then receive status (CSW).
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    lun                 Logical Unit Number.
 *
 * @param    dir                 Direction of data transfer, if present.
 *
 * @param    p_cmd_blk           Pointer to the command block.
 *
 * @param    cmd_blk_len         Command block length, in octets.
 *
 * @param    p_buf               Pointer to the data buffer, if data stage present.
 *
 * @param    buf_len             Length of data buffer in octets, if data stage present.
 *
 * @param    timeout             Timeout in ms.
 *
 * @param    p_cmd_status        Pointer to the variable that will receive the status of the command execution.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @return   Number of octets transferred (sent or received).
 *
 * @note     (1) The host shall perform a Reset Recovery if:
 *               - (a) The CSW is not valid             (cf. sections 6.3 & 6.5, USB MSC BOT specification).
 *               - (b) The CSW has a Phase Error status (cf. section 5.3.3.1, USB MSC BOT specification).
 *               - (c) The CSW has been stalled         (cf. Figure 2, USB MSC BOT specification).
 *******************************************************************************************************/
static CPU_INT32U USBH_MSC_CmdBlkExe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                     CPU_INT08U         lun,
                                     CPU_INT08U         dir,
                                     void               *p_cmd_blk,
                                     CPU_INT08U         cmd_blk_len,
                                     CPU_INT08U         *p_buf,
                                     CPU_INT32U         buf_len,
                                     CPU_INT32U         timeout,
                                     CPU_BOOLEAN        *p_cmd_status,
                                     RTOS_ERR           *p_err)
{
  USBH_MSC_CBW *p_cbw;
  CPU_INT08U   *p_cmd_blk08 = (CPU_INT08U *)p_cmd_blk;
  CPU_INT32U   signature = USBH_MSC_CBW_SIG;
  CPU_INT32U   xfer_len;
  USBH_MSC_CSW csw;
  CPU_INT08U   i;

  //                                                               ------------------ CMD TRANSPORT -------------------
  //                                                               Prepare CBW in little endian format.
  Mem_Clr((void *)p_msc_fnct_data->CmdStatusBufPtr,
          USBH_MSC_CBW_LEN);

  p_cbw = (USBH_MSC_CBW *)p_msc_fnct_data->CmdStatusBufPtr;
  p_cbw->dCBWSignature = MEM_VAL_GET_INT32U_LITTLE(&signature);
  p_cbw->dCBWTag = 0u;
  p_cbw->dCBWDataTransferLength = MEM_VAL_GET_INT32U_LITTLE(&buf_len);
  p_cbw->bmCBWFlags = (dir == USBH_MSC_CBW_DATA_DIR_NONE) ? 0u : dir;
  p_cbw->bCBWLUN = lun;
  p_cbw->bCBWCBLength = cmd_blk_len;

  for (i = 0u; i < cmd_blk_len; i++) {
    p_cbw->CBWCB[i] = p_cmd_blk08[i];
  }
  //                                                               Send CBW to dev.
  USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, CBW_Cnt);
  USBH_MSC_CBW_Exe(p_msc_fnct_data,
                   p_msc_fnct_data->CmdStatusBufPtr,
                   timeout,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  //                                                               ------------------ DATA TRANSPORT ------------------
  if (dir != USBH_MSC_CBW_DATA_DIR_NONE) {
    USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, DataPhaseCnt);

    USBH_MSC_DataExe(p_msc_fnct_data,
                     dir,
                     p_buf,
                     buf_len,
                     timeout,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }
  } else {
    USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, NoDataPhaseCnt);
  }
  //                                                               ---------------- STATUS TRANSPORT ------------------
  Mem_Set((void *)&csw,
          0u,
          USBH_MSC_CSW_LEN);
  //                                                               Receive CSW.
  USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, CSW_Cnt);
  USBH_MSC_CSW_Exe(p_msc_fnct_data,
                   &csw,
                   timeout,
                   p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_EP_STALL)) {
    return (0u);
  }
  //                                                               Analyze CSW (see Note #1).
  if ((csw.dCSWSignature != USBH_MSC_CSW_SIG)
      || (csw.bCSWStatus == USBH_MSC_CSW_STATUS_PHASE_ERR)
      || (csw.dCSWTag != 0u)
      || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_EP_STALL)) {
    RTOS_ERR local_err;

    LOG_DBG(("CSW: Issuing a Reset Recovery."));

    USBH_MSC_ResetRecovery(p_msc_fnct_data, &local_err);        // Invalid CSW, issue reset recovery.
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Reset Recovery failed w/ err = ", RTOS_ERR_LOG_ARG_GET(local_err)));
      RTOS_ERR_COPY(*p_err, local_err);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    }

    *p_cmd_status = DEF_FAIL;
    return (0u);
  }

  switch (csw.bCSWStatus) {
    case USBH_MSC_CSW_STATUS_CMD_PASSED:
      USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, CmdSuccessCnt);
      *p_cmd_status = DEF_OK;
      break;

    case USBH_MSC_CSW_STATUS_CMD_FAILED:
      USBH_DBG_MSC_STATS_LU_INC((p_msc_fnct_data->ID % USBH_DBG_MSC_DEV_QTY), lun, CmdErrCnt);
      *p_cmd_status = DEF_FAIL;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    xfer_len = buf_len - csw.dCSWDataResidue;                   // Actual len of data transferred to dev.
  } else {
    xfer_len = 0;
  }

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_CBW_Exe()
 *
 * @brief    Sends Command Block Wrapper (CBW) to device through bulk OUT endpoint.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    p_cbw_buf           Pointer to the CBW buffer.
 *
 * @param    timeout             Timeout in ms.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) According to BOT specification, section 5.3.1: "If the host detects a STALL of the
 *               Bulk-Out endpoint during command transport, the host shall respond with a Reset
 *               Recovery."
 *******************************************************************************************************/
static void USBH_MSC_CBW_Exe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             CPU_INT08U         *p_cbw_buf,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err)
{
  CPU_INT32U len;

  len = USBH_EP_BulkTx(p_msc_fnct_data->DevHandle,
                       p_msc_fnct_data->EP_OutHandle,
                       p_cbw_buf,
                       USBH_MSC_CBW_LEN,
                       timeout,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_EP_STALL) {         // See Note #1.
    RTOS_ERR local_err;

    LOG_DBG(("CBW: Issuing a Reset Recovery."));

    USBH_MSC_ResetRecovery(p_msc_fnct_data, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {        // If Reset Recovery fails, stall condition err returned
      LOG_ERR(("Reset Recovery failed w/ err = ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }
  } else if ((len != USBH_MSC_CBW_LEN)
             && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    LOG_DBG(("CBW sent is NOT 31 bytes long."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
  }
}

/****************************************************************************************************//**
 *                                               USBH_MSC_RxCSW()
 *
 * @brief    Receives Command Status Word (CSW) from device through bulk IN endpoint.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    p_msc_csw           Pointer to the variable that will receive the CSW data.
 *
 * @param    timeout             Timeout in ms.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) "Figure 2 - Status Transport Flow" of BOT specification defines the algorithm the
 *               host shall use for any CSW transfer. If a stall condition is detected, the host
 *               should clear the STALL condition and attempt to receive a CSW again.
 *******************************************************************************************************/
static void USBH_MSC_CSW_Exe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             USBH_MSC_CSW       *p_msc_csw,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err)
{
  CPU_INT32U retry;
  CPU_INT32U len;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  retry = USBH_MSC_MAX_RETRY;

  Mem_Clr((void *)p_msc_fnct_data->CmdStatusBufPtr,
          USBH_MSC_CSW_LEN);

  //                                                               Receive CSW from dev through bulk IN EP.
  while (retry > 0u) {
    len = USBH_EP_BulkRx(p_msc_fnct_data->DevHandle,
                         p_msc_fnct_data->EP_InHandle,
                         p_msc_fnct_data->CmdStatusBufPtr,
                         USBH_MSC_CSW_LEN,
                         timeout,
                         p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        retry = 0u;

        if (len != USBH_MSC_CSW_LEN) {
          LOG_DBG(("CSW received is NOT 13 bytes long."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        } else {
          USBH_MSC_CSW_Parse(p_msc_csw,
                             (void *)p_msc_fnct_data->CmdStatusBufPtr);
        }
        break;

      case RTOS_ERR_EP_STALL:                                   // See Note #1.
        LOG_DBG(("Stalled CSW."));
        USBH_EP_StallSet(p_msc_fnct_data->DevHandle,
                         p_msc_fnct_data->EP_InHandle,
                         DEF_CLR,
                         &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Clearing STALL condition during CSW phase failed w/ err = ", RTOS_ERR_LOG_ARG_GET(local_err)));
          RTOS_ERR_COPY(*p_err, local_err);
          retry = 0u;
        } else {
          retry--;
        }
        break;

      default:
        retry = 0u;
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_MSC_DataExe()
 *
 * @brief    Sends or receives data to/from device through bulk OUT or IN endpoint.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    dir                 Transfer direction:
 *                               USBH_MSC_CBW_DATA_DIR_OUT
 *                               USBH_MSC_CBW_DATA_DIR_IN
 *
 * @param    p_buf               Pointer to the data buffer.
 *
 * @param    buf_len             Data buffer length.
 *
 * @param    timeout             Timeout in ms.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) According to BOT specification, if the host detects a STALL condition during the Data
 *               phase, it has 2 options:
 *               - (a) Perform a Reset Recovery immediately following the stalled data phase. Section
 *                     6.6.1 "CBW Not Valid" and 6.6.2 "Internal Device Error" suggest that. If the
 *                     Reset Recovery succeeds, next CBW can be sent.
 *               - (b) Clear the the STALL condition on the bulk endpoint and attempt to receive the
 *                     CSW. Only during the CSW phase, the host should attempt a Reset Recovery if a
 *                     phase error is detected. Section 6.7.2 "Hi - Host expects to receive data from
 *                     the device" and 6.7.3 "Ho - Host expects to send data to the device" describe
 *                     that.
 *               The stall condition handling here follows the option b).
 *******************************************************************************************************/
static void USBH_MSC_DataExe(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                             CPU_INT08U         dir,
                             CPU_INT08U         *p_buf,
                             CPU_INT32U         buf_len,
                             CPU_INT32U         timeout,
                             RTOS_ERR           *p_err)
{
  CPU_INT16U retry_cnt = 0u;
  CPU_INT32U data_ix = 0u;
  CPU_INT32U data_len_rem = buf_len;
  CPU_INT32U data_len_transferred;

  while ((data_len_rem > 0u)
         && (retry_cnt < USBH_MSC_MAX_RETRY)) {
    if (dir == USBH_MSC_CBW_DATA_DIR_OUT) {
      data_len_transferred = USBH_EP_BulkTx(p_msc_fnct_data->DevHandle,
                                            p_msc_fnct_data->EP_OutHandle,
                                            (p_buf + data_ix),
                                            data_len_rem,
                                            timeout,
                                            p_err);
    } else {
      data_len_transferred = USBH_EP_BulkRx(p_msc_fnct_data->DevHandle,
                                            p_msc_fnct_data->EP_InHandle,
                                            (p_buf + data_ix),
                                            data_len_rem,
                                            timeout,
                                            p_err);
    }
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if ((data_len_transferred < data_len_rem)               // This condition detects end of xfer.
            && (data_len_transferred > 0u)) {                   // This condition detects ZLP.
          data_len_rem -= data_len_transferred;                 // Update remaining nbr of octets to move.
          data_ix += data_len_transferred;                      // Update buf ix.
        } else {
          data_len_rem = 0u;                                    // Quit the loop (end of xfer or ZLP).
        }
        break;

      case RTOS_ERR_IO:
        retry_cnt++;
        break;

      default:
        data_len_rem = 0u;
        break;
    }
  }
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_EP_STALL) {         // See Note #1.
    RTOS_ERR local_err;

    LOG_DBG(("Data phase stalled."));

    USBH_EP_StallSet(p_msc_fnct_data->DevHandle,
                     (dir == USBH_MSC_CBW_DATA_DIR_OUT ? p_msc_fnct_data->EP_OutHandle : p_msc_fnct_data->EP_InHandle),
                     DEF_CLR,
                     &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Clearing STALL condition during Data phase failed w/ err = ", RTOS_ERR_LOG_ARG_GET(local_err)));
      RTOS_ERR_COPY(*p_err, local_err);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // Attempt to receive CSW after a STALLed data phase.
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_MSC_ResetRecovery()
 *
 * @brief    Applies bulk-only reset recovery to device and clears stalled bulk endpoints.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) The Bulk-Only Transport protocol specification indicates that the host shall issue in
 *               the following order for the Reset Recovery:
 *               - (a) a Bulk-Only Mass Storage Reset
 *               - (b) a Clear Feature HALT to the Bulk-In endpoint
 *               - (c) a Clear Feature HALT to the Bulk-Out endpoint
 *******************************************************************************************************/
static void USBH_MSC_ResetRecovery(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                   RTOS_ERR           *p_err)
{
  //                                                               See Note #1.
  USBH_MSC_BulkOnlyReset(p_msc_fnct_data, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_EP_StallSet(p_msc_fnct_data->DevHandle,
                   p_msc_fnct_data->EP_InHandle,
                   DEF_CLR,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_EP_StallSet(p_msc_fnct_data->DevHandle,
                   p_msc_fnct_data->EP_OutHandle,
                   DEF_CLR,
                   p_err);
}

/****************************************************************************************************//**
 *                                           USBH_MSC_BulkOnlyReset()
 *
 * @brief    Issues bulk-only reset.
 *
 * @param    p_msc_fnct_data     Pointer to the MSC function data.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function.
 *
 * @note     (1) See specification "USB MSC Bulk-Only Transport, Revision 1.0", section 3.1 for more
 *               details about the class request "Bulk-Only Mass Storage Reset".
 *******************************************************************************************************/
static void USBH_MSC_BulkOnlyReset(USBH_MSC_FNCT_DATA *p_msc_fnct_data,
                                   RTOS_ERR           *p_err)
{
  CPU_INT08U if_nbr;
  CPU_INT32U std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  if_nbr = USBH_IF_NbrGet(p_msc_fnct_data->DevHandle,
                          p_msc_fnct_data->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_MSC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  (void)USBH_EP_CtrlXfer(p_msc_fnct_data->DevHandle,
                         USBH_MSC_REQ_MASS_STORAGE_RESET,
                         (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_IF),
                         0u,
                         if_nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         std_req_timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Sending Bulk Only Mass Storage Reset failed w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_MSC_CSW_Parse()
 *
 * @brief    Parse received CSW into CSW structure taking into account the little endian format.
 *
 * @param    p_csw       Variable that will receive CSW parsed in this function.
 *
 * @param    p_buf_src   Pointer to the buffer that contains CSW data.
 *******************************************************************************************************/
static void USBH_MSC_CSW_Parse(USBH_MSC_CSW *p_csw,
                               void         *p_buf_src)
{
  USBH_MSC_CSW *p_buf_src_cbw;

  p_buf_src_cbw = (USBH_MSC_CSW *)p_buf_src;

  p_csw->dCSWSignature = MEM_VAL_GET_INT32U_LITTLE(&p_buf_src_cbw->dCSWSignature);
  p_csw->dCSWTag = MEM_VAL_GET_INT32U_LITTLE(&p_buf_src_cbw->dCSWTag);
  p_csw->dCSWDataResidue = MEM_VAL_GET_INT32U_LITTLE(&p_buf_src_cbw->dCSWDataResidue);
  p_csw->bCSWStatus = p_buf_src_cbw->bCSWStatus;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_MSC_AVAIL))
