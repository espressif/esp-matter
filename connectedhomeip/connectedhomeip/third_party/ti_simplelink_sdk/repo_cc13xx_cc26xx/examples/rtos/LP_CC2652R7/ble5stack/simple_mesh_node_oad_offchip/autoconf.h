/******************************************************************************

@file  autoconf.h

@brief This file contains the definitions required for the Zephyr Mesh Profile
       to operate successfully.

       Please Note:
       All definitions in this file affect the Zephyr source files and the
       Mesh porting layer alone! Any BLE specific definitions in this file
       relate to the "Zephyr-side" and not the "TI-side".

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

#ifndef AUTOCONFIG_H_INCLUDE
#define AUTOCONFIG_H_INCLUDE

#ifndef ZEPHYR_KERNEL_BUILD
#include "ti_ble_mesh_config.h"
#endif

/******************************************************************************
* System
*/
/* This option allows the kernel to operate with its text and read-only
 * sections residing in ROM (or similar read-only memory). Not all boards
 * support this option so it must be used with care; you must also supply a
 * linker command file when building your image. Enabling this option increases
 * both the code and data footprint of the image.*/
#define CONFIG_XIP 1

/* ARM architecture */
#define CONFIG_ARM 1

#define CONFIG_ARCH_HAS_RAMFUNC_SUPPORT 1

/* Use the compiler builtin functions for atomic operations. This is the
 * preferred method. However, support for all arches in GCC is incomplete. */
#ifndef __IAR_SYSTEMS_ICC__
#define CONFIG_ATOMIC_OPERATIONS_BUILTIN 1
#endif /* ifndef __IAR_SYSTEMS_ICC__ */

/* This option specifies the size of the heap memory pool used when dynamically
 * allocating memory using k_malloc(). The maximum size of the memory pool is
 * only limited to available memory. A size of zero means that no heap memory
 * pool is defined. */
#define CONFIG_HEAP_MEM_POOL_SIZE 0

/* This option specifies that the kernel lacks timer support. Some device
 * configurations can eliminate significant code if this is disabled. Obviously
 *  timeout-related APIs will not work. */
#define CONFIG_SYS_CLOCK_EXISTS 1

/* Amount of memory reserved in each network buffer for user data. In most
 * cases this can be left as the default value. */
#define CONFIG_NET_BUF_USER_DATA_SIZE 4

/* Support legacy k_timeout_t API */
#define CONFIG_LEGACY_TIMEOUT_API

/* This option specifies the frequency of the hardware timer used for the system clock (in Hz). */
#define CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC 4000000

/* The interval (in seconds) to send the unprovisioned beacon */
#define CONFIG_BT_MESH_UNPROV_BEACON_INT 5

/* Number of loopback buffers */
#define CONFIG_BT_MESH_LOOPBACK_BUFS 3

/* Number of segment buffers available */
#define CONFIG_BT_MESH_SEG_BUFS 64

#define __fallthrough

/******************************************************************************
* Crypto
*/
/* This option enables support for SHA-256 hash function primitive. */
#define CONFIG_TINYCRYPT_SHA256 1

/* This option enables support for HMAC using SHA-256 message authentication
 * code. */
#define CONFIG_TINYCRYPT_SHA256_HMAC 1

/* This option enables support for pseudo-random number generator. */
#define CONFIG_TINYCRYPT_SHA256_HMAC_PRNG 1

/* This option enables support for the Elliptic curve Diffie-Hellman anonymous
 * key agreement protocol. Enabling ECC requires a cryptographically secure
 * random number generator. */
#define CONFIG_TINYCRYPT_ECC_DH 1

/* This option enables support for AES-128 decrypt and encrypt. */
#define CONFIG_TINYCRYPT_AES 1

/* This option enables support for AES-128 CMAC mode. */
#define CONFIG_TINYCRYPT_AES_CMAC 1


/******************************************************************************
* BT
*/

#define CONFIG_BT_CONN 1

/* Use a custom Bluetooth assert implementation instead of the kernel-wide
 * __ASSERT() when CONFIG_ASSERT is disabled. */
#define CONFIG_BT_ASSERT 1

/* When CONFIG_BT_ASSERT is enabled, this option turns on printing the cause of
 * the assert to the console using printk(). */
#define CONFIG_BT_ASSERT_VERBOSE 1

#define CONFIG_BT_DEBUG 1

/* bt_recv is called from RX thread */
#define CONFIG_BT_RECV_IS_RX_THREAD 1

#define CONFIG_BT_HOST_CRYPTO 1

/* Maximum number of paired Bluetooth devices. The minimum (and default) number
 * is 1. */
#define CONFIG_BT_MAX_PAIRED 1

/* Set the Bluetooth Company Identifier for this device - Texas Instruments */
#define CONFIG_BT_COMPANY_ID 0x000D

