
/*******************************************************************************
* @file  tlsio_openssl_sapi_wrapper.c
* @brief 
*******************************************************************************
* Copyright (c) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE file in the project root for full license information.
*
*******************************************************************************
*
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/**
 * Includes
 */
#ifdef AZURE_ENABLE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "azure_c_shared_utility/lock.h"
#include "tlsio_sapi.h"
#include "tlsio_openssl_sapi.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/const_defines.h"
#include "rsi_socket.h"
#include "rsi_wlan_apis.h"
#include "rsi_common_apis.h"
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_wlan.h"
#include "rsi_nwk.h"
#include "rsi_timer.h"
#include "rsi_utils.h"
#include "azure_iot_config.h"

typedef enum TLSIO_STATE_TAG {
  TLSIO_STATE_NOT_OPEN,
  TLSIO_STATE_OPENING_UNDERLYING_IO,
  TLSIO_STATE_IN_HANDSHAKE,
  //! TLSIO_STATE_HANDSHAKE_FAILED is an ephemeral state signalling successful socket
  //! operation but with rejected handshake. The tlsio will never be in this state
  //! At the start of any of the API calls.
  TLSIO_STATE_HANDSHAKE_FAILED,
  TLSIO_STATE_OPEN,
  TLSIO_STATE_CLOSING,
  TLSIO_STATE_ERROR
} TLSIO_STATE;

typedef enum TLSIO_VERSION_TAG {
  VERSION_1_0,
  VERSION_1_1,
  VERSION_1_2,
} TLSIO_VERSION;

static bool is_an_opening_state(TLSIO_STATE state)
{
  //! TLSIO_STATE_HANDSHAKE_FAILED is deliberately not one of these states.
  return state == TLSIO_STATE_OPENING_UNDERLYING_IO || state == TLSIO_STATE_IN_HANDSHAKE;
}

typedef struct TLS_IO_INSTANCE_TAG {
  XIO_HANDLE underlying_io;
  ON_BYTES_RECEIVED on_bytes_received;
  ON_IO_OPEN_COMPLETE on_io_open_complete;
  ON_IO_CLOSE_COMPLETE on_io_close_complete;
  ON_IO_ERROR on_io_error;
  void *on_bytes_received_context;
  void *on_io_open_complete_context;
  void *on_io_close_complete_context;
  void *on_io_error_context;
  TLSIO_STATE tlsio_state;
  char *certificate;
  const char *x509_certificate;
  const char *x509_private_key;
  const char *ca_root;
  int socket_id;
} TLS_IO_INSTANCE;

struct CRYPTO_dynlock_value {
  LOCK_HANDLE lock;
};
TLSIO_CONFIG tls_io_conf;

static const char *const OPTION_UNDERLYING_IO_OPTIONS = "underlying_io_options";
#define SSL_DO_HANDSHAKE_SUCCESS 1
unsigned char recv_buffer[1046];
rsi_fd_set read_fds;

int sent_flag = 0;

//! This function will clone an option given by name and value
static void *tlsio_ssl_CloneOption(const char *name, const void *value)
{
  return 0;
}
int tlsio_openssl_init(void)
{
  return 0;
}

int rsi_load_certificates(const char *optionName, uint8_t *buffer, uint32_t certificate_length)
{
  int result = 0;

  //! Root CA
  if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0) {
    if (buffer != NULL) {
      result = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, NULL, 0);

      //! Load CA certificate
      result = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, (uint8_t *)buffer, certificate_length);
      if (result != RSI_SUCCESS) {
        return result;
      }
    }
  } else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0) {
    if (buffer != NULL) {
      result = rsi_wlan_set_certificate(RSI_SSL_CLIENT, NULL, 0);

      //! Load Public Key 
      result = rsi_wlan_set_certificate(RSI_SSL_CLIENT, (uint8_t *)buffer, certificate_length);
      if (result != RSI_SUCCESS) {
        return result;
      }
    }
  } else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0) {
    if (buffer != NULL) {

      result = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, NULL, 0);

      //! Load Private Key 
      result = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, (uint8_t *)buffer, certificate_length);
      if (result != RSI_SUCCESS) {
        return result;
      }
    }
  } else {
    result = MU_FAILURE;
  }
  return result;
}

