/******************************************************************************

 @file  rf_hal.h

 @brief This file contains the data structures and APIs for CC26xx
        RF Core Hardware Abstraction Layer (HAL).

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef RF_HAL_H
#define RF_HAL_H

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"

/*******************************************************************************
 * CONSTANTS
 */

// Radio HAL API Status
#define RFHAL_SUCCESS                  0
#define RFHAL_ERROR_INVALID_PARAM      1
#define RFHAL_ERROR_OUT_OF_MEMORY      2
#define RFHAL_ERROR_FW_INFO_FAILED     3
#define RFHAL_ERROR_UNKNOWN_RAT_CHAN   4
#define RFHAL_ERROR_ADD_TX_ENTRY_FAIL  5

// Command Data Entry Status
#define DATASTAT_PENDING               0x00
#define DATASTAT_ACTIVE                0x01
#define DATASTAT_BUSY                  0x02
#define DATASTAT_FINISHED              0x03
#define DATASTAT_UNFINISHED            0x04

// RAT Channel API
#define RAT_CHAN_BUSY                  0
#define RAT_CHAN_FREE                  1
#define RAT_CHAN_INVALID               0xFF

// RF Core States
#define RFCORE_STATE_IDLE              0
#define RFCORE_STATE_READY             1
#define RFCORE_STATE_RUNNING           2
#define RFCORE_STATE_SLEEPING          3

// Miscellaneous
#define SYNCH_SIZE                     8
#define FW_BASE_ADDRESS                TBD

/*
** Radio Commands
**
** Format:
** Bits 15:12: Protocol
**             0000: Common/Generic
**             0001: BLE
**             0010: IEEE 802.15.4
** Bits 11:9:  Type
**             00x: Immediate command
**             010: Direct command with no parameter or 1-byte parameter
**             011: Direct command with 2-byte parameter
**             10x: Background level radio operation command
**             11x: Foreground level radio operation command
** Bits 8:0:   Identifier
**
*/

// Mailbox Common Immediate and Direct Commands
#define CMD_UPDATE_RADIO_SETUP         0x0001  // immediate command
#define CMD_GET_FW_INFO                0x0002  // immediate command
#define CMD_WRITE_RFREG                0x0003  // immediate command
#define CMD_MODIFY_RFREG               0x0004  // immediate command
#define CMD_ADD_DATA_ENTRY             0x0005  // immediate command
#define CMD_REMOVE_DATA_ENTRY          0x0006  // immediate command
#define CMD_FLUSH_QUEUE                0x0007  // immediate command
#define CMD_CLEAR_RX_QUEUE             0x0008  // immediate command
#define CMD_REMOVE_PENDING_ENTRIES     0x0009  // immediate command
#define CMD_SET_RAT_COMPARE            0x000A  // immediate command
#define CMD_READ_FW_PARAM              0x000B  // immediate command
#define CMD_WRITE_FW_PARAM             0x000C  // immediate command
#define CMD_READ_FS_CAL                0x000D  // immediate command
#if defined( CC26XX ) || defined( CC13XX ) || defined( CC13X2P )
#define CMD_COPY_MEMORY                0x000E  // immediate command
#define CMD_SET_MEMORY                 0x000F  // immediate command
#define CMD_SET_TX_POWER               0x0010  // immediate command
#define CMD_UPDATE_FS                  0x0011  // immediate command
#define CMD_SET_TRIM                   0x0012  // immediate command
#define CMD_SET_TX20_POWER             0x0014  // immediate command
#endif // CC26XX/CC13XX
//
#define CMD_ABORT                      0x0401  // direct command
#define CMD_STOP                       0x0402  // direct command
#define CMD_GET_RSSI                   0x0403  // immediate or direct command
#define CMD_TRIGGER                    0x0404  // immediate or direct command
#define CMD_START_RAT                  0x0405  // direct command
#define CMD_PING                       0x0406  // direct command
#define CMD_DISABLE_DEBUG              0x0407  // direct command
#define CMD_DISABLE_RAT_CHANNEL        0x0408  // immediate or direct command
#define CMD_ARM_RAT_CHANNEL            0x0409  // immediate or direct command
#define CMD_DISARM_RAT_CHANNEL         0x040A  // immediate or direct command
#define CMD_READ_ADI_REG_BUS_0         0x040B  // immediate or direct command
#define CMD_READ_ADI_REG_BUS_1         0x040C  // immediate or direct command
#if defined( CC26XX ) || defined( CC13XX )
#define CMD_SET_TX_SHAPE               0x040D  // direct command
#define CMD_BUS_REQUEST                0x040E  // immediate or direct command
#define CMD_READ_TRIM                  0x040F  // direct command
#endif // CC26XX/CC13XX
//
#define CMD_READ_RFREG                 0x0601  // immediate or direct command
#define CMD_ENABLE_DEBUG               0x0602  // immediate or direct command
#define CMD_SET_RAT_CAPTURE            0x0603  // immediate or direct command
#define CMD_SET_RAT_OUTPUT             0x0604  // immediate or direct command
#define CMD_WRITE_ADI_REG_BUS_0        0x0605  // immediate or direct command
#define CMD_WRITE_ADI_REG_BUS_1        0x0606  // immediate or direct command
#define CMD_FORCE_CLK_ENA              0x0607  // immediate or direct command
#if defined( CC26XX ) || defined( CC13XX )
#define CMD_UPDATE_BAW_FREQ            0x0608  // immediate or direct command
#endif // CC26XX/CC13XX

// Mailbox Common Radio Commands
#define CMD_NOP                        0x0801
#define CMD_RADIO_SETUP                0x0802
#define CMD_FS                         0x0803
#define CMD_FS_OFF                     0x0804
#define CMD_RX                         0x0805
#define CMD_TX                         0x0806
#define CMD_RX_TEST                    0x0807
#define CMD_TX_TEST                    0x0808
#define CMD_SYNC_STOP_RAT              0x0809
#define CMD_SYNC_START_RAT             0x080A
#define CMD_COUNTER                    0x080B
#define CMD_FS_POWERUP                 0x080C
#define CMD_FS_POWERDOWN               0x080D
#define CMD_TOPSM_COPY                 0x080E
#define CMD_WRITE_ADDR                 0x080F
#if defined( CC26XX ) || defined( CC13XX )
#define CMD_RUN_IMMEDIATE_COMMAND      0x0810
#define CMD_RUN_USER_FUNCTION          0x0811
#define CMD_COUNTER_BRANCH             0x0812
#define CMD_PATTERN_CHECK              0x0813
#endif // CC26XX/CC13XX

