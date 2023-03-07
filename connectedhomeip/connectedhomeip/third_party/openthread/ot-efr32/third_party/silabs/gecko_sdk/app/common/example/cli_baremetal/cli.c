/***************************************************************************//**
 * @file
 * @brief cli bare metal examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>
#include <stdio.h>
#include "cli.h"
#include "sl_cli.h"
#include "sl_cli_instances.h"
#include "sl_cli_arguments.h"
#include "sl_cli_handles.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

void echo_str(sl_cli_command_arg_t *arguments);
void echo_int(sl_cli_command_arg_t *arguments);
void led_cmd(sl_cli_command_arg_t *arguments);

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static const sl_cli_command_info_t cmd__echostr = \
  SL_CLI_COMMAND(echo_str,
                 "echoes string arguments to the output",
                 "Just a string...",
                 { SL_CLI_ARG_WILDCARD, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd__echoint = \
  SL_CLI_COMMAND(echo_int,
                 "echoes integer arguments to the output",
                 "Just a number...",
                 { SL_CLI_ARG_INT8, SL_CLI_ARG_ADDITIONAL, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd__led = \
  SL_CLI_COMMAND(led_cmd,
                 "Change an led status",
                 "led number: 0 or 1"SL_CLI_UNIT_SEPARATOR "instruction: on, off, or toggle",
                 { SL_CLI_ARG_UINT8, SL_CLI_ARG_WILDCARD, SL_CLI_ARG_END, });

static sl_cli_command_entry_t a_table[] = {
  { "echo_str", &cmd__echostr, false },
  { "echo_int", &cmd__echoint, false },
  { "led", &cmd__led, false },
  { NULL, NULL, false },
};

static sl_cli_command_group_t a_group = {
  { NULL },
  false,
  a_table
};

/*******************************************************************************
 *************************  EXPORTED VARIABLES   *******************************
 ******************************************************************************/

sl_cli_command_group_t *command_group = &a_group;

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Callback for echo_str
 *
 * This function is used as a callback when the echo_str command is called
 * in the cli. It simply echoes back all the arguments provided as strings.
 ******************************************************************************/
void echo_str(sl_cli_command_arg_t *arguments)
{
  char *ptr_string;

  printf("<<echo_str command>>\r\n");

  // Read all the arguments provided as strings and print them back
  for (int i = 0; i < sl_cli_get_argument_count(arguments); i++) {
    ptr_string = sl_cli_get_argument_string(arguments, i);

    printf("%s\r\n", ptr_string);
  }
}

/***************************************************************************//**
 * Callback for echo_int
 *
 * This function is used as a callback when the echo_int command is called
 * in the cli. It simply echoes back all the arguments provided as integers.
 ******************************************************************************/
void echo_int(sl_cli_command_arg_t *arguments)
{
  int8_t argument_value;

  printf("<<echo_int command>>\r\n");

  // Read all the arguments provided as integers and print them back
  for (int i = 0; i < sl_cli_get_argument_count(arguments); i++) {
    argument_value = sl_cli_get_argument_int8(arguments, i);

    printf("%i\r\n", argument_value);
  }
}

/***************************************************************************//**
 * Callback for the led
 *
 * This function is used as a callback when the led command is called
 * in the cli. The command is used to turn on, turn off and toggle leds.
 ******************************************************************************/
void led_cmd(sl_cli_command_arg_t *arguments)
{
  uint8_t led_number;
  char *instruction;
  sl_led_t led;

  // Make sure that both the arguments were provided
  if (sl_cli_get_argument_count(arguments) < 2) {
    printf("Please provide inputs.\r\n");
    return;
  }

  // Read the provided led number and verify it is valid
  led_number = sl_cli_get_argument_uint8(arguments, 0);
  if (led_number > 1) {
    printf("Invalid led. Only led 0 and 1 present.\r\n");
    return;
  }

  // Get an handle to the appropriate led
  led = led_number == 0 ? sl_led_led0 : sl_led_led1;

  // Get the instruction provided
  instruction = sl_cli_get_argument_string(arguments, 1);

  if (strcmp(instruction, "on") == 0) {
    // led on instruction provided
    sl_led_turn_on(&led);
  } else if (strcmp(instruction, "off") == 0) {
    // led off instruction provided
    sl_led_turn_off(&led);
  } else if (strcmp(instruction, "toggle") == 0) {
    // led toggle instruction provided
    sl_led_toggle(&led);
  } else {
    // led off instruction provided
    printf("Incorrect instruction. Please use on, off or toggle\r\n");
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * Initialize cli example.
 ******************************************************************************/
void cli_app_init(void)
{
  bool status;

  status = sl_cli_command_add_command_group(sl_cli_inst_handle, command_group);
  EFM_ASSERT(status);

  printf("\r\nStarted CLI Bare-metal Example\r\n\r\n");
}

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void cli_app_process_action(void)
{
}
