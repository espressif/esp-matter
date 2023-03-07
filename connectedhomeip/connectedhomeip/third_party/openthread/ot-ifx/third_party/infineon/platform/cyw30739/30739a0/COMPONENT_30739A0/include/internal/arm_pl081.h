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

//! \file
//!
//! Header for the ARM PL-081 DMA controller.  There is no dma.h because any generic interface to
//! "any" DMA controller would have to be the least common denominator, mitigating any benefit that
//! may have motivated a switch to a different controller.
//
//==================================================================================================

#ifndef _ARM_PL081_H_
#define _ARM_PL081_H_


#include "brcm_fw_types.h"
#include "dmachannelmap.h"


//==================================================================================================
// Types and constants
//==================================================================================================

//! Function pointer type used in DMA_TRANSFER_REQUEST_t for DMA transfer complete notification.  It
//! is called when a DMA transfer has completed.
typedef void (*fp_DoneInterrupt_t)(void* arg);

//! DMA linked list item, in exactly the format used by the DMA controller.  That is the format used
//! by the controller when it dereferences an address in its DMACCxLLI register.  Even when
//! DMACCxLLI is 0 (no linked list) at the start of a single-segment DMA transfer, this structure is
//! used to represent the intended values for the DMACCxSrcAddr, DMACCxDestAddr, DMACCxLLI, and
//! DMACCxControl registers in the controller.  Please refer to the ARM PL081 DMA controller
//! documentation, which is more comprehensive than what needs to be presented here.
typedef struct DMA_LLI
{
    //! The source address from which the data transferred by the DMA controller will be read.  The
    //! pointer can be incremented with each 8, 16, or 32 bits, depending on the SWidth and SI
    //! fields in DMACCxControl.
    const void*                                                     DMACCxSrcAddr;

    //! The destination address to which the data transferred by the DMA controller will be written.
    //! The pointer can be incremented with each 8, 16, or 32 bits, depending on the SWidth and SI
    //! fields in DMACCxControl.
    void*                                                           DMACCxDestAddr;

    //! Pointer to the next linked list item, to be processed by the DMA controller.  It contains
    //! the next set of channel control register values for as an additional segment to be
    //! transferred when this segment is done.
    struct DMA_LLI*                                                 DMACCxLLI;

    //! Please refer to the ARM PL081 DMA controller documentation.  That documentation is more
    //! comprehensive than what can be presented here.  As for this documentation, please refer to
    //! the defines for flags and masks with names beginning in DMACCXCONTROL_.
    UINT32                                                          DMACCxControl;
        //! Mask or value for the I (Terminal count interrupt enable) bit in DMACCxControl.  That is
        //! a register in a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_DMA_DONE_INTERRUPT_ENABLE                 0x80000000

        //! Mask or value for the cacheable bit in the Prot (Protection) field in DMACCxControl.
        //! That is a register in a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_PROT_CACHEABLE                            0x40000000

        //! Mask or value for the bufferable bit in the Prot (Protection) field in DMACCxControl.
        //! That is a register in a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_PROT_BUFFERABLE                           0x20000000

        //! Mask or value for the privileged bit in the Prot (Protection) field in DMACCxControl.
        //! That is a register in a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_PROT_PRIVILEGED                           0x10000000

        //! Mask or value for the DI (Destination increment) bit in DMACCxControl.  That is a
        //! register in a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_DEST_INCREMENT                            0x08000000

        //! Mask or value for the SI (Source increment) bit in DMACCxControl.  That is a register in
        //! a DMA controller channel register bank or field in DMA_LLI_t.
        #define DMACCXCONTROL_SRC_INCREMENT                             0x04000000

        //! Mask for the DWidth (Destination width) field in DMACCxControl.  That is a register in a
        //! DMA controller channel register bank or field in DMA_LLI_t.  The value must be one of
        //! the following constants: DMACCXCONTROL_DEST_WIDTH_8_BITS,
        //! DMACCXCONTROL_DEST_WIDTH_16_BITS, or DMACCXCONTROL_DEST_WIDTH_32_BITS.
        #define DMACCXCONTROL_DEST_WIDTH_MASK                           0x00e00000

        //! Value specifying a destination width of 8 bits per write in the DWidth.  That is a field
        //! in DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_DEST_WIDTH_8_BITS                         0x00000000

        //! Value specifying a destination width of 16 bits per write in the DWidth.  That is a
        //! field in DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_DEST_WIDTH_16_BITS                        0x00200000

        //! Value specifying a destination width of 32 bits per write in the DWidth.  That is a
        //! field in DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_DEST_WIDTH_32_BITS                        0x00400000

        //! Mask for the SWidth (Source width) field in DMACCxControl.  That is a register in a DMA
        //! controller channel register bank or field in DMA_LLI_t.  The value must be one of the
        //! following constants: DMACCXCONTROL_SRC_WIDTH_8_BITS, DMACCXCONTROL_SRC_WIDTH_16_BITS, or
        //! DMACCXCONTROL_SRC_WIDTH_32_BITS
        #define DMACCXCONTROL_SRC_WIDTH_MASK                            0x001c0000

        //! Value specifying a source width of 8 bits per read in SWidth.  That is a field in
        //! DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_SRC_WIDTH_8_BITS                          0x00000000

        //! Value specifying a source width of 16 bits per read in SWidth.  That is a field in
        //! DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_SRC_WIDTH_16_BITS                         0x00040000

        //! Value specifying a source width of 32 bits per read in SWidth.  That is a field in
        //! DMACCxControl, in a DMA controller channel register bank, or DMA_LLI_t.
        #define DMACCXCONTROL_SRC_WIDTH_32_BITS                         0x00080000

        //! Mask for the DBSize (Destination burst size) field in DMACCxControl.  That is a register
        //! in a DMA controller channel register bank or field in DMA_LLI_t.  The value must be one
        //! of the following constants: DMACCXCONTROL_DEST_BURST_SIZE_1_TRANSFER,
        //! DMACCXCONTROL_DEST_BURST_SIZE_4_TRANSFERS, DMACCXCONTROL_DEST_BURST_SIZE_8_TRANSFERS,
        //! DMACCXCONTROL_DEST_BURST_SIZE_16_TRANSFERS, DMACCXCONTROL_DEST_BURST_SIZE_32_TRANSFERS,
        //! DMACCXCONTROL_DEST_BURST_SIZE_64_TRANSFERS, DMACCXCONTROL_DEST_BURST_SIZE_128_TRANSFERS,
        //! or DMACCXCONTROL_DEST_BURST_SIZE_256_TRANSFERS.
        #define DMACCXCONTROL_DEST_BURST_SIZE_MASK                      0x00038000

        //! Value specifying a destination burst size of 1 transfer of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_1_TRANSFER                0x00000000

        //! Value specifying a destination burst size of 4 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_4_TRANSFERS               0x00008000

        //! Value specifying a destination burst size of 8 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_8_TRANSFERS               0x00010000

        //! Value specifying a destination burst size of 16 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_16_TRANSFERS              0x00018000

        //! Value specifying a destination burst size of 32 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_32_TRANSFERS              0x00020000

        //! Value specifying a destination burst size of 64 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_64_TRANSFERS              0x00028000

        //! Value specifying a destination burst size of 128 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_128_TRANSFERS             0x00030000

        //! Value specifying a destination burst size of 256 transfers of DWidth.  This value is
        //! used in the DBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_DEST_BURST_SIZE_256_TRANSFERS             0x00038000

        //! Mask for the SBSize (Source burst size) field in DMACCxControl.  That is a register in
        //! a DMA controller channel register bank or field in DMA_LLI_t.  The value must be one of
        //! the following constants: DMACCXCONTROL_SRC_BURST_SIZE_1_TRANSFER,
        //! DMACCXCONTROL_SRC_BURST_SIZE_4_TRANSFERS, DMACCXCONTROL_SRC_BURST_SIZE_8_TRANSFERS,
        //! DMACCXCONTROL_SRC_BURST_SIZE_16_TRANSFERS, DMACCXCONTROL_SRC_BURST_SIZE_32_TRANSFERS,
        //! DMACCXCONTROL_SRC_BURST_SIZE_64_TRANSFERS, DMACCXCONTROL_SRC_BURST_SIZE_128_TRANSFERS,
        //! or DMACCXCONTROL_SRC_BURST_SIZE_256_TRANSFERS.
        #define DMACCXCONTROL_SRC_BURST_SIZE_MASK                       0x00007000

        //! Value specifying a source burst size of 1 transfer of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_1_TRANSFER                 0x00000000

        //! Value specifying a source burst size of 4 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_4_TRANSFERS                0x00001000

        //! Value specifying a source burst size of 8 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_8_TRANSFERS                0x00002000

        //! Value specifying a source burst size of 16 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_16_TRANSFERS               0x00003000

        //! Value specifying a source burst size of 32 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_32_TRANSFERS               0x00004000

        //! Value specifying a source burst size of 64 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_64_TRANSFERS               0x00005000

        //! Value specifying a source burst size of 128 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_128_TRANSFERS              0x00006000

        //! Value specifying a source burst size of 256 transfers of SWidth.  This value is used in
        //! the SBSize field in DMACCxControl, in a DMA controller channel register bank, or
        //! DMA_LLI_t.
        #define DMACCXCONTROL_SRC_BURST_SIZE_256_TRANSFERS              0x00007000

        //! Mask for the TransferSize (transfer size) field in DMACCxControl.  That is a register in
        //! a DMA controller channel register bank or field in DMA_LLI_t.  Because this field
        //! occupies bits [11:0] in DMACCxControl, the number of transfers can be used directly,
        //! unshifted, in this field, but that number cannot have more than twelve bits.  Program
        //! the transfer size value to zero if the SMDMAC is not the flow controller (if the
        //! peripheral provides flow control and signals the DMA controller when a transfer is
        //! done).  If the DMA controller provides flow control, and if SWidth matches DWidth, this
        //! count will be the number of discrete transfers of that size before the linked list item
        //! or compelete transfer is done.  If the DMA controller provides flow control and those
        //! sizes don't match, the interpretation of the TransferSize field value is not documented
        //! in the ARM PL081 manual, and being unknown, is not documented here at this time.
        //! Whether the DMA controller provides flow control, or if not which peripheral does so
        //! remains constant for all linked list items in a segmented transfer, and is specified by
        //! the FlowCntrl field in the DMACCxConfiguration register, or DMACCxConfiguration member
        //! in DMA_TRANSFER_REQUEST_t.
        #define DMACCXCONTROL_TRANSFER_SIZE_MASK                        0x00000fff
} DMA_LLI_t;