//! This function destroys an option previously created
static void tlsio_ssl_DestroyOption(const char *name, const void *value)
{
  //! Since all options for this layer are actually string copies., disposing of one is just calling free
  if ((name == NULL) || (value == NULL)) {
    LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
  } else {
    if ((strcmp(name, OPTION_TRUSTED_CERT) == 0) || (strcmp(name, OPTION_OPENSSL_CIPHER_SUITE) == 0)
        || (strcmp(name, SU_OPTION_X509_CERT) == 0) || (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0)
        || (strcmp(name, OPTION_X509_ECC_CERT) == 0) || (strcmp(name, OPTION_X509_ECC_KEY) == 0)
        || (strcmp(name, OPTION_TLS_VERSION) == 0)) {
      free((void *)value);
    } else if ((strcmp(name, "tls_validation_callback") == 0) || (strcmp(name, "tls_validation_callback_data") == 0)) {
      // nothing to free.
    } else if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0) {
      OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
    } else {
      LogError("not handled option : %s", name);
    }
  }
}

static void indicate_open_complete(TLS_IO_INSTANCE *tls_io_instance, IO_OPEN_RESULT open_result)
{
  if (tls_io_instance->on_io_open_complete == NULL) {
    LogError("NULL on_io_open_complete.");
  } else {
    //! Reports the open operation status 
    tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, open_result);
  }
}

static void close_ssl_instance(TLS_IO_INSTANCE *tls_io_instance)
{
  rsi_shutdown(tls_io_conf.socket_id, 0);
}
static void on_underlying_io_close_complete(void *context)
{
  TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

  switch (tls_io_instance->tlsio_state) {
    default:
    case TLSIO_STATE_NOT_OPEN:
    case TLSIO_STATE_OPEN:
    case TLSIO_STATE_IN_HANDSHAKE:
    case TLSIO_STATE_HANDSHAKE_FAILED:
    case TLSIO_STATE_ERROR:
      break;

    case TLSIO_STATE_OPENING_UNDERLYING_IO:
      tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
      indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
      break;

    case TLSIO_STATE_CLOSING:
      tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

      if (tls_io_instance->on_io_close_complete != NULL) {
        tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
      }
      break;
  }
}

//! This API will read data from socket
static int decode_ssl_received_bytes(TLS_IO_INSTANCE *tls_io_instance)
{
  int result = 0, status = 0;
  int rcv_bytes = 1;
  struct rsi_timeval timeout;
  rsi_timer_instance_t tim;

  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_usec = RECV_TIMEOUT_VAL * 1000;
  timeout.tv_sec  = 0;

  status = rsi_setsockopt(tls_io_conf.socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  if (status != RSI_SUCCESS) {
    status = rsi_wlan_socket_get_status(tls_io_conf.socket_id);
    return result;
  }
	 
  rsi_init_timer(&tim,RECV_TIMEOUT_VAL);

  do {
    rcv_bytes = rsi_recv(tls_io_conf.socket_id, recv_buffer, sizeof(recv_buffer), 0);
    if (rcv_bytes > 0) {
      if (tls_io_instance->on_bytes_received == NULL) {
        LogError("NULL on_bytes_received.");
      } else {
        tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, recv_buffer, rcv_bytes);
      }
    }

    // RSC-10019 : Enable application to send data when continously receiving messages from cloud
    if(rsi_timer_expired(&tim))
    {
      /* Start timer for TIMEOUT_VAL_MS. On expiration brek from the while loop, 
       * to allow sending of queued messages from application to cloud */
      break;
    }
  } while (rcv_bytes > 0);

  return result;
}

