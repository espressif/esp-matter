/***************************************************************************//**
 * @file sl_wisun_rf_test.h
 * @brief Wi-SUN RF test API
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_RF_TEST_H
#define SL_WISUN_RF_TEST_H

/**************************************************************************//**
 * @addtogroup SL_WISUN_RF_TEST Wi-SUN RF Test API
 * @{
 *****************************************************************************/


/**************************************************************************//**
 * @brief Start transmitting a random stream of characters to enable
 * the measurement of radio modulation.
 *
 * @details Transmit a PN9 bytes sequence. See RAIL_StartTxStream for more
 * information.
 *
 * @param[in] channel Name of the Wi-SUN network as a zero-terminated string
 * @return One of the following:
 *  - SL_STATUS_OK if the stream transmission started successfully.
 *  - SL_STATUS_NOT_READY if called before the stack initialization.
 *  - SL_STATUS_BUSY if a test is already running.
 *  - SL_STATUS_NETWORK_UP if a connection is already established or in progress.
 *  - SL_STATUS_INVALID_PARAMETER if an invalid channel is configured.
 *****************************************************************************/
sl_status_t sl_wisun_start_stream(uint16_t channel);


/**************************************************************************//**
 * @brief Stop a previously started stream of characters.
 *
 * @details See RAIL_StopTxStream for more information.
 *
 * @param[in] channel Name of the Wi-SUN network as a zero-terminated string
 * @return One of the following:
 *  - SL_STATUS_OK if the stream transmission stopped successfully.
 *  - SL_STATUS_INVALID_STATE if while not transmitting a stream.
 *****************************************************************************/
sl_status_t sl_wisun_stop_stream();

/**************************************************************************//**
 * @brief Start transmitting an unmodulated tone.
 *
 * @details Transmit a PN9 bytes sequence. See RAIL_StartTxStream for more
 * information.
 *
 * @param[in] channel Name of the Wi-SUN network as a zero-terminated string
 * @return One of the following:
 *  - SL_STATUS_OK if the stream transmission started successfully.
 *  - SL_STATUS_NOT_READY if called before the stack initialization.
 *  - SL_STATUS_BUSY if a test is already running.
 *  - SL_STATUS_NETWORK_UP if a connection is already established or in progress.
 *  - SL_STATUS_INVALID_PARAMETER if an invalid channel is configured.
 *****************************************************************************/
sl_status_t sl_wisun_start_tone(uint16_t channel);

/**************************************************************************//**
 * @brief Stop a previously started tone.
 *
 * @details See RAIL_StopTxStream for more information.
 *
 * @param[in] channel Name of the Wi-SUN network as a zero-terminated string
 * @return One of the following:
 *  - SL_STATUS_OK if the tone stopped successfully.
 *  - SL_STATUS_INVALID_STATE if while not transmitting a tone.
 *****************************************************************************/
sl_status_t sl_wisun_stop_tone();

/**************************************************************************//**
 * @brief Set transmit power.
 *
 * @param[in] tx_power Transmit power in units of dBm, can be negative.
 *
 * @return always SL_STATUS_OK
 *****************************************************************************/
sl_status_t sl_wisun_set_test_tx_power(int8_t tx_power);

/**************************************************************************//**
 * @brief Return the current status of the RF test plugin.
 *
 * @return One of the following:
 *  - True if a test is running.
 *  - False otherwise.
 *****************************************************************************/
bool sl_wisun_is_running_rf_test();

/** @} (end addtogroup SL_WISUN_RF_TEST) */

#endif // SL_WISUN_RF_TEST_H
