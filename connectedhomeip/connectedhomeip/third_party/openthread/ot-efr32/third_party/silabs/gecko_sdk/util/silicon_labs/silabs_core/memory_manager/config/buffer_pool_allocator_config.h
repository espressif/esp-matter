/***************************************************************************//**
 * @file buffer_pool_allocator_config.h
 * @brief Configuration file for buffer pool allocator.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#ifndef __BUFFER_POOL_ALLOCATOR_CONFIG_H__
#define __BUFFER_POOL_ALLOCATOR_CONFIG_H__

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Memory Manager Configuration

// <o BUFFER_POOL_ALLOCATOR_POOL_SIZE> Number Buffer Pools
// <0-256:1>
// <i> Default: 5
#define BUFFER_POOL_ALLOCATOR_POOL_SIZE  5

// <o BUFFER_POOL_ALLOCATOR_BUFFER_SIZE_MAX> Length of Each Buffer Pool
// <0-1024:1>
// <i> Default: 256
#define BUFFER_POOL_ALLOCATOR_BUFFER_SIZE_MAX  256

// <q BUFFER_POOL_ALLOCATOR_CLEAR_ON_INIT> Clear Each Newly Allocated Buffer
#define BUFFER_POOL_ALLOCATOR_CLEAR_ON_INIT  0

// </h>
// <<< end of configuration section >>>

#endif // __BUFFER_POOL_ALLOCATOR_CONFIG_H__