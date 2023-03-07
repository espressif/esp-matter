/***************************************************************************//**
 * @file
 * @brief Network - DNS Client Request Module
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

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

#ifdef  NET_DNS_CLIENT_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "dns_client_req_priv.h"
#include  "dns_client_cache_priv.h"

#include  <net/include/net_cfg_net.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#endif

#include  <net/include/dns_client.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/include/net_app.h>
#include  <net/source/tcpip/net_util_priv.h>

#include  "../tcpip/net_def_priv.h"
#include  "../tcpip/net_if_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (NET, DNS)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_NET_APP

#define  DNSc_SERVER_IPv4_ADDR_DFLT_STR    "8.8.8.8"
#define  DNSc_SERVER_IPv6_ADDR_DFLT_STR    "2001:4860:4860::8888"

/********************************************************************************************************
 *                                   DNS MODULE CONFIGURATION DEFINES
 *******************************************************************************************************/

#define  DNSc_PKT_MAX_SIZE                               512u
#define  DNSc_PKT_HDR_SIZE                                12u
#define  DNSc_PKT_TYPE_SIZE                                2u
#define  DNSc_PKT_CLASS_SIZE                               2u
#define  DNSc_PKT_TTL_SIZE                                 4u

#define  DNSc_HDR_MSG_LEN_MAX       (DNSc_PKT_MAX_SIZE - DNSc_PKT_HDR_SIZE)

#define  DNSc_NAME_LEN_SIZE                                1u
#define  DNSc_ZERO_CHAR_SIZE                               1u

#define  DNSc_MAX_RX_RETRY                                 3u
#define  DNSc_MAX_RX_DLY_MS                              100u

#define  DNSc_SOCK_TX_RETRY_MAX                             5u
#define  DNSc_SOCK_TX_DLY_MS                                10u

/********************************************************************************************************
 *                                           DNS TYPE DEFINE
 *
 * Note(s) : (1) Fixed value of the the DNS hdr.
 *
 *               (a) Outgoing DNS msg might contain only one question and should not contain any answer
 *                   record, authority record or additional info.
 *
 *               (b) Value of param is fixed so that DNS outgoing pkt represent a standard query and that
 *                   recursion is desired.
 *
 *           (2) Message compression format is described in RFC #1035, Section 4.1.4.
 *
 *               (a) The returned host name may be whether a literal string in the format of a 1-byte count
 *                   followed by the characters that make up the name,or a pointer to a literal string. In
 *                   the case of a compressed host name, the pointer can be represented as follows :
 *
 *                           1  1  1  1  1  1
 *                           5  4  3  2  1  0  9  8  7  6  5  4  3  2  1  0
 *                         -------------------------------------------------
 *                         | 1| 1|                 OFFSET                  |
 *                         -------------------------------------------------
 *
 *                   where OFFSET specifies an offset from the first byte of the ID field in the domain
 *                   header. To reach the IP addr, we must skip over the returned host name, whether it
 *                   is compressed or not. To detect a compression, the 6 LSB of the first byte must be
 *                   masked.
 *
 *               (b) If the host name is compressed, the pkt pointer should skip over the pointer that
 *                   refers to the literal host name string, which has size of 2 bytes.
 *******************************************************************************************************/

//                                                                 See Note #1a.
#define  DNSc_QUESTION_NBR                                 1u
#define  DNSc_ANSWER_NBR                                   0u
#define  DNSc_AUTHORITY_NBR                                0u
#define  DNSc_ADDITIONAL_NBR                               0u

#define  DNSc_PARAM_ENTRY                             0x0100    // See Note #1b.

#define  DNSc_TYPE_A                                       1u   // Host      addr type (see RFC #1035, Section 3.2.2).
#define  DNSc_TYPE_CNAME                                   5u   // Canonical addr type (see RFC #1035, Section 3.3.1).
#define  DNSc_TYPE_AAAA                                   28u   // Host      addr type (see RFC #3596, Section 2.1).
#define  DNSc_CLASS_IN                                     1u   // Internet class      (see RFC #1035, Section 3.2.4).

#define  DNSc_PARAM_QUERY                                  0u   // Query operation     (see RFC #1035, Section 4.1.1).

#define  DNSc_PARAM_MASK_QR                           0x8000    // Mask the 15 MSBs to extract the operation type.
#define  DNSc_PARAM_MASK_RCODE                        0x000F    // Mask the 12 LSBs to extract the response code.

