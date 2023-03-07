/******************************************************************************
*  Filename:       hw_spi_h
*  Revised:        $Date$
*  Revision:       $Revision$
*
* Copyright (c) 2015 - 2017, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1) Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2) Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3) Neither the name of the ORGANIZATION nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __HW_SPI_H__
#define __HW_SPI_H__

//*****************************************************************************
//
// This section defines the register offsets of
// SPI component
//
//*****************************************************************************
// This register provides the highest priority enabled interrupt index. Value
// 0x00 means no event pending. Interrupt 1 is the highest priority, IIDX next
// highest, 4, 8, … IIDX^31 is the least priority. That is, the least bit
// position that is set to 1 denotes the highest priority pending interrupt.
// The priority order is fixed. However, users can implement their own
// prioritization schemes using other registers that expose the full set of
// interrupts that have occurred.
#define SPI_O_IIDX                                                  0x00000020

// Interrupt Mask. If a bit is set, then corresponding interrupt is un-masked.
// Un-masking the interrupt causes the raw interrupt to be visible in IIDX, as
// well as MIS.
#define SPI_O_IMASK                                                 0x00000028

// Raw interrupt status. Reflects all pending interrupts, regardless of
// masking. The RIS register allows the user to implement a poll scheme. A flag
// set in this register can be cleared by writing 1 to the ICLR register bit
// even if the corresponding IMASK bit is not enabled.
#define SPI_O_RIS                                                   0x00000030

// Masked interrupt status. This is an AND of the IMASK and RIS registers.
#define SPI_O_MIS                                                   0x00000038

// Interrupt set. Allows interrupts to be set by software (useful in
// diagnostics and safety checks). Writing a 1 to a bit in ISET will set the
// event and therefore the related RIS bit also gets set. If the interrupt is
// enabled through the mask, then the corresponding MIS bit is also set.
#define SPI_O_ISET                                                  0x00000040

// Interrupt clear. Write a 1 to clear corresponding Interrupt.
#define SPI_O_ICLR                                                  0x00000048

// Event mode register. It is used to select whether each line is disabled, in
// software mode (software clears the RIS) or in hardware mode (hardware clears
// the RIS)
#define SPI_O_EVT_MODE                                              0x000000E0

// This register identifies the peripheral and its exact version.
#define SPI_O_DESC                                                  0x000000FC

// SPI control register 0
#define SPI_O_CTL0                                                  0x00000100

// SPI control register 1
#define SPI_O_CTL1                                                  0x00000104

// Clock prescaler and divider register. This register contains the settings
// for the Clock prescaler and divider settings.
#define SPI_O_CLKCTL                                                0x00000108

// The IFLS register is the interrupt FIFO level select register. You can use
// this register to define the levels at which the TX, RX and timeout interrupt
// flags are triggered. The interrupts are generated based on a transition
// through a level rather than being based on the level. That is, the
// interrupts are generated when the fill level progresses through the trigger
// level. For example, if the receive trigger level is set to the half-way
// mark, the interrupt is triggered when the receive FIFO is filled with two or
// more characters. Out of reset, the TXIFLSEL and RXIFLSEL bits are configured
// so that the FIFOs trigger an interrupt at the half-way mark.
#define SPI_O_IFLS                                                  0x0000010C

// Status Register
#define SPI_O_STAT                                                  0x00000110

// This register is used to specify module-specific divide ratio of the
// functional clock.
#define SPI_O_CLKDIV2                                               0x00000114

// DMA Control Register
#define SPI_O_DMACR                                                 0x00000118

// RXDATA Register
#define SPI_O_RXDATA                                                0x00000130

// TXDATA Register
#define SPI_O_TXDATA                                                0x00000140

//*****************************************************************************
//
// Register: SPI_O_IIDX
//
//*****************************************************************************
// Field:   [7:0] STAT
//
// Interrupt index status
// ENUMs:
// DMA_DONE_TX_EVT          DMA Done for Transmit Event/interrupt pending
// DMA_DONE_RX_EVT          DMA Done for Receive Event/interrupt pending
// IDLE_EVT                 End of Transmit Event/interrupt pending
// TX_EMPTY                 Transmit Buffer Empty Event/interrupt pending
// TX_EVT                   Transmit Event/interrupt pending
// RX_EVT                   Receive Event/interrupt pending
// RTOUT_EVT                SPI receive time-out interrupt
// PER_EVT                  Transmit Parity Event/interrupt pending
// RXFIFO_OFV_EVT           RX FIFO Overflow Event/interrupt pending
// NO_INTR                  No interrupt pending
#define SPI_IIDX_STAT_W                                                      8
#define SPI_IIDX_STAT_M                                             0x000000FF
#define SPI_IIDX_STAT_S                                                      0
#define SPI_IIDX_STAT_DMA_DONE_TX_EVT                               0x00000009
#define SPI_IIDX_STAT_DMA_DONE_RX_EVT                               0x00000008
#define SPI_IIDX_STAT_IDLE_EVT                                      0x00000007
#define SPI_IIDX_STAT_TX_EMPTY                                      0x00000006
#define SPI_IIDX_STAT_TX_EVT                                        0x00000005
#define SPI_IIDX_STAT_RX_EVT                                        0x00000004
#define SPI_IIDX_STAT_RTOUT_EVT                                     0x00000003
#define SPI_IIDX_STAT_PER_EVT                                       0x00000002
#define SPI_IIDX_STAT_RXFIFO_OFV_EVT                                0x00000001
#define SPI_IIDX_STAT_NO_INTR                                       0x00000000

//*****************************************************************************
//
// Register: SPI_O_IMASK
//
//*****************************************************************************
// Field:     [8] DMA_DONE_TX
//
// DMA Done 1 event for TX event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_DMA_DONE_TX                                       0x00000100
#define SPI_IMASK_DMA_DONE_TX_BITN                                           8
#define SPI_IMASK_DMA_DONE_TX_M                                     0x00000100
#define SPI_IMASK_DMA_DONE_TX_S                                              8
#define SPI_IMASK_DMA_DONE_TX_SET                                   0x00000100
#define SPI_IMASK_DMA_DONE_TX_CLR                                   0x00000000

// Field:     [7] DMA_DONE_RX
//
// DMA Done 1 event for RX event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_DMA_DONE_RX                                       0x00000080
#define SPI_IMASK_DMA_DONE_RX_BITN                                           7
#define SPI_IMASK_DMA_DONE_RX_M                                     0x00000080
#define SPI_IMASK_DMA_DONE_RX_S                                              7
#define SPI_IMASK_DMA_DONE_RX_SET                                   0x00000080
#define SPI_IMASK_DMA_DONE_RX_CLR                                   0x00000000

// Field:     [6] IDLE
//
// SPI Idle event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_IDLE                                              0x00000040
#define SPI_IMASK_IDLE_BITN                                                  6
#define SPI_IMASK_IDLE_M                                            0x00000040
#define SPI_IMASK_IDLE_S                                                     6
#define SPI_IMASK_IDLE_SET                                          0x00000040
#define SPI_IMASK_IDLE_CLR                                          0x00000000

// Field:     [5] TXEMPTY
//
// Transmit FIFO Empty event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_TXEMPTY                                           0x00000020
#define SPI_IMASK_TXEMPTY_BITN                                               5
#define SPI_IMASK_TXEMPTY_M                                         0x00000020
#define SPI_IMASK_TXEMPTY_S                                                  5
#define SPI_IMASK_TXEMPTY_SET                                       0x00000020
#define SPI_IMASK_TXEMPTY_CLR                                       0x00000000

// Field:     [4] TX
//
// Transmit FIFO event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_TX                                                0x00000010
#define SPI_IMASK_TX_BITN                                                    4
#define SPI_IMASK_TX_M                                              0x00000010
#define SPI_IMASK_TX_S                                                       4
#define SPI_IMASK_TX_SET                                            0x00000010
#define SPI_IMASK_TX_CLR                                            0x00000000

// Field:     [3] RX
//
// Receive FIFO event. This interrupt is set if the selected Receive FIFO level
// has been reached
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_RX                                                0x00000008
#define SPI_IMASK_RX_BITN                                                    3
#define SPI_IMASK_RX_M                                              0x00000008
#define SPI_IMASK_RX_S                                                       3
#define SPI_IMASK_RX_SET                                            0x00000008
#define SPI_IMASK_RX_CLR                                            0x00000000

// Field:     [2] RTOUT
//
//  Enable SPI Receive Time-Out event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_RTOUT                                             0x00000004
#define SPI_IMASK_RTOUT_BITN                                                 2
#define SPI_IMASK_RTOUT_M                                           0x00000004
#define SPI_IMASK_RTOUT_S                                                    2
#define SPI_IMASK_RTOUT_SET                                         0x00000004
#define SPI_IMASK_RTOUT_CLR                                         0x00000000

// Field:     [1] PER
//
// Parity error event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_PER                                               0x00000002
#define SPI_IMASK_PER_BITN                                                   1
#define SPI_IMASK_PER_M                                             0x00000002
#define SPI_IMASK_PER_S                                                      1
#define SPI_IMASK_PER_SET                                           0x00000002
#define SPI_IMASK_PER_CLR                                           0x00000000

// Field:     [0] RXFIFO_OVF
//
// RXFIFO overflow event mask.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_IMASK_RXFIFO_OVF                                        0x00000001
#define SPI_IMASK_RXFIFO_OVF_BITN                                            0
#define SPI_IMASK_RXFIFO_OVF_M                                      0x00000001
#define SPI_IMASK_RXFIFO_OVF_S                                               0
#define SPI_IMASK_RXFIFO_OVF_SET                                    0x00000001
#define SPI_IMASK_RXFIFO_OVF_CLR                                    0x00000000

//*****************************************************************************
//
// Register: SPI_O_RIS
//
//*****************************************************************************
// Field:     [8] DMA_DONE_TX
//
// DMA Done 1 event for TX. This interrupt is set if the TX DMA channel sends
// the DONE signal. This allows the handling of the DMA event inside the mapped
// peripheral.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_DMA_DONE_TX                                         0x00000100
#define SPI_RIS_DMA_DONE_TX_BITN                                             8
#define SPI_RIS_DMA_DONE_TX_M                                       0x00000100
#define SPI_RIS_DMA_DONE_TX_S                                                8
#define SPI_RIS_DMA_DONE_TX_SET                                     0x00000100
#define SPI_RIS_DMA_DONE_TX_CLR                                     0x00000000

// Field:     [7] DMA_DONE_RX
//
// DMA Done 1 event for RX. This interrupt is set if the RX DMA channel sends
// the DONE signal. This allows the handling of the DMA event inside the mapped
// peripheral.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_DMA_DONE_RX                                         0x00000080
#define SPI_RIS_DMA_DONE_RX_BITN                                             7
#define SPI_RIS_DMA_DONE_RX_M                                       0x00000080
#define SPI_RIS_DMA_DONE_RX_S                                                7
#define SPI_RIS_DMA_DONE_RX_SET                                     0x00000080
#define SPI_RIS_DMA_DONE_RX_CLR                                     0x00000000

// Field:     [6] IDLE
//
// SPI has done finished transfers and changed into IDLE mode. This bit is set
// when BUSY goes low.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_IDLE                                                0x00000040
#define SPI_RIS_IDLE_BITN                                                    6
#define SPI_RIS_IDLE_M                                              0x00000040
#define SPI_RIS_IDLE_S                                                       6
#define SPI_RIS_IDLE_SET                                            0x00000040
#define SPI_RIS_IDLE_CLR                                            0x00000000

// Field:     [5] TXEMPTY
//
// Transmit FIFO Empty interrupt mask. This interrupt is set if all data in the
// Transmit FIFO have been move to the shift register.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_TXEMPTY                                             0x00000020
#define SPI_RIS_TXEMPTY_BITN                                                 5
#define SPI_RIS_TXEMPTY_M                                           0x00000020
#define SPI_RIS_TXEMPTY_S                                                    5
#define SPI_RIS_TXEMPTY_SET                                         0x00000020
#define SPI_RIS_TXEMPTY_CLR                                         0x00000000

// Field:     [4] TX
//
// Transmit FIFO event. This interrupt is set if the selected Transmit FIFO
// level has been reached.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_TX                                                  0x00000010
#define SPI_RIS_TX_BITN                                                      4
#define SPI_RIS_TX_M                                                0x00000010
#define SPI_RIS_TX_S                                                         4
#define SPI_RIS_TX_SET                                              0x00000010
#define SPI_RIS_TX_CLR                                              0x00000000

// Field:     [3] RX
//
// Receive FIFO event. This interrupt is set if the selected Receive FIFO level
// has been reached
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_RX                                                  0x00000008
#define SPI_RIS_RX_BITN                                                      3
#define SPI_RIS_RX_M                                                0x00000008
#define SPI_RIS_RX_S                                                         3
#define SPI_RIS_RX_SET                                              0x00000008
#define SPI_RIS_RX_CLR                                              0x00000000

// Field:     [2] RTOUT
//
//  SPI Receive Time-Out event.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_RIS_RTOUT                                               0x00000004
#define SPI_RIS_RTOUT_BITN                                                   2
#define SPI_RIS_RTOUT_M                                             0x00000004
#define SPI_RIS_RTOUT_S                                                      2
#define SPI_RIS_RTOUT_SET                                           0x00000004
#define SPI_RIS_RTOUT_CLR                                           0x00000000

// Field:     [1] PER
//
// Parity error event: this bit is set if a Parity error has been detected
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_PER                                                 0x00000002
#define SPI_RIS_PER_BITN                                                     1
#define SPI_RIS_PER_M                                               0x00000002
#define SPI_RIS_PER_S                                                        1
#define SPI_RIS_PER_SET                                             0x00000002
#define SPI_RIS_PER_CLR                                             0x00000000

// Field:     [0] RXFIFO_OVF
//
// RXFIFO overflow event. This interrupt is set if an RX FIFO overflow has been
// detected.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_RIS_RXFIFO_OVF                                          0x00000001
#define SPI_RIS_RXFIFO_OVF_BITN                                              0
#define SPI_RIS_RXFIFO_OVF_M                                        0x00000001
#define SPI_RIS_RXFIFO_OVF_S                                                 0
#define SPI_RIS_RXFIFO_OVF_SET                                      0x00000001
#define SPI_RIS_RXFIFO_OVF_CLR                                      0x00000000

//*****************************************************************************
//
// Register: SPI_O_MIS
//
//*****************************************************************************
// Field:     [8] DMA_DONE_TX
//
// Masked DMA Done 1 event for TX.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_DMA_DONE_TX                                         0x00000100
#define SPI_MIS_DMA_DONE_TX_BITN                                             8
#define SPI_MIS_DMA_DONE_TX_M                                       0x00000100
#define SPI_MIS_DMA_DONE_TX_S                                                8
#define SPI_MIS_DMA_DONE_TX_SET                                     0x00000100
#define SPI_MIS_DMA_DONE_TX_CLR                                     0x00000000

// Field:     [7] DMA_DONE_RX
//
// Masked DMA Done 1 event for RX.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_DMA_DONE_RX                                         0x00000080
#define SPI_MIS_DMA_DONE_RX_BITN                                             7
#define SPI_MIS_DMA_DONE_RX_M                                       0x00000080
#define SPI_MIS_DMA_DONE_RX_S                                                7
#define SPI_MIS_DMA_DONE_RX_SET                                     0x00000080
#define SPI_MIS_DMA_DONE_RX_CLR                                     0x00000000

// Field:     [6] IDLE
//
// Masked SPI IDLE mode event.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_IDLE                                                0x00000040
#define SPI_MIS_IDLE_BITN                                                    6
#define SPI_MIS_IDLE_M                                              0x00000040
#define SPI_MIS_IDLE_S                                                       6
#define SPI_MIS_IDLE_SET                                            0x00000040
#define SPI_MIS_IDLE_CLR                                            0x00000000

// Field:     [5] TXEMPTY
//
// Masked Transmit FIFO Empty event.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_TXEMPTY                                             0x00000020
#define SPI_MIS_TXEMPTY_BITN                                                 5
#define SPI_MIS_TXEMPTY_M                                           0x00000020
#define SPI_MIS_TXEMPTY_S                                                    5
#define SPI_MIS_TXEMPTY_SET                                         0x00000020
#define SPI_MIS_TXEMPTY_CLR                                         0x00000000

// Field:     [4] TX
//
// Masked Transmit FIFO event. This interrupt is set if the selected Transmit
// FIFO level has been reached.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_TX                                                  0x00000010
#define SPI_MIS_TX_BITN                                                      4
#define SPI_MIS_TX_M                                                0x00000010
#define SPI_MIS_TX_S                                                         4
#define SPI_MIS_TX_SET                                              0x00000010
#define SPI_MIS_TX_CLR                                              0x00000000

// Field:     [3] RX
//
// Masked receive FIFO event. This interrupt is set if the selected Receive
// FIFO level has been reached
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_RX                                                  0x00000008
#define SPI_MIS_RX_BITN                                                      3
#define SPI_MIS_RX_M                                                0x00000008
#define SPI_MIS_RX_S                                                         3
#define SPI_MIS_RX_SET                                              0x00000008
#define SPI_MIS_RX_CLR                                              0x00000000

// Field:     [2] RTOUT
//
// Masked SPI Receive Time-Out Interrupt.
// ENUMs:
// SET                      Set Interrupt Mask
// CLR                      Clear Interrupt Mask
#define SPI_MIS_RTOUT                                               0x00000004
#define SPI_MIS_RTOUT_BITN                                                   2
#define SPI_MIS_RTOUT_M                                             0x00000004
#define SPI_MIS_RTOUT_S                                                      2
#define SPI_MIS_RTOUT_SET                                           0x00000004
#define SPI_MIS_RTOUT_CLR                                           0x00000000

// Field:     [1] PER
//
// Masked Parity error event: this bit if a Parity error has been detected
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_PER                                                 0x00000002
#define SPI_MIS_PER_BITN                                                     1
#define SPI_MIS_PER_M                                               0x00000002
#define SPI_MIS_PER_S                                                        1
#define SPI_MIS_PER_SET                                             0x00000002
#define SPI_MIS_PER_CLR                                             0x00000000

// Field:     [0] RXFIFO_OVF
//
// Masked RXFIFO overflow event. This interrupt is set if an RX FIFO overflow
// has been detected.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define SPI_MIS_RXFIFO_OVF                                          0x00000001
#define SPI_MIS_RXFIFO_OVF_BITN                                              0
#define SPI_MIS_RXFIFO_OVF_M                                        0x00000001
#define SPI_MIS_RXFIFO_OVF_S                                                 0
#define SPI_MIS_RXFIFO_OVF_SET                                      0x00000001
#define SPI_MIS_RXFIFO_OVF_CLR                                      0x00000000

//*****************************************************************************
//
// Register: SPI_O_ISET
//
//*****************************************************************************
// Field:     [8] DMA_DONE_TX
//
// Set DMA Done 1 event for TX.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_DMA_DONE_TX                                        0x00000100
#define SPI_ISET_DMA_DONE_TX_BITN                                            8
#define SPI_ISET_DMA_DONE_TX_M                                      0x00000100
#define SPI_ISET_DMA_DONE_TX_S                                               8
#define SPI_ISET_DMA_DONE_TX_SET                                    0x00000100
#define SPI_ISET_DMA_DONE_TX_NO_EFFECT                              0x00000000

// Field:     [7] DMA_DONE_RX
//
// Set DMA Done 1 event for RX.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_DMA_DONE_RX                                        0x00000080
#define SPI_ISET_DMA_DONE_RX_BITN                                            7
#define SPI_ISET_DMA_DONE_RX_M                                      0x00000080
#define SPI_ISET_DMA_DONE_RX_S                                               7
#define SPI_ISET_DMA_DONE_RX_SET                                    0x00000080
#define SPI_ISET_DMA_DONE_RX_NO_EFFECT                              0x00000000

// Field:     [6] IDLE
//
// Set SPI IDLE mode event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_IDLE                                               0x00000040
#define SPI_ISET_IDLE_BITN                                                   6
#define SPI_ISET_IDLE_M                                             0x00000040
#define SPI_ISET_IDLE_S                                                      6
#define SPI_ISET_IDLE_SET                                           0x00000040
#define SPI_ISET_IDLE_NO_EFFECT                                     0x00000000

// Field:     [5] TXEMPTY
//
// Set Transmit FIFO Empty event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_TXEMPTY                                            0x00000020
#define SPI_ISET_TXEMPTY_BITN                                                5
#define SPI_ISET_TXEMPTY_M                                          0x00000020
#define SPI_ISET_TXEMPTY_S                                                   5
#define SPI_ISET_TXEMPTY_SET                                        0x00000020
#define SPI_ISET_TXEMPTY_NO_EFFECT                                  0x00000000

// Field:     [4] TX
//
// Set Transmit FIFO event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_TX                                                 0x00000010
#define SPI_ISET_TX_BITN                                                     4
#define SPI_ISET_TX_M                                               0x00000010
#define SPI_ISET_TX_S                                                        4
#define SPI_ISET_TX_SET                                             0x00000010
#define SPI_ISET_TX_NO_EFFECT                                       0x00000000

// Field:     [3] RX
//
// Set Receive FIFO event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_RX                                                 0x00000008
#define SPI_ISET_RX_BITN                                                     3
#define SPI_ISET_RX_M                                               0x00000008
#define SPI_ISET_RX_S                                                        3
#define SPI_ISET_RX_SET                                             0x00000008
#define SPI_ISET_RX_NO_EFFECT                                       0x00000000

// Field:     [2] RTOUT
//
// Set SPI Receive Time-Out Event.
// ENUMs:
// SET                      Set Interrupt Mask
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_RTOUT                                              0x00000004
#define SPI_ISET_RTOUT_BITN                                                  2
#define SPI_ISET_RTOUT_M                                            0x00000004
#define SPI_ISET_RTOUT_S                                                     2
#define SPI_ISET_RTOUT_SET                                          0x00000004
#define SPI_ISET_RTOUT_NO_EFFECT                                    0x00000000

// Field:     [1] PER
//
// Set Parity error event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_PER                                                0x00000002
#define SPI_ISET_PER_BITN                                                    1
#define SPI_ISET_PER_M                                              0x00000002
#define SPI_ISET_PER_S                                                       1
#define SPI_ISET_PER_SET                                            0x00000002
#define SPI_ISET_PER_NO_EFFECT                                      0x00000000

// Field:     [0] RXFIFO_OVF
//
// Set RXFIFO overflow event.
// ENUMs:
// SET                      Set Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ISET_RXFIFO_OVF                                         0x00000001
#define SPI_ISET_RXFIFO_OVF_BITN                                             0
#define SPI_ISET_RXFIFO_OVF_M                                       0x00000001
#define SPI_ISET_RXFIFO_OVF_S                                                0
#define SPI_ISET_RXFIFO_OVF_SET                                     0x00000001
#define SPI_ISET_RXFIFO_OVF_NO_EFFECT                               0x00000000

//*****************************************************************************
//
// Register: SPI_O_ICLR
//
//*****************************************************************************
// Field:     [8] DMA_DONE_TX
//
// Clear DMA Done 1 event for TX.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_DMA_DONE_TX                                        0x00000100
#define SPI_ICLR_DMA_DONE_TX_BITN                                            8
#define SPI_ICLR_DMA_DONE_TX_M                                      0x00000100
#define SPI_ICLR_DMA_DONE_TX_S                                               8
#define SPI_ICLR_DMA_DONE_TX_CLR                                    0x00000100
#define SPI_ICLR_DMA_DONE_TX_NO_EFFECT                              0x00000000

// Field:     [7] DMA_DONE_RX
//
// Clear DMA Done 1 event for RX.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_DMA_DONE_RX                                        0x00000080
#define SPI_ICLR_DMA_DONE_RX_BITN                                            7
#define SPI_ICLR_DMA_DONE_RX_M                                      0x00000080
#define SPI_ICLR_DMA_DONE_RX_S                                               7
#define SPI_ICLR_DMA_DONE_RX_CLR                                    0x00000080
#define SPI_ICLR_DMA_DONE_RX_NO_EFFECT                              0x00000000

// Field:     [6] IDLE
//
// Clear SPI IDLE mode event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_IDLE                                               0x00000040
#define SPI_ICLR_IDLE_BITN                                                   6
#define SPI_ICLR_IDLE_M                                             0x00000040
#define SPI_ICLR_IDLE_S                                                      6
#define SPI_ICLR_IDLE_CLR                                           0x00000040
#define SPI_ICLR_IDLE_NO_EFFECT                                     0x00000000

// Field:     [5] TXEMPTY
//
// Clear Transmit FIFO Empty event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_TXEMPTY                                            0x00000020
#define SPI_ICLR_TXEMPTY_BITN                                                5
#define SPI_ICLR_TXEMPTY_M                                          0x00000020
#define SPI_ICLR_TXEMPTY_S                                                   5
#define SPI_ICLR_TXEMPTY_CLR                                        0x00000020
#define SPI_ICLR_TXEMPTY_NO_EFFECT                                  0x00000000

// Field:     [4] TX
//
// Clear Transmit FIFO event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_TX                                                 0x00000010
#define SPI_ICLR_TX_BITN                                                     4
#define SPI_ICLR_TX_M                                               0x00000010
#define SPI_ICLR_TX_S                                                        4
#define SPI_ICLR_TX_CLR                                             0x00000010
#define SPI_ICLR_TX_NO_EFFECT                                       0x00000000

// Field:     [3] RX
//
// Clear Receive FIFO event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_RX                                                 0x00000008
#define SPI_ICLR_RX_BITN                                                     3
#define SPI_ICLR_RX_M                                               0x00000008
#define SPI_ICLR_RX_S                                                        3
#define SPI_ICLR_RX_CLR                                             0x00000008
#define SPI_ICLR_RX_NO_EFFECT                                       0x00000000

// Field:     [2] RTOUT
//
// Clear SPI Receive Time-Out Event.
// ENUMs:
// CLR                      Set Interrupt Mask
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_RTOUT                                              0x00000004
#define SPI_ICLR_RTOUT_BITN                                                  2
#define SPI_ICLR_RTOUT_M                                            0x00000004
#define SPI_ICLR_RTOUT_S                                                     2
#define SPI_ICLR_RTOUT_CLR                                          0x00000004
#define SPI_ICLR_RTOUT_NO_EFFECT                                    0x00000000

// Field:     [1] PER
//
// Clear Parity error event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_PER                                                0x00000002
#define SPI_ICLR_PER_BITN                                                    1
#define SPI_ICLR_PER_M                                              0x00000002
#define SPI_ICLR_PER_S                                                       1
#define SPI_ICLR_PER_CLR                                            0x00000002
#define SPI_ICLR_PER_NO_EFFECT                                      0x00000000

// Field:     [0] RXFIFO_OVF
//
// Clear RXFIFO overflow event.
// ENUMs:
// CLR                      Clear Interrupt
// NO_EFFECT                Writing 0 has no effect
#define SPI_ICLR_RXFIFO_OVF                                         0x00000001
#define SPI_ICLR_RXFIFO_OVF_BITN                                             0
#define SPI_ICLR_RXFIFO_OVF_M                                       0x00000001
#define SPI_ICLR_RXFIFO_OVF_S                                                0
#define SPI_ICLR_RXFIFO_OVF_CLR                                     0x00000001
#define SPI_ICLR_RXFIFO_OVF_NO_EFFECT                               0x00000000

//*****************************************************************************
//
// Register: SPI_O_EVT_MODE
//
//*****************************************************************************
// Field:   [1:0] INT0_CFG
//
// Event line mode select for event corresponding to IPSTANDARD.INT_EVENT0
// ENUMs:
// HARDWARE                 The interrupt or event line is in hardware mode.
//                          The hardware (another module) clears
//                          automatically the associated RIS flag.
// SOFTWARE                 The interrupt or event line is in software mode.
//                          Software must clear the RIS.
// DISABLE                  The interrupt or event line is disabled.
#define SPI_EVT_MODE_INT0_CFG_W                                              2
#define SPI_EVT_MODE_INT0_CFG_M                                     0x00000003
#define SPI_EVT_MODE_INT0_CFG_S                                              0
#define SPI_EVT_MODE_INT0_CFG_HARDWARE                              0x00000002
#define SPI_EVT_MODE_INT0_CFG_SOFTWARE                              0x00000001
#define SPI_EVT_MODE_INT0_CFG_DISABLE                               0x00000000

//*****************************************************************************
//
// Register: SPI_O_DESC
//
//*****************************************************************************
// Field: [31:16] MODULEID
//
// Module identification contains a unique peripheral identification number.
// The assignments are maintained in a central database for all of the platform
// modules to ensure uniqueness.
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_DESC_MODULEID_W                                                 16
#define SPI_DESC_MODULEID_M                                         0xFFFF0000
#define SPI_DESC_MODULEID_S                                                 16
#define SPI_DESC_MODULEID_MAXIMUM                                   0xFFFF0000
#define SPI_DESC_MODULEID_MINIMUM                                   0x00000000

// Field: [15:12] FEATUREVER
//
// Feature Set for the module *instance*
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_DESC_FEATUREVER_W                                                4
#define SPI_DESC_FEATUREVER_M                                       0x0000F000
#define SPI_DESC_FEATUREVER_S                                               12
#define SPI_DESC_FEATUREVER_MAXIMUM                                 0x0000F000
#define SPI_DESC_FEATUREVER_MINIMUM                                 0x00000000

// Field:   [7:4] MAJREV
//
// Major rev of the IP
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_DESC_MAJREV_W                                                    4
#define SPI_DESC_MAJREV_M                                           0x000000F0
#define SPI_DESC_MAJREV_S                                                    4
#define SPI_DESC_MAJREV_MAXIMUM                                     0x000000F0
#define SPI_DESC_MAJREV_MINIMUM                                     0x00000000

// Field:   [3:0] MINREV
//
// Minor rev of the IP
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_DESC_MINREV_W                                                    4
#define SPI_DESC_MINREV_M                                           0x0000000F
#define SPI_DESC_MINREV_S                                                    0
#define SPI_DESC_MINREV_MAXIMUM                                     0x0000000F
#define SPI_DESC_MINREV_MINIMUM                                     0x00000000

//*****************************************************************************
//
// Register: SPI_O_CTL0
//
//*****************************************************************************
// Field:    [14] CSCLR
//
// Clear shift register counter on CS inactive
// This bit is relevant only in the slave mode, MS=0.
// 0: The shift counter will keep its state when CS goes low
// 1: The shift counter will be clear when CS goes low
// ENUMs:
// ENABLE                   Enable automatic clear of shift register when CS
//                          goes to disable.
// DISABLE                  Disable automatic clear of shift register when CS
//                          goes to disable.
#define SPI_CTL0_CSCLR                                              0x00004000
#define SPI_CTL0_CSCLR_BITN                                                 14
#define SPI_CTL0_CSCLR_M                                            0x00004000
#define SPI_CTL0_CSCLR_S                                                    14
#define SPI_CTL0_CSCLR_ENABLE                                       0x00004000
#define SPI_CTL0_CSCLR_DISABLE                                      0x00000000

// Field:     [9] SPH
//
// CLKOUT phase (Motorola SPI frame format only)
// This bit selects the clock edge that captures data and enables it to change
// state. It
// has the most impact on the first bit transmitted by either permitting or not
// permitting a clock transition before the first data capture edge.
// 0h = 1ST_CLK_EDGE : Data is captured on the first clock edge transition.
// 1h = 2ND_CLK_EDGE : Data is captured on the second clock edge transition.
// ENUMs:
// SECOND                   Data is captured on the second clock edge
//                          transition.
// FIRST                    Data is captured on the first clock edge
//                          transition.
#define SPI_CTL0_SPH                                                0x00000200
#define SPI_CTL0_SPH_BITN                                                    9
#define SPI_CTL0_SPH_M                                              0x00000200
#define SPI_CTL0_SPH_S                                                       9
#define SPI_CTL0_SPH_SECOND                                         0x00000200
#define SPI_CTL0_SPH_FIRST                                          0x00000000

// Field:     [8] SPO
//
// CLKOUT polarity (Motorola SPI frame format only)
// 0h = SPI produces a steady state LOW value on the CLKOUT pin when data is
// not being transferred.
// 1h = SPI produces a steady state HIGH value on the CLKOUT pin when data is
// not being transferred.
// ENUMs:
// HIGH                     SPI produces a steady state HIGH value on the
//                          CLKOUT
// LOW                      SPI produces a steady state LOW value on the
//                          CLKOUT
#define SPI_CTL0_SPO                                                0x00000100
#define SPI_CTL0_SPO_BITN                                                    8
#define SPI_CTL0_SPO_M                                              0x00000100
#define SPI_CTL0_SPO_S                                                       8
#define SPI_CTL0_SPO_HIGH                                           0x00000100
#define SPI_CTL0_SPO_LOW                                            0x00000000

// Field:   [6:5] FRF
//
// Frame format Select
// ENUMs:
// MIRCOWIRE                National Microwire frame format
// TI_SYNC                  TI synchronous serial frame format
// MOTOROLA_4WIRE           Motorola SPI frame format (4 wire mode)
// MOTOROLA_3WIRE           Motorola SPI frame format (3 wire mode)
#define SPI_CTL0_FRF_W                                                       2
#define SPI_CTL0_FRF_M                                              0x00000060
#define SPI_CTL0_FRF_S                                                       5
#define SPI_CTL0_FRF_MIRCOWIRE                                      0x00000060
#define SPI_CTL0_FRF_TI_SYNC                                        0x00000040
#define SPI_CTL0_FRF_MOTOROLA_4WIRE                                 0x00000020
#define SPI_CTL0_FRF_MOTOROLA_3WIRE                                 0x00000000

// Field:   [4:0] DSS
//
// Data Size Select.
// Note:
// Master mode: Values 0 - 2 are reserved and shall not be used. This will map
// to 4 bit  mode. 3h = 4_BIT : 4-bit data
// Slave mode: DSS should be no less than 6 which means the minimum frame
// length is 7 bits.
// ENUMs:
// DSS_32                   Data Size Select bits: 32
// DSS_31                   Data Size Select bits: 31
// DSS_30                   Data Size Select bits: 30
// DSS_29                   Data Size Select bits: 29
// DSS_28                   Data Size Select bits: 28
// DSS_27                   Data Size Select bits: 27
// DSS_26                   Data Size Select bits: 26
// DSS_25                   Data Size Select bits: 25
// DSS_24                   Data Size Select bits: 24
// DSS_23                   Data Size Select bits: 23
// DSS_22                   Data Size Select bits: 22
// DSS_21                   Data Size Select bits: 21
// DSS_20                   Data Size Select bits: 20
// DSS_19                   Data Size Select bits: 19
// DSS_18                   Data Size Select bits: 18
// DSS_17                   Data Size Select bits: 17
// DSS_16                   Data Size Select bits: 16
// DSS_15                   Data Size Select bits: 15
// DSS_14                   Data Size Select bits: 14
// DSS_13                   Data Size Select bits: 13
// DSS_12                   Data Size Select bits: 12
// DSS_11                   Data Size Select bits: 11
// DSS_10                   Data Size Select bits: 10
// DSS_9                    Data Size Select bits: 9
// DSS_8                    Data Size Select bits: 8
// DSS_7                    Data Size Select bits: 7
// DSS_6                    Data Size Select bits: 6
// DSS_5                    Data Size Select bits: 5
// DSS_4                    Data Size Select bits: 4
#define SPI_CTL0_DSS_W                                                       5
#define SPI_CTL0_DSS_M                                              0x0000001F
#define SPI_CTL0_DSS_S                                                       0
#define SPI_CTL0_DSS_DSS_32                                         0x0000001F
#define SPI_CTL0_DSS_DSS_31                                         0x0000001E
#define SPI_CTL0_DSS_DSS_30                                         0x0000001D
#define SPI_CTL0_DSS_DSS_29                                         0x0000001C
#define SPI_CTL0_DSS_DSS_28                                         0x0000001B
#define SPI_CTL0_DSS_DSS_27                                         0x0000001A
#define SPI_CTL0_DSS_DSS_26                                         0x00000019
#define SPI_CTL0_DSS_DSS_25                                         0x00000018
#define SPI_CTL0_DSS_DSS_24                                         0x00000017
#define SPI_CTL0_DSS_DSS_23                                         0x00000016
#define SPI_CTL0_DSS_DSS_22                                         0x00000015
#define SPI_CTL0_DSS_DSS_21                                         0x00000014
#define SPI_CTL0_DSS_DSS_20                                         0x00000013
#define SPI_CTL0_DSS_DSS_19                                         0x00000012
#define SPI_CTL0_DSS_DSS_18                                         0x00000011
#define SPI_CTL0_DSS_DSS_17                                         0x00000010
#define SPI_CTL0_DSS_DSS_16                                         0x0000000F
#define SPI_CTL0_DSS_DSS_15                                         0x0000000E
#define SPI_CTL0_DSS_DSS_14                                         0x0000000D
#define SPI_CTL0_DSS_DSS_13                                         0x0000000C
#define SPI_CTL0_DSS_DSS_12                                         0x0000000B
#define SPI_CTL0_DSS_DSS_11                                         0x0000000A
#define SPI_CTL0_DSS_DSS_10                                         0x00000009
#define SPI_CTL0_DSS_DSS_9                                          0x00000008
#define SPI_CTL0_DSS_DSS_8                                          0x00000007
#define SPI_CTL0_DSS_DSS_7                                          0x00000006
#define SPI_CTL0_DSS_DSS_6                                          0x00000005
#define SPI_CTL0_DSS_DSS_5                                          0x00000004
#define SPI_CTL0_DSS_DSS_4                                          0x00000003

//*****************************************************************************
//
// Register: SPI_O_CTL1
//
//*****************************************************************************
// Field: [29:24] RXTIMEOUT
//
// Receive Timeout (only for Slave mode)
// Defines the number of Clock Cycles before after which the Receive Timeout
// flag RTOUT is set.
// The time is calculated using the control register for the clock selection
// and divider in the Master mode configuration.
// A value of 0 disables this function.
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_CTL1_RXTIMEOUT_W                                                 6
#define SPI_CTL1_RXTIMEOUT_M                                        0x3F000000
#define SPI_CTL1_RXTIMEOUT_S                                                24
#define SPI_CTL1_RXTIMEOUT_MAXIMUM                                  0x3F000000
#define SPI_CTL1_RXTIMEOUT_MINIMUM                                  0x00000000

// Field: [23:16] REPEATTX
//
// Counter to repeat last transfer
// 0: repeat last transfer is disabled.
// x: repeat the last transfer with the given number.
// The transfer will be started with writing a data into the TX Buffer. Sending
// the data will be repeated with the given value, so the data will be
// transferred X+1 times in total.
// The behavior is identical as if the data would be written into the TX Buffer
// that many times as defined by the value here.
// It can be used to clean a transfer or to pull a certain amount of data by a
// slave.
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_CTL1_REPEATTX_W                                                  8
#define SPI_CTL1_REPEATTX_M                                         0x00FF0000
#define SPI_CTL1_REPEATTX_S                                                 16
#define SPI_CTL1_REPEATTX_MAXIMUM                                   0x00FF0000
#define SPI_CTL1_REPEATTX_MINIMUM                                   0x00000000

// Field:   [9:8] MODE
//
// SPI Communication Mode Select
// Note: Reserved/undefined Modes are identical to Legacy mode. MultiSPI mode
// is not supported
// ENUMs:
// MULTISPI4                multiSPI Mode with 4 Data Bits (not supported)
// MULTISPI2                multiSPI Mode with 2 Data Bits (not supported)
// LEGACY                   Legacy Mode
#define SPI_CTL1_MODE_W                                                      2
#define SPI_CTL1_MODE_M                                             0x00000300
#define SPI_CTL1_MODE_S                                                      8
#define SPI_CTL1_MODE_MULTISPI4                                     0x00000300
#define SPI_CTL1_MODE_MULTISPI2                                     0x00000200
#define SPI_CTL1_MODE_LEGACY                                        0x00000000

// Field:     [7] PBS
//
// Parity Bit Select:
// Disabled: Bit 0 is used for Parity
// Enabled: Bit 1 is used for Parity, Bit 0 is ignored
// ENUMs:
// ENABLE                   Bit 1 is used for Parity, Bit 0 is ignored
// DISABLE                  Bit 0 is used for Parity
#define SPI_CTL1_PBS                                                0x00000080
#define SPI_CTL1_PBS_BITN                                                    7
#define SPI_CTL1_PBS_M                                              0x00000080
#define SPI_CTL1_PBS_S                                                       7
#define SPI_CTL1_PBS_ENABLE                                         0x00000080
#define SPI_CTL1_PBS_DISABLE                                        0x00000000

// Field:     [6] PES
//
// Even Parity Select
// ENUMs:
// ENABLE                   Even Parity mode
// DISABLE                  Odd Parity mode
#define SPI_CTL1_PES                                                0x00000040
#define SPI_CTL1_PES_BITN                                                    6
#define SPI_CTL1_PES_M                                              0x00000040
#define SPI_CTL1_PES_S                                                       6
#define SPI_CTL1_PES_ENABLE                                         0x00000040
#define SPI_CTL1_PES_DISABLE                                        0x00000000

// Field:     [5] PEN
//
// Parity enable
// if enabled the last bit will be used as parity to evaluate the right
// transmission of the previous bits.
// In case of a parity miss-match the parity error flag RIS.PER will be set.
// ENUMs:
// ENABLE                   Enable Parity function
// DISABLE                  Disable Parity function
#define SPI_CTL1_PEN                                                0x00000020
#define SPI_CTL1_PEN_BITN                                                    5
#define SPI_CTL1_PEN_M                                              0x00000020
#define SPI_CTL1_PEN_S                                                       5
#define SPI_CTL1_PEN_ENABLE                                         0x00000020
#define SPI_CTL1_PEN_DISABLE                                        0x00000000

// Field:     [4] MSB
//
// MSB first select. Controls the direction of the receive and transmit shift
// register.
// 0b = LSB first
// 1b = MSB first
// ENUMs:
// ENABLE                   MSB first
// DISABLE                  LSB first
#define SPI_CTL1_MSB                                                0x00000010
#define SPI_CTL1_MSB_BITN                                                    4
#define SPI_CTL1_MSB_M                                              0x00000010
#define SPI_CTL1_MSB_S                                                       4
#define SPI_CTL1_MSB_ENABLE                                         0x00000010
#define SPI_CTL1_MSB_DISABLE                                        0x00000000

// Field:     [3] SOD
//
// Slave-mode: Data output disabled
// This bit is relevant only in the slave mode, MS=0. In multiple-slave
// systems, it is possible for an SPI master to broadcast a message to all
// slaves in the system while ensuring that only one slave drives data onto its
// serial output line. In such systems the RXD lines from multiple slaves could
// be tied together. To operate in such systems, this bitfield can be set if
// the SPI slave is not supposed to drive the TXD line:
// 0: SPI can drive the MISO output in slave mode.
// 1: SPI cannot drive the MISO output in slave mode.
// ENUMs:
// ENABLE                   SPI cannot drive the MISO output in slave mode.
// DISABLE                  SPI can drive the MISO output in slave mode.
#define SPI_CTL1_SOD                                                0x00000008
#define SPI_CTL1_SOD_BITN                                                    3
#define SPI_CTL1_SOD_M                                              0x00000008
#define SPI_CTL1_SOD_S                                                       3
#define SPI_CTL1_SOD_ENABLE                                         0x00000008
#define SPI_CTL1_SOD_DISABLE                                        0x00000000

// Field:     [2] MS
//
// Master or slave mode select. This bit can be modified only when SPI is
// disabled, CTL1.ENABLE=0.
// 0h = Device configured as slave
// 1h = Device configured as master
// ENUMs:
// ENABLE                   Select Master Mode
// DISABLE                  Select Slave Mode
#define SPI_CTL1_MS                                                 0x00000004
#define SPI_CTL1_MS_BITN                                                     2
#define SPI_CTL1_MS_M                                               0x00000004
#define SPI_CTL1_MS_S                                                        2
#define SPI_CTL1_MS_ENABLE                                          0x00000004
#define SPI_CTL1_MS_DISABLE                                         0x00000000

// Field:     [1] LBM
//
// Loop back mode:
// 0: Normal serial port operation enabled.
// 1: Output of transmit serial shifter is connected to input of receive serial
// shifter internally.
// ENUMs:
// ENABLE                   Enable loopback mode
// DISABLE                  Disable loopback mode
#define SPI_CTL1_LBM                                                0x00000002
#define SPI_CTL1_LBM_BITN                                                    1
#define SPI_CTL1_LBM_M                                              0x00000002
#define SPI_CTL1_LBM_S                                                       1
#define SPI_CTL1_LBM_ENABLE                                         0x00000002
#define SPI_CTL1_LBM_DISABLE                                        0x00000000

// Field:     [0] ENABLE
//
// SPI enable
// 0b = Disabled. SPI is disabled and logic held in reset state.
// 1b = Enabled. SPI  released for operation.
// ENUMs:
// ENABLE                   Enable module function
// DISABLE                  Disable module function
#define SPI_CTL1_ENABLE                                             0x00000001
#define SPI_CTL1_ENABLE_BITN                                                 0
#define SPI_CTL1_ENABLE_M                                           0x00000001
#define SPI_CTL1_ENABLE_S                                                    0
#define SPI_CTL1_ENABLE_ENABLE                                      0x00000001
#define SPI_CTL1_ENABLE_DISABLE                                     0x00000000

//*****************************************************************************
//
// Register: SPI_O_CLKCTL
//
//*****************************************************************************
// Field: [31:28] DSAMPLE
//
// Delayed sampling. In master mode the data on the input pin will be delayed
// sampled by the defined clock cycles.
// Note: As an example, if the SPI transmit frequency is set to 12 MHz  in the
// master mode, DSAMPLE should be set to a value of 2
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_CLKCTL_DSAMPLE_W                                                 4
#define SPI_CLKCTL_DSAMPLE_M                                        0xF0000000
#define SPI_CLKCTL_DSAMPLE_S                                                28
#define SPI_CLKCTL_DSAMPLE_MAXIMUM                                  0xF0000000
#define SPI_CLKCTL_DSAMPLE_MINIMUM                                  0x00000000

// Field:   [9:0] SCR
//
// Serial clock divider:
// This is used to generate the transmit and receive bit rate of the SPI.
// The SPI bit rate is
// (SPI's functional clock frequency)/((SCR+1)*2).
// SCR is a value from 0-1023.
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define SPI_CLKCTL_SCR_W                                                    10
#define SPI_CLKCTL_SCR_M                                            0x000003FF
#define SPI_CLKCTL_SCR_S                                                     0
#define SPI_CLKCTL_SCR_MAXIMUM                                      0x000003FF
#define SPI_CLKCTL_SCR_MINIMUM                                      0x00000000

//*****************************************************************************
//
// Register: SPI_O_IFLS
//
//*****************************************************************************
// Field:   [5:3] RXIFLSEL
//
// SPI Receive Interrupt FIFO Level Select  The trigger points for the receive
// interrupt are as follows:
// ENUMs:
// LEVEL_1                  Trigger when RX FIFO contains >= 1 byte
//                          Should be used with DMA
// LVL_RES6                 Reserved
// LVL_FULL                 RX FIFO is full
// LVL_RES4                 Reserved
// LVL_3_4                  RX FIFO >= 3/4 full
// LVL_1_2                  RX FIFO >= 1/2 full (default)
// LVL_1_4                  RX FIFO >= 1/4 full
// LVL_OFF                  Reserved
#define SPI_IFLS_RXIFLSEL_W                                                  3
#define SPI_IFLS_RXIFLSEL_M                                         0x00000038
#define SPI_IFLS_RXIFLSEL_S                                                  3
#define SPI_IFLS_RXIFLSEL_LEVEL_1                                   0x00000038
#define SPI_IFLS_RXIFLSEL_LVL_RES6                                  0x00000030
#define SPI_IFLS_RXIFLSEL_LVL_FULL                                  0x00000028
#define SPI_IFLS_RXIFLSEL_LVL_RES4                                  0x00000020
#define SPI_IFLS_RXIFLSEL_LVL_3_4                                   0x00000018
#define SPI_IFLS_RXIFLSEL_LVL_1_2                                   0x00000010
#define SPI_IFLS_RXIFLSEL_LVL_1_4                                   0x00000008
#define SPI_IFLS_RXIFLSEL_LVL_OFF                                   0x00000000

// Field:   [2:0] TXIFLSEL
//
// SPI Transmit Interrupt FIFO Level Select  The trigger points for the
// transmit interrupt are as follows:
// ENUMs:
// LEVEL_1                  Trigger when TX FIFO has >= 1 byte free
//                          Should be used with DMA
// LVL_RES6                 Reserved
// LVL_EMPTY                TX FIFO is empty
// LVL_RES4                 Reserved
// LVL_1_4                  TX FIFO <= 1/4 empty
// LVL_1_2                  TX FIFO <= 1/2 empty (default)
// LVL_3_4                  TX FIFO <= 3/4 empty
// LVL_OFF                  Reserved
#define SPI_IFLS_TXIFLSEL_W                                                  3
#define SPI_IFLS_TXIFLSEL_M                                         0x00000007
#define SPI_IFLS_TXIFLSEL_S                                                  0
#define SPI_IFLS_TXIFLSEL_LEVEL_1                                   0x00000007
#define SPI_IFLS_TXIFLSEL_LVL_RES6                                  0x00000006
#define SPI_IFLS_TXIFLSEL_LVL_EMPTY                                 0x00000005
#define SPI_IFLS_TXIFLSEL_LVL_RES4                                  0x00000004
#define SPI_IFLS_TXIFLSEL_LVL_1_4                                   0x00000003
#define SPI_IFLS_TXIFLSEL_LVL_1_2                                   0x00000002
#define SPI_IFLS_TXIFLSEL_LVL_3_4                                   0x00000001
#define SPI_IFLS_TXIFLSEL_LVL_OFF                                   0x00000000

//*****************************************************************************
//
// Register: SPI_O_STAT
//
//*****************************************************************************
// Field:     [4] BUSY
//
// Busy
// ENUMs:
// ACTIVE                   SPI is currently transmitting and/or receiving
//                          data, or transmit FIFO is not empty.
// IDLE                     SPI is in idle mode.
#define SPI_STAT_BUSY                                               0x00000010
#define SPI_STAT_BUSY_BITN                                                   4
#define SPI_STAT_BUSY_M                                             0x00000010
#define SPI_STAT_BUSY_S                                                      4
#define SPI_STAT_BUSY_ACTIVE                                        0x00000010
#define SPI_STAT_BUSY_IDLE                                          0x00000000

// Field:     [3] RNF
//
// Receive FIFO not full
// ENUMs:
// NOT_FULL                 Receive FIFO is not full.
// FULL                     Receive FIFO is full.
#define SPI_STAT_RNF                                                0x00000008
#define SPI_STAT_RNF_BITN                                                    3
#define SPI_STAT_RNF_M                                              0x00000008
#define SPI_STAT_RNF_S                                                       3
#define SPI_STAT_RNF_NOT_FULL                                       0x00000008
#define SPI_STAT_RNF_FULL                                           0x00000000

// Field:     [2] RFE
//
// Receive FIFO empty.
// ENUMs:
// EMPTY                    Receive FIFO is empty.
// NOT_EMPTY                Receive FIFO is not empty.
#define SPI_STAT_RFE                                                0x00000004
#define SPI_STAT_RFE_BITN                                                    2
#define SPI_STAT_RFE_M                                              0x00000004
#define SPI_STAT_RFE_S                                                       2
#define SPI_STAT_RFE_EMPTY                                          0x00000004
#define SPI_STAT_RFE_NOT_EMPTY                                      0x00000000

// Field:     [1] TNF
//
// Transmit FIFI not full
// ENUMs:
// NOT_FULL                 Transmit FIFO is not full.
// FULL                     Transmit FIFO is full.
#define SPI_STAT_TNF                                                0x00000002
#define SPI_STAT_TNF_BITN                                                    1
#define SPI_STAT_TNF_M                                              0x00000002
#define SPI_STAT_TNF_S                                                       1
#define SPI_STAT_TNF_NOT_FULL                                       0x00000002
#define SPI_STAT_TNF_FULL                                           0x00000000

// Field:     [0] TFE
//
// Transmit FIFO empty.
// ENUMs:
// EMPTY                    Transmit FIFO is empty.
// NOT_EMPTY                Transmit FIFO is not empty.
#define SPI_STAT_TFE                                                0x00000001
#define SPI_STAT_TFE_BITN                                                    0
#define SPI_STAT_TFE_M                                              0x00000001
#define SPI_STAT_TFE_S                                                       0
#define SPI_STAT_TFE_EMPTY                                          0x00000001
#define SPI_STAT_TFE_NOT_EMPTY                                      0x00000000

//*****************************************************************************
//
// Register: SPI_O_CLKDIV2
//
//*****************************************************************************
// Field:   [2:0] RATIO
//
// Selects divide ratio of module clock
// ENUMs:
// DIV_BY_8                 Divide clock source by 8
// DIV_BY_7                 Divide clock source by 7
// DIV_BY_6                 Divide clock source by 6
// DIV_BY_5                 Divide clock source by 5
// DIV_BY_4                 Divide clock source by 4
// DIV_BY_3                 Divide clock source by 3
// DIV_BY_2                 Divide clock source by 2
// DIV_BY_1                 Do not divide clock source
#define SPI_CLKDIV2_RATIO_W                                                  3
#define SPI_CLKDIV2_RATIO_M                                         0x00000007
#define SPI_CLKDIV2_RATIO_S                                                  0
#define SPI_CLKDIV2_RATIO_DIV_BY_8                                  0x00000007
#define SPI_CLKDIV2_RATIO_DIV_BY_7                                  0x00000006
#define SPI_CLKDIV2_RATIO_DIV_BY_6                                  0x00000005
#define SPI_CLKDIV2_RATIO_DIV_BY_5                                  0x00000004
#define SPI_CLKDIV2_RATIO_DIV_BY_4                                  0x00000003
#define SPI_CLKDIV2_RATIO_DIV_BY_3                                  0x00000002
#define SPI_CLKDIV2_RATIO_DIV_BY_2                                  0x00000001
#define SPI_CLKDIV2_RATIO_DIV_BY_1                                  0x00000000

//*****************************************************************************
//
// Register: SPI_O_DMACR
//
//*****************************************************************************
// Field:     [1] TXDMAE
//
// Transmit DMA enable. If this bit is set to 1, DMA for the transmit FIFO is
// enabled.
#define SPI_DMACR_TXDMAE                                            0x00000002
#define SPI_DMACR_TXDMAE_BITN                                                1
#define SPI_DMACR_TXDMAE_M                                          0x00000002
#define SPI_DMACR_TXDMAE_S                                                   1

// Field:     [0] RXDMAE
//
// Receive DMA enable. If this bit is set to 1, DMA for the receive FIFO is
// enabled.
#define SPI_DMACR_RXDMAE                                            0x00000001
#define SPI_DMACR_RXDMAE_BITN                                                0
#define SPI_DMACR_RXDMAE_M                                          0x00000001
#define SPI_DMACR_RXDMAE_S                                                   0

//*****************************************************************************
//
// Register: SPI_O_RXDATA
//
//*****************************************************************************
// Field:  [31:0] DATA
//
// Received Data
// Core Domain SPI: 32-bits wide data register
// ULL Domain SPI: 16-bits wide data register
// When read, the entry in the receive FIFO, pointed to by the current FIFO
// read pointer, is accessed. As data values are removed by the receive logic
// from the incoming data frame, they are placed into the entry in the receive
// FIFO, pointed to by the current FIFO write pointer.
// Received data less than 16 bits is automatically right justified in the
// receive buffer.
// ENUMs:
// MAXIMUM                  Highest possible value
// MAX16BIT                 Highest possible value
// MINIMUM                  Smallest value
#define SPI_RXDATA_DATA_W                                                   32
#define SPI_RXDATA_DATA_M                                           0xFFFFFFFF
#define SPI_RXDATA_DATA_S                                                    0
#define SPI_RXDATA_DATA_MAXIMUM                                     0xFFFFFFFF
#define SPI_RXDATA_DATA_MAX16BIT                                    0x0000FFFF
#define SPI_RXDATA_DATA_MINIMUM                                     0x00000000

//*****************************************************************************
//
// Register: SPI_O_TXDATA
//
//*****************************************************************************
// Field:  [31:0] DATA
//
// Transmit Data
// 32-bits wide data register:
// When read, the last entry in the transmit FIFO, pointed to by the current
// FIFO write pointer, is accessed.
// When written, the entry in the transmit FIFO, pointed to by the write
// pointer, is written to. Data values are removed from the transmit FIFO one
// value at a time by the transmit logic. It is loaded into the transmit serial
// shifter, then serially shifted out onto the TXD output pin at the programmed
// bit rate.
// When a data size of less than 32 bits is selected, the user must
// right-justify data written to the transmit FIFO. The transmit logic ignores
// the unused bits.
// ENUMs:
// MAXIMUM                  Highest possible value
// MAX16BIT                 Highest possible value
// MINIMUM                  Smallest value
#define SPI_TXDATA_DATA_W                                                   32
#define SPI_TXDATA_DATA_M                                           0xFFFFFFFF
#define SPI_TXDATA_DATA_S                                                    0
#define SPI_TXDATA_DATA_MAXIMUM                                     0xFFFFFFFF
#define SPI_TXDATA_DATA_MAX16BIT                                    0x0000FFFF
#define SPI_TXDATA_DATA_MINIMUM                                     0x00000000


#endif // __SPI__
