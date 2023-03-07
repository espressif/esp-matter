/******************************************************************************

@file  sensor_models.c

@brief  * 1. sensor model server root_model callbacks  - get callback
        * 2. sensor model client root_model callbacks  - status response callbacks
        * 3. client/server api to send commands        - get/set command
        * 4. register sensor into local DB

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

/*********************************************************************
* INCLUDES
*/
#include <ti/display/Display.h>
#include "menu/two_btn_menu.h"
#include "simple_mesh_node_menu.h"
#include "autoconf.h"
#include "sensor_models.h"
#include "mesh_erpc_wrapper.h"

/*********************************************************************
 * EXTERNS
*/
extern Display_Handle dispHandle;

/*********************************************************************
* TYPEDEFS
*/
#define SENSOR_MODELS_MAX_LIST  20

typedef struct
{
  uint16_t propertyId;
  uint8_t elem_idx;
  int dataLen;
  int hdrLen;
  sensorModels_CBs_t *pSensorCBs;
} sensorModel_t;

typedef struct
{
  int numRegistered;
  int totalDataLen;
  int totalHdrLen;
  sensorModel_t models[SENSOR_MODELS_MAX_LIST];
} sensorModelsList_t;

/*********************************************************************
* LOCAL VARIABLES
*/
sensorModelsList_t sensorModelsList = {
    .numRegistered = 0,
    .totalDataLen = 0,
    .totalHdrLen = 0
};

/*********************************************************************
 * @fn      sensorModels_registerModel
 *
 * @brief   register new model
 *
 */
int sensorModels_registerModel(uint8_t elem_idx, uint16_t propertyId, sensorModels_CBs_t *pSensorCBs, int sensorDataLen)
{
    sensorModel_t *pSensor;
    int sensorIndex;

    if (sensorModelsList.numRegistered < SENSOR_MODELS_MAX_LIST)
    {
        sensorIndex = sensorModelsList.numRegistered;
        pSensor =  &sensorModelsList.models[sensorIndex];

        if (sensorDataLen <= 16)
        {
            pSensor->hdrLen = sizeof(sensorHdrA_t);
        }
        else if (sensorDataLen <= 127)
        {
            pSensor->hdrLen = sizeof(sensorHdrB_t);
        }
        else
        {
            return SENSOR_MODELS_ERROR_NO_RESOURCES;
        }
        pSensor->elem_idx = elem_idx;
        pSensor->propertyId = propertyId;
        pSensor->dataLen = sensorDataLen;
        pSensor->pSensorCBs = pSensorCBs;

        // increment totals - prepare for request to send all sensors data
        sensorModelsList.numRegistered++;
        sensorModelsList.totalDataLen += pSensor->dataLen;
        sensorModelsList.totalHdrLen += pSensor->hdrLen;

        return sensorIndex;
    }

    return SENSOR_MODELS_ERROR_NO_RESOURCES;
}

/*********************************************************************
 * @fn      sensorModels_findPropertyId
 *
 * @brief   return sensorModel if found propertyId
 *
 */
sensorModel_t *sensorModels_findPropertyId(uint16_t propertyId)
{
    int i;

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        if (sensorModelsList.models[i].propertyId == propertyId)
        {
            return &sensorModelsList.models[i];
        }
    }

    return NULL;
}

/*********************************************************************
 * @fn      sensorModels_send_buf
 *
 * @brief   General function to send mesh message buffer
 *
 */
