/***************************************************************************/ /**
 * @file
 * @brief CPC System Endpoint RCP API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include <string.h>

#include "em_rmu.h"
#include "sli_cpc.h"
#include "sli_cpc_drv.h"
#include "sli_cpc_hdlc.h"
#include "sli_cpc_system_common.h"
#include "sli_mem_pool.h"
#include "sl_atomic.h"
#include "sl_component_catalog.h"
#include "sl_cpc.h"
#include "sl_cpc_config.h"
#include "sl_gsdk_version.h"

#if (defined(SL_CATALOG_CPC_BOOTLOADER_INTERFACE_PRESENT))
#include "btl_interface.h"
#endif

#if (defined(SL_CATALOG_CPC_SECURITY_PRESENT))
#include "sli_cpc_security.h"
#endif

#define CPC_PACKED_ENDPOINT_PRESENT 0   // Not yet implemented

/*******************************************************************************
 ***************************  GLOBAL VARIABLES   ********************************
 ******************************************************************************/

extern sli_cpc_drv_capabilities_t sli_cpc_driver_capabilities;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static bool restart_flag = false;

static uint32_t enter_irq_end_ms;
static sl_sleeptimer_timer_handle_t enter_irq_timer;

static bool process_uframes_flag = false;

static uint32_t fc_validation_value;

/***************************************************************************//**
 * System endpoint handle
 ******************************************************************************/
static sl_cpc_endpoint_handle_t system_ep = { .ep = NULL };

/***************************************************************************//**
 * bootloader_reboot_mode.
 * 0 - At the next reboot application is executed.
 * 1 -  At the next reboot bootloader is executed. Depending on the
 *      configuration of the bootloader, it either updates the application or
 *      starts an NCP mode.
 ******************************************************************************/
static sli_cpc_system_reboot_mode_t prop_bootloader_reboot_mode = REBOOT_APPLICATION;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

static void on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                               void *buffer,
                               void *arg,
                               sl_status_t status);

static sl_status_t open_endpoint(void);
static void on_system_ep_error(sl_cpc_user_endpoint_id_t endpoint_id);

static void send_reset_reason(void);

static void on_poll(uint8_t endpoint_id,
                    void *arg,
                    void *poll_data,
                    uint32_t poll_data_length,
                    void **reply_data,
                    uint32_t * reply_data_lenght,
                    void **on_write_complete_arg);

static void enter_irq_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                     void *data);

__WEAK void system_on_information_received(uint8_t endpoint_id, void *arg);
__WEAK const char* sl_cpc_secondary_app_version(void);

/***************************************************************************//**
 * Called when secondary app version is requested.
 * The format is up to the user. The string should be null terminated.
 ******************************************************************************/
__WEAK const char* sl_cpc_secondary_app_version(void)
{
  return "UNDEFINED";
}

/***************************************************************************//**
 * Initialize CPC System
 ******************************************************************************/
