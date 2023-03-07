/***************************************************************************//**
 * @file
 * @brief Universal asynchronous receiver/transmitter (EUSART) peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_eusart.h"
#if defined(EUART_PRESENT) || defined(EUSART_PRESENT)
#include "em_cmu.h"
#include <stddef.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

#if defined(EUART_PRESENT)
  #define EUSART_REF_VALID(ref)    ((ref) == EUART0)
  #define EUSART_EM2_CAPABLE(ref)  (true)
  #define EUSART_RX_FIFO_SIZE  4u
#elif defined(EUSART_PRESENT)
  #define EUSART_REF_VALID(ref)    (EUSART_NUM(ref) != -1)
  #define EUSART_RX_FIFO_SIZE  16u
#endif

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/
#if defined(EUSART_DALICFG_DALIEN)
static uint8_t dali_tx_nb_packets[EUSART_COUNT];
static uint8_t dali_rx_nb_packets[EUSART_COUNT];
#endif /* EUSART_DALICFG_DALIEN */

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

static CMU_Clock_TypeDef EUSART_ClockGet(EUSART_TypeDef *eusart);

static void EUSART_AsyncInitCommon(EUSART_TypeDef *eusart,
                                   const EUSART_UartInit_TypeDef *init,
                                   const EUSART_IrDAInit_TypeDef *irdaInit,
                                   const EUSART_DaliInit_TypeDef *daliInit);

#if defined(EUSART_PRESENT)
static void EUSART_SyncInitCommon(EUSART_TypeDef *eusart,
                                  const EUSART_SpiInit_TypeDef  *init);
#endif

/***************************************************************************//**
 * Wait for ongoing sync of register(s) to the low-frequency domain to complete.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param mask A bitmask corresponding to SYNCBUSY register defined bits,
 *             indicating registers that must complete any ongoing
 *             synchronization.
 ******************************************************************************/
__STATIC_INLINE void eusart_sync(EUSART_TypeDef *eusart, uint32_t mask)
{
  // Wait for any pending previous write operation to have been completed
  // in the low-frequency domain.
  while ((eusart->SYNCBUSY & mask) != 0U) {
  }
}

/***************************************************************************//**
 *   Calculate baudrate for a given reference frequency, clock division,
 *   and oversampling rate.
 ******************************************************************************/
__STATIC_INLINE uint32_t EUSART_AsyncBaudrateCalc(uint32_t refFreq,
                                                  uint32_t clkdiv,
                                                  EUSART_OVS_TypeDef ovs);

/***************************************************************************//**
 *   Execute the EUSART peripheral disabling sequence.
 ******************************************************************************/
__STATIC_INLINE void EUSART_Disable(EUSART_TypeDef *eusart);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initializes the EUSART when used with the high frequency clock.
 ******************************************************************************/
void EUSART_UartInitHf(EUSART_TypeDef *eusart, const EUSART_UartInit_TypeDef *init)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));
  // Init structure must be provided.
  EFM_ASSERT(init);

  // Assert features specific to HF.
  // The oversampling must not be disabled when using a high frequency clock.
  EFM_ASSERT(init->oversampling != eusartOVS0);

  // Uart mode only supports up to 9 databits frame.
  EFM_ASSERT(init->databits <= eusartDataBits9);

  // Initialize EUSART with common features to HF and LF.
  EUSART_AsyncInitCommon(eusart, init, NULL, NULL);
}

/***************************************************************************//**
 * Initializes the EUSART when used with the low frequency clock.
 *
 * @note (1) When EUSART oversampling is set to eusartOVS0 (Disable), the peripheral
 *           clock frequency must be at least three times higher than the
 *           chosen baud rate. In LF, max input clock is 32768 (LFXO or LFRCO),
 *           thus 32768 / 3 ~ 9600 baudrate.
 ******************************************************************************/
void EUSART_UartInitLf(EUSART_TypeDef *eusart, const EUSART_UartInit_TypeDef *init)
{
  // Make sure the module exists and is Low frequency capable.
  EFM_ASSERT(EUSART_REF_VALID(eusart) && EUSART_EM2_CAPABLE(EUSART_NUM(eusart)));
  // Init structure must be provided.
  EFM_ASSERT(init);

  // Assert features specific to LF.
  // LFXO, LFRCO, ULFRCO can be a clock source in LF.
#if defined(DEBUG_EFM) || defined(DEBUG_EFM_USER)
  {
    CMU_Select_TypeDef clock_source = (CMU_Select_TypeDef) NULL;
#if defined(EUART_PRESENT)
    if (eusart == EUART0) {
      clock_source = CMU_ClockSelectGet(cmuClock_EUART0);
    }
#endif
#if defined(EUSART_PRESENT) && defined(EUSART0)
    if (eusart == EUSART0) {
      clock_source = CMU_ClockSelectGet(cmuClock_EUSART0);
    }
#endif

    EFM_ASSERT(
      (clock_source == cmuSelect_ULFRCO)
      || (clock_source == cmuSelect_LFXO)
      || (clock_source == cmuSelect_LFRCO)
      || (clock_source == cmuSelect_EM23GRPACLK) /* ULFRCO, LFXO, or LFRCO */
      );
  }
#endif
  // Uart mode only supports up to 9 databits frame.
  EFM_ASSERT(init->databits <= eusartDataBits9);
  // The oversampling must be disabled when using a low frequency clock.
  EFM_ASSERT(init->oversampling == eusartOVS0);
  // The Majority Vote must be disabled when using a low frequency clock.
  EFM_ASSERT(init->majorityVote == eusartMajorityVoteDisable);
  // Number of stop bits can only be 1 or 2 in LF.
  EFM_ASSERT((init->stopbits == eusartStopbits1) || (init->stopbits == eusartStopbits2));
  // In LF, max baudrate is 9600. See Note #1.
  EFM_ASSERT(init->baudrate <= 9600 && init->baudrate != 0);

  // Initialize EUSART with common features to HF and LF.
  EUSART_AsyncInitCommon(eusart, init, NULL, NULL);
}

/***************************************************************************//**
 * Initializes the EUSART when used in IrDA mode with the high or low
 * frequency clock.
 ******************************************************************************/
void EUSART_IrDAInit(EUSART_TypeDef *eusart,
                     const EUSART_IrDAInit_TypeDef *irdaInit)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));
  // Init structure must be provided.
  EFM_ASSERT(irdaInit);

  if (irdaInit->irDALowFrequencyEnable) {
    // Validate the low frequency capability of the EUSART instance.
    EFM_ASSERT(EUSART_EM2_CAPABLE(EUSART_NUM(eusart)));
    // The oversampling must be disabled when using a low frequency clock.
    EFM_ASSERT(irdaInit->init.oversampling == eusartOVS0);
    // Number of stop bits can only be 1 or 2 in LF.
    EFM_ASSERT((irdaInit->init.stopbits == eusartStopbits1) || (irdaInit->init.stopbits == eusartStopbits2));
    // In LF, max baudrate is 9600. See Note #1.
    EFM_ASSERT(irdaInit->init.baudrate <= 9600);
    EFM_ASSERT(irdaInit->init.enable == eusartEnableRx || irdaInit->init.enable == eusartDisable);
  } else {
    EFM_ASSERT(irdaInit->init.oversampling != eusartOVS0);
    // In HF, 2.4 kbps <= baudrate <= 1.152 Mbps.
    EFM_ASSERT(irdaInit->init.baudrate >= 2400 && irdaInit->init.baudrate <= 1152000);
  }

  // Initialize EUSART with common features to HF and LF.
  EUSART_AsyncInitCommon(eusart, &irdaInit->init, irdaInit, NULL);
}

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * Initializes the EUSART when used in SPI mode.
 ******************************************************************************/
