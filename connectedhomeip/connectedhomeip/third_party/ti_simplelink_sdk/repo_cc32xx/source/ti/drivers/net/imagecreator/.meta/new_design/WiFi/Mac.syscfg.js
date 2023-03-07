/*!
 *  Device MAC Settings Section of ImageCreator for SysConfig
 */
"use strict";

/* Intro splash on GUI */
let longDescription = `Configure MAC Settings for Device, otherwise the device will use default values from ROM.`;

/*
 * Compose a device specific MAC config
 */
let devId = system.deviceData.deviceId;
let gen2 = true;    
if ((devId == "CC3235S") || (devId == "CC3235SF") || (devId == "CC3135R"))
{
	gen2 = false;
}

let Utils 		= system.getScript("/ti/drivers/net/imagecreator/Utils.js");
const ld       	= system.getScript("/ti/drivers/net/imagecreator/long_description.js");
let Common 		= system.getScript("/ti/drivers/Common.js");
let logWarning 	= Common.logWarning;
let logError 	= Common.logError;

let cc_g3= Utils.country_codes();

let cc_g2 =  [
	{ name: "EU", displayName: "Europe" },
	{ name: "US", displayName: "USA" },
	{ name: "JP", displayName: "Japan" }
]

let channels_g2 = [
	{ name: "1" },
	{ name: "2" },
	{ name: "3" },
	{ name: "4" },
	{ name: "5" },
	{ name: "6" },
	{ name: "7" },
	{ name: "8" },
	{ name: "9" },
	{ name: "10" },
	{ name: "11" },
	{ name: "12" },
	{ name: "13" }
]
let channels_g3 = [
	{ name: "1" },
	{ name: "2" },
	{ name: "3" },
	{ name: "4" },
	{ name: "5" },
	{ name: "6" },
	{ name: "7" },
	{ name: "8" },
	{ name: "9" },
	{ name: "10" },
	{ name: "11" },
	{ name: "12" },
	{ name: "13" },
	{ name: "14" }
]
function onDHCPServerChange(inst, ui) {
	ui.apipAddrText.readOnly = inst.AP_DHCP_SID_2
	ui.apdgText.readOnly = inst.AP_DHCP_SID_2
	ui.apDNSText.readOnly = inst.AP_DHCP_SID_2
	ui.apDhcpStartAddressText.readOnly = inst.AP_DHCP_SID_2
	ui.apDhcpLastAddressText.readOnly = inst.AP_DHCP_SID_2
	ui.apDhcpLeaseTimeText.readOnly = inst.AP_DHCP_SID_2
}


/*!
 * Common General configurable across all devices.
 */