// Common Radio Operation Command Status Codes
#define RFSTAT_IDLE                    0x0000
#define RFSTAT_PENDING                 0x0001
#define RFSTAT_ACTIVE                  0x0002
#define RFSTAT_SKIPPED                 0x0003
//
#define RFSTAT_DONE_OK                 0x0400  // result True
#define RFSTAT_DONE_COUNTDOWN          0x0401  // result False
#define RFSTAT_DONE_RXERR              0x0402  // result True
#define RFSTAT_DONE_TIMEOUT            0x0403  // result False
#define RFSTAT_DONE_STOPPED            0x0404  // result False
#define RFSTAT_DONE_ABORT              0x0405  // result Abort
#if defined( CC26XX ) || defined( CC13XX )
#define RFSTAT_DONE_FAILED             0x0406  // result Abort?
#endif // CC26XX/CC13XX
//
#define RFSTAT_ERROR_PAST_START        0x0800  // result Abort
#define RFSTAT_ERROR_START_TRIG        0x0801  // result Abort
#define RFSTAT_ERROR_CONDITION         0x0802  // result Abort
#define RFSTAT_ERROR_PARAM             0x0803  // result Abort
#define RFSTAT_ERROR_POINTER           0x0804  // result Abort
#define RFSTAT_ERROR_CMD_ID            0x0805  // result Abort
#define RFSTAT_ERROR_NO_SETUP          0x0807  // result Abort
#define RFSTAT_ERROR_NO_FS             0x0808  // result Abort
#define RFSTAT_ERROR_FS_PROG_FAIL      0x0809  // result Abort
#if defined( CC26XX ) || defined( CC13XX )
#define RFSTAT_ERROR_MODEM_TX_UNDF     0x080A  // result Abort
#define RFSTAT_ERROR_MODEM_RX_OVRF     0x080B  // result Abort
#define RFSTAT_ERROR_NO_REQ_RX_DATA    0x080C  // result Abort
#endif // CC26XX/CC13XX

// Radio Operation Command Trigger Types
#define TRIGTYPE_NOW                   0x00  // not applicable to end triggers
#define TRIGTYPE_NEVER                 0x01  // except if CMD_TRIGGER used
#define TRIGTYPE_AT_ABS_TIME           0x02
#define TRIGTYPE_REL_CMD_SUBMIT        0x03
#define TRIGTYPE_REL_CMD_START         0x04  // not allowed for start triggers
#define TRIGTYPE_REL_PREV_CMD_START    0x05
#define TRIGTYPE_REL_FIRST_CHAIN_CMD   0x06
#define TRIGTYPE_REL_END_PREV_CMD      0x07
#define TRIGTYPE_REL_EVENT_1           0x08
#define TRIGTYPE_REL_EVENT_2           0x09
#if defined( CC26XX ) || defined( CC13XX )
#define TRIGTYPE_EXTERNAL              0x0A  // on external trigger input to RAT
#endif // CC26XX/CC13XX
#define TRIGTYPE_REL_SYNC              0x0F  // Slave only! R2 only!
//
#define ALT_CMD_TRIG_ENABLE            0x10
#define PAST_TRIG_START_ASAP           0x80

// Radio Operation Command Condition Types
// Note: Condition affects the next command. E.g. CONDTYPE_RUN_TRUE_STOP_FALSE
//       means run the next command if the current command returned TRUE.
#define CONDTYPE_ALWAYS_RUN_NEXT_CMD   0x00  // except in case of Abort
#define CONDTYPE_NEVER_RUN_NEXT_CMD    0x01  // can still be used for skip
#define CONDTYPE_RUN_TRUE_STOP_FALSE   0x02
#define CONDTYPE_STOP_TRUE_RUN_FALSE   0x03
#define CONDTYPE_RUN_TRUE_SKIP_FALSE   0x04  // skip count given by condition
#define CONDTYPE_SKIP_TRUE_RUN_FALSE   0x05  // skip count given by condition

// Radio Operation Setup Parameter Types
#define RADIO_SETUP_MODE_BLE           0x00
#define RADIO_SETUP_MODE_15_4          0x01
#define RADIO_SETUP_MODE_2MBPS_GFSK    0x02
#define RADIO_SETUP_MODE_5MBPS_8FSK    0x05
#define RADIO_SETUP_MODE_ANT           0x06
//
#define RADIO_SETUP_HW_TYPE_32_16      0x00
#define RADIO_SETUP_HW_TYPE_32_32      0x01
#define RADIO_SETUP_HW_TYPE_16_16      0x02
#define RADIO_SETUP_HW_TYPE_16_PLUS_2  0x03
//
#define RADIO_SETUP_FW_DEFINED_ADDR    0x0F
//
#define RADIO_SETUP_16_BIT_VALUE       0x00
#define RADIO_SETUP_8_BIT_VALUE        0x01

// Radio Operation Receiver/Transmitter Parameter Types
#define RXTX_LITTLE_ENDIAN             0x00
#define RXTX_BIG_ENDIAN                0x01
//
#define RXTX_FREQ_SYNTH_OFF            0x00
#define RXTX_FREQ_SYNTH_ON             0x01
//
#define RXTX_NO_CRC                    0x00
#define RXTX_ADD_CRC                   0x01
//
#define RXTX_NO_SW_IN_CRC_CALC         0x00
#define RXTX_USE_SW_IN_CRC_CALC        0x01
//
#define RXTX_NO_HDR_IN_CRC_CALC        0x00
#define RXTX_USE_HDR_IN_CRC_CALC       0x01
//
#define RX_NO_CRC_REPORT               0x00
#define RX_REPORT_CRC                  0x01
//
#define RX_RX_PKT_AFTER_END_TRIG       0x00
#define RX_END_RX_PKT_ON_END_TRIG      0x01
//
#define RX_SINGLE_SYNCH_WORD           0x00
#define RX_DUAL_SYNCH_WORD             0x01
//
#define TX_SEND_MODULATED_SIGNAL       0x00
#define TX_SEND_CONTINUOUS_WAVE        0x01
//
#define TX_NO_WHITENING                0x00
#define TX_DEFAULT_WHITENING           0x01
#define TX_WHITEN_PRBS_15              0x02
#define TX_WHITEN_PRBS_32              0x03
//
#define RX_DISABLE_MODEM_FIFO          0x00
#define RX_ENABLE_MODEM_FIFO           0x01
//
#define RX_RUN_SYNC_SEARCH_NORMAL      0x00
#define RX_NO_SYNC                     0x01
//
#define TRX_KEEP_FS_ON_WHEN_DONE       0x00
#define TRX_TURN_FS_OFF_WHEN_DONE      0x01
//
#define FS_START_IN_RX_MODE            0x00
#define FS_START_IN_TX_MODE            0x01
//
#define FS_USE_STD_CALIBRATION         0x00
#define FS_OVERRIDE_CALIBRATION        0x01

