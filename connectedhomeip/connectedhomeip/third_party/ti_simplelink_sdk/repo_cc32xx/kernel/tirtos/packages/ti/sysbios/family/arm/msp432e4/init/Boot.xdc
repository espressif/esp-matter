/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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

package ti.sysbios.family.arm.msp432e4.init;

import xdc.rov.ViewInfo;

/*!
 *  ======== Boot ========
 *  MSP432E Boot Support.
 *
 *  The Boot module supports boot initialization for the MSP432E devices.
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

    metaonly enum XtalFreq {
        XTAL_5MHZ  = 0x00000000,  /*! 5 MHz */
        XTAL_6MHZ  = 0x00000040,  /*! 6 MHz */
        XTAL_8MHZ  = 0x00000080,  /*! 8 MHz */
        XTAL_10MHZ = 0x000000C0,  /*! 10 MHz */
        XTAL_12MHZ = 0x00000100,  /*! 12 MHz */
        XTAL_16MHZ = 0x00000140,  /*! 16 MHz */
        XTAL_18MHZ = 0x00000180,  /*! 18 MHz */
        XTAL_20MHZ = 0x000001C0,  /*! 20 MHz */
        XTAL_24MHZ = 0x00000200,  /*! 24 MHz */
        XTAL_25MHZ = 0x00000240   /*! 25 MHz */
    }

    metaonly enum OscSrc {
        OSCSRC_MAIN     = 0x00000000,  /*! Main oscillator */
        OSCSRC_INT      = 0x00000010,  /*! Internal oscillator */
        OSCSRC_INT4     = 0x00000020,  /*! Internal oscillator / 4 */
        OSCSRC_INT30    = 0x00000030,  /*! Internal 30 KHz oscillator */
        OSCSRC_EXT4_19  = 0x80000028,  /*! External 4.19 MHz */
        OSCSRC_EXT32    = 0x80000038   /*! External 32 KHz */
    }

    metaonly enum VcoFreq {
        VCO_480          = 0xF1000000,  /*! VCO is 480 MHz */
        VCO_320          = 0xF0000000   /*! VCO is 320 MHz */
    }

    /*!
     *  Clock configuration flag, default is true.
     */
    metaonly config Bool configureClock = true;

    /*! Crystal Value */
    metaonly config XtalFreq xtal = XTAL_25MHZ;

    /*! Oscillator Source */
    metaonly config OscSrc oscSrc = OSCSRC_MAIN;

    /*! VCO frequency */
    metaonly config VcoFreq vcoFreq = VCO_480;

    /*! PLL Bypass flag */
    metaonly config Bool pllBypass = false;

    /*! Internal Oscillator Disable flag */
    metaonly config Bool ioscDisable = false;

    /*! Main Oscillator Disable flag */
    metaonly config Bool moscDisable = false;

    /*!
     *  @_nodoc
     *  Enhanced Clocking Mode.
     *  This config parameter has been deprecated.
     */
    metaonly config Bool enhancedClockMode = true;

    /*!
     *  Desired CPU Clock Frequency in Hz.
     */
    metaonly config UInt cpuFrequency = 120000000;

    /*
     *  @_nodoc
     *  ======== sysCtlClockFreqSet ========
     *  Configures the system clock.
     *
     *  wrapper for Boot_sysCtlClockFreqSetI()
     *
     *  @param ulConfig is the required configuration of the device clocking.
     *  @param ulSysClock is the requested processor frequency.
     *
     *  @return The actual configured system clock frequency in Hz or zero if
     *  the value could not be changed due to a parameter error or PLL lock
     *  failure.
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
     *  ======== sysCtlClockFreqSetI ========
     *  Configures the system clock.
     *
     *  @param ulConfig is the required configuration of the device clocking.
     *  @param ulSysClock is the requested processor frequency.
     *
     *  @return The actual configured system clock frequency in Hz or zero if
     *  the value could not be changed due to a parameter error or PLL lock
     *  failure.
     */
    ULong sysCtlClockFreqSetI(ULong ulConfig, ULong ulSysClock);

    /*
     *  ======== init ========
     *  Generated entry point into clock initialization function.
     *
     *  Installed as a Startup.firstFxn.
     *  Calls Boot_sysCtlClockFreqSet().
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
