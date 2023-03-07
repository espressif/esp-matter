/*!
 *  General Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `The Device Identifier Composition Engine (DICE) is a security standard from the Trusted Computing
Group (TCG). It is designed to help address the need for increased security in the Internet of Things (IoT)
and targets devices such as microcontrollers. The DICE standard specifies a framework for hardware and
software based on cryptographic device identity for authentication and attestation through a
manufacturer’s cloud servers (for example, Azure IoT cloud service).`;
let Utils = system.getScript("/ti/drivers/net/imagecreator/Utils.js");
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;
let cc= Utils.country_codes();

/*!
 * Common General configurable across all devices.
 */
let config = [


                    {
                        name        : "ENABLE_DICE",
                        displayName : "Device Identity Configuration",
                        default     : "0",
                        options     : [
                            { name:   "0" , displayName: "Disable DICE"  },
                            { name:   "1" , displayName: "Enable DICE"  }
                        ],
                        onChange: (inst, ui) => {
                            
                            ui.CSR_VENDOR_1.hidden 	= (inst.ENABLE_DICE == "0");
                            ui.CSR_TOKEN.hidden 	= (!inst.CSR_VENDOR_1 || inst.ENABLE_DICE == "0");
                            ui.CSR_ISCA.readOnly    = (inst.ENABLE_DICE == "1");
                            inst.CSR_ISCA           = (inst.ENABLE_DICE == "1");

                        }
                    },
                    {
                        name        : "CREATE_CSR",
                        displayName : "Certificate Configuration",
                        default     : "2",
                        options     : [
                            { name:   "2" , displayName: "Disable"                   },
                            { name:   "0" , displayName: "Certificate Sign Request"  },
                            { name:   "1" , displayName: "Self Signed Certificate"   }
                        ],
                        onChange: (inst, ui) => {
                            ui.csrCertSerialNameText.hidden         = (inst.CREATE_CSR == "2") ;
                            ui.csrValidityName.hidden               = (inst.CREATE_CSR != "1") ;
                            ui.csrValidityNameEnd.hidden            = (inst.CREATE_CSR != "1") ;
                            ui.CSR_ISCA.hidden                      = (inst.CREATE_CSR == "2") ;
                            ui.csrCountryCodeInput.hidden           = (inst.CREATE_CSR == "2") ;
                            ui.csrStateNameText.hidden              = (inst.CREATE_CSR == "2") ;
                            ui.csrLocalityNameText.hidden           = (inst.CREATE_CSR == "2") ;
                            ui.csrSurnameNameText.hidden            = (inst.CREATE_CSR == "2") ;
                            ui.csrOrganizationNameText.hidden       = (inst.CREATE_CSR == "2") ;
                            ui.csrOrganizationUnitNameText.hidden   = (inst.CREATE_CSR == "2") ;
                            ui.csrEmailText.hidden                  = (inst.CREATE_CSR == "2") ;
                            ui.csrNameText.hidden                   = (inst.CREATE_CSR == "2") ;
                            ui.UDID_NAME_0.hidden                   = (inst.CREATE_CSR == "2") ;
                        }
                    },
                    {
                        name		: "CSR_VENDOR_1",
                        displayName	: "Use Secured Vendor",
                        default		: false,
                        hidden      : true,
                        onChange: (inst, ui) => {
                            ui.CSR_TOKEN.hidden 	= !inst.CSR_VENDOR_1 || inst.ENABLE_DICE == "0";
                        }
                    },
                    {
                        name: "CSR_TOKEN",
                        displayName: "File Token",
                        hidden      : true,
                        default     : 0,
                    },
                    {
                        name: "csrValidityName",
                        displayName: "Certification Validity Start",
                        hidden      : true,
                        default     : new Date("2018-01-01")
                    },
                    {
                        name: "csrValidityNameEnd",
                        displayName: "Certification Validity End",
                        hidden      : true,
                        default     : new Date("2021-12-31")
                    },
                    {
                        name: "csrCertSerialNameText",
                        displayName: "Certificate Serial Number",
                        hidden      : true,
                        default     : 0,
                    },
                    {
                        name: "CSR_ISCA",
                        displayName: "Is Certificate CA?",
                        hidden      : true,
                        default     : false
                    },
                    {
                        name: "csrCountryCodeInput",
                        displayName: "Subject Country Code",
                        hidden      : true,
                        default     : "US",
                        options     : cc
                    },
                    {
                        name: "csrStateNameText",
                        displayName: "State",
                        hidden      : true,
                        default     : "State",
                    },
                    {
                        name: "csrLocalityNameText",
                        displayName: "Locality",
                        hidden      : true,
                        default     : "Locality",
                    },
                    {
                        name: "csrSurnameNameText",
                        displayName: "Surname",
                        hidden      : true,
                        default     : "Surname",
                    },
                    {
                        name: "csrOrganizationNameText",
                        displayName: "Organization",
                        hidden      : true,
                        default     : "Org. Name",
                    },
                    {
                        name: "csrOrganizationUnitNameText",
                        displayName: "Organization Unit",
                        hidden      : true,
                        default     : "Org.Unit Name",
                    },
                    {
                        name: "csrEmailText",
                        displayName: "Email",
                        hidden      : true,
                        default     : "email@email.com",
                    },
                    {
                        name: "csrNameText",
                        displayName: "Common Name",
                        hidden      : true,
                        default     : "Name",
                    },
                    {
                        name: "UDID_NAME_0",
                        displayName: "Use UDID As Common Name.",
                        longDescription:"Use Unique Device ID (UDID) As Common Name.",
                        hidden      : true,
                        default     : false,
                        onChange: (inst, ui) => {
                            ui.csrNameText.readOnly 	= inst.UDID_NAME_0 ;
                        }
                    }

                /*
                    displayName: "Device Identity Configuration",
                    description: "",
                    config: [
                    ]
                }*/
                    
        
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

    if(inst.ENABLE_DICE == "1" && inst.CREATE_CSR == "2") 
    {
        logError(validation, inst, "CREATE_CSR", "Can't be disable!")
    }
}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "Device Identity",
    description   : "Device Identity Configuration",
	moduleStatic: {
    config        : config,
    validate      : validate
	},
    longDescription: longDescription
};


exports = base;

