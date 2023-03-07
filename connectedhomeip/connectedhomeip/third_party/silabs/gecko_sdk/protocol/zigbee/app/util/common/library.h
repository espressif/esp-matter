/***************************************************************************//**
 * @file
 * @brief Code to display or retrieve the presence or absence of
 * Ember stack libraries on the device.
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

#ifndef SILABS_APP_UTIL_LIBRARY_H
#define SILABS_APP_UTIL_LIBRARY_H

void printAllLibraryStatus(SL_CLI_COMMAND_ARG);
bool isLibraryPresent(uint8_t libraryId);

#define LIBRARY_COMMANDS                                     \
  emberCommandEntryAction("libs", printAllLibraryStatus, "", \
                          "Prints the status of all Ember stack libraries"),

#endif // SILABS_APP_UTIL_LIBRARY_H
