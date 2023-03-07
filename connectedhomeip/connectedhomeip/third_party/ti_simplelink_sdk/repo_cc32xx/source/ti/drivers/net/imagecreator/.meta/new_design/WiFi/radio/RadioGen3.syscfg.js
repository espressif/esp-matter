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
let cc31 = system.deviceData.deviceId == "CC3135R";
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
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
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
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
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
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
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
//	{name : "32" ,displayName : "SH_PAD01 (32) (52)"	},
	{name : "28" ,displayName : "   PAD40 (28) (18)"	}

]
function descriptionRD24(rate) {
	return `This is the power offset for ${rate} from the default TI design, limited to EVM and Mask constraints.  It can be both positive and negative to allow power increase.`;
}

const longDescriptionRD24 = `
This is the power offset from the default TI design, limited to EVM and Mask constraints.  It can be both positive and negative to allow power increase.  Valid values are -6[dB] to +6[dB]

Modulation | Rate (Mbps) | Spec | 11b/Low/High Rate
--- | --- | --- | ---
DSSS | 1 | 802.11b | 11b
DSSS | 2 | 802.11b | 11b
CCK | 5.5 | 802.11b | 11b
CCK | 11 | 802.11b | 11b
OFDM | 6 | 802.11g | Low
OFDM | 9 | 802.11g | Low
OFDM | 12 | 802.11g | Low
OFDM | 18 | 802.11g | Low
OFDM | 24 | 802.11g | Low
OFDM | 36 | 802.11g | Low
OFDM | 48 | 802.11g | High
OFDM | 54 | 802.11g | High
MCS0 (BPSK) | 6.5-7.2 | 802.11n | Low
MCS1 (QPSK) | 13-14.4 | 802.11n | Low
MCS2 (QPSK) | 19.7-21.7 | 802.11n | Low
MCS3 (16-QAM) | 26-28.9 | 802.11n | Low
MCS4 (16-QAM) | 39-43.3 | 802.11n | Low
MCS5 (64-QAM) | 52-57.8 | 802.11n | High
MCS6 (64-QAM) | 58.5-65 | 802.11n | High
MCS7 (64-QAM) | 65-72.2 | 802.11n | High
`;

const defaults = [
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
];

function getChannelKeys(start, count) {
	const a = [];
	for (let i = 0; i < count; ++i) {
		a.push(start + i * 4);
	}
	return a;
}

function allChannelKeys() {
	return [
		...getChannelKeys(36, 4),
		...getChannelKeys(52, 4),
		...getChannelKeys(100, 5),
		...getChannelKeys(120, 7),
		...getChannelKeys(149, 5)
	];
}

function createChannelConfigs(defaults, soname) {
	return _.map(allChannelKeys(), (chan, index) => {
		return {
			name: `${soname}channel${chan}`,
			displayName: `Channel ${chan}`,
			default: defaults[index],
			readOnly: true,
			displayFormat: {
				radix: "dec",
				fixedPoint: 3,
			},
		}
	});
}

function createExBOChannelConfigs(defaults) {
	return _.map(allChannelKeys(), (chan, index) => {
		return {
			name: `channel${chan}`,
			displayName: `Channel ${chan}`,
			config: [
				{
					name: `channel${chan}low`,
					displayName: "Low (dB)",
					default: 0,
					readOnly:true,
					displayFormat: {
						radix: "dec",
						fixedPoint: 3,
					},
				},
				{
					name: `channel${chan}high`,
					displayName: "High (dB)",
					default: 0,
					readOnly:true,
					displayFormat: {
						radix: "dec",
						fixedPoint: 3,
					},
				}
			]
		}
	});
}

