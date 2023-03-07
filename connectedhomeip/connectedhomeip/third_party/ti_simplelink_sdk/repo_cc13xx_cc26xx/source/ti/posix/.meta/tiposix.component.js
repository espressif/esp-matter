
let topModules = [];

/* Only show the Settings module for tirtos7 */
if (system.getRTOS() == "tirtos7")
{
    topModules = [
        {
            displayName: "TI RTOS",
            description: "POSIX API support for TI-RTOS",
            "modules": [
                "/ti/posix/tirtos/Settings"
            ],
        }
    ]
}

exports = {
    displayName: "TI RTOS",
    topModules: topModules
};
