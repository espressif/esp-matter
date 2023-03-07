/***************************************************************************//**
 * @file
 * @brief The source for our simple command line interpreter. This uses the C
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "command_interpreter.h"

// -----------------------------------------------------------------------------
// Configuration Macros
// -----------------------------------------------------------------------------
// Pull in correct config file:
#ifdef COMMAND_INTERPRETER_USE_LOCAL_CONFIG_HEADER
  #include "command_interpreter_config.h" // component-level config file (new method)
#else // !defined(COMMAND_INTERPRETER_USE_LOCAL_CONFIG_HEADER)
  #ifdef CONFIGURATION_HEADER
    #include CONFIGURATION_HEADER // application-level config file (old method)
  #endif

  #ifdef MAX_COMMAND_ARGUMENTS
    #define COMMAND_INTERPRETER_NUM_ARGS_MAX MAX_COMMAND_ARGUMENTS
  #else
    #define COMMAND_INTERPRETER_NUM_ARGS_MAX 20U
  #endif
#endif // defined(COMMAND_INTERPRETER_USE_LOCAL_CONFIG_HEADER)

// Define a WEAK macro to work across different compilers
#ifdef __ICCARM__
  #define WEAK __weak
#elif defined(__GNUC__)
  #define WEAK __attribute__ ((weak))
#else
  #define WEAK
#endif

// Static function prototypes
static bool processCommand(CommandEntry_t *commands, char *buffer);
static bool validateArguments(char *argstring, int argc, char **argv);
static bool validateInteger(char *str, int lengthInBytes, bool isSigned);

#if _WIN32 == 1
char* strtok_r(char *str, const char *delim, char **nextp);
#endif

// Default error callback implementation
WEAK void ciErrorCallback(char* command, CommandError_t error)
{
  if (error == CI_UNKNOWN_COMMAND) {
    printf("Error: Unknown command '%s'\n", command);
  } else if (error == CI_MAX_ARGUMENTS) {
    printf("Error: Cannot specify more than %d arguments\n",
           COMMAND_INTERPRETER_NUM_ARGS_MAX);
  } else if (error == CI_INVALID_ARGUMENTS) {
    printf("Error: Unexpected or invalid arguments for this command\n");
  }
}

// Default help print implementation
WEAK bool ciPrintHelp(CommandEntry_t *commands)
{
  int column0Width = 0;
  int column1Width = 0;
  CommandEntry_t *head = commands;

  // Find the max width of each column
  while (commands != NULL && commands->command != NULL) {
    int commandLen = strlen(commands->command);
    int argstrLen;

    if (commands->arguments != NULL) {
      argstrLen = strlen(commands->arguments);
    } else {
      argstrLen = 0;
    }

    if(commands->callback != NULL){
      column0Width = (column0Width > commandLen) ? column0Width : commandLen;
      column1Width = (column1Width > argstrLen) ? column1Width : argstrLen;
    }
    commands++;
  }

  // Print out some information about the help
  printf("<command> <args> <help text>\n");
  printf("  u=uint8, v=uint16, w=uint32, s=int32, b=string, ?=Anything, *=0 or more of previous\n");

  // Walk back through the list and print now that we know the widths
  commands = head;
  while (commands != NULL && commands->command != NULL) {
    char *arguments = "";

    if (commands->callback != NULL) {
      printf("%*s", -column0Width, commands->command);
      if ((commands->arguments != NULL) && (strlen(commands->arguments) != 0)) {
        arguments = commands->arguments;
      }
      printf(" %*s", -column1Width, arguments);

      if ((commands->helpStr != NULL) && (strlen(commands->helpStr) != 0)) {
        printf(" %s", commands->helpStr);
      }
    } else {
      // Treat this as a command separator
      printf(" --- %s ---", commands->command);
    }
    printf("\n");
    commands++;
  }
  return true;
}

uint8_t ciInitState(CommandState_t *state,
                    char *buffer,
                    uint32_t length,
                    CommandEntry_t *commands)
{
  // Make sure that things aren't NULL
  if (state == NULL || buffer == NULL) {
    return 1;
  }

  state->buffer = buffer;
  state->offset = 0;
  state->length = length;
  state->commands = commands;
  state->prevEol = '\0';

  return 0;
}

uint8_t ciResetState(CommandState_t *state)
{
  if (state == NULL) {
    return 1;
  }

  state->offset = 0;
  state->prevEol = '\0';
  return 0;
}

int8_t ciProcessInput(CommandState_t *state,
                      char *data,
                      uint32_t length)
{
  uint32_t i, start;
  int8_t rval = 0;

  // Make sure that state and data aren't NULL
  if (state == NULL || data == NULL) {
    return -1;
  }

  // Make sure the new data can fit into our buffer
  if ((length + state->offset) >= state->length) {
    return -2;
  }

  // Record the current offset for processing below
  i = state->offset;

  // Copy the new data into our buffer
  memcpy(state->buffer + state->offset, data, length);
  state->offset += length;

  // Walk the new section of data for a new line which indicates the
  // end of a command
  start = 0;
  for (; i < state->offset; i++) {
    // If we've found an end of line then process this command
    if (state->buffer[i] == '\n' || state->buffer[i] == '\r') {
      char eolChar = state->buffer[i];

      // Null terminate the string for processing
      state->buffer[i] = '\0';

      // Only process strings that have real content in them
      if ((i - start) > 0) {
        processCommand(state->commands, state->buffer + start);
      }

      // Do not count '\r\n' as two commands
      if (!(eolChar == '\n' && state->prevEol == '\r') || (i - start) > 0) {
        rval += 1;
      }

      // Move the data pointer up and store this EOL character
      start = i + 1;
      state->prevEol = eolChar;
    }
  }

  // If we processed some commands then shift the memory buffer
  if (start != 0) {
    state->offset -= start;
    memmove(state->buffer, state->buffer + start, state->offset);
  }

  return rval;
}

uint32_t ciGetUnsigned(char *arg)
{
  return strtoul(arg, NULL, 0);
}

int32_t ciGetSigned(char *arg)
{
  return strtol(arg, NULL, 0);
}

bool ciValidateInteger(char *arg, char type)
{
  bool isSigned = false;
  int length;

  switch (type) {
    case 'w':
      length = 4;
      break;
    case 'v':
      length = 2;
      break;
    case 'u':
      length = 1;
      break;
    case 's':
      length = 4;
      isSigned = true;
      break;
    default:
      return false;
  }

  return validateInteger(arg, length, isSigned);
}

static void parseBackspaces(char *buffer)
{
  char *wr = buffer;
  char *rd = buffer;
  for (; *rd; rd++) {
    if (*rd == '\b') {
      if (wr > buffer) {
        wr--;
      }
    } else {
      *wr++ = *rd;
    }
  }
  *wr = '\0';
}

static bool processCommand(CommandEntry_t *commands, char *buffer)
{
  bool success = false;
  char *saveptr, *token;
  CommandEntry_t *commandListStart = commands;

  // Deal with backspace corrections
  parseBackspaces(buffer);

  // Get the first token in the input buffer to find the command name
  token = strtok_r(buffer, "\n\r ", &saveptr);

  // Treat the empty command as a valid command
  if (token == NULL) {
    return true;
  }

  // Iterate through all the known commands and see if any of them match the
  // text we've received
  while (commands != NULL && commands->command != NULL) {
    uint32_t argc = 0;
    char *argv[COMMAND_INTERPRETER_NUM_ARGS_MAX];

    // See if any of the commands match the value entered
    if ((commands->callback != NULL)
        && (strcasecmp(token, commands->command) == 0)) {
      // Store the command name as the first argument
      argv[argc] = commands->command;
      argc++;
      // Create a list of tokens from the arguments
      token = strtok_r(NULL, "\n\r ", &saveptr);
      while (token != NULL) {
        argv[argc] = token;
        argc++;
        if (argc >= COMMAND_INTERPRETER_NUM_ARGS_MAX) {
          ciErrorCallback(buffer, CI_MAX_ARGUMENTS);
          return false;
        }
        token = strtok_r(NULL, "\n\r ", &saveptr);
      }

      // Make sure that the arguments we received match what was specified but
      // skip over the first argument (command name)
      if (!validateArguments(commands->arguments, argc - 1, argv + 1)) {
        ciErrorCallback(buffer, CI_INVALID_ARGUMENTS);
        return false;
      }

      // Call the appropriate callback with its arguments
      commands->callback(argc, argv);
      success = true;
      break;
    }
    commands++;
  }

  // No matching command found!
  if (commands == NULL || commands->command == NULL) {
    // The help command is implemented internally so make sure it's not that
    if ((commandListStart != NULL) && (strcasecmp(buffer, "help") == 0)) {
      success = ciPrintHelp(commandListStart);
    }
  }

  // If we fell all the way down to here then we can't handle this command
  // so indicate the issue
  if (!success && (token != NULL)) {
    ciErrorCallback(token, CI_UNKNOWN_COMMAND);
  }

  return success;
}

/**
 * Validate the input command arguments. This is pretty rudimentary for now,
 * but it will make sure that integers are given were specified. It also
 * attempts to parse argstring is a method that is compatible with Ember
 * command-interpreter-2.
 *   u: Unsigned 1 byte
 *   v: Unsigned 2 byte
 *   w: Unsigned 4 byte
 *   s: Signed (any length)
 *   b: String
 *   ?: Anything
 *   *: Zero or more of the previous argument. Must be the last thing.
 */