void sensorModels_send_buf(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint32_t opcode, uint8_t *pData, uint16_t dataLen)
{
    // msg room also for opcode and MIC
    NET_BUF_SIMPLE_DEFINE(msg, 2 + dataLen + 4);

    bt_mesh_model_msg_init(&msg, opcode);
    if ((dataLen != 0) && (pData != NULL))
    {
        net_buf_simple_add_mem(&msg, (void *)pData, dataLen);
    }

    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensorModels_send_desc_get
 *
 * @brief   Client send sensor_desc_get request
 *
 */
void sensorModels_send_desc_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId)
{
    sensorDescGet_t sensorDescGet;

    if (propertyId > 0)
    {
        sensorDescGet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET, (uint8_t *)&sensorDescGet, sizeof(sensorDescGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_desc_get
 *
 * @brief   Server handler for client request - send back the sensor descriptor data
 *
 */
void sensor_desc_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorDescriportState_t *pSensorDescriptor;
    uint16_t propertyId = 0;
    int numMsgSensors = 0;
    int sendAll = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) == 0)
    {
        // no property id parameter, send all
        numMsgSensors = sensorModelsList.numRegistered;
        sendAll = 1;
    }
    else
    {
        numMsgSensors = 1;
        propertyId = net_buf_simple_pull_le16(buf);
    }

    // msg descriptors room for all relevant sensors
    NET_BUF_SIMPLE_DEFINE(msg, 2 + numMsgSensors * sizeof(sensorDescriportState_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_desc_get callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors descriptor
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if (((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
                || sendAll)
        {

            // receive the sensor descriptor
            pSensorDescriptor = pSensor->pSensorCBs->getDescCB();
            net_buf_simple_add_mem(&msg, (void *)pSensorDescriptor, sizeof(sensorDescriportState_t));

            numAppend++;
        }
    }

    if ((numAppend == 0) && (!sendAll))
    {
        // on prohibited value, do not answer
        if (propertyId == 0)
        {
            return;
        }

        // unknown propertyId, should send the property id without the other descriptor fields
        net_buf_simple_add_mem(&msg, (void *)&propertyId, sizeof(propertyId));
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_desc_Status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_desc_Status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorDescriportState_t *pDescriptorStatus;
    int bufLen = NET_BUF_LEN(buf);
    int i;

    pDescriptorStatus = (sensorDescriportState_t *)net_buf_simple_pull_mem(buf, sizeof(sensorDescriportState_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_desc_status callback from addr=0x%x, pId=0x%x",
                                       ctx->addr, pDescriptorStatus->propertyId);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pDescriptorStatus->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor Desc data ", (uint8_t *)pDescriptorStatus, bufLen);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor Desc unknown propertyId, data ", (uint8_t *)pDescriptorStatus, bufLen);
}

/*********************************************************************
 * @fn      sensorModels_send_get
 *
 * @brief   Client send sensor_get request
 *
 */
void sensorModels_send_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t propertyId)
{
    sensorDataGet_t sensorDataGet;

    if (propertyId > 0)
    {
        sensorDataGet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_GET, (void *)&sensorDataGet, sizeof(sensorDataGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensorModels_send_status
 *
 * @brief   Server handler for client request - send back the sensor Marshalled header+data
 *
 */
void sensorModels_send_status(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        uint16_t propertyId, uint8_t *pSensorState, uint16_t sensorDataLen)
{
    sensorHdrA_t msgHeader;

    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(msgHeader) + sensorDataLen + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_STATUS);

    // Add header
    msgHeader.format = SENSOR_HDR_FORMAT_A;
    msgHeader.length = sensorDataLen-1;
    msgHeader.propertyId = propertyId;
    net_buf_simple_add_mem(&msg, (void *)&msgHeader, sizeof(msgHeader));

    if (pSensorState != NULL)
    {
        // Add sensor data
        net_buf_simple_add_mem(&msg, (void *)pSensorState, sensorDataLen);
    }

    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensorModels_send_no_data
 *
 * @brief   Server handler for client request - send back the empty sensor data
 *
 */
void sensorModels_send_no_data(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        uint16_t propertyId)
{
    sensorHdrB_t msgHeader;

    // Only header
    msgHeader.format = SENSOR_HDR_FORMAT_B;
    msgHeader.length = 0x7F;  // meaning length is zero
    msgHeader.propertyId = propertyId;

    sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_STATUS, (void *)&msgHeader, sizeof(sensorHdrB_t));
}

/*********************************************************************
 * @fn      sensorModels_send_status_by_id
 *
 * @brief   unsolicited message from App - send back the sensor Marshalled header+data
 *
 */
void sensorModels_send_status_by_id(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        uint16_t propertyId)
{
    sensorModel_t *pSensor;
    uint16_t dataLen = 0;
    uint8_t *pSensorData = NULL;

    pSensor = sensorModels_findPropertyId(propertyId);
    if (pSensor) // found
    {
        // send sensor data status as a result of sensor_get request
        pSensor->pSensorCBs->getDataCB(&pSensorData, &dataLen);
        sensorModels_send_status(model, ctx, propertyId, (uint8_t *)pSensorData, dataLen);
    }
    else
    {
        // no sensor with this propertyId, send header with zero length
        sensorModels_send_no_data(model, ctx, propertyId);
    }
}

/*********************************************************************
 * @fn      sensor_get
 *
 * @brief   Server handler for client request - send back the sensor data
 *
 */
void sensor_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    uint16_t dataLen = 0;
    uint8_t *pSensorData = NULL;
    uint16_t propertyId = 0;
    int msgDataSize = 0;
    int sendAll = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) == 0)
    {
        // no property id parameter, send all
        // msg room for all relevant sensors
        msgDataSize = sensorModelsList.numRegistered*sizeof(sensorHdrB_t) + sensorModelsList.totalDataLen;
        sendAll = 1;
    }
    else
    {
        msgDataSize = sizeof(sensorHdrB_t) + sensorModelsList.totalDataLen;
        propertyId = net_buf_simple_pull_le16(buf);
        // on prohibited value, do not answer
        if (propertyId == 0)
        {
            return;
        }
    }

    NET_BUF_SIMPLE_DEFINE(msg, 2 + msgDataSize + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_get callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if (((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
                || sendAll)
        {
            pSensor->pSensorCBs->getDataCB(&pSensorData, &dataLen);

            if (dataLen <= 16)
            {
                sensorHdrA_t sensorHdrA;

                sensorHdrA.format = SENSOR_HDR_FORMAT_A;
                sensorHdrA.length = dataLen-1;
                sensorHdrA.propertyId = propertyId;

                // append the header
                net_buf_simple_add_mem(&msg, (void *)&sensorHdrA, sizeof(sensorHdrA));
            }
            else
            {
                sensorHdrB_t sensorHdrB;

                sensorHdrB.format = SENSOR_HDR_FORMAT_B;
                sensorHdrB.length = dataLen-1;
                sensorHdrB.propertyId = propertyId;

                // append the header
                net_buf_simple_add_mem(&msg, (void *)&sensorHdrB, sizeof(sensorHdrB));
            }

            // append the data
            net_buf_simple_add_mem(&msg, (void *)pSensorData, dataLen);

            numAppend++;
        }
    }

    if ((numAppend == 0) && (!sendAll))
    {
        sensorHdrB_t sensorHdrB;

        sensorHdrB.format = SENSOR_HDR_FORMAT_B;
        sensorHdrB.length = 0x7F;
        sensorHdrB.propertyId = propertyId;

        // append the header
        net_buf_simple_add_mem(&msg, (void *)&sensorHdrB, sizeof(sensorHdrB));
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}


/*********************************************************************
 * @fn      sensor_status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    uint16_t propertyId = 0;
    int bufLen = NET_BUF_LEN(buf);
    int i;
    sensorHdrA_t *pMsgHdrA = NULL;
    sensorHdrB_t *pMsgHdrB = NULL;

    if ((NET_BUF_TEST_U8(buf) & SENSOR_DATA_HDR_FORMAT_MASK) == SENSOR_HDR_FORMAT_A)
    {
        pMsgHdrA = (sensorHdrA_t *)net_buf_simple_pull_mem(buf, sizeof(sensorHdrA_t));
        propertyId = pMsgHdrA->propertyId;
    }
    else
    {
        pMsgHdrB = (sensorHdrB_t *)net_buf_simple_pull_mem(buf, sizeof(sensorHdrB_t));
        propertyId = pMsgHdrB->propertyId;
    }

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_status callback from addr=0x%x, pId=0x%x len=%d",
                                   ctx->addr, propertyId, bufLen);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            if (bufLen == 0) // no data
            {
                if (pMsgHdrA != NULL)
                {
                    SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor NO data formatA len ", bufLen);
                }
                else
                {
                    SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor NO data formatB len ", bufLen);
                }
                return;
            }

            uint8_t temperature = net_buf_simple_pull_u8(buf);
            if (pMsgHdrA != NULL)
            {
                SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor data formatA Temperature ", temperature>>1);
            }
            else
            {
                SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor data formatB Temperature ", temperature>>1);
            }
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor data unknown propertyId, data ", (uint8_t *)buf->data, bufLen);
}

/*********************************************************************
 * @fn      sensorModels_send_Column_get
 *
 * @brief   Client send sensor_Column_get request
 *
 */
void sensorModels_send_Column_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId, uint8_t rawValueX)
{
    sensorColumnGet_t sensorColumnGet;

    if (propertyId > 0)
    {
        sensorColumnGet.propertyId = propertyId;
        sensorColumnGet.rawValueX = rawValueX;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_COLUMN_GET, (uint8_t *)&sensorColumnGet, sizeof(sensorColumnGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_COLUMN_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_Column_get
 *
 * @brief   Server handler for client request - send back the Column data
 *
 */
void sensor_Column_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    sensorColumnGet_t *pColumnGet;
    int bufLen = NET_BUF_LEN(buf);
    int i;

    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorColumnGet_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_COLUMN_STATUS);

    pColumnGet = (sensorColumnGet_t *)net_buf_simple_pull_mem(buf, sizeof(sensorColumnGet_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Column_get callback from addr=0x%x, pId=0x%x",
                           ctx->addr, pColumnGet->propertyId);

    // on prohibited value, do not answer
    if (pColumnGet->propertyId == 0)
    {
        return;
    }

    // on size error, send only propertyId
    if (bufLen < sizeof(sensorColumnGet_t))
    {
        net_buf_simple_add_mem(&msg, (void *)&pColumnGet->propertyId, sizeof(pColumnGet->propertyId));
        bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
        return;
    }

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pColumnGet->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Column data is not implemented, send status with only propertyId and x
            net_buf_simple_add_mem(&msg, (void *)pColumnGet, sizeof(sensorColumnGet_t));
            bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
            return;
        }
    }

    // unknown propertyId
    // Column data is not implemented, send status with only propertyId
    net_buf_simple_add_mem(&msg, (void *)pColumnGet, sizeof(pColumnGet->propertyId)/* only propertyId sizeof(sensorColumnGet_t) */);
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Column_Status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_Column_Status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorColumnStatus_t *pColumnStatus;
    int bufLen = NET_BUF_LEN(buf);
    int i;

    pColumnStatus = (sensorColumnStatus_t *)net_buf_simple_pull_mem(buf, sizeof(sensorColumnStatus_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Column_Status callback from addr=0x%x, pId=0x%x",
                           ctx->addr, pColumnStatus->propertyId);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pColumnStatus->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor column data ", (uint8_t *)pColumnStatus, bufLen);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor column unknown propertyId, data ", (uint8_t *)pColumnStatus, bufLen);
}

/*********************************************************************
 * @fn      sensorModels_send_Series_get
 *
 * @brief   Client send sensor_Series_get request
 *
 */
void sensorModels_send_Series_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId, uint8_t rawValueX1, uint8_t rawValueX2)
{
    sensorSeriesGet_t sensorSeriesGet;

    if (propertyId > 0)
    {
        sensorSeriesGet.propertyId = propertyId;
        sensorSeriesGet.rawValueX1 = rawValueX1;
        sensorSeriesGet.rawValueX2 = rawValueX2;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SERIES_GET, (uint8_t *)&sensorSeriesGet, sizeof(sensorSeriesGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SERIES_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_Series_get
 *
 * @brief   Server handler for client request - send back the Series data
 *
 */

void sensor_Series_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    sensorSeriesGet_t *pSeriesGet;
    int bufLen = NET_BUF_LEN(buf);
    int i;

    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorSeriesGet_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_SERIES_STATUS);

    pSeriesGet = (sensorSeriesGet_t *)net_buf_simple_pull_mem(buf, sizeof(sensorSeriesGet_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Series_get callback from addr=0x%x, pId=0x%x",
                           ctx->addr, pSeriesGet->propertyId);

    // on prohibited value, do not answer
    if (pSeriesGet->propertyId == 0)
    {
        return;
    }

    // on size error, send only propertyId
    if (bufLen < sizeof(sensorSeriesGet_t))
    {
        net_buf_simple_add_mem(&msg, (void *)&pSeriesGet->propertyId, sizeof(pSeriesGet->propertyId));
        bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
        return;
    }

    // find sensor and add status data
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pSeriesGet->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Series data is not implemented, send status with only propertyId and x
            net_buf_simple_add_mem(&msg, (void *)pSeriesGet, sizeof(pSeriesGet->propertyId)/* only propertyId sizeof(sensorSeriesGet_t) */);
            bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
            return;
        }
    }

    // unknown propertyId
    // Series data is not implemented, send status with only propertyId
    net_buf_simple_add_mem(&msg, (void *)pSeriesGet, sizeof(pSeriesGet->propertyId)/* only propertyId sizeof(sensorSeriesGet_t) */);
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Series_Status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_Series_Status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorSeriesStatus_t *pSeriesStatus;
    int bufLen = NET_BUF_LEN(buf);
    int i;

    pSeriesStatus = (sensorSeriesStatus_t *)net_buf_simple_pull_mem(buf, sizeof(sensorSeriesStatus_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Series_Status callback from addr=0x%x, pId=0x%x, len=%d",
                           ctx->addr, pSeriesStatus->propertyId, bufLen);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pSeriesStatus->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor Series data ", (uint8_t *)pSeriesStatus, bufLen);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLineBytes((uint8_t *)"Sensor Series unknown propertyId, data ", (uint8_t *)pSeriesStatus, bufLen);
}

/*********************************************************************
 * @fn      sensorModels_send_settings_get
 *
 * @brief   Client send sensor_settings_get request
 *
 */
void sensorModels_send_settings_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId)
{
    sensorSettingsGet_t sensorSettingsGet;

    if (propertyId > 0)
    {
        sensorSettingsGet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTINGS_GET, (uint8_t *)&sensorSettingsGet, sizeof(sensorSettingsGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTINGS_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_settings_get
 *
 * @brief   Server handler for client request - send back the sensor settings status
 *
 */
void sensor_settings_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    //sensorSettingStatus_t *pSensorSettings;
    uint16_t propertyId = 0;
    int numMsgSensors = 0;
    int sendAll = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) == 0)
    {
        // no property id parameter, send all
        numMsgSensors = sensorModelsList.numRegistered;
        sendAll = 1;
    }
    else
    {
        numMsgSensors = 1;
        propertyId = net_buf_simple_pull_le16(buf);
    }

    // msg room for all relevant sensors
    NET_BUF_SIMPLE_DEFINE(msg, 2 + numMsgSensors * sizeof(sensorSettingStatus_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_settings_get callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if (((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
                || sendAll)
        {
            // Settings data is not implemented, send status with only propertyId
            net_buf_simple_add_mem(&msg, (void *)&propertyId, sizeof(propertyId)/* only propertyId sizeof(sensorSettingStatus_t) */);
            numAppend++;
        }
    }

    if ((numAppend == 0) && (!sendAll))
    {
        // unknown propertyId, should send the property id without the other fields
        net_buf_simple_add_mem(&msg, (void *)&propertyId, sizeof(propertyId));
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Settings_status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_Settings_status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorSettingStatus_t *pSensorSettings;
    int i;

    pSensorSettings = (sensorSettingStatus_t *)net_buf_simple_pull_mem(buf, sizeof(sensorSettingStatus_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_settings_Status callback from addr=0x%x, pId=0x%x", ctx->addr, pSensorSettings->propertyId);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pSensorSettings->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor Settings ", pSensorSettings->settingPropertyId);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor Settings unknown property id ", pSensorSettings->settingPropertyId);
}

/*********************************************************************
 * @fn      sensorModels_send_setting_get
 *
 * @brief   Client send sensor_setting_get request
 *
 */
void sensorModels_send_setting_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId, uint16_t settingPropertyId)
{
    sensorSettingGet_t sensorSettingGet;

    if (propertyId > 0)
    {
        sensorSettingGet.propertyId = propertyId;
        sensorSettingGet.settingPropertyId = settingPropertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_GET, (uint8_t *)&sensorSettingGet, sizeof(sensorSettingGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensorModels_send_setting_set
 *
 * @brief   Client send sensor_setting_set request
 *
 */
void sensorModels_send_setting_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId, uint16_t settingPropertyId)
{
    sensorSettingSet_t sensorSettingSet;

    if (propertyId > 0)
    {
        sensorSettingSet.propertyId = propertyId;
        sensorSettingSet.settingPropertyId = settingPropertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_SET, (uint8_t *)&sensorSettingSet, sizeof(sensorSettingSet_t));
    }
    else
    {
        // set all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_SET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensorModels_send_setting_set_unack
 *
 * @brief   Client send sensor_setting_set request
 *
 */
void sensorModels_send_setting_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId, uint16_t settingPropertyId)
{
    sensorSettingSet_t sensorSettingSet;

    if (propertyId > 0)
    {
        sensorSettingSet.propertyId = propertyId;
        sensorSettingSet.settingPropertyId = settingPropertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK, (uint8_t *)&sensorSettingSet, sizeof(sensorSettingSet_t));
    }
    else
    {
        // set all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_setting_get
 *
 * @brief   Server handler for client request - send back the sensor setting status
 *
 */
void sensor_setting_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    //sensorSettingGet_t *pSensorSettingGet;
    sensorSettingEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;
    uint16_t settingPropertyId = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) != sizeof(sensorSettingGet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);
    settingPropertyId = net_buf_simple_pull_le16(buf);

    // msg room for status response
    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorSettingEmptyStatus_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_SETTING_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_settings_get callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Settings is not implemented, send status with only propertyId+settingPropertyId
            sensorEmptyStatus.propertyId = propertyId;
            sensorEmptyStatus.settingPropertyId = settingPropertyId;
            net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorSettingEmptyStatus_t)/* only propertyId and settingPropertyId */);
            numAppend++;
        }
    }

    if (numAppend == 0)
    {
        // unknown propertyId, should send the propertyId+settingPropertyId
        sensorEmptyStatus.propertyId = propertyId;
        sensorEmptyStatus.settingPropertyId = settingPropertyId;
        net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorSettingEmptyStatus_t)/* only propertyId and settingPropertyId */);
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_setting_set_unack
 *
 * @brief   Server handler for client request - save setting, don't send back the sensor setting status
 *
 */
void sensor_setting_set_unack(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
    //sensorSettingSet_t *pSensorSettingSet;
    //sensorSettingEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;
    uint16_t settingPropertyId = 0;
    uint8_t settingRaw = 0;

    if (NET_BUF_LEN(buf) != sizeof(sensorSettingSet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);
    settingPropertyId = net_buf_simple_pull_le16(buf);
    settingRaw = net_buf_simple_pull_u8(buf);
    // ToDo - save state of the this settings ????
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_setting_set_unack callback from addr=0x%x, pId=0x%x (%d, %d)",
                   ctx->addr, propertyId, settingPropertyId, settingRaw);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */
}

/*********************************************************************
 * @fn      sensor_setting_set
 *
 * @brief   Server handler for client request - save setting, send back the sensor setting status
 *
 */
void sensor_setting_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    //sensorSettingSet_t *pSensorSettingSet;
    sensorSettingEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;
    uint16_t settingPropertyId = 0;
    //uint8_t settingRaw = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) != sizeof(sensorSettingSet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);
    settingPropertyId = net_buf_simple_pull_le16(buf);
    //settingRaw = net_buf_simple_pull_u8(buf);
    // ToDo - save state of the this settings ????

    // msg room for status response
    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorSettingEmptyStatus_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_SETTING_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_settings_set callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Settings is not implemented, send status with only propertyId+settingPropertyId
            sensorEmptyStatus.propertyId = propertyId;
            sensorEmptyStatus.settingPropertyId = settingPropertyId;
            net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorSettingEmptyStatus_t)/* only propertyId and settingPropertyId */);
            numAppend++;
        }
    }

    if (numAppend == 0)
    {
        // unknown propertyId, should send the propertyId+settingPropertyId
        sensorEmptyStatus.propertyId = propertyId;
        sensorEmptyStatus.settingPropertyId = settingPropertyId;
        net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorSettingEmptyStatus_t)/* only propertyId and settingPropertyId */);
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Setting_status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_Setting_status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorSettingStatus_t *pSensorSetting;
    int i;

    // ToDo - keed the status in client DB
    pSensorSetting = (sensorSettingStatus_t *)net_buf_simple_pull_mem(buf, sizeof(sensorSettingStatus_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Setting_status callback from addr=0x%x, pId=0x%x", ctx->addr, pSensorSetting->propertyId);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pSensorSetting->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor Setting ", pSensorSetting->settingPropertyId);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor Setting unknown property id ", pSensorSetting->settingPropertyId);
}

