/**
 * \file
 *
 * \brief Provides api interfaces to use with ATSHA206A device.
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
#ifndef API_206A_H
#define API_206A_H

#ifdef __cplusplus
extern "C" {
#endif

#include "atca_status.h"

#define ATCA_SHA206A_ZONE_WRITE_LOCK        0x20
#define ATCA_SHA206A_DKEY_CONSUMPTION_MASK  0x01
#define ATCA_SHA206A_PKEY_CONSUMPTION_MASK  0x02
#define ATCA_SHA206A_SYMMETRIC_KEY_ID_SLOT  0X07

enum
{
    SHA206A_DATA_STORE0=8,
    SHA206A_DATA_STORE1,
    SHA206A_DATA_STORE2,
};


ATCA_STATUS sha206a_diversify_parent_key(uint8_t* parent_key, uint8_t* diversified_key);
ATCA_STATUS sha206a_generate_derive_key(uint8_t* parent_key, uint8_t* derived_key, uint8_t param1, uint16_t param2);
ATCA_STATUS sha206a_generate_challenge_response_pair(uint8_t* key, uint8_t* challenge, uint8_t* response);
ATCA_STATUS sha206a_authenticate(uint8_t* challenge, uint8_t* expected_response, uint8_t* is_authenticated);
ATCA_STATUS sha206a_verify_device_consumption(uint8_t* is_consumed);

ATCA_STATUS sha206a_check_dk_useflag_validity(uint8_t* is_valid);
ATCA_STATUS sha206a_check_pk_useflag_validity(uint8_t* is_valid);
ATCA_STATUS sha206a_get_dk_useflag_count(uint8_t* dk_available_count);
ATCA_STATUS sha206a_get_pk_useflag_count(uint8_t* pk_available_count);
ATCA_STATUS sha206a_get_dk_update_count(uint8_t* dk_update_count);

ATCA_STATUS sha206a_write_data_store(uint8_t slot, uint8_t* data, uint8_t block, uint8_t offset, uint8_t len, bool lock_after_write);
ATCA_STATUS sha206a_read_data_store(uint8_t slot, uint8_t* data, uint8_t offset, uint8_t len);
ATCA_STATUS sha206a_get_data_store_lock_status(uint8_t slot, uint8_t* is_locked);

#ifdef __cplusplus
}
#endif

#endif
