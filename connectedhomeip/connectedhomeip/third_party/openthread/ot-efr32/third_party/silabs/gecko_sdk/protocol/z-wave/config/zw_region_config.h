/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef _ZW_REGION_CONFIG_H_
#define _ZW_REGION_CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Z-Wave Radio Configuration
// <o ZW_REGION> The supported region of the device. Used only if region value is not previously written in MFG_ZWAVE_COUNTRY_FREQ token. To delete value from MFG token, run: commander flash --tokengroup znet --token MFG_ZWAVE_COUNTRY_FREQ:0xFF
// <REGION_DEFAULT=> Default (Europe)
// <REGION_ANZ=> Australia / New Zealand
// <REGION_CN=> China
// <REGION_EU=> Europe
// <REGION_HK=> Hong Kong
// <REGION_IL=> Israel
// <REGION_IN=> India
// <REGION_JP=> Japan
// <REGION_KR=> Korea
// <REGION_RU=> Russia
// <REGION_US=> United States
// <REGION_US_LR=> United States Long Range
// <i> Default: REGION_DEFAULT
#ifndef ZW_REGION
#define ZW_REGION      REGION_DEFAULT
#endif
// </h>

// <<< end of configuration section >>>

#endif /* _ZW_REGION_CONFIG_H_ */
