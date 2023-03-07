/******************************************************************************

@file  sensor_models.h

@brief  sensor model defines and API

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

#ifndef SENSOR_MODELS_H_
#define SENSOR_MODELS_H_

/*********************************************************************
* INCLUDES
*/
#include <bluetooth/mesh.h>

/*********************************************************************
* CONSTANTS
*/
#define TEMPERATURE_SENSOR_PROPERTY_ID  0x0056 // Indoor Ambient Temperature, Temperature 8
#define UNKNOWN_SENSOR_PROPERTY_ID      0x0042 // Unknown/unsupported
#define ALL_SENSOR_PROPERTY_ID          0x0000 // all sensors (internally code)

/* Sensor Model Operation Codes */
#define BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET      BT_MESH_MODEL_OP_2(0x82, 0x30)
#define BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS   BT_MESH_MODEL_OP_1(0x51)
#define BT_MESH_MODEL_OP_SENSOR_GET                 BT_MESH_MODEL_OP_2(0x82, 0x31)
#define BT_MESH_MODEL_OP_SENSOR_STATUS              BT_MESH_MODEL_OP_1(0x52)
#define BT_MESH_MODEL_OP_SENSOR_COLUMN_GET          BT_MESH_MODEL_OP_2(0x82, 0x32)
#define BT_MESH_MODEL_OP_SENSOR_COLUMN_STATUS       BT_MESH_MODEL_OP_1(0x53)
#define BT_MESH_MODEL_OP_SENSOR_SERIES_GET          BT_MESH_MODEL_OP_2(0x82, 0x33)
#define BT_MESH_MODEL_OP_SENSOR_SERIES_STATUS       BT_MESH_MODEL_OP_1(0x54)

/* Sensor Setup Model Operation Codes */
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_GET         BT_MESH_MODEL_OP_2(0x82, 0x34)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_SET         BT_MESH_MODEL_OP_1(0x55)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK   BT_MESH_MODEL_OP_2(0x00, 0x56)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_STATUS      BT_MESH_MODEL_OP_1(0x57)
#define BT_MESH_MODEL_OP_SENSOR_SETTINGS_GET        BT_MESH_MODEL_OP_2(0x82, 0x35)
#define BT_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS     BT_MESH_MODEL_OP_1(0x58)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_GET         BT_MESH_MODEL_OP_2(0x82, 0x36)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_SET         BT_MESH_MODEL_OP_1(0x59)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK   BT_MESH_MODEL_OP_1(0x5A)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_STATUS      BT_MESH_MODEL_OP_1(0x5B)

#define NET_BUF_LEN(buf) (buf->len)
#define NET_BUF_TEST_U8(buf) (buf->data[0])

#define SENSOR_MODELS_STATUS_SUCCESS         0
#define SENSOR_MODELS_ERROR                 -1
#define SENSOR_MODELS_ERROR_NO_RESOURCES    -2
#define SENSOR_MODELS_ERROR_SENSOR_LEN      -3

/*********************************************************************
* TYPEDEFS
*/
/*
 * Descriptor structures
 */
typedef struct
{
    uint16_t propertyId;
} sensorDescGet_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
  uint16_t propertyId;
  uint32_t PositiveTolerance:12;
  uint32_t NegativeTolerance:12;
  uint32_t  SamplingFunction:8;
  uint8_t  MeasurementPeriod;
  uint8_t  UpdateInterval;
} sensorDescriportState_t;

/*
 * Data structures
 */
typedef struct
{
    uint16_t propertyId;
} sensorDataGet_t;

typedef enum
{
  SENSOR_HDR_FORMAT_A,
  SENSOR_HDR_FORMAT_B
} sensorHdrFormat_e;

#define SENSOR_DATA_HDR_FORMAT_MASK  0x01
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t format:1;
    uint16_t length:4;
    uint16_t propertyId:11;
} sensorHdrA_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint8_t format:1;
    uint8_t length:7;
    uint16_t propertyId;
} sensorHdrB_t;

/*
 * Column structures
 */
typedef struct
{
    uint16_t propertyId;
    uint16_t rawValueX;
} sensorColumnGet_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint16_t rawValueX;
    uint16_t columnWidth;
    uint16_t rawValueY;
} sensorColumnStatus_t;

