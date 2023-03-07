/***************************************************************************//**
 * @file
 * @brief app_cli.c
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
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include "em_chip.h"
#include "app_log.h"
#include "sl_cli.h"
#include "app_process.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  #include "sl_flex_util_802154_protocol_types.h"
  #include "sl_flex_ieee802154_support.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Used for indicates the current status of forwarding rx packets on UART
#define ON        "ON"
/// Used for indicates the current status of forwarding rx packets on UART
#define OFF       "OFF"
/// Used for indicates the current status of auto-ACK
#define ENABLED   "ENABLED"
/// Used for indicates the current status of auto-ACK
#define DISABLED  "DISABLED"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating transmit request (button was pressed / CLI transmit request
/// has occurred)
extern volatile bool tx_requested;
/// Flag, indicating received packet is forwarded on CLI or not
extern volatile bool rx_requested;
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
/// contains all requests can be requested via cli
extern volatile sl_flex_ieee802154_cli_requests cli_requests;
/// contains all desired values via cli
extern volatile sl_flex_ieee802154_cli_data cli_desired_settings;
/// IEEE 802.15.4 communication status
extern sl_flex_ieee802154_status_t comm_status;
#endif

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
/// Used for response which standard has been selected
static const char *std[3] = {
  "IEEE 802.15.4 (2.4GHz)",
  "IEEE 802.15.4g (863MHz)",
  "IEEE 802.15.4g (915MHz)"
};
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - info message: Unique ID of the board
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  app_log_info("Info:\n");
  app_log_info("  MCU Id:       0x%llx\n", SYSTEM_GetUnique());
  app_log_info("  Fw RX Packet: %s\n", (rx_requested == true) ? ON : OFF);

  switch (_SILICON_LABS_EFR32_RADIO_TYPE) {
    case _SILICON_LABS_EFR32_RADIO_DUALBAND:
      app_log_info("  Radio type:   Dual band\n");
      break;
    case _SILICON_LABS_EFR32_RADIO_2G4HZ:
      app_log_info("  Radio type:   2P4GHz\n");
      break;
    case _SILICON_LABS_EFR32_RADIO_SUBGHZ:
      app_log_info("  Radio type:   SubGHz\n");
      break;
    default:
      break;
  }

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  app_log_info("Status:\n");
  app_log_info("  Std:          %s\n", std[comm_status.std]);
  app_log_info("  Channel:      %d\n", comm_status.channel);
  app_log_info("  Auto ACK:     %s\n",
               (comm_status.auto_ack == true) ? ENABLED : DISABLED);
  app_log_info("  ACK:          %s\n",
               (comm_status.ack == true) ? ENABLED : DISABLED);
  app_log_info("  PAN ID:       0x%04X\n", comm_status.destination_pan_id);
  app_log_info("  Dest. addr.:  0x%04X\n", comm_status.destination_address);
  app_log_info("  Src. addr.:   0x%04X\n", comm_status.source_address);

  if ((comm_status.std == SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ)
      || (comm_status.std == SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ)) {
    app_log_info("802.15.4g specific:\n");
    app_log_info("  Data wh:      %s\n",
                 (comm_status.data_whitening == true) ? ON : OFF);
    app_log_info("  CRC length:   %dBytes\n", comm_status.crc_length);
  }
#endif
}

/******************************************************************************
 * CLI - send: Sets a flag indicating that a packet has to be sent
 *****************************************************************************/