void EUSART_SpiInit(EUSART_TypeDef *eusart, EUSART_SpiInit_TypeDef const *init)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));
  // Init structure must be provided.
  EFM_ASSERT(init);
  if (init->master) {
    EFM_ASSERT(init->bitRate <= 20000000);

    if (init->advancedSettings) {
      EFM_ASSERT(!(init->advancedSettings->prsClockEnable));
    }
  } else {
    EFM_ASSERT(init->bitRate <= 10000000);

    if (init->advancedSettings && init->advancedSettings->forceLoad) {
      // If baud-rate is more than 5MHz, a value of 4 is recommended, any values
      // smaller than that can be tried out but avoid using 0. If baud-rate is less than 5MHz,
      // value of 5 is recommended, values higher than 5 can be used but it may make the load
      // error easy to occur. The recommended values for frequency bands should be sufficient
      // to work all the time.
      EFM_ASSERT((init->bitRate >= 5000000 && init->advancedSettings->setupWindow <= 4)
                 || (init->bitRate < 5000000 && init->advancedSettings->setupWindow >= 5));
    }
  }

  EUSART_SyncInitCommon(eusart, init);
}

#if defined(EUSART_DALICFG_DALIEN)
/***************************************************************************//**
 * Initializes the EUSART when used in DALI mode with the high or low
 * frequency clock.
 *
 * @note (1) When EUSART oversampling is set to eusartOVS0 (Disable), the peripheral
 *           clock frequency must be at least three times higher than the
 *           chosen baud rate. In LF, max input clock is 32768 (LFXO or LFRCO),
 *           thus 32768 / 3 ~ 9600 baudrate.
 ******************************************************************************/
void EUSART_DaliInit(EUSART_TypeDef *eusart,
                     const EUSART_DaliInit_TypeDef *daliInit)
{
  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));
  // Init structure must be provided.
  EFM_ASSERT(daliInit);

  if (daliInit->init.loopbackEnable) {
    // If LOOPBK in CFG0 is set to 1 in order to do loopback testing for DALI,
    // then in this case DALIRXENDT should be set to 1 and DALIRXDATABITS should
    // be set the same as DALITXDATABITS.
    EFM_ASSERT( (daliInit->TXdatabits >> _EUSART_DALICFG_DALITXDATABITS_SHIFT)
                == (daliInit->RXdatabits >> _EUSART_DALICFG_DALIRXDATABITS_SHIFT));
  }

  if (daliInit->daliLowFrequencyEnable) {
    // Validate the low frequency capability of the EUSART instance.
    EFM_ASSERT(EUSART_EM2_CAPABLE(EUSART_NUM(eusart)));
    // The oversampling must be disabled when using a low frequency clock.
    EFM_ASSERT(daliInit->init.oversampling == eusartOVS0);
    // In LF, max baudrate is 9600. See Note #1.
    // but manchester is running at 2x clock 9600 => 4800
    EFM_ASSERT(daliInit->init.baudrate <= 4800);
  } else {
    EFM_ASSERT(daliInit->init.oversampling != eusartOVS0);
    // In HF, 2.4 kbps <= baudrate <= 1.152 Mbps.
    // but manchester is running at 2x clock so 2.4 kbps => 1.2 kbps
    EFM_ASSERT(daliInit->init.baudrate >= 1200 && daliInit->init.baudrate <= 57600);
  }

  // Initialize EUSART with common features to HF and LF.
  EUSART_AsyncInitCommon(eusart, &daliInit->init, NULL, daliInit);
}
#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/***************************************************************************//**
 * Configure the EUSART to its reset state.
 ******************************************************************************/
void EUSART_Reset(EUSART_TypeDef *eusart)
{
  // 1. Properly disable the module
  EUSART_Disable(eusart);

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  // Manual toggling tx_sclk_mst to synchronize handshake
  // when switching from SPI master to other modes
  // so module is disabling correctly.
  uint32_t forcedClkCycle = 4u;

  while (forcedClkCycle--) {
    eusart->CFG2_SET = _EUSART_CFG2_CLKPHA_MASK;
    eusart->CFG2_CLR = _EUSART_CFG2_CLKPHA_MASK;
  }
#endif
  // All registers that end with CFG should be programmed before EUSART gets enabled (EUSARTn_EN is set).
  // Set all configurable register to its reset value.
  // Note: Program desired settings to all registers that have names ending with CFG in the following sequence:
  //  a. CFG2
#if defined(EUSART_PRESENT)
  eusart->CFG2 = _EUSART_CFG2_RESETVALUE;
#endif
  //  b. CFG1
  eusart->CFG1 = _EUSART_CFG1_RESETVALUE;
  //  c. CFG0
  eusart->CFG0 = _EUSART_CFG0_RESETVALUE;
  //  d. FRAMECFG, DTXDATCFG, TIMINGCFG (Any sequence)
  eusart->FRAMECFG = _EUSART_FRAMECFG_RESETVALUE;
#if defined(EUSART_PRESENT)
  eusart->DTXDATCFG = _EUSART_DTXDATCFG_RESETVALUE;
#if defined(EUSART_DALICFG_DALIEN)
  eusart->DALICFG = _EUSART_DALICFG_RESETVALUE;
#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */
  eusart->TIMINGCFG = _EUSART_TIMINGCFG_RESETVALUE;
  eusart->IRHFCFG = _EUSART_IRHFCFG_RESETVALUE;
  eusart->IRLFCFG = _EUSART_IRLFCFG_RESETVALUE;
  eusart->STARTFRAMECFG = _EUSART_STARTFRAMECFG_RESETVALUE;
  eusart->SIGFRAMECFG = _EUSART_SIGFRAMECFG_RESETVALUE;
  eusart->TRIGCTRL = _EUSART_TRIGCTRL_RESETVALUE;
  eusart->IEN = _EUSART_IEN_RESETVALUE;
  eusart->IF_CLR = _EUSART_IF_MASK;

  // no need to sync while EN=0, multiple writes can be queued up,
  // and the last one will synchronize once EN=1
  eusart->CLKDIV = _EUSART_CLKDIV_RESETVALUE;
}

/***************************************************************************//**
 * Enables/disables the EUSART receiver and/or transmitter.
 ******************************************************************************/
