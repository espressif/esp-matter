/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef APPLICATION_PROPERTIES_CONFIG_H_
#define APPLICATION_PROPERTIES_CONFIG_H_

#include "application_properties.h"
#include "config_app.h"

#define SL_APPLICATION_SIGNATURE               APPLICATION_SIGNATURE_NONE
#define SL_APPLICATION_SIGNATURE_LOCATION      0x0
#define SL_APPLICATION_TYPE          APPLICATION_TYPE_ZWAVE
#define SL_APPLICATION_VERSION       (APP_VERSION<<16 | APP_REVISION<<8 | APP_PATCH)
#define SL_APPLICATION_CAPABILITIES   0UL
#define SL_APPLICATION_PRODUCT_ID {(APP_MANUFACTURER_ID>>8) & 0xFF,(APP_MANUFACTURER_ID>>0) & 0xFF, \
						           (APP_PRODUCT_ID>>8) & 0xFF, (APP_PRODUCT_ID>>0) & 0xFF,          \
						           0x00, 0x00,                                                      \
						           0x00, 0x00,                                                      \
						           (PRODUCT_TYPE_ID>>8) & 0xFF, (PRODUCT_TYPE_ID>>0) & 0xFF,\
						           (APP_ICON_TYPE>>8) & 0xFF, (APP_ICON_TYPE>>0) & 0xFF,            \
						           (GENERIC_TYPE>>0) & 0xFF, (GENERIC_TYPE>>0) & 0xFF,              \
						           ((SPECIFIC_TYPE) & 0xFF),                                        \
						           ((REQUESTED_SECURITY_KEYS >> 0) & 0xFF)}              


#endif /* APPLICATION_PROPERTIES_CONFIG_H_ */
