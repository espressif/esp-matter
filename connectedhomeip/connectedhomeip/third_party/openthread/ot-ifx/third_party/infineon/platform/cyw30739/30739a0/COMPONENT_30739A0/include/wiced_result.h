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

/** @file
 *
 * Generic types
 *
 */
#ifndef __WICED_RESULT_H
#define __WICED_RESULT_H
#pragma once
#include "wiced_bt_constants.h"

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */

/*************************************************************************/
/**
 *  @addtogroup  Result         AIROC Result Codes
 *  @ingroup     wicedsys
 *
 *  <b> AIROC Result list </b> for Bluetooth BR/EDR and LE @b .
 *
 *  @{
 */
/*************************************************************************/

/** AIROC result list */
#define WICED_RESULT_LIST( prefix ) \
    RESULT_ENUM( prefix, SUCCESS,                       0x00 ),   /**< Success */                        \
    RESULT_ENUM( prefix, DELETED                       ,0x01 ),   \
    RESULT_ENUM( prefix, NO_MEMORY                     ,0x10 ),   \
    RESULT_ENUM( prefix, POOL_ERROR                    ,0x02 ),   \
    RESULT_ENUM( prefix, PTR_ERROR                     ,0x03 ),   \
    RESULT_ENUM( prefix, WAIT_ERROR                    ,0x04 ),   \
    RESULT_ENUM( prefix, SIZE_ERROR                    ,0x05 ),   \
    RESULT_ENUM( prefix, GROUP_ERROR                   ,0x06 ),   \
    RESULT_ENUM( prefix, NO_EVENTS                     ,0x07 ),   \
    RESULT_ENUM( prefix, OPTION_ERROR                  ,0x08 ),   \
    RESULT_ENUM( prefix, QUEUE_ERROR                   ,0x09 ),   \
    RESULT_ENUM( prefix, QUEUE_EMPTY                   ,0x0A ),   \
    RESULT_ENUM( prefix, QUEUE_FULL                    ,0x0B ),   \
    RESULT_ENUM( prefix, SEMAPHORE_ERROR               ,0x0C ),   \
    RESULT_ENUM( prefix, NO_INSTANCE                   ,0x0D ),   \
    RESULT_ENUM( prefix, THREAD_ERROR                  ,0x0E ),   \
    RESULT_ENUM( prefix, PRIORITY_ERROR                ,0x0F ),   \
    RESULT_ENUM( prefix, START_ERROR                   ,0x10 ),   \
    RESULT_ENUM( prefix, DELETE_ERROR                  ,0x11 ),   \
    RESULT_ENUM( prefix, RESUME_ERROR                  ,0x12 ),   \
    RESULT_ENUM( prefix, CALLER_ERROR                  ,0x13 ),   \
    RESULT_ENUM( prefix, SUSPEND_ERROR                 ,0x14 ),   \
    RESULT_ENUM( prefix, TIMER_ERROR                   ,0x15 ),   \
    RESULT_ENUM( prefix, TICK_ERROR                    ,0x16 ),   \
    RESULT_ENUM( prefix, ACTIVATE_ERROR                ,0x17 ),   \
    RESULT_ENUM( prefix, THRESH_ERROR                  ,0x18 ),   \
    RESULT_ENUM( prefix, SUSPEND_LIFTED                ,0x19 ),   \
    RESULT_ENUM( prefix, WAIT_ABORTED                  ,0x1A ),   \
    RESULT_ENUM( prefix, WAIT_ABORT_ERROR              ,0x1B ),   \
    RESULT_ENUM( prefix, MUTEX_ERROR                   ,0x1C ),   \
    RESULT_ENUM( prefix, NOT_AVAILABLE                 ,0x1D ),   \
    RESULT_ENUM( prefix, NOT_OWNED                     ,0x1E ),   \
    RESULT_ENUM( prefix, INHERIT_ERROR                 ,0x1F ),   \
    RESULT_ENUM( prefix, NOT_DONE                      ,0x20 ),   \
    RESULT_ENUM( prefix, CEILING_EXCEEDED              ,0x21 ),   \
    RESULT_ENUM( prefix, INVALID_CEILING               ,0x22 ),   \
    RESULT_ENUM( prefix, STA_JOIN_FAILED               ,0x23),   /**< Join failed */\
    RESULT_ENUM( prefix, SLEEP_ERROR                   ,0x24),   \
    RESULT_ENUM( prefix, PENDING,                       0x25),   /**< Pending */                        \
    RESULT_ENUM( prefix, TIMEOUT,                       0x26),   /**< Timeout */                        \
    RESULT_ENUM( prefix, PARTIAL_RESULTS,               0x27),   /**< Partial results */                \
    RESULT_ENUM( prefix, ERROR,                         0x28),   /**< Error */                          \
    RESULT_ENUM( prefix, BADARG,                        0x29),   /**< Bad Arguments */                  \
    RESULT_ENUM( prefix, BADOPTION,                     0x2A),   /**< Mode not supported */             \
    RESULT_ENUM( prefix, UNSUPPORTED,                   0x2B),   /**< Unsupported function */           \
    RESULT_ENUM( prefix, OUT_OF_HEAP_SPACE,             0x2C),   /**< Dynamic memory space exhausted */ \
    RESULT_ENUM( prefix, NOTUP,                         0x2D),   /**< Interface is not currently Up */  \
    RESULT_ENUM( prefix, UNFINISHED,                    0x2E),   /**< Operation not finished yet */     \
    RESULT_ENUM( prefix, CONNECTION_LOST,               0x2F),   /**< Connection to server lost */      \
    RESULT_ENUM( prefix, NOT_FOUND,                     0x30),   /**< Item not found */                 \
    RESULT_ENUM( prefix, PACKET_BUFFER_CORRUPT,         0x31),   /**< Packet buffer corrupted */        \
    RESULT_ENUM( prefix, ROUTING_ERROR,                 0x32),   /**< Routing error */                  \
    RESULT_ENUM( prefix, BADVALUE,                      0x33),   /**< Bad value */                      \
    RESULT_ENUM( prefix, WOULD_BLOCK,                   0x34),   /**< Function would block */           \
    RESULT_ENUM( prefix, ABORTED,                       0x35),   /**< Operation aborted */              \
    RESULT_ENUM( prefix, CONNECTION_RESET,              0x36),   /**< Connection has been reset */      \
    RESULT_ENUM( prefix, CONNECTION_CLOSED,             0x37),   /**< Connection is closed */           \
    RESULT_ENUM( prefix, NOT_CONNECTED,                 0x38),   /**< Connection is not connected */    \
    RESULT_ENUM( prefix, ADDRESS_IN_USE,                0x39),   /**< Address is in use */              \
    RESULT_ENUM( prefix, NETWORK_INTERFACE_ERROR,       0x3A),   /**< Network interface error */        \
    RESULT_ENUM( prefix, ALREADY_CONNECTED,             0x3B),   /**< Socket is already connected */    \
    RESULT_ENUM( prefix, INVALID_INTERFACE,             0x3C),   /**< Interface specified in invalid */ \
    RESULT_ENUM( prefix, SOCKET_CREATE_FAIL,            0x3D),   /**< Socket creation failed */         \
    RESULT_ENUM( prefix, INVALID_SOCKET,                0x3E),   /**< Socket is invalid */              \
    RESULT_ENUM( prefix, CORRUPT_PACKET_BUFFER,         0x3F),   /**< Packet buffer is corrupted */     \
    RESULT_ENUM( prefix, UNKNOWN_NETWORK_STACK_ERROR,   0x40),   /**< Unknown network stack error */    \
    RESULT_ENUM( prefix, NO_STORED_AP_IN_DCT,           0x41),   /**< DCT contains no AP credentials */ \
    RESULT_ENUM( prefix, ALREADY_INITIALIZED,           0x42),   /**< Already initialized*/ \
    RESULT_ENUM( prefix, FEATURE_NOT_ENABLED           ,0xFF ),   \

