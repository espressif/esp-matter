/******************************************************************************

 @file  stack_override_example.h

 @brief This file is an example of how stack specific overrides can be
        pulled into the SysConfig/RadioConfig module.

        The path to this file must be specified in the argument 'stackOverride'
        supplied to code export configuration during instantiation. The argument
        "stackOverrideMacro" is used to provide a macro names for use in the
        common override table.

 Example of usage in SysConfig:

 function moduleInstances(inst)
 {
    const dependencyModule = [];

    dependencyModule.push({
        name: "radioConfig",
        group: "bleRadioConfig",
        displayName: "BLE Radio Configuration",
        description: "The BLE module is not using RF_Mode from ti_radio_config.c",
        moduleName: "/ti/devices/radioconfig/settings/ble",
        collapsed: true,
        args: {
            $name: "RF_BLE_Setting",
            phyType: "bt5le1m",
            codeExportConfig: {
                symGenMethod: "Custom",
                rfMode: "RF_modeBle",
                txPower: "RF_BLE_txPowerTable",
                txPowerSize: "RF_BLE_TX_POWER_TABLE_SIZE",
                overrides: "pOverrides_ble",
                stackOverride: "ti/devices/radioconfig/stack_override_example.h",
                stackOverrideMacro: "BLE_STACK_OVERRIDES",
                cmdList_ble: []
            },
            permission: "ReadOnly"
        }
     });

    return(dependencyModule);
}

*******************************************************************************/

#ifndef _INCLUDE_STACK_OVERRIDE_H_
#define _INCLUDE_STACK_OVERRIDE_H_

// NB! The value of the configurable "stackOverrideMacro" MUST correspond to the 
// macro name given below! Failing to do so so will cause compilation errors in the
// the generated file 'ti_radio_config.c'.

#ifdef DONT_TRANSMIT_NEW_RPA
#define OVR_1 0x00158B73
#else
#define OVR_1 0x00178B73
#endif

#define BLE_STACK_OVERRIDES()\
    (uint32_t) OVR_1,\
    (uint32_t) 0xC0040361,\
    (uint32_t) 0x00000000

#endif // _INCLUDE_STACK_OVERRIDE_H_
