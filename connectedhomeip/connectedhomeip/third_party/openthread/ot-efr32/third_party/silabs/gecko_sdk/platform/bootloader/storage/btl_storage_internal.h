/***************************************************************************//**
 * @file
 * @brief Internal header for the storage component of the Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef BTL_STORAGE_INTERNAL_H
#define BTL_STORAGE_INTERNAL_H

#include <stddef.h>
#include <stdbool.h>

#include "api/btl_interface.h"
#include "btl_storage.h"

/// Size of the buffer used for chunked reads from storage
#define BTL_STORAGE_READ_BUFFER_SIZE            (128UL)

/// Storage layout information for the storage implementation.
extern const BootloaderStorageLayout_t storageLayout;

/***************************************************************************//**
 * Get information about the connected storage device.
 *
 * @return Pointer to device information.
 ******************************************************************************/
BootloaderStorageImplementationInformation_t getDeviceInfo(void);

/***************************************************************************//**
 * Get base address of the storage medium to use for bootloader storage.
 *
 * @return The base address of the storage medium.
 ******************************************************************************/
uint32_t storage_getBaseAddress(void);

/***************************************************************************//**
 * Custom storage init function.
 *
 * This function can be implemented by the user to perform extra operations
 * during initialization of the storage component, such as turning on a GPIO power
 * pin to an external storage medium, toggling a write protect pin, etc.
 * The custom init function is called by @ref storage_init.
 ******************************************************************************/
void storage_customInit(void);

/***************************************************************************//**
 * Custom storage shutdown function.
 *
 * This function can be implemented by the user to perform extra operations
 * during shutdown of the storage component, such as turning off a GPIO power
 * pin to an external storage medium.
 * The custom shutdown function is called by @ref storage_shutdown.
 ******************************************************************************/
void storage_customShutdown(void);

#endif // BTL_STORAGE_INTERNAL_H
