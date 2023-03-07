/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== mesh_dcd.syscfg.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
		name: "mesh_dcd",
		displayName: "Mesh DCD",
		description: "Device Composition Data",
		config: [
		{
			name         : 'cid',
			displayName  : 'Company ID',
			displayFormat: "hex",
			displayFormat: {
				bitSize: 16,
				radix: "hex",
			},
			default      : 0x000D,
			hidden		 : true,
		},
		{
			name         : 'pid',
			displayName  : 'Product ID',
			displayFormat: "hex",
			displayFormat: {
				bitSize: 16,
				radix: "hex",
			},
			default      : 0x0000,
			hidden		 : true,
			//displayFormat: 'hex'
		},
		{
			name         : 'vid',
			displayName  : 'Version Number',
			displayFormat: "hex",
			displayFormat: {
				bitSize: 16,
				radix: "hex",
			},
			hidden		 : true,
			default      : 0x0000
		},
		{
			name: "foundationModels",
			displayName: "Foundation Models",
			description: "Configure Foundation models",
			config: [
				{
					name: "configuration_server",
					displayName: "Configuration Server",
					description: "Configure Configuration Server Model",
					config: [
						{
							name         : 'default_ttl',
							displayName  : 'Default TTL',
							hidden		 : true,
							default      : 7,
							description	 : "The Default TTL state determines the TTL value used when sending messages."
						},
						{
							name         : 'net_transmit_count',
							displayName  : 'Network Transmission Count',
							hidden		 : true,
							description  : "Number of retransmissions (first transmission is excluded)",
							longDescription: "The Network Transmit state is a composite state that controls the number and timing of the transmissions \
							of Network PDU originating from a node. The state includes a Network Transmit Count field and a Network \
							Transmit Interval Steps field. There is a single instance of this state for the node. "+
							"\nThe Network Transmit Count field is a 3-bit value that controls the number of message transmissions of \
							the Network PDU originating from the node. The number of transmissions is the Transmit Count + 1. \
							For example a value of 0b000 represents a single transmission and a value of 0b111 represents 8 transmissions.",
							default      : 2
						},
						{
							name         : 'net_transmit_int_ms',
							displayName  : 'Network Transmission Interval Steps',
							hidden		 : true,
							description  : "Network Transmission Interval Steps(in milliseconds)",
							longDescription: "The Network Transmit Interval Steps field is a 5-bit value representing the number of \
							10 millisecond steps that controls the interval between message transmissions of Network PDUs originating \
							from the node. The transmission interval is calculated using the formula: \
							\ntransmission interval = (Network Retransmit Interval Steps + 1) * 10 \
							\nEach transmission should be perturbed by a random value between 0 to 10 milliseconds between each transmission. \
							For example, a value of 0b10000 represents a transmission interval between 170 and 180 milliseconds between each transmission.",
							default      : 20
						},
						{
							name         : 'relay_retransmit_count',
							displayName  : 'Relay Retransmit Count',
							hidden		 : true,
							description  : "Number of retransmissions (first transmission is excluded)",
							longDescription: "The Relay Retransmit state is a composite state that controls parameters of retransmission of the Network \
							PDU relayed by the node. The state includes a Relay Retransmit Count and a Relay Retransmit Interval Steps states.  \
							There is a single instance of this state for the node. "+
							"\nThe Relay Retransmit Count field is a 3-bit value that controls the number of message retransmissions of the Network \
							PDU relayed by the node. The Relay Retransmit Count + 1 is the number of times that packet is transmitted for \
							each packet that is relayed. For example, a value of 0b000 represents a single transmission with no retransmissions, \
							and a value of 0b111 represents a single transmission and 7 retransmissions for a total of 8 transmissions. ",
							default      : 3
						},
						{
							name         : 'relay_retransmit_int_ms',
							displayName  : 'Relay Retransmit Interval Steps',
							hidden		 : true,
							description  : "Relay Retransmit Interval Steps(in milliseconds)",
							longDescription: "The Relay Retransmit Interval Steps field is a 5-bit value representing the number of 10 millisecond \
							steps that controls the interval between message retransmissions of the Network PDU relayed by the node. \
							The retransmission interval is calculated using the formula: "+
							"\nretransmission interval = (Relay Retransmit Interval Steps + 1) * 10 ",
							default      : 20
						},
						{
							name         : 'trans_msg_seg_retrans_attempts',
							displayName  : 'Transport Message Segment Retransmit Attempts',
							hidden		 : true,
							description  : "Maximum number of transport message segment retransmit attempts.",
							longDescription: "Maximum number of transport message segment retransmit attempts \
	                        for outgoing segment message. Value ranges between 1 to 8",
							default      : 4
						},
						{
							name         : 'trans_msg_seg_retrans_int_unicast',
							displayName  : 'Transport message segment retransmit interval for unicast messages',
							hidden		 : true,
							description  : "Maximum time (in ms) of retransmit segment message to unicast address.",
							longDescription: "Maximum time (in ms) of retransmit segment message to unicast address. \
                            Valur ranges between 200 to 500 (ms). ",
							default      : 400
						},
						{
							name         : 'trans_msg_seg_retrans_int_group',
							displayName  : 'Transport message segment retransmit interval for group messages',
							hidden		 : true,
							description  : "Maximum time (in ms) of retransmit segment message to group address.",
							longDescription: "Maximum time (in ms) of retransmit segment message to group address. \
                            Valid ranges between 20 to 200 (ms)",
							default      : 50
						},

						{
							name: "mesh_hb_pub",
							displayName: "Heartbeat Publication Parameters",
							longDescription: "The Heartbeat Publication state is a composite state that controls sending of periodical Heartbeat transport control messages",
							config: [
								{
									name         : 'hb_pub_dst',
									displayName  : 'Destination address',
									hidden		 : true,
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									longDescription: "The Heartbeat Publication Destination state determines the destination address for Heartbeat messages.\n\
													The Heartbeat Publication Destination shall be the unassigned address,\n\
													a unicast address, or a group address, all other values are Prohibited. ",
									default      : 0
								},
								{
									name         : 'hb_pub_count',
									displayName  : 'Remaining Publish Count',
									hidden		 : true,
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									longDescription:"he Heartbeat Publication Count state is a 16-bit value that controls the number of periodical \n\
									Heartbeat transport control messages to be sent. When set to 0xFFFF, it is not decremented after\n\
									sending each Heartbeat message. When set to 0x0000, Heartbeat messages are not sent. When set\n\
									to a value greater than or equal to 0x0001 or less than or equal to 0xFFFE, it is decremented after\n\
									sending each Heartbeat message.\n\
									The Heartbeat Publication Count Log is a representation of the Heartbeat Publication Count value.\n\
									The Heartbeat Publication Count Log and Heartbeat Publication Count with the value 0x00 and 0x0000\n\
									are equivalent. The Heartbeat Publication Count Log value of 0xFF is equivalent to the Heartbeat \n\
									Publication count value of 0xFFFF. The Heartbeat Publication Count Log value between 0x01 and 0x11\n\
									shall represent that smallest integer n where 2(n-1) is greater than or equal to the Heartbeat \n\
									Publication Count value. For example, if the Heartbeat Publication Count value is 0x0579, then the \n\
									Heartbeat Publication Count Log value would be 0x0C. ",
									default      : 0
								},
								{
									name         : 'hb_pub_period',
									displayName  : 'Publication Period Log',
									description  : "Logarithmic publish interval in seconds",
									hidden		 : true,
									displayFormat: "hex",
									displayFormat: {
										bitSize: 8,
										radix: "hex",
									},
									longDescription: "The Heartbeat Publication Period Log state is an 8-bit value that controls the cadence of periodical\n\
													Heartbeat transport control messages. The value is represented as 2(n-1) seconds. For example, the \n\
													value 0x04 would have a publication period of 8 seconds, and the value 0x07 would have a publication \n\
													period of 64 seconds.",
									default      : 0
								},
								{
									name         : 'hb_pub_ttl',
									displayName  : 'Time To Live Value',
									hidden		 : true,
									displayFormat: "hex",
									displayFormat: {
										bitSize: 8,
										radix: "hex",
									},
									longDescription: "The Heartbeat Publication TTL state determines the TTL value used when sending Heartbeat messages.",
									default      : 0
								},
								{
									name         : 'hb_pub_feat',
									displayName  : 'Bitmap Of Features',
									longDescription  : "Bitmap of features that trigger a Heartbeat publication if they change."
												  +" Legal values are: FEAT_RELAY, FEAT_PROXY, FEAT_FRIEND and FEAT_LOW_POWER.",
									config: [
										{
											name: "hb_pub_feat_relay",
											displayName: "Relay",
											default: false,
											hidden: true
										},
										{
											name: "hb_pub_feat_proxy",
											displayName: "Proxy",
											default: false,
											hidden: true
										},
										{
											name: "hb_pub_feat_friend",
											displayName: "Friend",
											default: false,
											hidden: true
										},
										{
											name: "hb_pub_feat_low_power",
											displayName: "Low Power",
											default: false,
											hidden: true
										}
									],
								},
								{
									name         : 'hb_pub_net_idx',
									displayName  : 'Network Index Used For Publishing',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									hidden		 : true,
									default      : 0x0000
								},
							]
						},
						{
							name: "mesh_hb_sub",
							displayName: "Heartbeat Subscription parameters",
							longDescription: "The Heartbeat Subscription state is a composite state that controls receiving of periodical Heartbeat transport control messages.",
							config: [
								// {
								// 	name         : 'hb_sub_expiry',
								// 	displayName  : 'Subscription period exipration timestamp',
								// 	displayFormat: "hex",
								// 	displayFormat: {
								// 		bitSize: 64,
								// 		radix: "hex",
								// 	},
								// 	hidden		 : false,
								// 	default      : 0x0000000000000000
								// },
								{
									name         : 'hb_sub_src',
									displayName  : 'Source Address',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									longDescription: "The Heartbeat Subscription Source state determines the source address for Heartbeat messages a node \
									shall process. The Heartbeat Subscription Source shall be the unassigned address or a unicast address, \
									all other values are Prohibited. If the Heartbeat Subscription Source is set to the unassigned address, \
									the Heartbeat messages are not being processed. ",
									hidden		 : true,
									default      : 0x0000
								},
								{
									name         : 'hb_sub_dst',
									displayName  : 'Destination Address',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									longDescription: "The Heartbeat Subscription Destination state determines the destination address for Heartbeat messages.\
									This can be used by nodes to configure a proxy filter to allow them to receive Heartbeat messages, \
									for example, nodes connected using a GATT bearer or in a friendship. The Heartbeat Subscription Destination \
									shall be the unassigned address, the primary unicast address of the node, or a group address, all other \
									values are Prohibited. If the Heartbeat Subscription Destination is set to the unassigned address, the \
									Heartbeat messages are not being processed. ",
									hidden		 : true,
									default      : 0x0000
								},
								{
									name         : 'hb_sub_count',
									displayName  : 'Subscription Count State',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 16,
										radix: "hex",
									},
									longDescription: "The Heartbeat Subscription Count state is a 16-bit counter that controls the number of periodical \
									Heartbeat transport control messages received since receiving the most recent Config Heartbeat Subscription\
									Set message. The counter stops counting at 0xFFFF. ",
									hidden		 : true,
									default      : 0x0000
								},
								{
									name         : 'hb_sub_min_hops',
									displayName  : 'Minimum Hops In Received Messages',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 8,
										radix: "hex",
									},
									longDescription: "The Heartbeat Subscription Min Hops state determines the minimum hops value registered when receiving \
									Heartbeat messages since receiving the most recent Config Heartbeat Subscription Set message. ",
									hidden		 : true,
									default      : 0x00							},
								{
									name         : 'hb_sub_max_hops',
									displayName  : 'Maximum Hops In Received Messages',
									displayFormat: "hex",
									displayFormat: {
										bitSize: 8,
										radix: "hex",
									},
									longDescription: "The Heartbeat Subscription Max Hops state determines the maximum hops value registered when receiving \
									Heartbeat messages since receiving the most recent Config Heartbeat Subscription Set message. ",
									hidden		 : true,
									default      : 0x00
								},
								{
									name         : 'hb_sub_func',
									displayName  : 'Subscription Tracking Callback',
									description  : "Optional Heartbeat subscription tracking callback."
													+"Gets called on every received Heartbeat.",
									hidden		 : true,
									default      : "heartbeat"
								},
							]

						},
					]
				},
				{
					name: "configurationClient",
					displayName: "Configuration Client",
					hidden: true,
					default: false
				},
				{
					name: "healthServer",
					displayName: "Health Server",
					description: "Configure Health Server Model for Element 0",
					config: [
						{
							name         : 'healthSrvMaxFaultsFound',
							displayName  : 'Health Server Max Faults',
							hidden	     : true,
							default      : Common.sigModelMapping["health_srv"].maxFaults
						},
						{
							name         : 'healthSrvCbk',
							displayName  : 'Health Server Callback',
							hidden	     : true,
							default      : Common.sigModelMapping["health_srv"].healthSrvCbk
						}
					]
				},
			]
		}
    ]
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const dependencyModule = [];

	dependencyModule.push({
		name: "Element",
		displayName: "Elements",
		description: "Configure Elements",
		useArray: true,
		minInstanceCount: 1,
		moduleName: "/ti/ble5stack/mesh_dcd/mesh_element",
		collapsed: true,
		group: "mesh_dcd",
	});

    return(dependencyModule);
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - BLE instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
	//General
    if(inst.cid>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"cid");
    }
    if(inst.pid>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"pid");
	}
	if(inst.vid>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"vid");
	}
	//Configuration Server
	if(inst.default_ttl>255 || inst.default_ttl<1){
        validation.logError("Valid range is 1 to 255",inst,"default_ttl");
	}
	if(inst.relay_retransmit_count>255 || inst.relay_retransmit_count<0){
        validation.logError("Valid range is 0 to 255",inst,"relay_retransmit_count");
	}
	if(inst.net_transmit_count>255 || inst.net_transmit_count<0){
        validation.logError("Valid range is 0 to 255",inst,"net_transmit_count");
	}
	if(inst.net_transmit_int_ms>320 || inst.net_transmit_int_ms<10 || inst.net_transmit_int_ms%10 != 0 ){
        validation.logError("Must be greater than 0, less than or equal to 320, and a multiple of 10.",inst,"net_transmit_int_ms");
	}
	if(inst.relay_retransmit_int_ms>320 || inst.relay_retransmit_int_ms<10 || inst.relay_retransmit_int_ms%10 != 0 ){
        validation.logError("Must be greater than 0, less than or equal to 320, and a multiple of 10.",inst,"relay_retransmit_int_ms");
	}
	if(inst.trans_msg_seg_retrans_attempts>8 || inst.trans_msg_seg_retrans_attempts<1){
        validation.logError("Must be greater than 1, less than or equal to 8",inst,"trans_msg_seg_retrans_attempts");
	}
	if(inst.trans_msg_seg_retrans_int_unicast>500 || inst.trans_msg_seg_retrans_int_unicast<200){
        validation.logError("Must be greater than 200, less than or equal to 500",inst,"trans_msg_seg_retrans_int_unicast");
	}
	if(inst.trans_msg_seg_retrans_int_group>200 || inst.trans_msg_seg_retrans_int_group<20){
        validation.logError("Must be greater than 20, less than or equal to 200",inst,"trans_msg_seg_retrans_int_group");
	}
    //Health Server parameters
    // The max possible size is 65,532 because max_faults field is uint16_t and 3 bytes are occupied by test_id and opcode.
    if(inst.healthSrvMaxFaultsFound < 0 || inst.healthSrvMaxFaultsFound > 65532){
        validation.logError("Valid range is 0-65532",inst,"healthSrvMaxFaultsFound");
    }
    if(!Common.alphanumeric(inst.healthSrvCbk) && inst.healthSrvCbk!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"healthSrvCbk");
    }
	//Heartbeat Publication parameters
	if(inst.hb_pub_dst>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_pub_dst");
	}
	if(inst.hb_pub_count>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_pub_count");
	}
	if(inst.hb_pub_period>255){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_pub_period");
	}
	if(inst.hb_pub_ttl>255){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_pub_ttl");
	}
	if(inst.hb_pub_net_idx>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_pub_net_idx");
	}
	//Heartbeat Subscription parameters
	if(inst.hb_sub_src>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_sub_src");
	}
	if(inst.hb_sub_dst>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_sub_dst");
	}
	if(inst.hb_sub_count>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"hb_sub_count");
	}
	if(inst.hb_sub_min_hops>255 ){
        validation.logError("Valid range is 0 to 255",inst,"hb_sub_min_hops");
	}
	if(inst.hb_sub_min_hops>255 ){
        validation.logError("Valid range is 0 to 255",inst,"hb_sub_min_hops");
	}
	if(!Common.alphanumeric(inst.hb_sub_func) && inst.hb_sub_func!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"hb_sub_func");
    }
}

/*
 *  ======== exports ========
 *  Export the BLE RF Settings Configuration
 */
exports = {
    config: config,
	moduleInstances: moduleInstances,
	displayName          : 'Mesh DCD',
	description          : 'Mesh DCD',
	defaultInstanceName  : 'Mesh_DCD',
	maxInstances         : 100,
	validate: validate
};