#define  DNSc_ANSWER_NBR_MIN                               1u   // Response msg should contain at least one answer.

#define  DNSc_COMP_ANSWER                               0xC0    // See Note #2a.
#define  DNSc_HOST_NAME_PTR_SIZE                           2u   // See Note #2b.

#define  DNSc_RCODE_NO_ERR                                 0u   // No error code       (see RFC #1035, Section 4.1.1).
#define  DNSc_RCODE_INVALID_REQ_FMT                        1u
#define  DNSc_RCODE_SERVER_FAIL                            2u
#define  DNSc_RCODE_NAME_NOT_EXIST                         3u

#define  DNSc_PORT_DFLT                                     53u // Configure client IP port. Default is 53.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DNS MSG DATA TYPE
 *
 * Note(s) : (1) See RFC #1035, section 4.1 for DNS message format.
 *
 *           (2) Param is a 16 bits field that specifies the operation requested and a response code that
 *               can be represented as follows :
 *
 *                           1  1  1  1  1  1
 *                           5  4  3  2  1  0  9  8  7  6  5  4  3  2  1  0
 *                         -------------------------------------------------
 *                         |   RCODE   |   RSV  |RA|RD|TC|AA|   QTYPE   |QR|
 *                         -------------------------------------------------
 *
 *                   where
 *                           RCODE        Response code
 *                                           0    No error
 *                                           1    Format error in query
 *                                           2    Server failure
 *                                           3    Name does not exist
 *                                           4    Query not supported by server
 *                                           5    Query refused by server
 *
 *                           RSV          Reserved
 *                           RA           Set if recursion available
 *                           RD           Set if recursion desired
 *                           TC           Set if message truncated
 *                           AA           Set if answer authoritative
 *
 *                           QTYPE        Query type
 *                                           0    Standard
 *                                           1    Inverse
 *                                           2    Obsolete
 *                                           3    Obsolete
 *
 *                           QR           Operation type
 *                                           0    Query
 *                                           1    Response
 *******************************************************************************************************/

typedef  struct  DNSc_Msg {
  CPU_INT16U QueryID;                                           // Unique ID.
  CPU_INT16U Param;                                             // Parameters (see Note #2).
  CPU_INT16U QuestionNbr;                                       // Number of question records.
  CPU_INT16U AnswerNbr;                                         // Number of answer records.
  CPU_INT16U AuthorityNbr;                                      // Number of authoritative name server records.
  CPU_INT16U AdditionalNbr;                                     // Number of additional info.
  CPU_INT08U QueryMsg;
} DNSc_HDR;

typedef  struct  DNSc_query_info {
  CPU_INT16U Type;
  CPU_INT16U Class;
} DNSc_QUERY_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern DNSc_DATA *DNSc_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16U DNScReq_TxPrepareMsg(CPU_INT08U    *p_buf,
                                       CPU_INT16U    buf_len,
                                       CPU_CHAR      *p_host_name,
                                       DNSc_REQ_TYPE req_type,
                                       CPU_INT16U    req_query_id,
                                       RTOS_ERR      *p_err);

static void DNScReq_RxRespMsg(DNSc_HOST_OBJ *p_host,
                              CPU_INT08U    *p_resp_msg,
                              CPU_INT16U    resp_msg_len,
                              CPU_INT16U    req_query_id,
                              RTOS_ERR      *p_err);

static void DNScReq_RxRespAddAddr(DNSc_HOST_OBJ *p_host,
                                  CPU_INT16U    answer_type,
                                  CPU_INT08U    *p_data,
                                  RTOS_ERR      *p_err);

static void DNScReq_TxData(NET_SOCK_ID sock_id,
                           CPU_INT08U  *p_buf,
                           CPU_INT16U  data_len,
                           RTOS_ERR    *p_err);

