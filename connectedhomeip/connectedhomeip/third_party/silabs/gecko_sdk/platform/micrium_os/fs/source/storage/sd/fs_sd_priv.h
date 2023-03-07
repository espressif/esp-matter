/***************************************************************************//**
 * @file
 * @brief File System - Sdmmc Media Driver
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_SD_PRIV_H_
#define  FS_SD_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/logging/logging_priv.h>
#include  <fs/include/fs_sd.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
#include  <fs/source/shared/crc/edc_crc.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------------- CARD TYPES --------------------
#define  FS_DEV_SD_CARDTYPE_NONE                           0u   // Card type unspecified.
#define  FS_DEV_SD_CARDTYPE_SD_V1_X                        1u   // v1.x SD card.
#define  FS_DEV_SD_CARDTYPE_SD_V2_0                        2u   // v2.0 standard-capacity SD card.
#define  FS_DEV_SD_CARDTYPE_SD_V2_0_HC                     3u   // v2.0 high-capacity SD card.
#define  FS_DEV_SD_CARDTYPE_MMC                            4u   // Standard-capacity MMC (< 2GB).
#define  FS_DEV_SD_CARDTYPE_MMC_HC                         5u   // High-capacity MMC (>= 2GB).

//                                                                 ------------------ CARD REGISTERS ------------------
#define  FS_DEV_SD_REG_OCR                                 1u   // Operation Conditions Register.
#define  FS_DEV_SD_REG_CID                                 2u   // Card Identification Register.
#define  FS_DEV_SD_REG_CSD                                 3u   // Card-Specific Data Register.
#define  FS_DEV_SD_REG_RCA                                 4u   // Relative Card Address Register.
#define  FS_DEV_SD_REG_DSR                                 5u   // Driver Stage Register.
#define  FS_DEV_SD_REG_SCR                                 6u   // SD Card Configuration Register (SD only).
#define  FS_DEV_SD_REG_EXT_CSD                             7u   // Extended CSD Register (MMC only).

//                                                                 --------------- CARD REGISTER LENGTHS --------------
#define  FS_DEV_SD_OCR_REG_LEN                             4u   // Length of OCR register, in bytes.
#define  FS_DEV_SD_CID_REG_LEN                            16u   // Length of CID register, in bytes.
#define  FS_DEV_SD_CSD_REG_LEN                            16u   // Length of CSD register, in bytes.
#define  FS_DEV_SD_RCA_REG_LEN                             2u   // Length of RCA register, in bytes.
#define  FS_DEV_SD_DSR_REG_LEN                             2u   // Length of DSR register, in bytes.
#define  FS_DEV_SD_CID_SCR_LEN                             8u   // Length of SCR register, in bytes.
#define  FS_DEV_SD_CID_EXT_CSD_LEN                       512u   // Length of extended CSD register, in bytes.

#define  FS_SD_DFLT_CLK_SPD                           400000u   // Dflt clk spd, in Hz.

#define  FS_DEV_SD_BLK_SIZE_LOG2                           9u
#define  FS_DEV_SD_BLK_SIZE                              (1 << FS_DEV_SD_BLK_SIZE_LOG2)

#define  FS_DEV_SD_RESP_MAX_LEN                           16u   // Max resp len, in bytes.

/********************************************************************************************************
 *                                   SD COMMAND ARGUMENT BIT DEFINES
 *******************************************************************************************************/

//                                                                 ----------- ACMD41 ARGUMENT BIT DEFINES ------------
#define  FS_DEV_SD_ACMD41_HCS                    DEF_BIT_30

//                                                                 ------------ CMD8  ARGUMENT BIT DEFINES ------------
#define  FS_DEV_SD_CMD8_VHS_27_36_V              DEF_BIT_08
#define  FS_DEV_SD_CMD8_VHS_LOW                  DEF_BIT_09

#define  FS_DEV_SD_CMD8_CHK_PATTERN                     0xAAu

