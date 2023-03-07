/*******************************************************************************
* @file  rsi_sdio_frame_rd_wr.c
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
/**
 * @file     rsi_sdio_frame_rd_wr.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 * @brief SDIO Control: Functions to control SDIO frame read/write functions
 *
 * Description
 * SDIO function to read/write management descriptors frames to/from the wlan module
 *
 * Improvements
 * Function header is modified for frameDscRd() function
 */

/*
  Includes
 */
#include "rsi_driver.h"
#include "rsi_pkt_mgmt.h"
#ifdef RSI_SDIO_INTERFACE
/*
  Global Variables
 */
/** @addtogroup DRIVER1
* @{
*/
/*====================================================*/
/**
 */
int16_t rsi_frame_read(uint8_t *read_buff)
{
  int16_t retval   = RSI_SUCCESS;
  uint8_t response = 0;
  uint16_t no_of_blocks;

  // Read number of blocks
  retval = rsi_reg_rd(0xf1, &response);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  no_of_blocks = (response & 0x1F);

  // Frame read
  retval = rsi_sdio_read_multiple(read_buff, no_of_blocks);
  return retval;
}

/*====================================================*/
/**
 *
 */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
{
  UNUSED_PARAMETER(payloadparam); //This statement is added only to resolve compilation warning, value is unchanged
  int16_t retval        = RSI_SUCCESS;
  uint16_t Addr         = 0x0000;
  uint16_t no_of_blocks = 0;
  uint16_t queue_type   = 0;

  uint16_t size_of_headroom = 0;

  queue_type = (uFrameDscFrame->frame_len_queue_no[1] >> 4);
#ifdef CHIP_9117
  if ((queue_type == RSI_WLAN_DATA_Q) || (queue_type == RSI_WLAN_MGMT_Q)) {
    size_of_headroom = SIZE_OF_HEADROOM;
  }
#endif

  // Calculate number of blocks
  no_of_blocks = ((size_param + 16 + size_of_headroom) / 256);
  if ((size_param + 16 + size_of_headroom) % 256) {
    no_of_blocks = no_of_blocks + 1;
  }

  Addr = (no_of_blocks * 256);

  Addr = (Addr | (queue_type << 12));

  // Transfer packet
  retval = rsi_sdio_write_multiple(((uint8_t *)uFrameDscFrame - size_of_headroom), Addr, no_of_blocks);
  if (retval == 0) {
    return retval;
  } else {
    return RSI_ERROR_SDIO_WRITE_FAIL;
  }
}
#endif
/** @} */
