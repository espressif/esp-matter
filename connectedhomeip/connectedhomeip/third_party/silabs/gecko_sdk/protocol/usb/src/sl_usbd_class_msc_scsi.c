/***************************************************************************//**
 * @file
 * @brief USB Device Scsi Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_core.h"

#include "sl_string.h"
#include "sl_bit.h"
#include "sl_enum.h"
#include "sl_status.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_msc.h"
#include "sl_usbd_class_msc_scsi.h"
#include "sli_usbd_class_msc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

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

// Standard Inquiry Data Format (see Note #1)
#define  SLI_USBD_MSC_SCSI_DEV_MAX_VEND_ID_LEN                     8u
#define  SLI_USBD_MSC_SCSI_DEV_MAX_PROD_ID_LEN                    16u

#define  SLI_USBD_MSC_SCSI_LOCK_DLY_mS                             4u

#define  SLI_USBD_MSC_SCSI_BLOCK_DESC_LEN                          0u

#define  SLI_USBD_MSC_SCSI_INQUIRY_DATA_LEN                       36u
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_LEN                   256u
#define  SLI_USBD_MSC_SCSI_RD_CAPACITY_10_PARAM_DATA_LEN           8u
#define  SLI_USBD_MSC_SCSI_RD_CAPACITY_16_PARAM_DATA_LEN          16u
#define  SLI_USBD_MSC_SCSI_REQ_SENSE_DATA_LEN                     18u
// Data Xfer Dir
#define  SLI_USBD_MSC_SCSI_CBW_HOST_TO_DEVICE                    0x00
#define  SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST                    0x80
// Direct Access Medium Type
#define  SLI_USBD_MSC_SCSI_DISK_MEMORY_MEDIA                     0x00
// Inquiry Data
#define  SLI_USBD_MSC_SCSI_STD_INQUIRY_DATA                      0x00
#define  SLI_USBD_MSC_SCSI_INQUIRY_VERS_OBSOLETE                 0x02
#define  SLI_USBD_MSC_SCSI_INQUIRY_VERS_SPC_3                    0x05
#define  SLI_USBD_MSC_SCSI_INQUIRY_RESP_DATA_FMT_DEFAULT         0x02
// Req Sense Data
#define  SLI_USBD_MSC_SCSI_REQ_SENSE_RESP_CODE_CUR_ERR           0x70

/********************************************************************************************************
 *                                               SCSI COMMANDS
 *
 * Note(s) : (1) The SCSI Operation Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.3 Operation Codes.
 *
 *           (2) The SCSI status Codes can be found in SCSI Architecture Model - 4 (SAM-3),
 *               section 5.3.1 status Codes.
 *
 *           (3) The SCSI Sense Keys can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section 4.5.6 status Key and Sense Code Definitions, Table 27.
 *
 *           (4) The SCSI Additional Sense Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.2 Additional Sense Codes.
 *
 *           (5) The SCSI Mode Page Codes can be found in SCSI Primary Commands - 3 (SPC-3),
 *               section D.6 Mode Page Codes.
 *******************************************************************************************************/

// SCSI Opcodes (See Notes #1)
#define  SLI_USBD_MSC_SCSI_CMD_TEST_UNIT_READY                   0x00
#define  SLI_USBD_MSC_SCSI_CMD_REWIND                            0x01
#define  SLI_USBD_MSC_SCSI_CMD_REZERO_UNIT                       0x01
#define  SLI_USBD_MSC_SCSI_CMD_REQUEST_SENSE                     0x03
#define  SLI_USBD_MSC_SCSI_CMD_FORMAT_UNIT                       0x04
#define  SLI_USBD_MSC_SCSI_CMD_FORMAT_MEDIUM                     0x04
#define  SLI_USBD_MSC_SCSI_CMD_FORMAT                            0x04
#define  SLI_USBD_MSC_SCSI_CMD_READ_BLOCK_LIMITS                 0x05
#define  SLI_USBD_MSC_SCSI_CMD_REASSIGN_BLOCKS                   0x07
#define  SLI_USBD_MSC_SCSI_CMD_INITIALIZE_ELEMENT_STATUS         0x07
#define  SLI_USBD_MSC_SCSI_CMD_READ_06                           0x08
#define  SLI_USBD_MSC_SCSI_CMD_RECEIVE                           0x08
#define  SLI_USBD_MSC_SCSI_CMD_GET_MESSAGE_06                    0x08
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_06                          0x0A
#define  SLI_USBD_MSC_SCSI_CMD_SEND_06                           0x0A
#define  SLI_USBD_MSC_SCSI_CMD_SEND_MESSAGE_06                   0x0A
#define  SLI_USBD_MSC_SCSI_CMD_PRINT                             0x0A
#define  SLI_USBD_MSC_SCSI_CMD_SEEK_06                           0x0B
#define  SLI_USBD_MSC_SCSI_CMD_SET_CAPACITY                      0x0B
#define  SLI_USBD_MSC_SCSI_CMD_SLEW_AND_PRINT                    0x0B
#define  SLI_USBD_MSC_SCSI_CMD_READ_REVERSE_06                   0x0F

#define  SLI_USBD_MSC_SCSI_CMD_WRITE_FILEMARKS_06                0x10
#define  SLI_USBD_MSC_SCSI_CMD_SYNCHRONIZE_BUFFER                0x10
#define  SLI_USBD_MSC_SCSI_CMD_SPACE_06                          0x11
#define  SLI_USBD_MSC_SCSI_CMD_INQUIRY                           0x12
#define  SLI_USBD_MSC_SCSI_CMD_VERIFY_06                         0x13
#define  SLI_USBD_MSC_SCSI_CMD_RECOVER_BUFFERED_DATA             0x14
#define  SLI_USBD_MSC_SCSI_CMD_MODE_SELECT_06                    0x15
#define  SLI_USBD_MSC_SCSI_CMD_RESERVE_06                        0x16
#define  SLI_USBD_MSC_SCSI_CMD_RESERVE_ELEMENT_06                0x16
#define  SLI_USBD_MSC_SCSI_CMD_RELEASE_06                        0x17
#define  SLI_USBD_MSC_SCSI_CMD_RELEASE_ELEMENT_06                0x17
#define  SLI_USBD_MSC_SCSI_CMD_COPY                              0x18
#define  SLI_USBD_MSC_SCSI_CMD_ERASE_06                          0x19
#define  SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_06                     0x1A
#define  SLI_USBD_MSC_SCSI_CMD_START_STOP_UNIT                   0x1B
#define  SLI_USBD_MSC_SCSI_CMD_LOAD_UNLOAD                       0x1B
#define  SLI_USBD_MSC_SCSI_CMD_SCAN_06                           0x1B
#define  SLI_USBD_MSC_SCSI_CMD_STOP_PRINT                        0x1B
#define  SLI_USBD_MSC_SCSI_CMD_OPEN_CLOSE_IMPORT_EXPORT_ELEMENT  0x1B
#define  SLI_USBD_MSC_SCSI_CMD_RECEIVE_DIAGNOSTIC_RESULTS        0x1C
#define  SLI_USBD_MSC_SCSI_CMD_SEND_DIAGNOSTIC                   0x1D
#define  SLI_USBD_MSC_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL      0x1E

#define  SLI_USBD_MSC_SCSI_CMD_READ_FORMAT_CAPACITIES            0x23
#define  SLI_USBD_MSC_SCSI_CMD_SET_WINDOW                        0x24
#define  SLI_USBD_MSC_SCSI_CMD_READ_CAPACITY_10                  0x25
#define  SLI_USBD_MSC_SCSI_CMD_READ_CAPACITY                     0x25
#define  SLI_USBD_MSC_SCSI_CMD_READ_CARD_CAPACITY                0x25
#define  SLI_USBD_MSC_SCSI_CMD_GET_WINDOW                        0x25
#define  SLI_USBD_MSC_SCSI_CMD_READ_10                           0x28
#define  SLI_USBD_MSC_SCSI_CMD_GET_MESSAGE_10                    0x28
#define  SLI_USBD_MSC_SCSI_CMD_READ_GENERATION                   0x29
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_10                          0x2A
#define  SLI_USBD_MSC_SCSI_CMD_SEND_10                           0x2A
#define  SLI_USBD_MSC_SCSI_CMD_SEND_MESSAGE_10                   0x2A
#define  SLI_USBD_MSC_SCSI_CMD_SEEK_10                           0x2B
#define  SLI_USBD_MSC_SCSI_CMD_LOCATE_10                         0x2B
#define  SLI_USBD_MSC_SCSI_CMD_POSITION_TO_ELEMENT               0x2B
#define  SLI_USBD_MSC_SCSI_CMD_ERASE_10                          0x2C
#define  SLI_USBD_MSC_SCSI_CMD_READ_UPDATED_BLOCK                0x2D
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_AND_VERIFY_10               0x2E
#define  SLI_USBD_MSC_SCSI_CMD_VERIFY_10                         0x2F

