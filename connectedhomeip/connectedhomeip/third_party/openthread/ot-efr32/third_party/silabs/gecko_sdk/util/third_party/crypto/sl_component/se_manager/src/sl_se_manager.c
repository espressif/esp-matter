/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

#include "sl_se_manager.h"

#if !defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)

#include "sli_se_manager_internal.h"
#include "sli_se_manager_osal.h"
#include "em_se.h"
#include "sl_assert.h"
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
#include "em_bus.h"
#endif
#include <string.h>

/// @addtogroup sl_se_manager
/// @{

// -----------------------------------------------------------------------------
// Locals

#if defined(SL_SE_MANAGER_THREADING) \
  || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

// Flag to indicate that the SE Manager is initialized or not.
static volatile bool se_manager_initialized = false;

  #if defined(SL_SE_MANAGER_THREADING)
// Lock mutex for synchronizing multiple threads calling into the
// SE Manager API.
static se_manager_osal_mutex_t se_lock = { 0 };

  #define SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_START                     \
  int32_t kernel_lock_state = 0;                                           \
  osKernelState_t kernel_state = se_manager_osal_kernel_get_state();       \
  if (kernel_state != osKernelInactive && kernel_state != osKernelReady) { \
    kernel_lock_state = se_manager_osal_kernel_lock();                     \
    if (kernel_lock_state < 0) {                                           \
      return SL_STATUS_FAIL;                                               \
    }                                                                      \
  }

  #define SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_END                       \
  if (kernel_state != osKernelInactive && kernel_state != osKernelReady) { \
    if (se_manager_osal_kernel_restore_lock(kernel_lock_state) < 0) {      \
      return SL_STATUS_FAIL;                                               \
    }                                                                      \
  }

  #endif // SL_SE_MANAGER_THREADING

  #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)
// SE command completion.
static se_manager_osal_completion_t se_command_completion;
// SE mailbox command response code. This value is read from the SEMAILBOX
// in ISR in order to clear the command complete interrupt condition.
static SE_Response_t se_manager_command_response = SLI_SE_RESPONSE_INTERNAL_ERROR;
  #endif // SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION

#endif // #if defined (SL_SE_MANAGER_THREADING)
//   || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

// -----------------------------------------------------------------------------
// Global functions

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t sl_se_init(void)
{
  sl_status_t ret = SL_STATUS_OK;
  #if defined (SL_SE_MANAGER_THREADING) \
  || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  #if defined(SL_SE_MANAGER_THREADING)
  SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_START
  #endif

  if ( !se_manager_initialized ) {
      #if defined(SL_SE_MANAGER_THREADING)
    // Initialize SE lock
    ret = se_manager_osal_init_mutex(&se_lock);
      #endif
      #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)
    if (ret == SL_STATUS_OK) {
      // Initialize command completion object.
      ret = se_manager_osal_init_completion(&se_command_completion);
      if (ret == SL_STATUS_OK) {
        // Enable SE RX mailbox interrupt in NVIC, but not in SEMAILBOX
        // which will be enabled if the yield parameter in
        // sli_se_execute_and_wait is true.
        NVIC_SetPriority(SEMBRX_IRQn, SE_MANAGER_SEMBRX_IRQ_PRIORITY);
        NVIC_EnableIRQ(SEMBRX_IRQn);
      }
    }
      #endif // SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION
    if (ret == SL_STATUS_OK) {
      se_manager_initialized = true;
    }
  }

  #if defined(SL_SE_MANAGER_THREADING)
  SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_END
  #endif

  #endif // #if defined (SL_SE_MANAGER_THREADING)
  //   || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  return ret;
}

/***************************************************************************//**
 * Denitialize the SE Manager.
 ******************************************************************************/
