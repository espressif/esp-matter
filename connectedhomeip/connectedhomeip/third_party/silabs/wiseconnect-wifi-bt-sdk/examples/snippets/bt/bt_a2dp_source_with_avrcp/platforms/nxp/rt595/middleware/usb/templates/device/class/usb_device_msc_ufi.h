/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_MSC_UFI_H
#define _USB_MSC_UFI_H 1
/*!
 * @addtogroup msc_ufi
 * @{
 */
/*Sense Key of REQUEST SENSE command, refer to UFI specification chapter 5*/
/*! @brief Indicates that there is no specific sense key information to be reported*/
#define USB_DEVICE_MSC_UFI_NO_SENSE 0x00U
/*! @brief Indicates that the last command completed successfully with some recovery action performed by the UFI
 * device*/
#define USB_DEVICE_MSC_UFI_RECOVERED_ERROR 0x01U
/*! @brief Indicates that the UFI device cannot be accessed*/
#define USB_DEVICE_MSC_UFI_NOT_READY 0x02U
/*! @brief Indicates that the command terminated with a non-recovered
error condition that was probably caused by a flaw in the medium or an error in the
recorded data*/
#define USB_DEVICE_MSC_UFI_MEDIUM_ERROR 0x03U
/*! @brief Indicates that the UFI device detected a non-recoverable hardware failure while performing the command or
 * during a self test*/
#define USB_DEVICE_MSC_UFI_HARDWARE_ERROR 0x04U
/*! @brief Indicates that there was an illegal parameter in the Command
Packet or in the additional parameters supplied as data for some commands*/
#define USB_DEVICE_MSC_UFI_ILLEGAL_REQUEST 0x05U
/*! @brief Indicates that the removable medium may have been changed or the UFI device has been reset*/
#define USB_DEVICE_MSC_UFI_UNIT_ATTENTION 0x06U
/*! @brief Indicates that a command that writes the medium was attempted on a block that is protected from this
 * operation*/
#define USB_DEVICE_MSC_UFI_DATA_PROTECT 0x07U
/*! @brief Indicates that a write-once device or a sequential-access device
encountered blank medium or format-defined end-of-data indication while reading or
a write-once device encountered a non-blank medium while writing*/
#define USB_DEVICE_MSC_UFI_BLANK_CHECK 0x08U
/*! @brief This sense key is available for reporting vendor-specific conditions*/
#define USB_DEVICE_MSC_UFI_VENDOR_SPECIFIC_ERROR 0x09U
/*! @brief Indicates that the UFI device has aborted the command
The host may be able to recover by trying the command again*/
#define USB_DEVICE_MSC_UFI_ABORTED_COMMAND 0x0BU
/*! @brief Indicates that a buffered peripheral device has reached the
end-of-partition and data may remain in the buffer that has not been written to the medium*/
#define USB_DEVICE_MSC_UFI_VOLUME_OVERFLOW 0x0DU
/*! @brief Indicates that the source data did not match the data read from the medium*/
#define USB_DEVICE_MSC_UFI_MISCOMPARE 0x0EU

/*! @brief Invalid command operation code*/
#define USB_DEVICE_MSC_UFI_INVALID_COMMAND_OPCODE 0x20U
/*! @brief Write fault*/
#define USB_DEVICE_MSC_UFI_WRITE_FAULT 0x03U
/*! @brief Not recovered read error*/
#define USB_DEVICE_MSC_UFI_UNRECOVERED_READ_ERROR 0x11U
/*! @brief Unknown error*/
#define USB_DEVICE_MSC_UFI_UNKNOWN_ERROR 0xFFU
/*! @brief Invalid field in command packet*/
#define USB_DEVICE_MSC_UFI_INVALID_FIELD_IN_COMMAND_PKT 0x24U
/*! @brief Invalid logical block address out of range*/
#define USB_DEVICE_MSC_UFI_LBA_OUT_OF_RANGE 0x21U

/*! @brief Valid error code, 70h indicate current errors*/
#define USB_DEVICE_MSC_UFI_REQ_SENSE_VALID_ERROR_CODE 0x70U
/*! @brief The UFI device sets the value of this field to ten, to indicate that ten more bytes of sense data follow this
 * field*/
#define USB_DEVICE_MSC_UFI_REQ_SENSE_ADDITIONAL_SENSE_LEN 0x0AU

/*! @brief Prevent media removal flag*/
#define USB_DEVICE_MSC_UFI_PREVENT_ALLOW_REMOVAL_MASK 0x01U
/*! @brief LoEj Start flag */
#define USB_DEVICE_MSC_UFI_LOAD_EJECT_START_MASK 0x03U

