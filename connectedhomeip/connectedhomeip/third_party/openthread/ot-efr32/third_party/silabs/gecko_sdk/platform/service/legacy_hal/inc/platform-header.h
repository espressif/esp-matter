#ifndef PLATFORM_HEADER_H
#define PLATFORM_HEADER_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef EZSP_HOST
  #include <assert.h>
#endif // EZSP_HOST

// Standard program memory delcaration.
#define PGM     const

// Char pointer to program memory declaration.
#define PGM_P   const char *

#define TRUE    1u
#define FALSE   0u

// Define the legacy types to support old Zigbee and AppBuilder code that has
// not been updated to use stdint.h
typedef bool boolean;
typedef uint8_t int8u;
typedef int8_t int8s;
typedef uint16_t int16u;
typedef int16_t int16s;
typedef uint32_t int32u;
typedef int32_t int32s;
typedef uint64_t int64u;
typedef int64_t int64s;

#ifndef __SOURCEFILE__
/**
 * The __SOURCEFILE__ macro is used by asserts to list the
 * filename if it isn't otherwise defined, set it to the compiler intrinsic
 * which specifies the whole filename and path of the sourcefile
 */
  #define __SOURCEFILE__ __FILE__
#endif

#ifndef UNUSED_VAR
/**
 * Useful macro for avoiding compiler warnings related to unused
 * function arguments or unused variables.
 */
#define UNUSED_VAR(x) (void)(x)
#endif

/**
 * @brief Portable Segment names
 */
#define __NO_INIT__                         ".noinit"
#define __INTVEC__                          ".intvec"
#define __CSTACK__                          ".stack"
#define __BSS__                             ".bss"
#define __TEXT__                            ".text"
#define __DATA_INIT__                       ".data_init"
#define __NVM__                             ".nvm"
#define __DATA__                            ".data"
#define __CONST__                           ".rodata"
#if defined(__GNUC__)
#define __AAT__                             ".aat"
#define __INTERNAL_STORAGE__                ".internal_storage"
#define __SIMEE__                           ".simee"
#elif defined(__ICCARM__)
#define __AAT__                             "AAT"
#define __INTERNAL_STORAGE__                "INTERNAL_STORAGE"
#define __SIMEE__                           "SIMEE"
#endif

#define SECURITY_BLOCK_SIZE                 16
#define STACK_FILL_VALUE                    0xCDCDCDCDU

// GCC specific macros
#if defined(__GNUC__)

#define UNUSED __attribute__ ((unused))

#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
  __variableDeclaration __attribute__ ((section(__segmentName)))

#define NO_STRIPPING __attribute__((used))

#define WEAK(__symbol) \
  __attribute__ ((weak)) __symbol

// Provide a portable way to align data.
#define ALIGNMENT(__alignmentBytes) \
  __attribute__ ((aligned(__alignmentBytes)))

extern uint32_t __StackTop, __StackLimit;
extern uint32_t linker_code_begin, linker_code_end;
extern uint32_t __NO_INIT__begin, __NO_INIT__end;
extern uint32_t linker_storage_begin, linker_storage_end;
extern uint32_t __bss_start__, __bss_end__;
extern uint32_t linker_nvm_begin, linker_nvm_end;

#define _CSTACK_SEGMENT_BEGIN           (&__StackLimit)
#define _TEXT_SEGMENT_BEGIN             (&linker_code_begin)
#define _NO_INIT_SEGMENT_BEGIN          (&__NO_INIT__begin)
#define _INTERNAL_STORAGE_BEGIN         (&linker_storage_begin)
#define _BSS_SEGMENT_BEGIN              (&__bss_start__)
#define _SIMEE_SEGMENT_BEGIN            (&linker_nvm_begin)

#define _CSTACK_SEGMENT_END             (&__StackTop)
#define _TEXT_SEGMENT_END               (&linker_code_end)
#define _NO_INIT_SEGMENT_END            (&__NO_INIT__end)
#define _INTERNAL_STORAGE_END           (&linker_storage_end)
#define _BSS_SEGMENT_END                (&__bss_end__)
#define _SIMEE_SEGMENT_END              (&linker_nvm_end)

