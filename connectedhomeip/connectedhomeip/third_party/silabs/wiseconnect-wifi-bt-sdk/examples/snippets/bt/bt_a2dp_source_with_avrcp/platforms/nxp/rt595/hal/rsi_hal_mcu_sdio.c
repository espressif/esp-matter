/*******************************************************************************
* @file  rsi_hal_mcu_sdio.c
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
 * Includes
 */
#include "fsl_device_registers.h"
#include "fsl_spi_cmsis.h"
#include "board.h"

#include "fsl_inputmux.h"

#include <stdio.h>
#include "fsl_sdio.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_power.h"
#include "pin_mux_sdio.h"

#include "fsl_pca9420.h"
#include "ff.h"
#include "fsl_sd_disk.h"

volatile int sdio_init_done ;
#ifdef   RSI_SDIO_INTERFACE

/**
 * Global Variables
 */
 sdio_card_t card;
static FIL g_fileObject;   /* File object */
static pca9420_handle_t pca9420Handle;
    pca9420_config_t pca9420Config;
    pca9420_modecfg_t pca9420ModeCfg[2];

status_t RSI_SDIO_IO_Write_Direct(sdio_card_t *card,uint32_t argument,uint32_t *data);
/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdioCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};
#endif

static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};


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
 return SDIO_IO_Read_Extended(&card, kSDIO_FunctionNum1, addr,
                       dBuf, len, SDIO_EXTEND_CMD_OP_CODE_MASK ) ;
}


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
  uint16_t byte_blocksize = 256;
  /* set block size to a non-word aligned size for test */
  if (kStatus_Success != SDIO_SetBlockSize(&card, kSDIO_FunctionNum1,byte_blocksize))
  {
    return kStatus_Fail;
  }


  if (kStatus_Success != SDIO_IO_Write_Extended(&card, kSDIO_FunctionNum1, Addr,
        tx_data,(no_of_blocks),(SDIO_EXTEND_CMD_BLOCK_MODE_MASK )) )
  {

    return kStatus_Fail;
  }
  return kStatus_Success ;
}

/*==============================================*/
/**
 * @fn          uint8_t rsi_sdio_read_multiple(uint8_t *read_buff, uint32_t no_of_blocks)
 * @brief       API is used to read no of bytes in blocked mode from device.
 * @param[in]   read_buff is the buffer to be stored with the data read from device.
 * @param[in]   Addr of the mem to be read.
 * @return      0 in case of success ,*              - 1 in case of failure
 * @section description This function gets the packet coming from the module and
 * copies to the buffer pointed
 *
 *
 */

uint8_t rsi_sdio_read_multiple(uint8_t *read_buff,uint32_t no_of_blocks)
{
  uint32_t Addr = 0;
  uint16_t byte_blocksize = 256;
  /* set block size to a non-word aligned size for test */
  if (kStatus_Success != SDIO_SetBlockSize(&card, kSDIO_FunctionNum1, byte_blocksize))
  {
    return kStatus_Fail;
  }

  Addr = byte_blocksize* no_of_blocks;
  if (kStatus_Success != SDIO_IO_Read_Extended
      (&card, kSDIO_FunctionNum1,Addr, read_buff,no_of_blocks,SDIO_EXTEND_CMD_BLOCK_MODE_MASK )) //kSDIO_FunctionNum1
  {
    return kStatus_Fail;
  }

  return kStatus_Success ;
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
uint8_t sdio_reg_writeb(uint32_t Addr, uint8_t *dBuf)
{
 return  SDIO_IO_Write_Direct(&card, kSDIO_FunctionNum0,Addr, dBuf, 0U);
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
uint8_t sdio_reg_readb(uint32_t Addr, uint8_t *dBuf)
{

  return  SDIO_IO_Read_Direct(&card, kSDIO_FunctionNum0,Addr,dBuf) ;
}

/*==============================================*/


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
 return SDIO_IO_Write_Extended(&card, kSDIO_FunctionNum1, addr,
                         dBuf, len, SDIO_EXTEND_CMD_OP_CODE_MASK );
}


status_t RSI_SDIO_IO_Write_Direct(sdio_card_t *card,uint32_t argument,uint32_t *data )
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0U};
    SDMMCHOST_COMMAND command  = {0U};

    command.index    = kSDIO_RWIODirect;
    command.argument = argument ;
    command.responseType       = kCARD_ResponseTypeR5;
    command.responseErrorFlags = (kSDIO_StatusCmdCRCError | kSDIO_StatusIllegalCmd | kSDIO_StatusError |
                                  kSDIO_StatusFunctionNumError | kSDIO_StatusOutofRange);

    content.command = &command;
    content.data    = NULL;

    if (kStatus_Success != card->host.transfer(card->host.base, &content))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* read data from response */
    *data = command.response[0U] & SDIO_DIRECT_CMD_DATA_MASK;

    return kStatus_Success;
}