static bool validateArguments(char *argstring, int argc, char **argv)
{
  int argstringLen, minArgs;
  char argType = '.';  // Set this to something invalid to start
  char *loc;

  // If there is no argstring or only a '?' then treat any arguments
  // as valid
  if (argstring == NULL || (strcmp(argstring, "?") == 0)) {
    return true;
  }
  argstringLen = strlen(argstring);
  minArgs = argstringLen;

  // Validate the position of '*' characters
  loc = strstr(argstring, "*");
  if (loc != NULL) {
    // Ensure the '*' is at the end of the string and that the string has more
    // than just the '*' character
    if ((loc != (argstring + argstringLen - 1)) || (argstringLen == 1)) {
      return false;
    }
    minArgs -= 2; // Get rid of the '*' and its optional predecessor
  }

  // Validate each argument that was passed in
  for (int i = 0; i < argc; i++) {
    bool isValid;

    // Grab the next argument type from the argstring handling the special
    // case of a '*' character at the end.
    if (i >= argstringLen) {
      if (argstring[argstringLen - 1] != '*') {
        return false;
      }
    } else if (argstring[i] != '*') {
      argType = argstring[i];
    }

    // Process this command line argument using the current argument
    // type. We really only validate different types of integers here.
    switch (argType) {
      case 'w':
        isValid = validateInteger(argv[i], 4, false);
        break;
      case 'v':
        isValid = validateInteger(argv[i], 2, false);
        break;
      case 'u':
        isValid = validateInteger(argv[i], 1, false);
        break;
      case 's':
        isValid = validateInteger(argv[i], 4, true);
        break;
      case 'b':
        // Strings are always valid...
        isValid = true;
        break;
      default:
        // Error in the argstring so stop processing
        isValid = false;
        break;
    }
    if (!isValid) {
      return false;
    }
  }

  // As long as we had enough arguments then everything is fine
  return (argc >= minArgs);
}

