/*
 * Copyright (c) 2012-2018, Texas Instruments Incorporated
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
 *  ======== Boot.xs ========
 */

var Boot = null;
var Program = null;

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    Boot.$private.cFilesProvided = true;

    var boot = Boot.$package.packageBase + "Boot.c";
    var boot_sysctl = Boot.$package.packageBase + "Boot_sysctl.c";

    return ([boot, boot_sysctl]);
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* provide getCFiles() for Build.getCFiles() */
    this.$private.getCFiles = getCFiles;
    this.$private.cFilesProvided = false;

    Boot = this;

    /* Assign setters to the Clock configs. */
    var GetSet = xdc.module("xdc.services.getset.GetSet");

    GetSet.init(Boot);

    GetSet.onSet(this, "configureClock", updateFrequency);
    GetSet.onSet(this, "sysClockDiv", updateFrequency);
    GetSet.onSet(this, "xtal", updateFrequency);
    GetSet.onSet(this, "oscSrc", updateFrequency);
    GetSet.onSet(this, "pllBypass", updateFrequency);
    GetSet.onSet(this, "ioscDisable", updateFrequency);
    GetSet.onSet(this, "moscDisable", updateFrequency);
    GetSet.onSet(this, "vcoFreq", updateFrequency);
    GetSet.onSet(this, "cpuFrequency", updateFrequency);
    GetSet.onSet(this, "enhancedClockMode", updateFrequency);
    GetSet.onSet(this, "configureLdo", ldoConfig);
}

