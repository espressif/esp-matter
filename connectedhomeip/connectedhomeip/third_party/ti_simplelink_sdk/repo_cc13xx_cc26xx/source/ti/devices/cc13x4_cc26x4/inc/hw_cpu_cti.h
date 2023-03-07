/******************************************************************************
*  Filename:       hw_cpu_cti_h
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

#ifndef __HW_CPU_CTI_H__
#define __HW_CPU_CTI_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_CTI component
//
//*****************************************************************************
// CTI Control Register
#define CPU_CTI_O_CTICONTROL                                        0x00000000

// CTI Interrupt Acknowledge Register
#define CPU_CTI_O_CTIINTACK                                         0x00000010

// CTI Application Trigger Set Register
#define CPU_CTI_O_CTIAPPSET                                         0x00000014

// CTI Application Trigger Clear Register
#define CPU_CTI_O_CTIAPPCLEAR                                       0x00000018

// CTI Application Pulse Register
#define CPU_CTI_O_CTIAPPPULSE                                       0x0000001C

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN0                                          0x00000020

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN1                                          0x00000024

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN2                                          0x00000028

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN3                                          0x0000002C

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN4                                          0x00000030

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN5                                          0x00000034

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN6                                          0x00000038

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIINEN7                                          0x0000003C

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN0                                         0x000000A0

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN1                                         0x000000A4

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN2                                         0x000000A8

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN3                                         0x000000AC

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN4                                         0x000000B0

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN5                                         0x000000B4

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN6                                         0x000000B8

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTIOUTEN7                                         0x000000BC

// CTI Trigger to Channel Enable Registers
#define CPU_CTI_O_CTITRIGINSTATUS                                   0x00000130

// CTI Trigger In Status Register
#define CPU_CTI_O_CTITRIGOUTSTATUS                                  0x00000134

// CTI Channel In Status Register
#define CPU_CTI_O_CTICHINSTATUS                                     0x00000138

// Enable CTI Channel Gate register
#define CPU_CTI_O_CTIGATE                                           0x00000140

// External Multiplexer Control register
#define CPU_CTI_O_ASICCTL                                           0x00000144

// Integration Test Channel Output register
#define CPU_CTI_O_ITCHOUT                                           0x00000EE4

// Integration Test Trigger Output register
#define CPU_CTI_O_ITTRIGOUT                                         0x00000EE8

// Integration Test Channel Input register
#define CPU_CTI_O_ITCHIN                                            0x00000EF4

// Integration Mode Control register
#define CPU_CTI_O_ITCTRL                                            0x00000F00

// Device Architecture register
#define CPU_CTI_O_DEVARCH                                           0x00000FBC

// Device Configuration register
#define CPU_CTI_O_DEVID                                             0x00000FC8

// Device Type Identifier register
#define CPU_CTI_O_DEVTYPE                                           0x00000FCC

// CoreSight Periperal ID4
#define CPU_CTI_O_PIDR4                                             0x00000FD0

// CoreSight Periperal ID5
#define CPU_CTI_O_PIDR5                                             0x00000FD4

// CoreSight Periperal ID6
#define CPU_CTI_O_PIDR6                                             0x00000FD8

// CoreSight Periperal ID7
#define CPU_CTI_O_PIDR7                                             0x00000FDC

// CoreSight Periperal ID0
#define CPU_CTI_O_PIDR0                                             0x00000FE0

// CoreSight Periperal ID1
#define CPU_CTI_O_PIDR1                                             0x00000FE4

// CoreSight Periperal ID2
#define CPU_CTI_O_PIDR2                                             0x00000FE8

// CoreSight Periperal ID3
#define CPU_CTI_O_PIDR3                                             0x00000FEC

// CoreSight Component ID0
#define CPU_CTI_O_CIDR0                                             0x00000FF0

// CoreSight Component ID1
#define CPU_CTI_O_CIDR1                                             0x00000FF4

// CoreSight Component ID2
#define CPU_CTI_O_CIDR2                                             0x00000FF8

// CoreSight Component ID3
#define CPU_CTI_O_CIDR3                                             0x00000FFC

//*****************************************************************************
//
// Register: CPU_CTI_O_CTICONTROL
//
//*****************************************************************************
// Field:     [0] GLBEN
//
// Enables or disables the CTI.
#define CPU_CTI_CTICONTROL_GLBEN                                    0x00000001
#define CPU_CTI_CTICONTROL_GLBEN_BITN                                        0
#define CPU_CTI_CTICONTROL_GLBEN_M                                  0x00000001
#define CPU_CTI_CTICONTROL_GLBEN_S                                           0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINTACK
//
//*****************************************************************************
// Field:   [7:0] INTACK
//
// Acknowledges the corresponding ctitrigout output. There is one bit of the
// register for each ctitrigout output. When a 1 is written to a bit in this
// register, the corresponding ctitrigout is acknowledged, causing it to be
// cleared
#define CPU_CTI_CTIINTACK_INTACK_W                                           8
#define CPU_CTI_CTIINTACK_INTACK_M                                  0x000000FF
#define CPU_CTI_CTIINTACK_INTACK_S                                           0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIAPPSET
//
//*****************************************************************************
// Field:   [3:0] APPSET
//
// Setting a bit HIGH generates a channel event for the selected channel. There
// is one bit of the register for each channel. Reads as follows: 0 Application
// trigger is inactive. 1 Application trigger is active. Writes as follows: 0
// No effect. 1 Generate channel event.
#define CPU_CTI_CTIAPPSET_APPSET_W                                           4
#define CPU_CTI_CTIAPPSET_APPSET_M                                  0x0000000F
#define CPU_CTI_CTIAPPSET_APPSET_S                                           0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIAPPCLEAR
//
//*****************************************************************************
// Field:   [3:0] APPCLEAR
//
// Sets the corresponding bits in the CTIAPPSET to 0. There is one bit of the
// register for each channel.On writes, for each bit: 0 Has no effect. 1 Clears
// the corresponding channel event.Setting a bit HIGH generates a channel event
// for the selected channel. There is one bit of the register for each channel.
// Reads as follows: 0 Application trigger is inactive. 1 Application trigger
// is active. Writes as follows: 0 No effect. 1 Generate channel event.
#define CPU_CTI_CTIAPPCLEAR_APPCLEAR_W                                       4
#define CPU_CTI_CTIAPPCLEAR_APPCLEAR_M                              0x0000000F
#define CPU_CTI_CTIAPPCLEAR_APPCLEAR_S                                       0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIAPPPULSE
//
//*****************************************************************************
// Field:   [3:0] APPULSE
//
// Setting a bit HIGH generates a channel event pulse for the selected channel.
// There is one bit of the register for each channel. On writes, for each bit:
// 0 Has no effect. 1 Generate an event pulse on the corresponding channel.
#define CPU_CTI_CTIAPPPULSE_APPULSE_W                                        4
#define CPU_CTI_CTIAPPPULSE_APPULSE_M                               0x0000000F
#define CPU_CTI_CTIAPPPULSE_APPULSE_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN0
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 0 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 0,
// ctitrigin[0], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN0_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN0_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN0_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN1
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 1 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 1,
// ctitrigin[1], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN1_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN1_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN1_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN2
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 2 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 2,
// ctitrigin[2], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN2_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN2_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN2_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN3
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 3 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 3,
// ctitrigin[3], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN3_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN3_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN3_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN4
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 4 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 4,
// ctitrigin[4], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN4_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN4_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN4_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN5
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 5 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 5,
// ctitrigin[5], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN5_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN5_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN5_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN6
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 6 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 6,
// ctitrigin[6], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN6_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN6_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN6_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIINEN7
//
//*****************************************************************************
// Field:   [3:0] TRIGINEN
//
// Enables a cross trigger event to the corresponding channel when a ctitrigin
// input is activated. There is one bit of the field for each of the four
// channels. On writes, for each bit: 0 Input trigger 7 events are ignored by
// the corresponding channel. 1 When an event is received on input trigger 7,
// ctitrigin[7], generate an event on the channel corresponding to this bit.
#define CPU_CTI_CTIINEN7_TRIGINEN_W                                          4
#define CPU_CTI_CTIINEN7_TRIGINEN_M                                 0x0000000F
#define CPU_CTI_CTIINEN7_TRIGINEN_S                                          0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN0
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 0. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 0, ctitrigout[0].
#define CPU_CTI_CTIOUTEN0_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN0_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN0_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN1
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 1. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 1, ctitrigout[1].
#define CPU_CTI_CTIOUTEN1_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN1_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN1_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN2
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 2. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 2, ctitrigout[2].
#define CPU_CTI_CTIOUTEN2_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN2_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN2_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN3
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 3. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 3, ctitrigout[3].
#define CPU_CTI_CTIOUTEN3_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN3_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN3_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN4
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 4. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 4, ctitrigout[4].
#define CPU_CTI_CTIOUTEN4_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN4_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN4_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN5
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 5. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 5, ctitrigout[5].
#define CPU_CTI_CTIOUTEN5_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN5_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN5_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN6
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 6. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 6, ctitrigout[6].
#define CPU_CTI_CTIOUTEN6_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN6_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN6_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIOUTEN7
//
//*****************************************************************************
// Field:   [3:0] TRIGOUTEN
//
// Enables a cross trigger event to ctitrigout when the corresponding channel
// is activated. There is one bit of the field for each of the four channels.
// On writes, for each bit 0 The corresponding channel is ignored by the output
// trigger 7. 1 When an event occurs on the channel corresponding to this bit,
// generate an event on output event 7, ctitrigout[7].
#define CPU_CTI_CTIOUTEN7_TRIGOUTEN_W                                        4
#define CPU_CTI_CTIOUTEN7_TRIGOUTEN_M                               0x0000000F
#define CPU_CTI_CTIOUTEN7_TRIGOUTEN_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTITRIGINSTATUS
//
//*****************************************************************************
// Field:   [7:0] TRIGINSTATUS
//
// Shows the status of the ctitrigin inputs. There is one bit of the field for
// each trigger input.Because the register provides a view of the raw ctitrigin
// inputs, the reset value is UNKNOWN. 1 ctitrigin is active. 0 ctitrigin is
// inactive
#define CPU_CTI_CTITRIGINSTATUS_TRIGINSTATUS_W                               8
#define CPU_CTI_CTITRIGINSTATUS_TRIGINSTATUS_M                      0x000000FF
#define CPU_CTI_CTITRIGINSTATUS_TRIGINSTATUS_S                               0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTITRIGOUTSTATUS
//
//*****************************************************************************
// Field:   [7:0] TRIGOUTSTATUS
//
// Shows the status of the ctitrigout outputs. There is one bit of the field
// for each trigger output. 1 ctitrigout is active. 0 ctitrigout is inactive.
#define CPU_CTI_CTITRIGOUTSTATUS_TRIGOUTSTATUS_W                             8
#define CPU_CTI_CTITRIGOUTSTATUS_TRIGOUTSTATUS_M                    0x000000FF
#define CPU_CTI_CTITRIGOUTSTATUS_TRIGOUTSTATUS_S                             0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTICHINSTATUS
//
//*****************************************************************************
// Field:   [3:0] CTICHINSTATUS
//
// Shows the status of the ctichin inputs. There is one bit of the field for
// each channel input.Because the register provides a view of the raw ctichin
// inputs, the reset value is UNKNOWN. 0 ctichin is inactive. 1 ctichin is
// active.
#define CPU_CTI_CTICHINSTATUS_CTICHINSTATUS_W                                4
#define CPU_CTI_CTICHINSTATUS_CTICHINSTATUS_M                       0x0000000F
#define CPU_CTI_CTICHINSTATUS_CTICHINSTATUS_S                                0

//*****************************************************************************
//
// Register: CPU_CTI_O_CTIGATE
//
//*****************************************************************************
// Field:     [3] CTIGATEEN3
//
// Enable ctichout3. Set to 0 to disable channel propagation.
#define CPU_CTI_CTIGATE_CTIGATEEN3                                  0x00000008
#define CPU_CTI_CTIGATE_CTIGATEEN3_BITN                                      3
#define CPU_CTI_CTIGATE_CTIGATEEN3_M                                0x00000008
#define CPU_CTI_CTIGATE_CTIGATEEN3_S                                         3

// Field:     [2] CTIGATEEN2
//
// Enable ctichout2. Set to 0 to disable channel propagation.
#define CPU_CTI_CTIGATE_CTIGATEEN2                                  0x00000004
#define CPU_CTI_CTIGATE_CTIGATEEN2_BITN                                      2
#define CPU_CTI_CTIGATE_CTIGATEEN2_M                                0x00000004
#define CPU_CTI_CTIGATE_CTIGATEEN2_S                                         2

// Field:     [1] CTIGATEEN1
//
// Enable ctichout1. Set to 0 to disable channel propagation.
#define CPU_CTI_CTIGATE_CTIGATEEN1                                  0x00000002
#define CPU_CTI_CTIGATE_CTIGATEEN1_BITN                                      1
#define CPU_CTI_CTIGATE_CTIGATEEN1_M                                0x00000002
#define CPU_CTI_CTIGATE_CTIGATEEN1_S                                         1

// Field:     [0] CTIGATEEN0
//
// Enable ctichout0. Set to 0 to disable channel propagation.
#define CPU_CTI_CTIGATE_CTIGATEEN0                                  0x00000001
#define CPU_CTI_CTIGATE_CTIGATEEN0_BITN                                      0
#define CPU_CTI_CTIGATE_CTIGATEEN0_M                                0x00000001
#define CPU_CTI_CTIGATE_CTIGATEEN0_S                                         0

//*****************************************************************************
//
// Register: CPU_CTI_O_ASICCTL
//
//*****************************************************************************
// Field:   [7:0] ASICCTL
//
// When external multiplexing is implemented for trigger signals, then the
// number of multiplexed signals on each trigger must be shown in the Device ID
// Register. This is done using a Verilog define EXTMUXNUM
#define CPU_CTI_ASICCTL_ASICCTL_W                                            8
#define CPU_CTI_ASICCTL_ASICCTL_M                                   0x000000FF
#define CPU_CTI_ASICCTL_ASICCTL_S                                            0

//*****************************************************************************
//
// Register: CPU_CTI_O_ITCHOUT
//
//*****************************************************************************
// Field:   [3:0] CTCHOUT
//
// Sets the value of the ctichout outputs
#define CPU_CTI_ITCHOUT_CTCHOUT_W                                            4
#define CPU_CTI_ITCHOUT_CTCHOUT_M                                   0x0000000F
#define CPU_CTI_ITCHOUT_CTCHOUT_S                                            0

//*****************************************************************************
//
// Register: CPU_CTI_O_ITTRIGOUT
//
//*****************************************************************************
// Field:   [7:0] CTTRIGOUT
//
// Sets the value of the ctitrigout outputs
#define CPU_CTI_ITTRIGOUT_CTTRIGOUT_W                                        8
#define CPU_CTI_ITTRIGOUT_CTTRIGOUT_M                               0x000000FF
#define CPU_CTI_ITTRIGOUT_CTTRIGOUT_S                                        0

//*****************************************************************************
//
// Register: CPU_CTI_O_ITCHIN
//
//*****************************************************************************
// Field:   [3:0] CTCHIN
//
// Reads the value of the ctichin inputs
#define CPU_CTI_ITCHIN_CTCHIN_W                                              4
#define CPU_CTI_ITCHIN_CTCHIN_M                                     0x0000000F
#define CPU_CTI_ITCHIN_CTCHIN_S                                              0

//*****************************************************************************
//
// Register: CPU_CTI_O_ITCTRL
//
//*****************************************************************************
// Field:     [0] IME
//
// Integration Mode Enable. 0 Disable integration mode. 1 Enable integration
// mode
#define CPU_CTI_ITCTRL_IME                                          0x00000001
#define CPU_CTI_ITCTRL_IME_BITN                                              0
#define CPU_CTI_ITCTRL_IME_M                                        0x00000001
#define CPU_CTI_ITCTRL_IME_S                                                 0

//*****************************************************************************
//
// Register: CPU_CTI_O_DEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// Indicates the component architect: 0x23B ARM
#define CPU_CTI_DEVARCH_ARCHITECT_W                                         11
#define CPU_CTI_DEVARCH_ARCHITECT_M                                 0xFFE00000
#define CPU_CTI_DEVARCH_ARCHITECT_S                                         21

// Field:    [20] PRESENT
//
// Indicates whether the DEVARCH register is present: 0x1 Present.
#define CPU_CTI_DEVARCH_PRESENT                                     0x00100000
#define CPU_CTI_DEVARCH_PRESENT_BITN                                        20
#define CPU_CTI_DEVARCH_PRESENT_M                                   0x00100000
#define CPU_CTI_DEVARCH_PRESENT_S                                           20

// Field: [19:16] REVISION
//
// Indicates the architecture revision: 0x1 Revision 0.
#define CPU_CTI_DEVARCH_REVISION_W                                           4
#define CPU_CTI_DEVARCH_REVISION_M                                  0x000F0000
#define CPU_CTI_DEVARCH_REVISION_S                                          16

// Field:  [15:0] ARCHID
//
// Indicates the component: 0x0A34 CoreSight GPR
#define CPU_CTI_DEVARCH_ARCHID_W                                            16
#define CPU_CTI_DEVARCH_ARCHID_M                                    0x0000FFFF
#define CPU_CTI_DEVARCH_ARCHID_S                                             0

//*****************************************************************************
//
// Register: CPU_CTI_O_DEVID
//
//*****************************************************************************
// Field: [19:16] NUMCH
//
// Number of ECT channels available
#define CPU_CTI_DEVID_NUMCH_W                                                4
#define CPU_CTI_DEVID_NUMCH_M                                       0x000F0000
#define CPU_CTI_DEVID_NUMCH_S                                               16

// Field:  [15:8] NUMTRIG
//
// Number of ECT triggers available
#define CPU_CTI_DEVID_NUMTRIG_W                                              8
#define CPU_CTI_DEVID_NUMTRIG_M                                     0x0000FF00
#define CPU_CTI_DEVID_NUMTRIG_S                                              8

// Field:   [4:0] EXTMUXNUM
//
// Indicates the number of multiplexers available on Trigger Inputs and Trigger
// Outputs that are using asicctl. The default value of 0b00000 indicates that
// no multiplexing is present. This value of this bit depends on the Verilog
// define EXTMUXNUM that you must change accordingly.
#define CPU_CTI_DEVID_EXTMUXNUM_W                                            5
#define CPU_CTI_DEVID_EXTMUXNUM_M                                   0x0000001F
#define CPU_CTI_DEVID_EXTMUXNUM_S                                            0

//*****************************************************************************
//
// Register: CPU_CTI_O_DEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// Sub-classification of the type of the debug component as specified in the
// ARM Architecture Specification within the major classification as specified
// in the MAJOR field. 0b0001 Indicates that this component is a
// cross-triggering component
#define CPU_CTI_DEVTYPE_SUB_W                                                4
#define CPU_CTI_DEVTYPE_SUB_M                                       0x000000F0
#define CPU_CTI_DEVTYPE_SUB_S                                                4

// Field:   [3:0] MAJOR
//
// Major classification of the type of the debug component as specified in the
// ARM Architecture Specification for this debug and trace component. 0b0100
// Indicates that this component allows a debugger to control other components
// in a CoreSight SoC-400 system.
#define CPU_CTI_DEVTYPE_MAJOR_W                                              4
#define CPU_CTI_DEVTYPE_MAJOR_M                                     0x0000000F
#define CPU_CTI_DEVTYPE_MAJOR_S                                              0

//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// Always 0b0000. Indicates that the device only occupies 4KB of memory.
#define CPU_CTI_PIDR4_SIZE_W                                                 4
#define CPU_CTI_PIDR4_SIZE_M                                        0x000000F0
#define CPU_CTI_PIDR4_SIZE_S                                                 4

// Field:   [3:0] DES_2
//
// Together, PIDR1.DES_0, PIDR2.DES_1, and PIDR4.DES_2 identify the designer of
// the component. 0b0100 JEDEC continuation code.
#define CPU_CTI_PIDR4_DES_2_W                                                4
#define CPU_CTI_PIDR4_DES_2_M                                       0x0000000F
#define CPU_CTI_PIDR4_DES_2_S                                                0

//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// Bits[7:0] of the 12-bit part number of the component. The designer of the
// component assigns this part number. 0x06 Indicates bits[7:0] of the part
// number of the component.
#define CPU_CTI_PIDR0_PART_0_W                                               8
#define CPU_CTI_PIDR0_PART_0_M                                      0x000000FF
#define CPU_CTI_PIDR0_PART_0_S                                               0

//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// Together, PIDR1.DES_0, PIDR2.DES_1, and PIDR4.DES_2 identify the designer of
// the component. 0b1011 ARM. Bits[3:0] of the JEDEC JEP106 Identity Code.
#define CPU_CTI_PIDR1_DES_0_W                                                4
#define CPU_CTI_PIDR1_DES_0_M                                       0x000000F0
#define CPU_CTI_PIDR1_DES_0_S                                                4

// Field:   [3:0] PART_1
//
// Bits[11:8] of the 12-bit part number of the component. The designer of the
// component assigns this part number. 0b1001 Indicates bits[11:8] of the part
// number of the component.
#define CPU_CTI_PIDR1_PART_1_W                                               4
#define CPU_CTI_PIDR1_PART_1_M                                      0x0000000F
#define CPU_CTI_PIDR1_PART_1_S                                               0

//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// 0b0101 This device is at r1p0.
#define CPU_CTI_PIDR2_REVISION_W                                             4
#define CPU_CTI_PIDR2_REVISION_M                                    0x000000F0
#define CPU_CTI_PIDR2_REVISION_S                                             4

// Field:     [3] JEDEC
//
// Always 1. Indicates that the JEDEC-assigned designer ID is used
#define CPU_CTI_PIDR2_JEDEC                                         0x00000008
#define CPU_CTI_PIDR2_JEDEC_BITN                                             3
#define CPU_CTI_PIDR2_JEDEC_M                                       0x00000008
#define CPU_CTI_PIDR2_JEDEC_S                                                3

// Field:   [2:0] DES_1
//
// Together, PIDR1.DES_0, PIDR2.DES_1, and PIDR4.DES_2 identify the designer of
// the component. 0b011 ARM. Bits[6:4] of the JEDEC JEP106 Identity Code
#define CPU_CTI_PIDR2_DES_1_W                                                3
#define CPU_CTI_PIDR2_DES_1_M                                       0x00000007
#define CPU_CTI_PIDR2_DES_1_S                                                0

//*****************************************************************************
//
// Register: CPU_CTI_O_PIDR3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// Indicates minor errata fixes specific to the revision of the component being
// used, for example metal fixes after implementation. In most cases, this
// field is 0b0000. ARM recommends that the component designers ensure that a
// metal fix can change this field if required, for example, by driving it from
// registers that reset to 0b0000. 0b0000 Indicates that there are no errata
// fixes to this component.
#define CPU_CTI_PIDR3_REVAND_W                                               4
#define CPU_CTI_PIDR3_REVAND_M                                      0x000000F0
#define CPU_CTI_PIDR3_REVAND_S                                               4

// Field:   [3:0] CMOD
//
// Customer Modified. Indicates whether the customer has modified the behavior
// of the component. In most cases, this field is 0b0000. Customers change this
// value when they make authorized modifications to this component. 0b0000
// Indicates that the customer has not modified this component.
#define CPU_CTI_PIDR3_CMOD_W                                                 4
#define CPU_CTI_PIDR3_CMOD_M                                        0x0000000F
#define CPU_CTI_PIDR3_CMOD_S                                                 0

//*****************************************************************************
//
// Register: CPU_CTI_O_CIDR0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// Preamble[0]. Contains bits[7:0] of the component identification code. 0x0D
// Bits[7:0] of the identification code.
#define CPU_CTI_CIDR0_PRMBL_0_W                                              8
#define CPU_CTI_CIDR0_PRMBL_0_M                                     0x000000FF
#define CPU_CTI_CIDR0_PRMBL_0_S                                              0

//*****************************************************************************
//
// Register: CPU_CTI_O_CIDR1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// Class of the component, for example, whether the component is a ROM table or
// a generic CoreSight component. Contains bits[15:12] of the component
// identification code. 0b1001 Indicates that the component is a CoreSight
// component.
#define CPU_CTI_CIDR1_CLASS_W                                                4
#define CPU_CTI_CIDR1_CLASS_M                                       0x000000F0
#define CPU_CTI_CIDR1_CLASS_S                                                4

// Field:   [3:0] PRMBL_1
//
// Preamble[1]. Contains bits[11:8] of the component identification code.
// 0b0000 Bits[11:8] of the identification code.
#define CPU_CTI_CIDR1_PRMBL_1_W                                              4
#define CPU_CTI_CIDR1_PRMBL_1_M                                     0x0000000F
#define CPU_CTI_CIDR1_PRMBL_1_S                                              0

//*****************************************************************************
//
// Register: CPU_CTI_O_CIDR2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// Preamble[2]. Contains bits[23:16] of the component identification code. 0x05
// Bits[23:16] of the identification code.
#define CPU_CTI_CIDR2_PRMBL_2_W                                              8
#define CPU_CTI_CIDR2_PRMBL_2_M                                     0x000000FF
#define CPU_CTI_CIDR2_PRMBL_2_S                                              0

//*****************************************************************************
//
// Register: CPU_CTI_O_CIDR3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// Preamble[3]. Contains bits[31:24] of the component identification code. 0xB1
// Bits[31:24] of the identification code.
#define CPU_CTI_CIDR3_PRMBL_3_W                                              8
#define CPU_CTI_CIDR3_PRMBL_3_M                                     0x000000FF
#define CPU_CTI_CIDR3_PRMBL_3_S                                              0


#endif // __CPU_CTI__
