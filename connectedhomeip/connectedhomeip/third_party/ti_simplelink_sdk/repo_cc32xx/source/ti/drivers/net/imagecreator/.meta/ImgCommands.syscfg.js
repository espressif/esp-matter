"use strict";
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;
let logError = Common.logError;
let ld = system.getScript("/ti/drivers/net/imagecreator/long_description.js");
const homedir = getUserHome();
//let default_sli_name = homedir + system.deviceData.deviceId + "_" + timestamp() + ".sli";
let default_sli_name = homedir + system.deviceData.deviceId  + ".sli"; /*Real path to use for function call*/
let default_sli_fine = "<SDK>/tools/standalone_sysconfig/generated_images/" + system.deviceData.deviceId  + ".sli"; /*path to show in UI*/
let output_sli = default_sli_name;
let input_sli ="";
let sys_os = system.getOS() 

let ldci =  `Create Image (SLI) file for programming from UI parameters\n
Available on stand alone mode only`
if (sys_os == 'mac'){
	ldci =  `Create Image (SLI) file for programming from UI parameters\n
	Available on stand alone mode only. Output log exists in <SDK>/tools/standalone_sysconfig/generated_images/cilog.txt`
} 

let ldp = `Calls to back-end program function.\n
Available on stand alone mode only`
if (sys_os == 'mac'){
	ldp =  `Calls to back-end program function.\n
	Available on stand alone mode only. Output log exists in <SDK>/tools/standalone_sysconfig/generated_images/p10log.txt`
} 

let default_sli2_name = "/generated_images/" + system.deviceData.deviceId  + ".sli";
function getUserHome() {
	return `./../../../../../../tools/standalone_sysconfig/generated_images/`;
  }

function getCIScriptName()
{
	if (sys_os == 'win') return "../source/ti/drivers/net/imagecreator/bin/create_imaget.bat"
	else return "../source/ti/drivers/net/imagecreator/bin/create_imaget.sh"
}

function getPScriptName()
{
	if (sys_os == 'win') return "../source/ti/drivers/net/imagecreator/bin/programt.bat"
	else return "../source/ti/drivers/net/imagecreator/bin/programt.sh"
}

function timestamp() {
	//return new Date().getTime();
	let date = new Date();
	return date.getUTCFullYear()+ '_' + date.getMonth() + '_' + date.getDate() + '_' + date.getHours() + '_' + date.getMinutes() + '_' + date.getSeconds();
}

function validate(inst, validation) { 
	if (inst.USE_SEC_BTLDR_1 &&  inst.USE_OTP_1  && !inst.otpFileInput)
	{
		logError(validation, inst, "otpFileInput", "OTP file was not provided")
	}

	if (inst.outputCfgci !=0)
	{
		logError(validation, inst, "outputCfgci", "Image creation failed.")
	}
	if (inst.outputCfgp !=0)
	{
		logError(validation, inst, "outputCfgp", "Image programming failed.")
	}
	
}

function validateInputs (inst, ui) {

		//ui.createImage.hidden = !inst.outputSLI || !inst.inputJson;
		//ui.createImage.readonly = !inst.outputSLI ;
	

}

