/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (Cdc)
 *        Ethernet Emulation Model (Eem)
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
 * @note             (1) This implementation is compliant with the CDC-EEM specification "Universal Serial
 *                       Bus Communications Class Subclass Specification for Ethernet Emulation Model
 *                       Devices" revision 1.0. February 2, 2005.
 *
 * @note             (2) This class implementation does NOT use the CDC base class implementation as
 *                       CDC-EEM specification does not follow the CDC specification revision 1.2
 *                       December 6, 2012.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_EEM_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device CDC EEM class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    USBD_CDC_EEM_MODULE

#include  <em_core.h>

#include  <common/include/rtos_path.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/source/kal/kal_priv.h>

#include  <usbd_cfg.h>
#include  <rtos_cfg.h>

#include  <usb/include/device/usbd_cdc_eem.h>

#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
#include  <net/include/net_if_ether.h>
#include  <drivers/net/include/net_drv_ether.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, CDC, EEM)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

#define  USBD_CDC_EEM_MAX_RETRY_CNT                       3u    // Max nbr of retry when Rx buf submission fails.

#define  USBD_CDC_EEM_SUBCLASS_CODE                     0x0C
#define  USBD_CDC_EEM_PROTOCOL_CODE                     0x07

/********************************************************************************************************
 *                                   ZERO LENGTH EEM (ZLE) SPECIAL PACKET
 *
 * Note(s) : (1) ZLE packet is defined in "Universal Serial Bus Communications Class Subclass
 *               Specification for Ethernet Emulation Model Devices" revision 1.0. February 2, 2005,
 *               section 5.1.2.3.1.
 *******************************************************************************************************/

#define  USBD_CDC_EEM_HDR_ZLE                         0x0000

/********************************************************************************************************
 *                                               PACKET TYPES
 *
 * Note(s) : (1) Packet types are defined in "Universal Serial Bus Communications Class Subclass
 *               Specification for Ethernet Emulation Model Devices" revision 1.0. February 2, 2005,
 *               section 5.1.2.
 *******************************************************************************************************/

#define  USBD_CDC_EEM_PKT_TYPE_MASK               DEF_BIT_15
#define  USBD_CDC_EEM_PKT_TYPE_PAYLOAD                    0u
#define  USBD_CDC_EEM_PKT_TYPE_CMD                        1u

/********************************************************************************************************
 *                                   EEM PAYLOAD PACKET HEADER DEFINITIONS
 *
 * Note(s) : (1) Payload header format is defined in "Universal Serial Bus Communications Class Subclass
 *               Specification for Ethernet Emulation Model Devices" revision 1.0. February 2, 2005,
 *               section 5.1.2.1.
 *
 *               (a) Payload packets header have the following format:
 *
 *                   +---------+----------------------------------+
 *                   | Bit     |   15   |  14   | 13..0           |
 *                   +---------+----------------------------------+
 *                   | Content | bmType | bmCRC | Length of       |
 *                   |         |  (0)   |       | Ethernet frame  |
 *                   +---------+----------------------------------+
 *
 *                   (1) bmCRC indicates whether the Ethernet frame contains a valid CRC or a CRC set
 *                       to 0xDEADBEEF.
 *******************************************************************************************************/

//                                                                 ----------------- PAYLOAD PKT CRC ------------------
#define  USBD_CDC_EEM_PAYLOAD_CRC_MASK            DEF_BIT_14
#define  USBD_CDC_EEM_PAYLOAD_CRC_NOT_CALC                0u
#define  USBD_CDC_EEM_PAYLOAD_CRC_CALC                    1u

//                                                                 ----------------- PAYLOAD PKT LEN ------------------
#define  USBD_CDC_EEM_PAYLOAD_LEN_MASK         DEF_BIT_FIELD(14u, 0u)

/********************************************************************************************************
 *                                   EEM COMMAND PACKET HEADER DEFINITIONS
 *
 * Note(s) : (1) Command header format is defined in "Universal Serial Bus Communications Class Subclass
 *               Specification for Ethernet Emulation Model Devices" revision 1.0. February 2, 2005,
 *               section 5.1.2.2.
 *
 *               (a) Command packets header have the following format:
 *
 *                   +---------+------------------------------------------------+
 *                   | Bit     | 15     | 14         | 13..11   | 10..0         |
 *                   +---------+------------------------------------------------+
 *                   | Content | bmType | bmReserved | bmEEMCmd | bmEEMCmdParam |
 *                   |         |  (1)   |    (0)     |          |               |
 *                   +---------+------------------------------------------------+
 *
 *                   (1) bmEEMCmd gives the code of the command to execute.
 *
 *                   (2) bmEEMCmdParam gives extra information for the command execution. Format depends
 *                       on the command code.
 *******************************************************************************************************/

//                                                                 ------------------- CMD RSVD BIT -------------------
#define  USBD_CDC_EEM_CMD_RSVD_MASK               DEF_BIT_14
#define  USBD_CDC_EEM_CMD_RSVD_OK                         0u

//                                                                 ------------------- CMD PKT CODE -------------------
#define  USBD_CDC_EEM_CMD_CODE_MASK            DEF_BIT_FIELD(3u, 11u)
#define  USBD_CDC_EEM_CMD_CODE_ECHO                       0u
#define  USBD_CDC_EEM_CMD_CODE_ECHO_RESP                  1u
#define  USBD_CDC_EEM_CMD_CODE_SUSP_HINT                  2u
#define  USBD_CDC_EEM_CMD_CODE_RESP_HINT                  3u
#define  USBD_CDC_EEM_CMD_CODE_RESP_CMPL_HINT             4u
#define  USBD_CDC_EEM_CMD_CODE_TICKLE                     5u

//                                                                 ------------------- CMD PKT PARAM -------------------
#define  USBD_CDC_EEM_CMD_PARAM_MASK           DEF_BIT_FIELD(11u, 0u)

//                                                                 ------------------- ECHO CMD LEN -------------------
#define  USBD_CDC_EEM_CMD_PARAM_ECHO_LEN_MASK  DEF_BIT_FIELD(10u, 0u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Dflt CRC value.
static const CPU_INT08U USBD_CDC_EEM_PayloadCRC[] = { 0xDE, 0xAD, 0xBE, 0xEF };

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBD_CDC_EEM_INIT_CFG USBD_CDC_EEM_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .EchoBufLen = 64u,
  .RxBufQty = 1u,
  .RxBufLen = 1520u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_ctrl USBD_CDC_EEM_CTRL;             // Forward declaration.

/********************************************************************************************************
 *                                           CDC-EEM CLASS STATES
 *******************************************************************************************************/

typedef enum usbd_cdc_eem_state {
  USBD_CDC_EEM_STATE_NONE = 0,
  USBD_CDC_EEM_STATE_INIT,
  USBD_CDC_EEM_STATE_CFG
} USBD_CDC_EEM_STATE;

/********************************************************************************************************
 *                                   CDC-EEM CLASS BUFFER QUEUE ENTRY
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_buf_entry {
  CPU_INT08U  *BufPtr;                                          // Pointer to buffer.
  CPU_INT16U  BufLen;                                           // Buffer length in bytes.
  CPU_BOOLEAN CrcComputed;                                      // Flag indicates if CRC computed for this buf.
} USBD_CDC_EEM_BUF_ENTRY;

/********************************************************************************************************
 *                                       CDC-EEM CLASS COMM INFO
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_comm {
  USBD_CDC_EEM_CTRL *CtrlPtr;                                   // Ptr to ctrl info.
                                                                // Avail EP for comm: Bulk (and Intr)
  CPU_INT08U        DataInEpAddr;                               // Address of Bulk IN EP.
  CPU_INT08U        DataOutEpAddr;                              // Address of Bulk OUT EP.
} USBD_CDC_EEM_COMM;

/********************************************************************************************************
 *                                       CDC-EEM CLASS BUFFER QUEUE
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_buf_q {
  USBD_CDC_EEM_BUF_ENTRY *Tbl;                                  // Ptr to table of buffer entry.
  CPU_INT08U             InIdx;                                 // In  Q index.
  CPU_INT08U             OutIdx;                                // Out Q index.
  CPU_INT08U             Cnt;                                   // Nbr of elements in Q.
  CPU_INT08U             Size;                                  // Size of Q.
} USBD_CDC_EEM_BUF_Q;

/********************************************************************************************************
 *                                       CDC-EEM CLASS CTRL INFO
 *******************************************************************************************************/

