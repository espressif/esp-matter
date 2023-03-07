let topModules;
let displayName = "TI RTOS";
let description = "RTOS Global Settings";
let deviceId = system.deviceData.deviceId;

let cc13xx_cc26xx_topModules = [
    {
        displayName: displayName,
        description: description,
        "modules": [
            "/ti/sysbios/BIOS"
        ],
        "categories": [
            {
                "displayName" : "Core Kernel",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/knl/Clock",
                    "/ti/sysbios/knl/Event",
                    "/ti/sysbios/knl/Idle",
                    "/ti/sysbios/knl/Mailbox",
                    "/ti/sysbios/knl/Semaphore",
                    "/ti/sysbios/knl/Swi",
                    "/ti/sysbios/knl/Task"
                ]
            },
            {
                "displayName" : "Runtime",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/runtime/Assert",
                    "/ti/sysbios/runtime/Error",
                    "/ti/sysbios/runtime/Memory",
                    "/ti/sysbios/runtime/Startup",
                    "/ti/sysbios/runtime/SysCallback",
                    "/ti/sysbios/runtime/SysMin",
                    "/ti/sysbios/runtime/System",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Heaps",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/heaps/HeapBuf",
                    "/ti/sysbios/heaps/HeapCallback",
                    "/ti/sysbios/heaps/HeapMem",
                    "/ti/sysbios/heaps/HeapMin",
                    "/ti/sysbios/heaps/HeapMultiBuf",
                    "/ti/sysbios/heaps/HeapTrack"
                ]
            },
            {
                "displayName" : "Gates",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/gates/GateMutex",
                    "/ti/sysbios/gates/GateMutexPri",
                    "/ti/sysbios/gates/GateHwi",
                    "/ti/sysbios/gates/GateSwi",
                    "/ti/sysbios/gates/GateTask"
                ]
            },
            {
                "displayName" : "HAL",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/family/arm/m3/Hwi",
                    "/ti/sysbios/family/arm/cc26xx/Boot",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Utils",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/utils/Load"
                ]
            }
        ]
    }
];

let cc13x4_cc26x4_topModules = [
    {
        displayName: displayName,
        description: description,
        "modules": [
            "/ti/sysbios/BIOS"
        ],
        "categories": [
            {
                "displayName" : "Core Kernel",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/knl/Clock",
                    "/ti/sysbios/knl/Event",
                    "/ti/sysbios/knl/Idle",
                    "/ti/sysbios/knl/Mailbox",
                    "/ti/sysbios/knl/Semaphore",
                    "/ti/sysbios/knl/Swi",
                    "/ti/sysbios/knl/Task"
                ]
            },
            {
                "displayName" : "Runtime",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/runtime/Assert",
                    "/ti/sysbios/runtime/Error",
                    "/ti/sysbios/runtime/Memory",
                    "/ti/sysbios/runtime/Startup",
                    "/ti/sysbios/runtime/SysCallback",
                    "/ti/sysbios/runtime/SysMin",
                    "/ti/sysbios/runtime/System",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Heaps",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/heaps/HeapBuf",
                    "/ti/sysbios/heaps/HeapCallback",
                    "/ti/sysbios/heaps/HeapMem",
                    "/ti/sysbios/heaps/HeapMin",
                    "/ti/sysbios/heaps/HeapMultiBuf",
                    "/ti/sysbios/heaps/HeapTrack"
                ]
            },
            {
                "displayName" : "Gates",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/gates/GateMutex",
                    "/ti/sysbios/gates/GateMutexPri",
                    "/ti/sysbios/gates/GateHwi",
                    "/ti/sysbios/gates/GateSwi",
                    "/ti/sysbios/gates/GateTask"
                ]
            },
            {
                "displayName" : "HAL",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/family/arm/v8m/Hwi",
                    "/ti/sysbios/family/arm/cc26xx/Boot",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Utils",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/utils/Load"
                ]
            }
        ]
    }
];

let cc32xx_topModules = [
    {
        displayName: displayName,
        description: description,
        "modules": [
            "/ti/sysbios/BIOS"
        ],
        "categories": [
            {
                "displayName" : "Core Kernel",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/knl/Clock",
                    "/ti/sysbios/knl/Idle",
                    "/ti/sysbios/knl/Event",
                    "/ti/sysbios/knl/Mailbox",
                    "/ti/sysbios/knl/Semaphore",
                    "/ti/sysbios/knl/Swi",
                    "/ti/sysbios/knl/Task"
                ]
            },
            {
                "displayName" : "Runtime",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/runtime/Assert",
                    "/ti/sysbios/runtime/Error",
                    "/ti/sysbios/runtime/Memory",
                    "/ti/sysbios/runtime/Startup",
                    "/ti/sysbios/runtime/SysCallback",
                    "/ti/sysbios/runtime/SysMin",
                    "/ti/sysbios/runtime/System",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Heaps",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/heaps/HeapBuf",
                    "/ti/sysbios/heaps/HeapCallback",
                    "/ti/sysbios/heaps/HeapMem",
                    "/ti/sysbios/heaps/HeapMin",
                    "/ti/sysbios/heaps/HeapMultiBuf",
                    "/ti/sysbios/heaps/HeapTrack"
                ]
            },
            {
                "displayName" : "Gates",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/gates/GateMutex",
                    "/ti/sysbios/gates/GateMutexPri",
                    "/ti/sysbios/gates/GateTask",
                    "/ti/sysbios/gates/GateSwi",
                    "/ti/sysbios/gates/GateHwi"
                ]
            },
            {
                "displayName" : "HAL",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/family/arm/m3/Hwi",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Utils",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/utils/Load"
                ]
            }
        ]
    }
];