/*
 * Series structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint8_t  rawValueX1;
    uint8_t  rawValueX2;
} sensorSeriesGet_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    // ToDo number of tripples
    uint8_t  rawValueX;
    uint8_t  columnWidth;
    uint8_t  rawValueY;
} sensorSeriesStatus_t;

/*
 * Settings structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
} sensorSettingsGet_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    // 2*N - A sequence of N Sensor Setting Property IDs identifying

} sensorSettingsStatus_t;

/*
 * Setting structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint16_t settingPropertyId;
} sensorSettingGet_t;

#define MAX_SETTINGS_RAW_SIZE   1 // ToDo - find the exact value

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint16_t settingPropertyId;
    uint8_t  raw[MAX_SETTINGS_RAW_SIZE];
} sensorSettingSet_t;


ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint16_t settingPropertyId;
} sensorSettingEmptyStatus_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint16_t settingPropertyId;
    uint8_t  Access;
    uint8_t  Raw[MAX_SETTINGS_RAW_SIZE];
} sensorSettingStatus_t;

/*
 * Cadence structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
} sensorCadenceGet_t;


ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint8_t  fastCadencePeriodDivisor:7;
    uint8_t  statusTriggerType:1;
    uint8_t  statusTriggerDeltaDown;    //ToDo variable
    uint8_t  statusTriggerDeltaUp;      //ToDo variable
    uint8_t  statusMinInterval;
    uint8_t  fastCadenceLow;            //ToDo variable
    uint8_t  fas16tCadenceHigh;         //ToDo variable
} sensorCadenceSet_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
} sensorCadenceEmptyStatus_t;

ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t propertyId;
    uint8_t  fastCadencePeriodDivisor:7;
    uint8_t  statusTriggerType:1;
    uint8_t  statusTriggerDeltaDown;    //ToDo variable
    uint8_t  statusTriggerDeltaUp;      //ToDo variable
    uint8_t  statusMinInterval;
    uint8_t  fastCadenceLow;            //ToDo variable
    uint8_t  fas16tCadenceHigh;         //ToDo variable
} sensorCadenceStatus_t;

/*
 * Callbacks for getting sensors info
 */
typedef int                      (*sensorModels_getDataCB_t)(uint8_t **pData, uint16_t *dataLen);
typedef sensorDescriportState_t *(*sensorModels_getDescCB_t)(void);

typedef struct
{
    sensorModels_getDataCB_t getDataCB;
    sensorModels_getDescCB_t getDescCB;
} sensorModels_CBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      sensorModels_registerModel
 *
 * @brief   register new model
 *
 */
int  sensorModels_registerModel(uint8_t elem_idx, uint16_t propertyId, sensorModels_CBs_t *sensorCBs, int sensorDataLen);

/*********************************************************************
 * @fn      sensorModels_send_desc_get
 *
 * @brief   Client send sensor_desc_get request
 *
 */
void sensorModels_send_desc_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_get
 *
 * @brief   Client send sensor_get request
 *
 */
void sensorModels_send_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_Column_get
 *
 * @brief   Client send sensor_Column_get request
 *
 */
void sensorModels_send_Column_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId, uint8_t rawValueX);

/*********************************************************************
 * @fn      sensorModels_send_Series_get
 *
 * @brief   Client send sensor_Series_get request
 *
 */
void sensorModels_send_Series_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId, uint8_t rawValueX1, uint8_t rawValueX2);

/*********************************************************************
 * @fn      sensorModels_send_status_by_id
 *
 * @brief   unsolicited message from App - send back the sensor Marshalled header+data
 *
 */
void sensorModels_send_status_by_id(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_settings_get
 *
 * @brief   Client send sensor_settings_get request
 *
 */
void sensorModels_send_settings_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_cadence_get
 *
 * @brief   Client send sensor_Cadence_get request
 *
 */
void sensorModels_send_cadence_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_cadence_set
 *
 * @brief   Client send sensor_Cadence_set request
 *
 */
void sensorModels_send_cadence_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_cadence_set_unack
 *
 * @brief   Client send sensor_Cadence_set request
 *
 */
void sensorModels_send_cadence_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId);

/*********************************************************************
 * @fn      sensorModels_send_setting_get
 *
 * @brief   Client send sensor_setting_get request
 *
 */
void sensorModels_send_setting_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId, uint16_t settingPropertyId);

/*********************************************************************
 * @fn      sensorModels_send_setting_set
 *
 * @brief   Client send sensor_setting_set request
 *
 */
void sensorModels_send_setting_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId, uint16_t settingPropertyId);

/*********************************************************************
 * @fn      sensorModels_send_setting_set_unack
 *
 * @brief   Client send sensor_setting_set request
 *
 */
void sensorModels_send_setting_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId, uint16_t settingPropertyId);

#endif /* SENSOR_MODELS_H_ */
