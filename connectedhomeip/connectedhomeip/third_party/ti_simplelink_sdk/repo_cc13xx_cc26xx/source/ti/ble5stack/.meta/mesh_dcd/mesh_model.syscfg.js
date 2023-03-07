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
 *  ======== mesh_model.syscfg.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");
// Get the parameters of the default SIG model
const defaultSigModel = getDefaultSigModelParams("GEN_ONOFF_SRV");

/*
 *  ======== onModelTypeChange ========
 *  @param inst  - Module instance containing the config that changed
 *  @param ui    - The User Interface object
 */
function onModelTypeChange(inst,ui)
{
    if(inst.modelType =="Vendor"){
        ui.vendorID.hidden = false;
        ui.companyVendorID.hidden = false;
        ui.vendorName.hidden = false;
        ui.sigModelName.hidden = true;
        ui.numOfHandlers.hidden = false;
        ui.namePubCon.hidden  = false;
        ui.updateFunc.hidden  = false;
        ui.pubLength.hidden  = false;
        ui.modelCbStructName.hidden = false;
        ui.healthSrvMaxFaults.hidden = true;
        ui.userDataName.hidden = false;
        ui.userDataStructType.hidden = false;
        ui.userDataStructType.readOnly = false;
        inst.userDataName = "";
        inst.userDataStructType = "";
        inst.modelCbStructName = "";
    }else
    {
        ui.vendorID.hidden = true;
        ui.companyVendorID.hidden = true;
        ui.vendorName.hidden = true;
        ui.sigModelName.hidden = false;
        ui.numOfHandlers.hidden = true;
        ui.namePubCon.hidden  = true;
        ui.updateFunc.hidden  = true;
        ui.pubLength.hidden  = true;
        inst.userDataName = defaultSigModel.userData;
        inst.userDataStructType = defaultSigModel.userDataStruct;
        inst.modelCbStructName = defaultSigModel.modelCb;
    }

}

/*
 * ======== getDefaultSigModelParams ========
 * Returns the default SIG models model_cb
 * and user_data information
 *
 * @param sigModelName  - the default SIG model name
 *
 * @returns modelData - the data of the model
 */
function getDefaultSigModelParams(sigModelName)
{
    const SigModelData = system.getScript("/ti/ble5stack/ble_common.js").sigModelMapping;
    return { modelName:      sigModelName,
             userData:       SigModelData[sigModelName.toLowerCase()].defaultUserData,
             userDataStruct: SigModelData[sigModelName.toLowerCase()].userDataStructType,
             modelCb:        SigModelData[sigModelName.toLowerCase()].modelCbStructName};
}

/*
 *  ======== onSigModelChange ========
 *  @param inst  - Module instance containing the config that changed
 *  @param ui    - The User Interface object
 */
function onSigModelChange(inst,ui)
{
    const SigModelData = system.getScript("/ti/ble5stack/ble_common.js").sigModelMapping;

    if(inst.sigModelName =="HEALTH_SRV" || inst.sigModelName =="HEALTH_CLI"){
        ui.healthCbk.hidden = false;
        ui.modelCbStructName.hidden = true;
        ui.healthSrvMaxFaults.hidden = false;
        ui.userDataName.hidden = true;
        ui.userDataStructType.hidden = true;
        inst.healthCbk = SigModelData[inst.sigModelName.toLowerCase()].healthSrvCbk;
        inst.healthSrvMaxFaults = SigModelData[inst.sigModelName.toLowerCase()].maxFaults;
        inst.userDataName = "";
        inst.userDataStructType = "";
        inst.modelCbStructName = "";
    }
    else
    {
        ui.healthCbk.hidden = true;
        ui.modelCbStructName.hidden = false;
        ui.healthSrvMaxFaults.hidden = true;
        ui.userDataName.hidden = false;
        ui.userDataStructType.hidden = false;
        ui.userDataStructType.readOnly = true;
        inst.userDataName = SigModelData[inst.sigModelName.toLowerCase()].defaultUserData;
        inst.userDataStructType = SigModelData[inst.sigModelName.toLowerCase()].userDataStructType;
        inst.modelCbStructName = SigModelData[inst.sigModelName.toLowerCase()].modelCbStructName;
    }
}

