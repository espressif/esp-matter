/**
 * @file
 * Offers DCDC configuration for Silabs targets only.
 * @attention Must be linked for Silabs build targets only.
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <stdint.h>
#include "cmd_handlers.h"
#include "sl_dcdc.h"
#include "ZW_typedefs.h"
#include "ZW_SerialAPI.h"
#include "zpal_nvm.h"
#include "serialapi_file.h"

#define DCDC_CONFIG_UNALTERED   0xff

// 0x00 Auto, 0x01 Bypass, 0x02 LowNoise, 0xFF Original default

#define FILE_ID_DCDC_CONFIGURATION FILE_ID_PROPRIETARY_1

typedef struct
{
  uint8_t dcdc_config;
}
dcdc_configuration_file_t;

#define DCDC_CONFIGURATION_FILE_SIZE (sizeof(dcdc_configuration_file_t))

static dcdc_configuration_file_t dcdc_configuration_file;

bool SetDcDcConfig(uint8_t DcdcConfig)
{
  bool result = sl_dcdc_config_set(DcdcConfig);
  if (true == result)
  {
    dcdc_configuration_file.dcdc_config = DcdcConfig;
    zpal_status_t status = zpal_nvm_write(SerialAPIGetFileSystemHandle(),
                                          FILE_ID_DCDC_CONFIGURATION,
                                          &dcdc_configuration_file,
                                          DCDC_CONFIGURATION_FILE_SIZE);
    return (ZPAL_STATUS_OK == status);
  }
  return false;
}

uint8_t GetDcDcConfig(void)
{
  zpal_nvm_read(SerialAPIGetFileSystemHandle(), FILE_ID_DCDC_CONFIGURATION, &dcdc_configuration_file, DCDC_CONFIGURATION_FILE_SIZE);
  return dcdc_configuration_file.dcdc_config;
}

void SerialAPI_hw_psu_init(void)
{
  uint8_t dcdcConfig = GetDcDcConfig();
  if (DCDC_CONFIG_UNALTERED != dcdcConfig)
  {
    sl_dcdc_config_set(dcdcConfig);
  }
}

ZW_ADD_CMD(FUNC_ID_GET_DCDC_CONFIG)
{
  UNUSED(frame);
  uint8_t retVal = GetDcDcConfig();
  DoRespond(retVal);
}

ZW_ADD_CMD(FUNC_ID_SET_DCDC_CONFIG)
{
  uint8_t retVal = SetDcDcConfig(frame->payload[0]);
  DoRespond(retVal);
}
