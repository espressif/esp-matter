/***************************************************************************//**
 * @file
 * @brief wMBus support component
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef _SL_WMBUS_SUPPORT_H_
#define _SL_WMBUS_SUPPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"

/**
 * @addtogroup Header_helpers
 * @{
 */

/**
 * @enum WMBUS_functionCode_t
 * @brief Function code to use in the first block's C-field
 */
typedef enum {
  WMBUS_FUNCTION_SND_NKE = 0x0, /**< SND-NKE */
  WMBUS_FUNCTION_SND_UD = 0x3,  /**< SND-UD/SND-UD2 */
  WMBUS_FUNCTION_SND_NR = 0x4,  /**< SND-NR */
  WMBUS_FUNCTION_SND_IR = 0x6,  /**< SND-IR */
  WMBUS_FUNCTION_ACC_NR = 0x7,  /**< ACC-NR */
  WMBUS_FUNCTION_ACC_DMD = 0x8, /**< ACC-DMD */
  WMBUS_FUNCTION_REQ_UD1 = 0xA, /**< REQ-UD1 */
  WMBUS_FUNCTION_REQ_UD2 = 0xB, /**< REQ-UD2 */
} WMBUS_functionCode_t;

/**
 * @enum WMBUS_deviceType_t
 * @brief Device type to be used in the address field(s)
 */
typedef enum {
  WMBUS_DEVICE_OTHER = 0x00,                    /**< Other */
  WMBUS_DEVICE_OIL_METER,                       /**< Oil meter */
  WMBUS_DEVICE_ELECTRICITY_METER,               /**< Electricity meter */
  WMBUS_DEVICE_GAS_METER,                       /**< Gas meter */
  WMBUS_DEVICE_HEAT_METER,                      /**< Heat meter */
  WMBUS_DEVICE_STEAM_METER,                     /**< Steam meter */
  WMBUS_DEVICE_WARM_WATER_METER,                /**< Warm water meter (30-90C) */
  WMBUS_DEVICE_WATER_METER,                     /**< Water meter */
  WMBUS_DEVICE_HEAT_COST_ALLOCATOR,             /**< Heat cost allocator */
  WMBUS_DEVICE_COMPRESSED_AIR,                  /**< Compressed air */
  WMBUS_DEVICE_COOLING_METER_OUTLET,            /**< Cooling meter (volume measured at return temperature: outlet) */
  WMBUS_DEVICE_COOLING_METER_INLET,             /**< Cooling meter (volume measured at flow temperature: inlet) */
  WMBUS_DEVICE_HEAT_METER_INLET,                /**< Heat meter (volume measured at flow temperature: inlet) */
  WMBUS_DEVICE_COMBINED_HEAT_COOLING_METER,     /**< Combined heat/cooling meter */
  WMBUS_DEVICE_BUS_SYSTEM_COMPONENT,            /**< Bus/system component */
  WMBUS_DEVICE_CALORIFIC_VALUE = 0x14,          /**< Calorific value */
  WMBUS_DEVICE_HOT_WATER_METER,                 /**< Hot water meter (>90C) */
  WMBUS_DEVICE_COLD_WATER_METER,                /**< Cold water meter */
  WMBUS_DEVICE_DUAL_WATER_METER,                /**< Dual register (hot/cold) water meter */
  WMBUS_DEVICE_PRESSURE_METER,                  /**< Pressure meter */
  WMBUS_DEVICE_AD_CONVERTER,                    /**< A/D converter */
  WMBUS_DEVICE_SMOKE_DETECTOR,                  /**< Smoke detector */
  WMBUS_DEVICE_ROOM_SENSOR,                     /**< Room sensor (e.g. temperature or humidity) */
  WMBUS_DEVICE_GAS_DETECTOR,                    /**< Gas detector */
  WMBUS_DEVICE_BREAKER = 0x20,                  /**< Breaker (electricity) */
  WMBUS_DEVICE_VALVE,                           /**< Valve (gas or water) */
  WMBUS_DEVICE_CUSTOMER_UNIT = 0x25,            /**< Customer unit (display device) */
  WMBUS_DEVICE_WASTE_WATER_METER = 0x28,        /**< Waste water meter */
  WMBUS_DEVICE_GARBAGE,                         /**< Garbage */
  WMBUS_DEVICE_CARBON_DIOXIDE,                  /**< Reserved for carbon dioxide */
  WMBUS_DEVICE_COMMUNICATION_CONTROLLER = 0x31, /**< Communication controller (Gateway) */
  WMBUS_DEVICE_UNIDIRECTIONAL_REPEATER,         /**< Unidirectional repeater */
  WMBUS_DEVICE_BIDIRECTIONAL_REPEATER,          /**< Bidirectional repeater */
  WMBUS_DEVICE_RADIO_CONVERTER_SYSTEM = 0x36,   /**< Radio converter (system side) */
  WMBUS_DEVICE_RADIO_CONVERTER_METER,           /**< Radio converter (meter side) */
} WMBUS_deviceType_t;