exports = {
	displayName: "Image Commands",
	longDescription:`Calls to back-end image creation functions.\n
		Available on stand alone mode only`,
	description: "Build image file from project / Flash device - program",
	name       :"TT",
	moduleStatic: {
		config: [
				{
					displayName: "Create Image",
					longDescription:ldci,
					config: [
							{
								name        : "SLISelectCI",
								displayName : "Choose SLI File",
								longDescription : `Choose a way how to show SLI. 
								- Default     : <SDK>/tools/standalone_sysconfig/generated_images/ folder 
												Filename combined from device type; 
								- Add manually: Set path and file name manually`,
								default     : 'USE_DEFAULT',
								options     : [
									{ name:   'USE_DEFAULT'       , displayName: "Use default path" },
									{ name:   'ADD_MANUALLY'      , displayName: "Add manually" }
								],
								onChange: (inst, ui) => {
									inst.SLISelectCI == 'USE_DEFAULT' ? inst.outputSLI = default_sli_fine : inst.outputSLI = "";
									ui.outputSLI.readOnly = inst.SLISelectCI == 'USE_DEFAULT';
									}
								
							},
							{
								name: "outputSLI",
								displayName: "Image File (SLI)",
								longDescription:`Image (SLI) output file that keeps all configurations and should be programmed.`,
								readOnly:true,
								default: default_sli_fine,
								onChange: (inst, ui) => {

									if (inst.SLISelectP == 'COPY_FROM_C_I'){//Update program path
										inst.sliFile = inst.outputSLI;
									}
								}

							},
							{
								name: "createImage",
								displayName: " ",
								hidden:false,
								buttonText: "Create Image",
								onLaunch: (inst) => {
									if (inst.SLISelectCI == 'ADD_MANUALLY'){
										output_sli = inst.outputSLI;
									} else {
										output_sli = default_sli_name;
									}
									return {
										//command: "../source/ti/drivers/net/imagecreator/bin/create_imaget.bat",
										command: getCIScriptName(),
										args:["$comFile", `${output_sli}`, `${inst.SLISelectCI}`],
										initialData: system.getTemplate("/ti/drivers/net/imagecreator/ProjectNew.c.xdt")()
									};
								},
								onComplete: (inst, _ui, result) => {
									/*if (result.exitCode===0){
										inst.sliFile=inst.outputSLI;
										validateProgramInputs(inst, _ui);
									}
										
									else */if (result.exitCode==0xFFFFFFFF)
										inst.outputCfgci="-1"
									else
										inst.outputCfgci=result.exitCode.toString();
								},
							},
							{
								name: "outputCfgci",
								longDescription:`Return value from the backend function. 0-Ok, Otherwise error`,
								displayName: "Response",
								default: "0"
							}
					],
					collapsed:false
				},
				
				{
					displayName: "Program Image",
					longDescription:ldp,
					config: [
								{
									name        : "SLISelectP",
									displayName : "Choose SLI File",
									longDescription : `Choose a way how to show SLI. 
									- Default     : Set to <SDK>/tools/cc32xx_tools/sli_out/ folder. 
									- Add manually: Set path and file name manually`,
									default     : 'USE_DEFAULT',
									options     : [
										{ name:   'USE_DEFAULT'       , displayName: "Use default path" 	},
										{ name:   'ADD_MANUALLY'      , displayName: "Add manually" 		},
										{ name:   'COPY_FROM_C_I'     , displayName: "Same as Create image" }
									],
									onChange: (inst, ui) => {
										if(inst.SLISelectP == 'USE_DEFAULT'){
											ui.sliFile.hidden = false;
											ui.sliFileManually.hidden = true;
											inst.sliFile = default_sli_fine;
										} 
										if(inst.SLISelectP == 'COPY_FROM_C_I'){
											ui.sliFile.hidden = false;
											ui.sliFileManually.hidden = true;
											if (inst.SLISelectCI == 'USE_DEFAULT'){
												inst.sliFile = default_sli_fine;
											} else {
												inst.sliFile = inst.outputSLI;
											}
										}
										if(inst.SLISelectP == 'ADD_MANUALLY'){
											ui.sliFile.hidden = true;
											ui.sliFileManually.hidden = false;

										}
										ui.sliFile.readOnly = (inst.SLISelectP == 'USE_DEFAULT') || (inst.SLISelectP == 'COPY_FROM_C_I');
									}
								},
								{
									name: "sliFile",
									displayName: "Image File (SLI)",
									longDescription:"Set image file (SLI) automatically if it was created successfully by Create Image above",
									hidden:false,
									readOnly: true,
									default: default_sli_fine
								},
								{
									name: "sliFileManually",
									displayName: "Image File (SLI)",
									longDescription:"Set image file (SLI) automatically if it was created successfully by Create Image above",
									default: "",
									hidden:true,
									fileFilter		: ".sli"
								},
								{
									name: "keyFile",
									displayName: "Image Key File",
									longDescription:`16-byte key,for encrypted images (using AES-CTR encryption). An encrypted image
									can only be used with its key.`,
									default: "",
									fileFilter		: ".bin"
								},
								{
									name: "programImage",
									displayName: " ",
									buttonText: "Program Image",
									onLaunch: (inst) => {
										if (inst.SLISelectP == 'ADD_MANUALLY'){
											input_sli = inst.sliFileManually;
										} else  if (inst.SLISelectP == 'USE_DEFAULT'){
											input_sli = default_sli_name;
										} else {
											input_sli = output_sli;
										}

										return {
											command:getPScriptName(),
											args:["$comFile",`${input_sli}`, `${inst.keyFile}`],
											initialData: system.getTemplate("/ti/drivers/net/imagecreator/ProjectNew.c.xdt")()
										};
									},
									onComplete: (inst, _ui, result) => {
										if (result.exitCode==0xFFFFFFFF)
											inst.outputCfgp="-1"
										else
											inst.outputCfgp=result.exitCode.toString();
												}
								},
								{
									name: "outputCfgp",
									displayName: "Response",
									default: "0"
								}
							],
							collapsed:false
				}
				/*,
				{
					displayName: "Export Project",
					description: "",
					config: [
						{
							name: "export",
							displayName: "Export to ImageCreator project",
							buttonText: "Export",
							onLaunch: (inst) => {
								//alert(`test = ${system.getCurrentScriptPath().match('^.*[\\\/]')}`);
								var r = new RegExp( /^.*[\\\/]/)
								//var kk = /^.*[\\\/]/.test(system.getCurrentScriptPath())
								var projec_base_path = r.exec(system.getCurrentScriptPath());
								//var json_path = projec_base_path + 'Debug/syscfg/ti_drivers_net_wifi_config.json';
								
								//alert(`test = ${json_path}`);
								//console.log(r.exec(system.getCurrentScriptPath()));
								return {
									command: "../source/ti/drivers/net/imagecreator/bin/SLImageCreator.exe",
									//args:["syscfg", "export", `--json ${inst.outputJson}`, `${sdk_path}`, `${mcu}`, `${file}`],
									args:["syscfg", "export", `--json ${inst.inputJson}`, `${mcu}`, `${file}`],
									//args:["syscfg", "export", `--json ${json_path}`, `${mcu}`, `${file}`],
									initialData: inst.inputCfg,
								};
							},
							onComplete: (inst, _ui, result) => {
								inst.outputCfg = result.data;
							},
						},
					]
				}
				*/
			
		],
		validate: validate
	}
};
