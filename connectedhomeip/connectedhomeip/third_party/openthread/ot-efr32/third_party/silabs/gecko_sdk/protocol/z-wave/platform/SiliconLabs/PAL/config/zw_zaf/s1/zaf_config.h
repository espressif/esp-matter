/**
 * @file
 * @copyright 2021 Silicon Laboratories Inc.
 */
#ifndef _ZAF_CONFIG_H_
#define _ZAF_CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Command Class Firmware Update - Bootloader update

// <q> Indicates if the bootloader can be updated
// <i> Includes the bootloader as a target
// <i> Default: 0
#if !defined(ZAF_CONFIG_BOOTLOADER_UPGRADABLE)
#define ZAF_CONFIG_BOOTLOADER_UPGRADABLE  0
#endif /* !defined(ZAF_CONFIG_BOOTLOADER_UPGRADABLE) */

// <o> The bootloader target id <1..255:1>
// <i> If the bootloader update is enabled
// <i> This determines which target id should be used by the bootloader firmware
// <i> Default: 1
#if !defined(ZAF_CONFIG_BOOTLOADER_TARGET_ID)
#define ZAF_CONFIG_BOOTLOADER_TARGET_ID  1
#endif /* !defined(ZAF_CONFIG_BOOTLOADER_TARGET_ID) */
// </h>

// <h>Command Class Firmware Update - firmware targets

// <o> Number of firmware targets <0..10:1>
// <n> Defines the number of supported firmware targets. Setting this number to more than 1 requires
// <n> the application to implement CC_Version_GetFirmwareVersion_handler(). The default value is 1
// <n> because firmware target 0 is the Z-Wave application firmware (including Z-Wave stack).
// <d> 1
#define ZAF_CONFIG_FIRMWARE_TARGET_COUNT 1

// </h>

// <h>Version Configuration

// <o> Hardware version <0..255:1>
// <i>
// <d> 1
/**
 * The Hardware Version field MUST report a value which is unique to this particular
 * version of the product. It MUST be possible to uniquely determine the hardware
 * characteristics from the Hardware Version field in combination with the Manufacturer
 * ID, Product Type ID and Product ID fields of Manufacturer Specific Info Report
 * of the Manufacturer Specific Command Class.
 * This information allows a user to pick a firmware image version that is guaranteed
 * to work with this particular version of the product.
 * Note that the Hardware Version field is intended for the hardware version of the
 * entire product, not just the version of the Z-Wave radio chip.
 */
#define ZAF_CONFIG_HARDWARE_VERSION 1

// </h>

// <<< end of configuration section >>>

#endif /* _ZAF_CONFIG_H_ */