/**@}  AIROC result list */

#define FILESYSTEM_RESULT_LIST( prefix ) \
        RESULT_ENUM( prefix, SUCCESS,                   10000 ),   /**< Success */               \
        RESULT_ENUM( prefix, PENDING,                   10001 ),   /**< Pending */               \
        RESULT_ENUM( prefix, TIMEOUT,                   10002 ),   /**< Timeout */               \
        RESULT_ENUM( prefix, PARTIAL_RESULTS,           10003 ),   /**< Partial results */       \
        RESULT_ENUM( prefix, ERROR,                     10004 ),   /**< Error */                 \
        RESULT_ENUM( prefix, BADARG,                    10005 ),   /**< Bad Arguments */         \
        RESULT_ENUM( prefix, BADOPTION,                 10006 ),   /**< Mode not supported */    \
        RESULT_ENUM( prefix, UNSUPPORTED,               10007 ),   /**< Unsupported function */  \
        RESULT_ENUM( prefix, DISK_ERROR,                10008 ),   /**< Low level error accessing media */        \
        RESULT_ENUM( prefix, PATH_NOT_FOUND,            10009 ),   /**< Path was not found in filesystem */        \
        RESULT_ENUM( prefix, MEDIA_NOT_READY,           10010 ),   /**< Media is not present or ready for access */          \
        RESULT_ENUM( prefix, ACCESS_DENIED,             10011 ),   /**< Access denied due to permissions  */      \
        RESULT_ENUM( prefix, WRITE_PROTECTED,           10012 ),   /**< Media is write protected */    \
        RESULT_ENUM( prefix, OUT_OF_SPACE,              10013 ),   /**< No free space left on media  */ \
        RESULT_ENUM( prefix, FILENAME_BUFFER_TOO_SMALL, 10014 ),   /**< Filename buffer was too small when retrieving directory contents  */ \
        RESULT_ENUM( prefix, END_OF_RESOURCE,           10015 ),   /**< End of file/directory reached  */ \
        RESULT_ENUM( prefix, FILESYSTEM_INVALID,        10016 ),   /**< Filesystem has an unrecoverable error */ \
        RESULT_ENUM( prefix, BLOCK_SIZE_BAD,            10017 ),   /**< Block size is invalid - not a multiple or sub-multiple of DEFAULT_SECTOR_SIZE */

/*************************************************************************/
/**
 *  @addtogroup  Result       AIROC Result Codes
 *  @ingroup     wicedsys
 *
 *  <b> Result types </b> @b .
 *
 *  @{
 */
/*************************************************************************/
/** AIROC result */
typedef enum
{
    WICED_RESULT_LIST(WICED_)                 /**< Generic Error */
    BT_RESULT_LIST      (  WICED_BT_       )  /**< BT specific Error. Range: 8000 - 8999 */
    FILESYSTEM_RESULT_LIST( WICED_FILESYSTEM_ )      /* 10000 - 10999 */
} wiced_result_t;

/**@}  AIROC Result */

/******************************************************
 *            Structures
 ******************************************************/

/******************************************************
 *            Function Declarations
 ******************************************************/

#endif
