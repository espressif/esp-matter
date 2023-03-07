/***************************************************************************//**
 * @file
 * @brief Network Security Port Layer - mbed Tls
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#if (defined(RTOS_MODULE_NET_AVAIL)         \
  && defined(RTOS_MODULE_NET_SSL_TLS_AVAIL) \
  && defined(RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/net_cfg_net.h>
#include  <common/include/rtos_path.h>

#include  <net/include/net_secure.h>
#include  <net/source/tcpip/net_sock_priv.h>
#include  <net/source/ssl_tls/net_secure_priv.h>
#include  <net/include/net_sock.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_math.h>
#include  <common/source/logging/logging_priv.h>

#include  <mbedtls/ssl.h>
#include  <mbedtls/entropy.h>
#include  <mbedtls/ctr_drbg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET, SSL)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

#define  MBEDTLS_ERR_NET_SOCKET_FAILED                  -0x0042  ///< Failed to open a socket.
#define  MBEDTLS_ERR_NET_CONNECT_FAILED                 -0x0044  ///< The connection to the given server / port failed.
#define  MBEDTLS_ERR_NET_BIND_FAILED                    -0x0046  ///< Binding of the socket failed.
#define  MBEDTLS_ERR_NET_LISTEN_FAILED                  -0x0048  ///< Could not listen on the socket.
#define  MBEDTLS_ERR_NET_ACCEPT_FAILED                  -0x004A  ///< Could not accept the incoming connection.
#define  MBEDTLS_ERR_NET_RECV_FAILED                    -0x004C  ///< Reading information from the socket failed.
#define  MBEDTLS_ERR_NET_SEND_FAILED                    -0x004E  ///< Sending information through the socket failed.
#define  MBEDTLS_ERR_NET_CONN_RESET                     -0x0050  ///< Connection was reset by peer.
#define  MBEDTLS_ERR_NET_UNKNOWN_HOST                   -0x0052  ///< Failed to get an IP address for the given hostname.
#define  MBEDTLS_ERR_NET_BUFFER_TOO_SMALL               -0x0043  ///< Buffer is too small to hold the data.
#define  MBEDTLS_ERR_NET_INVALID_CONTEXT                -0x0045  ///< The context is invalid, eg because it was free()ed.

#ifndef  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN
#define  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN          DEF_DISABLED
#endif
#define  NET_SECURE_CERT_VERIFY_DBG_BUF_LEN                1024u
#define  NET_SECURE_CERT_VERIFY_BUF_LEN                     128u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_secure_mbed_tls_data {
  MEM_DYN_POOL             SessionPool;
  MEM_DYN_POOL             CtxPool;
  MEM_DYN_POOL             CfgPool;
  MEM_DYN_POOL             CtrDrbgPool;
  MEM_DYN_POOL             SrvCertPool;
  MEM_DYN_POOL             SrvPrivKeyPool;
  MEM_DYN_POOL             CliCertPool;
  MEM_DYN_POOL             CliPrivKeyPool;
  MEM_DYN_POOL             CertVerifyBufPool;                   // String holding cert verification fail reason
#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED
  MEM_DYN_POOL             CertDebugBufPool;                    // String holding cert detailed information for debug
#endif
  mbedtls_ctr_drbg_context CtrDrbgPtr;
  mbedtls_entropy_context  EntropyCtx;
  mbedtls_x509_crt         CA_CertList;                         // Certificate Authority List
} NET_SECURE_MBED_TLS_DATA;

typedef  struct  net_secure_mbed_tls_session {
  mbedtls_ssl_context        *CtxPtr;
  mbedtls_ssl_config         *CfgPtr;
  mbedtls_x509_crt           *SrvCertPtr;                       // Server certificate
  mbedtls_pk_context         *SrvPrivKeyPtr;                    // Server private key
  mbedtls_x509_crt           *CliCertPtr;                       // Client certificate
  mbedtls_pk_context         *CliPrivKeyPtr;                    // Client private key
  NET_SOCK_SECURE_TYPE       Type;
  NET_SOCK_SECURE_TRUST_FNCT TrustCallBackFnctPtr;
} NET_SECURE_MBED_TLS_SESSION;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_SECURE_MBED_TLS_DATA *NetSecure_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT32S NetSecure_EntropySource(void       *p_data,
                                          CPU_INT08U *p_output,
                                          CPU_SIZE_T len,
                                          CPU_SIZE_T *p_olen);

#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED
static int NetSecure_CertVerifyCallback(void             *p_data,
                                        mbedtls_x509_crt *p_crt,
                                        int              depth,
                                        uint32_t         *flags);
#endif

static CPU_INT32S NetSecure_MbedTLS_Receive(void       *p_ctx,
                                            CPU_INT08U *p_buf,
                                            CPU_SIZE_T len);

static int NetSecure_MbedTLS_Receive_Timeout(void          *p_ctx,
                                             unsigned char *p_buf,
                                             size_t        len,
                                             uint32_t      timeout_ms);

static CPU_INT32S NetSecure_MbedTLS_Send(void             *p_ctx,
                                         const CPU_INT08U *p_buf,
                                         CPU_SIZE_T       len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetSecure_CA_CertInstall()
 *
 * @brief    Install certificate autority's certificate.
 *
 * @param    p_ca_cert       Pointer to CA certificate.
 *
 * @param    ca_cert_len     Certificate length.
 *
 * @param    fmt             Certificate format:
 *                               - NET_SOCK_SECURE_CERT_KEY_FMT_PEM
 *                               - NET_SOCK_SECURE_CERT_KEY_FMT_DER
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_FAIL
 *                               - RTOS_ERR_INVALID_TYPE
 *
 * @return   DEF_OK,
 *           DEF_FAIL.
 *******************************************************************************************************/
