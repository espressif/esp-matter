/***************************************************************************//**
 * @file
 * @brief USB Device Scsi Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device MSC class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    USBD_SCSI_MODULE
#include  <em_core.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_str.h>

#include  <usb/source/device/class/usbd_scsi_priv.h>
#include  <usb/source/device/class/usbd_scsi_storage_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, MSD, SCSI)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 *                                                   DEFINES
 *
 * Note(s) : (1) The T10 VENDOR IDENTIFICATION field contains 8 bytes of left-aligned ASCII data
 *               identifying the vendor of the product. The T10 vendor identification shall be one
 *               assigned by INCITS.
 *               The PRODUCT IDENTIFICATION field contains 16 bytes of left-aligned ASCII data
 *               defined by the vendor.
 *               See 'SCSI Primary Commands - 3 (SPC-3)', section 6.4.2 for more details about
 *               Standard INQUIRY data format.
 *******************************************************************************************************/

//                                                                 ---- STANDARD INQUIRY DATA FORMAT (see Note #1) ----
#define  USBD_SCSI_DEV_MAX_VEND_ID_LEN                     8u
#define  USBD_SCSI_DEV_MAX_PROD_ID_LEN                    16u

#define  USBD_SCSI_LOCK_DLY_mS                             4u

#define  USBD_SCSI_BLOCK_DESC_LEN                          0u

#define  USBD_SCSI_INQUIRY_DATA_LEN                       36u
#define  USBD_SCSI_MODE_SENSE_DATA_LEN                   256u
#define  USBD_SCSI_RD_CAPACITY_10_PARAM_DATA_LEN           8u
#define  USBD_SCSI_RD_CAPACITY_16_PARAM_DATA_LEN          16u
#define  USBD_SCSI_REQ_SENSE_DATA_LEN                     18u
//                                                                 ------------------ DATA XFER DIR -------------------
#define  USBD_SCSI_CBW_HOST_TO_DEVICE                   0x00
#define  USBD_SCSI_CBW_DEVICE_TO_HOST                   0x80
//                                                                 ------------- DIRECT ACCESS MEDIUM TYPE ------------
#define  USBD_DISK_MEMORY_MEDIA                         0x00
//                                                                 ------------------ INQUIRY DATA --------------------
#define  USBD_SCSI_STD_INQUIRY_DATA                     0x00
#define  USBD_SCSI_INQUIRY_VERS_OBSOLETE                0x02
#define  USBD_SCSI_INQUIRY_VERS_SPC_3                   0x05
#define  USBD_SCSI_INQUIRY_RESP_DATA_FMT_DEFAULT        0x02
//                                                                 ------------------ REQ SENSE DATA ------------------
#define  USBD_SCSI_REQ_SENSE_RESP_CODE_CUR_ERR          0x70

/********************************************************************************************************
 *                                               SCSI COMMANDS
 *
 * Note(s) : (1) The SCSI Operation Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.3 Operation Codes.
 *
 *           (2) The SCSI Status Codes can be found in SCSI Architecture Model - 4 (SAM-3),
 *               section 5.3.1 Status Codes.
 *
 *           (3) The SCSI Sense Keys can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section 4.5.6 Status Key and Sense Code Definitions, Table 27.
 *
 *           (4) The SCSI Additional Sense Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.2 Additional Sense Codes.
 *
 *           (5) The SCSI Mode Page Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.6 Mode Page Codes.
 *******************************************************************************************************/

//                                                                 ------------ SCSI OPCODES (See Notes #1) ------------
#define  USBD_SCSI_CMD_TEST_UNIT_READY                   0x00
#define  USBD_SCSI_CMD_REWIND                            0x01
#define  USBD_SCSI_CMD_REZERO_UNIT                       0x01
#define  USBD_SCSI_CMD_REQUEST_SENSE                     0x03
#define  USBD_SCSI_CMD_FORMAT_UNIT                       0x04
#define  USBD_SCSI_CMD_FORMAT_MEDIUM                     0x04
#define  USBD_SCSI_CMD_FORMAT                            0x04
#define  USBD_SCSI_CMD_READ_BLOCK_LIMITS                 0x05
#define  USBD_SCSI_CMD_REASSIGN_BLOCKS                   0x07
#define  USBD_SCSI_CMD_INITIALIZE_ELEMENT_STATUS         0x07
#define  USBD_SCSI_CMD_READ_06                           0x08
#define  USBD_SCSI_CMD_RECEIVE                           0x08
#define  USBD_SCSI_CMD_GET_MESSAGE_06                    0x08
#define  USBD_SCSI_CMD_WRITE_06                          0x0A
#define  USBD_SCSI_CMD_SEND_06                           0x0A
#define  USBD_SCSI_CMD_SEND_MESSAGE_06                   0x0A
#define  USBD_SCSI_CMD_PRINT                             0x0A
#define  USBD_SCSI_CMD_SEEK_06                           0x0B
#define  USBD_SCSI_CMD_SET_CAPACITY                      0x0B
#define  USBD_SCSI_CMD_SLEW_AND_PRINT                    0x0B
#define  USBD_SCSI_CMD_READ_REVERSE_06                   0x0F

#define  USBD_SCSI_CMD_WRITE_FILEMARKS_06                0x10
#define  USBD_SCSI_CMD_SYNCHRONIZE_BUFFER                0x10
#define  USBD_SCSI_CMD_SPACE_06                          0x11
#define  USBD_SCSI_CMD_INQUIRY                           0x12
#define  USBD_SCSI_CMD_VERIFY_06                         0x13
#define  USBD_SCSI_CMD_RECOVER_BUFFERED_DATA             0x14
#define  USBD_SCSI_CMD_MODE_SELECT_06                    0x15
#define  USBD_SCSI_CMD_RESERVE_06                        0x16
#define  USBD_SCSI_CMD_RESERVE_ELEMENT_06                0x16
#define  USBD_SCSI_CMD_RELEASE_06                        0x17
#define  USBD_SCSI_CMD_RELEASE_ELEMENT_06                0x17
#define  USBD_SCSI_CMD_COPY                              0x18
#define  USBD_SCSI_CMD_ERASE_06                          0x19
#define  USBD_SCSI_CMD_MODE_SENSE_06                     0x1A
#define  USBD_SCSI_CMD_START_STOP_UNIT                   0x1B
#define  USBD_SCSI_CMD_LOAD_UNLOAD                       0x1B
#define  USBD_SCSI_CMD_SCAN_06                           0x1B
#define  USBD_SCSI_CMD_STOP_PRINT                        0x1B
#define  USBD_SCSI_CMD_OPEN_CLOSE_IMPORT_EXPORT_ELEMENT  0x1B
#define  USBD_SCSI_CMD_RECEIVE_DIAGNOSTIC_RESULTS        0x1C
#define  USBD_SCSI_CMD_SEND_DIAGNOSTIC                   0x1D
#define  USBD_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL      0x1E

#define  USBD_SCSI_CMD_READ_FORMAT_CAPACITIES            0x23
#define  USBD_SCSI_CMD_SET_WINDOW                        0x24
#define  USBD_SCSI_CMD_READ_CAPACITY_10                  0x25
#define  USBD_SCSI_CMD_READ_CAPACITY                     0x25
#define  USBD_SCSI_CMD_READ_CARD_CAPACITY                0x25
#define  USBD_SCSI_CMD_GET_WINDOW                        0x25
#define  USBD_SCSI_CMD_READ_10                           0x28
#define  USBD_SCSI_CMD_GET_MESSAGE_10                    0x28
#define  USBD_SCSI_CMD_READ_GENERATION                   0x29
#define  USBD_SCSI_CMD_WRITE_10                          0x2A
#define  USBD_SCSI_CMD_SEND_10                           0x2A
#define  USBD_SCSI_CMD_SEND_MESSAGE_10                   0x2A
#define  USBD_SCSI_CMD_SEEK_10                           0x2B
#define  USBD_SCSI_CMD_LOCATE_10                         0x2B
#define  USBD_SCSI_CMD_POSITION_TO_ELEMENT               0x2B
#define  USBD_SCSI_CMD_ERASE_10                          0x2C
#define  USBD_SCSI_CMD_READ_UPDATED_BLOCK                0x2D
#define  USBD_SCSI_CMD_WRITE_AND_VERIFY_10               0x2E
#define  USBD_SCSI_CMD_VERIFY_10                         0x2F

#define  USBD_SCSI_CMD_SEARCH_DATA_HIGH_10               0x30
#define  USBD_SCSI_CMD_SEARCH_DATA_EQUAL_10              0x31
#define  USBD_SCSI_CMD_OBJECT_POSITION                   0x31
#define  USBD_SCSI_CMD_SEARCH_DATA_LOW_10                0x32
#define  USBD_SCSI_CMD_SET_LIMITS_10                     0x33
#define  USBD_SCSI_CMD_PRE_FETCH_10                      0x34
#define  USBD_SCSI_CMD_READ_POSITION                     0x34
#define  USBD_SCSI_CMD_GET_DATA_BUFFER_STATUS            0x34
#define  USBD_SCSI_CMD_SYNCHRONIZE_CACHE_10              0x35
#define  USBD_SCSI_CMD_LOCK_UNLOCK_CACHE_10              0x36
#define  USBD_SCSI_CMD_READ_DEFECT_DATA_10               0x37
#define  USBD_SCSI_CMD_INIT_ELEMENT_STATUS_WITH_RANGE    0x37
#define  USBD_SCSI_CMD_MEDIUM_SCAN                       0x38
#define  USBD_SCSI_CMD_COMPARE                           0x39
#define  USBD_SCSI_CMD_COPY_AND_VERIFY                   0x3A
#define  USBD_SCSI_CMD_WRITE_BUFFER                      0x3B
#define  USBD_SCSI_CMD_READ_BUFFER                       0x3C
#define  USBD_SCSI_CMD_UPDATE_BLOCK                      0x3D
#define  USBD_SCSI_CMD_READ_LONG_10                      0x3E
#define  USBD_SCSI_CMD_WRITE_LONG_10                     0x3F