/*=============================================*/
/**
 * @fn                  int16_t rsi_mcu_sdcard_init(void)
 * @brief               Initialize the SD card interface
 * @param[in]           none
 * @param[out]          none
 * @return              errCode
 * @section description
 * This API initializes the SD card interface.
 */
int16_t rsi_mcu_sdcard_init(void)
{
  BOARD_InitPinsSDcard();
  /*Make sure USDHC ram buffer has power up*/
  POWER_DisablePD(kPDRUNCFG_APD_USDHC0_SRAM);
  POWER_DisablePD(kPDRUNCFG_PPD_USDHC0_SRAM);

  POWER_DisablePD(kPDRUNCFG_PD_LPOSC);
  POWER_ApplyPD();
    /* PMIC PCA9420 */
    CLOCK_AttachClk(kFRO48M_to_FLEXCOMM15);
    BOARD_PMIC_I2C_Init();
    PCA9420_GetDefaultConfig(&pca9420Config);
    pca9420Config.I2C_SendFunc    = BOARD_PMIC_I2C_Send;
    pca9420Config.I2C_ReceiveFunc = BOARD_PMIC_I2C_Receive;
    PCA9420_Init(&pca9420Handle, &pca9420Config);
    for (uint32_t i = 0; i < ARRAY_SIZE(pca9420ModeCfg); i++)
    {
        PCA9420_GetDefaultModeConfig(&pca9420ModeCfg[i]);
    }
    pca9420ModeCfg[0].ldo2OutVolt = kPCA9420_Ldo2OutVolt3V300;
    pca9420ModeCfg[1].ldo2OutVolt = kPCA9420_Ldo2OutVolt1V800;
    PCA9420_WriteModeConfigs(&pca9420Handle, kPCA9420_Mode0, &pca9420ModeCfg[0], ARRAY_SIZE(pca9420ModeCfg));
  /* SDIO0 */
  /* usdhc depend on 32K clock also */
  CLOCK_AttachClk(kLPOSC_DIV32_to_32KHZWAKE_CLK);
  CLOCK_AttachClk(kAUX0_PLL_to_SDIO0_CLK);
  CLOCK_SetClkDiv(kCLOCK_DivSdio0Clk, 1);

  g_sd.host.base           = SD_HOST_BASEADDR0;
  g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ0;
  /* card detect type */
  g_sd.usrParam.cd = &s_sdCardDetect;

  /* SD host init function */
  if (SD_HostInit(&g_sd) != kStatus_Success)
  {
    PRINTF("\r\nSDIO host init fail\r\n");
    return -1;
  }
    sd_filesystem_init();

  /* power off card */
  SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);

  if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR0, &s_sdCardDetect, true) == kStatus_Success)
  {
    /* power on the card */
    SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
  }
  else
  {
    PRINTF("\r\nCard detect fail.\r\n");
    return -1;
  }
  return kStatus_Success;
}

