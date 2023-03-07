/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== parameter_handler.js ========
 *  RF parameter calculation
 */

"use strict";

// Common utility functions
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");
const RfDesign = Common.getScript("rfdesign");

// Other dependencies
const DevInfo = Common.getScript("device_info.js");

// Constants
const BLE_ADV_CHAN_37 = 37;
const BLE_ADV_CHAN_38 = 38;
const BLE_ADV_CHAN_39 = 39;

const BLE_ADV_FREQ_37 = 2402;
const BLE_ADV_FREQ_38 = 2426;
const BLE_ADV_FREQ_39 = 2480;

const BLE_BASE_FREQ = 2404;
const BLE_MAX_FREQ = 2480;
const BLE_BASE_CHAN = 0;
const BLE_CHAN_INTV = 2;
const BLE_BASE_CHAN_RAW = 0x68;

const IEEE_15_4_BASE_FREQ = 2405;
const IEEE_15_4_MAX_FREQ = 2480;
const IEEE_15_4_BASE_CHAN = 11;
const IEEE_15_4_MAX_CHAN = 26;
const IEEE_15_4_CHAN_INTV = 5;

// Exported functions
exports = {
    getLoDivider: getLoDivider,
    bleFreqToChan: bleFreqToChan,
    bleFreqToChanRaw: bleFreqToChanRaw,
    bleChanToFreq: bleChanToFreq,
    ieee154FreqToChan: ieee154FreqToChan,
    ieee154ChanToFreq: ieee154ChanToFreq,
    validateFreqSymrateRxBW: validateFreqSymrateRxBW,
    validateTxPower: validateTxPower
};

/*!
 *  ======== bleChanToFreq ========
 * Function to convert from BLE channel to frequency (MHz)
 *
 * @param chan - BLE channel (0-39)
 */
function bleChanToFreq(chan) {
    let freq;

    switch (chan) {
    // Advertising channels
    case BLE_ADV_CHAN_37:
        freq = BLE_ADV_FREQ_37;
        break;
    case BLE_ADV_CHAN_38:
        freq = BLE_ADV_FREQ_38;
        break;
    case BLE_ADV_CHAN_39:
        freq = BLE_ADV_FREQ_39;
        break;
    default:
        if (chan >= BLE_BASE_CHAN && chan < BLE_ADV_CHAN_37) {
            // data channels
            freq = BLE_BASE_FREQ + chan * BLE_CHAN_INTV;

            if (freq >= BLE_ADV_FREQ_38) {
                // adjust for advertising channel 38 (2426)
                freq += BLE_CHAN_INTV;
            }
        }
        else {
            throw Error("Invalid BLE channel: " + chan);
        }
    }
    return freq;
}

/*!
 *  ======== bleFreqToChan ========
 * Function to convert from frequency (MHz) to BLE channel (0-39)
 *
 * @param freq - frequency
 */
function bleFreqToChan(freq) {
    let chan;

    switch (freq) {
    // Advertising channels
    case BLE_ADV_FREQ_37:
        chan = BLE_ADV_CHAN_37;
        break;
    case BLE_ADV_FREQ_38:
        chan = BLE_ADV_CHAN_38;
        break;
    case BLE_ADV_FREQ_39:
        chan = BLE_ADV_CHAN_39;
        break;
    default:
        // data channels
        if (freq >= BLE_BASE_FREQ && freq < BLE_ADV_FREQ_38) {
            chan = (freq - BLE_BASE_FREQ) / BLE_CHAN_INTV;
        }
        else if (freq < BLE_MAX_FREQ) {
            // adjust for advertising channel 38 (2426)
            chan = ((freq - BLE_BASE_FREQ)) / BLE_CHAN_INTV - 1;
        }
        else {
            throw Error("Invalid BLE frequency: " + freq);
        }
    }
    return chan;
}

/*!
 *  ======== bleFreqToChanRaw ========
 * Function to convert from frequency (MHz) to raw BLE channel value
 *
 * @param freq - frequency
 */
function bleFreqToChanRaw(freq) {
    return (freq - BLE_BASE_FREQ) + BLE_BASE_CHAN_RAW;
}

/*!
 *  ======== ieee154ChanToFreq ========
 * Function to convert from IEEE 802.15.4 channel to frequency (MHz)
 *
 * @param chan - IEEE 802.15.4 channel (11-26)
 *
 */