#define  USBD_SCSI_CMD_CHANGE_DEFINITION                 0x40
#define  USBD_SCSI_CMD_WRITE_SAME_10                     0x41
#define  USBD_SCSI_CMD_READ_SUBCHANNEL                   0x42
#define  USBD_SCSI_CMD_READ_TOC_PMA_ATIP                 0x43
#define  USBD_SCSI_CMD_REPORT_DENSITY_SUPPORT            0x44
#define  USBD_SCSI_CMD_READ_HEADER                       0x44
#define  USBD_SCSI_CMD_PLAY_AUDIO_10                     0x45
#define  USBD_SCSI_CMD_GET_CONFIGURATION                 0x46
#define  USBD_SCSI_CMD_PLAY_AUDIO_MSF                    0x47
#define  USBD_SCSI_CMD_GET_EVENT_STATUS_NOTIFICATION     0x4A
#define  USBD_SCSI_CMD_PAUSE_RESUME                      0x4B
#define  USBD_SCSI_CMD_LOG_SELECT                        0x4C
#define  USBD_SCSI_CMD_LOG_SENSE                         0x4D
#define  USBD_SCSI_CMD_STOP_PLAY_SCAN                    0x4E

#define  USBD_SCSI_CMD_XDWRITE_10                        0x50
#define  USBD_SCSI_CMD_XPWRITE_10                        0x51
#define  USBD_SCSI_CMD_READ_DISC_INFORMATION             0x51
#define  USBD_SCSI_CMD_XDREAD_10                         0x52
#define  USBD_SCSI_CMD_READ_TRACK_INFORMATION            0x52
#define  USBD_SCSI_CMD_RESERVE_TRACK                     0x53
#define  USBD_SCSI_CMD_SEND_OPC_INFORMATION              0x54
#define  USBD_SCSI_CMD_MODE_SELECT_10                    0x55
#define  USBD_SCSI_CMD_RESERVE_10                        0x56
#define  USBD_SCSI_CMD_RESERVE_ELEMENT_10                0x56
#define  USBD_SCSI_CMD_RELEASE_10                        0x57
#define  USBD_SCSI_CMD_RELEASE_ELEMENT_10                0x57
#define  USBD_SCSI_CMD_REPAIR_TRACK                      0x58
#define  USBD_SCSI_CMD_MODE_SENSE_10                     0x5A
#define  USBD_SCSI_CMD_CLOSE_TRACK_SESSION               0x5B
#define  USBD_SCSI_CMD_READ_BUFFER_CAPACITY              0x5C
#define  USBD_SCSI_CMD_SEND_CUE_SHEET                    0x5D
#define  USBD_SCSI_CMD_PERSISTENT_RESERVE_IN             0x5E
#define  USBD_SCSI_CMD_PERSISTENT_RESERVE_OUT            0x5F

#define  USBD_SCSI_CMD_EXTENDED_CDB                      0x7E
#define  USBD_SCSI_CMD_VARIABLE_LENGTH_CDB               0x7F

#define  USBD_SCSI_CMD_XDWRITE_EXTENDED_16               0x80
#define  USBD_SCSI_CMD_WRITE_FILEMARKS_16                0x80
#define  USBD_SCSI_CMD_REBUILD_16                        0x81
#define  USBD_SCSI_CMD_READ_REVERSE_16                   0x81
#define  USBD_SCSI_CMD_REGENERATE_16                     0x82
#define  USBD_SCSI_CMD_EXTENDED_COPY                     0x83
#define  USBD_SCSI_CMD_RECEIVE_COPY_RESULTS              0x84
#define  USBD_SCSI_CMD_ATA_COMMAND_PASS_THROUGH_16       0x85
#define  USBD_SCSI_CMD_ACCESS_CONTROL_IN                 0x86
#define  USBD_SCSI_CMD_ACCESS_CONTROL_OUT                0x87
#define  USBD_SCSI_CMD_READ_16                           0x88
#define  USBD_SCSI_CMD_WRITE_16                          0x8A
#define  USBD_SCSI_CMD_ORWRITE                           0x8B
#define  USBD_SCSI_CMD_READ_ATTRIBUTE                    0x8C
#define  USBD_SCSI_CMD_WRITE_ATTRIBUTE                   0x8D
#define  USBD_SCSI_CMD_WRITE_AND_VERIFY_16               0x8E
#define  USBD_SCSI_CMD_VERIFY_16                         0x8F

#define  USBD_SCSI_CMD_PREFETCH_16                       0x90
#define  USBD_SCSI_CMD_SYNCHRONIZE_CACHE_16              0x91
#define  USBD_SCSI_CMD_SPACE_16                          0x91
#define  USBD_SCSI_CMD_LOCK_UNLOCK_CACHE_16              0x92
#define  USBD_SCSI_CMD_LOCATE_16                         0x92
#define  USBD_SCSI_CMD_WRITE_SAME_16                     0x93
#define  USBD_SCSI_CMD_ERASE_16                          0x93
#define  USBD_SCSI_CMD_SERVICE_ACTION_IN_16              0x9E
#define  USBD_SCSI_CMD_SERVICE_ACTION_OUT_16             0x9F

#define  USBD_SCSI_CMD_REPORT_LUNS                       0xA0
#define  USBD_SCSI_CMD_BLANK                             0xA1
#define  USBD_SCSI_CMD_ATA_COMMAND_PASS_THROUGH_12       0xA1
#define  USBD_SCSI_CMD_SECURITY_PROTOCOL_IN              0xA2
#define  USBD_SCSI_CMD_MAINTENANCE_IN                    0xA3
#define  USBD_SCSI_CMD_SEND_KEY                          0xA3
#define  USBD_SCSI_CMD_MAINTENANCE_OUT                   0xA4
#define  USBD_SCSI_CMD_REPORT_KEY                        0xA4
#define  USBD_SCSI_CMD_MOVE_MEDIUM                       0xA5
#define  USBD_SCSI_CMD_PLAY_AUDIO_12                     0xA5
#define  USBD_SCSI_CMD_EXCHANGE_MEDIUM                   0xA6
#define  USBD_SCSI_CMD_LOAD_UNLOAD_CDVD                  0xA6
#define  USBD_SCSI_CMD_MOVE_MEDIUM_ATTACHED              0xA7
#define  USBD_SCSI_CMD_SET_READ_AHEAD                    0xA7
#define  USBD_SCSI_CMD_READ_12                           0xA8
#define  USBD_SCSI_CMD_GET_MESSAGE_12                    0xA8
#define  USBD_SCSI_CMD_SERVICE_ACTION_OUT_12             0xA9
#define  USBD_SCSI_CMD_WRITE_12                          0xAA
#define  USBD_SCSI_CMD_SEND_MESSAGE_12                   0xAA
#define  USBD_SCSI_CMD_SERVICE_ACTION_IN_12              0xAB
#define  USBD_SCSI_CMD_ERASE_12                          0xAC
#define  USBD_SCSI_CMD_GET_PERFORMANCE                   0xAC
#define  USBD_SCSI_CMD_READ_DVD_STRUCTURE                0xAD
#define  USBD_SCSI_CMD_WRITE_AND_VERIFY_12               0xAE
#define  USBD_SCSI_CMD_VERIFY_12                         0xAF

#define  USBD_SCSI_CMD_SEARCH_DATA_HIGH_12               0xB0
#define  USBD_SCSI_CMD_SEARCH_DATA_EQUAL_12              0xB1
#define  USBD_SCSI_CMD_SEARCH_DATA_LOW_12                0xB2
#define  USBD_SCSI_CMD_SET_LIMITS_12                     0xB3
#define  USBD_SCSI_CMD_READ_ELEMENT_STATUS_ATTACHED      0xB4
#define  USBD_SCSI_CMD_SECURITY_PROTOCOL_OUT             0xB5
#define  USBD_SCSI_CMD_REQUEST_VOLUME_ELEMENT_ADDRESS    0xB5
#define  USBD_SCSI_CMD_SEND_VOLUME_TAG                   0xB6
#define  USBD_SCSI_CMD_SET_STREAMING                     0xB6
#define  USBD_SCSI_CMD_READ_DEFECT_DATA_12               0xB7
#define  USBD_SCSI_CMD_READ_ELEMENT_STATUS               0xB8
#define  USBD_SCSI_CMD_READ_CD_MSF                       0xB9
#define  USBD_SCSI_CMD_REDUNDANCY_GROUP_IN               0xBA
#define  USBD_SCSI_CMD_SCAN                              0xBA
#define  USBD_SCSI_CMD_REDUNDANCY_GROUP_OUT              0xBB
#define  USBD_SCSI_CMD_SET_CD_SPEED                      0xBB
#define  USBD_SCSI_CMD_SPARE_IN                          0xBC
#define  USBD_SCSI_CMD_SPARE_OUT                         0xBD
#define  USBD_SCSI_CMD_MECHANISM_STATUS                  0xBD
#define  USBD_SCSI_CMD_VOLUME_SET_IN                     0xBE
#define  USBD_SCSI_CMD_READ_CD                           0xBE
#define  USBD_SCSI_CMD_VOLUME_SET_OUT                    0xBF
#define  USBD_SCSI_CMD_SEND_DVD_STRUCTURE                0xBF