void EUSART_Enable(EUSART_TypeDef *eusart, EUSART_Enable_TypeDef enable)
{
  uint32_t tmp = 0;

  // Make sure that the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  if (enable == eusartDisable) {
    EUSART_Disable(eusart);
  } else {
    // Enable peripheral to configure Rx and Tx.
    eusart->EN_SET = EUSART_EN_EN;

    // Enable or disable Rx and/or Tx
    tmp = (enable)
          & (_EUSART_CMD_RXEN_MASK | _EUSART_CMD_TXEN_MASK
             | _EUSART_CMD_RXDIS_MASK | _EUSART_CMD_TXDIS_MASK);

    eusart_sync(eusart, _EUSART_SYNCBUSY_MASK);
    eusart->CMD = tmp;
    eusart_sync(eusart,
                EUSART_SYNCBUSY_RXEN | EUSART_SYNCBUSY_TXEN
                | EUSART_SYNCBUSY_RXDIS | EUSART_SYNCBUSY_TXDIS);

    // Wait for the status register to be updated.
    tmp = 0;
    if (_EUSART_CMD_RXEN_MASK & enable) {
      tmp |= EUSART_STATUS_RXENS;
    }
    if (_EUSART_CMD_TXEN_MASK & enable) {
      tmp |= EUSART_STATUS_TXENS;
    }
    while ((eusart->STATUS & (_EUSART_STATUS_TXENS_MASK | _EUSART_STATUS_RXENS_MASK)) != tmp) {
    }
  }
}

/***************************************************************************//**
 * Receives one 8 bit frame, (or part of 9 bit frame).
 ******************************************************************************/
uint8_t EUSART_Rx(EUSART_TypeDef *eusart)
{
  while (!(eusart->STATUS & EUSART_STATUS_RXFL)) {
  } // Wait for incoming data.

  return (uint8_t)eusart->RXDATA;
}

/***************************************************************************//**
 * Receives one 8-9 bit frame with extended information.
 ******************************************************************************/
uint16_t EUSART_RxExt(EUSART_TypeDef *eusart)
{
  while (!(eusart->STATUS & EUSART_STATUS_RXFL)) {
  } // Wait for incoming data.

  return (uint16_t)eusart->RXDATA;
}

/***************************************************************************//**
 * Transmits one frame.
 ******************************************************************************/
void EUSART_Tx(EUSART_TypeDef *eusart, uint8_t data)
{
  // Check that transmit FIFO is not full.
  while (!(eusart->STATUS & EUSART_STATUS_TXFL)) {
  }

  eusart->TXDATA = (uint32_t)data;
}

/***************************************************************************//**
 * Transmits one 8-9 bit frame with extended control.
 ******************************************************************************/
void EUSART_TxExt(EUSART_TypeDef *eusart, uint16_t data)
{
  // Check that transmit FIFO is not full.
  while (!(eusart->STATUS & EUSART_STATUS_TXFL)) {
  }

  eusart->TXDATA = (uint32_t)data;
}

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * Transmits one 8-16 bit frame and return received data.
 ******************************************************************************/
uint16_t EUSART_Spi_TxRx(EUSART_TypeDef *eusart, uint16_t data)
{
  // Check that transmit FIFO is not full.
  while (!(eusart->STATUS & EUSART_STATUS_TXFL)) {
  }
  eusart->TXDATA = (uint32_t)data;

  // Wait for Rx data to be available.
  while (!(eusart->STATUS & EUSART_STATUS_RXFL)) {
  }
  return (uint16_t)eusart->RXDATA;
}

#if defined(EUSART_DALICFG_DALIEN)
/***************************************************************************//**
 * Transmits one frame.
 ******************************************************************************/
void EUSART_Dali_Tx(EUSART_TypeDef *eusart, uint32_t data)
{
  uint32_t packet;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  // Check that transmit FIFO is not full.
  while (!(eusart->STATUS & EUSART_STATUS_TXFL)) {
  }

  for (uint8_t index = 0; index < dali_tx_nb_packets[EUSART_NUM(eusart)]; index++) {
    // when DALICFG.DALIEN is set to 1, then all 16 bits [15:0] represent data
    // First write to TXDATA register should contain 16 LSBs of the TX frame.
    // Transmission will not start after this first write.
    // Second write to TXDATA register should contain the remaining TX frame bits.
    // This second write will result in start of transmission.
    packet = (data >> (index * 16));
    // To ensure compatibility with future devices, always write bits [31:16] to 0.
    packet &= 0x0000FFFF;
    eusart->TXDATA = packet;
  }
}

/***************************************************************************//**
 * Receive one frame.
 ******************************************************************************/
uint32_t EUSART_Dali_Rx(EUSART_TypeDef *eusart)
{
  uint32_t data = 0;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  while (!(eusart->STATUS & EUSART_STATUS_RXFL)) {
  }   // Wait for incoming data.

  for (uint8_t index = 0; index < dali_rx_nb_packets[EUSART_NUM(eusart)]; index++) {
    // when DALICFG.DALIEN is set to 1, then all 16 bits [15:0] represent data
    // When receiving a frame that has more than 16 databits,
    // RXDATA register needs to be read twice:
    //    First read will provide 16 LSBs of the received frame.
    //    Second read will provide the remaining RX frame bits.
    data |= ((eusart->RXDATA & _EUSART_RXDATA_RXDATA_MASK) << (index * 16));
  }
  return data;
}

#endif /* EUSART_DALICFG_DALIEN */
#endif /* EUSART_PRESENT */

/***************************************************************************//**
 * Configures the baudrate (or as close as possible to a specified baudrate)
 * depending on the current mode of the EU(S)ART peripheral.
 *
 * @note (1) When the oversampling is disabled, the peripheral clock frequency
 *           must be at least three times higher than the chosen baud rate.
 ******************************************************************************/
