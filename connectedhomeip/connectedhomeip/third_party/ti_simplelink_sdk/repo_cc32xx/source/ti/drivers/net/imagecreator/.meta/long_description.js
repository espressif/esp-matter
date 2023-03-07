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
 *  ======== long_description.js ========
 */

"use strict";

// Long description for the deviceName configuration parameter
const certStoreLongDescritpion = `The trusted root-certificate is a file provided by TI.\n
The store contains a list of known and trusted root CAs and a list of revoked certificates.\n
The list of the CAs supported by TI can be found in the CC3x20, CC3x35 SimpleLink™ Wi-Fi® Internet-on-a chip™ solution built-in security features Application Report.`

const calibrationLongDescritpion = `For low power applications, TI recommends choosing Triggered mode over One-Time calibration mode,
unless current peak limit is an absolute constraint.\n
Triggered mode does not issue calibrations unless absolutely necessary, or manually triggered.\n
Normal calibration mode is used to achieve the best RF performance, or when the environment of the device is prone to changes (temperature changes).`

const passwordLongDescritpion = `'Wi-Fi® password in AP role.For WEP, it is either 5 bytes or 13 bytes in ASCII format.For WPA it is 8-63 bytes'`

const pspollLongDescritpion = `PS Poll is a packet that gets sent from a Station to AP when awaking from sleep. The packet allows the Station to stay in Power Save mode while recieving packets. In higher throughput use cases, it can make sense to disable this feature to reduce the number of packets sent.`
const vendorLongDescription = `Enabling this option will allow using a certificate catalog that is\nsigned by the vendor root of trust.\nThe vendor root certificate needs to be stored in the OTP file.\nWhen using this option the  vendor should use his own\ncertificate catalog and the official certificate catalog from TI cannot be used.` 
const useotpLongDescription = `The OTP file is stored in the OTP section of the serial flash during\nthe first programming and cannot be changed afterwards.\nIt holds the vendor root certificate.\n\nNote: Once the OTP section is programmed, Secure files must be signed by the vendor chain of trust.` 
const developmodeLongDescription =`Production mode –\n The created image is programmable on any device.\n
For the device security, production mode exposes limited operations:\n
    - An online operation on the file system using the Image Creator is disabled.\n
    - Using JTAG (on the CC32xx device) is disabled.\n\n
Development mode – Requires the target device MAC address to program it.\n
The target device MAC address is set by the Image Creator setting window.\n 
This mode allows:\n
	- Browsing and modification of the device file system (see Section 6.13).\n
	- Using JTAG (CC32xx) is enabled.\n
	-The programming image file can be used to program only the device with the same 
    MAC addressas the one set into the image.`
const reg24domainLongDescription = `The term Back-Off Offset (BO) determines that the value configured in dB is the power offset from the
default TI design, limited to EVM and Mask constraints. The offset can be both positive and negative to
allow power increase. To change specific regulatory domain BO, check the relevant box and change the
offset according to wanted channel and rate group.
• 11b – only 11b rate
• H – High rates (MCS7, 54 Mbps, 48 Mbps)
• L – Low rates (all the rest)
Valid values are -6[dB] to +6[dB].`
const coex2LongDescription = `To allow maximum flexibility for every platform configuration, there are multiple choices for assigning the
coexistence and antenna selection interface on the device’s pins. These choices differ slightly based on
device family (CC3135 versus CC3235x).`
const coexLongDescription = `Coexistence modes:\n
    - Off – BLE coexistence is not used (default)\n
    - Single antenna – Choose this option when the platform\n 
     includes an RF switch to share a single antenna between\n
     the BLE and Wi-Fi.\n 
     This option requires the allocation of two GPIOs – \n
     one is input from the BLE as well as to the RF switch,\n
     the other is an output from the Wi-Fi to the RF switch.\n
    - Dual antenna – Choose this option when the platform\n
     has separate antennas for BLE and Wi-Fi. In this mode,\n
     BLE signals Wi-Fi when it requires the channel,\n
     and Wi-Fi stops ongoing transmissions during those times.\n
     This mode requires the usage of just one I/O.`
const      G5Description = `Device that supports 5GHz  \n
• Support 5GHz + 2.4GHz (CC3235x)\n
• Support 2.4GHz only (CC3230x)`
// Exports the long descriptions for each configurable
 exports = {
    certStoreLongDescritpion: certStoreLongDescritpion,
    calibrationLongDescritpion: calibrationLongDescritpion,
    pspollLongDescritpion: pspollLongDescritpion,
    passwordLongDescritpion:passwordLongDescritpion,
    vendorLongDescription:vendorLongDescription,
    useotpLongDescription:useotpLongDescription,
    developmodeLongDescription:developmodeLongDescription,
    coexLongDescription:coexLongDescription,
    coex2LongDescription:coex2LongDescription,
    G5Description:G5Description
};