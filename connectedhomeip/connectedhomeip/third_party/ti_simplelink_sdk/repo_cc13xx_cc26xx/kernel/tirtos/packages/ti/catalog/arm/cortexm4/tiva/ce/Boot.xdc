/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

/*
 *  ======== Boot.xdc ========
 */

package ti.catalog.arm.cortexm4.tiva.ce;

import xdc.rov.ViewInfo;
import xdc.runtime.Assert;

/*!
 *  ======== Boot ========
 *  Stellaris M3 Boot Support.
 *
 *  The Boot module supports boot initialization for the Stellaris M3 devices.
 *  A special boot init function is created based on the configuration
 *  settings for this module.  This function is hooked into the
 *  xdc.runtime.Reset.fxns[] array and called very early at boot time (prior
 *  to cinit processing).
 *
 *  The code to support the boot module is placed in a separate section
 *  named `".text:.bootCodeSection"` to allow placement of this section in
 *  the linker .cmd file if necessary. This section is a subsection of the
 *  `".text"` section so this code will be placed into the .text section unless
 *  explicitly placed, either through
 *  `{@link xdc.cfg.Program#sectMap Program.sectMap}` or through a linker
 *  command file.
 */
@Template("./Boot.xdt")
module Boot
{
    metaonly struct ModuleView {
        Bool    configureClock;
        Bool    sysClockDivEnable;
        String  sysClockDiv;
        Bool    pwmClockDivEnable;
        String  pwmClockDiv;
        String  xtal;
        String  oscSrc;
        Bool    pllBypass;
        Bool    ioscDisable;
        Bool    moscDisable;
    }

    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
            [
                'Module',
                {
                    type: ViewInfo.MODULE,
                    viewInitFxn: 'viewInitModule',
                    structName: 'ModuleView'
                }
            ],
            ]
        });


    metaonly enum SysDiv {
        SYSDIV_1     = 0x07800000,  /*! CPU clock is osc/pll / 1 */
        SYSDIV_2     = 0x00C00000,  /*! CPU clock is osc/pll / 2 */
        SYSDIV_3     = 0x01400000,  /*! CPU clock is osc/pll / 3 */
        SYSDIV_4     = 0x01C00000,  /*! CPU clock is osc/pll / 4 */
        SYSDIV_5     = 0x02400000,  /*! CPU clock is osc/pll / 5 */
        SYSDIV_6     = 0x02C00000,  /*! CPU clock is osc/pll / 6 */
        SYSDIV_7     = 0x03400000,  /*! CPU clock is osc/pll / 7 */
        SYSDIV_8     = 0x03C00000,  /*! CPU clock is osc/pll / 8 */
        SYSDIV_9     = 0x04400000,  /*! CPU clock is osc/pll / 9 */
        SYSDIV_10    = 0x04C00000,  /*! CPU clock is osc/pll / 10 */
        SYSDIV_11    = 0x05400000,  /*! CPU clock is osc/pll / 11 */
        SYSDIV_12    = 0x05C00000,  /*! CPU clock is osc/pll / 12 */
        SYSDIV_13    = 0x06400000,  /*! CPU clock is osc/pll / 13 */
        SYSDIV_14    = 0x06C00000,  /*! CPU clock is osc/pll / 14 */
        SYSDIV_15    = 0x07400000,  /*! CPU clock is osc/pll / 15 */
        SYSDIV_16    = 0x07C00000,  /*! CPU clock is osc/pll / 16 */
        SYSDIV_17    = 0x88400000,  /*! CPU clock is osc/pll / 17 */
        SYSDIV_18    = 0x88C00000,  /*! CPU clock is osc/pll / 18 */
        SYSDIV_19    = 0x89400000,  /*! CPU clock is osc/pll / 19 */
        SYSDIV_20    = 0x89C00000,  /*! CPU clock is osc/pll / 20 */
        SYSDIV_21    = 0x8A400000,  /*! CPU clock is osc/pll / 21 */
        SYSDIV_22    = 0x8AC00000,  /*! CPU clock is osc/pll / 22 */
        SYSDIV_23    = 0x8B400000,  /*! CPU clock is osc/pll / 23 */
        SYSDIV_24    = 0x8BC00000,  /*! CPU clock is osc/pll / 24 */
        SYSDIV_25    = 0x8C400000,  /*! CPU clock is osc/pll / 25 */
        SYSDIV_26    = 0x8CC00000,  /*! CPU clock is osc/pll / 26 */
        SYSDIV_27    = 0x8D400000,  /*! CPU clock is osc/pll / 27 */
        SYSDIV_28    = 0x8DC00000,  /*! CPU clock is osc/pll / 28 */
        SYSDIV_29    = 0x8E400000,  /*! CPU clock is osc/pll / 29 */
        SYSDIV_30    = 0x8EC00000,  /*! CPU clock is osc/pll / 30 */
        SYSDIV_31    = 0x8F400000,  /*! CPU clock is osc/pll / 31 */
        SYSDIV_32    = 0x8FC00000,  /*! CPU clock is osc/pll / 32 */
        SYSDIV_33    = 0x90400000,  /*! CPU clock is osc/pll / 33 */
        SYSDIV_34    = 0x90C00000,  /*! CPU clock is osc/pll / 34 */
        SYSDIV_35    = 0x91400000,  /*! CPU clock is osc/pll / 35 */
        SYSDIV_36    = 0x91C00000,  /*! CPU clock is osc/pll / 36 */
        SYSDIV_37    = 0x92400000,  /*! CPU clock is osc/pll / 37 */
        SYSDIV_38    = 0x92C00000,  /*! CPU clock is osc/pll / 38 */
        SYSDIV_39    = 0x93400000,  /*! CPU clock is osc/pll / 39 */
        SYSDIV_40    = 0x93C00000,  /*! CPU clock is osc/pll / 40 */
        SYSDIV_41    = 0x94400000,  /*! CPU clock is osc/pll / 41 */
        SYSDIV_42    = 0x94C00000,  /*! CPU clock is osc/pll / 42 */
        SYSDIV_43    = 0x95400000,  /*! CPU clock is osc/pll / 43 */
        SYSDIV_44    = 0x95C00000,  /*! CPU clock is osc/pll / 44 */
        SYSDIV_45    = 0x96400000,  /*! CPU clock is osc/pll / 45 */
        SYSDIV_46    = 0x96C00000,  /*! CPU clock is osc/pll / 46 */
        SYSDIV_47    = 0x97400000,  /*! CPU clock is osc/pll / 47 */
        SYSDIV_48    = 0x97C00000,  /*! CPU clock is osc/pll / 48 */
        SYSDIV_49    = 0x98400000,  /*! CPU clock is osc/pll / 49 */
        SYSDIV_50    = 0x98C00000,  /*! CPU clock is osc/pll / 50 */
        SYSDIV_51    = 0x99400000,  /*! CPU clock is osc/pll / 51 */
        SYSDIV_52    = 0x99C00000,  /*! CPU clock is osc/pll / 52 */
        SYSDIV_53    = 0x9A400000,  /*! CPU clock is osc/pll / 53 */
        SYSDIV_54    = 0x9AC00000,  /*! CPU clock is osc/pll / 54 */
        SYSDIV_55    = 0x9B400000,  /*! CPU clock is osc/pll / 55 */
        SYSDIV_56    = 0x9BC00000,  /*! CPU clock is osc/pll / 56 */
        SYSDIV_57    = 0x9C400000,  /*! CPU clock is osc/pll / 57 */
        SYSDIV_58    = 0x9CC00000,  /*! CPU clock is osc/pll / 58 */
        SYSDIV_59    = 0x9D400000,  /*! CPU clock is osc/pll / 59 */
        SYSDIV_60    = 0x9DC00000,  /*! CPU clock is osc/pll / 60 */
        SYSDIV_61    = 0x9E400000,  /*! CPU clock is osc/pll / 61 */
        SYSDIV_62    = 0x9EC00000,  /*! CPU clock is osc/pll / 62 */
        SYSDIV_63    = 0x9F400000,  /*! CPU clock is osc/pll / 63 */
        SYSDIV_64    = 0x9FC00000,  /*! CPU clock is osc/pll / 64 */
        SYSDIV_2_5   = 0xC1000000,  /*! CPU clock is osc/pll / 2.5 */
        SYSDIV_3_5   = 0xC1800000,  /*! CPU clock is osc/pll / 3.5 */
        SYSDIV_4_5   = 0xC2000000,  /*! CPU clock is osc/pll / 4.5 */
        SYSDIV_5_5   = 0xC2800000,  /*! CPU clock is osc/pll / 5.5 */
        SYSDIV_6_5   = 0xC3000000,  /*! CPU clock is osc/pll / 6.5 */
        SYSDIV_7_5   = 0xC3800000,  /*! CPU clock is osc/pll / 7.5 */
        SYSDIV_8_5   = 0xC4000000,  /*! CPU clock is osc/pll / 8.5 */
        SYSDIV_9_5   = 0xC4800000,  /*! CPU clock is osc/pll / 9.5 */
        SYSDIV_10_5  = 0xC5000000,  /*! CPU clock is osc/pll / 10.5 */
        SYSDIV_11_5  = 0xC5800000,  /*! CPU clock is osc/pll / 11.5 */
        SYSDIV_12_5  = 0xC6000000,  /*! CPU clock is osc/pll / 12.5 */
        SYSDIV_13_5  = 0xC6800000,  /*! CPU clock is osc/pll / 13.5 */
        SYSDIV_14_5  = 0xC7000000,  /*! CPU clock is osc/pll / 14.5 */
        SYSDIV_15_5  = 0xC7800000,  /*! CPU clock is osc/pll / 15.5 */
        SYSDIV_16_5  = 0xC8000000,  /*! CPU clock is osc/pll / 16.5 */
        SYSDIV_17_5  = 0xC8800000,  /*! CPU clock is osc/pll / 17.5 */
        SYSDIV_18_5  = 0xC9000000,  /*! CPU clock is osc/pll / 18.5 */
        SYSDIV_19_5  = 0xC9800000,  /*! CPU clock is osc/pll / 19.5 */
        SYSDIV_20_5  = 0xCA000000,  /*! CPU clock is osc/pll / 20.5 */
        SYSDIV_21_5  = 0xCA800000,  /*! CPU clock is osc/pll / 21.5 */
        SYSDIV_22_5  = 0xCB000000,  /*! CPU clock is osc/pll / 22.5 */
        SYSDIV_23_5  = 0xCB800000,  /*! CPU clock is osc/pll / 23.5 */
        SYSDIV_24_5  = 0xCC000000,  /*! CPU clock is osc/pll / 24.5 */
        SYSDIV_25_5  = 0xCC800000,  /*! CPU clock is osc/pll / 25.5 */
        SYSDIV_26_5  = 0xCD000000,  /*! CPU clock is osc/pll / 26.5 */
        SYSDIV_27_5  = 0xCD800000,  /*! CPU clock is osc/pll / 27.5 */
        SYSDIV_28_5  = 0xCE000000,  /*! CPU clock is osc/pll / 28.5 */
        SYSDIV_29_5  = 0xCE800000,  /*! CPU clock is osc/pll / 29.5 */
        SYSDIV_30_5  = 0xCF000000,  /*! CPU clock is osc/pll / 30.5 */
        SYSDIV_31_5  = 0xCF800000,  /*! CPU clock is osc/pll / 31.5 */
        SYSDIV_32_5  = 0xD0000000,  /*! CPU clock is osc/pll / 32.5 */
        SYSDIV_33_5  = 0xD0800000,  /*! CPU clock is osc/pll / 33.5 */
        SYSDIV_34_5  = 0xD1000000,  /*! CPU clock is osc/pll / 34.5 */
        SYSDIV_35_5  = 0xD1800000,  /*! CPU clock is osc/pll / 35.5 */
        SYSDIV_36_5  = 0xD2000000,  /*! CPU clock is osc/pll / 36.5 */
        SYSDIV_37_5  = 0xD2800000,  /*! CPU clock is osc/pll / 37.5 */
        SYSDIV_38_5  = 0xD3000000,  /*! CPU clock is osc/pll / 38.5 */
        SYSDIV_39_5  = 0xD3800000,  /*! CPU clock is osc/pll / 39.5 */
        SYSDIV_40_5  = 0xD4000000,  /*! CPU clock is osc/pll / 40.5 */
        SYSDIV_41_5  = 0xD4800000,  /*! CPU clock is osc/pll / 41.5 */
        SYSDIV_42_5  = 0xD5000000,  /*! CPU clock is osc/pll / 42.5 */
        SYSDIV_43_5  = 0xD5800000,  /*! CPU clock is osc/pll / 43.5 */
        SYSDIV_44_5  = 0xD6000000,  /*! CPU clock is osc/pll / 44.5 */
        SYSDIV_45_5  = 0xD6800000,  /*! CPU clock is osc/pll / 45.5 */
        SYSDIV_46_5  = 0xD7000000,  /*! CPU clock is osc/pll / 46.5 */
        SYSDIV_47_5  = 0xD7800000,  /*! CPU clock is osc/pll / 47.5 */
        SYSDIV_48_5  = 0xD8000000,  /*! CPU clock is osc/pll / 48.5 */
        SYSDIV_49_5  = 0xD8800000,  /*! CPU clock is osc/pll / 49.5 */
        SYSDIV_50_5  = 0xD9000000,  /*! CPU clock is osc/pll / 50.5 */
        SYSDIV_51_5  = 0xD9800000,  /*! CPU clock is osc/pll / 51.5 */
        SYSDIV_52_5  = 0xDA000000,  /*! CPU clock is osc/pll / 52.5 */
        SYSDIV_53_5  = 0xDA800000,  /*! CPU clock is osc/pll / 53.5 */
        SYSDIV_54_5  = 0xDB000000,  /*! CPU clock is osc/pll / 54.5 */
        SYSDIV_55_5  = 0xDB800000,  /*! CPU clock is osc/pll / 55.5 */
        SYSDIV_56_5  = 0xDC000000,  /*! CPU clock is osc/pll / 56.5 */
        SYSDIV_57_5  = 0xDC800000,  /*! CPU clock is osc/pll / 57.5 */
        SYSDIV_58_5  = 0xDD000000,  /*! CPU clock is osc/pll / 58.5 */
        SYSDIV_59_5  = 0xDD800000,  /*! CPU clock is osc/pll / 59.5 */
        SYSDIV_60_5  = 0xDE000000,  /*! CPU clock is osc/pll / 60.5 */
        SYSDIV_61_5  = 0xDE800000,  /*! CPU clock is osc/pll / 61.5 */
        SYSDIV_62_5  = 0xDF000000,  /*! CPU clock is osc/pll / 62.5 */
        SYSDIV_63_5  = 0xDF800000   /*! CPU clock is osc/pll / 63.5 */
    }

    metaonly enum PwmDiv {
        PWMDIV_1     = 0x00000000,  /*! PWM clock /1 */
        PWMDIV_2     = 0x00100000,  /*! PWM clock /2 */
        PWMDIV_4     = 0x00120000,  /*! PWM clock /4 */
        PWMDIV_8     = 0x00140000,  /*! PWM clock /8 */
        PWMDIV_16    = 0x00160000,  /*! PWM clock /16 */
        PWMDIV_32    = 0x00180000,  /*! PWM clock /32 */
        PWMDIV_64    = 0x001A0000   /*! PWM clock /64 */
    }

    metaonly enum XtalFreq {
        XTAL_1MHZ    = 0x00000000,  /*! 1.0 MHz */
        XTAL_1_84MHZ = 0x00000040,  /*! 1.8432 MHz */
        XTAL_2MHZ    = 0x00000080,  /*! 2.0 MHz */
        XTAL_2_45MHZ = 0x000000C0,  /*! 2.4576 MHz */
        XTAL_3_57MHZ = 0x00000100,  /*! 3.579545 MHz */
        XTAL_3_68MHZ = 0x00000140,  /*! 3.6864 MHz */
        XTAL_4MHZ    = 0x00000180,  /*! 4.0 MHz */
        XTAL_4_09MHZ = 0x000001C0,  /*! 4.096 MHz */
        XTAL_4_91MHZ = 0x00000200,  /*! 4.9152 MHz */
        XTAL_5MHZ    = 0x00000240,  /*! 5.0 MHz */
        XTAL_5_12MHZ = 0x00000280,  /*! 5.12 MHz */
        XTAL_6MHZ    = 0x000002C0,  /*! 6.0 MHz */
        XTAL_6_14MHZ = 0x00000300,  /*! 6.144 MHz */
        XTAL_7_37MHZ = 0x00000340,  /*! 7.3728 MHz */
        XTAL_8MHZ    = 0x00000380,  /*! 8.0 MHz */
        XTAL_8_19MHZ = 0x000003C0,  /*! 8.192 MHz */
        XTAL_10MHZ   = 0x00000400,  /*! 10.0 MHz (USB) */
        XTAL_12MHZ   = 0x00000440,  /*! 12.0 MHz (USB) */
        XTAL_12_2MHZ = 0x00000480,  /*! 12.288 MHz */
        XTAL_13_5MHZ = 0x000004C0,  /*! 13.56 MHz */
        XTAL_14_3MHZ = 0x00000500,  /*! 14.31818 MHz */
        XTAL_16MHZ   = 0x00000540,  /*! 16.0 MHz (USB) */
        XTAL_16_3MHZ = 0x00000580,  /*! 16.384 MHz */
        XTAL_18MHZ   = 0x000005C0,  /*! 18.0 MHz */
        XTAL_20MHZ   = 0x00000600,  /*! 20.0 MHz */
        XTAL_24MHZ   = 0x00000640,  /*! 24.0 MHz */
        XTAL_25MHZ   = 0x00000680   /*! 25.0 MHz */
    }

    metaonly enum OscSrc {
        OSCSRC_MAIN     = 0x00000000,  /*! Main oscillator */
        OSCSRC_INT      = 0x00000010,  /*! Internal oscillator */
        OSCSRC_INT4     = 0x00000020,  /*! Internal oscillator / 4 */
        OSCSRC_INT30    = 0x00000030,  /*! Internal 30 KHz oscillator */
        OSCSRC_EXT4_19  = 0x80000028,  /*! External 4.19 MHz */
        OSCSRC_EXT32    = 0x80000038   /*! External 32 KHz */
    }

    /* The LDO setting is not supported on TIVA devices */
    metaonly enum LdoOut {
        LDOPCTL_2_55V    = 0x0000001F,  /* LDO output of 2.55V */
        LDOPCTL_2_60V    = 0x0000001E,  /* LDO output of 2.60V */
        LDOPCTL_2_65V    = 0x0000001D,  /* LDO output of 2.65V */
        LDOPCTL_2_70V    = 0x0000001C,  /* LDO output of 2.70V */
        LDOPCTL_2_75V    = 0x0000001B,  /* LDO output of 2.75V */
        LDOPCTL_2_25V    = 0x00000005,  /* LDO output of 2.25V */
        LDOPCTL_2_30V    = 0x00000004,  /* LDO output of 2.30V */
        LDOPCTL_2_35V    = 0x00000003,  /* LDO output of 2.35V */
        LDOPCTL_2_40V    = 0x00000002,  /* LDO output of 2.40V */
        LDOPCTL_2_45V    = 0x00000001,  /* LDO output of 2.45V */
        LDOPCTL_2_50V    = 0x00000000   /* LDO output of 2.50V */
    }

    metaonly enum VcoFreq {
        VCO_480          = 0xF1000000,  /*! VCO is 480 MHz */
        VCO_320          = 0xF0000000   /*! VCO is 320 MHz */
    }

    /*! Asserted in Boot_sysCtlClockSet */
    config Assert.Id A_mustUseEnhancedClockMode = {
        msg: "A_mustUseEnhancedClockMode: This device requires the Enhanced Clock Mode."
    };

    /*! Asserted in Boot_sysCtlClockFreqSet */
    config Assert.Id A_mustNotUseEnhancedClockMode = {
        msg: "A_mustNotUseEnhancedClockMode: This device does not support the Enhanced Clock Mode."
    };


    /*!
     *  Clock configuration flag, default is false.
     *
     *  Set to true to automatically configure the Clock.
     */
    metaonly config Bool configureClock = false;

    /*! SYS Clock Divisor */
    metaonly config SysDiv sysClockDiv = SYSDIV_1;

    /*! PWM Clock Divisor */
    metaonly config PwmDiv pwmClockDiv = PWMDIV_1;

    /*! Crystal Value */
    metaonly config XtalFreq xtal = XTAL_1MHZ;

    /*! Oscillator Source */
    metaonly config OscSrc oscSrc = OSCSRC_MAIN;

    /*! VCO frequency */
    metaonly config VcoFreq vcoFreq = VCO_320;

    /*! PLL Bypass flag */
    metaonly config Bool pllBypass = false;

    /*! Internal Oscillator Disable flag */
    metaonly config Bool ioscDisable = false;

    /*! Main Oscillator Disable flag */
    metaonly config Bool moscDisable = false;

    /*!
     *  @_nodoc
     *  LDO configuration flag, default is false.
     *
     *  Set to true to automatically configure the LDO.
     */
    metaonly config Bool configureLdo = false;

    /*!
     *  @_nodoc
     *  LDO VADJ setting, default is 2.5V
     */
    metaonly config LdoOut ldoOut = LDOPCTL_2_50V;

    /*!
     *  Enahnced Clocking Mode.
     *
     *  Must be set to true for OneMCU devices that support this feature.
     */
    metaonly config Bool enhancedClockMode = false;

    /*!
     *  Desired CPU Clock Frequency in Hz.
     *
     *  Configurable only on devices with "Enhanced Clocking Mode" support.
     */
    metaonly config UInt cpuFrequency = 40000000;

    /*
     *  @_nodoc
     *  ======== sysCtlClockSet ========
     *  wrapper for Boot_sysCtlClockSetI()
     *  verifies that this function is correct for this devices.
     *  then calls Boot_sysCtlClockSetI()
     *
     *  param ulConfig is the required configuration of the device clocking.
     */
    @DirectCall
    Void sysCtlClockSet(ULong ulConfig);

    /*
     *  @_nodoc
     *  ======== sysCtlClockFreqSet ========
     *  Configures the system clock.
     *
     *  wrapper for Boot_sysCtlClockFreqSetI()
     *  verifies that this function is correct for this devices.
     *  then calls Boot_sysCtlClockFreqSetI()
     *
     *  @param ulConfig is the required configuration of the device clocking.
     *  @param ulSysClock is the requested processor frequency.
     *
     *  @return The actual configured system clock frequency in Hz or zero if the
     *  value could not be changed due to a parameter error or PLL lock failure.
     */
    @DirectCall
    ULong sysCtlClockFreqSet(ULong ulConfig, ULong ulSysClock);

    /*!
     *  @_nodoc
     *  ======== registerFreqListener ========
     *  Register a module to be notified whenever the frequency changes.
     *
     *  The registered module must have a function named 'fireFrequencyUpdate'
     *  which takes the new frequency as an argument.
     */
    function registerFreqListener();

