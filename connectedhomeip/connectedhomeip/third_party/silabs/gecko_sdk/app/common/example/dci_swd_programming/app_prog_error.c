/***************************************************************************//**
 * @file app_prog_error.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_prog_error.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Program error code
uint32_t error_code;

/// Program error index
int8_t prog_error_index = -1;

/// Program error buffer
jmp_buf prog_error[EXCEPTION_MAX_LEVEL];

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get string from error code.
 ******************************************************************************/
char *get_error_string(uint32_t code)
{
  switch (code) {
    case SWD_ERROR_OK:
      return "No error.";
    case SWD_ERROR_WAIT:
      return "Timed out while waiting for WAIT response.";
    case SWD_ERROR_FAULT:
      return "Target returned FAULT response.";
    case SWD_ERROR_PROTOCOL:
      return "Protocol error, target does not respond.";
    case SWD_ERROR_PARITY:
      return "Parity error.";
    case SWD_ERROR_INVALID_IDR:
      return "Invalid IDR value.";
    case SWD_ERROR_FLASH_WRITE_FAILED:
      return "Write to flash failed.";
    case SWD_ERROR_WRONG_START_ADDR:
      return "The start address is not page aligned.";
    case SWD_ERROR_BINARY_SIZE:
      return "The image size is too big.";
    case SWD_ERROR_VERIFY_FW_FAILED:
      return "Application verification failed.";
    case SWD_ERROR_TIMEOUT_WAITING_RESET:
      return "Timeout while waiting for target to reset.";
    case SWD_ERROR_UNKNOWN_DEVICE:
      return "Unknown device.";
    case SWD_ERROR_DEVICE_ERASE_FAILED:
      return "Device erase failed.";
    case SWD_ERROR_TIMEOUT_HALT:
      return "Timeout while waiting to halt target.";
    case SWD_ERROR_DEBUG_POWER:
      return "Failed to power up debug interface.";
    case SWD_ERROR_INVALID_IDCODE:
      return "Invalid IDCODE.";
    case SWD_ERROR_USERDATA_LOCK:
      return "The user data is locked.";
    case SWD_ERROR_MASSERASE_LOCK:
      return "The mass erase is locked.";
    case DCI_ERROR_WRITE_COMMAND:
      return "Fail to write command due to RDATAVALID is high.";
    case DCI_ERROR_WRITE_TIMEOUT:
      return "Timed out while writing DCI register.";
    case DCI_ERROR_READ_TIMEOUT:
      return "Timed out while reading DCI register.";
    case DCI_RESPONSE_OK:
      return "Command successful.";
    case DCI_RESPONSE_INVALID_COMMAND:
      return "Unsupported command.";
    case DCI_RESPONSE_AUTHORIZATION_ERROR:
      return "Unauthorized command.";
    case DCI_RESPONSE_INVALID_SIGNATURE:
      return "Invalid signature.";
    case DCI_RESPONSE_BUS_ERROR:
      return "Bus error.";
    case DCI_RESPONSE_INTERNAL_ERROR:
      return "Internal SE error.";
    case DCI_RESPONSE_CRYPTO_ERROR:
      return "Error in crypto operation.";
    case DCI_RESPONSE_INVALID_PARAMETER:
      return "Parameters are invalid or buffer is too small.";
    case DCI_RESPONSE_INTEGRITY_ERROR:
      return "SE is in a deficient state due to hardware corruption.";
    case DCI_RESPONSE_SECUREBOOT_ERROR:
      return "Failure while checking the host for secure boot.";
    case DCI_RESPONSE_SELFTEST_ERROR:
      return "Failure during self-test.";
    case DCI_RESPONSE_NOT_INITIALIZED:
      return "Feature/item is not present or not initialized.";
    default:
      return "Unknown error.";
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
