/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== EDDSA.syscfg.js ========
 *  Edwards Curve Digital Signature Algorithm
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "EDDSA");

let config = [];

/*
 *  ======== base ========
 *  Define the base EDDSA properties and methods
 */
let base = {
    displayName         : "EDDSA",
    description         : "Edwards Curve Digital Signature Algorithm (EDDSA) Driver",
    longDescription     : `
The [__EDDSA driver__][1] provides APIs for generating
and verifying digital signatures.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]
[1]: /drivers/doxygen/html/_e_d_d_s_a_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_e_d_d_s_a_8h.html#ti_drivers_EDDSA_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_e_d_d_s_a_8h.html#ti_drivers_EDDSA_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#EDDSA_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName : "CONFIG_EDDSA_",
    config              : Common.addNameConfig(config, "/ti/drivers/EDDSA", "CONFIG_EDDSA_"),
    modules             : Common.autoForceModules(["Board", "Power"])
};

/* extend the base exports to include family-specific content */
let devEDDSA = system.getScript("/ti/drivers/eddsa/EDDSA" + family);
exports = devEDDSA.extend(base);
