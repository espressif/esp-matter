/*******************************************************************************
* @file  rsi_hal_mcu_sdio.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_driver.h"

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
  UNUSED_PARAMETER(tx_data); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(Addr); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(no_of_blocks); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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

int8_t rsi_sdio_read_multiple(uint8_t *read_buff, uint32_t Addr)
{ 
  UNUSED_PARAMETER(read_buff); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(Addr); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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
  UNUSED_PARAMETER(Addr); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(dBuf); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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
  UNUSED_PARAMETER(Addr); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(dBuf); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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
  UNUSED_PARAMETER(addr); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(len); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(dBuf); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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
  UNUSED_PARAMETER(addr); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(len); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(dBuf); //This statement is added only to resolve compilation warnings, value is unchanged
	// Function is not Implemented 
	return RSI_FAILURE;    
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
int16_t rsi_mcu_sdio_init(void)
{ 
	// Function is not Implemented 
	return RSI_FAILURE;    
}
