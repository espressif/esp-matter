/***************************************************************************//**
 * @file
 * @brief wmbus_sample_frame.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "sl_wmbus_support.h"

#ifndef WMBUS_SAMPLE_FRAM_H_
#define WMBUS_SAMPLE_FRAM_H_

/**
 * Creates a standard mbus frame with volume and flow rate fields
 * (typical water meter) and water (0x07) device type
 *
 * @param[out] buffer
 *    The buffer the frame will be written to
 * @param[in] accessNumber
 *    Access number of the field (part of the STL header)
 * @param[in] accessibility
 *    Accessibility setting of the device. The application is responsible
 *    for implementing the claimed accessibility
 * @param[in] volume
 *    Volume in 10^-3 m^3
 * @param[in] volumeFlow
 *    Flow rate in 10^-3 m^3/h
 * @param[in] periodic
 *    Set it true if this is a periodic frame. Note that periodic frames
 *    have strict timing requirements, which should be kept by the application
 * @param[in] encrypt
 *    Set it to true to enable mode5 encryption on the frame
 */
uint16_t WMBUS_SAMPLE_setupFrame(uint8_t *buffer, uint8_t accessNumber, WMBUS_accessibility_t accessibility, int32_t volume, int16_t volumeFlow, bool periodic, bool encrypt);

#endif /* WMBUS_SAMPLE_FRAM_H_ */
