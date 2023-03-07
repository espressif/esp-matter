/**
 * @file
 * Configuration file for the ZAF 
 *
 * @copyright 2022 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <zaf_config.h>
#include <zaf_config_api.h>
#include <zw_build_no.h>
/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/****************************************************************************/
/*                             PUBLIC FUNCTIONS                             */
/****************************************************************************/

uint8_t zaf_config_get_bootloader_upgradable(void)
{
    return ZAF_CONFIG_BOOTLOADER_UPGRADABLE;
}

uint8_t zaf_config_get_bootloader_target_id(void)
{
    return ZAF_CONFIG_BOOTLOADER_TARGET_ID;
}

uint16_t zaf_config_get_build_no(void)
{
    return (uint16_t)ZAF_BUILD_NO;
}

uint8_t zaf_config_get_hardware_version(void)
{
  return ZAF_CONFIG_HARDWARE_VERSION;
}

uint8_t zaf_config_get_firmware_target_count(void)
{
  return ZAF_CONFIG_FIRMWARE_TARGET_COUNT;
}
