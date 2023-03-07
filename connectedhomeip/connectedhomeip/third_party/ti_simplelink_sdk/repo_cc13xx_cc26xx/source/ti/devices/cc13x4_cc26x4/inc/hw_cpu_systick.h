/******************************************************************************
*  Filename:       hw_cpu_systick_h
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

#ifndef __HW_CPU_SYSTICK_H__
#define __HW_CPU_SYSTICK_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_SYSTICK component
//
//*****************************************************************************
// Controls the SysTick timer and provides status data `FTSSS
#define CPU_SYSTICK_O_CSR                                           0x00000000

// Provides access SysTick timer counter reload value `FTSSS
#define CPU_SYSTICK_O_RVR                                           0x00000004

// Reads or clears the SysTick timer current counter value `FTSSS
#define CPU_SYSTICK_O_CVR                                           0x00000008

// Reads the SysTick timer calibration value and parameters `FTSSS
#define CPU_SYSTICK_O_CALIB                                         0x0000000C

//*****************************************************************************
//
// Register: CPU_SYSTICK_O_CSR
//
//*****************************************************************************
// Field:    [16] COUNTFLAG
//
// Indicates whether the counter has counted to zero since the last read of
// this register
#define CPU_SYSTICK_CSR_COUNTFLAG                                   0x00010000
#define CPU_SYSTICK_CSR_COUNTFLAG_BITN                                      16
#define CPU_SYSTICK_CSR_COUNTFLAG_M                                 0x00010000
#define CPU_SYSTICK_CSR_COUNTFLAG_S                                         16

// Field:     [2] CLKSOURCE
//
// Indicates the SysTick clock source
#define CPU_SYSTICK_CSR_CLKSOURCE                                   0x00000004
#define CPU_SYSTICK_CSR_CLKSOURCE_BITN                                       2
#define CPU_SYSTICK_CSR_CLKSOURCE_M                                 0x00000004
#define CPU_SYSTICK_CSR_CLKSOURCE_S                                          2

// Field:     [1] TICKINT
//
// Indicates whether counting to 0 causes the status of the SysTick exception
// to change to pending
#define CPU_SYSTICK_CSR_TICKINT                                     0x00000002
#define CPU_SYSTICK_CSR_TICKINT_BITN                                         1
#define CPU_SYSTICK_CSR_TICKINT_M                                   0x00000002
#define CPU_SYSTICK_CSR_TICKINT_S                                            1

// Field:     [0] ENABLE
//
// Indicates the enabled status of the SysTick counter
#define CPU_SYSTICK_CSR_ENABLE                                      0x00000001
#define CPU_SYSTICK_CSR_ENABLE_BITN                                          0
#define CPU_SYSTICK_CSR_ENABLE_M                                    0x00000001
#define CPU_SYSTICK_CSR_ENABLE_S                                             0

//*****************************************************************************
//
// Register: CPU_SYSTICK_O_RVR
//
//*****************************************************************************
// Field:  [23:0] RELOAD
//
// The value to load into the SYST_CVR `FTSSS when the counter reaches 0
#define CPU_SYSTICK_RVR_RELOAD_W                                            24
#define CPU_SYSTICK_RVR_RELOAD_M                                    0x00FFFFFF
#define CPU_SYSTICK_RVR_RELOAD_S                                             0

//*****************************************************************************
//
// Register: CPU_SYSTICK_O_CVR
//
//*****************************************************************************
// Field:  [23:0] CURRENT
//
// Writing any value clears the SysTick timer counter `FTSSS to zero
#define CPU_SYSTICK_CVR_CURRENT_W                                           24
#define CPU_SYSTICK_CVR_CURRENT_M                                   0x00FFFFFF
#define CPU_SYSTICK_CVR_CURRENT_S                                            0

//*****************************************************************************
//
// Register: CPU_SYSTICK_O_CALIB
//
//*****************************************************************************
// Field:    [31] NOREF
//
// Indicates whether the IMPLEMENTATION DEFINED reference clock is implemented
#define CPU_SYSTICK_CALIB_NOREF                                     0x80000000
#define CPU_SYSTICK_CALIB_NOREF_BITN                                        31
#define CPU_SYSTICK_CALIB_NOREF_M                                   0x80000000
#define CPU_SYSTICK_CALIB_NOREF_S                                           31

// Field:    [30] SKEW
//
// Indicates whether the 10ms calibration value is exact
#define CPU_SYSTICK_CALIB_SKEW                                      0x40000000
#define CPU_SYSTICK_CALIB_SKEW_BITN                                         30
#define CPU_SYSTICK_CALIB_SKEW_M                                    0x40000000
#define CPU_SYSTICK_CALIB_SKEW_S                                            30

// Field:  [23:0] TENMS
//
// Optionally, holds a reload value to be used for 10ms (100Hz) timing, subject
// to system clock skew errors. If this field is zero, the calibration value is
// not known
#define CPU_SYSTICK_CALIB_TENMS_W                                           24
#define CPU_SYSTICK_CALIB_TENMS_M                                   0x00FFFFFF
#define CPU_SYSTICK_CALIB_TENMS_S                                            0


#endif // __CPU_SYSTICK__
