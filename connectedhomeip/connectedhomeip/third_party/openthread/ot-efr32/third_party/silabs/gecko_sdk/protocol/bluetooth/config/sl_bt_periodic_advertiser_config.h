#ifndef SL_BT_PERIODIC_ADVERTISER_CONFIG_H
#define SL_BT_PERIODIC_ADVERTISER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bluetooth Periodic Advertiser Configuration

// <o SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS> Max number of advertising sets that support periodic advertising <0-255>
// <i> Default: 1
// <i> Define the number of periodic advertising sets that the application needs to use concurrently. This number must not exceed the total number of advertising sets configured by SL_BT_CONFIG_USER_ADVERTISERS in the component "bluetooth_feature_advertiser".
#define SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS     (1)

// </h> End Bluetooth Periodic Advertiser Configuration

// <<< end of configuration section >>>

#endif // SL_BT_PERIODIC_ADVERTISER_CONFIG_H
