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

#ifndef SL_WFX_H
#define SL_WFX_H

#include "sl_wfx_configuration_defaults.h"
#include "sl_wfx_host_api.h"
#include "sl_wfx_version.h"
#include "bus/sl_wfx_bus.h"
#include "sl_wfx_registers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/// Define the WEAK macro for GCC compatible compilers
#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

sl_status_t sl_wfx_init(sl_wfx_context_t *context);

sl_status_t sl_wfx_deinit(void);

sl_status_t sl_wfx_enable_irq(void);

sl_status_t sl_wfx_disable_irq(void);

sl_status_t sl_wfx_shutdown(void);

sl_status_t sl_wfx_receive_frame(uint16_t *ctrl_reg);

sl_status_t sl_wfx_send_configuration(const char *pds_data, uint32_t pds_data_length);

sl_status_t sl_wfx_control_gpio(uint8_t gpio_label, uint8_t gpio_mode, uint32_t *value);

sl_status_t sl_wfx_pta_settings(uint8_t pta_mode,
                                uint8_t request_signal_active_level,
                                uint8_t priority_signal_active_level,
                                uint8_t freq_signal_active_level,
                                uint8_t grant_signal_active_level,
                                uint8_t coex_type,
                                uint8_t default_grant_state,
                                uint8_t simultaneous_rx_access,
                                uint8_t priority_sampling_time,
                                uint8_t tx_rx_sampling_time,
                                uint8_t freq_sampling_time,
                                uint8_t grant_valid_time,
                                uint8_t fem_control_time,
                                uint8_t first_slot_time,
                                uint16_t periodic_tx_rx_sampling_time,
                                uint16_t coex_quota,
                                uint16_t wlan_quota);

sl_status_t sl_wfx_pta_priority(uint32_t priority);

sl_status_t sl_wfx_pta_state(uint32_t pta_state);

sl_status_t sl_wfx_set_cca_config(uint8_t cca_thr_mode);

sl_status_t sl_wfx_prevent_rollback(uint32_t magic_word);

sl_status_t sl_wfx_get_opn(uint8_t **opn);

sl_status_t sl_wfx_get_status_code(uint32_t wfx_status, uint8_t command_id);

/*
 * Send Ethernet frame
 */
sl_status_t sl_wfx_send_ethernet_frame(sl_wfx_send_frame_req_t *frame,
                                       uint32_t data_length,
                                       sl_wfx_interface_t interface,
                                       uint8_t priority);

/*
 * Send generic WF200 command
 */
sl_status_t sl_wfx_send_command(uint8_t command_id,
                                void *data,
                                uint32_t data_size,
                                sl_wfx_interface_t interface,
                                sl_wfx_generic_confirmation_t **response);

sl_status_t sl_wfx_send_request(uint8_t id, sl_wfx_generic_message_t *request, uint16_t request_length);

/*
 * Synchronous WF200 commands
 */
sl_status_t sl_wfx_set_access_mode_message(void);

sl_status_t sl_wfx_set_mac_address(const sl_wfx_mac_address_t *mac, sl_wfx_interface_t interface);

sl_status_t sl_wfx_set_power_mode(sl_wfx_pm_mode_t mode, sl_wfx_pm_poll_t strategy, uint16_t interval);

sl_status_t sl_wfx_set_wake_up_bit(uint8_t state);

sl_status_t sl_wfx_enable_device_power_save(void);

sl_status_t sl_wfx_disable_device_power_save(void);

sl_status_t sl_wfx_get_signal_strength(uint32_t *rcpi);

sl_status_t sl_wfx_add_multicast_address(const sl_wfx_mac_address_t *mac_address, sl_wfx_interface_t interface);

sl_status_t sl_wfx_remove_multicast_address(const sl_wfx_mac_address_t *mac_address, sl_wfx_interface_t interface);

sl_status_t sl_wfx_set_arp_ip_address(uint32_t *arp_ip_addr, uint8_t num_arp_ip_addr);

sl_status_t sl_wfx_set_ns_ip_address(uint8_t *ns_ip_addr, uint8_t num_ns_ip_addr);

sl_status_t sl_wfx_set_broadcast_filter(uint32_t filter);

sl_status_t sl_wfx_set_unicast_filter(uint32_t filter);

sl_status_t sl_wfx_add_whitelist_address(const sl_wfx_mac_address_t *mac_address);

sl_status_t sl_wfx_add_blacklist_address(const sl_wfx_mac_address_t *mac_address);

