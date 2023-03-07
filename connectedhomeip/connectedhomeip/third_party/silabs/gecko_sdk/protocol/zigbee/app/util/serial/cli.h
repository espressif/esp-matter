/***************************************************************************//**
 * @file
 * @brief Simple command line interface (CLI) for use with Ember applications.
 * The application defines a list of top-level commands and functions
 * to be called when those commands are used. The application also
 * defines a prompt to use. Then the application can parse arguments
 * passed to the top-level commands using the utility functions provided
 * for comparing strings, getting integer values, and hex values. See below
 * for an example.
 *
 * The cli library is deprecated and will be removed in a future release.
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

// EXAMPLE:
// ------------------------------
// // define the functions called for the top-level cmds: version, reset, print
// void versionCB(void)
//   {  emberSerialPrintf(APP_SERIAL, "version 0.1\r\n"); }
//
// void resetCB(void)
//   { while (1) ; }
//
// // the commands accepted are "print info", "print child", and
// // "print bindings"
// void printCB(void)
// {
//   if (compare("info",4,1) == true) {
//     printInfo();
//   }
//   else if (compare("child",5,1) == true) {
//     printChildTable();
//   }
//   else if (compare("bindings",8,1) == true) {
//     printBindingTableUtil(APP_SERIAL, EMBER_BINDING_TABLE_SIZE);
//   }
// }
//
// // match top-level commands to the functions they will call
// cliSerialCmdEntry cliCmdList[] = {
//   {"version", versionCB},
//   {"reset", resetCB},
//   {"print", printCB}
// };
// uint8_t cliCmdListLen = sizeof(cliCmdList)/sizeof(cliSerialCmdEntry);
//
// // define the prompt
// const char * cliPrompt = "qa-host";
//
// // initialize the cli by setting the port
// cliInit(APP_SERIAL);
//
//  Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include PLATFORM_HEADER //compiler/micro specifics, types

// The CONFIGURATION_HEADER is included to allow the user to change the
// maximim number of arguments accepted and/or the maximim size of cmds.
// This is done by setting the defines: CLI_MAX_NUM_SERIAL_ARGS and/or
// CLI_MAX_SERIAL_CMD_LINE
#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif

// *********************************************
// apps must initialize cli with a serial port before using it
void cliInit(uint8_t serialPort);

// apps should call this from their main loop
// this processes characters from the serial port
void cliProcessSerialInput(void);

// *********************************************
// Constants for the size of the largest argument and the number of
// arguments accepted. These can be defined by the application in the
// CONFIGURATION_HEADER to fit the applications needs. Decreasing
// these reduces RAM usage. Increasing these increases RAM usage.
#ifndef CLI_MAX_SERIAL_CMD_LINE
  #define CLI_MAX_SERIAL_CMD_LINE  17
#endif
#ifndef CLI_MAX_NUM_SERIAL_ARGS
  #define CLI_MAX_NUM_SERIAL_ARGS  8
#endif

// *********************************************
// these are utility functions to help the application parse the cmd line

// this returns the nth byte from an argument entered as a hex string
// as a byte value
uint8_t cliGetHexByteFromArgument(uint8_t whichByte,
                                  uint8_t argument);

// this returns an int16_t from an argument entered as a string in decimal
int16_t cliGetInt16sFromArgument(uint8_t bufferIndex);

// this returns an uint16_t from an argument entered as a string in decimal
uint16_t cliGetInt16uFromArgument(uint8_t argument);

// this returns an uint32_t from an argument entered as a string in decimal
uint32_t cliGetInt32uFromArgument(uint8_t argument);

// returns an uint16_t from an argument entered as a hex string
uint16_t cliGetInt16uFromHexArgument(uint8_t index);

// returns an uint32_t from an argument entered as a hex string
uint32_t cliGetInt32uFromHexArgument(uint8_t index);

// this returns true if the argument specified matches the keyword provided
bool cliCompareStringToArgument(const char * keyword,
                                uint8_t argument);

// This copies the string at the argument specified into the bufferToFill.
// The bufferToFill must already point to initialized memory and the maximum
// length of this buffer should be maxBufferToFillLength. If
// maxBufferToFillLength is smaller than the string being copied into
// bufferToFill, no copy will be done and a length of zero will be returned.
// If maxBufferToFillLength is larger than the string being copied in,
// the copy will be done and the length of the copied string will be returned.
uint8_t cliGetStringFromArgument(uint8_t argument,
                                 uint8_t* bufferToFill,
                                 uint8_t maxBufferToFillLength);

// *********************************************
// This is the structure that the application uses to enter
// commands into the command parser. This should not be changed.
typedef const struct cliSerialCmdEntryS {
  const char * cmd;
  void (*action)(void);
} cliSerialCmdEntry;

// *********************************************
// the application needs to define these:
// --------------------------------------
extern cliSerialCmdEntry cliCmdList[];
extern uint8_t cliCmdListLen;
extern const char * cliPrompt;