static sl_status_t open_endpoint(void)
{
  sl_status_t status;

  status = sli_cpc_open_service_endpoint(&system_ep, SL_CPC_ENDPOINT_SYSTEM, SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE, 1);
  if (status == SL_STATUS_ALREADY_EXISTS) {
    return SL_STATUS_ALREADY_INITIALIZED;
  } else if (status != SL_STATUS_OK) {
    return status;
  }

  status = sl_cpc_set_endpoint_option(&system_ep, SL_CPC_ENDPOINT_ON_POLL, (void *)on_poll);
  if (status != SL_STATUS_OK) {
    sl_cpc_close_endpoint(&system_ep);
    return status;
  }

  if (system_on_information_received) {
    status = sl_cpc_set_endpoint_option(&system_ep, SL_CPC_ENDPOINT_ON_UFRAME_RECEIVE, (void *)system_on_information_received);
    if (status != SL_STATUS_OK) {
      sl_cpc_close_endpoint(&system_ep);
      return status;
    }
  }

  status = sl_cpc_set_endpoint_option(&system_ep, SL_CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED, (void *)on_write_completed);
  if (status != SL_STATUS_OK) {
    sl_cpc_close_endpoint(&system_ep);
    return status;
  }

  status = sl_cpc_set_endpoint_option(&system_ep, SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void *)on_write_completed);
  if (status != SL_STATUS_OK) {
    sl_cpc_close_endpoint(&system_ep);
    return status;
  }

  status = sl_cpc_set_endpoint_option(&system_ep, SL_CPC_ENDPOINT_ON_ERROR, (void *)on_system_ep_error);
  if (status != SL_STATUS_OK) {
    sl_cpc_close_endpoint(&system_ep);
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Process the system endpoint
 ******************************************************************************/
void sli_cpc_system_process(void)
{
  sl_cpc_endpoint_state_t state;

  if (restart_flag) {
    state = sl_cpc_get_endpoint_state(&system_ep);

    if (state == SL_CPC_STATE_FREED) {
      EFM_ASSERT(open_endpoint() == SL_STATUS_OK);
      restart_flag = false;
    } else {
      if (state != SL_CPC_STATE_CLOSED) {
        sl_cpc_close_endpoint(&system_ep);
      }
    }
  }

  if (process_uframes_flag) {
    void *data;
    uint16_t data_length;
    sl_status_t status = sl_cpc_read(&system_ep, &data, &data_length, 0, SL_CPC_FLAG_NO_BLOCK | SL_CPC_FLAG_UNNUMBERED_INFORMATION);
    if (status == SL_STATUS_OK) {
      if (data_length > 0) {
        fc_validation_value += ((uint8_t *)data)[0];
      }
      sl_cpc_free_rx_buffer(data);
    }
  }
}

/***************************************************************************//**
 * Initialize CPC System
 ******************************************************************************/
sl_status_t sli_cpc_system_init(void)
{
  sl_status_t status = open_endpoint();

  send_reset_reason();

  return status;
}

/***************************************************************************//**
 * System endpoint on error callback
 ******************************************************************************/
static void on_system_ep_error(sl_cpc_user_endpoint_id_t endpoint_id)
{
  EFM_ASSERT(endpoint_id == SL_CPC_ENDPOINT_SYSTEM);
  restart_flag = true;
}

/***************************************************************************//**
 * System endpoint on write completed callback
 ******************************************************************************/
static void on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                               void *buffer,
                               void *arg,
                               sl_status_t status)
{
  (void) endpoint_id;
  (void) status;

  // Unnumbered ACK has no payload
  if (buffer != NULL) {
    sli_cpc_free_command_buffer(buffer);
  }

  if ((uint32_t) arg == 0xDEADBEEF) {
#if (defined(SL_CATALOG_CPC_BOOTLOADER_INTERFACE_PRESENT))
    // The reset command asked to perform a reset.

    BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (RAM_MEM_BASE);

    // Set reset reason to bootloader entry
    switch (prop_bootloader_reboot_mode) {
      case REBOOT_APPLICATION:
        resetCause->reason = BOOTLOADER_RESET_REASON_GO;
        break;
      case REBOOT_BOOTLOADER:
        resetCause->reason = BOOTLOADER_RESET_REASON_BOOTLOAD;
        break;
      default:
        return;
    }

    resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;
#endif
#if defined(RMU_PRESENT)
    // Clear resetcause
    RMU->CMD = RMU_CMD_RCCLR;
    // Trigger a software system reset
    RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_EXTENDED;
#endif
    NVIC_SystemReset();
  }
}

/***************************************************************************//**
 * Endpoint was closed, notify the host
 ******************************************************************************/
sl_status_t sli_cpc_send_disconnection_notification(uint8_t endpoint_id)
{
  sl_status_t status;
  sli_cpc_system_cmd_t *tx_command;
  sli_cpc_system_property_cmd_t *tx_prop_command;
  sl_cpc_endpoint_state_t *ep_state;

  status = sli_cpc_get_system_command_buffer(&tx_command);
  if (status != SL_STATUS_OK) {
    return status;
  }

  tx_command->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

  tx_command->header.seq = 0;

  tx_prop_command = (sli_cpc_system_property_cmd_t*)(tx_command->payload);

  tx_prop_command->property_id = EP_ID_TO_PROPERTY_ID(endpoint_id);

  ep_state = (sl_cpc_endpoint_state_t *)tx_prop_command->payload;

  tx_command->header.length = sizeof(sli_cpc_system_property_cmd_t) + sizeof(sl_cpc_endpoint_state_t);

  *ep_state = SL_CPC_STATE_CLOSING;

  status = sl_cpc_write(&system_ep,
                        (void *)tx_command,
                        sizeof(sli_cpc_system_cmd_header_t) + tx_command->header.length,
                        0,
                        NULL);

  EFM_ASSERT(status == SL_STATUS_OK);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Send reset reason
 ******************************************************************************/
static void send_reset_reason(void)
{
  sl_status_t status;
  sli_cpc_system_cmd_t *tx_command;
  sli_cpc_system_property_cmd_t *tx_prop_command;
  sl_cpc_system_status_t *tx_prop_last_status;
  uint32_t reset_cause;

  status = sli_cpc_get_system_command_buffer(&tx_command);
  EFM_ASSERT(status == SL_STATUS_OK);

  tx_command->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

  /* Seq is irrelevant in u-frame information */
  tx_command->header.seq = 0;

  tx_prop_command = (sli_cpc_system_property_cmd_t*)(tx_command->payload);

  tx_prop_command->property_id = PROP_LAST_STATUS;

  tx_prop_last_status = (sl_cpc_system_status_t*)tx_prop_command->payload;

  reset_cause = RMU_ResetCauseGet();

  *tx_prop_last_status = STATUS_RESET_UNKNOWN;

#if defined(_RMU_RSTCAUSE_MASK)
  if (reset_cause & RMU_RSTCAUSE_PORST) {
    *tx_prop_last_status = STATUS_RESET_POWER_ON;
  } else if (reset_cause & RMU_RSTCAUSE_AVDDBOD) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  } else if (reset_cause & RMU_RSTCAUSE_DVDDBOD) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  } else if (reset_cause & RMU_RSTCAUSE_DECBOD) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  } else if (reset_cause & RMU_RSTCAUSE_EXTRST) {
    *tx_prop_last_status = STATUS_RESET_EXTERNAL;
  } else if (reset_cause & RMU_RSTCAUSE_WDOGRST) {
    *tx_prop_last_status = STATUS_RESET_WATCHDOG;
  } else if (reset_cause & RMU_RSTCAUSE_LOCKUPRST) {
    *tx_prop_last_status = STATUS_RESET_CRASH;
  } else if (reset_cause & RMU_RSTCAUSE_SYSREQRST) {
    *tx_prop_last_status = STATUS_RESET_SOFTWARE;
  } else if (reset_cause & RMU_RSTCAUSE_EM4RST) {
    *tx_prop_last_status = STATUS_RESET_OTHER;
  }
