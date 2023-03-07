/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader internal storage
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef BTL_INTERNAL_STORAGE_CONFIG_H
#define BTL_INTERNAL_STORAGE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Bootloader Internal Storage Configuration

// <q BOOTLOADER_MSC_DMA_WRITE> Enable DMA-based MSC write
// <i> Default: 0
// <i> Enable DMA-based MSC write by reserving a DMA channel. DMA-based MSC write will enhance the speed of the MSC operations.
// <i> Be cautious to NOT use the interface storage APIs for writing data from flash to flash with this option enabled on Series-1 devices.
#define BOOTLOADER_MSC_DMA_WRITE 0

// <o BOOTLOADER_MSC_DMA_CHANNEL> DMA channel to reserve
// <i> Default: 2
// <i> Reserve a DMA channel for DMA-based MSC write. The reserved DMA channel should not be shared with the applications.
#define BOOTLOADER_MSC_DMA_CHANNEL 2


// </h>
// <<< end of configuration section >>>


#endif // BTL_INTERNAL_STORAGE_CONFIG_H
