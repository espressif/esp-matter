/***************************************************************************//**
 * @file
 * @brief Processes commands coming from the serial port.
 * See @ref commands2 for documentation.
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

#ifndef SILABS_COMMAND_INTERPRETER2_H
#define SILABS_COMMAND_INTERPRETER2_H

/** @addtogroup commands2
 * Interpret serial port commands. See command-interpreter2.c for source code.
 *
 * See the following application usage example followed by a brief explanation.
 * @code
 *
 * &frasl;&frasl; Usage: network form 22 0xAB12 -3 { 00 01 02 A3 A4 A5 A6 A7 }
 * void formCommand(void)
 * {
 *   uint8_t channel = emberUnsignedCommandArgument(0);
 *   uint16_t panId  = emberUnsignedCommandArgument(1);
 *   int8_t power   = emberSignedCommandArgument(2);
 *   uint8_t length;
 *   uint8_t *eui64  = emberStringCommandArgument(3, &length);
 *   ...
 *   ... call emberFormNetwork() etc
 *   ...
 * }
 *
 * &frasl;&frasl; The main command table.
 * EmberCommandEntry emberCommandTable[] = {
 *   emberCommandEntrySubMenu("network",  networkCommands, "Network form/join commands"),
 *   emberCommandEntryAction("status",    statusCommand,   "Prints application status),
 *   ...
 *   emberCommandEntryTerminator()
   };
 *
 * &frasl;&frasl; The table of network commands.
 * EmberCommandEntry networkCommands[] = {
 *   emberCommandEntryAction("form", formCommand, "uvsh", "Form a network"),
 *   emberCommandEntryAction("join", joinCommand, "uvsh", "Join a network"),
 *   ...
 *   emberCommandEntryTerminator()
   };
 *
 * void main(void)
 * {
 *    emberCommandReaderInit();
 *    while(0) {
 *      ...
 *      &frasl;&frasl; Process input and print prompt if it returns true.
 *      if (emberProcessCommandInput(serialPort)) {
 *         emberSerialPrintf(1, "%p>", PROMPT);
 *      }
 *      ...
 *    }
 * }
 * @endcode
 *
 * -# Applications specify the commands that can be interpreted
 *    by defining the emberCommandTable array of type ::EmberCommandEntry.
 *    The table includes the following information for each command:
 *   -# The full command name.
 *   -# Your application's function name that implements the command.
 *   -# An ::EmberCommandEntry::argumentTypes string specifies the number and types of arguments
 *      the command accepts.  See ::argumentTypes for details.
 *   -# A description string explains the command.
 *   .
 * -# A default error handler ::emberCommandErrorHandler() is provided to
 *    deal with incorrect command input. Applications may override it.
 * -# The application calls ::emberCommandReaderInit() to initalize, and
 *    ::emberProcessCommandInput() in its main loop.
 * -# Within the application's command functions, use emberXXXCommandArgument()
 *    functions to retrieve command arguments.
 *
 * The command interpreter does extensive processing and validation of the
 * command input before calling the function that implements the command.
 * It checks that the number, type, syntax, and range of all arguments
 * are correct.  It performs any conversions necessary (for example,
 * converting integers and strings input in hexadecimal notation into
 * the corresponding bytes), so that no additional parsing is necessary
 * within command functions.  If there is an error in the command input,
 * ::emberCommandErrorHandler() is called rather than a command function.
 *
 * The command interpreter allows inexact matches of command names.  The
 * input command may be either shorter or longer than the actual command.
 * However, if more than one inexact match is found and there is no exact
 * match, an error of type EMBER_CMD_ERR_NO_SUCH_COMMAND will be generated.
 * To disable this feature, define EMBER_REQUIRE_EXACT_COMMAND_NAME in the
 * application configuration header.
 *
 *@{
 */

/** @name Command Table Settings
 *@{
 */
#ifndef EMBER_MAX_COMMAND_ARGUMENTS
/** The maximum number of arguments a command can have.  A nested command
 * counts as an argument.
 */
#define EMBER_MAX_COMMAND_ARGUMENTS 16
#endif

