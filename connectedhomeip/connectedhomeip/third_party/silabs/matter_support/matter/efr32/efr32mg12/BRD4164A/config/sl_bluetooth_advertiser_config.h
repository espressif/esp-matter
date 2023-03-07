#ifndef SL_BT_ADVERTISER_CONFIG_H
#define SL_BT_ADVERTISER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <o SL_BT_CONFIG_USER_ADVERTISERS> Max number of advertising sets reserved for user <0-255>
// <i> Default: 1
// <i> Define the number of advertising sets that the application needs to use concurrently. Note that all types of advertising uses the same pool of advertising sets, but periodic advertising has extra configuration to define the number of advertising sets that are capable of periodic advertising.
// <i>
// <i> Specifically, if the component "bluetooth_feature_periodic_advertiser" is used, its configuration SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS specifies how many of the SL_BT_CONFIG_USER_ADVERTISERS advertising sets are capable of periodic advertising. Similarly, if the component bluetooth_feature_pawr_advertiser is used, its configuration SL_BT_CONFIG_MAX_PAWR_ADVERTISERS specifies how many of the periodic advertising sets are capable of Periodic Advertising with Responses.
// <i>
// <i> The configuration values must satisfy the condition SL_BT_CONFIG_USER_ADVERTISERS >= SL_BT_CONFIG_MAX_PERIODIC_ADVERTISERS >= SL_BT_CONFIG_MAX_PAWR_ADVERTISERS.
#define SL_BT_CONFIG_USER_ADVERTISERS     (1)
// <<< end of configuration section >>>

#endif
