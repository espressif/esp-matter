"use strict";
const Common = system.getScript("/ti/ble5stack/ble_common.js");
let gServiceIndex = 0;

// Function to check if a string contains only letters, numbers and underscore.
function alphanumeric(inputtxt)
{
	var letterNumber = /^[a-zA-Z0-9_]+$/;
	if(inputtxt.match(letterNumber))
	{
	   return true;
	}
	else
	{
	   return false;
	}
}
/**
 * Validate this module's configuration
 *
 * @param {object} inst - Watchdog instance to be validated
 * @param {object} vo   - Issue reporting object
 */
function validate(inst, vo)
{
	 // Validate uuid Value length
	if(inst.uuidSize == '16-bit')
	{
		if(inst.uuid.toString(16).length > 4)
		{
			vo["uuid"].errors.push("Uuid is bigger than 16 bit.");
		}
	}

	if(inst.uuidSize == '128-bit')
	{
		if(inst.uuid.toString(16).length > 16)
		{
			vo["uuid"].errors.push("Uuid is bigger than 128 bit.");
		}
	}

	if(inst.userReadCBfunc){
		if(!alphanumeric(inst.userReadCBfunc)){
			vo["userReadCBfunc"].errors.push("Read attribute CB function name should only contain alphabet or digits.");
		}
	}
	if(inst.userWriteCBfunc){
		if(!alphanumeric(inst.userWriteCBfunc)){
			vo["userWriteCBfunc"].errors.push("Write attribute CB function name should only contain alphabet or digits.");
		}
	}
	if(inst.name == ''){
		vo["name"].errors.push("Please insert service name.");
	}
	if(inst.name)
	{
		if(!alphanumeric(inst.name))
		{
			vo["name"].errors.push("Characteristic name should only contain alphabet or digits.");
		}
	}

	//check for dupicate UUID
	let serviceMod  = system.modules['/ti/ble5stack/gatt_services/Service'];
	let counter = 0;
	for (let i = 0; i < serviceMod.$instances.length; i++) {
		if(serviceMod.$instances[i].uuid == inst.uuid){
			counter ++;
			if(counter > 1){
				vo["uuid"].errors.push("This UUID is taken by another service/characteristic.");
				break;
			}
		}
		for (let cidx = 0; cidx < serviceMod.$instances[i].characteristics.length; ++ cidx) {
			if(serviceMod.$instances[i].characteristics[cidx].uuid == inst.uuid) {
				counter ++;
				if(counter > 1) {
					vo["uuid"].errors.push("This UUID is taken by another service/characteristic.");
					break;
				}
			}
		}
	}
	
	//check for duplicate CB name- in the same service
	let counterWriteCB = 0;
	let counterReadCB = 0;
	let counterAuthoCB = 0;
	if(inst.userReadCBfunc.toLowerCase() == inst.userWriteCBfunc.toLowerCase() && inst.userReadCBfunc != "")
	{
        counterReadCB++;
        counterWriteCB++;
	}	
    if(inst.userReadCBfunc.toLowerCase() == inst.userAuthorizationCBfunc.toLowerCase() && inst.userAuthorizationCBfunc != "")
    {
        counterReadCB++;
        counterAuthoCB++;
    }
    if(inst.userWriteCBfunc.toLowerCase()  == inst.userAuthorizationCBfunc.toLowerCase() && inst.userWriteCBfunc != "")
    {
        counterWriteCB++;
        counterAuthoCB++;
    }
    if(counterAuthoCB > 0){ vo["userAuthorizationCBfunc"].errors.push("This function is already in use in another callback."); }
    if(counterReadCB > 0){ vo["userReadCBfunc"].errors.push("This function is already in use in another callback."); }
    if(counterWriteCB > 0){ vo["userWriteCBfunc"].errors.push("This function is already in use in another callback."); }


	//check duplicate CB function between other service
    for (let i = 0; i < serviceMod.$instances.length; i++) {
        if(!(inst === serviceMod.$instances[i]) && inst.userReadCBfunc != "" &&
          (serviceMod.$instances[i].userWriteCBfunc.toLowerCase() == inst.userReadCBfunc.toLowerCase() ||
           serviceMod.$instances[i].userAuthorizationCBfunc.toLowerCase() == inst.userReadCBfunc.toLowerCase())){
                vo["userReadCBfunc"].errors.push("This function is already in use by another service.");
                break;
        }
    }

    for (let i = 0; i < serviceMod.$instances.length; i++) {
        if(!(inst === serviceMod.$instances[i]) && inst.userWriteCBfunc != "" && 
          (serviceMod.$instances[i].userReadCBfunc.toLowerCase() == inst.userWriteCBfunc.toLowerCase() ||
           serviceMod.$instances[i].userAuthorizationCBfunc.toLowerCase() == inst.userWriteCBfunc.toLowerCase())){
                vo["userWriteCBfunc"].errors.push("This function is already in use by another service.");
                break;
        }
    }

    for (let i = 0; i < serviceMod.$instances.length; i++) {
        if(!(inst === serviceMod.$instances[i]) && inst.userAuthorizationCBfunc != "" && 
          (serviceMod.$instances[i].userReadCBfunc.toLowerCase() == inst.userAuthorizationCBfunc.toLowerCase() ||
           serviceMod.$instances[i].userWriteCBfunc.toLowerCase() == inst.userAuthorizationCBfunc.toLowerCase())){
                vo["userAuthorizationCBfunc"].errors.push("This function is already in use by another service.");
                break;
        }
    }

    //check for duplicate service name
    counter = 0;
    for (let i = 0; i < serviceMod.$instances.length; i++) {
        if(serviceMod.$instances[i].name.toLowerCase() == inst.name.toLowerCase()){
            counter ++;
            if(counter > 1){
                vo["name"].errors.push("This name is taken by another service/characteristic.");
                break;
            }
        }

        for (let cidx = 0; cidx < serviceMod.$instances[i].characteristics.length; ++ cidx) {
            if(serviceMod.$instances[i].characteristics[cidx].name.toLowerCase() == inst.name.toLowerCase()) {
                counter ++;
                if(counter > 1) {
                    vo["name"].errors.push("This name is taken by another service/characteristic.");
                    break;
                }
            }
        }
    }
}