#define NO_INIT(__symbol) VAR_AT_SEGMENT(__symbol, __NO_INIT__)

#define asm(...) __asm__ (__VA_ARGS__)

// Ignore the __no_init attribute if not using IAR
#define __no_init

#endif // __GNUC__

// IAR Specific macros
#if defined(__ICCARM__)

#define UNUSED

#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
  __variableDeclaration @ __segmentName

#define NO_STRIPPING  __root

#define WEAK(__symbol) \
  __weak __symbol

// Provide a portable way to align data.
#define ALIGNMENT(X) \
  _Pragma(STRINGIZE(data_alignment = X))

#pragma segment=__CSTACK__
#pragma segment=__TEXT__
#pragma segment=__NO_INIT__
#pragma segment=__INTERNAL_STORAGE__
#pragma segment=__BSS__
#pragma segment=__SIMEE__
#pragma segment=__AAT__

#define _CSTACK_SEGMENT_BEGIN           __segment_begin(__CSTACK__)
#define _TEXT_SEGMENT_BEGIN             __segment_begin(__TEXT__)
#define _NO_INIT_SEGMENT_BEGIN          __segment_begin(__NO_INIT__)
#define _INTERNAL_STORAGE_BEGIN         __segment_begin(__INTERNAL_STORAGE__)
#define _BSS_SEGMENT_BEGIN              __segment_begin(__BSS__)
#define _SIMEE_SEGMENT_BEGIN            __segment_begin(__SIMEE__)

#define _CSTACK_SEGMENT_END             __segment_end(__CSTACK__)
#define _TEXT_SEGMENT_END               __segment_end(__TEXT__)
#define _NO_INIT_SEGMENT_END            __segment_end(__NO_INIT__)
#define _INTERNAL_STORAGE_END           __segment_end(__INTERNAL_STORAGE__)
#define _BSS_SEGMENT_END                __segment_end(__BSS__)
#define _SIMEE_SEGMENT_END              __segment_end(__SIMEE__)

#define NO_INIT(__symbol) __no_init __symbol

#endif // __ICCARM__

/**
 * @name  Byte Manipulation Macros
 */
//@{

/**
 * @brief Returns the low byte of the 16-bit value \c n as an \c uint8_t.
 */
#define LOW_BYTE(n)                     ((uint8_t)((n) & 0xFF))

/**
 * @brief Returns the high byte of the 16-bit value \c n as an \c uint8_t.
 */
#define HIGH_BYTE(n)                    ((uint8_t)(LOW_BYTE((n) >> 8)))

/**
 * @brief Returns the value built from the two \c uint8_t
 * values \c high and \c low.
 */
#define HIGH_LOW_TO_INT(high, low) ( \
    (((uint16_t) (high)) << 8)       \
    + ((uint16_t) ((low) & 0xFF))    \
    )

/**
 * @brief Useful to reference a single bit of a byte.
 */
#define BIT(x) (1U << (x))  // Unsigned avoids compiler warnings re BIT(15)

/**
 * @brief Useful to reference a single bit of an uint32_t type.
 */
#define BIT32(x) (((uint32_t) 1) << (x))

/**
 * @brief Returns the low byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_0(n)                    ((uint8_t)((n) & 0xFF))

/**
 * @brief Returns the second byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_1(n)                    BYTE_0((n) >> 8)

/**
 * @brief Returns the third byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_2(n)                    BYTE_0((n) >> 16)

/**
 * @brief Returns the high byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_3(n)                    BYTE_0((n) >> 24)

/**
 * @brief Returns the fifth byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_4(n)                    BYTE_0((n) >> 32)

/**
 * @brief Returns the sixth byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_5(n)                    BYTE_0((n) >> 40)

/**
 * @brief Returns the seventh byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_6(n)                    BYTE_0((n) >> 48)

/**
 * @brief Returns the high byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_7(n)                    BYTE_0((n) >> 56)

/**
 * @brief Sets \c bit in the \c reg register or byte.
 * @note Assuming \c reg is an IO register, some platforms (such as the
 * AVR) can implement this in a single atomic operation.
 */
#define SETBIT(reg, bit)      (reg) |= BIT(bit)

