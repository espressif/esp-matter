/**
 * @file
 * Handler for Command Class Manufacturer Specific.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_ManufacturerSpecific.h>
#include <ZW_TransportEndpoint.h>
#include <string.h>
#include <ZW_product_id_enum.h>
#include "config/CC_ManufacturerSpecific_config.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static received_frame_status_t
CC_ManufacturerSpecific_handler(
    RECEIVE_OPTIONS_TYPE_EX *rxOpt,
    ZW_APPLICATION_TX_BUFFER *pFrameIn,
    uint8_t cmdLength,
    ZW_APPLICATION_TX_BUFFER * pFrameOut,
    uint8_t * pLengthOut
)
{
  UNUSED(cmdLength);

  if (true == Check_not_legal_response_job(rxOpt))
  {
    // None of the following commands support endpoint bit addressing.
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  switch(pFrameIn->ZW_Common.cmd)
  {
    case MANUFACTURER_SPECIFIC_GET_V2:
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.cmdClass = COMMAND_CLASS_MANUFACTURER_SPECIFIC_V2;
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.cmd      = MANUFACTURER_SPECIFIC_REPORT_V2;

      uint16_t manufacturerID = 0;
      uint16_t productID      = 0;
      CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(&manufacturerID,
                                                              &productID);
      uint16_t productTypeID = CC_ManufacturerSpecific_get_product_type_id();

      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.manufacturerId1 = (uint8_t)(manufacturerID >> 8);
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.manufacturerId2 = (uint8_t)(manufacturerID &  0xFF);
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.productTypeId1  = (uint8_t)(productTypeID  >> 8);
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.productTypeId2  = (uint8_t)(productTypeID  &  0xFF);
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.productId1      = (uint8_t)(productID      >> 8);
      pFrameOut->ZW_ManufacturerSpecificReportV2Frame.productId2      = (uint8_t)(productID      &  0xFF);

      *pLengthOut = (uint8_t)sizeof(ZW_MANUFACTURER_SPECIFIC_REPORT_V2_FRAME);

      return RECEIVED_FRAME_STATUS_SUCCESS;
    case DEVICE_SPECIFIC_GET_V2:
      pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.cmdClass = COMMAND_CLASS_MANUFACTURER_SPECIFIC_V2;
      pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.cmd      = DEVICE_SPECIFIC_REPORT_V2;

      device_id_type_t deviceIDType       = NUMBER_OF_DEVICE_ID_TYPES;
      device_id_format_t deviceIDDataFormat = NUMBER_OF_DEVICE_ID_FORMATS;
      uint8_t deviceIDDataLength = 0;
      CC_ManufacturerSpecific_DeviceSpecificGet_handler(&deviceIDType,
                                                        &deviceIDDataFormat,
                                                        &deviceIDDataLength,
                                                        &pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.deviceIdData1);
      pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.properties1 = deviceIDType & 0x07;
      pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.properties2 = (uint8_t)(deviceIDDataFormat << 5) & 0xE0;
      uint8_t length_masked = deviceIDDataLength & 0x1F; // Silence conversion warning with separate line.
      pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.properties2 |= length_masked;

      *pLengthOut = sizeof(ZW_DEVICE_SPECIFIC_REPORT_1BYTE_V2_FRAME) + (pFrameOut->ZW_DeviceSpecificReport1byteV2Frame.properties2 & 0x1F) - 1;
      return RECEIVED_FRAME_STATUS_SUCCESS;
    default:
      return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }
}

REGISTER_CC_V2(COMMAND_CLASS_MANUFACTURER_SPECIFIC, MANUFACTURER_SPECIFIC_VERSION_V2, CC_ManufacturerSpecific_handler);
