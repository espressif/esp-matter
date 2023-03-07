/***************************************************************************//**
 * @file
 * @brief Network - Telnet Server
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

#if (defined(RTOS_MODULE_NET_TELNET_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error Telnet Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

#if (!defined(RTOS_MODULE_COMMON_SHELL_AVAIL))
#error Telnet Server Module requires Shell module. Make sure it is part of your project \
  and that RTOS_MODULE_COMMON_SHELL_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/include/rtos_prio.h>

#include  "../tcpip/net_sock_priv.h"

#include  <net/include/telnet_server.h>
#include  <net/include/net_cfg_net.h>
#include  <net/include/net_util.h>
#include  <net/include/net_app.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#include  "../tcpip/net_ipv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#include  "../tcpip/net_ipv6_priv.h"
#endif

#include  <common/include/kal.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/include/rtos_types.h>
#include  <common/include/shell.h>
#include  <common/include/auth.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               RTOS DEFINES
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, TELNET)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                               TASK NAMES
 *******************************************************************************************************/

#define  TELNETs_TASK_SERVER_NAME        "TELNET (Server)"
#define  TELNETs_TASK_SESSION_NAME       "TELNET (Session)"

/********************************************************************************************************
 *                                       TELNETs ERROR CODES DEFINES
 *
 * Note(s) : (1) The TELNETs_CmdProcess() function MUST return TELNETs_CMDPROCESS_ERR when an error occurred.
 *               Any other return value means the processing of the command was successful.
 *******************************************************************************************************/

#define  TELNETs_CMDPROCESS_ERR_NONE                       0
#define  TELNETs_CMDPROCESS_ERR                           -1    // See Note #1.

#define  TELNETs_SHELL_ERR_TX                             -1

/********************************************************************************************************
 *                                   TELNETs SESSION AND CONNECTION DEFINES
 *
 * Note(s) : (1) This defines the maximum number of session task(s) supported.
 *
 *               (a) #### This current version of TELNETs only supports 1 client at a time.  Therefore,
 *                   this define should always be set to '1'.
 *******************************************************************************************************/

#define  TELNETs_SESSION_MAX_NBR                           1    // See Note #1.

#define  TELNETs_CONN_Q_SIZE                               3

#define  TELNETs_FAILED_LOGIN_DLY_MS                     200    // Dly between login tries.

/********************************************************************************************************
 *                                       TELNET USASCII GRAPHIC CODES
 *
 * Note(s) : From RFC #1938, section 'The NVT printer and keyboard', "The NVT can produce representations
 *           of all 95 USASCII graphics (codes 32 through 126)".  The lower and higher codes are here
 *           defined to help determine if they should be sent to the printer or not.
 *******************************************************************************************************/

#define  TELNET_ASCII_GRAPHIC_LOWER                       32    // Lower  graphic code displayed by the NVT.
#define  TELNET_ASCII_GRAPHIC_HIGHER                     126    // Higher graphic code displayed by the NVT.

/********************************************************************************************************
 *                                           TELNET COMMAND DEFINES
 *
 * Note(s) : (1) According to RFC #854, 'TELNET PROTOCOL SPECIFICATION', This should always be accompanied
 *               by a TCP Urgent notification.
 *
 *           (2) The 'SB' command indicates that what follows is sub-negotiation of the indicated option.
 *
 *           (3) From RFC #854, 'TELNET PROTOCOL SPECIFICATION', section 'General Considerations', "The
 *               principle of negotiated options takes cognizance of the fact that many hosts will wish
 *               to provide additional services over and above those available within an NVT [and these
 *               options] may be used with the DO, DON'T, WILL, and WON'T structure to allow a user and
 *               server to agree to use a more elaborate set of conventions for their TELNET connection".
 *
 *               WILL XXX is sent to indicate a party's desire (offer) to begin performing option XXX.
 *                           DO    XXX    positive response
 *                           DON'T XXX    negative response
 *
 *               DO   XXX is sent to indicate a desire (request) that the other party begin performing
 *               option XXX
 *                           WILL  XXX    positive acknowledgment
 *                           WON'T XXX    negative acknowledgment
 *
 *               Since a basic NVT is what is left when no options are enabled, responding with DON'T and
 *               WON'T guarantees to leave the connection in a state which both hosts can handle.
 *******************************************************************************************************/

typedef enum {                                                  // See Note #3.
  TELNETs_OPT_STATUS_CMD_SE = 240,                              // End of subnegotiation parameters.
  TELNETs_OPT_STATUS_CMD_NOP = 241,                             // No operation.
  TELNETs_OPT_STATUS_CMD_DM = 242,                              // Data stream portion of a Synch (see Note #1).
  TELNETs_OPT_STATUS_CMD_BRK = 243,                             // NVT character BRK (Break).
  TELNETs_OPT_STATUS_CMD_IP = 244,                              // The function IP   (Interrupt Process).
  TELNETs_OPT_STATUS_CMD_AO = 245,                              // The function AO   (Abord Output).
  TELNETs_OPT_STATUS_CMD_AYT = 246,                             // The function AYT  (Are You There).
  TELNETs_OPT_STATUS_CMD_EC = 247,                              // The function EC   (Erase Character).
  TELNETs_OPT_STATUS_CMD_EL = 248,                              // The function EL   (Erase Line).
  TELNETs_OPT_STATUS_CMD_GA = 249,                              // The GA signal     (Go Ahead).
  TELNETs_OPT_STATUS_CMD_SB = 250,                              // Beginning of subnegotiation (see Note #2).
  TELNETs_OPT_STATUS_CMD_WILL = 251,
  TELNETs_OPT_STATUS_CMD_WONT = 252,
  TELNETs_OPT_STATUS_CMD_DO = 253,
  TELNETs_OPT_STATUS_CMD_DONT = 254,
  TELNETs_OPT_STATUS_CMD_IAC = 255                              // The IAC command   (Indicate As Command).
}  TELNETs_OPT_STATUS_CMD;

#define  TELNETs_CMD_MAX_BUF_LEN                           3    // Cmd buf defines.
#define  TELNETs_CMD_BUF_LEN_NO_OPT                        2
#define  TELNETs_CMD_BUF_LEN_WITH_OPT                      3

#define  TELNETs_CMD_IAC_OFFSET                            0
#define  TELNETs_CMD_CMD_OFFSET                            1
#define  TELNETs_CMD_OPT_OFFSET                            2

/********************************************************************************************************
 *                                           TELNET OPTION DEFINES
 *
 * Note(s) : (1) This section defines some telnet option codes.  This list is not intended to be exhaustive,
 *               and the listed options codes are not necessarily supported and implemented.  Indeed, the
 *               list of supported options can be found in the file telnet-server.c, under the
 *               "INITIALIZED DATA" section.
 *           (2) When calling function TELNETs_TxCmd() with a command not related to any option, this
 *               defined dummy value should be passed as the third parameter. Values 141-254 are
 *               unassigned as per IANA.
 *******************************************************************************************************/

#define  TELNET_OPT_TX_BINARY                              0
#define  TELNET_OPT_ECHO                                   1
#define  TELNET_OPT_SUPPRESS_GA                            3
#define  TELNET_OPT_STATUS                                 5
#define  TELNET_OPT_TIMING_MARK                            6
#define  TELNET_OPT_NO_OPT                               254    // See Note #2.
#define  TELNET_OPT_EXT_OPT_LIST                         255

/********************************************************************************************************
 *                                       TELNET SUPPORTED OPTIONS DEFINE
 *
 * Note(s) : (1) This defines the number of supported options in this current implementation.  The particular
 *                   options are defined in the TELNETs_SupportedOptTbl table in the implementation file.  This
 *                   define MUST match the number of options appearing in previously mentioned table.
 *******************************************************************************************************/

#define  TELNET_NBR_OPT_SUPPORTED                          2    // See Note #1.

/********************************************************************************************************
 *                                   TELNETs STRING AND CHARACTER DEFINES
 *******************************************************************************************************/

#define  TELNETs_SYS_MSG_STR                       "\x0D\x0A" \
                                                   "Micrium Telnet Server\x0D\x0A\x0D\x0A"

#define  TELNETs_NO_SERVICE_STR                    "Service not available, try again later\x0D\x0A"

#define  TELNETs_PROMPT_STR                        ">"

#define  TELNETs_LOGIN_STR                         "login: "
#define  TELNETs_PW_STR                            "Password: "
#define  TELNETs_LOGIN_FAILURE_STR                 "Login incorrect\x0D\x0A"

#define  TELNETs_CMD_PROCESS_ERR_STR               "Command not recognized\x0D\x0A"

#define  TELNETs_EOL_STR                           "\x0D\x0A"
#define  TELNETs_EOL_STR_LEN                               2

/********************************************************************************************************
 *                                   TELNETs INTERNAL COMMANDS DEFINES
 *******************************************************************************************************/

#define  TELNETs_INT_CMD_LOGOUT                    "logout"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  TELNETs_BS_CHAR                                  "\b"  // Backspace  char.
#define  TELNETs_BS_CHAR_LEN                               2u   // Baskspace  char len.

#define  TELNETs_WS_CHAR                                  " "   // Whitespace char.
#define  TELNETs_WS_CHAR_LEN                               1u   // Whitespace char len.

#define  TELNETs_INIT_CFG_DFLT          { \
    .MemSegPtr = DEF_NULL                 \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       TELNET NVT PARSE STATES
 *
 * Note(s) : (1) The TELNETs_NVTPrint() function relies on a state machine in to parse incoming data in
 *               order to treat them as telnet command or as regular USASCII graphic characters.  The
 *               TELNETs_NVT_STATE enum defines the various states the machine may be in.
 *******************************************************************************************************/

typedef enum {
  TELNETs_NVT_STATE_GRAPHIC = 1,
  TELNETs_NVT_STATE_CR = 2,
  TELNETs_NVT_STATE_IAC = 3,
  TELNETs_NVT_STATE_CODE = 4,
  TELNETs_NVT_STATE_OPTION = 5
} TELNETs_NVT_STATE;

/********************************************************************************************************
 *                                       TELNETs OPTION STATUS DATA TYPES
 *
 * Note(s) : (1) This structure contains the option status for both sides of the connection (server and
 *               client), as well as a flag indicating whether or not we have sent a request for a change
 *               in option status not yet acknowledged for.
 *******************************************************************************************************/

typedef struct  TELNETs_Opt {
  CPU_INT08U             code;                                  // Num code   for the opt.
  TELNETs_OPT_STATUS_CMD server_status;                         // Status     for the opt (server side).
  TELNETs_OPT_STATUS_CMD client_status;                         // Status     for the opt (client side).
  CPU_BOOLEAN            server_status_req_tx;                  // Server status change req tx'd.
  CPU_BOOLEAN            client_status_req_tx;                  // Client status change req tx'd.
}  TELNETs_OPT;

/********************************************************************************************************
 *                                       TELNETs SESSION DATA TYPE
 *
 * Note(s) : (1) This structure is used by the session tasks to maintain their connection information.
 *
 *           (2) The reception buffer has its size set to a configurable length.
 *
 *           (3) The NVT printer buffer is used to simulate the presence of a console.  What this buffer
 *               contains is the last line (current one) of the terminal.  The character in this buffer
 *               should match the ones on the client's terminal.
 *
 *           (4) The list of the options is set when initializing the TELNETs_SESSION structure instance
 *                   in the TELNETs_NVTInit() function.  When option are supported, the NVT will accept request
 *                   from the client to change their status, and will allow the server to issue such a request.
 *******************************************************************************************************/