#else
#if defined(_EMU_RSTCTRL_AVDDBODRMODE_MASK)
  if (reset_cause & rmuResetAVDD) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  }
#endif
#if defined(_EMU_RSTCTRL_IOVDD0BODRMODE_MASK)
  if (reset_cause & rmuResetIOVDD0) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  }
#endif
#if defined(_EMU_RSTCTRL_DECBODRMODE_MASK)
  if (reset_cause & rmuResetDecouple) {
    *tx_prop_last_status = STATUS_RESET_FAULT;
  }
#endif
#if defined(_EMU_RSTCTRL_WDOG0RMODE_MASK)
  if (reset_cause & rmuResetWdog0) {
    *tx_prop_last_status = STATUS_RESET_WATCHDOG;
  }
#endif
#if defined(_EMU_RSTCTRL_WDOG1RMODE_MASK)
  if (reset_cause & rmuResetWdog1) {
    *tx_prop_last_status = STATUS_RESET_WATCHDOG;
  }
#endif
#if defined(_EMU_RSTCTRL_LOCKUPRMODE_MASK)
  if (reset_cause & rmuResetCoreLockup) {
    *tx_prop_last_status = STATUS_RESET_CRASH;
  }
#endif
#if defined(_EMU_RSTCTRL_SELOCKUPRMODE_MASK)
  if (reset_cause & rmuResetSELockup) {
    *tx_prop_last_status = STATUS_RESET_CRASH;
  }
#endif
#if defined(_EMU_RSTCTRL_SYSRMODE_MASK)
  if (reset_cause & rmuResetSys) {
    *tx_prop_last_status = STATUS_RESET_SOFTWARE;
  }
#endif
#if defined(_EMU_RSTCTRL_SESYSRMODE_MASK)
  if (reset_cause & rmuResetSESys) {
    *tx_prop_last_status = STATUS_RESET_SOFTWARE;
  }
#endif
#endif

  RMU_ResetCauseClear();

  tx_command->header.length = sizeof(sli_cpc_system_property_cmd_t) + sizeof(sl_cpc_system_status_t);

  status = sl_cpc_write(&system_ep,
                        (void *)tx_command,
                        sizeof(sli_cpc_system_cmd_header_t) + tx_command->header.length,
                        SL_CPC_FLAG_UNNUMBERED_INFORMATION,
                        NULL);

  EFM_ASSERT(status == SL_STATUS_OK);
}

/*******************************************************************************
 ***************************  PROPERTY-GET HANDLERS  ***************************
 ******************************************************************************/

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_LAST_STATUS
 *   When replying to any CMD_PROPERTY_GET/SET which result in an unsuccessful
 *   operation, the RCP will reply with PROP_LAST_STATUS instead. Here, this
 *   property is specifically queried. There is nothing more meaningful than
 *   to reply with a STATUS_OK.
 ******************************************************************************/