/*
 * Validate that the given string represents an integer and, if it's unsigned,
 * that the size of that integer fits within lengthInBytes bytes.
 */
static bool validateInteger(char *str, int lengthInBytes, bool isSigned)
{
  char *endptr;

  // For signed values just check that strtol doesn't report an error
  if (isSigned) {  // Signed
    errno = 0;
    strtol(str, &endptr, 0);
  } else {
    // For unsigned values also make sure that we don't use more bytes
    // than were specfied in the lengthInBytes variable
    uint32_t val, mask = (1 << (lengthInBytes * 8)) - 1;

    errno = 0;
    val = strtoul(str, &endptr, 0);

    // Make sure that this value doesn't set higher order bits
    if ((val & mask) != val) {
      return false;
    }
  }

  // If we didn't process the whole string or errno was set then something
  // probably went wrong
  if ((errno != 0) || ((uint32_t)(endptr - str) != strlen(str))) {
    return false;
  }

  return true;
}

#if _WIN32 == 1
// Implement strtok_r() function for MSYS
char* strtok_r(char *str, const char *delim, char **nextp)
{
  char *ret;

  if (str == NULL) {
    str = *nextp;
  }

  str += strspn(str, delim);

  if (*str == '\0') {
    return NULL;
  }

  ret = str;

  str += strcspn(str, delim);

  if (*str) {
    *str++ = '\0';
  }

  *nextp = str;

  return ret;
}
#endif // _WIN32 == 1
