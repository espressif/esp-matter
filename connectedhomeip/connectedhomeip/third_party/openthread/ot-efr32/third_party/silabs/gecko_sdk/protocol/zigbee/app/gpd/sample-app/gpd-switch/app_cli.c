/***************************************************************************//**
 * @file app_cli.c
 * @brief app_cli.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#if SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include "app_process.h"

#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"

/******************************************************************************
 * CLI command to send a toggle
 *****************************************************************************/
void emberGpdAfCliSwitchToggle(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  emberGpdSwitchToggleEvent();
}

/******************************************************************************
 * Sets the sleepy flag that will allow the module sleep untill a button
 * is pressed.
 *****************************************************************************/
void emberGpdAfCliSwitchSleep(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  emberGpdSwitchSetSleep();
}

#endif // SL_CATALOG_CLI_PRESENT
