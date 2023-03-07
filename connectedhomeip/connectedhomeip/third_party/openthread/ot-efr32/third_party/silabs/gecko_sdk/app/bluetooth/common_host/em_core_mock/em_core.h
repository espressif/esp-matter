/***************************************************************************//**
 * @file
 * @brief Core interrupt handling API implementation with empty macros
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef EM_CORE_H
#define EM_CORE_H

#include <stdbool.h>
#include <stdint.h>

#define EXT_IRQ_COUNT               0

/***************************************************************************//**
 * @addtogroup core
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** Use PRIMASK register to disable interrupts in ATOMIC sections. */
#define CORE_ATOMIC_METHOD_PRIMASK  0

/** Use BASEPRI register to disable interrupts in ATOMIC sections. */
#define CORE_ATOMIC_METHOD_BASEPRI  1

/** Number of words in a NVIC mask set. */
#define CORE_NVIC_REG_WORDS   ((EXT_IRQ_COUNT + 31) / 32)

/** Number of entries in a default interrupt vector table. */
#define CORE_DEFAULT_VECTOR_TABLE_ENTRIES   (EXT_IRQ_COUNT + 16)

/** Highest priority for core interrupt. */
#define CORE_INTERRUPT_HIGHEST_PRIORITY 0

/** Default priority for core interrupt. */
#define CORE_INTERRUPT_DEFAULT_PRIORITY 5

/** Lowest priority for core interrupt. */
#define CORE_INTERRUPT_LOWEST_PRIORITY 7

#if !defined(CORE_ATOMIC_BASE_PRIORITY_LEVEL)
/** The interrupt priority level disabled within ATOMIC regions. Interrupts
 *  with priority level equal to or lower than this definition will be disabled
 *  within ATOMIC regions. */
#define CORE_ATOMIC_BASE_PRIORITY_LEVEL  3
#endif

#define CORE_ATOMIC_METHOD    CORE_ATOMIC_METHOD_PRIMASK

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ************************   MACRO API   ***************************************
 ******************************************************************************/

//
//  CRITICAL section macro API.
//

/** Allocate storage for PRIMASK or BASEPRI value for use by
 *  CORE_ENTER/EXIT_ATOMIC() and CORE_ENTER/EXIT_CRITICAL() macros. */
#define CORE_DECLARE_IRQ_STATE

/** CRITICAL style interrupt disable. */
#define CORE_CRITICAL_IRQ_DISABLE()

/** CRITICAL style interrupt enable. */
#define CORE_CRITICAL_IRQ_ENABLE()

/** Convenience macro for implementing a CRITICAL section. */
#define CORE_CRITICAL_SECTION(yourcode) \
  {                                     \
    CORE_DECLARE_IRQ_STATE;             \
    CORE_ENTER_CRITICAL();              \
    {                                   \
      yourcode                          \
    }                                   \
    CORE_EXIT_CRITICAL();               \
  }

/** Enter CRITICAL section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_ENTER_CRITICAL()
/** Exit CRITICAL section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_EXIT_CRITICAL()
/** CRITICAL style yield. */
#define CORE_YIELD_CRITICAL()

//
//  ATOMIC section macro API.
//

/** ATOMIC style interrupt disable. */
#define CORE_ATOMIC_IRQ_DISABLE()

/** ATOMIC style interrupt enable. */
#define CORE_ATOMIC_IRQ_ENABLE()

/** Convenience macro for implementing an ATOMIC section. */
#define CORE_ATOMIC_SECTION(yourcode) \
  {                                   \
    CORE_DECLARE_IRQ_STATE;           \
    CORE_ENTER_ATOMIC();              \
    {                                 \
      yourcode                        \
    }                                 \
    CORE_EXIT_ATOMIC();               \
  }

/** Enter ATOMIC section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_ENTER_ATOMIC()

/** Exit ATOMIC section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_EXIT_ATOMIC()

/** ATOMIC style yield. */
#define CORE_YIELD_ATOMIC()

//
//  NVIC mask section macro API.
//

/** Allocate storage for NVIC interrupt masks for use by
 *  CORE_ENTER/EXIT_NVIC() macros. */
