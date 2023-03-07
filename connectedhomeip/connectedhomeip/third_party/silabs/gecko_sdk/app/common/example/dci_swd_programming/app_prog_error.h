/***************************************************************************//**
 * @file app_prog_error.h
 * @brief Error handling functions.
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
#ifndef APP_PROG_ERROR_H
#define APP_PROG_ERROR_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <setjmp.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Error codes
#define SWD_ERROR_OK                            1
#define SWD_ERROR_WAIT                          2
#define SWD_ERROR_FAULT                         3
#define SWD_ERROR_PROTOCOL                      4
#define SWD_ERROR_PARITY                        5
#define SWD_ERROR_INVALID_IDR                   6
#define SWD_ERROR_INVALID_IDCODE                7
#define SWD_ERROR_FLASH_WRITE_FAILED            8
#define SWD_ERROR_WRONG_START_ADDR              9
#define SWD_ERROR_BINARY_SIZE                   10
#define SWD_ERROR_VERIFY_FW_FAILED              11
#define SWD_ERROR_TIMEOUT_WAITING_RESET         12
#define SWD_ERROR_UNKNOWN_DEVICE                13
#define SWD_ERROR_DEVICE_ERASE_FAILED           14
#define SWD_ERROR_TIMEOUT_HALT                  15
#define SWD_ERROR_DEBUG_POWER                   16
#define SWD_ERROR_USERDATA_LOCK                 17
#define SWD_ERROR_MASSERASE_LOCK                18
#define DCI_ERROR_WRITE_COMMAND                 19
#define DCI_ERROR_WRITE_TIMEOUT                 20
#define DCI_ERROR_READ_TIMEOUT                  21
#define DCI_RESPONSE_OK                         22
#define DCI_RESPONSE_INVALID_COMMAND            23
#define DCI_RESPONSE_AUTHORIZATION_ERROR        24
#define DCI_RESPONSE_INVALID_SIGNATURE          25
#define DCI_RESPONSE_BUS_ERROR                  26
#define DCI_RESPONSE_INTERNAL_ERROR             27
#define DCI_RESPONSE_CRYPTO_ERROR               28
#define DCI_RESPONSE_INVALID_PARAMETER          29
#define DCI_RESPONSE_INTEGRITY_ERROR            30
#define DCI_RESPONSE_SECUREBOOT_ERROR           31
#define DCI_RESPONSE_SELFTEST_ERROR             32
#define DCI_RESPONSE_NOT_INITIALIZED            33

/***************************************************************************//**
 * @note
 * These macros implements a simple exception-like system for handling errors in
 * C. By using this we can avoid checking for error return values on every
 * function call. The exception system is based on the library functions
 * setjmp() and longjmp(). Four macros are defined, TRY, CATCH, ENDRY and
 * RAISE(x).
 *
 * The macros are used like this:
 * TRY
 *   someFunction();
 *   someOtherFunction();
 * CATCH
 *   printf("Error occurred: %s\n", getErrorString(errorCode));
 * ENDTRY
 *
 * It is possible to raise an exception like this:
 * void someFunction(void)
 * {
 *   .....
 *   if (someErrorCondition) {
 *     RAISE(SWD_ERROR_SOME_ERROR);
 *   }
 *   ....
 * }
 *
 * The error codes are defined in app_prog_error.h. After a call to RAISE(), the
 * execution will jump to the CATCH block which will receive the error in a
 * variable called error_code. The get_error_string() function can be used to
 * conveniently print an error message.
 *
 * TRY/CATCH blocks can be nested. I.e. someFunction() can include its own
 * TRY/CATCH block. Any RAISE()'ed error will return to the closest CATCH block.
 * The maximum nesting level is specified with EXCEPTION_MAX_LEVEL.
 *
 * IMPORTANT
 *   Do not RAISE() an error within a CATCH block.
 *   Exiting early from a TRY block in any other way than RAISE() WILL break the
 *   system and lead to undefined behavior. Therefore make sure to never have a
 *   return statement within the TRY or CATCH blocks.
 ******************************************************************************/
/// Maximum number of nested TRY/CATCH blocks
#define EXCEPTION_MAX_LEVEL     5

/// Start TRY/CATCH block
#define TRY                                            \
  error_code = setjmp(prog_error[++prog_error_index]); \
  if (error_code == 0) {
/// Start CATCH clause
#define CATCH           } else {
/// End TRY/CATCH block
#define ENDTRY          } prog_error_index--;

/// Raise an error
#define RAISE(x)        longjmp(prog_error[prog_error_index], x)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Program error code
extern uint32_t error_code;

/// Program error index
extern int8_t prog_error_index;

/// Program error buffer
extern jmp_buf prog_error[EXCEPTION_MAX_LEVEL];

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get string from error code.
 *
 * @param code The error code to get string.
 * @returns The pointer to the string.
 ******************************************************************************/
char *get_error_string(uint32_t code);

#endif  // APP_PROG_ERROR_H