//                                                                 ------------ CMD59 ARGUMENT BIT DEFINES ------------
#define  FS_DEV_SD_CMD59_CRC_OPT                 DEF_BIT_00

//                                                                 ------------ ACMD06 ARGUMENT BIT DEFINES -----------
#define  FS_DEV_SD_ACMD06_DATA_BUS_WIDTH_1_BIT   0x00000000u
#define  FS_DEV_SD_ACMD06_DATA_BUS_WIDTH_4_BIT   0x00000002u

/********************************************************************************************************
 *                                               SD COMMANDS
 *
 * Note(s) : (1) Detailed command descriptions are given in [Ref 1], Section 4.7.5, and [Ref 2], Section 7.9.4.
 *
 *           (2) (a) Commands that are not supported in SPI mode include :
 *
 *                       CMD2        CMD11       CMD39
 *                       CMD3        CMD15       CMD40
 *                       CMD4        CMD20
 *                       CMD7        CMD26
 *
 *               (b) Commands that are reserved for I/O mode include :
 *
 *                       CMD5
 *                       CMD52
 *                       CMD53
 *                       CMD54
 *
 *               (c) Reserved commands include :
 *
 *                       CMD14       CMD37       CMD50
 *                       CMD19       CMD41       CMD51
 *                       CMD21       CMD43       CMD57
 *                       CMD22       CMD44       CMD60
 *                       CMD23       CMD45       CMD61
 *                       CMD31       CMD46       CMD62
 *                       CMD34       CMD47       CMD63
 *                       CMD35       CMD48
 *                       CMD36       CMD49
 *******************************************************************************************************/

#define  FS_DEV_SD_CMD_CMD00                                0u
#define  FS_DEV_SD_CMD_GO_IDLE_STATE         FS_DEV_SD_CMD_CMD00 // Resets all cards to idle state.
#define  FS_DEV_MMC_CMD_GO_IDLE_STATE        FS_DEV_SD_CMD_CMD00

#define  FS_DEV_SD_CMD_CMD01                                1u
#define  FS_DEV_SD_CMD_SEND_OP_COND          FS_DEV_SD_CMD_CMD01 // Sends host cap supp info & act card's init.
#define  FS_DEV_MMC_CMD_SEND_OP_COND         FS_DEV_SD_CMD_CMD01

#define  FS_DEV_SD_CMD_CMD02                                2u
#define  FS_DEV_SD_CMD_ALL_SEND_CID          FS_DEV_SD_CMD_CMD02 // Asks any card to send CID numbers on CMD line.
#define  FS_DEV_MMC_CMD_ALL_SEND_CID         FS_DEV_SD_CMD_CMD02

#define  FS_DEV_SD_CMD_CMD03                                3u
#define  FS_DEV_SD_CMD_SEND_RELATIVE_ADDR    FS_DEV_SD_CMD_CMD03 // Asks card to publish new RCA.
#define  FS_DEV_MMC_CMD_SEND_RELATIVE_ADDR   FS_DEV_SD_CMD_CMD03

#define  FS_DEV_SD_CMD_CMD04                                4u
#define  FS_DEV_SD_CMD_SET_DSR               FS_DEV_SD_CMD_CMD04 // Programs the DSR of all cards.
#define  FS_DEV_MMC_CMD_SET_DSR              FS_DEV_SD_CMD_CMD04

#define  FS_DEV_SD_CMD_CMD06                                6u
#define  FS_DEV_SD_CMD_SWITCH_FUNC           FS_DEV_SD_CMD_CMD06 // Switches card function.
#define  FS_DEV_MMC_CMD_SWITCH_FUNC          FS_DEV_SD_CMD_CMD06

#define  FS_DEV_SD_CMD_CMD07                                7u
#define  FS_DEV_SD_CMD_SEL_DESEL_CARD        FS_DEV_SD_CMD_CMD07 // Toggle card between stand-by & transfer states.
#define  FS_DEV_MMC_CMD_SEL_DESEL_CARD       FS_DEV_SD_CMD_CMD07

