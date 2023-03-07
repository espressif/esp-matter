#ifndef THROUGHPUT_PERIPHERAL_CONFIG_H
#define THROUGHPUT_PERIPHERAL_CONFIG_H

#include "throughput_types.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Mode settings

// <o THROUGHPUT_PERIPHERAL_MODE_DEFAULT> Default test mode
//   <THROUGHPUT_MODE_CONTINUOUS=> Continuous mode
//   <THROUGHPUT_MODE_FIXED_LENGTH=> Fixed data size
//   <THROUGHPUT_MODE_FIXED_TIME=> Fixed time
// <i> Default: THROUGHPUT_MODE_CONTINUOUS
#define THROUGHPUT_PERIPHERAL_MODE_DEFAULT                  THROUGHPUT_MODE_CONTINUOUS

// <o THROUGHPUT_PERIPHERAL_FIXED_DATA_SIZE> Data size in bytes
// <i> Default: 100000
#define THROUGHPUT_PERIPHERAL_FIXED_DATA_SIZE            100000

// <o THROUGHPUT_PERIPHERAL_FIXED_TIME> Time in ms
// <i> Default: 10000
#define THROUGHPUT_PERIPHERAL_FIXED_TIME                 10000

// </h>

// <h> Power settings

// <o THROUGHPUT_PERIPHERAL_TX_POWER> Default maximum TX power in dBm
// <i> Default: 100
#define THROUGHPUT_PERIPHERAL_TX_POWER                      10

// <q THROUGHPUT_PERIPHERAL_TX_POWER_CONTROL_ENABLE> Adaptive power control
// <i> Default: 0
#define THROUGHPUT_PERIPHERAL_TX_POWER_CONTROL_ENABLE      0

// <q THROUGHPUT_PERIPHERAL_TX_SLEEP_ENABLE> Enable deep sleep
// <i> Default: 0
#define THROUGHPUT_PERIPHERAL_TX_SLEEP_ENABLE              0

// </h>

// <h> Data settings

// <o THROUGHPUT_PERIPHERAL_MTU_SIZE> Default MTU size <23-250>
// <i> Default: 247
#define THROUGHPUT_PERIPHERAL_MTU_SIZE                   247

// <o THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_INDICATIONS> Transfer size for indications <0-255>
// <i> Default: 0
// <i> If set to 0 or > MTU-3 then it will send MTU-3 bytes of data, otherwise it will use this value
#define THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_INDICATIONS               0
// <o THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_NOTIFICATIONS> Transfer size for notification <0-255>
// <i> Default: 0
// <i> If set to 0 or > MTU-3 then it will send MTU-3 bytes of data, otherwise it will use this value
#define THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_NOTIFICATIONS             0

// </h>

// <<< end of configuration section >>>

#endif // THROUGHPUT_PERIPHERAL_CONFIG_H
