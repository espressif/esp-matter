/**
 * @file
 * @brief Defines the procedures to check BA414EP integrity
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#ifndef SX_PK_INTEGRITY_H_
#define SX_PK_INTEGRITY_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_pk_integrity_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_pk_integrity.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_pk_integrity.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Check BA414EP ROM integrity
 * @return 0 when no error
 */
uint32_t ba414ep_ucode_integrity_check(void) CHECK_RESULT;

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif /* SX_PK_INTEGRITY_H */