#define  FS_DEV_SD_CMD_CMD08                                8u
#define  FS_DEV_SD_CMD_SEND_IF_COND          FS_DEV_SD_CMD_CMD08 // Sends SD interface conditions.
#define  FS_DEV_MMC_CMD_SEND_EXT_CSD         FS_DEV_SD_CMD_CMD08

#define  FS_DEV_SD_CMD_CMD09                                9u
#define  FS_DEV_SD_CMD_SEND_CSD              FS_DEV_SD_CMD_CMD09 // Addr'd card sends Card-Specific Data (CSD).
#define  FS_DEV_MMC_CMD_SEND_CSD             FS_DEV_SD_CMD_CMD09

#define  FS_DEV_SD_CMD_CMD10                               10u
#define  FS_DEV_SD_CMD_SEND_CID              FS_DEV_SD_CMD_CMD10 // Addr'd card sends Card Identification (CID).
#define  FS_DEV_MMC_CMD_SEND_CID             FS_DEV_SD_CMD_CMD10

#define  FS_DEV_SD_CMD_CMD11                               11u
#define  FS_DEV_SD_CMD_READ_DAT_UNTIL_STOP   FS_DEV_SD_CMD_CMD11 // Read data until stop transmission.
#define  FS_DEV_MMC_CMD_READ_DAT_UNTIL_STOP  FS_DEV_SD_CMD_CMD11

#define  FS_DEV_SD_CMD_CMD12                               12u
#define  FS_DEV_SD_CMD_STOP_TRANSMISSION     FS_DEV_SD_CMD_CMD12 // Forces the card to stop transmission.
#define  FS_DEV_MMC_CMD_STOP_TRANSMISSION    FS_DEV_SD_CMD_CMD12

#define  FS_DEV_SD_CMD_CMD13                               13u
#define  FS_DEV_SD_CMD_SEND_STATUS           FS_DEV_SD_CMD_CMD13 // Addressed card sends its status register.
#define  FS_DEV_MMC_CMD_SEND_STATUS          FS_DEV_SD_CMD_CMD13

#define  FS_DEV_SD_CMD_CMD14                               14u
#define  FS_DEV_SD_CMD_BUSTEST_R             FS_DEV_SD_CMD_CMD14 // Read the reversed bus testing data pattern.
#define  FS_DEV_MMC_CMD_BUSTEST_R            FS_DEV_SD_CMD_CMD14

#define  FS_DEV_SD_CMD_CMD15                               15u
#define  FS_DEV_SD_CMD_GO_INACTIVE_STATE     FS_DEV_SD_CMD_CMD15 // Sends an addressed card into the inactive state.
#define  FS_DEV_MMC_CMD_GO_INACTIVE_STATE    FS_DEV_SD_CMD_CMD15

#define  FS_DEV_SD_CMD_CMD16                               16u
#define  FS_DEV_SD_CMD_SET_BLOCKLEN          FS_DEV_SD_CMD_CMD16 // Sets the block length in block (std cap SD).
#define  FS_DEV_MMc_CMD_SET_BLOCKLEN         FS_DEV_SD_CMD_CMD16

#define  FS_DEV_SD_CMD_CMD17                               17u
#define  FS_DEV_SD_CMD_READ_SINGLE_BLOCK     FS_DEV_SD_CMD_CMD17 // Reads a block the size sel'd by SET_BLOCKLEN.
#define  FS_DEV_MMC_CMD_READ_SINGLE_BLOCK    FS_DEV_SD_CMD_CMD17

#define  FS_DEV_SD_CMD_CMD18                               18u
#define  FS_DEV_SD_CMD_READ_MULTIPLE_BLOCK   FS_DEV_SD_CMD_CMD18 // Continuously xfers data blks until STOP_TRAN.
#define  FS_DEV_MMC_CMD_READ_MULTIPLE_BLOCK  FS_DEV_SD_CMD_CMD18

