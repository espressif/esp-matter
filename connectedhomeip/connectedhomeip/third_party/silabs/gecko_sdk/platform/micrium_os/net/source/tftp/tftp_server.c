/***************************************************************************//**
 * @file
 * @brief Network - TFTP Server
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

#include <rtos_description.h>

#if (defined(RTOS_MODULE_NET_TFTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error TFTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

#if (!defined(RTOS_MODULE_FS_AVAIL))
#error TFTP Server Module requires File System module. Make sure it is part of your project \
  and that RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/tftp_server.h>
#include  <net/include/net_cfg_net.h>
#include  <net/include/net_util.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_fs.h>
#include  <net/source/tcpip/net_tmr_priv.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#endif

#include  <common/source/kal/kal_priv.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_prio.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                            (NET, TFTP)
#define  RTOS_MODULE_CUR                         RTOS_CFG_MODULE_NET

#define  TFTPs_TASK_NAME                        "TFTPs Task"

/********************************************************************************************************
 *                                           DEFAULT CONFIGURATION
 *******************************************************************************************************/

#define  TFTPs_INIT_CFG_DFLT            {                           \
    .ConnCfg =                                                      \
    {                                                               \
      .SockSel = TFTP_SERVER_CFG_SOCK_SEL_DFLT,                     \
      .Port = TFTP_SERVER_CFG_PORT_NBR_DFLT,                        \
      .RxTimeoutMax = TFTP_SERVER_CFG_RX_TIMEOUT_MS_DFLT,           \
    },                                                              \
    .StkSizeElements = TFTP_SERVER_TASK_CFG_STK_SIZE_ELEMENTS_DFLT, \
    .StkPtr = TFTP_SERVER_TASK_CFG_STK_PTR_DFLT,                    \
    .MemSegPtr = DEF_NULL                                           \
}

/********************************************************************************************************
 *                                       TFTPs SPECIFIC CONSTANTS
 *******************************************************************************************************/

#define  TFTPs_FILE_OPEN_RD                                0
#define  TFTPs_FILE_OPEN_WR                                1

/********************************************************************************************************
 *                                           TFTP SPECIFIC DEFINES
 *******************************************************************************************************/

#define  TFTP_PKT_OFFSET_OPCODE                            0
#define  TFTP_PKT_OFFSET_FILENAME                          2
#define  TFTP_PKT_OFFSET_BLK_NBR                           2
#define  TFTP_PKT_OFFSET_ERR_CODE                          2
#define  TFTP_PKT_OFFSET_ERR_MSG                           4
#define  TFTP_PKT_OFFSET_DATA                              4

#define  TFTP_PKT_SIZE_OPCODE                              2
#define  TFTP_PKT_SIZE_BLK_NBR                             2
#define  TFTP_PKT_SIZE_ERR_CODE                            2
#define  TFTP_PKT_SIZE_FILENAME_NUL                        1
#define  TFTP_PKT_SIZE_MODE_NUL                            1

//                                                                 ---- TFTP opcodes (see Stevens p. 466) -------------
#define  TFTP_OPCODE_RD_REQ                                1    // Read
#define  TFTP_OPCODE_WR_REQ                                2    // Write
#define  TFTP_OPCODE_DATA                                  3    // Data
#define  TFTP_OPCODE_ACK                                   4    // Acknowledge
#define  TFTP_OPCODE_ERR                                   5    // Error

/********************************************************************************************************
 *                                       TFTPs SPECIFIC DEFINES
 *******************************************************************************************************/

//                                                                 ---- TFTP Server error codes: (see Stevens p. 467) -
#define  TFTPs_ERR_CODE_ERR_STR                            0    // Not defined.
#define  TFTPs_ERR_CODE_FILE_NOT_FOUND                     1    // File not found.
#define  TFTPs_ERR_CODE_ACCESS_VIOLATION                   2    // Access violation.
#define  TFTPs_ERR_CODE_DISK_FULL                          3    // Disk full.
#define  TFTPs_ERR_CODE_ILLEGAL_OP                         4    // Illegal TFTP operation.
#define  TFTPs_ERR_CODE_BAD_PORT_NBR                       5    // Unknown port number.
#define  TFTPs_ERR_CODE_FILE_EXISTS                        6    // File already exists.
#define  TFTPs_ERR_CODE_NO_SUCH_USER                       7    // No such user.

//                                                                 ---- TFTP Server modes -----------------------------
#define  TFTPs_MODE_OCTET                                  1
#define  TFTPs_MODE_NETASCII                               2

//                                                                 ---- TFTP Server states ----------------------------
#define  TFTPs_STATE_IDLE                                  0
#define  TFTPs_STATE_DATA_RD                               1
#define  TFTPs_STATE_DATA_WR                               2

