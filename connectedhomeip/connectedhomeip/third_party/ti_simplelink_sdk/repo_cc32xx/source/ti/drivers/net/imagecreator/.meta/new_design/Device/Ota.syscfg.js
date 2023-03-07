/*!
 *  General Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `If the certificate catalog must be updated, OTA should be performed in two steps. First, the tar file should
include only the certificate catalog and its signature. In this case, the check box Certificate catalog OTA
bundle only should be marked. No other file can be included in the tar file once certificate catalog is
present. The second step includes a tar file with all other files that are required, such as MCU image,
service pack, and so forth.`;
let Utils = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;

/*!
 * Common General configurable across all devices.
 */
let config = [
        {
            name        : "CREATE_OTA",
            displayName : "Create OTA",
            description : "On successful builds, the OTA TAR file is located in your /Output folder",
            default     : false,
            onChange: (inst, ui) => {
                ui.ADD_CLST_OTA.hidden = !inst.CREATE_OTA;
                ui.OTA_SECURED.hidden = !inst.CREATE_OTA;
                ui.userOtaKeyFileInput.hidden = !inst.CREATE_OTA || !inst.OTA_SECURED;
            }
        },
        {
            name        : "ADD_CLST_OTA",
            displayName : "Certificate Catalog OTA Bundle Only",
            description : "Create OTA bundle with certificate catalog ONLY! Certificate catalog is used in the signature validation process and cannot be updated along with other files.",
            default     : false,
            hidden      : true
        },
        {
            name        : "OTA_SECURED",
            displayName : "Secured",
            default     : false,
            hidden      : true, 
            onChange: (inst, ui) => {
                ui.userOtaKeyFileInput.hidden = !inst.CREATE_OTA || !inst.OTA_SECURED;
            }
        },
        {
            name        : "userOtaKeyFileInput",
            displayName : "OTA Private Key File Name(pem/der):",
            default     : "",
            hidden      : true, 
            fileFilter	: ".pem,.der"
        },
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
function validate(inst, validation) {
	
	// if (inst.DevMA != '00:00:00:00:00:04')
	// {
	// 	alert(inst.MA);
	// 	Utils.validateMacAddr("DevMA", inst.DevMA, inst, validation);
		
	// }
		
	// Utils.validateMacAddr("MA", inst.MA, inst, validation);

	if (inst.CREATE_OTA && inst.OTA_SECURED && !inst.userOtaKeyFileInput)
	{
		logError(validation, inst, "userOtaKeyFileInput", "Security key file was not provided!!")
	}
}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "OTA Settings",
    description   : "General OTA Settings",
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

