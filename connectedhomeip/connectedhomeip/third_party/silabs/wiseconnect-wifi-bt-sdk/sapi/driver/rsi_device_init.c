/*******************************************************************************
* @file  rsi_device_init.c
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

/*
  Include files
 */
#include "rsi_driver.h"
#include "rsi_spi_cmd.h"
#define RSI_SAFE_UPGRADE_ADDR 0x1d408
#define RSI_SECURE_ZONE_ADDR  0x1d418
#define RSI_SAFE_UPGRADE      BIT(12)
#define RSI_SECURE_ZONE       BIT(20)
#define PING_BUFF             0
#define PONG_BUFF             1
#ifndef RSI_M4_INTERFACE
#ifndef RSI_UART_INTERFACE
#ifndef RSI_USB_INTERFACE

#if RSI_FAST_FW_UP
int16_t rsi_secure_ping_pong_wr(uint32_t ping_pong, uint8_t *src_addr, uint16_t size_param);
#endif

/** @addtogroup DRIVER5
* @{
*/
/** @fn          int16_t rsi_secure_ping_pong_wr(uint32_t ping_pong, uint8_t *src_addr, uint16_t size_param)
 * @brief       Write the given data to the specified register address in the Module. This API writes the given data in ping or pong buffer
 * @param[in]   ping_pong  - ping or pong buffer write
 * @param[in]   size_param - Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[in ]  src_addr   - pointer contain the buffer of content to be written
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 *              
 */

#if RSI_FAST_FW_UP
/// @private
int16_t rsi_secure_ping_pong_wr(uint32_t ping_pong, uint8_t *src_addr, uint16_t size_param)
{

  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(ping_pong);

  int16_t retval;
  rsi_frame_desc_t frame_desc;

  *(uint16_t *)frame_desc.frame_len_queue_no = size_param;

  // Write out the Command Frame Descriptor
  retval = rsi_spi_frame_dsc_wr(&frame_desc);

  if (retval != 0x00) {
#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL4, "Descriptor write failErr=%02x", retval);
#endif
    return retval;
  }

  // Write the Command Frame Data
  if (size_param) {
    size_param = (size_param + 3) & ~3;
    retval     = rsi_spi_frame_data_wr(size_param, src_addr, 0, NULL);
    if (retval != 0x00) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL4, "FramePayload writeErr=%02x", retval);
#endif
      return retval;
    }
  }
  return retval;
}
#endif
/** @} */
/** @addtogroup DRIVER6
* @{
*/
/*===========================================================*/
/**
 * @fn          int16_t rsi_bl_module_power_cycle(void)
 * @brief       Power cycle the module.This API is valid only if there is a power gate, external to the module,which is controlling the power 
 *              to the module using a GPIO signal of the MCU.
 * @pre         \ref rsi_driver_init() must be called before this API. 
 * @param[in]   void 
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */

int16_t rsi_bl_module_power_cycle(void)
{
  SL_PRINTF(SL_BL_MODULE_POWER_CYCLE_ENTRY, DRIVER, LOG_INFO);
  // configure Reset pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_RESET_PIN, RSI_HAL_GPIO_OUTPUT_MODE, RSI_HAL_GPIO_LOW);

  // reset/drive low value on the GPIO
  rsi_hal_clear_gpio(RSI_HAL_RESET_PIN);

  rsi_delay_ms(100);
  // Set/drive high value on the GPIO
  rsi_hal_set_gpio(RSI_HAL_RESET_PIN);

  // delay for 100 milli seconds
  rsi_delay_ms(100);

  SL_PRINTF(SL_BL_MODULE_POWER_CYCLE_EXIT, DRIVER, LOG_INFO);
  return RSI_SUCCESS;
}
/** @} */
/** @addtogroup DRIVER5
* @{
*/
/*==============================================*/
/**
 * @fn          int16_t rsi_bl_waitfor_boardready(void)
 * @brief       This API checks for the readiness of the bootloader to receive the commands from the host.
 * @param[in]   void 
 * @return      0              - Success \n
 *              Negative Value - Failure \n
 *                               -3 - Board ready not received \n
 *                               -4 - Bootup options last configuration not saved \n
 *                               -5 - Bootup options checksum failed \n
 *                               -6 - Bootloader version mismatch 
 */
