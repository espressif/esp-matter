/*******************************************************************************
* @file rsi_hal_mcu_sdio.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com
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

/**
 * Includes
 */

#include "rsi_sdio.h"
#include "rsi_driver.h"
#include "sl_rsi_host_api.h"

uint8_t sdio_init_done;

/*==============================================*/
/**
 * @fn          void rsi_sdio_write_multiple(uint8_t *tx_data, uint32_t Addr, uint16_t no_of_blocks)
 * @brief       This API is used to write the packet on to the SDIO interface in block mode.
 * @param[in]   tx_data is the buffer to be written to sdio.
 * @param[in]   Addr of the mem to which the data has to be written.
 * @param[in]   no_of_blocks is the blocks present to be transfered.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description 
 *  This API is used to write the packet on to the SDIO interface
 * 
 *
 */
int16_t rsi_sdio_write_multiple(uint8_t *tx_data, uint32_t Addr, uint16_t no_of_blocks)
{
  return sl_rsi_host_sdio_transfer_cmd53(SL_RSI_BUS_WRITE, 1, Addr, (uint16_t *)tx_data , no_of_blocks*256);
}

/*==============================================*/
/**
 * @fn          uint8_t rsi_sdio_read_multiple(uint8_t *read_buff, uint32_t Addr)
 * @brief       API is used to read no of bytes in blocked mode from device.
 * @param[in]   read_buff is the buffer to be stored with the data read from device.
 * @param[in]   Addr of the mem to be read.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function gets the packet coming from the module and 
 * copies to the buffer pointed 
 * 
 *
 */

int8_t rsi_sdio_read_multiple(uint8_t *read_buff, uint32_t no_of_blocks)
{
  uint32_t Addr = 0;
  uint16_t byte_blocksize = 256;
  Addr = byte_blocksize* no_of_blocks;
  return sl_rsi_host_sdio_transfer_cmd53(SL_RSI_BUS_READ, 1, Addr, (uint16_t *)read_buff , no_of_blocks * 256); //no_of_blocks);
}

/*==============================================*/
/**
 * @fn          uint8_t sdio_reg_writeb(uint32_t Addr, uint8_t *dBuf)
 * @brief       API is used to write 1 byte of data to sdio slave register space.
 * @param[in]   Addr of the reg to be written.
 * @param[in]   Buffer of data to be written to sdio slave reg.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function writes 1 byte of data to the slave device 
 * 
 *
 */
int8_t sdio_reg_writeb(uint32_t Addr, uint8_t *dBuf)
{
  return sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE , 0, Addr, dBuf);
}

/*==============================================*/
/**
 * @fn          uint8_t sdio_reg_readb(uint32_t Addr, uint8_t *dBuf)
 * @brief       API is used to read 1 byte of data from sdio slave register space.
 * @param[in]   Addr of the reg to be read.
 * @param[in]   Buffer of data to be read from sdio slave reg.  
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function gets the 1 byte of data from the slave device 
 * 
 *
 */
int8_t sdio_reg_readb(uint32_t Addr, uint8_t *dBuf)
{
  return sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ , 0, Addr, dBuf);
}

/*==============================================*/
/**
 * @fn          int16_t rsi_sdio_readb(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       API is used to read n bytes of data from device space in byte mode.
 * @param[in]   Addr of the data to be read.
 * @param[in]   Buffer of data to be read from sdio device.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function gets the n  bytes of data from the device
 * 
 *
 */
int16_t rsi_sdio_readb(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  return sl_rsi_host_sdio_transfer_cmd53(SL_RSI_BUS_READ, 1, addr, (uint16_t *)dBuf, len);
}

/*==============================================*/
/**
 * @fn          int16_t rsi_sdio_writeb(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       API is used to write n bytes of data to device space in byte mode.
 * @param[in]   Addr of the data to be written.
 * @param[in]   Buffer of data to be written to sdio device.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function writes the n bytes of data to the device
 * 
 *
 */
int16_t rsi_sdio_writeb(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  return sl_rsi_host_sdio_transfer_cmd53(SL_RSI_BUS_WRITE, 1, addr, (uint16_t *)dBuf, len);
}
/*=============================================*/
/**
 * @fn                  int16_t rsi_sdio_init(void)
 * @brief               Start the SDIO interface
 * @param[in]           none
 * @param[out]          none
 * @return              errCode
 * @section description         
 * This API initializes the Wi-Fi modules Slave SDIO interface.
 */
int32_t rsi_mcu_sdio_init(void)
{
  sl_status_t result;
  uint8_t     value_u8;

  
  // Add code to reset chip
 result = sl_rsi_host_init_bus();
  SL_RSI_ERROR_CHECK(result);

  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ, 0,
                                           SL_RSI_SDIO_CCCR_IO_QUEUE_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  // Enables Function 1
  value_u8 |= (1 << 1);
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_CCCR_IO_QUEUE_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);
  while(1)
    {
          result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ, 0,
                                           SL_RSI_SDIO_CCCR_IOR_ENABLE,
                                             &value_u8);
      if (value_u8 & 0x2)
      {
        break;
      }
  }

  // Enables Master and Function 1 interrupts
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ, 0,
                                           SL_RSI_SDIO_CCCR_IRQ_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);
  value_u8 |= 0x1 | (1 << 1);
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_CCCR_IRQ_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  // Set bus width to 4-bit
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ, 0,
                                           SL_RSI_SDIO_CCCR_BUS_INTERFACE_CONTROL,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);
  value_u8 = (value_u8 & 0xFC) | 0x2;
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_CCCR_BUS_INTERFACE_CONTROL,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  // Switch to HS mode
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_READ, 0,
                                           SL_RSI_SDIO_CCCR_HIGH_SPEED_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);
  // Set Enable_High_Speed to 1
  value_u8 |= 0x2;
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_CCCR_HIGH_SPEED_ENABLE,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  // Enabled SDIO high speed mode
  sl_rsi_host_sdio_enable_high_speed_mode();

  // Set function 1 block size
  value_u8 = SL_RSI_SDIO_BLOCK_SIZE & 0xff;
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_FBR1_BLOCK_SIZE_LSB,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  value_u8 = (SL_RSI_SDIO_BLOCK_SIZE >> 8) & 0xff;
  result = sl_rsi_host_sdio_transfer_cmd52(SL_RSI_BUS_WRITE, 0,
                                           SL_RSI_SDIO_FBR1_BLOCK_SIZE_MSB,
                                           &value_u8);
  SL_RSI_ERROR_CHECK(result);

  sdio_init_done =1;

  error_handler:
  return result;

}
