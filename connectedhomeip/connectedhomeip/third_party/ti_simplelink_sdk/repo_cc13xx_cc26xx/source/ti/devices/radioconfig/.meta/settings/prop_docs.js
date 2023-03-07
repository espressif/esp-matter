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
 *  ======== prop_docs.js ========
 *  Documentation for Proprietary settings.
 */

"use strict";

const propDocs = [
    {
        configurable: "freqBand",
        longDescription: "The selected frequency band of the current PHY Type."
    },
    {
        configurable: "carrierFrequency",
        longDescription: "The frequency in MHz to which the synthesizer should be tuned. "
            + "Setting the frequency to a non-characterized value might give sub-optimal performance. \n\n"
            + " * __Range__ (433 MHz band): 430 to 510 MHz\n"
            + " * __Characterized range__ (433 MHz band): 431 to 527 MHz\n"
            + " * __Range__ (868 MHz band): 770 to 930 MHz\n"
            + " * __Characterized range__ (868 MHz band): 863 to 876 MHz and 902 to 930 MHz\n"
    },
    {
        configurable: "symbolRate",
        longDescription: "Sets the symbol rate to the desired speed. \n\n"
            + "__Range__: 4.3 to 200 kBaud"
    },
    {
        configurable: "deviation",
        longDescription: "Sets the deviation of the signal to be transmitted. \n\n"
            + "__Range__: 0 to 1000 kHz"
    },
    {
        configurable: "rxFilterBw",
        longDescription: "The RX Filter BW parameter gives the receiver filter bandwidth."
    },
    {
        configurable: "whitening",
        longDescription: "Radio operation is optimal when the data bits being transmitted are random and DC-free, "
            + "not only because this gives a smooth power distribution over the occupied RF bandwidth, but "
            + "also because random and DC-free data prevents the possibility of data dependencies in the receiver "
            + "control loops. Whitening is used to introduce some pseudo-random noise on the transmitted data bits\n\n "
            + "This configurable sets the whitening mode to use for the transmission. "
            + "The choices of the different types of whitening are:\n\n"
            + "* No whitening\n"
            + "* CC1101/CC2500 compatible\n"
            + "* PN9 Whitening\n"
            + "* No whitener, 32-bit IEEE 802.15.4g compatible CRC\n"
            + "* IEEE 802.15.4g compatible whitener and 32-bit CRC\n"
            + "* No whitener, dynamically IEEE 802.15.4g compatible 16/32-bit CRC\n"
            + "* Dynamically IEEE 802.15.4g compatible whitener and 16/32-bit CRC\n"
    },
    {
        configurable: ["preambleCount", "preambleMode"],
        longDescription: "The preamble in the proprietary PHY consists of an alternating bit pattern 0101... "
            + "and is used to determine the amplifier gain. Unlike in older TI devices, it is not needed "
            + "for bit synchronization. More info can be found in the [Packet Format Documentation]"
            + "(/proprietary-rf/proprietary-rf-users-guide/proprietary-rf/packet-format.html#preamble)"
    },
    {
        configurable: ["syncWord", "syncWordLength"],
        longDescription: "The sync word is used by the packet engine to detect the packet start. "
            + "For best performance, it must satisfy multiple criteria:\n\n"
            + "* Its auto-correlation must have one high peak and only flat side lobes.\n"
            + "* It must be long enough to be unique. 4 bytes are recommended.\n\n"
            + "Reducing the sync word length to less than 4 bytes and choosing a bad sync word "
            + "will likely have a negative impact on the packet error rate. In only rare cases is it recommended "
            + "to change this value. More info can be found in the [Packet Format Documentation]"
            + "(/proprietary-rf/proprietary-rf-users-guide/proprietary-rf/packet-format.html#sync-word)"
    },
    {
        configurable: "packetLengthConfig",
        longDescription: "Selects if the first byte of packet data should be the length of the packet "
            + "and if the packet should be fixed or variable length."
    },
    {
        configurable: "fixedPacketLength",
        longDescription: "The length of the packets if the packet length config is set to fixed. "
            + "Maximum length is decided by the CMD_PROP_TX(_ADV) command.\n\n"
            + "__Range__ (CMD_PROP_TX): 0 to 255\n\n"
            + "__Range__ (CMD_PROP_TX_ADV): 0 to 2047"
    },
    {
        configurable: "addressMode",
        longDescription: "The RX Address Mode selects if the device should filter on the address of a device. "
            + "The address is the first byte sent in the packet. "
    },
    {
        configurable: ["address0", "address1"],
        longDescription: "Sets the address to filter on.\n\n"
            + "__Range__: 0x00 to 0xFF"
    }
];

exports = {
    propDocs: propDocs
};