void EUSART_BaudrateSet(EUSART_TypeDef *eusart,
                        uint32_t refFreq,
                        uint32_t baudrate)
{
  uint32_t          clkdiv;
  uint8_t           oversample = 0;

  // Prevent dividing by 0.
  EFM_ASSERT(baudrate);

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  // Get the current frequency.
  if (!refFreq) {
    refFreq = CMU_ClockFreqGet(EUSART_ClockGet(eusart));
  }

#if defined(EUSART_PRESENT)
  // In synchronous mode (ex: SPI)
  if (eusart->CFG0 & _EUSART_CFG0_SYNC_MASK ) {
    EFM_ASSERT(baudrate <= refFreq);

    EUSART_Enable_TypeDef txrxEnStatus = eusartDisable;
    bool wasEnabled = (eusart->EN & _EUSART_EN_EN_MASK) == true;
    clkdiv = refFreq / baudrate - 1UL;

    // If the desired bit rate requires a divider larger than the Synchronous divider bitfield (CFG2_SDIV),
    // the resulting spi master bus clock will be undefined because the result will be truncated.
    EFM_ASSERT(clkdiv <= (_EUSART_CFG2_SDIV_MASK >> _EUSART_CFG2_SDIV_SHIFT));

    if (wasEnabled) {
      eusart_sync(eusart, _EUSART_SYNCBUSY_RXEN_MASK | _EUSART_SYNCBUSY_TXEN_MASK);

      // Save the state of the reveiver and transmitter before disabling the peripheral.
      if (eusart->STATUS & (_EUSART_STATUS_RXENS_MASK | _EUSART_STATUS_TXENS_MASK)) {
        txrxEnStatus = eusartEnable;
      } else if (eusart->STATUS & (_EUSART_STATUS_RXENS_MASK)) {
        txrxEnStatus = eusartEnableRx;
      } else if (eusart->STATUS & (_EUSART_STATUS_TXENS_MASK)) {
        txrxEnStatus = eusartEnableTx;
      } else {
        EFM_ASSERT(false);
      }

      // Disable the eusart to be able to modify the CFG2 register.
      EUSART_Disable(eusart);
    }

    // In Synchronous mode the clock divider that is managing the bitRate
    // is located inside the sdiv bitfield of the CFG2 register instead of
    // the CLKDIV register combined with the oversample setting for asynchronous mode.
    eusart->CFG2 = (eusart->CFG2 & ~(_EUSART_CFG2_SDIV_MASK)) | ((clkdiv << _EUSART_CFG2_SDIV_SHIFT) & _EUSART_CFG2_SDIV_MASK);

    if (wasEnabled) {
      EUSART_Enable(eusart, txrxEnStatus);
    }
  } else // In asynchronous mode (ex: UART)
#endif
  {
    // The peripheral must be enabled to configure the baud rate.
    EFM_ASSERT(eusart->EN == EUSART_EN_EN);

#if defined(EUSART_DALICFG_DALIEN)
    if (eusart->DALICFG & EUSART_DALICFG_DALIEN) {
      // adjust for manchester double-clocking scheme
      baudrate *= 2;
    }
#endif

    /*
     * Use integer division to avoid forcing in float division
     * utils, and yet keep rounding effect errors to a minimum.
     *
     * CLKDIV is given by:
     *
     * CLKDIV = 256 * (fUARTn/(oversample * br) - 1)
     * or
     * CLKDIV = (256 * fUARTn)/(oversample * br) - 256
     *
     * Since fUARTn may be derived from HFCORECLK, consider the overflow when
     * using integer arithmetic.
     *
     * The basic problem with integer division in the above formula is that
     * the dividend (256 * fUARTn) may become higher than the maximum 32 bit
     * integer. Yet, the dividend should be evaluated first before dividing
     * to get as small rounding effects as possible.
     * Also, harsh restrictions on the maximum fUARTn value should not be made.
     *
     * Since the last 3 bits of CLKDIV are don't care, base the
     * integer arithmetic on the below formula:
     *
     * CLKDIV/8 = ((32*fUARTn)/(br * Oversample)) - 32
     *
     * and calculate 1/8 of CLKDIV first. This allows for fUARTn
     * up to 128 MHz without overflowing a 32 bit value.
     */

    // Map oversampling.
    switch (eusart->CFG0 & _EUSART_CFG0_OVS_MASK) {
      case eusartOVS16:
        EFM_ASSERT(baudrate <= (refFreq / 16));
        oversample = 16;
        break;

      case eusartOVS8:
        EFM_ASSERT(baudrate <= (refFreq / 8));
        oversample = 8;
        break;

      case eusartOVS6:
        EFM_ASSERT(baudrate <= (refFreq / 6));
        oversample = 6;
        break;

      case eusartOVS4:
        EFM_ASSERT(baudrate <= (refFreq / 4));
        oversample = 4;
        break;

      case eusartOVS0:
        EFM_ASSERT(refFreq >= (3 * baudrate)); // See Note #1.
        oversample = 1;
        break;

      default:
        // Invalid input
        EFM_ASSERT(0);
        break;
    }

    if (oversample > 0U) {
      // Calculate and set the CLKDIV with fractional bits.
      clkdiv  = (32 * refFreq) / (baudrate * oversample);
      clkdiv -= 32;
      clkdiv *= 8;

      // Verify that the resulting clock divider is within limits.
      EFM_ASSERT(clkdiv <= _EUSART_CLKDIV_MASK);

      // If the EFM_ASSERT is not enabled, make sure not to write to reserved bits.
      clkdiv &= _EUSART_CLKDIV_MASK;

      eusart_sync(eusart, _EUSART_SYNCBUSY_DIV_MASK);
      eusart->CLKDIV = clkdiv;
      eusart_sync(eusart, _EUSART_SYNCBUSY_DIV_MASK);
    }
  }
}

/***************************************************************************//**
 * Gets the current baudrate.
 ******************************************************************************/
uint32_t EUSART_BaudrateGet(EUSART_TypeDef *eusart)
{
  uint32_t freq;
  uint32_t div = 1;
  uint32_t br = 0;
  EUSART_OVS_TypeDef ovs = eusartOVS0;

  // Make sure the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  freq = CMU_ClockFreqGet(EUSART_ClockGet(eusart));

#if defined(EUSART_PRESENT)
  // In synchronous mode (ex: SPI)
  if (eusart->CFG0 & _EUSART_CFG0_SYNC_MASK) {
    div = (eusart->CFG2 & _EUSART_CFG2_SDIV_MASK) >> _EUSART_CFG2_SDIV_SHIFT;
    br = freq / (div + 1);
  }
  // In asynchronous mode (ex: UART)
  else
#endif
  {
    div = eusart->CLKDIV;
    ovs = (EUSART_OVS_TypeDef)(eusart->CFG0 & _EUSART_CFG0_OVS_MASK);
    br = EUSART_AsyncBaudrateCalc(freq, div, ovs);

#if defined(EUSART_DALICFG_DALIEN)
    if (eusart->DALICFG & EUSART_DALICFG_DALIEN) {
      // adjust for manchester double-clocking scheme
      br /= 2;
    }
#endif
  }

  return br;
}

/***************************************************************************//**
 * Enable/Disable reception operations until the configured start frame is
 * received.
 ******************************************************************************/
void EUSART_RxBlock(EUSART_TypeDef *eusart, EUSART_BlockRx_TypeDef enable)
{
  uint32_t tmp;

  // Make sure that the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  tmp   = ((uint32_t)(enable));
  tmp  &= (_EUSART_CMD_RXBLOCKEN_MASK | _EUSART_CMD_RXBLOCKDIS_MASK);

  eusart_sync(eusart, EUSART_SYNCBUSY_RXBLOCKEN | EUSART_SYNCBUSY_RXBLOCKDIS);
  eusart->CMD_SET = tmp;
  eusart_sync(eusart, EUSART_SYNCBUSY_RXBLOCKEN | EUSART_SYNCBUSY_RXBLOCKDIS);

  tmp = 0u;
  if ((_EUSART_CMD_RXBLOCKEN_MASK & enable) != 0u) {
    tmp |= EUSART_STATUS_RXBLOCK;
  }
  while ((eusart->STATUS & _EUSART_STATUS_RXBLOCK_MASK) != tmp) {
  } // Wait for the status register to be updated.
}