let config = [

			{
				displayName: "General Role Settings",
				description: "",
				config: [
							
									{
										name: "StartRoleSelect",
										displayName: "Start Role",
										description:`Device mode`,
										default: "2",
										
										options: [
											{ name: "2", displayName: "Access Point" },
											{ name: "3", displayName: "P2P" },
											{ name: "0", displayName: "Station" }
										]
									},
									{
										name: "CountryCodeInput",
										displayName: "Country Code",
										default: "EU",
										
										options: gen2 ? cc_g2 : cc_g3
									},
									{
										name: "deviceNameText",
										displayName: "Device Name",
										
										default: "mysimplelink"
									},
									
									{
										name: "conPolAutoProvisioningSelect",
										displayName: "Auto Provisioning",
										default: "1",
										
										options: [
											{ name: "0", displayName: "Disable" },
											{ name: "1", displayName: "Enable" }
										]
									},
									{
										name: "AutoProvExternalConfSelect",
										displayName: "Auto Provisioning Ext. Confirmation",
										default: "0",
										
										options: [
											{ name: "0", displayName: "Disable" },
											{ name: "1", displayName: "Enable" }
										]
									}

						],
						collapsed :false
			},
			{
				displayName: "STA Role Settings",
				description: "",
				config: [
							{
								name        : "NO_PSPOLL_MODE",
								displayName : "Legacy PS Poll Mechanism",
								longDescription : "PS Poll is a packet that gets sent from a station to AP when awaking from sleep. The packet allows the Station to stay in Power Save mode while recieving packets. In higher throughput use cases, it can make sense to disable this feature to reduce the number of packets sent.",
								longDescription : ld.pspollLongDescritpion,
								default     : "1",
								options: [
									{ name: "0", displayName: "Enable" },
									{ name: "1", displayName: "Disable" }
								]
							},
							{
								name: "conPolAutoStartSelect",
								displayName: "Auto Connect Policy",
								default: "1",
								options: [
									{ name: "0", displayName: "Disable" },
									{ name: "1", displayName: "Enable" }
								]
							},
							{
								name: "conPolFastConnectSelect",
								displayName: "Fast Connect Policy",
								default: "0",
								options: [
									{ name: "0", displayName: "Disable" },
									{ name: "1", displayName: "Enable" }
								]
							},
							{
								name: "stap2pDHCPEnable",
								displayName: "DHCP Client Enable",
								default: true,
								//onChange: onDHCPClientChange
								onChange: (inst, ui) => {
									ui.staipAddrText.readOnly = inst.stap2pDHCPEnable;
									ui.stasubnetMaskText.readOnly = inst.stap2pDHCPEnable;
									ui.stadgwText.readOnly = inst.stap2pDHCPEnable;
									ui.stadnsText.readOnly = inst.stap2pDHCPEnable;
								}
							},
							{
								name: "staipAddrText",
								displayName: "IP Address",
								textType: "ipv4_address",
								default: "192.168.1.101",
								
								readOnly: true
							},
							{
								name: "stasubnetMaskText",
								displayName: "Subnet Mask",
								textType: "ipv4_address",
								default: "255.255.255.0",
								
								readOnly: true
							},
							{
								name: "stadgwText",
								displayName: "Default Gateway",
								textType: "ipv4_address",
								default: "192.168.1.31",
								
								readOnly: true
							},
							{
								name: "stadnsText",
								displayName: "DNS Server",
								textType: "ipv4_address",
								default: "192.168.1.31",
								
								readOnly: true
							}
						],
						collapsed :true
			},
			{
				displayName: "AP Role Settings",
				description: "",
				config: [
							{
								name: "apSSIDText",
								displayName: "SSID",
								
								default: "mysimplelink"
							},
							{
								name: "apMaxStaNum",
								displayName: "Max Stations",
								
								default: 4
							},
							{
								name: "hiddenSSIDSelect",
								displayName: "Hidden SSID",
								
								default: "0",
								options: [
									{ name: "1", displayName: "Enable" },
									{ name: "0", displayName: "Disable" }
								]
							},
							{
								name: "apSecuritySelect",
								displayName: "Security",
								
								default: "0",
								options: [
									{ name: "0", displayName: "Open" },
									{ name: "1", displayName: "WEP" },
									{ name: "2", displayName: "WPAv2" }
						
								],
								onChange: (inst, ui) => {

									if (inst.apSecuritySelect == "1") {
										inst.apPWText = "1234567890123"
										ui.apPWText.readOnly = false;
									}
									if (inst.apSecuritySelect == "2") {
										inst.apPWText = "12345678"
										ui.apPWText.readOnly = false;
									}
									if (inst.apSecuritySelect == "0") {
										inst.apPWText = ""
										ui.apPWText.readOnly = true;
									}
								}
							},
							{
								name: "apPWText",
								displayName: "Password",
								readOnly: true,
								default: "",
								longDescription : ld.passwordLongDescritpion
							},
							{
								name: "apChannelNum",
								displayName: "Channel",
								
								default: "1",
								options: gen2 ? channels_g2 : channels_g3
							},
							{
								name: "AP_DHCP_SID_2",
								displayName: "DHCP Server Enable",
								default: true,
								onChange: (inst, ui) => {
									ui.apipAddrText.readOnly = !inst.AP_DHCP_SID_2
									ui.apdgText.readOnly = !inst.AP_DHCP_SID_2
									ui.apDhcpStartAddressText.readOnly = !inst.AP_DHCP_SID_2
									ui.apDhcpLastAddressText.readOnly = !inst.AP_DHCP_SID_2
									ui.apDhcpLeaseTimeText.readOnly = !inst.AP_DHCP_SID_2
								}
							},
							{
							name: "AP_DNS_SID_8",
							displayName: "DNS Server Enable",
							default: true,
							onChange: (inst, ui) => {
								ui.apDNSText.readOnly = !inst.AP_DNS_SID_8
							}
							},
							{
								name: "apipAddrText",
								displayName: "IP Address",
								textType: "ipv4_address",
								readOnly: false,
								default: "10.123.45.1"
							},
							{
								name: "apdgText",
								displayName: "Default Gateway",
								default: "10.123.45.1",
								readOnly: false,
								textType: "ipv4_address"
							},
							{
								name: "apDNSText",
								displayName: "DNS Server",
								default: "10.123.45.1",
								readOnly: false,
								textType: "ipv4_address"
						
							},
							{
								name: "apDhcpStartAddressText",
								displayName: "DHCP Start Address",
								default: "10.123.45.2",
								readOnly: false,
								textType: "ipv4_address"
							},
							{
								name: "apDhcpLastAddressText",
								displayName: "DHCP Last Address",
								default: "10.123.45.254",
								readOnly: false,
								textType: "ipv4_address"
							},
							{
								name: "apDhcpLeaseTimeText",
								displayName: "DHCP Lease Time (hours)",
								readOnly: false,
								default: 24
							}
							

						],
						collapsed :true
			},
			{
				displayName: "Wi-Fi® Direct/GO Settings",
				description: "",
				config: [
					{
						name: "conAnyWFDirectSelect",
						displayName: "Wi-Fi® Direct/GO",
						default: "0",
						options: [
							{ name: "0", displayName: "Disable" },
							{ name: "1", displayName: "Enable" }
						]
					},
					{
					name: "GO_DNS_SID_8",
					displayName: "DNS Server Enable",
					default: true//,
					
					//onChange: onDHCPServerChange
					},
					{
						name: "GO_DHCP_SID_2",
						displayName: "DHCP Server Enable",
						default: true//,
						
						//onChange: onDHCPServerChange
						},
					],
					collapsed :true
			}
			

];

