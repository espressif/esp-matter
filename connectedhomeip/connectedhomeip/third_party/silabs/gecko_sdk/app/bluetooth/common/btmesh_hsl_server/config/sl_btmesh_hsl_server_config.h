#ifndef SL_BTMESH_HSL_SERVER_CONFIG_H
#define SL_BTMESH_HSL_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> HSL Server configuration

// <o SL_BTMESH_HSL_SERVER_NVM_SAVE_TIME_CFG_VAL> Timeout [ms] for saving States of the model to NVM.
// <i> Default: 5000
// <i> Timeout [ms] for saving States of the model to NVM.
#define SL_BTMESH_HSL_SERVER_NVM_SAVE_TIME_CFG_VAL   (5000)

// <o SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL> PS Key for NVM Page where the States of the HSL Models are saved.
// <i> Default: 0x4008
// <i> PS Key for NVM Page where the States of the HSL Models are saved.
#define SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL   (0x4008)

// <o SL_BTMESH_HSL_SERVER_HUE_UPDATE_PERIOD_CFG_VAL> Periodicity [ms] for updating the hue during a transition.
// <i> Default: 1
// <i> Periodicity [ms] for updating the hue during a transition.
#define SL_BTMESH_HSL_SERVER_HUE_UPDATE_PERIOD_CFG_VAL   (1)

// <o SL_BTMESH_HSL_SERVER_SATURATION_UPDATE_PERIOD_CFG_VAL> Periodicity [ms] for updating the saturation during a transition.
// <i> Default: 1
// <i> Periodicity [ms] for updating the saturation during a transition.
#define SL_BTMESH_HSL_SERVER_SATURATION_UPDATE_PERIOD_CFG_VAL   (1)

// <o SL_BTMESH_HSL_SERVER_HUE_UI_UPDATE_PERIOD_CFG_VAL> Periodicity [ms] for updating the UI with hue during a transition.
// <i> Default: 100
// <i> Periodicity [ms] for updating the hue values on the UI.
#define SL_BTMESH_HSL_SERVER_HUE_UI_UPDATE_PERIOD_CFG_VAL   (100)

// <o SL_BTMESH_HSL_SERVER_SATURATION_UI_UPDATE_PERIOD_CFG_VAL> Periodicity [ms] for updating the UI with saturation during a transition.
// <i> Default: 100
// <i> Periodicity [ms] for updating the saturation values on the UI.
#define SL_BTMESH_HSL_SERVER_SATURATION_UI_UPDATE_PERIOD_CFG_VAL   (100)

// <o SL_BTMESH_HSL_SERVER_DEFAULT_HUE_CFG_VAL> Default Hue
// <i> Default: 0
// <i> Default Hue value.
#define SL_BTMESH_HSL_SERVER_DEFAULT_HUE_CFG_VAL   (0)

// <o SL_BTMESH_HSL_SERVER_DEFAULT_SATURATION_CFG_VAL> Default Saturation
// <i> Default: 0
// <i> Default Saturation.
#define SL_BTMESH_HSL_SERVER_DEFAULT_SATURATION_CFG_VAL   (0)

// <o SL_BTMESH_HSL_SERVER_MINIMUM_HUE_CFG_VAL> Minimum Hue
// <i> Default: 0
// <i> Minimum Hue.
#define SL_BTMESH_HSL_SERVER_MINIMUM_HUE_CFG_VAL   (0)

// <o SL_BTMESH_HSL_SERVER_MAXIMUM_HUE_CFG_VAL> Maximum Hue
// <i> Default: 65535
// <i> Maximum Hue.
#define SL_BTMESH_HSL_SERVER_MAXIMUM_HUE_CFG_VAL   (65535)

// <o SL_BTMESH_HSL_SERVER_MINIMUM_SATURATION_CFG_VAL> Minimum Saturation
// <i> Default: 0
// <i> Minimum Saturation.
#define SL_BTMESH_HSL_SERVER_MINIMUM_SATURATION_CFG_VAL   (0)

// <o SL_BTMESH_HSL_SERVER_MAXIMUM_SATURATION_CFG_VAL> Maximum Saturation
// <i> Default: 65535
// <i> Maximum Saturation.
#define SL_BTMESH_HSL_SERVER_MAXIMUM_SATURATION_CFG_VAL   (65535)

// <e SL_BTMESH_HSL_SERVER_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable / disable UART Logging for HSL Server models specific messages for this component.
#define SL_BTMESH_HSL_SERVER_LOGGING_CFG_VAL   (1)

// </e>

// </h>

// <<< end of configuration section >>>

// The hue update period shall not be greater than the hue UI update period
#if (SL_BTMESH_HSL_SERVER_HUE_UI_UPDATE_PERIOD_CFG_VAL) < (SL_BTMESH_HSL_SERVER_HUE_UPDATE_PERIOD_CFG_VAL)
#error "The SL_BTMESH_HSL_SERVER_HUE_UPDATE_PERIOD_CFG_VAL shall be less than SL_BTMESH_HSL_SERVER_HUE_UI_UPDATE_PERIOD_CFG_VAL."
#endif

// The saturation update period shall not be greater than the saturation UI update period
#if (SL_BTMESH_HSL_SERVER_SATURATION_UI_UPDATE_PERIOD_CFG_VAL) < (SL_BTMESH_HSL_SERVER_SATURATION_UPDATE_PERIOD_CFG_VAL)
#error "The SL_BTMESH_HSL_SERVER_SATURATION_UPDATE_PERIOD_CFG_VAL shall be less than SL_BTMESH_HSL_SERVER_SATURATION_UI_UPDATE_PERIOD_CFG_VAL."
#endif

#endif // SL_BTMESH_HSL_SERVER_CONFIG_H
