/*!
 *  System Settings-> Device -> Radio Settings of ImageCreator for SysConfig Gen3
 */
"use strict";
/* $super is used to call generic module's methods */
let $super;
/* Intro splash on GUI */
let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logInfo = Common.logInfo;
let cc31 = system.deviceData.deviceId == "CC3130R";
let ld = system.getScript("/ti/drivers/net/imagecreator/long_description.js");

let coex_input_cc31 = [
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]
let coex_output_cc31 = [
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "25" ,displayName : "   PAD25 (25) (21)"	},
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]
let coex_input_cc32 = [
	{name : "00" ,displayName : "   PAD00 (00) (50)"	},
	{name : "03" ,displayName : "   PAD03 (03) (58)"	},
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "11" ,displayName : "   PAD11 (11) (02)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "14" ,displayName : "   PAD14 (14) (05)"	},
	{name : "15" ,displayName : "   PAD15 (15) (06)"	},
	{name : "16" ,displayName : "   PAD16 (16) (07)"	},
	{name : "17" ,displayName : "   PAD17 (17) (08)"	},
	{name : "22" ,displayName : "   PAD22 (22) (15)"	},
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]
let coex_output_cc32 = [
	{name : "00" ,displayName : "   PAD00 (00) (50)"	},
	{name : "03" ,displayName : "   PAD03 (03) (58)"	},
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "11" ,displayName : "   PAD11 (11) (02)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "14" ,displayName : "   PAD14 (14) (05)"	},
	{name : "15" ,displayName : "   PAD15 (15) (06)"	},
	{name : "16" ,displayName : "   PAD16 (16) (07)"	},
	{name : "17" ,displayName : "   PAD17 (17) (08)"	},
	{name : "22" ,displayName : "   PAD22 (22) (15)"	},
	{name : "25" ,displayName : "   PAD25 (25) (21)"	},
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]
let ant_sel_1_cc31 = [
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "26" ,displayName : "   PAD26 (26) (29)"	},
	{name : "27" ,displayName : "   PAD27 (27) (30)"	},
	//{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]

let ant_sel_2_cc31 = [
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "25" ,displayName : "   PAD25 (25) (21)"	},
	{name : "26" ,displayName : "   PAD26 (26) (29)"	},
	{name : "27" ,displayName : "   PAD27 (27) (30)"	},
	//{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]

let ant_sel_1_cc32 = [
	{name : "03" ,displayName : "   PAD03 (03) (58)"	},
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "11" ,displayName : "   PAD11 (11) (02)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "14" ,displayName : "   PAD14 (14) (05)"	},
	{name : "15" ,displayName : "   PAD15 (15) (06)"	},
	{name : "16" ,displayName : "   PAD16 (16) (07)"	},
	{name : "17" ,displayName : "   PAD17 (17) (08)"	},
	{name : "22" ,displayName : "   PAD22 (22) (15)"	},
	{name : "26" ,displayName : "   PAD26 (26) (29)"	},
	{name : "27" ,displayName : "   PAD27 (27) (30)"	},
	//{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}
]

let ant_sel_2_cc32 = [
	{name : "03" ,displayName : "   PAD03 (03) (58)"	},//default 3235
	{name : "04" ,displayName : "   PAD04 (04) (59)"	},
	{name : "05" ,displayName : "   PAD05 (05) (60)"	},
	{name : "06" ,displayName : "   PAD06 (06) (61)"	},
	{name : "08" ,displayName : "   PAD08 (08) (63)"	},
	{name : "09" ,displayName : "   PAD09 (09) (64)"	},
	{name : "10" ,displayName : "   PAD10 (10) (01)"	},
	{name : "11" ,displayName : "   PAD11 (11) (02)"	},
	{name : "12" ,displayName : "   PAD12 (12) (03)"	},
	{name : "13" ,displayName : "   PAD13 (13) (04)"	},
	{name : "14" ,displayName : "   PAD14 (14) (05)"	},
	{name : "15" ,displayName : "   PAD15 (15) (06)"	},
	{name : "16" ,displayName : "   PAD16 (16) (07)"	},
	{name : "17" ,displayName : "   PAD17 (17) (08)"	},
	{name : "22" ,displayName : "   PAD22 (22) (15)"	},
	{name : "25" ,displayName : "   PAD25 (25) (21)"	},
	{name : "26" ,displayName : "   PAD26 (26) (29)"	},
	{name : "27" ,displayName : "   PAD27 (27) (30)"	},
	//{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}

]