/*
 *  ======== updateName ========
 *  @param inst  - Module instance containing the config that changed
 */
function updateName(inst)
{
  inst.name = inst.$name;
}

let config = [
    {
        name         : 'name',
        displayName  : 'Service Name',
        default      : ""
    },
    {
        name       : 'hiddenServName',
        displayName: 'Hidden Service Name',
        hidden	   : true,
        default    : '',
        onChange   : updateName
    },
    {
        name       : 'numOfServices',
        default	   : 0,
        hidden	   : true,
        onChange   : updateNumOfServices
    },
    {
        name		: 'serviceType',
        displayName	: 'Service Type',
        default		: 'Primary Service',
        options		: [
            {name: 'Primary Service'},
            {name: 'Secondary Service'},
        ]
    },
    {
        name       : 'uuidSize',
        displayName: 'Service UUID Size',
        default    : '16-bit',
        options    : [
            { name: '16-bit'  },
            { name: '128-bit' }
        ]
    },
    {
        name         : 'uuid',
        displayName  : 'Service UUID',
        default      : 0,
        displayFormat: 'hex'
    },
    {
        name         : 'userWriteCBfunc',
        displayName  : 'Write Attribute CB Function',
        default      : '',
        placeholder : "Optional- Enter a function name to enable",
    },
    {
        name         : 'userReadCBfunc',
        displayName  : 'Read Attribute CB Function',
        default      : '',
        placeholder : "Optional- Enter a function name to enable",
    },
    {
        name         : 'userAuthorizationCBfunc',
        displayName  : 'Authorization CB Function',
        default      : '',
        placeholder : "Optional- Enter a function name to enable",
    },
];

function updateNumOfServices(inst,ui)
{
	inst.uuid = parseInt((256 +gServiceIndex*256).toString(16), 16);
	gServiceIndex++;
	if(inst.serviceExamples == "Custom Service"){
		inst.name = inst.$name;
	}

}
function moduleInstances(inst) {
    return [
        {
            name            : 'characteristics',
            displayName     : inst.$name + ' - Characteristic',
            useArray        : true,
            moduleName      : '/ti/ble5stack/gatt_services/Characteristic',
            collapsed       : false,
            args            : {
                hiddenUuid	: (gServiceIndex).toString(),
                hiddenName  : "1",
            }
        }
    ];
}

// Define the common/portable base Watchdog
exports = {
    displayName          : 'Services',
    description          : 'Service',
    defaultInstanceName  : 'Service_',
    config               : config,
    validate             : validate,
    maxInstances         : 100,
    moduleInstances      : moduleInstances,
    gServiceIndex		 : gServiceIndex,
    templates            : {
        "/ti/ble5stack/templates/ble_gatt_service.h.xdt":
        "/ti/ble5stack/templates/ble_gatt_service.h.xdt",

        "/ti/ble5stack/templates/ble_gatt_service.c.xdt":
        "/ti/ble5stack/templates/ble_gatt_service.c.xdt"
    }
};
