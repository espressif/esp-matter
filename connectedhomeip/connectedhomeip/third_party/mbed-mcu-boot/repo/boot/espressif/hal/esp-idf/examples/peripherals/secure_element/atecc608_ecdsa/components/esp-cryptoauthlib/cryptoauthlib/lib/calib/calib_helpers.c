/**
 * \file
 * \brief CryptoAuthLib Basic API - Helper Functions to
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

#include "cryptoauthlib.h"

/** \brief Executes Read command, which reads the configuration zone to see if
 *          the specified slot is locked.
 *
 *  \param[in]  device     Device context pointer
 *  \param[in]  slot       Slot to query for locked (slot 0-15)
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_is_slot_locked(ATCADevice device, uint16_t slot, bool* is_locked)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t data[ATCA_WORD_SIZE];
    uint16_t slot_locked;

    do
    {
        if ((slot > 15) || (is_locked == NULL))
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "Either Invalid slot or NULL pointer received");
            break;
        }

        // Read the word with the lock bytes ( SlotLock[2], RFU[2] ) (config block = 2, word offset = 6)
        if ((status = calib_read_zone(device, ATCA_ZONE_CONFIG, 0, 2 /*block*/, 6 /*offset*/, data, ATCA_WORD_SIZE)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_read_zone - failed");
            break;
        }

        slot_locked = ((uint16_t)data[0]) | ((uint16_t)data[1] << 8);
        *is_locked = ((slot_locked & (1 << slot)) == 0);
    }
    while (0);

    return status;
}

/** \brief Executes Read command, which reads the configuration zone to see if
 *          the specified zone is locked.
 *
 *  \param[in]  device     Device context pointer
 *  \param[in]  zone       The zone to query for locked (use LOCK_ZONE_CONFIG or
 *                         LOCK_ZONE_DATA).
 *  \param[out] is_locked  Lock state returned here. True if locked.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS calib_is_locked(ATCADevice device, uint8_t zone, bool* is_locked)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t data[ATCA_WORD_SIZE];

    do
    {
        if (is_locked == NULL)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
            break;
        }

        // Read the word with the lock bytes (UserExtra, Selector, LockValue, LockConfig) (config block = 2, word offset = 5)
        if ((status = calib_read_zone(device, ATCA_ZONE_CONFIG, 0, 2 /*block*/, 5 /*offset*/, data, ATCA_WORD_SIZE)) != ATCA_SUCCESS)
        {
            ATCA_TRACE(status, "calib_read_zone - failed");
            break;
        }

        // Determine the index into the word_data based on the zone we are querying for
        switch (zone)
        {
        case LOCK_ZONE_CONFIG: *is_locked = (data[3] != 0x55); break;
        case LOCK_ZONE_DATA:   *is_locked = (data[2] != 0x55); break;
        default: status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received"); break;
        }
    }
    while (0);

    return status;
}


#ifdef ATCA_ECC204_SUPPORT
/** \brief Use Info command to check ECC204 Config zone lock status
 *
 *  \param[in]   device       Device context pointer
 *  \param[out]  is_locked    return lock status
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_ecc204_is_config_locked(ATCADevice device, bool* is_locked)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint16_t param2;
    uint8_t slot = 0;

    if (NULL == is_locked)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    while (slot <= 3)
    {
        param2 = ATCA_ECC204_ZONE_CONFIG | (slot << 1);
        if (ATCA_SUCCESS != (status = calib_info_lock_status(device, param2, (uint8_t*)is_locked)))
        {
            *is_locked = false;
            break;
        }

        if (*is_locked)
        {
            slot += 1; // increment slot
        }
        else
        {
            break;
        }
    }

    return status;
}

/** \brief Use Info command to check ECC204 Data zone lock status
 *
 *  \param[in]   device       Device context pointer
 *  \param[out]  is_locked    return lock status
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_ecc204_is_data_locked(ATCADevice device, bool* is_locked)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint16_t param2;
    uint8_t slot = 0;

    if (NULL == is_locked)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer encountered");
    }

    while (slot <= 3)
    {
        param2 = ATCA_ECC204_ZONE_DATA | (slot << 1);
        if (ATCA_SUCCESS != (status = calib_info_lock_status(device, param2, (uint8_t*)is_locked)))
        {
            *is_locked = false;
            break;
        }

        if (*is_locked)
        {
            slot += 1; // increment slot
        }
        else
        {
            break;
        }
    }

    return status;
}

/** \brief Use Info command to check config/data is locked or not
 *
 *  \param[in]   device     Device contect pointer
 *  \param[in]   zone       Config/Data zone
 *  \param[out]  is_locked  return lock status here
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_ecc204_is_locked(ATCADevice device, uint8_t zone, bool* is_locked)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (ATCA_ECC204_ZONE_CONFIG == zone)
    {
        status = calib_ecc204_is_config_locked(device, is_locked);
    }
    else if (ATCA_ECC204_ZONE_DATA == zone)
    {
        status = calib_ecc204_is_data_locked(device, is_locked);
    }
    else
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid zone received");
    }

    return status;
}

#endif


/** \brief Check if a slot is a private key
 *
 *  \param[in]   device         Device context pointer
 *  \param[in]   slot           Slot to query (slot 0-15)
 *  \param[out]  is_private     return true if private
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code
 */
ATCA_STATUS calib_is_private(ATCADevice device, uint16_t slot, bool* is_private)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    ATCADeviceType dev_type = atcab_get_device_type_ext(device);

    if (device && is_private)
    {
        switch (dev_type)
        {
        case ATECC108A:
        /* fallthrough */
        case ATECC508A:
        /* fallthrough */
        case ATECC608:
        {
            uint8_t key_config[2] = { 0 };
            if (ATCA_SUCCESS == (status = calib_read_bytes_zone(device, ATCA_ZONE_CONFIG, 0, ATCA_KEY_CONFIG_OFFSET((size_t)slot), key_config, sizeof(key_config))))
            {
                *is_private = (key_config[0] & ATCA_KEY_CONFIG_PRIVATE_MASK);
            }
            break;
        }
#ifdef ATCA_ECC204_SUPPORT
        case ECC204:
            break;
#endif
        default:
            *is_private = false;
            break;
        }
    }

    return status;
}