/// @private
int16_t rsi_bl_waitfor_boardready(void)
{
  SL_PRINTF(SL_BL_WAITFOR_BOARDREADY_ENTRY, DRIVER, LOG_INFO);
  int16_t retval      = 0;
  uint16_t read_value = 0;

  retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);

  if (retval < 0) {
    SL_PRINTF(SL_BL_WAITFOR_BOARDREADY_EXIT, DRIVER, LOG_ERROR, "retval: %d", retval);
    return retval;
  }
  if ((read_value & 0xFF00) == (RSI_HOST_INTERACT_REG_VALID & 0xFF00)) {
    if ((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED) {
      return RSI_ERROR_BOOTUP_OPTIONS_NOT_SAVED;
    } else if ((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL) {
      return RSI_ERROR_BOOTUP_OPTIONS_CHECKSUM_FAIL;
    } else if ((read_value & 0xFF) == RSI_BOOTLOADER_VERSION_1P0) {
      rsi_driver_cb_non_rom->rom_version_info = RSI_ROM_VERSION_1P0;
    } else if ((read_value & 0xFF) == RSI_BOOTLOADER_VERSION_1P1) {
      rsi_driver_cb_non_rom->rom_version_info = RSI_ROM_VERSION_1P1;
    }
#if RSI_BOOTLOADER_VERSION_CHECK
    else if ((read_value & 0xFF) == RSI_BOOTLOADER_VERSION) {
    } else {
      SL_PRINTF(SL_BL_WAITFOR_BOARDREADY_BOOTLOADER_VERSION_NOT_MATCHING, DRIVER, LOG_ERROR);
      return RSI_ERROR_BOOTLOADER_VERSION_NOT_MATCHING;
    }
#endif
    SL_PRINTF(SL_BL_WAITFOR_BOARDREADY_SUCCESS, DRIVER, LOG_ERROR);
    return RSI_SUCCESS;
  }
  SL_PRINTF(SL_BL_WAITFOR_BOARDREADY_WAITING_FOR_BOARD_READY, DRIVER, LOG_ERROR);
  return RSI_ERROR_WAITING_FOR_BOARD_READY;
}
/*==============================================*/
/**
 * @fn          int16_t rsi_bl_select_option(uint8_t cmd)
 * @brief       Send firmware load request to module or update default configurations.
 * @param[in]   cmd - type of configuration to be saved \n
 *                    BURN_NWP_FW                    - 0x42 \n
 *                    LOAD_NWP_FW                    - 0x31 \n
 *                    LOAD_DEFAULT_NWP_FW_ACTIVE_LOW - 0x71 \n
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure \n
 *                               -28 - Firmware Load or Upgrade timeout error \n
 *                               -14 - Valid Firmware not present \n
 *                               -15 - Invalid Option
 *  
 */
/// @private
int16_t rsi_bl_select_option(uint8_t cmd)
{
  uint16_t boot_cmd   = 0;
  int16_t retval      = 0;
  uint16_t read_value = 0;
  rsi_timer_instance_t timer_instance;

  retval = rsi_mem_wr(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&boot_cmd);
  if (retval < 0) {
    return retval;
  }

  if (cmd == BURN_NWP_FW) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID_FW | cmd;
  } else {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | cmd;
  }
  retval = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
  if (retval < 0) {
    return retval;
  }

  rsi_init_timer(&timer_instance, 300);

  while ((cmd != LOAD_NWP_FW) && (cmd != LOAD_DEFAULT_NWP_FW_ACTIVE_LOW)) {
    retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
    if (retval < 0) {
      return retval;
    }
    if (cmd == BURN_NWP_FW) {
      if (read_value == (RSI_HOST_INTERACT_REG_VALID | RSI_SEND_RPS_FILE)) {
        break;
      }
    }

    else if (read_value == (RSI_HOST_INTERACT_REG_VALID | cmd)) {
      break;
    }
    if (rsi_timer_expired(&timer_instance)) {
      return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
    }
  }
  if ((cmd == LOAD_NWP_FW) || (cmd == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW)) {
    rsi_init_timer(&timer_instance, 3000);
    do {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if (retval < 0) {
        return retval;
      }
      if ((read_value & 0xF000) == (RSI_HOST_INTERACT_REG_VALID_FW & 0xF000)) {
        if ((read_value & 0xFF) == VALID_FIRMWARE_NOT_PRESENT) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL4, "VALID_FIRMWARE_NOT_PRESENT\n");
#endif
          return RSI_ERROR_VALID_FIRMWARE_NOT_PRESENT;
        }
        if ((read_value & 0xFF) == RSI_INVALID_OPTION) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL4, "INVALID CMD\n");
#endif

          return RSI_ERROR_INVALID_OPTION;
        }
        if ((read_value & 0xFF) == RSI_CHECKSUM_SUCCESS) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL4, "LOAD SUCCESS\n");
#endif
          break;
        }
      }
      if (rsi_timer_expired(&timer_instance)) {
        return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
      }

    } while (1);
  }
  return retval;
}
/** @} */
/** @addtogroup COMMON
* @{
*/
/*==============================================*/
/**
 * @fn          int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
 * @brief       Upgrade the firmware in the WiSeConnect device from the host. The firmware file is given in chunks to this API.
 *              Each chunk must be a multiple of 4096 bytes unless it is the last chunk.For the first chunk, 
 *              set RSI_FW_START_OF_FILE in flags.For the last chunk set RSI_FW_END_OF_FILE in flags. This is blocking API. 
 * @param[in]   firmware_image - This is a pointer to firmware image buffer 
 * @param[in]   fw_image_size  - This is the size of firmware image 
 * @param[in]   flags          - 1 - RSI_FW_START_OF_FILE  \n
 *                               2 - RSI_FW_END_OF_FILE \n
 *                               Set flags to \n
 *                               1 - if it is the first chunk \n
 *                               2 - if it is last chunk \n
 *                               0 - for all other chunks  
 * @note        1. In rsi_bootloader_instructions() API, By default, RSI_HAL_MAX_WR_BUFF_LEN Macro (Max supported write buffer size) is 4096 bytes. \n
 *                 Users can reduce this buffer size (Minimum value of 1600 bytes is expected) \n
 *                 as per HAL memory availability at the cost of the increase in execution time of this API. \n
 *                 This macro can be configured at rsi_user.h \n
 *                 Example: uint8_t dummy[RSI_HAL_MAX_WR_BUFF_LEN];   
 * @note        2. For safe firmware upgrade via the bootloader, \n
 *                 it will take approx. 65 sec duration for upgrading the firmware of 1.5 MB file. 
 * @note        3. For Fast firmware upgrade via the bootloader, \n
 *                 it will take approx. 35 sec duration for upgrading the firmware of 1.5 MB file.                
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 *              
 */

int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags)
{
  SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_ENTRY, COMMON, LOG_INFO);
  static uint16_t boot_cmd;
  uint16_t read_value = 0;
  uint32_t offset     = 0;
  int16_t retval      = 0;
  uint32_t boot_insn = 0, poll_resp = 0;
  rsi_timer_instance_t timer_instance;

  // If it is a start of file set the boot cmd to pong valid
  if (flags & RSI_FW_START_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  // check for invalid packet
  if ((fw_image_size % (RSI_MIN_CHUNK_SIZE) != 0) && (!(flags & RSI_FW_END_OF_FILE))) {
    return RSI_ERROR_INVALID_PACKET;
  }

  // loop to execute multiple of 4K chunks
  while (offset < fw_image_size) {
    switch (boot_cmd) {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;
      default: {
      }
    }

    retval = rsi_bootloader_instructions(boot_insn, (uint16_t *)((uint8_t *)firmware_image + offset));
    if (retval < 0) {
      SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT1, COMMON, LOG_ERROR, "retval: %d", retval);
      return retval;
    }

    rsi_init_timer(&timer_instance, 1000);

    while (1) {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if (retval < 0) {
        SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT2, COMMON, LOG_ERROR, "retval: %d", retval);
        return retval;
      }
      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp)) {
        break;
      }
      if (rsi_timer_expired(&timer_instance)) {
        SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT3, COMMON, LOG_ERROR);
        return RSI_ERROR_FW_UPGRADE_TIMEOUT;
      }
    }
    offset += RSI_MIN_CHUNK_SIZE;
  }

  // For last chunk set boot cmd as End of file reached
  if (flags & RSI_FW_END_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
    if (retval < 0) {
      SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT4, COMMON, LOG_ERROR, "retval: %d", retval);
      return retval;
    }
    rsi_init_timer(&timer_instance, 40000);
    // check for successful firmware upgrade
    do {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if (retval < 0) {
        SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT5, COMMON, LOG_ERROR, "retval: %d", retval);
        return retval;
      }
      if (rsi_timer_expired(&timer_instance)) {
        SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT6, COMMON, LOG_ERROR);
        return RSI_ERROR_FW_UPGRADE_TIMEOUT;
      }

    } while (read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  SL_PRINTF(SL_BL_UPGRADE_FIRMWARE_EXIT5, COMMON, LOG_INFO, "retval: %d", retval);
  return retval;
}
/** @} */
/** @addtogroup DRIVER5
* @{
*/
/**
 * @fn          int32_t rsi_set_fast_fw_up(void)
 * @brief       Fast setting of fw_up
 * @param[in]   void 
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */
///@private
int32_t rsi_set_fast_fw_up(void)
{
  SL_PRINTF(SL_SET_FAST_FW_UP_ENTRY, DRIVER, LOG_INFO);
  uint32_t read_data = 0;
  int32_t retval     = 0;
  retval             = rsi_mem_rd(RSI_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
  if (retval < 0) {
    SL_PRINTF(SL_SDIO_IFACE_INIT_EXIT_1, DRIVER, LOG_ERROR, "retval: %d", retval);
    return retval;
  }
  //disabling safe upgradation bit
  if ((read_data & RSI_SAFE_UPGRADE)) {
    read_data &= ~(RSI_SAFE_UPGRADE);
    retval = rsi_mem_wr(RSI_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
    if (retval < 0) {
      SL_PRINTF(SL_SDIO_IFACE_INIT_EXIT_2, DRIVER, LOG_ERROR, "retval: %d", retval);
      return retval;
    }
  }
  SL_PRINTF(SL_SDIO_IFACE_INIT_EXIT_3, DRIVER, LOG_INFO, "retval: %d", retval);
  return retval;
}
/** @} */
/** @addtogroup DRIVER6
* @{
*/
/*==============================================*/
/**
 * @fn          int32_t rsi_get_rom_version(void)
 * @brief       Get the rom version of the module.
 * @param[in]   void 
 * @return      1 - RSI_ROM_VERSION_1P0 \n 
 *              2 - RSI_ROM_VERSION_1P1 
 */

int32_t rsi_get_rom_version(void)
{
  SL_PRINTF(SL_GET_ROM_VERSION_ENTRY, DRIVER, LOG_INFO);
  return rsi_driver_cb_non_rom->rom_version_info;
}
/** @} */
/** @addtogroup DRIVER5
* @{
*/
/*==============================================*/
/**
 * @fn          int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
 * @brief       Send boot instructions to module.
 * @param[in]   type - type of the insruction to perform \n
 *                     0xD1 - RSI_REG_READ \n
 *                     0xD2 - RSI_REG_WRITE \n
 *                     0xD5 - RSI_PING_WRITE \n
 *                     0xD4 - RSI_PONG_WRITE \n
 *                     0x42 - BURN_NWP_FW \n
 *                     0x31 - LOAD_NWP_FW \n
 *                     0x71 - LOAD_DEFAULT_NWP_FW_ACTIVE_LOW 
 * @param[in]   data - pointer to data which is to be read/write \n
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure \n
 *                               -28       - Firmware Load or Upgrade timeout error \n
 *                                -2       - Invalid Parameter \n
 *                                -1 or -2 - SPI Failure 
 * @note        This is a proprietry API and it is not recommended to be used by the user directly.
 */
/// @private
int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
{
  int16_t retval     = 0;
  uint32_t cmd       = 0;
  uint32_t j         = 0;
  uint16_t len       = 0;
  uint16_t offset    = 0;
  uint16_t local     = 0;
  uint16_t read_data = 0;
  rsi_timer_instance_t timer_instance;

  switch (type) {
    case RSI_REG_READ:
      retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_data);
      *data  = read_data;
      break;

    case RSI_REG_WRITE:
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)data);
      break;
    case RSI_PING_WRITE:

      for (j = 0; j <= RSI_PING_PONG_CHUNK_SIZE / RSI_HAL_MAX_WR_BUFF_LEN; j++) {
        if (j == RSI_PING_PONG_CHUNK_SIZE / RSI_HAL_MAX_WR_BUFF_LEN) {
          len = (RSI_PING_PONG_CHUNK_SIZE % RSI_HAL_MAX_WR_BUFF_LEN);
          if (len == 0) {
            break;
          }
        } else {
          len = RSI_HAL_MAX_WR_BUFF_LEN;
        }
        retval = rsi_mem_wr(RSI_PING_BUFFER_ADDR + offset, len, (uint8_t *)((uint32_t)data + offset));
        if (retval < 0) {
          return retval;
        }
        offset += len;
      }
      local  = (RSI_PING_AVAIL | RSI_HOST_INTERACT_REG_VALID);
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case RSI_PONG_WRITE:

      for (j = 0; j <= RSI_PING_PONG_CHUNK_SIZE / RSI_HAL_MAX_WR_BUFF_LEN; j++) {
        if (j == RSI_PING_PONG_CHUNK_SIZE / RSI_HAL_MAX_WR_BUFF_LEN) {
          len = (RSI_PING_PONG_CHUNK_SIZE % RSI_HAL_MAX_WR_BUFF_LEN);
          if (len == 0) {
            break;
          }
        } else {
          len = RSI_HAL_MAX_WR_BUFF_LEN;
        }
        retval = rsi_mem_wr(RSI_PONG_BUFFER_ADDR + offset, len, (uint8_t *)((uint32_t)data + offset));
        if (retval < 0) {
          return retval;
        }
        offset += len;
      }
      // Perform the write operation
      local = (RSI_PONG_AVAIL | RSI_HOST_INTERACT_REG_VALID);

      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case BURN_NWP_FW:

      cmd = BURN_NWP_FW | RSI_HOST_INTERACT_REG_VALID;

      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      if (retval < 0) {
        return retval;
      }

      rsi_init_timer(&timer_instance, 300);

      do {
        retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_data);
        if (retval < 0) {
          return retval;
        }
        if (rsi_timer_expired(&timer_instance)) {
          return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
        }
      } while (read_data != (RSI_SEND_RPS_FILE | RSI_HOST_INTERACT_REG_VALID));
      break;
    case LOAD_NWP_FW:
      cmd    = LOAD_NWP_FW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    case LOAD_DEFAULT_NWP_FW_ACTIVE_LOW:
      cmd    = LOAD_DEFAULT_NWP_FW_ACTIVE_LOW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    default:
      retval = RSI_ERROR_INVALID_PARAM;
      break;
  }
  return retval;
}
/** @} */