static void on_property_get_last_status(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_LAST_STATUS;
  *((sl_cpc_system_status_t*)(prop_cmd_buff->payload)) = STATUS_OK;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sl_cpc_system_status_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_PROTOCOL_VERSION
 *   Ship the hardcoded major and minor version number back to the primary.
 ******************************************************************************/
static void on_property_get_protocol_version(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_PROTOCOL_VERSION;
  prop_cmd_buff->payload[0] = SLI_CPC_PROTOCOL_VERSION;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(uint8_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_SECONDARY_CPC_VERSION
 *   Ship the hardcoded major and minor version number back to the primary.
 ******************************************************************************/
static void on_property_get_secondary_cpc_version(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;
  uint32_t* version;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_SECONDARY_CPC_VERSION;
  version = (uint32_t*)(prop_cmd_buff->payload);

  version[0] = SL_GSDK_MAJOR_VERSION;
  version[1] = SL_GSDK_MINOR_VERSION;
  version[2] = SL_GSDK_PATCH_VERSION;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + (3 * sizeof(uint32_t));
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_SECONDARY_APP_VERSION
 *   Send a string version of the secondary application to the primary
 ******************************************************************************/
static void on_property_get_secondary_app_version(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;
  const char* app_version;
  uint32_t app_version_len;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_SECONDARY_APP_VERSION;
  app_version = sl_cpc_secondary_app_version();
  app_version_len = strlen(app_version) + 1;
  memcpy(prop_cmd_buff->payload, app_version, app_version_len > SL_CPC_RX_PAYLOAD_MAX_LENGTH ? SL_CPC_RX_PAYLOAD_MAX_LENGTH : app_version_len);

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + app_version_len;
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_CAPABILITIES
 *   Send the capabilities of the secondary to the primary
 ******************************************************************************/
static void on_property_get_capabilities(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;
  uint32_t *capabilities;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_CAPABILITIES;
  capabilities = (uint32_t*)(prop_cmd_buff->payload);

  *capabilities = SL_CPC_ENDPOINT_SECURITY_ENABLED * CPC_CAPABILITIES_SECURITY_ENDPOINT_MASK
                  + CPC_PACKED_ENDPOINT_PRESENT * CPC_CAPABILITIES_PACKED_ENDPOINT_MASK
                  + SLI_CPC_ENDPOINT_GPIO_ENABLED * CPC_CAPABILITIES_GPIO_ENDPOINT_MASK
                  + sli_cpc_driver_capabilities.uart_flowcontrol * CPC_CAPABILITIES_UART_FLOW_CONTROL_MASK;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_RX_CAPABILITY
 *   Send the rx buffer capability of the secondary to the primary
 ******************************************************************************/
static void on_property_get_rx_capabilities(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;
  uint16_t *rx_capability;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_RX_CAPABILITY;
  rx_capability = (uint16_t *)(prop_cmd_buff->payload);
  *rx_capability = SL_CPC_RX_PAYLOAD_MAX_LENGTH;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(uint16_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_FC_VALIDATION_VALUE
 *   Send the flow control validation value of the secondary to the primary
 ******************************************************************************/
static void on_property_get_fc_validation_value(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *prop_cmd_buff;
  uint32_t *flow_control_validation_value;

  prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  prop_cmd_buff->property_id = PROP_FC_VALIDATION_VALUE;
  flow_control_validation_value = (uint32_t *)(prop_cmd_buff->payload);
  *flow_control_validation_value = fc_validation_value;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_BOOTLOADER_INFO
 *   Reply to the PRIMARY the bootloader infos.
 ******************************************************************************/
#if (defined(SL_CATALOG_CPC_BOOTLOADER_INTERFACE_PRESENT))
static void on_property_get_bootloader_info(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *tx_property;
  BootloaderInformation_t bootloader_infos;
  uint32_t* infos;

  tx_property = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property->property_id = PROP_BOOTLOADER_INFO;
  infos = (uint32_t*)(tx_property->payload);

  bootloader_getInfo(&bootloader_infos);

  infos[0] = (uint32_t)bootloader_infos.type;
  infos[1] = (bootloader_infos.type == SL_BOOTLOADER) ? bootloader_infos.version : 0xFFFFFFFF;
  infos[2] = bootloader_infos.capabilities;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + 3 * sizeof(uint32_t);
}
#endif

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_BOOTLOADER_REBOOT_MODE
 *   Reply to the PRIMARY the bootloader reboot mode.
 ******************************************************************************/
static void on_property_get_bootloader_reboot_mode(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *tx_property;
  sli_cpc_system_reboot_mode_t* mode;

  tx_property = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property->property_id = PROP_BOOTLOADER_REBOOT_MODE;
  mode = (sli_cpc_system_reboot_mode_t*)(tx_property->payload);
  *mode = prop_bootloader_reboot_mode;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sli_cpc_system_reboot_mode_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_SECURITY_STATE
 *   Reply to the PRIMARY the security state.
 ******************************************************************************/
static void on_property_get_security_state(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *tx_property;
  uint32_t* security_state;

  tx_property = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  security_state = (uint32_t*)(tx_property->payload);

#ifdef SL_CATALOG_CPC_SECURITY_PRESENT
  tx_property->property_id = PROP_SECURITY_STATE;
  *security_state = sl_cpc_security_get_state();
#else
  tx_property->property_id = PROP_LAST_STATUS;
  *security_state = STATUS_UNIMPLEMENTED;
#endif

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_ENDPOINT_STATE_x
 *   The primary queried the status of a specific endpoint number.
 ******************************************************************************/
static void on_property_get_endpoint_state(sli_cpc_system_cmd_t *tx_command,
                                           uint8_t ep_id)
{
  sli_cpc_system_property_cmd_t *reply_prop_cmd_buff;
  sl_cpc_endpoint_state_t *reply_ep_state;
  sl_cpc_endpoint_handle_t dummy_ep;

  reply_prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  reply_ep_state = (sl_cpc_endpoint_state_t*) reply_prop_cmd_buff->payload;
  dummy_ep.ep = NULL;
  dummy_ep.id = ep_id;

  reply_prop_cmd_buff->property_id = EP_ID_TO_PROPERTY_ID(ep_id);

  *reply_ep_state = sl_cpc_get_endpoint_state(&dummy_ep);

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sl_cpc_endpoint_state_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_ENDPOINT_STATES
 *   The primary queried the status of all endpoints.
 ******************************************************************************/
static void on_property_get_endpoint_states(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *reply_prop_cmd_buff;
  uint8_t *reply_ep_states;

  reply_prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  reply_ep_states = (uint8_t*)(reply_prop_cmd_buff->payload);
  reply_prop_cmd_buff->property_id = PROP_ENDPOINT_STATES;

  // process 2 endpoints per iteration
  for (size_t i = 0; i != SL_CPC_ENDPOINT_MAX_COUNT / 2; i++) {
    sl_cpc_endpoint_handle_t dummy_ep1 = { .ep = NULL, .id = 2 * i };
    sl_cpc_endpoint_handle_t dummy_ep2 = { .ep = NULL, .id = 2 * i + 1 };

    sl_cpc_endpoint_state_t ep1_state = sl_cpc_get_endpoint_state(&dummy_ep1);
    sl_cpc_endpoint_state_t ep2_state = sl_cpc_get_endpoint_state(&dummy_ep2);

    // Although an 'sl_cpc_endpoint_state_t' is an 8 bit value, the number of
    // values in the enum makes it possible to encode it with a nibble (4 bits)
    // as only 3 bits are required to encode those 6 values. Put the first
    // endpoint in the low nibble and the second in the high nibble.
    // This aggregation will make it possible to send all 256 endpoint states
    // in one reply as it will fit within 255 bytes (limited by the length field
    // within a command frame)
    reply_ep_states[i] = (ep2_state << 4) | ep1_state;
  }

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + (SL_CPC_ENDPOINT_MAX_COUNT * sizeof(uint8_t) / 2);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_GET
 * Property ID: PROP_DEBUG_COUNTERS
 *   The primary queried the debug counters.
 ******************************************************************************/
#if (SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1)
static void on_property_get_core_debug_counters(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *reply_prop_cmd_buff;

  reply_prop_cmd_buff = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  memcpy(reply_prop_cmd_buff->payload, &sl_cpc_core_debug.core_counters, sizeof(sl_cpc_core_debug_counters_t));

  reply_prop_cmd_buff->property_id = PROP_CORE_DEBUG_COUNTERS;
  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sl_cpc_core_debug_counters_t);
}
#endif

/***************************************************************************//**
 * Handler for when the primary asks about a property not found:
 *   As with any property-get/set which is unsuccessful, the rcp replies with
 *   a property id of PROP_LAST_STATUS. Since the property the primary asked about
 *   can't be handled by the rcp, the status returned is STATUS_PROP_NOT_FOUND.
 ******************************************************************************/
static void on_property_get_set_not_found(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *tx_property_cmd;

  tx_property_cmd = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property_cmd->property_id = PROP_LAST_STATUS;
  *((sl_cpc_system_status_t*)(tx_property_cmd->payload)) = STATUS_PROP_NOT_FOUND;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sl_cpc_system_status_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_SET:
 * Property ID: PROP_ENDPOINT_STATE_x
 *   The primary notifies the secondary of an endpoint state change
 ******************************************************************************/
static void on_property_set_state(uint8_t endpoint_id,
                                  sli_cpc_system_cmd_t *tx_command,
                                  sli_cpc_system_cmd_t *rx_command)
{
  (void)rx_command;

  sli_cpc_system_property_cmd_t *tx_property_command;
  tx_property_command = (sli_cpc_system_property_cmd_t*) tx_command->payload;

  sli_cpc_remote_disconnected(endpoint_id);

  tx_command->header.length = sizeof(sli_cpc_property_id_t);
  tx_property_command->property_id = EP_ID_TO_PROPERTY_ID(endpoint_id);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_SET:
 * Property ID: PROP_UFRAME_PROCESSING
 *   The primary enables/disables u-frame processing on the secondary.
 ******************************************************************************/
static void on_property_set_uframe_processing(sli_cpc_system_cmd_t *tx_command,
                                              sli_cpc_system_cmd_t *rx_command)
{
  sli_cpc_system_property_cmd_t *tx_property_command;
  sli_cpc_system_property_cmd_t *rx_property_command;

  rx_property_command = (sli_cpc_system_property_cmd_t*) rx_command->payload;
  process_uframes_flag = *(bool*)rx_property_command->payload;

  tx_property_command = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property_command->property_id = PROP_UFRAME_PROCESSING;
  tx_command->header.length = sizeof(sli_cpc_property_id_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_SET:
 * Property ID: PROP_ENTER_IRQ
 *   The primary requests the secondary to enter IRQ in start_ms for end_ms time.
 ******************************************************************************/
static void on_property_set_enter_irq(sli_cpc_system_cmd_t *tx_command,
                                      sli_cpc_system_cmd_t *rx_command)
{
  sli_cpc_system_property_cmd_t *tx_property_command;
  sli_cpc_system_property_cmd_t *rx_property_command;
  sli_cpc_system_enter_irq_cmd_t *enter_irq_command;

  rx_property_command = (sli_cpc_system_property_cmd_t*) rx_command->payload;
  enter_irq_command = (sli_cpc_system_enter_irq_cmd_t*) rx_property_command->payload;
  enter_irq_end_ms = enter_irq_command->end_in_ms;
  sl_sleeptimer_start_timer_ms(&enter_irq_timer,
                               enter_irq_command->start_in_ms,
                               enter_irq_timer_callback,
                               &enter_irq_end_ms,
                               0,
                               0);

  tx_property_command = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property_command->property_id = PROP_ENTER_IRQ;
  tx_command->header.length = sizeof(sli_cpc_property_id_t);
}

/***************************************************************************//**
 * Command ID:  CMD_PROPERTY_SET:
 * Property ID: PROP_BOOTLOADER_REBOOT_MODE
 *   The primary sets the reboot mode.
 ******************************************************************************/
static void on_property_set_bootloader_reboot_mode(sli_cpc_system_cmd_t *tx_command,
                                                   sli_cpc_system_cmd_t *rx_command)
{
  sli_cpc_system_property_cmd_t *tx_property_command;
  sli_cpc_system_property_cmd_t *rx_property_command;
  sli_cpc_system_reboot_mode_t *rx_mode;
  sli_cpc_system_reboot_mode_t *tx_mode;

  tx_property_command = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  rx_property_command = (sli_cpc_system_property_cmd_t*) rx_command->payload;
  rx_mode = (sli_cpc_system_reboot_mode_t*) rx_property_command->payload;
  tx_mode = (sli_cpc_system_reboot_mode_t*) tx_property_command->payload;

  tx_command->header.length = sizeof(sli_cpc_property_id_t);

  switch (*rx_mode) {
    case REBOOT_APPLICATION:
    case REBOOT_BOOTLOADER:
      prop_bootloader_reboot_mode = *rx_mode;
      tx_property_command->property_id = PROP_BOOTLOADER_REBOOT_MODE;
      *tx_mode = *rx_mode;
      tx_command->header.length += sizeof(sli_cpc_system_reboot_mode_t);
      break;

    default:
      // The requested bootloader mode doesn't exist. Reply with a
      // PROP_LAST_STATUS::STATUS_INVALID_ARGUMENT instead
      tx_property_command->property_id = PROP_LAST_STATUS;
      *((sl_cpc_system_status_t*)(tx_property_command->payload)) = STATUS_INVALID_ARGUMENT;
      tx_command->header.length += sizeof(sl_cpc_system_status_t);
      break;
  }
}

/***************************************************************************//**
 * Handler for when the primary sets a read-only property
 ******************************************************************************/
static void on_property_set_read_only(sli_cpc_system_cmd_t *tx_command)
{
  sli_cpc_system_property_cmd_t *tx_property_cmd;

  tx_property_cmd = (sli_cpc_system_property_cmd_t*) tx_command->payload;
  tx_property_cmd->property_id = PROP_LAST_STATUS;
  *((sl_cpc_system_status_t*)(tx_property_cmd->payload)) = STATUS_INVALID_COMMAND_FOR_PROP;

  tx_command->header.length = sizeof(sli_cpc_property_id_t) + sizeof(sl_cpc_system_status_t);
}

/*******************************************************************************
 ***************************  COMMAND HANDLERS  ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Handle no-op from PRIMARY:
 *   This functions is called when a no-op command is received from the PRIMARY.
 *   The RCP simply sends a no-op back so that the primary can assert the success
 *   of the operation.
 ******************************************************************************/
static void on_noop(sli_cpc_system_cmd_t *noop,
                    uint32_t *reply_data_lenght)
{
  noop->header.command_id = CMD_SYSTEM_NOOP;
  noop->header.length = 0;
  *reply_data_lenght = sizeof(sli_cpc_system_cmd_header_t) + noop->header.length;
}
/***************************************************************************//**
 * Handle reset from PRIMARY:
 *   This functions is called when a reset command is received from the PRIMARY.
 *   The secondary will emit back a reset command with a status indicating
 *   wheter or not it can reset in the desired mode dictated by 'prop_bootloader
 *   _reboot_mode'
 ******************************************************************************/
static void on_reset(sli_cpc_system_cmd_t *reset,
                     uint32_t *reply_data_lenght,
                     void **on_write_complete_arg)
{
  sl_cpc_system_status_t *reset_status;

  reset_status = (sl_cpc_system_status_t*)(reset->payload);
  reset->header.command_id = CMD_SYSTEM_RESET;

  switch (prop_bootloader_reboot_mode) {
    case REBOOT_APPLICATION:
      // We can always reboot into application
      *reset_status = STATUS_OK;
      break;

#if (defined(SL_CATALOG_CPC_BOOTLOADER_INTERFACE_PRESENT))
    case REBOOT_BOOTLOADER:
    {
      BootloaderInformation_t btl_info;
      bootloader_getInfo(&btl_info);

      // In case of bootloader reboot, check that a bootloader is present
      if (btl_info.type == NO_BOOTLOADER ) {
        *reset_status = STATUS_FAILURE;
      } else {   // SL_BOOTLOADER
        *reset_status = STATUS_OK;
      }
      break;
    }
#endif

    default:
      EFM_ASSERT(0);
      break;
  }

  reset->header.length = sizeof(sl_cpc_system_status_t);
  *reply_data_lenght = sizeof(sli_cpc_system_cmd_header_t) + reset->header.length;

  // Set on_write_complete argument to 0xDEADBEEF. This will
  // indicate to the on_write_complete callback to reset the device.
  // We need to postpone the reset to after the
  // write completion.
  if (*reset_status == STATUS_OK) {
    // The reset will be able to occurre in the mode requested, provide a
    // magic number to the callback so it knows it has to reset the device
    *on_write_complete_arg = (void*)0xDEADBEEF;
  } else {
    // The reset won't occur, don't tell the callback to reset
    *on_write_complete_arg = 0;
  }
}

/***************************************************************************//**
 * Handle property-get from PRIMARY:
 *   This functions is called when a property-get command is received from the
 *   PRIMARY. Causes the SECONDARY to emit a "CMD_PROP_VALUE_IS " command for the
 *   given property identifier.
 ******************************************************************************/
static void on_property_get(sli_cpc_system_cmd_t *rx_command,
                            sli_cpc_system_cmd_t *reply,
                            uint32_t *reply_data_lenght)
{
  sli_cpc_system_property_cmd_t *rx_property_cmd = (sli_cpc_system_property_cmd_t *)rx_command->payload;

  // Reply to a PROPERTY-GET with a PROPERTY-IS
  reply->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

  // Populate the reply command buffer according to the property_id
  switch (rx_property_cmd->property_id) {
    case PROP_LAST_STATUS:
      on_property_get_last_status(reply);
      break;

    case PROP_PROTOCOL_VERSION:
      on_property_get_protocol_version(reply);
      break;

    case PROP_SECONDARY_CPC_VERSION:
      on_property_get_secondary_cpc_version(reply);
      break;

    case PROP_SECONDARY_APP_VERSION:
      on_property_get_secondary_app_version(reply);
      break;

    case PROP_CAPABILITIES:
      on_property_get_capabilities(reply);
      break;

    case PROP_RX_CAPABILITY:
      on_property_get_rx_capabilities(reply);
      break;

    case PROP_FC_VALIDATION_VALUE:
      on_property_get_fc_validation_value(reply);
      break;

#if (defined(SL_CATALOG_CPC_BOOTLOADER_INTERFACE_PRESENT))
    case PROP_BOOTLOADER_INFO:
      on_property_get_bootloader_info(reply);
      break;
#endif

    case PROP_BOOTLOADER_REBOOT_MODE:
      on_property_get_bootloader_reboot_mode(reply);
      break;

    case PROP_SECURITY_STATE:
      on_property_get_security_state(reply);
      break;

    case PROP_ENDPOINT_STATES:
      on_property_get_endpoint_states(reply);
      break;

#if (SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1)
    case PROP_CORE_DEBUG_COUNTERS:
      on_property_get_core_debug_counters(reply);
      break;
#endif

    default:
      // Deal with endpoint state range
      if (rx_property_cmd->property_id >= PROP_ENDPOINT_STATE_0
          && rx_property_cmd->property_id <= PROP_ENDPOINT_STATE_255) {
        uint8_t ep_id = PROPERTY_ID_TO_EP_ID(rx_property_cmd->property_id);
        on_property_get_endpoint_state(reply, ep_id);
        break;
      }

      on_property_get_set_not_found(reply);
      break;
  }

  *reply_data_lenght = sizeof(sli_cpc_system_cmd_header_t) + reply->header.length;
}

/***************************************************************************//**
 * Handle property-set from PRIMARY:
 *   This functions is called when a property-set command is received from the
 *   PRIMARY. Causes the RCP to emit a "CMD_PROP_VALUE_IS " command for the given
 *   property identifier.
 ******************************************************************************/
static void on_property_set(sli_cpc_system_cmd_t* rx_command,
                            sli_cpc_system_cmd_t *reply,
                            uint32_t * reply_data_lenght)
{
  sli_cpc_system_property_cmd_t *rx_property_cmd;

  rx_property_cmd = (sli_cpc_system_property_cmd_t*)(rx_command->payload);

  // Reply to a PROPERTY-GET with a PROPERTY-IS
  reply->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

  // Populate the reply command buffer according to the property_id
  if (rx_property_cmd->property_id >= PROP_ENDPOINT_STATE_0
      && rx_property_cmd->property_id <= PROP_ENDPOINT_STATE_255) {
    on_property_set_state(PROPERTY_ID_TO_EP_ID(rx_property_cmd->property_id), reply, rx_command);
  } else {
    switch (rx_property_cmd->property_id) {
      case PROP_LAST_STATUS:
      case PROP_PROTOCOL_VERSION:
      case PROP_SECONDARY_CPC_VERSION:
      case PROP_SECONDARY_APP_VERSION:
      case PROP_CAPABILITIES:
      case PROP_BOOTLOADER_INFO:
      case PROP_SECURITY_STATE:
      case PROP_ENDPOINT_STATES:
        // All those properties fall through read-only handling
        on_property_set_read_only(reply);
        break;

      case PROP_BOOTLOADER_REBOOT_MODE:
        on_property_set_bootloader_reboot_mode(reply, rx_command);
        break;

      case PROP_UFRAME_PROCESSING:
        on_property_set_uframe_processing(reply, rx_command);
        break;

      case PROP_ENTER_IRQ:
        on_property_set_enter_irq(reply, rx_command);
        break;

      default:
        on_property_get_set_not_found(reply);
        break;
    }
  }

  *reply_data_lenght = sizeof(sli_cpc_system_cmd_header_t) + reply->header.length;
}

/***************************************************************************//**
 * Called by CPC core when uframe/poll is received
 ******************************************************************************/
static void on_poll(uint8_t endpoint_id,
                    void *arg,
                    void *poll_data,
                    uint32_t poll_data_length,
                    void **reply_data,
                    uint32_t *reply_data_lenght,
                    void **on_write_complete_arg)
{
  uint32_t frame_type = (uint32_t)arg;
  sli_cpc_system_cmd_t *rx_command = (sli_cpc_system_cmd_t *)poll_data;
  sli_cpc_system_cmd_t *tx_command;
  *reply_data = NULL;
  *reply_data_lenght = 0;
  (void)endpoint_id;

  EFM_ASSERT(frame_type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED || frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA);
  EFM_ASSERT(endpoint_id == SL_CPC_ENDPOINT_SYSTEM);
  // Make sure the length of the payload from the command matches the returned length.
  EFM_ASSERT(rx_command->header.length == (poll_data_length - sizeof(sli_cpc_system_cmd_header_t)));

  *on_write_complete_arg = NULL;

  // Allocate command buffer. Freed on acknowledgment. (On write completion callback)
  sl_status_t status = sli_cpc_get_system_command_buffer(&tx_command);

  // If no more memory, leave the reply data to NULL and let the upper layer retry later
  if (status == SL_STATUS_NO_MORE_RESOURCE) {
    return;
  }

  EFM_ASSERT(status == SL_STATUS_OK);

  *reply_data = tx_command;

  // Assign the sequence number of the request to the reply so the primary can
  // route it back to the right request.
  tx_command->header.seq = rx_command->header.seq;

  // Only the reset can be a u-frame (non encrypted)
  if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED) {
    sli_cpc_system_property_cmd_t *rx_property_cmd = (sli_cpc_system_property_cmd_t *)rx_command->payload;
    switch (rx_command->header.command_id) {
      case CMD_SYSTEM_RESET:
        on_reset((sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght, on_write_complete_arg);
        break;
      case CMD_SYSTEM_PROP_VALUE_GET:
        if (rx_property_cmd->property_id == PROP_RX_CAPABILITY
            || rx_property_cmd->property_id == PROP_CAPABILITIES
            || rx_property_cmd->property_id == PROP_PROTOCOL_VERSION
            || rx_property_cmd->property_id == PROP_SECONDARY_CPC_VERSION
            || rx_property_cmd->property_id == PROP_SECONDARY_APP_VERSION) {
          on_property_get(rx_command, (sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght);
        }
        break;
      case CMD_SYSTEM_PROP_VALUE_SET:
        if (rx_property_cmd->property_id == PROP_BOOTLOADER_REBOOT_MODE) {
          on_property_set(rx_command, (sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght);
        }
        break;
      default:
        EFM_ASSERT(false);
        break;
    }
  } else if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA) {
    switch (rx_command->header.command_id) {
      case CMD_SYSTEM_NOOP:
        on_noop((sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght);
        break;
      case CMD_SYSTEM_PROP_VALUE_GET:
        on_property_get(rx_command, (sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght);
        break;

      case CMD_SYSTEM_PROP_VALUE_SET:
        on_property_set(rx_command, (sli_cpc_system_cmd_t *)*reply_data, reply_data_lenght);
        break;

      default:
        // Command not supported
        EFM_ASSERT(false);
        break;
    }
  } else {
    EFM_ASSERT(false);
  }
}

/***************************************************************************//**
 * Function called when enter_irq_timer expires.
 ******************************************************************************/
static void enter_irq_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                     void *data)
{
  (void)handle;

  uint32_t now_ms = sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count());
  uint32_t end_ms = *(uint32_t*) data;
  while ((sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count()) - now_ms) <= end_ms) ;
}
