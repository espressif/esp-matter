/***************************************************************************//**
 * @file
 * @brief Provides support features for security, such as random number generation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "tool/random/random-number-generation.h"

EmberStatus emberAfGenerateRandomKey(EmberKeyData* result)
{
  if (platformRandomDataFunction(emberKeyContents(result),
                                 EMBER_ENCRYPTION_KEY_SIZE)) {
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }
  return EMBER_SUCCESS;
}

EmberStatus emberAfGenerateRandomData(uint8_t* result, uint8_t size)
{
  if (platformRandomDataFunction(result, size)) {
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }
  return EMBER_SUCCESS;
}