// Radio Immediate Enable Debug Trace Parameter Types
#define DBGTRC_DIVIDE_BY_1             0x00
#define DBGTRC_DIVIDE_BY_2             0x01
#define DBGTRC_DIVIDE_BY_3             0x02
#define DBGTRC_DIVIDE_BY_4             0x03
//
#define DBGTRC_DISAB_TIMESTAMPS        0x00
#define DBGTRC_ENAB_TIMESTAMPS         0x01
//
#define DBGTRC_DISAB_CHAN3             0x00
#define DBGTRC_ENAB_CHAN3              0x01
//
#define DBGTRC_DISAB_CHAN2             0x00
#define DBGTRC_ENAB_CHAN2_NORMAL       0x01
#define DBGTRC_ENAB_CHAN2_BACKDOOR     0x02
#define DBGTRC_ENAB_CHAN2_RESERVED     0x03
//
#define DBGTRC_DISAB_CHAN1             0x00
#define DBGTRC_ENAB_CHAN1              0x01
//
#define DBGTRC_DISAB_SYSTICKS          0x00
#define DBGTRC_ENAB_SYSTICKS_CHAN1     0x01
#define DBGTRC_ENAB_SYSTICKS_CHAN2     0x02
#define DBGTRC_ENAB_SYSTICKS_CHAN3     0x03

// Radio Immediate Set RAT Channel To Capture Mode Parameter Types
#define RAT_CHAN_CAPT_SINGLE_MODE      0x00
#define RAT_CHAN_CAPT_REPEATED_MODE    0x01

#if defined( CC26XX ) || defined( CC13XX )
// External Trigger Input Mode
#define EXT_TRIG_RISING_EDGE           0x00
#define EXT_TRIG_FALLING_EDGE          0x01
#define EXT_TRIG_BOTH_EDGES            0x02
#define EXT_TRIG_RESERVED              0x03

// External Trigger RAT Input Event Number
#define EXT_TRIG_RAT_INPUT_EVENT_0     0x00
#define EXT_TRIG_RAT_INPUT_EVENT_1     0x01
#define EXT_TRIG_RAT_INPUT_EVENT_2     0x02
#define EXT_TRIG_RAT_INPUT_EVENT_3     0x03
#define EXT_TRIG_RAT_INPUT_EVENT_4     0x04
#define EXT_TRIG_RAT_INPUT_EVENT_5     0x05
#define EXT_TRIG_RAT_INPUT_EVENT_6     0x06
#define EXT_TRIG_RAT_INPUT_EVENT_7     0x07
#define EXT_TRIG_RAT_INPUT_EVENT_8     0x08
#define EXT_TRIG_RAT_INPUT_EVENT_9     0x09
#define EXT_TRIG_RAT_INPUT_EVENT_10    0x0A
#define EXT_TRIG_RAT_INPUT_EVENT_11    0x0B
#define EXT_TRIG_RAT_INPUT_EVENT_12    0x0C
#define EXT_TRIG_RAT_INPUT_EVENT_13    0x0D
#define EXT_TRIG_RAT_INPUT_EVENT_14    0x0E
#define EXT_TRIG_RAT_INPUT_EVENT_15    0x0F
#define EXT_TRIG_RAT_INPUT_EVENT_16    0x10
#define EXT_TRIG_RAT_INPUT_EVENT_17    0x11
#define EXT_TRIG_RAT_INPUT_EVENT_18    0x12
#define EXT_TRIG_RAT_INPUT_EVENT_19    0x13
#define EXT_TRIG_RAT_INPUT_EVENT_20    0x14
#define EXT_TRIG_RAT_INPUT_EVENT_21    0x15
#define EXT_TRIG_RAT_INPUT_EVENT_22    0x16
#define EXT_TRIG_RAT_INPUT_EVENT_23    0x17
#define EXT_TRIG_RAT_INPUT_EVENT_24    0x18
#define EXT_TRIG_RAT_INPUT_EVENT_25    0x19
#define EXT_TRIG_RAT_INPUT_EVENT_26    0x1A
#define EXT_TRIG_RAT_INPUT_EVENT_27    0x1B
#define EXT_TRIG_RAT_INPUT_EVENT_28    0x1C
#define EXT_TRIG_RAT_INPUT_EVENT_29    0x1D
#define EXT_TRIG_RAT_INPUT_EVENT_30    0x1E
#define EXT_TRIG_RAT_INPUT_EVENT_31    0x1F
#endif // CC26XX/CC13XX

/*
** Command Data Entry
*/

// Data Entry Type
#define DATA_ENTRY_TYPE_GENERAL        0x00
#define DATA_ENTRY_TYPE_EXTENDED       0x01
#define DATA_ENTRY_TYPE_POINTER        0x02
// Note: The RF Driver name conflicts with mailbox.h!
//#define DATA_ENTRY_TYPE_PARTIAL        0x03

// Data Entry Length Size
#define DATA_ENTRY_LEN_SIZE_0          0x00
#define DATA_ENTRY_LEN_SIZE_1          0x04
#define DATA_ENTRY_LEN_SIZE_2          0x08
#define DATA_ENTRY_LEN_SIZE_RESERVED   0x0C

/*
** Software RAT Channels
*/
#define MAX_NUM_RAT_CHANS              16
#define MAX_NUM_SW_RAT_CHANS           4

/*******************************************************************************
 * MACROS
 */

/*
** Set and Build Direct Command
*/
#define BUILD_DIRECT_CMD( c )                                                  \
  (((c) << 16) | 0x0001)

#define SET_DIRECT_CMD( x, c )                                                 \
  (x) = BUILD_DIRECT_CMD((c))

#define BUILD_DIRECT_PARAM_CMD( c, p )                                         \
  ((c) << 16) | ((((p) & 0xFF) << 8) | 0x0001)

#define SET_DIRECT_PARAM_CMD( x, c, p )                                        \
  (x) = BUILD_DIRECT_PARAM_CMD((c), (p))

#define BUILD_DIRECT_PARAM_EXT_CMD( c, e )                                     \
  ((c) << 16) | (((e) & 0xFFFC) | 0x0001)