/***************************************************************************//**
 * Enables/Disables the tristating of the transmitter output.
 ******************************************************************************/
void  EUSART_TxTristateSet(EUSART_TypeDef *eusart,
                           EUSART_TristateTx_TypeDef enable)
{
  uint32_t tmp;

  // Make sure that the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  tmp   = ((uint32_t)(enable));
  tmp  &= (_EUSART_CMD_TXTRIEN_MASK | _EUSART_CMD_TXTRIDIS_MASK);

  eusart_sync(eusart, EUSART_SYNCBUSY_TXTRIEN | EUSART_SYNCBUSY_TXTRIDIS);
  eusart->CMD = tmp;
  eusart_sync(eusart, EUSART_SYNCBUSY_TXTRIEN | EUSART_SYNCBUSY_TXTRIDIS);

  tmp = 0u;
  if ((_EUSART_CMD_TXTRIEN_MASK & enable) != 0u) {
    tmp |= EUSART_STATUS_TXTRI;
  }
  while ((eusart->STATUS & _EUSART_STATUS_TXTRI_MASK) != tmp) {
  } // Wait for the status register to be updated.
}

/***************************************************************************//**
 * Initializes the automatic enabling of transmissions and/or reception using
 * the PRS as a trigger.
 ******************************************************************************/
void EUSART_PrsTriggerEnable(EUSART_TypeDef *eusart,
                             const EUSART_PrsTriggerInit_TypeDef *init)
{
  uint32_t tmp;

  // Make sure that the module exists on the selected chip.
  EFM_ASSERT(EUSART_REF_VALID(eusart));

  // The peripheral must be enabled to configure the PRS trigger.
  EFM_ASSERT(eusart->EN == EUSART_EN_EN);

#if defined(EUART_PRESENT)
  PRS->CONSUMER_EUART0_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUART0_TRIGGER_MASK);
#else

#if defined(EUSART0)
  if (eusart == EUSART0) {
    PRS->CONSUMER_EUSART0_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUSART0_TRIGGER_MASK);
  }
#endif
#if defined(EUSART1)
  if (eusart == EUSART1) {
    PRS->CONSUMER_EUSART1_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUSART1_TRIGGER_MASK);
  }
#endif
#if defined(EUSART2)
  if (eusart == EUSART2) {
    PRS->CONSUMER_EUSART2_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUSART2_TRIGGER_MASK);
  }
#endif
#if defined(EUSART3)
  if (eusart == EUSART3) {
    PRS->CONSUMER_EUSART3_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUSART3_TRIGGER_MASK);
  }
#endif
#if defined(EUSART4)
  if (eusart == EUSART4) {
    PRS->CONSUMER_EUSART4_TRIGGER = (init->prs_trigger_channel & _PRS_CONSUMER_EUSART4_TRIGGER_MASK);
  }
#endif
#endif

  tmp   = ((uint32_t)(init->prs_trigger_enable));
  tmp  &= (_EUSART_TRIGCTRL_RXTEN_MASK | _EUSART_TRIGCTRL_TXTEN_MASK);

  eusart->TRIGCTRL_SET = tmp;
  eusart_sync(eusart, EUSART_SYNCBUSY_RXTEN | EUSART_SYNCBUSY_TXTEN);

  tmp   = ~((uint32_t)(init->prs_trigger_enable));
  tmp  &= (_EUSART_TRIGCTRL_RXTEN_MASK | _EUSART_TRIGCTRL_TXTEN_MASK);
  eusart->TRIGCTRL_CLR = tmp;
  eusart_sync(eusart, EUSART_SYNCBUSY_RXTEN | EUSART_SYNCBUSY_TXTEN);
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Gets the clock associated to the specified EUSART instance.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 *
 * @return Clock corresponding to the eusart.
 ******************************************************************************/
static CMU_Clock_TypeDef EUSART_ClockGet(EUSART_TypeDef *eusart)
{
  CMU_Clock_TypeDef clock;

#if defined(EUART0)
  if (eusart == EUART0) {
    clock = cmuClock_EUART0;
  }
#endif
#if defined(EUSART0)
  if (eusart == EUSART0) {
    clock = cmuClock_EUSART0;
  }
#endif
#if defined(EUSART1)
  else if (eusart == EUSART1) {
    clock = cmuClock_EUSART1;
  }
#endif
#if defined(EUSART2)
  else if (eusart == EUSART2) {
    clock = cmuClock_EUSART2;
  }
#endif
#if defined(EUSART3)
  else if (eusart == EUSART3) {
    clock = cmuClock_EUSART3;
  }
#endif
#if defined(EUSART4)
  else if (eusart == EUSART4) {
    clock = cmuClock_EUSART4;
  }
#endif
  else {
    EFM_ASSERT(0);
    return (CMU_Clock_TypeDef)0u;
  }
  return clock;
}

/***************************************************************************//**
 * Initializes the EUSART with asynchronous common settings to high
 * and low frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init A pointer to the initialization structure.
 * @param irdaInit Pointer to IrDA initialization structure.
 ******************************************************************************/
