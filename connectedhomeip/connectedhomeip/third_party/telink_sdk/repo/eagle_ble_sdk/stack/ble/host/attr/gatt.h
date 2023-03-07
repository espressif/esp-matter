/********************************************************************************************************
 * @file	gatt.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#ifndef GATT_H_
#define GATT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	   This function is used to notify a client of the value of a Characteristic Value from a server.
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle  -  attribute handle.
 * @param[in]  *p -  data buffer pointer
 * @param[in]  len - data byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t	blc_gatt_pushHandleValueNotify  (u16 connHandle, u16 attHandle, u8 *p, int len);


/**
 * @brief	   This function is used to indicate the Characteristic Value from a server to a client.
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle - attribute handle.
 * @param[in]  *p -  data buffer pointer
 * @param[in]  len - data byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t	blc_gatt_pushHandleValueIndicate(u16 connHandle, u16 attHandle, u8 *p, int len);


/**
 * @brief	   This function is used to This function is used to request the server to write the value of an attribute without response.
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle - attribute handle.
 * @param[in]  *p -  data buffer pointer
 * @param[in]  len - data byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushWriteComand (u16 connHandle, u16 attHandle, u8 *p, int len);


/**
 * @brief	   This function is used to request the server to write the value of an attribute.
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle - attribute handle.
 * @param[in]  *p -  data buffer pointer
 * @param[in]  len - data byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushWriteRequest (u16 connHandle, u16 attHandle, u8 *p, int len);




/**
 * @brief	   This function is used to obtain the mapping of attribute handles with their associated types
 * 			   and allows a client to discover the list of attributes and their types on a server.
 * @param[in]  connHandle -  connection handle
 * @param[in]  start_attHandle - start attribute handle.
 * @param[in]  end_attHandle -  end attribute handle.
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushFindInformationRequest(u16 connHandle, u16 start_attHandle, u16 end_attHandle);


/**
 * @brief	   This function is used to obtain the handles of attributes that have a 16bit uuid attribute type and attribute value.
 * @param[in]  connHandle -  connection handle
 * @param[in]  start_attHandle - start attribute handle.
 * @param[in]  end_attHandle   - end attribute handle.
 * @param[in]  uuid
 * @param[in]  attr_value - attribute value
 * @param[in]  len - data byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushFindByTypeValueRequest (u16 connHandle, u16 start_attHandle, u16 end_attHandle, u16 uuid, u8* attr_value, int len);


/**
 * @brief	   This function is used to obtain the values of attributes where the attribute type is known but handle is not known.
 * @param[in]  connHandle -  connection handle
 * @param[in]  start_attHandle - start attribute handle.
 * @param[in]  end_attHandle   - end attribute handle.
 * @param[in]  uuid
 * @param[in]  uuid_len -uuid byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushReadByTypeRequest (u16 connHandle, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);


/**
 * @brief	   This function is used to request the server to read the value of an attribute.
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle -   attribute handle.
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushReadRequest (u16 connHandle, u16 attHandle);


/**
 * @brief	   This function is used to request the server to read part of the value of an attribute at a given offset
 * @param[in]  connHandle -  connection handle
 * @param[in]  attHandle - attribute handle.
 * @param[in]  offset
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushReadBlobRequest (u16 connHandle, u16 attHandle, u16 offset);


/**
 * @brief	   This function is used to obtain the values of attributes according to the uuid.
 * @param[in]  connHandle -  connection handle
 * @param[in]  start_attHandle - start attribute handle.
 * @param[in]  end_attHandle   - end attribute handle.
 * @param[in]  uuid -
 * @param[in]  uuid_len - uuid byte number
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_gatt_pushReadByGroupTypeRequest (u16 connHandle, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);

#ifdef __cplusplus
}
#endif

#endif /* GATT_H_ */