#ifndef EMBER_COMMAND_BUFFER_LENGTH
#define EMBER_COMMAND_BUFFER_LENGTH 100
#endif

/** Whether or not the command entry structure will include descriptions for
 *  the commands.  This consumes additional CONST space.
 *  By default descriptions are not included.
 */
#if defined(DOXYGEN_SHOULD_SKIP_THIS)
#define EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD
#endif

/** @} END name group
 */

// The (+ 1) takes into account the leading command.
#define MAX_TOKEN_COUNT (EMBER_MAX_COMMAND_ARGUMENTS + 1)

typedef void (*CommandAction)(void);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**@brief Command entry for a command table.
 */
typedef struct {
#else
typedef const struct {
#endif
  /** Use letters, digits, and underscores, '_', for the command name.
   * Command names are case-sensitive.
   */
  const char * name;
  /** A reference to a function in the application that implements the
   *  command.
   *  If this entry refers to a nested command, then action field
   *  has to be set to NULL.
   */
  CommandAction action;
  /**
   * In case of normal (non-nested) commands, argumentTypes is a
   * string that specifies the number and types of arguments the
   *  command accepts.  The argument specifiers are:
   *  - u:   one-byte unsigned integer.
   *  - v:   two-byte unsigned integer
   *  - w:   four-byte unsigned integer
   *  - s:   one-byte signed integer
   *  - r:   two-byte signed integer
   *  - q:   four-byte signed integer
   *  - b:   string.  The argument can be entered in ascii by using
   *         quotes, for example: "foo".  Or it may be entered
   *         in hex by using curly braces, for example: { 08 A1 f2 }.
   *         There must be an even number of hex digits, and spaces
   *         are ignored.
   *  - *:   zero or more of the previous type.
   *         If used, this must be the last specifier.
   *  - ?:   Unknown number of arguments. If used this must be the only
   *         character. This means, that command interpreter will not
   *         perform any validation of arguments, and will call the
   *         action directly, trusting it that it will handle with
   *         whatever arguments are passed in.
   *  - !:   Optional argument delimiter.
   *         If the command input has sufficient arguments such that
   *         the parser ends on a ! symbol, then the command will be
   *         considered valid, even if it does not necessarily process
   *         the entire argumentTypes string.
   *         If there are additional arguments beyond a !, but
   *         still fewer arguments than the next either the next ! or the
   *         end of the string, then the command will be considered invalid.
   *         Note that this leaves the called CommandAction function to
   *         actually validate what number of optional arguments it
   *         actually got.
   *         Example: Given a argumentTypes String: uu!vv!u!vv
   *         The following inputs type orders are valid:
   *         uu, uuvv, uuvvu, uuvvuvv
   *         The following are invalid:
   *         u, uuv, uuvvuv, uuvvuvvv
   *
   *  Integer arguments can be either decimal or hexidecimal.
   *  A 0x prefix indicates a hexidecimal integer.  Example: 0x3ed.
   *
   *  In case of a nested command (action is NULL), then this field
   *  contains a pointer to the nested EmberCommandEntry array.
   */
  const char * argumentTypes;
  /** A description of the command.
   */

#if defined(EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD)
  const char * description;

  /** An array of strings, with a NULL terminator, indicating what
      each argument is. */
  const char * const * argumentDescriptions;
#endif
} EmberCommandEntry;

#if defined(EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD)
/* @brief Macro to define a CLI action */
  #define emberCommandEntryAction(name, action, argumentTypes, description) \
  { (name), (action), (argumentTypes), (description), NULL }

  #define emberCommandEntryActionWithDetails(name,                     \
                                             action,                   \
                                             argumentTypes,            \
                                             description,              \
                                             argumentDescriptionArray) \
  { (name), (action), (argumentTypes), (description), (argumentDescriptionArray) }

/* @brief Macro to define a CLI sub-menu (nested command) */
  #define emberCommandEntrySubMenu(name, subMenu, description) \
  { (name), NULL, (const char *)(subMenu), (description), NULL }

