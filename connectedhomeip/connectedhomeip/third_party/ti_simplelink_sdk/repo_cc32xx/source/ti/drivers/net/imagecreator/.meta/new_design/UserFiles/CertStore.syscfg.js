/*!
 *  System settings -> Network Applications
 */
"use strict";

/* Intro splash on GUI */
let gkeyCertStoreFileInput = ""
let gkeyCertStoreSignFileInput = ""

let Utils  = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;
let ld = system.getScript("/ti/drivers/net/imagecreator/long_description.js");



/*!
 * Common General configurable across all devices.
 */
let config = [
				/*{
				displayName: "Trusted Root-Certificate Catalog",
				description: "",
				config: [*/
							{
								name        : "CertStoreSelect",
								displayName : "Certificate Catalog",
								description : "Trusted Root-Certificate Catalog",
								default     : 'USE_DUMMY_CERT',
								options     : [
									{ name:   'USE_DEF_CERT_STORE_1' , displayName: "Use Trusted Root Certificate Catalog from SDK"  },
									{ name:   'USE_DUMMY_CERT'       , displayName: "Use Dummy Root Certificate Playground from SDK" },
									{ name:   'ADD_MANUALLY'         , displayName: "Add manually" }
								],
								onChange: (inst, ui) => {
									inst.USE_DEF_CERT_STORE_1 = (inst.CertStoreSelect == 'USE_DEF_CERT_STORE_1');
									inst.USE_DUMMY_CERT = (inst.CertStoreSelect == 'USE_DUMMY_CERT');
									ui.keyCertStoreFileInput.hidden 	= inst.USE_DEF_CERT_STORE_1 || inst.USE_DUMMY_CERT;
									ui.keyCertStoreSignFileInput.hidden = inst.USE_DEF_CERT_STORE_1 || inst.USE_DUMMY_CERT;

								}
							},
							{
								name: "USE_DEF_CERT_STORE_1",
								displayName: "Use Trusted Root-Certificate Catalog from SDK",
								default: false,
								hidden : true
								
							},
							{
								name: "USE_DUMMY_CERT",
								displayName: "Use Dummy Certificates from SDK certificate-playground",
								description   : "True - dummy, False - real",
								default: true,
								hidden : true
							},
							{
								name: "keyCertStoreFileInput",
								displayName: "Source File",
								default: "",
								hidden      :true,
								fileFilter: Utils.wildcard()
							},
							{
								name: "keyCertStoreSignFileInput",
								displayName: "Signature Source File",
								default: "",
								hidden      :true,
								fileFilter: Utils.wildcard()
							},
				/*]
			},
			{
				displayName: "Vendor Root-Certificate Catalog",
				description: "",
				config: [*/
							{
								name: "USE_SEC_BTLDR_1",
								displayName: "Use Vendor Certificate Catalog",
								longDescription:ld.vendorLongDescription,
								default: false,
								hidden 		: true, /*hidden due to SDK request */
								onChange: (inst, ui) => {
									ui.USE_OTP_1.hidden 	= !inst.USE_SEC_BTLDR_1;
									ui.otpFileInput.hidden 	=  !inst.USE_OTP_1 || !inst.USE_SEC_BTLDR_1;
									//ui.CertStoreSelect.readOnly = inst.USE_SEC_BTLDR_1;
								}
							},
							{
								name		: "USE_OTP_1",
								displayName	: "Add OTP File",
								longDescription:ld.useotpLongDescription,
								default		: false,
								hidden 		: true,
								onChange: (inst, ui) => {
									ui.otpFileInput.hidden 	= !inst.USE_OTP_1 || !inst.USE_SEC_BTLDR_1;
								}
							},
							{
								name: "otpFileInput",
								displayName: "Source File",
								default: "",
								hidden      :true,
								fileFilter: Utils.wildcard()
							}
			/*			]
			}*/


];

/*!
 * Validate this module's configuration
 *
 * @param inst       - instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation) { 

	if (inst.CertStoreSelect == 'ADD_MANUALLY' && !inst.keyCertStoreFileInput)
	{
		logError(validation, inst, "keyCertStoreFileInput", "Catalog file was not provided.")
	}

	if (inst.CertStoreSelect == 'ADD_MANUALLY' && !inst.keyCertStoreSignFileInput)
	{
		logError(validation, inst, "keyCertStoreSignFileInput", "Catalog signature file was not provided.")
	}

	if (inst.USE_SEC_BTLDR_1 &&  inst.USE_OTP_1  && !inst.otpFileInput)
	{
		logError(validation, inst, "otpFileInput", "OTP file was not provided.")
	}
}


/* Common Network Applications instance components across all devices. */
let base = {
	displayName: "Certificate Store",
	description: "Certificate Store",
	moduleStatic: {
		config: config,
		validate: validate,
	},
	longDescription: ld.certStoreLongDescritpion,
};

exports = base;