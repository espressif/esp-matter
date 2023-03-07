#ifndef SL_BT_PERIODIC_ADV_CONFIG_H
#define SL_BT_PERIODIC_ADV_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <o SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS> Max number of advertising sets that support periodic advertising <0-255>
// <i> Default: 0
// <i> This configuration is only used in applications that use the Bluetooth HCI interface. If the application uses the Bluetooth stack API, all advertising sets are capable of periodic advertising when this component is included.
// <i>
// <i> Define the number of periodic advertising sets that the application needs to use concurrently. This number must not exceed the total number of advertising sets configured by SL_BT_CONFIG_USER_ADVERTISERS in the component "bluetooth_feature_advertiser".
#define SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS     (0)
// <<< end of configuration section >>>

#endif