struct usbd_cdc_eem_ctrl {
  CPU_INT08U         DevNbr;                                    // Dev nbr to which this class instance is associated.
  CPU_INT08U         ClassNbr;                                  // Class instacne number.
  USBD_CDC_EEM_COMM  *CommPtr;                                  // Ptr to comm.

  USBD_CDC_EEM_STATE State;                                     // Class instance current state.
  CPU_INT08U         StartCnt;                                  // Start cnt.
  KAL_LOCK_HANDLE    StateLockHandle;                           // Handle to lock for class instance state.

  CPU_INT08U         RxErrCnt;                                  // Cnt of Rx error.

  CPU_INT08U         **RxBufPtrTbl;                             // Ptr to Rx buffer table.

  CPU_INT08U         *BufEchoPtr;                               // Ptr to buffer that contains echo data.

  USBD_CDC_EEM_DRV   *DrvPtr;                                   // Ptr to network driver.
  void               *DrvArgPtr;                                // Arg of network driver.

  //                                                               ----------------- RX STATE MACHINE -----------------
  CPU_INT16U         CurHdr;                                    // Hdr of current buffer.
  CPU_INT32U         CurBufLenRem;                              // Remaining length of current buffer.
  CPU_INT08U         *CurBufPtr;                                // Pointer to current network buffer.
  CPU_INT16U         CurBufIx;                                  // Index of network buffer to write.
  CPU_BOOLEAN        CurBufCrcComputed;                         // Flag indicates if CRC is computed on current buf.

  //                                                               ---------------- NETWORK BUFFER QS -----------------
  USBD_CDC_EEM_BUF_Q RxBufQ;                                    // Rx buffer Q.
  USBD_CDC_EEM_BUF_Q TxBufQ;                                    // Tx buffer Q.
  CPU_BOOLEAN        TxInProgress;                              // Flag that indicates if a Tx is in progress.
};

/********************************************************************************************************
 *                                       CDC EEM CLASS ROOT STRUCTURE
 *******************************************************************************************************/

typedef struct usbd_cdc_eem {
  USBD_CDC_EEM_CTRL *CtrlTbl;
  CPU_INT08U        CtrlNbrNext;
  //                                                               CDC EEM class comm array.
  USBD_CDC_EEM_COMM *CommTbl;
  CPU_INT08U        CommNbrNext;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_INT08U        ClassInstanceQty;                           // Quantity of class instances.
#endif
} USBD_CDC_EEM;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_CDC_EEM *USBD_CDC_EEM_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBD_CDC_EEM_INIT_CFG USBD_CDC_EEM_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .EchoBufLen = 64u,
  .RxBufQty = 1u,
  .RxBufLen = 1520u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL
};
#else
extern USBD_CDC_EEM_INIT_CFG USBD_CDC_EEM_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_CDC_EEM_Conn(CPU_INT08U dev_nbr,
                              CPU_INT08U config_nbr,
                              void       *p_if_class_arg);

static void USBD_CDC_EEM_Disconn(CPU_INT08U dev_nbr,
                                 CPU_INT08U config_nbr,
                                 void       *p_if_class_arg);

static void USBD_CDC_EEM_CommStart(USBD_CDC_EEM_CTRL *p_ctrl,
                                   RTOS_ERR          *p_err);

static void USBD_CDC_EEM_RxCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err);

static void USBD_CDC_EEM_TxCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err);

static void USBD_CDC_EEM_TxBufSubmit(USBD_CDC_EEM_CTRL *p_ctrl,
                                     CPU_INT08U        *p_buf,
                                     CPU_INT16U        buf_len,
                                     RTOS_ERR          *p_err);

static void USBD_CDC_EEM_BufQ_Add(USBD_CDC_EEM_BUF_Q *p_buf_q,
                                  CPU_INT08U         *p_buf,
                                  CPU_INT16U         buf_len,
                                  CPU_BOOLEAN        crc_computed);

static CPU_INT08U *USBD_CDC_EEM_BufQ_Get(USBD_CDC_EEM_BUF_Q *p_buf_q,
                                         CPU_INT16U         *p_buf_len,
                                         CPU_BOOLEAN        *p_crc_computed);

static void USBD_CDC_EEM_StateLock(USBD_CDC_EEM_CTRL *p_ctrl,
                                   RTOS_ERR          *p_err);

static void USBD_CDC_EEM_StateUnlock(USBD_CDC_EEM_CTRL *p_ctrl,
                                     RTOS_ERR          *p_err);

/********************************************************************************************************
 *                                           CDC-EEM CLASS DRIVER
 *******************************************************************************************************/

static USBD_CLASS_DRV USBD_CDC_EEM_Drv = {
  USBD_CDC_EEM_Conn,
  USBD_CDC_EEM_Disconn,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  DEF_NULL,
  DEF_NULL,
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBD_CDC_EEM_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC EEM class is initialized via the
 *               function USBD_CDC_EEM_Init().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_EEM_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_EEM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_EEM_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_ConfigureRxBuf()
 *
 * @brief    Configures the quantity CDC EEM receive buffer(s).
 *
 * @param    rx_buf_qty  Quantity of receive buffer. Unless your USB driver supports URQ queuing,
 *                       always set to 1.
 *
 * @param    rx_buf_len  Length, in octets, of the receive buffer(s).
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC EEM class is initialized via the
 *               function USBD_CDC_EEM_Init().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_EEM_ConfigureRxBuf(CPU_INT08U rx_buf_qty,
                                 CPU_INT32U rx_buf_len)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_EEM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_EEM_InitCfg.RxBufQty = rx_buf_qty;
  USBD_CDC_EEM_InitCfg.RxBufLen = rx_buf_len;
}
#endif