/*!
 * Validate this module's configuration
 * @param inst       - MAC config instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
	let regexp = /[^\u00A1-\uFFFFa-zA-Z0-9\\-]+/
	if (gen2 == true) {
		regexp = /[^a-zA-Z0-9\\-]+/
	}

	if(regexp.test(inst.deviceNameText)){
		logError(validation, inst, "deviceNameText", "May not contain anything other than letters, digits and dashes (-)");
	} 

	if (inst.deviceNameText.trim()=== "") {
		logError(validation, inst, "deviceNameText", "May not be empty or made out of spaces (-)");
	}

	if (inst.deviceNameText.length > 32) {
		logError(validation, inst, "deviceNameText", "May not be longer than 32 characters");
	}

	if (inst.apMaxStaNum > 4 ) {
        logError(validation, inst, "apMaxStaNum", "Max number of stations is 4");
	}
	
	if (inst.apMaxStaNum < 1 ) {
        logError(validation, inst, "apMaxStaNum", "Min number of stations is 1");
	}

	if (inst.apSecuritySelect == "1") //WEP
	{
		if ((inst.apPWText.length == 5) || (inst.apPWText.length == 13))
		{
			if(/^[\x00-\x7F]*$/.test(inst.apPWText)){
				//
			}
			else {
				logError(validation, inst, "apPWText", "Wrong ASCII input.");
			}
			
		}
		else 
			logError(validation, inst, "apPWText", "Wrong password length. Should be 13 or 5.");
	}

	if (inst.apSecuritySelect == "2") //WPA
	{
		if ((inst.apPWText.length < 8) || (inst.apPWText.length > 63))
			logError(validation, inst, "apPWText", "Wrong password length. Should be in the range 8-63.");
	}
	
	
	if (Utils.checkIpaddrInRange(inst.apipAddrText, inst.apDhcpStartAddressText, inst.apDhcpLastAddressText)){
		logError(validation, inst, "apipAddrText", "IP is in the DHCP range.");
	}

	if (Utils.atoi(inst.apDhcpStartAddressText) > Utils.atoi(inst.apDhcpLastAddressText)){
		logError(validation, inst, "apDhcpLastAddressText", "Last address should be bigger than start address.");
	}
}

/* Common MAC config instance components across all devices. */
let base = {
    displayName   : "Role Settings",
    description   : "System -> Role -> General Settings",
	moduleStatic: {
		config        : config,
		validate      : validate
	},
    longDescription: longDescription,
    defaultInstanceName: "Mac"
};

/*
 * Compose a device specific MAC config
 
let devId = system.deviceData.deviceId;
let gen = "Gen2";    
if ((devId == "CC3235S") || (devId == "CC3235SF") || (devId == "CC3135R"))
{
	gen = "Gen3";
}
*/
//let devGen = system.getScript("mac/Mac" + gen);
//exports     = devGen.extend(base);
exports = base;

