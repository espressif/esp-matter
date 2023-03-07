/******************************************************************************

 @file  app_override_example.h

 @brief This file is an example of how application specific overrides can be
        pulled into the SysConfig/RadioConfig module.

        The path to this file must be specified in the argument "appOverride"
        supplied to code export configuration during instantiation. The argument
        "appOverrideMacro" is used to provide a macro names for use in the
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
                appOverride: "ti/devices/radioconfig/app_override_example.h",
                appOverrideMacro: "APP_OVERRIDES",
                cmdList_ble: []
            },
            permission: "ReadOnly"
        }
     });

    return(dependencyModule);
}

*******************************************************************************/

#ifndef _INCLUDE_APP_OVERRIDE_H_
#define _INCLUDE_APP_OVERRIDE_H_

// NB! The value of the configurable "appOverrideMacro" MUST correspond to the 
// macro name given below! Failing to do so so will cause compilation errors in the
// the generated file 'ti_radio_config.c'.

#define APP_OVERRIDES()\
    (uint32_t) 0xC0040361,\
    (uint32_t) 0x00000000

#endif // _INCLUDE_APP_OVERRIDE_H_