//                                                                 -------- SCSI STATUS CODES (See Notes #2) ----------
#define  USBD_SCSI_STATUS_GOOD                           0x00
#define  USBD_SCSI_STATUS_CHECK_CONDITION                0x02
#define  USBD_SCSI_STATUS_CONDITION_MET                  0x04
#define  USBD_SCSI_STATUS_BUSY                           0x08
#define  USBD_SCSI_STATUS_RESERVATION_CONFLICT           0x18
#define  USBD_SCSI_STATUS_TASK_SET_FULL                  0x28
#define  USBD_SCSI_STATUS_ACA_ACTIVE                     0x30
#define  USBD_SCSI_STATUS_TASK_ABORTED                   0x40

//                                                                 -------- SCSI SENSE KEYS (See Notes #3) ------------
#define  USBD_SCSI_SENSE_KEY_NO_SENSE                    0x00
#define  USBD_SCSI_SENSE_KEY_RECOVERED_ERROR             0x01
#define  USBD_SCSI_SENSE_KEY_NOT_RDY                     0x02
#define  USBD_SCSI_SENSE_KEY_MEDIUM_ERROR                0x03
#define  USBD_SCSI_SENSE_KEY_HARDWARE_ERROR              0x04
#define  USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST             0x05
#define  USBD_SCSI_SENSE_KEY_UNIT_ATTENTION              0x06
#define  USBD_SCSI_SENSE_KEY_DATA_PROTECT                0x07
#define  USBD_SCSI_SENSE_KEY_BLANK_CHECK                 0x08
#define  USBD_SCSI_SENSE_KEY_VENDOR_SPECIFIC             0x09
#define  USBD_SCSI_SENSE_KEY_COPY_ABORTED                0x0A
#define  USBD_SCSI_SENSE_KEY_ABORTED_COMMAND             0x0B
#define  USBD_SCSI_SENSE_KEY_VOLUME_OVERFLOW             0x0D
#define  USBD_SCSI_SENSE_KEY_MISCOMPARE                  0x0E

//                                                                 ---- SCSI ADDITIONAL SENSE CODES (See Notes #4) ----
#define  USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO          0x00
#define  USBD_SCSI_ASC_NO_INDEX_SECTOR_SIGNAL            0x01
#define  USBD_SCSI_ASC_NO_SEEK_COMPLETE                  0x02
#define  USBD_SCSI_ASC_PERIPHERAL_DEV_WR_FAULT           0x03
#define  USBD_SCSI_ASC_LOG_UNIT_NOT_RDY                  0x04
#define  USBD_SCSI_ASC_LOG_UNIT_NOT_RESPOND_TO_SELECTION 0x05
#define  USBD_SCSI_ASC_NO_REFERENCE_POSITION_FOUND       0x06
#define  USBD_SCSI_ASC_MULTIPLE_PERIPHERAL_DEVS_SELECTED 0x07
#define  USBD_SCSI_ASC_LOG_UNIT_COMMUNICATION_FAIL       0x08
#define  USBD_SCSI_ASC_TRACK_FOLLOWING_ERR               0x09
#define  USBD_SCSI_ASC_ERR_LOG_OVERFLOW                  0x0A
#define  USBD_SCSI_ASC_WARNING                           0x0B
#define  USBD_SCSI_ASC_WR_ERR                            0x0C
#define  USBD_SCSI_ASC_ERR_DETECTED_BY_THIRD_PARTY       0x0D
#define  USBD_SCSI_ASC_INVALID_INFO_UNIT                 0x0E

#define  USBD_SCSI_ASC_ID_CRC_OR_ECC_ERR                 0x10
#define  USBD_SCSI_ASC_UNRECOVERED_RD_ERR                0x11
#define  USBD_SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_ID        0x12
#define  USBD_SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_DATA      0x13
#define  USBD_SCSI_ASC_RECORDED_ENTITY_NOT_FOUND         0x14
#define  USBD_SCSI_ASC_RANDOM_POSITIONING_ERR            0x15
#define  USBD_SCSI_ASC_DATA_SYNCHRONIZATION_MARK_ERR     0x16
#define  USBD_SCSI_ASC_RECOVERED_DATA_NO_ERR_CORRECT     0x17
#define  USBD_SCSI_ASC_RECOVERED_DATA_ERR_CORRECT        0x18
#define  USBD_SCSI_ASC_DEFECT_LIST_ERR                   0x19
#define  USBD_SCSI_ASC_PARAMETER_LIST_LENGTH_ERR         0x1A
#define  USBD_SCSI_ASC_SYNCHRONOUS_DATA_TRANSFER_ERR     0x1B
#define  USBD_SCSI_ASC_DEFECT_LIST_NOT_FOUND             0x1C
#define  USBD_SCSI_ASC_MISCOMPARE_DURING_VERIFY_OP       0x1D
#define  USBD_SCSI_ASC_RECOVERED_ID_WITH_ECC_CORRECTION  0x1E
#define  USBD_SCSI_ASC_PARTIAL_DEFECT_LIST_TRANSFER      0x1F

#define  USBD_SCSI_ASC_INVALID_CMD_OP_CODE               0x20
#define  USBD_SCSI_ASC_LOG_BLOCK_ADDR_OUT_OF_RANGE       0x21
#define  USBD_SCSI_ASC_ILLEGAL_FUNCTION                  0x22
#define  USBD_SCSI_ASC_INVALID_FIELD_IN_CDB              0x24
#define  USBD_SCSI_ASC_LOG_UNIT_NOT_SUPPORTED            0x25
#define  USBD_SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST   0x26
#define  USBD_SCSI_ASC_WR_PROTECTED                      0x27
#define  USBD_SCSI_ASC_NOT_RDY_TO_RDY_CHANGE             0x28
#define  USBD_SCSI_ASC_POWER_ON_OR_BUS_DEV_RESET         0x29
#define  USBD_SCSI_ASC_PARAMETERS_CHANGED                0x2A
#define  USBD_SCSI_ASC_CANNOT_COPY_CANNOT_DISCONNECT     0x2B
#define  USBD_SCSI_ASC_CMD_SEQUENCE_ERR                  0x2C
#define  USBD_SCSI_ASC_OVERWR_ERR_ON_UPDATE_IN_PLACE     0x2D
#define  USBD_SCSI_ASC_INSUFFICIENT_TIME_FOR_OP          0x2E
#define  USBD_SCSI_ASC_CMDS_CLEARED_BY_ANOTHER_INIT      0x2F

#define  USBD_SCSI_ASC_INCOMPATIBLE_MEDIUM_INSTALLED     0x30
#define  USBD_SCSI_ASC_MEDIUM_FORMAT_CORRUPTED           0x31
#define  USBD_SCSI_ASC_NO_DEFECT_SPARE_LOCATION_AVAIL    0x32
#define  USBD_SCSI_ASC_TAPE_LENGTH_ERR                   0x33
#define  USBD_SCSI_ASC_ENCLOSURE_FAIL                    0x34
#define  USBD_SCSI_ASC_ENCLOSURE_SERVICES_FAIL           0x35
#define  USBD_SCSI_ASC_RIBBON_INK_OR_TONER_FAIL          0x36
#define  USBD_SCSI_ASC_ROUNDED_PARAMETER                 0x37
#define  USBD_SCSI_ASC_EVENT_STATUS_NOTIFICATION         0x38
#define  USBD_SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED   0x39
#define  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT                0x3A
#define  USBD_SCSI_ASC_SEQUENTIAL_POSITIONING_ERR        0x3B
#define  USBD_SCSI_ASC_INVALID_BITS_IN_IDENTIFY_MSG      0x3D
#define  USBD_SCSI_ASC_LOG_UNIT_HAS_NOT_SELF_CFG_YET     0x3E
#define  USBD_SCSI_ASC_TARGET_OP_CONDITIONS_HAVE_CHANGED 0x3F

#define  USBD_SCSI_ASC_RAM_FAIL                          0x40
#define  USBD_SCSI_ASC_DATA_PATH_FAIL                    0x41
#define  USBD_SCSI_ASC_POWER_ON_SELF_TEST_FAIL           0x42
#define  USBD_SCSI_ASC_MSG_ERR                           0x43
#define  USBD_SCSI_ASC_INTERNAL_TARGET_FAIL              0x44
#define  USBD_SCSI_ASC_SELECT_OR_RESELECT_FAIL           0x45
#define  USBD_SCSI_ASC_UNSUCCESSFUL_SOFT_RESET           0x46
#define  USBD_SCSI_ASC_SCSI_PARITY_ERR                   0x47
#define  USBD_SCSI_ASC_INIT_DETECTED_ERR_MSG_RECEIVED    0x48
#define  USBD_SCSI_ASC_INVALID_MSG_ERR                   0x49
#define  USBD_SCSI_ASC_CMD_PHASE_ERR                     0x4A
#define  USBD_SCSI_ASC_DATA_PHASE_ERR                    0x4B
#define  USBD_SCSI_ASC_LOG_UNIT_FAILED_SELF_CFG          0x4C
#define  USBD_SCSI_ASC_OVERLAPPED_CMDS_ATTEMPTED         0x4E