//! This function will connects to the network 
static int ConnecttoNetwork(TLS_IO_INSTANCE *tls_io_instance, uint8_t flags, char *addr, int dst_port, int src_port)
{
  int type = SOCK_STREAM;
  struct rsi_sockaddr_in address, clientAddr;
  struct rsi_sockaddr_in6 address_v6, clientAddr_v6;
  int rc = -1, status = 0;

  memset(&address, 0, sizeof(address));
  memset(&address_v6, 0, sizeof(address_v6));

  memset(&address, 0, sizeof(address));
  memset(&clientAddr, 0, sizeof(clientAddr));

  if (flags == RSI_IPV6) {
    address_v6.sin6_family = AF_INET6;

    address_v6.sin6_port = htons(dst_port);

    memcpy(&address_v6.sin6_addr._S6_un._S6_u8, addr, RSI_IPV6_ADDRESS_LENGTH);

    if (flags & RSI_SSL_ENABLE)
      tls_io_instance->socket_id = rsi_socket(AF_INET6, type, RSI_SOCKET_FEAT_SSL);
    else
      tls_io_instance->socket_id = rsi_socket(AF_INET6, type, 0);

    clientAddr_v6.sin6_family = AF_INET6;
    clientAddr_v6.sin6_port   = htons(src_port);
  } else {
    //! Set family type
    address.sin_family = AF_INET;

    //! Set local port number
    address.sin_port = htons(dst_port);

    address.sin_addr.s_addr = rsi_bytes4R_to_uint32((uint8_t *)addr);

    if (flags & RSI_SSL_ENABLE)
      tls_io_instance->socket_id = rsi_socket(AF_INET, type, RSI_SOCKET_FEAT_SSL);
    else
      tls_io_instance->socket_id = rsi_socket(AF_INET, type, 0);

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port   = htons(src_port);

    //! Set all bits of the padding field to 0 
    memset(clientAddr.sin_zero, '\0', sizeof(clientAddr.sin_zero));
  }
  if (tls_io_instance->socket_id == -1) {
    status = rsi_wlan_get_status();
    return status;
  }
  if (flags == RSI_IPV6) {
    //! Bind socket
    status = rsi_bind(tls_io_instance->socket_id, (struct rsi_sockaddr *)&clientAddr_v6, sizeof(clientAddr_v6));
  } else {
    //! Bind socket
    status = rsi_bind(tls_io_instance->socket_id, (struct rsi_sockaddr *)&clientAddr, sizeof(clientAddr));
  }
  if (status != 0) {
    status = rsi_wlan_get_status();
    return status;
  }
  if (flags == RSI_IPV6) {
    rc = rsi_connect(tls_io_instance->socket_id, (struct rsi_sockaddr *)&address_v6, sizeof(address_v6));
  } else {
    rc = rsi_connect(tls_io_instance->socket_id, (struct rsi_sockaddr *)&address, sizeof(address));
  }
  tls_io_conf.socket_id = tls_io_instance->socket_id;

  if (rc == -1) {
    status = rsi_wlan_get_status();
  }
  return status;
}

//! The tlsio_ssl_create shall create a new instance of the tlsio 
CONCRETE_IO_HANDLE tlsio_ssl_create(void *io_create_parameters)
{
  TLS_IO_INSTANCE *result;
  TLSIO_CONFIG *tls_io_config = io_create_parameters;

  if (tls_io_config == NULL) {
    result = NULL;
    LogError("NULL tls_io_config.");
  } 
  else 
  {
    result = malloc(sizeof(TLS_IO_INSTANCE));
    if (result == NULL) 
    {
      LogError("Failed allocating TLSIO instance.");
    } 
    else 
    {
      const IO_INTERFACE_DESCRIPTION *underlying_io_interface;
      if (tls_io_config->underlying_io_interface != NULL) 
      {
        underlying_io_interface = tls_io_config->underlying_io_interface;
      } 
      else 
      {
        tls_io_conf.hostname    = tls_io_config->hostname;
        tls_io_conf.port        = tls_io_config->port;
        underlying_io_interface = tlsio_openssl_get_interface_description();
      }

      if (underlying_io_interface == NULL)
      {
        free(result);
        result = NULL;
        LogError("Failed getting socket IO interface description.");
      } 
      else 
      {
        result->certificate                  = NULL;
        result->on_bytes_received            = NULL;
        result->on_bytes_received_context    = NULL;
        result->on_io_open_complete          = NULL;
        result->on_io_open_complete_context  = NULL;
        result->on_io_close_complete         = NULL;
        result->on_io_close_complete_context = NULL;
        result->on_io_error                  = NULL;
        result->on_io_error_context          = NULL;
        result->x509_certificate             = NULL;
        result->x509_private_key             = NULL;
        result->underlying_io                = NULL;
        result->tlsio_state                  = TLSIO_STATE_NOT_OPEN;
      }
    }
  }
  return result;
}

