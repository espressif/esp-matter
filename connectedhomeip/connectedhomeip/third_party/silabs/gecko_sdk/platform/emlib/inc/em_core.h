/***************************************************************************//**
 * @file
 * @brief Core interrupt handling API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"
#include "sl_common.h"

#include <stdbool.h>

#if defined(EMLIB_USER_CONFIG)
#include "emlib_config.h"
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_EMLIB_CORE_DEBUG_CONFIG_PRESENT)
#include "emlib_core_debug_config.h"
#endif

#if !defined(SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING)
#define SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING   0
#endif

#if (SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING == 1)
#include "sl_cycle_counter.h"
#endif

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

// Interrupt priorities based on processor architecture
#if defined(__CM3_REV) || defined(__CM4_REV) || defined(__CM7_REV) \
  || defined(__CM23_REV) || defined(__CM33_REV)

/** Highest priority for core interrupt. */
#define CORE_INTERRUPT_HIGHEST_PRIORITY 0

/** Default priority for core interrupt. */
#define CORE_INTERRUPT_DEFAULT_PRIORITY 5

/** Lowest priority for core interrupt. */
#define CORE_INTERRUPT_LOWEST_PRIORITY 7

/** Default method to disable interrupts in ATOMIC sections. */
#define CORE_ATOMIC_METHOD_DEFAULT  CORE_ATOMIC_METHOD_BASEPRI
#elif defined(__CM0_REV) || defined(__CM0PLUS_REV)

/** Highest priority for core interrupt. */
#define CORE_INTERRUPT_HIGHEST_PRIORITY 0

/** Default priority for core interrupt. */
#define CORE_INTERRUPT_DEFAULT_PRIORITY 1

/** Lowest priority for core interrupt. */
#define CORE_INTERRUPT_LOWEST_PRIORITY 3

/** Default method to disable interrupts in ATOMIC sections. */
#define CORE_ATOMIC_METHOD_DEFAULT  CORE_ATOMIC_METHOD_PRIMASK
#endif

#if !defined(CORE_ATOMIC_BASE_PRIORITY_LEVEL)
/** The interrupt priority level disabled within ATOMIC regions. Interrupts
 *  with priority level equal to or lower than this definition will be disabled
 *  within ATOMIC regions. */
#define CORE_ATOMIC_BASE_PRIORITY_LEVEL  3
#endif

#if !defined(CORE_ATOMIC_METHOD)
/** Specify which method to use when implementing ATOMIC sections. You can
 *  select between BASEPRI or PRIMASK method.
 *  @note On Cortex-M0+ devices only PRIMASK can be used. */
#if !defined(SL_CATALOG_DEVICE_INIT_NVIC_PRESENT)
#define CORE_ATOMIC_METHOD    CORE_ATOMIC_METHOD_PRIMASK
#else
#define CORE_ATOMIC_METHOD   CORE_ATOMIC_METHOD_DEFAULT
#endif
#endif

// Compile time sanity check.
#if (CORE_NVIC_REG_WORDS > 3)
#error "em_core: Unexpected NVIC external interrupt count."
#endif

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
#define CORE_DECLARE_IRQ_STATE        CORE_irqState_t irqState

/** CRITICAL style interrupt disable. */
#define CORE_CRITICAL_IRQ_DISABLE() CORE_CriticalDisableIrq()

/** CRITICAL style interrupt enable. */
#define CORE_CRITICAL_IRQ_ENABLE()  CORE_CriticalEnableIrq()

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
#define CORE_ENTER_CRITICAL()   irqState = CORE_EnterCritical()

/** Exit CRITICAL section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_EXIT_CRITICAL()    CORE_ExitCritical(irqState)

/** CRITICAL style yield. */
#define CORE_YIELD_CRITICAL()   CORE_YieldCritical()

//
//  ATOMIC section macro API.
//

/** ATOMIC style interrupt disable. */
#define CORE_ATOMIC_IRQ_DISABLE()   CORE_AtomicDisableIrq()

/** ATOMIC style interrupt enable. */
#define CORE_ATOMIC_IRQ_ENABLE()    CORE_AtomicEnableIrq()

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
#define CORE_ENTER_ATOMIC()   irqState = CORE_EnterAtomic()

/** Exit ATOMIC section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
 *  scope. */
#define CORE_EXIT_ATOMIC()    CORE_ExitAtomic(irqState)

/** ATOMIC style yield. */
#define CORE_YIELD_ATOMIC()   CORE_YieldAtomic()

//
//  NVIC mask section macro API.
//

