/***************************************************************************//**
* \file <wiced_firmware_upgrade.h>
*
* \brief Provides definitions and function prototypes for the common
*        functionality used during the AIROC firmware upgrade procedure.
*        Primarily, the functionality provided is for managing, storing,
*        and retrieving information to/from memory.The data being stored
*        are the DS portions of the memory map.
*//*****************************************************************************
* Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#ifndef WICED_FIRMWARE_UPGRADE_H
#define WICED_FIRMWARE_UPGRADE_H

#include "wiced.h"

/**
*
* \addtogroup group_wiced_firmware_upgrade   AIROC Firmware Upgrade
* \ingroup    wicedsys
* @{
*
******************************************************************************/

#define OTA_FW_UPGRADE_CHUNK_SIZE_TO_COMMIT         512

/******************************************************************************
*
* Recommended firmware-upgrade 4-MBit serial flash offsets.
* -------------------------------------------------------------------------------------------------------------------
* |  SS1 (4K @ 0)  |  Fail safe area(4K @ 0x1000)  |  VS1 (4K @ 0x2000)  | VS2 (4K @ 0x3000)  | DS1 (248K @ 0x4000)  | DS2 (248K @ 0x42000)
*  -------------------------------------------------------------------------------------------------------------------
*
* For reference only.
*
*    uint32_t ss_locations          = 0x0000;
*    uint32_t vs_location1          = 0x2000;      VS section occupies 1 sector
*    uint32_t vs_length1            = 0x1000;      4K = 1 SF sector
*    uint32_t vs_location2          = 0x3000;      Double buffer for VS
*    uint32_t vs_length2            = 0x1000;      4K = 1 SF sector = vs_length1
*    uint32_t ds1_location          = 0x4000;
*    uint32_t ds1_length            = 0x3E000;     240K
*    uint32_t ds2_location          = 0x42000;
*    uint32_t ds2_length            = 0x3E000;     240K = ds1 length
******************************************************************************/
typedef struct
{
    uint32_t ss_loc;    /** Static section location */
    uint32_t ds1_loc;   /** ds1 location */
    uint32_t ds1_len;   /** ds1 length */
    uint32_t ds2_loc;   /** ds2 location */
    uint32_t ds2_len;   /** ds2 length */
    uint32_t vs1_loc;   /** Vendor specific location 1 */
    uint32_t vs1_len;   /** Vendor specific location 1 length */
    uint32_t vs2_loc;   /** Vendor specific location 2 */
    uint32_t vs2_len;   /** Vendor specific location 2 length */
} wiced_fw_upgrade_nv_loc_len_t;

/******************************************************************************
* Function Name: wiced_firmware_upgrade_init
***************************************************************************//**
* \brief Initializes the AIROC Firmware Upgrade module.
*
* \details Called by the application during initialization
*          to initialize the upgrade module with serial flash offsets.
*
* \param p_sflash_nv_loc_len   Offsets of different sections present in serial flash.
* \param sflash_size           Serial flash size present on the tag board (default size 4MB ).
*
******************************************************************************/
wiced_bool_t wiced_firmware_upgrade_init(wiced_fw_upgrade_nv_loc_len_t *p_sflash_nv_loc_len, uint32_t sflash_size);

/******************************************************************************
* Function Name: wiced_firmware_upgrade_init_nv_locations
***************************************************************************//**
* \brief Initializes NV locations.
*
* \details The application calls this function during a start of firmware download
*          to set up memory locations depending on which partition is being used.
*
******************************************************************************/
uint32_t     wiced_firmware_upgrade_init_nv_locations(void);

/******************************************************************************
* Function Name: wiced_firmware_upgrade_store_to_nv
***************************************************************************//**
* \brief Stores a memory chunk to memory.
*
* \details The application can call this function to store a next memory chunk in
* none-volatile memory.  The application does not need to know which type of memory is
* used or which partition is being upgraded.
*
* \param offset Offset in the memory to store data.
* \param data   The pointer to a chunk of data to store.
* \param len    The size of a memory chunk to store.
*
******************************************************************************/
uint32_t   wiced_firmware_upgrade_store_to_nv(uint32_t offset, uint8_t *data, uint32_t len);

/******************************************************************************
* Function Name: wiced_firmware_upgrade_retrieve_from_nv
***************************************************************************//**
* \brief Retrieves a memory chunk from memory.
*
* \details The application calls this function when an upgrade process
*          is completed to verify data was successfully stored.
*          The application does not need to know which type of memory
*          is used or which partition is being upgraded.
*
* \param offset Offset in the memory to retrieve data from.
* \param data   The pointer to the location to retrieve data.
* \param len    The size of a memory chunk to retrieve.
*
******************************************************************************/
uint32_t   wiced_firmware_upgrade_retrieve_from_nv(uint32_t offset, uint8_t *data, uint32_t len);

/******************************************************************************
* Function Name: wiced_firmware_upgrade_finish
***************************************************************************//**
* \brief Retrieves a memory chunk from memory.
*
* \details The application calls this function after download is completed and verified,
*          to switch active partitions with the one that received a new image.
*
******************************************************************************/
void     wiced_firmware_upgrade_finish(void);

/******************************************************************************
*
* Stored in DS header area, product id and version used to compare with upgrade image.
*
******************************************************************************/
typedef struct
{
    uint16_t product_id;
    uint8_t major;
    uint8_t minor;
} wiced_bt_application_id_t;

/******************************************************************************
* Function Name: wiced_get_current_app_id_and_version
***************************************************************************//**
* \brief Retrieves application version information from nvram.
*
* \details The application calls this function to retrieve the appplication id
*          and version to compare with the downloaded image.
*
******************************************************************************/
wiced_bool_t wiced_get_current_app_id_and_version(wiced_bt_application_id_t *app_id_and_version);

#endif /* WICED_FIRMWARE_UPGRADE_H */
/** \} group_wiced_firmware_upgrade */
