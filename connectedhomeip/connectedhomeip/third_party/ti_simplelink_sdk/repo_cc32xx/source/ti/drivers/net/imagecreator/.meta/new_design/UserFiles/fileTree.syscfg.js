
"use strict";
/*global exports*/
/*
 * Example of a file tree
 */
let Utils  = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;

function validate(inst, validation) {

	if (inst.fileTokenCfg){

		Utils.validateToken("fileTokenCfg", inst.fileTokenCfg, inst, validation);
	}

	if (inst.secureCfg && !inst.KeyCfg && !inst.noSignatureTestCfg)
		{
			logError(validation, inst, "KeyCfg", "Security key file was not added.")
		}

	if (inst.secureCfg && !inst.CertCfg && !inst.noSignatureTestCfg)
	{
		logError(validation, inst, "CertCfg", "Certificate was not added.")
	}
}
exports = {
			displayName: "User Files",
			moduleStatic: {
					config: [{
						default: "",
						fileFilter: Utils.wildcard(),
						pickDirectory: true,
						name: "rootDirCfg",
						displayName: "Root Directory"
					}]
			},
	config: [
	{
		name: "includeOTA",
		displayName: "Include File to OTA Bundle",
		default: false,
	},
	{
		name: "filePathCfg",
		displayName: "File Full Path",
		readOnly: true,
		default: "",
	}, {
		name: "fileNameCfg",
		displayName: "File Name",
		readOnly: true,
		default: "",
	}, {
		name: "paddingFileSizeCfg",
		displayName: "Max File Size (bytes)",
		description: "Max size should take into consideration file updates",
		//readOnly: true,
		default: 0,
	}, {
		default: false,
		name: "failSafeCfg",
		displayName: "Failsafe",
		longDescription:`Editing the file is fail-safe.`,
	}, {
		default: false,
		name: "secureCfg",
		displayName: "Secure",
		longDescription:`File is encrypted on the serial flash.`,
		onChange: (inst, ui) => {
			ui.staticCfg.readOnly = !inst.secureCfg;
			ui.vendorCfg.readOnly = !inst.secureCfg;
			ui.publicWriteCfg.readOnly = !inst.secureCfg;
			ui.publicReadCfg.readOnly = !inst.secureCfg;
			ui.noSignatureTestCfg.readOnly = !inst.secureCfg;
			ui.fileTokenCfg.readOnly = !inst.secureCfg || !inst.vendorCfg;
			//ui.KeySelect.readOnly = !inst.secureCfg;
			ui.KeyCfg.readOnly = !inst.secureCfg;
			ui.CertCfg.readOnly = !inst.secureCfg;
			ui.KeyCfg.hidden = !inst.secureCfg;
			ui.CertCfg.hidden = !inst.secureCfg;
		}
	}, {
		default: false,
		name: "staticCfg",
		readOnly: true,
		displayName: "Static",
		longDescription:`Relevant only for secure files. Tokens are not replaced each time a file is open for write.`,
	}, {
		default: false,
		name: "vendorCfg",
		readOnly: true,
		displayName: "Vendor",
		longDescription:`Relevant only for secure files. The master token is set by the vendor.`,
		onChange: (inst, ui) => {
			ui.fileTokenCfg.readOnly = !inst.secureCfg || !inst.vendorCfg;
		}
	}, {
		default: 0,
		name: "fileTokenCfg",
		readOnly: true,
		displayName: "File Token",
		longDescription:`Relevant only when using the vendor flag. Token for secured file.`,
	},
	{
		default: false,
		name: "publicWriteCfg",
		readOnly: true,
		displayName: "Public Write",
		longDescription:`Relevant only for secure files. The file can be written without a token.`,
	}, {
		default: false,
		name: "publicReadCfg",
		readOnly: true,
		displayName: "Public Read",
		longDescription:`Relevant only for secure files. The file can be read without a token.`,
	}, {
		default: false,
		name: "noSignatureTestCfg",
		readOnly: true,
		displayName: "No Signature Test",
		longDescription:`Relevant only for secure files. By default, secure files require a signature.`,
		onChange: (inst, ui) => {
			//ui.KeySelect.hidden = inst.noSignatureTestCfg;
			ui.KeyCfg.hidden = inst.noSignatureTestCfg;
			ui.CertCfg.hidden = inst.noSignatureTestCfg;
		}
	},
	/*{
		name        : "KeySelect",
		displayName : "Private Key Sign Mode",
		longDescription : `Choose a way how to sign secured file. No effect for unsecured file. 
		- Simple     : Automatically signed by the dummy root certificate of SDK  (dummy-root-ca-cert-key)
		- Advanced   : Set private key file manually`,
		default     : 'ADVANCED',
		options     : [
			{ name:   'SIMPLE'        , displayName: "Simple" 	},
			{ name:   'ADVANCED'      , displayName: "Advanced" }
		],
		onChange: (inst, ui) => {
			ui.KeyCfg.hidden = (inst.CertSelect == 'SIMPLE');
			ui.CertCfg.hidden = (inst.CertSelect == 'SIMPLE');
		}
	},*/
	{
		name        	: "KeyCfg",
		displayName 	: "Private Key",
		default     	: "",
		fileFilter		: Utils.wildcard(),
		longDescription	:'Private key for certificate',
		hidden    		: true,
		readOnly        : true
		/*onChange: (inst, ui) => {
			let key_name = inst.KeyCfg.substring(inst.KeyCfg.lastIndexOf('\\')+1)
			inst.KeyCfg = key_name
		}*/
	}, 
	{
		name        	: "CertCfg",
		displayName 	: "Certificate",
		default     	: "",
		fileFilter		: Utils.wildcard(),
		longDescription	:'Certificate for secured file. Certificate should be taken from userFiles root folder.',
		hidden    		: true,
		readOnly    	: true
	}
],

	validate:validate, 
	longDescription: "This is a file tree. All certificates should exist in root folder.",
	uiView: "fileTree",

};