/**
 * @enum WBMUS_ciField_t
 * @brief CI field codes
 */
typedef enum {
  WMBUS_CI_DATA_TO_METER_NONE = 0x51,               /**< Data sent by readout device to the Meter without transport layer */
  WMBUS_CI_MBUS_DATA_TO_METER_SHORT = 0x5A,         /**< MBUS data sent by readout device to the Meter with short transport layer */
  WMBUS_CI_MBUS_DATA_TO_METER_LONG,                 /**< MBUS data sent by readout device to the Meter with long transport layer */
  WMBUS_CI_COSEM_DATA_TO_METER_LONG = 0x60,         /**< COSEM data sent by readout device to the Meter with long transport layer */
  WMBUS_CI_COSEM_DATA_TO_METER_SHORT,               /**< COSEM data sent by readout device to the Meter with short transport layer */
  WMBUS_CI_OBIS_DATA_TO_METER_LONG = 0x64,          /**< OBIS based data sent by readout device to the Meter with long transport layer */
  WMBUS_CI_OBIS_DATA_TO_METER_SHORT,                /**< OBIS based data sent by readout device to the Meter with short transport layer */
  WMBUS_CI_EN13757_3_DATA_TO_METER_NONE = 0x69,     /**< EN13757-3 application layer with Format frame and no transport layer */
  WMBUS_CI_EN13757_3_DATA_TO_METER_SHORT,           /**< EN13757-3 application layer with Format frame and short transport layer */
  WMBUS_CI_EN13757_3_DATA_TO_METER_LONG,            /**< EN13757-3 application layer with Format frame and long transport layer */
  WMBUS_CI_CLOCK_SYNC_ABSOLUTE,                     /**< Clock synchronisation (absolute) */
  WMBUS_CI_CLOCK_SYNC_RELATIVE,                     /**< Clock synchronisation (relative) */
  WMBUS_CI_APPLICATION_ERROR_SHORT,                 /**< Application error from Meter with short transport layer */
  WMBUS_CI_APPLICATION_ERROR_LONG,                  /**< Application error from Meter with long transport layer */
  WMBUS_CI_APPLICATION_ERROR_NONE,                  /**< Application error from Meter with no transport layer */
  WMBUS_CI_ALARM_NONE,                              /**< Alarm from Meter without transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_LONG,              /**< EN13757-3 application layer with long transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_CF_LONG,           /**< EN13757-3 application layer with compact frame and long transport layer */
  WMBUS_CI_ALARM_SHORT,                             /**< Alarm from Meter with short transport layer */
  WMBUS_CI_ALARM_LONG,                              /**< Alarm from Meter with long transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_NONE = 0x78,       /**< EN13757-3 application layer with no transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_CF_NONE,           /**< EN13757-3 application layer with compact frame and no transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_SHORT,             /**< EN13757-3 application layer with short transport layer */
  WMBUS_CI_EN13757_3_APPLICATION_CF_SHORT,          /**< EN13757-3 application layer with compact frame and short transport layer */
  WMBUS_CI_COSEM_APPLICATION_LONG,                  /**< COSEM application layer with long transport layer */
  WMBUS_CI_COSEM_APPLICATION_SHORT,                 /**< COSEM application layer with short transport layer */
  WMBUS_CI_OBIS_APPLICATION_LONG,                   /**< OBIS based application layer with long transport layer */
  WMBUS_CI_OBIS_APPLICATION_SHORT,                  /**< OBIS based application layer with short transport layer */
  WMBUS_CI_EN13757_3_TRANSPORT_TO_METER_LONG,       /**< EN13757-3 Transport layer (long) from readout device to the meter */
  WMBUS_CI_NETWORK_LAYER_DATA,                      /**< Network layer data */
  WMBUS_CI_FOR_FUTURE_USE,                          /**< For future use */
  WMBUS_CI_NETWORK_MANAGEMENT_APPLICATION,          /**< Network management application */
  WMBUS_CI_TRANSPORT_TO_METER_MBUS_COMPACT,         /**< Transport layer to Meter (M Bus-Compact frame expected) */
  WMBUS_CI_NETWORK_MANAGEMENT_DATA = 0x89,          /**< Reserved for Network management data (EN 13757-5) */
  WMBUS_CI_EN13757_3_TRANSPORT_FROM_METER_SHORT,    /**< EN13757-3 Transport layer (short) from the meter to the readout device */
  WMBUS_CI_EN13757_3_TRANSPORT_FROM_METER_LONG,     /**< EN13757-3 Transport layer (long) from the meter to the readout device */
  WMBUS_CI_EXTENDED_LINK_LAYER1,                    /**< Extended Link Layer I (2B) */
  WMBUS_CI_EXTENDED_LINK_LAYER2,                    /**< Extended Link Layer II (8B) */
  WMBUS_CI_EXTENDED_LINK_LAYER3,                    /**< Extended Link Layer III (10B) */
  WMBUS_CI_EXTENDED_LINK_LAYER4,                    /**< Extended Link Layer IV (16B) */
} WBMUS_ciField_t;