/* @briefy Macro to define a command entry array terminator.*/
  #define emberCommandEntryTerminator() \
  { NULL, NULL, NULL, NULL, NULL }

#else  // Don't include description data in struct

/* @brief Macro to define a CLI action */
  #define emberCommandEntryAction(name, action, argumentTypes, description) \
  { (name), (action), (argumentTypes) }

  #define emberCommandEntryActionWithDetails(name,                     \
                                             action,                   \
                                             argumentTypes,            \
                                             description,              \
                                             argumentDescriptionArray) \
  { (name), (action), (argumentTypes) }

/* @brief Macro to define a CLI sub-menu (nested command) */
  #define emberCommandEntrySubMenu(name, subMenu, description) \
  { (name), NULL, (const char *)(subMenu) }

/* @briefy Macro to define a command entry array terminator.*/
  #define emberCommandEntryTerminator() \
  { NULL, NULL, NULL }

#endif

/**
 * A pointer to the currently matching command entry.
 * Only valid from within a command function.
 * If the original command was nested, points to the
 * final (non-nested) command entry.
 */
extern EmberCommandEntry *emberCurrentCommand;

extern EmberCommandEntry emberCommandTable[];

/**
 * Configuration byte.
 */
extern uint8_t emberCommandInterpreter2Configuration;

#define EMBER_COMMAND_INTERPRETER_CONFIGURATION_ECHO (0x01)

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Command error states.
 *
 * If you change this list, ensure you also change the strings that describe
 * these errors in the array emberCommandErrorNames[] in command-interpreter.c.
 */
enum EmberCommandStatus
#else
typedef uint8_t EmberCommandStatus;
enum
#endif
{
  EMBER_CMD_SUCCESS,
  EMBER_CMD_ERR_PORT_PROBLEM,
  EMBER_CMD_ERR_NO_SUCH_COMMAND,
  EMBER_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS,
  EMBER_CMD_ERR_ARGUMENT_OUT_OF_RANGE,
  EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR,
  EMBER_CMD_ERR_STRING_TOO_LONG,
  EMBER_CMD_ERR_INVALID_ARGUMENT_TYPE
};

/** @name Functions to Retrieve Arguments
 * Use the following functions in your functions that process commands to
 * retrieve arguments from the command interpreter.
 * These functions pull out unsigned integers, signed integers, and strings,
 * and hex strings.  Index 0 is the first command argument.
 *@{
 */

/** Returns the number of arguments for the current command. */
uint8_t emberCommandArgumentCount(void);

/** Retrieves unsigned integer arguments. */
uint32_t emberUnsignedCommandArgument(uint8_t argNum);

/** Retrieves signed integer arguments. */
int32_t emberSignedCommandArgument(uint8_t argNum);

/** Parses an IPv4 address string and returns a host order uint32_t.
 * Returns true if address is valid dotted quad notation (A.B.C.D), false otherwise.
 */
bool emberStringToHostOrderIpv4Address(const uint8_t* string, uint32_t* hostOrderIpv4Address);

/** Parses an IPv4 address string from a command argument and returns host order uint32_t.
 * Returns true if address is valid dotted quad notation (A.B.C.D), false otherwise.
 */
bool emberStringArgumentToHostOrderIpv4Address(uint8_t argNum, uint32_t* hostOrderIpv4Address);

/** Retrieve quoted string or hex string arguments.
 * Hex strings have already been converted into binary.
 * To retrieve the name of the command itself, use an argNum of -1.
 * For example, to retrieve the first character of the command, do:
 * uint8_t firstChar = emberStringCommandArgument(-1, NULL)[0].
 * If the command is nested, an index of -2, -3, etc will work to retrieve
 * the higher level command names.
 * Note that [-1] only returns the text entered. If an abbreviated command
 * name is entered only the text entered will be returned with [-1].
 */
uint8_t *emberStringCommandArgument(int8_t argNum, uint8_t *length);

const char *emberCommandName(void);