internal:

    /*
     *  ======== sysCtlClockSetI ========
     *  Configures the system clock. (legacy function)
     *
     *  param ulConfig is the required configuration of the device clocking.
     */
    Void sysCtlClockSetI(ULong ulConfig);

    /*
     *  ======== sysCtlClockFreqSetI ========
     *  Configures the system clock.
     *
     *  @param ulConfig is the required configuration of the device clocking.
     *  @param ulSysClock is the requested processor frequency.
     *
     *  @return The actual configured system clock frequency in Hz or zero if the
     *  value could not be changed due to a parameter error or PLL lock failure.
     */
    ULong sysCtlClockFreqSetI(ULong ulConfig, ULong ulSysClock);

    /*
     *  ======== sysCtlDelayI ========
     *  Provides a small delay.
     *
     *  @param ulCount is the number of delay loop iterations to perform.
     */
    Void sysCtlDelayI(ULong ulCount);

    /*
     *  ======== init ========
     *  Generated entry point into clock initialization function.
     *
     *  Installed as a Startup.firstFxn.
     *  Calls either Boot_sysCtlClockSet() or Boot_sysCtlClockFreqSet()
     *  depending on value of 'Boot.enhancedClockMode'.
     */
    Void init();

    /*!
     *  computed RCC value based on settings
     */
    metaonly config UInt ulConfig;

    /*!
     *  computed cpu frequency based on clock settings
     */
    metaonly config UInt computedCpuFrequency;

};
