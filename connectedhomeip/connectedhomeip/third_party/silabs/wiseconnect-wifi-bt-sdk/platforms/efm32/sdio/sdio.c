/***************************************************************************//**
 * @file
 * @brief SDIO Peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include <stdio.h>
#include "em_assert.h"
#include "em_device.h"
#include "em_core.h"
#include "sdio.h"
#include "rsi_driver.h"

#ifdef SDIO
#define SDIO_REF_VALID(ref)   ((ref) == SDIO)
#else
#error "SDIO unsupported."
#endif

/***************************************************************************//**
 * @brief
 *   Initialize a SDIO peripheral.
 *
 * @details
 *   This function will initialize and start a SDIO peripheral as well as
 *   enable the SDIO IRQ in the NVIC.
 *
 * @note
 *   Since this function enables the SDIO IRQ, always add a custom
 *   SDIO_IRQHandler to the application to handle any interrupts
 *   from SDIO.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] init
 *   A pointer to the initialization structure used to configure the SDIO.
 ******************************************************************************/
void SDIO_Init (SDIO_TypeDef *sdio, const SDIO_Init_TypeDef_t *init)
{
  uint32_t hostctrl1 = 0;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));

  sdio->CTRL =            (0 << _SDIO_CTRL_ITAPDLYEN_SHIFT)
                        | (0 << _SDIO_CTRL_ITAPDLYSEL_SHIFT)
                        | (0 << _SDIO_CTRL_ITAPCHGWIN_SHIFT)
                        | (1 << _SDIO_CTRL_OTAPDLYEN_SHIFT)
                        | (8 << _SDIO_CTRL_OTAPDLYSEL_SHIFT);

  sdio->CFG0 =            (0x20 << _SDIO_CFG0_TUNINGCNT_SHIFT)
                        | (0x30 << _SDIO_CFG0_TOUTCLKFREQ_SHIFT)
                        | (1 << _SDIO_CFG0_TOUTCLKUNIT_SHIFT)
                        | (0xD0 << _SDIO_CFG0_BASECLKFREQ_SHIFT)
                        | (SDIO_CFG0_MAXBLKLEN_2048B)
                        | (1 << _SDIO_CFG0_C8BITSUP_SHIFT)
                        | (1 << _SDIO_CFG0_CADMA2SUP_SHIFT)
                        | (1 << _SDIO_CFG0_CHSSUP_SHIFT)
                        | (1 << _SDIO_CFG0_CSDMASUP_SHIFT)
                        | (1 << _SDIO_CFG0_CSUSPRESSUP_SHIFT)
                        | (1 << _SDIO_CFG0_C3P3VSUP_SHIFT)
                        | (1 << _SDIO_CFG0_C3P0VSUP_SHIFT)
                        | (1 << _SDIO_CFG0_C1P8VSUP_SHIFT);

  sdio->CFG1 =            (0 << _SDIO_CFG1_ASYNCINTRSUP_SHIFT)
                        | (SDIO_CFG1_SLOTTYPE_EMSDSLOT)
                        | (1 << _SDIO_CFG1_CSDR50SUP_SHIFT)
                        | (1 << _SDIO_CFG1_CSDR104SUP_SHIFT)
                        | (1 << _SDIO_CFG1_CDDR50SUP_SHIFT)
                        | (1 << _SDIO_CFG1_CDRVASUP_SHIFT)
                        | (1 << _SDIO_CFG1_CDRVCSUP_SHIFT)
                        | (1 << _SDIO_CFG1_CDRVDSUP_SHIFT)
                        | (1 << _SDIO_CFG1_RETUNTMRCTL_SHIFT)
                        | (1 << _SDIO_CFG1_TUNSDR50_SHIFT)
                        | (0 << _SDIO_CFG1_RETUNMODES_SHIFT)
                        | (1 << _SDIO_CFG1_SPISUP_SHIFT)
                        | (1 << _SDIO_CFG1_ASYNCWKUPEN_SHIFT);

  sdio->CFGPRESETVAL0 =   (0 << _SDIO_CFGPRESETVAL0_INITSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_INITCLKGENEN_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_INITDRVST_SHIFT)
                        | (0x4 << _SDIO_CFGPRESETVAL0_DSPSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL0_DSPCLKGENEN_SHIFT)
                        | (0x3 << _SDIO_CFGPRESETVAL0_DSPDRVST_SHIFT);

  sdio->CFGPRESETVAL1 =   (2 << _SDIO_CFGPRESETVAL1_HSPSDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL1_HSPCLKGENEN_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL1_HSPDRVST_SHIFT)
                        | (4 << _SDIO_CFGPRESETVAL1_SDR12SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL1_SDR12CLKGENEN_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL1_SDR12DRVST_SHIFT);

  sdio->CFGPRESETVAL2 =   (2 << _SDIO_CFGPRESETVAL2_SDR25SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR25CLKGENEN_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR25DRVST_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL2_SDR50SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL2_SDR50CLKGENEN_SHIFT)
                        | (1 << _SDIO_CFGPRESETVAL2_SDR50DRVST_SHIFT);

  sdio->CFGPRESETVAL3 =   (0 << _SDIO_CFGPRESETVAL3_SDR104SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL3_SDR104CLKGENEN_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL3_SDR104DRVST_SHIFT)
                        | (2 << _SDIO_CFGPRESETVAL3_DDR50SDCLKFREQ_SHIFT)
                        | (0 << _SDIO_CFGPRESETVAL3_DDR50CLKGENEN_SHIFT)
                        | (3 << _SDIO_CFGPRESETVAL3_DDR50DRVST_SHIFT);

  // Enable all the status bits
  sdio->IFENC = _SDIO_IFENC_MASK;

  switch (init->transferWidth) {
    case SDIO_TRANSFER_WIDTH_1BIT:
      hostctrl1 = SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V;
      break;
    case SDIO_TRANSFER_WIDTH_4BIT:
      hostctrl1 = ( SDIO_HOSTCTRL1_DATTRANWD_SD4
                  | SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V);
      break;
    case SDIO_TRANSFER_WIDTH_8BIT:
      hostctrl1 = ( SDIO_HOSTCTRL1_EXTDATTRANWD
                  | SDIO_HOSTCTRL1_SDBUSVOLTSEL_3P0V);
      break;
  }
  sdio->HOSTCTRL1 = hostctrl1;

  SDIO_SetClockFrequency(sdio, init->refFreq, init->desiredFreq);

  sdio->HOSTCTRL1 |=      SDIO_HOSTCTRL1_SDBUSPOWER;

  NVIC_ClearPendingIRQ(SDIO_IRQn);
  NVIC_SetPriority(SDIO_IRQn,1);
  NVIC_EnableIRQ(SDIO_IRQn);
}