static CPU_INT16U DNScReq_RxData(NET_SOCK_ID sock_id,
                                 CPU_INT08U  *p_buf,
                                 CPU_INT16U  buf_len,
                                 CPU_INT16U  timeout_ms,
                                 RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          DNScReq_ServerSet()
 *
 * @brief    Set server's address.
 *
 * @param    p_addr  Pointer to IP address.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void DNScReq_ServerSet(DNSc_SERVER_ADDR_TYPE addr_type,
                       NET_IP_ADDR_OBJ       *p_addr)
{
  CORE_DECLARE_IRQ_STATE;

  switch (addr_type) {
    case DNSc_SERVER_ADDR_TYPE_STATIC:
      CORE_ENTER_ATOMIC();
      DNSc_DataPtr->ServerAddrStatic.AddrObj = *p_addr;
      DNSc_DataPtr->ServerAddrStatic.IsValid = DEF_YES;
      CORE_EXIT_ATOMIC();
      break;

    case DNSc_SERVER_ADDR_TYPE_AUTO:
      CORE_ENTER_ATOMIC();
      DNSc_DataPtr->ServerAddrAuto.AddrObj = *p_addr;
      DNSc_DataPtr->ServerAddrAuto.IsValid = DEF_YES;
      CORE_EXIT_ATOMIC();
      break;

    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                          DNScReq_ServerGet()
 *
 * @brief    Get the server's address configured.
 *
 * @param    p_addr  Pointer to structure that will receive the IP address of the DNS server.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void DNScReq_ServerGet(NET_IP_ADDR_OBJ *p_addr,
                       RTOS_ERR        *p_err)
{
  CPU_BOOLEAN valid = DEF_NO;
  DNSc_SERVER server_addr;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  server_addr = DNSc_DataPtr->ServerAddrStatic;
  CORE_EXIT_ATOMIC();

  valid = server_addr.IsValid;
  if (valid == DEF_YES) {
    *p_addr = server_addr.AddrObj;
  } else {
    CORE_ENTER_ATOMIC();
    server_addr = DNSc_DataPtr->ServerAddrAuto;
    CORE_EXIT_ATOMIC();

    valid = server_addr.IsValid;
    if (valid == DEF_YES) {
      *p_addr = server_addr.AddrObj;
    } else {
      if (DNSc_DataPtr->CfgDfltServerAddrFallbackEn == DEF_YES) {
#if  (NET_IPv4_CFG_EN == DEF_ENABLED)
        DNScCache_AddrObjSet(p_addr, DNSc_SERVER_IPv4_ADDR_DFLT_STR, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
#elif  (NET_IPv6_CFG_EN == DEF_ENABLED)
        DNScCache_AddrObjSet(p_addr, DNSc_SERVER_IPv6_ADDR_DFLT_STR, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
#endif
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                            DNScReq_Init()
 *
 * @brief    Initialize request.
 *
 * @param    p_server_addr   Pointer to the server address to use for the request.
 *
 * @param    server_port     Server port.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Socket ID,        if successfully initialized.
 *           NET_SOCK_ID_NONE, otherwise.
 *******************************************************************************************************/
NET_SOCK_ID DNScReq_Init(NET_IP_ADDR_OBJ *p_server_addr,
                         NET_PORT_NBR    server_port,
                         RTOS_ERR        *p_err)
{
  NET_IPv4_ADDR            net_ipv4_addr_any;
  NET_IP_ADDR_OBJ          *p_server;
  NET_IP_ADDR_OBJ          server_addr;
  NET_SOCK_ADDR_FAMILY     addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
  NET_SOCK_PROTOCOL_FAMILY protocol_family = NET_SOCK_PROTOCOL_FAMILY_NONE;
  NET_SOCK_ID              sock_id = NET_SOCK_ID_NONE;
  NET_PORT_NBR             port = DNSc_PORT_DFLT;
  NET_SOCK_ADDR            sock_addr_server;
  NET_SOCK_ADDR            sock_addr_local;
  NET_SOCK_ADDR_LEN        addr_len = 0u;
  CPU_INT08U               *p_addr = DEF_NULL;
  CPU_BOOLEAN              close_sock = DEF_NO;

  if (p_server_addr == DEF_NULL) {
    DNScReq_ServerGet(&server_addr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    p_server = &server_addr;
  } else {
    p_server = p_server_addr;
  }

  if (server_port != NET_PORT_NBR_NONE) {
    port = server_port;
  }

  switch (p_server->AddrLen) {
    case NET_IPv4_ADDR_LEN:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      net_ipv4_addr_any = NET_IPv4_ADDR_ANY;
      p_addr = (CPU_INT08U *)&net_ipv4_addr_any;
      addr_len = NET_IPv4_ADDR_SIZE;
      break;

#ifdef  NET_IPv6_MODULE_EN
    case NET_IPv6_ADDR_LEN:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      p_addr = (CPU_INT08U *)&NetIPv6_AddrAny;
      addr_len = NET_IPv6_ADDR_SIZE;
      break;
#endif

    case NET_IP_ADDR_FAMILY_UNKNOWN:
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_SOCK_ID_NONE);
  }

  //                                                               --- CREATE SOCKET TO COMMUNICATE WITH DNS SERVER ---
  sock_id = NetSock_Open(protocol_family,
                         NET_SOCK_TYPE_DATAGRAM,
                         NET_SOCK_PROTOCOL_UDP,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  close_sock = DEF_YES;                                         // After this point, close socket in case of errors.

  NetApp_SetSockAddr(&sock_addr_local,
                     addr_family,
                     NET_PORT_NBR_NONE,
                     p_addr,
                     addr_len,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetApp_SetSockAddr(&sock_addr_server,
                     addr_family,
                     port,
                     &p_server->Addr.Array[0],
                     p_server->AddrLen,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  (void)NetSock_Bind(sock_id,
                     &sock_addr_local,
                     sizeof(sock_addr_local),
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetSock_Conn(sock_id,                                         // Open sock to DNS server.
               &sock_addr_server,
               sizeof(sock_addr_server),
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  close_sock = DEF_NO;                                          // No errors, don't close the socket.

exit:
  if (close_sock == DEF_YES) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(sock_id,
                  &local_err);

    sock_id = NET_SOCK_ID_NONE;
  }

  return (sock_id);
}

/****************************************************************************************************//**
 *                                           DNSc_ReqIF_Sel()
 *
 * @brief    Select next interface for DNS process.
 *
 * @param    if_nbr_last     Previous Interface number.
 *
 * @param    sock_id         Socket ID to use.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   New interface number.
 *******************************************************************************************************/
NET_IF_NBR DNSc_ReqIF_Sel(NET_IF_NBR  if_nbr_last,
                          NET_SOCK_ID sock_id,
                          RTOS_ERR    *p_err)
{
  NET_IF_NBR if_nbr_up = NET_IF_NBR_NONE;
  NET_IF_NBR if_nbr_ix;
  NET_IF_NBR if_nbr_cfgd;
  NET_IF_NBR if_nbr_base;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if_nbr_base = NetIF_GetNbrBaseCfgd();
  if_nbr_cfgd = NetIF_GetExtAvailCtr(&local_err);
  if_nbr_cfgd -= if_nbr_base;

  if (if_nbr_last != NET_IF_NBR_WILDCARD) {
    if_nbr_ix = if_nbr_last + 1;
    if (if_nbr_ix > if_nbr_cfgd) {
      if_nbr_ix = if_nbr_base;
      goto exit;
    }
  } else {
    if_nbr_ix = if_nbr_base;
  }

  for (; if_nbr_ix <= if_nbr_cfgd; if_nbr_ix++) {
    NET_IF_LINK_STATE state = NET_IF_LINK_DOWN;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    state = NetIF_LinkStateGet(if_nbr_ix, &local_err);
    if ((state == NET_IF_LINK_UP)
        && (if_nbr_up == NET_IF_NBR_NONE)) {
      if_nbr_up = if_nbr_ix;
      break;
    }
  }

  if (if_nbr_up == NET_IF_NBR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_IF_LINK_DOWN);
    goto exit;
  }

  NetSock_CfgIF(sock_id, if_nbr_up, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (if_nbr_up);
}

/****************************************************************************************************//**
 *                                            DNSc_ReqClose()
 *
 * @brief    Close request objects.
 *
 * @param    sock_id     Socket ID used during the request.
 *******************************************************************************************************/
void DNSc_ReqClose(NET_SOCK_ID sock_id)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetSock_Close(sock_id, &local_err);
}

/****************************************************************************************************//**
 *                                            DNScReq_TxReq()
 *
 * @brief    Prepare request and transmit to the server.
 *
 * @param    p_host_name     Pointer to a string that contains the host name to resolve.
 *
 * @param    sock_id         Socket ID.
 *
 * @param    query_id        Query ID of the request.
 *                           DNSc_QUERY_ID_NONE a new query ID is generated.
 *
 * @param    req_type        Request type:
 *                           DNSc_REQ_TYPE_IPv4  Request IPv4 address(es)
 *                           DNSc_REQ_TYPE_IPv6  Request IPv6 address(es)
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Query ID, if successfully transmitted.
 *           DNSc_QUERY_ID_NONE, Otherwise.
 *******************************************************************************************************/
CPU_INT16U DNScReq_TxReq(CPU_CHAR      *p_host_name,
                         NET_SOCK_ID   sock_id,
                         CPU_INT16U    query_id,
                         DNSc_REQ_TYPE req_type,
                         RTOS_ERR      *p_err)
{
  CPU_INT08U buf[DNSc_PKT_MAX_SIZE];
  CPU_INT16U req_query_id = DNSc_QUERY_ID_NONE;
  CPU_INT16U data_len;

  if (query_id == DNSc_QUERY_ID_NONE) {
    query_id = (CPU_INT16U)NetUtil_RandomRangeGet(0, DEF_INT_16U_MAX_VAL);
  }

  data_len = DNScReq_TxPrepareMsg(buf, DNSc_PKT_MAX_SIZE, p_host_name, req_type, query_id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  DNScReq_TxData(sock_id, buf, data_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  req_query_id = query_id;

exit:
  return (req_query_id);
}

/****************************************************************************************************//**
 *                                           DNScReq_RxResp()
 *
 * @brief    Receive DNS response.
 *
 * @param    p_host      Pointer to the host object.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    query_id    Query ID of the request.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Request Status:
 *               - DNSc_STATUS_PENDING
 *               - DNSc_STATUS_RESOLVED
 *******************************************************************************************************/
DNSc_STATUS DNScReq_RxResp(DNSc_HOST_OBJ *p_host,
                           NET_SOCK_ID   sock_id,
                           CPU_INT16U    query_id,
                           CPU_INT16U    timeout_ms,
                           RTOS_ERR      *p_err)
{
  DNSc_STATUS status = DNSc_STATUS_PENDING;
  CPU_INT08U  buf[DNSc_PKT_MAX_SIZE];
  CPU_INT16U  data_len;

  data_len = DNScReq_RxData(sock_id, buf, sizeof(buf), timeout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  DNScReq_RxRespMsg(p_host, buf, data_len, query_id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  status = DNSc_STATUS_RESOLVED;

exit:
  return (status);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        DNScReq_TxPrepareMsg()
 *
 * @brief    Prepare request's message.
 *
 * @param    p_buf           Buffer where to format the request.
 *
 * @param    buf_len         Request's buffer length.
 *
 * @param    p_host_name     Pointer to a string that contains the host name to resolve.
 *
 * @param    req_type        Request type:
 *                           DNSc_REQ_TYPE_IPv4  Request IPv4 address(es)
 *                           DNSc_REQ_TYPE_IPv6  Request IPv6 address(es)
 *
 * @param    req_query_id    Request ID.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Message length.
 *******************************************************************************************************/
static CPU_INT16U DNScReq_TxPrepareMsg(CPU_INT08U    *p_buf,
                                       CPU_INT16U    buf_len,
                                       CPU_CHAR      *p_host_name,
                                       DNSc_REQ_TYPE req_type,
                                       CPU_INT16U    req_query_id,
                                       RTOS_ERR      *p_err)
{
  DNSc_HDR   *p_hdr = (DNSc_HDR *)p_buf;
  CPU_INT08U *p_query;
  CPU_INT08U *p_cname;
  CPU_INT16U msg_len = 0u;
  CPU_INT16U msg_type = DNSc_TYPE_A;
  CPU_INT16U buf_rem_len = buf_len;
  CPU_INT16U query_class;
  CPU_CHAR   *p_name;
  CPU_CHAR   *p_dot;

  switch (req_type) {
    case DNSc_REQ_TYPE_IPv4:
      msg_type = DNSc_TYPE_A;
      break;

    case DNSc_REQ_TYPE_IPv6:
      msg_type = DNSc_TYPE_AAAA;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
  }

  p_hdr->QueryID = NET_UTIL_HOST_TO_NET_16(req_query_id);
  p_hdr->Param = NET_UTIL_HOST_TO_NET_16(DNSc_PARAM_ENTRY);
  p_hdr->QuestionNbr = NET_UTIL_HOST_TO_NET_16(DNSc_QUESTION_NBR);
  p_hdr->AnswerNbr = NET_UTIL_HOST_TO_NET_16(DNSc_ANSWER_NBR);
  p_hdr->AuthorityNbr = NET_UTIL_HOST_TO_NET_16(DNSc_AUTHORITY_NBR);
  p_hdr->AdditionalNbr = NET_UTIL_HOST_TO_NET_16(DNSc_ADDITIONAL_NBR);

  p_query = &p_hdr->QueryMsg;
  p_cname = p_query;

  buf_rem_len -= (p_query - p_buf);
  p_name = p_host_name;

  do {                                                          // Message compression (See RFC-1035 Section 4.1.4)
    CPU_INT08U str_len;

    p_dot = Str_Char(p_name, ASCII_CHAR_FULL_STOP);
    if (p_dot != DEF_NULL) {
      str_len = p_dot - p_name;                                 // Nb of chars between char and next '.'
    } else {
      str_len = Str_Len_N(p_name, buf_len);                     // Nb of chars between first char and '\0'
    }

    if ((str_len <= 0u)
        && (str_len > buf_rem_len)) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
    }

    *p_cname = str_len;                                         // Put number of char that follow before the next stop.
    p_cname++;

    Mem_Copy(p_cname, p_name, str_len);                         // Copy Chars

    p_name = (p_dot   + 1u);
    p_cname += str_len;
    buf_rem_len -= str_len;
  } while (p_dot);

  *p_cname = ASCII_CHAR_NULL;                                   // Insert end of line char
  p_cname++;

  p_query = p_cname;

  msg_type = NET_UTIL_HOST_TO_NET_16(msg_type);                 // Set query TYPE.
  Mem_Copy(p_query, &msg_type, sizeof(msg_type));

  p_query += sizeof(msg_type);

  query_class = NET_UTIL_HOST_TO_NET_16(DNSc_CLASS_IN);         // Set query CLASS.
  Mem_Copy(p_query, &query_class, sizeof(query_class));
  p_query += sizeof(query_class);

  msg_len = p_query - p_buf;                                    // Compute total pkt size (see Note #4).

  return (msg_len);
}

/****************************************************************************************************//**
 *                                          DNScReq_RxRespMsg()
 *
 * @brief    Analyze response message.
 *
 * @param    p_resp_msg      Pointer to the response's message.
 *
 * @param    resp_msg_len    Response's message length
 *
 * @param    req_query_id    Request ID expected.
 *
 * @param    p_host          Pointer to a host object.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void DNScReq_RxRespMsg(DNSc_HOST_OBJ *p_host,
                              CPU_INT08U    *p_resp_msg,
                              CPU_INT16U    resp_msg_len,
                              CPU_INT16U    req_query_id,
                              RTOS_ERR      *p_err)
{
  DNSc_HDR   *p_dns_msg = (DNSc_HDR *)p_resp_msg;
  CPU_INT16U query_id;
  CPU_INT16U question_nbr;
  CPU_INT16U answer_nbr;
  CPU_INT08U *p_data;
  CPU_INT16U answer_type;
  CPU_INT16U data_16;
  CPU_INT16U msg_len_remaining;
  CPU_INT08U ix;

  msg_len_remaining = resp_msg_len;

  Mem_Copy(&data_16, &p_dns_msg->Param, sizeof(p_dns_msg->Param));
  data_16 = NET_UTIL_NET_TO_HOST_16(data_16) & DNSc_PARAM_MASK_QR;
  if (data_16 == DNSc_PARAM_QUERY) {                            // If the response is not a query response,         ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // ... rtn err.
    return;
  }

  Mem_Copy(&query_id, &p_dns_msg->QueryID, sizeof(p_dns_msg->QueryID));
  query_id = NET_UTIL_NET_TO_HOST_16(query_id);                 // If the query ID is incorrect,                    ...
  if (query_id != req_query_id) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // ... rtn err.
    goto exit;
  }

  Mem_Copy(&data_16, &p_dns_msg->Param, sizeof(p_dns_msg->Param));
  data_16 = NET_UTIL_NET_TO_HOST_16(data_16) & DNSc_PARAM_MASK_RCODE;
  switch (data_16) {
    case DNSc_RCODE_NO_ERR:
      break;

    case DNSc_RCODE_INVALID_REQ_FMT:
      LOG_ERR(("RX Invalid Format"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;

    case DNSc_RCODE_SERVER_FAIL:
      LOG_ERR(("RX Server Fail"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;

    case DNSc_RCODE_NAME_NOT_EXIST:
      LOG_ERR(("RX Name Not Exist"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  Mem_Copy(&question_nbr, &p_dns_msg->QuestionNbr, sizeof(p_dns_msg->QuestionNbr));
  question_nbr = NET_UTIL_NET_TO_HOST_16(question_nbr);
  if (question_nbr != DNSc_QUESTION_NBR) {                      // If nbr of question do not match the query,       ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // ... rtn err.
    goto exit;
  }

  Mem_Copy(&answer_nbr, &p_dns_msg->AnswerNbr, sizeof(p_dns_msg->AnswerNbr));
  answer_nbr = NET_UTIL_NET_TO_HOST_16(answer_nbr);
  if (answer_nbr < DNSc_ANSWER_NBR_MIN) {                       // If nbr of answer is null,                        ...
    goto exit;                                                  // No answer for this type of request.
  }

  // Remove header length from message length.
  msg_len_remaining -= 12;

  //                                                               Skip over the questions section.
  p_data = &p_dns_msg->QueryMsg;

  for (ix = 0u; ix < question_nbr; ix++) {
    while (*p_data != ASCII_CHAR_NULL
           && msg_len_remaining > 0) {                        // Step through the host name until reaching the ZERO.
      if (msg_len_remaining < *p_data) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      msg_len_remaining -= *p_data;
      p_data += *p_data;
      if (msg_len_remaining < 1) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      msg_len_remaining--;
      p_data++;
    }

    if (msg_len_remaining < (DNSc_ZERO_CHAR_SIZE + DNSc_PKT_TYPE_SIZE + DNSc_PKT_CLASS_SIZE)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    msg_len_remaining -= (DNSc_ZERO_CHAR_SIZE + DNSc_PKT_TYPE_SIZE + DNSc_PKT_CLASS_SIZE);


    p_data += (DNSc_ZERO_CHAR_SIZE                              // Skip over the ZERO.
               + DNSc_PKT_TYPE_SIZE                             // Skip over the TYPE.
               + DNSc_PKT_CLASS_SIZE);                          // Skip over the CLASS.
  }

  //                                                               Extract the rtn'd IP addr (see Note #5).
  for (ix = 0; ix < answer_nbr; ix++) {
    //                                                             Skip over the answer host name.
    if ((*p_data & DNSc_COMP_ANSWER) == DNSc_COMP_ANSWER) {     // If the host name is compressed,                  ...
      if (msg_len_remaining < DNSc_HOST_NAME_PTR_SIZE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      msg_len_remaining -= DNSc_HOST_NAME_PTR_SIZE;
      p_data += DNSc_HOST_NAME_PTR_SIZE;                        // ... skip over the host name pointer.
    } else {
      while (*p_data != ASCII_CHAR_NULL
             && msg_len_remaining > 0) {                        // Step through the host name until reaching the ZERO.
        if (msg_len_remaining < *p_data) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
        msg_len_remaining -= *p_data;
        p_data += *p_data;
        if (msg_len_remaining < 1) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
        msg_len_remaining--;
        p_data++;
      }

      if (msg_len_remaining < DNSc_ZERO_CHAR_SIZE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      msg_len_remaining -= DNSc_ZERO_CHAR_SIZE;
      p_data += DNSc_ZERO_CHAR_SIZE;                            // Skip over the ZERO.
    }

    if (msg_len_remaining < sizeof(CPU_INT16U)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    Mem_Copy(&answer_type, p_data, sizeof(CPU_INT16U));
    answer_type = NET_UTIL_NET_TO_HOST_16(answer_type);         // Get answer TYPE.

    if (msg_len_remaining < (DNSc_PKT_TYPE_SIZE + DNSc_PKT_CLASS_SIZE + DNSc_PKT_TTL_SIZE)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    msg_len_remaining -= (DNSc_PKT_TYPE_SIZE + DNSc_PKT_CLASS_SIZE + DNSc_PKT_TTL_SIZE);
    p_data += (DNSc_PKT_TYPE_SIZE                               // Skip over the CLASS & the TTL.
               + DNSc_PKT_CLASS_SIZE
               + DNSc_PKT_TTL_SIZE);

    if (msg_len_remaining < sizeof(CPU_INT16U)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    msg_len_remaining -= sizeof(CPU_INT16U);
    Mem_Copy(&data_16, p_data, sizeof(CPU_INT16U));
    data_16 = NET_UTIL_NET_TO_HOST_16(data_16);                 // Addr len.
    p_data += sizeof(CPU_INT16U);

    if (msg_len_remaining < data_16) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    msg_len_remaining -= data_16;
    DNScReq_RxRespAddAddr(p_host, answer_type, p_data, p_err);

    p_data += data_16;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DNScReq_RxRespAddAddr()
 *
 * @brief    Add address for the response message to the host.
 *
 * @param    p_host          Pointer to the host object.
 *
 * @param    answer_type     Answer type.
 *
 * @param    p_data          Pointer to the data that contains the address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void DNScReq_RxRespAddAddr(DNSc_HOST_OBJ *p_host,
                                  CPU_INT16U    answer_type,
                                  CPU_INT08U    *p_data,
                                  RTOS_ERR      *p_err)
{
  NET_IP_ADDR_OBJ *p_addr;
  CPU_INT32U      *p_net_addr;
  CPU_INT32U      *p_host_addr;

  p_addr = DNScCache_AddrObjGet(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (answer_type) {
    case DNSc_TYPE_A:
      Mem_Copy(&p_addr->Addr, p_data, sizeof(CPU_INT32U));

      p_net_addr = (CPU_INT32U *)&p_addr->Addr.Array[0];
      p_host_addr = (CPU_INT32U *)&p_addr->Addr.Array[0];
      *p_host_addr = NET_UTIL_NET_TO_HOST_32(*p_net_addr);
      p_addr->AddrLen = NET_IPv4_ADDR_LEN;
      break;

    case DNSc_TYPE_AAAA:
      Mem_Copy(&p_addr->Addr.Array[0], p_data, NET_IPv6_ADDR_LEN);
      p_addr->AddrLen = NET_IPv6_ADDR_LEN;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_release_addr;
  }

  DNScCache_HostAddrInsert(p_host, p_addr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release_addr;
  }

  goto exit;

exit_release_addr:
  DNScCache_AddrObjFree(p_addr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DNScReq_TxData()
 *
 * @brief    Transmit data on the network
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_buf       Pointer to the buffer that contains the data to transmit.
 *
 * @param    data_len    Data length to transmit.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void DNScReq_TxData(NET_SOCK_ID sock_id,
                           CPU_INT08U  *p_buf,
                           CPU_INT16U  data_len,
                           RTOS_ERR    *p_err)
{
  CPU_INT32S  data_txd;
  CPU_INT08U  *p_data = p_buf;
  CPU_INT08U  fail_retry = 0u;
  CPU_INT32U  len = data_len;
  CPU_BOOLEAN req_done = DEF_NO;

  do {
    //                                                             Tx DNS req.
    data_txd = NetSock_TxData(sock_id,
                              p_data,
                              data_len,
                              NET_SOCK_FLAG_TX_NO_BLOCK,
                              p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if (data_txd > 0) {
          p_data += data_txd;
          len -= data_txd;
        }
        if (len <= 0) {
          req_done = DEF_YES;
        }
        break;

      //                                                           Retry on transitory tx err(s).
      case RTOS_ERR_POOL_EMPTY:
      case RTOS_ERR_TIMEOUT:
        if ((len > 0u)
            && (fail_retry < DNSc_SOCK_TX_RETRY_MAX)) {
          KAL_Dly(DNSc_SOCK_TX_DLY_MS);
          fail_retry++;
        } else {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_RETRY_MAX);
          goto exit;
        }
        break;

      case RTOS_ERR_NET_IF_LINK_DOWN:
        goto exit;

      default:                                                  // Rtn   on any fatal  tx err(s).
        goto exit;
    }
  } while (req_done != DEF_YES);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DNScReq_RxData()
 *
 * @brief    Receive data from the network.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_buf       Pointer to the buffer that will receive the data.
 *
 * @param    data_len    Buffer length.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of bytes received.
 *******************************************************************************************************/
static CPU_INT16U DNScReq_RxData(NET_SOCK_ID sock_id,
                                 CPU_INT08U  *p_buf,
                                 CPU_INT16U  buf_len,
                                 CPU_INT16U  timeout_ms,
                                 RTOS_ERR    *p_err)
{
  CPU_INT32S         rx_len = 0;
  NET_SOCK_API_FLAGS flag;

  if (timeout_ms != 0) {
    NetSock_CfgTimeoutRxQ_Set(sock_id, timeout_ms, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    flag = NET_SOCK_FLAG_NONE;
  } else {
    flag = NET_SOCK_FLAG_RX_NO_BLOCK;
  }

  rx_len = NetSock_RxData(sock_id,
                          p_buf,
                          buf_len,
                          flag,
                          p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_TIMEOUT:
      rx_len = 0;
      goto exit;

    default:                                    // Rtn   on any fatal  rx err(s).
      goto exit;
  }

exit:
  return ((CPU_INT16U)rx_len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DNS_CLIENT_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