sl_status_t sl_wfx_set_max_ap_client(uint32_t max_clients);

sl_status_t sl_wfx_set_max_ap_client_inactivity(uint32_t inactivity_timeout);

sl_status_t sl_wfx_set_scan_parameters(uint16_t active_channel_time,
                                       uint16_t passive_channel_time,
                                       uint16_t num_probe_requestuests);

sl_status_t sl_wfx_set_roam_parameters(uint8_t rcpi_threshold,
                                       uint8_t rcpi_hysteresis,
                                       uint8_t beacon_lost_count,
                                       const uint8_t *channel_list,
                                       uint8_t channel_list_count);

sl_status_t sl_wfx_set_tx_rate_parameters(sl_wfx_rate_set_bitmask_t rate_set_bitmask, uint8_t use_minstrel);

sl_status_t sl_wfx_set_max_tx_power(int32_t max_tx_power, sl_wfx_interface_t interface);

sl_status_t sl_wfx_get_max_tx_power(int32_t *max_tx_power_rf_port_1,
                                    int32_t *max_tx_power_rf_port_2,
                                    sl_wfx_interface_t interface);

sl_status_t sl_wfx_get_pmk(sl_wfx_password_t *password,
                           uint32_t *password_length,
                           sl_wfx_interface_t interface);

sl_status_t sl_wfx_get_ap_client_signal_strength(const sl_wfx_mac_address_t *client, uint32_t *signal_strength);

sl_status_t sl_wfx_ext_auth(sl_wfx_ext_auth_data_type_t auth_data_type,
                            uint16_t auth_data_length,
                            const uint8_t *auth_data);

sl_status_t sl_wfx_set_probe_request_filter(const uint8_t *ie_data_mask,
                                            const uint8_t *ie_data,
                                            uint16_t ie_data_length);

/*
 * Asynchronous WF200 commands
 */
sl_status_t sl_wfx_send_join_command(const uint8_t *ssid,
                                     uint32_t ssid_length,
                                     const sl_wfx_mac_address_t *bssid,
                                     uint16_t channel,
                                     sl_wfx_security_mode_t security_mode,
                                     uint8_t prevent_roaming,
                                     uint16_t management_frame_protection,
                                     const uint8_t *passkey,
                                     uint16_t passkey_length,
                                     const uint8_t *ie_data,
                                     uint16_t ie_data_length);

sl_status_t sl_wfx_send_disconnect_command(void);

sl_status_t sl_wfx_send_scan_command(uint16_t scan_mode,
                                     const uint8_t *channel_list,
                                     uint16_t channel_list_count,
                                     const sl_wfx_ssid_def_t *ssid_list,
                                     uint16_t ssid_list_count,
                                     const uint8_t *ie_data,
                                     uint16_t ie_data_length,
                                     const uint8_t *bssid);

sl_status_t sl_wfx_send_stop_scan_command(void);

sl_status_t sl_wfx_start_ap_command(uint16_t channel,
                                    uint8_t *ssid,
                                    uint32_t ssid_length,
                                    uint8_t hidden_ssid,
                                    uint8_t client_isolation,
                                    sl_wfx_security_mode_t security_mode,
                                    uint8_t management_frame_protection,
                                    const uint8_t *passkey,
                                    uint16_t passkey_length,
                                    const uint8_t *beacon_ie_data,
                                    uint16_t beacon_ie_data_length,
                                    const uint8_t *probe_response_ie_data,
                                    uint16_t probe_response_ie_data_length);

sl_status_t sl_wfx_update_ap_command(uint16_t beacon_ie_data_length,
                                     uint16_t probe_response_ie_data_length,
                                     uint32_t *beacon_ie_data,
                                     uint32_t *probe_response_ie_data);

sl_status_t sl_wfx_stop_ap_command(void);

sl_status_t sl_wfx_disconnect_ap_client_command(const sl_wfx_mac_address_t *client);

sl_status_t sl_wfx_set_antenna_config(sl_wfx_antenna_config_t config);

sl_status_t sl_wfx_allocate_command_buffer(sl_wfx_generic_message_t **buffer,
                                           uint32_t command_id,
                                           sl_wfx_buffer_type_t type,
                                           uint32_t buffer_size);

sl_status_t sl_wfx_free_command_buffer(sl_wfx_generic_message_t *buffer, uint32_t command_id, sl_wfx_buffer_type_t type);

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif // SL_WFX_H