#define  FS_DEV_SD_CMD_CMD19                               19u
#define  FS_DEV_SD_CMD_BUSTEST_W             FS_DEV_SD_CMD_CMD19 // Send the bus test data pattern to a card.
#define  FS_DEV_MMC_CMD_BUSTEST_W            FS_DEV_SD_CMD_CMD19

#define  FS_DEV_SD_CMD_CMD20                               20u
#define  FS_DEV_SD_CMD_WRITE_DAT_UNTIL_STOP  FS_DEV_SD_CMD_CMD20 // Write a data stream until stop transmission.
#define  FS_DEV_MMC_CMD_WRITE_DAT_UNTIL_STOP FS_DEV_SD_CMD_CMD20

#define  FS_DEV_SD_CMD_CMD23                               23u
#define  FS_DEV_SD_CMD_SET_BLOCK_COUNT       FS_DEV_SD_CMD_CMD23 // Define the number of blocks to be transferred.
#define  FS_DEV_MMC_CMD_SET_BLOCK_COUNT      FS_DEV_SD_CMD_CMD23

#define  FS_DEV_SD_CMD_CMD24                               24u
#define  FS_DEV_SD_CMD_WRITE_BLOCK           FS_DEV_SD_CMD_CMD24 // Writes a block the size sel'd by SET_BLOCKLEN.
#define  FS_DEV_MMC_CMD_WRITE_BLOCK          FS_DEV_SD_CMD_CMD24

#define  FS_DEV_SD_CMD_CMD25                               25u
#define  FS_DEV_SD_CMD_WRITE_MULTIPLE_BLOCK  FS_DEV_SD_CMD_CMD25 // Continuously writes data blks until STOP_TRAN.
#define  FS_DEV_MMC_CMD_WRITE_MULTIPLE_BLOCK FS_DEV_SD_CMD_CMD25

#define  FS_DEV_SD_CMD_CMD26                               26u
#define  FS_DEV_SD_CMD_PROGRAM_CID           FS_DEV_SD_CMD_CMD26 // Programming of the CID.
#define  FS_DEV_MMC_CMD_PROGRAM_CID          FS_DEV_SD_CMD_CMD26

#define  FS_DEV_SD_CMD_CMD27                               27u
#define  FS_DEV_SD_CMD_PROGRAM_CSD           FS_DEV_SD_CMD_CMD27 // Programming of the programmable bits of the CSD.
#define  FS_DEV_MMC_CMD_PROGRAM_CSD          FS_DEV_SD_CMD_CMD27

#define  FS_DEV_SD_CMD_CMD28                               28u
#define  FS_DEV_SD_CMD_SET_WRITE_PROT        FS_DEV_SD_CMD_CMD28 // Sets the write protection bit of addr'd group.
#define  FS_DEV_MMC_CMD_SET_WRITE_PROT       FS_DEV_SD_CMD_CMD28

#define  FS_DEV_SD_CMD_CMD29                               29u
#define  FS_DEV_SD_CMD_CLR_WRITE_PROT        FS_DEV_SD_CMD_CMD29 // Clrs the write protection bit of addr'd group.
#define  FS_DEV_MMC_CMD_CLR_WRITE_PROT       FS_DEV_SD_CMD_CMD29

#define  FS_DEV_SD_CMD_CMD30                               30u
#define  FS_DEV_SD_CMD_SEND_WRITE_PROT       FS_DEV_SD_CMD_CMD30 // Asks card to send status of wr protection bits.
#define  FS_DEV_MMC_CMD_SEND_WRITE_PROT      FS_DEV_SD_CMD_CMD30

#define  FS_DEV_SD_CMD_CMD32                               32u
#define  FS_DEV_SD_CMD_ERASE_WR_BLK_START    FS_DEV_SD_CMD_CMD32 // Sets addr of 1st wr blk to be erased.
#define  FS_DEV_MMC_CMD_ERASE_WR_BLK_START   FS_DEV_SD_CMD_CMD32

