/*******************************************************************************
* @file  socketio_sapi_wrapper.c
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

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "azure_c_shared_utility/socketio.h"

#include <stdint.h>
#include <errno.h>
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/optionhandler.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/const_defines.h"
#include "azure_c_shared_utility/dns_resolver.h"
#include "rsi_socket.h"
#include "rsi_wlan_apis.h"
#include "rsi_common_apis.h"
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_wlan.h"
#include "rsi_nwk.h"

#define SOCKET_SUCCESS            0
#define INVALID_SOCKET            -1
#define SOCKET_SEND_FAILURE       -1
#define MAC_ADDRESS_STRING_LENGTH 18

#ifndef IFREQ_BUFFER_SIZE
#define IFREQ_BUFFER_SIZE 1024
#endif

//! Connect timeout in seconds
#define CONNECT_TIMEOUT 10
#define TCP_KEEPIDLE    20
#define TCP_KEEPALIVE   1

typedef enum IO_STATE_TAG {
  IO_STATE_CLOSED,
  IO_STATE_OPENING,
  IO_STATE_OPEN,
  IO_STATE_CLOSING,
  IO_STATE_ERROR
} IO_STATE;

typedef struct PENDING_SOCKET_IO_TAG {
  unsigned char *bytes;
  size_t size;
  ON_SEND_COMPLETE on_send_complete;
  void *callback_context;
  SINGLYLINKEDLIST_HANDLE pending_io_list;
} PENDING_SOCKET_IO;

typedef struct SOCKET_IO_INSTANCE_TAG {
  int socket;
  SOCKETIO_ADDRESS_TYPE address_type;
  ON_BYTES_RECEIVED on_bytes_received;
  ON_IO_ERROR on_io_error;
  ON_IO_OPEN_COMPLETE on_io_open_complete;
  void *on_bytes_received_context;
  void *on_io_error_context;
  void *on_io_open_complete_context;
  char *hostname;
  int port;
  char *target_mac_address;
  IO_STATE io_state;
  SINGLYLINKEDLIST_HANDLE pending_io_list;
  unsigned char recv_bytes[XIO_RECEIVE_BUFFER_SIZE];
  DNSRESOLVER_HANDLE dns_resolver;
} SOCKET_IO_INSTANCE;

typedef struct NETWORK_INTERFACE_DESCRIPTION_TAG {
  char *name;
  char *mac_address;
  char *ip_address;
  struct NETWORK_INTERFACE_DESCRIPTION_TAG *next;
} NETWORK_INTERFACE_DESCRIPTION;

//! This function will clone an option given by name and value
static void *socketio_CloneOption(const char *name, const void *value)
{
  void *result;

  if (name != NULL) {
    result = NULL;

    if (strcmp(name, OPTION_NET_INT_MAC_ADDRESS) == 0) {
      if (value == NULL) {
        LogError("Failed cloning option %s (value is NULL)", name);
      } else {
        if ((result = malloc(sizeof(char) * (strlen((char *)value) + 1))) == NULL) {
          LogError("Failed cloning option %s (malloc failed)", name);
        } else if (strcpy((char *)result, (char *)value) == NULL) {
          LogError("Failed cloning option %s (strcpy failed)", name);
          free(result);
          result = NULL;
        }
      }
    } else {
      LogError("Cannot clone option %s (not suppported)", name);
    }
  } else {
    result = NULL;
  }
  return result;
}

//! This function destroys an option previously created
static void socketio_DestroyOption(const char *name, const void *value)
{
  if (name != NULL) {
    if (strcmp(name, OPTION_NET_INT_MAC_ADDRESS) == 0 && value != NULL) {
      free((void *)value);
    }
  }
}

static OPTIONHANDLER_HANDLE socketio_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
  OPTIONHANDLER_HANDLE result;

  if (handle == NULL) {
    LogError("failed retrieving options (handle is NULL)");
    result = NULL;
  } else {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)handle;

    result = OptionHandler_Create(socketio_CloneOption, socketio_DestroyOption, socketio_setoption);
    if (result == NULL) {
      LogError("unable to OptionHandler_Create");
    } else if (socket_io_instance->target_mac_address != NULL
               && OptionHandler_AddOption(result, OPTION_NET_INT_MAC_ADDRESS, socket_io_instance->target_mac_address)
                    != OPTIONHANDLER_OK) {
      LogError("failed retrieving options (failed adding net_interface_mac_address)");
      OptionHandler_Destroy(result);
      result = NULL;
    }
  }

  return result;
}

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description = {
  socketio_retrieveoptions, socketio_create, socketio_destroy, socketio_open,
  socketio_close,           socketio_send,   socketio_dowork,  socketio_setoption
};

static void indicate_error(SOCKET_IO_INSTANCE *socket_io_instance)
{
  socket_io_instance->io_state = IO_STATE_ERROR;
  if (socket_io_instance->on_io_error != NULL) {
    socket_io_instance->on_io_error(socket_io_instance->on_io_error_context);
  }
}

static int add_pending_io(SOCKET_IO_INSTANCE *socket_io_instance,
                          const unsigned char *buffer,
                          size_t size,
                          ON_SEND_COMPLETE on_send_complete,
                          void *callback_context)
{
  int result;
  PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)malloc(sizeof(PENDING_SOCKET_IO));
  if (pending_socket_io == NULL) {
    result = MU_FAILURE;
  } else {
    pending_socket_io->bytes = (unsigned char *)malloc(size);
    if (pending_socket_io->bytes == NULL) {
      LogError("Allocation Failure: Unable to allocate pending list.");
      free(pending_socket_io);
      result = MU_FAILURE;
    } else {
      pending_socket_io->size             = size;
      pending_socket_io->on_send_complete = on_send_complete;
      pending_socket_io->callback_context = callback_context;
      pending_socket_io->pending_io_list  = socket_io_instance->pending_io_list;
      (void)memcpy(pending_socket_io->bytes, buffer, size);

      if (singlylinkedlist_add(socket_io_instance->pending_io_list, pending_socket_io) == NULL) {
        LogError("Failure: Unable to add socket to pending list.");
        free(pending_socket_io->bytes);
        free(pending_socket_io);
        result = MU_FAILURE;
      } else {
        result = 0;
      }
    }
  }
  return result;
}


static int lookup_address(SOCKET_IO_INSTANCE *socket_io_instance)
{
  int result = 0;

  if (socket_io_instance->address_type == ADDRESS_TYPE_IP) {

  } else 
  {
    socket_io_instance->io_state = IO_STATE_OPEN;
  }

  return result;
}

static int initiate_socket_connection(SOCKET_IO_INSTANCE *socket_io_instance)
{
  int result = 0;
 // struct addrinfo *addr             = NULL;
  struct rsi_sockaddr *connect_addr = NULL;
  rsi_socklen_t connect_addr_len = 0;

  if (socket_io_instance->address_type == ADDRESS_TYPE_IP) {
    if (0) {
      LogError("DNS did not resolve IP address");
      result = MU_FAILURE;
    }
  } else {
#if 0
	size_t hostname_len = strlen(socket_io_instance->hostname);
        if (hostname_len + 1 > sizeof(addrInfoUn.sun_path))
        {
            LogError("Hostname %s is too long for a unix socket (max len = %lu)", socket_io_instance->hostname, (unsigned long)sizeof(addrInfoUn.sun_path));
            result = MU_FAILURE;
        }
        else
        {
            memset(&addrInfoUn, 0, sizeof(addrInfoUn));
            addrInfoUn.sun_family = AF_UNIX;
            //! No need to add NULL terminator due to the above memset
            (void)memcpy(addrInfoUn.sun_path, socket_io_instance->hostname, hostname_len);

            connect_addr = (struct rsi_sockaddr*)&addrInfoUn;
            connect_addr_len = sizeof(addrInfoUn);
            result = 0;
        }
#endif
  }

  if (result == 0) {
    if (0)
        {
      LogError("Failure: fcntl failure.");
      result = MU_FAILURE;
    } else {
      result = rsi_connect(socket_io_instance->socket, connect_addr, connect_addr_len);
      if ((result != 0)) {
        LogError("Failure: connect failure %d.", errno);
        result = MU_FAILURE;
      } else {
        //! Async connect will return -1.
        result = 0;
        if (socket_io_instance->on_io_open_complete != NULL) {
          socket_io_instance->on_io_open_complete(socket_io_instance->on_io_open_complete_context,
                                                  IO_OPEN_OK);
        }
      }
    }
  }

  return result;
}

static int lookup_address_and_initiate_socket_connection(SOCKET_IO_INSTANCE *socket_io_instance)
{
  int result;

  result = lookup_address(socket_io_instance);

  if (socket_io_instance->io_state == IO_STATE_OPEN) {
    if (result == 0) {
      result = initiate_socket_connection(socket_io_instance);
    }
  }

  return result;
}

static int wait_for_connection(SOCKET_IO_INSTANCE *socket_io_instance)
{
  int result;
  int err;
  int retval;

  rsi_fd_set fdset;
  struct rsi_timeval tv;

  RSI_FD_ZERO(&fdset);
  RSI_FD_SET(socket_io_instance->socket, &fdset);
  tv.tv_sec  = CONNECT_TIMEOUT;
  tv.tv_usec = 0;

  do {
    retval = rsi_select(socket_io_instance->socket + 1, NULL, &fdset, NULL, &tv, NULL);
  } while (retval < 0);

  if (retval != 1) {
    LogError("Failure: select failure.");
    result = MU_FAILURE;
  } else {
    int so_error      = 0;
    rsi_socklen_t len = sizeof(so_error);
    err               = rsi_getsockopt(socket_io_instance->socket, SOL_SOCKET, SO_ERROR, &so_error, len);
    if (err != 0) {
      LogError("Failure: getsockopt failure %d.", errno);
      result = MU_FAILURE;
    } else if (so_error != 0) {
      err = so_error;
      LogError("Failure: connect failure %d.", so_error);
      result = MU_FAILURE;
    } else {
      result = 0;
    }
  }

  return result;
}

static void destroy_socket_io_instance(SOCKET_IO_INSTANCE *instance)
{
  if (instance->dns_resolver != NULL) {
  }

  free(instance->hostname);
  free(instance->target_mac_address);

  if (instance->pending_io_list != NULL) {
    singlylinkedlist_destroy(instance->pending_io_list);
  }

  free(instance);
}

CONCRETE_IO_HANDLE socketio_create(void *io_create_parameters)
{
  SOCKETIO_CONFIG *socket_io_config = io_create_parameters;
  SOCKET_IO_INSTANCE *result;

  if (socket_io_config == NULL) {
    LogError("Invalid argument: socket_io_config is NULL");
    result = NULL;
  } else {
    result = malloc(sizeof(SOCKET_IO_INSTANCE));
    if (result != NULL) {
      (void)memset(result, 0, sizeof(SOCKET_IO_INSTANCE));

      result->address_type    = ADDRESS_TYPE_IP;
      result->pending_io_list = singlylinkedlist_create();
      if (result->pending_io_list == NULL) {
        LogError("Failure: singlylinkedlist_create unable to create pending list.");
        destroy_socket_io_instance(result);
        result = NULL;
      } else {
        if (socket_io_config->hostname != NULL) {
          result->hostname = (char *)malloc(strlen(socket_io_config->hostname) + 1);
          if (result->hostname != NULL) {
            (void)strcpy(result->hostname, socket_io_config->hostname);
          }

          result->socket = INVALID_SOCKET;
        } else {
          result->hostname = NULL;
          result->socket   = *((int *)socket_io_config->accepted_socket);
        }

        if ((result->hostname == NULL) && (result->socket == INVALID_SOCKET)) {
          LogError("Failure: hostname == NULL and socket is invalid.");
          destroy_socket_io_instance(result);
          result = NULL;
        } else {
          result->port                = socket_io_config->port;
          result->on_io_open_complete = NULL;
          result->dns_resolver        = NULL; 
          result->target_mac_address  = NULL;
          result->on_bytes_received   = NULL;
          result->on_io_error         = NULL;
          result->on_bytes_received_context = NULL;
          result->on_io_error_context       = NULL;
          result->io_state                  = IO_STATE_CLOSED;
        }
      }
    } else {
      LogError("Allocation Failure: SOCKET_IO_INSTANCE");
    }
  }

  return result;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{
  if (socket_io != NULL) {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    //! We cannot do much if the close fails, so just ignore the result 
    if (socket_io_instance->socket != INVALID_SOCKET) {
      (void)rsi_shutdown(socket_io_instance->socket, 0);
    }

    //! Clear allpending IOs 
    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    while (first_pending_io != NULL) {
      PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)singlylinkedlist_item_get_value(first_pending_io);
      if (pending_socket_io != NULL) {
        free(pending_socket_io->bytes);
        free(pending_socket_io);
      }

      (void)singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io);
      first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    }

    destroy_socket_io_instance(socket_io_instance);
  }
}

int socketio_open(CONCRETE_IO_HANDLE socket_io,
                  ON_IO_OPEN_COMPLETE on_io_open_complete,
                  void *on_io_open_complete_context,
                  ON_BYTES_RECEIVED on_bytes_received,
                  void *on_bytes_received_context,
                  ON_IO_ERROR on_io_error,
                  void *on_io_error_context)
{
  int result;

  SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
  if (socket_io == NULL) {
    LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
    result = MU_FAILURE;
  } else {
    if (socket_io_instance->io_state != IO_STATE_CLOSED) {
      LogError("Failure: socket state is not closed.");
      result = MU_FAILURE;
    } else if (socket_io_instance->socket != INVALID_SOCKET) {
      //! Opening an accepted socket
      socket_io_instance->on_bytes_received_context = on_bytes_received_context;
      socket_io_instance->on_bytes_received         = on_bytes_received;
      socket_io_instance->on_io_error               = on_io_error;
      socket_io_instance->on_io_error_context       = on_io_error_context;

      socket_io_instance->io_state = IO_STATE_OPEN;

      result = 0;
    } else {
      socket_io_instance->socket =
        rsi_socket(socket_io_instance->address_type == /* ADDRESS_TYPE_IP ? */ AF_INET /*: AF_UNIX*/, SOCK_STREAM, 0);
      if (socket_io_instance->socket < SOCKET_SUCCESS) {
        LogError("Failure: socket create failure %d.", socket_io_instance->socket);
        result = MU_FAILURE;
      } else if ((result = lookup_address_and_initiate_socket_connection(socket_io_instance)) != 0) {
        LogError("lookup_address_and_connect_socket failed");
      } else if ((result = wait_for_connection(socket_io_instance)) != 0) {
        LogError("wait_for_connection failed");
      }

      if (result == 0) {
        socket_io_instance->on_bytes_received         = on_bytes_received;
        socket_io_instance->on_bytes_received_context = on_bytes_received_context;

        socket_io_instance->on_io_error         = on_io_error;
        socket_io_instance->on_io_error_context = on_io_error_context;

        socket_io_instance->on_io_open_complete         = on_io_open_complete;
        socket_io_instance->on_io_open_complete_context = on_io_open_complete_context;
      } else {
        if (socket_io_instance->socket >= SOCKET_SUCCESS) {
          (void)rsi_shutdown(socket_io_instance->socket, 0);
        }
        socket_io_instance->socket = INVALID_SOCKET;
      }
    }
  }

  if (socket_io_instance->io_state != IO_STATE_OPENING) {
    if (on_io_open_complete != NULL) {
      on_io_open_complete(on_io_open_complete_context, result == 0 ? IO_OPEN_OK : IO_OPEN_ERROR);
    }
  }

  return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context)
{
  int result = 0;

  if (socket_io == NULL) {
    result = MU_FAILURE;
  } else {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if ((socket_io_instance->io_state != IO_STATE_CLOSED) && (socket_io_instance->io_state != IO_STATE_CLOSING)) {
      (void)rsi_shutdown(socket_io_instance->socket, 0);
      socket_io_instance->socket   = INVALID_SOCKET;
      socket_io_instance->io_state = IO_STATE_CLOSED;
    }

    if (on_io_close_complete != NULL) {
      on_io_close_complete(callback_context);
    }

    result = 0;
  }

  return result;
}

