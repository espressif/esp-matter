/*******************************************************************************
* @file  tlsio_rs9116.h
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
#ifndef TLSIO_H
#define TLSIO_H

#include "azure_c_shared_utility/xio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct TLSIO_CONFIG_TAG {
  const char *hostname;
  int port;
  const IO_INTERFACE_DESCRIPTION *underlying_io_interface;
  void *underlying_io_parameters;
  int socket_id;
} TLSIO_CONFIG;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_H */

#endif