/* Maximum number of supported local identity addresses. For most products this
 * is safe to leave as the default value (1). */
#define CONFIG_BT_ID_MAX 1

// Allow to test the IV Update Procedure
//#define CONFIG_BT_MESH_IV_UPDATE_TEST 1

#ifndef CONFIG_BT_MESH_IV_UPDATE_TEST
#define ERPC_REMOVE__bt_mesh_iv_update_test
#define ERPC_REMOVE__bt_mesh_iv_update
#endif

/* ------------------------------------------------------------------------- */
// Generic
/* ------------------------------------------------------------------------- */

/* This value defines in seconds how soon the RPL gets written to persistent
 * storage after a change occurs. If the node receives messages frequently it
 * may make sense to have this set to a large value, whereas if the RPL gets
 * updated infrequently a value as low as 0 (write immediately) may make sense.
 * Note that if the node operates a security sensitive use case, and there’s a
 * risk of sudden power loss, it may be a security vulnerability to set this
 * value to anything else than 0 (a power loss before writing to storage
 * exposes the node to potential message replay attacks). */
#define CONFIG_BT_MESH_RPL_STORE_TIMEOUT 5

/* This value defines how often the local sequence number gets updated in
 * persistent storage (i.e. flash). E.g. a value of 100 means that the sequence
 * number will be stored to flash on every 100th increment. If the node sends
 * messages very frequently a higher value makes more sense, whereas if the
 * node sends infrequently a value as low as 0 (update storage for every
 * increment) can make sense. When the stack gets initialized it will add this
 * number to the last stored one, so that it starts off with a value that’s
 * guaranteed to be larger than the last one used before power off. */
#define CONFIG_BT_MESH_SEQ_STORE_RATE 128

/* This value defines in seconds how soon any pending changes are actually
 * written into persistent storage (flash) after a change occurs. */
#define CONFIG_BT_MESH_STORE_TIMEOUT 2

/* When the IV Update state enters Normal operation or IV Update in Progress,
 * we need to keep track of how many hours has passed in the state, since the
 * specification requires us to remain in the state at least for 96 hours
 * (Update in Progress has an additional upper limit of 144 hours).
 * In order to fulfill the above requirement, even if the node might be powered
 * off once in a while, we need to store persistently how many hours the node
 * has been in the state. This doesn’t necessarily need to happen every hour
 * (thanks to the flexible duration range). The exact cadence will depend a lot
 * on the ways that the node will be used and what kind of power source it has.
 * Since there is no single optimal answer, this configuration option allows
 * specifying a divider, i.e. how many intervals the 96 hour minimum gets split
 * into. After each interval the duration that the node has been in the current
 * state gets stored to flash. E.g. the default value of 4 means that the state
 * is saved every 24 hours (96 / 4). */
#define CONFIG_BT_MESH_IVU_DIVIDER 4
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
// Config Server
/* ------------------------------------------------------------------------- */

/* This options specifies the maximum capacity of the replay protection list.
 * This option is similar to the network message cache size, but has a
 * different purpose. */
#define CONFIG_BT_MESH_CRPL 10

/* This option specifies how many Label UUIDs can be stored. */
#define CONFIG_BT_MESH_LABEL_COUNT 3
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
// Settings
/* ------------------------------------------------------------------------- */

#ifdef CONFIG_BT_SETTINGS

/* Flash Specific */
#define CONFIG_FLASH_PAGE_LAYOUT 1
#define CONFIG_FLASH_PAGE_SIZE 8192

/* Enable support of Non-volatile Storage. */
#define CONFIG_NVS 1
#define CONFIG_SETTINGS_NVS 1
#define CONFIG_SETTINGS_NVS_PAGE_SIZE CONFIG_FLASH_PAGE_SIZE
#define CONFIG_SETTINGS_NVS_PAGE_COUNT 3
#define CONFIG_SETTINGS_NVS_SECTOR_SIZE_MULT 1
#define CONFIG_SETTINGS_NVS_SECTOR_COUNT 1

/* Device Tree Implementation */
#define DT_FLASH_AREA_0_DEV                         "BLE_MESH_NVS"
#define DT_FLASH_AREA_0_SIZE                        CONFIG_SETTINGS_NVS_PAGE_SIZE    //8K
#define DT_FLASH_AREA_0_OFFSET                      0
#define DT_FLASH_AREA_0_ID                          0
#define DT_FLASH_AREA_STORAGE_ID                    DT_FLASH_AREA_0_ID
#define DT_FLASH_AREA_NUM           1

#endif


/* ------------------------------------------------------------------------- */
// Mesh App MSG Segment Size
/* ------------------------------------------------------------------------- */

#ifdef ZEPHYR_ADV_EXT
#define BT_MESH_SEG_SIZE 226
#else
#define BT_MESH_SEG_SIZE 12
#endif

/* ------------------------------------------------------------------------- */

#endif