sl_status_t sl_se_deinit(void)
{
  sl_status_t ret = SL_STATUS_OK;

  #if defined (SL_SE_MANAGER_THREADING) \
  || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  #if defined(SL_SE_MANAGER_THREADING)
  SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_START
  #endif

  if ( se_manager_initialized ) {
    // We need to exit the critical section in case the SE lock is held by a
    // thread, and we want to take it before de-initializing.
    #if defined(SL_SE_MANAGER_THREADING)
    SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_END
    #endif

    // Acquire the SE lock to make sure no thread is executing SE commands
    // when we de-initialize.
    ret = sli_se_lock_acquire();
    if (ret != SL_STATUS_OK) {
      return ret;
    }

      #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)
    // Disable SE RX mailbox interrupt in NVIC.
    NVIC_ClearPendingIRQ(SEMBRX_IRQn);
    NVIC_DisableIRQ(SEMBRX_IRQn);
    // Free command completion object.
    ret = se_manager_osal_free_completion(&se_command_completion);
      #endif // SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION

      #if defined(SL_SE_MANAGER_THREADING)
    if (ret == SL_STATUS_OK) {
      // Free the SE lock mutex
      ret = se_manager_osal_free_mutex(&se_lock);
    }
      #endif

    // Mark the SE Manager as un-initialized.
    se_manager_initialized = false;
  }
  #if defined(SL_SE_MANAGER_THREADING)
  else {
    SLI_SE_MANAGER_KERNEL_CRITICAL_SECTION_END
  }
  #endif

  #endif // #if defined (SL_SE_MANAGER_THREADING)
  //   || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  return ret;
}

/***************************************************************************//**
 * @brief
 *   Translate SE response codes to sl_status_t codes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sli_se_to_sl_status(SE_Response_t res)
{
  switch (res) {
    case SLI_SE_RESPONSE_OK:
      return SL_STATUS_OK;
    case SLI_SE_RESPONSE_INVALID_COMMAND:
      return SL_STATUS_COMMAND_IS_INVALID;
    case SLI_SE_RESPONSE_AUTHORIZATION_ERROR:
      return SL_STATUS_INVALID_CREDENTIALS;
    case SLI_SE_RESPONSE_INVALID_SIGNATURE:
      return SL_STATUS_INVALID_SIGNATURE;
    case SLI_SE_RESPONSE_BUS_ERROR:
      return SL_STATUS_BUS_ERROR;
    case SLI_SE_RESPONSE_INTERNAL_ERROR:
      return SL_STATUS_FAIL;
    case SLI_SE_RESPONSE_CRYPTO_ERROR:
      return SL_STATUS_FAIL;
    case SLI_SE_RESPONSE_INVALID_PARAMETER:
      return SL_STATUS_INVALID_PARAMETER;
    case SLI_SE_RESPONSE_ABORT:
      return SL_STATUS_ABORT;
    case SLI_SE_RESPONSE_SELFTEST_ERROR:
      return SL_STATUS_INITIALIZATION;
    case SLI_SE_RESPONSE_NOT_INITIALIZED:
      return SL_STATUS_NOT_INITIALIZED;
#if defined(CRYPTOACC_PRESENT)
    case SLI_SE_RESPONSE_MAILBOX_INVALID:
      return SL_STATUS_COMMAND_IS_INVALID;
#endif
    default:
      // Assert we do not get a bad SE response code.
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Acquire the SE lock for exclusive access if necessary (thread mode).
 * Enable the SEMAILBOX clock if necessary.
 ******************************************************************************/
sl_status_t sli_se_lock_acquire(void)
{
  #if defined(SL_SE_MANAGER_THREADING)
  sl_status_t status = se_manager_osal_take_mutex(&se_lock);
  #else
  sl_status_t status = SL_STATUS_OK;
  #endif
  #if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  if (status == SL_STATUS_OK) {
    BUS_RegBitWrite(&CMU->CLKEN1, _CMU_CLKEN1_SEMAILBOXHOST_SHIFT, 1);
  }
  #endif
  return status;
}

/***************************************************************************//**
 * Release the SE lock if necessary (thread mode).
 * Disable the SEMAILBOX clock if necessary.
 ******************************************************************************/
sl_status_t sli_se_lock_release(void)
{
  #if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  BUS_RegBitWrite(&CMU->CLKEN1, _CMU_CLKEN1_SEMAILBOXHOST_SHIFT, 0);
  #endif
  #if defined(SL_SE_MANAGER_THREADING)
  return se_manager_osal_give_mutex(&se_lock);
  #else
  return SL_STATUS_OK;
  #endif
}

#if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

/***************************************************************************//**
 * @brief
 *   SE Mailbox Interrupt Service Routine
 ******************************************************************************/
void SEMBRX_IRQHandler(void)
{
  sl_status_t status;
  // Check if the SE mailbox is the source of the interrupt.
  if (SEMAILBOX_HOST->RX_STATUS & SEMAILBOX_RX_STATUS_RXINT) {
    // Signal SE mailbox completion.
    status = se_manager_osal_complete(&se_command_completion);
    EFM_ASSERT(status == SL_STATUS_OK);
  }
  // Get command response (clears interrupt condition in SEMAILBOX)
  se_manager_command_response = SE_readCommandResponse();
  // Clear interrupt condition in NVIC
  NVIC_ClearPendingIRQ(SEMBRX_IRQn);
}

