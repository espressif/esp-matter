/*
 * File: rom_jump_table.h
 *       ROM jump table location.
 * Copyright (C) 2015 Greenpeak Technologies.
 * Copyright (C) 2018 Qorvo International.
 */

#ifndef __ROM_JUMP_TABLE_H__
#define __ROM_JUMP_TABLE_H__

#define GP_MM_ROM_JUMP_TABLE_START  (GP_MM_ROM_START + 0x10)

#define GO_TO_SLEEP                         0
#define FLASH_SECTOR_ERASE                  1
#define FLASH_WRITE                         2
#define FLASH_INFO                          3

#define GET_P256_CORTEX_ECDH_LIB_INFO       4
#define P256_POINTMULT                      5
#define P256_ECDH_KEYGEN                    6
#define P256_ECDH_SHARED_SECRET             7

#define X25519_VERIFY                       8
#define AES_MMO_START                       9
#define AES_MMO_UPDATE                      10
#define AES_MMO_FINALIZE                    11



#endif  /* __ROM_JUMP_TABLE_H__ */
