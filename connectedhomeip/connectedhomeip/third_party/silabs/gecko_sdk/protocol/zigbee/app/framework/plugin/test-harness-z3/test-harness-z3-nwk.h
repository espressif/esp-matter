/***************************************************************************//**
 * @file
 * @brief ZigBee 3.0 nwk test harness functionality
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

// -----------------------------------------------------------------------------
// Constants

#define NWK_LEAVE_COMMAND           (0x04)
#define NWK_REJOIN_REQUEST_COMMAND  (0x06)
#define NWK_REJOIN_RESPONSE_COMMAND (0x07)

// -----------------------------------------------------------------------------
// Globals

extern bool emAfPluginTestHarnessZ3IgnoreLeaveCommands;