#define  FS_DEV_SD_CMD_CMD33                               33u
#define  FS_DEV_SD_CMD_ERASE_WR_BLK_END      FS_DEV_SD_CMD_CMD33 // Sets addr of last wr blk to be erased.
#define  FS_DEV_MMC_CMD_ERASE_WR_BLK_END     FS_DEV_SD_CMD_CMD33

#define  FS_DEV_SD_CMD_CMD35                               35u
#define  FS_DEV_SD_CMD_ERASE_GROUP_START     FS_DEV_SD_CMD_CMD35 // Sets address of first erase group within a range.
#define  FS_DEV_MMC_CMD_ERASE_GROUP_START    FS_DEV_SD_CMD_CMD35

#define  FS_DEV_SD_CMD_CMD36                               36u
#define  FS_DEV_SD_CMD_ERASE_GROUP_END       FS_DEV_SD_CMD_CMD36 // Sets address of last  erase group within a range.
#define  FS_DEV_MMC_CMD_ERASE_GROUP_END      FS_DEV_SD_CMD_CMD36

#define  FS_DEV_SD_CMD_CMD38                               38u
#define  FS_DEV_SD_CMD_ERASE                 FS_DEV_SD_CMD_CMD38 // Erases all prev sel'd wr blks.
#define  FS_DEV_MMC_CMD_ERASE                FS_DEV_SD_CMD_CMD38

#define  FS_DEV_SD_CMD_CMD39                               39u
#define  FS_DEV_SD_CMD_FAST_IO               FS_DEV_SD_CMD_CMD39 // Used to write & read 8-bit data fields.
#define  FS_DEV_MMC_CMD_FAST_IO              FS_DEV_SD_CMD_CMD39

#define  FS_DEV_SD_CMD_CMD40                               40u
#define  FS_DEV_SD_CMD_GO_IRQ_STATE          FS_DEV_SD_CMD_CMD40 // Sets the system into interrupt mode.
#define  FS_DEV_MMC_CMD_GO_IRQ_STATE         FS_DEV_SD_CMD_CMD40

#define  FS_DEV_SD_CMD_CMD42                               42u
#define  FS_DEV_SD_CMD_LOCK_UNLOCK           FS_DEV_SD_CMD_CMD42 // Used to set/reset password or lock/unlock card.
#define  FS_DEV_MMC_CMD_LOCK_UNLOCK          FS_DEV_SD_CMD_CMD42

#define  FS_DEV_SD_CMD_CMD55                               55u
#define  FS_DEV_SD_CMD_APP_CMD               FS_DEV_SD_CMD_CMD55 // Indicates that next cmd is app cmd.
#define  FS_DEV_MMC_CMD_APP_CMD              FS_DEV_SD_CMD_CMD55

#define  FS_DEV_SD_CMD_CMD56                               56u
#define  FS_DEV_SD_CMD_GEN_CMD               FS_DEV_SD_CMD_CMD56 // Gets/sends data blk from app cmd.
#define  FS_DEV_MMC_CMD_GEN_CMD              FS_DEV_SD_CMD_CMD56

#define  FS_DEV_SD_CMD_CMD58                               58u
#define  FS_DEV_SD_CMD_READ_OCR              FS_DEV_SD_CMD_CMD58 // Reads OCR register of card.
#define  FS_DEV_MMC_CMD_READ_OCR             FS_DEV_SD_CMD_CMD58

#define  FS_DEV_SD_CMD_CMD59                               59u
#define  FS_DEV_SD_CMD_CRC_ON_OFF            FS_DEV_SD_CMD_CMD59 // Turns the CRC option on or off.
#define  FS_DEV_MMC_CMD_CRC_ON_OFF           FS_DEV_SD_CMD_CMD59