/** Allocate storage for NVIC interrupt masks for use by
 *  CORE_ENTER/EXIT_NVIC() macros. */
#define CORE_DECLARE_NVIC_STATE       CORE_nvicMask_t nvicState

/** Allocate storage for NVIC interrupt masks.
 *  @param[in] x
 *    The storage variable name to use.*/
#define CORE_DECLARE_NVIC_MASK(x)     CORE_nvicMask_t x

/** Allocate storage for and zero initialize NVIC interrupt mask.
 *  @param[in] x
 *    The storage variable name to use.*/
#define CORE_DECLARE_NVIC_ZEROMASK(x) CORE_nvicMask_t x = { { 0 } }

/** NVIC mask style interrupt disable.
 *  @param[in] mask
 *    Mask specifying which NVIC interrupts to disable. */
#define CORE_NVIC_DISABLE(mask)   CORE_NvicDisableMask(mask)

/** NVIC mask style interrupt enable.
 *  @param[in] mask
 *    Mask specifying which NVIC interrupts to enable. */
#define CORE_NVIC_ENABLE(mask)    CORE_NvicEnableMask(mask)

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
#define CORE_ENTER_NVIC(disable)  CORE_EnterNvicMask(&nvicState, disable)

/** Exit NVIC mask section. Assumes that a @ref CORE_DECLARE_NVIC_STATE exist
 *  in scope. */
#define CORE_EXIT_NVIC()          CORE_NvicEnableMask(&nvicState)

/** NVIC maks style yield.
 * @param[in] enable
 *   Mask specifying which NVIC interrupts to briefly enable. */
#define CORE_YIELD_NVIC(enable)   CORE_YieldNvicMask(enable)

//
//  Miscellaneous macros.
//

/** Check if IRQ is disabled. */
#define CORE_IRQ_DISABLED()       CORE_IrqIsDisabled()

/** Check if inside an IRQ handler. */
#define CORE_IN_IRQ_CONTEXT()     CORE_InIrqContext()

// Support for cycle counter
#if (SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING == 1)
/** Start counter. */
#define START_COUNTER(handle) sl_cycle_counter_start(handle)
/** Stop counter. */
#define STOP_COUNTER(handle) sl_cycle_counter_stop(handle)
#else
/** Start counter. */
#define START_COUNTER(handle)
/** Stop counter. */
#define STOP_COUNTER(handle)
#endif

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

void CORE_CriticalDisableIrq(void);
void CORE_CriticalEnableIrq(void);
void CORE_ExitCritical(CORE_irqState_t irqState);
void CORE_YieldCritical(void);
CORE_irqState_t CORE_EnterCritical(void);

void  CORE_AtomicDisableIrq(void);
void  CORE_AtomicEnableIrq(void);
void  CORE_ExitAtomic(CORE_irqState_t irqState);
void  CORE_YieldAtomic(void);
CORE_irqState_t CORE_EnterAtomic(void);

bool  CORE_InIrqContext(void);
bool  CORE_IrqIsBlocked(IRQn_Type irqN);
bool  CORE_IrqIsDisabled(void);

void  CORE_GetNvicEnabledMask(CORE_nvicMask_t *mask);
bool  CORE_GetNvicMaskDisableState(const CORE_nvicMask_t *mask);

void  CORE_EnterNvicMask(CORE_nvicMask_t *nvicState,
                         const CORE_nvicMask_t *disable);
void  CORE_NvicDisableMask(const CORE_nvicMask_t *disable);
void  CORE_NvicEnableMask(const CORE_nvicMask_t *enable);
void  CORE_YieldNvicMask(const CORE_nvicMask_t *enable);
void  CORE_NvicMaskSetIRQ(IRQn_Type irqN, CORE_nvicMask_t *mask);
void  CORE_NvicMaskClearIRQ(IRQn_Type irqN, CORE_nvicMask_t *mask);
bool  CORE_NvicIRQDisabled(IRQn_Type irqN);

void *CORE_GetNvicRamTableHandler(IRQn_Type irqN);
void  CORE_SetNvicRamTableHandler(IRQn_Type irqN, void *handler);
void  CORE_InitNvicVectorTable(uint32_t *sourceTable,
                               uint32_t sourceSize,
                               uint32_t *targetTable,
                               uint32_t targetSize,
                               void *defaultHandler,
                               bool overwriteActive);

#if (SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING == 1)
uint32_t CORE_get_max_time_critical_section(void);
uint32_t CORE_get_max_time_atomic_section(void);
#endif

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup core) */

#endif /* EM_CORE_H */
