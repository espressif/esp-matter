/***************************************************************************//**
 * @file
 * @brief sl_poll.c
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include "sl_poll.h"

static struct pollfd polled[SL_POLL_MAX];
static int max_polled = 0;
static int update_polled = 0;

int sl_poll_add(int fd)
{
  if (max_polled >= SL_POLL_MAX) {
    return -1;
  }
  polled[max_polled].fd = fd;
  polled[max_polled].events = POLLIN;
  polled[max_polled].revents = 0;
  max_polled++;
  return 0;
}

void sl_poll_remove(int fd)
{
  int i;
  for (i = 0; i < max_polled; i++) {
    if (polled[i].fd == fd) {
      polled[i].fd = -1;
      update_polled = 1;
      return;
    }
  }
}

int sl_poll_is_polled(int fd)
{
  for (int i = 0; i < max_polled; i++) {
    if (polled[i].fd == fd) {
      return 1;
    }
  }
  return 0;
}

int sl_poll_init()
{
  return 0;
}

int sl_poll_wait(int timeout)
{
  int result;

  //clean table if descriptor was removed
  if (update_polled) {
    int r, w;
    for (r = w = 0; r < max_polled; r++) {
      if (polled[r].fd == -1) {
        continue;
      }
      polled[w] = polled[r];
      w++;
    }
    max_polled = w;
  }

  result = poll(polled, max_polled, timeout == 0 ? -1 : timeout);
  return result;
}

int sl_poll_update()
{
  for (int i = 0; i < max_polled; i++) {
    if (polled[i].revents == 0) {
      continue;
    }

    sl_poll_cb(polled[i].fd, polled[i].revents);
  }

  return 0;
}