#endif // #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

/***************************************************************************//**
 * Set the yield attribute of the SE command context object.
 ******************************************************************************/
sl_status_t sl_se_set_yield(sl_se_command_context_t *cmd_ctx,
                            bool yield)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)
  cmd_ctx->yield = yield;
  return SL_STATUS_OK;
  #else
  if (yield) {
    return SL_STATUS_NOT_AVAILABLE;
  } else {
    (void) cmd_ctx;
    return SL_STATUS_OK;
  }
  #endif
}

/***************************************************************************//**
 * @brief
 *   Execute and wait for SE mailbox command to complete.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
#if defined(SEMAILBOX_PRESENT)
sl_status_t sli_se_execute_and_wait(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t status;
  SE_Response_t command_response;

  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Try to acquire SE lock
  status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Execute SE mailbox command
  SE_executeCommand(&cmd_ctx->command);

  #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)
  if (cmd_ctx->yield) {
    // Enable SEMAILBOX RXINT interrupt
    SE_enableInterrupt(SEMAILBOX_CONFIGURATION_RXINTEN);

    // Yield and Wait for the command completion signal
    status = se_manager_osal_wait_completion(&se_command_completion,
                                             SE_MANAGER_OSAL_WAIT_FOREVER);

    // Disable SEMAILBOX RXINT interrupt.
    SE_disableInterrupt(SEMAILBOX_CONFIGURATION_RXINTEN);

    if (status != SL_STATUS_OK) {
      return status;
    }

    // Get response which is read in the ISR to clear interrupt condition.
    command_response = se_manager_command_response;
    // Default to an error.
    se_manager_command_response = SLI_SE_RESPONSE_INTERNAL_ERROR;
  } else {
    // Wait for command completion and get command response
    command_response = SE_readCommandResponse();
  }

  #else // #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  // Wait for command completion and get command response
  command_response = SE_readCommandResponse();

  #endif // #if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION)

  // Release SE lock
  status = sli_se_lock_release();

  // Return sl_status_t code.
  if (command_response == SLI_SE_RESPONSE_OK) {
    return status;
  } else {
    // Convert from SE_Response_t to sl_status_t code and return.
    return sli_se_to_sl_status(command_response);
  }
}

#elif defined(CRYPTOACC_PRESENT) // SEMAILBOX_PRESENT

sl_status_t sli_se_execute_and_wait(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t status;

  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Try to acquire SE lock
  status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Execute SE mailbox command
  SE_executeCommand(&cmd_ctx->command);

  return SL_STATUS_FAIL; // Should never get to this point
}

/***************************************************************************//**
 * From VSE mailbox read which command, if any, was executed.
 ******************************************************************************/
sl_status_t sl_se_read_executed_command(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t status;

  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Try to acquire SE lock
  status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Read command
  cmd_ctx->command.command = SE_readExecutedCommand();

  // Release SE lock
  status = sli_se_lock_release();

  // Return sl_status_t code.
  if (cmd_ctx->command.command == SE_RESPONSE_MAILBOX_INVALID) {
    // Convert from SE_Response_t to sl_status_t code and return.
    return sli_se_to_sl_status(SE_RESPONSE_MAILBOX_INVALID);
  } else {
    return status;
  }
}

/***************************************************************************//**
 * Acknowledge and get status and output data of a completed command.
 ******************************************************************************/
sl_status_t sl_se_ack_command(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t status;
  SE_Response_t command_response;

  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Try to acquire SE lock
  status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Acknowledge VSE mailbox command
  command_response = SE_ackCommand(&cmd_ctx->command);

  // Release SE lock
  status = sli_se_lock_release();

  // Return sl_status_t code.
  if (command_response == SLI_SE_RESPONSE_OK) {
    return status;
  } else {
    // Convert from SE_Response_t to sl_status_t code and return.
    return sli_se_to_sl_status(command_response);
  }
}

#endif // CRYPTOACC_PRESENT

#endif // !SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT

/***************************************************************************//**
 * Initialize an SE command context object
 ******************************************************************************/
sl_status_t sl_se_init_command_context(sl_se_command_context_t *cmd_ctx)
{
  sl_se_command_context_t v = SL_SE_COMMAND_CONTEXT_INIT;

  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  *cmd_ctx = v;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * De-initialize an SE command context object
 ******************************************************************************/
sl_status_t sl_se_deinit_command_context(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_se_init_command_context(cmd_ctx);
}

/** @} (end addtogroup sl_se) */

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
