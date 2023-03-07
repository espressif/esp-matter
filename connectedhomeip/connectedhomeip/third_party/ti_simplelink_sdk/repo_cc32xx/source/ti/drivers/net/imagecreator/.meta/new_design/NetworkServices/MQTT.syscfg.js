/*!
 *  General Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = "MQTT";
let Utils = system.getScript("/ti/drivers/net/imagecreator/Utils.js");



/*!
 * Common General configurable across all devices.
 */
let config = [
	{
				displayName: "MQTT Broker",
				config: [
			
						],
			collapsed:false
	}

];

/*!
 * Validate this module's configuration
 *
 * issue: what if there are no instances but UART is used???
 *
 * @param inst       - UART instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation) {}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "MQTT Broker",
    description   : "Network Service - MQTT Broker",
	moduleStatic: {
    config        : config,
    validate      : validate
	},
    longDescription: longDescription
};

/*
 * Compose a device specific UART from this common UART definition and
 * export the device specific UART
 
let devId = system.deviceData.deviceId;
let devGen = system.getScript("/ti/drivers/net/imagecreator/general/General" + devId);
exports     = devGen.extend(base);
*/
exports = base;
