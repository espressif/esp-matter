/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef _SL_WFX_GENERAL_ERROR_API_H_
#define _SL_WFX_GENERAL_ERROR_API_H_

/**********************************************************************/
/*                  Assert Error Codes                                */
/**********************************************************************/

/**
 * @addtogroup GENERAL_API
 * @{
 *
 * @addtogroup ERROR_IDS
 * @{
 */

/**
 * @brief specifies the type of error reported by the indication message sl_wfx_error_ind_body_t
 *
 * */
typedef enum sl_wfx_error_e {
  SL_WFX_ERROR_FIRMWARE_ROLLBACK                 = 0x0,    ///<Firmware rollback error, no data returned
  SL_WFX_ERROR_DEPRECATED_0                      = 0x1,    ///<Not used anymore
  SL_WFX_ERROR_DEPRECATED_1                      = 0x2,    ///<Not used anymore
  SL_WFX_ERROR_INVALID_SESSION_KEY               = 0x3,    ///<Secure Link Session key is invalid (probably not initialized)
  SL_WFX_ERROR_OOR_VOLTAGE                       = 0x4,    ///<Out-of-range power supply voltage detected, the last voltage value is returned. Param: measured voltage (mV)
  SL_WFX_ERROR_PDS_VERSION                       = 0x5,    ///<Wrong PDS version detected, no data returned
  SL_WFX_ERROR_OOR_TEMPERATURE                   = 0x6,    ///<Out-of-range temperature, no data returned
  SL_WFX_ERROR_REQ_DURING_KEY_EXCHANGE           = 0x7,    ///<Requests from Host are forbidden until the end of key exchange (Host should wait for the associated indication)
  SL_WFX_ERROR_DEPRECATED_2                      = 0x8,    ///<Not used anymore
  SL_WFX_ERROR_DEPRECATED_3                      = 0x9,    ///<Not used anymore
  SL_WFX_ERROR_SECURELINK_DECRYPTION             = 0xa,    ///<An error occured during message decryption (can be a counter mismatch or wrong CCM tag)
  SL_WFX_ERROR_SECURELINK_WRONG_ENCRYPTION_STATE = 0xb,    ///< Encryption state of the received message doesn't match the SecureLink bitmap. Param: was encrypted
  SL_WFX_SPI_OR_SDIO_FREQ_TOO_LOW                = 0xc,    ///<SPI or SDIO bus clock is too slow (<1kHz)
  SL_WFX_ERROR_DEPRECATED_4                      = 0xd,    ///<Not used anymore
  SL_WFX_ERROR_DEPRECATED_5                      = 0xe,    ///<Not used anymore
  SL_WFX_HIF_BUS_ERROR                           = 0xf,    ///<HIF HW has reported an error. Param: HIF status register
  SL_WFX_PDS_TESTFEATURE_MODE_ERROR              = 0x10,   ///<Unknown TestFeatureMode during test feature init
  SL_WFX_ERROR_SECURELINK_EXPECTED_BITMAP        = 0x11    ///<The SecureLink bitmap must be configured right after key exchange
} sl_wfx_error_t;

/**
 * @}
 *
 * @}
 */

#endif /* _SL_WFX_GENERAL_ERROR_API_H_ */
