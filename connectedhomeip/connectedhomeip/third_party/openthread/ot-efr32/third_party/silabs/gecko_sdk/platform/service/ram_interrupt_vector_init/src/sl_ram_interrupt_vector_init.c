/***************************************************************************//**
 * @file
 * @brief RAM interrupt vector initialization implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_common.h"
#include "em_core.h"
#include "stddef.h"

#define IRQ_TABLE_SIZE (EXT_IRQ_COUNT + 16)
//if table is larger than 256 bytes, align to 512
#if IRQ_TABLE_SIZE > 64
#define IRQ_TABLE_ALIGNMENT 512
#else
#define IRQ_TABLE_ALIGNMENT 256
#endif

typedef void (*vectors_irq_func_ptr)(void);

__ALIGNED(IRQ_TABLE_ALIGNMENT) vectors_irq_func_ptr gecko_vector_table[IRQ_TABLE_SIZE] = { 0 };

void sl_ram_interrupt_vector_init(void)
{
  vectors_irq_func_ptr *user_vector_table = (vectors_irq_func_ptr*)SCB->VTOR;

  //Initialize new table in RAM if not already initialized
  if (((uint32_t)user_vector_table < RAM_MEM_BASE)
      || ((uint32_t)user_vector_table >= (RAM_MEM_BASE + RAM_MEM_SIZE))) {
    CORE_InitNvicVectorTable((uint32_t*)user_vector_table, IRQ_TABLE_SIZE, (uint32_t*) gecko_vector_table, IRQ_TABLE_SIZE, NULL, false);
  }
}
