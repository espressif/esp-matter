/******************************************************************************
*  Filename:       hw_aux_sysif_h
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

#ifndef __HW_AUX_SYSIF_H__
#define __HW_AUX_SYSIF_H__

//*****************************************************************************
//
// This section defines the register offsets of
// AUX_SYSIF component
//
//*****************************************************************************
// Operational Mode Request
#define AUX_SYSIF_O_OPMODEREQ                                       0x00000000

// Operational Mode Acknowledgement
#define AUX_SYSIF_O_OPMODEACK                                       0x00000004

// Event Synchronization Rate
#define AUX_SYSIF_O_EVSYNCRATE                                      0x00000048

// Peripheral Operational Rate
#define AUX_SYSIF_O_PEROPRATE                                       0x0000004C

// ADC Clock Control
#define AUX_SYSIF_O_ADCCLKCTL                                       0x00000050

// TDC Counter Clock Control
#define AUX_SYSIF_O_TDCCLKCTL                                       0x00000054

// TDC Reference Clock Control
#define AUX_SYSIF_O_TDCREFCLKCTL                                    0x00000058

// Real Time Counter Sub Second Increment 0
#define AUX_SYSIF_O_RTCSUBSECINC0                                   0x0000007C

// Real Time Counter Sub Second Increment 1
#define AUX_SYSIF_O_RTCSUBSECINC1                                   0x00000080

// Real Time Counter Sub Second Increment Control
#define AUX_SYSIF_O_RTCSUBSECINCCTL                                 0x00000084

// AON_RTC Event Clear
#define AUX_SYSIF_O_RTCEVCLR                                        0x00000090

// Timer Halt
#define AUX_SYSIF_O_TIMERHALT                                       0x000000A0

// Software Power Profiler
#define AUX_SYSIF_O_SWPWRPROF                                       0x000000B4

//*****************************************************************************
//
// Register: AUX_SYSIF_O_OPMODEREQ
//
//*****************************************************************************
// Field:   [1:0] REQ
//
// AUX operational mode request.
// ENUMs:
// PDLP                     Powerdown operational mode with wakeup to lowpower
//                          mode, characterized by:
//                          - Powerdown system power
//                          supply state (uLDO) request.
//                          -
//                          AON_PMCTL:AUXSCECLK.PD_SRC sets the SCE clock
//                          frequency (SCE_RATE).
//                          - An active wakeup flag
//                          overrides the operational mode externally to
//                          lowpower (LP) as long as the flag is set.
// PDA                      Powerdown operational mode with wakeup to active
//                          mode, characterized by:
//                          - Powerdown system power
//                          supply state (uLDO) request.
//                          -
//                          AON_PMCTL:AUXSCECLK.PD_SRC sets the SCE clock
//                          frequency (SCE_RATE).
//                          - An active wakeup flag
//                          overrides the operational mode externally to
//                          active (A) as long as the flag is set.
// LP                       Lowpower operational mode, characterized by:
//                          - Powerdown system power
//                          supply state (uLDO) request.
//                          - SCE clock frequency
//                          (SCE_RATE) equals SCLK_MF.
//                          - An active wakeup flag
//                          does not change operational mode.
// A                        Active operational mode, characterized by:
//                          - Active system power
//                          supply state (GLDO or DCDC) request.
//                          - AON_PMCTL:AUXSCECLK.SRC
//                          sets the SCE clock frequency (SCE_RATE).
//                          - An active wakeup flag
//                          does not change operational mode.
#define AUX_SYSIF_OPMODEREQ_REQ_W                                            2
#define AUX_SYSIF_OPMODEREQ_REQ_M                                   0x00000003
#define AUX_SYSIF_OPMODEREQ_REQ_S                                            0
#define AUX_SYSIF_OPMODEREQ_REQ_PDLP                                0x00000003
#define AUX_SYSIF_OPMODEREQ_REQ_PDA                                 0x00000002
#define AUX_SYSIF_OPMODEREQ_REQ_LP                                  0x00000001
#define AUX_SYSIF_OPMODEREQ_REQ_A                                   0x00000000

//*****************************************************************************
//
// Register: AUX_SYSIF_O_OPMODEACK
//
//*****************************************************************************
// Field:   [1:0] ACK
//
// AUX operational mode acknowledgement.
// ENUMs:
// PDLP                     Powerdown operational mode with wakeup to lowpower
//                          mode is acknowledged.
// PDA                      Powerdown operational mode with wakeup to active
//                          mode is acknowledged.
// LP                       Lowpower operational mode is acknowledged.
// A                        Active operational mode is acknowledged.
#define AUX_SYSIF_OPMODEACK_ACK_W                                            2
#define AUX_SYSIF_OPMODEACK_ACK_M                                   0x00000003
#define AUX_SYSIF_OPMODEACK_ACK_S                                            0
#define AUX_SYSIF_OPMODEACK_ACK_PDLP                                0x00000003
#define AUX_SYSIF_OPMODEACK_ACK_PDA                                 0x00000002
#define AUX_SYSIF_OPMODEACK_ACK_LP                                  0x00000001
#define AUX_SYSIF_OPMODEACK_ACK_A                                   0x00000000

//*****************************************************************************
//
// Register: AUX_SYSIF_O_EVSYNCRATE
//
//*****************************************************************************
// Field:     [2] AUX_COMPA_SYNC_RATE
//
// Select synchronization rate for AUX_EVCTL:EVSTAT2.AUX_COMPA event.
// ENUMs:
// BUS_RATE                 AUX bus rate
// SCE_RATE                 SCE rate
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE                    0x00000004
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE_BITN                        2
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE_M                  0x00000004
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE_S                           2
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE_BUS_RATE           0x00000004
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPA_SYNC_RATE_SCE_RATE           0x00000000

// Field:     [1] AUX_COMPB_SYNC_RATE
//
// Select synchronization rate for AUX_EVCTL:EVSTAT2.AUX_COMPB event.
// ENUMs:
// BUS_RATE                 AUX bus rate
// SCE_RATE                 SCE rate
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE                    0x00000002
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE_BITN                        1
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE_M                  0x00000002
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE_S                           1
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE_BUS_RATE           0x00000002
#define AUX_SYSIF_EVSYNCRATE_AUX_COMPB_SYNC_RATE_SCE_RATE           0x00000000

//*****************************************************************************
//
// Register: AUX_SYSIF_O_PEROPRATE
//
//*****************************************************************************
// Field:     [3] ANAIF_DAC_OP_RATE
//
// Select operational rate for AUX_ANAIF DAC sample clock state machine.
// ENUMs:
// BUS_RATE                 AUX bus rate
// SCE_RATE                 SCE rate
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE                       0x00000008
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_BITN                           3
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_M                     0x00000008
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_S                              3
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_BUS_RATE              0x00000008
#define AUX_SYSIF_PEROPRATE_ANAIF_DAC_OP_RATE_SCE_RATE              0x00000000

// Field:     [2] TIMER01_OP_RATE
//
// Select operational rate for AUX_TIMER01.
// ENUMs:
// BUS_RATE                 AUX bus rate
// SCE_RATE                 SCE rate
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE                         0x00000004
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE_BITN                             2
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE_M                       0x00000004
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE_S                                2
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE_BUS_RATE                0x00000004
#define AUX_SYSIF_PEROPRATE_TIMER01_OP_RATE_SCE_RATE                0x00000000

//*****************************************************************************
//
// Register: AUX_SYSIF_O_ADCCLKCTL
//
//*****************************************************************************
// Field:     [1] ACK
//
// Clock acknowledgement.
//
// 0: ADC clock is disabled.
// 1: ADC clock is enabled.
#define AUX_SYSIF_ADCCLKCTL_ACK                                     0x00000002
#define AUX_SYSIF_ADCCLKCTL_ACK_BITN                                         1
#define AUX_SYSIF_ADCCLKCTL_ACK_M                                   0x00000002
#define AUX_SYSIF_ADCCLKCTL_ACK_S                                            1

// Field:     [0] REQ
//
// ADC clock request.
//
// 0: Disable ADC clock.
// 1: Enable ADC clock.
//
// Only modify REQ when equal to ACK.
#define AUX_SYSIF_ADCCLKCTL_REQ                                     0x00000001
#define AUX_SYSIF_ADCCLKCTL_REQ_BITN                                         0
#define AUX_SYSIF_ADCCLKCTL_REQ_M                                   0x00000001
#define AUX_SYSIF_ADCCLKCTL_REQ_S                                            0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_TDCCLKCTL
//
//*****************************************************************************
// Field:     [1] ACK
//
// TDC counter clock acknowledgement.
//
// 0: TDC counter clock is disabled.
// 1: TDC counter clock is enabled.
#define AUX_SYSIF_TDCCLKCTL_ACK                                     0x00000002
#define AUX_SYSIF_TDCCLKCTL_ACK_BITN                                         1
#define AUX_SYSIF_TDCCLKCTL_ACK_M                                   0x00000002
#define AUX_SYSIF_TDCCLKCTL_ACK_S                                            1

// Field:     [0] REQ
//
// TDC counter clock request.
//
// 0: Disable TDC counter clock.
// 1: Enable TDC counter clock.
//
// Only modify REQ when equal to ACK.
#define AUX_SYSIF_TDCCLKCTL_REQ                                     0x00000001
#define AUX_SYSIF_TDCCLKCTL_REQ_BITN                                         0
#define AUX_SYSIF_TDCCLKCTL_REQ_M                                   0x00000001
#define AUX_SYSIF_TDCCLKCTL_REQ_S                                            0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_TDCREFCLKCTL
//
//*****************************************************************************
// Field:     [1] ACK
//
// TDC reference clock acknowledgement.
//
// 0: TDC reference clock is disabled.
// 1: TDC reference clock is enabled.
#define AUX_SYSIF_TDCREFCLKCTL_ACK                                  0x00000002
#define AUX_SYSIF_TDCREFCLKCTL_ACK_BITN                                      1
#define AUX_SYSIF_TDCREFCLKCTL_ACK_M                                0x00000002
#define AUX_SYSIF_TDCREFCLKCTL_ACK_S                                         1

// Field:     [0] REQ
//
// TDC reference clock request.
//
// 0: Disable TDC reference clock.
// 1: Enable TDC reference clock.
//
// Only modify REQ when equal to ACK.
#define AUX_SYSIF_TDCREFCLKCTL_REQ                                  0x00000001
#define AUX_SYSIF_TDCREFCLKCTL_REQ_BITN                                      0
#define AUX_SYSIF_TDCREFCLKCTL_REQ_M                                0x00000001
#define AUX_SYSIF_TDCREFCLKCTL_REQ_S                                         0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_RTCSUBSECINC0
//
//*****************************************************************************
// Field:  [15:0] INC15_0
//
// New value for bits 15:0 in AON_RTC:SUBSECINC.
#define AUX_SYSIF_RTCSUBSECINC0_INC15_0_W                                   16
#define AUX_SYSIF_RTCSUBSECINC0_INC15_0_M                           0x0000FFFF
#define AUX_SYSIF_RTCSUBSECINC0_INC15_0_S                                    0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_RTCSUBSECINC1
//
//*****************************************************************************
// Field:   [7:0] INC23_16
//
// New value for bits 23:16 in AON_RTC:SUBSECINC.
#define AUX_SYSIF_RTCSUBSECINC1_INC23_16_W                                   8
#define AUX_SYSIF_RTCSUBSECINC1_INC23_16_M                          0x000000FF
#define AUX_SYSIF_RTCSUBSECINC1_INC23_16_S                                   0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_RTCSUBSECINCCTL
//
//*****************************************************************************
// Field:     [1] UPD_ACK
//
// Update acknowledgement.
//
// 0: AON_RTC has not acknowledged UPD_REQ.
// 1: AON_RTC has acknowledged UPD_REQ.
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK                           0x00000002
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK_BITN                               1
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK_M                         0x00000002
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK_S                                  1

// Field:     [0] UPD_REQ
//
// Request AON_RTC to update AON_RTC:SUBSECINC.
//
// 0: Clear request to update.
// 1: Set request to update.
//
// Only change UPD_REQ when it equals UPD_ACK. Clear UPD_REQ after UPD_ACK is
// 1.
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ                           0x00000001
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ_BITN                               0
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ_M                         0x00000001
#define AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ_S                                  0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_RTCEVCLR
//
//*****************************************************************************
// Field:     [0] RTC_CH2_EV_CLR
//
// Clear events from AON_RTC channel 2.
//
// 0: No effect.
// 1: Clear events from AON_RTC channel 2.
//
// Keep RTC_CH2_EV_CLR high until AUX_EVCTL:EVSTAT2.AON_RTC_CH2 and
// AUX_EVCTL:EVSTAT2.AON_RTC_CH2_DLY are 0.
#define AUX_SYSIF_RTCEVCLR_RTC_CH2_EV_CLR                           0x00000001
#define AUX_SYSIF_RTCEVCLR_RTC_CH2_EV_CLR_BITN                               0
#define AUX_SYSIF_RTCEVCLR_RTC_CH2_EV_CLR_M                         0x00000001
#define AUX_SYSIF_RTCEVCLR_RTC_CH2_EV_CLR_S                                  0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_TIMERHALT
//
//*****************************************************************************
// Field:     [1] AUX_TIMER1
//
// Halt AUX_TIMER01 Timer 1.
//
// 0: AUX_TIMER01 Timer 1 operates as normal.
// 1: Halt AUX_TIMER01 Timer 1 operation.
#define AUX_SYSIF_TIMERHALT_AUX_TIMER1                              0x00000002
#define AUX_SYSIF_TIMERHALT_AUX_TIMER1_BITN                                  1
#define AUX_SYSIF_TIMERHALT_AUX_TIMER1_M                            0x00000002
#define AUX_SYSIF_TIMERHALT_AUX_TIMER1_S                                     1

// Field:     [0] AUX_TIMER0
//
// Halt AUX_TIMER01 Timer 0.
//
// 0: AUX_TIMER01 Timer 0 operates as normal.
// 1: Halt AUX_TIMER01 Timer 0 operation.
#define AUX_SYSIF_TIMERHALT_AUX_TIMER0                              0x00000001
#define AUX_SYSIF_TIMERHALT_AUX_TIMER0_BITN                                  0
#define AUX_SYSIF_TIMERHALT_AUX_TIMER0_M                            0x00000001
#define AUX_SYSIF_TIMERHALT_AUX_TIMER0_S                                     0

//*****************************************************************************
//
// Register: AUX_SYSIF_O_SWPWRPROF
//
//*****************************************************************************
// Field:   [2:0] STAT
//
// Software status bits that can be read by the power profiler.
#define AUX_SYSIF_SWPWRPROF_STAT_W                                           3
#define AUX_SYSIF_SWPWRPROF_STAT_M                                  0x00000007
#define AUX_SYSIF_SWPWRPROF_STAT_S                                           0


#endif // __AUX_SYSIF__
