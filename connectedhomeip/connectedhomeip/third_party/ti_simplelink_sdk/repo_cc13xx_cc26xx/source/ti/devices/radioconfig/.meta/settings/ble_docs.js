/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ble_docs.js ========
 *  Documentation for BLE settings.
 */

"use strict";

const bleDocs = [
    {
        configurable: "frequency",
        longDescription: "Sets the Bluetooth data/advertising channel. Channel 0-36 are data channels, "
            + "and 37-39 are advertising channels. The channels span from 2404 MHz to 2480 MHz, with a "
            + "channel width of 2 MHz. Note that the setting only has effect if the commands "
            + "that support it are selected.\n\n"
    },
    {
        configurable: "whitening",
        longDescription: "Selects if the header and the payload shall be scrambled with a data "
            + "whitening word in order to randomize the data from highly redundant patterns and "
            + "to minimize DC bias in the packet. Note that the setting only has effect if the commands "
            + "that support it are selected.\n\n"
            + "__Default__: True"
    },
    {
        configurable: "settingGroup",
        longDescription: "TI recommends to keep this option as the default value unless you have determined "
            + "you are not impacted by errata Radio_03. To do so, please reference your device's errata sheet. "
            + "In order to achieve best sensitivity, the radio can be configured to trade off selectivity. "
            + "Changing the RF performance focus to selectivity may lead to radio sensitivity degradation on "
            + "certain units for certain RX RSSI levels when using the Bluetooth LE 2M PHY.\n\n"
            + "Note: The CC2642R-Q1 and CC2662R-Q1 are not required to use this setting.\n\n"
            + "__Default__: Sensitivity"
    }
];

exports = {
    bleDocs: bleDocs
};