#define CORE_DECLARE_NVIC_STATE

/** Allocate storage for NVIC interrupt masks.
 *  @param[in] x
 *    The storage variable name to use.*/
#define CORE_DECLARE_NVIC_MASK(x)

/** Allocate storage for and zero initialize NVIC interrupt mask.
 *  @param[in] x
 *    The storage variable name to use.*/
#define CORE_DECLARE_NVIC_ZEROMASK(x)

/** NVIC mask style interrupt disable.
 *  @param[in] mask
 *    Mask specifying which NVIC interrupts to disable. */
#define CORE_NVIC_DISABLE(mask)

/** NVIC mask style interrupt enable.
 *  @param[in] mask
 *    Mask specifying which NVIC interrupts to enable. */
#define CORE_NVIC_ENABLE(mask)

/** Convenience macro for implementing a NVIC mask section.
 *  @param[in] mask
 *    Mask specifying which NVIC interrupts to disable within the section.
 *  @param[in] yourcode
 *    The code for the section. */
#define CORE_NVIC_SECTION(mask, yourcode) \
  {                                       \
    CORE_DECLARE_NVIC_STATE;              \
    CORE_ENTER_NVIC(mask);                \
    {                                     \
      yourcode                            \
    }                                     \
    CORE_EXIT_NVIC();                     \
  }

/** Enter NVIC mask section. Assumes that a @ref CORE_DECLARE_NVIC_STATE exist
 *  in scope.
 *  @param[in] disable
 *    Mask specifying which NVIC interrupts to disable within the section. */
#define CORE_ENTER_NVIC(disable)

/** Exit NVIC mask section. Assumes that a @ref CORE_DECLARE_NVIC_STATE exist
 *  in scope. */
#define CORE_EXIT_NVIC()

/** NVIC maks style yield.
 * @param[in] enable
 *   Mask specifying which NVIC interrupts to briefly enable. */
#define CORE_YIELD_NVIC(enable)

//
//  Miscellaneous macros.
//

/** Check if IRQ is disabled. */
#define CORE_IRQ_DISABLED()

/** Check if inside an IRQ handler. */
#define CORE_IN_IRQ_CONTEXT()

/** Start counter. */
#define START_COUNTER(handle)
/** Stop counter. */
#define STOP_COUNTER(handle)

/*******************************************************************************
 *************************   TYPEDEFS   ****************************************
 ******************************************************************************/

/** Storage for PRIMASK or BASEPRI value. */
typedef uint32_t CORE_irqState_t;

/** Storage for NVIC interrupt masks. */
typedef struct {
  uint32_t a[CORE_NVIC_REG_WORDS];    /*!< Array of NVIC mask words. */
} CORE_nvicMask_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

#define CORE_CriticalDisableIrq(...)
#define CORE_CriticalEnableIrq(...)
#define CORE_ExitCritical(...)
#define CORE_YieldCritical(...)
#define CORE_EnterCritical(...)

#define CORE_AtomicDisableIrq(...)
#define CORE_AtomicEnableIrq(...)
#define CORE_ExitAtomic(...)
#define CORE_YieldAtomic(...)
#define CORE_EnterAtomic(...)

#define CORE_InIrqContext(...)
#define CORE_IrqIsBlocked(...)
#define CORE_IrqIsDisabled(...)

#define CORE_GetNvicEnabledMask(...)
#define CORE_GetNvicMaskDisableState(...)

#define CORE_EnterNvicMask(...)
#define CORE_NvicDisableMask(...)
#define CORE_NvicEnableMask(...)
#define CORE_YieldNvicMask(...)
#define CORE_NvicMaskSetIRQ(...)
#define CORE_NvicMaskClearIRQ(...)
#define CORE_NvicIRQDisabled(...)

#define CORE_GetNvicRamTableHandler(...)
#define CORE_SetNvicRamTableHandler(...)
#define CORE_InitNvicVectorTable(...)

#define CORE_get_max_time_critical_section(...)
#define CORE_get_max_time_atomic_section(...)

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup core) */

#endif /* EM_CORE_H */
