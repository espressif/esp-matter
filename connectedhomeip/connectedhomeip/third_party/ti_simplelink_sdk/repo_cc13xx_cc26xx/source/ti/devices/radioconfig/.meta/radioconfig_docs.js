/*
 * Copyright (c) 2019-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 * ======== radioconfig_docs.js ========
 * Documentation for the RadioConfig module
 */

"use strict";

/* eslint-disable max-len */

const customDescription = `
The [__Custom RF module__][1] allows the user full flexibility 
in choosing what PHY settings to include in the application. It is possible 
to combine settings from all PHY groups (BLE, IEEE 802.15.4 and Proprietary). The module 
provides a set of typical (PHY) settings that are identical to SmartRF Studio, and also 
offers a proprietary custom setting for each of the frequency bands (169 MHz, 433 MHz, 868 MHz, 2.4 GHz).

* [Usage Synopsis][1]
* [Examples][2]

[1]: /proprietary-rf/proprietary-rf-users-guide/sysconfig/proprietary-rf-driver.html#adding-rf-settings "Adding RF Settings"
[2]: /proprietary-rf/proprietary-rf-users-guide/proprietary-rf-guide/examples-cc13xx_cc26xx.html#rf-driver-examples "Radio Configuration Examples"
`;

const rfDesignDescription = `
The **RF Design module** allows the user to choose which radio features to
include in the design: **frequency bands**, use of **High-Power Amplifier** (for the CC1352P and CC2652P devices), 
**RF front-end** (differential or single-ended), and **bias**. All designs are based on existing TI reference designs.

Supported frequency bands are 169 MHz, 433 MHz, 868 MHz and 2.4 GHz. The devices use Power Amplifiers with 
maximum output of 5 dBm, 13 dBm, 20 dBm, and 10 dBm.

Only certain combinations of frequency bands and power amplifiers are possible. 
13 dBm is only used with the Sub-1 GHz frequency bands whereas 5 dBm is exclusively used by the 2.4 GHz band.
The 20 dBm PA can be combined with one and only one of the frequency bands. It is therefore not possible to use 
the High-Power Amplifier with Sub-1 GHz and 2.4 GHz simultaneously.

* [Usage Synopsis][1]

[1]: /proprietary-rf/proprietary-rf-users-guide/sysconfig/proprietary-rf-driver.html#rfdesign "RF Design"
`;

const txPowerDescription = `
// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient)
// RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldoTrim)
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.
`;

exports = {
    customDescription: customDescription,
    rfDesignDescription: rfDesignDescription,
    txPowerDescription: txPowerDescription
};
