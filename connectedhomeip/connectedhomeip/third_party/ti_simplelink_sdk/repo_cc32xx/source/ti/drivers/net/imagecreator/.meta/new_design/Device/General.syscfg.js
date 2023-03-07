/*!
 *  General Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = "";
let Utils = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;
let ld = system.getScript("/ti/drivers/net/imagecreator/long_description.js");
const isStandAloneGUI = system.isStandAloneGUI();


/*!
 * Common General configurable across all devices.
 */
let config = [
			{
				name        : "ModeSelect",
				displayName : "Image Mode",
				longDescription:ld.developmodeLongDescription,
				default     : 'production',
				options     : [
					{ name:   'production' , displayName: "Production" 	},
					{ name:   'development', displayName: "Development" }
				],
				onChange: (inst, ui) => {
					ui.SET_DEV_MAC.hidden = (inst.ModeSelect == 'production');
					if(ui.SET_DEV_MAC.hidden == false && (inst.SET_DEV_MAC == false))
					{
						ui.DevMA.hidden = (inst.ModeSelect == 'production');
					}
					else{
						ui.DevMA.hidden = true;
					}
				}
			},
			{
				name        : "ModuleSelect",
				displayName : "IC/Module",
				longDescription:`Module specific settings`,
				default     : '0',
				options     : [
					{ name:   '0', displayName: "I/C" 	},
					{ name:   '1', displayName: "Antenna Module" },
					{ name:   '2', displayName: "Non Ant Module" }
				]
				
			},
			{
				name        : "SET_DEV_MAC",
				displayName : "Auto-Detect Target",
				longDescription : "Automatically obtain connected device's MAC address to create development mode image.",
				default     : true,
				hidden      : true,
				//readOnly    : true,
				onChange: (inst, ui) => {
					ui.DevMA.readOnly = inst.SET_DEV_MAC;
					ui.DevMA.hidden = inst.SET_DEV_MAC;
				}
			},
			{
				name        : "DevMA",
				displayName : "Custom Target MAC Address",
				description : "Please input the MAC Address of the device you'd like to flash this image on. If the MAC address of the device and image doesn't match, programming will fail.",
				//default     : '70:ff:76:1c:2c:24',
				default     : '00:00:00:00:00:04',
				textType    : "mac_address",
				readOnly    :true,
				hidden      :true
			},
			{
				name        : "capacitySelect",
				displayName : "Capacity",
				description : `Device capacity`,
				default     : '4194304',
				options     : [
					{ name:   '1048576'  , displayName: "1M  Byte" 	},
					{ name:   '2097152'  , displayName: "2M  Byte" 	},
					{ name:   '4194304'  , displayName: "4M  Byte" 	},
					{ name:   '8388608'  , displayName: "8M  Byte" 	},
					{ name:   '16777216' , displayName: "16M Byte" 	}
				]
			},
			{
				name        : "FACTORY",
				displayName : "Restore To Factory Configuration",
				default     : 'defaults_and_image',
				options     : [
					{ name:   'defaults_and_image' 	, displayName: "Defaults and Image" },
					{ name:   'defaults_only'		, displayName: "Defaults Only" 		}
				]
			},
			{
				name        : "IGNORE_FORCE_AP",
				displayName : "Restore To Factory Image SOP",
				default     : '0',
				options     : [
					{ name:   '0' 	, displayName: "Enable"  },
					{ name:   '1'	, displayName: "Disable" }
				]
			},
			{
				name        : "USE_DEV_MAC",
				displayName : "Use Default MAC Address",
				longDescription : "Change Device's MAC Address after boot. Use this field if you want a different MAC Address than the device's hardware default",
				default     : true,
				onChange: (inst, ui) => {
					ui.MA.hidden = inst.USE_DEV_MAC;
				}
			},
			{
				name        : "MA",
				displayName : "Modified Device MAC Address",
				longDescription : "This modified MAC address will take effect after the first successful flashing of the device.",
				default     : '08:00:28:11:22:33',
				textType    : "mac_address",
				hidden      :true
			},
			{
				name		: "USE_KEY_1",/*for stand alone only*/
				displayName	: "Use Encryption Key",
				longDescription :`Create encrypted images (using AES-CTR encryption). An encrypted image
				can only be used with its key.`,
				default		: false,
				hidden      : !isStandAloneGUI, 
				onChange: (inst, ui) => {
					ui.keyFileInput.hidden 	= !inst.USE_KEY_1;
				}
			},
			{
				name: "keyFileInput",
				displayName: "Source File",
				longDescription:`16-byte file, used to encrypt image`,
				default: "",
				hidden      :true,
				fileFilter: Utils.wildcard()
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
function validate(inst, validation) {
	
	if ((inst.DevMA != '00:00:00:00:00:04') && !inst.SET_DEV_MAC)
	{
		Utils.validateMacAddr("DevMA", inst.DevMA, inst, validation);
	}
		
	Utils.validateMacAddr("MA", inst.MA, inst, validation);

	if (inst.USE_KEY_1  && !inst.keyFileInput)
	{
		logError(validation, inst, "keyFileInput", "Security key file was not provided!!")
	}

}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "General Settings",
    description   : "General Settings",
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