//! DMA transfer request control block, to be used with a call to dma_RequestTransfer.
typedef struct DMA_TRANSFER_REQUEST
{
    //! Pointer to a pending transfer request on a shared channel.  The list is maintained by the
    //! DMA driver, and deferred requests will be initiated in first-in-first-out order whenever a
    //! previous transfer request has completed.
    struct DMA_TRANSFER_REQUEST*                                    next;

    //! DMA channel number on which the transfer is to be executed.  Which channel should be used
    //! is determined by a chip-specific DMA channel map, with constants named DMA_CHANNEL_*.
    UINT8                                                           channel;

    //! Function pointer specifying a function to be called when the DMA transfer has completed.  As
    //! the name indicates, this function is always called from the DMA complete interrupt.  The
    //! function pointer can be NULL if there is some other mechanism for completing the transfer,
    //! though using fp_DoneInterrupt is the safest way to assure that the DMA driver's state
    //! remains consistent with the state of the firmware that is using it.
    fp_DoneInterrupt_t                                              fp_DoneInterrupt;

    //! Arbitrary argument to be passed to fp_DoneInterrupt.  It can be used by that function to
    //! identify what transfer was completed, using whatever information seems appropriate to the
    //! designer.  The argument could be a pointer to this very DMA_TRANSFER_REQUEST_t, but
    //! typically it is more useful to pass a pointer to the "larger" relevant state information
    //! block that described whatever the transfer was serving.
    void*                                                           fp_DoneInterrupt_arg;

    //! The register values that should be set in the DMA controller for the target channel.  These
    //! values will be written to the DMA controller's channel register bank when this transfer
    //! starts.  DMA_LLI_t contains all those registers, although linked list items won't actually
    //! be used unless DMACCx_first_segment_regs.DMACCxLLI points to an actual linked list item.
    DMA_LLI_t                                                       DMACCx_first_segment_regs;

    //! Please refer to the ARM PL081 DMA controller documentation.  That documentation is more
    //! comprehensive than what can be presented here.  As for this documentation, please refer to
    //! the defines for flags and masks with names beginning in DMACCXCONFIG_.
    UINT32                                                          DMACCxConfiguration;

        //! Mask or value for the H (Halt) field in DMACCxConfiguration.  That is a register in a
        //! DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t.  Please refer
        //! to the ARM PL081 DMA controller documentation, but don't set this bit in a
        //! DMA_TRANSFER_REQUEST_t DMACCxConfiguration field.
        #define DMACCXCONFIG_HALT                                       0x00040000

        //! Mask or value for the A (Active) field in DMACCxConfiguration.  That is a register in a
        //! DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t.  Please refer
        //! to the ARM PL081 DMA controller documentation, note that writing to this bit has no
        //! effect and don't set this bit.
        #define DMACCXCONFIG_ACTIVE                                     0x00020000

        //! Mask or value for the L (Lock) field in DMACCxConfiguration.  That is a register in a
        //! DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t.  Please refer
        //! to the ARM PL081 DMA controller documentation.
        #define DMACCXCONFIG_LOCK                                       0x00010000

        //! Mask or value for the ITC (Terminal count interrupt) field in DMACCxConfiguration.  That
        //! is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t.  Please refer to the ARM PL081 DMA controller documentation and
        //! do make sure to set this bit for DMACCxConfiguration in any DMA_TRANSFER_REQUEST_t.
        #define DMACCXCONFIG_DMA_DONE_INTERRUPT_ENABLE                  0x00008000

        //! Mask or value for the IE (Interrupt error) field in DMACCxConfiguration.  That is a
        //! register in a DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t.
        //! Please refer to the ARM PL081 DMA controller documentation.
        #define DMACCXCONFIG_DMA_ERROR_INTERRUPT_ENABLE                 0x00004000

        //! Mask for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t.  The value must be one of the following constants:
        //! DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_MEMORY,
        //! DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_PERIPH_NO_FLOW,
        //! DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_MEMORY_NO_FLOW,
        //! DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_NO_FLOW,
        //! DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_DEST_FLOW,
        //! DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_PERIPH_DEST_FLOW,
        //! DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_MEMORY_SRC_FLOW, or
        //! DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_SRC_FLOW.
        #define DMACCXCONFIG_FLOWCNTRL_MASK                             0x00003800

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! memory-to-memory transfer.
        #define DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_MEMORY                 0x00000000

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! memory-to-peripheral transfer without flow control.
        #define DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_PERIPH_NO_FLOW         0x00000800

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! peripheral-to-memory transfer without flow control.
        #define DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_MEMORY_NO_FLOW         0x00001000

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! peripheral-to-peripheral transfer without flow control.
        #define DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_NO_FLOW         0x00001800

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! peripheral-to-peripheral transfer with the destination peripheral providing flow
        //! control.
        #define DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_DEST_FLOW       0x00002000

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! memory-to-peripheral transfer with the destination peripheral providing flow control.
        #define DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_PERIPH_DEST_FLOW       0x00002800

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! peripheral-to-memory transfer with the source peripheral providing flow control.
        #define DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_MEMORY_SRC_FLOW        0x00003000

        //! Value for the FlowCntrl (Flow control and transfer type) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying that the DMA controller should perform a
        //! peripheral-to-peripheral transfer with the source peripheral providing flow control.
        #define DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_PERIPH_SRC_FLOW        0x00003800

        //! Shift for the DestPeripheral (Destination peripheral) field in DMACCxConfiguration.
        //! That is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying which peripheral index should be used as the transfer
        //! destination.  This mapping is determined in hardware, and in firmware a particular piece
        //! of hardware's DMA peripheral index is specified using the chip-specific DMA channel map
        //! header file.  The channel map peripheral indices are not shifted.
        #define DMACCXCONFIG_DEST_PERIPHERAL_INDEX_SHIFT                6

        //! Mask for the DestPeripheral (Destination peripheral) field in DMACCxConfiguration.  That
        //! is a register in a DMA controller channel register bank or field in
        //! DMA_TRANSFER_REQUEST_t, specifying which peripheral index should be used as the transfer
        //! destination.  This mapping is determined in hardware, and in firmware a particular piece
        //! of hardware's DMA peripheral index is specified using the chip-specific DMA channel map
        //! header file.
        #define DMACCXCONFIG_DEST_PERIPHERAL_INDEX_MASK                 0x000003c0

        //! Shift for the SrcPeripheral (Source peripheral) field in DMACCxConfiguration.  That is a
        //! register in a DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t,
        //! specifying which peripheral index should be used as the transfer source.  This mapping
        //! is determined in hardware, and in firmware a particular piece of hardware's DMA
        //! peripheral index is specified using the chip-specific DMA channel map header file.  The
        //! channel map peripheral indices are not shifted.
        #define DMACCXCONFIG_SRC_PERIPHERAL_INDEX_SHIFT                 1

        //! Mask for the SrcPeripheral (Source peripheral) field in DMACCxConfiguration.  That is a
        //! register in a DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t,
        //! specifying which peripheral index should be used as the transfer source.  This mapping
        //! is determined in hardware, and in firmware a particular piece of hardware's DMA
        //! peripheral index is specified using the chip-specific DMA channel map header file.
        #define DMACCXCONFIG_SRC_PERIPHERAL_INDEX_MASK                  0x0000001e

        //! Mask or value for the E (Channel enable) field in DMACCxConfiguration.  That is a
        //! register in a DMA controller channel register bank or field in DMA_TRANSFER_REQUEST_t.
        //! Please refer to the ARM PL081 DMA controller documentation.  This bit must always be set
        //! for all DMACCxConfiguration values in DMA_TRANSFER_REQUEST_t or linked DMA_LLI_t passed
        //! to dma_RequestTransfer.
        #define DMACCXCONFIG_CHANNEL_ENABLED                            0x00000001
} DMA_TRANSFER_REQUEST_t;