#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_HIGH_10               0x30
#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_EQUAL_10              0x31
#define  SLI_USBD_MSC_SCSI_CMD_OBJECT_POSITION                   0x31
#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_LOW_10                0x32
#define  SLI_USBD_MSC_SCSI_CMD_SET_LIMITS_10                     0x33
#define  SLI_USBD_MSC_SCSI_CMD_PRE_FETCH_10                      0x34
#define  SLI_USBD_MSC_SCSI_CMD_READ_POSITION                     0x34
#define  SLI_USBD_MSC_SCSI_CMD_GET_DATA_BUFFER_STATUS            0x34
#define  SLI_USBD_MSC_SCSI_CMD_SYNCHRONIZE_CACHE_10              0x35
#define  SLI_USBD_MSC_SCSI_CMD_LOCK_UNLOCK_CACHE_10              0x36
#define  SLI_USBD_MSC_SCSI_CMD_READ_DEFECT_DATA_10               0x37
#define  SLI_USBD_MSC_SCSI_CMD_INIT_ELEMENT_STATUS_WITH_RANGE    0x37
#define  SLI_USBD_MSC_SCSI_CMD_MEDIUM_SCAN                       0x38
#define  SLI_USBD_MSC_SCSI_CMD_COMPARE                           0x39
#define  SLI_USBD_MSC_SCSI_CMD_COPY_AND_VERIFY                   0x3A
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_BUFFER                      0x3B
#define  SLI_USBD_MSC_SCSI_CMD_READ_BUFFER                       0x3C
#define  SLI_USBD_MSC_SCSI_CMD_UPDATE_BLOCK                      0x3D
#define  SLI_USBD_MSC_SCSI_CMD_READ_LONG_10                      0x3E
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_LONG_10                     0x3F

#define  SLI_USBD_MSC_SCSI_CMD_CHANGE_DEFINITION                 0x40
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_SAME_10                     0x41
#define  SLI_USBD_MSC_SCSI_CMD_READ_SUBCHANNEL                   0x42
#define  SLI_USBD_MSC_SCSI_CMD_READ_TOC_PMA_ATIP                 0x43
#define  SLI_USBD_MSC_SCSI_CMD_REPORT_DENSITY_SUPPORT            0x44
#define  SLI_USBD_MSC_SCSI_CMD_READ_HEADER                       0x44
#define  SLI_USBD_MSC_SCSI_CMD_PLAY_AUDIO_10                     0x45
#define  SLI_USBD_MSC_SCSI_CMD_GET_CONFIGURATION                 0x46
#define  SLI_USBD_MSC_SCSI_CMD_PLAY_AUDIO_MSF                    0x47
#define  SLI_USBD_MSC_SCSI_CMD_GET_EVENT_STATUS_NOTIFICATION     0x4A
#define  SLI_USBD_MSC_SCSI_CMD_PAUSE_RESUME                      0x4B
#define  SLI_USBD_MSC_SCSI_CMD_LOG_SELECT                        0x4C
#define  SLI_USBD_MSC_SCSI_CMD_LOG_SENSE                         0x4D
#define  SLI_USBD_MSC_SCSI_CMD_STOP_PLAY_SCAN                    0x4E

#define  SLI_USBD_MSC_SCSI_CMD_XDWRITE_10                        0x50
#define  SLI_USBD_MSC_SCSI_CMD_XPWRITE_10                        0x51
#define  SLI_USBD_MSC_SCSI_CMD_READ_DISC_INFORMATION             0x51
#define  SLI_USBD_MSC_SCSI_CMD_XDREAD_10                         0x52
#define  SLI_USBD_MSC_SCSI_CMD_READ_TRACK_INFORMATION            0x52
#define  SLI_USBD_MSC_SCSI_CMD_RESERVE_TRACK                     0x53
#define  SLI_USBD_MSC_SCSI_CMD_SEND_OPC_INFORMATION              0x54
#define  SLI_USBD_MSC_SCSI_CMD_MODE_SELECT_10                    0x55
#define  SLI_USBD_MSC_SCSI_CMD_RESERVE_10                        0x56
#define  SLI_USBD_MSC_SCSI_CMD_RESERVE_ELEMENT_10                0x56
#define  SLI_USBD_MSC_SCSI_CMD_RELEASE_10                        0x57
#define  SLI_USBD_MSC_SCSI_CMD_RELEASE_ELEMENT_10                0x57
#define  SLI_USBD_MSC_SCSI_CMD_REPAIR_TRACK                      0x58
#define  SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_10                     0x5A
#define  SLI_USBD_MSC_SCSI_CMD_CLOSE_TRACK_SESSION               0x5B
#define  SLI_USBD_MSC_SCSI_CMD_READ_BUFFER_CAPACITY              0x5C
#define  SLI_USBD_MSC_SCSI_CMD_SEND_CUE_SHEET                    0x5D
#define  SLI_USBD_MSC_SCSI_CMD_PERSISTENT_RESERVE_IN             0x5E
#define  SLI_USBD_MSC_SCSI_CMD_PERSISTENT_RESERVE_OUT            0x5F

#define  SLI_USBD_MSC_SCSI_CMD_EXTENDED_CDB                      0x7E
#define  SLI_USBD_MSC_SCSI_CMD_VARIABLE_LENGTH_CDB               0x7F

#define  SLI_USBD_MSC_SCSI_CMD_XDWRITE_EXTENDED_16               0x80
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_FILEMARKS_16                0x80
#define  SLI_USBD_MSC_SCSI_CMD_REBUILD_16                        0x81
#define  SLI_USBD_MSC_SCSI_CMD_READ_REVERSE_16                   0x81
#define  SLI_USBD_MSC_SCSI_CMD_REGENERATE_16                     0x82
#define  SLI_USBD_MSC_SCSI_CMD_EXTENDED_COPY                     0x83
#define  SLI_USBD_MSC_SCSI_CMD_RECEIVE_COPY_RESULTS              0x84
#define  SLI_USBD_MSC_SCSI_CMD_ATA_COMMAND_PASS_THROUGH_16       0x85
#define  SLI_USBD_MSC_SCSI_CMD_ACCESS_CONTROL_IN                 0x86
#define  SLI_USBD_MSC_SCSI_CMD_ACCESS_CONTROL_OUT                0x87
#define  SLI_USBD_MSC_SCSI_CMD_READ_16                           0x88
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_16                          0x8A
#define  SLI_USBD_MSC_SCSI_CMD_ORWRITE                           0x8B
#define  SLI_USBD_MSC_SCSI_CMD_READ_ATTRIBUTE                    0x8C
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_ATTRIBUTE                   0x8D
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_AND_VERIFY_16               0x8E
#define  SLI_USBD_MSC_SCSI_CMD_VERIFY_16                         0x8F

#define  SLI_USBD_MSC_SCSI_CMD_PREFETCH_16                       0x90
#define  SLI_USBD_MSC_SCSI_CMD_SYNCHRONIZE_CACHE_16              0x91
#define  SLI_USBD_MSC_SCSI_CMD_SPACE_16                          0x91
#define  SLI_USBD_MSC_SCSI_CMD_LOCK_UNLOCK_CACHE_16              0x92
#define  SLI_USBD_MSC_SCSI_CMD_LOCATE_16                         0x92
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_SAME_16                     0x93
#define  SLI_USBD_MSC_SCSI_CMD_ERASE_16                          0x93
#define  SLI_USBD_MSC_SCSI_CMD_SERVICE_ACTION_IN_16              0x9E
#define  SLI_USBD_MSC_SCSI_CMD_SERVICE_ACTION_OUT_16             0x9F

#define  SLI_USBD_MSC_SCSI_CMD_REPORT_LUNS                       0xA0
#define  SLI_USBD_MSC_SCSI_CMD_BLANK                             0xA1
#define  SLI_USBD_MSC_SCSI_CMD_ATA_COMMAND_PASS_THROUGH_12       0xA1
#define  SLI_USBD_MSC_SCSI_CMD_SECURITY_PROTOCOL_IN              0xA2
#define  SLI_USBD_MSC_SCSI_CMD_MAINTENANCE_IN                    0xA3
#define  SLI_USBD_MSC_SCSI_CMD_SEND_KEY                          0xA3
#define  SLI_USBD_MSC_SCSI_CMD_MAINTENANCE_OUT                   0xA4
#define  SLI_USBD_MSC_SCSI_CMD_REPORT_KEY                        0xA4
#define  SLI_USBD_MSC_SCSI_CMD_MOVE_MEDIUM                       0xA5
#define  SLI_USBD_MSC_SCSI_CMD_PLAY_AUDIO_12                     0xA5
#define  SLI_USBD_MSC_SCSI_CMD_EXCHANGE_MEDIUM                   0xA6
#define  SLI_USBD_MSC_SCSI_CMD_LOAD_UNLOAD_CDVD                  0xA6
#define  SLI_USBD_MSC_SCSI_CMD_MOVE_MEDIUM_ATTACHED              0xA7
#define  SLI_USBD_MSC_SCSI_CMD_SET_READ_AHEAD                    0xA7
#define  SLI_USBD_MSC_SCSI_CMD_READ_12                           0xA8
#define  SLI_USBD_MSC_SCSI_CMD_GET_MESSAGE_12                    0xA8
#define  SLI_USBD_MSC_SCSI_CMD_SERVICE_ACTION_OUT_12             0xA9
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_12                          0xAA
#define  SLI_USBD_MSC_SCSI_CMD_SEND_MESSAGE_12                   0xAA
#define  SLI_USBD_MSC_SCSI_CMD_SERVICE_ACTION_IN_12              0xAB
#define  SLI_USBD_MSC_SCSI_CMD_ERASE_12                          0xAC
#define  SLI_USBD_MSC_SCSI_CMD_GET_PERFORMANCE                   0xAC
#define  SLI_USBD_MSC_SCSI_CMD_READ_DVD_STRUCTURE                0xAD
#define  SLI_USBD_MSC_SCSI_CMD_WRITE_AND_VERIFY_12               0xAE
#define  SLI_USBD_MSC_SCSI_CMD_VERIFY_12                         0xAF