int socketio_send(CONCRETE_IO_HANDLE socket_io,
                  const void *buffer,
                  size_t size,
                  ON_SEND_COMPLETE on_send_complete,
                  void *callback_context)
{
  int result;

  if ((socket_io == NULL) || (buffer == NULL) || (size == 0)) {
    //! Invalid arguments 
    LogError("Invalid argument: send given invalid parameter");
    result = MU_FAILURE;
  } else {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if (socket_io_instance->io_state != IO_STATE_OPEN) {
      LogError("Failure: socket state is not opened.");
      result = MU_FAILURE;
    } else {
      LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
      if (first_pending_io != NULL) {
        if (add_pending_io(socket_io_instance, buffer, size, on_send_complete, callback_context) != 0) {
          LogError("Failure: add_pending_io failed.");
          result = MU_FAILURE;
        } else {
          result = 0;
        }
      } else {
        int32_t send_result = rsi_send(socket_io_instance->socket, buffer, size, MSG_NOSIGNAL);
        if ((int32_t)send_result != size) {
          if ((int32_t)send_result == SOCKET_SEND_FAILURE) {
            LogError("Failure: sending socket failed. errno=%d (%s).", errno, strerror(errno));
            result = MU_FAILURE;
          } else {
            //! Send says "come back later" with EAGAIN - likely the socket buffer cannot accept more data
            //! queue data 
            size_t bytes_sent = (send_result < 0 ? 0 : send_result);

            if (add_pending_io(socket_io_instance,
                               (const unsigned char *)buffer + bytes_sent,
                               size - bytes_sent,
                               on_send_complete,
                               callback_context)
                != 0) {
              LogError("Failure: add_pending_io failed.");
              result = MU_FAILURE;
            } else {
              result = 0;
            }
          }
        } else {
          if (on_send_complete != NULL) {
            on_send_complete(callback_context, IO_SEND_OK);
          }

          result = 0;
        }
      }
    }
  }

  return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
  if (socket_io != NULL) {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;

    if (socket_io_instance->io_state == IO_STATE_OPEN) {
      LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
      while (first_pending_io != NULL) {
        PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)singlylinkedlist_item_get_value(first_pending_io);
        if (pending_socket_io == NULL) {
          indicate_error(socket_io_instance);
          LogError("Failure: retrieving socket from list");
          break;
        }

        int32_t send_result =
          rsi_send(socket_io_instance->socket, (const int8_t *)pending_socket_io->bytes, pending_socket_io->size, MSG_NOSIGNAL);
        if ((send_result < 0) || ((uint32_t)send_result != pending_socket_io->size)) {
          if ((int32_t)send_result == INVALID_SOCKET) {
            //! need to handle 
          } else {
            //! Simply wait until next dowork 
            (void)memmove(pending_socket_io->bytes,
                          pending_socket_io->bytes + send_result,
                          pending_socket_io->size - send_result);
            pending_socket_io->size -= send_result;
            break;
          }
        } else {
          if (pending_socket_io->on_send_complete != NULL) {
            pending_socket_io->on_send_complete(pending_socket_io->callback_context, IO_SEND_OK);
          }

          free(pending_socket_io->bytes);
          free(pending_socket_io);
          if (singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io) != 0) {
            indicate_error(socket_io_instance);
            LogError("Failure: unable to remove socket from list");
          }
        }

        first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
      }

      if (socket_io_instance->io_state == IO_STATE_OPEN) {
        int32_t received = 0;
        do {
          received =
            rsi_recv(socket_io_instance->socket, socket_io_instance->recv_bytes, XIO_RECEIVE_BUFFER_SIZE, MSG_NOSIGNAL);
          if (received > 0) {
            if (socket_io_instance->on_bytes_received != NULL) {
              //! Explicitly ignoring here the result of the callback 
              (void)socket_io_instance->on_bytes_received(socket_io_instance->on_bytes_received_context,
                                                          socket_io_instance->recv_bytes,
                                                          received);
            }
          } else if (received == 0) {
            //! Do not log error here due to this is probably the socket being closed on the other end
            indicate_error(socket_io_instance);
          } else if (received < 0) {
            LogError("Socketio_Failure: Receiving data from endpoint: errno=%d.", errno);
            indicate_error(socket_io_instance);
          }

        } while (received > 0 && socket_io_instance->io_state == IO_STATE_OPEN);
      }
    } else {
      if (socket_io_instance->io_state == IO_STATE_OPENING) {
        if (lookup_address(socket_io_instance) != 0) {
          LogError("Socketio_Failure: lookup address failed");
          indicate_error(socket_io_instance);
        } else {
          if (socket_io_instance->io_state == IO_STATE_OPEN) {
            initiate_socket_connection(socket_io_instance);
          }
        }
      }
    }
  }
}

