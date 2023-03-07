/***************************************************************************//**
 * @file
 * @brief dummy entropy function. This dummy entropy function is used
 *        to emulate entropy
 *******************************************************************************
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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#include <string.h>

#include "mbedtls/entropy.h"

#if defined(_SILICON_LABS_32B_SERIES_0)
  #define ENABLE_MOCK_ENTROPY 1
#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) && !defined(_EFR_DEVICE)
  #define ENABLE_MOCK_ENTROPY 1
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && defined(ENABLE_MOCK_ENTROPY)

// The mbedtls_hardware_poll() function is meant for internal use by Mbed TLS
// and is not declared in any external header files. We will therefore declare
// it as an extern function here.
extern int mbedtls_hardware_poll(void *data,
                                 unsigned char *output,
                                 size_t len,
                                 size_t *olen);

int mbedtls_hardware_poll(void *data,
                          unsigned char *output,
                          size_t len,
                          size_t *olen)
{
  ((void) data);

  #define SIZE_OF_FIXED_RANDOM_DATA  (128)
  const uint8_t fixed_random_data[SIZE_OF_FIXED_RANDOM_DATA] =
  {
    0xca, 0xd3, 0x83, 0x27, 0xe0, 0xea, 0x07, 0xe9,
    0xf5, 0x16, 0x59, 0x22, 0x45, 0x77, 0x88, 0x53,
    0x23, 0x28, 0xd1, 0x92, 0x05, 0xc0, 0xfa, 0xe4,
    0xd8, 0xef, 0x98, 0x41, 0xe1, 0x57, 0xfb, 0x10,

    0xe8, 0xaa, 0x12, 0x0a, 0x7f, 0xeb, 0xd5, 0xd2,
    0xeb, 0xa5, 0x58, 0x48, 0xff, 0x30, 0x87, 0x98,
    0x2f, 0x4e, 0xd0, 0x8a, 0xf0, 0xab, 0x92, 0xd5,
    0xda, 0x3e, 0x82, 0x17, 0x0b, 0x29, 0x3b, 0xed,

    0x05, 0xf0, 0xc4, 0xb6, 0x0a, 0x65, 0x10, 0xbf,
    0x01, 0x7d, 0xf7, 0x27, 0x0a, 0xb4, 0xdc, 0xe0,
    0x00, 0x25, 0xdf, 0xee, 0x06, 0x2a, 0xf6, 0x91,
    0x07, 0x2d, 0x5b, 0x98, 0x0b, 0xc1, 0xce, 0x20,

    0x05, 0xab, 0x4e, 0x95, 0x07, 0xbd, 0x83, 0xe8,
    0x01, 0x22, 0x39, 0x7e, 0x0c, 0x57, 0x3a, 0x45,
    0x0c, 0x17, 0xc8, 0xd7, 0x09, 0x87, 0xcf, 0x82,
    0x07, 0xa2, 0xe3, 0x80, 0x02, 0x0c, 0xd1, 0xa8
  };

  if (len > SIZE_OF_FIXED_RANDOM_DATA) {
    while (1) ;
  }

  memcpy(output, fixed_random_data, len);

  *olen = len;
  return(0);
}

#endif