#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_HIGH_12               0xB0
#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_EQUAL_12              0xB1
#define  SLI_USBD_MSC_SCSI_CMD_SEARCH_DATA_LOW_12                0xB2
#define  SLI_USBD_MSC_SCSI_CMD_SET_LIMITS_12                     0xB3
#define  SLI_USBD_MSC_SCSI_CMD_READ_ELEMENT_STATUS_ATTACHED      0xB4
#define  SLI_USBD_MSC_SCSI_CMD_SECURITY_PROTOCOL_OUT             0xB5
#define  SLI_USBD_MSC_SCSI_CMD_REQUEST_VOLUME_ELEMENT_ADDRESS    0xB5
#define  SLI_USBD_MSC_SCSI_CMD_SEND_VOLUME_TAG                   0xB6
#define  SLI_USBD_MSC_SCSI_CMD_SET_STREAMING                     0xB6
#define  SLI_USBD_MSC_SCSI_CMD_READ_DEFECT_DATA_12               0xB7
#define  SLI_USBD_MSC_SCSI_CMD_READ_ELEMENT_STATUS               0xB8
#define  SLI_USBD_MSC_SCSI_CMD_READ_CD_MSF                       0xB9
#define  SLI_USBD_MSC_SCSI_CMD_REDUNDANCY_GROUP_IN               0xBA
#define  SLI_USBD_MSC_SCSI_CMD_SCAN                              0xBA
#define  SLI_USBD_MSC_SCSI_CMD_REDUNDANCY_GROUP_OUT              0xBB
#define  SLI_USBD_MSC_SCSI_CMD_SET_CD_SPEED                      0xBB
#define  SLI_USBD_MSC_SCSI_CMD_SPARE_IN                          0xBC
#define  SLI_USBD_MSC_SCSI_CMD_SPARE_OUT                         0xBD
#define  SLI_USBD_MSC_SCSI_CMD_MECHANISM_STATUS                  0xBD
#define  SLI_USBD_MSC_SCSI_CMD_VOLUME_SET_IN                     0xBE
#define  SLI_USBD_MSC_SCSI_CMD_READ_CD                           0xBE
#define  SLI_USBD_MSC_SCSI_CMD_VOLUME_SET_OUT                    0xBF
#define  SLI_USBD_MSC_SCSI_CMD_SEND_DVD_STRUCTURE                0xBF

// SCSI Status Codes (See Notes #2)
#define  SLI_USBD_MSC_SCSI_STATUS_GOOD                           0x00
#define  SLI_USBD_MSC_SCSI_STATUS_CHECK_CONDITION                0x02
#define  SLI_USBD_MSC_SCSI_STATUS_CONDITION_MET                  0x04
#define  SLI_USBD_MSC_SCSI_STATUS_BUSY                           0x08
#define  SLI_USBD_MSC_SCSI_STATUS_RESERVATION_CONFLICT           0x18
#define  SLI_USBD_MSC_SCSI_STATUS_TASK_SET_FULL                  0x28
#define  SLI_USBD_MSC_SCSI_STATUS_ACA_ACTIVE                     0x30
#define  SLI_USBD_MSC_SCSI_STATUS_TASK_ABORTED                   0x40

// SCSI Sense Keys (See Notes #3)
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_NO_SENSE                    0x00
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_RECOVERED_ERROR             0x01
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY                     0x02
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_MEDIUM_ERROR                0x03
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_HARDWARE_ERROR              0x04
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST             0x05
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_UNIT_ATTENTION              0x06
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_DATA_PROTECT                0x07
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_BLANK_CHECK                 0x08
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_VENDOR_SPECIFIC             0x09
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_COPY_ABORTED                0x0A
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_ABORTED_COMMAND             0x0B
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_VOLUME_OVERFLOW             0x0D
#define  SLI_USBD_MSC_SCSI_SENSE_KEY_MISCOMPARE                  0x0E

// SCSI Additional Sense Codes (See Notes #4)
#define  SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO          0x00
#define  SLI_USBD_MSC_SCSI_ASC_NO_INDEX_SECTOR_SIGNAL            0x01
#define  SLI_USBD_MSC_SCSI_ASC_NO_SEEK_COMPLETE                  0x02
#define  SLI_USBD_MSC_SCSI_ASC_PERIPHERAL_DEV_WR_FAULT           0x03
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_NOT_RDY                  0x04
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_NOT_RESPOND_TO_SELECTION 0x05
#define  SLI_USBD_MSC_SCSI_ASC_NO_REFERENCE_POSITION_FOUND       0x06
#define  SLI_USBD_MSC_SCSI_ASC_MULTIPLE_PERIPHERAL_DEVS_SELECTED 0x07
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_COMMUNICATION_FAIL       0x08
#define  SLI_USBD_MSC_SCSI_ASC_TRACK_FOLLOWING_ERR               0x09
#define  SLI_USBD_MSC_SCSI_ASC_ERR_LOG_OVERFLOW                  0x0A
#define  SLI_USBD_MSC_SCSI_ASC_WARNING                           0x0B
#define  SLI_USBD_MSC_SCSI_ASC_WR_ERR                            0x0C
#define  SLI_USBD_MSC_SCSI_ASC_ERR_DETECTED_BY_THIRD_PARTY       0x0D
#define  SLI_USBD_MSC_SCSI_ASC_INVALID_INFO_UNIT                 0x0E

#define  SLI_USBD_MSC_SCSI_ASC_ID_CRC_OR_ECC_ERR                 0x10
#define  SLI_USBD_MSC_SCSI_ASC_UNRECOVERED_RD_ERR                0x11
#define  SLI_USBD_MSC_SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_ID        0x12
#define  SLI_USBD_MSC_SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_DATA      0x13
#define  SLI_USBD_MSC_SCSI_ASC_RECORDED_ENTITY_NOT_FOUND         0x14
#define  SLI_USBD_MSC_SCSI_ASC_RANDOM_POSITIONING_ERR            0x15
#define  SLI_USBD_MSC_SCSI_ASC_DATA_SYNCHRONIZATION_MARK_ERR     0x16
#define  SLI_USBD_MSC_SCSI_ASC_RECOVERED_DATA_NO_ERR_CORRECT     0x17
#define  SLI_USBD_MSC_SCSI_ASC_RECOVERED_DATA_ERR_CORRECT        0x18
#define  SLI_USBD_MSC_SCSI_ASC_DEFECT_LIST_ERR                   0x19
#define  SLI_USBD_MSC_SCSI_ASC_PARAMETER_LIST_LENGTH_ERR         0x1A
#define  SLI_USBD_MSC_SCSI_ASC_SYNCHRONOUS_DATA_TRANSFER_ERR     0x1B
#define  SLI_USBD_MSC_SCSI_ASC_DEFECT_LIST_NOT_FOUND             0x1C
#define  SLI_USBD_MSC_SCSI_ASC_MISCOMPARE_DURING_VERIFY_OP       0x1D
#define  SLI_USBD_MSC_SCSI_ASC_RECOVERED_ID_WITH_ECC_CORRECTION  0x1E
#define  SLI_USBD_MSC_SCSI_ASC_PARTIAL_DEFECT_LIST_TRANSFER      0x1F

#define  SLI_USBD_MSC_SCSI_ASC_INVALID_CMD_OP_CODE               0x20
#define  SLI_USBD_MSC_SCSI_ASC_LOG_BLOCK_ADDR_OUT_OF_RANGE       0x21
#define  SLI_USBD_MSC_SCSI_ASC_ILLEGAL_FUNCTION                  0x22
#define  SLI_USBD_MSC_SCSI_ASC_INVALID_FIELD_IN_CDB              0x24
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_NOT_SUPPORTED            0x25
#define  SLI_USBD_MSC_SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST   0x26
#define  SLI_USBD_MSC_SCSI_ASC_WR_PROTECTED                      0x27
#define  SLI_USBD_MSC_SCSI_ASC_NOT_RDY_TO_RDY_CHANGE             0x28
#define  SLI_USBD_MSC_SCSI_ASC_POWER_ON_OR_BUS_DEV_RESET         0x29
#define  SLI_USBD_MSC_SCSI_ASC_PARAMETERS_CHANGED                0x2A
#define  SLI_USBD_MSC_SCSI_ASC_CANNOT_COPY_CANNOT_DISCONNECT     0x2B
#define  SLI_USBD_MSC_SCSI_ASC_CMD_SEQUENCE_ERR                  0x2C
#define  SLI_USBD_MSC_SCSI_ASC_OVERWR_ERR_ON_UPDATE_IN_PLACE     0x2D
#define  SLI_USBD_MSC_SCSI_ASC_INSUFFICIENT_TIME_FOR_OP          0x2E
#define  SLI_USBD_MSC_SCSI_ASC_CMDS_CLEARED_BY_ANOTHER_INIT      0x2F