/*********************************************************************
 * @fn      sensorModels_send_cadence_get
 *
 * @brief   Client send sensor_Cadence_get request
 *
 */
void sensorModels_send_cadence_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId)
{
    sensorCadenceGet_t sensorCadenceGet;

    if (propertyId > 0)
    {
        sensorCadenceGet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_GET, (uint8_t *)&sensorCadenceGet, sizeof(sensorCadenceGet_t));
    }
    else
    {
        // get all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_GET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensorModels_send_cadence_set
 *
 * @brief   Client send sensor_Cadence_set request
 *
 */
void sensorModels_send_cadence_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId)
{
    sensorCadenceSet_t sensorCadenceSet;

    if (propertyId > 0)
    {
        sensorCadenceSet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_SET, (uint8_t *)&sensorCadenceSet, sizeof(sensorCadenceSet_t));
    }
    else
    {
        // set all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_SET, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensorModels_send_cadence_set_unack
 *
 * @brief   Client send sensor_Cadence_set request
 *
 */
void sensorModels_send_cadence_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                            uint16_t propertyId)
{
    sensorCadenceSet_t sensorCadenceSet;

    if (propertyId > 0)
    {
        sensorCadenceSet.propertyId = propertyId;
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK, (uint8_t *)&sensorCadenceSet, sizeof(sensorCadenceSet_t));
    }
    else
    {
        // set all sensors
        sensorModels_send_buf(model, ctx, BT_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK, NULL, 0);
    }
}