/**
 * @enum WMBUS_status_t
 * @brief Application errors coded into the status field
 */
typedef enum {
  WMBUS_STATUS_APPLICATION_BUSY = 0x1,    /**< Application busy */
  WMBUS_STATUS_APPLICATION_ERROR = 0x2,   /**< Any application error */
  WMBUS_STATUS_ABNORMAL_CONDITION = 0x3,  /**< Abnormal condition/alarm */
  WMBUS_STATUS_POWER_LOW = 1 << 2,        /**< Power low */
    WMBUS_STATUS_PERMANENT_ERROR = 1 << 3, /**< Permanent error */
    WMBUS_STATUS_TEMPORARY_ERROR = 1 << 4, /**< Temporary error */
} WMBUS_status_t;

/**
 * @enum WMBUS_accessibility_t
 * @brief Accessibility settings in the config word
 */
typedef enum {
  WMBUS_ACCESSIBILITY_NO_ACCESS = 0,        /**< No access (unidirectional) */
  WMBUS_ACCESSIBILITY_TEMPORARY_NO_ACCESS,  /**< Bidirectional, temporary no access */
  WMBUS_ACCESSIBILITY_LIMITED_ACCESS,       /**< Bidirectional, limited access (short time after TX) */
  WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS,     /**< Bidirectional, unlimited access */
} WMBUS_accessibility_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
_Pragma("pack(1)")
#endif

/**
 * @struct WMBUS_dll_header_t
 * @brief Data link layer header (first block)
 */
