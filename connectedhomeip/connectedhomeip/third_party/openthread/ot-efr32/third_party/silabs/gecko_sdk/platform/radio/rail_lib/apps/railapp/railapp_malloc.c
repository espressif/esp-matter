/***************************************************************************//**
 * @file
 * @brief Header file for RAIL error rate functionality
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

#include "rail.h"
#include "em_core.h"

static volatile bool mallocLock = false;
#define RAIL_HEAP_SIZE 0x1400
__ALIGNED(4) uint8_t railMallocBuffer[RAIL_HEAP_SIZE];

void *RAILAPP_Malloc(uint32_t size)
{
  CORE_DECLARE_IRQ_STATE;
  void *buffer;
  CORE_ENTER_CRITICAL();
  if (mallocLock || size > sizeof(railMallocBuffer)) {
    buffer = NULL;
  } else {
    mallocLock = true;
    buffer = railMallocBuffer;
  }
  CORE_EXIT_CRITICAL();
  return buffer;
}

void RAILAPP_Free(void * buffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  if (buffer == railMallocBuffer) {
    mallocLock = false;
  }
  CORE_EXIT_CRITICAL();
}