/***************************************************************************//**
 * @brief
 *   Set the bus frequency of a SDIO peripheral.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] refFreq
 *   The value of the reference clock.
 *
 * @param[in] desiredFreq
 *   The bus frequency desired, the actual frequency generated may not have
   * exactly the same value.
 ******************************************************************************/
void SDIO_SetClockFrequency (SDIO_TypeDef *sdio,
                             uint32_t refFreq,
                             uint32_t desiredFreq)
{
  uint32_t divisor_msb = 0;
  uint32_t divisor_lsb = 0;

  // Calculate the divisor for SD clock frequency (empirical formula)
  divisor_lsb = (refFreq / desiredFreq) / 2;

  if (divisor_lsb > 0xFF) {
    divisor_msb = divisor_lsb >> 8;
  }

  sdio->CLOCKCTRL =       (0xF << _SDIO_CLOCKCTRL_DATTOUTCNTVAL_SHIFT)
                        | ((divisor_msb << _SDIO_CLOCKCTRL_UPPSDCLKFRE_SHIFT) & _SDIO_CLOCKCTRL_UPPSDCLKFRE_MASK)
                        | ((divisor_lsb << _SDIO_CLOCKCTRL_SDCLKFREQSEL_SHIFT) & _SDIO_CLOCKCTRL_SDCLKFREQSEL_MASK)
                        | SDIO_CLOCKCTRL_INTCLKEN;

  // Wait for the clock to be stable
  while (!(sdio->CLOCKCTRL & _SDIO_CLOCKCTRL_INTCLKSTABLE_MASK));

  sdio->CLOCKCTRL |=      SDIO_CLOCKCTRL_SDCLKEN;
}