/** Copies the string argument to the given destination up to maxLength.
 * If the argument length is nonzero but less than maxLength
 * and leftPad is true, leading zeroes are prepended to bring the
 * total length of the target up to maxLength.  If the argument
 * is longer than the maxLength, it is truncated to maxLength.
 * Returns the minimum of the argument length and maxLength.
 *
 * This function is commonly used for reading in hex strings
 * such as EUI64 or key data and left padding them with zeroes.
 * See ::emberCopyKeyArgument and ::emberCopyEui64Argument for
 * convenience macros for this purpose.
 */
uint8_t emberCopyStringArgument(int8_t argNum,
                                uint8_t *destination,
                                uint8_t maxLength,
                                bool leftPad);

/** A convenience macro for copying security key arguments to an
 * EmberKeyData pointer.
 */
#define emberCopyKeyArgument(index, keyDataPointer)            \
  (emberCopyStringArgument((index),                            \
                           emberKeyContents((keyDataPointer)), \
                           EMBER_ENCRYPTION_KEY_SIZE,          \
                           true))

/** A convenience macro for copying eui64 arguments to an EmberEUI64. */
#define emberCopyEui64Argument(index, eui64) \
  (emberCopyStringArgument((index), (eui64), EUI64_SIZE, true))
#define emberGetEui64Argument(index, eui64) \
  (emberCopyStringArgument((index), (eui64), EUI64_SIZE, true))

/** Copies eui64 arguments in big-endian format to an EmberEUI64.  This is
 *  useful because eui64s are often presented to users in big-endian format
 *  even though they are used in software in little-endian format.
 */
uint8_t emberCopyBigEndianEui64Argument(int8_t index, EmberEUI64 destination);

/** @} END name group
 */

void emberCommandReaderSetDefaultBase(uint8_t base);

/** The application may implement this handler.  To override
 * the default handler, define EMBER_APPLICATION_HAS_COMMAND_ACTION_HANDLER
 * in the CONFIGURATION_HEADER.
 */
void emberCommandActionHandler(const CommandAction action);
/** The application may implement this handler.  To override
 * the default handler, define EMBER_APPLICATION_HAS_COMMAND_ERROR_HANDLER
 * in the CONFIGURATION_HEADER.  Defining this will also remove the
 * help functions ::emberPrintCommandUsage(), ::emberPrintCommandUsageNotes(),
 * and ::emberPrintCommandTable().
 */
void emberCommandErrorHandler(EmberCommandStatus status);
void emberPrintCommandUsage(EmberCommandEntry *entry);
void emberPrintCommandUsageNotes(void);
void emberPrintCommandTable(void);
void emberCommandClearBuffer(void);

/** @brief Initialize the command interpreter.
 */
void emberCommandReaderInit(void);

/** @brief Process the given string as a command.
 */
bool emberProcessCommandString(uint8_t *input, uint8_t sizeOrPort);

/** @brief Process input coming in on the given serial port.
 * @return true if an end of line character was read.
 * If the application uses a command line prompt,
 * this indicates it is time to print the prompt.
 * @code
 * void emberProcessCommandInput(uint8_t port);
 * @endcode
 */
#define emberProcessCommandInput(port) \
  emberProcessCommandString(NULL, port)

/** @brief Turn echo of command line on.
 */
#define emberCommandInterpreterEchoOn()  \
  (emberCommandInterpreter2Configuration \
     |= EMBER_COMMAND_INTERPRETER_CONFIGURATION_ECHO)

/** @brief Turn echo of command line off.
 */
#define emberCommandInterpreterEchoOff() \
  (emberCommandInterpreter2Configuration \
     &= (~EMBER_COMMAND_INTERPRETER_CONFIGURATION_ECHO))

/** @brief Returns true if echo is on, false otherwise.
 */
#define emberCommandInterpreterIsEchoOn() \
  (emberCommandInterpreter2Configuration  \
   & EMBER_COMMAND_INTERPRETER_CONFIGURATION_ECHO)

/** @brief Returns true interpreter is reading a command.
 */
bool emberCommandInterpreterBusy(void);

/** @} END addtogroup
 */

#endif // SILABS_COMMAND_INTERPRETER2_H
