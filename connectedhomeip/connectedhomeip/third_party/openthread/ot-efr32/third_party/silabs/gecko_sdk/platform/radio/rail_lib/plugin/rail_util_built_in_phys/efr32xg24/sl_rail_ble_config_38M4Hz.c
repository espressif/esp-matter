/***************************************************************************//**
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config  -  DO NOT EDIT
 *   Radio Configurator Version: 2022.5.2
 *   RAIL Adapter Version: 2.4.19
 *   RAIL Compatibility: 2.x
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
#include "em_device.h"
#include "sl_rail_ble_config_38M4Hz.h"

static const uint8_t irCalConfig_0[] = {
  25, 63, 1, 6, 4, 16, 1, 0, 0, 1, 1, 6, 0, 16, 39, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t irCalConfig_1[] = {
  25, 63, 1, 6, 4, 16, 1, 0, 0, 1, 1, 6, 0, 16, 39, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0
};

static const int32_t timingConfig_0[] = {
  50000, 11250, 750
};

static const int32_t timingConfig_1[] = {
  50000, 5750, 375
};

static const int32_t timingConfig_2[] = {
  187125, 9000, 875
};

static const int32_t timingConfig_3[] = {
  49125, 9000, 875
};

static const int32_t timingConfig_4[] = {
  50000, 11750, 2000
};

static const uint8_t hfxoRetimingConfigEntries[] = {
  2, 0, 0, 0, 0x00, 0xf0, 0x49, 0x02, 6, 20, 0, 0, 0x00, 0xe0, 0x93, 0x04, 5, 56, 0, 0, 0xa0, 0x08, 0, 0, 0, 0, 0x58, 0x09, 1, 4, 7, 6, 0x10, 0x0a, 1, 4, 7, 7, 0xc8, 0x0a, 0, 4, 8, 7, 0x80, 0x0b, 0, 4, 8, 8, 0x38, 0x0c, 0, 4, 9, 8, 0x61, 0x08, 0, 0, 0, 0, 0x68, 0x08, 0, 0, 0, 0, 0xc7, 0x09, 1, 4, 4, 3, 0x2c, 0x0b, 1, 4, 4, 4, 0x92, 0x0c, 1, 4, 5, 4
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr_0 = {
#if RAIL_SUPPORTS_OFDM_PA
  {
#ifdef RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
    { 0xFFFFFFFFUL, 0xFFFFFFFFUL, },
#else
    { 0xFFFFFFFFUL },
#endif // RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
    { 0xFFFFFFFFUL, 0xFFFFFFFFUL }
  }
#else // RAIL_SUPPORTS_OFDM_PA
#ifdef RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
  { 0xFFFFFFFFUL, 0xFFFFFFFFUL, },
#else
  { 0xFFFFFFFFUL },
#endif // RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
#endif // RAIL_SUPPORTS_OFDM_PA
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr_1 = {
#if RAIL_SUPPORTS_OFDM_PA
  {
#ifdef RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
    { 0xFFFFFFFFUL, 0xFFFFFFFFUL, },
#else
    { 0xFFFFFFFFUL },
#endif // RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
    { 0xFFFFFFFFUL, 0xFFFFFFFFUL }
  }
#else // RAIL_SUPPORTS_OFDM_PA
#ifdef RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
  { 0xFFFFFFFFUL, 0xFFFFFFFFUL, },
#else
  { 0xFFFFFFFFUL },
#endif // RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
#endif // RAIL_SUPPORTS_OFDM_PA
};

static const uint32_t phyInfo_0[] = {
  13UL,
  0x00924924UL, // 146.28571428571428
  (uint32_t) NULL,
  (uint32_t) irCalConfig_0,
  (uint32_t) timingConfig_0,
  0x00000000UL,
  0UL,
  16000000UL,
  1000000UL,
  0x00F80101UL,
  0x03502555UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  999979UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

static const uint32_t phyInfo_1[] = {
  13UL,
  0x00444444UL, // 68.26666666666667
  (uint32_t) NULL,
  (uint32_t) irCalConfig_0,
  (uint32_t) timingConfig_1,
  0x00000000UL,
  0UL,
  48000000UL,
  2000000UL,
  0x00F80101UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  1999995UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

static const uint32_t phyInfo_2[] = {
  13UL,
  0x0071C71CUL, // 113.77777777777779
  (uint32_t) NULL,
  (uint32_t) irCalConfig_1,
  (uint32_t) timingConfig_2,
  0x00000000UL,
  0UL,
  36000000UL,
  1000000UL,
  0x00F60801UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  999978UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

static const uint32_t phyInfo_3[] = {
  13UL,
  0x0071C71CUL, // 113.77777777777779
  (uint32_t) NULL,
  (uint32_t) irCalConfig_1,
  (uint32_t) timingConfig_3,
  0x00000000UL,
  0UL,
  36000000UL,
  1000000UL,
  0x00F60201UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  999978UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

static const uint32_t phyInfo_4[] = {
  13UL,
  0x0071C71CUL, // 113.77777777777779
  (uint32_t) NULL,
  (uint32_t) irCalConfig_1,
  (uint32_t) timingConfig_4,
  0x00000000UL,
  0UL,
  36000000UL,
  1000000UL,
  0x00F60801UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  999978UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

const uint32_t sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase[] = {
  0x0001400CUL, 0x00018101UL,
  0x00024020UL, 0x00000000UL,
  /*    4024 */ 0x00000001UL,
  0x00034030UL, 0x00000000UL,
  /*    4034 */ 0x00000000UL,
  /*    4038 */ 0x00000000UL,
  0x00014060UL, 0x00000101UL,
  0x000140A8UL, 0x00000007UL,
  0x000440BCUL, 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  /*    40C4 */ 0x00000000UL,
  /*    40C8 */ 0x00000000UL,
  0x0003410CUL, 0x00004CFFUL,
  /*    4110 */ 0x00004DFFUL,
  /*    4114 */ 0x00004DFFUL,
  0x1001C020UL, 0x0007F800UL,
  0x3001C020UL, 0x012802F5UL,
  0x0009C024UL, 0x00001300UL,
  /*    C028 */ 0x03B380ECUL,
  /*    C02C */ 0x51407543UL,
  /*    C030 */ 0xF8000FA0UL,
  /*    C034 */ 0x00004000UL,
  /*    C038 */ 0x0007AAA8UL,
  /*    C03C */ 0x00000000UL,
  /*    C040 */ 0x00000000UL,
  /*    C044 */ 0x00000000UL,
  0x0009C070UL, 0x000010BAUL,
  /*    C074 */ 0x00200400UL,
  /*    C078 */ 0x00801804UL,
  /*    C07C */ 0x01203C0BUL,
  /*    C080 */ 0x02107C18UL,
  /*    C084 */ 0x06E0FC2FUL,
  /*    C088 */ 0x0000007FUL,
  /*    C08C */ 0x00000000UL,
  /*    C090 */ 0x00000000UL,
  0x0005C0A8UL, 0x15724BBDUL,
  /*    C0AC */ 0x0518A311UL,
  /*    C0B0 */ 0x76543210UL,
  /*    C0B4 */ 0x00000A98UL,
  /*    C0B8 */ 0x00000000UL,
  0x0004C0CCUL, 0x00000001UL,
  /*    C0D0 */ 0x00000000UL,
  /*    C0D4 */ 0x000A0001UL,
  /*    C0D8 */ 0x00280001UL,
  0x01010008UL, 0x00000708UL,
  0x01010018UL, 0x00AAAAAAUL,
  0x01010020UL, 0x00DA6000UL,
  0x0102405CUL, 0x03000000UL,
  /*    4060 */ 0x20000000UL,
  0x01044078UL, 0x71764129UL,
  /*    407C */ 0x00000000UL,
  /*    4080 */ 0x000C008CUL,
  /*    4084 */ 0x00000000UL,
  0x010440C4UL, 0x00000000UL,
  /*    40C8 */ 0x00000000UL,
  /*    40CC */ 0x00000000UL,
  /*    40D0 */ 0x00000000UL,
  0x010140E0UL, 0x00000200UL,
  0x01024110UL, 0x00051E33UL,
  /*    4114 */ 0x00000000UL,
  0x01064120UL, 0x00000000UL,
  /*    4124 */ 0x078304FFUL,
  /*    4128 */ 0x3AC81388UL,
  /*    412C */ 0x0C6606FFUL,
  /*    4130 */ 0x078304FFUL,
  /*    4134 */ 0x03FF1388UL,
  0x01014158UL, 0x00000000UL,
  0x01014164UL, 0x0000010CUL,
  0x01054184UL, 0x00000101UL,
  /*    4188 */ 0x00000000UL,
  /*    418C */ 0x00000000UL,
  /*    4190 */ 0x00000000UL,
  /*    4194 */ 0x00000000UL,
  0x010241A4UL, 0x00000000UL,
  /*    41A8 */ 0x00000000UL,
  0x010241B0UL, 0x00000000UL,
  /*    41B4 */ 0xC03E9258UL,
  0x010341BCUL, 0x00000000UL,
  /*    41C0 */ 0x003C0000UL,
  /*    41C4 */ 0x0006AAAAUL,
  0x0103422CUL, 0x40001860UL,
  /*    4230 */ 0x00000000UL,
  /*    4234 */ 0x00000000UL,
  0x0101423CUL, 0x00000000UL,
  0x01024244UL, 0x00000014UL,
  /*    4248 */ 0x00000000UL,
  0x01024280UL, 0x00000000UL,
  /*    4284 */ 0x00000081UL,
  0x01054298UL, 0x0200003FUL,
  /*    429C */ 0x0000FFFFUL,
  /*    42A0 */ 0x0000FFFFUL,
  /*    42A4 */ 0x000003FFUL,
  /*    42A8 */ 0x0000FFFFUL,
  0x010142B4UL, 0x00000000UL,
  0x010A4330UL, 0x01200040UL,
  /*    4334 */ 0x000000A0UL,
  /*    4338 */ 0x01005008UL,
  /*    433C */ 0x1F1F1F1FUL,
  /*    4340 */ 0x1B1F1F1FUL,
  /*    4344 */ 0x11131518UL,
  /*    4348 */ 0x0C0D0E10UL,
  /*    434C */ 0x2F87C145UL,
  /*    4350 */ 0x00000000UL,
  /*    4354 */ 0x00000000UL,
  0x01018010UL, 0x00000003UL,
  0x0101803CUL, 0x00000001UL,
  0x0101809CUL, 0x00000000UL,
  0x010280B0UL, 0x02000300UL,
  /*    80B4 */ 0x01000037UL,
  0x0201009CUL, 0x04000C00UL,
  0x020300D8UL, 0xAA400005UL,
  /*    00DC */ 0x00000188UL,
  /*    00E0 */ 0x000000C0UL,
  0x020100F0UL, 0x0000012BUL,
  0x12010110UL, 0x000FFF00UL,
  0x32010110UL, 0x31000002UL,
  0x12010150UL, 0x0001C000UL,
  0x32010150UL, 0x00A200C1UL,
  0x02010174UL, 0x019BF169UL,
  0x12010178UL, 0x001C0000UL,
  0x32010178UL, 0x1FE00410UL,
  0x12010180UL, 0x00000779UL,
  0x32010180UL, 0x00000002UL,
  0x02010188UL, 0x00000050UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_0_37_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_0,
  0x00014010UL, 0x00004101UL,
  0x0004403CUL, 0x00000000UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x030007A0UL,
  0x00014050UL, 0x00000000UL,
  0x0001405CUL, 0x00000000UL,
  0x00014108UL, 0x00004CFFUL,
  0x0007C054UL, 0x00301151UL,
  /*    C058 */ 0xE609000EUL,
  /*    C05C */ 0x0000002DUL,
  /*    C060 */ 0x07060504UL,
  /*    C064 */ 0x00000008UL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x00003564UL,
  0x01074040UL, 0x51500000UL,
  /*    4044 */ 0x11A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x86000000UL,
  /*    4050 */ 0x0081C01FUL,
  /*    4054 */ 0x20000000UL,
  /*    4058 */ 0x00000000UL,
  0x01054064UL, 0x40000000UL,
  /*    4068 */ 0x00FF04C8UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000002UL,
  /*    4074 */ 0x00040092UL,
  0x010F4088UL, 0x003B03B7UL,
  /*    408C */ 0xE0000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x0D050200UL,
  /*    4098 */ 0x53422F1CUL,
  /*    409C */ 0x60686962UL,
  /*    40A0 */ 0x1F2E4053UL,
  /*    40A4 */ 0x08080A11UL,
  /*    40A8 */ 0x06070808UL,
  /*    40AC */ 0x03030405UL,
  /*    40B0 */ 0x01020202UL,
  /*    40B4 */ 0x01010101UL,
  /*    40B8 */ 0x00000000UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  0x0101411CUL, 0x8CB16000UL,
  0x01074138UL, 0xF00A20BCUL,
  /*    413C */ 0x005262F6UL,
  /*    4140 */ 0x40985B05UL,
  /*    4144 */ 0x94826E8EUL,
  /*    4148 */ 0x5248781CUL,
  /*    414C */ 0x00403B89UL,
  /*    4150 */ 0x800003C0UL,
  0x0101415CUL, 0x0000DDFFUL,
  0x01074168UL, 0x00FA53E8UL,
  /*    416C */ 0x40000000UL,
  /*    4170 */ 0x00000000UL,
  /*    4174 */ 0x00000000UL,
  /*    4178 */ 0x00000000UL,
  /*    417C */ 0x00000000UL,
  /*    4180 */ 0x00000000UL,
  0x010341D0UL, 0xAA000000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00120004UL,
  0x011341E0UL, 0x00000000UL,
  /*    41E4 */ 0x0A00ABFFUL,
  /*    41E8 */ 0x000FF15CUL,
  /*    41EC */ 0x000241D3UL,
  /*    41F0 */ 0x00B1ED95UL,
  /*    41F4 */ 0x0FD87B19UL,
  /*    41F8 */ 0x04B90812UL,
  /*    41FC */ 0x1F6D1BEAUL,
  /*    4200 */ 0x0AC2B7C9UL,
  /*    4204 */ 0x003B200BUL,
  /*    4208 */ 0x00000643UL,
  /*    420C */ 0x00F36259UL,
  /*    4210 */ 0x00967E72UL,
  /*    4214 */ 0x00193DA7UL,
  /*    4218 */ 0x3BF122D0UL,
  /*    421C */ 0xA0000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000038UL,
  /*    4228 */ 0x00000000UL,
  0x0101424CUL, 0x04030008UL,
  0x01014268UL, 0x00000001UL,
  0x01018038UL, 0x00102555UL,
  0x010280A0UL, 0x00037870UL,
  /*    80A4 */ 0x000000D0UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01CB4205UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x00FD3E05UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x5151200CUL,
  0x02010184UL, 0x00001000UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_0_37_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_1,
  0x00014010UL, 0x00004101UL,
  0x0004403CUL, 0x00000000UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x030007A0UL,
  0x00014050UL, 0x00000000UL,
  0x0001405CUL, 0x00000000UL,
  0x00014108UL, 0x00004CFFUL,
  0x0007C054UL, 0x00302151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x07060504UL,
  /*    C064 */ 0x00000008UL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x00000520UL,
  0x01074040UL, 0x70400000UL,
  /*    4044 */ 0x21A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x86000000UL,
  /*    4050 */ 0x0081C01FUL,
  /*    4054 */ 0x20000000UL,
  /*    4058 */ 0x00000000UL,
  0x01054064UL, 0x40000000UL,
  /*    4068 */ 0x00FF0264UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000008UL,
  /*    4074 */ 0x00080092UL,
  0x010F4088UL, 0x002A03D9UL,
  /*    408C */ 0xE0000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x11070201UL,
  /*    4098 */ 0x5A4B3721UL,
  /*    409C */ 0x54626967UL,
  /*    40A0 */ 0x05152940UL,
  /*    40A4 */ 0x00000000UL,
  /*    40A8 */ 0x04030201UL,
  /*    40AC */ 0x03030404UL,
  /*    40B0 */ 0x02030303UL,
  /*    40B4 */ 0x02020202UL,
  /*    40B8 */ 0x01010102UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  0x0101411CUL, 0x8B212000UL,
  0x01074138UL, 0xF00A20BCUL,
  /*    413C */ 0x0051C04AUL,
  /*    4140 */ 0x40985B05UL,
  /*    4144 */ 0x94826E8EUL,
  /*    4148 */ 0x52483C29UL,
  /*    414C */ 0x00403B89UL,
  /*    4150 */ 0x800003C0UL,
  0x0101415CUL, 0x0000DDFFUL,
  0x01074168UL, 0x00FA53E8UL,
  /*    416C */ 0x40000000UL,
  /*    4170 */ 0x00000000UL,
  /*    4174 */ 0x00000000UL,
  /*    4178 */ 0x00000000UL,
  /*    417C */ 0x00000000UL,
  /*    4180 */ 0x00000000UL,
  0x010341D0UL, 0xAAAA0000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00120004UL,
  0x011341E0UL, 0x00000000UL,
  /*    41E4 */ 0x0D328C32UL,
  /*    41E8 */ 0x003167EFUL,
  /*    41EC */ 0x007F258BUL,
  /*    41F0 */ 0x00372388UL,
  /*    41F4 */ 0x0DA8FD2DUL,
  /*    41F8 */ 0x038BFADCUL,
  /*    41FC */ 0x31532650UL,
  /*    4200 */ 0x0AC2B7C9UL,
  /*    4204 */ 0x003B200BUL,
  /*    4208 */ 0x00000643UL,
  /*    420C */ 0x00F36259UL,
  /*    4210 */ 0x00967E72UL,
  /*    4214 */ 0x00193DA7UL,
  /*    4218 */ 0x3BF122D0UL,
  /*    421C */ 0xA0000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000040UL,
  /*    4228 */ 0x00000000UL,
  0x0101424CUL, 0x04030008UL,
  0x01014268UL, 0x00000001UL,
  0x01018038UL, 0x00104911UL,
  0x010280A0UL, 0x00037870UL,
  /*    80A4 */ 0x0000C0D5UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01CB4205UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x008D2205UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
  0x02010184UL, 0x00000000UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_1,
  0x00014010UL, 0x00004101UL,
  0x0004403CUL, 0x00000000UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x030007A0UL,
  0x00014050UL, 0x00000000UL,
  0x0001405CUL, 0x00000000UL,
  0x00014108UL, 0x00004CFFUL,
  0x0007C054UL, 0x00302151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x07060504UL,
  /*    C064 */ 0x00000008UL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x00000520UL,
  0x01074040UL, 0x70400000UL,
  /*    4044 */ 0x21A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x86000000UL,
  /*    4050 */ 0x0081C01FUL,
  /*    4054 */ 0x20000000UL,
  /*    4058 */ 0x00000000UL,
  0x01054064UL, 0x40000000UL,
  /*    4068 */ 0x00FF0264UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000008UL,
  /*    4074 */ 0x00080092UL,
  0x010F4088UL, 0x002A03D7UL,
  /*    408C */ 0xE0000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x0D050200UL,
  /*    4098 */ 0x503F2D1AUL,
  /*    409C */ 0x63696860UL,
  /*    40A0 */ 0x25354657UL,
  /*    40A4 */ 0x08090D15UL,
  /*    40A8 */ 0x05050607UL,
  /*    40AC */ 0x03040404UL,
  /*    40B0 */ 0x02030303UL,
  /*    40B4 */ 0x02020202UL,
  /*    40B8 */ 0x01010101UL,
  /*    40BC */ 0x01010101UL,
  /*    40C0 */ 0x01010101UL,
  0x0101411CUL, 0x8B212000UL,
  0x01074138UL, 0xF00A20BCUL,
  /*    413C */ 0x0051C04AUL,
  /*    4140 */ 0x40985B05UL,
  /*    4144 */ 0x94826E8EUL,
  /*    4148 */ 0x52483C23UL,
  /*    414C */ 0x00403B89UL,
  /*    4150 */ 0x800003C0UL,
  0x0101415CUL, 0x0000DDFFUL,
  0x01074168UL, 0x00FA53E8UL,
  /*    416C */ 0x40000000UL,
  /*    4170 */ 0x00000000UL,
  /*    4174 */ 0x00000000UL,
  /*    4178 */ 0x00000000UL,
  /*    417C */ 0x00000000UL,
  /*    4180 */ 0x00000000UL,
  0x010341D0UL, 0xAAAA0000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00120004UL,
  0x011341E0UL, 0x00000000UL,
  /*    41E4 */ 0x0D328C32UL,
  /*    41E8 */ 0x003167EFUL,
  /*    41EC */ 0x007F258BUL,
  /*    41F0 */ 0x00372388UL,
  /*    41F4 */ 0x0DA8FD2DUL,
  /*    41F8 */ 0x038BFADCUL,
  /*    41FC */ 0x31532650UL,
  /*    4200 */ 0x0AC2B7C9UL,
  /*    4204 */ 0x003B200BUL,
  /*    4208 */ 0x00000643UL,
  /*    420C */ 0x00F36259UL,
  /*    4210 */ 0x00967E72UL,
  /*    4214 */ 0x00193DA7UL,
  /*    4218 */ 0x3BF122D0UL,
  /*    421C */ 0xA0000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000040UL,
  /*    4228 */ 0x00000000UL,
  0x0101424CUL, 0x04030008UL,
  0x01014268UL, 0x00000001UL,
  0x01018038UL, 0x00104911UL,
  0x010280A0UL, 0x00037870UL,
  /*    80A4 */ 0x0002C0FFUL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01CB4205UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x000D0A05UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
  0x02010184UL, 0x00000000UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_125kbps_38M4Hz_0_37_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_2,
  0x00014010UL, 0x00004100UL,
  0x0004403CUL, 0x00000010UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x03000720UL,
  0x00014050UL, 0x00000400UL,
  0x0001405CUL, 0x00000F0DUL,
  0x00014108UL, 0x0001C003UL,
  0x0007C054UL, 0x00301151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x0D0C0B08UL,
  /*    C064 */ 0x0000000DUL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x00000580UL,
  0x01074040UL, 0x70200000UL,
  /*    4044 */ 0x11A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x46000030UL,
  /*    4050 */ 0x0081C81FUL,
  /*    4054 */ 0x30002000UL,
  /*    4058 */ 0x00008000UL,
  0x01054064UL, 0x00000000UL,
  /*    4068 */ 0x00FF04C8UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000010UL,
  /*    4074 */ 0x000A0091UL,
  0x010F4088UL, 0x001203B7UL,
  /*    408C */ 0x60000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x0D050200UL,
  /*    4098 */ 0x53422F1CUL,
  /*    409C */ 0x60686962UL,
  /*    40A0 */ 0x1F2E4053UL,
  /*    40A4 */ 0x08080A11UL,
  /*    40A8 */ 0x06070808UL,
  /*    40AC */ 0x03030405UL,
  /*    40B0 */ 0x01020202UL,
  /*    40B4 */ 0x01010101UL,
  /*    40B8 */ 0x00000000UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  0x0101411CUL, 0x8C2C3000UL,
  0x01074138UL, 0xF00A20BCUL,
  /*    413C */ 0x0051C04AUL,
  /*    4140 */ 0x40000001UL,
  /*    4144 */ 0x123556B7UL,
  /*    4148 */ 0x50087800UL,
  /*    414C */ 0x00400809UL,
  /*    4150 */ 0x800003C0UL,
  0x0101415CUL, 0x00000000UL,
  0x01074168UL, 0x3BE8D44CUL,
  /*    416C */ 0x700D1904UL,
  /*    4170 */ 0x1A140E08UL,
  /*    4174 */ 0x322C2620UL,
  /*    4178 */ 0x32103E38UL,
  /*    417C */ 0x0A987654UL,
  /*    4180 */ 0xB440691DUL,
  0x010341D0UL, 0x00000000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00020000UL,
  0x011341E0UL, 0x00000000UL,
  /*    41E4 */ 0x2CDDCFE2UL,
  /*    41E8 */ 0x003AB67EUL,
  /*    41EC */ 0x00190960UL,
  /*    41F0 */ 0x00E22251UL,
  /*    41F4 */ 0x0E51B94CUL,
  /*    41F8 */ 0x04A98239UL,
  /*    41FC */ 0x299B22D8UL,
  /*    4200 */ 0x2CDDCFE2UL,
  /*    4204 */ 0x003AB67EUL,
  /*    4208 */ 0x00190960UL,
  /*    420C */ 0x00E22251UL,
  /*    4210 */ 0x0E51B94CUL,
  /*    4214 */ 0x04A98239UL,
  /*    4218 */ 0x299B22D8UL,
  /*    421C */ 0x00000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000044UL,
  /*    4228 */ 0x3C3C3C3CUL,
  0x0101424CUL, 0x04000008UL,
  0x01014268UL, 0x00000000UL,
  0x01018038UL, 0x00104911UL,
  0x010280A0UL, 0x00033870UL,
  /*    80A4 */ 0x000000D0UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01FD5A05UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x00FD3E05UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
  0x02010184UL, 0x00000000UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_500kbps_38M4Hz_0_37_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_3,
  0x00014010UL, 0x00004100UL,
  0x0004403CUL, 0x00000010UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x03001720UL,
  0x00014050UL, 0x00000401UL,
  0x0001405CUL, 0x00000F0DUL,
  0x00014108UL, 0x0001C003UL,
  0x0007C054UL, 0x00301151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x0D0C0B08UL,
  /*    C064 */ 0x0000000DUL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x00000580UL,
  0x01074040UL, 0x70200000UL,
  /*    4044 */ 0x11A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x46000030UL,
  /*    4050 */ 0x0081C81FUL,
  /*    4054 */ 0x30002000UL,
  /*    4058 */ 0x00008000UL,
  0x01054064UL, 0x00000000UL,
  /*    4068 */ 0x00FF04C8UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000010UL,
  /*    4074 */ 0x000A0091UL,
  0x010F4088UL, 0x001203B7UL,
  /*    408C */ 0x60000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x0D050200UL,
  /*    4098 */ 0x53422F1CUL,
  /*    409C */ 0x60686962UL,
  /*    40A0 */ 0x1F2E4053UL,
  /*    40A4 */ 0x08080A11UL,
  /*    40A8 */ 0x06070808UL,
  /*    40AC */ 0x03030405UL,
  /*    40B0 */ 0x01020202UL,
  /*    40B4 */ 0x01010101UL,
  /*    40B8 */ 0x00000000UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  0x0101411CUL, 0x8C2C3000UL,
  0x01074138UL, 0xF00A20BCUL,
  /*    413C */ 0x0051C04AUL,
  /*    4140 */ 0x40000001UL,
  /*    4144 */ 0x123556B7UL,
  /*    4148 */ 0x50087800UL,
  /*    414C */ 0x00400809UL,
  /*    4150 */ 0x800003C0UL,
  0x0101415CUL, 0x00000000UL,
  0x01074168UL, 0x3BE8D44CUL,
  /*    416C */ 0x700D1904UL,
  /*    4170 */ 0x1C16100AUL,
  /*    4174 */ 0x342E2822UL,
  /*    4178 */ 0x3210403AUL,
  /*    417C */ 0x0A987654UL,
  /*    4180 */ 0xA480691DUL,
  0x010341D0UL, 0x00000000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00020000UL,
  0x011341E0UL, 0x00000000UL,
  /*    41E4 */ 0x2CDDCFE2UL,
  /*    41E8 */ 0x003AB67EUL,
  /*    41EC */ 0x00190960UL,
  /*    41F0 */ 0x00E22251UL,
  /*    41F4 */ 0x0E51B94CUL,
  /*    41F8 */ 0x04A98239UL,
  /*    41FC */ 0x299B22D8UL,
  /*    4200 */ 0x2CDDCFE2UL,
  /*    4204 */ 0x003AB67EUL,
  /*    4208 */ 0x00190960UL,
  /*    420C */ 0x00E22251UL,
  /*    4210 */ 0x0E51B94CUL,
  /*    4214 */ 0x04A98239UL,
  /*    4218 */ 0x299B22D8UL,
  /*    421C */ 0x00000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000044UL,
  /*    4228 */ 0x3C3C3C3CUL,
  0x0101424CUL, 0x04000008UL,
  0x01014268UL, 0x00000000UL,
  0x01018038UL, 0x00104911UL,
  0x010280A0UL, 0x00033870UL,
  /*    80A4 */ 0x000000D0UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01FD5A05UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x00FD3E05UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
  0x02010184UL, 0x00000000UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ble_phy_simulscan_38M4Hz_0_37_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_4,
  0x00014010UL, 0x00004100UL,
  0x0004403CUL, 0x00000010UL,
  /*    4040 */ 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  /*    4048 */ 0x03000720UL,
  0x00014050UL, 0x00000000UL,
  0x0001405CUL, 0x00000F0DUL,
  0x00014108UL, 0x00004CFFUL,
  0x0007C054UL, 0x00301151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x0D0C0B08UL,
  /*    C064 */ 0x0000000DUL,
  /*    C068 */ 0x0002B6D1UL,
  /*    C06C */ 0x000004E0UL,
  0x01074040UL, 0x70200000UL,
  /*    4044 */ 0x11A00000UL,
  /*    4048 */ 0x00000010UL,
  /*    404C */ 0x46000000UL,
  /*    4050 */ 0x0081C01FUL,
  /*    4054 */ 0x20000000UL,
  /*    4058 */ 0x00008000UL,
  0x01054064UL, 0x00000000UL,
  /*    4068 */ 0x00FF04C8UL,
  /*    406C */ 0x00000840UL,
  /*    4070 */ 0x00000010UL,
  /*    4074 */ 0x00040091UL,
  0x010F4088UL, 0x001203B7UL,
  /*    408C */ 0x60000000UL,
  /*    4090 */ 0x00000000UL,
  /*    4094 */ 0x0D050200UL,
  /*    4098 */ 0x53422F1CUL,
  /*    409C */ 0x60686962UL,
  /*    40A0 */ 0x1F2E4053UL,
  /*    40A4 */ 0x08080A11UL,
  /*    40A8 */ 0x06070808UL,
  /*    40AC */ 0x03030405UL,
  /*    40B0 */ 0x01020202UL,
  /*    40B4 */ 0x01010101UL,
  /*    40B8 */ 0x00000000UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  0x0101411CUL, 0x8C2C3000UL,
  0x01074138UL, 0xF00B20BCUL,
  /*    413C */ 0x0051C04AUL,
  /*    4140 */ 0x40A05B05UL,
  /*    4144 */ 0x94826E8EUL,
  /*    4148 */ 0x52488221UL,
  /*    414C */ 0x00400809UL,
  /*    4150 */ 0x80000DC0UL,
  0x0101415CUL, 0x0000DDFFUL,
  0x01074168UL, 0x3BE8D44CUL,
  /*    416C */ 0x500D1904UL,
  /*    4170 */ 0x1A140E08UL,
  /*    4174 */ 0x322C2620UL,
  /*    4178 */ 0x32103E38UL,
  /*    417C */ 0x0A987654UL,
  /*    4180 */ 0xB440001DUL,
  0x010341D0UL, 0x00000000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00120004UL,
  0x011341E0UL, 0x80000000UL,
  /*    41E4 */ 0x2CDDCFE2UL,
  /*    41E8 */ 0x003AB67EUL,
  /*    41EC */ 0x00190960UL,
  /*    41F0 */ 0x00E22251UL,
  /*    41F4 */ 0x0E51B94CUL,
  /*    41F8 */ 0x04A98239UL,
  /*    41FC */ 0x299B22D8UL,
  /*    4200 */ 0x2CDDCFE2UL,
  /*    4204 */ 0x003AB67EUL,
  /*    4208 */ 0x00190960UL,
  /*    420C */ 0x00E22251UL,
  /*    4210 */ 0x0E51B94CUL,
  /*    4214 */ 0x04A98239UL,
  /*    4218 */ 0x299B22D8UL,
  /*    421C */ 0x00000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x00000044UL,
  /*    4228 */ 0x3C3C3C3CUL,
  0x0101424CUL, 0x04030008UL,
  0x01014268UL, 0x00000000UL,
  0x01018038UL, 0x00104911UL,
  0x010280A0UL, 0x00033870UL,
  /*    80A4 */ 0x000000D0UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01FD5A05UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x00FD3E05UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
  0x02010184UL, 0x00000000UL,
  0xFFFFFFFFUL,
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 37,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 183,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = 140,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 37,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 160,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_2Mbps_aox_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 34,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 35,
    .channelNumberEnd = 35,
    .maxPower = 182,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 36,
    .channelNumberEnd = 36,
    .maxPower = 157,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 37,
    .channelNumberEnd = 37,
    .maxPower = 125,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 83,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_2Mbps_aox_38M4Hz_0_34_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_0,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_125kbps_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_125kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 37,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_125kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 183,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_125kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = 140,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_500kbps_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_500kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 37,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_500kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 183,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_500kbps_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = 140,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ble_phy_simulscan_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_simulscan_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 37,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_simulscan_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 38,
    .channelNumberEnd = 38,
    .maxPower = 183,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
  {
    .phyConfigDeltaAdd = sl_rail_ble_phy_simulscan_38M4Hz_0_37_modemConfig,
    .baseFrequency = 2402000000,
    .channelSpacing = 2000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 39,
    .channelNumberEnd = 39,
    .maxPower = 140,
    .attr = &channelConfigEntryAttr_1,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_channels,
  .length = 3U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_2Mbps_viterbi_38M4Hz_channels,
  .length = 3U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_2Mbps_aox_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_2Mbps_aox_38M4Hz_channels,
  .length = 6U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_125kbps_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_125kbps_38M4Hz_channels,
  .length = 3U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_500kbps_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_500kbps_38M4Hz_channels,
  .length = 3U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ble_phy_simulscan_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ble_phy_1Mbps_viterbi_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ble_phy_simulscan_38M4Hz_channels,
  .length = 3U,
  .signature = 0UL,
};
