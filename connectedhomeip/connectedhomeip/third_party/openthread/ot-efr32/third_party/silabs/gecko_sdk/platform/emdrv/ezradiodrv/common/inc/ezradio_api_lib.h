/***************************************************************************//**
 * @file
 * @brief This file contains the common API library of the EZRadio and
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

#ifndef _EZRADIO_API_LIB_H_
#define _EZRADIO_API_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_api EZRADIODRV API Layer
 * @brief EzRadio Driver API Layer
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define EZRADIO_FIFO_SIZE       64
/// @endcond

/** EZRadio device configuration return values */
typedef enum {
  EZRADIO_CONFIG_SUCCESS,         /**< Configuration succeded. */
  EZRADIO_CONFIG_NO_PATCH,        /**< No patch is given. */
  EZRADIO_CONFIG_CTS_TIMEOUT,     /**< CTS timeout error during configuration. */
  EZRADIO_CONFIG_PATCH_FAIL,      /**< Patching is failed. */
  EZRADIO_CONFIG_COMMAND_ERROR    /**< Command error during configuration. */
} EZRADIO_ConfigRet_t;

/* Minimal driver support functions */
void ezradio_reset(void);
void ezradio_power_up(uint8_t boot_options, uint8_t xtal_options, uint32_t xo_freq);

void ezradio_part_info(ezradio_cmd_reply_t *ezradioReply);

void ezradio_start_tx(uint8_t channel, uint8_t condition, uint16_t tx_len);
void ezradio_start_rx(uint8_t channel, uint8_t condition, uint16_t rx_len, uint8_t next_state1, uint8_t next_state2, uint8_t next_state3);

void ezradio_get_int_status(uint8_t ph_clr_pend, uint8_t modem_clr_pend, uint8_t chip_clr_pend, ezradio_cmd_reply_t *ezradioReply);

void ezradio_gpio_pin_cfg(uint8_t gpio0, uint8_t gpio1, uint8_t gpio2, uint8_t gpio3, uint8_t nirq, uint8_t sdo, uint8_t gen_config,
                          ezradio_cmd_reply_t *ezradioReply);

void ezradio_set_property(uint8_t group, uint8_t num_props, uint8_t start_prop, ...);

void ezradio_change_state(uint8_t next_state1);

#ifdef EZRADIO_DRIVER_EXTENDED_SUPPORT
/* Extended driver support functions */
void ezradio_nop(void);

void ezradio_fifo_info(uint8_t fifo, ezradio_cmd_reply_t *ezradioReply);

void ezradio_write_tx_fifo(uint8_t numbytes, uint8_t* pdata);
void ezradio_read_rx_fifo(uint8_t numbytes, uint8_t* prxdata);

void ezradio_get_property(uint8_t group, uint8_t num_props, uint8_t start_prop, ezradio_cmd_reply_t *ezradioReply);

#ifdef EZRADIO_DRIVER_FULL_SUPPORT
/* Full driver support functions */

void ezradio_func_info(ezradio_cmd_reply_t *ezradioReply);

void ezradio_frr_a_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
void ezradio_frr_b_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
void ezradio_frr_c_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
void ezradio_frr_d_read(uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);

void ezradio_request_device_state(ezradio_cmd_reply_t *ezradioReply);
void ezradio_read_cmd_buff(ezradio_cmd_reply_t *ezradioReply);

void ezradio_get_ph_status(uint8_t ph_clr_pend, ezradio_cmd_reply_t *ezradioReply);
void ezradio_get_modem_status(uint8_t modem_clr_pend, ezradio_cmd_reply_t *ezradioReply);
void ezradio_get_chip_status(uint8_t chip_clr_pend, ezradio_cmd_reply_t *ezradioReply);

void ezradio_get_packet_info(uint8_t field_number_mask, uint16_t len, int16_t diff_len, ezradio_cmd_reply_t *ezradioReply);

void ezradio_start_tx_fast(void);
void ezradio_start_rx_fast(void);

void ezradio_get_int_status_fast_clear(void);
void ezradio_get_int_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply);

void ezradio_gpio_pin_cfg_fast(ezradio_cmd_reply_t *ezradioReply);

void ezradio_get_ph_status_fast_clear(void);
void ezradio_get_ph_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply);

void ezradio_get_modem_status_fast_clear(void);
void ezradio_get_modem_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply);

void ezradio_get_chip_status_fast_clear(void);
void ezradio_get_chip_status_fast_clear_read(ezradio_cmd_reply_t *ezradioReply);

void ezradio_fifo_info_fast_reset(uint8_t fifo);
void ezradio_fifo_info_fast_read(ezradio_cmd_reply_t *ezradioReply);

#endif /* EZRADIO_DRIVER_FULL_SUPPORT */
#endif /* EZRADIO_DRIVER_EXTENDED_SUPPORT */

/** @} (end addtogroup ezradiodrv_api) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif //_EZRADIO_API_LIB_H_
