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

//==================================================================================================
//! \file
//!
//! BCM20739 DMA channel map.
//
//==================================================================================================


#if DMA_NUM_CHANNELS != 5
#error "Mapping below is inconsistent with DMA_NUM_CHANNELS in the chip_features.xml spreadsheet"
#endif


//==================================================================================================
// Channel assignments and sharing property declarations

//! Flag indicating that DMA channel 0, used by the Bluetooth baseband, is not shared.
#define DMA_CHANNEL_0_SHARED        FALSE

//! DMA channel assignment for the Bluetooth baseband.
#define DMA_CHANNEL_BT_BASEBAND     0

//! Flag indicating that DMA channel 1 is shared.  It is used by a variety of transports for device
//! to host data.
#define DMA_CHANNEL_1_SHARED        FALSE

//! DMA channel assignment for UART transmit (device to host).
#define DMA_CHANNEL_UART_TX         1

//! DMA channel assignment for UART receive (host to device).
#define DMA_CHANNEL_UART_RX         2

//! DMA channel assignment for USB receive (host to device).
#define DMA_CHANNEL_USB_RX          2

//! Flag indicating that DMA channel 2 is shared.  It is used by a variety of transports for host to
//! device data.
#define DMA_CHANNEL_2_SHARED        FALSE

//! DMA channel assignment for USB transmit (device to host).
#define DMA_CHANNEL_USB_TX          1

//! TODO: assign channel 3, determine whether it is shared.
#define DMA_CHANNEL_3_SHARED        FALSE

//! DMA channel assignment for ADC Audio receive
#define DMA_CHANNEL_ADC_AUDIO       3

//! DMA channel assignment for ADC Audio receive
#define DMA_CHANNEL2_ADC_AUDIO      4


//==================================================================================================

//! Peripheral index in the DMA controller for the PTU as DMA source.  This is the index at which
//! the Peripheral Transport Unit's DMA request lines are connected for transfers from PTU to memory
//! (receive).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in the
//! dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_PTU_TO_MEMORY     0

//! Peripheral index in the DMA controller for the PTU as DMA destination.  This is the index at
//! which the Peripheral Transport Unit's DMA request lines are connected for transfers from memory
//! to PTU (transmit).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in
//! the dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_MEMORY_TO_PTU     1

//! Peripheral index in the DMA controller for the Bluetooth baseband.  This is the index at which
//! the its DMA request lines are connected for transfers either to or from memory (receive or
//! transmit).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in the
//! dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_BT_BASEBAND             3
