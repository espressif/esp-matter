/***************************************************************************//**
 * @file
 * @brief rail_util_coex_cli.c
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

#include "sl_cli.h"
#include "response_print.h"
#include "sl_rail_util_coex_config.h"
#include "sl_rail_util_ieee802154_stack_event.h"
#include "coexistence-802154.h"
#include "coexistence-ble.h"
#include "coexistence-ble-ll.h"
#include "coexistence-hal.h"
#include "rail_ble.h"
#include "rail_ieee802154.h"
#include <string.h>
#include <stdio.h>

#include "response_print.h"
#include "app_common.h"

/** Macro for getting the number of elements in an array. */
#define NUMOF_ARRAY_ELEMENTS(array)  (sizeof(array) / (sizeof(array[0])))

COEX_ENUM(COEX_CLI_GpioIndex_t) {
  COEX_CLI_GPIO_INDEX_REQ = 0,
  COEX_CLI_GPIO_INDEX_PRI = 1,
  COEX_CLI_GPIO_INDEX_GNT = 2,
  COEX_CLI_GPIO_INDEX_RHO = 3,
  COEX_CLI_GPIO_INDEX_PWM_REQ = 4,
  COEX_CLI_GPIO_INDEX_PHY_SELECT = 5,
  COEX_CLI_GPIO_INDEX_WIFI_TX = 6
};

typedef struct COEX_CLI_config {
  /** GPIO HAL config **/
  COEX_HAL_GpioConfig_t *config;

  /** GPIO HAL config handler */
  bool (*handler)(COEX_HAL_GpioConfig_t *gpioConfig);

  /** GPIO signal name **/
  char * name;

  /** GPIO index */
  COEX_CLI_GpioIndex_t index;
} COEX_CLI_config_t;

#if defined(SL_RAIL_UTIL_COEX_REQ_PORT)         \
  || defined(SL_RAIL_UTIL_COEX_PRI_PORT)        \
  || defined(SL_RAIL_UTIL_COEX_PWM_REQ_PORT)    \
  || defined(SL_RAIL_UTIL_COEX_GNT_PORT)        \
  || defined(SL_RAIL_UTIL_COEX_RHO_PORT)        \
  || defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PORT) \
  || defined(SL_RAIL_UTIL_COEX_WIFI_TX_PORT)
#define COEX_CONFIGURED
#endif

#ifdef COEX_CONFIGURED
static COEX_CLI_config_t coex_gpios[] = {
#ifdef SL_RAIL_UTIL_COEX_REQ_PORT
  {
    .name = "Request",
    .config = &sli_coex_ptaReqCfg,
    .handler = &COEX_HAL_ConfigRequest,
    .index = COEX_CLI_GPIO_INDEX_REQ
  },
#endif //SL_RAIL_UTIL_COEX_REQ_PORT
#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .name = "Priority",
    .config = &sli_coex_ptaPriCfg,
    .handler = &COEX_HAL_ConfigPriority,
    .index = COEX_CLI_GPIO_INDEX_PRI
  },
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .name = "PWM Request",
    .config = &sli_coex_ptaPwmReqCfg,
    .handler = &COEX_HAL_ConfigPwmRequest,
    .index = COEX_CLI_GPIO_INDEX_PWM_REQ
  },
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#ifdef SL_RAIL_UTIL_COEX_GNT_PORT
  {
    .name = "Grant",
    .config = &sli_coex_ptaGntCfg,
    .handler = &COEX_HAL_ConfigGrant,
    .index = COEX_CLI_GPIO_INDEX_GNT
  },
#endif //SL_RAIL_UTIL_COEX_GNT_PORT
#ifdef SL_RAIL_UTIL_COEX_RHO_PORT
  {
    .name = "Radio Hold Off",
    .config = &sli_coex_rhoCfg,
    .handler = &COEX_HAL_ConfigRadioHoldOff,
    .index = COEX_CLI_GPIO_INDEX_RHO
  },
#endif //SL_RAIL_UTIL_COEX_RHO_PORT
#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
  {
    .name = "PHY Select",
    .config = &sli_coex_phySelectCfg,
    .handler = &COEX_HAL_ConfigPhySelect,
    .index = COEX_CLI_GPIO_INDEX_PHY_SELECT
  },