#define  USBD_SCSI_ASC_WR_APPEND_ERR                     0x50
#define  USBD_SCSI_ASC_ERASE_FAIL                        0x51
#define  USBD_SCSI_ASC_CARTRIDGE_FAULT                   0x52
#define  USBD_SCSI_ASC_MEDIA_LOAD_OR_EJECT_FAILED        0x53
#define  USBD_SCSI_ASC_SCSI_TO_HOST_SYSTEM_IF_FAIL       0x54
#define  USBD_SCSI_ASC_SYSTEM_RESOURCE_FAIL              0x55
#define  USBD_SCSI_ASC_UNABLE_TO_RECOVER_TOC             0x57
#define  USBD_SCSI_ASC_GENERATION_DOES_NOT_EXIST         0x58
#define  USBD_SCSI_ASC_UPDATED_BLOCK_RD                  0x59
#define  USBD_SCSI_ASC_OP_REQUEST_OR_STATE_CHANGE_INPUT  0x5A
#define  USBD_SCSI_ASC_LOG_EXCEPT                        0x5B
#define  USBD_SCSI_ASC_RPL_STATUS_CHANGE                 0x5C
#define  USBD_SCSI_ASC_FAIL_PREDICTION_TH_EXCEEDED       0x5D
#define  USBD_SCSI_ASC_LOW_POWER_CONDITION_ON            0x5E

#define  USBD_SCSI_ASC_LAMP_FAIL                         0x60
#define  USBD_SCSI_ASC_VIDEO_ACQUISITION_ERR             0x61
#define  USBD_SCSI_ASC_SCAN_HEAD_POSITIONING_ERR         0x62
#define  USBD_SCSI_ASC_END_OF_USER_AREA_ENCOUNTERED      0x63
#define  USBD_SCSI_ASC_ILLEGAL_MODE_FOR_THIS_TRACK       0x64
#define  USBD_SCSI_ASC_VOLTAGE_FAULT                     0x65
#define  USBD_SCSI_ASC_AUTO_DOCUMENT_FEEDER_COVER_UP     0x66
#define  USBD_SCSI_ASC_CONFIGURATION_FAIL                0x67
#define  USBD_SCSI_ASC_LOG_UNIT_NOT_CONFIGURED           0x68
#define  USBD_SCSI_ASC_DATA_LOSS_ON_LOG_UNIT             0x69
#define  USBD_SCSI_ASC_INFORMATIONAL_REFER_TO_LOG        0x6A
#define  USBD_SCSI_ASC_STATE_CHANGE_HAS_OCCURRED         0x6B
#define  USBD_SCSI_ASC_REBUILD_FAIL_OCCURRED             0x6C
#define  USBD_SCSI_ASC_RECALCULATE_FAIL_OCCURRED         0x6D
#define  USBD_SCSI_ASC_CMD_TO_LOG_UNIT_FAILED            0x6E
#define  USBD_SCSI_ASC_COPY_PROTECTION_KEY_EXCHANGE_FAIL 0x6F
#define  USBD_SCSI_ASC_DECOMPRESSION_EXCEPT_LONG_ALGO_ID 0x71
#define  USBD_SCSI_ASC_SESSION_FIXATION_ERR              0x72
#define  USBD_SCSI_ASC_CD_CONTROL_ERR                    0x73
#define  USBD_SCSI_ASC_SECURITY_ERR                      0x74

//                                                                 ------- SCSI MODE PAGE CODES (See Notes #5) --------
#define USBD_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY    0x01
#define USBD_SCSI_PAGE_CODE_FORMAT_DEVICE                0x03
#define USBD_SCSI_PAGE_CODE_FLEXIBLE_DISK                0x05
#define USBD_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS     0x1C
#define USBD_SCSI_PAGE_CODE_ALL                          0x3F

#define USBD_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS   0x0A
#define USBD_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY  0x0A
#define USBD_SCSI_PAGE_LENGTH_FLEXIBLE_DISK              0x1E
#define USBD_SCSI_PAGE_LENGTH_FORMAT_DEVICE              0x16

/********************************************************************************************************
 *                                               INQUIRY CMD
 *
 * Note(s) : (1) The peripheral qualifiers are specified in 'SCSI Primary Commands - 3' (SPC-3),
 *               Revision 23, Section 6.4.2, Table 82 - Peripheral Qualifer.
 *
 *           (2) The peripheral device types are specified in 'SCSI Primary Commands - 3' (SPC-3),
 *               Revision 23, Section 6.4.2, Table 83 - Peripheral Device Type.
 *******************************************************************************************************/

//                                                                 -------------- PERIPHERAL QUALIFIERS ---------------
#define  USBD_SCSI_PER_QUAL_CONN                        0x00
#define  USBD_SCSI_PER_QUAL_NOT_CONN                    0x01
#define  USBD_SCSI_PER_QUAL_NOT_SUPPORTED               0x03
//                                                                 ------------- PERIPHERAL DEVICE TYPES --------------
#define  USBD_SCSI_PER_DEV_TYPE_DIRECT_ACCESS_BLOCK_DEV 0x00
#define  USBD_SCSI_PER_DEV_TYPE_SEQ_ACCESS_DEV          0x01
#define  USBD_SCSI_PER_DEV_TYPE_PRINTER_DEV             0x02
#define  USBD_SCSI_PER_DEV_TYPE_PROCESSOR_DEV           0x03
#define  USBD_SCSI_PER_DEV_TYPE_WR_ONCE_DEV             0x04
#define  USBD_SCSI_PER_DEV_TYPE_CD_DVD_DEV              0x05
#define  USBD_SCSI_PER_DEV_TYPE_OPTICAL_MEM_DEV         0x07
#define  USBD_SCSI_PER_DEV_TYPE_MEDIUM_CHANGER_DEV      0x08
#define  USBD_SCSI_PER_DEV_TYPE_STORAGE_ARRAY_CTRL_DEV  0x0C
#define  USBD_SCSI_PER_DEV_TYPE_ENCLOSURE_SVC_DEV       0x0D
#define  USBD_SCSI_PER_DEV_TYPE_SIMPL_DIRECT_ACCESS_DEV 0x0E
#define  USBD_SCSI_PER_DEV_TYPE_OPTICAL_CARD_RD_WR_DEV  0x0F
#define  USBD_SCSI_PER_DEV_TYPE_BRIDGE_CTRL_CMDS        0x10
#define  USBD_SCSI_PER_DEV_TYPE_OBJ_BASED_STORAGE_DEV   0x11
#define  USBD_SCSI_PER_DEV_TYPE_AUTOMATION_DRIVE_IF     0x12

#define  USBD_SCSI_INQUIRY_RMB                          DEF_BIT_07

/********************************************************************************************************
 *                                           START STOP UNIT CMD
 *******************************************************************************************************/

#define  USBD_SCSI_START_STOP_UNIT_START            DEF_BIT_00
#define  USBD_SCSI_START_STOP_UNIT_LOEJ             DEF_BIT_01

/********************************************************************************************************
 *                                       MODE SENSE CMD AND DATA
 *******************************************************************************************************/

#define  USBD_SCSI_MSK_PAGE_CODE                        0x3F
//                                                                 ----------------- MODE SENSE DATA ------------------
#define  USBD_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_EN     0x00
#define  USBD_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_PROT   0x80

#define  USBD_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_6_LEN    3u
#define  USBD_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_10_LEN   6u
#define  USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN       2u
//                                                                 --------- INFO EXCEPTIONS CTRL PAGE PARAM ----------
#define  USBD_SCSI_MODE_SENSE_DATA_MRIE_NO_SENSE        0x05
//                                                                 ---------- READ/WRITE ERR RECOVERY PARAM -----------
#define  USBD_SCSI_MODE_SENSE_DATA_AWRE                 0x80
#define  USBD_SCSI_MODE_SENSE_DATA_RD_RETRY_CNT            3u
#define  USBD_SCSI_MODE_SENSE_DATA_WR_RETRY_CNT            3u
#define  USBD_SCSI_MODE_SENSE_DATA_CORRECTION_SPAN      0x00
#define  USBD_SCSI_MODE_SENSE_DATA_HEAD_OFFSET_CNT      0x00
#define  USBD_SCSI_MODE_SENSE_DATA_DATA_STROBE_OFFSET   0x00
#define  USBD_SCSI_MODE_SENSE_DATA_RECOVERY_LIMIT       0x00

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ SCSI MODULE ROOT STRUCTURE ------------
typedef struct usbd_scsi {
  MEM_SEG               *MemSegPtr;                             // Pointer to data memory segment.

  USBD_SCSI_STORAGE_LUN *LunTbl;                                // Pointer to table of logical units.
  CPU_INT08U            LunNbrNext;                             // Indesx of next logical unit to allocate.
} USBD_SCSI;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_SCSI *USBD_SCSI_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_SCSI_ModeSenseDataPrepare(const USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                           CPU_INT08U                  *p_data_buf,
                                           CPU_INT08U                  scsi_cmd,
                                           CPU_INT08U                  page_code);

static void USBD_SCSI_SenseDataUpdate(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                      CPU_INT08U            sense_key,
                                      CPU_INT08U            sense_code);

static void USBD_SCSI_InquiryDataPrepare(const USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                         CPU_INT08U                  *p_resp_buf);

static void USBD_SCSI_PageRdWrErrRecovery(void *p_buf_dest);