/********************************************************************************************************
 *                                   SD APPLICATION-SPECIFIC COMMANDS
 *
 * Note(s) : (1) Detailed application-specific command descriptions are given in [Ref 1], Section 4.7.5,
 *               Table 4-26.
 *
 *           (2) (a) Application-specific commands that are not supported in SPI mode include :
 *
 *                       ACMD6
 *
 *               (b) Commands that are reserved for SD security applications include :
 *
 *                       ACMD18      ACMD43      ACMD47
 *                       ACMD25      ACMD44      ACMD48
 *                       ACMD26      ACMD45      ACMD49
 *                       ACMD38      ACMD46
 *
 *               (c) Reserved commands include :
 *
 *                       ACMD17      ACMD24
 *                       ACMD19      ACMD39
 *                       ACMD20      ACMD40
 *                       AMCD21
 *******************************************************************************************************/

#define  FS_DEV_SD_ACMD_ACMD06                              6u
#define  FS_DEV_SD_ACMD_BUS_WIDTH              FS_DEV_SD_ACMD_ACMD06 // Define the data bus width for data transfer.

#define  FS_DEV_SD_ACMD_ACMD13                             13u
#define  FS_DEV_SD_ACMD_SD_STATUS              FS_DEV_SD_ACMD_ACMD13 // Send the SD status.

#define  FS_DEV_SD_ACMD_ACMD22                             22u
#define  FS_DEV_SD_ACMD_SEND_NUM_WR_BLOCKS     FS_DEV_SD_ACMD_ACMD22 // Send the nbr of wr'n wr blks.

#define  FS_DEV_SD_ACMD_ACMD23                             23u
#define  FS_DEV_SD_ACMD_SET_WR_BLK_ERASE_COUNT FS_DEV_SD_ACMD_ACMD23 // Send the nbr of wr blks to be pre-erased.

#define  FS_DEV_SD_ACMD_ACMD41                             41u
#define  FS_DEV_SD_ACMD_SD_SEND_OP_COND        FS_DEV_SD_ACMD_ACMD41 // Sends host capacity support & gets OCR.

#define  FS_DEV_SD_ACMD_ACMD42                             42u
#define  FS_DEV_SD_ACMD_SET_CLR_CARD_DETECT    FS_DEV_SD_ACMD_ACMD42 // Conn/disconn 50 kOhm res on DAT3.

#define  FS_DEV_SD_ACMD_ACMD51                             51u
#define  FS_DEV_SD_ACMD_SEND_SCR               FS_DEV_SD_ACMD_ACMD51 // Reads the SD configuration register.

/********************************************************************************************************
 *                                       OCR REGISTER BIT DEFINES
 *
 * Note(s) : (1) See [Ref 1], Table 5-1, & [Ref 2], Section 8.1.
 *******************************************************************************************************/

#define  FS_DEV_SD_OCR_LVR                       DEF_BIT_07          // Card is dual-voltage.
#define  FS_DEV_SD_OCR_20_21V                    DEF_BIT_08          // Card supports 2.0-2.1V.
#define  FS_DEV_SD_OCR_21_22V                    DEF_BIT_09          // Card supports 2.1-2.2V.
#define  FS_DEV_SD_OCR_22_33V                    DEF_BIT_10          // Card supports 2.2-2.3V.
#define  FS_DEV_SD_OCR_23_24V                    DEF_BIT_11          // Card supports 2.3-2.4V.
#define  FS_DEV_SD_OCR_24_25V                    DEF_BIT_12          // Card supports 2.4-2.5V.
#define  FS_DEV_SD_OCR_25_26V                    DEF_BIT_13          // Card supports 2.5-2.6V.
#define  FS_DEV_SD_OCR_26_27V                    DEF_BIT_14          // Card supports 2.6-2.7V.
#define  FS_DEV_SD_OCR_27_28V                    DEF_BIT_15          // Card supports 2.7-2.8V.
#define  FS_DEV_SD_OCR_28_29V                    DEF_BIT_16          // Card supports 2.8-2.9V.
#define  FS_DEV_SD_OCR_29_30V                    DEF_BIT_17          // Card supports 2.9-3.0V.
#define  FS_DEV_SD_OCR_30_31V                    DEF_BIT_18          // Card supports 3.0-3.1V.
#define  FS_DEV_SD_OCR_31_32V                    DEF_BIT_19          // Card supports 3.1-3.2V.
#define  FS_DEV_SD_OCR_32_33V                    DEF_BIT_20          // Card supports 3.2-3.3V.
#define  FS_DEV_SD_OCR_33_34V                    DEF_BIT_21          // Card supports 3.3-3.4V.
#define  FS_DEV_SD_OCR_34_35V                    DEF_BIT_22          // Card supports 3.4-3.5V.
#define  FS_DEV_SD_OCR_35_36V                    DEF_BIT_23          // Card supports 3.5-3.6V.
#define  FS_DEV_SD_OCR_CCS                       DEF_BIT_30          // HC card.
#define  FS_DEV_SD_OCR_BUSY                      DEF_BIT_31          // Card has finished power-up routine.

