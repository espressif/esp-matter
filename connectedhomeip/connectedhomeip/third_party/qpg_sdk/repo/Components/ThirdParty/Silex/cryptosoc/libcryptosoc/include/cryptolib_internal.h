/*
 * This file should be included by all cryptolib implementation (*.c) files.
 * This file should NOT be included by application code or by any public cryptolib header (*.h) files.
 */

#ifndef CRYPTOLIB_INTERNAL_H_
#define CRYPTOLIB_INTERNAL_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "cryptolib_internal_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)


#if !defined(GP_DIVERSITY_GPHAL_K8E)
#error CryptoSoc only suppported on K8C/E
#endif
#ifndef GP_DIVERSITY_GPHAL_INTERN
#error CryptoSoc only suppported on internal CortexM4
#endif

#define GP_COMPONENT_ID             GP_COMPONENT_ID_SILEXCRYPTOSOC

#include "global.h"
#include "hal.h"
#include "gpHal_kx_mm.h"
#include "gpAssert.h"
#include "gpLog.h"

// addresses
#define ADDR_CRYPTOMASTER_REGS      GP_MM_AHB_CRYPTOMASTER_DMA_START
#define ADDR_BA431_REGS             GP_MM_AHB_RNG_CONTROL_REG_START
#define ADDR_BA414E_REGS            GP_MM_AHB_PUBLIC_KEY_REG_START
#define ADDR_BA431_FIFO             GP_MM_AHB_RNG_OUTPUT_START
#define ADDR_BA414E_CRYPTORAM       GP_MM_AHB_PUBLIC_KEY_DATARAM_START
#define ADDR_BA414E_UCODE           GP_MM_AHB_PUBLIC_KEY_CODEROM_START

// memory barrier before and after DMA
#define CRYPTOLIB_DMA_BARRIER()     __DMB()

// Wait type
// 0: use irq hook macro bellow
// 1: use register polling
#define WAIT_CRYPTOMASTER_WITH_REGISTER_POLLING     1
#define WAIT_RNG_WITH_REGISTER_POLLING              1
#define WAIT_PK_WITH_REGISTER_POLLING               1

// hooks for functions
#define CRYPTOMASTER_WAITIRQ_FCT()    ;   // void
#define RNG_WAITIRQ_FCT()             ;   // void
#define PK_WAITIRQ_FCT()              ;   // void
#define TRIGGER_HARDFAULT_FCT()       cryptodma_reset();   // void

/**
 * Align on word boundary
 */
#define ALIGNED COMPILER_ALIGNED(4)

// bind CryptoSoc print statements to gpLog
#define CRYPTOLIB_PRINTF(fmt, ...)  do { } while(0)
#define SX_PRINT_BLK(blk)           do { } while(0)

// bind CryptoSoc assert statements to gpAssert
#define CRYPTOLIB_ASSERT(cond, msg)     GP_ASSERT_DEV_INT(cond)
#define CRYPTOLIB_ASSERT_NM(cond)       GP_ASSERT_DEV_INT(cond)
#define CRYPTOLIB_ASSERT_VAR(cond, ...) GP_ASSERT_DEV_INT(cond)

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //CRYPTOLIB_INTERNAL_H_