/***************************************************************************//**
 * @brief
 *   Denitialize a SDIO peripheral.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 ******************************************************************************/
void SDIO_DeInit (SDIO_TypeDef *sdio)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));

  NVIC_DisableIRQ(SDIO_IRQn);
  sdio->IEN  = 0;
  sdio->IFENC  = 0;
  sdio->CLOCKCTRL &= ~( SDIO_CLOCKCTRL_SDCLKEN
                      | SDIO_CLOCKCTRL_INTCLKEN);
}

/***************************************************************************//**
 * @brief
 *   Send a SDIO command.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] cmd
 *   A pointer to a command structure to send.
 ******************************************************************************/
void SDIO_TxCmd (SDIO_TypeDef *sdio, SDIO_Cmd_t *cmd)
{
  uint32_t int_status;
  CORE_DECLARE_IRQ_STATE;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));

  // Wait for the command transmission end
  while (sdio->PRSSTAT & _SDIO_PRSSTAT_CMDINHIBITCMD_MASK);

  // Wait for the data reception end
  while (sdio->PRSSTAT & _SDIO_PRSSTAT_CMDINHIBITDAT_MASK);

  CORE_ENTER_ATOMIC();
  // Clear status register
  int_status = sdio->IFCR;
  sdio->IFCR = int_status;
  CORE_EXIT_ATOMIC();

  // Enable relevant interrupts
  sdio->IEN |= ( SDIO_IEN_CMDCOMSEN
               | SDIO_IEN_TRANCOMSEN
               | SDIO_IEN_CMDTOUTERRSEN
               | SDIO_IEN_CMDCRCERRSEN
               | SDIO_IEN_CMDENDBITERRSEN
               | SDIO_IEN_CMDINDEXERRSEN
               | SDIO_IEN_DATTOUTERRSEN
               | SDIO_IEN_DATCRCERRSEN
               | SDIO_IEN_DATENDBITERRSEN);

  uint32_t transfer_mode = (  (cmd->respType << _SDIO_TFRMODE_RESPTYPESEL_SHIFT)
                            | ((!!cmd->checkCrc) << _SDIO_TFRMODE_CMDCRCCHKEN_SHIFT)
                            | ((!!cmd->checkIndex) << _SDIO_TFRMODE_CMDINDXCHKEN_SHIFT)
                            | ((cmd->index << _SDIO_TFRMODE_CMDINDEX_SHIFT) & _SDIO_TFRMODE_CMDINDEX_MASK));

  switch (cmd->dataDirection) {
    case SDIO_DATA_DIRECTION_CARD_TO_HOST:
      transfer_mode |= (SDIO_TFRMODE_DATPRESSEL | SDIO_TFRMODE_DATDIRSEL);
      break;
    case SDIO_DATA_DIRECTION_HOST_TO_CARD:
      transfer_mode |= SDIO_TFRMODE_DATPRESSEL;
      break;
    case SDIO_DATA_DIRECTION_NODATA:
    default:
      break;
  }

  if (cmd->enableDma) {
    transfer_mode |= SDIO_TFRMODE_DMAEN;
    sdio->IEN |= SDIO_IEN_DMAINTSEN;
  }

  if (cmd->multiblocks) {
    transfer_mode |= (SDIO_TFRMODE_MULTSINGBLKSEL | SDIO_TFRMODE_BLKCNTEN);
  }

  sdio->CMDARG1 = cmd->args;

  // Update the register fields
  sdio->TFRMODE = transfer_mode;
}

/***************************************************************************//**
 * @brief
 *   Send a SDIO command and wait for the command to complete.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] cmd
 *   A pointer to a command structure to send.
 ******************************************************************************/
