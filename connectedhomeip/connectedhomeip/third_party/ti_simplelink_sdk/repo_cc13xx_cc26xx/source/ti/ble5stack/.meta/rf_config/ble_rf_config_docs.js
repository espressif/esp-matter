/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ble_rf_config_docs.js ========
 */

"use strict";

// Long description for the frontEndMode configuration parameter
const frontEndModeLongDescription = `The device supports multiple RF front end options\
to optimize performance or cost. Reference designs are available for multiple RF front\
end options to aid in decoupling and RF layout. In order to achieve optimal sensitivity,\
it is important to configure the BLE application with the correct RF front end setting\
used on the custom board. An incorrectly configured RF front end may result in\
substantially degraded RF performance such as the inability to maintain a connection\
with a peer device.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
cc13x2_26x2/custom-hardware-cc13x2_26x2.html#configuring-the-rf-front-end-for-custom\
-hardware).\n
Options |
--- |
RF_FE_DIFFERENTIAL |
RF_FE_SINGLE_ENDED_RFP |
RF_FE_SINGLE_ENDED_RFN |
RF_FE_ANT_DIVERSITY_RFP_FIRST |
RF_FE_ANT_DIVERSITY_RFN_FIRST |
RF_FE_SINGLE_ENDED_RFP_EXT_PINS |
RF_FE_SINGLE_ENDED_RFN_EXT_PINS |\n

__Default__: Differential\n
__Note__: The use of these values completely depends on how the PCB is laid 
out. Please see Device Package and Evaluation Module (EM) Board below.`

// Long description for the biasConfiguration configuration parameter
const biasConfigurationLongDescription = `The LNA can be biased by an internal or external\
inductor. Both types of biasing can be selected for single-ended and differential configuration.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
cc13x2_26x2/custom-hardware-cc13x2_26x2.html#configuring-the-rf-front-end-for-custom\
-hardware).\n
Options |
--- |
RF_FE_INT_BIAS |
RF_FE_EXT_BIAS |\n

__Default__: Internal BIAS\n
__Note__: The use of these values completely depends on how the PCB is laid
out. Please see Device Package and Evaluation Module (EM) Board below.`


 // Exports the long descriptions for each configurable
 exports = {
    frontEndModeLongDescription: frontEndModeLongDescription,
    biasConfigurationLongDescription: biasConfigurationLongDescription
};