//! This function destroys an option previously created 
void tlsio_ssl_destroy(CONCRETE_IO_HANDLE tls_io)
{
  if (tls_io == NULL) {
    LogError("NULL tls_io.");
  } else {
    TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
    if (tls_io_instance->certificate != NULL) {
      free(tls_io_instance->certificate);
      tls_io_instance->certificate = NULL;
    }
    free((void *)tls_io_instance->x509_certificate);
    free((void *)tls_io_instance->x509_private_key);
    close_ssl_instance(tls_io_instance);
    if (tls_io_instance->underlying_io != NULL) {
      tls_io_instance->underlying_io = NULL;
    }
    free(tls_io);
  }
}

/*
The tlsio_ssl_open shall start the process to open the ssl connection with the host provided in the tlsio_ssl_create.
This API create a TLS TCP socket to the configure address using the credentials provided via the NewNetwork API call.
On success, tlsio_open_async shall store the provided on_bytes_received, on_bytes_received_context, on_io_error, on_io_error_context, on_io_open_complete, 
and on_io_open_complete_context parameters
*/
int tlsio_ssl_open(CONCRETE_IO_HANDLE tls_io,
                   ON_IO_OPEN_COMPLETE on_io_open_complete,
                   void *on_io_open_complete_context,
                   ON_BYTES_RECEIVED on_bytes_received,
                   void *on_bytes_received_context,
                   ON_IO_ERROR on_io_error,
                   void *on_io_error_context)
{
  int result;
  int32_t status = 0;
  rsi_rsp_dns_query_t dns_query_rsp;
  uint32_t server_address = 0;
  uint8_t count           = DNS_REQ_COUNT;

  if (tls_io == NULL) {
    result = MU_FAILURE;
    LogError("NULL tls_io.");
  } else {
    TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
    if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN) {
      LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN.");
      result = MU_FAILURE;
    } else {
      tls_io_instance->on_io_open_complete         = on_io_open_complete;
      tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

      tls_io_instance->on_bytes_received         = on_bytes_received;
      tls_io_instance->on_bytes_received_context = on_bytes_received_context;

      tls_io_instance->on_io_error         = on_io_error;
      tls_io_instance->on_io_error_context = on_io_error_context;

      tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

      //! Dns request
      do {

        status = rsi_dns_req(RSI_IP_VERSION_4,
                             (uint8_t *)tls_io_conf.hostname,
                             NULL,
                             NULL,
                             &dns_query_rsp,
                             sizeof(dns_query_rsp));
        if (status == RSI_SUCCESS) {
          break;
        }
        count--;
      } while (count != 0);

      if (status != RSI_SUCCESS) {
        return MU_FAILURE;
      }

      server_address = rsi_bytes4R_to_uint32(dns_query_rsp.ip_address[0].ipv4_address);

      //! Connect to server address
      result = ConnecttoNetwork(tls_io_instance,
                                2,
                                (char *)&server_address,
                                tls_io_conf.port,
                                CLIENT_PORT); 
      if (result != 0) {
	LOG_PRINT("\nConnect to network failed\n");
	tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
	indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
	result = MU_FAILURE;
      } else {
        /* Codes_SRS_TLSIO_SSL_ESP8266_99_034: [ If tlsio_ssl_open get success to open the ssl connection, it shall set the tlsio state as TLSIO_STATE_OPEN, and return 0. ]*/
        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
        /* Codes_SRS_TLSIO_SSL_ESP8266_99_041: [ If the tlsio_ssl_open get success to open the tls connection, and the on_io_open_complete callback was provided, it shall call the on_io_open_complete with IO_OPEN_OK. ]*/
        indicate_open_complete(tls_io_instance, IO_OPEN_OK);

        result = 0;
      }
    }
  }
  return result;
}

