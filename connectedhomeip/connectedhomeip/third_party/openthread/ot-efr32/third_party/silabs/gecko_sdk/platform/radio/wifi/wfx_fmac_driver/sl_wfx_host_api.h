/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_HOST_API_H
#define SL_WFX_HOST_API_H

#include "sl_wfx_api.h"
#include "sl_wfx_constants.h"

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                    Variables
******************************************************/

extern sl_wfx_context_t *sl_wfx_context;

/**************************************************************************//**
 * @addtogroup HOST_API
 * @{
 *****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* Initialization phase*/
/**************************************************************************//**
 * @brief Driver hook to initialize the host resources
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called once during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_init(void);

/**************************************************************************//**
 * @brief Driver hook to retrieve a firmware chunk
 *
 * @param data is a pointer to the firmware data
 * @param data_size is the size of data requested by the driver
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called multiple times during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_get_firmware_data(const uint8_t **data, uint32_t data_size);

/**************************************************************************//**
 * @brief Driver hook to retrieve the firmware size
 *
 * @param firmware_size is a pointer to the firmware size value
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called once during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_get_firmware_size(uint32_t *firmware_size);

/**************************************************************************//**
 * @brief Driver hook to retrieve a PDS line
 *
 * @param pds_data is a pointer to the PDS data
 * @param index is the index of the line requested by the driver
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called multiple times during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_get_pds_data(const char **pds_data, uint16_t index);

/**************************************************************************//**
 * @brief Driver hook to get the number of lines of the PDS
 *
 * @param pds_size is a pointer to the PDS size value
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called once during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_get_pds_size(uint16_t *pds_size);

/**************************************************************************//**
 * @brief Driver hook to deinitialize the host resources
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called if an error occurs during the initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_deinit(void);

/* GPIO interface */
/**************************************************************************//**
 * @brief Implement the reset of the WFx chip
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note This function asserts the reset pin of the WFx chip for a while before
 * returning
 *****************************************************************************/
sl_status_t sl_wfx_host_reset_chip(void);

/**************************************************************************//**
 * @brief Drive the wake up pin in the requested state
 *
 * @param state to be applied to the wake up pin
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called if the sleep mode is enabled
 *****************************************************************************/
sl_status_t sl_wfx_host_set_wake_up_pin(uint8_t state);

/**************************************************************************//**
 * @brief Called once the WFx chip is waking up
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called if the sleep mode is enabled. The function waits for the WFx
 * interruption
 *****************************************************************************/
sl_status_t sl_wfx_host_wait_for_wake_up(void);

/**************************************************************************//**
 * @brief Called when the driver is considering putting the WFx in
 * sleep mode
 *
 * @param type is the type of the message sent
 * @param address is the address of the message sent
 * @param length is the length of the message to be sent
 * @returns Returns SL_STATUS_WIFI_SLEEP_GRANTED to let the WFx go to sleep,
 * SL_STATUS_WIFI_SLEEP_NOT_GRANTED otherwise
 *
 * @note The parameters are given as information for the host to take a decision
 * on whether or not the WFx is put back to sleep mode.
 *****************************************************************************/
sl_status_t sl_wfx_host_sleep_grant(sl_wfx_host_bus_transfer_type_t type,
                                    sl_wfx_register_address_t address,
                                    uint32_t length);

/**************************************************************************//**
 * @brief Hold the WFx chip in reset mode
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note The reset pin is asserted by the host to keep the WFx chip in reset
 *****************************************************************************/
sl_status_t sl_wfx_host_hold_in_reset(void);

/**************************************************************************//**
 * @brief Set up the next event that the driver will wait
 *
 * @param event_id is the ID to be waited
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called every time a API command is called
 *****************************************************************************/
sl_status_t sl_wfx_host_setup_waited_event(uint8_t event_id);

/**************************************************************************//**
 * @brief Called when the driver is waiting for a confirmation
 *
 * @param confirmation_id is the ID to be waited
 * @param timeout_ms is the time before the command times out
 * @param event_payload_out is a pointer to the data returned by the
 * confirmation
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called every time a API command is called
 *****************************************************************************/
