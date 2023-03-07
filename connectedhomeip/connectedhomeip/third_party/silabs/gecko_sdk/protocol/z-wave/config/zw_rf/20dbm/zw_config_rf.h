/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef _ZW_CONFIG_RF_H_
#define _ZW_CONFIG_RF_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Max Tx Power LR Configuration

// <o> The maximum allowed Tx power in decibel-milliwatts for Z-Wave Long Range network [dBm] <140..200:1>
// <i> The maximum allowed Tx power in decibel-milliwatts for Z-Wave Long Range network.
// <i> Default: 200
#define APP_MAX_TX_POWER_LR         200

// <q> Enable Radio Debug
// <i> Enable Radio Debug
// <i> Default: 0
#define ENABLE_RADIO_DEBUG          0

// <o> The maximum allowed Tx power in decibel-milliwatts for Z-Wave network [dBm] <-10..140:1>
// <i> The maximum allowed Tx power in decibel-milliwatts for Z-Wave Long Range network.
// <i> Default: 0
#define APP_MAX_TX_POWER            0

// <o> The decibel-milliwatts output measured at a PA setting of 0dBm [dBm] <-50..50:1>
// <i> The decibel-milliwatts output measured at a PA setting of 0dBm.
// <i> Default: 0
#define APP_MEASURED_0DBM_TX_POWER  0

// </h>

// <<< end of configuration section >>>

#endif /* _ZW_CONFIG_RF_H_ */
