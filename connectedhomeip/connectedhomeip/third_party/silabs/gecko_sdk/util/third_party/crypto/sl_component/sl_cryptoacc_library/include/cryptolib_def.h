
#ifndef CRYPTOLIB_DEF_H
#define CRYPTOLIB_DEF_H

#include "em_device.h"

/* Define 'uint8_t', 'uint32_t'... */
#include <stdint.h>

/* Define 'bool' */
#include <stdbool.h>

/* Define 'NULL' */
#include <stddef.h>

/* Define 'CHECK_RESULT' macro (from cryptolib) */
#include "compiler_extentions.h"

/* Define 'block_t' (from cryptolib) */
#include "cryptolib_types.h"

/* Define 'memcpy_*' functions (from cryptolib) */
#include "sx_memcpy.h"

/* Define 'memcmp*' functions (from cryptolib) */
#include "sx_memcmp.h"

/*****************************************************************************
 ** Address map
 *****************************************************************************/

#ifndef ADDR_CRYPTOWRAP
#define ADDR_CRYPTOWRAP          CRYPTOACC_BASE
#endif

#define ADDR_CRYPTOMASTER_REGS   (ADDR_CRYPTOWRAP+0x00000000)
#define ADDR_BA431_REGS          (ADDR_CRYPTOWRAP+0x00001000)
#define ADDR_BA414EP_REGS        (ADDR_CRYPTOWRAP+0x00002000)
#define ADDR_BA431_FIFO          (ADDR_CRYPTOWRAP+0x00004000)
#define ADDR_BA414EP_CRYPTORAM   (ADDR_CRYPTOWRAP+0x00008000)
#define ADDR_BA414EP_UCODE       (ADDR_CRYPTOWRAP+0x0000C000)

#define ADDR_INCL_IPS_HW_CFG     (ADDR_CRYPTOWRAP+0x00000400)
#define ADDR_BA411E_HW_CFG_1     (ADDR_CRYPTOWRAP+0x00000404)
#define ADDR_BA411E_HW_CFG_2     (ADDR_CRYPTOWRAP+0x00000408)
#define ADDR_BA413_HW_CFG        (ADDR_CRYPTOWRAP+0x0000040C)
#define ADDR_BA418_HW_CFG        (ADDR_CRYPTOWRAP+0x00000410)
#define ADDR_BA419_HW_CFG        (ADDR_CRYPTOWRAP+0x00000414)
#define ADDR_BA424_HW_CFG        (ADDR_CRYPTOWRAP+0x00000418)

/*****************************************************************************
 ** Enable polling defines
 *****************************************************************************/

#define WAIT_CRYPTOMASTER_WITH_REGISTER_POLLING (1)
#define WAIT_RNG_WITH_REGISTER_POLLING          (1)
#define WAIT_PK_WITH_REGISTER_POLLING           (1)

/*****************************************************************************
 ** Bus error handling
 *****************************************************************************/

/** @brief Trigger hardfault by access to invalid memory address */
static inline void trigger_hardfault(void) {
   uint32_t *bad_mem_addr = (uint32_t*) 0x3FFFFFFC;
   *bad_mem_addr = 1;
}

#define TRIGGER_HARDFAULT_FCT()  trigger_hardfault();

/*****************************************************************************
 ** Debug
 *****************************************************************************/

#define DEBUG_ENABLED                     (1)

#define CRYPTOLIB_PRINTF(...)             {}
#define CRYPTOLIB_ASSERT(cond, msg)       {}
#define CRYPTOLIB_ASSERT_NM(cond)         {}
#define CRYPTOLIB_ASSERT_VAR(cond, ...)   {}

/*****************************************************************************
 ** Crypto DMA
 *****************************************************************************/

#define BLK_MEMCPY_MIN_DMA_SIZE  (16UL)

/*****************************************************************************
 ** Supported algorithms
 *****************************************************************************/

#define DH_MODP_ENABLED       (0)
#define AIS31_ENABLED         (1)

#ifdef _SILICON_LABS_32B_SERIES_2_CONFIG_2
   #define JPAKE_ENABLED      (0)
#else
   #define JPAKE_ENABLED      (1)
#endif

/*****************************************************************************
 ** Max sizes
 *****************************************************************************/

#define PRIME_MAX_SIZE        ((4096 / 8) / 2)
#define ECC_MAX_KEY_SIZE      (256/8)
#define DERIV_MAX_SALT_SIZE   (512)
#define DERIV_MAX_INFO_SIZE   (512)

/*****************************************************************************
 ** RNG settings
 *****************************************************************************/

// Control subsampling at output of ring oscillators (ring sampling
// frequency = ClkSec frequency / (RNG_CLKDIV+1)).
#define RNG_CLKDIV            (7)

// Number of clock cycles after FIFO being full until TRNG goes to sleep.
// A negative value will keep the ring oscillators always running.
#define RNG_OFF_TIMER_VAL     (0)

// When the FIFO level (number of 128-bit block) becomes smaller or equal
// to RNG_FIFO_WAKEUP_LVL, the TRNG is restarted.
#define RNG_FIFO_WAKEUP_LVL   (8)

// Number of clock cycles to wait after startup before sampling data from
// the noise source.
#define RNG_INIT_WAIT_VAL     (512)

// Number of 128-bit blocks of data from the noise source that are used
// as input to the conditioning function.
#define RNG_NB_128BIT_BLOCKS  (8)

/*****************************************************************************
 ** Countermeasures
 *****************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
   #define PK_CM_ENABLED      (0)
#else
   #define PK_CM_ENABLED      (1)
#endif

#if PK_CM_ENABLED
   // PK_CM_RAND_SIZE sets the byte size of the random number that is used for
   // side channel countermeasures. Maximum allowable size is dependent on the
   // maximum supported curve size:
   // * 256 bit: 0 < rand < 2^16 => 2 bytes
   // * 521 bit: 0 < rand < 2^23 => still 2 bytes (3 bytes could lead to 2^24-1)
   #define PK_CM_RAND_SIZE       (2)

   #define PK_CM_RAND_PROJ       (1)
   #define PK_CM_RAND_SCALAR     (1)
   #define PK_CM_RAND_MODULUS    (0)  // Only relevant for RSA/DSA/SRP
   #define PK_CM_RAND_EXPONENT   (0)  // Only relevant for RSA/DSA/SRP
#endif

#endif
