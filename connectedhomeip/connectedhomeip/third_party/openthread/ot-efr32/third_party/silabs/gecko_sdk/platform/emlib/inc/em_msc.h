/***************************************************************************//**
 * @file
 * @brief Flash Controller (MSC) Peripheral API
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

#ifndef EM_MSC_H
#define EM_MSC_H

#include "em_device.h"
#if defined(MSC_COUNT) && (MSC_COUNT > 0)

#include <stdint.h>
#include <stdbool.h>
#include "em_bus.h"
#include "em_msc_compat.h"
#include "em_ramfunc.h"
#include "sl_assert.h"

#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  #include "sli_tz_ns_interface.h"
  #include "sli_tz_service_msc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup msc MSC - Memory System Controller
 * @brief Memory System Controller API
 * @details
 *  Contains functions to control the MSC, primarily the Flash.
 *  Users can perform Flash memory write and erase operations, as well as
 *  optimization of the CPU instruction fetch interface for the application.
 *  Available instruction fetch features depends on the MCU or SoC family, but
 *  features such as instruction pre-fetch, cache, and configurable branch prediction
 *  are typically available.
 *
 * @note Flash wait-state configuration is handled by @ref cmu.
 *       When core clock configuration is changed by a call to functions such as
 *       CMU_ClockSelectSet() or CMU_HFRCOBandSet(), then Flash wait-state
 *       configuration is also updated.
 *
 *  MSC resets into a safe state. To initialize the instruction interface
 *  to recommended settings:
 *  @include em_msc_init_exec.c
 *
 * @note The optimal configuration is highly application dependent. Performance
 *       benchmarking is supported by most families. See MSC_StartCacheMeasurement()
 *       and MSC_GetCacheMeasurement() for more details.
 *
 * @note
 *   The flash write and erase runs from RAM on the EFM32G devices. On all other
 *   devices the flash write and erase functions run from flash.
 *
 * @note
 *   Flash erase may add ms of delay to interrupt latency if executing from Flash.
 *
 * Flash write and erase operations are supported by @ref MSC_WriteWord(),
 * @ref MSC_ErasePage(), and MSC_MassErase().
 * Mass erase is supported for MCU and SoC families with larger Flash sizes.
 *
 * @note
 *  @ref MSC_Init() must be called prior to any Flash write or erase operation.
 *
 *  The following steps are necessary to perform a page erase and write:
 *  @include em_msc_erase_write.c
 *
 * @deprecated
 *   The configuration called EM_MSC_RUN_FROM_FLASH is deprecated. This was
 *   previously used for allocating the flash write functions in either flash
 *   or RAM.
 *
 * @note
 *   The configuration EM_MSC_RUN_FROM_RAM is used for allocating the flash
 *   write functions in FLASH and RAM respectively. By default, flash write
 *   functions are placed in RAM on EFM32G and Series 2 devices
 *   automatically and that could not be changed. For other devices,
 *   flash write functions are placed in FLASH but that could be changed using
 *   EM_MSC_RUN_FROM_RAM.
 *
 * @deprecated
 *   The function called MSC_WriteWordFast() is deprecated.
 *
 * @{
 ******************************************************************************/

/*******************************************************************************
 *************************   DEFINES   *****************************************
 ******************************************************************************/

/**
 * @brief
 *    Timeout used while waiting for Flash to become ready after a write.
 *    This number indicates the number of iterations to perform before
 *    issuing a timeout.
 *
 * @note
 *    Timeout is set very large (in the order of 100x longer than
 *    necessary). This is to avoid any corner case.
 */
#define MSC_PROGRAM_TIMEOUT    10000000UL

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if (defined(_EFM32_GECKO_FAMILY)        \
  || defined(_SILICON_LABS_32B_SERIES_2) \
  || defined(EM_MSC_RUN_FROM_RAM))       \
  && !defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