sl_status_t sl_wfx_host_wait_for_confirmation(uint8_t confirmation_id,
                                              uint32_t timeout_ms,
                                              void **event_payload_out);

/**************************************************************************//**
 * @brief Called when the FMAC driver wants to add a delay
 *
 * @param wait_ms is the time to wait
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Can be a passive wait or in a RTOS context a task sleep
 *****************************************************************************/
sl_status_t sl_wfx_host_wait(uint32_t wait_ms);

/**************************************************************************//**
 * @brief Called when a message is received from the WFx chip
 *
 * @param event_payload is a pointer to the data received
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called by ::sl_wfx_receive_frame function
 *****************************************************************************/
sl_status_t sl_wfx_host_post_event(sl_wfx_generic_message_t *event_payload);

/* Memory management */
/**************************************************************************//**
 * @brief Called when the driver wants to allocate memory
 *
 * @param buffer is a pointer to the data
 * @param type is the type of buffer to allocate (see ::sl_wfx_buffer_type_t)
 * @param buffer_size represents the amount of memory to allocate
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called by the driver every time it needs memory
 *****************************************************************************/
sl_status_t sl_wfx_host_allocate_buffer(void **buffer,
                                        sl_wfx_buffer_type_t type,
                                        uint32_t buffer_size);

/**************************************************************************//**
 * @brief Called when the driver wants to free memory
 *
 * @param buffer is the pointer to the memory to free
 * @param type is the type of buffer to free (see ::sl_wfx_buffer_type_t)
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_free_buffer(void *buffer, sl_wfx_buffer_type_t type);

/* Frame hook */
/**************************************************************************//**
 * @brief Called when the driver sends a frame to the WFx chip
 *
 * @param frame is a pointer to the frame data
 * @param frame_len is size of the frame
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_transmit_frame(void *frame, uint32_t frame_len);

/**************************************************************************//**
 * @brief Called when the driver needs to lock its access
 *
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_NO_MORE_RESOURCE
 * otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_lock(void);

/**************************************************************************//**
 * @brief Called when the driver needs to unlock its access
 *
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_unlock(void);

/* WF200 host bus API */
/**************************************************************************//**
 * @brief Initialize the host bus
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called once during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_init_bus(void);

/**************************************************************************//**
 * @brief Deinitialize the host bus
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called if an error occurs during the initialization phase
 *****************************************************************************/
sl_status_t sl_wfx_host_deinit_bus(void);

/**************************************************************************//**
 * @brief Enable the bus interrupt
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_enable_platform_interrupt(void);

/**************************************************************************//**
 * @brief Disable the bus interrupt
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_disable_platform_interrupt(void);

/* WF200 host SPI bus API */
/**************************************************************************//**
 * @brief Assert the SPI Chip Select pin
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_spi_cs_assert(void);

/**************************************************************************//**
 * @brief Deassert the SPI Chip Select pin
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_spi_cs_deassert(void);

/**************************************************************************//**
 * @brief Send data on the SPI bus
 *
 * @param type is the type of bus action (see ::sl_wfx_host_bus_transfer_type_t)
 * @param header is a pointer to the header data
 * @param header_length is the length of the header data
 * @param buffer is a pointer to the buffer data
 * @param buffer_length is the length of the buffer data
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_spi_transfer_no_cs_assert(sl_wfx_host_bus_transfer_type_t type,
                                                  uint8_t *header,
                                                  uint16_t header_length,
                                                  uint8_t *buffer,
                                                  uint16_t buffer_length);

/* WF200 host SDIO bus API */
/**************************************************************************//**
 * @brief Send command 52 on the SDIO bus
 *
 * @param type is the type of bus action (see ::sl_wfx_host_bus_transfer_type_t)
 * @param function is the function to use in the SDIO command
 * @param address is the address to use in the SDIO command
 * @param buffer is a pointer to the buffer data
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_sdio_transfer_cmd52(sl_wfx_host_bus_transfer_type_t type,
                                            uint8_t  function,
                                            uint32_t address,
                                            uint8_t *buffer);

/**************************************************************************//**
 * @brief Send command 53 on the SDIO bus
 *
 * @param type is the type of bus action (see ::sl_wfx_host_bus_transfer_type_t)
 * @param function is the function to use in the SDIO command
 * @param address is the address to use in the SDIO command
 * @param buffer is a pointer to the buffer data
 * @param buffer_length is the length of the buffer data
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_sdio_transfer_cmd53(sl_wfx_host_bus_transfer_type_t type,
                                            uint8_t  function,
                                            uint32_t address,
                                            uint8_t *buffer,
                                            uint16_t buffer_length);

/**************************************************************************//**
 * @brief Enable the SDIO high-speed mode
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_sdio_enable_high_speed_mode(void);

#ifdef SL_WFX_USE_SECURE_LINK
/* WFX host SecureLink API */