/*!
 * Common General configures across all devices.
 */
let devSpecific = {
		displayName: "Radio Settings",
		moduleStatic: {
			config: [ 
						{ //CoEx
							displayName: "Coexistence",
							longDescription: ld.coex2LongDescription,
							config: [
										{
											name        : "COEX_MODE",
											displayName : "Mode",
											longDescription: ld.coexLongDescription,
											default     : "0",
											options     :[
												{name : "0"  ,displayName : "Disable" 	 },
												{name : "1"  ,displayName : "Single Ant"},
												{name : "2"  ,displayName : "Dual Ant"	 }
											],
											onChange: (inst, ui) => {
												ui.INPUT_PAD.hidden  = (inst.COEX_MODE == "0");
												ui.OUTPUT_PAD.hidden =((inst.COEX_MODE == "0")||(inst.COEX_MODE == "2"));
											}
										},
										{
											name        : "INPUT_PAD",
											displayName : "Input Pad (GPIO) (PIN)",
											//default     : "04",
											default     : cc31 ? "09":"04",
											hidden      : true,
											options     : cc31 ? coex_input_cc31:coex_input_cc32

										},
										{
											name        : "OUTPUT_PAD",
											displayName : "Output Pad (GPIO) (PIN)",
											default     : cc31 ? "12":"03",
											hidden      : true,
											options     : cc31 ? coex_output_cc31:coex_output_cc32
										}
								],
								
						},
						{ //Ant Sel
							displayName: "Antenna Selection",
							longDescription: ld.coex2LongDescription,
							config: [
										
										{
											name        : "ANT_SEL_MODE",
											displayName : "Mode",
											default     : "0",
											options     :[
												{name : "0"  ,displayName : "Disable" 	},
												{name : "1"  ,displayName : "Antenna 1"		},
												{name : "2"  ,displayName : "Antenna 2"	 	},
												{name : "3"  ,displayName : "Auto-Select"}
											],
											onChange: (inst, ui) => {
												ui.ANT1_PAD.hidden = (inst.ANT_SEL_MODE == "0");
												ui.ANT2_PAD.hidden = (inst.ANT_SEL_MODE == "0");
											}
										},
										{
											name        : "ANT1_PAD",
											displayName : "Antenna 1 Pad (GPIO) (PIN)",
											default     : cc31 ? "28":"08",
											hidden      : true,
											options: cc31 ? ant_sel_1_cc31:ant_sel_1_cc32
										},
										{
											name        : "ANT2_PAD",
											displayName : "Antenna 2 Pad (GPIO) (PIN)",
											default     : cc31 ? "10":"09",
											hidden      : true,
											options: cc31 ? ant_sel_2_cc31:ant_sel_2_cc32
								
										}

										
									],
						},		
						{ //RF2.4G
							displayName: "2.4GHz Radio Settings ",
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
												{name : "1" ,displayName : "Trigger"},
												{name : "2" ,displayName : "Onetime"}
											]
										}
								],
								collapsed :false
						}
					],
		/* override device-specific templates */
		templates: {
		},

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

	if (inst.INPUT_PAD == inst.OUTPUT_PAD)
	{
		logError(validation, inst, "OUTPUT_PAD", "Output pad should be different than input");
	}

	if (inst.ANT1_PAD == inst.ANT2_PAD)
	{
		logError(validation, inst, "ANT2_PAD", "Ant2 pad should be different than Ant1");
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