typedef struct {
  uint8_t lField;                             /**< Length field */
  union {
    uint8_t raw;                              /**< Control field raw mode */
    struct {
      WMBUS_functionCode_t functionCode : 4;    /**< Control field function code */
      bool fcvDcf : 1;                          /**< Control field fcv/dcf */
      bool fcbAcd : 1;                          /**< Control field fcb/acd */
      bool primary : 1;                         /**< Control field, primary station bit */
    } detailed;                               /**< Control field detailed mode */
  } cField;                                   /**< Control field */
  union {
    uint8_t raw[8];                           /**< Address field raw mode */
    struct {
      uint16_t manufacturer;                  /**< Address, manufacturer ID */
      uint32_t id;                            /**< Address, device ID */
      uint8_t version;                        /**< Address, version */
      uint8_t deviceType;                     /**< Address, device type */
    } detailed;                               /**< Address field detailed mode */
  } address;                                  /**< Address field */
} WMBUS_dll_header_t;

/**
 * @union WMBUS_confWord_t
 * @brief Data link layer header (first block)
 */
typedef union {
  uint16_t raw;                             /**< Raw config word */
  struct {
    bool hopCounter : 1;                     /**< Hopcounter */
    bool repeatedAccess : 1;                 /**< Repeated access */
    uint8_t contentOfMessage : 2;            /**< Content of message */
    uint8_t numOfEncBlocks : 4;              /**< Number of encoded blocks */
    uint8_t mode : 5;                        /**< Mode (security) 5th bit is reserved as per 13757-4, but OMS uses it */
    bool synchronized : 1;                   /**< Synchronized or not */
    WMBUS_accessibility_t accessibility : 2; /**< Accessibility */
  } mode_0_5;                               /**< Detailed config word for mode 0 and 5 */
//add m2_3, m7 and m13 here
} WMBUS_confWord_t;

/**
 * @struct WMBUS_stl_header_t
 * @brief Short transport layer header
 */
typedef struct {
  uint8_t ciField;           /**< CI (control information) field */
  uint8_t accessNumber;      /**< Access number */
  uint8_t status;            /**< Status byte */
  WMBUS_confWord_t confWord; /**< Config word */
} WMBUS_stl_header_t;

/**
 * @struct WMBUS_ltl_header_t
 * @brief Long transport layer header
 */
typedef struct {
  uint8_t ciField;                  /**< CI (control information) field */
  union {
    uint8_t raw[8];                 /**< Raw address */
    struct {
      uint32_t id;                  /**< Address, device ID */
      uint16_t manufacturer;        /**< Address, manufacturer ID */
      uint8_t version;              /**< Address, version */
      uint8_t deviceType;           /**< Address, device type */
    } detailed;                     /**< Detailed address */
  } address;                        /**< Address */
  uint8_t accessNumber;             /**< Access number */
  uint8_t status;                   /**< Status byte */
  WMBUS_confWord_t confWord;        /**< Config word */
} WMBUS_ltl_header_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
_Pragma("pack()")
#endif

/**
 * @}
 */

/**
 * @addtogroup Payload_helpers
 * @{
 */

/**
 * @enum WMBUSframe_dibFunction_t
 * @brief Values for function field in DIB
 */
typedef enum {
  WMBUS_DIB_FUNCTION_INSTANTANEOUS, /**< Instantaneous value */
  WMBUS_DIB_FUNCTION_MINIMUM,       /**< Minimum value */
  WMBUS_DIB_FUNCTION_MAXIMUM,       /**< Maximum value */
  WMBUS_DIB_FUNCTION_ERROR          /**< Value during error state */
} WMBUSframe_dibFunction_t;

/**
 * @enum WMBUS_Mode_t
 * @brief MBUS modes
 */
typedef enum {
  WMBUS_MODE_S,           /**< S mode */
  WMBUS_MODE_T_METER,     /**< T mode, Meter to Other */
  WMBUS_MODE_T_COLLECTOR, /**< T mode, Other to Meter */
  WMBUS_MODE_C,     /**< C mode */
  WMBUS_MODE_F,     /**< F mode */
  WMBUS_MODE_R2,    /**< R2 mode */
  WMBUS_MODE_N_24,  /**< N mode, 2.4kbps (channel 2a and 2b) */
  WMBUS_MODE_N_48,  /**< N mode, 4.8kbps (channel 1a, 1b, 3a and 3b) */
  WMBUS_MODE_N_192, /**< N mode, 19.2kbps (channel 0) */
} WMBUS_Mode_t;

