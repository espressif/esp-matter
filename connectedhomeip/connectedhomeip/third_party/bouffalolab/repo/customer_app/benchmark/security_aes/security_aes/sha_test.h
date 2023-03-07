#ifndef __SHA_TEST_H__
#define __SHA_TEST_H__

#include <stddef.h>
#include <stdint.h>
#include <bl_sec.h>
#include "malloc_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SHA_256_MODE,                         /*!< SHA mode type:256 */
    SHA_224_MODE,                         /*!< SHA mode type:224 */
    SHA_1_MODE,
    SHA_1_RSVD_MODE,
}SHA_Type;

void sha_test(uint32_t length, uint32_t count,
              SHA_Type mode, uint8_t is_soft, ram_type_t ram_type);

#ifdef __cplusplus
}
#endif

#endif
