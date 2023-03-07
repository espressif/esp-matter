/***************************************************************************//**
 * @file
 * @brief Signal handler abstaction module.
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

#include <stddef.h>
#include <windows.h>
#include "app_signal.h"

static app_signal_handler_t sigint_handler = SIG_DFL;
static app_signal_handler_t sigterm_handler = SIG_DFL;

static app_signal_handler_t app_signal_sigint(app_signal_handler_t func);
static app_signal_handler_t app_signal_sigterm(app_signal_handler_t func);
static BOOL WINAPI app_signal_sigint_handler(_In_ DWORD dwCtrlType);
static BOOL WINAPI app_signal_sigterm_handler(_In_ DWORD dwCtrlType);

app_signal_handler_t app_signal(int sig, app_signal_handler_t func)
{
  app_signal_handler_t ret = NULL;

  switch (sig) {
    case SIGINT:
      ret = app_signal_sigint(func);
      break;

    case SIGTERM:
      ret = app_signal_sigterm(func);
      break;

    default:
      ret = SIG_ERR;
      break;
  }
  return ret;
}

static app_signal_handler_t app_signal_sigint(app_signal_handler_t func)
{
  app_signal_handler_t ret = sigint_handler;
  BOOL success = FALSE;

  if (SIG_DFL == func) {
    // Set default handler for this signal
    success = SetConsoleCtrlHandler(NULL, FALSE);
  } else if (SIG_IGN == func) {
    // Ignore this signal
    success = SetConsoleCtrlHandler(NULL, TRUE);
  } else {
    // Set custom signal handler
    success = SetConsoleCtrlHandler(app_signal_sigint_handler, TRUE);
  }

  if (success) {
    sigint_handler = func;
  } else {
    ret = SIG_ERR;
  }

  return ret;
}

static app_signal_handler_t app_signal_sigterm(app_signal_handler_t func)
{
  app_signal_handler_t ret = sigint_handler;
  BOOL success = FALSE;

  if (SIG_DFL == func) {
    // Set default handler for this signal
    success = SetConsoleCtrlHandler(app_signal_sigterm_handler, FALSE);
  } else {
    // Set custom signal handler or ignore this signal
    success = SetConsoleCtrlHandler(app_signal_sigterm_handler, TRUE);
  }

  if (success) {
    sigint_handler = func;
  } else {
    ret = SIG_ERR;
  }

  return ret;
}

static BOOL WINAPI app_signal_sigint_handler(_In_ DWORD dwCtrlType)
{
  BOOL ret = FALSE;

  if (dwCtrlType == CTRL_C_EVENT) {
    if (SIG_DFL == sigint_handler) {
      // Let the default handler handle this signal
    } else if (SIG_IGN == sigint_handler) {
      // Mark the signal handled but do nothing
      ret = TRUE;
    } else {
      // Mark the signal handled and call handler
      if (NULL != sigint_handler) {
        sigint_handler(SIGINT);
      }
      ret = TRUE;
    }
  }
  return ret;
}

static BOOL WINAPI app_signal_sigterm_handler(_In_ DWORD dwCtrlType)
{
  BOOL ret = FALSE;

  if (dwCtrlType == CTRL_CLOSE_EVENT) {
    if (SIG_DFL == sigterm_handler) {
      // Let the default handler handle this signal
    } else if (SIG_IGN == sigterm_handler) {
      // Mark the signal handled but do nothing
      ret = TRUE;
    } else {
      // Mark the signal handled and call handler
      if (NULL != sigterm_handler) {
        sigterm_handler(SIGTERM);
      }
      ret = TRUE;
    }
  }
  return ret;
}
