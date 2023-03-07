/***************************************************************************//**
 * @file
 * @brief Definitions of manufacturing tokens used by ZWave protocols
 * @version beta
 *******************************************************************************
 * # License
 * @copyright 2022 Silicon Laboratories Inc.
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef ZW_MFGTOKENS_H
#define ZW_MFGTOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

#define TOKEN_MFG_ZWAVE_COUNTRY_FREQ_ID	  0x0001
#define TOKEN_MFG_ZW_PRK_ID               0x0002
#define TOKEN_MFG_ZW_PUK_ID               0x0003
#define TOKEN_MFG_ZW_INITIALIZED_ID       0x0004
#define TOKEN_MFG_ZW_QR_CODE_ID           0x0005

#define TOKEN_MFG_ZWAVE_COUNTRY_FREQ_SIZE 1 // bytes
#define TOKEN_MFG_ZW_PRK_SIZE 32 // bytes
#define TOKEN_MFG_ZW_PUK_SIZE 32 // bytes
#define TOKEN_MFG_ZW_INITIALIZED_SIZE 1 // bytes
#define TOKEN_MFG_ZW_QR_CODE_SIZE 106 // bytes

/**
 * Function for initializing manufacturing module
 * @return        none
 */
void ZW_MfgTokenModuleInit(void);

/* Bellow are the public API to set and get tokens */

/**
 * Function for setting the manufacturing token data
 *
 * @param[in]     token
 * @param[in]     data pointer
 * @param[in]     len
 * @return        none
 */
void ZW_SetMfgTokenData(uint16_t token, void *data, uint8_t len);

/**
 * Function for Getting the manufacturing token data
 *
 * @param[out]    data
 * @param[in]     token
 * @param[in]     len
 * @return        none
 */
void ZW_GetMfgTokenData(void *data, uint16_t token, uint8_t len);

/**
 * Wrapper function for getting the manufacturing "country freq" token
 *
 * @param[out]    data Country frequency token
 * @return        none
 */
void ZW_GetMfgTokenDataCountryFreq(void *data);

/**
* Writes Rf Region to manufacturing token
* @param[in] region Region to write
*/
void ZW_SetMfgTokenDataCountryRegion(void* region);

/**
 * Lock manufacturing token storage
 * @return        none
 */
void ZW_LockMfgTokenData(void);

#ifdef __cplusplus
}
#endif

#endif /* ZW_MFGTOKENS_H */
