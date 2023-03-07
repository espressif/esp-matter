/*
 * $ Copyright 2016-YEAR Cypress Semiconductor $
 */

/**
 * @file
 *
 * GigaDevice Dual and Quad Serial Flash - GD25WQ64E
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define GW25WQ64E_BLOCKS            128
#define GW25WQ64E_SECTORS_PER_BLCOK 16
#define GW25WQ64E_PAGES_PER_SECTOR  16
#define GW25WQ64E_BYTES_PER_PAGE    256 /**< Each page size in bytes. */
#define GD25WQ64E_SIZE              (GW25WQ64E_BLOCKS * \
                                     GW25WQ64E_SECTORS_PER_BLCOK * \
                                     GW25WQ64E_PAGES_PER_SECTOR * \
                                     GW25WQ64E_BYTES_PER_PAGE)  /**< Flash Size in bytes. */

/**
 * Commands
 */
#define GD25WQ64E_WRITE_ENABLE                              0x06
#define GD25WQ64E_WRITE_DISABLE                             0x04
#define GD25WQ64E_READ_STATUS_REGISTER_1                    0x05
#define GD25WQ64E_READ_STATUS_REGISTER_2                    0x35
#define GD25WQ64E_READ_STATUS_REGISTER_3                    0x15
#define GD25WQ64E_WRITE_STATUS_REGISTER_1                   0x01
#define GD25WQ64E_WRITE_STATUS_REGISTER_2                   0x31
#define GD25WQ64E_WRITE_STATUS_REGISTER_3                   0x11
#define GD25WQ64E_VOLATILE_SR_WRITE_ENABLE                  0x50
#define GD25WQ64E_READ_DATA                                 0x03
#define GD25WQ64E_FAST_READ                                 0x0b
#define GD25WQ64E_DUAL_OUTPUT_FAST_READ                     0x3b
#define GD25WQ64E_QUAD_OUTPUT_FAST_READ                     0x6b
#define GD25WQ64E_DUAL_IO_FAST_READ                         0xbb
#define GD25WQ64E_QUAD_IO_FAST_READ                         0xeb
#define GD25WQ64E_SET_BURST_WITH_WRAP                       0x77
#define GD25WQ64E_PAGE_PROGRAM                              0x02
#define GD25WQ64E_QUAD_PAGE_PROGRAM                         0x32
#define GD25WQ64E_SECTOR_ERASE                              0x20
#define GD25WQ64E_BLOCK_ERASE_32K                           0x52
#define GD25WQ64E_BLOCK_ERASE_64K                           0xd8
#define GD25WQ64E_CHIP_ERASE                                0xc7
#define GD25WQ64E_READ_MANUFACTURE_DEVICE_ID                0x90
#define GD25WQ64E_READ_IDENTIFICATION                       0x9f
#define GD25WQ64E_READ_UNIQUE_ID                            0x4b
#define GD25WQ64E_ERASE_SECURITY_REGISTERS                  0x44
#define GD25WQ64E_PROGRAM_SECURITY_REGISTERS                0x42
#define GD25WQ64E_READ_SECURITY_REGISTERS                   0x48
#define GD25WQ64E_ENABLE_RESET                              0x66
#define GD25WQ64E_RESET                                     0x99
#define GD25WQ64E_PROGRAM_ERASE_SUSPEND                     0x75
#define GD25WQ64E_PROGRAM_ERASE_RESUME                      0x7a
#define GD25WQ64E_DEEP_POWER_DOWN                           0xb9
#define GD25WQ64E_RELEASE_FROME_DEEP_POWER_DOWN             0xab
#define GD25WQ64E_READ_SERIAL_FLASH_DISCOVERABLE_PARAMETER  0x5a


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/


/******************************************************
 *                    Structures
 ******************************************************/
/* Status Register 1 */
typedef struct
{
    union
    {
        struct
        {
            uint8_t wip     :   1;  /* Erase/Write In Progress */
            uint8_t wel     :   1;  /* Write Enable Latch */
            uint8_t bp0     :   1;  /* Block Protect Bit */
            uint8_t bp1     :   1;  /* Block Protect Bit */
            uint8_t bp2     :   1;  /* Block Protect Bit */
            uint8_t bp3     :   1;  /* Block Protect Bit */
            uint8_t bp4     :   1;  /* Block Protect Bit */
            uint8_t srp0    :   1;  /* Status Register Protection Bit */
        } field;

        uint8_t value;
    };
} GD25WQ64E_SR_1_t;

/* Status Register 2 */
typedef struct
{
    union
    {
        struct
        {
            uint8_t srp1    :   1;  /* Status Register Protection Bit */
            uint8_t qe      :   1;  /* Quad Enable Bit */
            uint8_t sus2    :   1;  /* Program Suspend Bit */
            uint8_t lb1     :   1;  /* Security Register Lock Bit */
            uint8_t lb2     :   1;  /* Security Register Lock Bit */
            uint8_t lb3     :   1;  /* Security Register Lock Bit */
            uint8_t cmp     :   1;  /* Complement Protect Bit */
            uint8_t sus1    :   1;  /* Erase Suspend Bit */
        } field;

        uint8_t value;
    };
} GD25WQ64E_SR_2_t;

/* Status Register 3 */
typedef struct
{
    union
    {
        struct
        {
            uint8_t dc          :   1;  /* Dummy Configuration Bit */
            uint8_t reserved    :   4;  /* Reserved */
            uint8_t drv0        :   1;  /* Output Driver Strength Bit */
            uint8_t drv1        :   1;  /* Output Driver Strength Bit */
            uint8_t reserved_1  :   1;  /* Reserved */
        } field;

        uint8_t value;
    };
} GD25WQ64E_SR_3_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


#ifdef __cplusplus
} /*extern "C" */
#endif