#ifdef DMA_ENABLED
//==================================================================================================
// Functions
//==================================================================================================
#ifdef __cplusplus
extern "C" {
#endif


//! Inititalizes the DMA controller.
extern void dma_Init(void);

//! Sets whether a peripheral requires synchronization logic to be enabled or not.  You must use
//! synchronization logic when the peripheral generating the DMA request runs on a different clock
//! to the SMDMAC.  For peripherals running on the same clock the SMDMAC, disabling the
//! as synchronization logic improves the DMA request response time.  This requirement must be set
//! before a DMA peripheral can be used with a transfer request.
extern void dma_SetPeripheralDMACSync( UINT8 peripheral_index, BOOL32 sync_required );

//! Requests a DMA transfer.  On shared channels, the request may not be serviced immediately if the
//! channel is in use.  A transfer should never be requested unless data is either immediately
//! available or is assured to become available within a millisecond.  Otherwise, the pending but
//! unsatisfied request will interfere with sleep decisions.  If a transfer request completes 2 ms
//! or more after it was requested, an assertion failure will result.
extern void dma_RequestTransfer(INOUT DMA_TRANSFER_REQUEST_t* request);


#else
#define dma_Init()
#define dma_SetPeripheralDMACSync( A,B)
#define dma_RequestTransfer(A)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _ARM_PL081_H_