#define  FS_DEV_SD_OCR_VOLTAGE_MASK             (FS_DEV_SD_OCR_27_28V | FS_DEV_SD_OCR_28_29V | FS_DEV_SD_OCR_29_30V   \
                                                 | FS_DEV_SD_OCR_30_31V | FS_DEV_SD_OCR_31_32V | FS_DEV_SD_OCR_32_33V \
                                                 | FS_DEV_SD_OCR_33_34V | FS_DEV_SD_OCR_34_35V | FS_DEV_SD_OCR_35_36V)
#define  FS_DEV_SD_OCR_ACCESS_MODE_SEC           DEF_BIT_30     // Access mode: sector (for MMC only).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_sd {
  FS_MEDIA            Media;
  CPU_BOOLEAN         IsClosed;
  FS_BLK_DEV          BlkDev;
  FS_SD_INFO          Info;
  const FS_SD_BUS_API *BusApiPtr;
} FS_SD;

struct fs_sd_bus_api {
  void (*Open)   (FS_SD    *p_sd,
                  RTOS_ERR *p_err);

  void (*Close)  (FS_SD    *p_sd,
                  RTOS_ERR *p_err);

  void (*InfoGet)(FS_SD      *p_sd,
                  FS_SD_INFO *p_info);

  void (*CidRd)  (FS_SD      *p_sd,
                  CPU_INT08U *p_dest,
                  RTOS_ERR   *p_err);

  void (*CsdRd)  (FS_SD      *p_sd,
                  CPU_INT08U *p_dest,
                  RTOS_ERR   *p_err);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL
extern const FS_BLK_DEV_API FS_SD_Card_BlkDevApi;
#endif

#ifdef RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
extern const FS_BLK_DEV_API FS_SD_SPI_BlkDevApi;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_BOOLEAN FS_SD_ParseCSD(CPU_INT08U csd[],
                           FS_SD_INFO *p_sd_info,
                           CPU_INT08U card_type);

void FS_SD_ParseCID(CPU_INT08U cid[],
                    FS_SD_INFO *p_sd_info,
                    CPU_INT08U card_type);

CPU_INT08U FS_SD_ChkSumCalc_7Bit(CPU_INT08U *p_data,
                                 CPU_INT32U size);

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
CPU_INT16U FS_SD_ChkSumCalc_16Bit(CPU_INT08U *p_data,
                                  CPU_INT32U size);
#endif

#if LOG_TO_IS_EN((FS, DRV, SD), VRB)
void FS_SD_TraceInfo(FS_SD_INFO *p_sd_info);
#endif

void FS_SD_ClrInfo(FS_SD_INFO *p_sd_info);

void FS_SD_ParseEXT_CSD(CPU_INT08U ext_csd[],
                        FS_SD_INFO *p_sd_info);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