/* Codes_SRS_TLSIO_30_009: [ The phrase "enter TLSIO_STATE_EXT_CLOSING" means the adapter shall iterate through any unsent messages in the queue and shall delete each message after calling its on_send_complete with the associated callback_context and IO_SEND_CANCELLED. ]*/
/* Codes_SRS_TLSIO_30_006: [ The phrase "enter TLSIO_STATE_EXT_CLOSED" means the adapter shall forcibly close any existing connections then call the on_io_close_complete function and pass the on_io_close_complete_context that was supplied in tlsio_close_async. ]*/
/* Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing, then the adapter shall enter TLSIO_STATE_EXT_CLOSED immediately after entering TLSIO_STATE_EX_CLOSING. ]*/
int tlsio_ssl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context)
{
  int result;
  /* Codes_SRS_TLSIO_30_050: [ If the tlsio_handle parameter is NULL, tlsio_close_async shall log an error and return _FAILURE_. ]*/
  if (tls_io == NULL) {
    LogError("NULL tls_io.");
    result = MU_FAILURE;
  } else {
    TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

    if (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR && tls_io_instance->tlsio_state != TLSIO_STATE_OPEN) {
      /* Codes_RS_TLSIO_30_053: [ If the adapter is in any state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR then tlsio_close_async shall log that tlsio_close_async has been called and then continue normally. ]*/
      //! LogInfo rather than LogError because this is an unusual but not erroneous situation
      LogInfo("Closing tlsio from a state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR");
    }

    if (is_an_opening_state(tls_io_instance->tlsio_state)) {
      /* Codes_SRS_TLSIO_30_057: [ On success, if the adapter is in TLSIO_STATE_EXT_OPENING, it shall call on_io_open_complete with the on_io_open_complete_context supplied in tlsio_open_async and IO_OPEN_CANCELLED. This callback shall be made before changing the internal state of the adapter. ]*/
      tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_CANCELLED);
    }

    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN) {
      //! Attempt a graceful shutdown
      //! Codes_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]
      tls_io_instance->tlsio_state                  = TLSIO_STATE_CLOSING;
      tls_io_instance->on_io_close_complete         = on_io_close_complete;
      tls_io_instance->on_io_close_complete_context = callback_context;
      //! xio_close is guaranteed to succeed from the open state, and the callback completes the
      //! transition into TLSIO_STATE_NOT_OPEN
      if (xio_close(tls_io_instance->underlying_io, on_underlying_io_close_complete, tls_io_instance) != 0) {
        close_ssl_instance(tls_io_instance);
        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
      }
    } else {
      //! Just force the shutdown
      //! Codes_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]
      //! Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing or if the adapter is not in //!TLSIO_STATE_EXT_OPEN, then the adapter shall enter TLSIO_STATE_EXT_CLOSED immediately after entering TLSIO_STATE_EXT_CLOSING. ]
      //! Current implementations of xio_close will fail if not in the open state, but we don't care
      (void)xio_close(tls_io_instance->underlying_io, NULL, NULL);
      close_ssl_instance(tls_io_instance);
      tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
    }
    result = 0;
  }
  //! Codes_SRS_TLSIO_30_054: [ On failure, the adapter shall not call on_io_close_complete. ]
  return result;
}

//! This API will write data to the ssl coonection
int tlsio_ssl_send(CONCRETE_IO_HANDLE tls_io,
                   const void *buffer,
                   size_t size,
                   ON_SEND_COMPLETE on_send_complete,
                   void *callback_context)
{
  int result;

  if (tls_io == NULL) {
    LogError("NULL tls_io.");
    result = MU_FAILURE;
  } else {
    TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
    if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN) {
      LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPEN.");
      result = MU_FAILURE;
    } else {
      int res;
      //! This API Write to the TLS network buffer
      res = rsi_send(tls_io_conf.socket_id, buffer, (int32_t)size, 0); 
      if (res != size) {
        LogError("SSL_write error.");
        result = MU_FAILURE;
      }

      else {
        result = 0;
      }
    }
  }
  return result;
}

//! The tlsio_dowork call executes async jobs for the tlsio
void tlsio_ssl_dowork(CONCRETE_IO_HANDLE tls_io)
{
  if (tls_io != NULL) {
    TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPENING_UNDERLYING_IO
        || tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE
        || tls_io_instance->tlsio_state == TLSIO_STATE_OPEN) {
      decode_ssl_received_bytes(tls_io_instance);
    }
  }

  return;
}