void cli_send_packet(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  tx_requested = true;
  app_log_info("Send packet request\n");
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - receive: Turn on/off received message
 *****************************************************************************/
void cli_receive_packet(sl_cli_command_arg_t *arguments)
{
  uint8_t rxForward = sl_cli_get_argument_uint8(arguments, 0);
  const char* str_rx_fw = "";
  if (rxForward == 0) {
    rx_requested = false;
    str_rx_fw = OFF;
    app_log_info("Received packets: %s\n", str_rx_fw);
  } else if (rxForward == 1) {
    rx_requested = true;
    str_rx_fw = ON;
    app_log_info("Received packets: %s\n", str_rx_fw);
  } else {
    app_log_info("Wrong parameter\n");
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - std: Sets the standard (IEEE 802.15.4e or IEEE 802.15.4g)
 *****************************************************************************/
void cli_std(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  char *arg0 = sl_cli_get_argument_string(arguments, 0);  // fix argument
#if RF_BAND_868 || RF_BAND_915 || RF_BAND_914 || RF_BAND_924
  char *arg1 = NULL;                                      // optional argument
  uint8_t arg_cnt = sl_cli_get_argument_count(arguments);
#endif
  // checks the first parameter
  if ((*arg0 != 'b') && (*arg0 != 'g')) {
    app_log_info("Wrong argument: %s\n", arg0);
    return;
  }

  // 802154
  if (*arg0 == 'b') {
#if (RF_BAND_2P4)
    cli_desired_settings.desired_std = SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ;
    app_log_info("IEEE 802.15.4 2.4GHz standard set.\n");
    cli_requests.std_requested = true;
#else
    app_log_info("IEEE 802.15.4 is unsupported.\n");
#endif
  }

  // 802154g
  if (*arg0 == 'g') {
#if RF_BAND_868 || RF_BAND_915 || RF_BAND_914 || RF_BAND_924

    // checks the number of arguments for 802.15.4g standard
    if (arg_cnt == 2) {
      arg1 = sl_cli_get_argument_string(arguments, 1);
    } else {
      app_log_info("Wrong argument of 802.15.4g: the frequency designator is missing\n");
      return;
    }

    // checks the second parameter
    if (strcmp(arg1, "863") && strcmp(arg1, "915")) {
      app_log_info("Wrong argument of 802.15.4g: %s\n", arg1);
      return;
    }

    // 863
    if (!strcmp(arg1, "863")) {
#if RF_BAND_868
      cli_desired_settings.desired_std = SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ;
      app_log_info("IEEE 802.15.4g 863MHz standard set.\n");
      cli_requests.std_requested = true;
#else
      app_log_info("IEEE 802.15.4g 863MHz is unsupported.\n");
#endif
    }

    // 915
    if (!strcmp(arg1, "915")) {
#if RF_BAND_915 || RF_BAND_914 || RF_BAND_924
      cli_desired_settings.desired_std = SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ;
      app_log_info("IEEE 802.15.4g 915MHz standard set.\n");
      cli_requests.std_requested = true;
#else
      app_log_info("IEEE 802.15.4g 915MHz is unsupported.\n");
#endif
    }
#else
    app_log_info("IEEE 802.15.4g is unsupported.\n");
#endif
  }

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - panid: Sets PAN ID
 *****************************************************************************/
void cli_pan_id(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  uint16_t arg = 0U;
  (void)sl_cli_get_argument_hex(arguments, 0, (size_t*)&arg);

  // the limited rage of the data type allows that there is no argument check
  // 0x0000 - 0xFFFF
  cli_requests.panid_requested = true;
  cli_desired_settings.desired_panid = arg;
  app_log_info("PAN ID: %d\n", arg);

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - srcaddr: Sets short address of source
 *****************************************************************************/
void cli_src_addr(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  uint16_t arg = 0U;
  (void)sl_cli_get_argument_hex(arguments, 0, (size_t*)&arg);

  // the limited rage of the data type allows that there is no argument check
  // 0x0000 - 0xFFFF
  cli_requests.srcaddr_requested = true;
  cli_desired_settings.desired_srcaddr = arg;
  app_log_info("Source Address: %d\n", arg);

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - destaddr: Sets short address of destination
 *****************************************************************************/
void cli_dest_addr(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  uint16_t arg = 0U;
  (void)sl_cli_get_argument_hex(arguments, 0, (size_t*)&arg);

  // the limited rage of the data type allows that there is no argument check
  // 0x0000 - 0xFFFF
  cli_requests.destaddr_requested = true;
  cli_desired_settings.desired_destaddr = arg;
  app_log_info("Destination Address: %d\n", arg);

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - ackreq: ACK is requested or not
 *****************************************************************************/
void cli_ack_req(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  uint8_t arg = sl_cli_get_argument_uint8(arguments, 0);

  if ((arg == 1) || (arg == 0)) {
    cli_requests.ack_requested = true;
    if (arg == 1) {
      cli_desired_settings.desired_ack = true;
    } else {
      cli_desired_settings.desired_ack = false;
    }
    app_log_info("ACK is %s\n", (arg == 1) ? "requested" : "NOT requested");
  } else {
    app_log_info("Wrong parameter\n");
  }
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - configdw: Configurates the IEEE Std. 802.15.4g whitening
 *****************************************************************************/
void cli_cfg_dw(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT

  if (sl_flex_ieee802154_get_std() != SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ) {
#if RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX \
    && RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX
    uint8_t arg_dw = sl_cli_get_argument_uint8(arguments, 0);

    if ((arg_dw == 1) || (arg_dw == 0)) {
      cli_requests.cfgdw_requested = true;
      if (arg_dw == 0) {
        cli_desired_settings.desired_dw = false;
      } else {
        cli_desired_settings.desired_dw = true;
      }
      app_log_info("Config - Whitening: %s\n", (arg_dw == 0) ? OFF : ON);
    } else {
      app_log_info("Wrong parameter\n");
    }
#else
    (void)arguments;
    app_log_info("Config - Whitening: ON, Unwhitening is not supported on xg1 and xg23.\n");
#endif
  } else {
    (void)arguments;
    app_log_info("Unsupported command\n");
  }

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - configcrc: Configurates the IEEE Std. 802.15.4g CRC
 *****************************************************************************/
void cli_cfg_crc(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT

  if (sl_flex_ieee802154_get_std() != SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ) {
#if RAIL_FEAT_IEEE802154_G_4BYTE_CRC_SUPPORTED
    uint8_t arg_crc = sl_cli_get_argument_uint8(arguments, 0);

    if ((arg_crc == 2) || (arg_crc == 4)) {
      cli_requests.cfgcrc_requested = true;
      if (arg_crc == SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE) {
        cli_desired_settings.desired_crc = SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE;
      } else {
        cli_desired_settings.desired_crc = SL_FLEX_IEEE802154G_CRC_LENGTH_4BYTE;
      }
      app_log_info("Config - CRC size: %d\n", arg_crc);
    } else {
      app_log_info("Wrong parameter\n");
    }
#else
    (void)arguments;
    app_log_info("Config - CRC size: 2, the 4-bytes CRC is not supported on xg1 and xg23.\n");
#endif
  } else {
    (void)arguments;
    app_log_info("Unsupported command\n");
  }

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  (void)arguments;
  app_log_info("Unsupported command\n");
#else
  (void)arguments;
  app_log_info("Unsupported command\n");
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