function ieee154ChanToFreq(chan) {
    if (chan >= IEEE_15_4_BASE_CHAN && chan <= IEEE_15_4_MAX_CHAN) {
        return IEEE_15_4_BASE_FREQ + (chan - IEEE_15_4_BASE_CHAN) * IEEE_15_4_CHAN_INTV;
    }

    throw Error("Invalid IEEE 802.15.4 channel: " + chan);
}

/*!
 *  ======== ieee154FreqToChan ========
 * Function to convert from frequency (MHz) to IEEE 802.15.4 channel (11-16)
 *
 * @param freq - frequency
 */
function ieee154FreqToChan(freq) {
    if (freq >= IEEE_15_4_BASE_FREQ && freq <= IEEE_15_4_MAX_FREQ) {
        if ((freq % IEEE_15_4_CHAN_INTV) === 0) {
            return IEEE_15_4_BASE_CHAN + (freq - IEEE_15_4_BASE_FREQ) / IEEE_15_4_CHAN_INTV;
        }
    }
    throw Error("Invalid IEEE 802.15.4 frequency: " + freq);
}

/*!
 *  ======== validateFreqSymrateRxBW ========
 * Function to verify that the relationship between carrier frequency,
 * symbol rate and rxFilter bandwidth is correct
 *
 * @param carrierFrequency - The selected carrier frequency [MHz]
 * @param symbolRate - The number of symbols per time unit [kBaud]
 * @param rxFilterBw - The bandwidth of the RX Filter [kHz]
 */
function validateFreqSymrateRxBW(carrierFrequency, symbolRate, rxFilterBw) {
    const decFactor = calculateDecimationFactor(rxFilterBw);
    if (decFactor === -1) {
        return {
            inst: "rxFilterBw",
            message: "Error with rxFilterBW table"
        };
    }

    const fvcoFrequency = calculateVcoFrequency(carrierFrequency);
    const maxSymbolRate = calculateSymbolRate(fvcoFrequency, decFactor, 1, 0);
    const minSymbolRate = calculateSymbolRate(fvcoFrequency, decFactor, 4, 2);
    const curSymbolRate = symbolRate;

    if (curSymbolRate < minSymbolRate) {
        return {
            inst: "symbolRate",
            message: "Incompatible values for Frequency, Symbol Rate and RX Filter Bandwidth."
                + " To use the higher Symbol Rate, the RX Filter BW must be increased"
        };
    }
    else if (curSymbolRate > maxSymbolRate) {
        return {
            inst: "symbolRate",
            message: "Incompatible values for Frequency, Symbol Rate and RX Filter Bandwidth."
                + " The symbol rate must be decreased"
        };
    }
    return null;
}

/*!
 *  ======== validateTxPower ========
 * Function to verify if the value of CCFG_FORCE_VDDR_HH in the ccfg.c
 * file needs to be high or low
 *
 * @param txPower - The current selected TX Power
 * @param freq - The selected carrier frequency [MHz]
 * @param highPA - True if using high PA
 */
function validateTxPower(txPower, freq, highPA) {
    const paTable = RfDesign.getPaTable(freq, highPA);
    let vddr = false;
    _.forEach(paTable, (values) => {
        if (values._text === txPower) {
            if (_.has(values, "Option") && values.Option._name === "vddr") {
                vddr = (values.Option.$ === "HIGH");
                return false;
            }
        }
        return true;
    });
    return vddr;
}

/*
 * Private functions
 */

/*!
 *  ======== calculateDecimationFactor ========
 *  Returns the decimation factor based on the Rx Filter Bandwidth
 *
 *  @param rxBw - RX Filter Bandwidth
 */