#define SET_DIRECT_PARAM_EXT_CMD( x, c, e )                                    \
  (x) = BUILD_DIRECT_PARAM_EXT_CMD((c), (e))

/*
** Radio Operation Trigger
*/
#define SET_RFOP_ALT_TRIG_CMD(t)                                               \
  (t) |= BV(4)

#define CLR_RFOP_ALT_TRIG_CMD(t)                                               \
  (t) &= ~BV(4)

#define SET_RFOP_TRIG_TYPE(t, tType)                                           \
  (t) = (((t) & ~0x0F) | ((tType) & 0x0F))

#define CHK_RFOP_TRIG_TYPE(t, tType)                                           \
  (((t) & 0x0F) == (tType))

#define SET_RFOP_TRIG_NUM(t, tnum)                                             \
  (t) = (((t) & ~0x60) | (((tnum) & 0x03) << 5))

#define SET_RFOP_PAST_TRIG(t)                                                  \
  (t) |= BV(7)

#define CLR_RFOP_PAST_TRIG(t)                                                  \
  (t) &= ~BV(7)

/*
** Radio Operation Condition
*/

#define SET_RFOP_COND_RULE(c, rule)                                            \
  (c) = (((c) & 0xF0) | (rule))

#define SET_RFOP_COND_SKIP_CNT(c, skipCnt)                                     \
  (c) = (((c) & 0x0F) | ((skipCnt)<<4))

/*
** Radio Setup Hardware and Firmware Register Override
*/
#define SET_RFOP_HWFW_REG_VAL( x, v )                                          \
  (x) = ((x) & 0x0000FFFF) | ((v) << 16)

#define SET_RFOP_HWFW_SELECT( x, v )                                           \
  (x) = ((x) & 0xFFFF0FFF) | (((v) & 0x0F) << 12))

#define SET_RFOP_HW_ADDR( x, v )                                               \
  (x) = ((x) & 0xFFFFF003) | (((v) & 0x3FF) << 2)

#define SET_RFOP_HW_TYPE( x, v )                                               \
  (x) = ((x) & 0xFFFFFFFC) | ((v) & 0x03)

#define SET_RFOP_FW_VAL_SIZE( x, v )                                           \
  (x) = ((x) & 0xFFFFF7FF) | (((v) & 0x01) << 11)

#define SET_RFOP_FW_ADDR( x, v )                                               \
  (x) = ((x) & 0xFFFFF800) | ((v) & 0x7FF)

/*
** Radio Operation Receiver & Transmitter Command Packet Configuration
*/
#define SET_RFOP_TXRX_CFG_ENDIAN( x, v )                                       \
  (x) = ((x) & 0xFFFE) | ((v) & 0x01)

#define SET_RFOP_TXRX_CFG_HDR_BITS( x, v )                                     \
  (x) = ((x) & 0xFF81) | (((v) & 0x3F) << 6)

#define SET_RFOP_TXRX_FS_ON_OFF( x, v )                                        \
  (x) = ((x) & 0xFF7F) | (((v) & 0x01) << 7)

#define SET_RFOP_TXRX_USE_CRC( x, v )                                          \
  (x) = ((x) & 0xFEFF) | (((v) & 0x01) << 8)

#define SET_RFOP_TXRX_SYNCH_CRC( x, v )                                        \
  (x) = ((x) & 0xFDFF) | (((v) & 0x01) << 9)

#define SET_RFOP_TXRX_SYNCH_HDR( x, v )                                        \
  (x) = ((x) & 0xFBFF) | (((v) & 0x01) << 10)

#define SET_RFOP_RX_REPORT_CRC( x, v )                                         \
  (x) = ((x) & 0xF7FF) | (((v) & 0x01) << 11)

#define SET_RFOP_RX_END_TYPE( x, v )                                           \
  (x) = ((x) & 0xEFFF) | (((v) & 0x01) << 12)

#define SET_RFOP_RX_NUM_SYNCH( x, v )                                          \
  (x) = ((x) & 0xDFFF) | (((v) & 0x01) << 13)

#define SET_RFOP_RX_LEN_BIT_SIZE( x, v )                                       \
  (x) = ((x) & 0xFFF0) | ((v) & 0x0F)

#define SET_RFOP_RX_LEN_BIT_POSITION( x, v )                                   \
  (x) = ((x) & 0xFE0F) | (((v) & 0x1F) << 4)

#define SET_RFOP_RX_LEN_OFFSET( x, v )                                         \
  (x) = ((x) & 0x01FF) | (((v) & 0x3F) << 9)

/*
** Radio Immediate Enable Debug Trace Command Configuration
*/
#define SET_DBGTRC_SYSTICKS_CHAN( x, v )                                       \
  (x) = ((x) & 0xFF3F) | (((v) & 0x03) << 6)

#define SET_DBGTRC_CHAN1( x, v )                                               \
  (x) = ((x) & 0xFEFF) | (((v) & 0x01) << 8)

#define SET_DBGTRC_CHAN2( x, v )                                               \
  (x) = ((x) & 0xF9FF) | (((v) & 0x03) << 9)

#define SET_DBGTRC_CHAN3( x, v )                                               \
  (x) = ((x) & 0xF7FF) | (((v) & 0x01) << 11)

#define SET_DBGTRC_TIMESTAMPS( x, v )                                          \
  (x) = ((x) & 0xEFFF) | (((v) & 0x01) << 12)

#define SET_DBGTRC_PRESCALER( x, v )                                           \
  (x) = ((x) & 0x3FFF) | (((v) & 0x03) << 14)

/*
** Radio Immediate Set RAT Channel To Capture Mode Command Configuration
** Note: Unused bits are zeroed.
*/
#define SET_RATCAPT_INPUT_SRC( x, v )                                          \
  (x) = ((x) & 0x7F00) | (((v) & 0x1F) << 3)

#define SET_RATCAPT_RAT_CHAN( x, v )                                           \
  (x) = ((x) & 0x70F8) | (((v) & 0x0F) << 8)

#define SET_RATCAPT_REPEAT( x, v )                                             \
  (x) = ((x) & 0x6FF8) | (((v) & 0x01) << 12)

#define SET_RATCAPT_INPUT_MODE( x, v )                                         \
  (x) = ((x) & 0x1FF8) | (((v) & 0x03) << 13)

/*
** Radio Immediate Set RAT Output To A Specified Mode Command Configuration
** Note: Unused bits are zeroed.
*/
#define SET_RATOUT_RAT_CHAN( x, v )                                            \
  (x) = ((x) & 0x00FC) | (((v) & 0x0F) << 8)

