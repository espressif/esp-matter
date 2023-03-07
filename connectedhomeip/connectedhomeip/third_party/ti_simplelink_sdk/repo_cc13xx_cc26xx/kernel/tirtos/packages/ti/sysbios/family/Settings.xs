/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 */
/*
 *  ======== Settings.xs ========
 */

var family = {
    "ti.targets.C28_large" :                    "c28",
    "ti.targets.C28_float" :                    "c28",
    "ti.targets.elf.C64P" :                     "c64p",
    "ti.targets.elf.C66" :                      "c64p",
    "ti.targets.elf.C66_big_endian" :           "c64p",
    "ti.targets.elf.C674" :                     "c64p",
    "ti.targets.elf.C67P" :                     "c67p",
    "ti.targets.elf.nda.C71" :                  "c7x",
    "ti.targets.elf.C28_float" :                "c28",
    "ti.targets.arm.elf.Arm9" :                 "arm",
    "ti.targets.arm.elf.A8Fnv" :                "arm",
    "ti.targets.arm.elf.M3" :                   "arm",
    "ti.targets.arm.elf.M4" :                   "arm",
    "ti.targets.arm.elf.M4F" :                  "arm",
    "ti.targets.arm.elf.R5F" :                  "arm",
    "ti.targets.arm.elf.R4F" :                  "arm",
    "ti.targets.arm.elf.R4Ft" :                 "arm",
    "ti.targets.arm.clang.M33" :                "arm",
    "ti.targets.arm.clang.M33F" :               "arm",
    "ti.targets.arm.clang.M3" :                 "arm",
    "ti.targets.arm.clang.M4" :                 "arm",
    "ti.targets.arm.clang.M4F" :                "arm",
    "gnu.targets.arm.M3" :                      "arm",
    "gnu.targets.arm.M4" :                      "arm",
    "gnu.targets.arm.M4F" :                     "arm",
    "gnu.targets.arm.A8F" :                     "arm",
    "gnu.targets.arm.A9F" :                     "arm",
    "gnu.targets.arm.A15F" :                    "arm",
    "gnu.targets.arm.A53F" :                    "arm",
    "iar.targets.arm.M3" :                      "arm",
    "iar.targets.arm.M4" :                      "arm",
    "iar.targets.arm.M4F" :                     "arm",
    "ti.targets.arp32.elf.ARP32" :              "arp32",
    "ti.targets.arp32.elf.ARP32_far" :          "arp32",
};

var familySettings = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /*
     * Check if target not supported anymore. If not, then raise an
     * error and abort build.
     */
    unsupportedTargetCheck(this);

    this.familyName = getFamilyName();
    
    if (this.familyName === undefined) {
        var s = new Array(family.length);
        var i = 0;

        for (var t in family) {
            s[i++] = t;
        }

        s.sort(); /* make it a little prettier */

        var n = "";

        for (i=1; i < s.length; i++) {
            n = n + "\n" + s[i];
        }
        n = n + "\n" + s[0]; /* tack Microsoft to the end. */

        throw Error("Target not supported: " + Program.build.target.$name
            + "\nPlease choose one of the following targets supported by BIOS:"
            + n);
    }

    /* Point to family Settings Module */
    familySettings = xdc.module("ti.sysbios.family." 
                                + this.familyName + ".Settings");

    /* point to family-specific GUI configuration page */
    this.familyConfigPage = getFamilySettingsXml();
 
    this.bootModule = getDefaultBootModule(); 
    this.mmuModule = getDefaultMmuModule(); 
    this.timerDelegate = getDefaultTimerDelegate();
    this.clockTimerDelegate = getDefaultClockTimerDelegate();
    this.secondsDelegate = getDefaultSecondsDelegate();
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    this.cacheDelegate = getDefaultCacheDelegate();
    this.hwiDelegate = getDefaultHwiDelegate();
    this.timestampDelegate = getDefaultTimestampDelegate();
}

/*
 *  ======== getFamilyName ========
 */
function getFamilyName()
{
    if (Program != null && ("build" in Program)) {
        return (family[Program.build.target.$name]);
    }
    return (null);
}

/*
 *  ======== getDefaultCoreDelegate ========
 */
function getDefaultCoreDelegate()
{
    return (familySettings.getDefaultCoreDelegate());
}

/*
 *  ======== getDefaultHwiDelegate ========
 */
function getDefaultHwiDelegate()
{
    return (familySettings.getDefaultHwiDelegate());
}

/*
 *  ======== getDefaultTimerDelegate ========
 */
function getDefaultTimerDelegate()
{
    return (familySettings.getDefaultTimerDelegate());
}

/*
 *  ======== getDefaultClockTimerDelegate ========
 */
function getDefaultClockTimerDelegate()
{
    return (familySettings.getDefaultClockTimerDelegate());
}

/*
 *  ======== getDefaultTimerSupportDelegate ========
 */
function getDefaultTimerSupportDelegate()
{
    return (familySettings.getDefaultTimerSupportDelegate());
}

/*
 *  ======== getDefaultTimestampDelegate ========
 */
function getDefaultTimestampDelegate()
{
    return (familySettings.getDefaultTimestampDelegate());
}

/*
 *  ======== getDefaultTaskSupportDelegate ========
 */
function getDefaultTaskSupportDelegate()
{
    return (familySettings.getDefaultTaskSupportDelegate());
}

/*
 *  ======== getDefaultIntrinsicsSupportDelegate ========
 */
function getDefaultIntrinsicsSupportDelegate()
{
    return (familySettings.getDefaultIntrinsicsSupportDelegate());
}

/*
 *  ======== getDefaultCacheDelegate ========
 */
function getDefaultCacheDelegate()
{
    return (familySettings.getDefaultCacheDelegate());
}

/*
 *  ======== getDefaultPowerDelegate ========
 */
function getDefaultPowerDelegate()
{
    return (familySettings.getDefaultPowerDelegate());
}

/*
 *  ======== getDefaultSecondsDelegate ========
 */
function getDefaultSecondsDelegate()
{
    return (familySettings.getDefaultSecondsDelegate());
}

/*
 *  ======== getDefaultBootModule ========
 */
function getDefaultBootModule()
{
    return (familySettings.getDefaultBootModule());
}

/*
 *  ======== getDefaultMmmModule ========
 */
function getDefaultMmuModule()
{
    return (familySettings.getDefaultMmuModule());
}

/*
 *  ======== getDefaultClockTickPeriod ========
 */
function getDefaultClockTickPeriod()
{
    return (familySettings.getDefaultClockTickPeriod());
}

/*
 *  ======== getFamilySettingsXml ========
 */
function getFamilySettingsXml()
{
    return (familySettings.getFamilySettingsXml());
}

/*
 *  ======== unsupportedTargetCheck ========
 */
function unsupportedTargetCheck(mod)
{
    if (Program.build.target.$name == "ti.targets.arm.elf.A8F") {
        mod.$logError(Program.build.target.$name + " is no longer supported. " +
            "Please use ti.targets.arm.elf.A8Fnv instead.", mod);
        throw Error();
    }
    else if ((Program.build.target.name == "C64T") ||
             (Program.build.target.$name == "ti.targets.C64P") ||
             (Program.build.target.name == "C64P_big_endian")) {
        mod.$logError(Program.build.target.$name + " is no longer supported. " +
            "Please use SYS/BIOS 6.42.03 or older.", mod);
        throw Error();
    }
    else if (Program.build.target.$name == "ti.targets.C674") {
        mod.$logError(Program.build.target.$name + " is no longer supported. " +
            "Please use ti.targets.elf.C674 instead.", mod);
        throw Error();
    }
}