/*! @brief Formatted Media - Current media capacity */
#define USB_DEVICE_MSC_UFI_FORMATTED_MEDIA 0x02U
/*! @brief Unformatted Media - Maximum formatting capacity for this cartridge*/
#define USB_DEVICE_MSC_UFI_UNFORMATTED_MEDIA 0x01U
/*! @brief No Cartridge in Drive - Maximum formating capacity for any cartridge*/
#define USB_DEVICE_MSC_UFI_NO_CARTRIDGE_IN_DRIVE 0x03U

/*! @brief INQUIRY Data length of INQUIRY Command*/
#define USB_DEVICE_MSC_UFI_INQUIRY_ALLOCATION_LENGTH 0x24U
/*! @brief Request Sense Data length of REQUEST SENSE Command*/
#define USB_DEVICE_MSC_UFI_REQ_SENSE_DATA_LENGTH 18U
/*! @brief READ CAPACITY Data length of READ CAPACITY Command*/
#define USB_DEVICE_MSC_UFI_READ_CAPACITY_DATA_LENGTH 0x08U
/*! @brief READ CAPACITY Data length of READ CAPACITY Command*/
#define USB_DEVICE_MSC_UFI_READ_CAPACITY16_DATA_LENGTH 0x0CU

/*! @brief Reserved*/
#define USB_DEVICE_MSC_UFI_PERIPHERAL_QUALIFIER 0U
/*! @brief Peripheral Device Type shift*/
#define USB_DEVICE_MSC_UFI_PERIPHERAL_QUALIFIER_SHIFT 5U
/*! @brief Version value*/
#define USB_DEVICE_MSC_UFI_VERSIONS 4U
/*! @brief Peripheral Device Type value of INQUIRY Data*/
#define USB_DEVICE_MSC_UFI_PERIPHERAL_DEVICE_TYPE 0x00U
/*! @brief Removable Media Bit value, this shall be set to one to indicate removable media*/
#define USB_DEVICE_MSC_UFI_REMOVABLE_MEDIUM_BIT 1U
/*! @brief  Removable Media Bit shift*/
#define USB_DEVICE_MSC_UFI_REMOVABLE_MEDIUM_BIT_SHIFT 7U
/*! @brief  Additional Length*/
#define USB_DEVICE_MSC_UFI_ADDITIONAL_LENGTH 0x20U

/*! @brief  UFI inquiry command structure*/
typedef struct _usb_device_inquiry_command_struct
{
    uint8_t operationCode;     /*!< Operation Code*/
    uint8_t logicalUnitNumber; /*!< Specifies the logical unit (0~7) for which Inquiry data should be returned*/
    uint8_t pageCode;          /*!< Page Code*/
    uint8_t reserved;          /*!< Reserved*/
    uint8_t allocationLength;  /*!< Specifies the maximum number of bytes of inquiry data to be returned*/
    uint8_t reserved1[7];      /*!< Reserved*/
} usb_device_inquiry_command_struct_t;

/*! @brief  UFI request sense command structure*/
typedef struct _usb_device_request_sense_command_struct
{
    uint8_t operationCode;     /*!< Operation Code*/
    uint8_t logicalUnitNumber; /*!< Logical Unit Number*/
    uint8_t reserved[2];       /*!< reserved*/
    uint8_t allocationLength;  /*!< Allocation Length*/
    uint8_t reserved1[7];      /*!< reserved*/
} usb_device_request_sense_command_struct_t;

/*! @brief  UFI read format capacities command structure*/
typedef struct _usb_device_read_format_capacities_command_struct
{
    uint8_t operationCode;     /*!< Operation Code*/
    uint8_t logicalUnitNumber; /*!< Logical Unit Number*/
    uint8_t reserved[5];       /*!< reserved*/
    uint16_t allocationLength; /*!< Allocation Length*/
    uint8_t reserved1[3];      /*!< reserved*/
} usb_device_read_format_capacities_command_struct_t;

/*! @brief  UFI read capacities command structure*/
typedef struct _usb_device_read_capacities_command_struct
{
    uint8_t operationCode;     /*!< Operation Code*/
    uint8_t logicalUnitNumber; /*!< Logical Unit Number*/
    uint32_t lba;              /*!< Logical Block Address*/
    uint8_t reserved[2];       /*!< Reserved*/
    uint8_t pmi;               /*!< This bit should be set to zero for UFI*/
    uint8_t reserved1[3];      /*!< Reserved*/
} usb_device_read_capacities_command_struct_t;

/*! @brief  UFI read write 10 structure*/
typedef struct _usb_device_read_write_10_command_struct
{
    uint8_t operationCode;     /*!< Operation Code*/
    uint8_t lunDpoFuaReladr;   /*!< Logical Unit Number DPO FUA RelAdr*/
    uint32_t lba;              /*!< Logical Block Address*/
    uint8_t reserved;          /*!< Reserved*/
    uint8_t transferLengthMsb; /*!< Transfer Length (MSB)*/
    uint8_t transferLengthLsb; /*!< Transfer Length (LSB)*/
    uint8_t reserved1[3];      /*!< Reserved*/
} usb_device_read_write_10_command_struct_t;