//! Note:Certificates should be loaded before scan ,So loading certificates from this function is not possible,  
//! Use the function int rsi_load_certificates(const char* optionName,  uint8_t *buffer ,uint32_t certificate_length) to load certificates

int tlsio_ssl_setoption(CONCRETE_IO_HANDLE tls_io, const char *optionName, const void *value)
{
  int result = 0;

  if (tls_io == NULL || optionName == NULL) {
    result = MU_FAILURE;
  } else {
    //! Root CA
    if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0) {
      if (value != NULL) {
        result = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, NULL, 0);

        //! Load CA certificate
        result = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, (uint8_t *)value, (sizeof(value) - 1));
        if (result != RSI_SUCCESS) {
          return result;
        }
      }
    } else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0) {
      if (value != NULL) {
        result = rsi_wlan_set_certificate(RSI_SSL_CLIENT, NULL, 0);

        result = rsi_wlan_set_certificate(RSI_SSL_CLIENT, (uint8_t *)value, (sizeof(value) - 1));
        if (result != RSI_SUCCESS) {
          return result;
        }
      }
    } else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0) {
      if (value != NULL) {

        result = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, NULL, 0);
        result = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, (uint8_t *)value, (sizeof(value) - 1));
        if (result != RSI_SUCCESS) {
          return result;
        }
      }
    } else {
      result = MU_FAILURE;
    }
  }
  return result;
}

//! Retrieve function is not applicable
static OPTIONHANDLER_HANDLE tlsio_ssl_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
  OPTIONHANDLER_HANDLE result;
  if (handle == NULL) {
    LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle);
    result = NULL;
  } else {
    result = OptionHandler_Create(tlsio_ssl_CloneOption, tlsio_ssl_DestroyOption, tlsio_ssl_setoption);
    if (result == NULL) {
      LogError("unable to OptionHandler_Create");
    } else {
      //! This layer cares about the certificates and the x509 credentials
      TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)handle;
      OPTIONHANDLER_HANDLE underlying_io_options;

      if ((underlying_io_options = xio_retrieveoptions(tls_io_instance->underlying_io)) == NULL
          || OptionHandler_AddOption(result, OPTION_UNDERLYING_IO_OPTIONS, underlying_io_options) != OPTIONHANDLER_OK) {
        LogError("unable to save underlying_io options");
        OptionHandler_Destroy(underlying_io_options);
        OptionHandler_Destroy(result);
        result = NULL;
      } else if ((tls_io_instance->certificate != NULL)
                 && (OptionHandler_AddOption(result, OPTION_TRUSTED_CERT, tls_io_instance->certificate)
                     != OPTIONHANDLER_OK)) {
        LogError("unable to save TrustedCerts option");
        OptionHandler_Destroy(result);
        result = NULL;
      } else if (tls_io_instance->x509_certificate != NULL
                 && (OptionHandler_AddOption(result, SU_OPTION_X509_CERT, tls_io_instance->x509_certificate)
                     != OPTIONHANDLER_OK)) {
        LogError("unable to save x509 certificate option");
        OptionHandler_Destroy(result);
        result = NULL;
      } else if (tls_io_instance->x509_private_key != NULL
                 && (OptionHandler_AddOption(result, SU_OPTION_X509_PRIVATE_KEY, tls_io_instance->x509_private_key)
                     != OPTIONHANDLER_OK)) {
        LogError("unable to save x509 privatekey option");
        OptionHandler_Destroy(result);
        result = NULL;
      } else {
        //! All is fine, all interesting options have been saved
        //! Return as is
      }
    }
  }
  return result;
}
static const IO_INTERFACE_DESCRIPTION tlsio_ssl_interface_description = {
  tlsio_ssl_retrieveoptions, tlsio_ssl_create, tlsio_ssl_destroy, tlsio_ssl_open,
  tlsio_ssl_close,           tlsio_ssl_send,   tlsio_ssl_dowork,  tlsio_ssl_setoption
};
//! Should be called in platform_get_default_tlsio() function in platform_xyz.c file in c-utility/adapters in sdk
const IO_INTERFACE_DESCRIPTION *tlsio_openssl_get_interface_description(void)
{
  return &tlsio_ssl_interface_description;
}
#endif

