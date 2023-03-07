/*******************************************************************************
* @file  platform_sapi_wrapper.c
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
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "tlsio_openssl_sapi.h"
#ifdef USE_OPENSSL
#include "tlsio_openssl.h"
#endif
#if USE_CYCLONESSL
#include "tlsio_cyclonessl.h"
#endif
#if USE_WOLFSSL
#include "tlsio_wolfssl.h"
#endif
#if USE_MBEDTLS
#include "tlsio_mbedtls.h"
#endif

#include <stdlib.h>

int platform_init(void)
{
  int result;
  result = tlsio_openssl_init();
  return result;
}

const IO_INTERFACE_DESCRIPTION *platform_get_default_tlsio(void)
{
#if USE_CYCLONESSL
  return tlsio_cyclonessl_get_interface_description();
#elif USE_WOLFSSL
  return tlsio_wolfssl_get_interface_description();
#elif USE_MBEDTLS
  return tlsio_mbedtls_get_interface_description();
#else
  //! Default to openssl
  return tlsio_openssl_get_interface_description();
#endif
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
  //! This fucntionality is not needed 
  //! No applicable options, so ignoring parameter
  (void)options;
  //! Expected format: "(<runtime name>; <operating system name>; <platform>)"
  return STRING_construct("(native; undefined; undefined)");
}

void platform_deinit(void)
{

}
#endif

