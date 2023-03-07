/*!
 *  System settings -> Network Applications
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `The service pack is used to upgrade the network peripheral internal firmware. The service pack file is
provided by TI in the SDK package.\nTI recommends adding the service pack to the programming image; this action, however, is not
mandatory. If it is not programmed, the device uses its factory code.`;
let Utils  = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;


/*!
 * Common General configurable across all devices.
 */
let config = [
	{
		name: "USE_DEF_SP_1",
		displayName: "Use Service Pack from SDK",
		default: true,
		onChange: (inst, ui) => {
			ui.spFileInput.hidden 	= inst.USE_DEF_SP_1;
		}
	},
	{
		name        	: "spFileInput",
		displayName 	: "Service Pack File Name",
		default     	: "",
		hidden          : true,
		fileFilter		: Utils.wildcard()
	}
];

/*!
 * Validate this module's configuration
 *
 * @param inst       - instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation){

	if (!inst.USE_DEF_SP_1 && !inst.spFileInput)
	{
		logError(validation, inst, "spFileInput", "Service Pack file was not provided.")
	}
}


/* Common Network Applications instance components across all devices. */
let base = {
    displayName   : "Service Pack",
    description   : "Service Pack",
	moduleStatic: {
    config        : config,
    validate      : validate
	},
	longDescription: longDescription
};

 exports = base;