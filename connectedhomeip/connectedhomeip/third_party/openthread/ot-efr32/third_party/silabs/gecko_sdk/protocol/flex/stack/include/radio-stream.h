/***************************************************************************//**
 * @brief Radio stream API
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

#ifndef RADIO_STREAM_H
#define RADIO_STREAM_H

/**
 * @addtogroup radio_stream
 * @brief Connect API managing radio stream for RF testing purpose
 *
 * See radio-stream.h for source code.
 * @{
 */

/**
 * @brief Start a continuous TX stream to test RF.
 *
 * @param[in] parameters Stream mode. See ::EmberTxStreamParameters.
 * @param[in] channel RF channel.
 * @return EMBER_INVALID_CALL if the stack can not process the request\n
 *         EMBER_BAD_ARGUMENT if the parameters are wrong\n
 *         EMBER_SUCCESS if the stream can be started
 */
EmberStatus emberStartTxStream(EmberTxStreamParameters parameters, uint16_t channel);

/**
 * @brief Stop an RF stream in progress.
 *
 * @return EMBER_INVALID_CALL if no stream is in progress\n
 *         EMBER_SUCCESS otherwise
 */
EmberStatus emberStopTxStream(void);

/**
 * @}
 */

 #endif