let cc23x0_topModules = [
    {
        displayName: displayName,
        description: description,
        "modules": [
            "/ti/sysbios/BIOS"
        ],
        "categories": [
            {
                "displayName" : "Core Kernel",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/knl/Clock",
                    "/ti/sysbios/knl/Idle",
                    "/ti/sysbios/knl/Event",
                    "/ti/sysbios/knl/Mailbox",
                    "/ti/sysbios/knl/Semaphore",
                    "/ti/sysbios/knl/Swi",
                    "/ti/sysbios/knl/Task"
                ]
            },
            {
                "displayName" : "Runtime",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/runtime/Assert",
                    "/ti/sysbios/runtime/Error",
                    "/ti/sysbios/runtime/Memory",
                    "/ti/sysbios/runtime/Startup",
                    "/ti/sysbios/runtime/SysCallback",
                    "/ti/sysbios/runtime/SysMin",
                    "/ti/sysbios/runtime/System",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Heaps",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/heaps/HeapBuf",
                    "/ti/sysbios/heaps/HeapCallback",
                    "/ti/sysbios/heaps/HeapMem",
                    "/ti/sysbios/heaps/HeapMin",
                    "/ti/sysbios/heaps/HeapMultiBuf",
                    "/ti/sysbios/heaps/HeapTrack"
                ]
            },
            {
                "displayName" : "Gates",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/gates/GateMutex",
                    "/ti/sysbios/gates/GateMutexPri",
                    "/ti/sysbios/gates/GateTask",
                    "/ti/sysbios/gates/GateSwi",
                    "/ti/sysbios/gates/GateHwi"
                ]
            },
            {
                "displayName" : "HAL",
                "expanded" : false,
                "modules": [
                    "/ti/sysbios/family/arm/v6m/Hwi",
                    "/ti/sysbios/family/arm/cc26xx/Boot",
                    "/ti/sysbios/runtime/Timestamp"
                ]
            },
            {
                "displayName" : "Utils",
                "expanded" : false,
                "modules" : [
                    "/ti/sysbios/utils/Load"
                ]
            }
        ]
    }
];

if (deviceId.match(/CC23.0/)) {
    topModules = cc23x0_topModules;
} else if (deviceId.match(/CC13.2|CC26.2/)) {
    /* CC13X2/CC26X2 */
    topModules = cc13xx_cc26xx_topModules;
} else if (deviceId.match(/CC13.4.*|CC26.4.*|CC2653.*/)) {
    /* CC13X4/CC26X4 */
    topModules = cc13x4_cc26x4_topModules;
} else if (deviceId.match(/CC32.*/)) {
    /* CC32XX */
    topModules = cc32xx_topModules;
} else {
    /* default to CC13X2 & CC26X2 */
    topModules = cc13xx_cc26xx_topModules;
}

/* append helper module for the execution graph */
topModules.push({
    displayName: "TI Utils Run-time",
    categories: [
        {
            displayName: "Execution Graph",
            modules: [ "/ti/sysbios/ExGr_SYSBIOS" ]
        }
    ]
});

let templates = [
    {
        "name": "/ti/sysbios/BIOS_config.h.xdt",
        "outputPath": "ti_sysbios_config.h",
        "alwaysRun": false
    },
    {
        "name": "/ti/sysbios/BIOS_config.c.xdt",
        "outputPath": "ti_sysbios_config.c",
        "alwaysRun": false
    }
];

let finalTopModules = [];

/*
 * If nortos or freertos is configured, omit the TI-RTOS part of the list. Since
 * it is needed for compatibility and tooling, continue to display the modules
 * in the UI if RTOS is "none" - but if the user references the BIOS module, it
 * will throw a validation error demanding the user switch to --rtos tirtos7.
 */
if (system.getRTOS() == "tirtos7" || system.getRTOS() == "none")
{
    finalTopModules = topModules;
}

exports = {
    displayName: "TI RTOS",
    topModules: finalTopModules,
    templates: templates
};