#define MSC_RAMFUNC_DECLARATOR          SL_RAMFUNC_DECLARATOR
#define MSC_RAMFUNC_DEFINITION_BEGIN    SL_RAMFUNC_DEFINITION_BEGIN
#define MSC_RAMFUNC_DEFINITION_END      SL_RAMFUNC_DEFINITION_END
#else
#define MSC_RAMFUNC_DECLARATOR
#define MSC_RAMFUNC_DEFINITION_BEGIN
#define MSC_RAMFUNC_DEFINITION_END
#endif
/** @endcond */

/*******************************************************************************
 *************************   TYPEDEFS   ****************************************
 ******************************************************************************/

/** Return codes for writing/erasing Flash. */
typedef enum {
  mscReturnOk          =  0, /**< Flash write/erase successful. */
  mscReturnInvalidAddr = -1, /**< Invalid address. Write to an address that is not Flash. */
  mscReturnLocked      = -2, /**< Flash address is locked. */
  mscReturnTimeOut     = -3, /**< Timeout while writing to Flash. */
  mscReturnUnaligned   = -4  /**< Unaligned access to Flash. */
} MSC_Status_TypeDef;

#if defined(_MSC_READCTRL_BUSSTRATEGY_MASK)
/** Strategy for prioritized bus access. */
typedef enum {
  mscBusStrategyCPU = MSC_READCTRL_BUSSTRATEGY_CPU,       /**< Prioritize CPU bus accesses. */
  mscBusStrategyDMA = MSC_READCTRL_BUSSTRATEGY_DMA,       /**< Prioritize DMA bus accesses. */
  mscBusStrategyDMAEM1 = MSC_READCTRL_BUSSTRATEGY_DMAEM1, /**< Prioritize DMAEM1 for bus accesses. */
  mscBusStrategyNone = MSC_READCTRL_BUSSTRATEGY_NONE      /**< No unit has bus priority. */
} MSC_BusStrategy_Typedef;
#endif

#if defined(_SYSCFG_DMEM0PORTMAPSEL_MASK)
/** AHBHOST masters that can use alternate MPAHBRAM ports. */
typedef enum {
  mscDmemMasterLDMA    = _SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_SHIFT,
  mscDmemMasterSRWAES  = _SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_SHIFT,
  mscDmemMasterAHBSRW  = _SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_SHIFT,
  mscDmemMasterSRWECA0 = _SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_SHIFT,
  mscDmemMasterSRWECA1 = _SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_SHIFT,
#if defined(_SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA0PORTSEL_MASK)
  mscDmemMasterMVPAHBDATA0 = _SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA0PORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA1PORTSEL_MASK)
  mscDmemMasterMVPAHBDATA1 = _SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA1PORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA2PORTSEL_MASK)
  mscDmemMasterMVPAHBDATA2 = _SYSCFG_DMEM0PORTMAPSEL_MVPAHBDATA2PORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_LDMA1PORTSEL_MASK)
  mscDmemMasterLDMA1   = _SYSCFG_DMEM0PORTMAPSEL_LDMA1PORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_SRWLDMAPORTSEL_MASK)
  mscDmemMasterSRWLDMA = _SYSCFG_DMEM0PORTMAPSEL_SRWLDMAPORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_USBPORTSEL_MASK)
  mscDmemMasterUSB     = _SYSCFG_DMEM0PORTMAPSEL_USBPORTSEL_SHIFT,
#endif
#if defined(_SYSCFG_DMEM0PORTMAPSEL_BUFCPORTSEL_MASK)
  mscDmemMasterBUFC    = _SYSCFG_DMEM0PORTMAPSEL_BUFCPORTSEL_SHIFT
#endif
} MSC_DmemMaster_TypeDef;
#endif