#endif //SL_RAIL_UTIL_COEX_PHY_ENABLE_PORT
#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
  {
    .name = "WiFi TX",
    .config = &sli_coex_wifiTxCfg,
    .handler = &COEX_HAL_ConfigWifiTx,
    .index = COEX_CLI_GPIO_INDEX_WIFI_TX
  },
#endif //SL_RAIL_UTIL_COEX_WIFI_TX_PORT
};
#endif //COEX_CONFIGURED

static bool coexInitialized = false;

static uint16_t fastRandom(void)
{
  return COEX_HAL_GetPseudoRandom(0, 0xFFFF);
}

static void abortTxCallback(void)
{
  (void)RAIL_StopTx(railHandle, RAIL_STOP_MODE_ACTIVE);
}

void sl_bt_class_coex_init(void)
{
}

static void initCoexTest(void)
{
  if (coexInitialized) {
    return;
  }
  if (RAIL_IEEE802154_IsEnabled(railHandle)) {
    sl_rail_util_coex_init();
    sl_rail_util_coex_set_enable(true);
    sl_rail_util_ieee802154_on_event(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK, 0U);
  }
  if (RAIL_BLE_IsEnabled(railHandle)) {
    sl_bt_ll_coex_set_context(railHandle,
                              &abortTxCallback,
                              &fastRandom);

    sl_bt_init_coex_hal();
  }
  coexInitialized = true;
}

static COEX_CLI_config_t *get_gpio_config(uint8_t index)
{
  uint8_t i;
  #ifdef COEX_CONFIGURED
  for (i = 0U; i < NUMOF_ARRAY_ELEMENTS(coex_gpios); ++i) {
    if (coex_gpios[i].index == index) {
      return &coex_gpios[i];
    }
  }
  #endif //COEX_CONFIGURED
  return NULL;
}

void cli_coex_get_gpio_config(sl_cli_command_arg_t *args)
{
#ifdef COEX_CONFIGURED
  uint8_t i;
  responsePrintStart(sl_cli_get_command_string(args, 0));
  for (i = 0U;
       i < (NUMOF_ARRAY_ELEMENTS(coex_gpios) - 1);
       ++i) {
    responsePrintContinue(
      "Coexistence %s port:%d,"
      "pin:%d,"
      "polarity:%d,",
      coex_gpios[i].name,
      coex_gpios[i].config->port,
      coex_gpios[i].config->pin,
      coex_gpios[i].config->polarity);
  }
  responsePrintEnd(
    "Coexistence %s port:%d,"
    "pin:%d,"
    "polarity:%d,",
    coex_gpios[i].name,
    coex_gpios[i].config->port,
    coex_gpios[i].config->pin,
    coex_gpios[i].config->polarity);
#else
  responsePrint(sl_cli_get_command_string(args, 0),
                "No coexistence GPIOs configured."
                "Status:Unsupported");
#endif //COEX_CONFIGURED
}

void cli_coex_set_gpio_config(sl_cli_command_arg_t *args)
{
  initCoexTest();
  COEX_CLI_config_t *gpioConfig = get_gpio_config(sl_cli_get_argument_uint8(args, 0));
  if (gpioConfig == NULL) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "Unsupported coexistence GPIO index:%d,"
                  "Status:Unsupported",
                  sl_cli_get_argument_uint8(args, 0));
  }
  COEX_HAL_GpioConfig_t *halConfig = gpioConfig->config;
  halConfig->port = sl_cli_get_argument_uint8(args, 1);
  halConfig->pin = sl_cli_get_argument_uint8(args, 2);
  halConfig->polarity = !!sl_cli_get_argument_uint8(args, 3);
  responsePrint(sl_cli_get_command_string(args, 0),
                "Coexistence %s port:%d,"
                "pin:%d,"
                "polarity:%d,"
                "Status:%s",
                gpioConfig->name,
                halConfig->port,
                halConfig->pin,
                halConfig->polarity,
                gpioConfig->handler(halConfig) ? "Success" : "Fail");
}

void cli_coex_set_software_request(sl_cli_command_arg_t *args)
{
  initCoexTest();
  COEX_Req_t coexReq = (COEX_Req_t)sl_cli_get_argument_uint8(args, 0);
  responsePrint(sl_cli_get_command_string(args, 0),
                "Coexistence SW Request mode:%d,"
                "Status:Success",
                coexReq);
}