#define SET_RATOUT_RAT_OUT_MODE( x, v )                                        \
  (x) = ((x) & 0x0F1C) | (((v) & 0x07) << 5)

#define SET_RATOUT_RAT_OUT_SELECT( x, v )                                      \
  (x) = ((x) & 0x0FE0) | (((v) & 0x07) << 2)

/*
** Data Entry Configuration
*/
#define SET_DATAENTRY_TYPE( x, v )                                             \
  (x) = ((x) & 0xFC) | (v)

#define SET_DATAENTRY_LEN_SIZE( x, v )                                         \
  (x) = ((x) & 0xF3) | (v)

/*
** Tx and Rx Test Command Configuration
*/
#define SET_TX_TEST_CONFIG( x, v1, v2, v3 )                                    \
  (x) = ((v1)&0x01) | (((v2)&0x01) << 1) | (((v3)&0x03) << 2)

#define SET_RX_TEST_CONFIG( x, v1, v2, v3 )                                    \
  (x) = ((v1)&0x01) | (((v2)&0x01) << 1) | ((((v3)&0x01) << 2))

#if defined( CC26XX ) || defined( CC13XX )
/*
** External Trigger
*/
#define SET_EXT_TRIG_EVT( x, v1, v2 )                                          \
    (x) = (((v1)&0x03) << 2) | (((v2)&0x1F) << 8)
#endif // CC26XX/CC13XX

/*******************************************************************************
 * TYPEDEFS
 */

// Radio Operation Status
typedef uint8 rfhalStat_t;

// Radio Operation Trigger
// |     7     |         6..5           |           4            |     3..0     |
// | Past Trig | Command Trigger Number | Command Trigger Enable | Trigger Type |
//
typedef uint8 trig_t;

// Radio Operation Condition
// |      7..4       |  3..0  |
// | Number of Skips |  Rule  |
//
typedef uint8 cond_t;

// Radio Operation Radio Setup Hardware Register Override
// | 31..16 |   15..12    |  11..2  |   1..0  |
// | Value  | HWFW Select | HW Addr | HW Type |
//
typedef uint32 hwOverride_t;

// Radio Operation Radio Setup Firmware Register Overide
// | 31..16 |   15..12    |   11..11   |  10..0  |
// | Value  | HWFW Select | Value Size | FW Addr |
//
typedef uint32 fwOverride_t;

// Radio Operation Receiver Command Packet Configuration
// |       13      |    12    |     11     |      10     |      9      |    8    |     7     |   6..1   |  0..0  |
// | One/Two Synch | End Type | Report CRC | Synch + Hdr | Synch + CRC | Use CRC | FS On/Off | Hdr Bits | Endian |
//
typedef uint16 rxPktCfg_t;

// Radio Operation Receiver Command Length Configuration
// |      15..9    |          8..4         |        3..0        |
// | Length Offset | Length Field Position | Number Length Bits |
//
typedef uint16 lenCfg_t;

// Radio Operation Transmitter Command Packet Configuration
// |      10     |       9     |    8    |     7     |   6..1   |  0..0  |
// | Synch + Hdr | Synch + CRC | Use CRC | FS On/Off | Hdr Bits | Endian |
//
typedef uint16 txPktCfg_t;

// Radio Immediate Enable Debug Trace Command Configuration
// |   15..14  |  13 |     12     |   11   | 10..9  |    8   |    7..6      | 5..0 |
// | Prescaler | N/A | Timestamps | Chan 3 | Chan 2 | Chan 1 | Systick Chan |  N/A |
//
typedef uint16 dbgTraceCfg_t;

// Radio Immediate Set RAT Channel To Capture Mode Command Configuration
// |  15 |   14..13   |   12   |   11..8  |     7..3     | 2..0 |
// | N/A | Input Mode | Repeat | RAT Chan | Input Source | N/A  |
//
typedef uint16 ratCapCfg_t;

// Radio Immediate Set RAT Output To A Specified Mode Command Configuration
// | 15..12 |   11..8  |     7..5    |     4..2      | 1..0 |
// |  N/A   | RAT Chan | Output Mode | Output Select | N/A  |
//
typedef uint16 ratOutModeCfg_t;

// Data Entry Configuration
// Note: Bits 7..4 are for partial read Rx entry only.
// |       7..4       |    3..2     |  1..0  |
// | numInterIntBytes | Length Size |  Type  |
//
typedef uint8  dataEntryCfg_t;

// Data Entry Partial Read Rx Status
// |        15       |        14        |     13    |    12..0     |
// | Last Elem Conts | First Elem Conts | Open Elem | Num Elements |
//
typedef uint16 entryStatus_t;

// Tx Test Configuration
// |     3..2    |      1     |        0        |
// | Whiten Mode | FS Control | Continuous Wave |
//
typedef uint8 txTstCfg_t;

// Rx Test Configuration
// |      2    |     1      |     0       |
// | Sync Mode | FS Control | FIFO Enable |
//
typedef uint8 rxTstCfg_t;

/*
** Radio Command and Data Structures
**
** R  = System CPU reads; Radio CPU will not read.
** W  = System CPU writes; Radio CPU reads but will not modify.
** RW = System CPU writes initially; Radio CPU reads and may modify.
*/

//
// Radio Command Data Queue and Entry Structures
//
// There are four kinds of data queue data entry headers:
//   General (also just called Data Entry)
//   Multi-Element
//   Pointer
//   Partial Read.
//

/*******************************************************************************
** Data Entry Queue and Data Queue
**
** The data entry queue points to the current data entry and the last data
** entry. These pointers are setup by the system software, but these pointers
** are owned by (i.e. updated by) the radio software.
**
** The data queue is how the system software would define a data entry queue.
** It is needed to keep track of the next entry on the queue. The radio software
** is unaware of the additional fields required by the system software.
**
**/

// Queue Types
typedef struct dataQ         dataQ_t;
typedef struct dataEntryQ    dataEntryQ_t;

// Data Entry Types
typedef struct dataEntry     dataEntry_t;
typedef struct dataEntryPtr  dataEntryPtr_t;
typedef struct dataEntryExt  dataEntryExt_t;
typedef struct dataEntryPart dataEntryPart_t;

// Data Entry Queue
PACKED_STRUCT dataEntryQ
{
  dataEntry_t    *pCurEntry;           // RW: ptr to current data entry, or NULL
  dataEntry_t    *pLastEntry;          // RW: ptr to last data entry, or NULL
};

