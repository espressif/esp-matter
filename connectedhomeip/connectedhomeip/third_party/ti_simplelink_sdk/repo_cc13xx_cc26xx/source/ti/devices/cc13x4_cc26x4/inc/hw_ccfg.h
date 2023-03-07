/******************************************************************************
*  Filename:       hw_ccfg_h
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

#ifndef __HW_CCFG_H__
#define __HW_CCFG_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CCFG component
//
//*****************************************************************************
// CCFG Size and Disable Flags
#define CCFG_O_SIZE_AND_DIS_FLAGS                                   0x00000000

// Mode Configuration 0
#define CCFG_O_MODE_CONF                                            0x00000004

// Mode Configuration 1
#define CCFG_O_MODE_CONF_1                                          0x00000008

// Voltage Load 0
#define CCFG_O_VOLT_LOAD_0                                          0x0000000C

// Voltage Load 1
#define CCFG_O_VOLT_LOAD_1                                          0x00000010

// Extern LF clock configuration
#define CCFG_O_EXT_LF_CLK                                           0x00000014

// IEEE MAC Address 0
#define CCFG_O_IEEE_MAC_0                                           0x00000018

// IEEE MAC Address 1
#define CCFG_O_IEEE_MAC_1                                           0x0000001C

// IEEE BLE Address 0
#define CCFG_O_IEEE_BLE_0                                           0x00000020

// IEEE BLE Address 1
#define CCFG_O_IEEE_BLE_1                                           0x00000024

// Bootloader Configuration
#define CCFG_O_BL_CONFIG                                            0x00000028

// Erase Configuration
#define CCFG_O_ERASE_CONF                                           0x0000002C

// Erase Configuration 1
#define CCFG_O_ERASE_CONF_1                                         0x00000030

// TI Options
#define CCFG_O_CCFG_TI_OPTIONS                                      0x00000034

// Test Access Points Enable 0
#define CCFG_O_CCFG_TAP_DAP_0                                       0x00000038

// Test Access Points Enable 1
#define CCFG_O_CCFG_TAP_DAP_1                                       0x0000003C

// Image Valid
#define CCFG_O_IMAGE_VALID_CONF                                     0x00000040

// Protect Sectors 0-31
#define CCFG_O_CCFG_WEPROT_31_0_BY2K                                0x00000044

// Spare register for WriteErase configuration
#define CCFG_O_CCFG_WEPROT_SPARE_1                                  0x00000048

// Spare register for WriteErase configuration
#define CCFG_O_CCFG_WEPROT_SPARE_2                                  0x0000004C

// Spare register for WriteErase configuration
#define CCFG_O_CCFG_WEPROT_SPARE_3                                  0x00000050

// Trustzone configuration register for flash
#define CCFG_O_TRUSTZONE_FLASH_CFG                                  0x00000054

// Trustzone configuration register for MCU SRAM
#define CCFG_O_TRUSTZONE_SRAM_CFG                                   0x00000058

// Configuration register for MCU SRAM
#define CCFG_O_SRAM_CFG                                             0x0000005C

// Configuration register for MCU CPU lock options
#define CCFG_O_CPU_LOCK_CFG                                         0x00000064

// Configuration register for debug authentication
#define CCFG_O_DEB_AUTH_CFG                                         0x00000068

//*****************************************************************************
//
// Register: CCFG_O_SIZE_AND_DIS_FLAGS
//
//*****************************************************************************
// Field: [31:16] SIZE_OF_CCFG
//
// Total size of CCFG in bytes.
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_W                              16
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_M                      0xFFFF0000
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_S                              16

// Field:  [15:4] DISABLE_FLAGS
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_W                             12
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_M                     0x0000FFF0
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_S                              4

// Field:     [3] DIS_TCXO
//
// Deprecated. Must be set to 1.
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO                            0x00000008
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_BITN                                3
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_M                          0x00000008
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_S                                   3

// Field:     [2] DIS_GPRAM
//
// Disable GPRAM (or use the 8K VIMS RAM as CACHE RAM).
// 0: GPRAM is enabled and hence CACHE disabled.
// 1: GPRAM is disabled and instead CACHE is enabled (default).
// Notes:
// - Disabling CACHE will reduce CPU execution speed (up to 60%).
// - GPRAM is 8 K-bytes in size and located at 0x11000000-0x11001FFF if
// enabled.
// See:
// VIMS:CTL.MODE
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM                           0x00000004
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_BITN                               2
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_M                         0x00000004
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_S                                  2

// Field:     [1] DIS_ALT_DCDC_SETTING
//
// Disable alternate DC/DC settings.
// 0: Enable alternate DC/DC settings.
// 1: Disable alternate DC/DC settings.
// See:
// MODE_CONF_1.ALT_DCDC_VMIN
// MODE_CONF_1.ALT_DCDC_DITHER_EN
// MODE_CONF_1.ALT_DCDC_IPEAK
//
// NOTE! The DriverLib function SysCtrl_DCDC_VoltageConditionalControl() must
// be called regularly to apply this field (handled automatically if using TI
// RTOS!).
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING                0x00000002
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_BITN                    1
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_M              0x00000002
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_S                       1

// Field:     [0] DIS_XOSC_OVR
//
// Disable XOSC override functionality.
// 0: Enable XOSC override functionality.
// 1: Disable XOSC override functionality.
// See:
// MODE_CONF_1.DELTA_IBIAS_INIT
// MODE_CONF_1.DELTA_IBIAS_OFFSET
// MODE_CONF_1.XOSC_MAX_START
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR                        0x00000001
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_BITN                            0
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_M                      0x00000001
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_S                               0

//*****************************************************************************
//
// Register: CCFG_O_MODE_CONF
//
//*****************************************************************************
// Field: [31:28] VDDR_TRIM_SLEEP_DELTA
//
// Signed delta value to apply to the
// VDDR_TRIM_SLEEP target, minus one. See FCFG1:VOLT_TRIM.VDDR_TRIM_SLEEP_H.
// 0x8 (-8) : Delta = -7
// ...
// 0xF (-1) : Delta = 0
// 0x0 (0) : Delta = +1
// ...
// 0x7 (7) : Delta = +8
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_W                               4
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_M                      0xF0000000
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_S                              28

// Field:    [27] DCDC_RECHARGE
//
// DC/DC during recharge in powerdown.
// 0: Use the DC/DC during recharge in powerdown.
// 1: Do not use the DC/DC during recharge in powerdown (default).
//
// NOTE! The DriverLib function SysCtrl_DCDC_VoltageConditionalControl() must
// be called regularly to apply this field (handled automatically if using TI
// RTOS!).
#define CCFG_MODE_CONF_DCDC_RECHARGE                                0x08000000
#define CCFG_MODE_CONF_DCDC_RECHARGE_BITN                                   27
#define CCFG_MODE_CONF_DCDC_RECHARGE_M                              0x08000000
#define CCFG_MODE_CONF_DCDC_RECHARGE_S                                      27

// Field:    [26] DCDC_ACTIVE
//
// DC/DC in active mode.
// 0: Use the DC/DC during active mode.
// 1: Do not use the DC/DC during active mode (default).
//
// NOTE! The DriverLib function SysCtrl_DCDC_VoltageConditionalControl() must
// be called regularly to apply this field (handled automatically if using TI
// RTOS!).
#define CCFG_MODE_CONF_DCDC_ACTIVE                                  0x04000000
#define CCFG_MODE_CONF_DCDC_ACTIVE_BITN                                     26
#define CCFG_MODE_CONF_DCDC_ACTIVE_M                                0x04000000
#define CCFG_MODE_CONF_DCDC_ACTIVE_S                                        26

// Field:    [25] VDDR_EXT_LOAD
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_MODE_CONF_VDDR_EXT_LOAD                                0x02000000
#define CCFG_MODE_CONF_VDDR_EXT_LOAD_BITN                                   25
#define CCFG_MODE_CONF_VDDR_EXT_LOAD_M                              0x02000000
#define CCFG_MODE_CONF_VDDR_EXT_LOAD_S                                      25

// Field:    [24] VDDS_BOD_LEVEL
//
// VDDS BOD level.
// 0: VDDS BOD level is 2.0V (necessary for external load mode, or for maximum
// PA output power on CC13xx).
// 1: VDDS BOD level is 1.8V (or 1.65V for external regulator mode) (default).
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL                               0x01000000
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL_BITN                                  24
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL_M                             0x01000000
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL_S                                     24

// Field: [23:22] SCLK_LF_OPTION
//
// Select source for SCLK_LF.
// ENUMs:
// RCOSC_LF                 Low frequency RCOSC (default)
// XOSC_LF                  32.768kHz low frequency XOSC
// EXTERNAL_LF              External low frequency clock on DIO defined by
//                          EXT_LF_CLK.DIO. The RTC tick speed
//                          AON_RTC:SUBSECINC is updated to
//                          EXT_LF_CLK.RTC_INCREMENT (done in the
//                          SetupTrimDevice() driverlib boot function).
//                          External clock must always be running when the
//                          chip is in standby for VDDR recharge timing.
// XOSC_HF_DLF              31.25kHz clock derived from 48MHz XOSC or HPOSC.
//                          The RTC tick speed AON_RTC:SUBSECINC is updated
//                          to 0x8637BD, corresponding to a 31.25kHz clock
//                          (done in the SetupTrimDevice() driverlib boot
//                          function). The device must be blocked from
//                          entering Standby mode when using this clock
//                          source.
#define CCFG_MODE_CONF_SCLK_LF_OPTION_W                                      2
#define CCFG_MODE_CONF_SCLK_LF_OPTION_M                             0x00C00000
#define CCFG_MODE_CONF_SCLK_LF_OPTION_S                                     22
#define CCFG_MODE_CONF_SCLK_LF_OPTION_RCOSC_LF                      0x00C00000
#define CCFG_MODE_CONF_SCLK_LF_OPTION_XOSC_LF                       0x00800000
#define CCFG_MODE_CONF_SCLK_LF_OPTION_EXTERNAL_LF                   0x00400000
#define CCFG_MODE_CONF_SCLK_LF_OPTION_XOSC_HF_DLF                   0x00000000

// Field:    [21] VDDR_TRIM_SLEEP_TC
//
// 0x1: VDDR_TRIM_SLEEP_DELTA is not temperature compensated
// 0x0: RTOS/driver temperature compensates VDDR_TRIM_SLEEP_DELTA every time
// standby mode is entered. This improves low-temperature RCOSC_LF frequency
// stability in standby mode.
//
// When temperature compensation is performed, the delta is calculates this
// way:
// Delta = max (delta, min(8, floor(62-temp)/8))
// Here, delta is given by VDDR_TRIM_SLEEP_DELTA, and temp is the current
// temperature in degrees C.
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC                           0x00200000
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_BITN                              21
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_M                         0x00200000
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_S                                 21

// Field:    [20] RTC_COMP
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_MODE_CONF_RTC_COMP                                     0x00100000
#define CCFG_MODE_CONF_RTC_COMP_BITN                                        20
#define CCFG_MODE_CONF_RTC_COMP_M                                   0x00100000
#define CCFG_MODE_CONF_RTC_COMP_S                                           20

// Field: [19:18] XOSC_FREQ
//
// Selects which high frequency oscillator is used (required for radio usage).
// ENUMs:
// 24M                      24 MHz XOSC_HF. Not supported.
// 48M                      48 MHz XOSC_HF
// HPOSC                    Internal high precision oscillator.
// TCXO                     External 48Mhz TCXO.
//                          Refer to
//                          MODE_CONF_1.TCXO_MAX_START and
//                          MODE_CONF_1.TCXO_TYPE bit fields for additional
//                          configuration of TCXO.
#define CCFG_MODE_CONF_XOSC_FREQ_W                                           2
#define CCFG_MODE_CONF_XOSC_FREQ_M                                  0x000C0000
#define CCFG_MODE_CONF_XOSC_FREQ_S                                          18
#define CCFG_MODE_CONF_XOSC_FREQ_24M                                0x000C0000
#define CCFG_MODE_CONF_XOSC_FREQ_48M                                0x00080000
#define CCFG_MODE_CONF_XOSC_FREQ_HPOSC                              0x00040000
#define CCFG_MODE_CONF_XOSC_FREQ_TCXO                               0x00000000

// Field:    [17] XOSC_CAP_MOD
//
// Enable modification (delta) to XOSC cap-array. Value specified in
// XOSC_CAPARRAY_DELTA.
// 0: Apply cap-array delta
// 1: Do not apply cap-array delta (default)
#define CCFG_MODE_CONF_XOSC_CAP_MOD                                 0x00020000
#define CCFG_MODE_CONF_XOSC_CAP_MOD_BITN                                    17
#define CCFG_MODE_CONF_XOSC_CAP_MOD_M                               0x00020000
#define CCFG_MODE_CONF_XOSC_CAP_MOD_S                                       17

// Field:    [16] HF_COMP
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_MODE_CONF_HF_COMP                                      0x00010000
#define CCFG_MODE_CONF_HF_COMP_BITN                                         16
#define CCFG_MODE_CONF_HF_COMP_M                                    0x00010000
#define CCFG_MODE_CONF_HF_COMP_S                                            16

// Field:  [15:8] XOSC_CAPARRAY_DELTA
//
// Signed 8-bit value, directly modifying trimmed XOSC cap-array step value.
// Enabled by XOSC_CAP_MOD.
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_W                                 8
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_M                        0x0000FF00
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_S                                 8

// Field:   [7:0] VDDR_CAP
//
// Unsigned 8-bit integer, representing the minimum decoupling capacitance
// (worst case) on VDDR, in units of 100nF. This should take into account
// capacitor tolerance and voltage dependent capacitance variation. This bit
// affects the recharge period calculation when going into powerdown or
// standby.
//
// NOTE! If using the following functions this field must be configured (used
// by TI RTOS):
// SysCtrlSetRechargeBeforePowerDown() SysCtrlAdjustRechargeAfterPowerDown()
#define CCFG_MODE_CONF_VDDR_CAP_W                                            8
#define CCFG_MODE_CONF_VDDR_CAP_M                                   0x000000FF
#define CCFG_MODE_CONF_VDDR_CAP_S                                            0

//*****************************************************************************
//
// Register: CCFG_O_MODE_CONF_1
//
//*****************************************************************************
// Field:    [31] TCXO_TYPE
//
// Selects the TCXO type.
//
// 0: CMOS type. Internal common-mode bias will not be enabled.
// 1: Clipped-sine type. Internal common-mode bias will be enabled when TCXO is
// used.
//
// Bit field value is only valid if MODE_CONF.XOSC_FREQ=0.
#define CCFG_MODE_CONF_1_TCXO_TYPE                                  0x80000000
#define CCFG_MODE_CONF_1_TCXO_TYPE_BITN                                     31
#define CCFG_MODE_CONF_1_TCXO_TYPE_M                                0x80000000
#define CCFG_MODE_CONF_1_TCXO_TYPE_S                                        31

// Field: [30:24] TCXO_MAX_START
//
// Maximum TCXO startup time in units of 100us.
// Bit field value is only valid if MODE_CONF.XOSC_FREQ=0.
#define CCFG_MODE_CONF_1_TCXO_MAX_START_W                                    7
#define CCFG_MODE_CONF_1_TCXO_MAX_START_M                           0x7F000000
#define CCFG_MODE_CONF_1_TCXO_MAX_START_S                                   24

// Field: [23:20] ALT_DCDC_VMIN
//
// Minimum voltage for when DC/DC should be used if alternate DC/DC setting is
// enabled (SIZE_AND_DIS_FLAGS.DIS_ALT_DCDC_SETTING=0).
// Voltage = (28 + ALT_DCDC_VMIN) / 16.
// 0: 1.75V
// 1: 1.8125V
// ...
// 14: 2.625V
// 15: 2.6875V
//
// NOTE! The DriverLib function SysCtrl_DCDC_VoltageConditionalControl() must
// be called regularly to apply this field (handled automatically if using TI
// RTOS!).
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_W                                     4
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_M                            0x00F00000
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_S                                    20

// Field:    [19] ALT_DCDC_DITHER_EN
//
// Enable DC/DC dithering if alternate DC/DC setting is enabled
// (SIZE_AND_DIS_FLAGS.DIS_ALT_DCDC_SETTING=0).
// 0: Dither disable
// 1: Dither enable
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN                         0x00080000
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_BITN                            19
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_M                       0x00080000
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_S                               19

// Field: [18:16] ALT_DCDC_IPEAK
//
// Inductor peak current if alternate DC/DC setting is enabled
// (SIZE_AND_DIS_FLAGS.DIS_ALT_DCDC_SETTING=0). Assuming 10uH external
// inductor!
//
// 0: 46mA (min)
// ...
// 4: 70mA
// ...
// 7: 87mA (max)
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_W                                    3
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_M                           0x00070000
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_S                                   16

// Field: [15:12] DELTA_IBIAS_INIT
//
// Signed delta value for IBIAS_INIT. Delta value only applies if
// SIZE_AND_DIS_FLAGS.DIS_XOSC_OVR=0.
// See FCFG1:AMPCOMP_CTRL1.IBIAS_INIT
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_W                                  4
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_M                         0x0000F000
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_S                                 12

// Field:  [11:8] DELTA_IBIAS_OFFSET
//
// Signed delta value for IBIAS_OFFSET. Delta value only applies if
// SIZE_AND_DIS_FLAGS.DIS_XOSC_OVR=0.
// See FCFG1:AMPCOMP_CTRL1.IBIAS_OFFSET
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_W                                4
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_M                       0x00000F00
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_S                                8

// Field:   [7:0] XOSC_MAX_START
//
// Unsigned value of maximum XOSC startup time (worst case) in units of 100us.
// Value only applies if SIZE_AND_DIS_FLAGS.DIS_XOSC_OVR=0.
#define CCFG_MODE_CONF_1_XOSC_MAX_START_W                                    8
#define CCFG_MODE_CONF_1_XOSC_MAX_START_M                           0x000000FF
#define CCFG_MODE_CONF_1_XOSC_MAX_START_S                                    0

//*****************************************************************************
//
// Register: CCFG_O_VOLT_LOAD_0
//
//*****************************************************************************
// Field: [31:24] VDDR_EXT_TP45
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP45_W                                     8
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP45_M                            0xFF000000
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP45_S                                    24

// Field: [23:16] VDDR_EXT_TP25
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP25_W                                     8
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP25_M                            0x00FF0000
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP25_S                                    16

// Field:  [15:8] VDDR_EXT_TP5
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP5_W                                      8
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP5_M                             0x0000FF00
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TP5_S                                      8

// Field:   [7:0] VDDR_EXT_TM15
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TM15_W                                     8
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TM15_M                            0x000000FF
#define CCFG_VOLT_LOAD_0_VDDR_EXT_TM15_S                                     0

//*****************************************************************************
//
// Register: CCFG_O_VOLT_LOAD_1
//
//*****************************************************************************
// Field: [31:24] VDDR_EXT_TP125
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP125_W                                    8
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP125_M                           0xFF000000
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP125_S                                   24

// Field: [23:16] VDDR_EXT_TP105
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP105_W                                    8
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP105_M                           0x00FF0000
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP105_S                                   16

// Field:  [15:8] VDDR_EXT_TP85
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP85_W                                     8
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP85_M                            0x0000FF00
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP85_S                                     8

// Field:   [7:0] VDDR_EXT_TP65
//
// Reserved for future use. Software should not rely on the value of a
// reserved. Writing any other value than the reset/default value may result in
// undefined behavior.
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP65_W                                     8
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP65_M                            0x000000FF
#define CCFG_VOLT_LOAD_1_VDDR_EXT_TP65_S                                     0

//*****************************************************************************
//
// Register: CCFG_O_EXT_LF_CLK
//
//*****************************************************************************
// Field: [31:24] DIO
//
// Unsigned integer, selecting the DIO to supply external 32kHz clock as
// SCLK_LF when MODE_CONF.SCLK_LF_OPTION is set to EXTERNAL. The selected DIO
// will be marked as reserved by the pin driver (TI-RTOS environment) and hence
// not selectable for other usage.
#define CCFG_EXT_LF_CLK_DIO_W                                                8
#define CCFG_EXT_LF_CLK_DIO_M                                       0xFF000000
#define CCFG_EXT_LF_CLK_DIO_S                                               24

// Field:  [23:0] RTC_INCREMENT
//
// Unsigned integer, defining the input frequency of the external clock and is
// written to AON_RTC:SUBSECINC.VALUEINC. Defined as follows:
// EXT_LF_CLK.RTC_INCREMENT = 2^38/InputClockFrequency in Hertz (e.g.:
// RTC_INCREMENT=0x800000 for InputClockFrequency=32768 Hz)
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_W                                     24
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_M                             0x00FFFFFF
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_S                                      0

//*****************************************************************************
//
// Register: CCFG_O_IEEE_MAC_0
//
//*****************************************************************************
// Field:  [31:0] ADDR
//
// Bits[31:0] of the 64-bits custom IEEE MAC address.
// If different from 0xFFFFFFFF then the value of this field is applied;
// otherwise use value from FCFG.
#define CCFG_IEEE_MAC_0_ADDR_W                                              32
#define CCFG_IEEE_MAC_0_ADDR_M                                      0xFFFFFFFF
#define CCFG_IEEE_MAC_0_ADDR_S                                               0

//*****************************************************************************
//
// Register: CCFG_O_IEEE_MAC_1
//
//*****************************************************************************
// Field:  [31:0] ADDR
//
// Bits[63:32] of the 64-bits custom IEEE MAC address.
// If different from 0xFFFFFFFF then the value of this field is applied;
// otherwise use value from FCFG.
#define CCFG_IEEE_MAC_1_ADDR_W                                              32
#define CCFG_IEEE_MAC_1_ADDR_M                                      0xFFFFFFFF
#define CCFG_IEEE_MAC_1_ADDR_S                                               0

//*****************************************************************************
//
// Register: CCFG_O_IEEE_BLE_0
//
//*****************************************************************************
// Field:  [31:0] ADDR
//
// Bits[31:0] of the 64-bits custom IEEE BLE address.
// If different from 0xFFFFFFFF then the value of this field is applied;
// otherwise use value from FCFG.
#define CCFG_IEEE_BLE_0_ADDR_W                                              32
#define CCFG_IEEE_BLE_0_ADDR_M                                      0xFFFFFFFF
#define CCFG_IEEE_BLE_0_ADDR_S                                               0

//*****************************************************************************
//
// Register: CCFG_O_IEEE_BLE_1
//
//*****************************************************************************
// Field:  [31:0] ADDR
//
// Bits[63:32] of the 64-bits custom IEEE BLE address.
// If different from 0xFFFFFFFF then the value of this field is applied;
// otherwise use value from FCFG.
#define CCFG_IEEE_BLE_1_ADDR_W                                              32
#define CCFG_IEEE_BLE_1_ADDR_M                                      0xFFFFFFFF
#define CCFG_IEEE_BLE_1_ADDR_S                                               0

//*****************************************************************************
//
// Register: CCFG_O_BL_CONFIG
//
//*****************************************************************************
// Field: [31:24] BOOTLOADER_ENABLE
//
// Bootloader enable. Boot loader can be accessed if
// IMAGE_VALID_CONF.IMAGE_VALID is non-zero or BL_ENABLE is enabled (and
// conditions for boot loader backdoor are met).
// 0xC5: Boot loader is enabled.
// Any other value: Boot loader is disabled.
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_W                                   8
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_M                          0xFF000000
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_S                                  24

// Field:    [16] BL_LEVEL
//
// Sets the active level of the selected DIO number BL_PIN_NUMBER if boot
// loader backdoor is enabled by the BL_ENABLE field.
// 0: Active low.
// 1: Active high.
#define CCFG_BL_CONFIG_BL_LEVEL                                     0x00010000
#define CCFG_BL_CONFIG_BL_LEVEL_BITN                                        16
#define CCFG_BL_CONFIG_BL_LEVEL_M                                   0x00010000
#define CCFG_BL_CONFIG_BL_LEVEL_S                                           16

// Field:  [15:8] BL_PIN_NUMBER
//
// DIO number that is level checked if the boot loader backdoor is enabled by
// the BL_ENABLE field.
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_W                                       8
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_M                              0x0000FF00
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_S                                       8

// Field:   [7:0] BL_ENABLE
//
// Enables the boot loader backdoor.
// 0xC5: Boot loader backdoor is enabled.
// Any other value: Boot loader backdoor is disabled.
//
// NOTE! Boot loader must be enabled (see BOOTLOADER_ENABLE) if boot loader
// backdoor is enabled.
#define CCFG_BL_CONFIG_BL_ENABLE_W                                           8
#define CCFG_BL_CONFIG_BL_ENABLE_M                                  0x000000FF
#define CCFG_BL_CONFIG_BL_ENABLE_S                                           0

//*****************************************************************************
//
// Register: CCFG_O_ERASE_CONF
//
//*****************************************************************************
// Field:     [8] CHIP_ERASE_DIS_N
//
// Chip erase.
// This bit controls if a chip erase requested through the JTAG WUC TAP will be
// ignored in a following boot caused by a reset of the MCU VD.
// A successful chip erase operation will force the content of the flash main
// bank back to the state as it was when delivered by TI.
// 0: Disable. Any chip erase request detected during boot will be ignored.
// 1: Enable. Any chip erase request detected during boot will be performed by
// the boot FW.
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N                            0x00000100
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N_BITN                                8
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N_M                          0x00000100
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N_S                                   8

// Field:     [0] BANK_ERASE_DIS_N
//
// Bank erase.
// This bit controls if the ROM serial boot loader will accept a received Bank
// Erase command (COMMAND_BANK_ERASE).
// A successful Bank Erase operation will erase all main bank sectors not
// protected by write protect configuration bits in CCFG.
// 0: Disable the boot loader bank erase function.
// 1: Enable the boot loader bank erase function.
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N                            0x00000001
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N_BITN                                0
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N_M                          0x00000001
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N_S                                   0

//*****************************************************************************
//
// Register: CCFG_O_ERASE_CONF_1
//
//*****************************************************************************
// Field:     [0] WEPROT_CCFG_N
//
// WriteErase protect the CCFG sector
// Setting this bit = 0 will set FLASH:WEPROT_AUX_BY1.WEPROT_B0_CCFG_BY1 = 1
// during boot and hence WriteErase protect the CCFG
#define CCFG_ERASE_CONF_1_WEPROT_CCFG_N                             0x00000001
#define CCFG_ERASE_CONF_1_WEPROT_CCFG_N_BITN                                 0
#define CCFG_ERASE_CONF_1_WEPROT_CCFG_N_M                           0x00000001
#define CCFG_ERASE_CONF_1_WEPROT_CCFG_N_S                                    0

//*****************************************************************************
//
// Register: CCFG_O_CCFG_TI_OPTIONS
//
//*****************************************************************************
// Field:  [15:8] IDAU_CFG_ENABLE
//
// IDAU configuration.
// 0xC5: Disable IDAU configuration controlled by TRUSTZONE_FLASH_CFG and
// TRUSTZONE_SRAM_CFG.
// All other values: Enable IDAU configuration controlled by
// TRUSTZONE_FLASH_CFG and TRUSTZONE_SRAM_CFG.
#define CCFG_CCFG_TI_OPTIONS_IDAU_CFG_ENABLE_W                               8
#define CCFG_CCFG_TI_OPTIONS_IDAU_CFG_ENABLE_M                      0x0000FF00
#define CCFG_CCFG_TI_OPTIONS_IDAU_CFG_ENABLE_S                               8

// Field:   [7:0] TI_FA_ENABLE
//
// TI Failure Analysis.
// 0xC5: Enable the functionality of unlocking the TI FA (TI Failure Analysis)
// option with the unlock code.
// All other values: Disable the functionality of unlocking the TI FA option
// with the unlock code.
#define CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE_W                                  8
#define CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE_M                         0x000000FF
#define CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE_S                                  0

//*****************************************************************************
//
// Register: CCFG_O_CCFG_TAP_DAP_0
//
//*****************************************************************************
// Field: [23:16] CPU_DAP_ENABLE
//
// Enable CPU DAP.
// 0xC5: Main CPU DAP access is enabled during power-up/system-reset by ROM
// boot FW.
// Any other value: Main CPU DAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_W                                 8
#define CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_M                        0x00FF0000
#define CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_S                                16

// Field:  [15:8] PWRPROF_TAP_ENABLE
//
// Enable PWRPROF TAP.
// 0xC5: PWRPROF TAP access is enabled during power-up/system-reset by ROM boot
// FW if enabled by corresponding configuration value in FCFG1 defined by TI.
// Any other value: PWRPROF TAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_W                             8
#define CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_M                    0x0000FF00
#define CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_S                             8

// Field:   [7:0] TEST_TAP_ENABLE
//
// Enable Test TAP.
// 0xC5: TEST TAP access is enabled during power-up/system-reset by ROM boot FW
// if enabled by corresponding configuration value in FCFG1 defined by TI.
// Any other value: TEST TAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_W                                8
#define CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_M                       0x000000FF
#define CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_S                                0

//*****************************************************************************
//
// Register: CCFG_O_CCFG_TAP_DAP_1
//
//*****************************************************************************
// Field: [23:16] PBIST2_TAP_ENABLE
//
// Enable PBIST2 TAP.
// 0xC5: PBIST2 TAP access is enabled during power-up/system-reset by ROM boot
// FW if enabled by corresponding configuration value in FCFG1 defined by TI.
// Any other value: PBIST2 TAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_1_PBIST2_TAP_ENABLE_W                              8
#define CCFG_CCFG_TAP_DAP_1_PBIST2_TAP_ENABLE_M                     0x00FF0000
#define CCFG_CCFG_TAP_DAP_1_PBIST2_TAP_ENABLE_S                             16

// Field:  [15:8] PBIST1_TAP_ENABLE
//
// Enable PBIST1 TAP.
// 0xC5: PBIST1 TAP access is enabled during power-up/system-reset by ROM boot
// FW if enabled by corresponding configuration value in FCFG1 defined by TI.
// Any other value: PBIST1 TAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_1_PBIST1_TAP_ENABLE_W                              8
#define CCFG_CCFG_TAP_DAP_1_PBIST1_TAP_ENABLE_M                     0x0000FF00
#define CCFG_CCFG_TAP_DAP_1_PBIST1_TAP_ENABLE_S                              8

// Field:   [7:0] AON_TAP_ENABLE
//
// Enable AON TAP
// 0xC5: AON TAP access is enabled during power-up/system-reset by ROM boot FW
// if enabled by corresponding configuration value in FCFG1 defined by TI.
// Any other value: AON TAP access will remain disabled out of
// power-up/system-reset.
#define CCFG_CCFG_TAP_DAP_1_AON_TAP_ENABLE_W                                 8
#define CCFG_CCFG_TAP_DAP_1_AON_TAP_ENABLE_M                        0x000000FF
#define CCFG_CCFG_TAP_DAP_1_AON_TAP_ENABLE_S                                 0

//*****************************************************************************
//
// Register: CCFG_O_IMAGE_VALID_CONF
//
//*****************************************************************************
// Field:  [31:0] IMAGE_VALID
//
// This field must have the address value of the start of the flash vector
// table in order to enable the boot FW in ROM to transfer control to a flash
// image.
// Any illegal vector table start address value will force the boot FW in ROM
// to transfer control to the serial boot loader in ROM.
#define CCFG_IMAGE_VALID_CONF_IMAGE_VALID_W                                 32
#define CCFG_IMAGE_VALID_CONF_IMAGE_VALID_M                         0xFFFFFFFF
#define CCFG_IMAGE_VALID_CONF_IMAGE_VALID_S                                  0

//*****************************************************************************
//
// Register: CCFG_O_CCFG_WEPROT_31_0_BY2K
//
//*****************************************************************************
// Field:    [31] WEPROT_SEC_31_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_31_N                  0x80000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_31_N_BITN                     31
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_31_N_M                0x80000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_31_N_S                        31

// Field:    [30] WEPROT_SEC_30_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_30_N                  0x40000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_30_N_BITN                     30
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_30_N_M                0x40000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_30_N_S                        30

// Field:    [29] WEPROT_SEC_29_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_29_N                  0x20000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_29_N_BITN                     29
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_29_N_M                0x20000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_29_N_S                        29

// Field:    [28] WEPROT_SEC_28_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_28_N                  0x10000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_28_N_BITN                     28
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_28_N_M                0x10000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_28_N_S                        28

// Field:    [27] WEPROT_SEC_27_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_27_N                  0x08000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_27_N_BITN                     27
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_27_N_M                0x08000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_27_N_S                        27

// Field:    [26] WEPROT_SEC_26_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_26_N                  0x04000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_26_N_BITN                     26
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_26_N_M                0x04000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_26_N_S                        26

// Field:    [25] WEPROT_SEC_25_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_25_N                  0x02000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_25_N_BITN                     25
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_25_N_M                0x02000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_25_N_S                        25

// Field:    [24] WEPROT_SEC_24_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_24_N                  0x01000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_24_N_BITN                     24
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_24_N_M                0x01000000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_24_N_S                        24

// Field:    [23] WEPROT_SEC_23_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_23_N                  0x00800000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_23_N_BITN                     23
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_23_N_M                0x00800000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_23_N_S                        23

// Field:    [22] WEPROT_SEC_22_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_22_N                  0x00400000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_22_N_BITN                     22
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_22_N_M                0x00400000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_22_N_S                        22

// Field:    [21] WEPROT_SEC_21_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_21_N                  0x00200000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_21_N_BITN                     21
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_21_N_M                0x00200000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_21_N_S                        21

// Field:    [20] WEPROT_SEC_20_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_20_N                  0x00100000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_20_N_BITN                     20
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_20_N_M                0x00100000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_20_N_S                        20

// Field:    [19] WEPROT_SEC_19_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_19_N                  0x00080000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_19_N_BITN                     19
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_19_N_M                0x00080000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_19_N_S                        19

// Field:    [18] WEPROT_SEC_18_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_18_N                  0x00040000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_18_N_BITN                     18
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_18_N_M                0x00040000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_18_N_S                        18

// Field:    [17] WEPROT_SEC_17_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_17_N                  0x00020000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_17_N_BITN                     17
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_17_N_M                0x00020000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_17_N_S                        17

// Field:    [16] WEPROT_SEC_16_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_16_N                  0x00010000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_16_N_BITN                     16
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_16_N_M                0x00010000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_16_N_S                        16

// Field:    [15] WEPROT_SEC_15_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_15_N                  0x00008000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_15_N_BITN                     15
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_15_N_M                0x00008000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_15_N_S                        15

// Field:    [14] WEPROT_SEC_14_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_14_N                  0x00004000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_14_N_BITN                     14
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_14_N_M                0x00004000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_14_N_S                        14

// Field:    [13] WEPROT_SEC_13_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_13_N                  0x00002000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_13_N_BITN                     13
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_13_N_M                0x00002000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_13_N_S                        13

// Field:    [12] WEPROT_SEC_12_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_12_N                  0x00001000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_12_N_BITN                     12
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_12_N_M                0x00001000
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_12_N_S                        12

// Field:    [11] WEPROT_SEC_11_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_11_N                  0x00000800
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_11_N_BITN                     11
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_11_N_M                0x00000800
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_11_N_S                        11

// Field:    [10] WEPROT_SEC_10_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_10_N                  0x00000400
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_10_N_BITN                     10
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_10_N_M                0x00000400
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_10_N_S                        10

// Field:     [9] WEPROT_SEC_9_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_9_N                   0x00000200
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_9_N_BITN                       9
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_9_N_M                 0x00000200
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_9_N_S                          9

// Field:     [8] WEPROT_SEC_8_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_8_N                   0x00000100
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_8_N_BITN                       8
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_8_N_M                 0x00000100
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_8_N_S                          8

// Field:     [7] WEPROT_SEC_7_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_7_N                   0x00000080
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_7_N_BITN                       7
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_7_N_M                 0x00000080
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_7_N_S                          7

// Field:     [6] WEPROT_SEC_6_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_6_N                   0x00000040
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_6_N_BITN                       6
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_6_N_M                 0x00000040
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_6_N_S                          6

// Field:     [5] WEPROT_SEC_5_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_5_N                   0x00000020
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_5_N_BITN                       5
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_5_N_M                 0x00000020
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_5_N_S                          5

// Field:     [4] WEPROT_SEC_4_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_4_N                   0x00000010
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_4_N_BITN                       4
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_4_N_M                 0x00000010
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_4_N_S                          4

// Field:     [3] WEPROT_SEC_3_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_3_N                   0x00000008
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_3_N_BITN                       3
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_3_N_M                 0x00000008
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_3_N_S                          3

// Field:     [2] WEPROT_SEC_2_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_2_N                   0x00000004
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_2_N_BITN                       2
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_2_N_M                 0x00000004
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_2_N_S                          2

// Field:     [1] WEPROT_SEC_1_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_1_N                   0x00000002
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_1_N_BITN                       1
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_1_N_M                 0x00000002
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_1_N_S                          1

// Field:     [0] WEPROT_SEC_0_N
//
// 0: Sector protected
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_0_N                   0x00000001
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_0_N_BITN                       0
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_0_N_M                 0x00000001
#define CCFG_CCFG_WEPROT_31_0_BY2K_WEPROT_SEC_0_N_S                          0

//*****************************************************************************
//
// Register: CCFG_O_CCFG_WEPROT_SPARE_1
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CCFG_O_CCFG_WEPROT_SPARE_2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CCFG_O_CCFG_WEPROT_SPARE_3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CCFG_O_TRUSTZONE_FLASH_CFG
//
//*****************************************************************************
// Field: [16:10] NSADDR_BOUNDARY
//
// Value will be written to PRCM:NVMNSADDR.BOUNDARY by ROM boot FW only if
// CCFG_TI_OPTIONS.IDAU_CFG_ENABLE != 0xC5.
#define CCFG_TRUSTZONE_FLASH_CFG_NSADDR_BOUNDARY_W                           7
#define CCFG_TRUSTZONE_FLASH_CFG_NSADDR_BOUNDARY_M                  0x0001FC00
#define CCFG_TRUSTZONE_FLASH_CFG_NSADDR_BOUNDARY_S                          10

// Field:   [9:0] NSCADDR_BOUNDARY
//
// Value will be written to PRCM:NVMNSCADDR.BOUNDARY by ROM boot FW only if
// CCFG_TI_OPTIONS.IDAU_CFG_ENABLE != 0xC5.
#define CCFG_TRUSTZONE_FLASH_CFG_NSCADDR_BOUNDARY_W                         10
#define CCFG_TRUSTZONE_FLASH_CFG_NSCADDR_BOUNDARY_M                 0x000003FF
#define CCFG_TRUSTZONE_FLASH_CFG_NSCADDR_BOUNDARY_S                          0

//*****************************************************************************
//
// Register: CCFG_O_TRUSTZONE_SRAM_CFG
//
//*****************************************************************************
// Field:  [17:9] NSADDR_BOUNDARY
//
// Value will be written to PRCM:SRAMNSADDR.BOUNDARY by ROM boot FW only if
// CCFG_TI_OPTIONS.IDAU_CFG_ENABLE != 0xC5.
#define CCFG_TRUSTZONE_SRAM_CFG_NSADDR_BOUNDARY_W                            9
#define CCFG_TRUSTZONE_SRAM_CFG_NSADDR_BOUNDARY_M                   0x0003FE00
#define CCFG_TRUSTZONE_SRAM_CFG_NSADDR_BOUNDARY_S                            9

// Field:   [8:0] NSCADDR_BOUNDARY
//
// Value will be written to PRCM:SRAMNSCADDR.BOUNDARY by ROM boot FW only if
// CCFG_TI_OPTIONS.IDAU_CFG_ENABLE != 0xC5.
#define CCFG_TRUSTZONE_SRAM_CFG_NSCADDR_BOUNDARY_W                           9
#define CCFG_TRUSTZONE_SRAM_CFG_NSCADDR_BOUNDARY_M                  0x000001FF
#define CCFG_TRUSTZONE_SRAM_CFG_NSCADDR_BOUNDARY_S                           0

//*****************************************************************************
//
// Register: CCFG_O_SRAM_CFG
//
//*****************************************************************************
// Field:  [31:8] MEM_SEL
//
// Value will be written to SRAM_MMR:MEM_CTL.MEM_SEL by ROM boot FW
#define CCFG_SRAM_CFG_MEM_SEL_W                                             24
#define CCFG_SRAM_CFG_MEM_SEL_M                                     0xFFFFFF00
#define CCFG_SRAM_CFG_MEM_SEL_S                                              8

// Field:     [0] PARITY_DIS
//
// Value will be inverted and then written to PRCM:MCUSRAMCFG.PARITY_EN by ROM
// boot FW
#define CCFG_SRAM_CFG_PARITY_DIS                                    0x00000001
#define CCFG_SRAM_CFG_PARITY_DIS_BITN                                        0
#define CCFG_SRAM_CFG_PARITY_DIS_M                                  0x00000001
#define CCFG_SRAM_CFG_PARITY_DIS_S                                           0

//*****************************************************************************
//
// Register: CCFG_O_CPU_LOCK_CFG
//
//*****************************************************************************
// Field:     [4] LOCKNSVTOR_N
//
// Value will be inverted and written to PRCM:CPULOCK.LOCKNSVTOR by ROM boot FW
#define CCFG_CPU_LOCK_CFG_LOCKNSVTOR_N                              0x00000010
#define CCFG_CPU_LOCK_CFG_LOCKNSVTOR_N_BITN                                  4
#define CCFG_CPU_LOCK_CFG_LOCKNSVTOR_N_M                            0x00000010
#define CCFG_CPU_LOCK_CFG_LOCKNSVTOR_N_S                                     4

// Field:     [3] LOCKSVTAIRCR_N
//
// Value will be inverted and written to PRCM:CPULOCK.LOCKSVTAIRCR by ROM boot
// FW
#define CCFG_CPU_LOCK_CFG_LOCKSVTAIRCR_N                            0x00000008
#define CCFG_CPU_LOCK_CFG_LOCKSVTAIRCR_N_BITN                                3
#define CCFG_CPU_LOCK_CFG_LOCKSVTAIRCR_N_M                          0x00000008
#define CCFG_CPU_LOCK_CFG_LOCKSVTAIRCR_N_S                                   3

// Field:     [2] LOCKSAU_N
//
// Value will be inverted and written to PRCM:CPULOCK.LOCKSAU by ROM boot FW
#define CCFG_CPU_LOCK_CFG_LOCKSAU_N                                 0x00000004
#define CCFG_CPU_LOCK_CFG_LOCKSAU_N_BITN                                     2
#define CCFG_CPU_LOCK_CFG_LOCKSAU_N_M                               0x00000004
#define CCFG_CPU_LOCK_CFG_LOCKSAU_N_S                                        2

// Field:     [1] LOCKNSMPU_N
//
// Value will be inverted and written to PRCM:CPULOCK.LOCKNSMPU by ROM boot FW
#define CCFG_CPU_LOCK_CFG_LOCKNSMPU_N                               0x00000002
#define CCFG_CPU_LOCK_CFG_LOCKNSMPU_N_BITN                                   1
#define CCFG_CPU_LOCK_CFG_LOCKNSMPU_N_M                             0x00000002
#define CCFG_CPU_LOCK_CFG_LOCKNSMPU_N_S                                      1

// Field:     [0] LOCKSMPU_N
//
// Value will be inverted and written to PRCM:CPULOCK.LOCKSMPU by ROM boot FW
#define CCFG_CPU_LOCK_CFG_LOCKSMPU_N                                0x00000001
#define CCFG_CPU_LOCK_CFG_LOCKSMPU_N_BITN                                    0
#define CCFG_CPU_LOCK_CFG_LOCKSMPU_N_M                              0x00000001
#define CCFG_CPU_LOCK_CFG_LOCKSMPU_N_S                                       0

//*****************************************************************************
//
// Register: CCFG_O_DEB_AUTH_CFG
//
//*****************************************************************************
// Field:     [3] INTSPNIDEN
//
// Value will be written to CPU_DCB:DAUTHCTRL.INTSPNIDEN by ROM boot FW
#define CCFG_DEB_AUTH_CFG_INTSPNIDEN                                0x00000008
#define CCFG_DEB_AUTH_CFG_INTSPNIDEN_BITN                                    3
#define CCFG_DEB_AUTH_CFG_INTSPNIDEN_M                              0x00000008
#define CCFG_DEB_AUTH_CFG_INTSPNIDEN_S                                       3

// Field:     [2] SPNIDENSEL
//
// Value will be written to CPU_DCB:DAUTHCTRL.SPNIDENSEL by ROM boot FW
#define CCFG_DEB_AUTH_CFG_SPNIDENSEL                                0x00000004
#define CCFG_DEB_AUTH_CFG_SPNIDENSEL_BITN                                    2
#define CCFG_DEB_AUTH_CFG_SPNIDENSEL_M                              0x00000004
#define CCFG_DEB_AUTH_CFG_SPNIDENSEL_S                                       2

// Field:     [1] INTSPIDEN
//
// Value will be written to CPU_DCB:DAUTHCTRL.INTSPIDEN by ROM boot FW
#define CCFG_DEB_AUTH_CFG_INTSPIDEN                                 0x00000002
#define CCFG_DEB_AUTH_CFG_INTSPIDEN_BITN                                     1
#define CCFG_DEB_AUTH_CFG_INTSPIDEN_M                               0x00000002
#define CCFG_DEB_AUTH_CFG_INTSPIDEN_S                                        1

// Field:     [0] SPIDENSEL
//
// Value will be written to CPU_DCB:DAUTHCTRL.SPIDENSEL by ROM boot FW
#define CCFG_DEB_AUTH_CFG_SPIDENSEL                                 0x00000001
#define CCFG_DEB_AUTH_CFG_SPIDENSEL_BITN                                     0
#define CCFG_DEB_AUTH_CFG_SPIDENSEL_M                               0x00000001
#define CCFG_DEB_AUTH_CFG_SPIDENSEL_S                                        0


#endif // __CCFG__