/*! @brief  UFI inquiry data format structure*/
typedef struct _usb_device_inquiry_data_fromat_struct
{
    uint8_t peripheralDeviceType; /*!< Peripheral Device Type*/
    uint8_t rmb;                  /*!< Removable Media Bit*/
    uint8_t versions;             /*!< ISO Version, ECMA Version, ANSI Version*/
    uint8_t responseDataFormat;   /*!< Response Data Format*/
    uint8_t additionalLength;     /*!< The Additional Length field shall specify the length in bytes of the parameters*/
    uint8_t reserved[3];          /*!< reserved*/
    uint8_t vendorInformatin[8];  /*!< Vendor Identification*/
    uint8_t productId[16];        /*!< Product Identification*/
    uint8_t productVersionLevel[4]; /*!< Product Revision Level*/
} usb_device_inquiry_data_fromat_struct_t;

/*! @brief  UFI request sense data structure*/
typedef struct _usb_device_request_sense_data_struct
{
    uint8_t validErrorCode;          /*!< Error Code*/
    uint8_t reserved;                /*!< reserved*/
    uint8_t senseKey;                /*!< Sense Key*/
    uint8_t information[4];          /*!< Information*/
    uint8_t additionalSenseLength;   /*!< Additional Sense Length*/
    uint8_t reserved1[4];            /*!< reserved*/
    uint8_t additionalSenseCode;     /*!< Additional Sense Code*/
    uint8_t additionalSenseQualifer; /*!< Additional Sense Code Qualifier*/
    uint8_t reserved2[4];            /*!< reserved*/
} usb_device_request_sense_data_struct_t;

/*! @brief  UFI read capacity data structure*/
typedef struct _usb_device_read_capacity_struct
{
    uint32_t lastLogicalBlockAddress; /*!< Last Logical Block Address*/
    uint32_t blockSize;               /*!< Block Length In Bytes*/
} usb_device_read_capacity_struct_t;

/*! @brief  UFI read capacity data structure*/
typedef struct _usb_device_read_capacity16_data_struct
{
    uint32_t lastLogicalBlockAddress0; /*!<  Last Logical Block Address*/
    uint32_t lastLogicalBlockAddress1; /*!<  Last Logical Block Address*/
    uint32_t blockSize;                /*!< Block Length In Bytes*/
} usb_device_read_capacity16_data_struct_t;

/*! @brief  UFI capacity list header structure*/
typedef struct _usb_device_capacity_list_header_struct
{
    uint8_t reserverd[3];       /*!< reserved*/
    uint8_t capacityListLength; /*!< Capacity List Length*/
} usb_device_capacity_list_header_struct_t;

/*! @brief  UFI current maximum capacity structure*/
typedef struct _usb_device_current_max_capacity_descriptor_struct
{
    uint32_t blockNumber;               /*!< Number of Blocks*/
    uint32_t descriptorCodeBlockLength; /*!< Byte 4 Descriptor Code , byte 5-7 Block Length*/
} usb_device_current_max_capacity_descriptor_struct_t;

/*! @brief  UFI formatting capacity structure*/
typedef struct _usb_device_formattable_capacity_descriptor_struct
{
    uint32_t blockNumber; /*!< Number of Blocks*/
    uint32_t blockLength; /*!< Block Length*/
} usb_device_formattable_capacity_descriptor_struct_t;

/*! @brief  UFI mode parameters header structure*/
typedef struct _usb_device_mode_parameters_header_struct
{
    uint16_t modeDataLength; /*!< Mode Data Length*/
    uint8_t mediumTypeCode;  /*!< The Medium Type Code field specifies the inserted medium type*/
    uint8_t wpDpfua;         /*!< WP and DPOFUA bit*/
    uint8_t reserved[4];     /*!< Reserved*/
} usb_device_mode_parameters_header_struct_t;

/*! @brief  UFI Capacity List structure*/
typedef struct _usb_device_format_capacity_response_data_struct
{
    uint8_t capacityListHead[sizeof(usb_device_capacity_list_header_struct_t)]; /*!<Capacity List Header*/
    uint8_t currentMaxCapacityDesccriptor[sizeof(
        usb_device_current_max_capacity_descriptor_struct_t)]; /*!<Current/Maximum Capacity Header*/
    uint8_t formattableCapacityDesccriptor[sizeof(usb_device_formattable_capacity_descriptor_struct_t) *
                                           3]; /*!<Formatting Capacity Descriptor*/
} usb_device_format_capacity_response_data_struct_t;
/*! @}*/

#endif
