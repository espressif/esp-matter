/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== ImageCreator.syscfg.js ========
 *  Define the SysConfig for ImageCreator
 */

"use strict";
let displayName = "Image Configuration";
let description =
    "The SimpleLink Wifi is a platform for development of network enabled " +
    "applications on Texas Instruments embedded processors."
let deviceId = system.deviceData.deviceId;
const isStandAloneGUI = system.isStandAloneGUI();
let topModules ;
let templates ;
let gen3    = (deviceId == "CC3235S") || (deviceId == "CC3235SF") || (deviceId == "CC3135R");
let advanced;

if ((deviceId == "CC3235S") || (deviceId == "CC3235SF")|| (deviceId == "CC3230S") || (deviceId == "CC3230SF")){
	advanced = 		{
						displayName: "Advanced",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/Device/Ota"/*,
							"/ti/drivers/net/imagecreator/new_design/Device/CSR"*/
						]
					}
} else {
	advanced = 		{
						displayName: "Advanced",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/Device/Ota"
						]
					}
}
	
if (deviceId.match(/CC32.*/)){
	topModules = [

					{
						displayName: "Network Processor",
						description: "Use these settings to configure the Network Processor settings and services",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/Device/General", 
							"/ti/drivers/net/imagecreator/new_design/WiFi/Mac",
							"/ti/drivers/net/imagecreator/new_design/WiFi/Radio",
						    "/ti/drivers/net/imagecreator/new_design/NetworkServices/Network"
						]
					},
				   {
						displayName: "File System",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/UserFiles/MCUImage",
							"/ti/drivers/net/imagecreator/new_design/UserFiles/fileTree",
							"/ti/drivers/net/imagecreator/new_design/UserFiles/ServicePack",
							"/ti/drivers/net/imagecreator/new_design/UserFiles/CertStore"						
							]
					},
					advanced
					
				];

				
	 templates = [
								{
									"name": "/ti/drivers/net/imagecreator/summary6.xdt",
									"outputPath": "RegDomainSum5.csv",
									"alwaysRun": gen3
								},
								{
									"name": "/ti/drivers/net/imagecreator/summary7.xdt",
									"outputPath": "RegDomainSum24.csv",
									"alwaysRun": gen3
								},
								
								{
									"name": "/ti/drivers/net/imagecreator/ProjectNew.c.xdt",
									"outputPath": "ti_drivers_net_wifi_config.json",
									"alwaysRun": true
								}
							];
}
  else if (deviceId.match(/CC31.*/)) {
	
	topModules = [
					{
						displayName: "Network Processor",
						description: "Use these settings to configure the Network Processor settings and services",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/Device/General", 
							"/ti/drivers/net/imagecreator/new_design/WiFi/Mac",
							"/ti/drivers/net/imagecreator/new_design/WiFi/Radio",
						    "/ti/drivers/net/imagecreator/new_design/NetworkServices/Network"
						]
					},
				   {
						displayName: "File System",
						modules: [
							"/ti/drivers/net/imagecreator/new_design/UserFiles/fileTree",
							"/ti/drivers/net/imagecreator/new_design/UserFiles/ServicePack",
							"/ti/drivers/net/imagecreator/new_design/UserFiles/CertStore"							
							]
					},
					advanced
				];
				
				templates = [
								{
									"name": "/ti/drivers/net/imagecreator/summary6.xdt",
									"outputPath": "RegDomainSum5.csv",
									"alwaysRun": gen3
								},
								{
									"name": "/ti/drivers/net/imagecreator/summary7.xdt",
									"outputPath": "RegDomainSum24.csv",
									"alwaysRun": gen3
								},
								
								{
									"name": "/ti/drivers/net/imagecreator/ProjectNew.c.xdt",
									"outputPath": "ti_drivers_net_wifi_config.json",
									"alwaysRun": true
								}
							];

}

if (deviceId.match(/CC32.*|CC31.*/) && isStandAloneGUI){

	//Add Syscfg Standalone Modules
	topModules.unshift({
		displayName: "Image Creation",
		modules: [
			"/ti/drivers/net/imagecreator/ImgCommands"
		]
		});
}



exports = {
	displayName,
    topModules,
    templates,
	ccxxImageComponent: true
};
