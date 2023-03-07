#ifndef RAIL_PA_CONFIG_H
#define RAIL_PA_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o RAIL_PA_MODE> 2.4 GHz PA Mode
// <RAIL_TX_POWER_MODE_2P4_HP=> High Power
// <RAIL_TX_POWER_MODE_2P4_LP=> Low Power
// <i> Default: RAIL_TX_POWER_MODE_2P4_HP
#define RAIL_PA_MODE        RAIL_TX_POWER_MODE_2P4_HP

// <o RAIL_PA_VOLTAGE> PA Voltage
// <i> Default: 3300
#define RAIL_PA_VOLTAGE     3300

// <o RAIL_PA_RAMP_US> PA Ramp Time (us)
// <i> Default: 10
#define RAIL_PA_RAMP_US     10

// <o RAIL_PA_DEFAULT_POWER> PA Default Power
// <i> Default: 100
#define RAIL_PA_DEFAULT_POWER 100

// <<< end of configuration section >>>

// Map settings to config struct
#define RAIL_PA_2P4_CONFIG \
  {                        \
    RAIL_PA_MODE,          \
    RAIL_PA_VOLTAGE,       \
    RAIL_PA_RAMP_US,       \
  }

#endif // RAIL_PA_CONFIG_H
