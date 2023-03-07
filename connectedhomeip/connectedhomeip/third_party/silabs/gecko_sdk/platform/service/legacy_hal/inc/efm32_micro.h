/***************************************************************************//**
 * @file
 * @brief Utility and convenience functions for EFM32 microcontroller.
 * See @ref micro for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup micro
 * See also hal/micro/cortexm3/micro.h for source code.
 *@{
 */

#ifndef __EFM_MICRO_H__
#define __EFM_MICRO_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef __MICRO_H__
#error do not include this file directly - include micro/micro.h
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
//This is necessary here because halSetRadioHoldOff returns an
//EmberStatus and not adding this typedef to this file breaks a
//whole lot of builds due to include mis-ordering
typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__
#endif // DOXYGEN_SHOULD_SKIP_THIS

#include "em_device.h"

// Define the priority registers of system handlers.

// For Cortex-M faults and exceptions
#define HANDLER_PRIORITY_REGISTER(handler) \
  (*(((uint8_t *)SCS_SHPR_7to4_ADDR) + handler##_VECTOR_INDEX - 4))

// The reset types have both a base type and an extended type.  The extended
//  type is a 16-bit value which has the base type in the upper 8-bits, and the
//  extended classification in the lower 8-bits
// For backwards compatibility with other platforms, only the base type is
//  returned by the halGetResetInfo() API.  For the full extended type, the
//  halGetExtendedResetInfo() API should be called.

#define RESET_BASE_TYPE(extendedType)   ((uint8_t)(((extendedType) >> 8) & 0xFF))
#define RESET_EXTENDED_FIELD(extendedType) ((uint8_t)((extendedType) & 0xFF))
#define RESET_VALID_SIGNATURE           (0xF00F)
#define RESET_INVALID_SIGNATURE         (0xC33C)

// Define the base reset cause types
#define RESET_BASE_DEF(basename, value, string)  RESET_##basename = value,
#define RESET_EXT_DEF(basename, extname, extvalue, string)     /*nothing*/
enum {
  #include "reset-def.h"
  NUM_RESET_BASE_TYPES
};

#undef RESET_BASE_DEF
#undef RESET_EXT_DEF

// Define the extended reset cause types
#define RESET_EXT_VALUE(basename, extvalue) \
  (((RESET_##basename) << 8) + extvalue)
#define RESET_BASE_DEF(basename, value, string)    /*nothing*/
#define RESET_EXT_DEF(basename, extname, extvalue, string) \
  RESET_##basename##_##extname = RESET_EXT_VALUE(basename, extvalue),
enum {
  #include "reset-def.h"
};

#undef RESET_EXT_VALUE
#undef RESET_BASE_DEF
#undef RESET_EXT_DEF

// Helper functions to get the location of the stack/heap
uint32_t halInternalGetCStackBottom(void);
uint32_t halInternalGetHeapTop(void);
uint32_t halInternalGetHeapBottom(void);

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @name Vector Table Index Definitions
 * These are numerical definitions for vector table. Only Cortex-M standard
 * exception vectors (indices 0 through 15) are represented since device
 * specific vectors can be (and often are) different across dies.
 *@{
 */

/**
 * @brief A numerical definition for a vector.
 */
#define STACK_VECTOR_INDEX          0U // special case: stack pointer at reset
#define RESET_VECTOR_INDEX          1U
#define NMI_VECTOR_INDEX            2U
#define HARD_FAULT_VECTOR_INDEX     3U
#define MEMORY_FAULT_VECTOR_INDEX   4U
#define BUS_FAULT_VECTOR_INDEX      5U
#define USAGE_FAULT_VECTOR_INDEX    6U
#define RESERVED07_VECTOR_INDEX     7U
#define RESERVED08_VECTOR_INDEX     8U
#define RESERVED09_VECTOR_INDEX     9U
#define RESERVED10_VECTOR_INDEX     10U
#define SVCALL_VECTOR_INDEX         11U
#define DEBUG_MONITOR_VECTOR_INDEX  12U
#define RESERVED13_VECTOR_INDEX     13U
#define PENDSV_VECTOR_INDEX         14U
#define SYSTICK_VECTOR_INDEX        15U

/**
 * @brief Utility macro to convert from IRQ numbers to exception numbers/
 * vector indices.
 *
 * These are different because the latter include the Cortex-M standard
 * exceptions while the former do not.
 */
#define IRQ_TO_VECTOR_NUMBER(x)     ((x) + 16U)

/**
 * @brief Number of vectors.
 *
 * EXT_IRQ_COUNT is defined in the device header but does not include the
 * Cortex-M standard exceptions.
 */
#define VECTOR_TABLE_LENGTH         (IRQ_TO_VECTOR_NUMBER(EXT_IRQ_COUNT))

/** @}  Vector Table Index Definitions */

/**
 * @brief EM2xx-compatible reset code returned by halGetEm2xxResetInfo()
 */
#define EM2XX_RESET_UNKNOWN               0
#define EM2XX_RESET_EXTERNAL              1   // EM2XX reports POWERON instead
#define EM2XX_RESET_POWERON               2
#define EM2XX_RESET_WATCHDOG              3
#define EM2XX_RESET_ASSERT                6
#define EM2XX_RESET_BOOTLOADER            9
#define EM2XX_RESET_SOFTWARE              11

/**
 * @brief Records the specified reset cause then forces a reboot.
 */
void halInternalSysReset(uint16_t extendedCause);

/**
 * @brief Returns the Extended Reset Cause information
 *
 * @return A 16-bit code identifying the base and extended cause of the reset
 */
uint16_t halGetExtendedResetInfo(void);

/** @brief Calls ::halGetExtendedResetInfo() and translates the EM35x
 *  reset code to the corresponding value used by the EM2XX HAL. Any reset codes
 * not present in the EM2XX are returned after being OR'ed with 0x80.
 *
 * @appusage Used by the EZSP host as a platform-independent NCP reset code.
 *
 * @return The EM2XX-compatible reset code. If not supported by the EM2XX,
 *         return the platform-specific code with B7 set.
 */
uint8_t halGetEm2xxResetInfo(void);

/** @brief Calls ::halGetExtendedResetInfo() and supplies a string describing
 *  the extended cause of the reset.  halGetResetString() should also be called
 *  to get the string for the base reset cause
 *
 * @appusage Useful for diagnostic printing of text just after program
 * initialization.
 *
 * @return A pointer to a program space string.
 */
const char * halGetExtendedResetString(void);

#endif //__EFM_MICRO_H__

/** @} (end addtogroup micro) */
/** @} (end addtogroup legacyhal) */
