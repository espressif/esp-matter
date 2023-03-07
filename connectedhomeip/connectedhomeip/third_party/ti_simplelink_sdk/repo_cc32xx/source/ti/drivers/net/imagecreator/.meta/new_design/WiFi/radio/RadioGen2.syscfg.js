/*!
 *  System Settings-> Device -> Radio Settings of ImageCreator for SysConfig Gen2
 */
"use strict";
/* $super is used to call generic module's methods */
let $super;
/* Intro splash on GUI */
let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logInfo = Common.logInfo;
let ld = system.getScript("/ti/drivers/net/imagecreator/long_description.js");

/*!
 * Common General configures across all devices.
 */
let devSpecific = {
		displayName: "Radio Settings",
		moduleStatic: {
			config: [ 
						{
						displayName: "2.4GHz Radio Settings",
						description: "",
						config: [
									{
										name        : "STA_TX_PL",
										displayName : "Station Tx Power Level (dBm)",
										longDescription   : ` 0- Max Tx Power, 15- Min Tx Power`,
										default     : 0
									},
									{
										name        : "AP_TX_PL",
										displayName : "Access Point Tx Power Level (dBm)",
										longDescription   : ` 0- Max Tx Power, 15- Min Tx Power`,
										default     : 0
									},
									{
										name        : "PHY_CAL_MODE",
										displayName : "PHY Calibration Mode",
										default     : "0",
										longDescription   : ld.calibrationLongDescritpion,
										options     :[
											{name : "0" ,displayName : "Normal"	},
											{name : "1" ,displayName : "Trigger"	},
											{name : "2" ,displayName : "Onetime"	}
										]
									}
								],
								collapsed :false
							
						}
					],

			
		/* override device-specific templates */
		templates: {},

		/* override generic validation with ours */
		validate              : validate
		}
};
/*!
 * Validate this module's configuration
 *
 * @param inst       - instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation){

    if (inst.STA_TX_PL < 0) {
        logError(validation, inst, "STA_TX_PL", "Value must be positive, between 0-15.");
    }

    /* don't allow an unreasonably large ring buffer size */
    if (inst.STA_TX_PL > 15) {
        logError(validation, inst, "STA_TX_PL", "Value must be positive, between 0-15.");
    }
	
	if (inst.AP_TX_PL < 0) {
        logError(validation, inst, "AP_TX_PL", "Value must be positive, between 0-15.");
    }

    /* don't allow an unreasonably large ring buffer size */
    if (inst.AP_TX_PL > 15) {
        logError(validation, inst, "AP_TX_PL", "Value must be positive, between 0-15.");
    }
	
	if (inst.PHY_CAL_MODE == "2")
	{
		logInfo(validation, inst, "PHY_CAL_MODE" ,"For One-Time calibration mode, the calibration is made once on the first power/hibernate cycle after the device programming; One-time should be used only when the system power source is not able to handle the peak calibration current. For example Any OTA that contain RF changes in this mode will result in failure.");
	}
}


function extend(base)
{
    /* save base properties/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
	devSpecific.moduleStatic.config = base.moduleStatic.config.concat(devSpecific.moduleStatic.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    extend: extend
};
