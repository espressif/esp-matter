/***************************************************************************//**
 * @file
 * @brief Network Core Example - TLS/SSL Client
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>
#include <net_cfg.h>

#if  (defined(RTOS_MODULE_NET_SSL_TLS_AVAIL) \
  && NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_secure.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  TCP_SERVER_IP_ADDR     "98.139.211.125"
#define  TCP_SERVER_PORT        12345

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

// CA certificate:
// (1) Can be obtained on CA website.
// (2) Can be generated using a tool such as OpenSSL.
CPU_CHAR *p_cert =
  "MIIDVDCCAjygAwIBAgIDAjRWMA0GCSqGSIb3DQEBBQUAMEIxCzAJBgNVBAYTAlVT"
  "MRYwFAYDVQQKEw1HZW9UcnVzdCBJbmMuMRswGQYDVQQDExJHZW9UcnVzdCBHbG9i"
  "YWwgQ0EwHhcNMDIwNTIxMDQwMDAwWhcNMjIwNTIxMDQwMDAwWjBCMQswCQYDVQQG"
  "EwJVUzEWMBQGA1UEChMNR2VvVHJ1c3QgSW5jLjEbMBkGA1UEAxMSR2VvVHJ1c3Qg"
  "R2xvYmFsIENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2swYYzD9"
  "9BcjGlZ+W988bDjkcbd4kdS8odhM+KhDtgPpTSEHCIjaWC9mOSm9BXiLnTjoBbdq"
  "fnGk5sRgprDvgOSJKA+eJdbtg/OtppHHmMlCGDUUna2YRpIuT8rxh0PBFpVXLVDv"
  "iS2Aelet8u5fa9IAjbkU+BQVNdnARqN7csiRv8lVK83Qlz6cJmTM386DGXHKTubU"
  "1XupGc1V3sjs0l44U+VcT4wt/lAjNvxm5suOpDkZALeVAjmRCw7+OC7RHQWa9k0+"
  "bw8HHa8sHo9gOeL6NlMTOdReJivbPagUvTLrGAMoUgRx5aszPeE4uwc2hGKceeoW"
  "MPRfwCvocWvk+QIDAQABo1MwUTAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTA"
  "ephojYn7qwVkDBF9qn1luMrMTjAfBgNVHSMEGDAWgBTAephojYn7qwVkDBF9qn1l"
  "uMrMTjANBgkqhkiG9w0BAQUFAAOCAQEANeMpauUvXVSOKVCUn5kaFOSPeCpilKIn"
  "Z57QzxpeR+nBsqTP3UEaBU6bS+5Kb1VSsyShNwrrZHYqLizz/Tt1kL/6cdjHPTfS"
  "tQWVYrmm3ok9Nns4d0iXrKYgjy6myQzCsplFAMfOEVEiIuCl6rYVSAlk6l5PdPcF"
  "PseKUgzbFbS9bZvlxrFUaKnjaZC2mqUPuLk/IH2uSrW4nOQdtqvmlKXBx4Ot2/Un"
  "hw4EbNX/3aBd7YdStysVAq45pmp06drE57xNNB6pXE0zX5IJL4hmXXeXxx12E6nV"
  "5fEWCRE11azbJHFwLJhWC9kXtNHjUStedejV0NxPNO3CBWaAocvmMw==";

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN Ex_ClientCertTrustCallBackFnct(void                             *p_cert_dn,
                                                  NET_SOCK_SECURE_UNTRUSTED_REASON reason);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    Ex_Net_SockSecureClientConnect()
 *
 * @brief  (1) Initialize a client secure socket:
 *           - (a) Install CA certificate.
 *           - (b) Open a TCP socket.
 *           - (c) Configure socket's option to be secure.
 *           - (d) Connect the socket, establish a secure connection with the server.
 *
 * @return  Socket ID, if successfully connected.
 *          -1,        Otherwise.
 *******************************************************************************************************/