#define  SLI_USBD_MSC_SCSI_ASC_INCOMPATIBLE_MEDIUM_INSTALLED     0x30
#define  SLI_USBD_MSC_SCSI_ASC_MEDIUM_FORMAT_CORRUPTED           0x31
#define  SLI_USBD_MSC_SCSI_ASC_NO_DEFECT_SPARE_LOCATION_AVAIL    0x32
#define  SLI_USBD_MSC_SCSI_ASC_TAPE_LENGTH_ERR                   0x33
#define  SLI_USBD_MSC_SCSI_ASC_ENCLOSURE_FAIL                    0x34
#define  SLI_USBD_MSC_SCSI_ASC_ENCLOSURE_SERVICES_FAIL           0x35
#define  SLI_USBD_MSC_SCSI_ASC_RIBBON_INK_OR_TONER_FAIL          0x36
#define  SLI_USBD_MSC_SCSI_ASC_ROUNDED_PARAMETER                 0x37
#define  SLI_USBD_MSC_SCSI_ASC_EVENT_STATUS_NOTIFICATION         0x38
#define  SLI_USBD_MSC_SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED   0x39
#define  SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT                0x3A
#define  SLI_USBD_MSC_SCSI_ASC_SEQUENTIAL_POSITIONING_ERR        0x3B
#define  SLI_USBD_MSC_SCSI_ASC_INVALID_BITS_IN_IDENTIFY_MSG      0x3D
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_HAS_NOT_SELF_CFG_YET     0x3E
#define  SLI_USBD_MSC_SCSI_ASC_TARGET_OP_CONDITIONS_HAVE_CHANGED 0x3F

#define  SLI_USBD_MSC_SCSI_ASC_RAM_FAIL                          0x40
#define  SLI_USBD_MSC_SCSI_ASC_DATA_PATH_FAIL                    0x41
#define  SLI_USBD_MSC_SCSI_ASC_POWER_ON_SELF_TEST_FAIL           0x42
#define  SLI_USBD_MSC_SCSI_ASC_MSG_ERR                           0x43
#define  SLI_USBD_MSC_SCSI_ASC_INTERNAL_TARGET_FAIL              0x44
#define  SLI_USBD_MSC_SCSI_ASC_SELECT_OR_RESELECT_FAIL           0x45
#define  SLI_USBD_MSC_SCSI_ASC_UNSUCCESSFUL_SOFT_RESET           0x46
#define  SLI_USBD_MSC_SCSI_ASC_SCSI_PARITY_ERR                   0x47
#define  SLI_USBD_MSC_SCSI_ASC_INIT_DETECTED_ERR_MSG_RECEIVED    0x48
#define  SLI_USBD_MSC_SCSI_ASC_INVALID_MSG_ERR                   0x49
#define  SLI_USBD_MSC_SCSI_ASC_CMD_PHASE_ERR                     0x4A
#define  SLI_USBD_MSC_SCSI_ASC_DATA_PHASE_ERR                    0x4B
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_FAILED_SELF_CFG          0x4C
#define  SLI_USBD_MSC_SCSI_ASC_OVERLAPPED_CMDS_ATTEMPTED         0x4E

#define  SLI_USBD_MSC_SCSI_ASC_WR_APPEND_ERR                     0x50
#define  SLI_USBD_MSC_SCSI_ASC_ERASE_FAIL                        0x51
#define  SLI_USBD_MSC_SCSI_ASC_CARTRIDGE_FAULT                   0x52
#define  SLI_USBD_MSC_SCSI_ASC_MEDIA_LOAD_OR_EJECT_FAILED        0x53
#define  SLI_USBD_MSC_SCSI_ASC_SCSI_TO_HOST_SYSTEM_IF_FAIL       0x54
#define  SLI_USBD_MSC_SCSI_ASC_SYSTEM_RESOURCE_FAIL              0x55
#define  SLI_USBD_MSC_SCSI_ASC_UNABLE_TO_RECOVER_TOC             0x57
#define  SLI_USBD_MSC_SCSI_ASC_GENERATION_DOES_NOT_EXIST         0x58
#define  SLI_USBD_MSC_SCSI_ASC_UPDATED_BLOCK_RD                  0x59
#define  SLI_USBD_MSC_SCSI_ASC_OP_REQUEST_OR_STATE_CHANGE_INPUT  0x5A
#define  SLI_USBD_MSC_SCSI_ASC_LOG_EXCEPT                        0x5B
#define  SLI_USBD_MSC_SCSI_ASC_RPL_STATUS_CHANGE                 0x5C
#define  SLI_USBD_MSC_SCSI_ASC_FAIL_PREDICTION_TH_EXCEEDED       0x5D
#define  SLI_USBD_MSC_SCSI_ASC_LOW_POWER_CONDITION_ON            0x5E

#define  SLI_USBD_MSC_SCSI_ASC_LAMP_FAIL                         0x60
#define  SLI_USBD_MSC_SCSI_ASC_VIDEO_ACQUISITION_ERR             0x61
#define  SLI_USBD_MSC_SCSI_ASC_SCAN_HEAD_POSITIONING_ERR         0x62
#define  SLI_USBD_MSC_SCSI_ASC_END_OF_USER_AREA_ENCOUNTERED      0x63
#define  SLI_USBD_MSC_SCSI_ASC_ILLEGAL_MODE_FOR_THIS_TRACK       0x64
#define  SLI_USBD_MSC_SCSI_ASC_VOLTAGE_FAULT                     0x65
#define  SLI_USBD_MSC_SCSI_ASC_AUTO_DOCUMENT_FEEDER_COVER_UP     0x66
#define  SLI_USBD_MSC_SCSI_ASC_CONFIGURATION_FAIL                0x67
#define  SLI_USBD_MSC_SCSI_ASC_LOG_UNIT_NOT_CONFIGURED           0x68
#define  SLI_USBD_MSC_SCSI_ASC_DATA_LOSS_ON_LOG_UNIT             0x69
#define  SLI_USBD_MSC_SCSI_ASC_INFORMATIONAL_REFER_TO_LOG        0x6A
#define  SLI_USBD_MSC_SCSI_ASC_STATE_CHANGE_HAS_OCCURRED         0x6B
#define  SLI_USBD_MSC_SCSI_ASC_REBUILD_FAIL_OCCURRED             0x6C
#define  SLI_USBD_MSC_SCSI_ASC_RECALCULATE_FAIL_OCCURRED         0x6D
#define  SLI_USBD_MSC_SCSI_ASC_CMD_TO_LOG_UNIT_FAILED            0x6E
#define  SLI_USBD_MSC_SCSI_ASC_COPY_PROTECTION_KEY_EXCHANGE_FAIL 0x6F
#define  SLI_USBD_MSC_SCSI_ASC_DECOMPRESSION_EXCEPT_LONG_ALGO_ID 0x71
#define  SLI_USBD_MSC_SCSI_ASC_SESSION_FIXATION_ERR              0x72
#define  SLI_USBD_MSC_SCSI_ASC_CD_CONTROL_ERR                    0x73
#define  SLI_USBD_MSC_SCSI_ASC_SECURITY_ERR                      0x74

// SCSI Mode Page Codes (See Notes #5)
#define SLI_USBD_MSC_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY    0x01
#define SLI_USBD_MSC_SCSI_PAGE_CODE_FORMAT_DEVICE                0x03
#define SLI_USBD_MSC_SCSI_PAGE_CODE_FLEXIBLE_DISK                0x05
#define SLI_USBD_MSC_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS     0x1C
#define SLI_USBD_MSC_SCSI_PAGE_CODE_ALL                          0x3F

#define SLI_USBD_MSC_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS   0x0A
#define SLI_USBD_MSC_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY  0x0A
#define SLI_USBD_MSC_SCSI_PAGE_LENGTH_FLEXIBLE_DISK              0x1E
#define SLI_USBD_MSC_SCSI_PAGE_LENGTH_FORMAT_DEVICE              0x16

/********************************************************************************************************
 *                                               INQUIRY CMD
 *
 * Note(s) : (1) The peripheral qualifiers are specified in 'SCSI Primary Commands - 3' (SPC-3),
 *               Revision 23, Section 6.4.2, Table 82 - Peripheral Qualifer.
 *
 *           (2) The peripheral device types are specified in 'SCSI Primary Commands - 3' (SPC-3),
 *               Revision 23, Section 6.4.2, Table 83 - Peripheral Device Type.
 *******************************************************************************************************/

// Peripheral Qualifiers
#define  SLI_USBD_MSC_SCSI_PER_QUAL_CONN                        0x00
#define  SLI_USBD_MSC_SCSI_PER_QUAL_NOT_CONN                    0x01
#define  SLI_USBD_MSC_SCSI_PER_QUAL_NOT_SUPPORTED               0x03
// Peripheral Device Types
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_DIRECT_ACCESS_BLOCK_DEV 0x00
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_SEQ_ACCESS_DEV          0x01
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_PRINTER_DEV             0x02
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_PROCESSOR_DEV           0x03
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_WR_ONCE_DEV             0x04
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_CD_DVD_DEV              0x05
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_OPTICAL_MEM_DEV         0x07
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_MEDIUM_CHANGER_DEV      0x08
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_STORAGE_ARRAY_CTRL_DEV  0x0C
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_ENCLOSURE_SVC_DEV       0x0D
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_SIMPL_DIRECT_ACCESS_DEV 0x0E
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_OPTICAL_CARD_RD_WR_DEV  0x0F
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_BRIDGE_CTRL_CMDS        0x10
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_OBJ_BASED_STORAGE_DEV   0x11
#define  SLI_USBD_MSC_SCSI_PER_DEV_TYPE_AUTOMATION_DRIVE_IF     0x12

#define  SLI_USBD_MSC_SCSI_INQUIRY_RMB                          0x80u

/********************************************************************************************************
 *                                           START STOP UNIT CMD
 *******************************************************************************************************/