void SDIO_TxCmdB (SDIO_TypeDef *sdio, SDIO_Cmd_t *cmd)
{
  uint32_t int_status;
  CORE_DECLARE_IRQ_STATE;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));

  // Wait for the command transmission end
  while (sdio->PRSSTAT & _SDIO_PRSSTAT_CMDINHIBITCMD_MASK);

  // Wait for the data reception end
  while (sdio->PRSSTAT & _SDIO_PRSSTAT_CMDINHIBITDAT_MASK);

  CORE_ENTER_ATOMIC();
  // Clear status register
  int_status = sdio->IFCR;
  sdio->IFCR = int_status;
  CORE_EXIT_ATOMIC();

  uint32_t transfer_mode = (  (cmd->respType << _SDIO_TFRMODE_RESPTYPESEL_SHIFT)
                            | ((!!cmd->checkCrc) << _SDIO_TFRMODE_CMDCRCCHKEN_SHIFT)
                            | ((!!cmd->checkIndex) << _SDIO_TFRMODE_CMDINDXCHKEN_SHIFT)
                            | ((cmd->index << _SDIO_TFRMODE_CMDINDEX_SHIFT) & _SDIO_TFRMODE_CMDINDEX_MASK));

  switch (cmd->dataDirection) {
    case SDIO_DATA_DIRECTION_CARD_TO_HOST:
      transfer_mode |= (SDIO_TFRMODE_DATPRESSEL | SDIO_TFRMODE_DATDIRSEL);
      break;
    case SDIO_DATA_DIRECTION_HOST_TO_CARD:
      transfer_mode |= SDIO_TFRMODE_DATPRESSEL;
      break;
    case SDIO_DATA_DIRECTION_NODATA:
    default:
      break;
  }

  if (cmd->enableDma) {
    transfer_mode |= SDIO_TFRMODE_DMAEN;
    sdio->IEN |= SDIO_IEN_DMAINTSEN;
  }

  if (cmd->multiblocks) {
    transfer_mode |= (SDIO_TFRMODE_MULTSINGBLKSEL | SDIO_TFRMODE_BLKCNTEN);
  }

  sdio->CMDARG1 = cmd->args;

  // Update the register fields
  sdio->TFRMODE = transfer_mode;

  // Wait for the command transfer completion or an error
  //while ((sdio->IFCR & (_SDIO_IFCR_CMDCOM_MASK | _SDIO_IFCR_ERRINT_MASK)) == 0);
}

/***************************************************************************//**
 * @brief
 *   Retrieve the command completion status.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 ******************************************************************************/
uint32_t SDIO_GetCmdStatus (SDIO_TypeDef *sdio)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));
  rsi_delay_ms(10);
  return sdio->IFCR;
}

/***************************************************************************//**
 * @brief
 *   Get the command response.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] respIndex
 *   Index of the response register to retrieve [0-3].
 ******************************************************************************/
uint32_t SDIO_GetResp (SDIO_TypeDef *sdio, uint8_t respIndex)
{

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(SDIO_REF_VALID(sdio));

  return (uint32_t)(*(&(sdio->RESP0) + respIndex));
}

/***************************************************************************//**
 * @brief
 *   Configure a transfer.
 *
 * @param[in] sdio
 *   A pointer to the SDIO peripheral instance.
 *
 * @param[in] itemSize
 *   Size of the item(s) to transfer
 *
 * @param[in] itemCount
 *   Number of items to transfer
 ******************************************************************************/
void SDIO_ConfigureTransfer (SDIO_TypeDef *sdio,
                             uint16_t itemSize,
                             uint16_t itemCount)
{
  sdio->BLKSIZE = ( (itemCount << _SDIO_BLKSIZE_BLKSCNTFORCURRTFR_SHIFT)
                  | SDIO_BLKSIZE_HSTSDMABUFSIZE_SIZE512
                  | (itemSize & _SDIO_BLKSIZE_TFRBLKSIZE_MASK));
}