#define  TFTPs_BLOCK_SIZE                                512
#define  TFTPs_BUF_SIZE                         (TFTPs_BLOCK_SIZE + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  tftps_data {
  TFTPs_SOCK_SEL    SockSel;
  CPU_INT16U        Port;
  CPU_INT32U        RxTimeoutMax;

  const CPU_CHAR    *RootDirPtr;

  CPU_INT16U        RxBlkNbr;                                   // Current block number received.
  CPU_INT08U        RxMsgBuf[TFTPs_BUF_SIZE];                   // Incoming packet buffer.
  CPU_INT32U        RxMsgCtr;                                   // Number of messages received.
  CPU_INT32S        RxMsgLen;

  CPU_INT16U        TxBlkNbr;                                   // Current block number being sent.
  CPU_INT08U        TxMsgBuf[TFTPs_BUF_SIZE];                   // Outgoing packet buffer.
  CPU_INT16U        TxMsgCtr;
  CPU_SIZE_T        TxMsgLen;

  NET_SOCK_ADDR     SockAddr;
  NET_SOCK_ADDR_LEN SockAddrLen;
  NET_SOCK_ID       SockID;

  CPU_INT08U        State;                                      // Current state of TFTPs state machine.

  CPU_INT16U        OpCode;

  void              *FileHandle;                                // File handle of currently opened file.

  KAL_TASK_HANDLE   TaskHandle;
} TFTPs_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const TFTPs_INIT_CFG  TFTPs_InitCfgDflt = TFTPs_INIT_CFG_DFLT;
static TFTPs_INIT_CFG TFTPs_InitCfg = TFTPs_INIT_CFG_DFLT;
#else
extern const TFTPs_INIT_CFG TFTPs_InitCfg;
#endif

#ifdef  RTOS_MODULE_FS_AVAIL
static const NET_FS_API *TFTPs_FS_API_Ptr = &NetFS_API_Native;
#else
static const NET_FS_API *TFTPs_FS_API_Ptr = DEF_NULL;
#endif

TFTPs_DATA *TFTPs_DataPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void TFTPs_Task(void *p_data);

static void TFTPs_ServerSockInit(NET_SOCK_PROTOCOL_FAMILY family,
                                 RTOS_ERR                 *p_err);

static void TFTPs_StateIdle(RTOS_ERR *p_err);

static void TFTPs_StateDataRd(RTOS_ERR *p_err);

static void TFTPs_StateDataWr(RTOS_ERR *p_err);

static void TFTPs_GetRxBlkNbr(void);

static void TFTPs_Terminate(void);

static void TFTPs_FileOpen(CPU_BOOLEAN rw,
                           RTOS_ERR    *p_err);

static void *TFTPs_FileOpenMode(CPU_CHAR    *p_filename,
                                CPU_BOOLEAN rw);

static void TFTPs_DataRd(RTOS_ERR *p_err);

static void TFTPs_DataWr(RTOS_ERR *p_err);

static void TFTPs_DataWrAck(CPU_INT32U blk_nbr);

//                                                                 --------------------- TX FNCTS ---------------------
static void TFTPs_TxErr(CPU_INT16U err_code,
                        CPU_CHAR   *p_err_msg);