/**
 * Runs the required software based phy processes on the buffer, depending
 * on the mode. The buffer must be big enough to hold the processed data.
 * Currently, only T mode's Meter to Other requies software based crc and 3of6 encoding.
 * For that, the buffer should be at 436B long for the longest packet.
 * All other modes passes through this call without modifications
 * WMBUS_SetMode should be called before using this function.
 *
 * @param[in,out] buffer        The buffer that holds the data, and what will hold the processed data
 * @param[in]     length        Length of the input buffer
 * @param[in]     buffer_length Size of the buffer, to prevent writing outside of the buffer
 *
 * @return        The length of the processed packet, that can be passed to the hardware, or 0 on error
 */
uint16_t WMBUS_phy_software(uint8_t *buffer, uint8_t length,
                            uint16_t buffer_length);

/**
 * Get the current WMBUS accessibility settings
 * @return the current value
 */
WMBUS_accessibility_t get_wmbus_accessibility(void);

/**
 * Set the current WMBUS accessibility settings
 * @param new_wmbus_accessibility to be applied
 */
void set_wmbus_accessibility(WMBUS_accessibility_t new_wmbus_accessibility);

/**
 * Initializes the MBUS and its mode.
 */
void WMBUS_Init(void);

/**
 * Sets the MBUS mode to use. Currently only used to init and enable GPCRC
 * for T mode Tx, and it also changes the time returned by
 * \ref WMBUS_getMeterLimitedAccRxStart().
 *
 * @param[in] mode is the desired mode of the WMBUS
 */
void WMBUS_SetMode(WMBUS_Mode_t mode);

/**
 * Gets the selected WMBUS mode.
 *
 * @return      The selected mode of the WMBUS
 */
WMBUS_Mode_t WMBUS_GetSelectedMode(void);

/**
 * Returns the delay after TX when RX should start in
 * limited accessibility
 * WMBUS_SetMode should be called before using this function.
 * @param[in] slowMode
 *    True in case of slow mode. Only used in mode C and F
 * @return
 *    The delay in microseconds
 */
uint32_t WMBUS_getMeterLimitedAccRxStart(bool slowMode);

/**
 * Returns the delay after TX when RX should stop in
 * limited accessibility
 * WMBUS_SetMode should be called before using this function.
 * @param[in] slowMode
 *    True in case of slow mode. Only used in mode C and F
 * @return
 *    The delay in microseconds
 */
uint32_t WMBUS_getMeterLimitedAccRxStop(bool slowMode);

/**
 * Helper function for building the payload of a WMBUS frame. Adds an 8 bit
 * integer (binary) data block to a buffer.
 * @param[in] buffer
 *    The buffer to write into. Should point at the first free byte
 * @param[in] dibFunction
 *    Function field in the DIB (Data Information Block)
 * @param[in] vib
 *    Value information block containing the VIF (Value Information Field)
 * @param[in] value
 *    The value of the data block.
 * @return
 *    Returns the number of bytes written into buffer
 */
uint8_t WMBUSframe_addPayloadI8(uint8_t *buffer,
                                WMBUSframe_dibFunction_t dibFunction, uint8_t vib, int8_t value);

/**
 * Helper function for building the payload of a WMBUS frame. Adds a 16 bit
 * integer (binary) data block to a buffer.
 * @param[in] buffer
 *    The buffer to write into. Should point at the first free byte
 * @param[in] dibFunction
 *    Function field in the DIB (Data Information Block)
 * @param[in] vib
 *    Value information block containing the VIF (Value Information Field)
 * @param[in] value
 *    The value of the data block.
 * @return
 *    Returns the number of bytes written into buffer
 */
uint8_t WMBUSframe_addPayloadI16(uint8_t *buffer,
                                 WMBUSframe_dibFunction_t dibFunction, uint8_t vib, int16_t value);

/**
 * Helper function for building the payload of a WMBUS frame. Adds a 32 bit
 * integer (binary) data block to a buffer.
 * @param[in] buffer
 *    The buffer to write into. Should point at the first free byte
 * @param[in] dibFunction
 *    Function field in the DIB (Data Information Block)
 * @param[in] vib
 *    Value information block containing the VIF (Value Information Field)
 * @param[in] value
 *    The value of the data block.
 * @return
 *    Returns the number of bytes written into buffer
 */
uint8_t WMBUSframe_addPayloadI32(uint8_t *buffer,
                                 WMBUSframe_dibFunction_t dibFunction, uint8_t vib, int32_t value);

/**
 * Helper function for building the payload of a WMBUS frame.
 * Adds idle filler bytes
 * @param[in] buffer
 *    The buffer to write into. Should point at the first free byte
 * @param[in] fillerLength
 *    Number of bytes to fill.
 * @return
 *    Returns the number of bytes written into buffer
 */
uint16_t WMBUSframe_addIdleFiller(uint8_t *buffer, uint16_t fillerLength);

/**
 * Helper function for converting uppercase ASCII letters to
 * 2B manufacturer ID.
 * @param[in] manuf
 *    Pointer to a 3 element char array, containing the uppercase letters
 *    of the manufacturer assigned by http://www.dlms.com/flag/
 * @return
 *    Returns the manufacturer id (or M-field) for M-Bus addressing
 */
uint16_t WMBUSframe_Chars2MField(const char* manuf);

/**
 * Helper function for converting 2B manufacturer ID to
 * 3 uppercase ASCII letter
 * @param[in] mfield
 *    mfield value
 * @param[out] manuf
 *    pointer to a 3 element char array to write the 3 letters
 */
void WMBUSframe_MField2Chars(const uint16_t mfield, char* manuf);

/**
 * Initializes crypto engine.
 */
void WMBUSframe_crypto5Init();

/**
 * Sets key for mode5 encryption/decryption (AES-CBC)
 * @param[in] newKey
 *    16B array containing the new key
 */
void WMBUSframe_crypto5SetKey(const uint8_t *newKey);

/**
 * Returns key for mode5 encryption/decryption (AES-CBC)
 * @param[out] key
 *    Copy of the current key
 */
void WMBUSframe_crypto5GetKey(uint8_t *key);

/**
 * Encrypts blocks using mode5 (AES-CBC) encryption.
 * Also adds padding if needed, but the payload should include
 * the 2B AES check (0x2f2f) in the beginning of the payload
 *
 * @param[in] input
 *    Input data. Should be at least 16B*encryptedBlocks long.
 * @param[out] output
 *    Output data. Should be at least 16B*encryptedBlocks long.
 *    Can be the same as input.
 * @param[in,out] iv
 *    Initialization vector, 16B long
 * @param[in] length
 *    Length of the input data
 * @param[in] encryptedBlocks
 *    Number of blocks to encode. If 0, all data will be encoded
 *    based on length
 * @return
 *    Returns the number of blocks encoded. Output length is
 *    16 times this value.
 */
uint8_t WMBUSframe_crypto5encrypt(uint8_t *input, uint8_t *output, uint8_t *iv,
                                  uint16_t length, uint8_t encryptedBlocks);

/**
 * Decrypts blocks using mode5 (AES-CBC) encryption.
 *
 * @param[in] input
 *    Input data. Should be at least length long.
 * @param[out] output
 *    Output data. Should be at least length long.
 *    Can be the same as input.
 * @param[in,out] iv
 *    Initialization vector, 16B long
 * @param[in] length
 *    Length of the input data. Should be devidable by 16
 */
void WMBUSframe_crypto5decrypt(uint8_t *input, uint8_t *output, uint8_t *iv,
                               uint16_t length);

/**
 * @}
 */

#endif /*_SL_WMBUS_SUPPORT_H_*/