/**
 * @brief Sets the bits in the \c reg register or the byte
 * as specified in the bitmask \c bits.
 * @note This is never a single atomic operation.
 */
#define SETBITS(reg, bits)    (reg) |= (bits)

/**
 * @brief Clears a bit in the \c reg register or byte.
 * @note Assuming \c reg is an IO register, some platforms (such as the AVR)
 * can implement this in a single atomic operation.
 */
#define CLEARBIT(reg, bit)    (reg) &= ~(BIT(bit))

/**
 * @brief Clears the bits in the \c reg register or byte
 * as specified in the bitmask \c bits.
 * @note This is never a single atomic operation.
 */
#define CLEARBITS(reg, bits)  (reg) &= ~(bits)

/**
 * @brief Returns the value of \c bit within the register or byte \c reg.
 */
#define READBIT(reg, bit)     ((reg) & (BIT(bit)))

/**
 * @brief Returns the value of the bitmask \c bits within
 * the register or byte \c reg.
 */
#define READBITS(reg, bits)   ((reg) & (bits))

/**
 * @brief Returns the number of entries in an array.
 */
#define COUNTOF(a) (sizeof(a) / sizeof(a[0]))

//@} \\END Byte manipulation macros

////////////////////////////////////////////////////////////////////////////////

/**
 * @name  Time Manipulation Macros
 */
//@{

/**
 * @brief Returns the elapsed time between two 8 bit values.
 *        Result may not be valid if the time samples differ by more than 127
 */
#define elapsedTimeInt8u(oldTime, newTime) \
  ((uint8_t) ((uint8_t)(newTime) - (uint8_t)(oldTime)))

/**
 * @brief Returns the elapsed time between two 16 bit values.
 *        Result may not be valid if the time samples differ by more than 32767
 */
#define elapsedTimeInt16u(oldTime, newTime) \
  ((uint16_t) ((uint16_t)(newTime) - (uint16_t)(oldTime)))

/**
 * @brief Returns the elapsed time between two 32 bit values.
 *   Result may not be valid if the time samples differ by more than 2147483647
 */
#define elapsedTimeInt32u(oldTime, newTime) \
  ((uint32_t) ((uint32_t)(newTime) - (uint32_t)(oldTime)))

/**
 * @brief Returns true if t1 is greater than t2.  Can only account for 1 wrap
 * around of the variable before it is wrong.
 */
#define MAX_INT8U_VALUE       (0xFF)
#define HALF_MAX_INT8U_VALUE  (0x80)
#define timeGTorEqualInt8u(t1, t2) \
  (elapsedTimeInt8u(t2, t1) <= (HALF_MAX_INT8U_VALUE))

/**
 * @brief Returns true if t1 is greater than t2.  Can only account for 1 wrap
 * around of the variable before it is wrong.
 */
#define MAX_INT16U_VALUE      (0xFFFF)
#define HALF_MAX_INT16U_VALUE (0x8000)
#define timeGTorEqualInt16u(t1, t2) \
  (elapsedTimeInt16u(t2, t1) <= (HALF_MAX_INT16U_VALUE))

/**
 * @brief Returns true if t1 is greater than t2.  Can only account for 1 wrap
 * around of the variable before it is wrong.
 */
#define MAX_INT32U_VALUE      (0xFFFFFFFFUL)
#define HALF_MAX_INT32U_VALUE (0x80000000UL)
#define timeGTorEqualInt32u(t1, t2) \
  (elapsedTimeInt32u(t2, t1) <= (HALF_MAX_INT32U_VALUE))

#ifndef EMBER_TEST
#include "em_core.h"

// -----------------------------------------------------------------------------
//  Global Interrupt Manipulation Macros
//@{

#define ATOMIC_LITE(blah) ATOMIC(blah)
#define DECLARE_INTERRUPT_STATE_LITE DECLARE_INTERRUPT_STATE
#define DISABLE_INTERRUPTS_LITE() DISABLE_INTERRUPTS()
#define RESTORE_INTERRUPTS_LITE() RESTORE_INTERRUPTS()

#define DECLARE_INTERRUPT_STATE CORE_DECLARE_IRQ_STATE

