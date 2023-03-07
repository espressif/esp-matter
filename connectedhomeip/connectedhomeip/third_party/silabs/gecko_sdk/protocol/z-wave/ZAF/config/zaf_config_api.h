/**
 * @file
 * @copyright 2022 Silicon Laboratories Inc.
 */
#ifndef _ZAF_CONFIG_API_H_
#define _ZAF_CONFIG_API_H_

#include <stdbool.h>
#include <stdint.h>
#include <ZW_classcmd.h>



/**
 * @brief Indicates if the bootloader is upgradable
 * 
 * @return uint8_t 1 if the bootloader is upgradable else 0
 */
uint8_t zaf_config_get_bootloader_upgradable(void);

/**
 * @brief Returns the target id of the bootloader
 * 
 * This function is only relevant if \ref zaf_config_get_bootloader_upgradable 
 * is true. The returned target id can be invalid if the bootloader is 
 * not upgradable
 * 
 * @return uint8_t the bootloader target id
 */
uint8_t zaf_config_get_bootloader_target_id(void);

/**
 * @brief
 * 
 * @param
 *   
 * @return 
 */
uint16_t zaf_config_get_build_no(void);

/**
 * Returns the configured hardware version.
 * @return hardware version
 */
uint8_t zaf_config_get_hardware_version(void);

/**
 * Returns the configured hardware version.
 * @return hardware version
 */
uint8_t zaf_config_get_firmware_target_count(void);

#endif /* _ZAF_CONFIG_API_H_ */