static void EUSART_AsyncInitCommon(EUSART_TypeDef *eusart,
                                   const EUSART_UartInit_TypeDef  *init,
                                   const EUSART_IrDAInit_TypeDef  *irdaInit,
                                   const EUSART_DaliInit_TypeDef  *daliInit)
{
  // LF register about to be modified requires sync busy check.
  if (eusart->EN) {
    eusart_sync(eusart, _EUSART_SYNCBUSY_MASK);
  }
  // Initialize EUSART registers to hardware reset state.
  EUSART_Reset(eusart);

  // Configure frame format
  eusart->FRAMECFG = (eusart->FRAMECFG & ~(_EUSART_FRAMECFG_DATABITS_MASK
                                           | _EUSART_FRAMECFG_STOPBITS_MASK
                                           | _EUSART_FRAMECFG_PARITY_MASK))
                     | (uint32_t)(init->databits)
                     | (uint32_t)(init->parity)
                     | (uint32_t)(init->stopbits);

  // Configure global configuration register 0.
  eusart->CFG0 = (eusart->CFG0 & ~(_EUSART_CFG0_OVS_MASK
                                   | _EUSART_CFG0_LOOPBK_MASK
                                   | _EUSART_CFG0_MVDIS_MASK))
                 | (uint32_t)(init->oversampling)
                 | (uint32_t)(init->loopbackEnable)
                 | (uint32_t)(init->majorityVote);

  if (init->baudrate == 0) {
    eusart->CFG0 |= EUSART_CFG0_AUTOBAUDEN;
  }

  if (init->advancedSettings) {
    eusart->CFG0 |= (uint32_t)init->advancedSettings->dmaHaltOnError << _EUSART_CFG0_ERRSDMA_SHIFT;
    eusart->CFG0 |= (uint32_t)init->advancedSettings->txAutoTristate << _EUSART_CFG0_AUTOTRI_SHIFT;
    eusart->CFG0 |= (uint32_t)init->advancedSettings->invertIO & (_EUSART_CFG0_RXINV_MASK | _EUSART_CFG0_TXINV_MASK);
    eusart->CFG0 |= (uint32_t)init->advancedSettings->collisionDetectEnable << _EUSART_CFG0_CCEN_SHIFT;
    eusart->CFG0 |= (uint32_t)init->advancedSettings->multiProcessorEnable << _EUSART_CFG0_MPM_SHIFT;
    eusart->CFG0 |= (uint32_t)init->advancedSettings->multiProcessorAddressBitHigh << _EUSART_CFG0_MPAB_SHIFT;
    eusart->CFG0 |= (uint32_t)init->advancedSettings->msbFirst << _EUSART_CFG0_MSBF_SHIFT;

    // Configure global configuration register 1.
    eusart->CFG1 = (uint32_t)init->advancedSettings->dmaWakeUpOnRx << _EUSART_CFG1_RXDMAWU_SHIFT
                   | (uint32_t)init->advancedSettings->dmaWakeUpOnTx << _EUSART_CFG1_TXDMAWU_SHIFT;

    if (init->advancedSettings->hwFlowControl == eusartHwFlowControlCts
        || init->advancedSettings->hwFlowControl == eusartHwFlowControlCtsAndRts) {
      eusart->CFG1 |= EUSART_CFG1_CTSEN;
    }
    // Enable RTS route pin if necessary. CTS is an input so it is enabled by default.
    if ((init->advancedSettings->hwFlowControl == eusartHwFlowControlRts)
        || (init->advancedSettings->hwFlowControl == eusartHwFlowControlCtsAndRts)) {
#if defined(EUART0)
      GPIO->EUARTROUTE_SET->ROUTEEN = GPIO_EUART_ROUTEEN_RTSPEN;
#elif defined(EUSART0)
      GPIO->EUSARTROUTE_SET[EUSART_NUM(eusart)].ROUTEEN = GPIO_EUSART_ROUTEEN_RTSPEN;
#endif
    } else {
#if defined(EUART0)
      GPIO->EUARTROUTE_CLR->ROUTEEN = GPIO_EUART_ROUTEEN_RTSPEN;
#elif defined(EUSART0)
      GPIO->EUSARTROUTE_CLR[EUSART_NUM(eusart)].ROUTEEN = GPIO_EUSART_ROUTEEN_RTSPEN;
#endif
    }
    eusart->STARTFRAMECFG_SET = (uint32_t)init->advancedSettings->startFrame;
    if (init->advancedSettings->startFrame) {
      eusart->CFG1 |= EUSART_CFG1_SFUBRX;
    }
    if (init->advancedSettings->prsRxEnable) {
      eusart->CFG1 |= EUSART_CFG1_RXPRSEN;
      // Configure PRS channel as input data line for EUSART.
#if defined(EUART_PRESENT)
      PRS->CONSUMER_EUART0_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUART0_RX_MASK);
#elif defined(EUSART_PRESENT)

      if (eusart == EUSART0) {
        PRS->CONSUMER_EUSART0_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART0_RX_MASK);
      }
#if defined(EUSART1)
      if (eusart == EUSART1) {
      }
      PRS->CONSUMER_EUSART1_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART1_RX_MASK);
#endif
#if defined(EUSART2)
      if (eusart == EUSART2) {
        PRS->CONSUMER_EUSART2_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART2_RX_MASK);
      }
#endif
#if defined(EUSART3)
      if (eusart == EUSART3) {
        PRS->CONSUMER_EUSART3_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART3_RX_MASK);
      }
#endif
#if defined(EUSART4)
      if (eusart == EUSART4) {
        PRS->CONSUMER_EUSART4_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART4_RX_MASK);
      }
#endif
#endif
    }
  }

  if (irdaInit) {
    if (irdaInit->irDALowFrequencyEnable) {
      eusart->IRLFCFG_SET = (uint32_t)(EUSART_IRLFCFG_IRLFEN);
    } else {
      // Configure IrDA HF configuration register.
      eusart->IRHFCFG_SET = (eusart->IRHFCFG & ~(_EUSART_IRHFCFG_IRHFEN_MASK
                                                 | _EUSART_IRHFCFG_IRHFEN_MASK
                                                 | _EUSART_IRHFCFG_IRHFFILT_MASK))
                            | (uint32_t)(EUSART_IRHFCFG_IRHFEN)
                            | (uint32_t)(irdaInit->irDAPulseWidth)
                            | (uint32_t)(irdaInit->irDARxFilterEnable);
    }
  }

#if defined(EUSART_DALICFG_DALIEN)
  // DALI-specific configuration section
  if (daliInit) {
    if (init->loopbackEnable) {
      // If LOOPBK in CFG0 is set to 1 in order to do loopback testing for DALI,
      // then in this case DALIRXENDT should be set to 1.
      eusart->DALICFG_SET = EUSART_DALICFG_DALIRXENDT;
    }

    // keep track of the number of 16-bits packet to send
    if (daliInit->TXdatabits <= eusartDaliTxDataBits16) {
      dali_tx_nb_packets[EUSART_NUM(eusart)] = 1;
    } else {
      dali_tx_nb_packets[EUSART_NUM(eusart)] = 2;
    }

    // keep track of the number of 16-bits packet to receive
    if (daliInit->RXdatabits <= eusartDaliRxDataBits16) {
      dali_rx_nb_packets[EUSART_NUM(eusart)] = 1;
    } else {
      dali_rx_nb_packets[EUSART_NUM(eusart)] = 2;
    }

    // Configure the numbers of bits per TX and RX frames
    eusart->DALICFG = (eusart->DALICFG & ~(_EUSART_DALICFG_DALITXDATABITS_MASK
                                           | _EUSART_DALICFG_DALIRXDATABITS_MASK))
                      | daliInit->TXdatabits
                      | daliInit->RXdatabits;
    eusart->DALICFG_SET = EUSART_DALICFG_DALIEN;
  }
#else
  (void)(daliInit);
#endif /* EUSART_DALICFG_DALIEN */

  // Enable EUSART IP.
  EUSART_Enable(eusart, eusartEnable);

  // Configure the baudrate if auto baud detection is not used.
  if (init->baudrate) {
    EUSART_BaudrateSet(eusart, init->refFreq, init->baudrate);
  }

  // Finally enable the Rx and/or Tx channel (as specified).
  EUSART_Enable(eusart, init->enable);
  while (~EUSART_StatusGet(eusart) & (_EUSART_STATUS_RXIDLE_MASK | _EUSART_STATUS_TXIDLE_MASK)) {
  }
}

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * Initializes the EUSART with synchronous common settings to high
 * and low frequency clock.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 * @param init A pointer to the initialization structure.
 ******************************************************************************/