#ifndef LINUX_PLATFORM
#if ((defined RSI_SPI_INTERFACE || defined RSI_SDIO_INTERFACE))
/** @addtogroup COMMON 
* @{
*/
/*==============================================*/
/**
 * @fn          int32_t rsi_get_ram_dump(uint32_t addr, uint16_t length, uint8_t *buf)
 * @brief       Get ram dump through master reads. This is blocking API. 
 * @pre         \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]   addr   - address of memory location in RS9116 
 * @param[in]   length - length of the content to read
 * @param[out]  buf    -  buffer to keep the read content
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 *              
 */

int32_t rsi_get_ram_dump(uint32_t addr, uint16_t length, uint8_t *buf)
{
  SL_PRINTF(SL_GET_RAM_DUMP_ENTRY, COMMON, LOG_INFO);
  uint32_t retval = 0;
  if (buf == NULL) {
    SL_PRINTF(SL_SET_RTC_TIMER_NULL_BUFFER, COMMON, LOG_ERROR);
    return -1;
  }
  if (!length || length > 4096) {
    SL_PRINTF(SL_SET_RTC_TIMER_BUFFER_LENGTH_ERROR, COMMON, LOG_ERROR);
    return -2;
  }
  retval = rsi_mem_rd(addr, length, buf);
  SL_PRINTF(SL_SET_RTC_TIMER_EXIT, COMMON, LOG_INFO, "retval: %d", retval);
  return retval;
}
/** @} */
#endif
#endif
#endif
#endif
#endif