/*********************************************************************
 * @fn      sensor_Cadence_get
 *
 * @brief   Server handler for client request - send back the sensor cadence status
 *
 */
void sensor_Cadence_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    //sensorCadenceGet_t *pSensorCadenceGet;
    sensorCadenceEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) != sizeof(sensorCadenceGet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);

    // msg room for status response
    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorCadenceEmptyStatus_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_CADENCE_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_cadences_get callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Cadence is not implemented, send status with only propertyId
            sensorEmptyStatus.propertyId = propertyId;
            net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorCadenceEmptyStatus_t)/* only propertyId */);
            numAppend++;
        }
    }

    if (numAppend == 0)
    {
        // unknown propertyId, should send the propertyId
        sensorEmptyStatus.propertyId = propertyId;
        net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorCadenceEmptyStatus_t)/* only propertyId */);
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Cadence_set_unack
 *
 * @brief   Server handler for client request - save cadence, don't send back the sensor cadence status
 *
 */
void sensor_Cadence_set_unack(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
    //sensorCadenceSet_t *pSensorCadenceSet;
    //sensorCadenceEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;

    if (NET_BUF_LEN(buf) != sizeof(sensorCadenceSet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);
    // ToDo - save state of the this cadences ????
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Cadence_set_unack callback from addr=0x%x, pId=0x%x",
                   ctx->addr, propertyId);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */
}

