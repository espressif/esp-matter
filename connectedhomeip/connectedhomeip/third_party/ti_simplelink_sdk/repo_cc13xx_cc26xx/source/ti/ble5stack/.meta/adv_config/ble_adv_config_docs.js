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
 *  ======== ble_adv_config_docs.js ========
 */

"use strict";

// Long description for the disableDisplayModule configuration parameter
const disableDisplayModuleLongDescription = `If This option is active, all display statements\
are removed and no display operations will take place.\n
For more information, refer to Display.h and the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/stack-configuration-cc13x2_26x2.html#run-time-bluetooth-low-energy-protocol-stack\
-configuration).\n
__Default__: False (unchecked)\n`

// Long description for the powerMamagement configuration parameter
const powerMamagementLongDescription = `Power management is enabled when defined, \
and disabled when not defined.\n
For more information, refer to and the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/stack-configuration-cc13x2_26x2.html#run-time-bluetooth-low-energy-protocol-stack-\
configuration).\n
__Default__: True (checked)\n`

// Long description for the halAssert configuration parameter
const halAssertLongDescription = `Defining this assert will forward \
ble5stack asserts to the application.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/stack-configuration-cc13x2_26x2.html#run-time-bluetooth-low-energy-protocol-stack-\
configuration).\n
__Default__: False (unchecked)\n`

// Long description for the tbmActiveItemsOnly configuration parameter
const tbmActiveItemsOnlyLongDescription = `When using the Two Button Menu, Only \
active items will be displayed.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/stack-configuration-cc13x2_26x2.html#run-time-bluetooth-low-energy-protocol-stack-\
configuration).\n
__Default__: True (checked)\n`

// Long description for the stackLibrary configuration parameter
const stackLibraryLongDescription = `Build/Use the build configuration where \
the stack is built as a library.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/stack-configuration-cc13x2_26x2.html#run-time-bluetooth-low-energy-protocol-stack-\
configuration).\n
__Default__: True (checked)\n`

// Long description for the dontTransmitNewRpa configuration parameter
const dontTransmitNewRpaLongDescription = `According to E10336 errata, when \
using connectable directed extended advertising, it is recommended that the \
advertiser will use a different initiator RPA in the connect response than \
the one that was used in the connect request.\n
For backwards compatibility the DONT_TRANSMIT_NEW_RPA flag was added. \n
When DONT_TRANSMIT_NEW_RPA flag is enabled, this recommendation will not take \
affect and the advertiser will use the same RPA. \n
__Default__: False (unchecked)\n`

// Long description for the extendedStackSettings configuration parameter
const extendedStackSettingsLongDescription = `
In our BLE5-Stack, there are 2 options for LL scheduler to form anchor points: \n

Options | Description
--- | ---
Default | The anchor points are formed randomly with no restriction on timing.\
This is the case for all our out of box examples
Guard Time | The anchor points are formed randomly with minimum 5ms guard time\n

__Note__: \n
We can achieve 32 connections running out of box simple_central project. \n
To enable this functionality choose Guard Time.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-common/link-layer-cc13x2_26x2.html#connections-design).\n`

// Long description for the maxNumEntIcall configuration parameter
const maxNumEntIcallLongDescription = `Defines maximum number of \
entities that use ICall, including service entities and application entities. \
Modify only if adding a new TI-RTOS task that uses ICall services.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/the-application.html#icall).\n
__Default__: 6\n`

// Long description for the maxNumIcallEnabledTasks configuration parameter
const maxNumIcallEnabledTasksLongDescription = `Defines the number of ICall aware \
tasks. Modify only if adding a new TI-RTOS task that uses ICall services.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/the-application.html#icall).\n
__Default__: 3\n`

// Long description for the flowControl configuration parameter
const flowControlLongDescription = `Defines whether the NPI will enable packet flow control.\n
__Note__: The only way to prevent RxBuf overflow is to enable NPI_FLOW_CTRL. If the buffer \
has overflowed there is no way to safely recover. All received bytes can be packet fragments \
so if a packet fragment is lost the frame parser behavior becomes undefined.\n
If NPI_FLOW_CTRL is not enabled then  the slave cannot control the master's transfer rate. \
With NPI_FLOW_CTRL the slave has SRDY to use as a software flow control mechanism.\n
When using NPI_FLOW_CTRL make sure to increase NPI_TL_BUF_SIZE to suit the NPI frame length \
that is expected to be received.\n
__Default__: 0\n`

// Long description for the ptm configuration parameter
const ptmLongDescription = `Production Test Mode (PTM), allows for an embedded\
 software application to support direct test mode without exposing the HCI to the UART\
 pins under normal operation.\n
 __Note__: The pins used for PTM can also be used for an application UART interface.\
 In this case, it is necessary to ensure that the other device that is connected to the\
 UART interface does not run at the same time that DTM is being exercised. If the device\
 powers up and goes into PTM mode (by a GPIO being held high or low or some other stimulus),\
 the UART will then be used for DTM commands. If the device powers up normally and does\
 not go into PTM mode, then the UART can be initialized by the application and used to\
 communicate with the other device. DTM commands can also be called by the embedded BLE\
 application.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/creating-a-custom-bluetooth-low-energy-application-cc13x2_26x2.html\
#sec-using-production-test-mode).\n
__Default__: False (unchecked)\n`

// Long description for the peerConnParamUpdateRejectInd configuration parameter
const peerConnParamUpdateRejectIndLongDescription = `When enabling this parameter\
 the application will be notified on any incoming connection parameter\
 update request that has been received from the peer and was rejected by\
 the Link Layer. \n
__Default__: False (unchecked)\n`

 // Exports the long descriptions for each configurable
 exports = {
    disableDisplayModuleLongDescription: disableDisplayModuleLongDescription,
    powerMamagementLongDescription: powerMamagementLongDescription,
    halAssertLongDescription: halAssertLongDescription,
    tbmActiveItemsOnlyLongDescription: tbmActiveItemsOnlyLongDescription,
    stackLibraryLongDescription: stackLibraryLongDescription,
    dontTransmitNewRpaLongDescription: dontTransmitNewRpaLongDescription,
    extendedStackSettingsLongDescription: extendedStackSettingsLongDescription,
    maxNumEntIcallLongDescription: maxNumEntIcallLongDescription,
    maxNumIcallEnabledTasksLongDescription: maxNumIcallEnabledTasksLongDescription,
    ptmLongDescription: ptmLongDescription,
    flowControlLongDescription: flowControlLongDescription,
    peerConnParamUpdateRejectIndLongDescription: peerConnParamUpdateRejectIndLongDescription
};