/***************************************************************************//**
 * @file
 * @brief CLI commands various options.
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

#if !defined(OPTION_CLI_H)
#define OPTION_CLI_H

#ifndef UC_BUILD
extern EmberCommandEntry emAfOptionCommands[];
#endif // UC_BUILD

void emAfCliServiceDiscoveryCallback(const EmberAfServiceDiscoveryResult* result);

#endif