// Data Queue
PACKED_STRUCT dataQ
{
  dataEntryQ_t   dataEntryQ;           // Data Entry Queue
  dataEntry_t   *pNextDataEntry;       // ptr to Data Entry (internal use only)
  dataEntry_t   *pTempDataEntry;       // ptr to Temp Data Entry (internal use only)
};


/*******************************************************************************
** General Data Entry
**
** The data entry uses general data entry headers.
**
** Note: The data buffer is expected to immediately follow the header (RW).
**
**/

// General Data Entry
PACKED_STRUCT  dataEntry
{
  dataEntry_t    *pNextEntry;          // RW: ptr to next data entry in queue
  uint8           status;              // RW: status of data entry
  dataEntryCfg_t  config;              // W:  data entry configuration
  uint16          length;              // W:  Tx pkt size or total Rx space in bytes
};

/*******************************************************************************
** Pointer Data Entry
**
** The pointer data entry uses pointer data entry headers.
**
*/

// Pointer Data Entry
PACKED_STRUCT dataEntryPtr
{
  dataEntry_t     dataEntry;           // general data entry header
  uint8          *pData;               // W:  ptr to data buffer
};

/*******************************************************************************
** Extended Data Entry Queue
**
** The extended data entry uses extended data entry headers.
**
** Note: Extended data entry headers are also called Multi-Element data entry
**       headers.
**
*/

// Extended Data Entry
PACKED_STRUCT dataEntryExt
{
  dataEntry_t     dataEntry;           // general data entry header
  uint16          numElements;         // R:  num of committed entry elements
  uint16          nextIndex;           // R:  index to next entry element location
};

/*******************************************************************************
** Partial Data Entry
**
** The partial data entry uses partial data entry headers.
**
*/

// Partial Data Entry
PACKED_STRUCT  dataEntryPart
{
  dataEntry_t     dataEntry;           // general data entry header
  entryStatus_t   pktStatus;           // R:  num committed entry elements in data entry
  uint16          nextIndex;           // R:  index to byte after last byte of last entry element
};


/*
** Radio Operation Commands
*/

//
// Radio Operation Data Structures
//

// Radio Operation Command Common Structure
typedef struct rfOpCmd rfOpCmd_t;
PACKED_STRUCT rfOpCmd
{
  uint16     cmdNum;                   // W:  radio command number
  uint16     status;                   // RW: radio status
  rfOpCmd_t *pNextRfOp;                // W:  ptr to next radio command op
  uint32     startTime;                // W:  RAT start time
  trig_t     startTrig;                // W:  radio command trigger
  cond_t     condition;                // W:  radio command condition
};

// Radio Haredware/Firmware Register Overrides
// Note: List must be terminated by 16 bits of zero.
typedef union
{
  hwOverride_t hwRegOverride;
  fwOverride_t fwRegOverride;
} regOverride_t;

// CMD_NOP: No Operation Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t nopCmd;                     // radio command common structure
} rfOpCmd_NOP_t;

// CMD_RADIO_SETUP: Radio Setup Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t      rfOpCmd;              // radio command common structure
  uint8          mode;                 // W:  radio mode (BLE, 15.4, etc.)
#if defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
  uint8          loDivider;            // W:  LO divder setting to use
#else // !Agama
  uint8          reserved;             // unused
#endif // CC26X2 | CC13X2 | CC13X2P
  uint16         config;               // W:  bias, trim, FS power up, etc.
  uint16         txPower;              // W:  IB, GC, tempCoeff
  regOverride_t *pRegOverride;         // W:  ptr to list of HW regs to override
#if defined(CC13X2P)
  regOverride_t *pRegOverrideTxStd;
  regOverride_t *pRegOverrideTx20;     // ptr to list of High Gain overrides (only applicable on CC1352P)
#endif
} rfOpCmd_RadioSetup_t;

// CMD_FS_POWERUP: Frequency Synthesizer Power Up
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      reserved;                // unused
  uint32      *pRegOverride;           // W:  ptr to list of HW regs to override
} rfOpCmd_freqSynthUp_t;

// CMD_FS_POWERDOWN: Frequency Synthesizer Power Down
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
} rfOpCmd_freqSynthDown_t;

// CMD_FS: Frequency Synthesizer Controls Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t  rfOpCmd;                  // radio command common structure
  uint16     freq;                     // W:  frequency in MHz to tune radio to
  uint16     fractFreq;                // W:  fractional freq (mod 64K) in MHz
  uint8      synthCfg;                 // W:  specify Tx/Rx and ref freq
  uint8      calCfg;                   // W:  calibration override
  uint8      midPrecal;                // W:  mid calibration value
  uint8      ktPrecal;                 // W:  KT precal value
  uint16     tdcPrecal;                // W:  TDC calibration value
} rfOpCmd_freqSynthCtrl_t;

// CMD_FS_OFF: Turn Off Frequency Synthesizer Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
} rfOpCmd_freqSynthOff_t;

// CMD_RX: Radio Receiver Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  rxPktCfg_t  rxPktConfig;             // W:  receiver packet configuration
  uint8       synchWord[SYNCH_SIZE];   // W:  synchronization word (single/dual)
  lenCfg_t    lenConfig;               // W:  length configuration
  uint16      maxLen;                  // W:  max number of bytes in Rx pkt
  uint8      *pRxBuf;                  // W:  ptr to buffer for Rx pkt, or NULL
  uint32      endTime;                 // W:  RAT time to end operation
  trig_t      endTrigger;              // W:  trigger for ending operation
  uint8       rssi;                    // R:  RSSI of Rx pkt
  uint16      rxlen;                   // R:  number of received bytes
  uint32      timeStamp;               // R:  timestamp of received pkt
  uint16      nRxOk;                   // RW: num Rx pkts with good CRC
  uint16      nRxNok;                  // RW: num Rx pkts with bad CRC
  uint16      nRx2Ok;                  // RW: num Rx pkts with good CRC and second sync word (optional if bDualSw is 0)
  uint16      nRx2Nok;                 // RW: num Rx pkts with bad CRC and second sync word (optional if bDualSw is 0)
} rfOpCmd_RxPkt_t;

// CMD_TX: Radio Transmitter Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  txPktCfg_t  txPktConfig;             // W:  transmitter packet configuration
  uint32      synchWord;               // W:  synchronization word
  uint8      *pTxBuf;                  // W:  ptr to buffer for Tx pkt
  uint16      pktLen;                  // W"  Tx pkt size in bytes
} rfOpCmd_TxPkt_t;

