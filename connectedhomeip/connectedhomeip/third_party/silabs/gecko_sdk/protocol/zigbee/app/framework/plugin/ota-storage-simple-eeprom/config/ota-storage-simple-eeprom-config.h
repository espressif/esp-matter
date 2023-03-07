/***************************************************************************//**
 * @brief Zigbee OTA Simple Storage EEPROM Driver component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee OTA Simple Storage EEPROM Driver configuration

// <q EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_SOC_BOOTLOADING_SUPPORT> SOC Bootloading Support
// <i> Default: TRUE
// <i> This option enables bootloading support for SOC devices.  When enabled, it will re-map the OTA image file so that the EBL data is at the top of the EEPROM and therefore can be accessed by all existing Ember bootloaders.  It requires that the EBL portion of the image is the first TAG in the file.  The OTA storage starting offset should be 0 when this is enabled.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_SOC_BOOTLOADING_SUPPORT   1

// <o EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_GECKO_BOOTLOADER_STORAGE_SUPPORT> Gecko Bootloader Storage Support
// <i> Gecko Bootloader Storage Support
// <DO_NOT_USE_SLOTS=> Do not use slots
// <USE_FIRST_SLOT=> Use first slot
// <USE_LAST_SLOT=> Use last slot
// <USE_SPECIFIC_SLOT=> Use specific slot
// <i> Default: DO_NOT_USE_SLOTS
// <i> This option dictates the method for saving OTA images to slots. This is only applicable if a Gecko storage bootloader is running on the chip. The Slot Manager plugin must be selected in order for slots to be used. If a Gecko storage bootloader is not present on the chip, the offsets entered below will be used. If "Do not use slots" is selected, then the offsets entered below will be used to determine where to save the image. This is not recommended, as using set offsets to addresses with a Gecko storage bootloader requires knowledge of storage slot addresses and boundaries. A mismatch in addresses will cause OTA to not work.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_GECKO_BOOTLOADER_STORAGE_SUPPORT   DO_NOT_USE_SLOTS

// <o EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_SLOT_TO_USE> Storage Slot To Save Images To <0-255>
// <i> Default: 0
// <i> If the user selects "Use specific slot" for Gecko Bootloader Storage Support, then this value dicatates the slot to use.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_SLOT_TO_USE   0

// <o EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_STORAGE_START> OTA Storage Start Offset <0-4294967295>
// <i> Default: 0
// <i> This is the starting offset for where the OTA image will be stored in the EEPROM.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_STORAGE_START   0

// <o EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_STORAGE_END> OTA Storage End Offset <0-4294967295>
// <i> Default: 262144
// <i> This is the last offset for where the OTA image may be stored in the EEPROM.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_STORAGE_END   262144

// <q EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_READ_MODIFY_WRITE_SUPPORT> SOC Read-Modify-Write Support
// <i> Default: FALSE
// <i> This indicates to the OTA code whether the underlying EEPROM driver has support for 'read-modify-write'.  Read-modify-write assumes a page erase is not required prior to writing, and any location or length of data can be re-written.  If not present, then the OTA code will note each full page of data downloaded and must erase entire pages before downloading a chunk of data.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_READ_MODIFY_WRITE_SUPPORT   0

// <o EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_DOWNLOAD_OFFSET_SAVE_RATE> Frequency for Saving Download Offset to EEPROM (bytes) <0-32768>
// <i> Default: 1024
// <i> How often the current download offset is stored to EEPROM, in bytes.  If set to 0 it will always be written to EEPROM.  This is only used for "read-modify-write" drivers.
#define EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_DOWNLOAD_OFFSET_SAVE_RATE   1024

// </h>

// <<< end of configuration section >>>