function setUlConfig()
{
    Boot = xdc.module('ti.catalog.arm.cortexm4.tiva.ce.Boot');

    if (Boot.configureClock == false) {
        return;
    }

    /* Compute ulConfig based on config settings */

    /* start clean */
    Boot.ulConfig = 0;

    Boot.ulConfig |= Boot.oscSrc;

    if (Boot.enhancedClockMode) {
        Boot.ulConfig |= Boot.vcoFreq;
    }
    else {
        Boot.ulConfig |= Boot.sysClockDiv;
    }

    Boot.ulConfig |= Boot.pwmClockDiv;

    Boot.ulConfig |= Boot.xtal;

    if (Boot.pllBypass) {
        Boot.ulConfig |= 0x00000800;
    }

    if (Boot.ioscDisable) {
        Boot.ulConfig |= 0x00000002;
    }

    if (Boot.moscDisable) {
        Boot.ulConfig |= 0x00000001;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Program = xdc.useModule('xdc.cfg.Program');

    /* only install Boot_init if using XDC runtime */
    if (Program.build.rtsName === null) {
        return;
    }

    /*
     * Install Boot_init as a FirstFxn
     * It would be faster to install it as a Reset functions
     * but Asserts don't work until AFTER cinit
     */

    var Startup = xdc.useModule('xdc.runtime.Startup');
    Startup.firstFxns.$add(Boot.init);

    setUlConfig();
}

/*
 *  ======== getEnumString ========
 *  Get the enum value string name, not 0, 1, 2 or 3, etc.  For an enumeration
 *  type property.
 *
 *  Example usage:
 *  if obj contains an enumeration type property "Enum enumProp"
 *
 *  view.enumString = getEnumString(obj.enumProp);
 *
 */
function getEnumString(enumProperty)
{
    /*
     *  Split the string into tokens in order to get rid of the huge package
     *  path that precedes the enum string name. Return the last 2 tokens
     *  concatenated with "_"
     */
    var enumStrArray = String(enumProperty).split(".");
    var len = enumStrArray.length;
    return (enumStrArray[len - 1]);
}

/*
 *  ======== viewInitModule ========
 *  Display the module properties in ROV
 */
function viewInitModule(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Boot = xdc.useModule('ti.catalog.arm.cortexm4.tiva.ce.Boot');
    var modCfg = Program.getModuleConfig(Boot.$name);

    view.configureClock         = modCfg.configureClock;
    view.sysClockDivEnable      = (modCfg.sysClockDiv != Boot.SYSDIV_1);
    view.sysClockDiv            = getEnumString(modCfg.sysClockDiv);
    view.pwmClockDivEnable      = (modCfg.pwmClockDiv != Boot.PWMDIV_1);
    view.pwmClockDiv            = getEnumString(modCfg.pwmClockDiv);
    view.xtal                   = getEnumString(modCfg.xtal);
    view.oscSrc                 = getEnumString(modCfg.oscSrc);
    view.pllBypass              = modCfg.pllBypass;
    view.ioscDisable            = modCfg.ioscDisable;
    view.moscDisable            = modCfg.moscDisable;
}

function doSysDiv(freq)
{
    switch(Boot.sysClockDiv) {
        case Boot.SYSDIV_1:
            freq /= 1;
            break;

        case Boot.SYSDIV_2:
            freq /= 2;
            break;

        case Boot.SYSDIV_3:
            freq /= 3;
            break;

        case Boot.SYSDIV_4:
            freq /= 4;
            break;

        case Boot.SYSDIV_5:
            freq /= 5;
            break;

        case Boot.SYSDIV_6:
            freq /= 6;
            break;

        case Boot.SYSDIV_7:
            freq /= 7;
            break;

        case Boot.SYSDIV_8:
            freq /= 8;
            break;

        case Boot.SYSDIV_9:
            freq /= 9;
            break;

        case Boot.SYSDIV_10:
            freq /= 10;
            break;

        case Boot.SYSDIV_11:
            freq /= 11;
            break;

        case Boot.SYSDIV_12:
            freq /= 12;
            break;

        case Boot.SYSDIV_13:
            freq /= 13;
            break;

        case Boot.SYSDIV_14:
            freq /= 14;
            break;

        case Boot.SYSDIV_15:
            freq /= 15;
            break;

        case Boot.SYSDIV_16:
            freq /= 16;
            break;

        case Boot.SYSDIV_17:
            freq /= 17;
            break;

        case Boot.SYSDIV_18:
            freq /= 18;
            break;

        case Boot.SYSDIV_19:
            freq /= 19;
            break;

        case Boot.SYSDIV_20:
            freq /= 20;
            break;

        case Boot.SYSDIV_21:
            freq /= 21;
            break;

        case Boot.SYSDIV_22:
            freq /= 22;
            break;

        case Boot.SYSDIV_23:
            freq /= 23;
            break;

        case Boot.SYSDIV_24:
            freq /= 24;
            break;

        case Boot.SYSDIV_25:
            freq /= 25;
            break;

        case Boot.SYSDIV_26:
            freq /= 26;
            break;

        case Boot.SYSDIV_27:
            freq /= 27;
            break;

        case Boot.SYSDIV_28:
            freq /= 28;
            break;

        case Boot.SYSDIV_29:
            freq /= 29;
            break;

        case Boot.SYSDIV_30:
            freq /= 30;
            break;

        case Boot.SYSDIV_31:
            freq /= 31;
            break;

        case Boot.SYSDIV_32:
            freq /= 32;
            break;

        case Boot.SYSDIV_33:
            freq /= 33;
            break;

        case Boot.SYSDIV_34:
            freq /= 34;
            break;

        case Boot.SYSDIV_35:
            freq /= 35;
            break;

        case Boot.SYSDIV_36:
            freq /= 36;
            break;

        case Boot.SYSDIV_37:
            freq /= 37;
            break;

        case Boot.SYSDIV_38:
            freq /= 38;
            break;

        case Boot.SYSDIV_39:
            freq /= 39;
            break;

        case Boot.SYSDIV_40:
            freq /= 40;
            break;

        case Boot.SYSDIV_41:
            freq /= 41;
            break;

        case Boot.SYSDIV_42:
            freq /= 42;
            break;

        case Boot.SYSDIV_43:
            freq /= 43;
            break;

        case Boot.SYSDIV_44:
            freq /= 44;
            break;

        case Boot.SYSDIV_45:
            freq /= 45;
            break;

        case Boot.SYSDIV_46:
            freq /= 46;
            break;

        case Boot.SYSDIV_47:
            freq /= 47;
            break;

        case Boot.SYSDIV_48:
            freq /= 48;
            break;

        case Boot.SYSDIV_49:
            freq /= 49;
            break;

        case Boot.SYSDIV_50:
            freq /= 50;
            break;

        case Boot.SYSDIV_51:
            freq /= 51;
            break;

        case Boot.SYSDIV_52:
            freq /= 52;
            break;

        case Boot.SYSDIV_53:
            freq /= 53;
            break;

        case Boot.SYSDIV_54:
            freq /= 54;
            break;

        case Boot.SYSDIV_55:
            freq /= 55;
            break;

        case Boot.SYSDIV_56:
            freq /= 56;
            break;

        case Boot.SYSDIV_57:
            freq /= 57;
            break;

        case Boot.SYSDIV_58:
            freq /= 58;
            break;

        case Boot.SYSDIV_59:
            freq /= 59;
            break;

        case Boot.SYSDIV_60:
            freq /= 60;
            break;

        case Boot.SYSDIV_61:
            freq /= 61;
            break;

        case Boot.SYSDIV_62:
            freq /= 62;
            break;

        case Boot.SYSDIV_63:
            freq /= 63;
            break;

        case Boot.SYSDIV_64:
            freq /= 64;
            break;

        case Boot.SYSDIV_2_5:
            freq /= 2.5;
            break;

        case Boot.SYSDIV_3_5:
            freq /= 3.5;
            break;

        case Boot.SYSDIV_4_5:
            freq /= 4.5;
            break;

        case Boot.SYSDIV_5_5:
            freq /= 5.5;
            break;

        case Boot.SYSDIV_6_5:
            freq /= 6.5;
            break;

        case Boot.SYSDIV_7_5:
            freq /= 7.5;
            break;

        case Boot.SYSDIV_8_5:
            freq /= 8.5;
            break;

        case Boot.SYSDIV_9_5:
            freq /= 9.5;
            break;

        case Boot.SYSDIV_10_5:
            freq /= 10.5;
            break;

        case Boot.SYSDIV_11_5:
            freq /= 11.5;
            break;

        case Boot.SYSDIV_12_5:
            freq /= 12.5;
            break;

        case Boot.SYSDIV_13_5:
            freq /= 13.5;
            break;

        case Boot.SYSDIV_14_5:
            freq /= 14.5;
            break;

        case Boot.SYSDIV_15_5:
            freq /= 15.5;
            break;

        case Boot.SYSDIV_16_5:
            freq /= 16.5;
            break;

        case Boot.SYSDIV_17_5:
            freq /= 17.5;
            break;

        case Boot.SYSDIV_18_5:
            freq /= 18.5;
            break;

        case Boot.SYSDIV_19_5:
            freq /= 19.5;
            break;

        case Boot.SYSDIV_20_5:
            freq /= 20.5;
            break;

        case Boot.SYSDIV_21_5:
            freq /= 21.5;
            break;

        case Boot.SYSDIV_22_5:
            freq /= 22.5;
            break;

        case Boot.SYSDIV_23_5:
            freq /= 23.5;
            break;

        case Boot.SYSDIV_24_5:
            freq /= 24.5;
            break;

        case Boot.SYSDIV_25_5:
            freq /= 25.5;
            break;

        case Boot.SYSDIV_26_5:
            freq /= 26.5;
            break;

        case Boot.SYSDIV_27_5:
            freq /= 27.5;
            break;

        case Boot.SYSDIV_28_5:
            freq /= 28.5;
            break;

        case Boot.SYSDIV_29_5:
            freq /= 29.5;
            break;

        case Boot.SYSDIV_30_5:
            freq /= 30.5;
            break;

        case Boot.SYSDIV_31_5:
            freq /= 31.5;
            break;

        case Boot.SYSDIV_32_5:
            freq /= 32.5;
            break;

        case Boot.SYSDIV_33_5:
            freq /= 33.5;
            break;

        case Boot.SYSDIV_34_5:
            freq /= 34.5;
            break;

        case Boot.SYSDIV_35_5:
            freq /= 35.5;
            break;

        case Boot.SYSDIV_36_5:
            freq /= 36.5;
            break;

        case Boot.SYSDIV_37_5:
            freq /= 37.5;
            break;

        case Boot.SYSDIV_38_5:
            freq /= 38.5;
            break;

        case Boot.SYSDIV_39_5:
            freq /= 39.5;
            break;

        case Boot.SYSDIV_40_5:
            freq /= 40.5;
            break;

        case Boot.SYSDIV_41_5:
            freq /= 41.5;
            break;

        case Boot.SYSDIV_42_5:
            freq /= 42.5;
            break;

        case Boot.SYSDIV_43_5:
            freq /= 43.5;
            break;

        case Boot.SYSDIV_44_5:
            freq /= 44.5;
            break;

        case Boot.SYSDIV_45_5:
            freq /= 45.5;
            break;

        case Boot.SYSDIV_46_5:
            freq /= 46.5;
            break;

        case Boot.SYSDIV_47_5:
            freq /= 47.5;
            break;

        case Boot.SYSDIV_48_5:
            freq /= 48.5;
            break;

        case Boot.SYSDIV_49_5:
            freq /= 49.5;
            break;

        case Boot.SYSDIV_50_5:
            freq /= 50.5;
            break;

        case Boot.SYSDIV_51_5:
            freq /= 51.5;
            break;

        case Boot.SYSDIV_52_5:
            freq /= 52.5;
            break;

        case Boot.SYSDIV_53_5:
            freq /= 53.5;
            break;

        case Boot.SYSDIV_54_5:
            freq /= 54.5;
            break;

        case Boot.SYSDIV_55_5:
            freq /= 55.5;
            break;

        case Boot.SYSDIV_56_5:
            freq /= 56.5;
            break;

        case Boot.SYSDIV_57_5:
            freq /= 57.5;
            break;

        case Boot.SYSDIV_58_5:
            freq /= 58.5;
            break;

        case Boot.SYSDIV_59_5:
            freq /= 59.5;
            break;

        case Boot.SYSDIV_60_5:
            freq /= 60.5;
            break;

        case Boot.SYSDIV_61_5:
            freq /= 61.5;
            break;

        case Boot.SYSDIV_62_5:
            freq /= 62.5;
            break;

        case Boot.SYSDIV_63_5:
            freq /= 63.5;
            break;

    }

    return (freq);
}

/*
 *  ======== getXtalFrequency ========
 */
function getXtalFrequency()
{
   switch (Boot.xtal) {
        case Boot.XTAL_1MHZ:
            return (1000000);

        case Boot.XTAL_1_84MHZ:
            return (1843200);

        case Boot.XTAL_2MHZ:
            return (2000000);

        case Boot.XTAL_2_45MHZ:
            return (2457600);

        case Boot.XTAL_3_57MHZ:
            return (3579450);

        case Boot.XTAL_3_68MHZ:
            return (3686400);

        case Boot.XTAL_4MHZ:
            return (4000000);

        case Boot.XTAL_4_09MHZ:
            return (4096000);

        case Boot.XTAL_4_91MHZ:
            return (4915200);

        case Boot.XTAL_5MHZ:
            return (5000000);

        case Boot.XTAL_5_12MHZ:
            return (5120000);

        case Boot.XTAL_6MHZ:
            return (6000000);

        case Boot.XTAL_6_14MHZ:
            return (6144000);

        case Boot.XTAL_7_37MHZ:
            return (7372800);

        case Boot.XTAL_8MHZ:
            return (8000000);

        case Boot.XTAL_8_19MHZ:
            return (8192000);

        case Boot.XTAL_10MHZ:
            return (10000000);

        case Boot.XTAL_12MHZ:
            return (12000000);

        case Boot.XTAL_12_2MHZ:
            return (12288000);

        case Boot.XTAL_13_5MHZ:
            return (13560000);

        case Boot.XTAL_14_3MHZ:
            return (14318180);

        case Boot.XTAL_16MHZ:
            return (16000000);

        case Boot.XTAL_16_3MHZ:
            return (16384000);
    }
}

/*
 *  ======== getFrequency ========
 */
function getFrequency()
{
    if (Boot.configureClock == false) {
        /* no way to know */
        return (0);
    }

    /*
     * newer devices use a more complex algorithm for
     * computing frequency
     */
    if (Boot.enhancedClockMode == true) {
        return (sysCtlClockFreqSet(Boot.ulConfig, Boot.cpuFrequency));
    }

    /*
     * legacy algorithm below
     */

    /*
     * If the pll is being used (which is normal)
     * then it will always be configured to output 200MHz.
     * Then the System Clock is the 200MHz PLL output or
     * 200MHz divided down by some fixed amount (Boot.SYSDIV).
     */
    if (Boot.pllBypass == false) {
        var freq = doSysDiv(200000000);
        return (freq);
    }

    switch (Boot.oscSrc) {
        case Boot.OSCSRC_MAIN:
            return (doSysDiv(getXtalFrequency()));

        case Boot.OSCSRC_INT:
            return (doSysDiv(15000000)); /* 15MHz */

        case Boot.OSCSRC_INT4:
            return (doSysDiv(3750000));  /* 3.75MHz */

        case Boot.OSCSRC_30:
            return (doSysDiv(30000));    /* 30KHz */
    }
}


/* Array of listeners, used by 'registerFreqListener' and 'updateFrequency'. */
var listeners = new Array();

/*
 *  ======== registerFreqListener ========
 *  Called by other modules (e.g., BIOS), to register themselves to listen
 *  for changes to the device frequency configured by the Boot module.
 */
function registerFreqListener(listener)
{
    listeners[listeners.length] = listener;
}

/*
 *  ======== updateFrequency ========
 *  Update all of the listeners whenever the PLL configuration changes
 *  (assuming configurePll is true).
 */
function updateFrequency(field, val)
{
    /* Don't fire the update if configurePll is false. */
    if (!Boot.configureClock) {
        return;
    }

    /* make sure Boot.ulConfig is current */
    setUlConfig();

    Boot.computedCpuFrequency = getFrequency();

    /* Notify each of the listeners of the new frequency value. */
    for each (var listener in listeners) {
        listener.fireFrequencyUpdate(Boot.computedCpuFrequency);
    }
}

/*
 *  All the below code and tables are derived from sysctl.c
 *  and a required to compute the actual CPU frequency that
 *  BIOS_sysCtlClockFreqSetI() will set the device to run at
 */

var SYSCTL_RSCLKCFG_MEMTIMU         = 0x80000000;  // Memory Timing Register Update
var SYSCTL_RSCLKCFG_NEWFREQ         = 0x40000000;  // New PLLFREQ Accept
var SYSCTL_RSCLKCFG_ACG             = 0x20000000;  // Auto Clock Gating Enable
var SYSCTL_RSCLKCFG_USEPLL          = 0x10000000;  // Use PLL
var SYSCTL_RSCLKCFG_PLLSRC_M        = 0x0F000000;  // PLL Source
var SYSCTL_RSCLKCFG_PLLSRC_PIOSC    = 0x00000000;  // PIOSC is PLL input clock source
var SYSCTL_RSCLKCFG_PLLSRC_MOSC     = 0x03000000;  // MOSC is PLL input clock source
var SYSCTL_RSCLKCFG_OSCSRC_M        = 0x00F00000;  // Oscillator Source
var SYSCTL_RSCLKCFG_OSCSRC_PIOSC    = 0x00000000;  // PIOSC is oscillator source
var SYSCTL_RSCLKCFG_OSCSRC_LFIOSC   = 0x00200000;  // LFIOSC is oscillator source
var SYSCTL_RSCLKCFG_OSCSRC_MOSC     = 0x00300000;  // MOSC is oscillator source
var SYSCTL_RSCLKCFG_OSCSRC_RTC      = 0x00400000;  // RTC is oscillator source
var SYSCTL_RSCLKCFG_OSYSDIV_M       = 0x000FFC00;  // Oscillator System Clock Divisor
var SYSCTL_RSCLKCFG_PSYSDIV_M       = 0x000003FF;  // PLL System Clock Divisor
var SYSCTL_RSCLKCFG_OSYSDIV_S       = 10;
var SYSCTL_RSCLKCFG_PSYSDIV_S       = 0;

var SYSCTL_PLLFREQ0_PLLPWR          = 0x00800000;  // PLL Power
var SYSCTL_PLLFREQ0_MFRAC_M         = 0x000FFC00;  // PLL M Fractional Value
var SYSCTL_PLLFREQ0_MINT_M          = 0x000003FF;  // PLL M Integer Value
var SYSCTL_PLLFREQ0_MFRAC_S         = 10;
var SYSCTL_PLLFREQ0_MINT_S          = 0;

var SYSCTL_PLLFREQ1_Q_M     = 0x00001F00;  // PLL Q Value
var SYSCTL_PLLFREQ1_N_M     = 0x0000001F;  // PLL N Value
var SYSCTL_PLLFREQ1_Q_S     = 8;
var SYSCTL_PLLFREQ1_N_S     = 0;

var SYSCTL_OSC_MAIN         = 0x00000000;  // Osc source is main osc
var SYSCTL_OSC_INT          = 0x00000010;  // Osc source is int. osc
var SYSCTL_OSC_INT4         = 0x00000020;  // Osc source is int. osc /4
var SYSCTL_OSC_INT30        = 0x00000030;  // Osc source is int. 30 KHz
var SYSCTL_OSC_EXT4_19      = 0x80000028;  // Osc source is ext. 4.19 MHz
var SYSCTL_OSC_EXT32        = 0x80000038;  // Osc source is ext. 32 KHz

var SYSCTL_XTAL_1MHZ        = 0x00000000;  // External crystal is 1MHz
var SYSCTL_XTAL_1_84MHZ     = 0x00000040;  // External crystal is 1.8432MHz
var SYSCTL_XTAL_2MHZ        = 0x00000080;  // External crystal is 2MHz
var SYSCTL_XTAL_2_45MHZ     = 0x000000C0;  // External crystal is 2.4576MHz
var SYSCTL_XTAL_3_57MHZ     = 0x00000100;  // External crystal is 3.579545MHz
var SYSCTL_XTAL_3_68MHZ     = 0x00000140;  // External crystal is 3.6864MHz
var SYSCTL_XTAL_4MHZ        = 0x00000180;  // External crystal is 4MHz
var SYSCTL_XTAL_4_09MHZ     = 0x000001C0;  // External crystal is 4.096MHz
var SYSCTL_XTAL_4_91MHZ     = 0x00000200;  // External crystal is 4.9152MHz
var SYSCTL_XTAL_5MHZ        = 0x00000240;  // External crystal is 5MHz
var SYSCTL_XTAL_5_12MHZ     = 0x00000280;  // External crystal is 5.12MHz
var SYSCTL_XTAL_6MHZ        = 0x000002C0;  // External crystal is 6MHz
var SYSCTL_XTAL_6_14MHZ     = 0x00000300;  // External crystal is 6.144MHz
var SYSCTL_XTAL_7_37MHZ     = 0x00000340;  // External crystal is 7.3728MHz
var SYSCTL_XTAL_8MHZ        = 0x00000380;  // External crystal is 8MHz
var SYSCTL_XTAL_8_19MHZ     = 0x000003C0;  // External crystal is 8.192MHz
var SYSCTL_XTAL_10MHZ       = 0x00000400;  // External crystal is 10 MHz
var SYSCTL_XTAL_12MHZ       = 0x00000440;  // External crystal is 12 MHz
var SYSCTL_XTAL_12_2MHZ     = 0x00000480;  // External crystal is 12.288 MHz
var SYSCTL_XTAL_13_5MHZ     = 0x000004C0;  // External crystal is 13.56 MHz
var SYSCTL_XTAL_14_3MHZ     = 0x00000500;  // External crystal is 14.31818 MHz
var SYSCTL_XTAL_16MHZ       = 0x00000540;  // External crystal is 16 MHz
var SYSCTL_XTAL_16_3MHZ     = 0x00000580;  // External crystal is 16.384 MHz
var SYSCTL_XTAL_18MHZ       = 0x000005C0;  // External crystal is 18.0 MHz
var SYSCTL_XTAL_20MHZ       = 0x00000600;  // External crystal is 20.0 MHz
var SYSCTL_XTAL_24MHZ       = 0x00000640;  // External crystal is 24.0 MHz
var SYSCTL_XTAL_25MHZ       = 0x00000680;  // External crystal is 25.0 MHz

var SYSCTL_MOSCCTL_SESRC    = 0x00000020;  // Single-Ended Source
var SYSCTL_MOSCCTL_OSCRNG   = 0x00000010;  // Oscillator Range
var SYSCTL_MOSCCTL_PWRDN    = 0x00000008;  // Power Down
var SYSCTL_MOSCCTL_NOXTAL   = 0x00000004;  // No Crystal Connected
var SYSCTL_MOSCCTL_MOSCIM   = 0x00000002;  // MOSC Failure Action
var SYSCTL_MOSCCTL_CVAL     = 0x00000001;  // Clock Validation for MOSC

var SYSCTL_USE_PLL          = 0x00000000;  // System clock is the PLL clock
var SYSCTL_USE_OSC          = 0x00003800;  // System clock is the osc clock

var SYSCTL_PLLSTAT_LOCK     = 0x00000001;  // PLL Lock

function PLL_M_TO_REG(mi, mf)
{
    return (mi | (mf << SYSCTL_PLLFREQ0_MFRAC_S));
}

function PLL_N_TO_REG(n)
{
   return ((n - 1) << SYSCTL_PLLFREQ1_N_S);
}

var g_pulXTALtoVCO = [
    [
        //
        // VCO 320 MHz
        //
        [ PLL_M_TO_REG(64, 0),   PLL_N_TO_REG(1) ],     // 5 MHz
        [ PLL_M_TO_REG(62, 512), PLL_N_TO_REG(1) ],     // 5.12 MHz
        [ PLL_M_TO_REG(160, 0),  PLL_N_TO_REG(3) ],     // 6 MHz
        [ PLL_M_TO_REG(52, 85),  PLL_N_TO_REG(1) ],     // 6.144 MHz
        [ PLL_M_TO_REG(43, 412), PLL_N_TO_REG(1) ],     // 7.3728 MHz
        [ PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(1) ],     // 8 MHz
        [ PLL_M_TO_REG(39, 64),  PLL_N_TO_REG(1) ],     // 8.192 MHz
        [ PLL_M_TO_REG(32, 0),   PLL_N_TO_REG(1) ],     // 10 MHz
        [ PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(3) ],     // 12 MHz
        [ PLL_M_TO_REG(26, 43),  PLL_N_TO_REG(1) ],     // 12.288 MHz
        [ PLL_M_TO_REG(23, 613), PLL_N_TO_REG(1) ],     // 13.56 MHz
        [ PLL_M_TO_REG(22, 358), PLL_N_TO_REG(1) ],     // 14.318180 MHz
        [ PLL_M_TO_REG(20, 0),   PLL_N_TO_REG(1) ],     // 16 MHz
        [ PLL_M_TO_REG(19, 544), PLL_N_TO_REG(1) ],     // 16.384 MHz
        [ PLL_M_TO_REG(160, 0),  PLL_N_TO_REG(9) ],     // 18 MHz
        [ PLL_M_TO_REG(16, 0),   PLL_N_TO_REG(1) ],     // 20 MHz
        [ PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(3) ],     // 24 MHz
        [ PLL_M_TO_REG(64, 0),   PLL_N_TO_REG(5) ],     // 25 MHz
    ],
    [
        //
        // VCO 480 MHz
        //
        [ PLL_M_TO_REG(96, 0),   PLL_N_TO_REG(1) ],     // 5 MHz
        [ PLL_M_TO_REG(93, 768), PLL_N_TO_REG(1) ],     // 5.12 MHz
        [ PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(1) ],     // 6 MHz
        [ PLL_M_TO_REG(78, 128), PLL_N_TO_REG(1) ],     // 6.144 MHz
        [ PLL_M_TO_REG(65, 107), PLL_N_TO_REG(1) ],     // 7.3728 MHz
        [ PLL_M_TO_REG(60, 0),   PLL_N_TO_REG(1) ],     // 8 MHz
        [ PLL_M_TO_REG(58, 608), PLL_N_TO_REG(1) ],     // 8.192 MHz
        [ PLL_M_TO_REG(48, 0),   PLL_N_TO_REG(1) ],     // 10 MHz
        [ PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(1) ],     // 12 MHz
        [ PLL_M_TO_REG(39, 64),  PLL_N_TO_REG(1) ],     // 12.288 MHz
        [ PLL_M_TO_REG(35, 408), PLL_N_TO_REG(1) ],     // 13.56 MHz
        [ PLL_M_TO_REG(33, 536), PLL_N_TO_REG(1) ],     // 14.318180 MHz
        [ PLL_M_TO_REG(30, 0),   PLL_N_TO_REG(1) ],     // 16 MHz
        [ PLL_M_TO_REG(29, 304), PLL_N_TO_REG(1) ],     // 16.384 MHz
        [ PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(3) ],     // 18 MHz
        [ PLL_M_TO_REG(24, 0),   PLL_N_TO_REG(1) ],     // 20 MHz
        [ PLL_M_TO_REG(20, 0),   PLL_N_TO_REG(1) ],     // 24 MHz
        [ PLL_M_TO_REG(96, 0),   PLL_N_TO_REG(5) ],     // 25 MHz
    ]
];

var g_pulXtals = [
    1000000,
    1843200,
    2000000,
    2457600,
    3579545,
    3686400,
    4000000,
    4096000,
    4915200,
    5000000,
    5120000,
    6000000,
    6144000,
    7372800,
    8000000,
    8192000,
    10000000,
    12000000,
    12288000,
    13560000,
    14318180,
    16000000,
    16384000,
    18000000,
    20000000,
    24000000,
    25000000
];

var g_pulVCOFrequencies = [
    320000000,                              // VCO 320
    480000000,                              // VCO 480
];

/* simulated HW registers */
var SYSCTL_PLLFREQ0         = 0;
var SYSCTL_PLLFREQ1         = 0;

function HWREG(x)
{
    return x;
}

function SYSCTL_XTALCFG_TO_INDEX(a)
{
    return ((a & 0x7c0) >> 6);
}

function GetFrequency(ulXtal)
{
    var ulF1, ulF2, ulResult;
    var ulPInt, ulPFract, ulQ, ulN;

    //
    // Extract all of the values from the hardware registers.
    //
    ulPFract = (HWREG(SYSCTL_PLLFREQ0) & SYSCTL_PLLFREQ0_MFRAC_M) >>
               SYSCTL_PLLFREQ0_MFRAC_S;
    ulPInt = HWREG(SYSCTL_PLLFREQ0) & SYSCTL_PLLFREQ0_MINT_M;

    ulQ = ((HWREG(SYSCTL_PLLFREQ1) & SYSCTL_PLLFREQ1_Q_M) >>
           SYSCTL_PLLFREQ1_Q_S) + 1;
    ulN = ((HWREG(SYSCTL_PLLFREQ1) & SYSCTL_PLLFREQ1_N_M) >>
           SYSCTL_PLLFREQ1_N_S) + 1;

    //
    // Calculate the multiplier for bits 9:5.
    //
    ulF1 = ulPFract / 32;

    //
    // Calculate the multiplier for bits 4:0.
    //
    ulF2 = ulPFract - (ulF1 * 32);

    //
    // Get the integer portion.
    //
    ulResult = ulXtal * ulPInt;

    //
    // Add first fractional bits portion(9:0).
    //
    ulResult += (ulXtal * ulF1) / 32;

    //
    // Add the second fractional bits portion(4:0).
    //
    ulResult += (ulXtal * ulF2) / 1024;

    //
    // Handle the N and Q dividers.
    //
    ulResult = ulResult /(ulN * ulQ);

    return(ulResult);
}

function sysCtlClockFreqSet(ulConfig, ulSysClock)
{
    var lTimeout, lVCOIdx;
    var lXtalIdx;
    var ulSysDiv, ulOsc;
    var bNewPLL;
    lXtalIdx = SYSCTL_XTALCFG_TO_INDEX(ulConfig);

    //
    // Determine which non-PLL source was selected.
    //
    if((ulConfig & 0x38) == SYSCTL_OSC_INT)
    {
        //
        // Use the nominal frequency for the PIOSC oscillator and set the
        // crystal select.
        //
        ulOsc = 16000000;

        //
        // Force the crystal index to the value for 16-MHz.
        //
        lXtalIdx = SYSCTL_XTALCFG_TO_INDEX(SYSCTL_XTAL_16MHZ);
    }
    else if((ulConfig & 0x38) == SYSCTL_OSC_INT30)
    {
        //
        // Use the nominal frequency for the low frequency oscillator.
        //
        ulOsc = 30000;
    }
    else if((ulConfig & 0x38) == (SYSCTL_OSC_EXT32 & 0x38))
    {
        //
        // Use the RTC frequency.
        //
        ulOsc = 32768;
    }
    else if((ulConfig & 0x38) == SYSCTL_OSC_MAIN)
    {
        //
        // Bounds check the source frequency for the main oscillator.  The is
        // because the PLL tables in the g_pulXTALtoVCO structure range from
        // 5MHz to 25MHz.
        //
        if((lXtalIdx > (SYSCTL_XTALCFG_TO_INDEX(SYSCTL_XTAL_25MHZ))) ||
           (lXtalIdx < (SYSCTL_XTALCFG_TO_INDEX(SYSCTL_XTAL_5MHZ))))
        {
            Boot.$logError("Crystal Frequency must be 5MHz <= x <= 25MHz.",
                               Boot, "xtal");
            return(0);
        }

        ulOsc = g_pulXtals[lXtalIdx];
    }
    else
    {
        //
        // This was an invalid request because no oscillator source was
        // indicated.
        //
        Boot.$logError("Only OSCSRC_MAIN, OSCSRC_INT, " +
                               "OSCSRC_INT30, and OSRSRC_EXT32 " +
                               "are valid settings for the Oscillator source",
                               Boot, "oscSrc");
	return (0);
    }

    //
    // Check if the running with the PLL enabled was requested.
    //
    if((ulConfig & SYSCTL_USE_OSC) == SYSCTL_USE_PLL)
    {
        //
        // ulConfig must be SYSCTL_OSC_MAIN or SYSCTL_OSC_INT.
        //
        if(((ulConfig & 0x38) != SYSCTL_OSC_MAIN) &&
           ((ulConfig & 0x38) != SYSCTL_OSC_INT))
        {
        //
        // This was an invalid request because no oscillator source was
        // indicated.
        //
	    Boot.$logError("When using the PLL, only OSCSRC_MAIN and OSCSRC_INT " +
                               "are valid settings for the Oscillator source",
                               Boot, "oscSrc");
            return(0);
        }

        //
        // Get the VCO index out of the ulConfig parameter.
        //
        lVCOIdx = (ulConfig >> 24) & 7;

        //
        // The table starts at 5-MHz so modify the index to match this.
        //
        lXtalIdx -= SYSCTL_XTALCFG_TO_INDEX(SYSCTL_XTAL_5MHZ);

        //
        // Set the M, N and Q values provided from the table and preserve
        // the power state of the main PLL.
        //
        (SYSCTL_PLLFREQ1) = g_pulXTALtoVCO[lVCOIdx][lXtalIdx][1];
        (SYSCTL_PLLFREQ0) = g_pulXTALtoVCO[lVCOIdx][lXtalIdx][0] |
                                     (HWREG(SYSCTL_PLLFREQ0) &
                                      SYSCTL_PLLFREQ0_PLLPWR);

        //
        // Calculate the System divider such that we get a frequency that is
        // the closest to the requested frequency without going over.
        //
        ulSysDiv = Math.floor((g_pulVCOFrequencies[lVCOIdx] + ulSysClock - 1) / ulSysClock);

        //
        // Calculate the actual system clock.
        //
        ulSysClock = GetFrequency(ulOsc);
        ulSysClock = ulSysClock / ulSysDiv;
    }
    else
    {
        //
        // Calculate the System divider based on the requested
        // frequency.
        //
        ulSysDiv = Math.floor(ulOsc / ulSysClock);

        //
        // If the system divisor is not already zero, subtract one to
        // set the value in the register which requires the value to
        // be n-1.
        //
        if(ulSysDiv != 0)
        {
            ulSysDiv -= 1;
        }

        //
        // Calculate the system clock.
        //
        ulSysClock = ulOsc / (ulSysDiv + 1);
    }

    return(ulSysClock);
}

/*
 *  ======== ldoConfig ========
 */
function ldoConfig(field, val)
{
    if (val = true) {
	this.$logError("LDO Configuration is NOT supported on TIVA devices.", this, "configureLdo");
    }
}