/****************************************************************************************************//**
 *                                   USBD_CDC_EEM_ConfigureEchoBufLen()
 *
 * @brief    Configures the length, in octets, of the buffer used for echo requests.
 *
 * @param    echo_buf_len    Length, in octets, of the echo buffer.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC EEM class is initialized via the
 *               function USBD_CDC_EEM_Init().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_EEM_ConfigureEchoBufLen(CPU_INT16U echo_buf_len)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_EEM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_EEM_InitCfg.EchoBufLen = echo_buf_len;
}
#endif

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the CDC EEM class is initialized via the
 *               function USBD_CDC_EEM_Init().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_EEM_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                                  MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBD_CDC_EEM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBD_CDC_EEM_InitCfg.MemSegPtr = p_mem_seg;
  USBD_CDC_EEM_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Init()
 *
 * @brief    Initializes the internal structures and variables used by the CDC EEM class.
 *
 * @param    p_qty_cfg   Pointer to the configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_CDC_EEM_Init(USBD_CDC_EEM_QTY_CFG *p_qty_cfg,
                       RTOS_ERR             *p_err)
{
  CPU_INT08U        ix;
  USBD_CDC_EEM_CTRL *p_ctrl;
  USBD_CDC_EEM_COMM *p_comm;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_qty_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((USBD_CDC_EEM_InitCfg.EchoBufLen >= 2u)
                           && (USBD_CDC_EEM_InitCfg.RxBufLen >= 1520u)
                           && (USBD_CDC_EEM_InitCfg.RxBufQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_qty_cfg->ClassInstanceQty > 0u)
                           && (p_qty_cfg->ConfigQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );

  USBD_CDC_EEM_Ptr = (USBD_CDC_EEM *)Mem_SegAlloc("USBD - CDC EEM root struct",
                                                  USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                  sizeof(USBD_CDC_EEM),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_EEM_Ptr->CtrlNbrNext = p_qty_cfg->ClassInstanceQty;
  USBD_CDC_EEM_Ptr->CommNbrNext = p_qty_cfg->ClassInstanceQty * p_qty_cfg->ConfigQty;

  USBD_CDC_EEM_Ptr->CtrlTbl = (USBD_CDC_EEM_CTRL *)Mem_SegAlloc("USBD - CDC EEM ctrl table",
                                                                USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                                sizeof(USBD_CDC_EEM_CTRL) * USBD_CDC_EEM_Ptr->CtrlNbrNext,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_CDC_EEM_Ptr->CommTbl = (USBD_CDC_EEM_COMM *)Mem_SegAlloc("USBD - CDC EEM comm table",
                                                                USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                                sizeof(USBD_CDC_EEM_COMM) * USBD_CDC_EEM_Ptr->CommNbrNext,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < USBD_CDC_EEM_Ptr->CtrlNbrNext; ix++) {     // Init ctrl struct.
    CPU_INT08U buf_cnt;

    p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[ix];
    p_ctrl->DevNbr = USBD_DEV_NBR_NONE;
    p_ctrl->State = USBD_CDC_EEM_STATE_NONE;
    p_ctrl->ClassNbr = USBD_CLASS_NBR_NONE;
    p_ctrl->CommPtr = DEF_NULL;
    p_ctrl->DrvPtr = DEF_NULL;
    p_ctrl->DrvArgPtr = DEF_NULL;
    p_ctrl->StartCnt = 0u;

    p_ctrl->StateLockHandle = KAL_LockCreate("USBD - CDC EEM State lock",
                                             DEF_NULL,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Alloc buffer used by echo response command.
    p_ctrl->BufEchoPtr = (CPU_INT08U *)Mem_SegAllocHW("USBD - CDC EEM Echo buf",
                                                      USBD_CDC_EEM_InitCfg.MemSegBufPtr,
                                                      USBD_CDC_EEM_InitCfg.EchoBufLen * sizeof(CPU_INT08U),
                                                      USBD_CDC_EEM_InitCfg.BufAlignOctets,
                                                      DEF_NULL,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_ctrl->RxBufPtrTbl = (CPU_INT08U **)Mem_SegAlloc("USBD - CDC EEM Rx buf table",
                                                      USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                      sizeof(CPU_INT08U *) * USBD_CDC_EEM_InitCfg.RxBufQty,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Allocate Rx buffers.
    for (buf_cnt = 0u; buf_cnt < USBD_CDC_EEM_InitCfg.RxBufQty; buf_cnt++) {
      p_ctrl->RxBufPtrTbl[buf_cnt] = (CPU_INT08U *)Mem_SegAllocHW("USBD - CDC EEM Rx buf",
                                                                  USBD_CDC_EEM_InitCfg.MemSegBufPtr,
                                                                  sizeof(CPU_INT08U) * USBD_CDC_EEM_InitCfg.RxBufLen,
                                                                  USBD_CDC_EEM_InitCfg.BufAlignOctets,
                                                                  DEF_NULL,
                                                                  p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    }
  }

  for (ix = 0u; ix < USBD_CDC_EEM_Ptr->CommNbrNext; ix++) {     // Init comm struct.
    p_comm = &USBD_CDC_EEM_Ptr->CommTbl[ix];
    p_comm->CtrlPtr = DEF_NULL;
    p_comm->DataInEpAddr = USBD_EP_ADDR_NONE;
    p_comm->DataOutEpAddr = USBD_EP_ADDR_NONE;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  USBD_CDC_EEM_Ptr->ClassInstanceQty = p_qty_cfg->ClassInstanceQty;
#endif
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_NetIF_Reg()
 *
 * @brief    Adds a new instance of the CDC EEM class AND registers a new Ethernet controller to the
 *           platform manager.
 *
 * @param    p_name              Pointer to a string that will receive the Network Interface Name
 *                               as registered in the platform manager.
 *
 * @param    p_net_if_ether_cfg  Pointer to structure containing the configurations related to the
 *                               Ethernet Interface.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SUBCLASS_INSTANCE_ALLOC
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_SEG_OVF
 *
 * @return   Class instance number, if no errors are returned.
 *           USBD_CLASS_NBR_NONE,   if any errors are returned.
 *
 * @note     (1) µC/OS-Net Ethernet module MUST be available in your product to be able to use this
 *               function.
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
CPU_INT08U USBD_CDC_EEM_NetIF_Reg(CPU_CHAR                      **p_name,
                                  USBD_CDC_EEM_NET_IF_ETHER_CFG *p_net_if_ether_cfg,
                                  RTOS_ERR                      *p_err)
{
  CPU_INT08U                   class_nbr;
  NET_IF_ETHER_HW_INFO         *p_ether_ctrlr_info;
  NET_DEV_CFG_ETHER            *p_net_dev_cfg_ether;
  NET_DEV_CFG_USBD_CDC_EEM_EXT *p_net_cfg_ext_cdceem;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CLASS_NBR_NONE);

  class_nbr = USBD_CDC_EEM_Add(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  *p_name = (CPU_CHAR *)Mem_SegAlloc("USBD - CDC EEM Ether IF name",
                                     USBD_CDC_EEM_InitCfg.MemSegPtr,
                                     (6u * sizeof(CPU_CHAR)),
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  p_ether_ctrlr_info = (NET_IF_ETHER_HW_INFO *)Mem_SegAlloc("USBD - CDC EEM Ether ctrlr info struct",
                                                            USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                            sizeof(NET_IF_ETHER_HW_INFO),
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  p_net_dev_cfg_ether = (NET_DEV_CFG_ETHER *)Mem_SegAlloc("USBD - CDC EEM Ether Net Dev Cfg",
                                                          USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                          sizeof(NET_DEV_CFG_ETHER),
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  p_net_cfg_ext_cdceem = (NET_DEV_CFG_USBD_CDC_EEM_EXT *)Mem_SegAlloc("USBD - CDC EEM Ether Bet Dev Cfg Ext",
                                                                      USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                                      sizeof(NET_DEV_CFG_USBD_CDC_EEM_EXT),
                                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  p_net_cfg_ext_cdceem->ClassNbr = class_nbr;

  p_net_dev_cfg_ether->RxBufPoolType = NET_IF_MEM_TYPE_MAIN;
  p_net_dev_cfg_ether->RxBufLargeSize = 1518u;
  p_net_dev_cfg_ether->RxBufLargeNbr = p_net_if_ether_cfg->RxBufQty;
  p_net_dev_cfg_ether->RxBufAlignOctets = USBD_CDC_EEM_InitCfg.BufAlignOctets;
  p_net_dev_cfg_ether->RxBufIxOffset = 0u;
  p_net_dev_cfg_ether->TxBufPoolType = NET_IF_MEM_TYPE_MAIN;
  p_net_dev_cfg_ether->TxBufLargeSize = 1518u;
  p_net_dev_cfg_ether->TxBufLargeNbr = p_net_if_ether_cfg->TxBufQty;
  p_net_dev_cfg_ether->TxBufSmallSize = p_net_if_ether_cfg->TxBufSmallLen;
  p_net_dev_cfg_ether->TxBufSmallNbr = p_net_if_ether_cfg->TxBufSmallQty;
  p_net_dev_cfg_ether->TxBufAlignOctets = USBD_CDC_EEM_InitCfg.BufAlignOctets;
  p_net_dev_cfg_ether->TxBufIxOffset = 2u;
  p_net_dev_cfg_ether->MemAddr = 0u;
  p_net_dev_cfg_ether->MemSize = 0u;
  p_net_dev_cfg_ether->Flags = 0u;
  p_net_dev_cfg_ether->RxDescNbr = 0u;
  p_net_dev_cfg_ether->TxDescNbr = 0u;
  p_net_dev_cfg_ether->BaseAddr = 0u;
  p_net_dev_cfg_ether->DataBusSizeNbrBits = 0u;
  p_net_dev_cfg_ether->CfgExtPtr = p_net_cfg_ext_cdceem;

  Mem_Copy(p_net_dev_cfg_ether->HW_AddrStr,
           p_net_if_ether_cfg->HW_AddrStr,
           NET_IF_802x_ADDR_SIZE_STR);

  p_ether_ctrlr_info->DrvAPI_Ptr = &NetDev_API_USBD_CDCEEM;
  p_ether_ctrlr_info->DevCfgPtr = p_net_dev_cfg_ether;
  p_ether_ctrlr_info->BSP_API_Ptr = DEF_NULL;
  p_ether_ctrlr_info->PHY_API_Ptr = DEF_NULL;
  p_ether_ctrlr_info->PHY_CfgPtr = DEF_NULL;

  (void)Str_Snprintf(*p_name, 6u, "eem%d\0", class_nbr);

  NetIF_Ether_Reg(*p_name, p_ether_ctrlr_info, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBD_CLASS_NBR_NONE);
  }

  return (class_nbr);
}
#endif

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Add()
 *
 * @brief    Adds a new instance of the CDC EEM class.
 *
 * @param    p_err   Pointer to the variable that will receive one of these returned error codes from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SUBCLASS_INSTANCE_ALLOC
 *
 * @return   Class instance number, if no errors are returned.
 *           USBD_CLASS_NBR_NONE,   if any errors are returned.
 *
 * @note     (1) This function should be used only if you cannot use the function
 *               USBD_CDC_EEM_NetIF_Reg().
 *******************************************************************************************************/
