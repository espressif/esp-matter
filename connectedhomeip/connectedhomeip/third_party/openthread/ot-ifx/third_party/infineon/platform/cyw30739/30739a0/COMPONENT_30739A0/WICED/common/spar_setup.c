/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/**
 * The Stackable Patch and Application Runtime
 * C-runtime setup of this SPAR tier
 */
#include <errno.h>
#include "spar_utils.h"
#include "sparcommon.h"
#include <stdio.h>
#include <sys/unistd.h>
#ifdef BLE_OTA_FW_UPGRADE
#include <ota_fw_upgrade.h>
#endif
#include <wiced_hal_wdog.h>
#include <wiced_platform.h>
#include <wiced_sleep.h>

void * __dso_handle = NULL;

typedef struct __attribute__((packed))
{
    uint16_t host_claim_host_to_device_size;
    uint16_t host_to_device_size;
    uint16_t device_claim_device_to_host_size;
    uint16_t device_to_host_size;
    uint8_t host_claim_host_to_device_count;
    uint8_t host_to_device_count;
    uint8_t device_to_host_count;
} BT_CONFIG_ACL_POOLS_t;

typedef struct __attribute__((packed))
{
    // The size of each block in this pool.
    uint16_t size;

    // The number of blocks in this pool.
    uint8_t count;

    // The number of blocks in this pool that are reserved for dynamic_memory_AllocateOrDie calls.
    // This number of reserved blocks cannot be consumed by calls to
    // dynamic_memory_AllocateOrReturnNULL, which will return NULL if the block count is below the
    // die_reserve threshold.
    uint8_t die_reserve;
} FOUNDATION_CONFIG_DYNAMIC_MEMORY_POOL_t;

typedef struct __attribute__((packed))
{
    // The number of pools that are to be created from the general pools.  The default value is
    // DYNAMIC_MEMORY_NUM_POOLS, but we reserve an extra pool control block, in case we need to add
    // a block size category from configuration data.  Unless we need to add a new block size
    // category pool, config data (.ags, .cgx) should probably just use DYNAMIC_MEMORY_NUM_POOLS
    // as a named value for this field.
    uint8_t num_pools;

    //$ DEFINE num_pools: DYNAMIC_MEMORY_NUM_POOLS

    // Info on the size, count, and blocks reserved for dynamic_memory_AllocateOrDie in each pool.
    FOUNDATION_CONFIG_DYNAMIC_MEMORY_POOL_t pools[DYNAMIC_MEMORY_NUM_POOLS];
} FOUNDATION_CONFIG_DYNAMIC_MEMORY_t;


/*****************************************************************
*   External definitions
*
*****************************************************************/
extern BT_CONFIG_ACL_POOLS_t g_bt_config_ACL_Pools;
extern BT_CONFIG_ACL_POOLS_t g_bt_config_BLE_Pools;
extern uint8_t g_ble_max_connections;
extern void *g_dynamic_memory_MinAddress;
extern void *g_dynamic_memory_MaxAddressPlusOne;
extern void *g_aon_memory_manager_MinAddress;
extern void install_libs(void);

typedef void (*constructor_t)(void);
extern constructor_t __init_array_start[];
extern constructor_t __init_array_end[];

extern FOUNDATION_CONFIG_DYNAMIC_MEMORY_t g_foundation_config_DynamicMemory;

void init_cpp_ctors()
{
    constructor_t *p;

    for (p = __init_array_start; p < __init_array_end; ++p)
    {
        (*p)();
    }
}

__attribute__((section(".app_init_code")))
void application_start_internal(void)
{
    wiced_platform_init();
    init_cpp_ctors();
}

/* WEAK application_pre_init function. Will be called if not defined anywhere else */
__attribute__((weak))
void wiced_pre_init(void)
{
    /* BR/EDR ACL buffer */
    g_bt_config_ACL_Pools.device_to_host_count = 0;
    g_bt_config_ACL_Pools.host_to_device_count = 0;

    /* BLE ACL buffer */
    g_bt_config_BLE_Pools.device_to_host_count =  5;
    g_bt_config_BLE_Pools.host_to_device_count =  5;

    /* the Maximum Number of BLE Connections */
    g_ble_max_connections = 5;

    /* Change the Foundation Dynamic Memory pools. */
    // pool[0]: [16, 36, 3] => [8, 128, 3]
    g_foundation_config_DynamicMemory.pools[0].size     = 8;
    g_foundation_config_DynamicMemory.pools[0].count    = 128;

    // pool[1]: [48, 36, 2] => [32, 32, 2]
    g_foundation_config_DynamicMemory.pools[1].size     = 32;
    g_foundation_config_DynamicMemory.pools[1].count    = 32;

    // pool[2]: [96, 20, 2] => [96, 50, 1]
    g_foundation_config_DynamicMemory.pools[2].count    = 50;

    // pool[3]: [268, 10, 1] => [268, 40, 1]
    g_foundation_config_DynamicMemory.pools[3].count    = 40;
}