CPU_INT16S Ex_Net_SockSecureClientConnect(void)
{
  NET_SOCK_ID        sock_id;
  NET_SOCK_ADDR_IPv4 addr_server;
  CPU_INT32U         len_addr_server;
  CPU_INT32U         len;
  RTOS_ERR           err;

  //                                                               -------------- INSTALL CA CERTIFICATE --------------
  len = Str_Len(p_cert);
  NetSecure_CA_CertInstall(p_cert, len, NET_SOCK_SECURE_CERT_KEY_FMT_PEM, &err);

  //                                                               ------------------ OPEN THE SOCKET -----------------
  sock_id = NetApp_SockOpen(NET_SOCK_PROTOCOL_FAMILY_IP_V4,
                            NET_SOCK_TYPE_STREAM,
                            NET_SOCK_PROTOCOL_TCP,
                            3,
                            5,
                            &err);
  if (err.Code != RTOS_ERR_NONE) {
    return (-1);
  }

  //                                                               ------------ CONFIGURE SOCKET AS SECURE ------------
  (void)NetSock_CfgSecure(sock_id,                              // First the socket option secure must be set.
                          DEF_YES,
                          &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetApp_SockClose(sock_id, 1, &err);
    return (-1);
  }

  (void)NetSock_CfgSecureClientCommonName(sock_id,              // Configure the common name of the server ...
                                          "domain_name.com",    // certificate, most of the time it is the Domain name.
                                          &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetApp_SockClose(sock_id, 1, &err);
    return (-1);
  }

  //                                                               Configure the callback function to call if the ...
  //                                                               ... server certificate is not trusted. So the  ...
  //                                                               ... connection can be allow even if the        ...
  //                                                               ... certificate is not trusted.
  (void)NetSock_CfgSecureClientTrustCallBack(sock_id,
                                             &Ex_ClientCertTrustCallBackFnct,
                                             &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetApp_SockClose(sock_id, 1, &err);
    return (-1);
  }

  //                                                               ------------- ESTABLISH TCP CONNECTION -------------
  Mem_Clr((void *)&addr_server, NET_SOCK_ADDR_IPv4_SIZE);

  addr_server.AddrFamily = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR,
                                              (void *)&addr_server.Addr,
                                              sizeof(addr_server.Addr),
                                              &err);
  addr_server.Port = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);
  len_addr_server = sizeof(addr_server);

  (void)NetApp_SockConn(sock_id,                                // Connect to server using TLS/SSL.
                        (NET_SOCK_ADDR *)&addr_server,
                        len_addr_server,
                        3,
                        5,
                        5,
                        &err);
  if (err.Code != RTOS_ERR_NONE) {
    NetApp_SockClose(sock_id, 1, &err);
    return (-1);
  }

  // Now all the data transfered on this socket is encrypted.
  // You just have to use any socket API such as NetApp_Rx() or NetApp_Tx().

  return (sock_id);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    Ex_ClientCertTrustCallBackFnct()
 *
 * @brief  Function called when the server's certificate is not trusted.
 *
 * @param  p_cert_dn  Pointer to certificate distinguish name.
 *
 * @param  reason     Reason why the certificate is not trusted:
 *                        - NET_SOCK_SECURE_UNTRUSTED_BY_CA
 *                        - NET_SOCK_SECURE_EXPIRE_DATE
 *                        - NET_SOCK_SECURE_INVALID_DATE
 *                        - NET_SOCK_SECURE_SELF_SIGNED
 *                        - NET_SOCK_SECURE_UNKNOWN
 *
 * @return  DEF_OK,   The connection can be established even if the certificated is not trusted.
 *          DEF_FAIL, Connection refused.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_ClientCertTrustCallBackFnct(void                             *p_cert_dn,
                                                  NET_SOCK_SECURE_UNTRUSTED_REASON reason)
{
  PP_UNUSED_PARAM(p_cert_dn);
  PP_UNUSED_PARAM(reason);

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_SSL_TLS_AVAIL