#if defined(_MPAHBRAM_CTRL_AHBPORTPRIORITY_MASK)
/** AHB port given priority. */
typedef enum {
  mscPortPriorityNone  = _MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE,
  mscPortPriorityPort0 = _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0,
  mscPortPriorityPort1 = _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1,
#if defined(_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT2)
  mscPortPriorityPort2 = _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT2,
#endif
#if defined(_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT3)
  mscPortPriorityPort3 = _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT3,
#endif
} MSC_PortPriority_TypeDef;
#endif

#if defined(MSC_READCTRL_DOUTBUFEN) || defined(MSC_RDATACTRL_DOUTBUFEN)
/** Code execution configuration */
typedef struct {
  bool doutBufEn;       /**< Flash dout pipeline buffer enable */
} MSC_ExecConfig_TypeDef;

/** Default MSC ExecConfig initialization */
#define MSC_EXECCONFIG_DEFAULT \
  {                            \
    false,                     \
  }

#else
/** Code execution configuration. */
typedef struct {
  bool scbtEn;          /**< Enable Suppressed Conditional Branch Target Prefetch. */
  bool prefetchEn;      /**< Enable MSC prefetching. */
  bool ifcDis;          /**< Disable instruction cache. */
  bool aiDis;           /**< Disable automatic cache invalidation on write or erase. */
  bool iccDis;          /**< Disable automatic caching of fetches in interrupt context. */
  bool useHprot;        /**< Use ahb_hprot to determine if the instruction is cacheable or not. */
} MSC_ExecConfig_TypeDef;

/** Default MSC ExecConfig initialization. */
#define MSC_EXECCONFIG_DEFAULT \
  {                            \
    false,                     \
    true,                      \
    false,                     \
    false,                     \
    false,                     \
    false,                     \
  }
#endif

#if defined(_MSC_ECCCTRL_MASK)          \
  || defined(_SYSCFG_DMEM0ECCCTRL_MASK) \
  || defined(_MPAHBRAM_CTRL_MASK)

#if defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
/** EFM32GG11B incorporates 2 memory banks including ECC support. */
#define MSC_ECC_BANKS  (2)

/** Default MSC EccConfig initialization. */
#define MSC_ECCCONFIG_DEFAULT \
  {                           \
    { false, false },         \
    { 0, 1 },                 \
  }

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_106)
/** EFM32GG12B incorporates 3 memory banks including ECC support. */
#define MSC_ECC_BANKS  (3)

/** Default MSC EccConfig initialization. */
#define MSC_ECCCONFIG_DEFAULT \
  {                           \
    { false, false, false },  \
    { 0, 1 },                 \
  }

#elif defined(_SILICON_LABS_32B_SERIES_2)

/** Series 2 chips incorporate 1 memory bank including ECC support. */
#define MSC_ECC_BANKS  (1)
/** Default MSC EccConfig initialization */
#define MSC_ECCCONFIG_DEFAULT \
  {                           \
    { false },                \
    { 0, 1 },                 \
  }

#else
#error Device not supported.
#endif

/** ECC configuration. */
typedef struct {
  bool     enableEccBank[MSC_ECC_BANKS]; /**< Array of bools to enable/disable
                                            Error Correcting Code (ECC) for
                                            each RAM bank that supports ECC on
                                            the device. */
  uint32_t dmaChannels[2];               /**< Array of 2 DMA channel numbers to
                                            use for ECC initialization. */
} MSC_EccConfig_TypeDef;

#endif /* #if defined(_MSC_ECCCTRL_MASK) */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Deprecated type names. */
#define mscBusStrategy_Typedef MSC_BusStrategy_Typedef
#define msc_Return_TypeDef MSC_Status_TypeDef
/** @endcond */

/*******************************************************************************
 *************************   Inline Functions   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get the status of the MSC register lock.
 *
 * @return
 *   Boolean true if register lock is applied, false otherwise.
 ******************************************************************************/
