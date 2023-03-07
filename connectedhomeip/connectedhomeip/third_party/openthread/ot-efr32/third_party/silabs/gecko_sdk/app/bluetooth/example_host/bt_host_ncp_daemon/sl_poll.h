/***************************************************************************//**
 * @file
 * @brief sl_poll.h
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

#ifndef SL_POLL_H
#define SL_POLL_H

//max number of descriptors to poll
#define SL_POLL_MAX 10

int sl_poll_init();
int sl_poll_wait(int timeout);
int sl_poll_update();
int sl_poll_add(int fd);
void sl_poll_remove(int fd);
int sl_poll_cb(int fd, int revents);
int sl_poll_is_polled(int fd);

#endif
