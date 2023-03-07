/*******************************************************************************
* @file  tlsio_openssl_rs9116.h
* @brief 
*******************************************************************************
* Copyright (c) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE file in the project root for full license information.
******************************************************************************
*
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
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
#ifndef TLSIO_SSL_H
#define TLSIO_SSL_H

#include "azure_c_shared_utility/xio.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif /* __cplusplus */

const IO_INTERFACE_DESCRIPTION *tlsio_openssl_get_interface_description(void);
CONCRETE_IO_HANDLE tlsio_ssl_create(void *io_create_parameters);
void tlsio_ssl_destroy(CONCRETE_IO_HANDLE tls_io);
int tlsio_ssl_open(CONCRETE_IO_HANDLE tls_io,
                   ON_IO_OPEN_COMPLETE on_io_open_complete,
                   void *on_io_open_complete_context,
                   ON_BYTES_RECEIVED on_bytes_received,
                   void *on_bytes_received_context,
                   ON_IO_ERROR on_io_error,
                   void *on_io_error_context);

int tlsio_ssl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context);

int tlsio_ssl_send(CONCRETE_IO_HANDLE tls_io,
                   const void *buffer,
                   size_t size,
                   ON_SEND_COMPLETE on_send_complete,
                   void *callback_context);

void tlsio_ssl_dowork(CONCRETE_IO_HANDLE tls_io);
int tlsio_ssl_setoption(CONCRETE_IO_HANDLE tls_io, const char *optionName, const void *value);
int tlsio_openssl_init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_SSL_H */
#endif