static NET_SOCK_RTN_CODE TFTPs_Tx(CPU_INT16U opcode,
                                  CPU_INT16U blk_nbr,
                                  CPU_INT08U *p_buf,
                                  CPU_INT16U len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           TFTPs_ConfigureTaskStk()
 *
 * @brief    Configure the TFTP server task stack properties to use the parameters contained in
 *           the passed structure instead of the default parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void TFTPs_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base)
{
  RTOS_ASSERT_DBG((TFTPs_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  TFTPs_InitCfg.StkSizeElements = stk_size_elements;
  TFTPs_InitCfg.StkPtr = p_stk_base;
}
#endif

/****************************************************************************************************//**
 *                                           TFTPs_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by TFTP
 *           server module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void TFTPs_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((TFTPs_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  TFTPs_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                       TFTPs_ConfigureConnParam()
 *
 * @brief    Overwrite the Connection configuration object for TFTP server.
 *
 * @param    p_conn_cfg  Pointer to structure containing the connection parameters.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void TFTPs_ConfigureConnParam(TFTPs_CONN_CFG *p_conn_cfg)
{
  RTOS_ASSERT_DBG((TFTPs_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  TFTPs_InitCfg.ConnCfg = *p_conn_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               TFTPs_Init()
 *
 * @brief    Initializes and starts up the TFTP server.
 *
 * @param    p_root_dir  Pointer to root directory name for the TFTP server.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_SUPPORTED
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void TFTPs_Init(const CPU_CHAR *p_root_dir,
                RTOS_ERR       *p_err)
{
  TFTPs_DATA      *p_data;
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )
  RTOS_ASSERT_DBG_ERR_SET((p_root_dir != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_data = (TFTPs_DATA *)Mem_SegAlloc("TFTP Server Module",
                                      TFTPs_InitCfg.MemSegPtr,
                                      sizeof(TFTPs_DATA),
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------- INIT TFTPs GLOBAL VARS --------------
  p_data->RxBlkNbr = 0;
  p_data->RxMsgCtr = 0;
  p_data->TxBlkNbr = 0;
  p_data->TxMsgCtr = 0;

  p_data->FileHandle = DEF_NULL;
  p_data->State = TFTPs_STATE_IDLE;

  switch (TFTPs_InitCfg.ConnCfg.SockSel) {
    case TFTPs_SOCK_SEL_IPv4:
#ifndef   NET_IPv4_MODULE_EN
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
#endif
      break;

    case TFTPs_SOCK_SEL_IPv6:
#ifndef   NET_IPv6_MODULE_EN
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
#endif
      break;

    case TFTPs_SOCK_SEL_IPv4_IPv6:
#if  (!defined(NET_IPv4_MODULE_EN)) || (!defined(NET_IPv6_MODULE_EN))
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
#endif
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
  }

  //                                                               ------- SAVE NEEDED CONFIGURATION PARAMETERS -------
  p_data->SockSel = TFTPs_InitCfg.ConnCfg.SockSel;
  p_data->Port = TFTPs_InitCfg.ConnCfg.Port;
  p_data->RxTimeoutMax = TFTPs_InitCfg.ConnCfg.RxTimeoutMax;

  p_data->RootDirPtr = p_root_dir;

  //                                                               ------- ALLOCATE MEMORY SPACE FOR TFTPs TASK  ------
  task_handle = KAL_TaskAlloc((const  CPU_CHAR *)TFTPs_TASK_NAME,
                              (CPU_STK *)TFTPs_InitCfg.StkPtr,
                              TFTPs_InitCfg.StkSizeElements,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_data->TaskHandle = task_handle;

  //                                                               ------------- CREATE TFTP SERVER TASK --------------
  KAL_TaskCreate(task_handle,
                 TFTPs_Task,
                 DEF_NULL,
                 TFTP_SERVER_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  TFTPs_DataPtr = p_data;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           TFTPs_TaskPrioSet()
 *
 * @brief    Sets priority of the TFTP server task.
 *
 * @param    prio    New priority for the TFTP server task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void TFTPs_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((TFTPs_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  task_handle = TFTPs_DataPtr->TaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               TFTPs_Task()
 *
 * @brief    TFTP server code loop.
 *
 * @param    p_data  Pointer to task initialization data.
 *
 *                   @note         (1) TID stands for "transfer identifier" as referenced by RFC #1350.
 *
 *                   @note         (2) #### In case of a timeout error, retransmission of the last sent packet should take
 *                   place.  Terminating the transmission might NOT the correct action to take according
 *                   to RFC #1350, Section 2 'Overview of the Protocol', which states that "If a packet
 *                   gets lost in the network, the intended recipient will timeout and may retransmit
 *                   his last packet [...], thus causing the sender of the lost packet to retransmit that
 *                   lost packet".
 *******************************************************************************************************/
static void TFTPs_Task(void *p_data)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_SOCK_ADDR_IPv4 *p_addr_v4_remote;
  NET_SOCK_ADDR_IPv4 *p_addr_v4_server;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_SOCK_ADDR_IPv6 *p_addr_v6_remote;
  NET_SOCK_ADDR_IPv6 *p_addr_v6_server;
#endif
  NET_SOCK_PROTOCOL_FAMILY sock_family = NET_SOCK_PROTOCOL_FAMILY_NONE;
  CPU_INT16U               *p_opcode;
  CPU_BOOLEAN              same_addr;
  CPU_BOOLEAN              valid_tid;                           // See Note #1.
  NET_SOCK_ADDR            addr_ip_remote;
  NET_SOCK_ADDR            addr_ip_server;
  RTOS_ERR                 err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(p_data);

  switch (TFTPs_DataPtr->SockSel) {
    case TFTPs_SOCK_SEL_IPv4:
      sock_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      break;

    case TFTPs_SOCK_SEL_IPv6:
      sock_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      break;

    case TFTPs_SOCK_SEL_IPv4_IPv6:
    default:
      LOG_ERR(("TFTPs: invalid ip family"));
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ------------- SET FS WORKING DIRECTORY -------------
  if (TFTPs_FS_API_Ptr->WorkingFolderSet != DEF_NULL) {
    CPU_BOOLEAN is_set;

    is_set = TFTPs_FS_API_Ptr->WorkingFolderSet((CPU_CHAR *)TFTPs_DataPtr->RootDirPtr);
    if (is_set != DEF_OK) {
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
    }
  }

  //                                                               ----------------- INIT SERVER SOCK -----------------
  TFTPs_ServerSockInit(sock_family, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Set sock to blocking until incoming req.
  NetSock_CfgTimeoutRxQ_Set(TFTPs_DataPtr->SockID,
                            NET_TMR_TIME_INFINITE,
                            &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ----------------- TFTP SERVER LOOP -----------------
  while (DEF_ON) {
    TFTPs_DataPtr->SockAddrLen = sizeof(addr_ip_remote);

    //                                                             --------------- WAIT FOR INCOMING PKT --------------

    TFTPs_DataPtr->RxMsgLen = NetSock_RxDataFrom(TFTPs_DataPtr->SockID,
                                                 &TFTPs_DataPtr->RxMsgBuf[0],
                                                 sizeof(TFTPs_DataPtr->RxMsgBuf),
                                                 NET_SOCK_FLAG_NONE,
                                                 &addr_ip_remote,
                                                 &TFTPs_DataPtr->SockAddrLen,
                                                 DEF_NULL,
                                                 DEF_NULL,
                                                 DEF_NULL,
                                                 &err);
    switch (RTOS_ERR_CODE_GET(err)) {
      case RTOS_ERR_NONE:
        break;

      default:
        TFTPs_Terminate();
        continue;
    }

    TFTPs_DataPtr->RxMsgCtr++;                                  // Inc nbr or rx'd pkts.

    //                                                             --------------- PROCESS INCOMING PKT ---------------
    valid_tid = DEF_YES;

    switch (addr_ip_remote.AddrFamily) {
#ifdef  NET_IPv4_MODULE_EN
      case NET_SOCK_ADDR_FAMILY_IP_V4:
        p_addr_v4_remote = (NET_SOCK_ADDR_IPv4 *)&addr_ip_remote;
        p_addr_v4_server = (NET_SOCK_ADDR_IPv4 *)&TFTPs_DataPtr->SockAddr;

        if ((p_addr_v4_remote->Port != p_addr_v4_server->Port)
            || (p_addr_v4_remote->Addr != p_addr_v4_server->Addr)) {
          valid_tid = DEF_NO;
        }
        PP_UNUSED_PARAM(same_addr);
        break;
#endif
#ifdef  NET_IPv6_MODULE_EN
      case NET_SOCK_ADDR_FAMILY_IP_V6:
        p_addr_v6_remote = (NET_SOCK_ADDR_IPv6 *)&addr_ip_remote;
        p_addr_v6_server = (NET_SOCK_ADDR_IPv6 *)&TFTPs_DataPtr->SockAddr;

        same_addr = Mem_Cmp(&p_addr_v6_remote->Addr.Addr,
                            &p_addr_v6_server->Addr.Addr,
                            NET_IPv6_ADDR_SIZE);

        if (same_addr == DEF_NO) {
          valid_tid = DEF_NO;
        }
        break;
#endif

      default:
        valid_tid = DEF_NO;
        break;
    }

    p_opcode = (CPU_INT16U *)&TFTPs_DataPtr->RxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
    TFTPs_DataPtr->OpCode = NET_UTIL_NET_TO_HOST_16(*p_opcode);
    switch (TFTPs_DataPtr->State) {
      case TFTPs_STATE_IDLE:                                    // Idle state, expecting a new req.
        TFTPs_DataPtr->SockAddr = addr_ip_remote;
        TFTPs_StateIdle(&err);
        break;

      case TFTPs_STATE_DATA_RD:                                 // Processing a rd req.
        if (valid_tid == DEF_YES) {
          TFTPs_StateDataRd(&err);
        } else {
          addr_ip_server = TFTPs_DataPtr->SockAddr;
          TFTPs_DataPtr->SockAddr = addr_ip_remote;
          TFTPs_TxErr(0u, "Transaction denied, Server BUSY");
          TFTPs_DataPtr->SockAddr = addr_ip_server;
        }
        break;

      case TFTPs_STATE_DATA_WR:                                 // Processing a wr req.
        if (valid_tid == DEF_YES) {
          TFTPs_StateDataWr(&err);
        } else {
          addr_ip_server = TFTPs_DataPtr->SockAddr;
          TFTPs_DataPtr->SockAddr = addr_ip_remote;
          TFTPs_TxErr(0u, "Transaction denied, Server BUSY");
          TFTPs_DataPtr->SockAddr = addr_ip_server;
        }
        break;

      default:
        RTOS_ERR_SET(err, RTOS_ERR_INVALID_STATE);
        break;
    }

    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {              // If err, terminate file tx.
      LOG_DBG(("TFTP server: Task, Error, session terminated"));
      TFTPs_Terminate();
    }
  }
}

/****************************************************************************************************//**
 *                                           TFTPs_ServerSockInit()
 *
 * @brief    Initialize the TFTP server socket.
 *
 * @param    family  IP family of the server socket.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_ServerSockInit(NET_SOCK_PROTOCOL_FAMILY family,
                                 RTOS_ERR                 *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR ipv4_addr;
#endif
  CPU_INT08U        *p_addr = 0;
  NET_IP_ADDR_LEN   addr_len = NET_IPv4_ADDR_SIZE;
  NET_SOCK_RTN_CODE bind_status;
  RTOS_ERR          err;

  //                                                               Open a socket to listen for incoming connections.
  TFTPs_DataPtr->SockID = NetSock_Open(family,
                                       NET_SOCK_TYPE_DATAGRAM,
                                       NET_SOCK_PROTOCOL_UDP,
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Bind a local address so the client can send to us.
  Mem_Set(&TFTPs_DataPtr->SockAddr, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);

  switch (family) {
    case NET_SOCK_ADDR_FAMILY_IP_V4:
#ifdef  NET_IPv4_MODULE_EN
      ipv4_addr = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_NONE);
      p_addr = (CPU_INT08U *)&ipv4_addr;
      addr_len = NET_IPv4_ADDR_SIZE;
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
      break;

    case NET_SOCK_ADDR_FAMILY_IP_V6:
#ifdef  NET_IPv6_MODULE_EN
      p_addr = (CPU_INT08U *)&NetIPv6_AddrAny;
      addr_len = NET_IPv6_ADDR_SIZE;
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  NetApp_SetSockAddr(&TFTPs_DataPtr->SockAddr,
                     family,
                     TFTPs_DataPtr->Port,
                     p_addr,
                     addr_len,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bind_status = NetSock_Bind(TFTPs_DataPtr->SockID,
                             &TFTPs_DataPtr->SockAddr,
                             NET_SOCK_ADDR_SIZE,
                             p_err);
  if (bind_status != NET_SOCK_BSD_ERR_NONE) {                   // Could not bind to the TFTPs port.
    NetSock_Close(TFTPs_DataPtr->SockID, &err);
    return;
  }
}

/****************************************************************************************************//**
 *                                               TFTPs_StateIdle()
 *
 * @brief    TFTP server idle state handler.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_StateIdle(RTOS_ERR *p_err)
{
  LOG_VRB(("TFTP server: Idle State"));
  switch (TFTPs_DataPtr->OpCode) {
    case TFTP_OPCODE_RD_REQ:
      //                                                           Open the desired file for reading.
      TFTPs_FileOpen(TFTPs_FILE_OPEN_RD, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      LOG_VRB(("TFTP server: Rd Request, File Opened"));
      TFTPs_DataPtr->TxBlkNbr = 0;
      TFTPs_DataPtr->State = TFTPs_STATE_DATA_RD;
      TFTPs_DataRd(p_err);                                      // Read the first block of data from the file and send
                                                                // to client.
      break;

    case TFTP_OPCODE_ACK:                                       // NOT supposed to get ACKs in the Idle state.
      LOG_VRB(("TFTP server: ACK received, not supposed to!"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;

    case TFTP_OPCODE_WR_REQ:
      TFTPs_DataPtr->TxBlkNbr = 0;
      //                                                           Open the desired file for writing.
      TFTPs_FileOpen(TFTPs_FILE_OPEN_WR, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      LOG_VRB(("TFTP server: Wr Request, File Opened"));
      TFTPs_DataPtr->State = TFTPs_STATE_DATA_WR;
      TFTPs_DataWrAck(TFTPs_DataPtr->TxBlkNbr);                 // Acknowledge the client.
      break;

    case TFTP_OPCODE_DATA:                                      // NOT supposed to get DATA packets in the Idle state.
    case TFTP_OPCODE_ERR:                                       // NOT supposed to get ERR packets in the Idle state.
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    NetSock_CfgTimeoutRxQ_Set(TFTPs_DataPtr->SockID,
                              TFTPs_DataPtr->RxTimeoutMax,
                              p_err);
  }
}

/****************************************************************************************************//**
 *                                           TFTPs_StateDataRd()
 *
 * @brief    Process read action.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_StateDataRd(RTOS_ERR *p_err)
{
  NET_SOCK_RTN_CODE tx_size;

  switch (TFTPs_DataPtr->OpCode) {
    case TFTP_OPCODE_RD_REQ:                                    // NOT supposed to get RRQ pkts in the DATA Read state.
                                                                // Close and re-open file.
      TFTPs_FS_API_Ptr->Close(TFTPs_DataPtr->FileHandle);
      TFTPs_DataPtr->FileHandle = DEF_NULL;
      TFTPs_FileOpen(TFTPs_FILE_OPEN_RD, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        LOG_VRB(("TFTP server: Data Rd, Rx RD_REQ."));
        TFTPs_DataPtr->TxBlkNbr = 0;
        TFTPs_DataPtr->State = TFTPs_STATE_DATA_RD;
        TFTPs_DataRd(p_err);                                    // Read first block of data and tx to client.
      }
      break;

    case TFTP_OPCODE_ACK:
      TFTPs_GetRxBlkNbr();
      if (TFTPs_DataPtr->RxBlkNbr == TFTPs_DataPtr->TxBlkNbr) {        // If sent data ACK'd, ...
        LOG_VRB(("TFTP server: Data Rd, ACK Rx'd"));
        TFTPs_DataRd(p_err);                                    // ... read next block of data and tx to client.
      } else {                                                  // Else re-tx prev block.
        tx_size = TFTPs_Tx(TFTP_OPCODE_DATA,
                           TFTPs_DataPtr->TxBlkNbr,
                           &TFTPs_DataPtr->TxMsgBuf[0],
                           TFTPs_DataPtr->TxMsgLen);

        if (tx_size < 0) {
          TFTPs_TxErr(0, (CPU_CHAR *)"RRQ file read error");
          RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        }
      }
      break;

    case TFTP_OPCODE_WR_REQ:                                    // NOT supposed to get WRQ pkts in the DATA Read state.
      LOG_VRB(("TFTP server: Data Rd, Rx'd WR_REQ"));
      TFTPs_TxErr(0, (CPU_CHAR *)"RRQ server busy, WRQ  opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;

    case TFTP_OPCODE_DATA:                                      // NOT supposed to get DATA pkts in the DATA Read state.
      LOG_VRB(("TFTP server: Data Rd, Rx'd DATA"));
      TFTPs_TxErr(0, (CPU_CHAR *)"RRQ server busy, DATA opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;

    case TFTP_OPCODE_ERR:
      LOG_VRB(("TFTP server: Data Rd, Rx'd ERR"));
      TFTPs_TxErr(0, (CPU_CHAR *)"RRQ server busy, ERR  opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }
}

/****************************************************************************************************//**
 *                                           TFTPs_StateDataWr()
 *
 * @brief    Process write action.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_StateDataWr(RTOS_ERR *p_err)
{
  switch (TFTPs_DataPtr->OpCode) {
    case TFTP_OPCODE_RD_REQ:
      LOG_VRB(("TFTP server: Data Wr, WRQ server busy, RRQ  opcode?"));
      TFTPs_TxErr(0, (CPU_CHAR *)"WRQ server busy, RRQ  opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;

    case TFTP_OPCODE_ACK:
      LOG_VRB(("TFTP server: Data Wr, WRQ server busy, ACK  opcode?"));
      TFTPs_TxErr(0, (CPU_CHAR *)"WRQ server busy, ACK  opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;

    case TFTP_OPCODE_WR_REQ:
      TFTPs_FS_API_Ptr->Close(TFTPs_DataPtr->FileHandle);
      TFTPs_DataPtr->FileHandle = DEF_NULL;
      TFTPs_DataPtr->TxBlkNbr = 0;
      //                                                           Open the desired file for writing.
      TFTPs_FileOpen(TFTPs_FILE_OPEN_WR, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        LOG_VRB(("TFTP server: Data Wr, Rx'd WR_REQ again"));
        TFTPs_DataPtr->State = TFTPs_STATE_DATA_WR;
        TFTPs_DataWrAck(TFTPs_DataPtr->TxBlkNbr);               // Acknowledge the client.
      }
      break;

    case TFTP_OPCODE_DATA:
      LOG_VRB(("TFTP server: Data Wr, Rx'd DATA --- OK"));
      TFTPs_DataWr(p_err);                                      // Write data to file.
      break;

    case TFTP_OPCODE_ERR:
      LOG_VRB(("TFTP server: Data Wr, WRQ server busy, ERR  opcode?"));
      TFTPs_TxErr(0, (CPU_CHAR *)"WRQ server busy, ERR  opcode?");
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }
}

/****************************************************************************************************//**
 *                                           TFTPs_GetRxBlkNbr()
 *
 * @brief    Extract the block number from the received TFTP command packet.
 *******************************************************************************************************/
static void TFTPs_GetRxBlkNbr(void)
{
  CPU_INT16U *p_blk_nbr;

  p_blk_nbr = (CPU_INT16U *)&TFTPs_DataPtr->RxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
  TFTPs_DataPtr->RxBlkNbr = NET_UTIL_NET_TO_HOST_16(*p_blk_nbr);
}

/****************************************************************************************************//**
 *                                               TFTPs_Terminate()
 *
 * @brief    Terminate the current file transfer process.
 *******************************************************************************************************/
static void TFTPs_Terminate(void)
{
  RTOS_ERR local_err;

  TFTPs_DataPtr->State = TFTPs_STATE_IDLE;                      // Abort current file transfer.
  if (TFTPs_DataPtr->FileHandle != DEF_NULL) {
    TFTPs_FS_API_Ptr->Close(TFTPs_DataPtr->FileHandle);         // Close the current opened file.
    TFTPs_DataPtr->FileHandle = DEF_NULL;
  }

  //                                                               Reset blocking timeout to infinite.
  NetSock_CfgTimeoutRxQ_Set(TFTPs_DataPtr->SockID,
                            NET_TMR_TIME_INFINITE,
                            &local_err);
}

/****************************************************************************************************//**
 *                                               TFTPs_FileOpen()
 *
 * @brief    Get filename and file mode from the TFTP packet and attempt to open that file.
 *
 * @param    rw      File access :
 *                   TFTPs_FILE_OPEN_RD      Open for reading
 *                   TFTPs_FILE_OPEN_WR      Open for writing
 *
 * @param    p_err   Error pointer.
 *
 *                   @note         (1) This function also extracts options as specified in RFC #2349 :
 *
 *                   "timeout"     specifies the timeout in seconds to wait in case we don't receive
 *                   data request after we initiated a read request.
 *
 *                   "tsize"       specifies the size of the file in bytes that the client is writing.
 *
 *                   Note that both these options may not be supported by the client and thus, we assume
 *                   default values if they are not specified.
 *******************************************************************************************************/
static void TFTPs_FileOpen(CPU_BOOLEAN rw,
                           RTOS_ERR    *p_err)
{
  CPU_CHAR *p_filename;
  CPU_CHAR *p_mode;
  CPU_CHAR *p_name;
#if 0
  CPU_CHAR *p_value;
#endif

  //                                                               ---- GET FILENAME ----------------------------------
  p_filename = (CPU_CHAR *)&TFTPs_DataPtr->RxMsgBuf[TFTP_PKT_OFFSET_FILENAME];
  //                                                               ---- GET FILE MODE ---------------------------------
  p_mode = p_filename;                                          // Point to the 'Mode' string.
  while (*p_mode > (CPU_CHAR)0) {
    p_mode++;
  }
  p_mode++;
  //                                                               ---- GET RFC2349 "timeout" OPTION (IF AVAILABLE) ---
  p_name = p_mode;                                              // Skip over the 'Mode' string.
  while (*p_name > (CPU_CHAR)0) {
    p_name++;
  }
  p_name++;

#if 0                                                           // See Note #1.
                                                                // See if the client specified a "timeout" string
  if (Str_Cmp(p_name, (CPU_CHAR *)"timeout") == 0) {            // (RFC2349).
    p_value = p_name;                                           // Yes, skip over the "timeout" string.
    while (*p_value > (CPU_CHAR)0) {
      p_value++;
    }
    p_value++;
    //                                                             Get the timeout (in seconds).
    TFTPs_DataPtr->Timeout_s = Str_ParseNbr_Int32U(p_value, DEF_NULL, 0);
  } else {
    TFTPs_DataPtr->Timeout_s = TFTPs_TIMEOUT_SEC_DFLT;          // No,  assume the default timeout (in seconds).

    //                                                             ---- GET RFC2349 "tsize" OPTION (IF AVAILABLE) -----
    //                                                             See if the client specified a "tsize" string
    //                                                             (RFC2349).
    if (Str_Cmp(p_name, (CPU_CHAR *)"tsize") == 0) {
      p_value = p_name;
      while (*p_value > (CPU_CHAR)0) {                          // Yes, skip over the "tsize" string.
        p_value++;
      }
      p_value++;
      //                                                           Get the size of the file to write.
      TFTPs_DataPtr->WrSize = Str_ParseNbr_Int32U(p_value, DEF_NULL, 0);
    } else {
      TFTPs_DataPtr->WrSize = 0;                                // Assume a default value of 0.
    }
  }
#endif
  //                                                               ---- OPEN THE FILE ---------------------------------
  TFTPs_DataPtr->FileHandle = TFTPs_FileOpenMode(p_filename, rw);

  if (TFTPs_DataPtr->FileHandle == DEF_NULL) {
    TFTPs_TxErr(0, (CPU_CHAR *)"file not found");
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  }
}

/****************************************************************************************************//**
 *                                           TFTPs_FileOpenMode()
 *
 * @brief    Open the specified file.
 *
 * @param    p_filename  File name to open.
 *
 * @param    rw          File access :
 *                       TFTPs_FILE_OPEN_RD      Open for reading
 *                       TFTPs_FILE_OPEN_WR      Open for writing
 *
 * @return   Pointer to a file handle for the opened file, if NO error.
 *           Pointer to NULL,                              otherwise.
 *******************************************************************************************************/
static void *TFTPs_FileOpenMode(CPU_CHAR    *p_filename,
                                CPU_BOOLEAN rw)
{
  void *p_file;

  p_file = DEF_NULL;
  switch (rw) {
    case TFTPs_FILE_OPEN_RD:
      p_file = TFTPs_FS_API_Ptr->Open(p_filename,
                                      NET_FS_FILE_MODE_OPEN,
                                      NET_FS_FILE_ACCESS_RD);
      break;

    case TFTPs_FILE_OPEN_WR:
      p_file = TFTPs_FS_API_Ptr->Open(p_filename,
                                      NET_FS_FILE_MODE_CREATE,
                                      NET_FS_FILE_ACCESS_WR);
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

  return (p_file);
}

/****************************************************************************************************//**
 *                                               TFTPs_DataRd()
 *
 * @brief    Read data from the opened file and send it to the client.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_DataRd(RTOS_ERR *p_err)
{
  NET_SOCK_RTN_CODE tx_size;
  CPU_BOOLEAN       ok;

  if (TFTPs_DataPtr->FileHandle == DEF_NULL) {
    return;
  }
  //                                                               Read data from file.
  ok = TFTPs_FS_API_Ptr->Rd(TFTPs_DataPtr->FileHandle,
                            &TFTPs_DataPtr->TxMsgBuf[TFTP_PKT_OFFSET_DATA],
                            TFTPs_BLOCK_SIZE,
                            &TFTPs_DataPtr->TxMsgLen);

  if (TFTPs_DataPtr->TxMsgLen < TFTPs_BLOCK_SIZE) {             // Close file when all data read.
    TFTPs_FS_API_Ptr->Close(TFTPs_DataPtr->FileHandle);
    TFTPs_DataPtr->FileHandle = DEF_NULL;
  }

  if (ok == DEF_FAIL) {                                         // If read err, ...
    TFTPs_TxErr(0, (CPU_CHAR *)"RRQ file read error");          // ... tx  err pkt.
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  TFTPs_DataPtr->TxMsgCtr++;
  TFTPs_DataPtr->TxBlkNbr++;

  TFTPs_DataPtr->TxMsgLen += TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;

  tx_size = TFTPs_Tx(TFTP_OPCODE_DATA,
                     TFTPs_DataPtr->TxBlkNbr,
                     &TFTPs_DataPtr->TxMsgBuf[0],
                     TFTPs_DataPtr->TxMsgLen);

  if (tx_size < 0) {                                            // If tx  err, ...
    TFTPs_TxErr(0, (CPU_CHAR *)"RRQ file read error");          // ... tx err pkt.
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
  }
}

/****************************************************************************************************//**
 *                                               TFTPs_DataWr()
 *
 * @brief    Write data to the opened file.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void TFTPs_DataWr(RTOS_ERR *p_err)
{
  CPU_INT16U blk_nbr;
  CPU_INT16S data_bytes;
  CPU_SIZE_T data_bytes_wr;
  CPU_INT16U *p_blk_nbr;

  PP_UNUSED_PARAM(p_err);

  //                                                               Get block nbr.
  p_blk_nbr = (CPU_INT16U *)&TFTPs_DataPtr->RxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
  blk_nbr = NET_UTIL_NET_TO_HOST_16(*p_blk_nbr);

  if (blk_nbr > TFTPs_DataPtr->TxBlkNbr) {                      // If block nbr > last block nbr, ...
    data_bytes = TFTPs_DataPtr->RxMsgLen - TFTP_PKT_SIZE_OPCODE - TFTP_PKT_SIZE_BLK_NBR;

    if (data_bytes > 0) {                                       // ... wr data to file.
      (void)TFTPs_FS_API_Ptr->Wr(TFTPs_DataPtr->FileHandle,
                                 &TFTPs_DataPtr->RxMsgBuf[TFTP_PKT_OFFSET_DATA],
                                 data_bytes,
                                 &data_bytes_wr);
      PP_UNUSED_PARAM(data_bytes_wr);
    }

    if (data_bytes < TFTPs_BLOCK_SIZE) {                        // If last block of transmission, ...
      TFTPs_FS_API_Ptr->Close(TFTPs_DataPtr->FileHandle);       // ... close file.
      TFTPs_DataPtr->FileHandle = DEF_NULL;
      TFTPs_DataPtr->State = TFTPs_STATE_IDLE;
    }
  }

  TFTPs_DataWrAck(blk_nbr);

  TFTPs_DataPtr->TxBlkNbr = blk_nbr;
}

/****************************************************************************************************//**
 *                                               TFTPs_DataWrAck()
 *
 * @brief    Send an acknowledgement to the client.
 *
 * @param    blk_nbr     Block number to acknowledge.
 *******************************************************************************************************/
static void TFTPs_DataWrAck(CPU_INT32U blk_nbr)
{
  CPU_INT16U tx_len;

  tx_len = TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;
  TFTPs_DataPtr->TxMsgCtr++;

  TFTPs_Tx(TFTP_OPCODE_ACK,
           (CPU_INT16U)blk_nbr,
           &TFTPs_DataPtr->TxMsgBuf[0],
           tx_len);
}

/****************************************************************************************************//**
 *                                               TFTPs_TxErr()
 *
 * @brief    Send error message to the client.
 *
 * @param    err_code    TFTP error code indicating the nature of the error.
 *
 * @param    p_err_msg   NULL terminated string indicating the nature of the error.
 *******************************************************************************************************/
static void TFTPs_TxErr(CPU_INT16U err_code,
                        CPU_CHAR   *p_err_msg)
{
  CPU_INT16S tx_len;

  tx_len = Str_Len(p_err_msg) + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR + 1;

  Str_Copy((CPU_CHAR *)&TFTPs_DataPtr->TxMsgBuf[TFTP_PKT_OFFSET_DATA], p_err_msg);

  TFTPs_Tx(TFTP_OPCODE_ERR,
           err_code,
           &TFTPs_DataPtr->TxMsgBuf[0],
           tx_len);
}

/****************************************************************************************************//**
 *                                               TFTPs_Tx()
 *
 * @brief    Send TFTP packet.
 *
 * @param    opcode      TFTP packet operation code.
 *
 * @param    blk_nbr     Block number (or error code) for packet to transmit.
 *
 * @param    p_buf       Pointer to                       packet to transmit.
 *
 * @param    tx_len      Length of the                    packet to transmit (in octets).
 *
 * @return   Number of positive data octets transmitted, if NO errors.
 *           NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed.
 *           NET_SOCK_BSD_ERR_TX,                        otherwise.
 *******************************************************************************************************/
static NET_SOCK_RTN_CODE TFTPs_Tx(CPU_INT16U opcode,
                                  CPU_INT16U blk_nbr,
                                  CPU_INT08U *p_buf,
                                  CPU_INT16U tx_len)
{
  CPU_INT16U        *p_buf16;
  NET_SOCK_RTN_CODE bytes_sent;
  RTOS_ERR          local_err;

  p_buf16 = (CPU_INT16U *)&TFTPs_DataPtr->TxMsgBuf[TFTP_PKT_OFFSET_OPCODE];
  *p_buf16 = NET_UTIL_NET_TO_HOST_16(opcode);

  p_buf16 = (CPU_INT16U *)&TFTPs_DataPtr->TxMsgBuf[TFTP_PKT_OFFSET_BLK_NBR];
  *p_buf16 = NET_UTIL_NET_TO_HOST_16(blk_nbr);

  bytes_sent = NetSock_TxDataTo(TFTPs_DataPtr->SockID,
                                p_buf,
                                tx_len,
                                NET_SOCK_FLAG_NONE,
                                &TFTPs_DataPtr->SockAddr,
                                NET_SOCK_ADDR_SIZE,
                                &local_err);

  return (bytes_sent);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_TFTP_SERVER_AVAIL
