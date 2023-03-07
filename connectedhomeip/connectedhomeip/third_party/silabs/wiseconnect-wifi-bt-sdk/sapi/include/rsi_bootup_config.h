/*******************************************************************************
* @file  rsi_bootup_config.h
* @brief 
*******************************************************************************
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

#ifndef RSI_BOOTUP_CONFIG_H
#define RSI_BOOTUP_CONFIG_H
/******************************************************
 * *                      Macros
 * ******************************************************/

#include <stdint.h>

//Select Default image
#define SELECT_DEFAULT_NWP_FW '5'

// Check CRC
#define CHECK_NWP_INTEGRITY 'K'

#define CMD_PASS                  0xAA
#define CMD_FAIL                  0xCC
#define ENABLE_GPIO_BASED_BYPASS  '7'
#define DISABLE_GPIO_BASED_BYPASS '8'
#define INVALID_DEFAULT_IMAGE     0xF5
#define RSI_HOST_INTF_REG_OUT     0x4105003C
#define RSI_HOST_INTF_REG_IN      0x41050034
#define RSI_HOST_INTF_STATUS_REG  0x41050004
#ifdef RSI_M4_INTERFACE
#define RSI_PING_BUFFER_ADDR 0x418000
#define RSI_PONG_BUFFER_ADDR 0x419000
#else
#ifdef CHIP_9117
#define RSI_PING_BUFFER_ADDR 0x59400
#define RSI_PONG_BUFFER_ADDR 0x5A400
#else
#define RSI_PING_BUFFER_ADDR 0x18000
#define RSI_PONG_BUFFER_ADDR 0x19000
#endif
#endif

#define RSI_PING_PONG_CHUNK_SIZE 4096
#define RSI_SDIO_INTF_STATUS_REG 0xF9
#define RSI_BOARD_READY          0xABCD
#define RSI_REG_READ             0xD1
#define RSI_REG_WRITE            0xD2
#define RSI_SEND_RPS_FILE        '2'
#define RSI_FWUP_SUCCESSFUL      'S'
#define RSI_EOF_REACHED          'E'
#define RSI_PONG_VALID           'O'
#define RSI_PING_VALID           'I'
#define RSI_PONG_WRITE           0xD4
#define RSI_PING_WRITE           0xD5
#define RSI_PONG_AVAIL           'O'
#define RSI_PING_AVAIL           'I'

#define RSI_MIN_CHUNK_SIZE   4096
#define RSI_FW_START_OF_FILE BIT(0)
#define RSI_FW_END_OF_FILE   BIT(1)

#define RSI_HOST_INTERACT_REG_VALID    (0xAB << 8)
#define RSI_HOST_INTERACT_REG_VALID_FW (0xA0 << 8)

#define RSI_LOADING_INITIATED '1'

#define RSI_EOF_REACHED                          'E'
#define RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED 0xF1
#define RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL         0xF2
#define RSI_INVALID_OPTION                       0xF3
#define RSI_CHECKSUM_SUCCESS                     0xAA
#define RSI_CHECKSUM_FAILURE                     0xCC
#define RSI_CHECKSUM_INVALID_ADDRESS             0x4C
#define VALID_FIRMWARE_NOT_PRESENT               0x23
#define RSI_BOOTLOADER_VERSION_1P0               0x10
#define RSI_BOOTLOADER_VERSION_1P1               0x11
#define RSI_ROM_VERSION_1P0                      1
#define RSI_ROM_VERSION_1P1                      2

//SPI Internal Register Offset
#define RSI_SPI_INT_REG_ADDR    0x00 //@ register access method
#define RSI_SPI_MODE_REG_ADDR   0x08 //@ register access method
#define RSI_SPI_LENGTH_REG_ADDR 0x20

int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags);
int32_t rsi_device_deinit(void);
int32_t rsi_device_init(uint8_t select_option);
int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data);
int16_t rsi_bl_waitfor_boardready(void);
int16_t rsi_bl_select_option(uint8_t cmd);
int32_t rsi_bl_module_power_off(void);
int32_t rsi_bl_module_power_on(void);
int16_t rsi_bl_module_power_cycle(void);
int32_t rsi_set_fast_fw_up(void);
int16_t rsi_waitfor_boardready(void);
int16_t rsi_select_option(uint8_t cmd);
int32_t rsi_get_rom_version(void);
int32_t rsi_get_ram_dump(uint32_t addr, uint16_t length, uint8_t *buf);

#endif