static void USBD_SCSI_PageInfoExcept(void *p_buf_dest);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBD_SCSI_Init()
 *
 * @brief    Initialize internal tables, variables and the storage layer.
 *
 * @param    p_cfg       Pointer to SCSI configuration structure.
 *
 * @param    p_mem_seg   Pointer to memory segment from where to allocate internal data.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_SCSI_Init(USBD_SCSI_CFG *p_cfg,
                    MEM_SEG       *p_mem_seg,
                    RTOS_ERR      *p_err)
{
  CPU_INT08U ix;

  USBD_SCSI_Ptr = (USBD_SCSI *)Mem_SegAlloc("USBD - MSC SCSI root struct",
                                            p_mem_seg,
                                            sizeof(USBD_SCSI),
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_SCSI_Ptr->LunNbrNext = p_cfg->LUN_Qty;

  USBD_SCSI_Ptr->LunTbl = (USBD_SCSI_STORAGE_LUN *)Mem_SegAlloc("USBD - MSC SCSI lun table",
                                                                p_mem_seg,
                                                                sizeof(USBD_SCSI_STORAGE_LUN) * p_cfg->LUN_Qty,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < p_cfg->LUN_Qty; ix++) {
    Mem_Clr((void *)&USBD_SCSI_Ptr->LunTbl[ix],
            sizeof(USBD_SCSI_STORAGE_LUN));
  }

  USBD_SCSI_Ptr->MemSegPtr = p_mem_seg;
}

/****************************************************************************************************//**
 *                                           USBD_SCSI_LunAdd()
 *
 * @brief    Initialize the specified logical unit.
 *
 * @param    p_scsi_storage_api  Pointer to storage driver API.
 *
 * @param    p_lu_info           Pointer to logical unit information structure.
 *
 * @param    p_lu_fncts          Pointer to logical unit callback functions structure.
 *                                   ---- Structure assumed to be persistent. ----
 *
 * @param    p_err               Pointer to error variable.
 *
 * @return   Pointer to SCSI-related LUN data.
 *******************************************************************************************************/
void *USBD_SCSI_LunAdd(CPU_INT08U         lu_nbr,
                       USBD_SCSI_LU_INFO  *p_lu_info,
                       USBD_SCSI_LU_FNCTS *p_lu_fncts,
                       RTOS_ERR           *p_err)
{
  CPU_INT08U            scsi_lun_nbr;
  USBD_SCSI_STORAGE_LUN *p_storage_lun;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((p_lu_info != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_lu_info->SCSI_StorageAPI_Ptr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_lu_info->VendIdPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_lu_info->ProdIdPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    CPU_SIZE_T str_len;

    str_len = Str_Len(p_lu_info->VendIdPtr);
    RTOS_ASSERT_DBG_ERR_SET((str_len <= USBD_SCSI_DEV_MAX_VEND_ID_LEN), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

    str_len = Str_Len(p_lu_info->ProdIdPtr);
    RTOS_ASSERT_DBG_ERR_SET((str_len <= USBD_SCSI_DEV_MAX_PROD_ID_LEN), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }
#endif

  CORE_ENTER_ATOMIC();
  if (USBD_SCSI_Ptr->LunNbrNext == 0u) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (DEF_NULL);
  }

  scsi_lun_nbr = USBD_SCSI_Ptr->LunNbrNext - 1u;
  USBD_SCSI_Ptr->LunNbrNext--;
  CORE_EXIT_ATOMIC();

  p_storage_lun = &USBD_SCSI_Ptr->LunTbl[scsi_lun_nbr];

  p_storage_lun->SCSI_StorageApiPtr = p_lu_info->SCSI_StorageAPI_Ptr;
  p_storage_lun->LBAddr = 0u;
  p_storage_lun->LBCnt = 0u;
  p_storage_lun->SenseKey = 0u;
  p_storage_lun->ASC = 0u;
  p_storage_lun->VendorIdStr = p_lu_info->VendIdPtr;
  p_storage_lun->ProdIdStr = p_lu_info->ProdIdPtr;
  p_storage_lun->ProdRevisionLevel = p_lu_info->ProdRevLevel;
  p_storage_lun->IsRdOnly = p_lu_info->IsRdOnly;
  p_storage_lun->LU_FnctsPtr = p_lu_fncts;
  p_storage_lun->LU_Nbr = lu_nbr;
  p_storage_lun->OpenedFlag = DEF_FALSE;
  p_storage_lun->PresentFlag = DEF_FALSE;

  p_storage_lun->StateLock = KAL_LockCreate("USBD - SCSI LUN state lock",
                                            DEF_NULL,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_lu_info->SCSI_StorageAPI_Ptr->Init(USBD_SCSI_Ptr->MemSegPtr, // Init logical unit.
                                       p_storage_lun,
                                       p_err);

  return ((void *)p_storage_lun);
}

/****************************************************************************************************//**
 *                                           USBD_SCSI_LunAttach()
 *
 * @brief    Attach a storage medium to the given SCSI LUN.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @param    media_name  Storage media name.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_SCSI_LunAttach(void     *p_lun_data,
                         CPU_CHAR *media_name,
                         RTOS_ERR *p_err)
{
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  RTOS_ERR              err_rel;

  KAL_LockAcquire(p_storage_lun->StateLock,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_storage_lun->OpenedFlag == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto end_rel;
  }

  p_storage_lun->SCSI_StorageApiPtr->Open(p_storage_lun,
                                          media_name,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_storage_lun->OpenedFlag = DEF_YES;
  p_storage_lun->PresentFlag = DEF_YES;

end_rel:
  KAL_LockRelease(p_storage_lun->StateLock, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR((RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                           USBD_SCSI_LunDetach()
 *
 * @brief    Detach a storage medium from the given SCSI LUN.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_SCSI_LunDetach(void     *p_lun_data,
                         RTOS_ERR *p_err)
{
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  RTOS_ERR              err_rel;

  KAL_LockAcquire(p_storage_lun->StateLock,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_storage_lun->SCSI_StorageApiPtr->Close(p_storage_lun,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_storage_lun->OpenedFlag = DEF_NO;
  p_storage_lun->PresentFlag = DEF_NO;

end_rel:
  KAL_LockRelease(p_storage_lun->StateLock, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR((RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//**
 *                                       USBD_SCSI_LunCapacityGet()
 *
 * @brief    Retrieves capacity of logical unit.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_SCSI_LunCapacityGet(void             *p_lun_data,
                              USBD_SCSI_LB_QTY *p_lb_qty,
                              CPU_INT32U       *p_blk_size,
                              RTOS_ERR         *p_err)
{
  CPU_BOOLEAN           result;
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  RTOS_ERR              err_rel;

  KAL_LockAcquire(p_storage_lun->StateLock,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((p_storage_lun->OpenedFlag != DEF_YES)
      || (p_storage_lun->PresentFlag != DEF_YES)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto end_rel;
  }

  //                                                               Get capacity, nbr of blks and blk size.
  result = p_storage_lun->SCSI_StorageApiPtr->CapacityGet(p_storage_lun,
                                                          p_lb_qty,
                                                          p_blk_size);
  if (!result) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    goto end_rel;
  }

end_rel:
  KAL_LockRelease(p_storage_lun->StateLock, &err_rel);
  if (RTOS_ERR_CODE_GET(err_rel) != RTOS_ERR_NONE) {
    LOG_ERR((RTOS_ERR_LOG_ARG_GET(err_rel)));
  }
}

/****************************************************************************************************//***
 *                                           USBD_SCSI_CmdProcess()
 *
 * @brief    Process operation(s) associated with the SCSI command.
 *
 * @param    p_lun       Pointer to Logical Unit information.
 *
 * @param    p_cbwcb     Pointer to the Command Block Wrapper that contains the SCSI command.
 *
 * @param    p_data_buf  Pointer to data buffer to use as a response buffer.
 *
 * @param    p_resp_len  Pointer to variable that will receive the length of the SCSI response
 *                       that needs to be transfered in the data stage.
 *
 * @param    p_data_dir  Pointer to variable that will receive the data transfer direction
 *                       indicator:
 *                           - USBD_SCSI_CBW_DEVICE_TO_HOST     Data stage response from device to host.
 *                           - USBD_SCSI_CBW_HOST_TO_DEVICE     Data stage response from host to device.
 *
 * @note     (1) The format of standard INQUIRY data is specified in 'SCSI Primary Commands - 3'
 *               (SPC-3), Revision 23, Section 6.4.
 *
 * @note     (2) The format of TEST UNIT READY command is specified in 'SCSI Primary Commands - 3'
 *               (SPC-3), Revision 23, Section 6.33.
 *
 * @note     (3) The format of READ CAPACITY (10) command is specified in 'SCSI Blocks Commands - 3'
 *               (SBC-3), Revision 16, Section 5.12.
 *
 * @note     (4) The format of READ CAPACITY (16) command is specified in 'SCSI Blocks Commands - 3'
 *               (SBC-3), Revision 16, Section 5.13.
 *
 * @note     (5) The format of READ(10) command is specified in 'SCSI Block Commands - 3'
 *               (SBC), Revision 16, Section 5.8.
 *
 * @note     (6) The format of READ(12) command is specified in 'SCSI Block Commands - 3'
 *               (SBC), Revision 16, Section 5.9.
 *
 * @note     (7) The format of READ(16) command is specified in 'SCSI Block Commands - 3'
 *               (SBC), Revision 16, Section 5.10.
 *
 * @note     (8) The format of WRITE(10) command is specified in 'SCSI Block Commands - 3'
 *               (SBC), Revision 16, Section 5.27.
 *
 * @note     (9) The format of WRITE(12) command is specified in 'SCSI Block Commands - 3'
 *               (SBC), Revision 16, Section 5.28.
 *
 * @note     (10) The format of WRITE(16) command is specified in 'SCSI Block Commands - 3'
 *                (SBC), Revision 16, Section 5.29.
 *
 * @note     (11) The format of VERIFY(10) command is specified in 'SCSI Block Commands - 3'
 *                (SBC), Revision 16, Section 5.22.
 *
 * @note     (12) The format of VERIFY(12) command is specified in 'SCSI Block Commands - 3'
 *                (SBC), Revision 16, Section 5.23.
 *
 * @note     (13) The format of VERIFY(16) command is specified in 'SCSI Block Commands - 3'
 *                (SBC), Revision 16, Section 5.24.
 *
 * @note     (14) The format of MODE SENSE(6) command is specified in 'SCSI Primary Commands - 3'
 *                (SPC-3), Revision 23, Section 6.9.
 *
 * @note     (15) The format of MODE SENSE(10) command is specified in 'SCSI Primary Commands - 3'
 *                (SPC-3), Revision 23, Section 6.10.
 *
 * @note     (16) The format of REQUEST SENSE command is specified in 'SCSI Primary Commands - 3'
 *                (SPC-3), Revision 23, Section 6.25.
 *
 * @note     (17) The format of PREVENT ALLOW MEDIUM REMOVAL command is specified in 'SCSI Primary
 *                Commands - 3' (SPC-3), Revision 23, Section 6.13.
 *
 * @note     (18) The format of START STOP UNIT command is specified in 'SCSI Primary
 *                Commands - 3' (SPC-3), Revision 23, Section 5.19.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_SCSI_CmdProcess(void             *p_lun_data,
                                 const CPU_INT08U *p_cbwcb,
                                 CPU_INT08U       *p_data_buf,
                                 CPU_INT32U       *p_resp_len,
                                 CPU_INT08U       *p_data_dir)
{
  CPU_BOOLEAN           result = DEF_OK;
  CPU_INT08U            scsi_cmd = p_cbwcb[0u];
  CPU_INT08U            page_code;
  CPU_INT08U            cmdt_evpd;
  CPU_INT08U            loej;
  CPU_INT08U            start_flag;
  USBD_SCSI_LB_QTY      nbr_blks;
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  USBD_SCSI_STORAGE_API *p_scsi_storage_api = p_storage_lun->SCSI_StorageApiPtr;
  CORE_DECLARE_IRQ_STATE;

  *p_resp_len = 0u;

  //                                                               Reset req sense data with no err cond.
  if (scsi_cmd != USBD_SCSI_CMD_REQUEST_SENSE) {
    USBD_SCSI_SenseDataUpdate(p_storage_lun,
                              USBD_SCSI_SENSE_KEY_NO_SENSE,
                              USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
  }

  switch (scsi_cmd) {
    case USBD_SCSI_CMD_INQUIRY:                                 // --------------  INQUIRY(see Notes #1) --------------
      LOG_VRB(("USBD MSC SCSI: INQUIRY Command"));

      cmdt_evpd = p_cbwcb[1u] & 0x03u;                          // Get the enable vital prod data bit.
      page_code = p_cbwcb[2u];                                  // Page code for vital prod data info.

      if ((cmdt_evpd == USBD_SCSI_STD_INQUIRY_DATA)
          && (page_code == 0u)) {
        USBD_SCSI_InquiryDataPrepare(p_storage_lun,
                                     p_data_buf);

        *p_resp_len = DEF_MIN(USBD_SCSI_INQUIRY_DATA_LEN, p_cbwcb[4u]);
        *p_data_dir = USBD_SCSI_CBW_DEVICE_TO_HOST;
      } else {                                                  // Unsupported request.
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                  USBD_SCSI_ASC_INVALID_FIELD_IN_CDB);
      }
      break;

    case USBD_SCSI_CMD_TEST_UNIT_READY:                         // ----------- TEST UNIT READY(see Notes #2) ----------
      LOG_VRB(("USBD MSC SCSI: TEST UNIT READY Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->PresentFlag != DEF_YES) {
        CORE_EXIT_ATOMIC();

        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_NOT_RDY,
                                  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT);

        result = DEF_FAIL;
        break;
      } else {
        CORE_EXIT_ATOMIC();
      }
      break;

    case USBD_SCSI_CMD_SERVICE_ACTION_IN_16:                    // --------- READ CAPACITY(16)(see Notes #4) ----------
#if (USBD_SCSI_CFG_64_BIT_LBA_EN == DEF_DISABLED)
      result = DEF_FAIL;
      USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case USBD_SCSI_CMD_READ_CAPACITY_10:                        // --------- READ CAPACITY(10)(see Notes #3) ----------
      LOG_VRB(("USBD MSC SCSI: READ CAPACITY 10 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->PresentFlag == DEF_YES) {
        CORE_EXIT_ATOMIC();

        //                                                         Get capacity, nbr of blks and blk size.
        result = p_scsi_storage_api->CapacityGet(p_storage_lun,
                                                 &p_storage_lun->NbrBlocks,
                                                 &p_storage_lun->BlockSize);
        if (!result) {
          USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                    USBD_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                    USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
          break;
        }

        if (scsi_cmd == USBD_SCSI_CMD_READ_CAPACITY_10) {
          MEM_VAL_SET_INT32U_BIG(&p_data_buf[0u], p_storage_lun->NbrBlocks - 1u);
          MEM_VAL_SET_INT32U_BIG(&p_data_buf[4u], p_storage_lun->BlockSize);

          *p_resp_len = USBD_SCSI_RD_CAPACITY_10_PARAM_DATA_LEN;
        } else {
          nbr_blks = p_storage_lun->NbrBlocks - 1u;
          MEM_VAL_COPY_SET_INTU_BIG(&p_data_buf[0u], &nbr_blks, 8u);

          Mem_Clr((void *)&p_data_buf[8u], 8u);

          MEM_VAL_SET_INT32U_BIG(&p_data_buf[8u], p_storage_lun->BlockSize);

          *p_resp_len = USBD_SCSI_RD_CAPACITY_16_PARAM_DATA_LEN;
        }

        *p_data_dir = USBD_SCSI_CBW_DEVICE_TO_HOST;
      } else {
        CORE_EXIT_ATOMIC();

        result = DEF_FAIL;
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_NOT_RDY,
                                  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case USBD_SCSI_CMD_READ_16:                                 // -------------- READ(16)(see Notes #7) --------------
#if (USBD_SCSI_CFG_64_BIT_LBA_EN == DEF_DISABLED)
      result = DEF_FAIL;
      USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case USBD_SCSI_CMD_READ_12:                                 // -------------- READ(12)(see Notes #6) --------------
    case USBD_SCSI_CMD_READ_10:                                 // -------------- READ(10)(see Notes #5) --------------

      LOG_VRB(("USBD MSC SCSI: READ 10 / 12 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->PresentFlag == DEF_YES) {
        CORE_EXIT_ATOMIC();

        if (scsi_cmd == USBD_SCSI_CMD_READ_10) {
          p_storage_lun->LBAddr = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT16U_BIG(&p_cbwcb[7u]);
        } else if (scsi_cmd == USBD_SCSI_CMD_READ_12) {
          p_storage_lun->LBAddr = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[6u]);
        } else {
          MEM_VAL_COPY_GET_INTU_BIG(&p_storage_lun->LBAddr, &p_cbwcb[2u], 8u);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[10u]);
        }

        *p_resp_len = p_storage_lun->LBCnt * p_storage_lun->BlockSize;
        *p_data_dir = USBD_SCSI_CBW_DEVICE_TO_HOST;
      } else {
        CORE_EXIT_ATOMIC();

        result = DEF_FAIL;
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_NOT_RDY,
                                  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case USBD_SCSI_CMD_WRITE_16:                                // ------------- WRITE(16)(see Notes #10) -------------
#if (USBD_SCSI_CFG_64_BIT_LBA_EN == DEF_DISABLED)
      result = DEF_FAIL;
      USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case USBD_SCSI_CMD_WRITE_10:                                // -------------- WRITE(10)(see Notes #8) -------------
    case USBD_SCSI_CMD_WRITE_12:                                // -------------- WRITE(12)(see Notes #9) -------------
      LOG_VRB(("USBD MSC SCSI: WRITE 10 / 12 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->PresentFlag == DEF_YES) {
        CORE_EXIT_ATOMIC();

        if (p_storage_lun->IsRdOnly == DEF_TRUE) {              // Check medium is wr protected or not.
          USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                    USBD_SCSI_SENSE_KEY_DATA_PROTECT,
                                    USBD_SCSI_ASC_WR_PROTECTED);
          result = DEF_FAIL;
          break;
        }

        if (scsi_cmd == USBD_SCSI_CMD_WRITE_10) {
          p_storage_lun->LBAddr = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT16U_BIG(&p_cbwcb[7u]);
        } else if (scsi_cmd == USBD_SCSI_CMD_WRITE_12) {
          p_storage_lun->LBAddr = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[6u]);
        } else {
          MEM_VAL_COPY_GET_INTU_BIG(&p_storage_lun->LBAddr, &p_cbwcb[2u], 8u);
          p_storage_lun->LBCnt = MEM_VAL_GET_INT32U_BIG(&p_cbwcb[10u]);
        }

        *p_resp_len = p_storage_lun->LBCnt * (p_storage_lun->BlockSize);
        *p_data_dir = USBD_SCSI_CBW_HOST_TO_DEVICE;
      } else {
        CORE_EXIT_ATOMIC();

        result = DEF_FAIL;
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_NOT_RDY,
                                  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case USBD_SCSI_CMD_MODE_SENSE_06:                           // ---------- MODE SENSE(6) (see Notes #14) -----------
    case USBD_SCSI_CMD_MODE_SENSE_10:                           // ---------- MODE SENSE(10) (see Notes #15) ----------
      LOG_VRB(("USBD MSC SCSI: MODE SENSE Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->PresentFlag == DEF_YES) {
        CORE_EXIT_ATOMIC();

        //                                                         Get page code.
        page_code = p_cbwcb[2] & USBD_SCSI_MSK_PAGE_CODE;

        switch (page_code) {
          case USBD_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS:
          case USBD_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY:
          case USBD_SCSI_PAGE_CODE_ALL:
            USBD_SCSI_ModeSenseDataPrepare(p_storage_lun,
                                           p_data_buf,
                                           scsi_cmd,
                                           page_code);

            if (scsi_cmd == USBD_SCSI_CMD_MODE_SENSE_06) {
              *p_resp_len = DEF_MIN(p_data_buf[0u] + 1u, p_cbwcb[4u]);
            } else {
              *p_resp_len = (p_cbwcb[7] << 8u) | p_cbwcb[8];
              *p_resp_len = DEF_MIN(((CPU_INT32U)p_data_buf[0u] + 1u), *p_resp_len);
            }

            *p_data_dir = USBD_SCSI_CBW_DEVICE_TO_HOST;
            break;

          default:                                              // Unsupported page code.
            USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                      USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      USBD_SCSI_ASC_INVALID_FIELD_IN_CDB);
            result = DEF_FAIL;
            break;
        }
      } else {
        CORE_EXIT_ATOMIC();

        result = DEF_FAIL;
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_NOT_RDY,
                                  USBD_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case USBD_SCSI_CMD_REQUEST_SENSE:                           // ----------- REQUEST SENSE(see Notes #16) -----------
      LOG_VRB(("USBD MSC SCSI: REQUEST SENSE Command"));
      *p_resp_len = DEF_MIN(USBD_SCSI_REQ_SENSE_DATA_LEN, p_cbwcb[4u]);

      Mem_Clr((void *)p_data_buf, *p_resp_len);

      p_data_buf[0u] = USBD_SCSI_REQ_SENSE_RESP_CODE_CUR_ERR;
      p_data_buf[2u] = p_storage_lun->SenseKey;
      p_data_buf[7u] = (USBD_SCSI_REQ_SENSE_DATA_LEN - 8u);
      p_data_buf[12u] = p_storage_lun->ASC;
      p_data_buf[13u] = 0u;

      *p_data_dir = USBD_SCSI_CBW_DEVICE_TO_HOST;
      break;

    case USBD_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:            // --- PREVENT ALLOW MEDIUM REMOVAL (see Notes #17) ---
      LOG_VRB(("USBD MSC SCSI: PREVENT ALLOW MEDIUM REMOVAL Command"));
      break;

    case USBD_SCSI_CMD_START_STOP_UNIT:                         // ---------- START STOP UNIT (see Notes #18) ---------
      LOG_VRB(("USBD MSC SCSI: START STOP UNIT Command"));

      loej = p_cbwcb[4u] & USBD_SCSI_START_STOP_UNIT_LOEJ;
      start_flag = p_cbwcb[4u] & USBD_SCSI_START_STOP_UNIT_START;

      //                                                           Eject the medium.
      if ((DEF_BIT_IS_SET(loej, USBD_SCSI_START_STOP_UNIT_LOEJ) == DEF_TRUE)
          && (DEF_BIT_IS_CLR(start_flag, USBD_SCSI_START_STOP_UNIT_START) == DEF_TRUE)) {
        USBD_SCSI_LU_FNCTS *p_lu_fnct = p_storage_lun->LU_FnctsPtr;

        CORE_ENTER_ATOMIC();
        p_storage_lun->PresentFlag = DEF_FALSE;
        CORE_EXIT_ATOMIC();

        if ((p_lu_fnct != DEF_NULL)
            && (p_lu_fnct->HostEjectEvent != DEF_NULL)) {
          p_lu_fnct->HostEjectEvent(p_storage_lun->LU_Nbr);
        }
      } else {
        result = DEF_FAIL;
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                  USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:                                                    // Cmd not supported.
      LOG_ERR(("USBD MSC SCSI: UNSUPPORTED Command"));

      result = DEF_FAIL;
      USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
  }

  return (result);
}

/****************************************************************************************************//***
 *                                           USBD_SCSI_DataRd()
 *
 * @brief    Read data from the SCSI device OR copy response data to buffer.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @param    scsi_cmd    SCSI command operation code.
 *
 * @param    p_data_buf  Pointer to receive buffer.
 *
 * @param    data_len    Number of bytes to read.
 *
 * @return   DEF_OK,   If successful,
 *           DEF_FAIL, Otherwise.
 *
 * @note     (1) SCSI commands that require a Data IN phase are: INQUIRY, READ CAPACITY, MODE SENSE,
 *               REQUEST SENSE and READ. For all these SCSI commands except READ, the buffer containing
 *               data for the host is prepared upfront during the CBW processing done in
 *               USBD_SCSI_CmdProcess().
 *******************************************************************************************************/
CPU_BOOLEAN USBD_SCSI_DataRd(void       *p_lun_data,
                             CPU_INT08U scsi_cmd,
                             CPU_INT08U *p_data_buf,
                             CPU_INT32U data_len)
{
  CPU_BOOLEAN           result = DEF_OK;
  CPU_INT32U            lb_cnt;
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  USBD_SCSI_STORAGE_API *p_scsi_storage_api = p_storage_lun->SCSI_StorageApiPtr;

  switch (scsi_cmd) {
    case USBD_SCSI_CMD_READ_10:
    case USBD_SCSI_CMD_READ_12:
    case USBD_SCSI_CMD_READ_16:
      LOG_VRB(("USBD MSC SCSI Read data from Disk."));
      lb_cnt = data_len / p_storage_lun->BlockSize;             // Nbr of blks that can fit in scsi_data_buf.

      result = p_scsi_storage_api->Rd(p_storage_lun,
                                      p_storage_lun->LBAddr,
                                      lb_cnt,
                                      p_data_buf);
      if (result) {
        p_storage_lun->LBAddr += lb_cnt;
        p_storage_lun->LBCnt -= lb_cnt;
      } else {
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                  USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:                                                    // See Note #1.
      break;
  }

  return (result);
}

/****************************************************************************************************//***
 *                                           USBD_SCSI_DataWr()
 *
 * @brief    Write data to the SCSI device.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @param    scsi_cmd    SCSI command operation code.
 *
 * @param    p_data_buf  Pointer to transmit buffer.
 *
 * @param    data_len    Number of bytes to write.
 *
 * @return   DEF_OK,   If successful,
 *           DEF_FAIL, Otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN USBD_SCSI_DataWr(void       *p_lun_data,
                             CPU_INT08U scsi_cmd,
                             void       *p_data_buf,
                             CPU_INT32U data_len)
{
  CPU_BOOLEAN           result = DEF_OK;
  CPU_INT32U            lb_cnt;
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  USBD_SCSI_STORAGE_API *p_scsi_storage_api = p_storage_lun->SCSI_StorageApiPtr;

  switch (scsi_cmd) {
    case USBD_SCSI_CMD_WRITE_10:
    case USBD_SCSI_CMD_WRITE_12:
    case USBD_SCSI_CMD_WRITE_16:
      LOG_VRB(("USBD MSC SCSI Write data to Disk."));
      lb_cnt = data_len / (p_storage_lun->BlockSize);           // Nbr of blks present in scsi_data_buf.

      result = p_scsi_storage_api->Wr(p_storage_lun,
                                      p_storage_lun->LBAddr,
                                      lb_cnt,
                                      (CPU_INT08U *)p_data_buf);
      if (result) {
        p_storage_lun->LBAddr += lb_cnt;
        p_storage_lun->LBCnt -= lb_cnt;
      } else {
        USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                  USBD_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                  USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:
      USBD_SCSI_SenseDataUpdate(p_storage_lun,
                                USBD_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                USBD_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      result = DEF_FAIL;
      break;
  }

  return (result);
}

/****************************************************************************************************//***
 *                                               USBD_SCSI_Conn()
 *
 * @brief    Reset eject flag upon new configuration activation.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *******************************************************************************************************/
void USBD_SCSI_Conn(void *p_lun_data)
{
  USBD_SCSI_STORAGE_LUN *p_storage_lun = (USBD_SCSI_STORAGE_LUN *)p_lun_data;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_storage_lun->OpenedFlag == DEF_YES) {
    p_storage_lun->PresentFlag = DEF_YES;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//***
 *                                           USBD_SCSI_Disconn()
 *
 * @brief    Close the SCSI Storage Medium.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @note     (1) Unlocking a logical unit can be done with a software eject (for instance Windows
 *               right's click eject). In that case, the unlock operation must not be executed another
 *               time. If a software eject has occurred, the unlock operation done upon physical
 *               disconnection of the device must be discarded.
 *******************************************************************************************************/
void USBD_SCSI_Disconn(void *p_lun_data)
{
  PP_UNUSED_PARAM(p_lun_data);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//***
 *                                       USBD_SCSI_ModeSenseDataPrepare()
 *
 * @brief    Prepare response to be sent to host for the MODE SENSE SCSI command (see Note #1).
 *
 * @param    p_storage_lun   Pointer to Logical Unit data.
 *
 * @param    p_data_buf      Pointer to data buffer.
 *
 * @param    scsi_cmd        SCSI command operation code.
 *
 * @param    page_code       Page code.
 *
 * @note     (1) MODE SENSE allows the host to request information related to the storage media, a
 *               logical unit or the device itself. The device reports the information through the
 *               mode parameter list whose general format is:
 *
 *               +--------------------------------------+
 *               | bit  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *               | byte |
 *               ----------------------------------------
 *               |      |    Mode parameter header      |
 *               ----------------------------------------
 *               |      |    Block descriptor(s)        |
 *               ----------------------------------------
 *               |      |Mode page(s) or vendor specific|
 *               +--------------------------------------+
 *
 *               See 'SCSI Primary Commands - 3' (SPC), Revision 23, sections 6.11, 6.12 & 7.4 for more
 *               details.
 *
 * @note     (2) The Informational Exceptions Control mode page defines the methods used by the device
 *               to control the reporting and the operations of specific informational exception
 *               conditions.
 *               The format of Informational Exceptions Control mode page is specified in
 *               'SCSI Primary Commands - 3' (SPC), Revision 23, Section 7.4.11.
 *
 * @note     (3) The Read-Write Error Recovery mode page specifies the error recovery parameters the
 *               device shall use during any command that performs a read or write operation to the
 *               medium.
 *               The format of Read/Write Error Recovery mode Page is specified in
 *               'SCSI Blocks Commands - 3' (SBC-3), Revision 16, Section 6.3.5.
 *******************************************************************************************************/
static void USBD_SCSI_ModeSenseDataPrepare(const USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                           CPU_INT08U                  *p_data_buf,
                                           CPU_INT08U                  scsi_cmd,
                                           CPU_INT08U                  page_code)
{
  CPU_INT08U ix_mode_data_len;
  CPU_INT08U ix_medium_type;
  CPU_INT08U ix_dev_spec_param;
  CPU_INT08U ix_mode_page;
  CPU_INT08U ix_nxt_page;
  CPU_INT08U mode_param_hdr_len;

  //                                                               Index preparation according to MODE SENSE cmd type.
  if (scsi_cmd == USBD_SCSI_CMD_MODE_SENSE_06) {
    ix_mode_data_len = 0;
    ix_medium_type = 1;
    ix_dev_spec_param = 2;
    ix_mode_page = 4;
    mode_param_hdr_len = USBD_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_6_LEN;
  } else {                                                      // MODE SENSE(10).
    ix_mode_data_len = 1;                                       // Mode data len LSB.
    ix_medium_type = 2;
    ix_dev_spec_param = 3;
    ix_mode_page = 8;
    mode_param_hdr_len = USBD_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_10_LEN;
  }
  //                                                               Ensure Mode Sense buf properly reset.
  Mem_Clr((void *)p_data_buf,
          USBD_SCSI_MODE_SENSE_DATA_LEN);

  //                                                               ------------------ MODE PARAM HDR ------------------
  //                                                               Medium type supported by LUN.
  p_data_buf[ix_medium_type] = USBD_DISK_MEMORY_MEDIA;
  //                                                               Indicate if medium is write-protected.
  if (p_storage_lun->IsRdOnly) {
    p_data_buf[ix_dev_spec_param] = USBD_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_PROT;
  } else {
    p_data_buf[ix_dev_spec_param] = USBD_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_EN;
  }

  switch (page_code) {
    case USBD_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS:          // See Note #1.
                                                                // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + USBD_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;

      //                                                           --------------- BLK DESC & MODE PAGE ---------------
      USBD_SCSI_PageInfoExcept((void *)&p_data_buf[ix_mode_page]);
      break;

    case USBD_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY:         // See Note #2.
                                                                // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + USBD_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY;

      //                                                           --------------- BLK DESC & MODE PAGE ---------------
      USBD_SCSI_PageRdWrErrRecovery((void *)&p_data_buf[ix_mode_page]);
      break;

    case USBD_SCSI_PAGE_CODE_ALL:                               // Page Code: all pages supported by target.
                                                                // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + USBD_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY
                                     + USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + USBD_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;

      //                                                           --------------- BLK DESC & MODE PAGE ---------------
      USBD_SCSI_PageRdWrErrRecovery((void *)&p_data_buf[ix_mode_page]);

      ix_nxt_page = ix_mode_page
                    + USBD_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                    + USBD_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY;
      USBD_SCSI_PageInfoExcept((void *)&p_data_buf[ix_nxt_page]);
      break;

    default:
      break;
  }
}

/****************************************************************************************************//***
 *                                       USBD_SCSI_ReqSenseDataUpdate()
 *
 * @brief    Update Request Sense data parameters.
 *
 * @param    p_storage_lun   Pointer to Logical Unit data.
 *
 * @param    sense_key       Sense key describing an error or exception condition.
 *
 * @param    sense_code      Additional Sense Code describing sense key in detail.
 *******************************************************************************************************/
static void USBD_SCSI_SenseDataUpdate(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                      CPU_INT08U            sense_key,
                                      CPU_INT08U            sense_code)
{
  p_storage_lun->SenseKey = sense_key;
  p_storage_lun->ASC = sense_code;
}

/****************************************************************************************************//***
 *                                       USBD_SCSI_InquiryDataPrepare()
 *
 * @brief    Prepare response for INQUIRY SCSI command.
 *
 * @param    p_storage_lun   Pointer to Logical Unit data.
 *
 * @param    p_resp_buf      Pointer to response buffer.
 *
 * @note     (1) The device reports the information through the inquiry data whose general format is:
 *
 *               +------------------------------------------------------+
 *               | bit  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 *               | byte |                                               |
 *               --------------------------------------------------------
 *               |  0   | Peripheral Qual |    Peripheral Device type   |
 *               --------------------------------------------------------
 *               | 1-7  |            Controls and Flags                 |
 *               --------------------------------------------------------
 *               | 8-15 |        T10 VENDOR IDENTIFICATION              |
 *               --------------------------------------------------------
 *               | 16-31|          PRODUCT IDENTIFICATION               |
 *               --------------------------------------------------------
 *               | 32-35|          PRODUCT REVISION LEVEL               |
 *               +------------------------------------------------------+
 *
 *               See 'SCSI Primary Commands - 3' (SPC-3), Revision 23, Section 6.4.2, fore more
 *               details.
 *******************************************************************************************************/
static void USBD_SCSI_InquiryDataPrepare(const USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                         CPU_INT08U                  *p_resp_buf)
{
  //                                                               Get target info.
  p_resp_buf[0u] = USBD_SCSI_PER_DEV_TYPE_DIRECT_ACCESS_BLOCK_DEV
                   | (USBD_SCSI_PER_QUAL_CONN << 5u);
  p_resp_buf[1u] = USBD_SCSI_INQUIRY_RMB;
  p_resp_buf[2u] = USBD_SCSI_INQUIRY_VERS_SPC_3;
  p_resp_buf[3u] = USBD_SCSI_INQUIRY_RESP_DATA_FMT_DEFAULT;
  p_resp_buf[4u] = (USBD_SCSI_INQUIRY_DATA_LEN - 5u);

  //                                                               Vendor ID info.
  Str_Copy((CPU_CHAR *)&p_resp_buf[8u],
           p_storage_lun->VendorIdStr);

  //                                                               Product ID info.
  Str_Copy((CPU_CHAR *)&p_resp_buf[16u],
           p_storage_lun->ProdIdStr);

  //                                                               Product revision level.
  *(CPU_INT32U *)(&p_resp_buf[32u]) = MEM_VAL_GET_INT32U_LITTLE(&p_storage_lun->ProdRevisionLevel);
}

/****************************************************************************************************//***
 *                                       USBD_SCSI_PageRdWrErrRecovery()
 *
 * @brief    Prepare Mode Sense Data with Read/Write Error Recovery page parameters.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold Mode sense data.
 *
 * @note     (1) The format of Read/Write Error Recovery mode Page is specified in
 *               'SCSI Blocks Commands - 3' (SBC-3), Revision 16, Section 6.3.5.
 *******************************************************************************************************/
static void USBD_SCSI_PageRdWrErrRecovery(void *p_buf_dest)
{
  CPU_INT08U *p_buf_dest_08;

  p_buf_dest_08 = (CPU_INT08U *)p_buf_dest;
  p_buf_dest_08[0u] = USBD_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY;   // Page Code: rd/wr err recovery page.
  p_buf_dest_08[1u] = USBD_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY; // Page Length: rd/wr err recovery page.
  p_buf_dest_08[2u] = USBD_SCSI_MODE_SENSE_DATA_AWRE;                  // Enable AWRE.
  p_buf_dest_08[3u] = USBD_SCSI_MODE_SENSE_DATA_RD_RETRY_CNT;          // Recovery algorithm attempts during rd ops.
  p_buf_dest_08[4u] = USBD_SCSI_MODE_SENSE_DATA_CORRECTION_SPAN;       // Obsolete.
  p_buf_dest_08[5u] = USBD_SCSI_MODE_SENSE_DATA_HEAD_OFFSET_CNT;       // Obsolete.
  p_buf_dest_08[6u] = USBD_SCSI_MODE_SENSE_DATA_DATA_STROBE_OFFSET;    // Obsolete.
  p_buf_dest_08[7u] = 0x00u;                                           // Reserved.
  p_buf_dest_08[8u] = USBD_SCSI_MODE_SENSE_DATA_WR_RETRY_CNT;          // Recovery algorithm attempts during wr ops.
  p_buf_dest_08[9u] = 0x00u;                                           // Reserved.
  //                                                               Err recovery time.
  MEM_VAL_SET_INT16U_BIG(p_buf_dest_08 + 10, USBD_SCSI_MODE_SENSE_DATA_RECOVERY_LIMIT);
}

/****************************************************************************************************//***
 *                                       USBD_SCSI_PageInfoExcept()
 *
 * @brief    Prepare Mode Sense Data with Informational Exceptions Control Page parameters.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold Mode sense data.
 *
 * @note     (1) The format of Informational Exceptions Control mode page is specified in
 *               'SCSI Primary Commands - 3' (SPC), Revision 23, Section 7.4.11.
 *******************************************************************************************************/
static void USBD_SCSI_PageInfoExcept(void *p_buf_dest)
{
  CPU_INT08U *p_buf_dest_08;

  p_buf_dest_08 = (CPU_INT08U *)p_buf_dest;
  p_buf_dest_08[0] = USBD_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS;
  p_buf_dest_08[1] = USBD_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;
  p_buf_dest_08[2] = 0u;
  p_buf_dest_08[3] = USBD_SCSI_MODE_SENSE_DATA_MRIE_NO_SENSE;   // Method of reporting info exceptions field.

  MEM_VAL_SET_INT32U_BIG(p_buf_dest_08 + 4, 0u);                // Interval Timer.
  MEM_VAL_SET_INT32U_BIG(p_buf_dest_08 + 8, 1u);                // Report Count.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))