__STATIC_INLINE bool MSC_LockGetLocked(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  return (bool)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_get_locked,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#elif defined(_MSC_STATUS_REGLOCK_MASK)
  return (MSC->STATUS & _MSC_STATUS_REGLOCK_MASK) != MSC_STATUS_REGLOCK_UNLOCKED;
#else
  return (MSC->LOCK & _MSC_LOCK_MASK) != MSC_LOCK_LOCKKEY_UNLOCK;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set the MSC register lock to a locked state.
 ******************************************************************************/
__STATIC_INLINE void MSC_LockSetLocked(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_set_locked,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#else
  MSC->LOCK = MSC_LOCK_LOCKKEY_LOCK;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set the MSC register lock to an unlocked state.
 ******************************************************************************/
__STATIC_INLINE void MSC_LockSetUnlocked(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_set_unlocked,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#else
  MSC->LOCK = MSC_LOCK_LOCKKEY_UNLOCK;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get the current value of the read control register (MSC_READCTRL).
 *
 * @return
 *   The 32-bit value read from the MSC_READCTRL register.
 ******************************************************************************/
__STATIC_INLINE uint32_t MSC_ReadCTRLGet(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  return sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_get_readctrl,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#else
  return MSC->READCTRL;
#endif
}

/***************************************************************************//**
 * @brief
 *   Write a value to the read control register (MSC_READCTRL).
 *
 * @param[in] value
 *   The 32-bit value to write to the MSC_READCTRL register.
 ******************************************************************************/
__STATIC_INLINE void MSC_ReadCTRLSet(uint32_t value)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_set_readctrl,
    value);
#else
  MSC->READCTRL = value;
#endif
}

#if defined(_MSC_PAGELOCK0_MASK) || defined(_MSC_INST_PAGELOCKWORD0_MASK)

/***************************************************************************//**
 * @brief
 *   Set the lockbit for a flash page in order to prevent page writes/erases to
 *   the corresponding page.
 *
 * @param[in] page_number
 *   The index of the page to apply the pagelock to. Must be in the range
 *   [0, (flash_size / page_size) - 1].
 ******************************************************************************/
__STATIC_INLINE void MSC_PageLockSetLocked(uint32_t page_number)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_set_pagelock,
    page_number);
#else
  EFM_ASSERT(page_number < (FLASH_SIZE / FLASH_PAGE_SIZE));

  #if defined(_MSC_PAGELOCK0_MASK)
  uint32_t *pagelock_registers = (uint32_t *)&MSC->PAGELOCK0;
  #elif defined(_MSC_INST_PAGELOCKWORD0_MASK)
  uint32_t *pagelock_registers = (uint32_t *)&MSC->INST_PAGELOCKWORD0;
  #endif

  pagelock_registers[page_number / 32] |= (1 << (page_number % 32));
#endif
}

/***************************************************************************//**
 * @brief
 *   Get the value of the lockbit for a flash page.
 *
 * @param[in] page_number
 *   The index of the page to get the lockbit value from. Must be in the range
 *   [0, (flash_size / page_size) - 1].
 *
 * @return
 *   Boolean true if the page is locked, false otherwise.
 ******************************************************************************/
__STATIC_INLINE bool MSC_PageLockGetLocked(uint32_t page_number)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  return (bool)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_get_pagelock,
    page_number);
#else
  EFM_ASSERT(page_number < (FLASH_SIZE / FLASH_PAGE_SIZE));

  #if defined(_MSC_PAGELOCK0_MASK)
  uint32_t *pagelock_registers = (uint32_t *)&MSC->PAGELOCK0;
  #elif defined(_MSC_INST_PAGELOCKWORD0_MASK)
  uint32_t *pagelock_registers = (uint32_t *)&MSC->INST_PAGELOCKWORD0;
  #endif

  return pagelock_registers[page_number / 32] & (1 << (page_number % 32));
#endif
}

#endif // _MSC_PAGELOCK0_MASK || _MSC_INST_PAGELOCKWORD0_MASK

