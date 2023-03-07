/***************************************************************************//**
 * @file
 * @brief Contains storage and function for retrieving attribute size.
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

#include PLATFORM_HEADER

#include "af.h"

static const uint8_t attributeSizes[] =
#ifdef UC_BUILD
  ZAP_GENERATED_ATTRIBUTE_SIZES;
#else
// Appbuilder generated a weird file that gets injected in here into branches
{
#include "attribute-size.h"
};
#endif

uint8_t emberAfGetDataSize(uint8_t dataType)
{
  uint8_t i;
  for (i = 0; (uint32_t) (i + 1) < (uint32_t) sizeof(attributeSizes); i += 2) {
    if (attributeSizes[i] == dataType) {
      return attributeSizes[i + 1];
    }
  }

  return 0;
}

uint16_t emberAfAttributeValueSize(EmberAfAttributeType dataType,
                                   const uint8_t *buffer,
                                   const uint16_t bufferSize)
{
  // If the dataType is a string or long string, refer to the buffer for the
  // string's length prefix; size is string length plus number of prefix bytes.
  // If non-string, determine size from dataType. If dataType is unrecognized,
  // return zero.
  //
  // Note: A non-empty long string has max length 0xFFFE, and adding 2 for its
  // length prefix would roll a uint16_t back to zero. Choosing not to
  // expand return type to uint32_t just to accommodate that one case.
  //
  // A return value of zero indicates an error. Either the type is not recognized,
  // or the size of the data exceeds the bufferSize from which the data value
  // wouldl be extracted.
  uint16_t dataSize = 0;

  if (emberAfIsStringAttributeType(dataType)) {
    // size is string length plus 1-byte length prefix
    if (buffer != NULL && bufferSize > 0) {
      dataSize = ((uint16_t) emberAfStringLength(buffer)) + 1u;
    }
  } else if (emberAfIsLongStringAttributeType(dataType)) {
    // size is long string length plus 2-byte length prefix
    if (buffer != NULL && bufferSize > 1) {
      dataSize = emberAfLongStringLength(buffer) + 2u;
    }
  } else {
    dataSize = (uint16_t) emberAfGetDataSize(dataType);
  }

  return (dataSize <= bufferSize ? dataSize : 0);
}
