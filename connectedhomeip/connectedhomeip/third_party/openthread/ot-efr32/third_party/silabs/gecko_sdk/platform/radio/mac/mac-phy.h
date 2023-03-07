/***************************************************************************//**
 * @file mac-phy.h
 * @brief Definitions moved from phy.h. this file should prefereably included indirectly
 * through other umac header file upper-mac.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __PHY_H__
#define __PHY_H__

#if     PHY_THIS
  #define PHY_THIS_ID (PHY_THIS - 1)
#else//!PHY_THIS
  #define PHY_THIS_ID 0
#endif//PHY_THIS

typedef uint8_t RadioPowerMode;
enum {
  EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_OFF,
  EMBER_RADIO_POWER_MODE_ED_ON,
  EMBER_RADIO_POWER_MODE_DC_RX,
};

#define RADIO_TRANSMIT_CONFIG_DEFAULTS                           \
  {                                                              \
    true,  /* waitForAck */                                      \
    true,  /* checkCca */                                        \
    RADIO_CCA_ATTEMPT_MAX_DEFAULT,      /* ccaAttemptMax */      \
    RADIO_BACKOFF_EXPONENT_MIN_DEFAULT, /* backoffExponentMin */ \
    RADIO_BACKOFF_EXPONENT_MAX_DEFAULT, /* backoffExponentMax */ \
    RADIO_MINIMUM_BACKOFF_DEFAULT,      /* minimumBackoff */     \
    true  /* appendCrc */                                        \
  }

typedef struct {
  bool waitForAck;       // Wait for ACK if ACK request set in FCF.
  bool checkCca;         // Backoff and check CCA before transmit.
  uint8_t ccaAttemptMax;      // Number of CCA attempts before failure.
  uint8_t backoffExponentMin; // Backoff exponent for the initial CCA attempt.
  uint8_t backoffExponentMax; // Backoff exponent for the final CCA attempt(s).
  uint8_t minimumBackoff;     // Minimum number of backoffs.
  bool appendCrc;        // Append CRC to transmitted packets.
} RadioTransmitConfig;

#ifndef MAC_HAS_CHANNEL_PAGES
  #ifdef  EMBER_STACK_CONNECT
    #define MAC_HAS_CHANNEL_PAGES 0
  #elif   defined(EMBER_TEST)
    #define MAC_HAS_CHANNEL_PAGES 1
  #else
    #define MAC_HAS_CHANNEL_PAGES (PHY_PRO2PLUS || PHY_DUAL || PHY_RAIL || PHY_EFR32)
  #endif
#endif//MAC_HAS_CHANNEL_PAGES

#if     MAC_HAS_CHANNEL_PAGES

  #define MAX_CHANNELS_PER_PAGE     27u // channels 0-26 // Must be < 32!
  #define CHANNEL_BITS              5u  // need 5 bits for 27 channels
// Some macros for messing with single-byte-encoded MAC Page+Channel values
  #define emMacPgChanPg(macPgChan)  ((uint8_t)(macPgChan) >> CHANNEL_BITS)
  #define emMacPgChanCh(macPgChan)  ((uint8_t)(macPgChan) & (BIT(CHANNEL_BITS) - 1))
  #define emMacPgChan(page, chan)   (((uint8_t)((page) << CHANNEL_BITS)) \
                                     | ((chan) & (BIT(CHANNEL_BITS) - 1)))

uint8_t emPhyGetChannelPageForChannel(uint8_t macPgChan);
uint8_t emPhyGetChannelPageInUse(void);

#else//!MAC_HAS_CHANNEL_PAGES

  #undef  MAC_HAS_CHANNEL_PAGES // Prevent some lame-o using #ifdef vs. #if
  #define MAX_CHANNELS_PER_PAGE     255u // channels 0-254
  #define CHANNEL_BITS              8u   // need 8 bits for 255 channels
// Some macros for messing with single-byte-encoded MAC Page+Channel values
  #define emMacPgChanPg(macPgChan)  0
  #define emMacPgChanCh(macPgChan)  (macPgChan)
  #define emMacPgChan(page, chan)   (chan)
  #define emPhyGetChannelPageForChannel(macPgChan)  (0)
  #define emPhyGetChannelPageInUse()                (0)
  #define emPhyGetChannelPageForChannel(macPgChan)  (0)

#endif//MAC_HAS_CHANNEL_PAGES

// 802.15.4 PHY payload length.
#define PHY_MAX_SIZE          127
// 802.15.4 PHY header length.
#define PHY_LENGTH_BYTE_SIZE  1
// 802.15.4 MAC footer length
#define PHY_APPENDED_CRC_SIZE 2

#ifndef EMBER_PHY_MAX_PHR_BYTES // it is 2 on subghz
// Basic timing parameters for 802.15.4-2011 QPSK 2.4 GHz PHY
#define EMBER_PHY_MAX_PHR_BYTES      1        // 802.15.4-2003/2006/2011
#endif

#ifndef MAX_RADIO_POWER
#define MAX_RADIO_POWER 5
#endif

#define SL_MIN_802_15_4_CHANNEL_NUMBER  11
#define SL_MAX_802_15_4_CHANNEL_NUMBER  26

/**
 * @brief There are sixteen 802.15.4 channels.
 */
#define SL_NUM_802_15_4_CHANNELS \
  (SL_MAX_802_15_4_CHANNEL_NUMBER - SL_MIN_802_15_4_CHANNEL_NUMBER + 1)
//-------zigbee stack calls
void emRadioInit(RadioPowerMode initialRadioPowerMode);
void emRadioSeedRandom(void);

#ifdef EMBER_TEST
bool _radioReceive(uint8_t *packet, uint32_t rxSynctime, uint8_t linkQuality);
void _radioTransmitComplete(void);
void simulatorSuspendRadioActivity(void);
void simulatorResumeRadioActivity(void);
#endif
#endif // __PHY_H__