/*********************************************************************
 * @fn      sensor_Cadence_set
 *
 * @brief   Server handler for client request - save cadence, send back the sensor cadence status
 *
 */
void sensor_Cadence_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    //sensorCadenceSet_t *pSensorCadenceSet;
    sensorCadenceEmptyStatus_t sensorEmptyStatus;
    uint16_t propertyId = 0;
    int numAppend = 0;
    int i;

    if (NET_BUF_LEN(buf) != sizeof(sensorCadenceSet_t))
    {
        // error
        return;
    }

    propertyId = net_buf_simple_pull_le16(buf);
    // ToDo - save state of the this cadences ????

    // msg room for status response
    NET_BUF_SIMPLE_DEFINE(msg, 2 + sizeof(sensorCadenceEmptyStatus_t) + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_CADENCE_STATUS);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_cadences_set callback from addr=0x%x, pId=0x%x", ctx->addr, propertyId);

    // append sensors info
    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            // Cadence is not implemented, send status with only propertyId
            sensorEmptyStatus.propertyId = propertyId;
            net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorCadenceEmptyStatus_t)/* only propertyId */);
            numAppend++;
        }
    }

    if (numAppend == 0)
    {
        // unknown propertyId, should send the propertyId
        sensorEmptyStatus.propertyId = propertyId;
        net_buf_simple_add_mem(&msg, (void *)&sensorEmptyStatus, sizeof(sensorCadenceEmptyStatus_t)/* only propertyId */);
    }

    // Send the message
    bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg);
}

/*********************************************************************
 * @fn      sensor_Cadence_Status
 *
 * @brief   Client handler for server response - save and print the response
 *
 */
void sensor_Cadence_status(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    sensorCadenceStatus_t *pSensorCadence;
    int i;

    // ToDo - keed the status in client DB
    pSensorCadence = (sensorCadenceStatus_t *)net_buf_simple_pull_mem(buf, sizeof(sensorCadenceStatus_t));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: sensor_Cadence_Status callback from addr=0x%x, pId=0x%x",
                   ctx->addr, pSensorCadence->propertyId);

    for (i=0; i<sensorModelsList.numRegistered; i++)
    {
        sensorModel_t *pSensor = &sensorModelsList.models[i];
        if ((pSensor->propertyId == pSensorCadence->propertyId) && (pSensor->elem_idx == model->elem_idx))
        {
            SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor cadence ", pSensorCadence->propertyId);
            return;
        }
    }

    //unknown property id
    SimpleMeshMenu_setSensorLine((uint8_t *)"Sensor Cadence unknown property id ", pSensorCadence->propertyId);
}
