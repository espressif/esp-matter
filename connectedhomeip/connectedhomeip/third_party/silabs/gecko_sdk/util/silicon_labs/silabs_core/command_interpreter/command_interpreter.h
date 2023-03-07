/***************************************************************************//**
 * @file
 * @brief The header for our simple command line interpreter. This uses the C
 *        standard library to do most of the parsing, but should be good enough
 *        to get up and running with.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
/***************************************************************************//**
 *  To use the command interpreter you must create a state variable and buffer
 *  in your appliation space and then initialize them along with the list of
 *  supported commands using ciInitState(). You then call ciProcessInput() with
 *  each new bit of data you get and this function will process commands as they
 *  are found in the input stream. The CommandEntry_t arguments string will be
 *  used to do simple validation of arguments which is described below.
 *    Arguments:
 *      u: Unsigned 1 byte
 *      v: Unsigned 2 byte
 *      w: Unsigned 4 byte
 *      s: Signed (any length)
 *      b: String
 *      ?: Anything
 *      *: Zero or more of the previous argument. Must be the last entry.
 *
 * Example:
 * @code
 *   CommandState_t state;
 *   char buffer[256];
 *   CommandEntry_t commands[] = {
 *    COMMAND_ENTRY("test", "uwb", testFunction),
 *    COMMAND_ENTRY(NULL, NULL, NULL)
 *   };
 *
 *   ciInitState(&state, ciBuffer, sizeof(ciBuffer), commands);
 *
 *   while(1)
 *   {
 *     char input = getchar();
 *     if(input != '\0' && input != 0xFF) {
 *       if(ciProcessInput(&state, &input, 1) > 0) {
 *         printf("> ");
 *       }
 *     }
 *   }
 *
 ******************************************************************************/

#ifndef COMMAND_INTERPRETER_SIMPLE_H__
#define COMMAND_INTERPRETER_SIMPLE_H__

// Get the standard include types
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Macros
// -----------------------------------------------------------------------------
#define COMMAND_ENTRY(command, args, callback, helpStr) \
  { command, args, callback, helpStr }
#define COMMAND_SEPARATOR(string) \
  { string, NULL, NULL, NULL }

// -----------------------------------------------------------------------------
// Structures and Types
// -----------------------------------------------------------------------------
// CommandError_t: Different error types passed to the ciErrorCallback()
typedef enum {CI_UNKNOWN_COMMAND,
              CI_MAX_ARGUMENTS,
              CI_INVALID_ARGUMENTS} CommandError_t;

// CommandEntry_t: An entry in the command table
typedef struct CommandEntry {
  char* command;
  char* arguments;
  void (*callback)(int argc, char **argv);
  char* helpStr;
} CommandEntry_t;

// CommandState_t: The state held in a user application which must be passed
// to the command interpreter.
typedef struct CommandState {
  char *buffer;             // The buffer to hold incoming data
  char prevEol;             // The last end of line character processed
  uint32_t offset;          // Current offset in the receive buffer
  uint32_t length;          // The length of the input buffer
  CommandEntry_t *commands; // List of known commands (null terminated)
} CommandState_t;

// -----------------------------------------------------------------------------
// Command Interpreter Functions
// -----------------------------------------------------------------------------
/*
 * Initialize the CommandState_t variable for this command interpreter.
 * @param state A pointer to the state variable to fill in
 * @param buffer A pointer to the buffer to be used for storing a line of data.
 *   This is chosen by the user, but should be large enough to hold the maximum
 *   line of data.
 * @param bufferLength The length of the buffer specified above
 * @param commands A list of the commands supported by this interpreter
 * @return Returns 0 on success or some other value on failure
 */
uint8_t ciInitState(CommandState_t *state,
                    char *buffer,
                    uint32_t bufferLength,
                    CommandEntry_t *commands);

/*
 * Reset the state variable specified to drop all previously recorded input and
 * start fresh.
 * @param state The state variable to reinitialize
 * @return Returns 0 on success or some other value on failure
 */
uint8_t ciResetState(CommandState_t *state);

/*
 * Add the input text to our command buffer and attempt to process a command
 * out of what's in there.
 * @param state A pointer to the state variable for this command interpreter
 * @param data A pointer to the new character data
 * @param dataLength The length of the data array
 * @return Returns the number of commands processed on success or a number less
 * than 0 on failure.
 */
int8_t ciProcessInput(CommandState_t *state,
                      char *data,
                      uint32_t dataLength);

/*
 * Helper function to get an unsigned integer from a given command argument.
 * @param arg The string to convert to an unsigned int
 * @return The unsigned integer value.
 */
uint32_t ciGetUnsigned(char *arg);

/*
 * Helper function to get a signed integer from a given command argument.
 * @param arg The string to convert to a signed int
 * @return The signed integer value.
 */
int32_t ciGetSigned(char *arg);

/**
 * Helper function to validate whether a command argument is of the expected
 * type. This will be handled by the command interpreter unless you specify a
 * '?' in the processing string or want to use a 'b' string for more
 * flexibility.
 * @param arg The argument string to validate.
 * @param type The argument type to validate against from. These are the same as
 * the processing strings.
 * - u: Unsigned 1 byte
 * - v: Unsigned 2 byte
 * - w: Unsigned 4 byte
 * - s: Signed (any length)
 * @return Returns true if the argument matches the type and false otherwise
 */
bool ciValidateInteger(char *arg, char type);

// -----------------------------------------------------------------------------
// Command Interpreter Callbacks
// -----------------------------------------------------------------------------
/*
 * This callback is called by the command interpreter when there is an error
 * during processing command input. The string that caused the error as well
 * as an error indicator are passed as arguments. There is a default weak
 * implementation of this included with the command interpreter. If you need
 * custom handling implement your own version.
 *
 * @param command The command name that generated the error
 * @param error The error that occurred
 */
void ciErrorCallback(char* command, CommandError_t error);

/*
 * This callback is called by the command interpreter when it receives the
 * 'help' command. It is passed the list of commands to be printed to the user.
 * There is a default implementation that will print all command names and
 * arguments which can be overriden by your own version if desired. The return
 * value indicates whether this command was handled or not.
 *
 * @param commands The list of commands understood by this command interpreter
 * @return true if the 'help' command was handled and false otherwise.
 */
bool ciPrintHelp(CommandEntry_t *commands);

#endif // COMMAND_INTERPRETER_SIMPLE_H__