function calculateDecimationFactor(rxBw) {
    const chfLookup = [];
    let decFactor = -1;

    // Get the register value from the RX Filter BW
    const rxBwReg = rxBwToRegValue(rxBw);
    if (rxBwReg === null) {
        return -1;
    }

    // Channel filter lookup table
    // Table taken from SmartRF Studio
    // ["Channel filter bandwidth", "Base band decimation stage1", "Base band Decimation stage 2"]
    chfLookup[64] = [3, 4, 5];
    chfLookup[65] = [1, 4, 5];
    chfLookup[66] = [2, 4, 5];
    chfLookup[67] = [0, 4, 5];
    chfLookup[68] = [3, 3, 5];
    chfLookup[69] = [1, 3, 5];
    chfLookup[70] = [2, 3, 5];
    chfLookup[71] = [0, 3, 5];
    chfLookup[72] = [3, 3, 4];
    chfLookup[73] = [1, 3, 4];
    chfLookup[74] = [2, 3, 4];
    chfLookup[75] = [0, 3, 4];
    chfLookup[76] = [3, 2, 4];
    chfLookup[77] = [1, 2, 4];
    chfLookup[78] = [2, 2, 4];
    chfLookup[79] = [0, 2, 4];
    chfLookup[80] = [3, 2, 3];
    chfLookup[81] = [1, 2, 3];
    chfLookup[82] = [2, 2, 3];
    chfLookup[83] = [0, 2, 3];
    chfLookup[84] = [3, 1, 3];
    chfLookup[85] = [1, 1, 3];
    chfLookup[86] = [2, 1, 3];
    chfLookup[87] = [0, 1, 3];
    chfLookup[88] = [3, 0, 3];
    chfLookup[89] = [1, 0, 3];
    chfLookup[90] = [2, 0, 3];
    chfLookup[91] = [0, 0, 3];
    chfLookup[92] = [3, 0, 2];
    chfLookup[93] = [1, 0, 2];
    chfLookup[94] = [2, 0, 2];
    chfLookup[95] = [0, 0, 2];
    chfLookup[96] = [3, 0, 1];
    chfLookup[97] = [1, 0, 1];
    chfLookup[98] = [2, 0, 1];
    chfLookup[99] = [0, 0, 1];
    chfLookup[100] = [3, 0, 0];
    chfLookup[101] = [1, 0, 0];
    chfLookup[102] = [2, 0, 0];
    chfLookup[103] = [0, 0, 0];

    const rxBwRegInt = parseInt(rxBwReg);
    if (rxBwRegInt in chfLookup) {
        decFactor = (2 ** (chfLookup[rxBwRegInt][1] + chfLookup[rxBwRegInt][2]));
    }

    return decFactor;
}

/*!
 *  ======== rxBwToRegValue ========
 *  Returns the register value from the selected RX Filter Bandwidth
 *
 *  @param RxBw - Bandwidth of the RX Filter
 */
function rxBwToRegValue(RxBw) {
    // Conversion table stored in SysConfig Params
    const devCfg = DevInfo.getConfiguration(Common.PHY_PROP);
    const paramFile = Common.flattenConfigs(devCfg.configs);
    // Find the parameter object in the param File
    const parameterObject = _.find(paramFile, ["name", "rxFilterBw"]);
    // Find the RxBwValue in the parameterObjects options
    const RxBwValue = _.find(parameterObject.options, ["name", RxBw]);

    if (typeof RxBwValue !== "undefined") {
        return RxBwValue.key;
    }
    return null;
}

/*!
 *  ======== calculateVcoFrequency ========
 *  The VCO frequency will be calculated deepening on the carrier frequency and
 *  the LO divider.
 *
 *  @param frequency - carrier frequency
 */
function calculateVcoFrequency(frequency) {
    let factor = getLoDivider(frequency);
    if (factor === 0) {
        factor = 2;
    }

    return frequency * 1e6 * factor;
}

/*!
 *  ======== getLoDivider ========
 *  Calculate the CMD_PROP_RADIO_DIV_SETUP.loDivider value for a given frequency
 *
 *  @param frequency - carrier frequency
 */
function getLoDivider(frequency) {
    let loDiv;

    if (frequency >= 2158) {
        loDiv = 0;
    }
    else if (frequency >= 863) {
        loDiv = 5;
    }
    else if (frequency >= 719) {
        loDiv = 6;
    }
    else if (frequency >= 431) {
        loDiv = 10;
    }
    else if (frequency >= 348) {
        loDiv = 12;
    }
    else if (frequency >= 288) {
        loDiv = 15;
    }
    else {
        // 169 MHz
        loDiv = 30;
    }
    return loDiv;
}

/*!
 *  ======== calculateSymbolRate ========
 *  Calculate the Symbol Rate
 *
 *  @param fVcoFrequency - VCO frequency
 *  @param decFactor - The decimation factor
 *  @param frac - The Fractional down sampler. A value between 1 and 4. The lowest value
 *                will give the maximum Symbol Rate. Ref. cc26xx_modem_spec.doc chapter 5.4.1.10
 *  @param pdif - The pdif decimation factor
 */
function calculateSymbolRate(fVcoFrequency, decFactor, frac, pdif) {
    // Phase Differentiation Factor
    const pDifFactor = 1024 / (2 ** pdif);
    // The p factor used in demodulation to calculate the final sampling frequency.
    const p = ((pDifFactor * fVcoFrequency) / 2000000.0) / frac;
    const rateWord = (p / 27) / decFactor;

    return (rateWord / (2 ** 20)) * 24000;
}