//! Edison is missing this from netinet/tcp.h, but this code still works if we manually define it.
#ifndef SOL_TCP
#define SOL_TCP 6
#endif

#ifndef __APPLE__
static void strtoup(char *str)
{
  if (str != NULL) {
    while (*str != '\0') {
      if (isalpha((int)*str) && islower((int)*str)) {
        *str = (char)toupper((int)*str);
      }
      str++;
    }
  }
}
#endif 

static int socketio_setaddresstype_option(SOCKET_IO_INSTANCE *socket_io_instance, const char *addressType)
{
  int result;

  if (socket_io_instance->io_state != IO_STATE_CLOSED) {
    LogError("Socket's type can only be changed when in state 'IO_STATE_CLOSED'.  Current state=%d",
             socket_io_instance->io_state);
    result = MU_FAILURE;
  } else if (strcmp(addressType, OPTION_ADDRESS_TYPE_DOMAIN_SOCKET) == 0) {
    socket_io_instance->address_type = ADDRESS_TYPE_DOMAIN_SOCKET;
    result                           = 0;
  } else if (strcmp(addressType, OPTION_ADDRESS_TYPE_IP_SOCKET) == 0) {
    socket_io_instance->address_type = ADDRESS_TYPE_IP;
    result                           = 0;
  } else {
    LogError("Address type %s is not supported", addressType);
    result = MU_FAILURE;
  }

  return result;
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char *optionName, const void *value)
{
  int result;

  if (socket_io == NULL || optionName == NULL || value == NULL) {
    result = MU_FAILURE;
  } else {
    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;

    if (strcmp(optionName, "tcp_keepalive") == 0) {
      result = rsi_setsockopt(socket_io_instance->socket, SOL_SOCKET, SO_KEEPALIVE, value, sizeof(int));
      if (result == -1)
        result = errno;
    } else if (strcmp(optionName, "tcp_keepalive_time") == 0) {
#ifdef __APPLE__
      result = setsockopt(socket_io_instance->socket, IPPROTO_TCP, TCP_KEEPALIVE, value, sizeof(int));
#else
      result = rsi_setsockopt(socket_io_instance->socket, SOL_TCP, TCP_KEEPIDLE, value, sizeof(int));
#endif
      if (result == -1)
        result = errno;
    } else if (strcmp(optionName, "tcp_keepalive_interval") == 0) {
      result = rsi_setsockopt(socket_io_instance->socket, SOL_TCP, TCP_KEEPALIVE, value, sizeof(int));
      if (result == -1)
        result = errno;
    } else if (strcmp(optionName, OPTION_NET_INT_MAC_ADDRESS) == 0) {
#ifdef __APPLE__
      LogError("option not supported.");
      result = MU_FAILURE;
#else
      if (strlen(value) == 0) {
        LogError("option value must be a valid mac address");
        result = MU_FAILURE;
      } else if ((socket_io_instance->target_mac_address = (char *)malloc(sizeof(char) * (strlen(value) + 1)))
                 == NULL) {
        LogError("failed setting net_interface_mac_address option (malloc failed)");
        result = MU_FAILURE;
      } else if (strcpy(socket_io_instance->target_mac_address, value) == NULL) {
        LogError("failed setting net_interface_mac_address option (strcpy failed)");
        free(socket_io_instance->target_mac_address);
        socket_io_instance->target_mac_address = NULL;
        result                                 = MU_FAILURE;
      } else {
        strtoup(socket_io_instance->target_mac_address);
        result = 0;
      }
#endif
    } else if (strcmp(optionName, OPTION_ADDRESS_TYPE) == 0) {
      result = socketio_setaddresstype_option(socket_io_instance, (const char *)value);
    } else {
      result = MU_FAILURE;
    }
  }

  return result;
}

const IO_INTERFACE_DESCRIPTION *socketio_get_interface_description(void)
{
  return &socket_io_interface_description;
}
#endif

