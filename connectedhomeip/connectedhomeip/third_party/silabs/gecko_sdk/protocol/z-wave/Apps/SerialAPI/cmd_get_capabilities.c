/**
 * @file
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <stdint.h>
#include "cmd_handlers.h"
#include "config_app.h"
#include "serialappl.h"

#define CAPABILITIES_SIZE (8 + 32) // Info + supported commands

static uint8_t SERIALAPI_CAPABILITIES[CAPABILITIES_SIZE] = {
    APP_VERSION,
    APP_REVISION,
    SERIALAPI_MANUFACTURER_ID1,
    SERIALAPI_MANUFACTURER_ID2,
    SERIALAPI_MANUFACTURER_PRODUCT_TYPE1,
    SERIALAPI_MANUFACTURER_PRODUCT_TYPE2,
    SERIALAPI_MANUFACTURER_PRODUCT_ID1,
    SERIALAPI_MANUFACTURER_PRODUCT_ID2
};

static bool add_cmd_to_capabilities(cmd_handler_map_t const * const p_cmd_entry, cmd_context_t context)
{
  ZW_NodeMaskSetBit(context, p_cmd_entry->cmd);
  return false;
}

ZW_ADD_CMD(FUNC_ID_SERIAL_API_GET_CAPABILITIES)
{
  cmd_foreach(add_cmd_to_capabilities, &SERIALAPI_CAPABILITIES[8]);
  /* HOST->ZW: no params defined */
  /* ZW->HOST: RES | 0x07 | */
  /*  SERIAL_APPL_VERSION | SERIAL_APPL_REVISION | SERIALAPI_MANUFACTURER_ID1 | SERIALAPI_MANUFACTURER_ID2 | */
  /*  SERIALAPI_MANUFACTURER_PRODUCT_TYPE1 | SERIALAPI_MANUFACTURER_PRODUCT_TYPE2 | */
  /*  SERIALAPI_MANUFACTURER_PRODUCT_ID1 | SERIALAPI_MANUFACTURER_PRODUCT_ID2 | FUNCID_SUPPORTED_BITMASK[] */
  Respond(frame->cmd, SERIALAPI_CAPABILITIES, sizeof(SERIALAPI_CAPABILITIES));
}
