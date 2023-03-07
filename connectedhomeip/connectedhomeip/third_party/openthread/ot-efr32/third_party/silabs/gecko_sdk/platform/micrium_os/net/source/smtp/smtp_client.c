/***************************************************************************//**
 * @file
 * @brief Network - SMTP Client
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
 * @note     (1) This code implements a subset of the SMTP protocol (RFC 2821).  More precisely, the
 *               following commands have been implemented:
 *                 - HELO
 *                 - AUTH (if enabled)
 *                 - MAIL
 *                 - RCPT
 *                 - DATA
 *                 - RSET
 *                 - NOOP
 *                 - QUIT
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_SMTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error SMTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/smtp_client.h>

#include  <net/include/net.h>
#include  <net/include/net_app.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_conn.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/include/smtp_client.h>

#include  <net/source/util/net_base64_priv.h>
#include  <net/source/cmd/smtp_client_cmd_priv.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>
#include  <common/include/lib_str.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH               (NET, SMTP)
#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_NET

#define  SMTPc_INIT_CFG_DFLT            { \
    .MemSegPtr = DEF_NULL,                \
    .AuthBufInputLen = 100,               \
    .AuthBufOutputLen = 140               \
}                                                                       // Max display name = 50; Max subject len = 50

#define  SMTPc_DFLT_CFG_TIMEOUT_CONN_MS      5000
#define  SMTPc_DFLT_CFG_TIMEOUT_RX_MS        5000
#define  SMTPc_DFLT_CFG_TIMEOUT_TX_MS        5000
#define  SMTPc_DFLT_CFG_TIMEOUT_CLOSE_MS     5000

/********************************************************************************************************
 *                                           SMTPc STRUCT DEFINES
 *
 * Note(s) : (1) Line length limit, as defined in RFC #2822.
 *
 *           (2) SMTPc_CFG_COMM_BUF_LEN is the length of the buffer used to receive replies from the SMTP
 *               server.  As stated in RFC #2821, Section 'The SMTP Specifications, Additional Implementation
 *               Issues, Sizes and Timeouts, Size limits and minimums', "The maximum total length of a
 *               reply code and the <CRLF> is 512 characters".
 *
 *               This buffer is also used to build outgoing messages and MUST NOT be smaller than 1000
 *               (see Note #1).
 *
 *           (3) Maximum length of key-value strings in structure SMTPc_KEY_VAL.
 *
 *           (4) As mentioned in RFC #2821, Section 'The SMTP Specifications, Additional
 *               Implementation Issues, Sizes and Timeouts, Size limits and minimums', "The maximum
 *               total length of a user name or other local-part is 64 characters [and] the maximum
 *               total length of a domain name or number is 255 characters.".  Adding 2 for
 *               '@' (see SMTPc_MBOX structure) and '\0'.
 *
 *           (5) Maximum length of content-type.
 *
 *           (6) From RFC #2822, Section 'Syntax, Fields definitions, Identification fields', "The message
 *               identifier (msg-id) [field] is similar in syntax to an angle-addr construct".
 *
 *           (7) Size of ParamArray in structure SMTPc_MIME_ENTITY_HDR.
 *
 *           (8) Maximum length of attachment's name and description.
 *******************************************************************************************************/

#define  SMTPc_LINE_LEN_LIM                             1000    // See Note #1.

#define  SMTPc_COMM_BUF_LEN                             1024    // See Note #2.

//                                                                 See Note #3.
#define  SMTPc_KEY_VAL_KEY_LEN                            30
#define  SMTPc_KEY_VAL_VAL_LEN                            30

//                                                                 See Note #4.
#define  SMTPc_MBOX_DOMAIN_NAME_LEN                      255
#define  SMTPc_MBOX_LOCAL_PART_LEN                        64
#define  SMTPc_MBOX_ADDR_LEN                   (SMTPc_MBOX_DOMAIN_NAME_LEN + SMTPc_MBOX_LOCAL_PART_LEN + 2)

//                                                                 See Note #5.
#define  SMTPc_MIME_CONTENT_TYPE_LEN                      20

//                                                                 See Note #6.
#define  SMTPc_MIME_ID_LEN                      SMTPc_MBOX_ADDR_LEN

//                                                                 See Note #7.
#define  SMTPc_MIME_MAX_KEYVAL                             1

//                                                                 See Note #8.
#define  SMTPc_ATTACH_NAME_LEN                            50
#define  SMTPc_ATTACH_DESC_LEN                            50

//                                                                 See Note #6.
#define  SMTPc_MSG_MSGID_LEN                    SMTPc_MBOX_ADDR_LEN

/********************************************************************************************************
 *                                           STATUS CODES DEFINES
 *
 * Note(s): (1) Reply codes are defined here and classified by category.  Note that this list is not meant
 *               to be exhaustive.
 *******************************************************************************************************/

//                                                                 ------------ POSITIVE PRELIMINARY REPLY ------------
#define  SMTPc_REP_POS_PRELIM_GRP                          1

//                                                                 ------------ POSITIVE COMPLETION REPLY -------------
#define  SMTPc_REP_POS_COMPLET_GRP                         2
#define  SMTPc_REP_220                                   220    // Service ready.
#define  SMTPc_REP_221                                   221    // Service closing transmission channel.
#define  SMTPc_REP_235                                   235    // Authentication successful.
#define  SMTPc_REP_250                                   250    // Requested mail action okay, completed.
#define  SMTPc_REP_251                                   251    // User not local; will forward to <forward-path>.

//                                                                 ------------ POSITIVE INTERMEDIATE REPLY -----------
#define  SMTPc_REP_POS_INTER_GRP                           3
#define  SMTPc_REP_354                                   354    // Start mail input; end with <CRLF>.<CRLF>.

//                                                                 -------- TRANSIENT NEGATIVE COMPLETION REPLY -------
#define  SMTPc_REP_NEG_TRANS_COMPLET_GRP                   4
#define  SMTPc_REP_421                                   421    // Service closing transmission channel.

//                                                                 -------- PERMANENT NEGATIVE COMPLETION REPLY -------
#define  SMTPc_REP_NEG_COMPLET_GRP                         5
#define  SMTPc_REP_503                                   503    // Bad sequence of commands.
#define  SMTPc_REP_504                                   504    // Command parameter not implemented.
#define  SMTPc_REP_535                                   535    // Authentication failure.
#define  SMTPc_REP_550                                   550    // Requested action not taken: mailbox unavailable.
#define  SMTPc_REP_554                                   554    // Transaction failed.

/********************************************************************************************************
 *                                       COMMANDS AND STRINGS DEFINES
 *******************************************************************************************************/

#define  SMTPc_CMD_HELO                         "HELO"
#define  SMTPc_CMD_MAIL                         "MAIL"
#define  SMTPc_CMD_RCPT                         "RCPT"
#define  SMTPc_CMD_DATA                         "DATA"
#define  SMTPc_CMD_RSET                         "RSET"
#define  SMTPc_CMD_NOOP                         "NOOP"
#define  SMTPc_CMD_QUIT                         "QUIT"

#define  SMTPc_CMD_AUTH                         "AUTH"
#define  SMTPc_CMD_AUTH_MECHANISM_PLAIN         "PLAIN"

#define  SMTPc_CRLF                             "\x0D\x0A"
#define  SMTPc_EOM                              "\x0D\x0A.\x0D\x0A"
#define  SMTPc_CRLF_SIZE                         2u

#define  SMTPc_HDR_FROM                         "From: "
#define  SMTPc_HDR_SENDER                       "Sender: "
#define  SMTPc_HDR_TO                           "To: "
#define  SMTPc_HDR_REPLYTO                      "Reply-to: "
#define  SMTPc_HDR_CC                           "Cc: "
#define  SMTPc_HDR_SUBJECT                      "Subject: "

#define  SMTPc_TAG_IPv6                         "IPv6:"

/********************************************************************************************************
 *                                       BASE 64 ENCODER DEFINES
 *******************************************************************************************************/

