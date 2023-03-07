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
#include "sli_cli_io.h"
#include "sl_cli_input.h"
#include "sl_cli_threaded_host.h"
#include <unistd.h> // for pipe()

#if defined(EZSP_HOST) && !defined(EMBER_TEST)

struct semaphore thread_event;

static volatile sl_cli_handle_t threaded_tick_cli_handle = 0;
pthread_t thread_rx;

static volatile bool tick_handle_input = false;

#define INVALID_FD -1
static int sl_cli_threaded_host_pipe_fds[2] = { INVALID_FD, INVALID_FD };
#define PIPE_DATA_READER sl_cli_threaded_host_pipe_fds[0]
#define PIPE_DATA_WRITER sl_cli_threaded_host_pipe_fds[1]

static inline void sema_init(struct semaphore *s, uint32_t value)
{
#ifdef __APPLE__
  dispatch_semaphore_t *sem = &s->sem;

  *sem = dispatch_semaphore_create(value);
#else // __APPLE__
  sem_init(&s->sem, 0, value);
#endif // __APPLE__
}

void sema_wait(struct semaphore *s)
{
#ifdef __APPLE__
  dispatch_semaphore_wait(s->sem, DISPATCH_TIME_NOW);
#else // __APPLE__
  sem_trywait(&s->sem);
#endif // __APPLE__
}

static inline void sema_post(struct semaphore *s)
{
#ifdef __APPLE__
  dispatch_semaphore_signal(s->sem);
#else // __APPLE__
  sem_post(&s->sem);
#endif // __APPLE__
}

bool sli_cli_is_input_handled(void)
{
  return tick_handle_input;
}

int sli_cli_get_pipe_read_fd(void)
{
  return PIPE_DATA_READER;
}

void *threaded_tick(void *ptr)
{
  int c;
  bool newline = false;
  tick_handle_input = false;
  char newLineChars[] = "\r\n"; // used by PIPE_DATA_WRITER to wake up the host app

  (void) ptr;

  while (1) {
    if (threaded_tick_cli_handle != 0) {
      threaded_tick_cli_handle->tick_in_progress = true;
#ifdef PRINT_THREADED_TICK_ACTIVITY
      fprintf(stderr, "[T]");
#endif // PRINT_THREADED_TICK_ACTIVITY

      if (threaded_tick_cli_handle->req_prompt) {
        threaded_tick_cli_handle->req_prompt = false;
        sli_cli_io_printf("%s", threaded_tick_cli_handle->prompt_string);
      }
#if defined(SL_CLI_ACTIVE_FLAG_EN)
      threaded_tick_cli_handle->active = false;
#endif

      do {
#if !defined(SL_CATALOG_KERNEL_PRESENT)
        if (threaded_tick_cli_handle->input_char != EOF) {
          c = threaded_tick_cli_handle->input_char;
          threaded_tick_cli_handle->input_char = EOF;
        } else
#endif
        {
#ifdef PRINT_GETCHAR_ACTIVITY
          fprintf(stderr, "[c]");
#endif // PRINT_GETCHAR_ACTIVITY
          c = sli_cli_io_getchar();
        }
        if (c != EOF) {
#ifdef PRINT_CLI_INPUT_CHAR_ACTIVITY
          fprintf(stderr, "[sl_cli_input_char]");
#endif // PRINT_CLI_INPUT_CHAR_ACTIVITY
          newline = sl_cli_input_char(threaded_tick_cli_handle, (char)c);
        } else {
          // Even if there is no valid input, sli_cli_handle_input_and_history() should
          // be invoked to ensure all behavior is executed regularly.
          tick_handle_input = true;
        }
      } while ((c != EOF) && (!newline));

      if (newline) {
        // Write a new line to the pipe to wake up the host app
        write(PIPE_DATA_WRITER, &newLineChars, 2);
#ifdef PRINT_SEMA_POST_ACTIVITY
        fprintf(stderr, "[sema_post]");
#endif // PRINT_SEMA_POST_ACTIVITY
        tick_handle_input = true;
        // This is the point where we used to call sli_cli_handle_input_and_history().
        // That function should not be invoked from a thread.
        sema_post(&thread_event);
#if defined(SL_CLI_ACTIVE_FLAG_EN)
        threaded_tick_cli_handle->req_prompt = true;
        threaded_tick_cli_handle->active = true;
#endif
      }
      threaded_tick_cli_handle->tick_in_progress = false;
    }
  }
  // This thread should never exit.
  assert(0);
}

static bool is_pthread_initilized = false;
// This function must be called during startup before CLI can be used,
// and before sli_zigbee_app_framework_init_callback().
void sli_cli_threaded_host_init(void)
{
  int iret;

  if (is_pthread_initilized) {
    return;
  }

  tick_handle_input = false;

  sema_init(&thread_event, 0);

  iret = pthread_create(&thread_rx, NULL, threaded_tick, NULL);
  if (iret) {
    fprintf(stderr, "pthread_create:%d\n", iret);
    assert(0);
  }

  // CLI is processed in a thread running threaded_tick while the host app
  // can be blocked at the select() running in the main thread. Hence, create
  // pipe descriptors here that can be used to wake up the host app.
  iret = pipe(sl_cli_threaded_host_pipe_fds);
  if (iret) {
    fprintf(stderr, "pipe:%d\n", iret);
    assert(0);
  }

  is_pthread_initilized = true;
}

bool sli_cli_tick(sl_cli_handle_t handle)
{
  if (threaded_tick_cli_handle == 0) {
    threaded_tick_cli_handle = handle;
  }

  // Note: sema_wait() should never block.
  sema_wait(&thread_event);

  if (tick_handle_input) {
#ifdef PRINT_TICK_INPUT_ACTIVITY
    fprintf(stderr, "[tick/tick_handle_input]");
#endif // PRINT_TICK_INPUT_ACTIVITY

    sli_cli_handle_input_and_history(threaded_tick_cli_handle);

    tick_handle_input = false;

    // Valid input was found.
    return true;
  }

  // No valid input.
  return false;
}

#else
// To satisfy builds that need a definition but don't actually
// use pthread.
void sli_cli_threaded_host_init(void)
{
}
#endif // EZSP_HOST
