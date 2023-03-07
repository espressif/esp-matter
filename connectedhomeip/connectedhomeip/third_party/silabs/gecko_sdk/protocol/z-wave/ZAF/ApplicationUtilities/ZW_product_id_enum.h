/**
 * @file
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _PRODUCT_ID_ENUM_H_
#define _PRODUCT_ID_ENUM_H_

/**
 * Standard enum Product type Id
 */
typedef enum _PRODUCT_TYPE_ID_ENUM_
{
  PRODUCT_TYPE_ID_ZWAVE_ZIP_GATEWAY = 1,
  PRODUCT_TYPE_ID_ZWAVE,
  PRODUCT_TYPE_ID_ZWAVE_PLUS,
  PRODUCT_TYPE_ID_ZWAVE_PLUS_V2
}
eProductTypeID;


/**
 * Contains the product IDs for the Silabs Z-Wave 700 applications.
 */
typedef enum _PRODUCT_PLUS_ID_ENUM_
{
  PRODUCT_ID_DoorLockKeyPad = 0x0001,
  PRODUCT_ID_SwitchOnOff = 0x0002,
  PRODUCT_ID_SensorPIR = 0x0003,
  PRODUCT_ID_SerialAPI = 0x0004,
  PRODUCT_ID_PowerStrip = 0x0005,
  PRODUCT_ID_WallController = 0x0006,
  PRODUCT_ID_LEDBulb = 0x0007,
  PRODUCT_ID_MultilevelSensor = 0x0008,
  PRODUCT_ID_KeyFob = 0x0009
} eProductPlusID;

#endif /* _PRODUCT_ID_ENUM_H_ */