#if defined(_MSC_USERDATASIZE_MASK)

/***************************************************************************//**
 * @brief
 *   Get the size of the user data region in flash.
 *
 * @return
 *   The size of the user data region divided by 256.
 ******************************************************************************/
__STATIC_INLINE uint32_t MSC_UserDataGetSize(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  return sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_get_userdata_size,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#else
  return MSC->USERDATASIZE;
#endif
}

#endif // _MSC_USERDATASIZE_MASK

#if defined(_MSC_MISCLOCKWORD_MASK)

/***************************************************************************//**
 * @brief
 *   Get the current value of the mass erase and user data page lock word
 *   (MSC_MISCLOCKWORD).
 *
 * @return
 *   The 32-bit value read from the MSC_MISCLOCKWORD register.
 ******************************************************************************/
__STATIC_INLINE uint32_t MSC_MiscLockWordGet(void)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  return sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_get_misclockword,
    SLI_TZ_DISPATCH_UNUSED_ARG);
#else
  return MSC->MISCLOCKWORD;
#endif
}

/***************************************************************************//**
 * @brief
 *   Write a value to the mass erase and user data page lock word
 *   (MSC_MISCLOCKWORD).
 *
 * @param[in] value
 *   The 32-bit value to write to the MSC_MISCLOCKWORD register.
 ******************************************************************************/
__STATIC_INLINE void MSC_MiscLockWordSet(uint32_t value)
{
#if defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_set_misclockword,
    value);
#else
  MSC->MISCLOCKWORD = value;
#endif
}

#endif // _MSC_USERDATASIZE_MASK

#if !defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)

/***************************************************************************//**
 * @brief
 *    Clear one or more pending MSC interrupts.
 *
 * @param[in] flags
 *    Pending MSC interrupt source to clear. Use a bitwise logic OR combination
 *   of valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void MSC_IntClear(uint32_t flags)
{
#if defined(MSC_HAS_SET_CLEAR)
  MSC->IF_CLR = flags;
#else
  MSC->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more MSC interrupts.
 *
 * @param[in] flags
 *   MSC interrupt sources to disable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void MSC_IntDisable(uint32_t flags)
{
#if defined(MSC_HAS_SET_CLEAR)
  MSC->IEN_CLR = flags;
#else
  MSC->IEN &= ~(flags);
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable one or more MSC interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   MSC_IntClear() prior to enabling the interrupt.
 *
 * @param[in] flags
 *   MSC interrupt sources to enable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void MSC_IntEnable(uint32_t flags)
{
#if defined(MSC_HAS_SET_CLEAR)
  MSC->IEN_SET = flags;
#else
  MSC->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get pending MSC interrupt flags.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @return
 *   MSC interrupt sources pending. A bitwise logic OR combination of valid
 *   interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t MSC_IntGet(void)
{
  return MSC->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending MSC interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled MSC interrupt sources.
 *   The return value is the bitwise AND of
 *   - the enabled interrupt sources in MSC_IEN and
 *   - the pending interrupt flags MSC_IF
 ******************************************************************************/