#ifdef TARGET_HAS_NO_32K_CLOCK
//! PMU LPO structure definitions.
typedef enum PMU_LPO_CLK_SOURCE_TAG
{
    PMU_LPO_CLK_INTERNAL,    // default
    PMU_LPO_CLK_EXTERNAL,
    PMU_LPO_CLK_CRYSTAL,
    PMU_LPO_NO_SELECTED,
    PMU_LPO_32KHZ_OSC,  // for 20735/739 and 43012/4347
    PMU_LPO_LHL_703,    // for 20735/739
    PMU_LPO_LHL_732,    // for 20735/739
    PMU_LPO_LHL_LPO2,   // for 43012/4347
}PMU_LPO_CLK_SOURCE;

typedef enum DEFAULT_ENHANCED_LPO_TAG
{
    // 3 bits for this field!
    PMU_DEFAULT_NO_LHL  = 0,
    PMU_DEFAULT_32kHz   = 1,    // for 20735/739 and 43012/4347
    PMU_DEFAULT_LHL_703 = 2,    // for 20735/739
    PMU_DEFAULT_LHL_LPO2 =2,    // for 43012/4347
    PMU_DEFAULT_LHL_732 = 4,    // for 20735/739
}DEFAULT_ENHANCED_LPO;



extern uint32_t g_aon_flags[];
void wiced_platform_default_lpo_init()
{
    g_aon_flags[0] |= PMU_DEFAULT_LHL_703 | PMU_DEFAULT_LHL_732;
    g_aon_flags[0] &= ~PMU_DEFAULT_32kHz;
}
#endif

/*****************************************************************
 *   Function: spar_setup()
 *
 *   Abstract: Process the information in .secinfo, copying and
 *   clearing sections as needed.
 *****************************************************************/
__attribute__((section(".spar_setup")))
void SPAR_CRT_SETUP(void)
{
    extern void* spar_iram_bss_begin;
    extern unsigned spar_iram_data_length, spar_iram_bss_length;
    extern void* spar_irom_data_begin, *spar_iram_data_begin, *spar_iram_end, *aon_iram_end;

    // Initialize initialized data if .data length is non-zero and it needs to be copied from NV to RAM.
    if(((uint32_t)&spar_irom_data_begin != (uint32_t)&spar_iram_data_begin) && ((uint32_t)&spar_iram_data_length != 0))
        memcpy((void*)&spar_iram_data_begin, (void*)&spar_irom_data_begin, (uint32_t)&spar_iram_data_length);

    // // Clear the ZI section
    if((uint32_t)&spar_iram_bss_length != 0)
    {
        memset((void*)&spar_iram_bss_begin, 0x00, (uint32_t)&spar_iram_bss_length);
    }

    // And move avail memory above this spar if required
    // Note that if there are other spars will be placed with minimum
    // alignment (because of the linker option to IRAM_SPAR_BEGIN) and itself
    // is responsible for moving the avail mem ptr.
    g_dynamic_memory_MinAddress = (void *)(((uint32_t)&spar_iram_end + 32) & 0xFFFFFFF0);
    if (WICED_SLEEP_COLD_BOOT == wiced_sleep_get_boot_mode())
    {
        g_aon_memory_manager_MinAddress = (void *)(&aon_iram_end);
    }
    // Install included libraries and patches if any
    install_libs();

#ifdef BLE_OTA_FW_UPGRADE
    wiced_firmware_upgrade_bootloader();
#endif

    // Setup the application start function.
    wiced_bt_app_pre_init = application_start_internal;

    // Call Application pre-initialization function (either the weak or the real (lib) one)
    wiced_pre_init();

#ifdef TARGET_HAS_NO_32K_CLOCK
    wiced_platform_default_lpo_init();
#endif
}

/* This function is required by the CHIP shell */
unsigned sleep(unsigned int __seconds)
{
    UNUSED_VARIABLE(__seconds);

    wiced_platform_application_thread_event_dispatch();

    return 0;
}

/* This function is required by the abort of libc */
void _exit(int __status)
{
    UNUSED_VARIABLE(__status);

#if ENABLE_DEBUG
    asm("bkpt");
#endif

    while (1);
}

/* This function is required by the abort of libc */
int raise(int sig)
{
    UNUSED_VARIABLE(sig);

    return 0;
}

/* This function is required by the sbrkr of libc */
void *_sbrk(ptrdiff_t incr)
{
    void *prev_heap_end;
    unsigned int flags;

    if (incr < 0) {
        errno = EINVAL;
        return (void *)-1;
    }

    /* Align to the size of pointer */
    incr = (incr + sizeof(void*) - 1) / sizeof(void*) * sizeof(void*);

    flags = _tx_v7m_get_and_disable_int();

    if (g_dynamic_memory_MaxAddressPlusOne < g_dynamic_memory_MinAddress + incr) {
        prev_heap_end = (void *)-1;
        errno = ENOMEM;
    }
    else {
        prev_heap_end = g_dynamic_memory_MinAddress;
        g_dynamic_memory_MinAddress += incr;
    }

    _tx_v7m_set_int(flags);

    return prev_heap_end;
}

/* This function is required by the assert.h */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    printf("ERROR %s:%d: assert", file, line);

    if (failedexpr != NULL) {
        printf("(%s)", failedexpr);
    }

    if (func != NULL) {
        printf("in function '%s'", func);
    }

    printf("\n");

    _tx_v7m_get_and_disable_int();
    wiced_hal_wdog_disable();

#if ENABLE_DEBUG
    asm("bkpt");
#endif

    while (1);
}