CPU_BOOLEAN NetSecure_CA_CertInstall(const void                   *p_ca_cert,
                                     CPU_INT32U                   ca_cert_len,
                                     NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                     RTOS_ERR                     *p_err)
{
  NET_SECURE_MBED_TLS_DATA *p_data;
  CPU_INT32S               rtn_val;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(fmt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  p_data = NetSecure_DataPtr;
  CORE_EXIT_ATOMIC();

  if (p_data == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return (DEF_FAIL);
  }

  rtn_val = mbedtls_x509_crt_parse(&p_data->CA_CertList, p_ca_cert, ca_cert_len);

  if (rtn_val < 0) {
    LOG_ERR(("CA NetSecure_CA_CertInstall failed. mbedtls_x509_crt_parse returned ", (x) - rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetSecure_Init()
 *
 * @brief    Initialize security port :
 *               - (a) Initialize security memory pools
 *               - (b) Initialize entropy source
 *               - (c) Initialize Deterministic Random Byte Generator
 *               - (d) Initialize CA descriptors
 *
 * @param    p_mem_seg   Pointer to memory segment for allocation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSecure_Init(MEM_SEG  *p_mem_seg,
                    RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_DATA *p_data;
  CPU_INT32U               rtn_val;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ------ ALLOCATE DATA OBJECT FOR SECURE MODULE ------
  p_data = (NET_SECURE_MBED_TLS_DATA *)Mem_SegAlloc("mbed TLS data", p_mem_seg, sizeof(NET_SECURE_MBED_TLS_DATA), p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- CREATE NECESSARY POOLS --------------
  Mem_DynPoolCreate("mbed TLS session pool",
                    &p_data->SessionPool,
                    p_mem_seg,
                    sizeof(NET_SECURE_MBED_TLS_SESSION),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS ctx pool",
                    &p_data->CtxPool,
                    p_mem_seg,
                    sizeof(mbedtls_ssl_context),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS cfg pool",
                    &p_data->CfgPool,
                    p_mem_seg,
                    sizeof(mbedtls_ssl_config),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS server cert pool",
                    &p_data->SrvCertPool,
                    p_mem_seg,
                    sizeof(mbedtls_x509_crt),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS server private key pool",
                    &p_data->SrvPrivKeyPool,
                    p_mem_seg,
                    sizeof(mbedtls_pk_context),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS client cert pool",
                    &p_data->CliCertPool,
                    p_mem_seg,
                    sizeof(mbedtls_x509_crt),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("mbed TLS client private key pool",
                    &p_data->CliPrivKeyPool,
                    p_mem_seg,
                    sizeof(mbedtls_pk_context),
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("Temp cert verify buffer",
                    &p_data->CertVerifyBufPool,
                    p_mem_seg,
                    sizeof(CPU_CHAR) * NET_SECURE_CERT_VERIFY_BUF_LEN,
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED
  Mem_DynPoolCreate("Cert debug buffer",
                    &p_data->CertDebugBufPool,
                    p_mem_seg,
                    sizeof(CPU_CHAR) * NET_SECURE_CERT_VERIFY_DBG_BUF_LEN,
                    sizeof(CPU_ALIGN),
                    0,
                    NET_SOCK_NBR_SOCK,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  //                                                               ---------------- SET ENTROPY SOURCE ----------------
  mbedtls_entropy_init(&p_data->EntropyCtx);

  rtn_val = mbedtls_entropy_add_source(&p_data->EntropyCtx,
                                       NetSecure_EntropySource,
                                       DEF_NULL,
                                       32,
                                       MBEDTLS_ENTROPY_SOURCE_STRONG);
  if (rtn_val != 0) {
    LOG_ERR(("NetSecure_InitSession failed. mbedtls_entropy_add_source returned", (d)rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
    goto exit;
  }

  //                                                               --------------- SET CTR_DRBG CONTEXT ---------------

  mbedtls_ctr_drbg_init(&p_data->CtrDrbgPtr);

  rtn_val = mbedtls_ctr_drbg_seed(&p_data->CtrDrbgPtr,
                                  mbedtls_entropy_func,
                                  &p_data->EntropyCtx,
                                  DEF_NULL,
                                  0);
  if (rtn_val != 0) {
    LOG_ERR(("NetSecure_InitSession failed. mbedtls_ctr_drbg_seed returned", (d)rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
    goto exit;
  }

  //                                                               ----------- INITIALIZE CERTIFICATE LIST ------------
  mbedtls_x509_crt_init(&p_data->CA_CertList);

  CORE_ENTER_ATOMIC();
  NetSecure_DataPtr = p_data;
  CORE_EXIT_ATOMIC();

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetSecure_InitSession()
 *
 * @brief    Initialize a new secure session.
 *
 * @param    p_sock  Pointer to the accepted/connected socket.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSecure_InitSession(NET_SOCK *p_sock,
                           RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session = DEF_NULL;

  p_session = (NET_SECURE_MBED_TLS_SESSION *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->SessionPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->CtxPtr = DEF_NULL;
  p_session->CfgPtr = DEF_NULL;

  p_session->CtxPtr = (mbedtls_ssl_context *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->CtxPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->CfgPtr = (mbedtls_ssl_config *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->CfgPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->SrvCertPtr = (mbedtls_x509_crt *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->SrvCertPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->SrvPrivKeyPtr = (mbedtls_pk_context *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->SrvPrivKeyPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->CliCertPtr = (mbedtls_x509_crt *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->CliCertPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_session->CliPrivKeyPtr = (mbedtls_pk_context *)Mem_DynPoolBlkGet(&NetSecure_DataPtr->CliPrivKeyPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  mbedtls_ssl_init(p_session->CtxPtr);
  mbedtls_ssl_config_init(p_session->CfgPtr);

  mbedtls_x509_crt_init(p_session->CliCertPtr);
  mbedtls_pk_init(p_session->CliPrivKeyPtr);

  mbedtls_x509_crt_init(p_session->SrvCertPtr);
  mbedtls_pk_init(p_session->SrvPrivKeyPtr);

  p_sock->SecureSession = p_session;

  return;

exit:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    if (p_session != DEF_NULL) {
      if (p_session->CtxPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->CtxPool, p_session->CtxPtr, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (p_session->CfgPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->CfgPool, p_session->CfgPtr, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (p_session->SrvCertPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->SrvCertPool, p_session->SrvCertPtr, &local_err);
        RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (p_session->SrvPrivKeyPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->SrvPrivKeyPool, p_session->SrvPrivKeyPtr, &local_err);
        RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (p_session->CliCertPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->CliCertPool, p_session->CliCertPtr, &local_err);
        RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      if (p_session->CliPrivKeyPtr != DEF_NULL) {
        Mem_DynPoolBlkFree(&NetSecure_DataPtr->CliPrivKeyPool, p_session->CliPrivKeyPtr, &local_err);
        RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    }

    p_sock->SecureSession = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                       NetSecure_SockCertKeyCfg()
 *
 * @brief    Configure server secure socket's certificate and key from buffers:
 *
 * @param    p_sock          Pointer to the server's socker to configure certificate and key.
 *
 * @param    sock_type       Secure socket type:
 *                               - NET_SOCK_SECURE_TYPE_SERVER
 *                               - NET_SOCK_SECURE_TYPE_CLIENT
 *
 * @param    p_buf_cert      Pointer to the certificate buffer to install.
 *
 * @param    buf_cert_size   Size of the certificate buffer to install.
 *
 * @param    p_buf_key       Pointer to the key buffer to install.
 *
 * @param    buf_key_size    Size of the key buffer to install.
 *
 * @param    fmt             Format of the certificate and key buffer to install.
 *                               - NET_SOCK_SECURE_CERT_KEY_FMT_PEM      Certificate and Key format is PEM.
 *                               - NET_SOCK_SECURE_CERT_KEY_FMT_DER      Certificate and Key format is DER.
 *
 * @param    cert_chain      Certificate points to a chain of certificate.
 *                               - DEF_YES   Certificate points to a chain  of certificate.
 *                               - DEF_NO    Certificate points to a single    certificate.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   Secure socket's certificate and key successfully configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/

#if (NET_SECURE_CFG_MAX_NBR_SOCK_SERVER > 0u \
     || NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0u)
CPU_BOOLEAN NetSecure_SockCertKeyCfg(NET_SOCK                     *p_sock,
                                     NET_SOCK_SECURE_TYPE         sock_type,
                                     const CPU_INT08U             *p_buf_cert,
                                     CPU_SIZE_T                   buf_cert_size,
                                     const CPU_INT08U             *p_buf_key,
                                     CPU_SIZE_T                   buf_key_size,
                                     NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                     CPU_BOOLEAN                  cert_chain,
                                     RTOS_ERR                     *p_err)
{
  mbedtls_x509_crt            *crt = DEF_NULL;
  mbedtls_pk_context          *privkey = DEF_NULL;
  CPU_INT32S                  rtn_val;
  NET_SECURE_MBED_TLS_SESSION *p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  PP_UNUSED_PARAM(p_sock);
  PP_UNUSED_PARAM(cert_chain);

  RTOS_ASSERT_DBG_ERR_SET((buf_cert_size <= NET_SECURE_CFG_MAX_CERT_LEN), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((buf_key_size <= NET_SECURE_CFG_MAX_KEY_LEN), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((fmt != NET_SOCK_SECURE_CERT_KEY_FMT_NONE), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               PARSE CERTIFICATE
  crt = (sock_type == NET_SOCK_SECURE_TYPE_CLIENT ? p_session->CliCertPtr : p_session->SrvCertPtr);
  rtn_val = mbedtls_x509_crt_parse(crt,
                                   (const unsigned char *) p_buf_cert,
                                   buf_cert_size);
  if (rtn_val != 0) {
    LOG_DBG(("NetSecure_SockCertKeyCfg failed. mbedtls_x509_crt_parse returned ", (x)rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (DEF_FAIL);
  }

  //                                                               PARSE PRIVATE KEY
  privkey = (sock_type == NET_SOCK_SECURE_TYPE_CLIENT ? p_session->CliPrivKeyPtr : p_session->SrvPrivKeyPtr);
  rtn_val = mbedtls_pk_parse_key(privkey,
                                 (const unsigned char *) p_buf_key,
                                 buf_key_size,
                                 DEF_NULL,
                                 0);
  if (rtn_val != 0) {
    LOG_DBG(("NetSecure_SockCertKeyCfg failed. mbedtls_pk_parse_key returned ", (x)rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (DEF_FAIL);
  }

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                       NetSecure_ClientCommonNameSet()
 *
 * @brief    Configure client secure socket's Common name.
 *
 * @param    p_sock          Pointer to the client's socket to configure common name.
 *
 * @param    p_common_name   Pointer to the common name.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   Client socket's common name successfully configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)
CPU_BOOLEAN NetSecure_ClientCommonNameSet(NET_SOCK *p_sock,
                                          CPU_CHAR *p_common_name,
                                          RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  CPU_INT32S                  rtn_val;

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  rtn_val = mbedtls_ssl_set_hostname(p_session->CtxPtr, p_common_name);
  if (rtn_val != 0) {
    LOG_DBG(("NetSecure_ClientCommonNameSet failed. mbedtls_ssl_set_hostname returned ", (x) - rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (DEF_NO);
  }

  return(DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                   NetSecure_ClientTrustCallBackSet()
 *
 * @brief    Configure client secure socket's trust callback function.
 *
 * @param    p_sock              Pointer to the client's socket to configure trust call back function.
 *
 * @param    p_callback_fnct     Pointer to the trust call back function
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   Client socket's trust call back function successfully configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)
CPU_BOOLEAN NetSecure_ClientTrustCallBackSet(NET_SOCK                   *p_sock,
                                             NET_SOCK_SECURE_TRUST_FNCT p_callback_fnct,
                                             RTOS_ERR                   *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;

  PP_UNUSED_PARAM(p_err);

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;
  p_session->Type = NET_SOCK_SECURE_TYPE_CLIENT;

  p_session->TrustCallBackFnctPtr = p_callback_fnct;

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                           NetSecure_SockConn()
 *
 * @brief    (1) Connect a socket to a remote host through an encrypted SSL handshake :
 *               - (a) Get & validate the SSL session of the connected socket
 *               - (b) Initialize the     SSL connect.
 *               - (c) Perform            SSL handshake.
 *
 * @param    p_sock  Pointer to a connected socket.
 *
 *
 * Argument(s) : p_sock      Pointer to a connected socket.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) When a client requires server authentication (authmode == MBEDTLS_SSL_VERIFY_REQUIRED
 *               or MBEDTLS_SSL_VERIFY_OPTIONAL), a CA certificate and the client's own certificate must
 *               have been configured before calling this function.
 *******************************************************************************************************/
void NetSecure_SockConn(NET_SOCK *p_sock,
                        RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION      *p_session;
  NET_SECURE_MBED_TLS_DATA         *p_data;
  NET_SOCK_SECURE_UNTRUSTED_REASON reason;
  CPU_INT32S                       rtn_val;
  CPU_INT32U                       cert_flags;
  CPU_CHAR                         *p_buf;
  CPU_INT08U                       rtn_buf_size;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_err);

  CORE_ENTER_ATOMIC();
  p_data = NetSecure_DataPtr;
  CORE_EXIT_ATOMIC();

  p_session = p_sock->SecureSession;
  p_session->Type = NET_SOCK_SECURE_TYPE_CLIENT;

  //                                                               --------------- SECURE CLIENT SETUP ----------------
  rtn_val = mbedtls_ssl_config_defaults(p_session->CfgPtr,
                                        MBEDTLS_SSL_IS_CLIENT,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT);
  if (rtn_val != 0) {
    LOG_DBG(("NetSecure_SockConn failed. mbedtls_ssl_config_defaults returned ", (x) - rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED    // Optional callback to provide certificate info
  mbedtls_ssl_conf_verify(p_session->CfgPtr, NetSecure_CertVerifyCallback, DEF_NULL);
#endif
  mbedtls_ssl_conf_authmode(p_session->CfgPtr, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(p_session->CfgPtr, &NetSecure_DataPtr->CA_CertList, DEF_NULL);
  mbedtls_ssl_conf_own_cert(p_session->CfgPtr, p_session->CliCertPtr, p_session->CliPrivKeyPtr);

  mbedtls_ssl_conf_rng(p_session->CfgPtr, mbedtls_ctr_drbg_random, &NetSecure_DataPtr->CtrDrbgPtr);
  mbedtls_ssl_conf_read_timeout(p_session->CfgPtr, p_sock->RxQ_SignalTimeout_ms);

  rtn_val = mbedtls_ssl_setup(p_session->CtxPtr, p_session->CfgPtr);
  if (rtn_val != 0) {
    LOG_DBG(("NetSecure_SockConn failed. mbedtls_ssl_setup returned ", (x) - rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
    goto exit;
  }

  mbedtls_ssl_set_bio(p_session->CtxPtr,
                      p_sock,
                      NetSecure_MbedTLS_Send,
                      NetSecure_MbedTLS_Receive,
                      NetSecure_MbedTLS_Receive_Timeout);

  //                                                               ---------------- SSL HANDSHAKE --------------------
  do {
    rtn_val = mbedtls_ssl_handshake(p_session->CtxPtr);
    if (rtn_val != 0 && rtn_val != MBEDTLS_ERR_SSL_WANT_READ && rtn_val != MBEDTLS_ERR_SSL_WANT_WRITE) {
      LOG_ERR(("mbedtls_ssl_handshake failed, handshake returned ", (x)(-rtn_val)));

      if (rtn_val != MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        LOG_ERR(("SSL Handshake error or error with the server certificate (reasons other than trust, revoked, expired, not effective)"));
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);                 // Is either a SSL handshake error or certificate error
        goto exit;
      } else {
        break;                                                  // Is a cert verification error: look further for cause
      }
    }
  } while (rtn_val != 0);

  //                                                               ----------- VERIFY SERVER CERTIFICATE -------------
  cert_flags = mbedtls_ssl_get_verify_result(p_session->CtxPtr);
  if (cert_flags != 0) {
    LOG_ERR(("Warning: Peer X.509 certificate validation failed."));

    p_buf = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_data->CertVerifyBufPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    //                                                             Analyse certificate info
    mbedtls_x509_crt_verify_info(p_buf, NET_SECURE_CERT_VERIFY_BUF_LEN, "  ", cert_flags);

    LOG_DBG(((s)p_buf));

    if (DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCERT_EXPIRED)
        || DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCERT_REVOKED)
        || DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCRL_EXPIRED)) {
      reason = NET_SOCK_SECURE_INVALID_DATE;
    } else if (DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCERT_NOT_TRUSTED)
               || DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCRL_NOT_TRUSTED)) {
      reason = NET_SOCK_SECURE_UNTRUSTED_BY_CA;
    } else if (DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCERT_FUTURE)
               || DEF_BIT_IS_SET(cert_flags, MBEDTLS_X509_BADCRL_FUTURE)) {
      reason = NET_SOCK_SECURE_INVALID_DATE;
    } else {
      reason = NET_SOCK_SECURE_UNKNOWN;
    }

    rtn_buf_size = mbedtls_x509_dn_gets(p_buf, NET_SECURE_CERT_VERIFY_BUF_LEN - 1, &NetSecure_DataPtr->CA_CertList.issuer);
    p_buf[rtn_buf_size] = 0;

    rtn_val = p_session->TrustCallBackFnctPtr((void *)p_buf, reason);

    if (p_buf != DEF_NULL) {
      Mem_DynPoolBlkFree(&p_data->CertVerifyBufPool, p_buf, p_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (rtn_val != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
      LOG_DBG(("  Client decides, through callback, to reject the connection."));
      goto exit;
    } else {
      LOG_DBG(("  Client decides, through callback, to trust the connection anyway."));
    }
  } else {
    LOG_VRB(("Peer X.509 certificate validation passed"));
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetSecure_SockAccept()
 *
 * @brief    (1) Return a new secure socket accepted from a listen socket :
 *               - (a) Get & validate SSL session of listening socket
 *               - (b) Initialize     SSL session of accepted  socket
 *               - (c) Initialize     SSL accept
 *               - (d) Perform        SSL handshake
 *
 * @param    p_sock_listen   Pointer to a listening socket.
 *
 * @param    p_sock_accept   Pointer to an accepted socket.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSecure_SockAccept(NET_SOCK *p_sock_listen,
                          NET_SOCK *p_sock_accept,
                          RTOS_ERR *p_err)

{
  NET_SECURE_MBED_TLS_SESSION *p_session_accept;
  NET_SECURE_MBED_TLS_SESSION *p_session_listen;
  CPU_INT32S                  rtn_val;

  PP_UNUSED_PARAM(p_sock_listen);

  NetSecure_InitSession(p_sock_accept, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("NetSecure_SockAccept failed.  NO session available"));
    goto exit;
  }

  p_session_listen = (NET_SECURE_MBED_TLS_SESSION *)p_sock_listen->SecureSession;
  p_session_accept = (NET_SECURE_MBED_TLS_SESSION *)p_sock_accept->SecureSession;
  p_session_accept->Type = NET_SOCK_SECURE_TYPE_SERVER;

  Mem_Copy(p_session_accept->SrvCertPtr, p_session_listen->SrvCertPtr, sizeof(mbedtls_x509_crt));
  Mem_Copy(p_session_accept->SrvPrivKeyPtr, p_session_listen->SrvPrivKeyPtr, sizeof(mbedtls_pk_context));
  Mem_Copy(p_session_accept->CliCertPtr, p_session_listen->CliCertPtr, sizeof(mbedtls_x509_crt));
  Mem_Copy(p_session_accept->CliPrivKeyPtr, p_session_listen->CliPrivKeyPtr, sizeof(mbedtls_pk_context));

  //                                                               --------------- SECURE SERVER SETUP ----------------

  rtn_val = mbedtls_ssl_config_defaults(p_session_accept->CfgPtr,
                                        MBEDTLS_SSL_IS_SERVER,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT);
  if (rtn_val != 0) {
    LOG_ERR(("NetSecure_SockAccept failed. mbedtls_ssl_config_defaults returned ", (x)(-rtn_val)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED      // Optional callback to provide certificate info
  mbedtls_ssl_conf_verify(p_session_accept->CfgPtr, NetSecure_CertVerifyCallback, DEF_NULL);
#endif
#if  NET_SECURE_CFG_CLIENT_AUTH_EN == DEF_ENABLED
  mbedtls_ssl_conf_authmode(p_session_accept->CfgPtr, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(p_session_accept->CfgPtr, &NetSecure_DataPtr->CA_CertList, DEF_NULL);
#else
  mbedtls_ssl_conf_authmode(p_session_accept->CfgPtr, MBEDTLS_SSL_VERIFY_NONE);
#endif
  mbedtls_ssl_conf_own_cert(p_session_accept->CfgPtr, p_session_accept->SrvCertPtr, p_session_accept->SrvPrivKeyPtr);

  mbedtls_ssl_conf_rng(p_session_accept->CfgPtr, mbedtls_ctr_drbg_random, &NetSecure_DataPtr->CtrDrbgPtr);
  mbedtls_ssl_conf_read_timeout(p_session_accept->CfgPtr, p_sock_accept->RxQ_SignalTimeout_ms);

  rtn_val = mbedtls_ssl_setup(p_session_accept->CtxPtr, p_session_accept->CfgPtr);
  if (rtn_val != 0) {
    LOG_ERR(("NetSecure_SockAccept failed. mbedtls_ssl_setup returned ", (x)(-rtn_val)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
    goto exit;
  }

  mbedtls_ssl_set_bio(p_session_accept->CtxPtr,
                      p_sock_accept,
                      NetSecure_MbedTLS_Send,
                      NetSecure_MbedTLS_Receive,
                      NetSecure_MbedTLS_Receive_Timeout);

  do {
    rtn_val = mbedtls_ssl_handshake(p_session_accept->CtxPtr);
  } while (rtn_val == MBEDTLS_ERR_SSL_WANT_READ || rtn_val == MBEDTLS_ERR_SSL_WANT_WRITE);

  if (rtn_val != 0) {
    LOG_ERR(("mbedtls_ssl_handshake returned ", (x)(-rtn_val)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
  } else {
    LOG_VRB(("mbedtls_ssl_handshake successful"));
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetSecure_SockRxDataHandler()
 *
 * @brief    Receive clear data through a secure socket :
 *               - (a) Get & validate the SSL session of the receiving socket
 *               - (b) Receive the data
 *
 * @param    p_sock          Pointer to a receive socket.
 *
 * @param    p_data_buf      Pointer to an application data buffer that will receive the socket's
 *                           received data.
 *
 * @param    data_buf_len    Size of the application data buffer (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of positive data octets received, if NO error(s).
 *           NET_SOCK_BSD_ERR_RX, otherwise.
 *******************************************************************************************************/
NET_SOCK_RTN_CODE NetSecure_SockRxDataHandler(NET_SOCK   *p_sock,
                                              void       *p_data_buf,
                                              CPU_INT16U data_buf_len,
                                              RTOS_ERR   *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  CPU_INT32S                  rtn_val;

  p_session = p_sock->SecureSession;

  rtn_val = mbedtls_ssl_read(p_session->CtxPtr,
                             (CPU_INT08U *)p_data_buf,
                             data_buf_len);

  if (rtn_val < 0) {
    if ((rtn_val == MBEDTLS_ERR_SSL_WANT_WRITE) || (rtn_val == MBEDTLS_ERR_SSL_WANT_READ)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);
      return (NET_SOCK_BSD_ERR_RX);
    }

    if (rtn_val == MBEDTLS_ERR_SSL_TIMEOUT) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      return (NET_SOCK_BSD_ERR_RX);
    }

    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (NET_SOCK_BSD_ERR_RX);
  }

  return (rtn_val);
}

/****************************************************************************************************//**
 *                                       NetSecure_SockRxIsDataPending()
 *
 * @brief    Is data pending in SSL receive queue.
 *
 * @param    p_sock  Pointer to a receive socket.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, If data is pending.
 *           DEF_NO,  Otherwise
 *******************************************************************************************************/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
CPU_BOOLEAN NetSecure_SockRxIsDataPending(NET_SOCK *p_sock,
                                          RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  CPU_SIZE_T                  bytes_nbr;

  PP_UNUSED_PARAM(p_err);

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  bytes_nbr = mbedtls_ssl_get_bytes_avail(p_session->CtxPtr);

  if (bytes_nbr != 0) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetSecure_SockTxDataHandler()
 *
 * @brief    Transmit clear data through a secure socket :
 *               - (a) Get & validate the SSL session of the transmitting socket
 *               - (b) Transmit the data
 *
 * @param    p_sock          Pointer to a transmit socket.
 *
 * @param    p_data_buf      Pointer to application data to transmit.
 *
 * @param    data_buf_len    Length of  application data to transmit (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of positive data octets transmitted, if NO error(s).
 *           NET_SOCK_BSD_ERR_RX, otherwise.
 *******************************************************************************************************/
NET_SOCK_RTN_CODE NetSecure_SockTxDataHandler(NET_SOCK   *p_sock,
                                              void       *p_data_buf,
                                              CPU_INT16U data_buf_len,
                                              RTOS_ERR   *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  CPU_INT32S                  rtn_val;

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  rtn_val = mbedtls_ssl_write(p_session->CtxPtr,
                              (const CPU_INT08U *)p_data_buf,
                              data_buf_len);

  if (rtn_val < 0) {
    if ((rtn_val == MBEDTLS_ERR_SSL_WANT_WRITE) || (rtn_val == MBEDTLS_ERR_SSL_WANT_READ)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);
      return (NET_SOCK_BSD_ERR_TX);
    }

    if (rtn_val == MBEDTLS_ERR_SSL_TIMEOUT) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      return (NET_SOCK_BSD_ERR_TX);
    }

    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (NET_SOCK_BSD_ERR_TX);
  }

  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetSecure_SockClose()
 *
 * @brief    (1) Close the secure socket :
 *               - (a) Get & validate the SSL session of the socket to close
 *               - (b) Transmit close notify alert to the peer
 *               - (c) Free the SSL session buffer
 *
 * @param    p_sock  Pointer to a socket.
 *
 *
 * Argument(s) : p_sock      Pointer to a socket.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
void NetSecure_SockClose(NET_SOCK *p_sock,
                         RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  RTOS_ERR                    local_err;

  LOG_VRB(("NetSecure_SockClose: Start"));

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  mbedtls_ssl_free(p_session->CtxPtr);
  mbedtls_ssl_config_free(p_session->CfgPtr);

  if (p_session != DEF_NULL) {
    if (p_session->CtxPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->CtxPool, p_session->CtxPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (p_session->CfgPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->CfgPool, p_session->CfgPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (p_session->SrvCertPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->SrvCertPool, p_session->SrvCertPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (p_session->SrvPrivKeyPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->SrvPrivKeyPool, p_session->SrvPrivKeyPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (p_session->CliCertPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->CliCertPool, p_session->CliCertPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (p_session->CliPrivKeyPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetSecure_DataPtr->CliPrivKeyPool, p_session->CliPrivKeyPtr, &local_err);
      RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    Mem_DynPoolBlkFree(&NetSecure_DataPtr->SessionPool, p_sock->SecureSession, &local_err);
    RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  p_sock->SecureSession = DEF_NULL;

  LOG_VRB(("NetSecure_SockClose: Normal exit"));
}

/****************************************************************************************************//**
 *                                       NetSecure_SockCloseNotify()
 *
 * @brief    Transmit the close notify alert to the peer through a SSL session.
 *
 * @param    p_sock  Pointer to a socket.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) If the server decides to close the connection, it SHOULD send a close notify
 *               alert to the connected peer prior to perform the socket close operations.
 *
 * @note     (2) This function will be called twice during a socket close process but the
 *               close notify alert will only transmitted during the first call.
 *               - (a) The error code that might me returned by 'SSL_shutdown()' is ignored because the
 *                   connection can be closed by the client. In that case, the SSL session will no
 *                   longer be valid and it will be impossible to send the close notify alert through
 *                   that session.
 *******************************************************************************************************/
void NetSecure_SockCloseNotify(NET_SOCK *p_sock,
                               RTOS_ERR *p_err)
{
  NET_SECURE_MBED_TLS_SESSION *p_session;
  CPU_INT32S                  rtn_val;

  p_session = (NET_SECURE_MBED_TLS_SESSION *)p_sock->SecureSession;

  rtn_val = mbedtls_ssl_close_notify(p_session->CtxPtr);

  if (rtn_val != 0 && rtn_val != MBEDTLS_ERR_SSL_WANT_READ && rtn_val != MBEDTLS_ERR_SSL_WANT_WRITE) {
    LOG_ERR(("NetSecure_SockCloseNotify: mbedtls_ssl_close_notify returned ", (x) - rtn_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SSL);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetSecure_EntropySource()
 *
 * @brief    Entropy source for mbed TLS.
 *
 * @param    p_data      Pointer callback specific data.
 *
 * @param    p_output    Pointer to variable that will receive the generated entropy.
 *
 * @param    len         Maximum size to provide.
 *
 * @param    p_olen      Pointer to the actual number of bytes that was put into the buffer.
 *
 * Return(s)   : 0
 *
 * Note(s)     : (1) This is a callback function of type 'mbedtls_entropy_f_source_ptr', registered with
 *                   mbed TLS in NetSecure_Init()
 *
 *******************************************************************************************************/
static CPU_INT32S NetSecure_EntropySource(void       *p_data,
                                          CPU_INT08U *p_output,
                                          CPU_SIZE_T len,
                                          CPU_SIZE_T *p_olen)
{
  CPU_INT08U *p_out = p_output;
  CPU_SIZE_T i;

  PP_UNUSED_PARAM(p_data);

  *p_olen = 1;

  for (i = 0; i < len; i++) {
    *p_out = (CPU_INT08U)Math_Rand();
    p_out++;
    *p_olen += 1;
  }

  return (0);
}

/****************************************************************************************************//**
 *                                       NetSecure_CertVerifyCallback()
 *
 * @brief    Callback for certificate verification.
 *
 * @param    p_data  Pointer callback specific data.
 *
 * @param    p_crt   Pointer to certificate being validated.
 *
 * @param    depth   Certificate chain depth.
 *
 * @param    flags   Certificate flags containing X509 verify codes.
 *
 * Return(s)   : 0
 *
 * Note(s)     : (1) This callback function is called for displaying information for each certificate
 *                   in the chain.
 *
 *******************************************************************************************************/

#if  NET_SECURE_CERT_VERIFY_DBG_CALLBACK_EN == DEF_ENABLED
static int NetSecure_CertVerifyCallback(void             *p_cb_data,
                                        mbedtls_x509_crt *p_crt,
                                        int              depth,
                                        uint32_t         *flags)
{
  NET_SECURE_MBED_TLS_DATA *p_data;
  CPU_CHAR                 *p_buf;
  RTOS_ERR                 local_err;

  PP_UNUSED_PARAM(p_cb_data);

  CORE_ENTER_ATOMIC();
  p_data = NetSecure_DataPtr;
  CORE_EXIT_ATOMIC();

  p_buf = (CPU_CHAR *)Mem_DynPoolBlkGet(&p_data->CertDebugBufPool, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  LOG_VRB(("\nVerify requested for (Depth", (u)depth, "):\n"));
  mbedtls_x509_crt_info(p_buf, NET_SECURE_CERT_VERIFY_DBG_BUF_LEN - 1, "", p_crt);
  LOG_VRB(((s)p_buf));                                          // Display entire certificate chain info

  if ((*flags) == 0) {
    LOG_VRB(("  This certificate has no flags (no validation error)\n"));
  } else {
    mbedtls_x509_crt_verify_info(p_buf, NET_SECURE_CERT_VERIFY_DBG_BUF_LEN, "  ! ", *flags);
    LOG_VRB(((s)p_buf));
  }

  if (p_buf != DEF_NULL) {
    Mem_DynPoolBlkFree(&p_data->CertDebugBufPool, p_buf, &local_err);
    RTOS_ASSERT_CRITICAL_ERR_SET(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, local_err, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return (0u);
}
#endif

/****************************************************************************************************//**
 *                                       NetSecure_MbedTLS_Receive()
 *
 * @brief    Read at most 'len' characters.
 *
 * @param    p_ctx   Pointer to a socket.
 *
 * @param    p_buf   Pointer to a data buffer that will receive the socket's received
 *                   data.
 *
 * @param    len     Length of the data buffer (in octets).
 *
 * Return(s)   : Number of positive data octets received, if NO error(s)
 *
 * Note(s)     : none.
 *
 *******************************************************************************************************/
static CPU_INT32S NetSecure_MbedTLS_Receive(void       *p_ctx,
                                            CPU_INT08U *p_buf,
                                            CPU_SIZE_T len)
{
  CPU_INT32S rtn_code;
  NET_SOCK   *p_sock = (NET_SOCK *)p_ctx;
  RTOS_ERR   local_err;

  if (p_sock->ID < 0) {
    return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  rtn_code = NetSock_RxDataHandlerStream(p_sock->ID,
                                         p_sock,
                                         (void *)p_buf,
                                         len,
                                         NET_SOCK_FLAG_NONE,
                                         DEF_NULL,
                                         0,
                                         &local_err);

  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // Data received.
      break;

    case RTOS_ERR_WOULD_BLOCK:                                  // Transitory rx err(s).
      return (MBEDTLS_ERR_SSL_WANT_READ);

    case RTOS_ERR_TIMEOUT:
      return(MBEDTLS_ERR_SSL_TIMEOUT);

    case RTOS_ERR_NET_CONN_CLOSED_FAULT:                        // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSE_RX:
      return (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY);

    default:                                                    // Fatal err.
      return (MBEDTLS_ERR_NET_RECV_FAILED);
  }

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                   NetSecure_MbedTLS_Receive_Timeout()
 *
 * @brief    Read at most 'len' characters, blocking for at most 'timeout_ms' ms
 *
 * @param    p_ctx       Pointer to a socket.
 *
 * @param    p_buf       Pointer to a data buffer that will receive the socket's received
 *                       data.
 *
 * @param    len         Length of the data buffer (in octets).
 *
 * @param    timeout_ms  Timeout of the receive operation, in ms.
 *
 * Return(s)   : Number of positive data octets received, if NO error(s)
 *
 * Note(s)     : none.
 *
 *******************************************************************************************************/
static int NetSecure_MbedTLS_Receive_Timeout(void          *p_ctx,
                                             unsigned char *p_buf,
                                             size_t        len,
                                             uint32_t      timeout_ms)
{
  CPU_INT32S rtn_code = 0;
  NET_SOCK   *p_sock = (NET_SOCK *)p_ctx;
  CPU_INT32U timeout_s;
  RTOS_ERR   local_err;

  if (p_sock->ID < 0) {
    rtn_code = MBEDTLS_ERR_NET_INVALID_CONTEXT;
    goto exit;
  }

  timeout_s = timeout_ms / 1000;

  //                                                               --------- SET SOCKET CONNECTION RX TIMEOUT ---------
  Net_GlobalLockRelease();
  (void)NetSock_OptSet(p_sock->ID,
                       NET_SOCK_PROTOCOL_SOCK,
                       NET_SOCK_OPT_SOCK_RX_TIMEOUT,
                       (void *)&timeout_s,
                       sizeof(timeout_s),
                       &local_err);
  Net_GlobalLockAcquire((void *)NetSecure_MbedTLS_Receive_Timeout);

  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  rtn_code = NetSecure_MbedTLS_Receive(p_ctx, p_buf, len);

exit:
  return (rtn_code);
}

/****************************************************************************************************//**
 *                                           NetSecure_MbedTLS_Send()
 *
 * @brief    Write at most 'len' characters
 *
 * @param    p_ctx   Pointer to a socket.
 *
 * @param    p_buf   Pointer to a data buffer that contains the data to send.
 *
 * @param    len     Length of the data (in octets).
 *
 * Return(s)   : Number of positive data octets transmitted, if NO error(s).
 *
 *               MBEDTLS_ERR_*, otherwise.
 *
 * Note(s)     : none.
 *
 *******************************************************************************************************/
static CPU_INT32S NetSecure_MbedTLS_Send(void             *p_ctx,
                                         const CPU_INT08U *p_buf,
                                         CPU_SIZE_T       len)
{
  CPU_INT32S rtn_code;
  NET_SOCK   *p_sock = (NET_SOCK *)p_ctx;
  RTOS_ERR   local_err;

  if (p_sock->ID < 0) {
    return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  rtn_code = NetSock_TxDataHandlerStream(p_sock->ID,
                                         p_sock,
                                         (void *)p_buf,
                                         len,
                                         NET_SOCK_FLAG_NONE,
                                         &local_err);

  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // Data transmitted.
      break;

    case RTOS_ERR_POOL_EMPTY:                                   // Transitory Errors
      return (MBEDTLS_ERR_SSL_ALLOC_FAILED);

    case RTOS_ERR_NET_IF_LINK_DOWN:
      return (MBEDTLS_ERR_SSL_INTERNAL_ERROR);

    case RTOS_ERR_TIMEOUT:
      return (MBEDTLS_ERR_SSL_TIMEOUT);

    case RTOS_ERR_WOULD_BLOCK:
      return (MBEDTLS_ERR_SSL_WANT_WRITE);

    case RTOS_ERR_NET_CONN_CLOSE_RX:                            // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSED_FAULT:
      return (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY);

    default:                                                    // Fatal err.
      return (MBEDTLS_ERR_NET_SEND_FAILED);
  }

  return (rtn_code);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL
