/***************************************************************************//**
 * @file
 * @brief Microsecond delay.
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

    .text
    .align 4
    .syntax unified
    .thumb_func
    .global  sli_delay_loop

/*
 * @brief
 *   Hardware delay loop
 *
 * @detail
 *   This is the hardware specific delay loop. It is designed specifically to
 *   execute in 4 or 3 cycles for each iteration depending on the architecture.
 *   Using this information the caller can use the core clock frequency to
 *   calculate the number of loops required in order to delay a specific time
 *   period.
 * 
 * @param[in] n (r0)
 *   n is the number of loops to execute. Each loop will execute in 4 cycles.
 *   Note that we assume that r0 > 0, so this invariant should be checked by
 *   the caller.
 */
sli_delay_loop:
    subs  r0, r0, #1
    beq   done
    b.n   sli_delay_loop
done:
    bx    lr

    .end