#define  SLI_USBD_MSC_SCSI_START_STOP_UNIT_START            0x01u
#define  SLI_USBD_MSC_SCSI_START_STOP_UNIT_LOEJ             0x02u

/********************************************************************************************************
 *                                       MODE SENSE CMD AND DATA
 *******************************************************************************************************/

#define  SLI_USBD_MSC_SCSI_MSK_PAGE_CODE                        0x3F
// Mode Sense Data
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_EN     0x00
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_PROT   0x80

#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_6_LEN    3u
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_10_LEN   6u
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN       2u
// Info Exceptions Ctrl Page Param
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MRIE_NO_SENSE        0x05
// Read/Write Err Recovery Param
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_AWRE                 0x80
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_RD_RETRY_CNT            3u
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_WR_RETRY_CNT            3u
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_CORRECTION_SPAN      0x00
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_HEAD_OFFSET_CNT      0x00
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_DATA_STROBE_OFFSET   0x00
#define  SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_RECOVERY_LIMIT       0x00

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/// SCSI Module Root Structure
typedef struct sli_usbd_msc_scsi {
  sl_usbd_msc_scsi_lun_t lun_table[SL_USBD_MSC_CONFIGURATION_QUANTITY * SL_USBD_MSC_LUN_QUANTITY];  ///< Pointer to table of logical units.
  uint8_t                 next_lun_nbr;                                                             ///< Index of next logical unit to allocate.
} sli_usbd_msc_scsi_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static sli_usbd_msc_scsi_t usbd_msc_scsi_obj = { 0 };

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static sl_status_t usbd_msc_scsi_process_command(uint8_t        class_nbr,
                                                 void           *p_lun_data,
                                                 const uint8_t  *p_cbwcb,
                                                 uint8_t        *p_data_buf,
                                                 uint32_t       *p_resp_len,
                                                 uint8_t        *p_data_dir,
                                                 void           *p_subclass_arg);

static sl_status_t usbd_msc_scsi_read_data(uint8_t   class_nbr,
                                           void      *p_lun_data,
                                           uint8_t   command,
                                           uint8_t   *p_data_buf,
                                           uint32_t  data_len,
                                           void      *p_subclass_arg);

static sl_status_t usbd_msc_scsi_write_data(uint8_t   class_nbr,
                                            void      *p_lun_data,
                                            uint8_t   command,
                                            void      *p_data_buf,
                                            uint32_t  data_len,
                                            void      *p_subclass_arg);

static sl_status_t usbd_msc_scsi_enable(uint8_t   class_nbr,
                                        void      *p_subclass_arg);

static sl_status_t usbd_msc_scsi_disable(uint8_t   class_nbr,
                                         void      *p_subclass_arg);

static void usbd_msc_scsi_prepare_mode_sense_data(const sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                                  uint8_t                      *p_data_buf,
                                                  uint8_t                      scsi_cmd,
                                                  uint8_t                      page_code);

static void usbd_msc_scsi_update_sense_data(sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                            uint8_t                sense_key,
                                            uint8_t                sense_code);

static void usbd_msc_scsi_prepare_inquiry_data(const sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                               uint8_t                      *p_resp_buf);

static void usbd_msc_scsi_page_rw_error_recovery(void *p_buf_dest);

static void usbd_msc_scsi_page_info_except(void *p_buf_dest);

/********************************************************************************************************
 *                                          MSC SCSI CLASS DRIVER
 *******************************************************************************************************/

