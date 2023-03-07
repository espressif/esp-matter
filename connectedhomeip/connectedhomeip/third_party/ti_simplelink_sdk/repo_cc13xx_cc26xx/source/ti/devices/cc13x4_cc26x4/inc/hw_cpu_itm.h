/******************************************************************************
*  Filename:       hw_cpu_itm_h
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

#ifndef __HW_CPU_ITM_H__
#define __HW_CPU_ITM_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_ITM component
//
//*****************************************************************************
// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM0                                             0x00000000

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM1                                             0x00000004

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM2                                             0x00000008

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM3                                             0x0000000C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM4                                             0x00000010

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM5                                             0x00000014

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM6                                             0x00000018

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM7                                             0x0000001C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM8                                             0x00000020

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM9                                             0x00000024

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM10                                            0x00000028

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM11                                            0x0000002C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM12                                            0x00000030

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM13                                            0x00000034

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM14                                            0x00000038

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM15                                            0x0000003C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM16                                            0x00000040

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM17                                            0x00000044

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM18                                            0x00000048

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM19                                            0x0000004C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM20                                            0x00000050

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM21                                            0x00000054

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM22                                            0x00000058

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM23                                            0x0000005C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM24                                            0x00000060

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM25                                            0x00000064

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM26                                            0x00000068

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM27                                            0x0000006C

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM28                                            0x00000070

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM29                                            0x00000074

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM30                                            0x00000078

// Provides the interface for generating Instrumentation packets
#define CPU_ITM_O_STIM31                                            0x0000007C

// Provide an individual enable bit for each ITM_STIM register
#define CPU_ITM_O_TER0                                              0x00000E00

// Controls which stimulus ports can be accessed by unprivileged code
#define CPU_ITM_O_TPR                                               0x00000E40

// Configures and controls transfers through the ITM interface
#define CPU_ITM_O_TCR                                               0x00000E80

// Integration Mode: Read ATB Ready
#define CPU_ITM_O_INT_ATREADY                                       0x00000EF0

// Integration Mode: Write ATB Valid
#define CPU_ITM_O_INT_ATVALID                                       0x00000EF8

// Integration Mode Control Register
#define CPU_ITM_O_ITCTRL                                            0x00000F00

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_DEVARCH                                           0x00000FBC

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_DEVTYPE                                           0x00000FCC

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR4                                             0x00000FD0

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR5                                             0x00000FD4

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR6                                             0x00000FD8

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR7                                             0x00000FDC

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR0                                             0x00000FE0

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR1                                             0x00000FE4

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR2                                             0x00000FE8

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_PIDR3                                             0x00000FEC

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_CIDR0                                             0x00000FF0

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_CIDR1                                             0x00000FF4

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_CIDR2                                             0x00000FF8

// Provides CoreSight discovery information for the ITM
#define CPU_ITM_O_CIDR3                                             0x00000FFC

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM0
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM0_DISABLED                                      0x00000002
#define CPU_ITM_STIM0_DISABLED_BITN                                          1
#define CPU_ITM_STIM0_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM0_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM0_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM0_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM0_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM0_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM1
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM1_DISABLED                                      0x00000002
#define CPU_ITM_STIM1_DISABLED_BITN                                          1
#define CPU_ITM_STIM1_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM1_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM1_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM1_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM1_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM1_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM2
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM2_DISABLED                                      0x00000002
#define CPU_ITM_STIM2_DISABLED_BITN                                          1
#define CPU_ITM_STIM2_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM2_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM2_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM2_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM2_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM2_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM3
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM3_DISABLED                                      0x00000002
#define CPU_ITM_STIM3_DISABLED_BITN                                          1
#define CPU_ITM_STIM3_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM3_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM3_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM3_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM3_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM3_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM4
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM4_DISABLED                                      0x00000002
#define CPU_ITM_STIM4_DISABLED_BITN                                          1
#define CPU_ITM_STIM4_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM4_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM4_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM4_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM4_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM4_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM5
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM5_DISABLED                                      0x00000002
#define CPU_ITM_STIM5_DISABLED_BITN                                          1
#define CPU_ITM_STIM5_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM5_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM5_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM5_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM5_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM5_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM6
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM6_DISABLED                                      0x00000002
#define CPU_ITM_STIM6_DISABLED_BITN                                          1
#define CPU_ITM_STIM6_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM6_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM6_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM6_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM6_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM6_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM7
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM7_DISABLED                                      0x00000002
#define CPU_ITM_STIM7_DISABLED_BITN                                          1
#define CPU_ITM_STIM7_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM7_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM7_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM7_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM7_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM7_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM8
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM8_DISABLED                                      0x00000002
#define CPU_ITM_STIM8_DISABLED_BITN                                          1
#define CPU_ITM_STIM8_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM8_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM8_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM8_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM8_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM8_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM9
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM9_DISABLED                                      0x00000002
#define CPU_ITM_STIM9_DISABLED_BITN                                          1
#define CPU_ITM_STIM9_DISABLED_M                                    0x00000002
#define CPU_ITM_STIM9_DISABLED_S                                             1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM9_FIFOREADY                                     0x00000001
#define CPU_ITM_STIM9_FIFOREADY_BITN                                         0
#define CPU_ITM_STIM9_FIFOREADY_M                                   0x00000001
#define CPU_ITM_STIM9_FIFOREADY_S                                            0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM10
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM10_DISABLED                                     0x00000002
#define CPU_ITM_STIM10_DISABLED_BITN                                         1
#define CPU_ITM_STIM10_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM10_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM10_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM10_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM10_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM10_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM11
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM11_DISABLED                                     0x00000002
#define CPU_ITM_STIM11_DISABLED_BITN                                         1
#define CPU_ITM_STIM11_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM11_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM11_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM11_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM11_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM11_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM12
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM12_DISABLED                                     0x00000002
#define CPU_ITM_STIM12_DISABLED_BITN                                         1
#define CPU_ITM_STIM12_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM12_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM12_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM12_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM12_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM12_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM13
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM13_DISABLED                                     0x00000002
#define CPU_ITM_STIM13_DISABLED_BITN                                         1
#define CPU_ITM_STIM13_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM13_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM13_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM13_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM13_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM13_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM14
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM14_DISABLED                                     0x00000002
#define CPU_ITM_STIM14_DISABLED_BITN                                         1
#define CPU_ITM_STIM14_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM14_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM14_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM14_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM14_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM14_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM15
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM15_DISABLED                                     0x00000002
#define CPU_ITM_STIM15_DISABLED_BITN                                         1
#define CPU_ITM_STIM15_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM15_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM15_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM15_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM15_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM15_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM16
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM16_DISABLED                                     0x00000002
#define CPU_ITM_STIM16_DISABLED_BITN                                         1
#define CPU_ITM_STIM16_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM16_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM16_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM16_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM16_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM16_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM17
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM17_DISABLED                                     0x00000002
#define CPU_ITM_STIM17_DISABLED_BITN                                         1
#define CPU_ITM_STIM17_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM17_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM17_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM17_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM17_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM17_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM18
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM18_DISABLED                                     0x00000002
#define CPU_ITM_STIM18_DISABLED_BITN                                         1
#define CPU_ITM_STIM18_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM18_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM18_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM18_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM18_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM18_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM19
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM19_DISABLED                                     0x00000002
#define CPU_ITM_STIM19_DISABLED_BITN                                         1
#define CPU_ITM_STIM19_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM19_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM19_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM19_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM19_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM19_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM20
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM20_DISABLED                                     0x00000002
#define CPU_ITM_STIM20_DISABLED_BITN                                         1
#define CPU_ITM_STIM20_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM20_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM20_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM20_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM20_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM20_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM21
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM21_DISABLED                                     0x00000002
#define CPU_ITM_STIM21_DISABLED_BITN                                         1
#define CPU_ITM_STIM21_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM21_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM21_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM21_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM21_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM21_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM22
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM22_DISABLED                                     0x00000002
#define CPU_ITM_STIM22_DISABLED_BITN                                         1
#define CPU_ITM_STIM22_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM22_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM22_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM22_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM22_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM22_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM23
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM23_DISABLED                                     0x00000002
#define CPU_ITM_STIM23_DISABLED_BITN                                         1
#define CPU_ITM_STIM23_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM23_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM23_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM23_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM23_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM23_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM24
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM24_DISABLED                                     0x00000002
#define CPU_ITM_STIM24_DISABLED_BITN                                         1
#define CPU_ITM_STIM24_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM24_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM24_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM24_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM24_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM24_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM25
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM25_DISABLED                                     0x00000002
#define CPU_ITM_STIM25_DISABLED_BITN                                         1
#define CPU_ITM_STIM25_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM25_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM25_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM25_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM25_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM25_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM26
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM26_DISABLED                                     0x00000002
#define CPU_ITM_STIM26_DISABLED_BITN                                         1
#define CPU_ITM_STIM26_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM26_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM26_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM26_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM26_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM26_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM27
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM27_DISABLED                                     0x00000002
#define CPU_ITM_STIM27_DISABLED_BITN                                         1
#define CPU_ITM_STIM27_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM27_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM27_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM27_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM27_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM27_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM28
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM28_DISABLED                                     0x00000002
#define CPU_ITM_STIM28_DISABLED_BITN                                         1
#define CPU_ITM_STIM28_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM28_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM28_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM28_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM28_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM28_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM29
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM29_DISABLED                                     0x00000002
#define CPU_ITM_STIM29_DISABLED_BITN                                         1
#define CPU_ITM_STIM29_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM29_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM29_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM29_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM29_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM29_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM30
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM30_DISABLED                                     0x00000002
#define CPU_ITM_STIM30_DISABLED_BITN                                         1
#define CPU_ITM_STIM30_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM30_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM30_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM30_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM30_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM30_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_STIM31
//
//*****************************************************************************
// Field:     [1] DISABLED
//
// Indicates whether the Stimulus Port is enabled or disabled
#define CPU_ITM_STIM31_DISABLED                                     0x00000002
#define CPU_ITM_STIM31_DISABLED_BITN                                         1
#define CPU_ITM_STIM31_DISABLED_M                                   0x00000002
#define CPU_ITM_STIM31_DISABLED_S                                            1

// Field:     [0] FIFOREADY
//
// Indicates whether the Stimulus Port can accept data
#define CPU_ITM_STIM31_FIFOREADY                                    0x00000001
#define CPU_ITM_STIM31_FIFOREADY_BITN                                        0
#define CPU_ITM_STIM31_FIFOREADY_M                                  0x00000001
#define CPU_ITM_STIM31_FIFOREADY_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_TER0
//
//*****************************************************************************
// Field:  [31:0] STIMENA
//
// For STIMENA[m] in ITM_TER*n, controls whether ITM_STIM(32*n + m) is enabled
#define CPU_ITM_TER0_STIMENA_W                                              32
#define CPU_ITM_TER0_STIMENA_M                                      0xFFFFFFFF
#define CPU_ITM_TER0_STIMENA_S                                               0

//*****************************************************************************
//
// Register: CPU_ITM_O_TPR
//
//*****************************************************************************
// Field:  [31:0] PRIVMASK
//
// For PRIVMASK[m], defines the access permissions of ITM_STIM Stimulus Ports
// 8m to 8m+7 inclusive
#define CPU_ITM_TPR_PRIVMASK_W                                              32
#define CPU_ITM_TPR_PRIVMASK_M                                      0xFFFFFFFF
#define CPU_ITM_TPR_PRIVMASK_S                                               0

//*****************************************************************************
//
// Register: CPU_ITM_O_TCR
//
//*****************************************************************************
// Field:    [23] BUSY
//
// Indicates whether the ITM is currently processing events
#define CPU_ITM_TCR_BUSY                                            0x00800000
#define CPU_ITM_TCR_BUSY_BITN                                               23
#define CPU_ITM_TCR_BUSY_M                                          0x00800000
#define CPU_ITM_TCR_BUSY_S                                                  23

// Field: [22:16] TraceBusID
//
// Identifier for multi-source trace stream formatting. If multi-source trace
// is in use, the debugger must write a unique non-zero trace ID value to this
// field
#define CPU_ITM_TCR_TRACEBUSID_W                                             7
#define CPU_ITM_TCR_TRACEBUSID_M                                    0x007F0000
#define CPU_ITM_TCR_TRACEBUSID_S                                            16

// Field: [11:10] GTSFREQ
//
// Defines how often the ITM generates a global timestamp, based on the global
// timestamp clock frequency, or disables generation of global timestamps
#define CPU_ITM_TCR_GTSFREQ_W                                                2
#define CPU_ITM_TCR_GTSFREQ_M                                       0x00000C00
#define CPU_ITM_TCR_GTSFREQ_S                                               10

// Field:   [9:8] TSPrescale
//
// Local timestamp prescaler, used with the trace packet reference clock
#define CPU_ITM_TCR_TSPRESCALE_W                                             2
#define CPU_ITM_TCR_TSPRESCALE_M                                    0x00000300
#define CPU_ITM_TCR_TSPRESCALE_S                                             8

// Field:     [5] STALLENA
//
// Stall the PE to guarantee delivery of Data Trace packets.
#define CPU_ITM_TCR_STALLENA                                        0x00000020
#define CPU_ITM_TCR_STALLENA_BITN                                            5
#define CPU_ITM_TCR_STALLENA_M                                      0x00000020
#define CPU_ITM_TCR_STALLENA_S                                               5

// Field:     [4] SWOENA
//
// Enables asynchronous clocking of the timestamp counter
#define CPU_ITM_TCR_SWOENA                                          0x00000010
#define CPU_ITM_TCR_SWOENA_BITN                                              4
#define CPU_ITM_TCR_SWOENA_M                                        0x00000010
#define CPU_ITM_TCR_SWOENA_S                                                 4

// Field:     [3] TXENA
//
// Enables forwarding of hardware event packet from the DWT unit to the ITM for
// output to the TPIU
#define CPU_ITM_TCR_TXENA                                           0x00000008
#define CPU_ITM_TCR_TXENA_BITN                                               3
#define CPU_ITM_TCR_TXENA_M                                         0x00000008
#define CPU_ITM_TCR_TXENA_S                                                  3

// Field:     [2] SYNCENA
//
// Enables Synchronization packet transmission for a synchronous TPIU
#define CPU_ITM_TCR_SYNCENA                                         0x00000004
#define CPU_ITM_TCR_SYNCENA_BITN                                             2
#define CPU_ITM_TCR_SYNCENA_M                                       0x00000004
#define CPU_ITM_TCR_SYNCENA_S                                                2

// Field:     [1] TSENA
//
// Enables Local timestamp generation
#define CPU_ITM_TCR_TSENA                                           0x00000002
#define CPU_ITM_TCR_TSENA_BITN                                               1
#define CPU_ITM_TCR_TSENA_M                                         0x00000002
#define CPU_ITM_TCR_TSENA_S                                                  1

// Field:     [0] ITMENA
//
// Enables the ITM
#define CPU_ITM_TCR_ITMENA                                          0x00000001
#define CPU_ITM_TCR_ITMENA_BITN                                              0
#define CPU_ITM_TCR_ITMENA_M                                        0x00000001
#define CPU_ITM_TCR_ITMENA_S                                                 0

//*****************************************************************************
//
// Register: CPU_ITM_O_INT_ATREADY
//
//*****************************************************************************
// Field:     [1] AFVALID
//
// A read of this bit returns the value of AFVALID
#define CPU_ITM_INT_ATREADY_AFVALID                                 0x00000002
#define CPU_ITM_INT_ATREADY_AFVALID_BITN                                     1
#define CPU_ITM_INT_ATREADY_AFVALID_M                               0x00000002
#define CPU_ITM_INT_ATREADY_AFVALID_S                                        1

// Field:     [0] ATREADY
//
// A read of this bit returns the value of ATREADY
#define CPU_ITM_INT_ATREADY_ATREADY                                 0x00000001
#define CPU_ITM_INT_ATREADY_ATREADY_BITN                                     0
#define CPU_ITM_INT_ATREADY_ATREADY_M                               0x00000001
#define CPU_ITM_INT_ATREADY_ATREADY_S                                        0

//*****************************************************************************
//
// Register: CPU_ITM_O_INT_ATVALID
//
//*****************************************************************************
// Field:     [1] AFREADY
//
// A write to this bit gives the value of AFREADY
#define CPU_ITM_INT_ATVALID_AFREADY                                 0x00000002
#define CPU_ITM_INT_ATVALID_AFREADY_BITN                                     1
#define CPU_ITM_INT_ATVALID_AFREADY_M                               0x00000002
#define CPU_ITM_INT_ATVALID_AFREADY_S                                        1

// Field:     [0] ATREADY
//
// A write to this bit gives the value of ATVALID
#define CPU_ITM_INT_ATVALID_ATREADY                                 0x00000001
#define CPU_ITM_INT_ATVALID_ATREADY_BITN                                     0
#define CPU_ITM_INT_ATVALID_ATREADY_M                               0x00000001
#define CPU_ITM_INT_ATVALID_ATREADY_S                                        0

//*****************************************************************************
//
// Register: CPU_ITM_O_ITCTRL
//
//*****************************************************************************
// Field:     [0] IME
//
// Integration mode enable bit - The possible values are:  0 - The trace unit
// is not in integration mode. 1 - The trace unit is in integration mode. This
// mode enables: A debug agent to perform topology detection. SoC test software
// to perform integration testing.
#define CPU_ITM_ITCTRL_IME                                          0x00000001
#define CPU_ITM_ITCTRL_IME_BITN                                              0
#define CPU_ITM_ITCTRL_IME_M                                        0x00000001
#define CPU_ITM_ITCTRL_IME_S                                                 0

//*****************************************************************************
//
// Register: CPU_ITM_O_DEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// Defines the architect of the component. Bits [31:28] are the JEP106
// continuation code (JEP106 bank ID, minus 1) and bits [27:21] are the JEP106
// ID code.
#define CPU_ITM_DEVARCH_ARCHITECT_W                                         11
#define CPU_ITM_DEVARCH_ARCHITECT_M                                 0xFFE00000
#define CPU_ITM_DEVARCH_ARCHITECT_S                                         21

// Field:    [20] PRESENT
//
// Defines that the DEVARCH register is present
#define CPU_ITM_DEVARCH_PRESENT                                     0x00100000
#define CPU_ITM_DEVARCH_PRESENT_BITN                                        20
#define CPU_ITM_DEVARCH_PRESENT_M                                   0x00100000
#define CPU_ITM_DEVARCH_PRESENT_S                                           20

// Field: [19:16] REVISION
//
// Defines the architecture revision of the component
#define CPU_ITM_DEVARCH_REVISION_W                                           4
#define CPU_ITM_DEVARCH_REVISION_M                                  0x000F0000
#define CPU_ITM_DEVARCH_REVISION_S                                          16

// Field: [15:12] ARCHVER
//
// Defines the architecture version of the component
#define CPU_ITM_DEVARCH_ARCHVER_W                                            4
#define CPU_ITM_DEVARCH_ARCHVER_M                                   0x0000F000
#define CPU_ITM_DEVARCH_ARCHVER_S                                           12

// Field:  [11:0] ARCHPART
//
// Defines the architecture of the component
#define CPU_ITM_DEVARCH_ARCHPART_W                                          12
#define CPU_ITM_DEVARCH_ARCHPART_M                                  0x00000FFF
#define CPU_ITM_DEVARCH_ARCHPART_S                                           0

//*****************************************************************************
//
// Register: CPU_ITM_O_DEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// Component sub-type
#define CPU_ITM_DEVTYPE_SUB_W                                                4
#define CPU_ITM_DEVTYPE_SUB_M                                       0x000000F0
#define CPU_ITM_DEVTYPE_SUB_S                                                4

// Field:   [3:0] MAJOR
//
// Component major type
#define CPU_ITM_DEVTYPE_MAJOR_W                                              4
#define CPU_ITM_DEVTYPE_MAJOR_M                                     0x0000000F
#define CPU_ITM_DEVTYPE_MAJOR_S                                              0

//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR4_SIZE_W                                                 4
#define CPU_ITM_PIDR4_SIZE_M                                        0x000000F0
#define CPU_ITM_PIDR4_SIZE_S                                                 4

// Field:   [3:0] DES_2
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR4_DES_2_W                                                4
#define CPU_ITM_PIDR4_DES_2_M                                       0x0000000F
#define CPU_ITM_PIDR4_DES_2_S                                                0

//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR0_PART_0_W                                               8
#define CPU_ITM_PIDR0_PART_0_M                                      0x000000FF
#define CPU_ITM_PIDR0_PART_0_S                                               0

//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR1_DES_0_W                                                4
#define CPU_ITM_PIDR1_DES_0_M                                       0x000000F0
#define CPU_ITM_PIDR1_DES_0_S                                                4

// Field:   [3:0] PART_1
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR1_PART_1_W                                               4
#define CPU_ITM_PIDR1_PART_1_M                                      0x0000000F
#define CPU_ITM_PIDR1_PART_1_S                                               0

//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR2_REVISION_W                                             4
#define CPU_ITM_PIDR2_REVISION_M                                    0x000000F0
#define CPU_ITM_PIDR2_REVISION_S                                             4

// Field:     [3] JEDEC
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR2_JEDEC                                         0x00000008
#define CPU_ITM_PIDR2_JEDEC_BITN                                             3
#define CPU_ITM_PIDR2_JEDEC_M                                       0x00000008
#define CPU_ITM_PIDR2_JEDEC_S                                                3

// Field:   [2:0] DES_1
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR2_DES_1_W                                                3
#define CPU_ITM_PIDR2_DES_1_M                                       0x00000007
#define CPU_ITM_PIDR2_DES_1_S                                                0

//*****************************************************************************
//
// Register: CPU_ITM_O_PIDR3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR3_REVAND_W                                               4
#define CPU_ITM_PIDR3_REVAND_M                                      0x000000F0
#define CPU_ITM_PIDR3_REVAND_S                                               4

// Field:   [3:0] CMOD
//
// See CoreSight Architecture Specification
#define CPU_ITM_PIDR3_CMOD_W                                                 4
#define CPU_ITM_PIDR3_CMOD_M                                        0x0000000F
#define CPU_ITM_PIDR3_CMOD_S                                                 0

//*****************************************************************************
//
// Register: CPU_ITM_O_CIDR0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// See CoreSight Architecture Specification
#define CPU_ITM_CIDR0_PRMBL_0_W                                              8
#define CPU_ITM_CIDR0_PRMBL_0_M                                     0x000000FF
#define CPU_ITM_CIDR0_PRMBL_0_S                                              0

//*****************************************************************************
//
// Register: CPU_ITM_O_CIDR1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// See CoreSight Architecture Specification
#define CPU_ITM_CIDR1_CLASS_W                                                4
#define CPU_ITM_CIDR1_CLASS_M                                       0x000000F0
#define CPU_ITM_CIDR1_CLASS_S                                                4

// Field:   [3:0] PRMBL_1
//
// See CoreSight Architecture Specification
#define CPU_ITM_CIDR1_PRMBL_1_W                                              4
#define CPU_ITM_CIDR1_PRMBL_1_M                                     0x0000000F
#define CPU_ITM_CIDR1_PRMBL_1_S                                              0

//*****************************************************************************
//
// Register: CPU_ITM_O_CIDR2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// See CoreSight Architecture Specification
#define CPU_ITM_CIDR2_PRMBL_2_W                                              8
#define CPU_ITM_CIDR2_PRMBL_2_M                                     0x000000FF
#define CPU_ITM_CIDR2_PRMBL_2_S                                              0

//*****************************************************************************
//
// Register: CPU_ITM_O_CIDR3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// See CoreSight Architecture Specification
#define CPU_ITM_CIDR3_PRMBL_3_W                                              8
#define CPU_ITM_CIDR3_PRMBL_3_M                                     0x000000FF
#define CPU_ITM_CIDR3_PRMBL_3_S                                              0


#endif // __CPU_ITM__