/**
 * @brief Disable interrupts, saving the previous state so it can be
 * later restored with RESTORE_INTERRUPTS().
 * \note Do not fail to call RESTORE_INTERRUPTS().
 * \note It is safe to nest this call.
 */
#define DISABLE_INTERRUPTS() CORE_ENTER_ATOMIC()

/**
 * @brief Restore the global interrupt state previously saved by
 * DISABLE_INTERRUPTS()
 * \note Do not call without having first called DISABLE_INTERRUPTS()
 * to have saved the state.
 * \note It is safe to nest this call.
 */
 #define RESTORE_INTERRUPTS() CORE_EXIT_ATOMIC()

/**
 * @brief Enable global interrupts without regard to the current or
 * previous state.
 */
 #define INTERRUPTS_ON() CORE_ATOMIC_IRQ_ENABLE()

/**
 * @brief Disable global interrupts without regard to the current or
 * previous state.
 */
 #define INTERRUPTS_OFF() CORE_ATOMIC_IRQ_DISABLE()

/**
 * @returns true if global interrupts are disabled.
 */
 #define INTERRUPTS_ARE_OFF() CORE_IrqIsDisabled()

/**
 * @returns true if global interrupt flag was enabled when
 * ::DISABLE_INTERRUPTS() was called.
 */
 #define INTERRUPTS_WERE_ON() (irqState == 0)

/**
 * @brief A block of code may be made atomic by wrapping it with this
 * macro.  Something which is atomic cannot be interrupted by interrupts.
 */
 #define ATOMIC(blah) CORE_ATOMIC_SECTION(blah)

/**
 * @brief Allows any pending interrupts to be executed. Usually this
 * would be called at a safe point while interrupts are disabled (such as
 * within an ISR).
 *
 * Takes no action if interrupts are already enabled.
 */
 #define HANDLE_PENDING_INTERRUPTS() CORE_YIELD_ATOMIC()

#else
 #include <stdio.h>
 #define ATOMIC_LITE(blah) ATOMIC(blah)
#define DECLARE_INTERRUPT_STATE_LITE DECLARE_INTERRUPT_STATE
#define DISABLE_INTERRUPTS_LITE()
#define RESTORE_INTERRUPTS_LITE()

#define DECLARE_INTERRUPT_STATE

/**
 * @brief Disable interrupts, saving the previous state so it can be
 * later restored with RESTORE_INTERRUPTS().
 * \note Do not fail to call RESTORE_INTERRUPTS().
 * \note It is safe to nest this call.
 */
#define DISABLE_INTERRUPTS()

/**
 * @brief Restore the global interrupt state previously saved by
 * DISABLE_INTERRUPTS()
 * \note Do not call without having first called DISABLE_INTERRUPTS()
 * to have saved the state.
 * \note It is safe to nest this call.
 */
 #define RESTORE_INTERRUPTS()

/**
 * @brief Enable global interrupts without regard to the current or
 * previous state.
 */
 #define INTERRUPTS_ON()

/**
 * @brief Disable global interrupts without regard to the current or
 * previous state.
 */
 #define INTERRUPTS_OFF()

/**
 * @returns true if global interrupts are disabled.
 */
 #define INTERRUPTS_ARE_OFF() false

/**
 * @returns true if global interrupt flag was enabled when
 * ::DISABLE_INTERRUPTS() was called.
 */
 #define INTERRUPTS_WERE_ON() true

/**
 * @brief A block of code may be made atomic by wrapping it with this
 * macro.  Something which is atomic cannot be interrupted by interrupts.
 */
 #define ATOMIC(blah) blah

/**
 * @brief Allows any pending interrupts to be executed. Usually this
 * would be called at a safe point while interrupts are disabled (such as
 * within an ISR).
 *
 * Takes no action if interrupts are already enabled.
 */
 #define HANDLE_PENDING_INTERRUPTS()

#endif //EMBER_TEST
#define MILLISECOND_TICKS_PER_SECOND 1000UL

#ifndef MILLISECOND_TICKS_PER_DECISECOND
  #define MILLISECOND_TICKS_PER_DECISECOND (MILLISECOND_TICKS_PER_SECOND / 10)
#endif