function create24ChannelConfigs(numChannels, soname){

	const instances = [];
		_.times(numChannels, (i) => {
			instances.push({
				name: `${soname}channel${i + 1}`,
				displayName: `Channel ${i + 1}`,
				config : [
					{
						name: `${soname}channel${i + 1}11b`,
						displayName: "11b (dB)",
						default: 0,
						readOnly: true,
						displayFormat: {
							radix: "dec",
							fixedPoint: 3,
						},
						description: descriptionRD24("only 802.11b rates"),
						longDescription: longDescriptionRD24,
					},
					{
						name: `${soname}channel${i + 1}low`,
						displayName: "Low (dB)",
						default: 0,
						readOnly: true,
						displayFormat: {
							radix: "dec",
							fixedPoint: 3,
						},
						description: descriptionRD24("low rates (non 802.11b and high rates)"),
						longDescription: longDescriptionRD24,
					},
					{
						name: `${soname}channel${i + 1}high`,
						displayName: "High (dB)",
						default: 0,
						readOnly: true,
						displayFormat: {
							radix: "dec",
							fixedPoint: 3,
						},
						description: descriptionRD24("high rates (MCS7, 54 Mbps, 48Mbps)"),
						longDescription: longDescriptionRD24,
					}
				],
				collapsed: true,
			})
		});
		return instances;
}

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
										ui.OUTPUT_PAD.hidden = ((inst.COEX_MODE == "0")||(inst.COEX_MODE == "2"));
									}
								},
								{
									name        : "INPUT_PAD",
									displayName : "Input Pad (GPIO) (PIN)",/*CC_COEX_BLE_IN*/
									description : `CC_COEX_BLE_IN`,
									default     : cc31 ? "09":"00",
									hidden      : true,
									options     : cc31 ? coex_input_cc31:coex_input_cc32

								},
								{
									name        : "OUTPUT_PAD",
									displayName : "Output Pad (GPIO) (PIN)",/**/
									description : `CC_COEX_SW_OUT`,
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
									displayName : "Antenna 1 Pad (GPIO) (PIN)",/**/
									description : `ANT_SEL_1`,
									default     : cc31 ? "28":"08",
									hidden      : true,
									options: cc31 ? ant_sel_1_cc31:ant_sel_1_cc32
								},
								{
									name        : "ANT2_PAD",
									displayName : "Antenna 2 Pad (GPIO) (PIN)",/*ANT_SEL_2*/
									description : `ANT_SEL_2`,
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
							},
							{//RegDomain2.4G
								displayName : "2.4GHz Regulatory Settings",
								longDescription:`Warning! Changing these values may defect regulatory radio certification or system performance`,
								config: [
											{//FCC24
												name: "FCC24",
												displayName: "FCC (dBm)",
												config: [
													{
														name: "FCC24enable",
														displayName: "Override Firmware Defaults",
														default: false,
														onChange: (inst, uiState) => {
															_.times(13, (i) => {
																uiState[`FCC24channel${i + 1}11b` ].readOnly = !inst.FCC24enable;
																uiState[`FCC24channel${i + 1}low` ].readOnly = !inst.FCC24enable;
																uiState[`FCC24channel${i + 1}high`].readOnly = !inst.FCC24enable;
															});
														}
													},

													...create24ChannelConfigs(13,"FCC24")
												]
											},
											{//ETSI24
												name: "ETSI24",
												displayName: "ETSI (dBm)",
												config: [
													{
														name: "ETSI24enable",
														displayName: "Override Firmware Defaults",
														default: false,
														onChange: (inst, uiState) => {
															_.times(13, (i) => {
																uiState[`ETSI24channel${i + 1}11b` ].readOnly = !inst.ETSI24enable;
																uiState[`ETSI24channel${i + 1}low` ].readOnly = !inst.ETSI24enable;
																uiState[`ETSI24channel${i + 1}high`].readOnly = !inst.ETSI24enable;
															});
														}
													},

													...create24ChannelConfigs(13,"ETSI24")
												]
											},
											{//JP24
												name: "JP24",
												displayName: "JP (dBm)",
												config: [
													{
														name: "JP24enable",
														displayName: "Override Firmware Defaults",
														default: false,
														onChange: (inst, uiState) => {
															_.times(13, (i) => {
																uiState[`JP24channel${i + 1}11b` ].readOnly = !inst.JP24enable;
																uiState[`JP24channel${i + 1}low` ].readOnly = !inst.JP24enable;
																uiState[`JP24channel${i + 1}high`].readOnly = !inst.JP24enable;
															});
														}
													},

													...create24ChannelConfigs(13,"JP24")
												]
											}

								],
								collapsed :true
							}
					],
					collapsed :false
				},
				{ //RF5G
					displayName: "5GHz Radio Settings ",
					description: "",
					config: [
								
								{
									name        : "Is5GEnabledSelect",
									displayName : "Support 5G",
									default     : "1",
									options     :[
										{name : "0" ,displayName : "Disable" 	},
										{name : "1"  ,displayName : "Enable"	}
									]
								},
								{
									name        : "PHY_5G_CAL_MODE",
									displayName : "PHY 5G Calibration Mode ",
									default     : "0",
									readOnly    : true,
									options     :[
										{name : "0" ,displayName : "Normal"	}
									]
								},
								{//RegDomain5G
									displayName : "5 GHz Regulatory Settings",
									longDescription:`Warning! Changing these values may defect regulatory radio certification or system performance!`,
									config: [
												{//FCC
													name: "FCC",
													displayName: "FCC (dBm)",
													config: [
															{
																name: "fccenable",
																displayName: "Override Firmware Defaults",
																default: false,
																onChange: (inst, uiState) => {
																	_.each(allChannelKeys(), (chan, index) => {
																		uiState[`FCCchannel${chan}`].readOnly = !inst.fccenable;
																		inst[`FCCchannel${chan}`] = defaults[index];
																	});
																}
															},
															...createChannelConfigs(defaults, "FCC"),
													]
												},
												{//ETSI
													name: "ETSI",
													displayName: "ETSI (dBm)",
													config: [
															{
																name: "etsienable",
																displayName: "Override Firmware Defaults",
																default: false,
																onChange: (inst, uiState) => {
																	_.each(allChannelKeys(), (chan, index) => {
																		uiState[`ETSIchannel${chan}`].readOnly = !inst.etsienable;
																		inst[`ETSIchannel${chan}`] = defaults[index];
																	});
																}
															},
															...createChannelConfigs(defaults, "ETSI"),
													]
												},
												{//JP
													name: "JP",
													displayName: "JP (dBm)",
													config: [
															{
																name: "jpenable",
																displayName: "Override Firmware Defaults",
																default: false,
																onChange: (inst, uiState) => {
																	_.each(allChannelKeys(), (chan, index) => {
																		uiState[`JPchannel${chan}`].readOnly = !inst.jpenable;
																		inst[`JPchannel${chan}`] = defaults[index];
																	});
																}
															},
															...createChannelConfigs(defaults, "JP"),
													]
												},
												{//ExtraBO
													name: "ExtraBO",
													displayName: "Extra Back Off",
													config: [
															{
																name: "exboenable",
																displayName: "Override Firmware Defaults",
																default: false,
																onChange: (inst, uiState) => {
																	_.each(allChannelKeys(), (chan, index) => {
																		uiState[`channel${chan}low`].readOnly = !inst.exboenable;
																		uiState[`channel${chan}high`].readOnly = !inst.exboenable;
																		//inst[`channel${chan}`] = defaults[index];
																	});
																	//console.log(uiState)
																}
															},
															...createExBOChannelConfigs(defaults),
													]
												},
												{ //Insertion Loss
													name: "IL",
													displayName: "Insertion Loss",
													config: [
															{
																name: "ilenable",
																displayName: "Override Firmware Defaults",
																default: false,
																onChange: (inst, uiState) => {
																	uiState.TX1.readOnly = !inst.ilenable;
																	uiState.TX2.readOnly = !inst.ilenable;
																	uiState.TX3.readOnly = !inst.ilenable;
																	uiState.TX4.readOnly = !inst.ilenable;
																	uiState.TX5.readOnly = !inst.ilenable;

																	uiState.RX1.readOnly = !inst.ilenable;
																	uiState.RX2.readOnly = !inst.ilenable;
																	uiState.RX3.readOnly = !inst.ilenable;
																	uiState.RX4.readOnly = !inst.ilenable;
																	uiState.RX5.readOnly = !inst.ilenable;

																	uiState.AntG1.readOnly = !inst.ilenable;
																	uiState.AntG2.readOnly = !inst.ilenable;
																	uiState.AntG3.readOnly = !inst.ilenable;
																	uiState.AntG4.readOnly = !inst.ilenable;
																	uiState.AntG5.readOnly = !inst.ilenable;
																	
																}
															},
															{
																name		: "U-NII1",
																displayName	: "U-NII1",
																config: [{
																			name: "TX1",
																			displayName: "TX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "RX1",
																			displayName: "RX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "AntG1",
																			displayName: "Antenna Gain (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																}],
																collapsed :true
															},
															{
																name		: "U-NII-2",
																displayName	: "U-NII-2",
																config: [{
																			name: "TX2",
																			displayName: "TX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "RX2",
																			displayName: "RX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "AntG2",
																			displayName: "Antenna Gain (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																}],
																collapsed :true
															},
															{
																name		: "U-NII-2C1",
																displayName	: "U-NII-2C1",
																config: [{
																			name: "TX3",
																			displayName: "TX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "RX3",
																			displayName: "RX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "AntG3",
																			displayName: "Antenna Gain (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																}],
																collapsed :true
															},
															{
																name		: "U-NII-2C2",
																displayName	: "U-NII-2C2",
																config: [{
																			name: "TX4",
																			displayName: "TX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "RX4",
																			displayName: "RX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "AntG4",
																			displayName: "Antenna Gain (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																}],
																collapsed :true
															},
															{
																name		: "U-NII-2C3",
																displayName	: "U-NII-2C3",
																config: [{
																			name: "TX5",
																			displayName: "TX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "RX5",
																			displayName: "RX (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																		}, {
																			name: "AntG5",
																			displayName: "Antenna Gain (dB)",
																			default: 0,
																			readOnly:true,
																			displayFormat: {
																				radix: "dec",
																				fixedPoint: 1,
																			},
																}],
																collapsed :true
															}
													]
												}	
									],
									collapsed :true
								}
								
						],
						
				},

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

	//console.log(inst)
	validateFCC24(inst, validation);
	validateETSI24(inst, validation);
	validateJP24(inst, validation);
	validateFCC(inst, validation);
	validateETSI(inst, validation);
	validateJP(inst, validation);
	validateBO (inst, validation);
	validateIL (inst, validation);
}

function validateIL(inst, vo){
	for ( let i = 1; i < 6; i++ ) {
		if (inst[`TX${i}`] < 0 || inst[`TX${i}`]  > 32) {
			vo.logError("Must be between 0 and +32", inst, "TX1");
		}
		if (inst[`RX${i}`] < 0 || inst[`RX${i}`]  > 32) {
			vo.logError("Must be between 0 and +32", inst, "RX1");
		}
		if (inst[`AntG${i}`] < 0 || inst[`AntG${i}`] > 32) {
			vo.logError("Must be between 0 and +32", inst, "AntG1");
		}
	}
}

function validateBO(inst, vo){
	_.each(allChannelKeys(), (chan, index) => {
		if (inst[`channel${chan}low`] < 0 || inst[`channel${chan}low`] > 6) {
			vo.logError("Must be between 0 and +6", inst, `channel${chan}low`);
		}
		if (inst[`channel${chan}high`] < 0 || inst[`channel${chan}high`] > 6) {
			vo.logError("Must be between 0 and +6", inst, `channel${chan}high`);
		}
	});
}

function validateFCC(inst, vo){
	_.each(allChannelKeys(), (chan, index) => {
		if (inst[`FCCchannel${chan}`] < 0 || inst[`FCCchannel${chan}`] > 32) {
			vo.logError("Must be between 0 and +32", inst, `FCCchannel${chan}`);
		}
	});
}

function validateETSI(inst, vo){
	_.each(allChannelKeys(), (chan, index) => {
		if (inst[`ETSIchannel${chan}`] < 0 || inst[`ETSIchannel${chan}`] > 32) {
			vo.logError("Must be between 0 and +32", inst, `ETSIchannel${chan}`);
		}
	});
}

function validateJP(inst, vo){
	_.each(allChannelKeys(), (chan, index) => {
		if (inst[`JPchannel${chan}`] < 0 || inst[`JPchannel${chan}`] > 32) {
			vo.logError("Must be between 0 and +32", inst, `JPchannel${chan}`);
		}
	});
}

/*function validateFCC24(inst, vo){

	_.times(13, (i) => {
		if (inst[`FCC24channel${i + 1}11b`] < -6 || inst[`FCC24channel${i + 1}11b`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}11b`);
		}
		if (inst[`FCC24channel${i + 1}low`] < -6 || inst[`FCC24channel${i + 1}low`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}low`);
		}
		if (inst[`FCC24channel${i + 1}high`] < -6 || inst[`FCC24channel${i + 1}high`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}high`);
		}
	});
}*/
function validateFCC24(inst, vo){

	_.times(13, (i) => {
		if (inst[`FCC24channel${i + 1}11b`] < -6 || inst[`FCC24channel${i + 1}11b`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}11b`);
		}
		if (inst[`FCC24channel${i + 1}low`] < -6 || inst[`FCC24channel${i + 1}low`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}low`);
		}
		if (inst[`FCC24channel${i + 1}high`] < -6 || inst[`FCC24channel${i + 1}high`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `FCC24channel${i + 1}high`);
		}
	});
}
function validateETSI24(inst, vo){

	_.times(13, (i) => {
		if (inst[`ETSI24channel${i + 1}11b`] < -6 || inst[`ETSI24channel${i + 1}11b`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `ETSI24channel${i + 1}11b`);
		}
		if (inst[`ETSI24channel${i + 1}low`] < -6 || inst[`ETSI24channel${i + 1}low`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `ETSI24channel${i + 1}low`);
		}
		if (inst[`ETSI24channel${i + 1}high`] < -6 || inst[`ETSI24channel${i + 1}high`] > 6) {
			vo.logError("Must be between -6 and +6", inst, `ETSI24channel${i + 1}high`);
		}
	});
}
	function validateJP24(inst, vo){

		_.times(13, (i) => {
			if (inst[`JP24channel${i + 1}11b`] < -6 || inst[`JP24channel${i + 1}11b`] > 6) {
				vo.logError("Must be between -6 and +6", inst, `JP24channel${i + 1}11b`);
			}
			if (inst[`JP24channel${i + 1}low`] < -6 || inst[`JP24channel${i + 1}low`] > 6) {
				vo.logError("Must be between -6 and +6", inst, `JP24channel${i + 1}low`);
			}
			if (inst[`JP24channel${i + 1}high`] < -6 || inst[`JP24channel${i + 1}high`] > 6) {
				vo.logError("Must be between -6 and +6", inst, `JP24channel${i + 1}high`);
			}
		});
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