__STATIC_INLINE uint32_t MSC_IntGetEnabled(void)
{
  uint32_t ien;

  ien = MSC->IEN;
  return MSC->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending MSC interrupts from SW.
 *
 * @param[in] flags
 *   MSC interrupt sources to set to pending. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void MSC_IntSet(uint32_t flags)
{
#if defined(MSC_HAS_SET_CLEAR)
  MSC->IF_SET = flags;
#else
  MSC->IFS = flags;
#endif
}

#if defined(MSC_IF_CHOF) && defined(MSC_IF_CMOF)
/***************************************************************************//**
 * @brief
 *   Start measuring  the cache hit ratio.
 * @details
 *   Starts performance counters. It is defined inline to
 *   minimize the impact of this code on the measurement itself.
 ******************************************************************************/
__STATIC_INLINE void MSC_StartCacheMeasurement(void)
{
  /* Clear CMOF and CHOF to catch these later. */
  MSC->IFC = MSC_IF_CHOF | MSC_IF_CMOF;

  /* Start performance counters. */
#if defined(_MSC_CACHECMD_MASK)
  MSC->CACHECMD = MSC_CACHECMD_STARTPC;
#else
  MSC->CMD = MSC_CMD_STARTPC;
#endif
}

/***************************************************************************//**
 * @brief
 *   Stop measuring the hit rate.
 * @note
 *   Defined inline to minimize the impact of this
 *   code on the measurement itself.
 *   Only works for relatively short sections of code.
 *   To measure longer sections of code, implement an IRQ Handler for
 *   the CHOF and CMOF overflow interrupts. These overflows need to be
 *   counted and included in the total.
 *   Functions can then be implemented as follows:
 * @verbatim
 * volatile uint32_t hitOverflows
 * volatile uint32_t missOverflows
 *
 * void MSC_IRQHandler(void)
 * {
 *   uint32_t flags;
 *   flags = MSC->IF;
 *   if (flags & MSC_IF_CHOF) {
 *      MSC->IFC = MSC_IF_CHOF;
 *      hitOverflows++;
 *   }
 *   if (flags & MSC_IF_CMOF) {
 *     MSC->IFC = MSC_IF_CMOF;
 *     missOverflows++;
 *   }
 * }
 *
 * void startPerformanceCounters(void)
 * {
 *   hitOverflows = 0;
 *   missOverflows = 0;
 *
 *   MSC_IntEnable(MSC_IF_CHOF | MSC_IF_CMOF);
 *   NVIC_EnableIRQ(MSC_IRQn);
 *
 *   MSC_StartCacheMeasurement();
 * }
 * @endverbatim
 * @return
 *   Returns -1 if there has been no cache accesses.
 *   Returns -2 if there has been an overflow in the performance counters.
 *   If not, it will return the percentage of hits versus misses.
 ******************************************************************************/
__STATIC_INLINE int32_t MSC_GetCacheMeasurement(void)
{
  int32_t total;
  int32_t hits;
  /* Stop counter before computing hit-rate. */
#if defined(_MSC_CACHECMD_MASK)
  MSC->CACHECMD = MSC_CACHECMD_STOPPC;
#else
  MSC->CMD = MSC_CMD_STOPPC;
#endif

  /* Check for overflows in performance counters. */
  if (MSC->IF & (MSC_IF_CHOF | MSC_IF_CMOF)) {
    return -2;
  }

  hits  = (int32_t)MSC->CACHEHITS;
  total = (int32_t)MSC->CACHEMISSES + hits;

  /* To avoid a division by zero. */
  if (total == 0) {
    return -1;
  }

  return (hits * 100) / total;
}

/***************************************************************************//**
 * @brief
 *   Flush contents of instruction cache.
 ******************************************************************************/
__STATIC_INLINE void MSC_FlushCache(void)
{
#if defined(_MSC_CACHECMD_MASK)
  MSC->CACHECMD = MSC_CACHECMD_INVCACHE;
#else
  MSC->CMD = MSC_CMD_INVCACHE;
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache functionality.
 * @param[in] enable
 *   Enable instruction cache. Default is on.
 ******************************************************************************/
__STATIC_INLINE void MSC_EnableCache(bool enable)
{
  BUS_RegBitWrite(&(MSC->READCTRL), _MSC_READCTRL_IFCDIS_SHIFT, !enable);
}

#if defined(MSC_READCTRL_ICCDIS)
/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache functionality in IRQs.
 * @param[in] enable
 *   Enable instruction cache. Default is on.
 ******************************************************************************/
__STATIC_INLINE void MSC_EnableCacheIRQs(bool enable)
{
  BUS_RegBitWrite(&(MSC->READCTRL), _MSC_READCTRL_ICCDIS_SHIFT, !enable);
}
#endif

/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache flushing when writing to flash.
 * @param[in] enable
 *   Enable automatic cache flushing. Default is on.
 ******************************************************************************/
__STATIC_INLINE void MSC_EnableAutoCacheFlush(bool enable)
{
  BUS_RegBitWrite(&(MSC->READCTRL), _MSC_READCTRL_AIDIS_SHIFT, !enable);
}
#endif /* defined( MSC_IF_CHOF ) && defined( MSC_IF_CMOF ) */

#if defined(_MSC_READCTRL_BUSSTRATEGY_MASK)
/***************************************************************************//**
 * @brief
 *   Configure which unit should get priority on system bus.
 * @param[in] mode
 *   Unit to prioritize bus accesses for.
 ******************************************************************************/
__STATIC_INLINE void MSC_BusStrategy(mscBusStrategy_Typedef mode)
{
  MSC->READCTRL = (MSC->READCTRL & ~(_MSC_READCTRL_BUSSTRATEGY_MASK)) | mode;
}
#endif

/*******************************************************************************
 *************************   PROTOTYPES   **************************************
 ******************************************************************************/

void MSC_ExecConfigSet(MSC_ExecConfig_TypeDef *execConfig);
#if defined(_MSC_ECCCTRL_MASK)          \
  || defined(_SYSCFG_DMEM0ECCCTRL_MASK) \
  || defined(_MPAHBRAM_CTRL_MASK)
void MSC_EccConfigSet(MSC_EccConfig_TypeDef *eccConfig);
#endif

#if defined(_SYSCFG_DMEM0PORTMAPSEL_MASK)
void MSC_DmemPortMapSet(MSC_DmemMaster_TypeDef master, uint8_t port);
#endif

#if defined(_MPAHBRAM_CTRL_AHBPORTPRIORITY_MASK)
void MSC_PortSetPriority(MSC_PortPriority_TypeDef portPriority);
MSC_PortPriority_TypeDef MSC_PortGetCurrentPriority(void);
#endif

#if !defined(_SILICON_LABS_32B_SERIES_2)
/* Note that this function is deprecated because we no longer support
 * placing msc code in ram. */
MSC_RAMFUNC_DECLARATOR
MSC_Status_TypeDef MSC_WriteWordFast(uint32_t *address,
                                     void const *data,
                                     uint32_t numBytes);
#endif

#if defined(MSC_WRITECMD_ERASEMAIN0)
/***************************************************************************//**
 * @brief
 *   Erase the entire Flash in one operation.
 *
 * @note
 *   This command will erase the entire contents of the device.
 *   Use with care, both a debug session and all contents of the flash will be
 *   lost. The lock bit, MLW will prevent this operation from executing and
 *   might prevent a successful mass erase.
 *
 * @return
 *   Returns the status of the operation.
 ******************************************************************************/
SL_RAMFUNC_DECLARATOR
MSC_Status_TypeDef MSC_MassErase(void);
#endif

#endif /* !SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT */

MSC_RAMFUNC_DECLARATOR
MSC_Status_TypeDef MSC_ErasePage(uint32_t *startAddress);

MSC_RAMFUNC_DECLARATOR
MSC_Status_TypeDef MSC_WriteWord(uint32_t *address,
                                 void const *data,
                                 uint32_t numBytes);

#if (_SILICON_LABS_32B_SERIES > 0)
MSC_Status_TypeDef MSC_WriteWordDma(int ch,
                                    uint32_t *address,
                                    const void *data,
                                    uint32_t numBytes);
#endif

void MSC_Init(void);
void MSC_Deinit(void);

/** @} (end addtogroup msc) */

#ifdef __cplusplus
}
#endif

#endif /* defined(MSC_COUNT) && (MSC_COUNT > 0) */
#endif /* EM_MSC_H */