#ifndef MILLISECOND_TICKS_PER_QUARTERSECOND
  #define MILLISECOND_TICKS_PER_QUARTERSECOND (MILLISECOND_TICKS_PER_SECOND >> 2)
#endif

#ifndef MILLISECOND_TICKS_PER_MINUTE
  #define MILLISECOND_TICKS_PER_MINUTE (60UL * MILLISECOND_TICKS_PER_SECOND)
#endif

#ifndef MILLISECOND_TICKS_PER_HOUR
  #define MILLISECOND_TICKS_PER_HOUR (60UL * MILLISECOND_TICKS_PER_MINUTE)
#endif

#ifndef MILLISECOND_TICKS_PER_DAY
  #define MILLISECOND_TICKS_PER_DAY (24UL * MILLISECOND_TICKS_PER_HOUR)
#endif

// This is no longer used but define it to nothing for backwards compatibility
#define PGM_NO_CONST

#ifdef _HAL_USE_COMMON_MEMUTILS_

/** \name C Standard Library Memory Utilities
 * These should be used in place of the standard library functions.
 *
 * These functions have the same parameters and expected results as their C
 * Standard Library equivalents but may take advantage of certain implementation
 * optimizations.
 *
 * Unless otherwise noted, these functions are utilized by the EmberStack and are
 * therefore required to be implemented in the HAL. Additionally, unless otherwise
 * noted, applications that find these functions useful may utilze them.
 */
//@{

/**
 * @brief Refer to the C stdlib memmove().
 */
void halCommonMemMove(void *dest, const void *src, uint16_t bytes);

/**
 * @brief Refer to the C stdlib memset().
 */
void halCommonMemSet(void *dest, uint8_t val, uint16_t bytes);

/**
 * @brief Refer to the C stdlib memcmp().
 */
int16_t halCommonMemCompare(const void *source0, const void *source1, uint16_t bytes);

/**
 * @brief Works like C stdlib memcmp(), but takes a flash space source
 * parameter.
 */
int8_t halCommonMemPGMCompare(const void *source0, const void PGM_NO_CONST *source1, uint16_t bytes);

/**
 * @brief Same as the C stdlib memcpy(), but handles copying from const
 * program space.
 */
void halCommonMemPGMCopy(void* dest, const void PGM_NO_CONST *source, uint16_t bytes);

/**
 * @brief Friendly convenience macro pointing to the full HAL function.
 */
#define MEMSET(d, v, l)  halCommonMemSet(d, v, l)
#define MEMCOPY(d, s, l) halCommonMemMove(d, s, l)
#define MEMMOVE(d, s, l) halCommonMemMove(d, s, l)
#define MEMPGMCOPY(d, s, l) halCommonMemPGMCopy(d, s, l)
#define MEMCOMPARE(s0, s1, l) halCommonMemCompare(s0, s1, l)
#define MEMPGMCOMPARE(s0, s1, l) halCommonMemPGMCompare(s0, s1, l)

//@}  // end of C Standard Library Memory Utilities
#else

#include "stdlib.h"
#include "string.h"

#define MEMSET(d, v, l)  memset(d, v, l)
#define MEMCOPY(d, s, l) memcpy(d, s, l)
#define MEMMOVE(d, s, l) memmove(d, s, l)
#define MEMPGMCOPY(d, s, l) memcpy(d, s, l)
#define MEMCOMPARE(s0, s1, l) memcmp(s0, s1, l)
#define MEMPGMCOMPARE(s0, s1, l) memcmp(s0, s1, l)

#endif //_HAL_USE_COMMON_MEMUTILS_

#ifdef TOKEN_MANAGER_TEST
void token_manager_test_assert_handler(const char *filename, int linenumber);
#define ASSERT_HANDLER token_manager_test_assert_handler
#else
void halInternalAssertFailed(const char * filename, int linenumber);
#define ASSERT_HANDLER halInternalAssertFailed
#endif
#ifndef EZSP_HOST

#undef assert
#define assert(condition)  \
  do { if (!(condition)) { \
         ASSERT_HANDLER(__SOURCEFILE__, __LINE__); } } while (0)
#endif // EZSP_HOST

#endif // PLATFORM_HEADER_H
