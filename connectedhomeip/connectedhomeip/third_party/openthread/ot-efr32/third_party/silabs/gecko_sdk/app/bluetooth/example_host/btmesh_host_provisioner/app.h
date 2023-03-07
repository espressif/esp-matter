/***************************************************************************//**
 * @file
 * @brief BT Mesh Host Provisioner Example Project.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_H
#define APP_H

#include <stdint.h>
#include "sl_btmesh_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Default netkey index
#define APP_NETKEY_IDX              0
/// UUID length in AABBCCDD format
#define UUID_LEN_WITHOUT_SEPARATORS 32
/// UUID length in AA:BB:CC:DD format
#define UUID_LEN_WITH_SEPARATORS    47
/// Address length in 1234 format
#define ADDRESS_LEN_WITHOUT_PREFIX  4
/// Address length in 0x1234 format
#define ADDRESS_LEN_WITH_PREFIX     6
/// 2 seconds timer for provisioner reset
#define RESET_TIMER_MS              2000
/// 5 seconds timer for scanning unprovisioned devices
#define SCAN_TIMER_MS               5000

typedef enum command_e{
  NONE,
  SCAN,
  PROVISION,
  NODELIST,
  NODEINFO,
  REMOVE_NODE,
  RESET
} command_t;

typedef enum command_state_e{
  INIT,
  START,
  IN_PROGRESS,
  FINISHED,
  UI_START,
  UI_INPUT,
  UI_IN_PROGRESS,
  UI_FINISHED
} command_state_t;

/**************************************************************************/ /**
 * Application Init.
 ******************************************************************************/
void app_init(int argc, char *argv[]);

/**************************************************************************/ /**
 * Application Process Action.
 ******************************************************************************/
void app_process_action(void);

/***************************************************************************/ /**
 * BT Mesh event handler in UI mode
 *
 * @param[in] evt Received BT Mesh event
 ******************************************************************************/
void app_ui_on_event(sl_btmesh_msg_t *evt);

/**************************************************************************/ /**
 * Application Deinit.
 ******************************************************************************/
void app_deinit(void);

/***************************************************************************/ /**
 * Callback to inform when the node configuration process ends
 *
 * @param[in] netkey_index Netkey index of the configured node
 * @param[in] server_address Primary subnet address of the configured node
 ******************************************************************************/
void app_on_node_configuration_end(uint16_t netkey_index,
                                   uint16_t server_address);

/***************************************************************************/ /**
 * Callback to inform when the node configuration process ends in UI mode
 *
 * @param[in] netkey_index Netkey index of the configured node
 * @param[in] server_address Primary subnet address of the configured node
 ******************************************************************************/
void app_ui_on_node_configuration_end(uint16_t netkey_index,
                                      uint16_t server_address);

/***************************************************************************/ /**
 * Callback to inform when the nodeinfo query ends
 *
 ******************************************************************************/
void app_on_nodeinfo_end(void);

/***************************************************************************/ /**
 * Parse UUID from the given string
 *
 * @param[in] input The string to parse
 * @param[in] length Length of the string
 * @param[out] parsed_uuid The struct to fill with the parsed data
 ******************************************************************************/
void app_parse_uuid(char *input, size_t length, uuid_128 *parsed_uuid);

/***************************************************************************/ /**
 * Parse primary element address from the given string
 *
 * @param[in] input The string to parse
 * @param[in] length Length of the string
 * @param[out] address The variable to fill with the parsed data
 ******************************************************************************/
void app_parse_address(char *input, size_t length, uint16_t *address);

/***************************************************************************/ /**
 * Callback to inform when the nodeinfo query ends in UI mode
 *
 ******************************************************************************/
void app_ui_on_nodeinfo_end(void);

/***************************************************************************/ /**
 * Set both the current command and command state
 *
 * @param[in] new_command The new requested command
 * @param[in] new_state The new requested state
 ******************************************************************************/
void set_command_and_state(command_t new_command, command_state_t new_state);

/***************************************************************************/ /**
 * Handle the main menu in UI mode
 *
 ******************************************************************************/
void handle_ui(void);

/***************************************************************************/ /**
 * Handle scan functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_scan(void);

/***************************************************************************/ /**
 * Handle provision functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_provision(void);

/***************************************************************************/ /**
 * Handle nodelist functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_nodelist(void);

/***************************************************************************/ /**
 * Handle nodeinfo functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_nodeinfo(void);

/***************************************************************************/ /**
 * Handle remove functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_remove(void);

/***************************************************************************/ /**
 * Handle reset functionality in UI mode
 *
 ******************************************************************************/
void handle_ui_reset(void);

/***************************************************************************/ /**
 * Callback to inform when the DDB list is ready in UI mode
 *
 ******************************************************************************/
void btmesh_prov_on_ddb_list_ready_ui(void);

/***************************************************************************/ /**
 * Callback to inform when provision failed in UI mode
 *
 ******************************************************************************/
void btmesh_prov_on_provision_failed_evt_ui(void);

#ifdef __cplusplus
};
#endif

#endif // APP_H