function getSigModelData(inst)
{
    let tempList = [];
    // Get the sigModels DB
    const SigModelDataScript = system.getScript("/ti/ble5stack/ble_common.js").sigModelMapping;
    let namelist =Object.keys(SigModelDataScript);

    // Add all SIG Models that their isSupported value is true to the list that will be presented to the user
    // The Health Server model will not be added to the list of element 0 since it is part of the
    // foundation models that should not be removed (see mesh_dcd.syscfg.js)
    for(let i =0;i<namelist.length;i++){
        if(SigModelDataScript[namelist[i]]["isSupported"] &&
           !(inst.$ownedBy.$name.includes("0") && namelist[i].toUpperCase() == "HEALTH_SRV")){
            tempList.push({ displayName: SigModelDataScript[namelist[i]]["Name"], name: namelist[i].toUpperCase()});
        }
    }
    return tempList;
}

let config = [
    {
        name       : 'modelType',
        displayName: 'Model Type',
        default    : 'SIG',
        options    : [{ name: 'SIG'  },{ name: 'Vendor' }],
        description: "Model type - SIG Model | Vendor Model",
        onChange: onModelTypeChange,
    },
    {
        name       : 'companyVendorID',
        displayName: 'Compeny ID',
        displayFormat: "hex",
        displayFormat: {
            bitSize: 16,
            radix: "hex",
        },
        default      : 0x000D,
        description: "Company Identifier for this model",
        hidden      :true,
    },
    {
        name       : 'vendorID',
        displayName: 'Model ID',
        displayFormat: "hex",
        displayFormat: {
            bitSize: 16,
            radix: "hex",
        },
        default    : 0x0000,
        description: "Vendor Model Identifier",
        hidden      :true,
    },
    {
        name       : 'vendorName',
        displayName: 'Vendor Name',
        description: "Vendor name as shown in the code",
        default    : 'vnd',
        hidden      :true,
    },
    {
        name: "opcodeHandlers",
        displayName: "Opcode Handlers",
        config: [
            {
                name         : 'numOfHandlers',
                displayName  : 'Number of Handelers',
                default: 0,
                hidden: true,
                description: "The amount of the Model opcode handlers",
                options: [
                    {name: 0},
                    {name: 1},
                    {name: 2},
                    {name: 3},
                    {name: 4},
                    {name: 5},
                    {name: 6},
                    {name: 7},
                    {name: 8},
                    {name: 9},
                    {name: 10},
                    {name: 11},
                    {name: 12},
                    {name: 13},
                    {name: 14},
                    {name: 15},
                    {name: 16},
                    {name: 17},
                    {name: 18},
                    {name: 19},
                    {name: 20}
                ]
            },
        ]
    },{
        name: "modPub ",
        displayName: "models publication",
        longDescription: "The Model Publication state is a composite state that controls parameters of messages that are published by a model.\
        The state includes a Publish Address, a Publish Period, a Publish AppKey Index, a Publish Friendship Credential Flag, a Publish TTL, \
        a Publish Retransmission Count, and a Publish Retransmit Interval Steps. Within an element, each model has a separate instance of Model \
        Publication state. It is highly recommended that models defined by higher layer specifications use instances of the Model Publication \
        state to control the publishing of messages. ",
        config: [
            {
                name         : 'namePubCon',
                displayName  : 'Name Given To The Context Variable',
                default: "",
                description: "Variable name given to the context",
                hidden: true,
            },
            {
                name         : 'updateFunc',
                displayName  : 'Message Update Callback',
                default: "",
                description: "Optional message update callback (may be empty).",
                hidden: true,
            },
            {
                name         : 'pubLength',
                displayName  : 'Length Of Message',
                default: 0,
                description: "Length of the publication message",
                hidden: true,
            },
        ]
    },
    {
        name       : 'sigModelName',
        displayName: 'SIG Model Name',
        default    : defaultSigModel.modelName,
        hidden      :false,
        description: "List of SIG models supported by this tool",
        onChange: onSigModelChange,
        options: (inst) => {
            return getSigModelData(inst);
        }
    },
    {
        name       : "modelCbStructName",
        displayName: "Model Callback Structure Name",
        default    : defaultSigModel.modelCb,
        description: "bt_mesh_model_cb struct for the current model",
        hidden     : false,
    },
    {
        name       : "healthSrvMaxFaults",
        displayName: "Max Number Of Faults",
        default    : 0,
        description: "The maximum number of supported health server test faults",
        hidden     : true,
    },
    {
        name       : "healthCbk",
        displayName: "Health Callback",
        default    : "",
        description: "Name of the instantiation of bt_mesh_health_cb structure.",
        hidden     : true,
    },
    {
        name: "userDataName",
        displayName: "User Data Name",
        default: defaultSigModel.userData,
        description: "Variable name given to the user data",
        hidden: false,
    },
    {
        name: "userDataStructType",
        displayName: "User Data Struct Type",
        default: defaultSigModel.userDataStruct,
        description: "The user data struct type",
        readOnly: true,
        hidden: false
    }
];



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
    if(inst.modelType== 'Vendor'){
        for(let index = 1;index<= inst.numOfHandlers ;index++){
            // if(inst.opcodeHandlers.indexOf("handlers"+index)<-1){

            // }else{
            // }
            dependencyModule.push({
                name: "handler" + index,
                displayName: "Handler " + index,
                moduleName: "/ti/ble5stack/mesh_dcd/handlers",
                collapsed: true,
                group: "opcodeHandlers",
                args: {
                    numOfHandler    : index,
                    handlerOpCode   : index,
                    handlerFunction :"handler" + index
                }
            });
        }
    }
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
    if(inst.vendorID>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"vendorID");
    }
    if(inst.companyVendorID>65535){
        validation.logError("Valid range is 0 to 65,535 (0x0000 to 0xFFFF)",inst,"companyVendorID");
    }
    if(inst.modelType == "SIG" && !Common.alphanumeric(inst.healthCbk) && inst.healthCbk!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"healthCbk");
    }
    if(inst.modelType == "Vendor" && !Common.alphanumeric(inst.vendorName) && inst.vendorName!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"vendorName");
    }
    if(inst.modelType == "Vendor" && !Common.alphanumeric(inst.namePubCon) && inst.namePubCon!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"namePubCon");
    }
    if(inst.modelType == "Vendor" && !Common.alphanumeric(inst.updateFunc)  && inst.updateFunc!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"updateFunc");
    }
    if(!Common.alphanumeric(inst.modelCbStructName) && inst.modelCbStructName!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"modelCbStructName");
    }
    // The max possible size is 65,532 because max_faults field is uint16_t and 3 bytes are occupied by test_id and opcode.
    if(inst.healthSrvMaxFaults < 0 || inst.healthSrvMaxFaults > 65532){
        validation.logError("Valid range is 0-65532",inst,"healthSrvMaxFaults");
    }
    if(!Common.alphanumeric(inst.userDataName) && inst.userDataName!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"userDataName");
    }
    if(!Common.alphanumeric(inst.userDataStructType) && inst.userDataStructType!= ""){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"userDataStructType");
    }

    let elementMod  = system.modules['/ti/ble5stack/mesh_dcd/mesh_element'].$instances;
	let vendorArrayNoDup = [];
	let sigArrayNoDup = [];
	let modelObj = {};
	if(elementMod){

	for(let elemIndex=0;elemIndex<elementMod.length ;elemIndex++){
		if(elementMod[elemIndex].model){
			for(let modelIndex = 0 ; modelIndex < elementMod[elemIndex].model.length;modelIndex++){
				modelObj = elementMod[elemIndex].model[modelIndex];
				if(modelObj.modelType == "Vendor"){
					if(vendorArrayNoDup.indexOf(modelObj.vendorID)<0){
					vendorArrayNoDup.push(modelObj.vendorID);
					}else{
                        if(inst === modelObj){
                            validation.logError("This Vendor Id already exists in the current element" ,inst,"vendorID");
                        }
					}
				}else{//SIG MODEL
					if(sigArrayNoDup.indexOf(modelObj.sigModelName)<0){
						sigArrayNoDup.push(modelObj.sigModelName);
					}else{
                        if(inst === modelObj){
                            validation.logError("This SIG Model already exists in the current element",inst,"sigModelName");
                        }
					}
                }
            }
            vendorArrayNoDup = [];
	        sigArrayNoDup = [];
		}
	}
	}
}


// Define the common/portable base Watchdog
exports = {
    displayName          : "Model",
    description          : 'Model',
    defaultInstanceName  : 'Model_',
    config               : config,
    moduleInstances: moduleInstances,
    validate             : validate

};
