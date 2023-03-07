/***************************************************************************//**
 * @brief Zigbee OTA Cluster Platform Bootloader component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee OTA Cluster Platform Bootloader configuration

// <q EMBER_AF_PLUGIN_OTA_BOOTLOAD_UART_HOST_REBOOT> UART Host Reboot after NCP bootload
// <i> Default: TRUE
// <i> This option will trigger the UART host to reboot.  On Unix hosts this will trigger the process to terminate.  Disabling this option will allow the UART host to continue running and just reset the NCP.  SPI Hosts are always rebooted.
#define EMBER_AF_PLUGIN_OTA_BOOTLOAD_UART_HOST_REBOOT   1

// </h>

// <<< end of configuration section >>>
