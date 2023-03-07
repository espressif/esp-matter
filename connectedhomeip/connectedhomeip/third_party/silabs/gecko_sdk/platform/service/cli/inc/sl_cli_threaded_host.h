/***************************************************************************//**
 * @file
 * @brief Threading of CLI for Hosts
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifdef EZSP_HOST
#include <assert.h>
#include <pthread.h>
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <errno.h>
#include <semaphore.h>
#endif
#include "sl_cli.h"

struct semaphore {
#ifdef __APPLE__
  dispatch_semaphore_t    sem;
#else // __APPLE__
  sem_t                   sem;
#endif // __APPLE__
};

extern struct semaphore thread_event;

void sema_wait(struct semaphore *s);

void sli_cli_handle_input_and_history(sl_cli_handle_t handle);

void sli_cli_threaded_host_init(void);

bool sli_cli_is_input_handled(void);

int sli_cli_get_pipe_read_fd(void);
#endif // EZSP_HOST
