/***************************************************************************//**
 * @file
 * @brief CLI for the Test Harness plugin.
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

// TODO: remove this header once we drop AppBuilder support

#if defined(EMBER_AF_PLUGIN_TEST_HARNESS)

  #define TEST_HARNESS_CLI_COMMANDS \
  emberCommandEntrySubMenu("test-harness", emberAfPluginTestHarnessCommands, "Commands for acting like a test harness."),
#else
  #define TEST_HARNESS_CLI_COMMANDS
#endif