// CMD_RX_TEST: Radio Receiver Test Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  rxTstCfg_t  config;                  // W:  FIFO, FS control, and Sync mode
  trig_t      endTrig;                 // W:  end trig for Rx
  uint32      syncWord;                // W:  synchronization word
  uint32      endTime;                 // W:  time to end operation
} rfOpCmd_RxTest_t;

// CMD_TX_TEST: Radio Transmitter Test Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  txTstCfg_t  config;                  // W:  signal type, whitening, FS control
  uint8       reserved1;               // unused
  uint16      txWord;                  // W:  payload to use
  uint8       reserved2;               // unused
  trig_t      endTrig;                 // W:  end trig for Tx
  uint32      syncWord;                // W:  synchronization word
  uint32      endTime;                 // W:  time to end operation
} rfOpCmd_TxTest_t;

// CMD_SYNC_START_RAT: Radio Synchronously Start and Synchronize Stop RAT Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      reserved;                // unused
  uint32      ratVal;                  // W: desired RAT timer on Start; R: RAT timer on Stop
} rfOpCmd_SynchStartStopRat_t;

// CMD_COUNT: Radio Counter Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      counter;                 // RW: command operation counter
} rfOpCmd_Count_t;

// CMD_TOPSM_COPY: Radio Copy TOPsm ROM-to-RAM Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  int8        mceBank;                 // W: ROM bank number for the MCE (0-5). Negative: Do not copy MCE ROM.
  int8        rfeBank;                 // W: ROM bank number for the RFE (0-5). Negative: Do not copy RFE ROM.
  uint16      mceStopAddr;             // W: Last 16-bit address top copy for MCE ROM. 0: Copy entire ROM
  uint16      rfeStopAddr;             // W: Last 16-bit address top copy for RFE ROM. 0: Copy entire ROM
} rfOpCmd_TopsmCopy_t;

// CMD_WRITE_ADDR: Radio Write Address Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint8       type;                    // W:  bus access type
  uint8       reserved;                // unused
  uint32      address;                 // W:  address to write to
  uint32      value;                   // W:  value to write
} rfOpCmd_writeAddr_t;

#if defined( CC26XX ) || defined( CC13XX )
// CMD_SCH_IMM: Radio Run Immediate Command As Radio Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      reserved;                // unused
  uint32      cmdVal;                  // W:  value to write to CMDR register
  uint32      cmdStatVal;              // R:  value returned from CMDSTA
} rfOpCmd_runImmedCmd_t;

// CMD_USER_FUN: Radio Run User Function on Radio CPU
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      reserved;                // unused
  uint32      funcAddr;                // W:  address of function to run
  uint32     *pParams;                 // RW: additional parameters as needed
} rfOpCmd_userFunc_t;

// CMD_COUNT_BRANCH: Counter Command with Branch of Command Chain
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      counter;                 // RW: counter
  rfOpCmd_t  *pNxtOpIfOK;              // W:  cond ptr to next op
} rfOpCmd_cntBranch_t;

// CMD_PATTERN_CHECK: Check a Value in Memory Against a Pattern
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t   rfOpCmd;                 // radio command common structure
  uint16      patternOpt;              // W:  various pattern options
  rfOpCmd_t  *pNxtOpIfOK;              // W:  cond ptr to next op
  uint32     *pValue;                  // W:  ptr to read from or offset
  uint32      mask;                    // W:  bit mask to apply before compare
  uint32      compVal;                 // W:  value to compare to
} rfOpCmd_patternCk_t;
#endif // CC26XX/CC13XX

/*
** Radio Protocol Independent Immediate Commands
*/

// CMD_UPDATED_RADIO_SETUP: Radio Update Radio Setup Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;          // unused
  regOverride_t    *pRegOverride;      // W:  ptr to list of HW regs to override
} rfOpImmedCmd_UpdateRadioSetup_t;

// CMD_TRIGGER: Radio Generate Command Trigger Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint8             triggerNum;        // W:  command trigger number
} rfOpImmedCmd_GenTrig_t;

// CMD_GET_FW_INFO: Radio Request Firmware Information Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            verNum;            // R:  FW version number
  uint16            startRamOffset;    // R:  offset of FW free C0 RAM
  uint16            freeRamSize;       // R:  size of FW free C0 RAM
  uint16            availRatChans;     // R:  bitmap of available RAT chans
} rfOpImmedCmd_ReqInfo_t;

// CMD_READ/WRITE_RFREG: Radio Read/Write RF Core Register Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            address;           // W:  RF core HW reg bank offset
  uint32            value;             // RW: value of reg read or to write
} rfOpImmedCmd_RW_RfReg_t;

// CMD_MODIFY_RFREG: Radio Modify RF Core Register Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            address;           // W:  RF core HW reg bank offset
  uint32            clrMask;           // W:  bitmask of bits to clear in reg
  uint32            setMask;           // W:  bitmask of bits to set in reg
} rfOpImmedCmd_ModReg_t;

// CMD_READ/WRITE_FWPAR: Radio Read/Write RF Core FW Configuration Parameter
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            address;           // W:  parameter structure index
  uint32            value;             // RW: parameter value read or to write
} rfOpImmedCmd_RW_FwParam_t;

// CMD_ENABLE/DISABLE_DBG: Radio Enable Debug Trace Module Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  dbgTraceCfg_t     dbgTraceCfg;       // W:  debug trace cfg
} rfOpImmedCmd_EnabDbgTrace_t;

// CMD_READ_FS_CAL: Radio Read Last Frequency Calibration Result
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint8             coarseCal;         // R:  coarse calibration result
  uint8             midCal;            // R:  mid-calibration result
  uint16            tdcCal;            // R:  TDC calibration result
} rfOpImmedCmd_LastCal_t;

// CMD_SET_RAT_CMP: Radio Set RAT Channel To Compare Mode Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint8             ratChan;           // W:  radio timer chan number
  uint8             reserved;          // unused
  uint32            compTime;          // W:  time at which compare occurs
} rfOpImmedCmd_RatChanComp_t;

// CMD_SET_RAT_CPT: Radio Set RAT Channel To Capture Mode Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  ratCapCfg_t       ratCapCfg;         // W:  radio timer capture cfg
} rfOpImmedCmd_RatChanCapt_t;

// CMD_SET_RAT_OUTPUT: Radio Set RAT Output To A Specified Mode Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  ratOutModeCfg_t   ratOutModeCfg;     // W:  output event/mode/chan
} rfOpImmedCmd_RatOutChan_t;

// CMD_DISABLE/ARM/DISARM_RAT_CH: Radio Modify RAT Channel Command
// Note: This applies to Disable, Arm, and Disarm RAT Channel commands.
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint32            ratChan;           // W:  radio timer chan number
} rfOpImmedCmd_ModRatChan_t;