#define  SMTPc_ENCODER_BASE64_DELIMITER_CHAR               0

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 *
 * Note(s): (1) From RFC #2821 'The SMTP Model, Terminology, Mail Objects', "SMTP transports a mail
 *               object.  A mail object contains an envelope and content.  The SMTP content is sent
 *               in the SMTP DATA protocol unit and has two parts: the headers and the body":
 *
 *                       |----------------------|
 *                       |                      |
 *                       |                      |  Envelope
 *                       |                      |
 *                       |======================|
 *                       |                      |
 *                       |  Headers             |
 *                       |                      |
 *                       |                      |
 *                       |                      |
 *                       |----------------------|  Content
 *                       |                      |
 *                       |  Body                |
 *                       |                      |
 *                       |                      |
 *                       |                      |
 *                       |----------------------|
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           KEY-VALUE STRUCT DATA TYPES
 *
 * Note(s): (1) This structure makes room for additional MIME header fields (see RFC #2045, Section
 *               'Additional MIME Header Fields').
 *******************************************************************************************************/

typedef  struct  SMTPc_key_val {
  CPU_CHAR Key[SMTPc_KEY_VAL_KEY_LEN];                          // Key (hdr field name).
  CPU_CHAR Val[SMTPc_KEY_VAL_VAL_LEN];                          // Val associated with the preceding key.
} SMTPc_KEY_VAL;

/********************************************************************************************************
 *                                           SMTP MIME ENTITY HEADER
 *
 * Note(s): (1) See RFC #2045 for details.
 *
 *           (2) Structure subject to change.  For instance, other data structures could be used to represent
 *               "Encoding", etc.  The encoding could also be left to do by the application
 *******************************************************************************************************/

typedef  struct  SMTPc_mime_entity_hdr {
  CPU_CHAR      *ContentType[SMTPc_MIME_CONTENT_TYPE_LEN];      // Description of contained body data (IANA assigned).
  SMTPc_KEY_VAL *ParamArray[SMTPc_MIME_MAX_KEYVAL];             // Additional param for specified content-type.
  void          *ContentEncoding;                               // Content transfer encoding.
  void (*EncodingFnctPtr)(CPU_CHAR *,                           // Ptr to fnct performing the encoding of the
                          RTOS_ERR *);                          // attachment.
  CPU_CHAR      ID[SMTPc_MIME_ID_LEN];                          // Unique attachment id.
} SMTPc_MIME_ENTITY_HDR;

typedef  struct  smtpc_addr {
  CPU_CHAR     *AddrPtr;
  SLIST_MEMBER Node;
} SMTPc_ADDR;

typedef  struct  smtpc_cfg {
  CPU_INT16U TimeoutConn_ms;
  CPU_INT16U TimeoutTx_ms;
  CPU_INT16U TimeoutRx_ms;
  CPU_INT16U TimeoutClose_ms;
} SMTPc_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_CHAR SMTPc_Comm_Buf[SMTPc_COMM_BUF_LEN];

static CPU_BOOLEAN SMTPc_InitActive = DEF_ACTIVE;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SMTPc_INIT_CFG  SMTPc_InitCfgDflt = SMTPc_INIT_CFG_DFLT;
static SMTPc_INIT_CFG SMTPc_InitCfg = SMTPc_INIT_CFG_DFLT;
#else
extern const SMTPc_INIT_CFG SMTPc_InitCfg;
#endif

static SMTPc_CFG *SMTPc_CfgPtr = DEF_NULL;

static MEM_DYN_POOL *SMTPc_AddrPoolPtr = DEF_NULL;
static MEM_DYN_POOL *SMTPc_MsgPoolPtr = DEF_NULL;
static MEM_DYN_POOL *SMTPc_AuthBufInputPoolPtr = DEF_NULL;
static MEM_DYN_POOL *SMTPc_AuthBufOutputPoolPtr = DEF_NULL;

static KAL_LOCK_HANDLE SMTPc_Lock;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------------- RX FNCT'S --------------------
static CPU_CHAR *SMTPc_RxReply(NET_SOCK_ID sock_id,
                               RTOS_ERR    *p_err);

static void SMTPc_ParseReply(CPU_CHAR   *p_server_reply,
                             CPU_INT32U *p_completion_code,
                             RTOS_ERR   *p_err);

//                                                                 --------------------- TX FNCT'S --------------------
static void SMTPc_SendBody(NET_SOCK_ID sock_id,
                           SMTPc_MSG   *msg,
                           RTOS_ERR    *p_err);

static void SMTPc_QueryServer(NET_SOCK_ID sock_id,
                              CPU_CHAR    *query,
                              CPU_INT32U  len,
                              RTOS_ERR    *p_err);

//                                                                 ------------------- UTIL FNCT'S -------------------
static CPU_INT32U SMTPc_BuildHdr(NET_SOCK_ID sock_id,
                                 CPU_CHAR    *buf,
                                 CPU_INT32U  buf_size,
                                 CPU_INT32U  buf_wr_ix,
                                 CPU_CHAR    *hdr,
                                 CPU_CHAR    *val,
                                 CPU_INT32U  *line_len,
                                 RTOS_ERR    *p_err);