static sl_usbd_msc_subclass_driver_t usbd_msc_scsi_driver = {
  usbd_msc_scsi_enable,
  usbd_msc_scsi_disable,
  usbd_msc_scsi_process_command,
  usbd_msc_scsi_read_data,
  usbd_msc_scsi_write_data
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initializes MSC SCSI class
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_init(void)
{
  uint8_t ix;

  usbd_msc_scsi_obj.next_lun_nbr = SL_USBD_MSC_CONFIGURATION_QUANTITY * SL_USBD_MSC_LUN_QUANTITY;

  for (ix = 0u; ix < (SL_USBD_MSC_CONFIGURATION_QUANTITY * SL_USBD_MSC_LUN_QUANTITY); ix++) {
    memset((void *)&usbd_msc_scsi_obj.lun_table[ix], 0u, sizeof(sl_usbd_msc_scsi_lun_t));
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a new instance of SCSI Mass Storage Class
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_create_instance(uint32_t                      msc_task_stack_size,
                                             osPriority_t                  msc_task_priority,
                                             sl_usbd_msc_scsi_callbacks_t *p_scsi_callbacks,
                                             uint8_t                      *p_class_nbr)
{
  sl_status_t status;

  if (p_class_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  status = sl_usbd_msc_create_instance(SL_USBD_MSC_SUBCLASS_CODE_SCSI,
                                       msc_task_stack_size,
                                       msc_task_priority,
                                       &usbd_msc_scsi_driver,
                                       (void *)p_scsi_callbacks,
                                       p_class_nbr);

  return status;
}

/****************************************************************************************************//**
 * Adds an existing MSC SCSI instance to the specified configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_add_to_configuration(uint8_t  class_nbr,
                                                  uint8_t  config_nbr)
{
  sl_status_t status;

  status = sl_usbd_msc_add_to_configuration(class_nbr, config_nbr);

  return status;
}

/****************************************************************************************************//**
 * Initialize the specified logical unit
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_add(uint8_t                           class_nbr,
                                     sl_usbd_msc_scsi_lun_info_t       *p_lu_info,
                                     sl_usbd_msc_scsi_lun_t            **p_scsi_lun_ptr)
{
  size_t            str_len;
  uint8_t            lun_ix;
  sl_usbd_msc_scsi_lun_t *p_scsi_lun;
  sl_status_t           status;
  CORE_DECLARE_IRQ_STATE;

  if (p_scsi_lun_ptr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_lu_info == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_lu_info->scsi_lun_api_ptr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_lu_info->vendor_id_ptr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_lu_info->product_id_ptr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  str_len = sl_strlen((char *)p_lu_info->vendor_id_ptr);

  if (str_len > SLI_USBD_MSC_SCSI_DEV_MAX_VEND_ID_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  str_len = sl_strlen((char *)p_lu_info->product_id_ptr);

  if (str_len > SLI_USBD_MSC_SCSI_DEV_MAX_PROD_ID_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_msc_scsi_obj.next_lun_nbr == 0u) {
    CORE_EXIT_ATOMIC();

    *p_scsi_lun_ptr = NULL;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  lun_ix = usbd_msc_scsi_obj.next_lun_nbr - 1u;
  usbd_msc_scsi_obj.next_lun_nbr--;
  CORE_EXIT_ATOMIC();

  p_scsi_lun = &usbd_msc_scsi_obj.lun_table[lun_ix];

  p_scsi_lun->lun_index = lun_ix;
  p_scsi_lun->scsi_lun_api_ptr = p_lu_info->scsi_lun_api_ptr;
  p_scsi_lun->lb_addr = 0u;
  p_scsi_lun->lb_count = 0u;
  p_scsi_lun->sense_key = 0u;
  p_scsi_lun->asc = 0u;
  p_scsi_lun->vendor_id_str = p_lu_info->vendor_id_ptr;
  p_scsi_lun->product_id_str = p_lu_info->product_id_ptr;
  p_scsi_lun->product_revision_level = p_lu_info->product_revision_level;
  p_scsi_lun->is_read_only = p_lu_info->is_read_only;
  p_scsi_lun->opened_flag = false;
  p_scsi_lun->present_flag = false;

  status = sl_usbd_msc_lun_add(class_nbr, p_scsi_lun, &(p_scsi_lun->lu_nbr));

  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  status = p_lu_info->scsi_lun_api_ptr->init(p_scsi_lun);

  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  *p_scsi_lun_ptr = p_scsi_lun;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Attach a storage medium to the given SCSI logical unit
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_attach(sl_usbd_msc_scsi_lun_t *p_scsi_lun)
{
  sl_status_t status;

  status = sli_usbd_msc_os_acquire_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_scsi_lun->opened_flag == true) {
    sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);
    return SL_STATUS_INVALID_STATE;
  }

  status = p_scsi_lun->scsi_lun_api_ptr->open(p_scsi_lun);

  if (status != SL_STATUS_OK) {
    sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);
    return status;
  }

  p_scsi_lun->opened_flag = true;
  p_scsi_lun->present_flag = true;

  status = sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Detach a storage medium from the given SCSI logical unit
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_detach(sl_usbd_msc_scsi_lun_t *p_scsi_lun)
{
  sl_status_t status;

  status = sli_usbd_msc_os_acquire_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  status = p_scsi_lun->scsi_lun_api_ptr->close(p_scsi_lun);

  if (status != SL_STATUS_OK) {
    sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);
    return status;
  }

  p_scsi_lun->opened_flag = false;
  p_scsi_lun->present_flag = false;

  status = sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Retrieves capacity of logical unit
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_get_capacity(sl_usbd_msc_scsi_lun_t     *p_scsi_lun,
                                              sl_usbd_msc_scsi_lb_qty_t  *p_lb_qty,
                                              uint32_t                   *p_blk_size)
{
  sl_status_t status;

  status = sli_usbd_msc_os_acquire_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((p_scsi_lun->opened_flag != true) || (p_scsi_lun->present_flag != true)) {
    sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);
    return SL_STATUS_INVALID_STATE;
  }

  // Get capacity, nbr of blks and blk size.
  status = p_scsi_lun->scsi_lun_api_ptr->get_capacity(p_scsi_lun,
                                                      p_lb_qty,
                                                      p_blk_size);
  if (status != SL_STATUS_OK) {
    sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);
    return status;
  }

  status = sli_usbd_msc_os_release_lun_lock(p_scsi_lun->lun_index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the MSC enable state of the SCSI interface
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_is_enable(uint8_t  class_nbr,
                                       bool     *p_enabled)
{
  sl_status_t status;

  if (p_enabled == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  status = sl_usbd_msc_is_enabled(class_nbr, p_enabled);

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           usbd_msc_scsi_process_command()
 *
 * @brief    Process operation(s) associated with the SCSI command.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_lun           Pointer to Logical Unit information.
 *
 * @param    p_cbwcb         Pointer to the Command Block Wrapper that contains the SCSI command.
 *
 * @param    p_data_buf      Pointer to data buffer to use as a response buffer.
 *
 * @param    p_resp_len      Pointer to variable that will receive the length of the SCSI response
 *                           that needs to be transfered in the data stage.
 *
 * @param    p_data_dir      Pointer to variable that will receive the data transfer direction
 *                           indicator:
 *                             - SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST     Data stage response from device to host.
 *                             - SLI_USBD_MSC_SCSI_CBW_HOST_TO_DEVICE     Data stage response from host to device.
 *
 * @param    p_subclass_arg  Pointer to SCSI callback structure.
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
static sl_status_t usbd_msc_scsi_process_command(uint8_t        class_nbr,
                                                 void           *p_lun_data,
                                                 const uint8_t  *p_cbwcb,
                                                 uint8_t        *p_data_buf,
                                                 uint32_t       *p_resp_len,
                                                 uint8_t        *p_data_dir,
                                                 void           *p_subclass_arg)
{
  bool               result = true;
  uint8_t            scsi_cmd = p_cbwcb[0u];
  uint8_t            page_code;
  uint8_t            cmdt_evpd;
  uint8_t            loej;
  uint8_t            start_flag;
  sl_usbd_msc_scsi_lb_qty_t     nbr_blks;
  sl_usbd_msc_scsi_lun_t       *p_storage_lun = (sl_usbd_msc_scsi_lun_t *)p_lun_data;
  sl_usbd_msc_scsi_lun_api_t   *p_scsi_storage_api = p_storage_lun->scsi_lun_api_ptr;
  sl_usbd_msc_scsi_callbacks_t *p_scsi_callbacks = (sl_usbd_msc_scsi_callbacks_t *) p_subclass_arg;
  sl_status_t        status;
  CORE_DECLARE_IRQ_STATE;

  *p_resp_len = 0u;

  // Reset req sense data with no err cond.
  if (scsi_cmd != SLI_USBD_MSC_SCSI_CMD_REQUEST_SENSE) {
    usbd_msc_scsi_update_sense_data(p_storage_lun,
                                    SLI_USBD_MSC_SCSI_SENSE_KEY_NO_SENSE,
                                    SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
  }

  switch (scsi_cmd) {
    case SLI_USBD_MSC_SCSI_CMD_INQUIRY:
      // Inquiry (see Notes #1)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: INQUIRY Command"));

      // Get the enable vital prod data bit.
      cmdt_evpd = p_cbwcb[1u] & 0x03u;
      // Page code for vital prod data info.
      page_code = p_cbwcb[2u];

      if ((cmdt_evpd == SLI_USBD_MSC_SCSI_STD_INQUIRY_DATA)
          && (page_code == 0u)) {
        usbd_msc_scsi_prepare_inquiry_data(p_storage_lun,
                                           p_data_buf);

        *p_resp_len = SLI_USBD_GET_MIN(SLI_USBD_MSC_SCSI_INQUIRY_DATA_LEN, p_cbwcb[4u]);
        *p_data_dir = SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST;
      } else {
        // Unsupported request.
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                        SLI_USBD_MSC_SCSI_ASC_INVALID_FIELD_IN_CDB);
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_TEST_UNIT_READY:
      // Test Unit Ready (see Notes #2)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: TEST UNIT READY Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->present_flag != true) {
        CORE_EXIT_ATOMIC();

        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY,
                                        SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT);

        result = false;
        break;
      } else {
        CORE_EXIT_ATOMIC();
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_SERVICE_ACTION_IN_16:
      // Read Capacity(16)(see Notes #4)
#if (SL_USBD_MSC_SCSI_64_BIT_LBA_EN == 0)
      result = false;
      usbd_msc_scsi_update_sense_data(p_storage_lun,
                                      SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case SLI_USBD_MSC_SCSI_CMD_READ_CAPACITY_10:
      // Read Capacity(10)(see Notes #3)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: READ CAPACITY 10 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->present_flag == true) {
        CORE_EXIT_ATOMIC();

        // Get capacity, nbr of blks and blk size.
        status = p_scsi_storage_api->get_capacity(p_storage_lun,
                                                  &p_storage_lun->blocks_nbr,
                                                  &p_storage_lun->block_size);
        if (status != SL_STATUS_OK) {
          usbd_msc_scsi_update_sense_data(p_storage_lun,
                                          SLI_USBD_MSC_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                          SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
          result = false;
          break;
        } else {
          result = true;
        }

        if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_READ_CAPACITY_10) {
          SLI_USBD_MEM_VAL_SET_INT32U_BIG(&p_data_buf[0u], p_storage_lun->blocks_nbr - 1u);
          SLI_USBD_MEM_VAL_SET_INT32U_BIG(&p_data_buf[4u], p_storage_lun->block_size);

          *p_resp_len = SLI_USBD_MSC_SCSI_RD_CAPACITY_10_PARAM_DATA_LEN;
        } else {
          nbr_blks = p_storage_lun->blocks_nbr - 1u;
          SLI_USBD_MEM_VAL_COPY_SET_INTU_BIG(&p_data_buf[0u], &nbr_blks, 8u);

          memset((void *)&p_data_buf[8u], 0u, 8u);

          SLI_USBD_MEM_VAL_SET_INT32U_BIG(&p_data_buf[8u], p_storage_lun->block_size);

          *p_resp_len = SLI_USBD_MSC_SCSI_RD_CAPACITY_16_PARAM_DATA_LEN;
        }

        *p_data_dir = SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST;
      } else {
        CORE_EXIT_ATOMIC();

        result = false;
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY,
                                        SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_READ_16:
      // Read(16)(see Notes #7)
#if (SL_USBD_MSC_SCSI_64_BIT_LBA_EN == 0)
      result = false;
      usbd_msc_scsi_update_sense_data(p_storage_lun,
                                      SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case SLI_USBD_MSC_SCSI_CMD_READ_12:
    // Read(12)(see Notes #6)
    case SLI_USBD_MSC_SCSI_CMD_READ_10:
      // Read(10)(see Notes #5)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: READ 10 / 12 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->present_flag == true) {
        CORE_EXIT_ATOMIC();

        if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_READ_10) {
          p_storage_lun->lb_addr = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT16U_BIG(&p_cbwcb[7u]);
        } else if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_READ_12) {
          p_storage_lun->lb_addr = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[6u]);
        } else {
          SLI_USBD_MEM_VAL_COPY_GET_INTU_BIG(&p_storage_lun->lb_addr, &p_cbwcb[2u], 8u);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[10u]);
        }

        *p_resp_len = p_storage_lun->lb_count * p_storage_lun->block_size;
        *p_data_dir = SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST;
      } else {
        CORE_EXIT_ATOMIC();

        result = false;
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY,
                                        SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_WRITE_16:
      // Write(16)(see Notes #10)
#if (SL_USBD_MSC_SCSI_64_BIT_LBA_EN == 0)
      result = false;
      usbd_msc_scsi_update_sense_data(p_storage_lun,
                                      SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
#endif

    case SLI_USBD_MSC_SCSI_CMD_WRITE_10:
    // Write(10)(see Notes #8)
    case SLI_USBD_MSC_SCSI_CMD_WRITE_12:
      // Write(12)(see Notes #9)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: WRITE 10 / 12 / 16 Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->present_flag == true) {
        CORE_EXIT_ATOMIC();

        // Check medium is wr protected or not.
        if (p_storage_lun->is_read_only == true) {
          usbd_msc_scsi_update_sense_data(p_storage_lun,
                                          SLI_USBD_MSC_SCSI_SENSE_KEY_DATA_PROTECT,
                                          SLI_USBD_MSC_SCSI_ASC_WR_PROTECTED);
          result = false;
          break;
        }

        if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_WRITE_10) {
          p_storage_lun->lb_addr = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT16U_BIG(&p_cbwcb[7u]);
        } else if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_WRITE_12) {
          p_storage_lun->lb_addr = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[2u]);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[6u]);
        } else {
          SLI_USBD_MEM_VAL_COPY_GET_INTU_BIG(&p_storage_lun->lb_addr, &p_cbwcb[2u], 8u);
          p_storage_lun->lb_count = SLI_USBD_MEM_VAL_GET_INT32U_BIG(&p_cbwcb[10u]);
        }

        *p_resp_len = p_storage_lun->lb_count * (p_storage_lun->block_size);
        *p_data_dir = SLI_USBD_MSC_SCSI_CBW_HOST_TO_DEVICE;
      } else {
        CORE_EXIT_ATOMIC();

        result = false;
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY,
                                        SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_06:
    // Mode Sense(6) (see Notes #14)
    case SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_10:
      // Mode Sense(10) (see Notes #15)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: MODE SENSE Command"));

      CORE_ENTER_ATOMIC();
      if (p_storage_lun->present_flag == true) {
        CORE_EXIT_ATOMIC();

        // Get page code.
        page_code = p_cbwcb[2] & SLI_USBD_MSC_SCSI_MSK_PAGE_CODE;

        switch (page_code) {
          case SLI_USBD_MSC_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS:
          case SLI_USBD_MSC_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY:
          case SLI_USBD_MSC_SCSI_PAGE_CODE_ALL:
            usbd_msc_scsi_prepare_mode_sense_data(p_storage_lun,
                                                  p_data_buf,
                                                  scsi_cmd,
                                                  page_code);

            if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_06) {
              *p_resp_len = SLI_USBD_GET_MIN(p_data_buf[0u] + 1u, p_cbwcb[4u]);
            } else {
              *p_resp_len = (p_cbwcb[7] << 8u) | p_cbwcb[8];
              *p_resp_len = SLI_USBD_GET_MIN(((uint32_t)p_data_buf[0u] + 1u), *p_resp_len);
            }

            *p_data_dir = SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST;
            break;

          default:
            // Unsupported page code.
            usbd_msc_scsi_update_sense_data(p_storage_lun,
                                            SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                            SLI_USBD_MSC_SCSI_ASC_INVALID_FIELD_IN_CDB);
            result = false;
            break;
        }
      } else {
        CORE_EXIT_ATOMIC();

        result = false;
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_NOT_RDY,
                                        SLI_USBD_MSC_SCSI_ASC_MEDIUM_NOT_PRESENT);
      }
      break;

    case SLI_USBD_MSC_SCSI_CMD_REQUEST_SENSE:
      // Request Sense(see Notes #16)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: REQUEST SENSE Command"));
      *p_resp_len = SLI_USBD_GET_MIN(SLI_USBD_MSC_SCSI_REQ_SENSE_DATA_LEN, p_cbwcb[4u]);

      memset((void *)p_data_buf, 0u, *p_resp_len);

      p_data_buf[0u] = SLI_USBD_MSC_SCSI_REQ_SENSE_RESP_CODE_CUR_ERR;
      p_data_buf[2u] = p_storage_lun->sense_key;
      p_data_buf[7u] = (SLI_USBD_MSC_SCSI_REQ_SENSE_DATA_LEN - 8u);
      p_data_buf[12u] = p_storage_lun->asc;
      p_data_buf[13u] = 0u;

      *p_data_dir = SLI_USBD_MSC_SCSI_CBW_DEVICE_TO_HOST;
      break;

    case SLI_USBD_MSC_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
      // Prevent Allow Medium Removal (see Notes #17)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: PREVENT ALLOW MEDIUM REMOVAL Command"));
      break;

    case SLI_USBD_MSC_SCSI_CMD_START_STOP_UNIT:
      // Start Stop Init(see Notes #18)
      SLI_USBD_LOG_VRB(("USBD MSC SCSI: START STOP UNIT Command"));

      loej = p_cbwcb[4u] & SLI_USBD_MSC_SCSI_START_STOP_UNIT_LOEJ;
      start_flag = p_cbwcb[4u] & SLI_USBD_MSC_SCSI_START_STOP_UNIT_START;

      // Eject the medium.
      if ((SL_IS_BIT_SET(loej, SLI_USBD_MSC_SCSI_START_STOP_UNIT_LOEJ) == true)
          && (SL_IS_BIT_CLEAR(start_flag, SLI_USBD_MSC_SCSI_START_STOP_UNIT_START) == true)) {
        CORE_ENTER_ATOMIC();
        p_storage_lun->present_flag = false;
        CORE_EXIT_ATOMIC();

        if (p_scsi_callbacks != NULL && p_scsi_callbacks->host_eject != NULL) {
          p_scsi_callbacks->host_eject(class_nbr, p_storage_lun->lu_nbr);
        }
      } else {
        result = false;
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                        SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:
      // Cmd not supported.
      SLI_USBD_LOG_ERR(("USBD MSC SCSI: UNSUPPORTED Command"));

      result = false;
      usbd_msc_scsi_update_sense_data(p_storage_lun,
                                      SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      break;
  }

  if (result) {
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}

/****************************************************************************************************//**
 *                                           usbd_msc_scsi_read_data()
 *
 * @brief    Read data from the SCSI device OR copy response data to buffer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_lun_data      Pointer to Logical Unit data.
 *
 * @param    command         SCSI command operation code.
 *
 * @param    p_data_buf      Pointer to receive buffer.
 *
 * @param    data_len        Number of bytes to read.
 *
 * @param    p_subclass_arg  Pointer to SCSI callback structure.
 *
 * @note     (1) SCSI commands that require a Data IN phase are: INQUIRY, READ CAPACITY, MODE SENSE,
 *               REQUEST SENSE and READ. For all these SCSI commands except READ, the buffer containing
 *               data for the host is prepared upfront during the cbw processing done in
 *               usbd_msc_scsi_process_command().
 *******************************************************************************************************/
static sl_status_t usbd_msc_scsi_read_data(uint8_t   class_nbr,
                                           void      *p_lun_data,
                                           uint8_t   command,
                                           uint8_t   *p_data_buf,
                                           uint32_t  data_len,
                                           void      *p_subclass_arg)
{
  sl_status_t         status = SL_STATUS_OK;
  uint32_t            lb_cnt;
  sl_usbd_msc_scsi_lun_t *p_storage_lun = (sl_usbd_msc_scsi_lun_t *)p_lun_data;
  sl_usbd_msc_scsi_lun_api_t *p_scsi_storage_api = p_storage_lun->scsi_lun_api_ptr;

  (void) &class_nbr;
  (void) &p_subclass_arg;

  switch (command) {
    case SLI_USBD_MSC_SCSI_CMD_READ_10:
    case SLI_USBD_MSC_SCSI_CMD_READ_12:
    case SLI_USBD_MSC_SCSI_CMD_READ_16:
      SLI_USBD_LOG_VRB(("USBD MSC SCSI Read data from Disk."));
      // Nbr of blks that can fit in scsi_data_buf.
      lb_cnt = data_len / p_storage_lun->block_size;

      status = p_scsi_storage_api->read(p_storage_lun,
                                        p_storage_lun->lb_addr,
                                        lb_cnt,
                                        p_data_buf);
      if (status == SL_STATUS_OK) {
        p_storage_lun->lb_addr += lb_cnt;
        p_storage_lun->lb_count -= lb_cnt;
      } else {
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                        SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:
      // See Note #1.
      break;
  }

  return status;
}

/****************************************************************************************************//**
 *                                           usbd_msc_scsi_write_data()
 *
 * @brief    Write data to the SCSI device.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_lun_data      Pointer to Logical Unit data.
 *
 * @param    command         SCSI command operation code.
 *
 * @param    p_data_buf      Pointer to transmit buffer.
 *
 * @param    p_subclass_arg  Pointer to SCSI callback structure.
 *
 * @param    data_len        Number of bytes to write.
 *******************************************************************************************************/
static sl_status_t usbd_msc_scsi_write_data(uint8_t   class_nbr,
                                            void      *p_lun_data,
                                            uint8_t   command,
                                            void      *p_data_buf,
                                            uint32_t  data_len,
                                            void      *p_subclass_arg)
{
  sl_status_t         status = SL_STATUS_OK;
  uint32_t            lb_cnt;
  sl_usbd_msc_scsi_lun_t *p_storage_lun = (sl_usbd_msc_scsi_lun_t *)p_lun_data;
  sl_usbd_msc_scsi_lun_api_t *p_scsi_storage_api = p_storage_lun->scsi_lun_api_ptr;

  (void) &class_nbr;
  (void) &p_subclass_arg;

  switch (command) {
    case SLI_USBD_MSC_SCSI_CMD_WRITE_10:
    case SLI_USBD_MSC_SCSI_CMD_WRITE_12:
    case SLI_USBD_MSC_SCSI_CMD_WRITE_16:
      SLI_USBD_LOG_VRB(("USBD MSC SCSI Write data to Disk."));
      // Nbr of blks present in scsi_data_buf.
      lb_cnt = data_len / (p_storage_lun->block_size);

      status = p_scsi_storage_api->write(p_storage_lun,
                                         p_storage_lun->lb_addr,
                                         lb_cnt,
                                         (uint8_t *)p_data_buf);
      if (status == SL_STATUS_OK) {
        p_storage_lun->lb_addr += lb_cnt;
        p_storage_lun->lb_count -= lb_cnt;
      } else {
        usbd_msc_scsi_update_sense_data(p_storage_lun,
                                        SLI_USBD_MSC_SCSI_SENSE_KEY_HARDWARE_ERROR,
                                        SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      }
      break;

    default:
      usbd_msc_scsi_update_sense_data(p_storage_lun,
                                      SLI_USBD_MSC_SCSI_SENSE_KEY_ILLEGAL_REQUEST,
                                      SLI_USBD_MSC_SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
      status = SL_STATUS_FAIL;
      break;
  }

  return status;
}

/****************************************************************************************************//**
 *                                               usbd_msc_scsi_enable()
 *
 * @brief    Reset eject flag upon new configuration activation.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *******************************************************************************************************/
static sl_status_t usbd_msc_scsi_enable(uint8_t   class_nbr,
                                        void      *p_subclass_arg)
{
  sl_usbd_msc_scsi_callbacks_t *p_scsi_callbacks = (sl_usbd_msc_scsi_callbacks_t *) p_subclass_arg;

  if (p_scsi_callbacks != NULL && p_scsi_callbacks->enable != NULL) {
    p_scsi_callbacks->enable(class_nbr);
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                           usbd_msc_scsi_disable()
 *
 * @brief    close the SCSI Storage Medium.
 *
 * @param    p_lun_data  Pointer to Logical Unit data.
 *
 * @note     (1) Unlocking a logical unit can be done with a software eject (for instance Windows
 *               right's click eject). In that case, the unlock operation must not be executed another
 *               time. If a software eject has occurred, the unlock operation done upon physical
 *               disconnection of the device must be discarded.
 *******************************************************************************************************/
static sl_status_t usbd_msc_scsi_disable(uint8_t   class_nbr,
                                         void      *p_subclass_arg)
{
  sl_usbd_msc_scsi_callbacks_t *p_scsi_callbacks = (sl_usbd_msc_scsi_callbacks_t *) p_subclass_arg;

  if (p_scsi_callbacks != NULL && p_scsi_callbacks->disable != NULL) {
    p_scsi_callbacks->disable(class_nbr);
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                       usbd_msc_scsi_prepare_mode_sense_data()
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
static void usbd_msc_scsi_prepare_mode_sense_data(const sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                                  uint8_t                      *p_data_buf,
                                                  uint8_t                      scsi_cmd,
                                                  uint8_t                      page_code)
{
  uint8_t ix_mode_data_len;
  uint8_t ix_medium_type;
  uint8_t ix_dev_spec_param;
  uint8_t ix_mode_page;
  uint8_t ix_nxt_page;
  uint8_t mode_param_hdr_len;

  // Index preparation according to MODE SENSE cmd type.
  if (scsi_cmd == SLI_USBD_MSC_SCSI_CMD_MODE_SENSE_06) {
    ix_mode_data_len = 0;
    ix_medium_type = 1;
    ix_dev_spec_param = 2;
    ix_mode_page = 4;
    mode_param_hdr_len = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_6_LEN;
  } else {
    // MODE SENSE(10).
    // Mode data len LSB.
    ix_mode_data_len = 1;
    ix_medium_type = 2;
    ix_dev_spec_param = 3;
    ix_mode_page = 8;
    mode_param_hdr_len = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PARAM_HDR_10_LEN;
  }
  // Ensure Mode Sense buf properly reset.
  memset((void *)p_data_buf,
         0u,
         SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_LEN);

  //  MODE PARAM HDR
  //  Medium type supported by lun.
  p_data_buf[ix_medium_type] = SLI_USBD_MSC_SCSI_DISK_MEMORY_MEDIA;
  //Indicate if medium is write-protected.
  if (p_storage_lun->is_read_only) {
    p_data_buf[ix_dev_spec_param] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_PROT;
  } else {
    p_data_buf[ix_dev_spec_param] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_SPEC_PARAM_WR_EN;
  }

  switch (page_code) {
    case SLI_USBD_MSC_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS:
      // See Note #1.
      // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + SLI_USBD_MSC_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;

      // BLK DESC & MODE PAGE
      usbd_msc_scsi_page_info_except((void *)&p_data_buf[ix_mode_page]);
      break;

    case SLI_USBD_MSC_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY:
      // See Note #2.
      // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + SLI_USBD_MSC_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY;

      // BLK DESC & MODE PAGE
      usbd_msc_scsi_page_rw_error_recovery((void *)&p_data_buf[ix_mode_page]);
      break;

    case SLI_USBD_MSC_SCSI_PAGE_CODE_ALL:
      // Page Code: all pages supported by target.
      // Mode Data Len.
      p_data_buf[ix_mode_data_len] = mode_param_hdr_len
                                     + SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + SLI_USBD_MSC_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY
                                     + SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                                     + SLI_USBD_MSC_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;

      // BLK DESC & MODE PAGE
      usbd_msc_scsi_page_rw_error_recovery((void *)&p_data_buf[ix_mode_page]);

      ix_nxt_page = ix_mode_page
                    + SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MODE_PAGE_HDR_LEN
                    + SLI_USBD_MSC_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY;
      usbd_msc_scsi_page_info_except((void *)&p_data_buf[ix_nxt_page]);
      break;

    default:
      break;
  }
}

/****************************************************************************************************//**
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
static void usbd_msc_scsi_update_sense_data(sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                            uint8_t                sense_key,
                                            uint8_t                sense_code)
{
  p_storage_lun->sense_key = sense_key;
  p_storage_lun->asc = sense_code;
}

/****************************************************************************************************//**
 *                                       usbd_msc_scsi_prepare_inquiry_data()
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
 *               | 1-7  |            Controls and flags                 |
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
static void usbd_msc_scsi_prepare_inquiry_data(const sl_usbd_msc_scsi_lun_t *p_storage_lun,
                                               uint8_t                      *p_resp_buf)
{
  // Get target info.
  p_resp_buf[0u] = SLI_USBD_MSC_SCSI_PER_DEV_TYPE_DIRECT_ACCESS_BLOCK_DEV
                   | (SLI_USBD_MSC_SCSI_PER_QUAL_CONN << 5u);
  p_resp_buf[1u] = SLI_USBD_MSC_SCSI_INQUIRY_RMB;
  p_resp_buf[2u] = SLI_USBD_MSC_SCSI_INQUIRY_VERS_SPC_3;
  p_resp_buf[3u] = SLI_USBD_MSC_SCSI_INQUIRY_RESP_DATA_FMT_DEFAULT;
  p_resp_buf[4u] = (SLI_USBD_MSC_SCSI_INQUIRY_DATA_LEN - 5u);

  // Vendor ID info.
  sl_strcpy_s((char *)&p_resp_buf[8u],
              (SL_USBD_MSC_DATA_BUFFER_SIZE - 8u),
              p_storage_lun->vendor_id_str);

  // Product ID info.
  sl_strcpy_s((char *)&p_resp_buf[16u],
              (SL_USBD_MSC_DATA_BUFFER_SIZE - 16u),
              p_storage_lun->product_id_str);

  // Product revision level.
  memcpy(&p_resp_buf[32u], &p_storage_lun->product_revision_level, 4u);
}

/****************************************************************************************************//**
 *                                       usbd_msc_scsi_page_rw_error_recovery()
 *
 * @brief    Prepare Mode Sense Data with Read/Write Error Recovery page parameters.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold Mode sense data.
 *
 * @note     (1) The format of Read/Write Error Recovery mode Page is specified in
 *               'SCSI Blocks Commands - 3' (SBC-3), Revision 16, Section 6.3.5.
 *******************************************************************************************************/
static void usbd_msc_scsi_page_rw_error_recovery(void *p_buf_dest)
{
  uint8_t *p_buf_dest_08;

  p_buf_dest_08 = (uint8_t *)p_buf_dest;
  p_buf_dest_08[0u] = SLI_USBD_MSC_SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY;   // Page Code: rd/wr err recovery page.
  p_buf_dest_08[1u] = SLI_USBD_MSC_SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY; // Page Length: rd/wr err recovery page.
  p_buf_dest_08[2u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_AWRE;                  // Enable AWRE.
  p_buf_dest_08[3u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_RD_RETRY_CNT;          // Recovery algorithm attempts during rd ops.
  p_buf_dest_08[4u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_CORRECTION_SPAN;       // Obsolete.
  p_buf_dest_08[5u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_HEAD_OFFSET_CNT;       // Obsolete.
  p_buf_dest_08[6u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_DATA_STROBE_OFFSET;    // Obsolete.
  p_buf_dest_08[7u] = 0x00u;                                                   // Reserved.
  p_buf_dest_08[8u] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_WR_RETRY_CNT;          // Recovery algorithm attempts during wr ops.
  p_buf_dest_08[9u] = 0x00u;                                                   // Reserved.
  // Err recovery time.
  SLI_USBD_MEM_VAL_SET_INT16U_BIG(p_buf_dest_08 + 10, SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_RECOVERY_LIMIT);
}

/****************************************************************************************************//**
 *                                       usbd_msc_scsi_page_info_except()
 *
 * @brief    Prepare Mode Sense Data with Informational Exceptions Control Page parameters.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold Mode sense data.
 *
 * @note     (1) The format of Informational Exceptions Control mode page is specified in
 *               'SCSI Primary Commands - 3' (SPC), Revision 23, Section 7.4.11.
 *******************************************************************************************************/
static void usbd_msc_scsi_page_info_except(void *p_buf_dest)
{
  uint8_t *p_buf_dest_08;

  p_buf_dest_08 = (uint8_t *)p_buf_dest;
  p_buf_dest_08[0] = SLI_USBD_MSC_SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS;
  p_buf_dest_08[1] = SLI_USBD_MSC_SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS;
  p_buf_dest_08[2] = 0u;
  p_buf_dest_08[3] = SLI_USBD_MSC_SCSI_MODE_SENSE_DATA_MRIE_NO_SENSE;   // Method of reporting info exceptions field.

  SLI_USBD_MEM_VAL_SET_INT32U_BIG(p_buf_dest_08 + 4, 0u);                // Interval Timer.
  SLI_USBD_MEM_VAL_SET_INT32U_BIG(p_buf_dest_08 + 8, 1u);                // Report Count.
}