/**************************************************************************//**
 * @brief Get the secure link mac key of the part
 *
 * @param sl_mac_key is the pointer to the sl_wfx_context key in which
 * the key must be copied.
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_WIFI_SECURE_LINK_MAC_KEY_ERROR otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_get_secure_link_mac_key(uint8_t *sl_mac_key);

/**************************************************************************//**
 * @brief Compute the secure link public key
 *
 * @param request is a pointer to the SL_WFX_SECURELINK_EXCHANGE_PUB_KEYS_REQ body.
 * @param sl_mac_key is a pointer to the context mac key
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_compute_pub_key(sl_wfx_securelink_exchange_pub_keys_req_body_t *request, const uint8_t *sl_mac_key);

/**************************************************************************//**
 * @brief Verify the WFX hashed public key
 *
 * @param response_packet is a pointer to the SL_WFX_SECURELINK_EXCHANGE_PUB_KEYS_IND
 * returned by the WFX.
 * @param sl_mac_key is a pointer to the context mac key
 * @param sl_host_pub_key is a pointer the the host public key
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_verify_pub_key(sl_wfx_securelink_exchange_pub_keys_ind_t *response_packet,
                                       const uint8_t *sl_mac_key,
                                       uint8_t *sl_host_pub_key);

/**************************************************************************//**
 * @brief Free any crypto context
 *
 * @returns Always returns SL_STATUS_OK
 *****************************************************************************/
sl_status_t sl_wfx_host_free_crypto_context(void);

/**************************************************************************//**
 * @brief Decode a packet
 *
 * @param buffer is the pointer to the encrypted part of the packet
 * @param length is the length of the encrypted part
 * @param session_key is the pointer to the context session key
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_decode_secure_link_data(uint8_t *buffer,
                                                uint32_t length,
                                                uint8_t *session_key);

/**************************************************************************//**
 * @brief Encode a packet
 *
 * @param buffer is the pointer to the message that will be encrypted
 * @param data_length is the length of the payload to be encrypted
 * @param session_key is the pointer to context session key
 * @param nonce is the pointer to the encryption nonce
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_encode_secure_link_data(sl_wfx_generic_message_t *buffer,
                                                uint32_t data_length,
                                                uint8_t *session_key,
                                                uint8_t *nonce);

/**************************************************************************//**
 * @brief Schedule a session key renegotiation
 *
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_wfx_host_schedule_secure_link_renegotiation(void);
#endif //SL_WFX_USE_SECURE_LINK

/* WF200 host debug API */
/**************************************************************************//**
 * @brief Log information about the driver
 *
 * @param string is a string to display
 * @param ... any number of arguments
 *****************************************************************************/
#if SL_WFX_DEBUG_MASK
extern void sl_wfx_host_log(const char *string, ...);
#endif

#ifdef __cplusplus
} /*extern "C" */
#endif

/** @} end HOST_API */

#endif // SL_WFX_HOST_API_H
