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
#include "sl_rail_ieee802154_config_38M4Hz.h"

static const uint8_t irCalConfig[] = {
  25, 63, 1, 6, 4, 16, 1, 0, 0, 1, 1, 6, 0, 16, 39, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0
};

static const int32_t timingConfig_0[] = {
  6125, 6125, 0
};

static const int32_t timingConfig_1[] = {
  6625, 6625, 0
};

static const uint8_t hfxoRetimingConfigEntries[] = {
  2, 0, 0, 0, 0x00, 0xf0, 0x49, 0x02, 6, 20, 0, 0, 0x00, 0xe0, 0x93, 0x04, 5, 56, 0, 0, 0xa0, 0x08, 0, 0, 0, 0, 0x58, 0x09, 1, 4, 7, 6, 0x10, 0x0a, 1, 4, 7, 7, 0xc8, 0x0a, 0, 4, 8, 7, 0x80, 0x0b, 0, 4, 8, 8, 0x38, 0x0c, 0, 4, 9, 8, 0x61, 0x08, 0, 0, 0, 0, 0x68, 0x08, 0, 0, 0, 0, 0xc7, 0x09, 1, 4, 4, 3, 0x2c, 0x0b, 1, 4, 4, 4, 0x92, 0x0c, 1, 4, 5, 4
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr = {
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
  0x00444444UL, // 68.26666666666667
  (uint32_t) NULL,
  (uint32_t) irCalConfig,
  (uint32_t) timingConfig_0,
  0x00000000UL,
  0UL,
  60000000UL,
  2000000UL,
  0x00F62004UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  2000034UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

static const uint32_t phyInfo_1[] = {
  13UL,
  0x00444444UL, // 68.26666666666667
  (uint32_t) NULL,
  (uint32_t) irCalConfig,
  (uint32_t) timingConfig_1,
  0x00000B00UL,
  0UL,
  60000000UL,
  2000000UL,
  0x00F82004UL,
  0x02104911UL,
  (uint32_t) NULL,
  (uint32_t) hfxoRetimingConfigEntries,
  (uint32_t) NULL,
  0UL,
  0UL,
  2000034UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
};

const uint32_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase[] = {
  0x0002400CUL, 0x00148001UL,
  /*    4010 */ 0x0000407FUL,
  0x00024020UL, 0x00000000UL,
  /*    4024 */ 0x00000000UL,
  0x00044030UL, 0x00000000UL,
  /*    4034 */ 0x00000000UL,
  /*    4038 */ 0x00000000UL,
  /*    403C */ 0x00000000UL,
  0x00014048UL, 0x030007A0UL,
  0x00014050UL, 0x00000000UL,
  0x0002405CUL, 0x00000000UL,
  /*    4060 */ 0x00000000UL,
  0x000140A8UL, 0x00000007UL,
  0x000440BCUL, 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  /*    40C4 */ 0x00000000UL,
  /*    40C8 */ 0x00000000UL,
  0x00044108UL, 0x00004000UL,
  /*    410C */ 0x00004CFFUL,
  /*    4110 */ 0x00004100UL,
  /*    4114 */ 0x00004DFFUL,
  0x0007C028UL, 0x03B380ECUL,
  /*    C02C */ 0x51407543UL,
  /*    C030 */ 0xF8000FA0UL,
  /*    C034 */ 0x00004000UL,
  /*    C038 */ 0x0007AAA8UL,
  /*    C03C */ 0x00000000UL,
  /*    C040 */ 0x00000000UL,
  0x0005C054UL, 0x00303151UL,
  /*    C058 */ 0xE60D000EUL,
  /*    C05C */ 0x0000002AUL,
  /*    C060 */ 0x0D0C0B08UL,
  /*    C064 */ 0x0000000DUL,
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
  0x0001C0CCUL, 0x00000001UL,
  0x0002C0D4UL, 0x000A0001UL,
  /*    C0D8 */ 0x00280001UL,
  0x01010008UL, 0x00000704UL,
  0x01010018UL, 0x00000000UL,
  0x01010020UL, 0x00008408UL,
  0x01034040UL, 0x00000000UL,
  /*    4044 */ 0x00000000UL,
  /*    4048 */ 0x00000010UL,
  0x01024058UL, 0x00000000UL,
  /*    405C */ 0x03000000UL,
  0x01064068UL, 0x00FF0264UL,
  /*    406C */ 0x00000841UL,
  /*    4070 */ 0x00000008UL,
  /*    4074 */ 0x000807B0UL,
  /*    4078 */ 0x000000A7UL,
  /*    407C */ 0x00000000UL,
  0x01024084UL, 0x744AC39BUL,
  /*    4088 */ 0x000F03F0UL,
  0x01104094UL, 0x30100101UL,
  /*    4098 */ 0x7F7F7050UL,
  /*    409C */ 0x00000000UL,
  /*    40A0 */ 0x00000000UL,
  /*    40A4 */ 0x00000000UL,
  /*    40A8 */ 0x00000000UL,
  /*    40AC */ 0x00000000UL,
  /*    40B0 */ 0x00000000UL,
  /*    40B4 */ 0x00000000UL,
  /*    40B8 */ 0x00000000UL,
  /*    40BC */ 0x00000000UL,
  /*    40C0 */ 0x00000000UL,
  /*    40C4 */ 0x00000000UL,
  /*    40C8 */ 0x00000000UL,
  /*    40CC */ 0x00000000UL,
  /*    40D0 */ 0x00000000UL,
  0x010140E0UL, 0x00000200UL,
  0x01024110UL, 0x00051E33UL,
  /*    4114 */ 0x00000000UL,
  0x0101411CUL, 0x8A8DB000UL,
  0x01054124UL, 0x078304FFUL,
  /*    4128 */ 0x3AC81388UL,
  /*    412C */ 0x0C6606FFUL,
  /*    4130 */ 0x078304FFUL,
  /*    4134 */ 0x03FF1388UL,
  0x0106413CUL, 0x0051B783UL,
  /*    4140 */ 0x00000000UL,
  /*    4144 */ 0x123556B7UL,
  /*    4148 */ 0x50000000UL,
  /*    414C */ 0x00003B80UL,
  /*    4150 */ 0x00000000UL,
  0x01014158UL, 0x00000000UL,
  0x01024164UL, 0x0000010CUL,
  /*    4168 */ 0x00FA53E8UL,
  0x010141A4UL, 0x00000000UL,
  0x010241B0UL, 0x00000000UL,
  /*    41B4 */ 0x00200000UL,
  0x010341BCUL, 0x00000000UL,
  /*    41C0 */ 0x003C0000UL,
  /*    41C4 */ 0x0006AAAAUL,
  0x010341D0UL, 0x00000000UL,
  /*    41D4 */ 0x000000D0UL,
  /*    41D8 */ 0x00020000UL,
  0x011641E0UL, 0x00000000UL,
  /*    41E4 */ 0x0BFFE7E6UL,
  /*    41E8 */ 0x000AA1CDUL,
  /*    41EC */ 0x006A06BDUL,
  /*    41F0 */ 0x004DB05EUL,
  /*    41F4 */ 0x0E42027DUL,
  /*    41F8 */ 0x0222B6A5UL,
  /*    41FC */ 0x34B225FFUL,
  /*    4200 */ 0x0BFFE7E6UL,
  /*    4204 */ 0x000AA1CDUL,
  /*    4208 */ 0x006A06BDUL,
  /*    420C */ 0x004DB05EUL,
  /*    4210 */ 0x0E42027DUL,
  /*    4214 */ 0x0222B6A5UL,
  /*    4218 */ 0x34B225FFUL,
  /*    421C */ 0x00000000UL,
  /*    4220 */ 0x00000000UL,
  /*    4224 */ 0x0000002CUL,
  /*    4228 */ 0x3675EE07UL,
  /*    422C */ 0x40001860UL,
  /*    4230 */ 0x00000000UL,
  /*    4234 */ 0x00000000UL,
  0x01034244UL, 0x00000014UL,
  /*    4248 */ 0x00000000UL,
  /*    424C */ 0x04000008UL,
  0x01014268UL, 0x00000000UL,
  0x01024298UL, 0x0200003FUL,
  /*    429C */ 0x0000FFFFUL,
  0x0104433CUL, 0x1F1F1F1FUL,
  /*    4340 */ 0x1B1F1F1FUL,
  /*    4344 */ 0x11131518UL,
  /*    4348 */ 0x0C0D0E10UL,
  0x01024350UL, 0x00000000UL,
  /*    4354 */ 0x00000000UL,
  0x01018010UL, 0x00000003UL,
  0x01028038UL, 0x00104911UL,
  /*    803C */ 0x00000001UL,
  0x0103809CUL, 0x000240EBUL,
  /*    80A0 */ 0x00037870UL,
  /*    80A4 */ 0x0000C0D5UL,
  0x110180A8UL, 0x000001F0UL,
  0x310180A8UL, 0x01CB4205UL,
  0x110180ACUL, 0x000001F0UL,
  0x310180ACUL, 0x008D2205UL,
  0x010280B0UL, 0x02000300UL,
  /*    80B4 */ 0x01000037UL,
  0x0201009CUL, 0x04000C00UL,
  0x020300D8UL, 0xAA400005UL,
  /*    00DC */ 0x00000188UL,
  /*    00E0 */ 0x000000C0UL,
  0x120100ECUL, 0x00000FE0UL,
  0x320100ECUL, 0x1151200CUL,
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
  0x02020184UL, 0x00000000UL,
  /*    0188 */ 0x00000050UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_0,
  0x00024040UL, 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  0x1001C020UL, 0x0007F800UL,
  0x3001C020UL, 0x01280214UL,
  0x0001C024UL, 0x00004300UL,
  0x0001C044UL, 0x0000022EUL,
  0x0002C068UL, 0x0002B6D1UL,
  /*    C06C */ 0x00000720UL,
  0x0001C0D0UL, 0x00000000UL,
  0x0103404CUL, 0x0413F920UL,
  /*    4050 */ 0x00620007UL,
  /*    4054 */ 0x00007038UL,
  0x01024060UL, 0x0F016800UL,
  /*    4064 */ 0x4024B840UL,
  0x01014080UL, 0x00006323UL,
  0x0102408CUL, 0x60008000UL,
  /*    4090 */ 0x00000ABEUL,
  0x01014120UL, 0x00000B59UL,
  0x01014138UL, 0xF00A2090UL,
  0x0101415CUL, 0x00001E00UL,
  0x010B416CUL, 0xC00C1400UL,
  /*    4170 */ 0x28211A14UL,
  /*    4174 */ 0x423B342EUL,
  /*    4178 */ 0x55435049UL,
  /*    417C */ 0x0CBA9876UL,
  /*    4180 */ 0x00007323UL,
  /*    4184 */ 0x00000101UL,
  /*    4188 */ 0x64282117UL,
  /*    418C */ 0x001A1714UL,
  /*    4190 */ 0x7DC80420UL,
  /*    4194 */ 0x093A20A1UL,
  0x010141A8UL, 0x00000029UL,
  0x0101423CUL, 0x00000000UL,
  0x01024280UL, 0x00000000UL,
  /*    4284 */ 0x00000081UL,
  0x010342A0UL, 0x0000FFFFUL,
  /*    42A4 */ 0x000003FFUL,
  /*    42A8 */ 0x0000FFFFUL,
  0x010142B4UL, 0x00000000UL,
  0x01034330UL, 0x01200040UL,
  /*    4334 */ 0x000000A0UL,
  /*    4338 */ 0x01005008UL,
  0x0101434CUL, 0x2F87C145UL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_1,
  0x00024040UL, 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  0x1001C020UL, 0x0007F800UL,
  0x3001C020UL, 0x012801FEUL,
  0x0001C024UL, 0x00001400UL,
  0x0001C044UL, 0x00000000UL,
  0x0002C068UL, 0x0002C688UL,
  /*    C06C */ 0x00000500UL,
  0x0001C0D0UL, 0x000000A1UL,
  0x0103404CUL, 0x0413FB20UL,
  /*    4050 */ 0x0042C007UL,
  /*    4054 */ 0x00000000UL,
  0x01024060UL, 0x28000000UL,
  /*    4064 */ 0x00028000UL,
  0x01014080UL, 0x08A0015AUL,
  0x0102408CUL, 0x60000000UL,
  /*    4090 */ 0x00000000UL,
  0x01014120UL, 0x00000000UL,
  0x01014138UL, 0xF00A20BCUL,
  0x0101415CUL, 0x00001003UL,
  0x010B416CUL, 0x40000000UL,
  /*    4170 */ 0x00000000UL,
  /*    4174 */ 0x00000000UL,
  /*    4178 */ 0x00000000UL,
  /*    417C */ 0x00000000UL,
  /*    4180 */ 0x00000000UL,
  /*    4184 */ 0x00000101UL,
  /*    4188 */ 0x00000000UL,
  /*    418C */ 0x00000000UL,
  /*    4190 */ 0x00000000UL,
  /*    4194 */ 0x00000000UL,
  0x010141A8UL, 0x00000000UL,
  0x0101423CUL, 0x00000112UL,
  0x01024280UL, 0x40090001UL,
  /*    4284 */ 0x00100801UL,
  0x010342A0UL, 0x03E80000UL,
  /*    42A4 */ 0x00000200UL,
  /*    42A8 */ 0x00100000UL,
  0x010142B4UL, 0x00C00000UL,
  0x01034330UL, 0x02400041UL,
  /*    4334 */ 0x04AC0140UL,
  /*    4338 */ 0x0100B050UL,
  0x0101434CUL, 0x2D0F285DUL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_1,
  0x00024040UL, 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  0x1001C020UL, 0x0007F800UL,
  0x3001C020UL, 0x012801FEUL,
  0x0001C024UL, 0x00001400UL,
  0x0001C044UL, 0x00000000UL,
  0x0002C068UL, 0x0002C688UL,
  /*    C06C */ 0x00000500UL,
  0x0001C0D0UL, 0x000000A1UL,
  0x0103404CUL, 0x0413FB20UL,
  /*    4050 */ 0x0042C007UL,
  /*    4054 */ 0x00000000UL,
  0x01024060UL, 0x28000000UL,
  /*    4064 */ 0x00028000UL,
  0x01014080UL, 0x08A0015AUL,
  0x0102408CUL, 0x60000000UL,
  /*    4090 */ 0x00000000UL,
  0x01014120UL, 0x00000000UL,
  0x01014138UL, 0xF00A20BCUL,
  0x0101415CUL, 0x00001003UL,
  0x010B416CUL, 0x40000000UL,
  /*    4170 */ 0x00000000UL,
  /*    4174 */ 0x00000000UL,
  /*    4178 */ 0x00000000UL,
  /*    417C */ 0x00000000UL,
  /*    4180 */ 0x00000000UL,
  /*    4184 */ 0x00000101UL,
  /*    4188 */ 0x00000000UL,
  /*    418C */ 0x00000000UL,
  /*    4190 */ 0x00000000UL,
  /*    4194 */ 0x00000000UL,
  0x010141A8UL, 0x00000000UL,
  0x0101423CUL, 0x00000112UL,
  0x01024280UL, 0x40090001UL,
  /*    4284 */ 0x00100801UL,
  0x010342A0UL, 0x03E80000UL,
  /*    42A4 */ 0x00000200UL,
  /*    42A8 */ 0x00100000UL,
  0x010142B4UL, 0x00C00000UL,
  0x01034330UL, 0x02400041UL,
  /*    4334 */ 0x05B40140UL,
  /*    4338 */ 0x0100B050UL,
  0x0101434CUL, 0x2D0F285DUL,
  0xFFFFFFFFUL,
};

const uint32_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_modemConfig[] = {
  0x03014FFCUL, (uint32_t) &phyInfo_0,
  0x00024040UL, 0x00000000UL,
  /*    4044 */ 0x00004000UL,
  0x1001C020UL, 0x0007F800UL,
  0x3001C020UL, 0x01280214UL,
  0x0001C024UL, 0x00004300UL,
  0x0001C044UL, 0x0000022EUL,
  0x0002C068UL, 0x0002B6D1UL,
  /*    C06C */ 0x00000720UL,
  0x0001C0D0UL, 0x00000000UL,
  0x0103404CUL, 0x0413F920UL,
  /*    4050 */ 0x00620007UL,
  /*    4054 */ 0x00007038UL,
  0x01024060UL, 0x0F016800UL,
  /*    4064 */ 0x4024B840UL,
  0x01014080UL, 0x00006323UL,
  0x0102408CUL, 0x60008000UL,
  /*    4090 */ 0x00000ABEUL,
  0x01014120UL, 0x00000B59UL,
  0x01014138UL, 0xF00A2090UL,
  0x0101415CUL, 0x00001E00UL,
  0x010B416CUL, 0xC00C1400UL,
  /*    4170 */ 0x2E27201AUL,
  /*    4174 */ 0x48413A34UL,
  /*    4178 */ 0x6654564FUL,
  /*    417C */ 0x0DCBA987UL,
  /*    4180 */ 0x00007323UL,
  /*    4184 */ 0x00000101UL,
  /*    4188 */ 0x6A2E2717UL,
  /*    418C */ 0x001A1714UL,
  /*    4190 */ 0x7DC80420UL,
  /*    4194 */ 0x093A20A1UL,
  0x010141A8UL, 0x00000029UL,
  0x0101423CUL, 0x00000000UL,
  0x01024280UL, 0x00000000UL,
  /*    4284 */ 0x00000081UL,
  0x010342A0UL, 0x0000FFFFUL,
  /*    42A4 */ 0x000003FFUL,
  /*    42A8 */ 0x0000FFFFUL,
  0x010142B4UL, 0x00000000UL,
  0x01034330UL, 0x01200040UL,
  /*    4334 */ 0x000000A0UL,
  /*    4338 */ 0x01005008UL,
  0x0101434CUL, 0x2F87C145UL,
  0xFFFFFFFFUL,
};

const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfig,
    .baseFrequency = 2405000000,
    .channelSpacing = 5000000,
    .physicalChannelOffset = 11,
    .channelNumberStart = 11,
    .channelNumberEnd = 26,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_modemConfig,
    .baseFrequency = 2405000000,
    .channelSpacing = 5000000,
    .physicalChannelOffset = 11,
    .channelNumberStart = 11,
    .channelNumberEnd = 26,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_modemConfig,
    .baseFrequency = 2405000000,
    .channelSpacing = 5000000,
    .physicalChannelOffset = 11,
    .channelNumberStart = 11,
    .channelNumberEnd = 26,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channels[] = {
  {
    .phyConfigDeltaAdd = sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_modemConfig,
    .baseFrequency = 2405000000,
    .channelSpacing = 5000000,
    .physicalChannelOffset = 11,
    .channelNumberStart = 11,
    .channelNumberEnd = 26,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0,
#endif
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
    .stackInfo = NULL,
#endif
  },
};

const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channelConfig = {
  .phyConfigBase = sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channels,
  .length = 1U,
  .signature = 0UL,
};