static void EUSART_SyncInitCommon(EUSART_TypeDef *eusart,
                                  EUSART_SpiInit_TypeDef const *init)
{
  void* advancedSetting_ptr = (void*)init->advancedSettings; // Used to avoid GCC over optimization.

  // LF register about to be modified requires sync busy check.
  if (eusart->EN) {
    eusart_sync(eusart, _EUSART_SYNCBUSY_MASK);
  }

  // Initialize EUSART registers to hardware reset state.
  EUSART_Reset(eusart);

  // Configure global configuration register 2.
  eusart->CFG2 = (eusart->CFG2 & ~(_EUSART_CFG2_MASTER_MASK
                                   | _EUSART_CFG2_CLKPOL_MASK
                                   | _EUSART_CFG2_CLKPHA_MASK
                                   | _EUSART_CFG2_FORCELOAD_MASK))
                 | (uint32_t)(init->master)
                 | (uint32_t)(init->clockMode)
                 | (uint32_t)(EUSART_CFG2_FORCELOAD); // Force load feature enabled by default.

  if (advancedSetting_ptr) {
    // Configure global configuration register 2.
    eusart->CFG2 = (eusart->CFG2 & ~(_EUSART_CFG2_FORCELOAD_MASK
                                     | _EUSART_CFG2_AUTOCS_MASK
                                     | _EUSART_CFG2_AUTOTX_MASK
                                     | _EUSART_CFG2_CSINV_MASK
                                     | _EUSART_CFG2_CLKPRSEN_MASK))
                   | (uint32_t)(init->advancedSettings->forceLoad << _EUSART_CFG2_FORCELOAD_SHIFT)
                   | (uint32_t)(init->advancedSettings->autoCsEnable << _EUSART_CFG2_AUTOCS_SHIFT)
                   | (uint32_t)(init->advancedSettings->autoTxEnable << _EUSART_CFG2_AUTOTX_SHIFT)
                   | (uint32_t)(init->advancedSettings->csPolarity)
                   | (uint32_t)(init->advancedSettings->prsClockEnable << _EUSART_CFG2_CLKPRSEN_SHIFT);

    // Only applicable to EM2 (low frequency) capable EUSART instances.
    eusart->CFG1 = (eusart->CFG1 & ~(_EUSART_CFG1_RXFIW_MASK
                                     | _EUSART_CFG1_TXFIW_MASK))
                   | (uint32_t)(init->advancedSettings->RxFifoWatermark)
                   | (uint32_t)(init->advancedSettings->TxFifoWatermark)
                   | (uint32_t)(init->advancedSettings->dmaWakeUpOnRx << _EUSART_CFG1_RXDMAWU_SHIFT)
                   | (uint32_t)(init->advancedSettings->prsRxEnable << _EUSART_CFG1_RXPRSEN_SHIFT);
  }

  eusart->CFG0 = (eusart->CFG0 & ~(_EUSART_CFG0_SYNC_MASK
                                   | _EUSART_CFG0_LOOPBK_MASK))
                 | (uint32_t)(_EUSART_CFG0_SYNC_SYNC)
                 | (uint32_t)(init->loopbackEnable);

  if (advancedSetting_ptr) {
    eusart->CFG0 |= (uint32_t)init->advancedSettings->invertIO & (_EUSART_CFG0_RXINV_MASK | _EUSART_CFG0_TXINV_MASK);
    eusart->CFG0 |= (uint32_t)init->advancedSettings->msbFirst << _EUSART_CFG0_MSBF_SHIFT;

    // Configure global configurationTiming register.
    eusart->TIMINGCFG = (eusart->TIMINGCFG & ~(_EUSART_TIMINGCFG_CSSETUP_MASK
                                               | _EUSART_TIMINGCFG_CSHOLD_MASK
                                               | _EUSART_TIMINGCFG_ICS_MASK
                                               | _EUSART_TIMINGCFG_SETUPWINDOW_MASK))
                        | ((uint32_t)(init->advancedSettings->autoCsSetupTime << _EUSART_TIMINGCFG_CSSETUP_SHIFT)
                           & _EUSART_TIMINGCFG_CSSETUP_MASK)
                        | ((uint32_t)(init->advancedSettings->autoCsHoldTime << _EUSART_TIMINGCFG_CSHOLD_SHIFT)
                           & _EUSART_TIMINGCFG_CSHOLD_MASK)
                        | ((uint32_t)(init->advancedSettings->autoInterFrameTime << _EUSART_TIMINGCFG_ICS_SHIFT)
                           & _EUSART_TIMINGCFG_ICS_MASK)
                        | ((uint32_t)(init->advancedSettings->setupWindow << _EUSART_TIMINGCFG_SETUPWINDOW_SHIFT)
                           & _EUSART_TIMINGCFG_SETUPWINDOW_MASK)
    ;
  }

  // Configure frame format
  eusart->FRAMECFG = (eusart->FRAMECFG & ~(_EUSART_FRAMECFG_DATABITS_MASK))
                     | (uint32_t)(init->databits);

  if (advancedSetting_ptr) {
    eusart->DTXDATCFG = (init->advancedSettings->defaultTxData & _EUSART_DTXDATCFG_MASK);

    if (init->advancedSettings->prsRxEnable) {
      //Configure PRS channel as input data line for EUSART.
      if (eusart == EUSART0) {
        PRS->CONSUMER_EUSART0_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART0_RX_MASK);
      }
#if defined(EUSART1)
      if (eusart == EUSART1) {
        PRS->CONSUMER_EUSART1_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART1_RX_MASK);
      }
#endif
#if defined(EUSART2)
      if (eusart == EUSART2) {
        PRS->CONSUMER_EUSART2_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART2_RX_MASK);
      }
#endif
#if defined(EUSART3)
      if (eusart == EUSART3) {
        PRS->CONSUMER_EUSART3_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART3_RX_MASK);
      }
#endif
#if defined(EUSART4)
      if (eusart == EUSART4) {
        PRS->CONSUMER_EUSART4_RX_SET = (init->advancedSettings->prsRxChannel & _PRS_CONSUMER_EUSART4_RX_MASK);
      }