//                                                                 -------------------- CMD FNCT'S -------------------
static CPU_CHAR *SMTPc_HELO(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_AUTH(NET_SOCK_ID sock_id,
                            CPU_CHAR    *username,
                            CPU_CHAR    *pw,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_MAIL(NET_SOCK_ID sock_id,
                            CPU_CHAR    *from,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_RCPT(NET_SOCK_ID sock_id,
                            CPU_CHAR    *to,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_DATA(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_RSET(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_CHAR *SMTPc_QUIT(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err);

static CPU_INT08U SMTPc_GetCodeGroup(CPU_INT32U completion_code,
                                     RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SMTPc_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           SMTP client module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before TELNETs_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SMTPc_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive == DEF_ACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  SMTPc_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                       SMTPc_ConfigureAuthBufLen()
 *
 * @brief    Configure the size of internal input buffer (See Notes).
 *
 * @param    buf_len     Input buffer length to configure.
 *
 * @note     (1) This function is optional. If it is called, it must be called before SMTPc_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *
 * @note     (2) The maximum input buffer passed to the base 64 encoder depends of the configured maximum
 *               lengths for the username and password. Two additional characters are added to these
 *               values to account for the delimiter.
 *
 * @note     (3) The size of the output buffer the base 64 encoder produces is typically bigger than the
 *               input buffer by a factor of (4 x 3). However, when padding is necessary, up to 3
 *               additional characters could by appended. Finally, one more character is used to NULL
 *               terminate the buffer.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SMTPc_ConfigureAuthBufLen(CPU_INT16U buf_len)
{
  CPU_INT16U buf_large_len;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive == DEF_ACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  buf_large_len = ((((buf_len + 2) * 4) / 3) + 4);
  SMTPc_InitCfg.AuthBufInputLen = buf_len;
  SMTPc_InitCfg.AuthBufOutputLen = buf_large_len;
}
#endif

/****************************************************************************************************//**
 *                                               SMTPc_Init()
 *
 * @brief    Initialize the SMTP client module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function.
 *******************************************************************************************************/
void SMTPc_Init(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive == DEF_ACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SMTPc_Lock = KAL_LockCreate("SMTPc Lock", DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_CfgPtr = (SMTPc_CFG *)Mem_SegAlloc("SMTPc cfg struct",
                                           SMTPc_InitCfg.MemSegPtr,
                                           sizeof(SMTPc_CFG),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_CfgPtr->TimeoutConn_ms = SMTPc_DFLT_CFG_TIMEOUT_CONN_MS;
  SMTPc_CfgPtr->TimeoutTx_ms = SMTPc_DFLT_CFG_TIMEOUT_TX_MS;
  SMTPc_CfgPtr->TimeoutRx_ms = SMTPc_DFLT_CFG_TIMEOUT_RX_MS;
  SMTPc_CfgPtr->TimeoutClose_ms = SMTPc_DFLT_CFG_TIMEOUT_CLOSE_MS;

  SMTPc_AddrPoolPtr = (MEM_DYN_POOL *)Mem_SegAlloc("SMTPc Addresses Pool Object",
                                                   SMTPc_InitCfg.MemSegPtr,
                                                   sizeof(MEM_DYN_POOL),
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(SMTPc_AddrPoolPtr, sizeof(MEM_DYN_POOL));

  Mem_DynPoolCreate("SMTPc Address Pool",
                    SMTPc_AddrPoolPtr,
                    SMTPc_InitCfg.MemSegPtr,
                    sizeof(SMTPc_ADDR),
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_MsgPoolPtr = (MEM_DYN_POOL *)Mem_SegAlloc("SMTPc Msg Pool Object",
                                                  SMTPc_InitCfg.MemSegPtr,
                                                  sizeof(MEM_DYN_POOL),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(SMTPc_MsgPoolPtr, sizeof(MEM_DYN_POOL));

  Mem_DynPoolCreate("SMTPc Message Pool",
                    SMTPc_MsgPoolPtr,
                    SMTPc_InitCfg.MemSegPtr,
                    sizeof(SMTPc_MSG),
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_AuthBufInputPoolPtr = (MEM_DYN_POOL *)Mem_SegAlloc("SMTPc Msg Pool Object",
                                                           SMTPc_InitCfg.MemSegPtr,
                                                           sizeof(MEM_DYN_POOL),
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(SMTPc_AuthBufInputPoolPtr, sizeof(MEM_DYN_POOL));

  Mem_DynPoolCreate("SMTPc Message Pool",
                    SMTPc_AuthBufInputPoolPtr,
                    SMTPc_InitCfg.MemSegPtr,
                    SMTPc_InitCfg.AuthBufInputLen,
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_AuthBufOutputPoolPtr = (MEM_DYN_POOL *)Mem_SegAlloc("SMTPc Msg Pool Object",
                                                            SMTPc_InitCfg.MemSegPtr,
                                                            sizeof(MEM_DYN_POOL),
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(SMTPc_AuthBufOutputPoolPtr, sizeof(MEM_DYN_POOL));

  Mem_DynPoolCreate("SMTPc Message Pool",
                    SMTPc_AuthBufOutputPoolPtr,
                    SMTPc_InitCfg.MemSegPtr,
                    SMTPc_InitCfg.AuthBufOutputLen,
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  SMTPcCmd_Init(p_err);
#endif

  CORE_ENTER_ATOMIC();
  SMTPc_InitActive = DEF_INACTIVE;
  CORE_EXIT_ATOMIC();

exit:
  return;
}

/****************************************************************************************************//**
 *                                           SMTPc_DfltCfgSet()
 *
 * @brief    Initialize the SMTP default configuration.
 *
 * @param    timeout_conn_ms     timeout on connection request.
 *
 * @param    timeout_rx_ms       timeout on receive.
 *
 * @param    timeout_tx_ms       timeout on transmit.
 *
 * @param    timeout_close_ms    timeout on socket close
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function.
 *
 * @note     (1) The new configuration value are applied only on new connection.
 *******************************************************************************************************/
void SMTPc_DfltCfgSet(CPU_INT16U timeout_conn_ms,
                      CPU_INT16U timeout_rx_ms,
                      CPU_INT16U timeout_tx_ms,
                      CPU_INT16U timeout_close_ms,
                      RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_CfgPtr->TimeoutConn_ms = timeout_conn_ms;
  SMTPc_CfgPtr->TimeoutTx_ms = timeout_tx_ms;
  SMTPc_CfgPtr->TimeoutRx_ms = timeout_rx_ms;
  SMTPc_CfgPtr->TimeoutClose_ms = timeout_close_ms;

  KAL_LockRelease(SMTPc_Lock, p_err);
exit:
  return;
}

/****************************************************************************************************//**
 *                                               SMTPc_MsgAlloc()
 *
 * @brief    Get a message structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function.
 *
 * @return   Pointer to an allocated message structure.
 *
 * @note     (1) If the message structure is allocated from the application. The function SMTPc_MsgClr()
 *           must be called to free internal structure allocated during message processing.
 *******************************************************************************************************/
SMTPc_MSG *SMTPc_MsgAlloc(RTOS_ERR *p_err)
{
  SMTPc_MSG *p_msg = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT, DEF_NULL);
  CORE_EXIT_ATOMIC();

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_msg = (SMTPc_MSG *)Mem_DynPoolBlkGet(SMTPc_MsgPoolPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  KAL_LockRelease(SMTPc_Lock, p_err);

exit:
  return (p_msg);
}

/****************************************************************************************************//**
 *                                               SMTPc_MsgFree()
 *
 * @brief    Free a message structure.
 *
 * @param    p_msg   Pointer to the message structure to free.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function.
 *******************************************************************************************************/
void SMTPc_MsgFree(SMTPc_MSG *p_msg,
                   RTOS_ERR  *p_err)
{
  CORE_DECLARE_IRQ_STATE;
  RTOS_ERR err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_MsgClr(p_msg, &err);
  Mem_DynPoolBlkFree(SMTPc_MsgPoolPtr, p_msg, &err);

  PP_UNUSED_PARAM(err);

  KAL_LockRelease(SMTPc_Lock, p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               SMTPc_MsgClr()
 *
 * @brief    Clear a message structure to be reused for another message.
 *
 * @param    p_msg   Pointer to the message structure to clear.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function.
 *
 * @note     (1) If the message structure is allocated from the application. The function SMTPc_MsgClr()
 *               must be called to free internal structure allocated during message processing.
 *******************************************************************************************************/
void SMTPc_MsgClr(SMTPc_MSG *p_msg,
                  RTOS_ERR  *p_err)
{
  SMTPc_ADDR *p_addr;
  CORE_DECLARE_IRQ_STATE;
  RTOS_ERR err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_msg->To_List, p_addr, SMTPc_ADDR, Node) {
    Mem_DynPoolBlkFree(SMTPc_AddrPoolPtr, p_addr, &err);
  }

  SLIST_FOR_EACH_ENTRY(p_msg->CC_List, p_addr, SMTPc_ADDR, Node) {
    Mem_DynPoolBlkFree(SMTPc_AddrPoolPtr, p_addr, &err);
  }

  SLIST_FOR_EACH_ENTRY(p_msg->BCC_List, p_addr, SMTPc_ADDR, Node) {
    Mem_DynPoolBlkFree(SMTPc_AddrPoolPtr, p_addr, &err);
  }

  p_msg->To_List = DEF_NULL;
  p_msg->CC_List = DEF_NULL;
  p_msg->BCC_List = DEF_NULL;
  p_msg->FromAddr = DEF_NULL;
  p_msg->FromName = DEF_NULL;
  p_msg->ReplyTo = DEF_NULL;
  p_msg->Sender = DEF_NULL;

  PP_UNUSED_PARAM(err);

  KAL_LockRelease(SMTPc_Lock, p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           SMTPc_MsgSetParam()
 *
 * @brief    Configure message parameter.
 *
 * @param    p_msg       Pointer to the message structure to configure.
 *
 * @param    param_type  Parameter to set in the message:
 *                           - SMTPc_FROM_ADDR
 *                           - SMTPc_FROM_DISPL_NAME
 *                           - SMTPc_TO_ADDR
 *                           - SMTPc_CC_ADDR
 *                           - SMTPc_BCC_ADDR
 *                           - SMTPc_REPLY_TO_ADDR
 *                           - SMTPc_SENDER_ADDR
 *                           - SMTPc_ATTACHMENT
 *                           - SMTPc_MSG_SUBJECT
 *                           - SMTPc_MSG_BODY
 *
 * @param    p_val       Pointer to the parameter value.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function.
 *******************************************************************************************************/
void SMTPc_MsgSetParam(SMTPc_MSG       *p_msg,
                       SMTPc_MSG_PARAM param_type,
                       void            *p_val,
                       RTOS_ERR        *p_err)
{
  SMTPc_ADDR *p_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  switch (param_type) {
    case SMTPc_FROM_ADDR:
      p_msg->FromAddr = (CPU_CHAR *)p_val;
      break;

    case SMTPc_FROM_DISPL_NAME:
      p_msg->FromName = (CPU_CHAR *)p_val;
      break;

    case SMTPc_TO_ADDR:
      p_addr = (SMTPc_ADDR *)Mem_DynPoolBlkGet(SMTPc_AddrPoolPtr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_addr->AddrPtr = (CPU_CHAR *)p_val;
      SList_PushBack(&p_msg->To_List, &p_addr->Node);
      break;

    case SMTPc_CC_ADDR:
      p_addr = (SMTPc_ADDR *)Mem_DynPoolBlkGet(SMTPc_AddrPoolPtr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_addr->AddrPtr = (CPU_CHAR *)p_val;
      SList_PushBack(&p_msg->CC_List, &p_addr->Node);
      break;

    case SMTPc_BCC_ADDR:
      p_addr = (SMTPc_ADDR *)Mem_DynPoolBlkGet(SMTPc_AddrPoolPtr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_addr->AddrPtr = (CPU_CHAR *)p_val;
      SList_PushBack(&p_msg->BCC_List, &p_addr->Node);
      break;

    case SMTPc_REPLY_TO_ADDR:
      p_msg->ReplyTo = (CPU_CHAR *)p_val;
      break;

    case SMTPc_SENDER_ADDR:
      p_msg->Sender = (CPU_CHAR *)p_val;
      break;

    case SMTPc_MSG_SUBJECT:
      p_msg->MsgSubject = (CPU_CHAR *)p_val;
      break;

    case SMTPc_MSG_BODY:
      p_msg->MsgBody = (CPU_CHAR *)p_val;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      break;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               SMTPc_SendMail()
 *
 * @brief    Sends an email.
 *
 * @param    p_host_name     Pointer to host name of the SMTP server to contact. Can be also an
 *                           IP address.
 *
 * @param    port            TCP port to use, or '0' if SMTPc_DFLT_PORT.
 *
 * @param    p_username      Pointer to user name, if authentication enabled.
 *
 * @param    p_pwd           Pointer to password,  if authentication enabled.
 *
 * @param    p_secure_cfg    Pointer to the secure configuration (TLS/SSL):
 *                           DEF_NULL, if no security enabled.
 *                           Pointer to a structure that contains the parameters.
 *
 * @param    p_msg           SMTPc_MSG structure encapsulating the message to send.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function.
 *
 * @note     (1) The function SMTPc_MsgSetParam() must be called before it can send a message.
 *******************************************************************************************************/
void SMTPc_SendMail(CPU_CHAR                *p_host_name,
                    CPU_INT16U              port,
                    CPU_CHAR                *p_username,
                    CPU_CHAR                *p_pwd,
                    NET_APP_SOCK_SECURE_CFG *p_secure_cfg,
                    SMTPc_MSG               *p_msg,
                    RTOS_ERR                *p_err)
{
  NET_SOCK_ID sock;
  RTOS_ERR    local_err;
  RTOS_ERR    *p_err_tmp;

  //                                                               -------------- CONNECT TO SMTP SEVER ---------------
  sock = SMTPc_Connect(p_host_name,
                       port,
                       p_username,
                       p_pwd,
                       p_secure_cfg,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ----------------- SEND THE MESSAGE -----------------
  SMTPc_SendMsg(sock, p_msg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_err_tmp = p_err;
  } else {
    p_err_tmp = &local_err;
  }
  //                                                               ----------- DISCONNECT FROM SMTP SERVER ------------
  SMTPc_Disconnect(sock, p_err_tmp);
}

/****************************************************************************************************//**
 *                                               SMTPc_Connect()
 *
 * @brief    Establishes a TCP connection to the SMTP server and initiates the SMTP session.
 *
 * @param    p_host_name     Pointer to hostname of the SMTP server to contact (can be an IP address).
 *
 * @param    port            TCP port to use, or enter '0' if SMTPc_DFLT_PORT.
 *
 * @param    p_username      Pointer to user name, if authentication is enabled.
 *
 * @param    p_pwd           Pointer to password,  if authentication is enabled.
 *
 * @param    p_secure_cfg    Pointer to the secure configuration (TLS/SSL):
 *                           DEF_NULL, if no security is enabled.
 *                           Pointer to a structure that contains the parameters.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function.
 *
 * @return   Socket descriptor/handle identifier, if NO error.
 *           -1, otherwise.
 *
 * @note     (1) Network security manager MUST be available and enabled to initialize the server in
 *           secure mode.
 *
 * @note     (2) If anything goes wrong while trying to connect to the server, the socket is closed
 *           by calling NetSock_Close. In case of a failure to establish the TCP connection,
 *           all data structures are returned to their original state.
 *           @n
 *           If the failure occurs when initiating the session, the application is responsible
 *           to take the appropriate action(s).
 *
 * @note     (3) The server will send a 220 "Service ready" reply when the connection is completed.
 *           The SMTP protocol allows a server to formally reject a transaction, while still
 *           allowing the initial connection by responding with a 554 "Transaction failed"
 *           reply.
 *
 * @note     (4) The Plain-text (PLAIN) authentication mechanism is implemented here. However, it
 *           takes some liberties from RFC #4964, section 4 'The AUTH Command', stating the "A
 *           server implementation MUST implement a configuration in which it does not permit
 *           any plaintext password mechanisms, unless either the STARTTLS command has been
 *           negotiated or some other mechanism that protects the session from password snooping
 *           has been provided".
 *           @n
 *           Since this client does not support SSL or TLS, nor any other protection against
 *           password snooping, it relies on the server to NOT fully follow RFC #4954 to be
 *           successful.
 *******************************************************************************************************/
NET_SOCK_ID SMTPc_Connect(CPU_CHAR                *p_host_name,
                          CPU_INT16U              port,
                          CPU_CHAR                *p_username,
                          CPU_CHAR                *p_pwd,
                          NET_APP_SOCK_SECURE_CFG *p_secure_cfg,
                          RTOS_ERR                *p_err)
{
  NET_SOCK_ID   sock_id = NET_SOCK_ID_NONE;
  CPU_INT32U    completion_code;
  CPU_CHAR      *p_reply;
  NET_SOCK_ADDR socket_addr;
  CPU_INT16U    port_server;
  RTOS_ERR      local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT, NET_SOCK_ID_NONE);
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((p_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_SOCK_ID_NONE);

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  //                                                               ------------------ VALIDATE PTR --------------------
#ifdef  NET_SECURE_MODULE_EN
  RTOS_ASSERT_DBG_ERR_SET((p_secure_cfg != DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, NET_SOCK_ID_NONE);
#endif

  //                                                               ------------------ DETERMINE PORT ------------------
  if (port != 0) {
    port_server = port;
  } else {
    if (p_secure_cfg != DEF_NULL) {                             // Set the port according to the secure mode cfg.
      port_server = SMTPc_PORT_SECURE_DFLT;
    } else {
      port_server = SMTPc_PORT_DFLT;
    }
  }

  //                                                               ----------------- OPEN CLIENT STREAM ---------------
  NetApp_ClientStreamOpenByHostname(&sock_id,
                                    p_host_name,
                                    port_server,
                                    &socket_addr,
                                    p_secure_cfg,
                                    SMTPc_CfgPtr->TimeoutConn_ms,
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
  //                                                               ---------------- CFG SOCK BLOCK OPT ----------------
  (void)NetSock_CfgBlock(sock_id, NET_SOCK_BLOCK_SEL_BLOCK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  NetSock_CfgTimeoutRxQ_Set(sock_id, SMTPc_CfgPtr->TimeoutRx_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  NetSock_CfgTimeoutTxQ_Set(sock_id, SMTPc_CfgPtr->TimeoutTx_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  NetSock_CfgTimeoutConnCloseSet(sock_id, SMTPc_CfgPtr->TimeoutClose_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  p_reply = SMTPc_RxReply(sock_id, p_err);                      // See Note #5.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  SMTPc_ParseReply(p_reply, &completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }
  //                                                               -------------- INITIATE SMTP SESSION ---------------
  p_reply = SMTPc_HELO(sock_id, &completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_disconnect;
  }

  //                                                               -------------------- AUTH CLIENT -------------------
  //                                                               See Note #5.
  if (p_username != DEF_NULL) {
    if (p_pwd == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto exit_disconnect;
    }

    SMTPc_AUTH(sock_id, p_username, p_pwd, &completion_code, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_disconnect;
    }
  }

  goto exit_release;

exit_disconnect:
  SMTPc_QUIT(sock_id, &completion_code, &local_err);

exit_close:
  NetSock_Close(sock_id, &local_err);
  sock_id = NET_SOCK_ID_NONE;

exit_release:
  KAL_LockRelease(SMTPc_Lock, p_err);

exit_err:
  return (sock_id);
}

/****************************************************************************************************//**
 *                                               SMTPc_SendMsg()
 *
 * @brief    Sends a message (an instance of the SMTPc_MSG structure) to the SMTP server.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_msg       SMTPc_MSG structure encapsulating the message to send.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function.
 *
 * @note     (1) The function SMTPc_MsgSetParam() must be called before it can send a message.
 *
 * @note     (2) The message must have at least one receiver: either "To", "CC", or "BCC".
 *******************************************************************************************************/
void SMTPc_SendMsg(NET_SOCK_ID sock_id,
                   SMTPc_MSG   *p_msg,
                   RTOS_ERR    *p_err)
{
  CPU_INT32U completion_code;
  SMTPc_ADDR *p_addr;
  RTOS_ERR   local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->FromAddr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_msg->To_List != DEF_NULL)
                           || (p_msg->CC_List != DEF_NULL)
                           || (p_msg->BCC_List != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((p_msg->MsgSubject != DEF_NULL)
                           || (p_msg->MsgBody != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((sock_id != NET_SOCK_ID_NONE), *p_err, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------------- INVOKE THE MAIL CMD ----------------
  SMTPc_MAIL(sock_id, p_msg->FromAddr, &completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Error MAIL.  Code: %u\n\r", (u)completion_code));
    goto exit_reset;
  }

  //                                                               --------------- INVOKE THE RCTP CMD ----------------
  //                                                               The RCPT cmd is tx'd for every recipient,
  //                                                               including CCs & BCCs.
  SLIST_FOR_EACH_ENTRY(p_msg->To_List, p_addr, SMTPc_ADDR, Node) {
    SMTPc_RCPT(sock_id, p_addr->AddrPtr, &completion_code, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Error RCPT.  Code: %u\n\r", (u)completion_code));
      goto exit_reset;
    }
  }

  //                                                               CCs
  SLIST_FOR_EACH_ENTRY(p_msg->CC_List, p_addr, SMTPc_ADDR, Node) {
    SMTPc_RCPT(sock_id, p_addr->AddrPtr, &completion_code, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Error RCPT.  Code: %u\n\r", (u)completion_code));
      goto exit_reset;
    }
  }

  //                                                               BCCs
  SLIST_FOR_EACH_ENTRY(p_msg->BCC_List, p_addr, SMTPc_ADDR, Node) {
    SMTPc_RCPT(sock_id, p_addr->AddrPtr, &completion_code, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Error RCPT (CC).  Code: %u\n\r", (u)completion_code));
      goto exit_reset;
    }
  }

  //                                                               --------------- INVOKE THE DATA CMD ----------------
  SMTPc_DATA(sock_id, &completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Error DATA.  Code: %u\n\r", (u)completion_code));
    goto exit_reset;
  }

  //                                                               ----------- BUILD & SEND THE ACTUAL MSG ------------
  SMTPc_SendBody(sock_id, p_msg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Error SMTPc_SendBody.  Error: %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit_reset;
  }

  goto exit_release;

exit_reset:
  if ((completion_code != SMTPc_REP_421)
      && (completion_code != SMTPc_REP_221)) {
    SMTPc_RSET(sock_id, &completion_code, &local_err);
  }

exit_release:
  KAL_LockRelease(SMTPc_Lock, p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           SMTPc_Disconnect()
 *
 * @brief    Closes the connection between the client and the server.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function.
 *
 * @note     (2) The receiver (client) MUST NOT intentionally close the transmission channel until
 *               it receives and replies to a QUIT command.
 *
 * @note     (3) The receiver of the QUIT command MUST send an OK reply, then close the transmission
 *               channel.
 *******************************************************************************************************/
void SMTPc_Disconnect(NET_SOCK_ID sock_id,
                      RTOS_ERR    *p_err)
{
  CPU_INT32U completion_code;
  RTOS_ERR   local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((SMTPc_InitActive != DEF_ACTIVE), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((sock_id != NET_SOCK_ID_NONE), *p_err, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(SMTPc_Lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SMTPc_QUIT(sock_id, &completion_code, p_err);

  NetSock_Close(sock_id, &local_err);

  KAL_LockRelease(SMTPc_Lock, p_err);

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SMTPc_RxReply()
 *
 * @brief    Receive and process reply from the SMTP server.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_err       Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL, otherwise.
 *
 * @note     (1) Server reply is at least 3 characters long (3 digits), plus CRLF.  Hence, receiving
 *           less than that automatically indicates an error.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_RxReply(NET_SOCK_ID sock_id,
                               RTOS_ERR    *p_err)
{
  NET_SOCK_RTN_CODE rx_len;

  //                                                               ---------------------- RX REPLY --------------------
  rx_len = NetSock_RxData(sock_id,
                          SMTPc_Comm_Buf,
                          SMTPc_COMM_BUF_LEN - 1,
                          NET_SOCK_FLAG_NONE,
                          p_err);
  //                                                               See Note #1.
  if ((rx_len == NET_SOCK_BSD_ERR_RX)
      || (rx_len <= 4)) {
    return (DEF_NULL);
  }

  SMTPc_Comm_Buf[rx_len] = '\0';

  return (SMTPc_Comm_Buf);
}

/****************************************************************************************************//**
 *                                           SMTPc_ParseReply()
 *
 * @brief    (1) Process reply received from the SMTP server.
 *               - (a) Parse reply
 *               - (b) Interpret reply
 *
 * @param    p_server_reply      Complete reply received from the server.
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *******************************************************************************************************/
static void SMTPc_ParseReply(CPU_CHAR   *p_server_reply,
                             CPU_INT32U *p_completion_code,
                             RTOS_ERR   *p_err)
{
  CPU_INT08U len;

  //                                                               -------------------- PARSE REPLY  ------------------
  len = Str_Len(p_server_reply);                                  // Make sure string is at least 3 + 1 char long.
  if (len < 4) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }

  *p_completion_code = Str_ParseNbr_Int32U(p_server_reply, DEF_NULL, 10);
  LOG_VRB(("Code: %u\n\r", (u) * p_completion_code));
}

/****************************************************************************************************//**
 *                                           SMTPc_GetCodeGroup()
 *
 * @brief    Retrieve the code group.
 *
 * @param    completion_code     Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *******************************************************************************************************/
static CPU_INT08U SMTPc_GetCodeGroup(CPU_INT32U completion_code,
                                     RTOS_ERR   *p_err)
{
  CPU_INT08U code_first_dig;

  code_first_dig = completion_code / 100;

  switch (code_first_dig) {
    case SMTPc_REP_POS_COMPLET_GRP:                             // Positive reply.
    case SMTPc_REP_POS_PRELIM_GRP:                              // Intermediate reply.
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:                       // Negative reply.
    case SMTPc_REP_NEG_COMPLET_GRP:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:                                                    // Should never happen, interpreted as negative.
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;
  }

  return (code_first_dig);
}

/****************************************************************************************************//**
 *                                           SMTPc_QueryServer()
 *
 * @brief    Send a query (or anything else) to the server.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    query       Query in question.
 *
 * @param    len         Length of message to transmit
 *
 * @param    p_err       Pointer to variable that will hold the return error code from this function.
 *******************************************************************************************************/
static void SMTPc_QueryServer(NET_SOCK_ID sock_id,
                              CPU_CHAR    *query,
                              CPU_INT32U  len,
                              RTOS_ERR    *p_err)
{
  NET_SOCK_RTN_CODE rtn_code;
  CPU_INT32U        cur_pos;
  RTOS_ERR          local_err;

  //                                                               ---------------------- TX QUERY --------------------
  cur_pos = 0;

  do {
    rtn_code = NetSock_TxData(sock_id,
                              &query[cur_pos],
                              len,
                              0,
                              &local_err);

    cur_pos = cur_pos + rtn_code;
    len = len     - rtn_code;
  } while ((len != 0) && (rtn_code != NET_SOCK_BSD_ERR_TX));

  if (rtn_code != NET_SOCK_BSD_ERR_TX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }
}

/****************************************************************************************************//**
 *                                               SMTPc_SendBody()
 *
 * @brief    (1) Prepare and send the actual data of the message, i.e. the body part of the message
 *               content.
 *               - (a) Built headers and transmit
 *               - (b) Transmit body content
 *               - (c) Prepare and transmit attachment(s)
 *               - (d) Transmit "end of mail data" indicator
 *               - (e) Receive the confirmation reply
 *
 * @param    sock_id     Socket ID.
 *
 * @param    msg         SMTPc_MSG structure encapsulating the message to send.
 *
 * @param    p_err       Pointer to variable that will hold the return error code from this function.
 *
 * @note     (2) The current implementation does not insert the names of the mailbox owners (member
 *               NameDisp of structure SMTPc_MBOX).
 *******************************************************************************************************/
static void SMTPc_SendBody(NET_SOCK_ID sock_id,
                           SMTPc_MSG   *p_msg,
                           RTOS_ERR    *p_err)
{
  CPU_SIZE_T len;
  CPU_INT32U cur_wr_ix;
  CPU_INT32U line_len;
  CPU_CHAR   *hdr;
  CPU_CHAR   *reply;
  CPU_INT32U completion_code;
  CPU_INT08U code_grp;
  SMTPc_ADDR *p_addr;
  RTOS_ERR   local_err;

  cur_wr_ix = 0;
  line_len = 0;

  //                                                               ------------------- BUILT HEADERS ------------------
  //                                                               Header "From: ".
  hdr = (CPU_CHAR *)SMTPc_HDR_FROM;
  cur_wr_ix = SMTPc_BuildHdr(sock_id,                           // Addr
                             SMTPc_Comm_Buf,
                             SMTPc_COMM_BUF_LEN,
                             cur_wr_ix,
                             hdr,
                             p_msg->FromAddr,
                             &line_len,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_msg->Sender != DEF_NULL) {                                // Header "Sender: ".
    hdr = (CPU_CHAR *)SMTPc_HDR_SENDER;
    cur_wr_ix = SMTPc_BuildHdr(sock_id,                         // Addr
                               SMTPc_Comm_Buf,
                               SMTPc_COMM_BUF_LEN,
                               cur_wr_ix,
                               hdr,
                               p_msg->Sender,
                               &line_len,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  //                                                               Header "To: ".
  hdr = (CPU_CHAR *)SMTPc_HDR_TO;
  SLIST_FOR_EACH_ENTRY(p_msg->To_List, p_addr, SMTPc_ADDR, Node) {
    cur_wr_ix = SMTPc_BuildHdr(sock_id,
                               SMTPc_Comm_Buf,
                               SMTPc_COMM_BUF_LEN,
                               cur_wr_ix,
                               hdr,
                               p_addr->AddrPtr,
                               &line_len,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    hdr = DEF_NULL;
  }

  //                                                               Header "Reply-to: ".
  hdr = (CPU_CHAR *)SMTPc_HDR_REPLYTO;
  if (p_msg->ReplyTo != DEF_NULL) {
    cur_wr_ix = SMTPc_BuildHdr(sock_id,
                               SMTPc_Comm_Buf,
                               SMTPc_COMM_BUF_LEN,
                               cur_wr_ix,
                               hdr,
                               p_msg->ReplyTo,
                               &line_len,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    hdr = DEF_NULL;
  }

  //                                                               Header "CC: ".
  hdr = (CPU_CHAR *)SMTPc_HDR_CC;
  SLIST_FOR_EACH_ENTRY(p_msg->CC_List, p_addr, SMTPc_ADDR, Node) {
    cur_wr_ix = SMTPc_BuildHdr(sock_id,
                               SMTPc_Comm_Buf,
                               SMTPc_COMM_BUF_LEN,
                               cur_wr_ix,
                               hdr,
                               p_addr->AddrPtr,
                               &line_len,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    hdr = DEF_NULL;
  }

  if (p_msg->MsgSubject != DEF_NULL) {                               // Header "MsgSubject: ".
    cur_wr_ix = SMTPc_BuildHdr(sock_id,
                               SMTPc_Comm_Buf,
                               SMTPc_COMM_BUF_LEN,
                               cur_wr_ix,
                               SMTPc_HDR_SUBJECT,
                               p_msg->MsgSubject,
                               &line_len,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  //                                                               ----------- INSERT HEADER/BODY DELIMITER -----------
  Mem_Copy(&SMTPc_Comm_Buf[cur_wr_ix], SMTPc_CRLF, SMTPc_CRLF_SIZE);
  cur_wr_ix += SMTPc_CRLF_SIZE;

  //                                                               ---------------- TX CONTENT HEADERS ----------------
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, cur_wr_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------------ TX BODY CONTENT -----------------
  len = Str_Len(p_msg->MsgBody);
  SMTPc_QueryServer(sock_id, p_msg->MsgBody, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------- TX END OF MAIL DATA INDICATOR ----------
  Mem_Copy(SMTPc_Comm_Buf, SMTPc_EOM, sizeof(SMTPc_EOM));
  len = Str_Len(SMTPc_Comm_Buf);
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------------- RX CONFIRMATION REPLY --------------
  reply = SMTPc_RxReply(sock_id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SMTPc_ParseReply(reply, &completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  code_grp = SMTPc_GetCodeGroup(completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      if (completion_code != SMTPc_REP_250) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;
    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if ((completion_code != SMTPc_REP_421)
        && (completion_code != SMTPc_REP_221)) {
      SMTPc_RSET(sock_id, &completion_code, &local_err);
    }
  }
}

/****************************************************************************************************//**
 *                                               SMTPc_BuildHdr()
 *
 * @brief    (1) Prepare (and send if necessary) the message content's headers.
 *               - (a) Calculate needed space
 *               - (b) Send data, if necessary
 *               - (c) Build header
 *
 * @param    sock_id     Socket ID.
 *
 * @param    buf         Buffer used to store the headers prior to their expedition.
 *
 * @param    buf_size    Size of buffer.
 *
 * @param    buf_wr_ix   Index of current "write" position.
 *
 * @param    hdr         Header name.
 *
 * @param    val         Value associated with header.
 *
 * @param    line_len    Current line total length.
 *
 * @param    p_err       Pointer to variable that will hold the return error code from this function.
 *                           - RTOS_ERR_NONE
 *
 * @return   "Write" position in buffer.
 *
 * @note     (2) If the parameter "hdr" is DEF_NULL, it means that it's already been passed in a
 *               previous call.  Hence, a "," will be inserted in the buffer prior to the value.
 *
 * @note     (3) If the SMTP line limit is exceeded, p_err is set to SMTPc_ERR_LINE_TOO_LONG and the
 *               function returns without having added the header.
 *
 * @note     (4) This implementation transmit the headers buffer if the next header is too large to
 *               be inserted in the remaining buffer space.
 *               @n
 *               Note that NO EXACT calculations are performed here;  a conservative approach is
 *               brought forward, without actually optimizing the process (i.e. a buffer could be
 *               sent even though a few more characters could have been inserted).
 *
 * @note     (5) CRLF is inserted even though more entries are still to come for a particular header
 *               (line folding is performed even if unnecessary).
 *******************************************************************************************************/
static CPU_INT32U SMTPc_BuildHdr(NET_SOCK_ID sock_id,
                                 CPU_CHAR    *buf,
                                 CPU_INT32U  buf_size,
                                 CPU_INT32U  buf_wr_ix,
                                 CPU_CHAR    *hdr,
                                 CPU_CHAR    *val,
                                 CPU_INT32U  *line_len,
                                 RTOS_ERR    *p_err)
{
  CPU_INT32U hdr_len;
  CPU_INT32U val_len;
  CPU_INT32U total_len;

  //                                                               ------------- CALCULATE NECESSARY SPACE ------------
  if (hdr == DEF_NULL) {
    hdr_len = 0;
  } else {
    hdr_len = Str_Len(hdr);
  }

  if (val == DEF_NULL) {
    val_len = 0;
  } else {
    val_len = Str_Len(val);
  }

  total_len = hdr_len + val_len + 2;

  if ((*line_len + total_len) > SMTPc_LINE_LEN_LIM) {           // See Note #3.
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (buf_wr_ix);
  }

  //                                                               -------------- SEND DATA, IF NECESSARY -------------
  //                                                               See Note #4.
  if ((buf_size - buf_wr_ix) < total_len) {
    SMTPc_QueryServer(sock_id, buf, buf_wr_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (buf_wr_ix);
    }
    buf_wr_ix = 0;
  }

  //                                                               ------------------ BUILDING HEADER -----------------
  if (hdr != DEF_NULL) {
    Mem_Copy(buf + buf_wr_ix, hdr, hdr_len);
    buf_wr_ix += hdr_len;
    *line_len += hdr_len;
  } else {                                                      // Not first item, adding ','.
    Mem_Copy(buf + buf_wr_ix, " ,", 2);
    buf_wr_ix += 2;
    *line_len += 2;
  }
  if (val != DEF_NULL) {
    Mem_Copy(buf + buf_wr_ix, val, val_len);
    buf_wr_ix += val_len;
    *line_len += val_len;
  }

  Mem_Copy(buf + buf_wr_ix, SMTPc_CRLF, 2);                     // See Note #5.
  buf_wr_ix += 2;
  *line_len = 0;

  buf[buf_wr_ix] = '\0';
  LOG_VRB(("String: %s\n", (s)buf));

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (buf_wr_ix);
}

/****************************************************************************************************//**
 *                                               SMTPc_HELO()
 *
 * @brief    (1) Build the HELO command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    completion_code     Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 *               completion_code Numeric value returned by server indicating command status.
 *
 *               p_err           Pointer to variable that will hold the return error code from this function.
 *
 * Return(s)   : Complete reply from the server, if NO reception error.
 *               DEF_NULL, otherwise.
 *
 * Note(s)     : (2) From RFC #2821, "the HELO command is used to identify the SMTP client to the SMTP
 *                   server".
 *
 *               (3) The server will send a 250 "Requested mail action okay, completed" reply upon
 *                   success.  A positive reply is the only reply that will lead to a  "RTOS_ERR_NONE"
 *                   error return code.
 *
 *               (4) This implementation will accept reply 250, as well as any other positive reply.
 *
 *               (5) From RFC #2821, section 4.1.3 Address Literals:
 *
 *                      For IPv4 addresses, this form uses four small decimal integers separated
 *                      by dots and enclosed by brackets such as [123.255.37.2], which
 *                      indicates an (IPv4) Internet Address in sequence-of-octets form.  For
 *                      IPv6 and other forms of addressing that might eventually be
 *                      standardized, the form consists of a standardized "tag" that
 *                      identifies the address syntax, a colon, and the address itself, in a
 *                      format specified as part of the IPv6 standards.
 *
 *
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_HELO(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR        *reply;
  CPU_SIZE_T      len;
  CPU_INT08U      client_addr[NET_CONN_ADDR_LEN_MAX];
  NET_SOCK_FAMILY client_addr_family;
  CPU_CHAR        client_addr_ascii[NET_ASCII_LEN_MAX_ADDR_IP];
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR ipv4_client_addr;
  CPU_INT32U    *p_client_addr_tmp;
#endif
  CPU_INT08U code_grp;

  //                                                               Get the IP address used in the conn.
  NetSock_GetLocalIPAddr(sock_id,
                         client_addr,
                         &client_addr_family,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  //                                                               Format the message depending of the address family.

  switch (client_addr_family) {
    case NET_SOCK_FAMILY_IP_V4:
#ifdef  NET_IPv4_MODULE_EN
      p_client_addr_tmp = (CPU_INT32U *)client_addr;

      ipv4_client_addr = NET_UTIL_NET_TO_HOST_32(*p_client_addr_tmp);
      NetASCII_IPv4_to_Str(ipv4_client_addr, client_addr_ascii, DEF_NO, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_NULL);
      }

      Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_HELO);
      Str_Cat(SMTPc_Comm_Buf, " [");
      Str_Cat(SMTPc_Comm_Buf, client_addr_ascii);
      Str_Cat(SMTPc_Comm_Buf, "]\r\n");
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif
      break;

    case NET_SOCK_FAMILY_IP_V6:
#ifdef  NET_IPv6_MODULE_EN
      NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)client_addr,
                           client_addr_ascii,
                           DEF_NO,
                           DEF_NO,
                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_NULL);
      }

      Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_HELO);
      Str_Cat(SMTPc_Comm_Buf, " [");
      Str_Cat(SMTPc_Comm_Buf, SMTPc_TAG_IPv6);
      Str_Cat(SMTPc_Comm_Buf, " ");
      Str_Cat(SMTPc_Comm_Buf, client_addr_ascii);
      Str_Cat(SMTPc_Comm_Buf, "]\r\n");
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif
      break;

    default:
      return (DEF_NULL);
  }

  len = Str_Len(SMTPc_Comm_Buf);
  //                                                               Send HELO Query.
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               --------- RX SERVER'S REPLY & VALIDATE -------------
  reply = SMTPc_RxReply(sock_id, p_err);                         // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(reply, p_completion_code, p_err);               // See Note #4.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_AUTH()
 *
 * @brief    (1) Build the AUTH command, send it to the server and validate reply.
 *               - (a) Encode username & password
 *               - (b) Send command to the server
 *               - (c) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    username            Mailbox username name for authentication.
 *
 * @param    pw                  Mailbox password for authentication.
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL,  otherwise.
 *
 * @note     (2) The user's credentials are transmitted to the server using a base 64 encoding and
 *               formated according to RFC #4616.  From Section 2 'PLAIN SALS Mechanism' "The client
 *               presents the authorization identity (identity to act as), followed by a NUL (U+0000)
 *               character, followed by the authentication identity (identity whose password will be
 *               used), followed by a NUL (U+0000) character, followed by the clear-text password.
 *               As with other SASL mechanisms, the client does not provide an authorization identity
 *               when it wishes the server to derive an identity from the credentials and use that
 *               as the authorization identity."
 *
 * @note     (3) The server will send a 235 "Authentication successful" reply upon success.  A positive
 *               reply is the only reply that will lead to a  "RTOS_ERR_NONE" error return code.
 *
 * @note     (4) This implementation will accept reply 235, as well as any other positive reply.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_AUTH(NET_SOCK_ID sock_id,
                            CPU_CHAR    *username,
                            CPU_CHAR    *pw,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *p_buf_unencoded;
  CPU_CHAR   *p_buf_encoded;
  CPU_INT16U unencoded_len;
  CPU_CHAR   *p_reply = DEF_NULL;
  CPU_INT16U wr_ix;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;

  p_buf_unencoded = (CPU_CHAR *)Mem_DynPoolBlkGet(SMTPc_AuthBufInputPoolPtr, p_err);
  p_buf_encoded = (CPU_CHAR *)Mem_DynPoolBlkGet(SMTPc_AuthBufOutputPoolPtr, p_err);

  //                                                               -------------- ENCODE USERNAME & PW ----------------
  //                                                               See Note #2.
  len = Str_Len(username) + Str_Len(pw) + 3;

  if (len > SMTPc_InitCfg.AuthBufInputLen) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }

  wr_ix = 0;
  p_buf_unencoded[wr_ix] = SMTPc_ENCODER_BASE64_DELIMITER_CHAR;
  wr_ix++;

  Str_Copy(&p_buf_unencoded[wr_ix], username);
  wr_ix += Str_Len(username);

  p_buf_unencoded[wr_ix] = SMTPc_ENCODER_BASE64_DELIMITER_CHAR;
  wr_ix++;

  Str_Copy(&p_buf_unencoded[wr_ix], pw);
  unencoded_len = wr_ix + Str_Len(pw);

  //                                                               Base 64 encoding.
  NetBase64_Encode(p_buf_unencoded, unencoded_len, p_buf_encoded, SMTPc_InitCfg.AuthBufOutputLen, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ TX CMD TO SERVER ----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_AUTH);
  Str_Cat(SMTPc_Comm_Buf, " ");
  Str_Cat(SMTPc_Comm_Buf, SMTPc_CMD_AUTH_MECHANISM_PLAIN);
  Str_Cat(SMTPc_Comm_Buf, " ");
  Str_Cat(SMTPc_Comm_Buf, p_buf_encoded);
  Str_Cat(SMTPc_Comm_Buf, "\r\n");
  len = Str_Len(SMTPc_Comm_Buf);

  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  p_reply = SMTPc_RxReply(sock_id, p_err);                        // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SMTPc_ParseReply(p_reply, p_completion_code, p_err);            // See Note #4.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      break;

    case SMTPc_REP_NEG_COMPLET_GRP:
      if (*p_completion_code == SMTPc_REP_535) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

exit:
  return (p_reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_MAIL()
 *
 * @brief    (1) Build the MAIL command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    from                Argument of the "MAIL" command (sender mailbox).
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) From RFC #2821, "the MAIL command is used to initiate a mail transaction in which
 *               the mail data is delivered to an SMTP server [...]".
 *
 * @note     (3) The server will send a 250 "Requested mail action okay, completed" reply upon
 *               success.  A positive reply is the only reply that will lead to a  "RTOS_ERR_NONE"
 *               error return code.
 *
 * @note     (4) This implementation will accept reply 250, as well as any other positive reply.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_MAIL(NET_SOCK_ID sock_id,
                            CPU_CHAR    *from,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *p_reply;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;
  //                                                               ----------------- TX CMD TO SERVER -----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_MAIL);
  Str_Cat(SMTPc_Comm_Buf, " FROM:<");
  Str_Cat(SMTPc_Comm_Buf, from);
  Str_Cat(SMTPc_Comm_Buf, ">\r\n");

  len = Str_Len(SMTPc_Comm_Buf);
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  p_reply = SMTPc_RxReply(sock_id, p_err);                      // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(p_reply, p_completion_code, p_err);          // See Note #4.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      break;

    case SMTPc_REP_NEG_COMPLET_GRP:
    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (p_reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_RCPT()
 *
 * @brief    (1) Build the RCPT command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    to                  Argument of the "RCPT" command (receiver mailbox).
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) From RFC #2821, "the RCPT command is used to identify an individual recipient of the
 *               mail data; multiple recipients are specified by multiple use of this command".
 *
 * @note     (3) The server will send a 250 "Requested mail action okay, completed"  or a 251 "User
 *               not local; will forwarded to <forward-path>" reply upon success.
 *
 * @note     (4) This implementation will accept replies 250 and 251 as positive replies.  Reply 551
 *               "User not local; please try <forward-path>" will result in an error.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_RCPT(NET_SOCK_ID sock_id,
                            CPU_CHAR    *to,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *p_reply;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;

  //                                                               ----------------- TX CMD TO SERVER -----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_RCPT);
  Str_Cat(SMTPc_Comm_Buf, " TO:<");
  Str_Cat(SMTPc_Comm_Buf, to);
  Str_Cat(SMTPc_Comm_Buf, ">\r\n");

  len = Str_Len(SMTPc_Comm_Buf);
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  p_reply = SMTPc_RxReply(sock_id, p_err);                      // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(p_reply, p_completion_code, p_err);          // See Note #4.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      if ((*p_completion_code != SMTPc_REP_250)
          && (*p_completion_code != SMTPc_REP_251)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (p_reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_DATA()
 *
 * @brief    (1) Build the DATA command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL,                       otherwise.
 *
 * @note     (2) The DATA command is used to indicate to the SMTP server that all the following lines
 *               up to but not including the end of mail data indicator are to be considered as the
 *               message text.
 *
 * @note     (3) The receiver normally sends a 354 "Start mail input" reply and then treats the lines
 *               following the command as mail data from the sender.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_DATA(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *reply;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;

  //                                                               ----------------- TX CMD TO SERVER -----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_DATA);
  Str_Cat(SMTPc_Comm_Buf, "\r\n");

  len = Str_Len(SMTPc_Comm_Buf);
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  reply = SMTPc_RxReply(sock_id, p_err);                        // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(reply, p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  switch (code_grp) {
    case SMTPc_REP_POS_INTER_GRP:
      if (*p_completion_code != SMTPc_REP_354) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_COMPLET_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_RSET()
 *
 * @brief    (1) Build the RSET command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) From RFC #2821, "the RSET command specifies that the current mail transaction will
 *               be aborted.  Any stored sender, recipients, and mail data MUST be discarded, and all
 *               buffers and state tables cleared".
 *
 * @note     (3) The server MUST send a 250 "Requested mail action okay, completed" reply to a RSET
 *               command with no arguments.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_RSET(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *reply;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;

  //                                                               ----------------- TX CMD TO SERVER -----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_RSET);
  Str_Cat(SMTPc_Comm_Buf, "\r\n");

  len = Str_Len(SMTPc_Comm_Buf);
  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  reply = SMTPc_RxReply(sock_id, p_err);                        // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(reply, p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      if (*p_completion_code != SMTPc_REP_250) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (reply);
}

/****************************************************************************************************//**
 *                                               SMTPc_QUIT()
 *
 * @brief    (1) Build the QUIT command, send it to the server and validate reply.
 *               - (a) Send command to the server
 *               - (b) Receive server's reply and validate
 *
 * @param    sock_id             Socket ID.
 *
 * @param    p_completion_code   Numeric value returned by server indicating command status.
 *
 * @param    p_err               Pointer to variable that will hold the return error code from this function.
 *
 * @return   Complete reply from the server, if NO reception error.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) From RFC #2821, "the QUIT command specifies that the receiver MUST send an OK reply,
 *               and then close the transmission channel.  The receiver MUST NOT intentionally close
 *               the transmission channel until it receives and replies to a QUIT command (even if
 *               there was an error).  the sender MUST NOT intentionally close the transmission
 *               channel until it sends a QUIT command and SHOULD wait until it receives the reply
 *               (even if there was an error response to a previous command)".
 *
 * @note     (3) The server MUST send a 221 "Service closing transmission channel" reply to a QUIT
 *               command.
 *******************************************************************************************************/
static CPU_CHAR *SMTPc_QUIT(NET_SOCK_ID sock_id,
                            CPU_INT32U  *p_completion_code,
                            RTOS_ERR    *p_err)
{
  CPU_CHAR   *reply;
  CPU_SIZE_T len;
  CPU_INT08U code_grp;

  //                                                               ----------------- TX CMD TO SERVER -----------------
  Str_Copy(SMTPc_Comm_Buf, SMTPc_CMD_QUIT);
  Str_Cat(SMTPc_Comm_Buf, "\r\n");
  len = Str_Len(SMTPc_Comm_Buf);

  SMTPc_QueryServer(sock_id, SMTPc_Comm_Buf, len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- RX SERVER'S RESPONSE & VALIDATE ---------
  reply = SMTPc_RxReply(sock_id, p_err);                        // See Note #3.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SMTPc_ParseReply(reply, p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  code_grp = SMTPc_GetCodeGroup(*p_completion_code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  switch (code_grp) {
    case SMTPc_REP_POS_COMPLET_GRP:
      if (*p_completion_code != SMTPc_REP_221) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      }
      break;

    case SMTPc_REP_POS_PRELIM_GRP:
    case SMTPc_REP_POS_INTER_GRP:
    case SMTPc_REP_NEG_TRANS_COMPLET_GRP:
    case SMTPc_REP_NEG_COMPLET_GRP:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  return (reply);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_SMTP_CLIENT_AVAIL