int16_t rsi_mcu_sdio_init(void)
{
  BOARD_InitPins_sdio();
  /*Make sure USDHC ram buffer has power up*/
  POWER_DisablePD(kPDRUNCFG_APD_USDHC1_SRAM);
  POWER_DisablePD(kPDRUNCFG_PPD_USDHC1_SRAM);

  POWER_DisablePD(kPDRUNCFG_PD_LPOSC);
  POWER_ApplyPD();

  /* SDIO0 */
  /* usdhc depend on 32K clock also */
  CLOCK_AttachClk(kLPOSC_DIV32_to_32KHZWAKE_CLK);
  CLOCK_AttachClk(kMAIN_CLK_to_SDIO1_CLK);
  CLOCK_SetClkDiv(kCLOCK_DivSdio1Clk, 1);

  card.host.base           = SD_HOST_BASEADDR1;
  card.host.sourceClock_Hz = SD_HOST_CLK_FREQ1;
  /* card detect type */
  card.usrParam.cd = &s_sdioCardDetect;

  /* SD host init function */
  if (SDIO_HostInit(&card) != kStatus_Success)
  {
    PRINTF("\r\nSDIO host init fail\r\n");
    return -1;
  }
#ifdef RSI_WITH_OS
  /* Set interrupt priority for SDIO interrupt */
  NVIC_SetPriority(USDHC1_IRQn,USDHC1_IRQn_PRIORITY_2);
#endif
  /* power off card */
  SDIO_PowerOffCard(card.host.base, card.usrParam.pwr);

  if (SDIO_WaitCardDetectStatus(SD_HOST_BASEADDR1, &s_sdioCardDetect, true) == kStatus_Success)
  {
    /* reset host once card re-plug in */
    SDIO_HostReset(&(card.host));
    /* power on the card */
    SDIO_PowerOnCard(card.host.base, card.usrParam.pwr);
  }
  else
  {
    PRINTF("\r\nCard detect fail.\r\n");
    return -1;
  }
  /* Init SDIO card. */
  if (kStatus_Success != SDIO_CardInit(&card))
  {
    PRINTF("\r\nSDIO card init failed.\r\n");
    return kStatus_Fail;
  }

#if 1
  uint32_t data ;
#ifndef SDIO_4BIT_MODE
#define CD_DISABLE_ARG            (0x80000E80)
  if (kStatus_Success != RSI_SDIO_IO_Write_Direct(&card,CD_DISABLE_ARG,&data) )
  {
    return kStatus_Fail;
  }

#endif
  while(1)
  {
    //Function1 argument
#define FUCNTION1_CHECK_ARG       (0x00000400)
#define FUNCTION1_ENABLE          (1 << 1)
#define FUCNTION1_ENB_ARG         (0x80000402)
    /* Enable function1 */
    if(kStatus_Success  == RSI_SDIO_IO_Write_Direct(&card,FUCNTION1_CHECK_ARG,&data))
    {
      if(!((data) & FUNCTION1_ENABLE))
      {
        if(kStatus_Success  != RSI_SDIO_IO_Write_Direct(&card,FUCNTION1_ENB_ARG,&data))
        {
          return kStatus_Fail;
        }
        else
        {
          break;
        }
      }
      else
        break;
    }
  }

#define FUCNTION1_READY_ARG       (0x00000600)
#define FUNCTION1_READY           (1 << 1)
  while(1)
  {
    /*Check for function ready*/
    if(kStatus_Success == RSI_SDIO_IO_Write_Direct(&card,FUCNTION1_READY_ARG,&data))
    {
      if(data & FUNCTION1_READY)
      {
        break;
      }
    }
  }

#define FUNCTION1_INTR_ENB_ARG    (0x80000803)
  /* Interrupt Enable */
  if(kStatus_Success != RSI_SDIO_IO_Write_Direct(&card,FUNCTION1_INTR_ENB_ARG,&data))
  {
    return kStatus_Fail;
  }
#define FUNCTION1_INTR_CHECK_ARG  (0x00000800)
  /* Check interrupts are enabled or not */
  if(kStatus_Success != RSI_SDIO_IO_Write_Direct(&card,FUNCTION1_INTR_CHECK_ARG,&data))
  {
    return kStatus_Fail;
  }

  sdio_init_done =1 ;


#endif
  return kStatus_Success;
}