#endif
    }

    if (init->advancedSettings->prsClockEnable) {
      //Configure PRS channel as SCLK input for EUSART.
      if (eusart == EUSART0) {
        PRS->CONSUMER_EUSART0_CLK_SET = (init->advancedSettings->prsClockChannel & _PRS_CONSUMER_EUSART0_CLK_MASK);
      }
#if defined(EUSART1)
      if (eusart == EUSART1) {
        PRS->CONSUMER_EUSART1_CLK_SET = (init->advancedSettings->prsClockChannel & _PRS_CONSUMER_EUSART1_CLK_MASK);
      }
#endif
#if defined(EUSART2)
      if (eusart == EUSART2) {
        PRS->CONSUMER_EUSART2_CLK_SET = (init->advancedSettings->prsClockChannel & _PRS_CONSUMER_EUSART2_CLK_MASK);
      }
#endif
#if defined(EUSART3)
      if (eusart == EUSART3) {
        PRS->CONSUMER_EUSART3_CLK_SET = (init->advancedSettings->prsClockChannel & _PRS_CONSUMER_EUSART3_CLK_MASK);
      }
#endif
#if defined(EUSART4)
      if (eusart == EUSART4) {
        PRS->CONSUMER_EUSART4_CLK_SET = (init->advancedSettings->prsClockChannel & _PRS_CONSUMER_EUSART4_CLK_MASK);
      }
#endif
    }
  }

  // Set baudrate for synchronous operation mode.
  EUSART_BaudrateSet(eusart, init->refFreq, init->bitRate);

  // Enable EUSART IP.
  EUSART_Enable(eusart, eusartEnable);

  // Finally enable the Rx and/or Tx channel (as specified).
  eusart_sync(eusart, _EUSART_SYNCBUSY_RXEN_MASK | _EUSART_SYNCBUSY_TXEN_MASK); // Wait for low frequency register synchronization.
  eusart->CMD = (uint32_t)init->enable;
  eusart_sync(eusart, _EUSART_SYNCBUSY_RXEN_MASK | _EUSART_SYNCBUSY_TXEN_MASK);
  while (~EUSART_StatusGet(eusart) & (_EUSART_STATUS_RXIDLE_MASK | _EUSART_STATUS_TXIDLE_MASK)) {
  }
}
#endif

/***************************************************************************//**
 * Calculate baudrate for a given reference frequency, clock division,
 * and oversampling rate when the module is in UART mode.
 *
 * @param refFreq The EUSART reference clock frequency in Hz that will be used.
 * @param clkdiv Clock division factor to be used.
 * @param ovs Oversampling to be used.
 *
 * @return Computed baudrate from given settings.
 ******************************************************************************/
__STATIC_INLINE uint32_t EUSART_AsyncBaudrateCalc(uint32_t refFreq,
                                                  uint32_t clkdiv,
                                                  EUSART_OVS_TypeDef ovs)
{
  uint32_t oversample;
  uint64_t divisor;
  uint64_t factor;
  uint64_t remainder;
  uint64_t quotient;
  uint32_t br;

  // Out of bound clkdiv.
  EFM_ASSERT(clkdiv <= _EUSART_CLKDIV_MASK);

  // Mask out unused bits
  clkdiv &= _EUSART_CLKDIV_MASK;

  /* Use integer division to avoid forcing in float division
   * utils and yet keep rounding effect errors to a minimum.
   *
   * Baudrate in is given by:
   *
   * br = fUARTn/(oversample * (1 + (CLKDIV / 256)))
   * or
   * br = (256 * fUARTn)/(oversample * (256 + CLKDIV))
   *
   * 256 factor of the dividend is reduced with a
   * (part of) oversample part of the divisor.
   */

  switch (ovs) {
    case eusartOVS16:
      oversample = 1;
      factor = 256 / 16;
      break;

    case eusartOVS8:
      oversample = 1;
      factor = 256 / 8;
      break;

    case eusartOVS6:
      oversample = 3;
      factor = 256 / 2;
      break;

    case eusartOVS4:
      oversample = 1;
      factor = 256 / 4;
      break;

    case eusartOVS0:
      oversample = 1;
      factor = 256;
      break;

    default:
      return 0u;
      break;
  }

  /*
   * The basic problem with integer division in the above formula is that
   * the dividend (factor * fUARTn) may become larger than a 32 bit
   * integer. Yet we want to evaluate the dividend first before dividing
   * to get as small rounding effects as possible. Too harsh restrictions
   * should not be made on the maximum fUARTn value either.
   *
   * For division a/b,
   *
   * a = qb + r
   *
   * where q is the quotient and r is the remainder, both integers.
   *
   * The original baudrate formula can be rewritten as
   *
   * br = xa / b = x(qb + r)/b = xq + xr/b
   *
   * where x is 'factor', a is 'refFreq' and b is 'divisor', referring to
   * variable names.
   */

  /*
   * The divisor will never exceed max 32 bit value since
   * clkdiv <= _EUSART_CLKDIV_MASK (currently 0x7FFFF8)
   * and 'oversample' has been reduced to <= 3.
   */
  divisor = (uint64_t)(oversample * (256 + clkdiv));

  quotient = refFreq / divisor;
  remainder = refFreq % divisor;

  // The factor <= 128 and since divisor >= 256, the below cannot exceed the maximum
  // 32 bit value. However, factor * remainder can become larger than 32-bit
  // because of the size of _EUSART_CLKDIV_DIV_MASK on some families.
  br = (uint32_t) (factor * quotient);

  /*
   * The factor <= 128 and remainder < (oversample*(256 + clkdiv)), which
   * means dividend (factor * remainder) worst case is
   * 128 * (3 * (256 + _EUSART_CLKDIV_MASK)) = 0xC001_7400.
   */
  br += (uint32_t) ((factor * remainder) / divisor);

  return br;
}

/***************************************************************************//**
 * Perform EUSART Module disablement - resetting all internal flops/FSM.
 *
 * @param eusart Pointer to the EUSART peripheral register block.
 ******************************************************************************/
__STATIC_INLINE void EUSART_Disable(EUSART_TypeDef *eusart)
{
  if (eusart->EN & _EUSART_EN_EN_MASK) {
    // This step should be skipped especially in Synchronous Slave mode when
    // external SCLK is not running and CS is active
#if defined(EUSART_PRESENT)
    if (!(eusart->CFG0 & _EUSART_CFG0_SYNC_MASK) || (eusart->CFG2 & _EUSART_CFG2_MASTER_MASK))
#endif
    {
      // General Programming Guideline to properly disable the module:
      // 1a. Disable TX and RX using TXDIS and RXDIS cmd
      eusart->CMD = EUSART_CMD_TXDIS | EUSART_CMD_RXDIS;
      // 1b. Poll for EUSARTn_SYNCBUSY.TXDIS and EUSARTn_SYNCBUSY.RXDIS to go low;
      eusart_sync(eusart, (EUSART_SYNCBUSY_TXDIS | EUSART_SYNCBUSY_RXDIS));
      // 1c. Wait for EUSARTn_STATUS.TXENS and EUSARTn_STATUS.RXENS to go low
      while (eusart->STATUS & (_EUSART_STATUS_TXENS_MASK | _EUSART_STATUS_RXENS_MASK)) {
      }
    }
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
    eusart->CLKDIV = eusart->CLKDIV;
    eusart_sync(eusart, _EUSART_SYNCBUSY_DIV_MASK);

    // Read data until FIFO is emptied
    // but taking care not to underflow the receiver
    while (eusart->STATUS & EUSART_STATUS_RXFL) {
      eusart->RXDATA;
    }
#endif

    eusart->EN_CLR = EUSART_EN_EN;

#if defined(_EUSART_EN_DISABLING_MASK)
    // 2. Polling for EUSARTn_EN.DISABLING = 0.
    while (eusart->EN & _EUSART_EN_DISABLING_MASK) {
    }
#endif
  }
}

#endif /* defined(EUART_PRESENT) || defined(EUSART_PRESENT) */
