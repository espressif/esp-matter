/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef _CC_MANUFACTURERSPECIFIC_CONFIG_H_
#define _CC_MANUFACTURERSPECIFIC_CONFIG_H_

#include "ZAF_types.h"
#include "ZW_product_id_enum.h"


#ifndef PRODUCT_TYPE_ID
#define PRODUCT_TYPE_ID     PRODUCT_TYPE_ID_ZWAVE_PLUS_V2
#endif

uint16_t CC_ManufacturerSpecific_get_product_type_id(void);

#endif /* _CC_MANUFACTURERSPECIFIC_CONFIG_H_ */