typedef struct  TELNETs_Session {
  const TELNETs_CFG      *CfgPtr;
  CPU_BOOLEAN            IsActive;                                     // Whether the session is active or not.
  NET_SOCK_ID            SockID;                                       // Sock id for this session.

#ifdef  RTOS_MODULE_FS_AVAIL
  void                   *CurWorkingDirPtr;                            // Cur working dir ptr.
#endif

  CPU_CHAR               *RxBufPtr;                                    // Rx buf          (see Note #2).
  CPU_INT32U             RxBufLen;                                     // Len of valid data in rx buf.

  //                                                               NVT printer buf (see Note #3).
  CPU_CHAR               *NVT_BufPtr;
  CPU_INT32U             NVT_BufLen;                                   // Len of valid data in nvt buf.

  CPU_CHAR               *               UserNameStrPtr;
  CPU_CHAR               *               PasswordStrPtr;

  CPU_INT08U             RxOptCode;                                    // Last rx'd opt code.
  TELNETs_OPT_STATUS_CMD RxOptStatusCmd;                               // Last rx'd opt status cmd.

  TELNETs_NVT_STATE      NVT_State;                                    // Last NVT state.

  TELNETs_OPT            Opt[TELNET_NBR_OPT_SUPPORTED];                // Supported opt   (See Note #4).

  SLIST_MEMBER           ListNode;
}  TELNETs_SESSION;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const TELNETs_CFG TELNETs_CfgDflt = {
  .Port = TELNET_SERVER_CFG_SERVER_PORT_DFLT,
  .IP_Type = TELNET_SERVER_CFG_SOCK_FAMILY_DFLT,
  .ConnInactivityTimeout_s = TELNET_SERVER_CFG_INACTIVITY_TIMEOUT_SEC_DFLT,
  .TxTriesMaxNbr = TELNET_SERVER_CFG_TX_TRIES_NBR_MAX_DFLT,
  .RxBufLen = TELNET_SERVER_CFG_RX_BUF_LEN_DFLT,
  .NVT_BufLen = TELNET_SERVER_CFG_NVT_BUF_LEN_DFLT,
  .UsernameStrLenMax = TELNET_SERVER_CFG_USERNAME_STR_LEN_MAX_DFLT,
  .PasswordStrLenMax = TELNET_SERVER_CFG_PASSWORD_STR_LEN_MAX_DFLT,
  .LoginTriesMaxNbr = TELNET_SERVER_CFG_LOGIN_TRIES_NBR_MAX_DFLT,
  .EchoEn = TELNET_SERVER_CFG_ECHO_EN_DFLT,
  .FS_PathLenMax = TELNET_SERVER_CFG_FS_PATH_LEN_MAX_DFLT,
  .WelcomeMsgStr = TELNET_SERVER_CFG_WELCOME_MSG_STR_DFLT
};

const RTOS_TASK_CFG TELNETs_TaskSrvCfgDflt = {
  .Prio = TELNET_SERVER_SRV_TASK_PRIO_DFLT,
  .StkSizeElements = TELNET_SERVER_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,
  .StkPtr = TELNET_SERVER_TASK_CFG_STK_PTR_DFLT
};

