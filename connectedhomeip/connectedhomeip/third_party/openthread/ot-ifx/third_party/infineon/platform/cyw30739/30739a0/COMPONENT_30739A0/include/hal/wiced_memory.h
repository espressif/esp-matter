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
 *  \addtogroup wiced_mem Memory Management
 *  \ingroup wicedsys
 *
 *  @{
 * AIROC Memory Management Interface
 */

#pragma once

#include "wiced_gki.h"

/** wiced buffer pool */
typedef struct wiced_pool_t wiced_bt_buffer_pool_t;

#pragma pack(1)

/** wiced bt dynamic buffer statistics */
typedef struct
{
    uint8_t     pool_id;                    /**< pool id */
    uint16_t    pool_size;                  /**< pool buffer size */
    uint16_t    current_allocated_count;    /**< number of  buffers currently allocated */
    uint16_t    max_allocated_count;        /**< maximum number of buffers allocated at any time */
    uint16_t    total_count;                /**< total number of buffers */
}wiced_bt_buffer_statistics_t;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_memory_get_free_bytes
 *
 *                  Returns the number of free bytes of RAM left
 *
 * @return          the number of free bytes of RAM left
 */
uint32_t wiced_memory_get_free_bytes( void );

/**
 * Function         wiced_memory_set_application_thread_stack_size
 *
 *                  Update the stack size of the application thread (MPAF)
 *
 *                  NOTE : This API will work only when invoked from SPAR_CRT_SETUP()
 *
 * @param[in]       new_stack_size - required size of the stack.
 *
 * @return          TRUE on success else FALSE
 */
wiced_bool_t wiced_memory_set_application_thread_stack_size(uint16_t new_stack_size);

/**
 * Function         wiced_bt_create_pool
 *
 *                  Creates a private buffer pool dedicated for the application usage.
 *
 * @param[in]       buffer_size          : size of the buffers in the pool
 *
 * @param[in]       buffer_cnt           : number of buffers in the pool
 *
 * @return          pointer to the created pool on success
 *                  NULL on failure
 *
 * Note :           The max number of buffer pools configured in
 *                  wiced_bt_cfg_settings_t.max_number_of_buffer_pools
 *                  must be increased for each buffer pool the application creates.
 */
wiced_bt_buffer_pool_t* wiced_bt_create_pool( uint32_t buffer_size, uint32_t buffer_cnt );

/**
 * Function         wiced_bt_get_buffer_from_pool
 *
 *                  Allocates a buffer from the private pool. Pass the pool pointer to
 *                  get the buffer from the desired pool
 *
 * @param[in]       p_pool           : pool pointer
 *
 * @return          the pointer to the buffer
 *                  NULL on failure
 */
void* wiced_bt_get_buffer_from_pool( wiced_bt_buffer_pool_t* p_pool );

/**
 * Function         wiced_bt_get_buffer_count
 *
 *                  To get the number of buffers available in the pool
 *
 * @param[in]       p_pool           : pool pointer
 *
 * @return          the number of buffers available in the pool
 */
uint32_t wiced_bt_get_buffer_count( wiced_bt_buffer_pool_t* p_pool );

/**
 * Function         wiced_bt_get_buffer
 *
 *                  Allocates a buffer from the public pools. Public pools shall be specified
 *                  using wiced_bt_cfg_buf_pool_t from the application.
 *
 * @param[in]       buffer_size           : size of the buffer
 *
 * @return          the pointer to the buffer
 *                  NULL on failure
 */
void* wiced_bt_get_buffer( uint16_t buffer_size );

/**
 * Function         wiced_bt_free_buffer
 *
 *                  Frees the buffer
 *
 * @param[in]       p_buf           : pointer to the start of the buffer to be freed
 *
 * @return          None
 */
void wiced_bt_free_buffer( void* p_buf );

/**
 * Function         wiced_bt_get_buffer_size
 *
 *                  Gets the buffer size
 *
 * @param[in]       p_buf           : pointer to the start of the buffer
 *
 * @return          the buffer size
 */
uint32_t wiced_bt_get_buffer_size( void* p_buf );

/**
 * Function         wiced_bt_get_buffer_usage
 *
 *                  Dumps dynamic buffer usage (see #wiced_bt_buffer_statistics_t),
 *                  from the last start of the system.
 *
 * @param[in]       p_buffer_stat - pointer buffer to fill statistics.
 * @param[in]       size - size of the memory to get the statistics.
 *
 * @return          WICED_BT_SUCCESS on success else error
 */
wiced_result_t wiced_bt_get_buffer_usage ( wiced_bt_buffer_statistics_t *p_buffer_stat, uint16_t size );

/**
 * Function         wiced_memory_allocate
 *
 *                  Allocates memory from Dynamic Memory pools
 *
 * @param[in]       size           :size of the memory to be allocated
 *
 * @return          pointer to the allocated memory on success
 *                  NULL on failure
 */
void* wiced_memory_allocate( uint32_t size );

/**
 * Function         wiced_memory_free
 *
 *                  Frees the buffer
 *
 * @param[in]       p_buf           : free memory allocated from Dynamic Memory pools
 *
 * @return          None
 */
void wiced_memory_free( void *memoryBlock );

/** Get specific buffer pool utilization
 *
 * @param[in]       pool_id     : (input) pool ID
 *
 * @return          % of buffers used from 0 to 100
 */
uint16_t wiced_bt_buffer_poolutilization (uint8_t pool_id);

/******************************************************************************
* Function Name: wiced_memory_permanent_allocate
***************************************************************************//**
*
* Allocates memory for permanent usage.
*
* @param[in]        size            : The size of the memory to be allocated
*
* @return           The pointer to the allocated memory on success
*                   NULL on failure
*
******************************************************************************/
void* wiced_memory_permanent_allocate( uint32_t size );

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif
