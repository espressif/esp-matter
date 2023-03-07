/*!
 *  MCUImage Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `Add the host application file (MCU Image) for CC32xx devices.`;

let Utils  = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;
const isStandAloneGUI = system.isStandAloneGUI();

let flash = (system.deviceData.deviceId == "CC3220SF") || (system.deviceData.deviceId == "CC3235SF") ||  (system.deviceData.deviceId == "CC3230SF");


let config = [   
                {   name        : "External", /*Valid only for CCS mode*/
					displayName : "Use Project MCU Image",
					default     : true,
					hidden      : isStandAloneGUI, 
                    onChange: (inst, ui) => {
						if(isStandAloneGUI)
						{
							ui.mcuFileInput.hidden = false;
						} else {
							ui.mcuFileInput.hidden = inst.External;
						}
                    }
				}, 
				{   name        : "mcuFileInput",
					displayName : "MCU Image",
					hidden      : !isStandAloneGUI, /*Should be shown always in standalone mode*/
					default     : "",
					fileFilter	: Utils.wildcard()
				},
				{   name        : "Filename",
					displayName : "File System Name",
					default     : flash ? "/sys/mcuflashimg.bin" : "/sys/mcuimg.bin",
					readOnly    : true
				}, 
                {
					name        : "ProjectPath",
					displayName : "File Name",
					default     : "",
					hidden      : true
				}, 
				{
					name        : "fileSizeCfg",
					displayName : "Max File Size",
					default     : flash ? 1024*1024 : 256*1024
				}, 
				{
					name        : "Failsafe",
					displayName : "Failsafe",
					longDescription:`MCU should be failsafe for working properly. User can uncheck it by his own responsibility.`,
					default     : true
				},
				{   name        : "Secure",
					displayName : "Secure",
					longDescription:`File is encrypted on the serial flash.`,
					default     : true,
					onChange: (inst, ui) => {
                        ui.Static.readOnly = !inst.Secure;
                        ui.Vendor.readOnly = !inst.Secure;
                        ui.KeySelect.readOnly = !inst.Secure;
                        ui.mcuKey.readOnly = !inst.Secure;
						ui.mcuCertificate.readOnly = !inst.Secure;
						ui.KeySelect.readOnly = isStandAloneGUI;
                    }
				},	
				{
					name        : "Static",
					displayName : "Static",
					longDescription:`Relevant only for secure files. Tokens are not replaced each time a file is open for write.`,
					default     : false
				},	
				{   name        : "Vendor",
					displayName : "Vendor",
					longDescription:`Relevant only for secure files. The master token is set by the vendor.`,
					default     : false,
					onChange: (inst, ui) => {
                        ui.FileToken.readOnly = !inst.Vendor
                    }
				},				
				{
					name        : "FileToken",
					displayName : "File Token",
					longDescription:`Relevant only when using the vendor flag. Token for secured file.`,
					default     : 0,
					readOnly    : true
				}, 
				{
					name        : "PublicWrite",
					displayName : "Public Write",
					longDescription:`Relevant only for secure files. The file can be written without a token.`,
					default     : true,
				},	
				{
					name        : "PublicRead",
					displayName : "Public Read",
					longDescription:`Relevant only for secure files. The file can be read without a token.`,
					default     : false
				},	
				{   name        : "NoSecurityTest",
					displayName : "No Security Test",
					default     : false,
					hidden      : true,
					onChange: (inst, ui) => {
                        ui.mcuKey.readOnly = inst.NoSecurityTest;
                        ui.mcuCertificate.readOnly = inst.NoSecurityTest;
                    }
				},	
				{
					name        : "KeySelect",
					displayName : "Security Sign Mode",
					longDescription : `Choose a way how to sign secured MCU file. 
					- Simple     : Automatically signed by the dummy root certificate of SDK (dummy-root-ca-cert), copied to userFiles root folder.
					- Advanced   : Set certificate and private key file manually. Certificate should be added to userFiles`,
					default     : isStandAloneGUI ?  'ADVANCED' : 'SIMPLE', /*simple mode is useless for sa*/
					//readOnly    : isStandAloneGUI,
					hidden      : isStandAloneGUI,
					options     : [
						{ name:   'SIMPLE'        , displayName: "Simple" 	},
						{ name:   'ADVANCED'      , displayName: "Advanced" }
					],
					onChange: (inst, ui) => {
						if(isStandAloneGUI)
						{
							ui.mcuKey.hidden = false;
							ui.mcuCertificate.hidden = false;
						} else {

							ui.mcuKey.hidden = (inst.KeySelect == 'SIMPLE');
							ui.mcuCertificate.hidden = (inst.KeySelect == 'SIMPLE');
						}

					}
				},
				{
					name        : "mcuKey",
					displayName : "Private Key",
					hidden      : !isStandAloneGUI,
					default     : "",
					fileFilter	: Utils.wildcard()
				}, 
				{
					name        : "mcuCertificate",
					displayName : "Certificate",
					default     : "",
					fileFilter	: Utils.wildcard(),
					hidden      : !isStandAloneGUI
				}
		];




/*!
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - UART instance to be validated
 *  @param validation - Issue reporting object
 */
function validate(inst, validation, ui)
{
	if (inst.FileToken){

		Utils.validateToken("FileToken", inst.FileToken, inst, validation);
	}

	if (isStandAloneGUI) {

		if (!inst.mcuFileInput) {

			logError(validation, inst, "mcuFileInput", "MCU file was not provided.")
		}

	} else {

		if (!inst.mcuFileInput && !inst.External) {
			logError(validation, inst, "mcuFileInput", "MCU file was not provided.")
		}

	}
	
	if (inst.KeySelect == 'ADVANCED') {

		if (inst.Secure && !inst.mcuKey && !inst.NoSecurityTest)
		{
			logError(validation, inst, "mcuKey", "Security key file was not added.")
		}

		if (inst.Secure && !inst.mcuCertificate && !inst.NoSecurityTest)
		{
			logError(validation, inst, "mcuCertificate", "Certificate was not added.")
		}

	}



	if (! inst.PublicWrite)
	{
		logWarning(validation, inst, "PublicWrite", "The file will not be overwriteable without a known token (Vendor flag) or public write access (Public Write flag)")
	}

	if (inst.fileSizeCfg < 0) {

		logError(validation, inst, "fileSizeCfg", "Illegal file size.")
	}
}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "MCU Image",
    description   : "MCU Image",
    //modules       : Common.autoForcePowerModule,
	moduleStatic: {
		config        : config,
		validate      : validate
	},
    longDescription: longDescription,
    defaultInstanceName: "MCU_Image"
};

/*
 * Compose a device specific UART from this common UART definition and
 * export the device specific UART
 */
//let devId = system.deviceData.deviceId;
//let devGen = system.getScript("/ti/drivers/net/imagecreator/mcuimage/MCUImage" + devId);
//exports     = devGen.extend(base);
exports = base;

