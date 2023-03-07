
#include "gpHal_kx_regprop_basic.h"
#include "gpHal_kx_mm.h"
#include "rom_jump_table.h"

/*
 * ROM access functions - GCC version.
 */

.macro func_def name, table_entry
    .section .text.\name, "ax", %progbits
    .global \name
    .type   \name, %function
\name:
    ldr     ip, =(GP_MM_ROM_JUMP_TABLE_START + \table_entry * 4 + 1)
    bx      ip
.endm

    .syntax unified
    .thumb

    .align 1
    .text

    func_def    ROM_go_to_sleep,                    GO_TO_SLEEP
    func_def    ROM_flash_sector_erase,             FLASH_SECTOR_ERASE
    func_def    ROM_flash_write,                    FLASH_WRITE
    func_def    ROM_flash_info,                     FLASH_INFO

    func_def    ROM_get_P256_cortex_ecdh_lib_info,  GET_P256_CORTEX_ECDH_LIB_INFO
    func_def    ROM_P256_pointmult,                 P256_POINTMULT
    func_def    ROM_P256_ecdh_keygen,               P256_ECDH_KEYGEN
    func_def    ROM_P256_ecdh_shared_secret,        P256_ECDH_SHARED_SECRET

    func_def    ROM_verify_x25519_signature,        X25519_VERIFY
    func_def    ROM_aes_mmo_start,                  AES_MMO_START
    func_def    ROM_aes_mmo_update,                 AES_MMO_UPDATE
    func_def    ROM_aes_mmo_finalize,               AES_MMO_FINALIZE