// CMD_FORCE_CLK_ENA: Radio Force Clock Enable
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            clkEnab;           // W:  bit mask for PWMCLKENABLE
} rfOpImmedCmd_ForceClkEnab_t;

#if defined( CC26XX ) || defined( CC13XX ) || defined( CC13X2P )
// CMD_MEMCPY: Radio Copy Memory
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            nWords;            // W:  num 32 bit words to copy
  uint32           *pDestAddr;         // W:  destination address
  uint32           *pSrcAddr;          // W:  source address
} rfOpImmedCmd_MemCpy_t;

// CMD_MEMSET: Radio Set Memory
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            nWords;            // W:  num 32 bit words to copy
  uint32           *pDestAddr;         // W:  destination address
  uint32            value;             // W:  value used to fill
} rfOpImmedCmd_MemSet_t;

// CMD_SET_TX_POWER: Radio Set Tx Power
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            txPower;           // W:  IB, GC, tempCoeff
} rfOpImmedCmd_SetTxPwr_t;

// CMD_SET_TX20_POWER: Radio Set Tx20 Power
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;
  uint32            tx20Power;         // W:  HP PA
} rfOpImmedCmd_SetTx20Pwr_t;

// CMD_UPDATE_FS: Update FS Without Recalibration
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            freq;              // W:  freq in Mhz
  uint16            fractFreq;         // W:  fractional freq
} rfOpImmedCmd_UpdateFS_t;

// CMD_UPDATE_BAW_FREQ: Set New Frequency Offset For BAW
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            freqOffset;        // W:  relative offset scaled by 2^-22
} rfOpImmedCmd_UpdateBawFreq_t;

// CMD_BUS_REQUEST: Request System Bus For RF Core
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint8             sysBusNeeded;      // W:  sys bus sleep or access needed
} rfOpImmedCmd_BusReq_t;

// CMD_Set_TRIM: Set Trim Values
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;          // unused
  uint32            cfgIfAdc;          // W:  FCFG1.CONFIG_IF_ADC
  uint32            cfgRfFrontEnd;     // W:  FCFG1.CONFIG_RF_FRONTEND
  uint32            cfgSynth;          // W:  FCFG1.CONFIG_SYNTH
  uint32            cfgMiscAdc;        // W:  FCFG1.CONFIG_MISC_ADC
} rfOpImmedCmd_SetTrim_t;

#endif // CC26XX/CC13XX/CC13X2P

// CMD_READ/WRITE_ADI0/1_REG: Radio Read/Write ADI Register Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint8             adiAddr;           // W:  ADI reg
  uint8             adiValue;          // RW: value of reg read or to write
} rfOpImmedCmd_RW_AdiReg_t;

/*
** Radio Immediate Data Queue Commands
*/

// CMD_ADD/REMOVE/FLUSH_DATA_ENTRY: Radio Add/Remove/Flush Data Queue Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;          // unused
  dataEntryQ_t     *pQueue;            // W:  ptr to queue structure
  uint8            *pEntry;            // W:  ptr to dataEntry_t or dataEntryExt_t
} rfOpImmedCmd_AddRemoveFlushQueue_t;

// CMD_CLEAR_RX: Radio Clear Data Queue Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;          // unused
  dataEntryQ_t     *pQueue;            // W: ptr to queue structure
} rfOpImmedCmd_ClrQueue_t;

// CMD_CLEAR_RX: Radio Clear Data Queue Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16            cmdNum;            // W:  radio command number
  uint16            reserved;          // unused
  dataEntryQ_t     *pQueue;            // W:  ptr to queue structure
  dataEntry_t      *pFirstEntry;       // R:  ptr to first entry removed
} rfOpImmedCmd_rmvPendingEntries_t;

/*
** Miscellaneous
*/

// Software RAT Channels
typedef void (*ratChanCBack_t)(void);

typedef struct
{
  uint8             ratChanNum;
  uint8             ratChanStat;
  ratChanCBack_t    ratChanCBack;
} ratChan_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern rfOpImmedCmd_ReqInfo_t fwInfoCmd;
extern ratChan_t              ratChanInfo[];

/*******************************************************************************
 * External API
 */

extern uint8         RFHAL_InitRfHal( void );

extern uint8         RFHAL_AllocRatChan( void );

extern uint8         RFHAL_FreeRatChan( uint8 ratChanNum );

extern uint32        RFHAL_MapRatChansToInt( void );

extern uint8         RFHAL_RegisterRatChanCallback( uint8          ratChanNum,
                                                    ratChanCBack_t ratChanCBack );

extern rfhalStat_t   RFHAL_AddTxDataEntry( dataEntryQ_t *pDataEntryQ,
                                           dataEntry_t  *pDataEntry,
                                           uint8         rfCoreState );

extern void          RFHAL_InitDataQueue( dataEntryQ_t *pDataEntryQ );

extern void          RFHAL_FreeNextTxDataEntry( dataEntryQ_t *pDataEntryQ );

extern dataEntryQ_t *RFHAL_AllocDataEntryQueue( dataEntry_t *pFirstDataEntry,
                                                dataEntry_t *pLastDataEntry );

extern void          RFHAL_FreeDataEntryQueue( dataEntryQ_t *pDataEntryQ );

extern dataEntry_t  *RFHAL_GetNextDataEntry( dataEntryQ_t *pDataEntryQ );

extern dataEntry_t  *RFHAL_GetTempDataEntry( dataEntryQ_t *pDataEntryQ );

extern void          RFHAL_NextDataEntryDone( dataEntryQ_t *pDataEntryQ );

extern uint8        *RFHAL_AllocDataEntry( uint8  entryType,
                                           uint8  lenSize,
                                           uint16 dataSize );

extern void          RFHAL_FreeDataEntry( uint8 *pDataEntry );

extern uint8        *RFHAL_BuildRingBuffer( uint8  entryType,
                                            uint8  lenSize,  // how about addLenSize?
                                            uint8  numEntries,
                                            uint16 dataSize );

extern dataEntry_t  *RFHAL_BuildDataEntryRingBuffer( uint8  numEntries,
                                                     uint8  prefixSize,
                                                     uint16 dataSize,
                                                     uint8  suffixSize );

extern uint8        *RFHAL_BuildLinkedBuffer( uint8  entryType,
                                              uint8  lenSize,
                                              uint8  numEntries,
                                              uint16 dataSize );

/*******************************************************************************
 */

#endif /* RF_HAL_H */
