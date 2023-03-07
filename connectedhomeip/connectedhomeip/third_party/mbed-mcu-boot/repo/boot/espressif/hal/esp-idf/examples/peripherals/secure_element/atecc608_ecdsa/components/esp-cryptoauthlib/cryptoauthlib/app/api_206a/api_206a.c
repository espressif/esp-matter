/**
 * \file
 *
 * \brief Provides APIs to use with ATSHA206A device.
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include "cryptoauthlib.h"
#include "api_206a.h"

#ifdef ATCA_ATSHA206A_SUPPORT

/** \brief Computes the diversified key based on the parent key provided and device serial number
 *  \param[in]  parent_key      parent key to be diversified
 *  \param[out] diversified_key diversified parent key
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_diversify_parent_key(uint8_t* parent_key, uint8_t* diversified_key)
{
    ATCA_STATUS status;
    uint8_t sha_data[96];
    uint8_t* p_temp = sha_data;
    uint8_t param1 = 0x0;      //These are fixed values.. Should match with value used in provisioning
    uint16_t param2 = 0x0;
    uint8_t sn[9];

    if ((parent_key == NULL) || (diversified_key == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        if ((status = atcab_read_serial_number(sn)) != ATCA_SUCCESS)
        {
            break;
        }

        memcpy(p_temp, parent_key, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;

        *p_temp++ = ATCA_DERIVE_KEY;
        *p_temp++ = param1;
        *p_temp++ = param2 & 0xFF;
        *p_temp++ = (param2 >> 8) & 0xFF;
        *p_temp++ = sn[8];
        *p_temp++ = sn[0];
        *p_temp++ = sn[1];
        memset(p_temp, 0, 25);
        p_temp += 25;
        memcpy(p_temp, sn, sizeof(sn));
        p_temp += sizeof(sn);
        memset(p_temp, 0, 23);

        if ((status = atcac_sw_sha2_256(sha_data, sizeof(sha_data), diversified_key)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    return status;
}
/** \brief Generates the derived key based on the parent key and other parameters provided
 *  \param[in]  parent_key      Input data contains device's parent key
 *  \param[out] derived_key     Output data derived from parent key
 *  \param[in]  param1          Input data to be used in derive key calculation
 *  \param[in]  param2          Input data to be used in derive key calculation
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_generate_derive_key(uint8_t* parent_key, uint8_t* derived_key, uint8_t param1, uint16_t param2)
{
    ATCA_STATUS status;
    uint8_t sha_data[96];
    uint8_t* p_temp = sha_data;
    uint8_t sn[9];

    if ((parent_key == NULL) || (derived_key == NULL) || (param1 == 0xFF))
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        if ((status = atcab_derivekey(param1, param2, NULL)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atcab_read_serial_number(sn)) != ATCA_SUCCESS)
        {
            break;
        }

        memcpy(p_temp, parent_key, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;
        *p_temp++ = ATCA_DERIVE_KEY;
        *p_temp++ = param1;
        *p_temp++ = param2 & 0xFF;
        *p_temp++ = (param2 >> 8) & 0xFF;
        *p_temp++ = sn[8];
        *p_temp++ = sn[0];
        *p_temp++ = sn[1];
        memset(p_temp, 0, (25 + 32));

        if ((status = atcac_sw_sha2_256(sha_data, sizeof(sha_data), derived_key)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    return status;
}
/** \brief Generates the response based on Key and Challenge provided
 *  \param[in]  key         Input data contains device's key
 *  \param[in]  challenge   Input data to be used in challenge response calculation
 *  \param[out] response    response derived from key and challenge
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_generate_challenge_response_pair(uint8_t* key, uint8_t* challenge, uint8_t* response)
{
    ATCA_STATUS status;
    uint8_t sha_data[88];
    uint8_t* p_temp = sha_data;
    uint8_t param1 = 0x0; //Should use the same value while authenticating with device
    uint16_t param2 = 0x0;
    uint8_t sn[9];

    if ((key == NULL) || (challenge == NULL) || (response == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        if ((status = atcab_read_serial_number(sn)) != ATCA_SUCCESS)
        {
            break;
        }

        memcpy(p_temp, key, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;
        memcpy(p_temp, challenge, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;
        *p_temp++ = ATCA_MAC;
        *p_temp++ = param1;
        *p_temp++ = param2 & 0xFF;
        *p_temp++ = (param2 >> 8) & 0xFF;
        memset(p_temp, 0, (8 + 3));
        p_temp += (8 + 3);
        *p_temp++ = sn[8];
        memset(p_temp, 0, 4);
        p_temp += 4;
        *p_temp++ = sn[0];
        *p_temp++ = sn[1];
        memset(p_temp, 0, 2);

        if ((status = atcac_sw_sha2_256(sha_data, sizeof(sha_data), response)) != ATCA_SUCCESS)
        {
            break;
        }

    }
    while (0);

    return status;
}
/** \brief verifies the challenge and provided response using key in device
 *  \param[in]  challenge           Challenge to be used in the response calculations
 *  \param[in]  expected_response   Expected response from the device.
 *  \param[out] is_authenticated    result of expected of response and calcualted response
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_authenticate(uint8_t* challenge, uint8_t* expected_response, uint8_t* is_authenticated)
{
    ATCA_STATUS status;
    uint8_t is_consumed;
    uint8_t digest[ATCA_KEY_SIZE];

    if ((challenge == NULL) || (expected_response == NULL) || (is_authenticated == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        if ((status = sha206a_verify_device_consumption(&is_consumed)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((is_consumed & ATCA_SHA206A_PKEY_CONSUMPTION_MASK) == ATCA_SHA206A_PKEY_CONSUMPTION_MASK)
        {
            status = ATCA_USE_FLAGS_CONSUMED;
            break;
        }
        else if ((is_consumed & ATCA_SHA206A_DKEY_CONSUMPTION_MASK) == ATCA_SHA206A_DKEY_CONSUMPTION_MASK)
        {
            if ((status = atcab_derivekey(0xFF, 0x0, NULL)) != ATCA_SUCCESS)
            {
                break;
            }
        }

        if ((status = atcab_mac(0x00, ATCA_SHA206A_SYMMETRIC_KEY_ID_SLOT, challenge, digest)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    *is_authenticated = false;
    if ((status == ATCA_SUCCESS) && (memcmp(digest, expected_response, ATCA_KEY_SIZE) == 0))
    {
        *is_authenticated = true;
    }

    return status;
}
/** \brief verifies the device is fully consumed or not based on Parent and Derived Key use flags.
 *  \param[out] is_consumed    result of device consumption
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_verify_device_consumption(uint8_t* is_consumed)
{
    ATCA_STATUS status;
    uint8_t is_dk_valid;
    uint8_t is_pk_valid = false;

    if (is_consumed == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *is_consumed = ATCA_SHA206A_DKEY_CONSUMPTION_MASK | ATCA_SHA206A_PKEY_CONSUMPTION_MASK;
    do
    {
        if ((status = sha206a_check_dk_useflag_validity(&is_dk_valid)) != ATCA_SUCCESS)
        {
            break;
        }

        if (is_dk_valid)
        {
            *is_consumed = 0;
        }
        else
        {
            if ((status = sha206a_check_pk_useflag_validity(&is_pk_valid)) != ATCA_SUCCESS)
            {
                break;
            }

            if (is_pk_valid)
            {
                *is_consumed &= ~ATCA_SHA206A_PKEY_CONSUMPTION_MASK;
            }
        }
    }
    while (0);

    return status;
}
/** \brief verifies Derived Key use flags for consumption
 *  \param[out] is_consumed    indicates if DK is available for consumption.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_check_dk_useflag_validity(uint8_t* is_consumed)
{
    ATCA_STATUS status;
    uint8_t dk_available_count;

    if (is_consumed == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *is_consumed = true;
    if ((status = sha206a_get_dk_useflag_count(&dk_available_count)) != ATCA_SUCCESS)
    {
        return status;
    }

    if (!dk_available_count)
    {
        *is_consumed = false;
    }

    return status;
}
/** \brief verifies Parent Key use flags for consumption
 *  \param[out] is_consumed    indicates if PK is available for consumption
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS sha206a_check_pk_useflag_validity(uint8_t* is_consumed)
{
    ATCA_STATUS status;
    uint8_t pk_available_count;

    if (is_consumed == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *is_consumed = true;
    if ((status = sha206a_get_pk_useflag_count(&pk_available_count)) != ATCA_SUCCESS)
    {
        return status;
    }

    if (!pk_available_count)
    {
        *is_consumed = false;
    }

    return status;
}
/** \brief calculates available Derived Key use counts
 *  \param[out] dk_available_count    counts available bit's as 1
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_get_dk_useflag_count(uint8_t* dk_available_count)
{
    ATCA_STATUS status;
    uint8_t read_buf[4];
    uint8_t bit_count;

    if (dk_available_count == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *dk_available_count = 0;
    if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 64, read_buf, sizeof(read_buf))) != ATCA_SUCCESS)
    {
        return status;
    }

    for (bit_count = 0; bit_count < 8; bit_count++)
    {
        if (!(read_buf[2] & 0x01))
        {
            break;
        }

        read_buf[2] >>= 1;
    }

    *dk_available_count = bit_count;

    return status;
}
/** \brief calculates available Parent Key use counts
 *  \param[out] pk_available_count    counts available bit's as 1
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_get_pk_useflag_count(uint8_t* pk_available_count)
{
    ATCA_STATUS status;
    uint8_t read_buf[16];
    uint8_t byte_count;
    uint8_t bit_count;

    if (pk_available_count == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *pk_available_count = 0;
    if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 68, read_buf, sizeof(read_buf))) != ATCA_SUCCESS)
    {
        return status;
    }

    for (bit_count = 0, byte_count = 16; bit_count < sizeof(read_buf) * 8; bit_count++)
    {
        if (bit_count % 8 == 0)
        {
            byte_count--;
        }

        if (!(read_buf[byte_count] & 0x01))
        {
            break;
        }

        read_buf[byte_count] >>= 1;
    }

    *pk_available_count = bit_count;

    return status;
}
/** \brief Read Derived Key slot update count. It will be wraps around 256
 *  \param[out] dk_update_count    returns number of times the slot has been updated with derived key
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_get_dk_update_count(uint8_t* dk_update_count)
{
    ATCA_STATUS status;
    uint8_t read_buf[4];

    if (dk_update_count == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *dk_update_count = 0;
    if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 64, read_buf, sizeof(read_buf))) != ATCA_SUCCESS)
    {
        return status;
    }

    *dk_update_count = read_buf[3];

    return status;
}

/** \brief Update the data store slot with user data and lock it if necessary
 *  \param[in] slot Slot number to be written with data
 *  \param[in] data Pointer that holds the data
 *  \param[in] block   32-byte block to write to.
 *  \param[in] offset  4-byte word within the specified block to write to. If
 *                     performing a 32-byte write, this should be 0.
 *  \param[in] len data length
 *  \param[in] lock_after_write set 1 to lock slot after write, otherwise 0
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_write_data_store(uint8_t slot, uint8_t* data, uint8_t block, uint8_t offset, uint8_t len, bool lock_after_write)
{
    ATCA_STATUS status;
    uint8_t zone = ATCA_ZONE_DATA;

    if ((slot < SHA206A_DATA_STORE0) || (slot > SHA206A_DATA_STORE2) ||
        (slot == SHA206A_DATA_STORE0 && lock_after_write == true) || (data == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (lock_after_write)
    {
        zone |= ATCA_SHA206A_ZONE_WRITE_LOCK;
    }

    status = atcab_write_zone(zone, slot, block, offset, data, len);

    return status;
}
/** \brief Read the data stored in Data store
 *  \param[in] slot Slot number to read from
 *  \param[in] data Pointer to hold slot data data
 *  \param[in]  offset  Byte offset within the zone to read from.
 *  \param[in] len data length
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_read_data_store(uint8_t slot, uint8_t* data, uint8_t offset, uint8_t len)
{
    ATCA_STATUS status;
    uint8_t zone = ATCA_ZONE_DATA;

    if ((slot < SHA206A_DATA_STORE0) || (slot > SHA206A_DATA_STORE2) || (data == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_read_bytes_zone(zone, slot, offset, data, len);

    return status;
}
/** \brief Returns the lock status of the given data store
 *  \param[in] slot Slot number of the data store
 *  \param[out] is_locked lock status of the data store
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS sha206a_get_data_store_lock_status(uint8_t slot, uint8_t* is_locked)
{
    ATCA_STATUS status;
    uint8_t read_buf[4];

    if ((is_locked == NULL) || (slot < SHA206A_DATA_STORE1) || (slot > SHA206A_DATA_STORE2))
    {
        return ATCA_BAD_PARAM;
    }

    *is_locked = true;

    if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 4, &read_buf[0], sizeof(read_buf))) != ATCA_SUCCESS)
    {
        return status;
    }

    if (slot == SHA206A_DATA_STORE1)
    {
        *is_locked = ((read_buf[0] == 0x55) && (read_buf[1] == 0x55)) ? false : true;
    }
    else
    {
        *is_locked = ((read_buf[2] == 0x55) && (read_buf[3] == 0x55)) ? false : true;
    }

    return status;
}

#endif