const RTOS_TASK_CFG TELNETs_TaskSessionCfgDflt = {
  .Prio = TELNET_SERVER_SESSION_TASK_PRIO_DFLT,
  .StkSizeElements = TELNET_SERVER_SESSION_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,
  .StkPtr = TELNET_SERVER_SESSION_TASK_CFG_STK_PTR_DFLT
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN TELNETs_InitActive = DEF_ACTIVE;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const TELNETs_INIT_CFG  TELNETs_InitCfgDflt = TELNETs_INIT_CFG_DFLT;
static TELNETs_INIT_CFG TELNETs_InitCfg = TELNETs_INIT_CFG_DFLT;
#else
extern const TELNETs_INIT_CFG TELNETs_InitCfg;
#endif

MEM_DYN_POOL *TELNETs_InstancesPoolPtr = DEF_NULL;

/********************************************************************************************************
 *                                           INITIALIZED DATA
 *
 * Note(s) : (1) This constant table defines the supported telnet options.  Those options should be
 *               defined in telnet_server.h, under the "TELNET OPTION DEFINES" section.  Also, the number
 *               of options in this table MUST match the TELNET_NBR_OPT_SUPPORTED constant define in
 *               the header file.
 *******************************************************************************************************/

//                                                                 See Note #1.
static const CPU_INT08U TELNETs_SupportedOptTbl[] = {
  TELNET_OPT_ECHO,
  TELNET_OPT_SUPPRESS_GA
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------------- TASK FNCT ---------------------
static TELNETs_INSTANCE *TELNETs_MemInstanceAlloc(const TELNETs_CFG *p_cfg,
                                                  RTOS_ERR          *p_err);

static void TELNETs_MemTaskAlloc(TELNETs_INSTANCE *p_instance,
                                 RTOS_ERR         *p_err);

static void TELNETs_TaskSrvHandler(void *p_arg);

static void TELNETs_TaskSessionStart(TELNETs_INSTANCE *p_instance,
                                     NET_SOCK_ID      sock_id_session,
                                     RTOS_ERR         *p_err);

static void TELNETs_TaskSessionHandler(void *p_arg);

//                                                                 --------------------- RX FNCT ----------------------
static CPU_BOOLEAN TELNETs_RxSessionData(TELNETs_INSTANCE *p_instance,
                                         TELNETs_SESSION  *p_session,
                                         CPU_BOOLEAN      echo_en,
                                         RTOS_ERR         *p_err);

static void TELNETs_RxOptHandler(TELNETs_SESSION        *p_session,
                                 TELNETs_OPT_STATUS_CMD opt_cmd_rx,
                                 CPU_INT08U             opt_code_rx,
                                 RTOS_ERR               *p_err);

static CPU_INT32S TELNETs_Rx(NET_SOCK_ID sock_id,
                             CPU_CHAR    *p_data_buf,
                             CPU_INT16U  data_buf_len,
                             CPU_INT32U  timeout_sec,
                             RTOS_ERR    *p_err);

//                                                                 --------------------- TX FNCT ----------------------
static void TELNETs_TxOptReq(TELNETs_SESSION        *p_session,
                             TELNETs_OPT_STATUS_CMD opt_status,
                             CPU_INT08U             opt_code,
                             RTOS_ERR               *p_err);

static void TELNETs_TxOptRep(TELNETs_SESSION        *p_session,
                             TELNETs_OPT_STATUS_CMD opt_status_req,
                             CPU_INT08U             opt_code,
                             TELNETs_OPT            *p_opt_cur,
                             RTOS_ERR               *p_err);

static CPU_BOOLEAN TELNETs_TxGA(TELNETs_SESSION *p_sesssion,
                                RTOS_ERR        *p_err);

static CPU_BOOLEAN TELNETs_TxCmd(TELNETs_SESSION *p_session,
                                 CPU_INT08U      cmd_code,
                                 CPU_INT08U      opt_code,
                                 RTOS_ERR        *p_err);

static CPU_BOOLEAN TELNETs_Tx(NET_SOCK_ID sock_id,
                              CPU_CHAR    *p_data_buf,
                              CPU_INT16U  data_buf_len,
                              CPU_INT08U  retry_max_nbr,
                              RTOS_ERR    *p_err);

//                                                                 -------------------- NVT FNCTS ---------------------
static CPU_BOOLEAN TELNETs_NVTInit(TELNETs_INSTANCE *p_instance,
                                   TELNETs_SESSION  *p_session,
                                   RTOS_ERR         *p_err);

static CPU_BOOLEAN TELNETs_NVTLogin(TELNETs_INSTANCE *p_instance,
                                    TELNETs_SESSION  *p_session,
                                    RTOS_ERR         *p_err);

static void TELNETs_NVTPrint(TELNETs_INSTANCE *p_instance,
                             TELNETs_SESSION  *p_session,
                             CPU_BOOLEAN      echo,
                             RTOS_ERR         *p_err);

static void TELNETs_NVTTxPrompt(TELNETs_INSTANCE *p_instance,
                                TELNETs_SESSION  *p_session,
                                RTOS_ERR         *p_err);

static void TELNETs_NVTGetBuf(TELNETs_SESSION *p_session,
                              CPU_CHAR        *p_dest_buf,
                              CPU_INT16U      dest_buf_len,
                              CPU_BOOLEAN     remove_eol,
                              RTOS_ERR        *p_err);

static void TELNETs_NVTTerminate(TELNETs_INSTANCE *p_instance,
                                 TELNETs_SESSION  *p_session);

//                                                                 --------------------- CMD FNCT ---------------------
static CPU_INT16S TELNETs_Cmd(CPU_CHAR        *p_cmd_line,
                              TELNETs_SESSION *p_session,
                              RTOS_ERR        *p_err);

static CPU_INT16S TELNETs_CmdHandlerInt(CPU_CHAR        *p_cmd_line,
                                        void            *p_cwd,
                                        CPU_BOOLEAN     *p_session_active,
                                        void            *p_out_opt,
                                        TELNET_OUT_FNCT out_fnct,
                                        RTOS_ERR        *p_err);

//                                                                 -------------------- UTIL FNCTS --------------------
static TELNETs_OPT *TELNETs_GetOpt(TELNETs_SESSION *p_session,
                                   CPU_INT08U      opt_code);

//                                                                 ------------------ SHELL OUT FNCT ------------------
static CPU_INT16S TELNETs_OutFnct(CPU_CHAR   *p_buf,
                                  CPU_INT16U buf_len,
                                  void       *p_opt);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           TELNETs_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           Telnet server module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before TELNETs_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void TELNETs_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((TELNETs_InitActive != DEF_INACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  TELNETs_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               TELNETs_Init()
 *
 * @brief    Initializes the TELNET server.
 *
 * @param    p_cfg               Pointer to the TELNET module configuration.
 *                               DEF_NULL, to use the default configuration TELNETs_CfgDflt.
 *
 * @param    p_secure_cfg        Desired value for server secure mode :
 *                               Secure Configuration Pointer  Server operations will be secured.
 *                               DEF_NULL                      Server operations will NOT be secured.
 *
 * @param    p_server_task_cfg   Pointer to the TELNET server task configuration.
 *                               DEF_NULL, to use the default configuration TELNETs_TaskCfgDflt.
 *
 * @param    p_session_task_cfg  Pointer to the session task configuration.
 *                               DEF_NULL, to use the default configuration TELNETs_SessionTaskCfgDflt.
 *
 * @param    p_mem_seg           Memory segment from which internal data will be allocated.
 *                               If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *
 * @note     (1) TELNETs_Init() MUST be called only AFTER the product's OS and network have
 *               been initialized.
 *
 * @note     (2) TELNETs_Init() MUST ONLY be called ONCE from the product's application.
 *
 * @note     (3) Network security manager MUST be available and enabled to initialize the server in
 *               secure mode.
 *******************************************************************************************************/
void TELNETs_Init(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  if (TELNETs_InitActive == DEF_INACTIVE) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();

  TELNETs_InstancesPoolPtr = (MEM_DYN_POOL *)Mem_SegAlloc("TELNETs Instances Pool Object",
                                                          TELNETs_InitCfg.MemSegPtr,
                                                          sizeof(MEM_DYN_POOL),
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(TELNETs_InstancesPoolPtr, sizeof(MEM_DYN_POOL));

  Mem_DynPoolCreate("TELNETs Instances Pool",
                    TELNETs_InstancesPoolPtr,
                    TELNETs_InitCfg.MemSegPtr,
                    sizeof(TELNETs_INSTANCE),
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  TELNETs_InitActive = DEF_INACTIVE;                            // Block telnet-s fncts/tasks until init complete.
  CORE_EXIT_ATOMIC();

exit:
  return;
}

/****************************************************************************************************//**
 *                                           TELNETs_InstanceInit()
 *
 * @brief    Initializes a TELNET server instance.
 *
 * @param    p_cfg               Pointer to the instance configuration object.
 *
 * @param    p_task_cfg          Pointer to the instance task server  configuration object.
 *
 * @param    p_task_session_cfg  Pointer to the instance task session configuration object.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_ALLOC
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_INIT
 *
 * @return   Pointer to the instance handler, if NO error(s).
 *           NULL pointer, otherwise.
 *******************************************************************************************************/
TELNETs_INSTANCE *TELNETs_InstanceInit(const TELNETs_CFG   *p_cfg,
                                       const RTOS_TASK_CFG *p_task_srv_cfg,
                                       const RTOS_TASK_CFG *p_task_session_cfg,
                                       RTOS_ERR            *p_err)
{
  TELNETs_INSTANCE    *p_instance = DEF_NULL;
  const TELNETs_CFG   *p_cfg_valid;
  const RTOS_TASK_CFG *p_task_srv_cfg_valid;
  const RTOS_TASK_CFG *p_task_session_cfg_valid;
  CPU_BOOLEAN         init_active;

  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  init_active = TELNETs_InitActive;
  CORE_EXIT_ATOMIC();

  if (init_active == DEF_ACTIVE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  if (p_cfg == DEF_NULL) {
    p_cfg_valid = &TELNETs_CfgDflt;
  } else {
    p_cfg_valid = p_cfg;
  }

  if (p_task_srv_cfg == DEF_NULL) {
    p_task_srv_cfg_valid = &TELNETs_TaskSrvCfgDflt;
  } else {
    p_task_srv_cfg_valid = p_task_srv_cfg;
  }

  if (p_task_session_cfg == DEF_NULL) {
    p_task_session_cfg_valid = &TELNETs_TaskSessionCfgDflt;
  } else {
    p_task_session_cfg_valid = p_task_session_cfg;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  switch (p_cfg_valid->IP_Type) {
    case TELNETs_IP_TYPE_IPv4:
#ifndef   NET_IPv4_MODULE_EN
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
      return (DEF_NULL);
#else
      break;
#endif // NET_IPv4_MODULE_EN
    case TELNETs_IP_TYPE_IPv6:
#ifndef   NET_IPv6_MODULE_EN
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
      return (DEF_NULL);
#else
      break;
#endif
    case TELNETs_IP_TYPE_IPv4_IPv6:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NULL);
    //                                                             Fallthrough

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  if (p_cfg_valid->SecureCfgPtr != DEF_NULL) {
#ifndef  NET_SECURE_MODULE_EN
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif
  }
#endif

  p_instance = TELNETs_MemInstanceAlloc(p_cfg_valid, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_instance->CfgPtr = p_cfg_valid;
  p_instance->TaskSrvCfgPtr = p_task_srv_cfg_valid;
  p_instance->TaskSessionCfgPtr = p_task_session_cfg_valid;

  TELNETs_MemTaskAlloc(p_instance, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (p_instance);
}

/****************************************************************************************************//**
 *                                           TELNETs_InstanceStart()
 *
 * @brief    Starts a specific TELNET server instance which had been previously initialized.
 *
 * @param    p_instance  Pointer to specific TELNET server instance handler.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_INVALID_TYPE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_ALREADY_EXISTS
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_NET_INVALID_CONN
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *******************************************************************************************************/
void TELNETs_InstanceStart(TELNETs_INSTANCE *p_instance,
                           RTOS_ERR         *p_err)
{
  const TELNETs_CFG *p_cfg;
  NET_SOCK_ID       sock_id_listen;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR ipv4_addr;
#endif
  CPU_INT08U               *p_addr = DEF_NULL;
  CPU_INT08U               addr_len = 0;
  NET_SOCK_PROTOCOL_FAMILY addr_family = NET_SOCK_PROTOCOL_FAMILY_NONE;
  NET_SOCK_ADDR            addr_server;
  NET_SOCK_ADDR_LEN        addr_server_size;
  CPU_BOOLEAN              close_sock = DEF_NO;
  RTOS_ERR                 local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  switch (p_instance->CfgPtr->IP_Type) {
#ifdef   NET_IPv4_MODULE_EN
    case TELNETs_IP_TYPE_IPv4:
      addr_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      break;
#endif // NET_IPv4_MODULE_EN

#ifdef   NET_IPv6_MODULE_EN
    case TELNETs_IP_TYPE_IPv6:
      addr_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      break;
#endif // NET_IPv6_MODULE_EN

    case TELNETs_IP_TYPE_IPv4_IPv6:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED,; );
    //                                                             Fallthrough

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
  //                                                               -------- PREPARE SOCKET & LISTEN FOR CLIENTS -------
  //                                                               Open a sock.
  sock_id_listen = NetSock_Open(addr_family,
                                NET_SOCK_TYPE_STREAM,
                                NET_SOCK_PROTOCOL_TCP,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  close_sock = DEF_YES;
  p_cfg = p_instance->CfgPtr;

#ifdef  NET_SECURE_MODULE_EN                                    // Set or clear socket secure mode.
  if (p_cfg->SecureCfgPtr != DEF_NULL) {
    (void)NetSock_CfgSecure(sock_id_listen,
                            DEF_YES,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("TELNETs NetSock_Open() failed: No secure socket available."));
      goto exit;
    }

    (void)NetSock_CfgSecureServerCertKeyInstall(sock_id_listen,
                                                p_cfg->SecureCfgPtr->CertPtr,
                                                p_cfg->SecureCfgPtr->CertLen,
                                                p_cfg->SecureCfgPtr->KeyPtr,
                                                p_cfg->SecureCfgPtr->KeyLen,
                                                p_cfg->SecureCfgPtr->Fmt,
                                                p_cfg->SecureCfgPtr->CertChain,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("TELNETs NetSock_Open() failed: No secure socket available."));
      goto exit;
    }
  }
#endif

  //                                                               Set Sock Cfg to Block mode.
  NetSock_CfgBlock(sock_id_listen,
                   NET_SOCK_BLOCK_SEL_BLOCK,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Set(&addr_server, (CPU_CHAR)0, NET_SOCK_ADDR_SIZE);       // Bind a local address so the client can send to us.

  switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      ipv4_addr = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_ANY);
      p_addr = (CPU_INT08U *)&ipv4_addr;
      addr_len = NET_IPv4_ADDR_SIZE;
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
      p_addr = (CPU_INT08U *)&NetIPv6_AddrAny;
      addr_len = NET_IPv6_ADDR_SIZE;
      break;
#endif

    default:
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetSock_Close(sock_id_listen, &local_err);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }

  NetApp_SetSockAddr(&addr_server,
                     addr_family,
                     p_cfg->Port,
                     p_addr,
                     addr_len,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  addr_server_size = NET_SOCK_ADDR_SIZE;
  //                                                               Bind to local addr and TELNETs port.
  NetSock_Bind(sock_id_listen,
               &addr_server,
               addr_server_size,
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               Listen for clients.
  NetSock_Listen(sock_id_listen,
                 TELNETs_CONN_Q_SIZE,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_instance->SockID = sock_id_listen;

  KAL_TaskCreate(p_instance->TaskSrvHandle,
                 TELNETs_TaskSrvHandler,
                 p_instance,
                 p_instance->TaskSrvCfgPtr->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  close_sock = DEF_NO;

exit:
  if (close_sock == DEF_YES) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(sock_id_listen, &local_err);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               TELNETs_MemInit()
 *
 * @brief    Initialize the memory object needed for Telnet module.
 *
 * @param    p_cfg           Pointer to the Telnet module configuration.
 *
 * @param    p_secure_cfg    Desired value for server secure mode :
 *                           Secure Configuration Pointer    Server operations will be secured.
 *                           DEF_NULL                        Server operations will NOT be secured.
 *
 * @param    p_mem_seg       Memory segment from which internal data will be allocated.
 *                           If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to Telnet server instance, if not error.
 *           NULL pointer, otherwise.
 *******************************************************************************************************/
static TELNETs_INSTANCE *TELNETs_MemInstanceAlloc(const TELNETs_CFG *p_cfg,
                                                  RTOS_ERR          *p_err)
{
  TELNETs_INSTANCE *p_instance;

  p_instance = Mem_DynPoolBlkGet(TELNETs_InstancesPoolPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SList_Init(&p_instance->SessionListPtr);

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->SessionPool,
                    TELNETs_InitCfg.MemSegPtr,
                    sizeof(TELNETs_SESSION),
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->RxBufPool,
                    TELNETs_InitCfg.MemSegPtr,
                    p_cfg->RxBufLen,
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->NVT_BufPool,
                    TELNETs_InitCfg.MemSegPtr,
                    (p_cfg->NVT_BufLen + TELNETs_EOL_STR_LEN + 1),
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->WorkingDirPool,
                    TELNETs_InitCfg.MemSegPtr,
                    p_cfg->FS_PathLenMax,
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->UserNamePool,
                    TELNETs_InitCfg.MemSegPtr,
                    p_cfg->UsernameStrLenMax,
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  Mem_DynPoolCreate("Session Pool",
                    &p_instance->PasswordPool,
                    TELNETs_InitCfg.MemSegPtr,
                    p_cfg->PasswordStrLenMax,
                    sizeof(CPU_ALIGN),
                    TELNETs_SESSION_MAX_NBR,
                    TELNETs_SESSION_MAX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

exit:
  return (p_instance);
}

/****************************************************************************************************//**
 *                                           TELNETs_TaskInit()
 *
 * @brief    (1) Perform TELNET server task initialization :
 *
 *           - (a) Create TELNET server task
 *
 *
 * Argument(s) : p_err   Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void TELNETs_MemTaskAlloc(TELNETs_INSTANCE *p_instance,
                                 RTOS_ERR         *p_err)
{
  const RTOS_TASK_CFG *p_cfg;

  p_cfg = p_instance->TaskSrvCfgPtr;
  p_instance->TaskSrvHandle = KAL_TaskAlloc(TELNETs_TASK_SERVER_NAME,
                                            p_cfg->StkPtr,
                                            p_cfg->StkSizeElements,
                                            DEF_NULL,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

  p_cfg = p_instance->TaskSessionCfgPtr;
  p_instance->TaskSessionHandle = KAL_TaskAlloc(TELNETs_TASK_SESSION_NAME,
                                                p_cfg->StkPtr,
                                                p_cfg->StkSizeElements,
                                                DEF_NULL,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Telnet server memory initialization failed."));
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       TELNETs_TaskServerHandler()
 *
 * @brief    (1) Main TELNET server code :
 *
 *           - (a) Prepare socket and listen for clients
 *           - (b) Accept incoming connections
 *           - (c) Process connection
 *
 *
 * Argument(s) : p_arg   Argument passed to the task.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) On fatal error, close the server socket, break accept loop and re-open listen
 *                   (server) socket.
 *
 * @note     (3) If all available sessions are in use, reply to the client that the service is not
 *                   currently available and close the session socket.
 *******************************************************************************************************/
static void TELNETs_TaskSrvHandler(void *p_arg)
{
  TELNETs_INSTANCE  *p_instance;
  NET_SOCK_ID       sock_id_session;
  NET_SOCK_ADDR     addr_client;
  NET_SOCK_ADDR_LEN addr_client_size;
  CPU_INT16U        msg_len;
  CPU_BOOLEAN       flag = DEF_DISABLED;
  RTOS_ERR          local_err;

  p_instance = (TELNETs_INSTANCE *)p_arg;

  while (DEF_ON) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    //                                                             ---------------- ACCEPT INCOMING CONN --------------
    addr_client_size = sizeof(addr_client);

    //                                                             Accept conn.
    sock_id_session = NetSock_Accept(p_instance->SockID,
                                     &addr_client,
                                     &addr_client_size,
                                     &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        NetSock_OptSet(sock_id_session,
                       NET_SOCK_PROTOCOL_TCP,
                       NET_SOCK_OPT_TCP_NO_DELAY,
                       &flag,
                       sizeof(flag),
                       &local_err);
        break;

      case RTOS_ERR_NOT_INIT:
      case RTOS_ERR_NULL_PTR:
      case RTOS_ERR_POOL_EMPTY:
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_WOULD_BLOCK:
        continue;                                           // Ignore transitory sock err.

      default:
        break;
    }

    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {    // See Note #2.
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetSock_Close(sock_id_session, &local_err);
      break;
    }

    //                                                             -------------------- PROCESS CONN ------------------
    //                                                             See Note #3.
    if (p_instance->NbrActiveSession >= TELNETs_SESSION_MAX_NBR) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      msg_len = Str_Len((CPU_CHAR *)TELNETs_NO_SERVICE_STR);
      TELNETs_Tx(sock_id_session,
                 TELNETs_NO_SERVICE_STR,
                 msg_len,
                 p_instance->CfgPtr->TxTriesMaxNbr,
                 &local_err);

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      NetSock_Close(sock_id_session, &local_err);
      continue;
    }

    TELNETs_TaskSessionStart(p_instance, sock_id_session, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      msg_len = Str_Len((CPU_CHAR *)TELNETs_NO_SERVICE_STR);
      TELNETs_Tx(sock_id_session,
                 TELNETs_NO_SERVICE_STR,
                 msg_len,
                 p_instance->CfgPtr->TxTriesMaxNbr,
                 &local_err);

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      NetSock_Close(sock_id_session, &local_err);
    } else {
      p_instance->NbrActiveSession++;
      //                                                          TELNETs_NbrActiveSessionTask++;
    }
  }
}

/****************************************************************************************************//**
 *                                       TELNETs_TaskSessionStart()
 *
 * @brief    (1) Perform TELNET server/OS session task initialization :
 *
 *           - (a) Create TELNET server session task
 *
 * @param    p_instance          Pointer to the Telnet server instance.
 *
 * @param    sock_id_session     Socket ID for the Telnet session.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TELNETs_TaskSessionStart(TELNETs_INSTANCE *p_instance,
                                     NET_SOCK_ID      sock_id_session,
                                     RTOS_ERR         *p_err)
{
  TELNETs_SESSION *p_session;

  p_session = (TELNETs_SESSION *)Mem_DynPoolBlkGet(&p_instance->SessionPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->RxBufPtr = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_instance->RxBufPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_session;
  }

  p_session->NVT_BufPtr = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_instance->NVT_BufPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_rx_buf;
  }

  p_session->UserNameStrPtr = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_instance->UserNamePool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_nvt_buf;
  }

  p_session->PasswordStrPtr = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_instance->PasswordPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_username;
  }

#ifdef  RTOS_MODULE_FS_AVAIL
  p_session->CurWorkingDirPtr = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_instance->WorkingDirPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_password;
  }
#endif

  p_session->CfgPtr = p_instance->CfgPtr;
  p_session->SockID = sock_id_session;

  SList_Push(&p_instance->SessionListPtr, &p_session->ListNode);

  KAL_TaskCreate(p_instance->TaskSessionHandle,
                 TELNETs_TaskSessionHandler,
                 p_instance,
                 p_instance->TaskSessionCfgPtr->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_working_dir;
  }

  return;

exit_release_working_dir:
#ifdef  RTOS_MODULE_FS_AVAIL
  if (p_session->CurWorkingDirPtr != DEF_NULL) {
    Mem_DynPoolBlkFree(&p_instance->WorkingDirPool, p_session->CurWorkingDirPtr, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

#ifdef  RTOS_MODULE_FS_AVAIL
exit_release_password:
  Mem_DynPoolBlkFree(&p_instance->PasswordPool, p_session->PasswordStrPtr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
#endif

exit_release_username:
  Mem_DynPoolBlkFree(&p_instance->UserNamePool, p_session->UserNameStrPtr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit_release_nvt_buf:
  Mem_DynPoolBlkFree(&p_instance->NVT_BufPool, p_session->NVT_BufPtr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit_release_rx_buf:
  Mem_DynPoolBlkFree(&p_instance->RxBufPool, p_session->RxBufPtr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit_release_session:
  Mem_DynPoolBlkFree(&p_instance->SessionPool, p_session, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit:
  return;
}

/****************************************************************************************************//**
 *                                       TELNETs_TaskSessionHandler()
 *
 * Description : (1) Main TELNET session code :
 *
 *                   (a) Initialize NVT
 *                   (b) Receive data from client
 *                   (c) Process received data
 *                   (d) Terminate session when needed
 *
 * Argument(s) : p_arg       Argument passed to the task.
 *
 * Note(s)     : (1) If TELNETs_CmdProcess() returns TELNETs_ERR_CMD_EXEC, meaning there was an error
 *                   while executing command, NO error message is transmitted by the session task.  It is
 *                   the command responsibility to output such error to the client.
 *******************************************************************************************************/
static void TELNETs_TaskSessionHandler(void *p_arg)
{
  TELNETs_INSTANCE *p_instance;
  TELNETs_SESSION  *p_session;
  SLIST_MEMBER     *p_list_node;
  CPU_BOOLEAN      init_done;
  CPU_INT16U       tx_str_len;
  RTOS_ERR         local_err;

  p_instance = (TELNETs_INSTANCE *)p_arg;
  p_list_node = SList_Pop(&p_instance->SessionListPtr);
  p_session = SLIST_ENTRY(p_list_node, TELNETs_SESSION, ListNode);
  p_session->IsActive = DEF_NO;

  //                                                               ---------------------- INIT NVT --------------------
  init_done = TELNETs_NVTInit(p_instance, p_session, &local_err);
  if (init_done == DEF_OK) {
    p_session->IsActive = DEF_YES;
#ifdef  RTOS_MODULE_FS_AVAIL
    Str_Copy(p_session->CurWorkingDirPtr, (CPU_CHAR *)"\\");
#endif
  }

  while (p_session->IsActive == DEF_YES) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    //                                                             ---------------- RX DATA FROM CLIENT ---------------
    TELNETs_RxSessionData(p_instance, p_session, DEF_YES, &local_err);

    //                                                             ----------------- PROCESS RX'D DATA ----------------
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        TELNETs_NVTPrint(p_instance, p_session, DEF_YES, &local_err);

        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_RX) {          // If EOL received ...
          //                                                       ... parse and invoke user fnct.
          if (p_session->NVT_BufLen > TELNETs_EOL_STR_LEN) {
            //                                                     Rem EOL.
            p_session->NVT_BufPtr[p_session->NVT_BufLen - 2] = (CPU_CHAR)0;
            p_session->NVT_BufLen = p_session->NVT_BufLen - 2;

            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

            TELNETs_Cmd(p_session->NVT_BufPtr,
                        p_session,
                        &local_err);

            switch (RTOS_ERR_CODE_GET(local_err)) {
              case RTOS_ERR_NONE:                               // No err ...
                break;                                          // ... nothing to do.

              case RTOS_ERR_SHELL_CMD_EXEC:                     // Err processing cmd ...
                                                                // ... tx err msg.
                RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

                tx_str_len = Str_Len((CPU_CHAR *)TELNETs_CMD_PROCESS_ERR_STR);
                TELNETs_Tx(p_session->SockID,
                           TELNETs_CMD_PROCESS_ERR_STR,
                           tx_str_len,
                           p_instance->CfgPtr->TxTriesMaxNbr,
                           &local_err);
                if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
                  p_session->IsActive = DEF_NO;
                }
                break;

              default:
                break;
            }
          }

          p_session->NVT_BufLen = 0;
          //                                                       Tx cmd prompt and GA.
          if (p_session->IsActive == DEF_YES) {
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            TELNETs_NVTTxPrompt(p_instance, p_session, &local_err);
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            TELNETs_TxGA(p_session, &local_err);
          }
        }
        break;

      default:
        p_session->IsActive = DEF_NO;
        break;
    }
  }

  //                                                               ----------------- TERMINATE SESSION ----------------
  LOG_DBG(("Telnet server closing session socket."));
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetSock_Close(p_session->SockID, &local_err);

  TELNETs_NVTTerminate(p_instance, p_session);

  LOG_DBG(("Telnet server deleting session task."));

  p_instance->NbrActiveSession--;                               // TODO_NET : this variable should be protected: can be
                                                                // accessed from 2 task, not a real problem until many
                                                                // simultaneous session can be opened.
  KAL_TaskDel(p_instance->TaskSessionHandle);
}

/****************************************************************************************************//**
 *                                           TELNETs_RxSessionData()
 *
 * @brief    Receive data from Telnet session.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    echo_en     Enables the echo mode.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK          Reception successful.
 *           DEF_FAIL        Reception failed.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_RxSessionData(TELNETs_INSTANCE *p_instance,
                                         TELNETs_SESSION  *p_session,
                                         CPU_BOOLEAN      echo_en,
                                         RTOS_ERR         *p_err)
{
  CPU_INT32S rx_data_len;
  RTOS_ERR   local_err;

  //                                                               ---------------------- RX DATA ---------------------
  rx_data_len = TELNETs_Rx(p_session->SockID,
                           p_session->RxBufPtr + p_session->RxBufLen,
                           p_instance->CfgPtr->RxBufLen - p_session->RxBufLen,
                           p_instance->CfgPtr->ConnInactivityTimeout_s,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if (p_instance->CfgPtr->EchoEn == DEF_ENABLED) {
    if (echo_en == DEF_YES) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      TELNETs_Tx(p_session->SockID,
                 p_session->RxBufPtr + p_session->RxBufLen,
                 rx_data_len,
                 p_instance->CfgPtr->TxTriesMaxNbr,
                 &local_err);
      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
        p_session->IsActive = DEF_NO;
      }
    }
  }

  PP_UNUSED_PARAM(echo_en);

  p_session->RxBufLen += rx_data_len;                         // Inc rx buf len.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           TELNETs_RxOptHandler()
 *
 * @brief    Receive option request or reply :
 *
 *       - (a) Get current option status, if any
 *       - (b) If option supported, determine if it is a reply
 *       - (c) Process option
 *
 * @param    p_session       Pointer to session structure.
 *
 * @param    opt_cmd_rx      Option status command received.
 *
 * @param    opt_code_rx     Option code received.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TELNETs_RxOptHandler(TELNETs_SESSION        *p_session,
                                 TELNETs_OPT_STATUS_CMD opt_cmd_rx,
                                 CPU_INT08U             opt_code_rx,
                                 RTOS_ERR               *p_err)
{
  TELNETs_OPT            *p_opt_cur;
  TELNETs_OPT_STATUS_CMD int_opt_status;
  CPU_BOOLEAN            is_opt_rep;
  TELNETs_OPT_STATUS_CMD *p_status;
  CPU_BOOLEAN            *p_status_req_tx;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  is_opt_rep = DEF_NO;

  //                                                               ---------------- GET CUR OPT STATUS ----------------
  p_opt_cur = TELNETs_GetOpt(p_session, opt_code_rx);

  //                                                               ------------- DETERMINE IF OPT IS A REP ------------
  if (p_opt_cur != DEF_NULL) {
    switch (opt_cmd_rx) {
      case TELNETs_OPT_STATUS_CMD_WILL:                         // Client-side (peer host) opt.
      case TELNETs_OPT_STATUS_CMD_WONT:
        if (p_opt_cur->client_status_req_tx == DEF_YES) {
          is_opt_rep = DEF_YES;
          p_status = &p_opt_cur->client_status;
          p_status_req_tx = &p_opt_cur->client_status_req_tx;

          int_opt_status = opt_cmd_rx == TELNETs_OPT_STATUS_CMD_WILL ? TELNETs_OPT_STATUS_CMD_DO
                           : TELNETs_OPT_STATUS_CMD_DONT;
        }
        break;

      case TELNETs_OPT_STATUS_CMD_DO:                           // Server-side (this host) opt.
      case TELNETs_OPT_STATUS_CMD_DONT:
        if (p_opt_cur->server_status_req_tx == DEF_YES) {
          is_opt_rep = DEF_YES;
          p_status = &p_opt_cur->server_status;
          p_status_req_tx = &p_opt_cur->server_status_req_tx;

          int_opt_status = opt_cmd_rx == TELNETs_OPT_STATUS_CMD_DO ? TELNETs_OPT_STATUS_CMD_WILL
                           : TELNETs_OPT_STATUS_CMD_WONT;
        }
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        break;
    }
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // Rtn if opt status unknown.
    return;
  }

  //                                                               -------------------- PROCESS OPT -------------------
  if (is_opt_rep == DEF_YES) {                                  // If opt is a rep ...
    if (*p_status == int_opt_status) {                          // If current status identical to rx'd one ...
      TELNETs_TxCmd(p_session,                                  // ... req refused, tx ack.
                    int_opt_status,
                    opt_code_rx,
                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TX) {
        p_session->IsActive = DEF_NO;
      }
    } else {                                                    // Else ...
      *p_status = int_opt_status;                               // ... req accepted.
    }

    *p_status_req_tx = DEF_NO;                                  // Req serviced, unset flag.
  } else {                                                      // Else ...
    TELNETs_TxOptRep(p_session,                                 // ... opt is a req, tx rep.
                     opt_cmd_rx,
                     opt_code_rx,
                     p_opt_cur,
                     p_err);
  }
}

/****************************************************************************************************//**
 *                                               TELNETs_Rx()
 *
 * @brief    (1) Receive data from socket :
 *
 *           - (a) Configure receive timeout value
 *           - (b) Receive data
 *
 * @param    sock_id         Session socket id.
 *
 * @param    pdata_buf       Pointer to data buffer that will receive client data.
 *
 * @param    data_buf_len    Size of the data buffer (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of positive data octets received, if NO errors.
 *
 *           NET_SOCK_BSD_RTN_CODE_CONN_CLOSED (0),   if socket connection closed.
 *
 *           NET_SOCK_BSD_ERR_RX (-1),                otherwise.
 *
 * @note     (2) The receive timeout value is configured using the TELNETs_CFG_INACTIVITY_TIMEOUT_S
 *               configuration variable.
 *
 * @note     (3) At this point, the function will return when either:
 *
 *           - (a) data is received from the client
 *           - (b) the connection is closed.
 *           - (c) the receive timeout expired
 *******************************************************************************************************/
static CPU_INT32S TELNETs_Rx(NET_SOCK_ID sock_id,
                             CPU_CHAR    *p_data_buf,
                             CPU_INT16U  data_buf_len,
                             CPU_INT32U  timeout_sec,
                             RTOS_ERR    *p_err)
{
  CPU_INT32S rx_data_len;

  //                                                               ------------------ SET RX TIMEOUT ------------------
  //                                                               See Note #2.
  NetSock_CfgTimeoutRxQ_Set(sock_id,
                            (timeout_sec * DEF_TIME_NBR_mS_PER_SEC),
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_SOCK_BSD_ERR_RX);
  }

  //                                                               ---------------------- RX DATA ---------------------
  //                                                               See Note #3.
  rx_data_len = NetSock_RxData(sock_id,
                               p_data_buf,
                               data_buf_len,
                               NET_SOCK_FLAG_NONE,
                               p_err);

  return (rx_data_len);
}

/****************************************************************************************************//**
 *                                           TELNETs_TxOptReq()
 *
 * @brief    (1) Transmit option request.
 *
 *           - (a) Get current option status structure
 *           - (b) Get current option status
 *           - (c) Validate request
 *           - (d) Transmit request
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    opt_status  Option status command for the request.
 *
 * @param    opt_code    Option code           for the request.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TELNETs_TxOptReq(TELNETs_SESSION        *p_session,
                             TELNETs_OPT_STATUS_CMD opt_status,
                             CPU_INT08U             opt_code,
                             RTOS_ERR               *p_err)
{
  TELNETs_OPT            *p_opt_cur;
  TELNETs_OPT_STATUS_CMD *p_status;
  CPU_BOOLEAN            *p_req_tx;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ GET CUR OPT STATUS STRUCT -------------
  p_opt_cur = TELNETs_GetOpt(p_session, opt_code);

  if (p_opt_cur == DEF_NULL) {                                  // Rtn if opt not supported.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  //                                                               ---------------- GET CUR OPT STATUS ----------------
  switch (opt_status) {
    case TELNETs_OPT_STATUS_CMD_DO:                             // Client-side (peer host) opt.
    case TELNETs_OPT_STATUS_CMD_DONT:
      p_status = &p_opt_cur->client_status;
      p_req_tx = &p_opt_cur->client_status_req_tx;
      break;

    case TELNETs_OPT_STATUS_CMD_WILL:                           // Server-side (this host) opt.
    case TELNETs_OPT_STATUS_CMD_WONT:
      p_status = &p_opt_cur->server_status;
      p_req_tx = &p_opt_cur->server_status_req_tx;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // Rtn if opt status unknown.
    return;
  }

  //                                                               ------------------- VALIDATE REQ -------------------
  if (opt_status == *p_status) {                                // If req'd opt status already set ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // ... no not tx req and rtn.
    return;
  }

  //                                                               ---------------------- TX REQ ----------------------
  TELNETs_TxCmd(p_session,
                opt_status,
                opt_code,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    *p_req_tx = DEF_YES;                                        // Set req_tx flag so reply are identified.
  } else if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TX) {
    p_session->IsActive = DEF_NO;
  }
}

/****************************************************************************************************//**
 *                                           TELNETs_TxOptRep()
 *
 * @brief    (1) Transmit option reply and set current option accordingly :
 *
 *           - (a) Validate    option request and set reply
 *           - (b) Transmit    option reply
 *           - (c) Set current option status
 *
 * @param    p_session       Pointer to session structure.
 *
 * @param    opt_status_req  Option reply status command.
 *
 * @param    opt_code        Option reply code.
 *
 * @param    p_opt_cur       Pointer to current option status.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TELNETs_TxOptRep(TELNETs_SESSION        *p_session,
                             TELNETs_OPT_STATUS_CMD opt_status_req,
                             CPU_INT08U             opt_code,
                             TELNETs_OPT            *p_opt_cur,
                             RTOS_ERR               *p_err)
{
  TELNETs_OPT_STATUS_CMD opt_status;
  TELNETs_OPT_STATUS_CMD *p_opt_status;
  CPU_INT08U             opt_code_rep;
  CPU_INT08U             opt_status_rep;

  opt_code_rep = opt_code;
  opt_status = TELNETs_OPT_STATUS_CMD_DONT;
  opt_status_rep = TELNETs_OPT_STATUS_CMD_DONT;
  p_opt_status = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- VALIDATE OPT REQ & SET REP -----------
  if (p_opt_cur != DEF_NULL) {                                  // If popt_cur not NULL ...
    switch (opt_status_req) {                                   // ... opt is supported, treat it.
      case TELNETs_OPT_STATUS_CMD_WILL:                         // Client-side (peer host) opt.
      case TELNETs_OPT_STATUS_CMD_WONT:
        opt_status = opt_status_req == TELNETs_OPT_STATUS_CMD_WILL ? TELNETs_OPT_STATUS_CMD_DO
                     : TELNETs_OPT_STATUS_CMD_DONT;
        if (opt_status != p_opt_cur->client_status) {
          p_opt_status = &p_opt_cur->client_status;
          opt_status_rep = opt_status;
        } else {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        }
        break;

      case TELNETs_OPT_STATUS_CMD_DO:                           // Server-side (this host) opt.
      case TELNETs_OPT_STATUS_CMD_DONT:
        opt_status = opt_status_req == TELNETs_OPT_STATUS_CMD_DO ? TELNETs_OPT_STATUS_CMD_WILL
                     : TELNETs_OPT_STATUS_CMD_WONT;
        if (opt_status != p_opt_cur->server_status) {
          p_opt_status = &p_opt_cur->server_status;
          opt_status_rep = opt_status;
        } else {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        }
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        break;
    }
  } else {                                                      // Else ...
    switch (opt_status_req) {                                   // ... opt is NOT supported, refuse it.
      case TELNETs_OPT_STATUS_CMD_WILL:
        opt_status_rep = TELNETs_OPT_STATUS_CMD_DONT;
        break;

      case TELNETs_OPT_STATUS_CMD_DO:
        opt_status_rep = TELNETs_OPT_STATUS_CMD_WONT;
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        break;
    }
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------------- TX OPT REP --------------------
  TELNETs_TxCmd(p_session,
                opt_status_rep,
                opt_code_rep,
                p_err);

  //                                                               ---------------- SET CUR OPT STATUS ----------------
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    if (p_opt_status != DEF_NULL) {                             // If  ptr not NULL ...
      *p_opt_status = opt_status;                               // ... set the ptr value.
    }
  } else if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TX) {
    p_session->IsActive = DEF_NO;
  }
}

/****************************************************************************************************//**
 *                                               TELNETs_TxGA()
 *
 * @brief    Transmit Go Ahead, if SUPPRESS_GA not enabled.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, Go Ahead transmitted (or attempted).
 *           DEF_NO,  otherwise.
 *
 * @note     (1) Returning 'DEF_YES' does not guarantee that a Go Ahead has been transmitted.  Check
 *               the variable receiving the return error code to make sure the transmission was
 *               completed.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_TxGA(TELNETs_SESSION *p_session,
                                RTOS_ERR        *p_err)
{
  TELNETs_OPT *p_opt;

  p_opt = TELNETs_GetOpt(p_session, TELNET_OPT_SUPPRESS_GA);

  if (p_opt != DEF_NULL) {
    if (p_opt->server_status == TELNETs_OPT_STATUS_CMD_WILL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return (DEF_NO);
    }
  }

  TELNETs_TxCmd(p_session,
                TELNETs_OPT_STATUS_CMD_GA,
                TELNET_OPT_NO_OPT,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TX) {
    p_session->IsActive = DEF_NO;
  }

  return (DEF_YES);                                             // See Note #1.
}

/****************************************************************************************************//**
 *                                               TELNETs_TxCmd()
 *
 * @brief    Transmit command
 *
 * @param    sock_id     Session socket id.
 *
 * @param    cmd_code    Command code.
 *
 * @param    opt_code    Optional option code.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK          Transmission successful.
 *           DEF_FAIL        Transmission failed.
 *
 * @note     (1) If a stand-alone command is to be sent (by opposition to an option command), the
 *               opt_code parameter SHOULD be passed TELNET_NO_OPT.  Indeed, when the cmd_code is not
 *               one of these:
 *
 *               - (a) TELNET_CMD_WILL
 *               - (b) TELNET_CMD_WONT
 *               - (c) TELNET_CMD_DO
 *               - (d) TELNET_CMD_DONT
 *
 *               the opt_code parameter is not taken into account.
 *
 * @note     (2) No command validation is performed by this function.  It is the caller's
 *               responsibility to make sure the specified command transmitted is valid and is
 *               supported.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_TxCmd(TELNETs_SESSION *p_session,
                                 CPU_INT08U      cmd_code,
                                 CPU_INT08U      opt_code,
                                 RTOS_ERR        *p_err)
{
  CPU_CHAR   opt_tx_buf[TELNETs_CMD_MAX_BUF_LEN];
  CPU_INT16U len;

  //                                                               Set IAC and cmd code.
  opt_tx_buf[TELNETs_CMD_IAC_OFFSET] = TELNETs_OPT_STATUS_CMD_IAC;
  opt_tx_buf[TELNETs_CMD_CMD_OFFSET] = cmd_code;

  switch (cmd_code) {
    case TELNETs_OPT_STATUS_CMD_WILL:
    case TELNETs_OPT_STATUS_CMD_WONT:
    case TELNETs_OPT_STATUS_CMD_DO:
    case TELNETs_OPT_STATUS_CMD_DONT:
      opt_tx_buf[TELNETs_CMD_OPT_OFFSET] = opt_code;            // Set opt code.
      len = TELNETs_CMD_BUF_LEN_WITH_OPT;
      break;

    default:
      len = TELNETs_CMD_BUF_LEN_NO_OPT;                         // No opt code.
      break;
  }

  TELNETs_Tx(p_session->SockID,
             opt_tx_buf,
             len,
             p_session->CfgPtr->TxTriesMaxNbr,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                               TELNETs_Tx()
 *
 * @brief    Transmit data to socket, handling transient errors and incomplete buffer transmit.
 *
 * @param    sock_id         Session socket id.
 *
 * @param    p_data_buf      Pointer to data buffer to send.
 *
 * @param    data_buf_len    Length of  data buffer to send.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK          Transmission successful.
 *           DEF_FAIL        Transmission failed.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_Tx(NET_SOCK_ID sock_id,
                              CPU_CHAR    *p_data_buf,
                              CPU_INT16U  data_buf_len,
                              CPU_INT08U  retry_max_nbr,
                              RTOS_ERR    *p_err)
{
  void        *p_tx_buf;
  CPU_INT16S  tx_buf_len;
  CPU_INT16S  tx_len;
  CPU_INT16S  tx_len_tot;
  CPU_INT08U  tx_retry_cnt;
  CPU_BOOLEAN tx_done;
  CPU_BOOLEAN tx_dly;
  RTOS_ERR    local_err;

  tx_len_tot = 0;
  tx_retry_cnt = 0;
  tx_done = DEF_NO;
  tx_dly = DEF_NO;

  while ((tx_len_tot < data_buf_len)                            // While tx tot len < data buf len ...
         && (tx_retry_cnt < retry_max_nbr)                      // ... & tx try     < MAX     ...
         && (tx_done == DEF_NO)) {                              // ... & tx NOT done;         ...
    if (tx_dly == DEF_YES) {                                    // Dly tx, if req'd.
      KAL_Dly(10);
    }

    p_tx_buf = p_data_buf   + tx_len_tot;
    tx_buf_len = data_buf_len - tx_len_tot;
    tx_len = NetSock_TxData(sock_id,                            // ... tx data.
                            p_tx_buf,
                            tx_buf_len,
                            NET_SOCK_FLAG_NONE,
                            p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if (tx_len > 0) {                                       // If          tx len > 0, ...
          tx_len_tot += tx_len;                                 // ... inc tot tx len.
          tx_dly = DEF_NO;
        } else {                                                // Else dly next tx.
          tx_dly = DEF_YES;
        }
        tx_retry_cnt = 0;
        break;

      case RTOS_ERR_NET_IF_LINK_DOWN:
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_POOL_EMPTY:
      case RTOS_ERR_WOULD_BLOCK:
        tx_dly = DEF_YES;                                       // ... dly next tx.
        tx_retry_cnt++;
        break;

      case RTOS_ERR_NET_CONN_CLOSED_FAULT:
      case RTOS_ERR_NET_CONN_CLOSE_RX:
        tx_done = DEF_YES;
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetSock_Close(sock_id, &local_err);
        break;

      default:
        tx_done = DEF_YES;
        break;
    }
  }

  if (tx_retry_cnt >= retry_max_nbr) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_RETRY_MAX);
    return (DEF_FAIL);
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    return (DEF_FAIL);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                               TELNETs_NVTInit()
 *
 * @brief    (1) Initialize Network Virtual Terminal (NVT) :
 *
 *           - (a) Initialize session structure
 *           - (b) Send system message
 *           - (c) Set mode
 *           - (d) Proceed with login
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK          Initialization successful.
 *           DEF_FAIL        Initialization failed.
 *
 * @note     (1) The server tries to operate in the character at a time mode, meaning that each
 *               character is separately transmitted and echoed by it.  For this purpose, both the
 *               echo and the suppress go ahead options are to be enabled by the server.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_NVTInit(TELNETs_INSTANCE *p_instance,
                                   TELNETs_SESSION  *p_session,
                                   RTOS_ERR         *p_err)
{
  CPU_BOOLEAN rtn_val;
  CPU_SIZE_T  sys_msg_str_len;
  CPU_INT16U  i;

  //                                                               ---------------- INIT SESSION STRUCT ---------------
  p_session->RxBufLen = 0;
  p_session->NVT_BufLen = 0;
  p_session->NVT_State = TELNETs_NVT_STATE_GRAPHIC;

  for (i = 0; i < TELNET_NBR_OPT_SUPPORTED; i++) {              // Set opt.
    p_session->Opt[i].code = TELNETs_SupportedOptTbl[i];
    p_session->Opt[i].server_status = TELNETs_OPT_STATUS_CMD_WONT;
    p_session->Opt[i].client_status = TELNETs_OPT_STATUS_CMD_DONT;
    p_session->Opt[i].server_status_req_tx = DEF_NO;
    p_session->Opt[i].client_status_req_tx = DEF_NO;
  }

  //                                                               --------------------- TX SYS MSG -------------------
  sys_msg_str_len = Str_Len((CPU_CHAR *)TELNETs_SYS_MSG_STR);

  rtn_val = TELNETs_Tx(p_session->SockID,
                       TELNETs_SYS_MSG_STR,
                       sys_msg_str_len,
                       p_session->CfgPtr->TxTriesMaxNbr,
                       p_err);
  if (rtn_val == DEF_FAIL) {
    return (DEF_FAIL);
  }

  //                                                               --------------------- SET MODE ---------------------
  //                                                               See Note #1.
  TELNETs_TxOptReq(p_session, TELNETs_OPT_STATUS_CMD_WILL, TELNET_OPT_ECHO, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               ----------------------- LOGIN ----------------------
  rtn_val = TELNETs_NVTLogin(p_instance, p_session, p_err);
  if (rtn_val == DEF_FAIL) {                                    // If error ...
    return (DEF_FAIL);                                          // ... let error message go through.
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           TELNETs_NVTLogin()
 *
 * @brief    (1) Process with user login on the system :
 *
 *           - (a) Request username
 *           - (b) Request password
 *           - (c) Validate credential
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK          Login successful.
 *           DEF_FAIL        Login failed.
 *******************************************************************************************************/
static CPU_BOOLEAN TELNETs_NVTLogin(TELNETs_INSTANCE *p_instance,
                                    TELNETs_SESSION  *p_session,
                                    RTOS_ERR         *p_err)
{
  CPU_BOOLEAN rtn_val;
  CPU_SIZE_T  tx_str_len;
  CPU_BOOLEAN logged;
  CPU_INT08U  login_retry_cnt;

  logged = DEF_FAIL;
  login_retry_cnt = 0;

  while ((logged == DEF_FAIL)                                        // While not logged in ...
         && (login_retry_cnt < p_instance->CfgPtr->LoginTriesMaxNbr)) {  // ... & login tries < MAX.
    //                                                             ------------------- REQ USERNAME -------------------
    tx_str_len = Str_Len((CPU_CHAR *)TELNETs_LOGIN_STR);

    rtn_val = TELNETs_Tx(p_session->SockID,                    // Tx login msg.
                         TELNETs_LOGIN_STR,
                         tx_str_len,
                         p_session->CfgPtr->TxTriesMaxNbr,
                         p_err);
    if (rtn_val == DEF_FAIL) {
      return (DEF_FAIL);
    }

    do {
      //                                                           Rx login name.
      rtn_val = TELNETs_RxSessionData(p_instance, p_session, DEF_YES, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_FAIL);
      }

      TELNETs_NVTPrint(p_instance, p_session, DEF_YES, p_err);
    } while (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_RX);

    //                                                             Get login from psession struct.
    TELNETs_NVTGetBuf(p_session, p_session->UserNameStrPtr, p_instance->CfgPtr->UsernameStrLenMax, DEF_YES, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

    //                                                             ---------------------- REQ PW ----------------------
    tx_str_len = Str_Len((CPU_CHAR *)TELNETs_PW_STR);

    rtn_val = TELNETs_Tx(p_session->SockID,                    // Tx pw msg.
                         TELNETs_PW_STR,
                         tx_str_len,
                         p_session->CfgPtr->TxTriesMaxNbr,
                         p_err);
    if (rtn_val == DEF_FAIL) {
      return (DEF_FAIL);
    }

    do {
      //                                                           Rx pw.
      rtn_val = TELNETs_RxSessionData(p_instance, p_session, DEF_NO, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_FAIL);
      }

      TELNETs_NVTPrint(p_instance, p_session, DEF_NO, p_err);
    } while (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_RX);

    TELNETs_Tx(p_session->SockID,
               TELNETs_EOL_STR,
               TELNETs_EOL_STR_LEN,
               p_session->CfgPtr->TxTriesMaxNbr,
               p_err);

    //                                                             Get pw from psession struct.
    TELNETs_NVTGetBuf(p_session, p_session->PasswordStrPtr, p_instance->CfgPtr->PasswordStrLenMax, DEF_YES, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

    //                                                             --------------- VALIDATE CREDENTIALS ---------------
    Auth_ValidateCredentials(p_session->UserNameStrPtr, p_session->PasswordStrPtr, p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:                                       // Success.
        logged = DEF_YES;
        break;

      case RTOS_ERR_INVALID_CREDENTIALS:                        // Client error.
        logged = DEF_NO;
        break;

      default:                                                  // Internal error.
        break;
    }

    if (logged == DEF_OK) {                                     // If logged ...
                                                                // ... tx welcome msg ...

      tx_str_len = Str_Len((CPU_CHAR *)p_instance->CfgPtr->WelcomeMsgStr);

      rtn_val = TELNETs_Tx(p_session->SockID,
                           p_instance->CfgPtr->WelcomeMsgStr,
                           tx_str_len,
                           p_session->CfgPtr->TxTriesMaxNbr,
                           p_err);
      if (rtn_val == DEF_FAIL) {
        return (DEF_FAIL);
      }

      TELNETs_NVTTxPrompt(p_instance, p_session, p_err);        // ... and tx cmd prompt.
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (DEF_FAIL);
      }
    } else {                                                    // Else dly and retry.
      KAL_Dly(TELNETs_FAILED_LOGIN_DLY_MS);
      login_retry_cnt++;
      //                                                           Tx login failure msg.
      tx_str_len = Str_Len((CPU_CHAR *)TELNETs_LOGIN_FAILURE_STR);

      rtn_val = TELNETs_Tx(p_session->SockID,
                           TELNETs_LOGIN_FAILURE_STR,
                           tx_str_len,
                           p_session->CfgPtr->TxTriesMaxNbr,
                           p_err);
      if (rtn_val == DEF_FAIL) {
        return (DEF_FAIL);
      }
    }
  }

  return (logged);
}

/****************************************************************************************************//**
 *                                           TELNETs_NVTPrint()
 *
 * @brief    Process received data from telnet session.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    echo        Whether or not 'echo' are allowed (see Note #3).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) The data received from the telnet session is parsed using a state machine consisting
 *                       of the following states:
 *
 *                           - (a) TELNETs_NVT_STATE_GRAPHIC
 *
 *                       In this state, graphic ASCII are being sent to the printer and other meaningful
 *                       code have the machine be switched into another state.  This is the state the
 *                       machine enters by default.
 *
 *                           - (1) If the NVT buffer is full while processing graphic codes, the last characters
 *                       are ignored until the EOL sequence is encounter.  That line is hence incomplete,
 *                       and its processing is most likely going to introduce an error.  It is the
 *                       developer's responsibility to ensure that TELNETs_CFG_NVT_BUF_LEN be defined with
 *                       a value large enough to provide room for the longest line transmitted.
 *
 *                           - (b) TELNETs_NVT_STATE_CR
 *
 *                       State entered whenever a 'CR' character is encounter in the TELNETs_NVT_STATE_GRAPHIC
 *                       state.  From there, you should either have a 'LF' following next (end of line), or a
 *                       NUL meaning a 'CR' alone was intended.
 *
 *                           - (1) However, some telnet client transmit 'CR NUL' at the end of a line.  Hence,
 *                       this implementation also accept this sequence as an EOL marker.  Note that
 *                       this 'CR NUL' is echoed to the client as 'CR LF'.
 *
 *                           - (c) TELNETs_NVT_STATE_IAC
 *
 *                       State entered when an Interpret as Command character ('255') is found in the
 *                       TELNETs_NVT_STATE_GRAPHIC state.
 *
 *                           - (d) TELNETs_NVT_STATE_OPTION
 *
 *                       The machine enters this state when an option verb follows the IAC command (DO,
 *                       DON'T, WILL, WON'T).  Appropriate action is then taken to either response to
 *                       a request or confirm a reply.
 *
 *                           - (e) TELNETs_NVT_STATE_CODE
 *
 *                       When the character following the IAC is neither another IAC nor an option verb,
 *                       it is considered as being a defined telnet command, and this state deals with
 *                       their meaning.
 *
 * @note     (3)     Echoing of received data is performed only when the echo option (TELNET_OPT_ECHO)
 *                       is enabled, and when the function's 'echo' parameter is passed DEF_YES.
 *******************************************************************************************************/
static void TELNETs_NVTPrint(TELNETs_INSTANCE *p_instance,
                             TELNETs_SESSION  *p_session,
                             CPU_BOOLEAN      echo,
                             RTOS_ERR         *p_err)
{
  TELNETs_NVT_STATE state;
  CPU_INT32U        rd_ix;
  CPU_INT32U        wr_ix;
  CPU_INT08U        cur_char;
  TELNETs_OPT       *p_opt_echo;
  CPU_CHAR          *p_cmd;
  CPU_CHAR          bs_cmd[TELNETs_BS_CHAR_LEN + TELNETs_WS_CHAR_LEN];
  CPU_BOOLEAN       bs_pressed;
  RTOS_ERR          local_err;

  state = p_session->NVT_State;
  rd_ix = 0;
  wr_ix = p_session->NVT_BufLen;
  bs_pressed = DEF_NO;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  while ((rd_ix < p_session->RxBufLen)
         && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_RX)        ) {
    cur_char = p_session->RxBufPtr[rd_ix];
    switch (state) {
      case TELNETs_NVT_STATE_GRAPHIC:                           // See Note 2a.
                                                                // ------------------ USASCII GRAPHIC -----------------
        if (cur_char >= TELNET_ASCII_GRAPHIC_LOWER
            && cur_char <= TELNET_ASCII_GRAPHIC_HIGHER) {
          //                                                       See Note 2a1.
          if (wr_ix < p_instance->CfgPtr->NVT_BufLen) {                 // If NVT buf not full ...
            p_session->NVT_BufPtr[wr_ix] = (CPU_CHAR)cur_char;              // ... wr char.
            wr_ix++;
          }
        } else {
          switch (cur_char) {
            //                                                     ------------------ REQUIRED CODES ------------------
            case ASCII_CHAR_CARRIAGE_RETURN:                    // Cur char is 'CR'.
              state = TELNETs_NVT_STATE_CR;
              break;

            case ASCII_CHAR_LINE_FEED:                          // Cur char is 'LF'.
              break;                                            // Do nothing.

            case ASCII_CHAR_NULL:                               // Cur char is 'NULL'.
              break;                                            // Do nothing.

            //                                                     --------------------- IAC CODE ---------------------
            case TELNETs_OPT_STATUS_CMD_IAC:
              state = TELNETs_NVT_STATE_IAC;
              break;

            case ASCII_CHAR_BACKSPACE:                          // Moves the print head 1 char pos towards left margin.
              if (p_session->NVT_BufLen > 0) {
                bs_pressed = DEF_YES;
                p_session->NVT_BufLen--;
                wr_ix--;
              }
              break;

            //                                                     -------------------- OTHER CODE --------------------
            case ASCII_CHAR_BELL:                               // Audible or visible signal without moving the  head.
            case ASCII_CHAR_CHARACTER_TABULATION:               // Moves the printer to the next horizontal tab stop.
            case ASCII_CHAR_LINE_TABULATION:                    // Moves the printer to the next vertical   tab stop.
            case ASCII_CHAR_FORM_FEED:                          // Moves to top of the next page, keep horizontal.
              break;                                            // Do nothing.

            default:
              break;
          }
        }

        rd_ix++;
        break;

      case TELNETs_NVT_STATE_CR:                                // See Note 2b.
        switch (cur_char) {
          case ASCII_CHAR_LINE_FEED:
          case ASCII_CHAR_NULL:                                 // See Note 2b1.
            p_session->NVT_BufPtr[wr_ix++] = ASCII_CHAR_CARRIAGE_RETURN;
            p_session->NVT_BufPtr[wr_ix++] = ASCII_CHAR_LINE_FEED;
            p_session->NVT_BufPtr[wr_ix] = (CPU_CHAR)0;

            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            break;

          default:                                              // Should never happen.
            break;
        }

        rd_ix++;
        state = TELNETs_NVT_STATE_GRAPHIC;
        break;

      case TELNETs_NVT_STATE_IAC:                               // See Note #2c.
        switch (cur_char) {
          case TELNETs_OPT_STATUS_CMD_WILL:
          case TELNETs_OPT_STATUS_CMD_WONT:
          case TELNETs_OPT_STATUS_CMD_DO:
          case TELNETs_OPT_STATUS_CMD_DONT:
            p_session->RxOptStatusCmd = (TELNETs_OPT_STATUS_CMD)cur_char;
            rd_ix++;
            state = TELNETs_NVT_STATE_OPTION;
            break;

          case TELNETs_OPT_STATUS_CMD_IAC:                          // Escape IAC, second should be displayed.
            if (wr_ix < p_instance->CfgPtr->NVT_BufLen) {               // If NVT buf not full ...
                                                                        // ... wr char.
              p_session->NVT_BufPtr[wr_ix] = cur_char;
              wr_ix++;
            }

            rd_ix++;
            state = TELNETs_NVT_STATE_GRAPHIC;
            break;

          default:                                                  // Presume next char is a code.
            state = TELNETs_NVT_STATE_CODE;
            break;
        }

        break;

      case TELNETs_NVT_STATE_OPTION:                            // See Note #2d.
        p_session->RxOptCode = cur_char;
        LOG_DBG(("Option: %u ", (u)p_session->RxOptCode, "Command: %u", (u)p_session->RxOptStatusCmd));

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        TELNETs_RxOptHandler(p_session,
                             p_session->RxOptStatusCmd,
                             p_session->RxOptCode,
                             &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        }

        rd_ix++;
        state = TELNETs_NVT_STATE_GRAPHIC;
        break;

      case TELNETs_NVT_STATE_CODE:                              // See Note 2e.
        switch (cur_char) {
          case TELNETs_OPT_STATUS_CMD_EC:                       // Erase char.
            if (p_session->NVT_BufLen > 0) {
              p_session->NVT_BufLen--;
              wr_ix--;
            }
            break;

          case TELNETs_OPT_STATUS_CMD_EL:                       // Erase line.
            if (p_session->NVT_BufLen > 0) {
              p_session->NVT_BufLen = 0;
              wr_ix = 0;
            }
            break;

          case TELNETs_OPT_STATUS_CMD_NOP:
          case TELNETs_OPT_STATUS_CMD_DM:
          case TELNETs_OPT_STATUS_CMD_BRK:
          case TELNETs_OPT_STATUS_CMD_IP:
          case TELNETs_OPT_STATUS_CMD_AO:
          case TELNETs_OPT_STATUS_CMD_AYT:
          case TELNETs_OPT_STATUS_CMD_GA:
          default:
            break;                                              // Unsupported / no opt cmd's, do nothing.
        }

        rd_ix++;
        state = TELNETs_NVT_STATE_GRAPHIC;
        break;

      default:                                                  // Should never happen.
        break;
    }
  }

  //                                                               ---------------------- TX ECHO ---------------------
  p_opt_echo = TELNETs_GetOpt(p_session, TELNET_OPT_ECHO);      // See Note #3.
  if (p_opt_echo != DEF_NULL) {
    if (p_opt_echo->server_status == TELNETs_OPT_STATUS_CMD_WILL
        && echo == DEF_YES) {
      if (wr_ix > p_session->NVT_BufLen) {
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        TELNETs_Tx(p_session->SockID,
                   p_session->NVT_BufPtr + p_session->NVT_BufLen,
                   wr_ix - p_session->NVT_BufLen,
                   p_session->CfgPtr->TxTriesMaxNbr,
                   &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
          p_session->IsActive = DEF_NO;
        }
      }

      if (bs_pressed == DEF_YES) {                              // If backspace pressed,                     ...
        p_cmd = &bs_cmd[0];
        (void)Str_Copy_N(p_cmd, (const CPU_CHAR *)TELNETs_BS_CHAR, TELNETs_BS_CHAR_LEN + TELNETs_WS_CHAR_LEN);
        (void)Str_Cat_N(p_cmd, (const CPU_CHAR *)TELNETs_WS_CHAR, TELNETs_WS_CHAR_LEN);

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        TELNETs_Tx(p_session->SockID,                          // ... replace previous char by a whitespace ...
                   p_cmd,
                   (TELNETs_BS_CHAR_LEN + TELNETs_WS_CHAR_LEN),
                   p_session->CfgPtr->TxTriesMaxNbr,
                   &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
          p_session->IsActive = DEF_NO;
        }

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        TELNETs_Tx(p_session->SockID,                          // ... & place the cursor before the whitespace.
                   TELNETs_BS_CHAR,
                   TELNETs_BS_CHAR_LEN,
                   p_session->CfgPtr->TxTriesMaxNbr,
                   &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_TX) {
          p_session->IsActive = DEF_NO;
        }
      }
    }
  }

  //                                                               Copy remaining rx_buf at beginning.
  if (rd_ix < p_session->RxBufLen) {
    Mem_Copy(p_session->RxBufPtr, p_session->RxBufPtr + rd_ix, p_session->RxBufLen - rd_ix);
  }

  p_session->RxBufLen = p_session->RxBufLen - rd_ix;
  p_session->NVT_BufLen = wr_ix;
  p_session->NVT_State = state;
}

/****************************************************************************************************//**
 *                                           TELNETs_NVTTxPrompt()
 *
 * @brief    Print the command prompt on the NVT.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TELNETs_NVTTxPrompt(TELNETs_INSTANCE *p_instance,
                                TELNETs_SESSION  *p_session,
                                RTOS_ERR         *p_err)
{
  CPU_SIZE_T prompt_len;

  prompt_len = Str_Len((CPU_CHAR *)TELNETs_PROMPT_STR);

  TELNETs_Tx(p_session->SockID,
             TELNETs_PROMPT_STR,
             prompt_len,
             p_instance->CfgPtr->TxTriesMaxNbr,
             p_err);
}

/****************************************************************************************************//**
 *                                           TELNETs_NVTGetBuf()
 *
 * @brief    Copy NVT buf into parameter dest_buf, appending the final NULL character.
 *
 * @param    p_session       Pointer to session structure.
 *
 * @param    p_dest_buf      Pointer to destination buffer to receive NVT buffer copy.
 *
 * @param    dest_buf_len    Length of  destination buffer.
 *
 * @param    remove_eol      Whether or not to remove the EOL termination characters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Buffer copy terminates when :
 *
 *                               - (a) Destination buffer pointer is passed NULL pointers.
 *                               - (1) No buffer copy performed.
 *
 *                               - (b) Entire source copied into destination buffer.
 *                               - (1) Termination NULL character appended to destination buffer.
 *******************************************************************************************************/
static void TELNETs_NVTGetBuf(TELNETs_SESSION *p_session,
                              CPU_CHAR        *p_dest_buf,
                              CPU_INT16U      dest_buf_len,
                              CPU_BOOLEAN     remove_eol,
                              RTOS_ERR        *p_err)
{
  CPU_CHAR *p_eol;

  RTOS_ASSERT_DBG_ERR_SET((p_dest_buf != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((dest_buf_len != 0), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_session->NVT_BufLen < dest_buf_len), *p_err, RTOS_ERR_INVALID_CFG,; );

  if (p_session->NVT_BufLen == 0) {                             // If NVT buf empty ...
    *p_dest_buf = (CPU_CHAR)0;                                  // ... copy termination char and rtn.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  //                                                               ------------------- COPY NVT BUF -------------------
  Mem_Copy(p_dest_buf,
           p_session->NVT_BufPtr,
           p_session->NVT_BufLen);

  p_dest_buf[p_session->NVT_BufLen] = (CPU_CHAR)0;               // Append termination NULL char.

  //                                                               -------------- REMOVING EOL DELIMITER --------------
  if (remove_eol == DEF_YES) {
    p_eol = Str_Str((CPU_CHAR *)p_dest_buf,
                    (CPU_CHAR *)TELNETs_EOL_STR);
    if (p_eol != DEF_NULL) {
      *p_eol = (CPU_CHAR)0;
    }
  }

  //                                                               ------------------ UPDATE NVT BUF ------------------
  p_session->NVT_BufLen = 0;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           TELNETs_NVTTerminate()
 *
 * @brief    Terminate Network Virtual Terminal (NVT)
 *
 * @param    p_instance  Pointer to Telnet server instance.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @return   DEF_OK          Termination successful.
 *           DEF_FAIL        Termination failed.
 *******************************************************************************************************/
static void TELNETs_NVTTerminate(TELNETs_INSTANCE *p_instance,
                                 TELNETs_SESSION  *p_session)
{
  RTOS_ERR local_err;

  p_session->SockID = (NET_SOCK_ID)NET_SOCK_ID_NONE;

  Mem_DynPoolBlkFree(&p_instance->RxBufPool, p_session->RxBufPtr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolBlkFree(&p_instance->NVT_BufPool, p_session->NVT_BufPtr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#ifdef  RTOS_MODULE_FS_AVAIL
  Mem_DynPoolBlkFree(&p_instance->WorkingDirPool, p_session->CurWorkingDirPtr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_session->CurWorkingDirPtr = DEF_NULL;
#endif

  Mem_DynPoolBlkFree(&p_instance->UserNamePool, p_session->UserNameStrPtr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolBlkFree(&p_instance->PasswordPool, p_session->PasswordStrPtr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolBlkFree(&p_instance->SessionPool, p_session, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               TELNETs_Cmd()
 *
 * Description : (1) Process the received command line :
 *
 *                   (a) Handle internal command
 *                   (b) Handle external command, if necessary
 *
 * Arguments   : p_cmd_line      Pointer to command line.
 *
 *               p_session       Pointer to telnet session structure.
 *
 *               p_err           Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : TELNETs_CMDPROCESS_ERR, if an error occurred.
 *               Command specific return value, otherwise.
 *
 * Note(s)     : (1) The function first look for a match in the internal telnet command.  If co such
 *                   command if found, TELNETs_CmdHandlerInt() returns TELNETs_ERR_CMD_PROCESS, and
 *                   the external command handler comes in.
 *******************************************************************************************************/
static CPU_INT16S TELNETs_Cmd(CPU_CHAR        *p_cmd_line,
                              TELNETs_SESSION *p_session,
                              RTOS_ERR        *p_err)
{
  CPU_INT16S  ret_val;
  NET_SOCK_ID sock;
  void        *p_dir = DEF_NULL;

  PP_UNUSED_PARAM(p_cmd_line);                                  // Prevent 'variable unused' compiler warning.

  sock = p_session->SockID;

#ifdef  RTOS_MODULE_FS_AVAIL
  p_dir = p_session->CurWorkingDirPtr;
#endif

  //                                                               ------------------ HANDLE INT CMD ------------------
  ret_val = TELNETs_CmdHandlerInt(p_session->NVT_BufPtr,
                                  p_dir,
                                  &p_session->IsActive,
                                  &sock,
                                  &TELNETs_OutFnct,
                                  p_err);

  //                                                               ------------------ HANDLE EXT CMD ------------------
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // See Note #1.
    SHELL_CMD_PARAM param;

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#ifdef  RTOS_MODULE_FS_AVAIL
    param.CurWorkingDirPtr = p_session->CurWorkingDirPtr;
#else
    param.CurWorkingDirPtr = DEF_NULL;
#endif
    param.SessionActiveFlagsPtr = &p_session->IsActive;
    param.OutputOptPtr = p_session;

    ret_val = Shell_Exec(p_cmd_line,
                         &TELNETs_OutFnct,
                         &param,
                         p_err);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               TELNETs_CmdHandlerInt()
 *
 * Description : Process received internal command.
 *
 * Arguments   : p_cmd_line       Pointer to command line.
 *
 *               p_cwd            Pointer to current working directory.
 *
 *               p_session_active Pointer to variable indicating whether the session is active or not.
 *
 *               p_out_opt        Pointer to output function optional parameter.
 *
 *               out_fnct         Pointer to output function.
 *
 *               p_err            Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : TELNETs_CMDPROCESS_ERR, if an error occurred.
 *               Command specific return value, otherwise.
 *
 * Note(s)     : (1) This implementation only support the 'logout' internal command.
 *******************************************************************************************************/
static CPU_INT16S TELNETs_CmdHandlerInt(CPU_CHAR        *p_cmd_line,
                                        void            *p_cwd,
                                        CPU_BOOLEAN     *p_session_active,
                                        void            *p_out_opt,
                                        TELNET_OUT_FNCT out_fnct,
                                        RTOS_ERR        *p_err)
{
  CPU_INT16S cmp;
  CPU_INT16S ret_val;

  PP_UNUSED_PARAM(p_cwd);                                       // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_out_opt);
  PP_UNUSED_PARAM(out_fnct);

  cmp = Str_Cmp(TELNETs_INT_CMD_LOGOUT, p_cmd_line);

  if (cmp == 0) {                                               // If cmd is 'logout' ...
    *p_session_active = DEF_NO;                                 // ... terminate the session.
    ret_val = TELNETs_CMDPROCESS_ERR_NONE;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {                                                      // Else ...
    ret_val = TELNETs_CMDPROCESS_ERR;                           // ... cmd not found.
    RTOS_ERR_SET(*p_err, RTOS_ERR_SHELL_CMD_EXEC);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               TELNETs_GetOpt()
 *
 * @brief    Get the Telnet option structure.
 *
 * @param    p_session   Pointer to session structure.
 *
 * @param    opt_code    Option code requested.
 *
 * @return   Pointer to a TELNETs_OPT if successful;
 *           NULL pointer if option not supported.
 *******************************************************************************************************/
static TELNETs_OPT *TELNETs_GetOpt(TELNETs_SESSION *p_session,
                                   CPU_INT08U      opt_code)
{
  TELNETs_OPT *p_opt;
  CPU_INT16U  i;

  //                                                               ---------------- GET CUR OPT STATUS ----------------
  p_opt = DEF_NULL;
  for (i = 0; i < TELNET_NBR_OPT_SUPPORTED; i++) {
    if (opt_code == p_session->Opt[i].code) {
      p_opt = &p_session->Opt[i];
      break;
    }
  }

  return (p_opt);
}

/****************************************************************************************************//**
 *                                               TELNETs_OutFnct()
 *
 * @brief    Output function used by command to transmit data to Telnet session.
 *
 * @param    p_buf       Pointer to buffer containing data to send.
 *
 * @param    buf_len     Length of buffer.
 *
 * @param    p_sock_id   Pointer to socket id.
 *
 * @return   Number of positive data octets transmitted, if NO errors.
 *           TELNETs_SHELL_ERR_TX, otherwise.
 *******************************************************************************************************/
static CPU_INT16S TELNETs_OutFnct(CPU_CHAR   *p_buf,
                                  CPU_INT16U buf_len,
                                  void       *p_arg_session)
{
  TELNETs_SESSION *p_session;
  CPU_INT16S      ret_val;
  RTOS_ERR        local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_session = (TELNETs_SESSION *)p_arg_session;

  TELNETs_Tx(p_session->SockID, p_buf, buf_len, p_session->CfgPtr->TxTriesMaxNbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = TELNETs_SHELL_ERR_TX;
  } else {
    ret_val = buf_len;
  }

  return (ret_val);
}

#endif // RTOS_MODULE_NET_AVAIL
