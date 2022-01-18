// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <app/InteractionModelEngine.h>

using chip::app::ConcreteCommandPath;
using chip::TLV::TLVReader;

#define COMMAND_MASK_CUSTOM 0x80

typedef esp_err_t (*esp_matter_command_custom_callback_t)(int endpoint_id, int cluster_id, int command_id,
                                                          TLVReader &tlv_data, void *priv_data);

typedef void (*esp_matter_command_callback_t)(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);

void esp_matter_command_callback_key_set_write(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data);
void esp_matter_command_callback_key_set_read(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_key_set_remove(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data);
void esp_matter_command_callback_key_set_read_all_indices(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data);
void esp_matter_command_callback_arm_fail_safe(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data);
void esp_matter_command_callback_set_regulatory_config(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_commissioning_complete(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data);
void esp_matter_command_callback_scan_networks(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data);
void esp_matter_command_callback_add_or_update_wifi_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data);
void esp_matter_command_callback_add_or_update_thread_network(void *command_obj,
                                                              const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data);
void esp_matter_command_callback_remove_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data);
void esp_matter_command_callback_connect_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data);
void esp_matter_command_callback_reorder_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data);
void esp_matter_command_callback_open_commissioning_window(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data);
void esp_matter_command_callback_open_basic_commissioning_window(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data);
void esp_matter_command_callback_revoke_commissioning(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_attestation_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data);
void esp_matter_command_callback_certificate_chain_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data);
void esp_matter_command_callback_csr_request(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data);
void esp_matter_command_callback_add_noc(void *command_obj, const ConcreteCommandPath &command_path,
                                         TLVReader &tlv_data);
void esp_matter_command_callback_update_noc(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data);
void esp_matter_command_callback_update_fabric_label(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data);
void esp_matter_command_callback_remove_fabric(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data);
void esp_matter_command_callback_add_trusted_root_certificate(void *command_obj,
                                                              const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data);
void esp_matter_command_callback_remove_trusted_root_certificate(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data);
void esp_matter_command_callback_query_image(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data);
void esp_matter_command_callback_apply_update_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_notify_update_applied(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_announce_ota_provider(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_identify(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data);
void esp_matter_command_callback_identify_query(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data);
void esp_matter_command_callback_add_group(void *command_obj, const ConcreteCommandPath &command_path,
                                           TLVReader &tlv_data);
void esp_matter_command_callback_view_group(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data);
void esp_matter_command_callback_get_group_membership(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_remove_group(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_remove_all_groups(void *command_obj, const ConcreteCommandPath &command_path,
                                                   TLVReader &tlv_data);
void esp_matter_command_callback_add_group_if_identifying(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data);
void esp_matter_command_callback_add_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                           TLVReader &tlv_data);
void esp_matter_command_callback_view_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data);
void esp_matter_command_callback_remove_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_remove_all_scenes(void *command_obj, const ConcreteCommandPath &command_path,
                                                   TLVReader &tlv_data);
void esp_matter_command_callback_store_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data);
void esp_matter_command_callback_recall_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_get_scene_membership(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_off(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data);
void esp_matter_command_callback_on(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data);
void esp_matter_command_callback_toggle(void *command_obj, const ConcreteCommandPath &command_path,
                                        TLVReader &tlv_data);
void esp_matter_command_callback_move_to_level(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data);
void esp_matter_command_callback_move(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data);
void esp_matter_command_callback_step(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data);
void esp_matter_command_callback_stop(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data);
void esp_matter_command_callback_move_to_level_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data);
void esp_matter_command_callback_move_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data);
void esp_matter_command_callback_step_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data);
void esp_matter_command_callback_stop_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data);
void esp_matter_command_callback_move_to_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data);
void esp_matter_command_callback_move_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data);
void esp_matter_command_callback_step_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data);
void esp_matter_command_callback_move_to_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data);
void esp_matter_command_callback_move_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data);
void esp_matter_command_callback_step_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data);
void esp_matter_command_callback_move_to_hue_and_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data);
void esp_matter_command_callback_key_set_read_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_key_set_read_all_indices_response(void *command_obj,
                                                                   const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data);
void esp_matter_command_callback_arm_fail_safe_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data);
void esp_matter_command_callback_set_regulatory_config_response(void *command_obj,
                                                                const ConcreteCommandPath &command_path,
                                                                TLVReader &tlv_data);
void esp_matter_command_callback_commissioning_complete_response(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data);
void esp_matter_command_callback_scan_networks_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data);
void esp_matter_command_callback_network_config_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                         TLVReader &tlv_data);
void esp_matter_command_callback_connect_network_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data);
void esp_matter_command_callback_attestation_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_certificate_chain_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data);
void esp_matter_command_callback_csr_response(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_noc_response(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data);
void esp_matter_command_callback_query_image_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_apply_update_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_identify_query_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                         TLVReader &tlv_data);
void esp_matter_command_callback_add_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data);
void esp_matter_command_callback_view_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data);
void esp_matter_command_callback_get_group_membership_response(void *command_obj,
                                                               const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data);
void esp_matter_command_callback_remove_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_add_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data);
void esp_matter_command_callback_view_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data);
void esp_matter_command_callback_remove_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data);
void esp_matter_command_callback_remove_all_scenes_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data);
void esp_matter_command_callback_store_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data);
void esp_matter_command_callback_get_scene_membership_response(void *command_obj,
                                                               const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data);
