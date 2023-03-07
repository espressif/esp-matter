/*!
 *  Device Radio Settings Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `Configure the RF settings for PHY radio, otherwise the device will use default values from ROM.`;


/*!
 * Common General configurable across all devices.
 */
let config = [];

/*!
 * Validate this module's configuration
 * @param inst       - Radio config instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
    
}
/*
 * Compose a device specific radio config
 */
let devId = system.deviceData.deviceId;
let gen = "Gen2";    
let templates = {}
if ((devId == "CC3235S") || (devId == "CC3235SF") || (devId == "CC3135R"))
{
	gen = "Gen3";

	templates = {
		"/ti/drivers/net/imagecreator/summary6.xdt": {},
		"/ti/drivers/net/imagecreator/summary7.xdt": {}
	}
} else if ((devId == "CC3230S") || (devId == "CC3230SF") || (devId == "CC3130R"))
{
	gen = "Gen32";
}
/* Common radio config instance components across all devices. */
let base = {
    displayName   : "Radio",
    description   : "System -> Device -> Radio Settings",
	moduleStatic: {
		config        : config,
		validate      : validate
	},
	templates : templates,
    longDescription: longDescription,
    defaultInstanceName: "Radio"
};



let devGen = system.getScript("radio/Radio" + gen);
exports     = devGen.extend(base);