CPU_INT08U USBD_CDC_EEM_Add(RTOS_ERR *p_err)
{
  CPU_INT08U class_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBD_CLASS_NBR_NONE);

  CORE_ENTER_ATOMIC();
  if (USBD_CDC_EEM_Ptr->CtrlNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_SUBCLASS_INSTANCE_ALLOC);
    return (USBD_CLASS_NBR_NONE);
  }

  USBD_CDC_EEM_Ptr->CtrlNbrNext--;
  class_nbr = USBD_CDC_EEM_Ptr->CtrlNbrNext;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (class_nbr);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_ConfigAdd()
 *
 * @brief    Adds the CDC-EEM class instance into the specified configuration.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    config_nbr  Configuration index to which to add the CDC-EEM class instance.
 *
 * @param    p_if_name   Pointer to the string that contains name of the CDC-EEM interface.
 *                       Can be DEF_NULL.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_USB_IF_ALT_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_USB_EP_NONE_AVAIL
 *******************************************************************************************************/
void USBD_CDC_EEM_ConfigAdd(CPU_INT08U     class_nbr,
                            CPU_INT08U     dev_nbr,
                            CPU_INT08U     config_nbr,
                            const CPU_CHAR *p_if_name,
                            RTOS_ERR       *p_err)
{
  USBD_CDC_EEM_CTRL *p_ctrl;
  USBD_CDC_EEM_COMM *p_comm;
  CPU_INT08U        if_nbr;
  CPU_INT08U        ep_addr;
  CPU_INT16U        comm_nbr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];
  CORE_ENTER_ATOMIC();
  if ((p_ctrl->DevNbr != USBD_DEV_NBR_NONE)                     // Chk if class is associated with a different dev.
      && (p_ctrl->DevNbr != dev_nbr)) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  p_ctrl->DevNbr = dev_nbr;

  if (USBD_CDC_EEM_Ptr->CommNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  USBD_CDC_EEM_Ptr->CommNbrNext--;
  comm_nbr = USBD_CDC_EEM_Ptr->CommNbrNext;
  CORE_EXIT_ATOMIC();

  p_comm = &USBD_CDC_EEM_Ptr->CommTbl[comm_nbr];

  //                                                               ------------------ BUILD USB FNCT ------------------
  if_nbr = USBD_IF_Add(dev_nbr,
                       config_nbr,
                       &USBD_CDC_EEM_Drv,
                       (void *)p_comm,
                       DEF_NULL,
                       USBD_CLASS_CODE_CDC_CONTROL,
                       USBD_CDC_EEM_SUBCLASS_CODE,
                       USBD_CDC_EEM_PROTOCOL_CODE,
                       p_if_name,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  ep_addr = USBD_BulkAdd(dev_nbr,
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_YES,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_comm->DataInEpAddr = ep_addr;

  ep_addr = USBD_BulkAdd(dev_nbr,
                         config_nbr,
                         if_nbr,
                         0u,
                         DEF_NO,
                         0u,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_comm->DataOutEpAddr = ep_addr;

  CORE_ENTER_ATOMIC();
  p_ctrl->State = USBD_CDC_EEM_STATE_INIT;
  p_ctrl->ClassNbr = class_nbr;
  p_ctrl->CommPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();

  p_comm->CtrlPtr = p_ctrl;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_IsConn()
 *
 * @brief    Gets the CDC-EEM class instance connection state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   DEF_YES, if the class instance is connected.
 *           DEF_NO,  if the class instance is NOT connected.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_CDC_EEM_IsConn(CPU_INT08U class_nbr)
{
  USBD_DEV_STATE     dev_state;
  USBD_CDC_EEM_CTRL  *p_ctrl;
  USBD_CDC_EEM_STATE class_state;
  RTOS_ERR           err;

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), err, RTOS_ERR_INVALID_ARG, DEF_NO);
#endif

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];
  dev_state = USBD_DevStateGet(p_ctrl->DevNbr, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  USBD_CDC_EEM_StateLock(p_ctrl, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  class_state = p_ctrl->State;
  USBD_CDC_EEM_StateUnlock(p_ctrl, &err);

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (dev_state == USBD_DEV_STATE_CONFIGURED)
      && (class_state == USBD_CDC_EEM_STATE_CFG)) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_InstanceInit()
 *
 * @brief    Initializes CDC-EEM class instance according to network driver needs.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_cfg           Pointer to CDC-EEM configuration structure.
 *
 * @param    p_cdc_eem_drv   Pointer to CDC-EEM driver structure.
 *
 * @param    p_arg           Pointer to CDC-EEM driver argument.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void USBD_CDC_EEM_InstanceInit(CPU_INT08U                class_nbr,
                               USBD_CDC_EEM_INSTANCE_CFG *p_cfg,
                               USBD_CDC_EEM_DRV          *p_cdc_eem_drv,
                               void                      *p_arg,
                               RTOS_ERR                  *p_err)
{
  USBD_CDC_EEM_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_cfg->RxBufQSize > 0u)
                           && (p_cfg->TxBufQSize > 1u)), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET((p_cdc_eem_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  //                                                               Ensure class instance is not already configured.
  RTOS_ASSERT_DBG_ERR_SET((p_ctrl->DrvPtr == DEF_NULL), *p_err, RTOS_ERR_INVALID_CLASS_STATE,; );

  p_ctrl->DrvPtr = p_cdc_eem_drv;
  p_ctrl->DrvArgPtr = p_arg;

  //                                                               ---------------- ALLOC TX AND RX QS ----------------
  p_ctrl->TxBufQ.Size = p_cfg->TxBufQSize + 1u;                 // Add 1 entry for echo buf.
  p_ctrl->TxBufQ.Tbl = (USBD_CDC_EEM_BUF_ENTRY *)Mem_SegAlloc("USBD - CDC EEM Tx buf Q",
                                                              USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CDC_EEM_BUF_ENTRY) * p_ctrl->TxBufQ.Size,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_ctrl->RxBufQ.Size = p_cfg->RxBufQSize;
  p_ctrl->RxBufQ.Tbl = (USBD_CDC_EEM_BUF_ENTRY *)Mem_SegAlloc("USBD - CDC EEM Rx buf Q",
                                                              USBD_CDC_EEM_InitCfg.MemSegPtr,
                                                              sizeof(USBD_CDC_EEM_BUF_ENTRY) * p_ctrl->RxBufQ.Size,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Start()
 *
 * @brief    Starts communication on given CDC-EEM class instance.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_USB_INVALID_DEV_STATE
 *                           - RTOS_ERR_NULL_PTR
 *                           - RTOS_ERR_USB_EP_QUEUING
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_USB_INVALID_EP_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @note     (1) This function will start communication only if the class instance is connected.
 *******************************************************************************************************/
void USBD_CDC_EEM_Start(CPU_INT08U class_nbr,
                        RTOS_ERR   *p_err)
{
  USBD_CDC_EEM_CTRL *p_ctrl;
  RTOS_ERR          err_unlock;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  USBD_CDC_EEM_StateLock(p_ctrl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_ctrl->StartCnt++;

  if ((p_ctrl->StartCnt == 1u)                                  // If net drv started and class connected, start comm.
      && (p_ctrl->State == USBD_CDC_EEM_STATE_CFG)) {
    USBD_CDC_EEM_CommStart(p_ctrl, p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  USBD_CDC_EEM_StateUnlock(p_ctrl, &err_unlock);
  (void)&err_unlock;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Stop()
 *
 * @brief    Stops communication on given CDC-EEM class instance.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_err       Pointer to the variable that will receive one of these returned error codes from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OWNERSHIP
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *           Network driver.
 *
 * @note     (1) This function will stop communication only if the class instance is connected.
 *******************************************************************************************************/
void USBD_CDC_EEM_Stop(CPU_INT08U class_nbr,
                       RTOS_ERR   *p_err)
{
  USBD_CDC_EEM_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  USBD_CDC_EEM_StateLock(p_ctrl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_ctrl->StartCnt > 0u) {
    p_ctrl->StartCnt--;
  }

  if ((p_ctrl->StartCnt == 0u)                                  // If class conn still connected, stop comm.
      && (p_ctrl->State == USBD_CDC_EEM_STATE_CFG)) {
    RTOS_ERR err_abort;

    USBD_EP_Abort(p_ctrl->DevNbr,
                  p_ctrl->CommPtr->DataInEpAddr,
                  &err_abort);

    USBD_EP_Abort(p_ctrl->DevNbr,
                  p_ctrl->CommPtr->DataOutEpAddr,
                  &err_abort);

    (void)&err_abort;
  }

  USBD_CDC_EEM_StateUnlock(p_ctrl, p_err);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_DevNbrGet()
 *
 * @brief    Gets the device number associated to this CDC-EEM class instance.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_err       Pointer to the variable that will receive the returned error code from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Device number.
 *******************************************************************************************************/
CPU_INT08U USBD_CDC_EEM_DevNbrGet(CPU_INT08U class_nbr,
                                  RTOS_ERR   *p_err)
{
  USBD_CDC_EEM_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, 0u);
#endif

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_ctrl->DevNbr);
}

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_RxDataPktGet()
 *
 * @brief    Gets first received data packet from received queue.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_rx_len        Pointer to the variable that will receive the length of the received
 *                           buffer in bytes.
 *
 * @param    p_crc_computed  Pointer to the variable that will receive the status of the ethernet CRC
 *                           in buffer. Can be DEF_NULL.
 *                               - DEF_YES     CRC is computed.
 *                               - DEF_NO      CRC is not computed. Last 4 bytes of buffer are set to
 *                                             0xDEADBEEF.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_RX
 *
 * @return   Pointer to received buffer, if successful.
 *           DEF_NULL,                   otherwise.
 *******************************************************************************************************/
CPU_INT08U *USBD_CDC_EEM_RxDataPktGet(CPU_INT08U  class_nbr,
                                      CPU_INT16U  *p_rx_len,
                                      CPU_BOOLEAN *p_crc_computed,
                                      RTOS_ERR    *p_err)
{
  CPU_INT08U        *p_buf;
  USBD_CDC_EEM_CTRL *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);
#endif

  RTOS_ASSERT_DBG_ERR_SET((p_rx_len != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  CORE_ENTER_ATOMIC();                                          // Get next buffer if any.
  p_buf = USBD_CDC_EEM_BufQ_Get(&p_ctrl->RxBufQ,
                                p_rx_len,
                                p_crc_computed);
  CORE_EXIT_ATOMIC();

  if (p_buf != DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
  }

  return (p_buf);
}

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_TxDataPktSubmit()
 *
 * @brief    Submits a Tx buffer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to buffer to submit.
 *
 * @param    buf_len         Buffer length in bytes.
 *
 * @param    crc_computed    Flag that indicates if submitted buffer contains a computed ethernet CRC.
 *
 * @param    p_err           Pointer to the variable that will receive one of these returned error codes from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_USB_INVALID_DEV_STATE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_USB_EP_QUEUING
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_USB_INVALID_EP
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_INVALID_CLASS_STATE
 *                               - RTOS_ERR_USB_INVALID_EP_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Buffers submitted using this function MUST have a padding of 2 bytes at the
 *               beginning.
 *******************************************************************************************************/
void USBD_CDC_EEM_TxDataPktSubmit(CPU_INT08U  class_nbr,
                                  CPU_INT08U  *p_buf,
                                  CPU_INT32U  buf_len,
                                  CPU_BOOLEAN crc_computed,
                                  RTOS_ERR    *p_err)
{
  CPU_INT16U        hdr;
  USBD_CDC_EEM_CTRL *p_ctrl;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (RTOS_ARG_CHK_EXT_EN)
  RTOS_ASSERT_DBG_ERR_SET((class_nbr < USBD_CDC_EEM_Ptr->ClassInstanceQty), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  RTOS_ASSERT_DBG_ERR_SET(((p_buf != DEF_NULL) || (buf_len == 0u)), *p_err, RTOS_ERR_NULL_PTR,; );

  p_ctrl = &USBD_CDC_EEM_Ptr->CtrlTbl[class_nbr];

  //                                                               Prepare header.
  hdr = DEF_BIT_FIELD_ENC(USBD_CDC_EEM_PKT_TYPE_PAYLOAD, USBD_CDC_EEM_PKT_TYPE_MASK);

  if (crc_computed == DEF_NO) {
    //                                                             Set CRC to 0xDEADBEEF as per CDC-EEM specification.
    Mem_Copy((void *)&p_buf[buf_len + USBD_CDC_EEM_HDR_LEN],
             (void *) USBD_CDC_EEM_PayloadCRC,
             4u);

    hdr |= DEF_BIT_FIELD_ENC(USBD_CDC_EEM_PAYLOAD_CRC_NOT_CALC, USBD_CDC_EEM_PAYLOAD_CRC_MASK)
           | DEF_BIT_FIELD_ENC(buf_len + 4u, USBD_CDC_EEM_PAYLOAD_LEN_MASK);
  } else {
    hdr |= DEF_BIT_FIELD_ENC(USBD_CDC_EEM_PAYLOAD_CRC_CALC, USBD_CDC_EEM_PAYLOAD_CRC_MASK)
           | DEF_BIT_FIELD_ENC(buf_len, USBD_CDC_EEM_PAYLOAD_LEN_MASK);
  }

  ((CPU_INT16U *)p_buf)[0u] = MEM_VAL_GET_INT16U_LITTLE(&hdr);  // Copy hdr to buffer.

  USBD_CDC_EEM_TxBufSubmit(p_ctrl,                              // Send buffer to host.
                           p_buf,
                           buf_len + USBD_CDC_EEM_HDR_LEN + 4u,
                           p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Conn()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface to.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *
 *           Core layer.
 *******************************************************************************************************/
static void USBD_CDC_EEM_Conn(CPU_INT08U dev_nbr,
                              CPU_INT08U config_nbr,
                              void       *p_if_class_arg)
{
  USBD_CDC_EEM_COMM *p_comm = (USBD_CDC_EEM_COMM *)p_if_class_arg;
  RTOS_ERR          err;

  (void)&dev_nbr;
  (void)&config_nbr;

  USBD_CDC_EEM_StateLock(p_comm->CtrlPtr, &err);

  p_comm->CtrlPtr->State = USBD_CDC_EEM_STATE_CFG;
  p_comm->CtrlPtr->CommPtr = p_comm;

  if (p_comm->CtrlPtr->StartCnt > 0u) {                         // If class instance is started by net drv, start comm.
    USBD_CDC_EEM_CommStart(p_comm->CtrlPtr, &err);
  }

  USBD_CDC_EEM_StateUnlock(p_comm->CtrlPtr, &err);

  (void)&err;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_Disconn()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    dev_nbr         Device number.
 *
 * @param    config_nbr      Configuration index to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *
 *           Core layer.
 *******************************************************************************************************/
static void USBD_CDC_EEM_Disconn(CPU_INT08U dev_nbr,
                                 CPU_INT08U config_nbr,
                                 void       *p_if_class_arg)
{
  USBD_CDC_EEM_COMM *p_comm = (USBD_CDC_EEM_COMM *)p_if_class_arg;
  RTOS_ERR          err_lock;

  (void)&dev_nbr;
  (void)&config_nbr;

  USBD_CDC_EEM_StateLock(p_comm->CtrlPtr, &err_lock);
  p_comm->CtrlPtr->State = USBD_CDC_EEM_STATE_INIT;
  p_comm->CtrlPtr->CommPtr = DEF_NULL;
  USBD_CDC_EEM_StateUnlock(p_comm->CtrlPtr, &err_lock);

  (void)&err_lock;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_CommStart()
 *
 * @brief    Starts communication on given class instance.
 *
 * @param    p_ctrl  Pointer to class instance control structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of these returned error codes from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_USB_INVALID_DEV_STATE
 *                       - RTOS_ERR_NULL_PTR
 *                       - RTOS_ERR_USB_EP_QUEUING
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_USB_INVALID_EP
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_USB_INVALID_EP_STATE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @note     (1) State of CDC-EEM must be locked by caller function.
 *******************************************************************************************************/
static void USBD_CDC_EEM_CommStart(USBD_CDC_EEM_CTRL *p_ctrl,
                                   RTOS_ERR          *p_err)
{
  CPU_INT08U        buf_cnt;
  USBD_CDC_EEM_COMM *p_comm;

  p_comm = p_ctrl->CommPtr;

  //                                                               Init Rx state machine.
  p_ctrl->CurBufLenRem = 0u;
  p_ctrl->CurBufPtr = DEF_NULL;
  p_ctrl->CurBufIx = 0u;
  p_ctrl->CurBufCrcComputed = DEF_NO;
  p_ctrl->RxErrCnt = 0u;

  //                                                               Init Tx and Rx Qs.
  p_ctrl->TxBufQ.InIdx = 0u;
  p_ctrl->TxBufQ.OutIdx = 0u;
  p_ctrl->TxBufQ.Cnt = 0u;
  p_ctrl->TxInProgress = DEF_NO;

  p_ctrl->RxBufQ.InIdx = 0u;
  p_ctrl->RxBufQ.OutIdx = 0u;
  p_ctrl->RxBufQ.Cnt = 0u;

  //                                                               Submit all avail Rx buffers.
  for (buf_cnt = 0u; buf_cnt < USBD_CDC_EEM_InitCfg.RxBufQty; buf_cnt++) {
    USBD_BulkRxAsync(p_ctrl->DevNbr,
                     p_comm->DataOutEpAddr,
                     p_ctrl->RxBufPtrTbl[buf_cnt],
                     USBD_CDC_EEM_InitCfg.RxBufLen,
                     USBD_CDC_EEM_RxCmpl,
                     (void *)p_ctrl,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      break;
    }
  }
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_RxCmpl()
 *
 * @brief    Inform the application about the Bulk OUT transfer completion.
 *
 * @param    dev_nbr     Device number
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_CDC_EEM_RxCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err)
{
  CPU_INT16U        buf_ix_cur = 0u;
  USBD_CDC_EEM_CTRL *p_ctrl = (USBD_CDC_EEM_CTRL *)p_arg;
  RTOS_ERR          err_usbd;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&ep_addr;
  (void)&buf_len;

  switch (RTOS_ERR_CODE_GET(err)) {                             // Chk errors.
    case RTOS_ERR_NONE:
      p_ctrl->RxErrCnt = 0u;
      break;

    case RTOS_ERR_ABORT:
      return;

    default:
      p_ctrl->RxErrCnt++;                                       // Retry a few times.
      if (p_ctrl->RxErrCnt > USBD_CDC_EEM_MAX_RETRY_CNT) {
        return;
      }

      goto resubmit;
  }

  if (p_ctrl->CurBufLenRem > 0u) {                              // If previous received buffer was incomplete.
    CPU_INT16U len_to_copy = DEF_MIN(xfer_len, p_ctrl->CurBufLenRem);

    if (p_ctrl->CurBufPtr != DEF_NULL) {
      Mem_Copy((void *)&p_ctrl->CurBufPtr[p_ctrl->CurBufIx],
               p_buf,
               len_to_copy);
    }

    p_ctrl->CurBufIx += len_to_copy;
    p_ctrl->CurBufLenRem -= len_to_copy;
    buf_ix_cur += len_to_copy;

    if (p_ctrl->CurBufLenRem != 0u) {
      goto resubmit;
    }
  }

  do {
    CPU_INT08U cmd;
    CPU_INT16U cmd_param;

    if (p_ctrl->CurBufIx == 0u) {                               // Get hdr only if not in data copy phase.
      p_ctrl->CurHdr = MEM_VAL_GET_INT16U_LITTLE(&((CPU_INT08U *)p_buf)[buf_ix_cur]);
    }

    //                                                             Ensure not a Zero-Length EEM (ZLE) pkt.
    if (p_ctrl->CurHdr == USBD_CDC_EEM_HDR_ZLE) {
      buf_ix_cur += USBD_CDC_EEM_HDR_LEN;
      continue;
    }

    switch (DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_PKT_TYPE_MASK)) {
      case USBD_CDC_EEM_PKT_TYPE_CMD:                           // --------------------- EEM CMD ----------------------
                                                                // Ensure rsvd bit is 0.
        if (DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_CMD_RSVD_MASK) != USBD_CDC_EEM_CMD_RSVD_OK) {
          buf_ix_cur += USBD_CDC_EEM_HDR_LEN;
          continue;
        }

        cmd = DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_CMD_CODE_MASK);
        cmd_param = DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_CMD_PARAM_MASK);

        switch (cmd) {
          case USBD_CDC_EEM_CMD_CODE_ECHO:                      // --------------------- ECHO CMD ---------------------
            if (p_ctrl->CurBufIx == 0u) {
              CPU_INT16U len_to_copy;

              buf_ix_cur += USBD_CDC_EEM_HDR_LEN;
              p_ctrl->CurBufIx += USBD_CDC_EEM_HDR_LEN;

              //                                                   Copy echo buffer.
              p_ctrl->CurBufPtr = p_ctrl->BufEchoPtr;
              p_ctrl->CurBufLenRem = DEF_MIN(DEF_BIT_FIELD_RD(cmd_param, USBD_CDC_EEM_CMD_PARAM_ECHO_LEN_MASK),
                                             USBD_CDC_EEM_InitCfg.EchoBufLen - USBD_CDC_EEM_HDR_LEN);

              len_to_copy = DEF_MIN(p_ctrl->CurBufLenRem, (xfer_len - buf_ix_cur));
              Mem_Copy((void *)&p_ctrl->CurBufPtr[p_ctrl->CurBufIx],
                       &((CPU_INT08U *)p_buf)[buf_ix_cur],
                       len_to_copy);
              p_ctrl->CurBufIx += len_to_copy;
              p_ctrl->CurBufLenRem -= len_to_copy;

              buf_ix_cur += len_to_copy;
            }

            if (p_ctrl->CurBufLenRem == 0u) {                   // If buf copy done, prepare echo resp reply.
              CPU_INT16U *p_hdr = &((CPU_INT16U *)p_ctrl->CurBufPtr)[0u];

              //                                                   Prepare hdr.
              *p_hdr = DEF_BIT_FIELD_ENC(USBD_CDC_EEM_PKT_TYPE_CMD, USBD_CDC_EEM_PKT_TYPE_MASK)
                       | DEF_BIT_FIELD_ENC(USBD_CDC_EEM_CMD_CODE_ECHO_RESP, USBD_CDC_EEM_CMD_CODE_MASK)
                       | DEF_BIT_FIELD_ENC(p_ctrl->CurBufIx - USBD_CDC_EEM_HDR_LEN, USBD_CDC_EEM_CMD_PARAM_ECHO_LEN_MASK);

              USBD_CDC_EEM_TxBufSubmit(p_ctrl,
                                       p_ctrl->CurBufPtr,
                                       p_ctrl->CurBufIx,
                                       &err_usbd);

              p_ctrl->CurBufIx = 0u;
            }
            break;

          case USBD_CDC_EEM_CMD_CODE_ECHO_RESP:                 // ----------------- UNSUPPORTED CMDS -----------------
          case USBD_CDC_EEM_CMD_CODE_TICKLE:
          case USBD_CDC_EEM_CMD_CODE_SUSP_HINT:
          case USBD_CDC_EEM_CMD_CODE_RESP_HINT:
          case USBD_CDC_EEM_CMD_CODE_RESP_CMPL_HINT:
          default:
            buf_ix_cur += USBD_CDC_EEM_HDR_LEN;
            break;
        }
        break;

      case USBD_CDC_EEM_PKT_TYPE_PAYLOAD:                       // ------------------- EEM PAYLOADS -------------------
        if (p_ctrl->CurBufIx == 0u) {
          CPU_INT16U net_buf_len;
          CPU_INT16U len_to_copy;

          buf_ix_cur += USBD_CDC_EEM_HDR_LEN;
          p_ctrl->CurBufPtr = p_ctrl->DrvPtr->RxBufGet(p_ctrl->ClassNbr,
                                                       p_ctrl->DrvArgPtr,
                                                       &net_buf_len);

          p_ctrl->CurBufLenRem = DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_PAYLOAD_LEN_MASK);

          if (DEF_BIT_FIELD_RD(p_ctrl->CurHdr, USBD_CDC_EEM_PAYLOAD_CRC_MASK) == USBD_CDC_EEM_PAYLOAD_CRC_CALC) {
            p_ctrl->CurBufCrcComputed = DEF_YES;
          } else {
            p_ctrl->CurBufCrcComputed = DEF_NO;
          }

          //                                                       Copy payload content to network drv's buf.
          len_to_copy = DEF_MIN(p_ctrl->CurBufLenRem, (xfer_len - buf_ix_cur));
          if (p_ctrl->CurBufPtr != DEF_NULL) {
            Mem_Copy((void *)&p_ctrl->CurBufPtr[p_ctrl->CurBufIx],
                     &((CPU_INT08U *)p_buf)[buf_ix_cur],
                     len_to_copy);
          }
          p_ctrl->CurBufIx += len_to_copy;
          p_ctrl->CurBufLenRem -= len_to_copy;

          buf_ix_cur += len_to_copy;
        }

        if (p_ctrl->CurBufLenRem == 0u) {                       // If reception complete, submit buf to network drv.
          if (p_ctrl->CurBufPtr != DEF_NULL) {
            CORE_ENTER_ATOMIC();
            USBD_CDC_EEM_BufQ_Add(&p_ctrl->RxBufQ,
                                  p_ctrl->CurBufPtr,
                                  p_ctrl->CurBufIx,
                                  p_ctrl->CurBufCrcComputed);
            CORE_EXIT_ATOMIC();

            p_ctrl->DrvPtr->RxBufRdy(p_ctrl->ClassNbr,
                                     p_ctrl->DrvArgPtr);
          }

          p_ctrl->CurBufIx = 0u;
        }
        break;

      default:
        break;
    }
  } while (buf_ix_cur <= (xfer_len - USBD_CDC_EEM_HDR_LEN));

resubmit:
  USBD_CDC_EEM_StateLock(p_ctrl, &err_usbd);

  if ((p_ctrl->StartCnt > 0u)                                   // Re-submit buf if still connected.
      && (p_ctrl->State == USBD_CDC_EEM_STATE_CFG)) {
    USBD_BulkRxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->DataOutEpAddr,
                     p_buf,
                     USBD_CDC_EEM_InitCfg.RxBufLen,
                     USBD_CDC_EEM_RxCmpl,
                     (void *)p_ctrl,
                     &err_usbd);
  }

  USBD_CDC_EEM_StateUnlock(p_ctrl, &err_usbd);

  (void)&err_usbd;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_TxCmpl()
 *
 * @brief    Inform the application about the Bulk IN transfer completion.
 *
 * @param    dev_nbr     Device number
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets sent.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    err         Transfer status: success or error.
 *******************************************************************************************************/
static void USBD_CDC_EEM_TxCmpl(CPU_INT08U dev_nbr,
                                CPU_INT08U ep_addr,
                                void       *p_buf,
                                CPU_INT32U buf_len,
                                CPU_INT32U xfer_len,
                                void       *p_arg,
                                RTOS_ERR   err)
{
  CPU_INT08U        *p_next_buf;
  CPU_INT16U        next_buf_len;
  USBD_CDC_EEM_CTRL *p_ctrl = (USBD_CDC_EEM_CTRL *)p_arg;
  RTOS_ERR          err_lock;
  CORE_DECLARE_IRQ_STATE;

  (void)&dev_nbr;
  (void)&ep_addr;
  (void)&xfer_len;
  (void)&err;

  if ((p_buf != p_ctrl->BufEchoPtr)
      && (p_buf != DEF_NULL)) {                                 // Free Tx buf to network drv.
    p_ctrl->DrvPtr->TxBufFree(p_ctrl->ClassNbr,
                              p_ctrl->DrvArgPtr,
                              (CPU_INT08U *)p_buf,
                              buf_len);
  }

  USBD_CDC_EEM_StateLock(p_ctrl, &err_lock);

  if ((p_ctrl->State != USBD_CDC_EEM_STATE_CFG)
      || (p_ctrl->StartCnt == 0u)) {
    USBD_CDC_EEM_StateUnlock(p_ctrl, &err_lock);
    return;
  }

  CORE_ENTER_ATOMIC();                                          // Submit next buffer if any.
  p_next_buf = USBD_CDC_EEM_BufQ_Get(&p_ctrl->TxBufQ,
                                     &next_buf_len,
                                     DEF_NULL);
  if (p_next_buf == DEF_NULL) {
    p_ctrl->TxInProgress = DEF_NO;
  }
  CORE_EXIT_ATOMIC();

  if (p_next_buf != DEF_NULL) {
    RTOS_ERR err_submit;

    USBD_BulkTxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->DataInEpAddr,
                     (void *)p_next_buf,
                     next_buf_len,
                     USBD_CDC_EEM_TxCmpl,
                     (void *)p_ctrl,
                     DEF_YES,
                     &err_submit);

    (void)&err_submit;
  }

  USBD_CDC_EEM_StateUnlock(p_ctrl, &err_lock);
  (void)&err_lock;
}

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_TxBufSubmit()
 *
 * @brief    Submits a transmit buffer.
 *
 * @param    p_ctrl      Pointer to class instance control structure.
 *
 * @param    p_buf       Pointer to buffer that contains data to send.
 *
 * @param    buf_len     Length of buffer in bytes.
 *
 * @param    p_err       Pointer to the variable that will receive the returned error code from this function.
 *******************************************************************************************************/
static void USBD_CDC_EEM_TxBufSubmit(USBD_CDC_EEM_CTRL *p_ctrl,
                                     CPU_INT08U        *p_buf,
                                     CPU_INT16U        buf_len,
                                     RTOS_ERR          *p_err)
{
  CPU_BOOLEAN tx_in_progress;
  RTOS_ERR    err_unlock;
  CORE_DECLARE_IRQ_STATE;

  USBD_CDC_EEM_StateLock(p_ctrl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((p_ctrl->State != USBD_CDC_EEM_STATE_CFG)
      || (p_ctrl->StartCnt == 0u)) {
    USBD_CDC_EEM_StateUnlock(p_ctrl, &err_unlock);

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CLASS_STATE);
    return;
  }

  CORE_ENTER_ATOMIC();
  tx_in_progress = p_ctrl->TxInProgress;
  if (tx_in_progress == DEF_NO) {
    p_ctrl->TxInProgress = DEF_YES;                             // Submit buffer if no buffer in Q.
  } else {
    USBD_CDC_EEM_BufQ_Add(&p_ctrl->TxBufQ,                      // Add buffer to Q.
                          p_buf,
                          buf_len,
                          DEF_NO);
  }
  CORE_EXIT_ATOMIC();

  if (tx_in_progress == DEF_NO) {
    USBD_BulkTxAsync(p_ctrl->DevNbr,
                     p_ctrl->CommPtr->DataInEpAddr,
                     (void *)p_buf,
                     buf_len,
                     USBD_CDC_EEM_TxCmpl,
                     (void *)p_ctrl,
                     DEF_YES,
                     p_err);
  }

  USBD_CDC_EEM_StateUnlock(p_ctrl, &err_unlock);
  (void)&err_unlock;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_BufQ_Add()
 *
 * @brief    Adds a buffer to the tail of the Q.
 *
 * @param    p_buf_q         Pointer to buffer Q.
 *
 * @param    p_buf           _q         Pointer to buffer Q.
 *
 * @param    buf_len         Length of buffer in bytes.
 *
 * @param    crc_computed    Flag that indicates if ethernet CRC is computed in submitted buffer.
 *
 * @note     (1) This function must be called from within a critical section.
 *******************************************************************************************************/
static void USBD_CDC_EEM_BufQ_Add(USBD_CDC_EEM_BUF_Q *p_buf_q,
                                  CPU_INT08U         *p_buf,
                                  CPU_INT16U         buf_len,
                                  CPU_BOOLEAN        crc_computed)
{
  if (p_buf_q->Cnt >= p_buf_q->Size) {
    return;
  }

  p_buf_q->Tbl[p_buf_q->InIdx].BufPtr = p_buf;
  p_buf_q->Tbl[p_buf_q->InIdx].BufLen = buf_len;
  p_buf_q->Tbl[p_buf_q->InIdx].CrcComputed = crc_computed;

  p_buf_q->InIdx++;
  if (p_buf_q->InIdx >= p_buf_q->Size) {
    p_buf_q->InIdx = 0u;
  }

  p_buf_q->Cnt++;
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_BufQ_Get()
 *
 * @brief    Gets a buffer from the head of the Q.
 *
 * @param    p_buf_q         Pointer to buffer Q.
 *
 * @param    p_buf_len       Pointer to the variable that will receive length of buffer in bytes.
 *
 * @param    p_crc_computed  Pointer to the variable that will receive the CRC computes state of the
 *                           buffer. Can be DEF_NULL if ignored.
 *                           DEF_YES     Buffer contains a valid/computed ethernet CRC.
 *                           DEF_NO      Buffer does not contains a valid/computed ethernet CRC.
 *
 * @return   Pointer to buffer.
 *
 * @note     (1) This function must be called from within a critical section.
 *******************************************************************************************************/
static CPU_INT08U *USBD_CDC_EEM_BufQ_Get(USBD_CDC_EEM_BUF_Q *p_buf_q,
                                         CPU_INT16U         *p_buf_len,
                                         CPU_BOOLEAN        *p_crc_computed)
{
  CPU_INT08U *p_buf;

  if (p_buf_q->Cnt == 0u) {
    return (DEF_NULL);
  }

  p_buf = p_buf_q->Tbl[p_buf_q->OutIdx].BufPtr;
  *p_buf_len = p_buf_q->Tbl[p_buf_q->OutIdx].BufLen;

  if (p_crc_computed != DEF_NULL) {
    *p_crc_computed = p_buf_q->Tbl[p_buf_q->OutIdx].CrcComputed;
  }

  p_buf_q->OutIdx++;
  if (p_buf_q->OutIdx >= p_buf_q->Size) {
    p_buf_q->OutIdx = 0u;
  }

  p_buf_q->Cnt--;

  return (p_buf);
}

/****************************************************************************************************//**
 *                                           USBD_CDC_EEM_StateLock()
 *
 * @brief    Locks CDC-EEM class instance state.
 *
 * @param    p_ctrl  Pointer to class instance control structure.
 *
 * @param    p_err   Pointer to the variable that will receive the returned error code from this function.
 *******************************************************************************************************/
static void USBD_CDC_EEM_StateLock(USBD_CDC_EEM_CTRL *p_ctrl,
                                   RTOS_ERR          *p_err)
{
  KAL_LockAcquire(p_ctrl->StateLockHandle,
                  KAL_OPT_PEND_NONE,
                  0u,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBD_CDC_EEM_StateUnlock()
 *
 * @brief    Unlocks CDC-EEM class instance state.
 *
 * @param    p_ctrl  Pointer to class instance control structure.
 *
 * @param    p_err   Pointer to the variable that will receive the returned error code from this function.
 *******************************************************************************************************/
static void USBD_CDC_EEM_StateUnlock(USBD_CDC_EEM_CTRL *p_ctrl,
                                     RTOS_ERR          *p_err)
{
  KAL_LockRelease(p_ctrl->StateLockHandle,
                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_EEM_AVAIL))
