/***************************************************************************//**
 * @file
 * @brief File System - Core Unicode String And Character Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <fs/include/fs_core.h>

#include  <common/include/lib_ascii.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  WC_DIFF_ALT                        DEF_INT_16S_MAX_VAL
#define  WC_DIFF_35332                     (DEF_INT_16S_MAX_VAL - 1)

#define  MB_MAX_SCALAR_VAL_1BYTE                  0x0000007Fu
#define  MB_MAX_SCALAR_VAL_2BYTE                  0x000007FFu
#define  MB_MAX_SCALAR_VAL_3BYTE                  0x0000FFFFu
#define  MB_MAX_SCALAR_VAL_4BYTE                  0x001FFFFFu

#define  WC_CHAR_NO_BREAK_SPACE                       0x00A0u
#define  WC_CHAR_OGHAM_SPACE_MARK                     0x1680u
#define  WC_CHAR_MONGOLIAN_VOWEL_SEPARATOR            0x180Eu
#define  WC_CHAR_EN_QUAD                              0x2000u
#define  WC_CHAR_HAIR_SPACE                           0x200Au
#define  WC_CHAR_LINE_SEPARATOR                       0x2028u
#define  WC_CHAR_PARAGRAPH_SEPARATOR                  0x2029u
#define  WC_CHAR_NARROW_NO_BREAK_SPACE                0x202Fu
#define  WC_CHAR_MEDIUM_MATHEMATICAL_SPACE            0x205Fu
#define  WC_CHAR_IDEOGRAPHIC_SPACE                    0x3000u

#define  WC_CHAR_FULLWIDTH_DIG_ZERO                   0xFF10u
#define  WC_CHAR_FULLWIDTH_DIG_SEVEN                  0xFF17u
#define  WC_CHAR_FULLWIDTH_DIG_NINE                   0xFF19u
#define  WC_CHAR_FULLWIDTH_LATIN_UPPER_A              0xFF21u
#define  WC_CHAR_FULLWIDTH_LATIN_UPPER_F              0xFF26u
#define  WC_CHAR_FULLWIDTH_LATIN_LOWER_A              0xFF41u
#define  WC_CHAR_FULLWIDTH_LATIN_LOWER_F              0xFF46u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct wc_srch_tbl_entry {
  CPU_WCHAR  Char;
  CPU_INT16S Diff;
} WC_SRCH_TBL_ENTRY;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const WC_SRCH_TBL_ENTRY WC_CharToCasefoldTbl[] = {
  { 0x0000u, 0 }, { 0x00B5u, 775 }, { 0x00B6u, 0 }, { 0x017Fu, -268 }, { 0x0180u, 0 }, { 0x0345u, 116 }, { 0x0346u, 0 }, { 0x03C2u, 1 },
  { 0x03C3u, 0 }, { 0x03D1u, -25 }, { 0x03D2u, 0 }, { 0x03D5u, -15 }, { 0x03D6u, -22 }, { 0x03D7u, 0 }, { 0x03F0u, -54 }, { 0x03F1u, -48 },
  { 0x03F2u, 0 }, { 0x03F5u, -64 }, { 0x03F6u, 0 }, { 0x1E9Bu, -58 }, { 0x1E9Cu, 0 }, { 0x1FBFu, 0 }, { 0xFFFFu, 0 }
};

static const WC_SRCH_TBL_ENTRY WC_CharToLowerTbl[] = {
  { 0x0000u, 0 }, { 0x0041u, 32 }, { 0x005Bu, 0 }, { 0x00C0u, 32 }, { 0x00D7u, 0 }, { 0x00D8u, 32 }, { 0x00DFu, 0 }, { 0x0100u, WC_DIFF_ALT },
  { 0x012Fu, 0 }, { 0x0130u, -199 }, { 0x0131u, 0 }, { 0x0132u, WC_DIFF_ALT }, { 0x0137u, 0 }, { 0x0139u, WC_DIFF_ALT }, { 0x0148u, 0 }, { 0x014Au, WC_DIFF_ALT },
  { 0x0177u, 0 }, { 0x0178u, -121 }, { 0x0179u, WC_DIFF_ALT }, { 0x017Eu, 0 }, { 0x0181u, 210 }, { 0x0182u, WC_DIFF_ALT }, { 0x0185u, 0 }, { 0x0186u, 206 },
  { 0x0187u, 1 }, { 0x0188u, 0 }, { 0x0189u, 205 }, { 0x018Bu, 1 }, { 0x018Cu, 0 }, { 0x018Eu, 79 }, { 0x018Fu, 202 }, { 0x0190u, 203 },
  { 0x0191u, 1 }, { 0x0192u, 0 }, { 0x0193u, 205 }, { 0x0194u, 207 }, { 0x0195u, 0 }, { 0x0196u, 211 }, { 0x0197u, 209 }, { 0x0198u, 1 },
  { 0x0199u, 0 }, { 0x019Cu, 211 }, { 0x019Du, 213 }, { 0x019Eu, 0 }, { 0x019Fu, 214 }, { 0x01A0u, WC_DIFF_ALT }, { 0x01A5u, 0 }, { 0x01A6u, 218 },
  { 0x01A7u, 1 }, { 0x01A8u, 0 }, { 0x01A9u, 218 }, { 0x01AAu, 0 }, { 0x01ACu, 1 }, { 0x01ADu, 0 }, { 0x01AEu, 218 }, { 0x01AFu, 1 },
  { 0x01B0u, 0 }, { 0x01B1u, 217 }, { 0x01B3u, WC_DIFF_ALT }, { 0x01B6u, 0 }, { 0x01B7u, 219 }, { 0x01B8u, 1 }, { 0x01B9u, 0 }, { 0x01BCu, 1 },
  { 0x01BDu, 0 }, { 0x01C4u, 2 }, { 0x01C5u, 1 }, { 0x01C6u, 0 }, { 0x01C7u, 2 }, { 0x01C8u, 1 }, { 0x01C9u, 0 }, { 0x01CAu, 2 },
  { 0x01CBu, WC_DIFF_ALT }, { 0x01DCu, 0 }, { 0x01DEu, WC_DIFF_ALT }, { 0x01EFu, 0 }, { 0x01F1u, 2 }, { 0x01F2u, WC_DIFF_ALT }, { 0x01F5u, 0 }, { 0x01F6u, -97 },
  { 0x01F7u, -56 }, { 0x01F8u, WC_DIFF_ALT }, { 0x021Fu, 0 }, { 0x0220u, -130 }, { 0x0221u, 0 }, { 0x0222u, WC_DIFF_ALT }, { 0x0233u, 0 }, { 0x023Au, 10795 },
  { 0x023Bu, 1 }, { 0x023Cu, 0 }, { 0x023Du, -163 }, { 0x023Eu, 10792 }, { 0x023Fu, 0 }, { 0x0241u, 1 }, { 0x0242u, 0 }, { 0x0243u, -195 },
  { 0x0244u, 69 }, { 0x0245u, 71 }, { 0x0246u, WC_DIFF_ALT }, { 0x024Fu, 0 }, { 0x0370u, WC_DIFF_ALT }, { 0x0373u, 0 }, { 0x0376u, 1 }, { 0x0377u, 0 },
  { 0x0386u, 38 }, { 0x0387u, 0 }, { 0x0388u, 37 }, { 0x038Bu, 0 }, { 0x038Cu, 64 }, { 0x038Du, 0 }, { 0x038Eu, 63 }, { 0x0390u, 0 },
  { 0x0391u, 32 }, { 0x03A2u, 0 }, { 0x03A3u, 32 }, { 0x03ACu, 0 }, { 0x03CFu, 8 }, { 0x03D0u, 0 }, { 0x03D8u, WC_DIFF_ALT }, { 0x03EFu, 0 },
  { 0x03F4u, -60 }, { 0x03F5u, 0 }, { 0x03F7u, 1 }, { 0x03F8u, 0 }, { 0x03F9u, -7 }, { 0x03FAu, 1 }, { 0x03FBu, 0 }, { 0x03FDu, -130 },
  { 0x0400u, 80 }, { 0x0410u, 32 }, { 0x0430u, 0 }, { 0x0460u, WC_DIFF_ALT }, { 0x0481u, 0 }, { 0x048Au, WC_DIFF_ALT }, { 0x04BFu, 0 }, { 0x04C0u, 15 },
  { 0x04C1u, WC_DIFF_ALT }, { 0x04CEu, 0 }, { 0x04D0u, WC_DIFF_ALT }, { 0x0523u, 0 }, { 0x0531u, 48 }, { 0x0557u, 0 }, { 0x10A0u, 7264 }, { 0x10C6u, 0 },
  { 0x1E00u, WC_DIFF_ALT }, { 0x1E95u, 0 }, { 0x1E9Eu, -7615 }, { 0x1E9Fu, 0 }, { 0x1EA0u, WC_DIFF_ALT }, { 0x1EFFu, 0 }, { 0x1F08u, -8 }, { 0x1F10u, 0 },
  { 0x1F18u, -8 }, { 0x1F1Eu, 0 }, { 0x1F28u, -8 }, { 0x1F30u, 0 }, { 0x1F38u, -8 }, { 0x1F40u, 0 }, { 0x1F48u, -8 }, { 0x1F4Eu, 0 },
  { 0x1F59u, -8 }, { 0x1F5Au, 0 }, { 0x1F5Bu, -8 }, { 0x1F5Cu, 0 }, { 0x1F5Du, -8 }, { 0x1F5Eu, 0 }, { 0x1F5Fu, -8 }, { 0x1F60u, 0 },
  { 0x1F68u, -8 }, { 0x1F70u, 0 }, { 0x1F88u, -8 }, { 0x1F90u, 0 }, { 0x1F98u, -8 }, { 0x1FA0u, 0 }, { 0x1FA8u, -8 }, { 0x1FB0u, 0 },
  { 0x1FB8u, -8 }, { 0x1FBAu, -74 }, { 0x1FBCu, -9 }, { 0x1FBDu, 0 }, { 0x1FC8u, -86 }, { 0x1FCCu, -9 }, { 0x1FCDu, 0 }, { 0x1FD8u, -8 },
  { 0x1FDAu, -100 }, { 0x1FDCu, 0 }, { 0x1FE8u, -8 }, { 0x1FEAu, -112 }, { 0x1FECu, -7 }, { 0x1FEDu, 0 }, { 0x1FF8u, -128 }, { 0x1FFAu, -126 },
  { 0x1FFCu, -9 }, { 0x1FFDu, 0 }, { 0x2126u, -7517 }, { 0x2127u, 0 }, { 0x212Au, -8383 }, { 0x212Bu, -8262 }, { 0x212Cu, 0 }, { 0x2132u, 28 },
  { 0x2133u, 0 }, { 0x2160u, 16 }, { 0x2170u, 0 }, { 0x2183u, 1 }, { 0x2184u, 0 }, { 0x24B6u, 26 }, { 0x24D0u, 0 }, { 0x2C00u, 48 },
  { 0x2C2Fu, 0 }, { 0x2C60u, 1 }, { 0x2C61u, 0 }, { 0x2C62u, -10743 }, { 0x2C63u, -3814 }, { 0x2C64u, -10727 }, { 0x2C65u, 0 }, { 0x2C67u, WC_DIFF_ALT },
  { 0x2C6Cu, 0 }, { 0x2C6Du, -10780 }, { 0x2C6Eu, -10749 }, { 0x2C6Fu, -10783 }, { 0x2C70u, 0 }, { 0x2C72u, 1 }, { 0x2C73u, 0 }, { 0x2C75u, 1 },
  { 0x2C76u, 0 }, { 0x2C80u, WC_DIFF_ALT }, { 0x2CE3u, 0 }, { 0xA640u, WC_DIFF_ALT }, { 0xA65Fu, 0 }, { 0xA662u, WC_DIFF_ALT }, { 0xA66Du, 0 }, { 0xA680u, WC_DIFF_ALT },
  { 0xA697u, 0 }, { 0xA722u, WC_DIFF_ALT }, { 0xA72Fu, 0 }, { 0xA732u, WC_DIFF_ALT }, { 0xA76Fu, 0 }, { 0xA779u, WC_DIFF_ALT }, { 0xA77Cu, 0 }, { 0xA77Du, WC_DIFF_35332 },
  { 0xA77Eu, WC_DIFF_ALT }, { 0xA787u, 0 }, { 0xA78Bu, 1 }, { 0xA78Cu, 0 }, { 0xFF21u, 32 }, { 0xFF3Bu, 0 }, { 0xFFFFu, 0 }
};

static const WC_SRCH_TBL_ENTRY WC_CharToUpperTbl[] = {
  { 0x0000u, 0 }, { 0x0061u, -32 }, { 0x007Bu, 0 }, { 0x00B5u, 743 }, { 0x00B6u, 0 }, { 0x00E0u, -32 }, { 0x00F7u, 0 }, { 0x00F8u, -32 },
  { 0x00FFu, 121 }, { 0x0100u, 0 }, { 0x0101u, WC_DIFF_ALT }, { 0x0130u, 0 }, { 0x0131u, -232 }, { 0x0132u, 0 }, { 0x0133u, WC_DIFF_ALT }, { 0x0138u, 0 },
  { 0x013Au, WC_DIFF_ALT }, { 0x0149u, 0 }, { 0x014Bu, WC_DIFF_ALT }, { 0x0178u, 0 }, { 0x017Au, WC_DIFF_ALT }, { 0x017Fu, -300 }, { 0x0180u, 195 }, { 0x0181u, 0 },
  { 0x0183u, WC_DIFF_ALT }, { 0x0186u, 0 }, { 0x0188u, -1 }, { 0x0189u, 0 }, { 0x018Cu, -1 }, { 0x018Du, 0 }, { 0x0192u, -1 }, { 0x0193u, 0 },
  { 0x0195u, 97 }, { 0x0196u, 0 }, { 0x0199u, -1 }, { 0x019Au, 163 }, { 0x019Bu, 0 }, { 0x019Eu, 130 }, { 0x019Fu, 0 }, { 0x01A1u, WC_DIFF_ALT },
  { 0x01A6u, 0 }, { 0x01A8u, -1 }, { 0x01A9u, 0 }, { 0x01ADu, -1 }, { 0x01AEu, 0 }, { 0x01B0u, -1 }, { 0x01B1u, 0 }, { 0x01B4u, WC_DIFF_ALT },
  { 0x01B7u, 0 }, { 0x01B9u, -1 }, { 0x01BAu, 0 }, { 0x01BDu, -1 }, { 0x01BEu, 0 }, { 0x01BFu, 56 }, { 0x01C0u, 0 }, { 0x01C5u, -1 },
  { 0x01C6u, -2 }, { 0x01C7u, 0 }, { 0x01C8u, -1 }, { 0x01C9u, -2 }, { 0x01CAu, 0 }, { 0x01CBu, -1 }, { 0x01CCu, -2 }, { 0x01CDu, 0 },
  { 0x01CEu, WC_DIFF_ALT }, { 0x01DDu, -79 }, { 0x01DEu, 0 }, { 0x01DFu, WC_DIFF_ALT }, { 0x01F0u, 0 }, { 0x01F2u, -1 }, { 0x01F3u, -2 }, { 0x01F4u, 0 },
  { 0x01F5u, -1 }, { 0x01F6u, 0 }, { 0x01F9u, WC_DIFF_ALT }, { 0x0220u, 0 }, { 0x0223u, WC_DIFF_ALT }, { 0x0234u, 0 }, { 0x023Cu, -1 }, { 0x023Du, 0 },
  { 0x0242u, -1 }, { 0x0243u, 0 }, { 0x0247u, WC_DIFF_ALT }, { 0x0250u, 10783 }, { 0x0251u, 10780 }, { 0x0252u, 0 }, { 0x0253u, -210 }, { 0x0254u, -206 },
  { 0x0255u, 0 }, { 0x0256u, -205 }, { 0x0258u, 0 }, { 0x0259u, -202 }, { 0x025Au, 0 }, { 0x025Bu, -203 }, { 0x025Cu, 0 }, { 0x0260u, -205 },
  { 0x0261u, 0 }, { 0x0263u, -207 }, { 0x0264u, 0 }, { 0x0268u, -209 }, { 0x0269u, -211 }, { 0x026Au, 0 }, { 0x026Bu, 10743 }, { 0x026Cu, 0 },
  { 0x026Fu, -211 }, { 0x0270u, 0 }, { 0x0271u, 10749 }, { 0x0272u, -213 }, { 0x0273u, 0 }, { 0x0275u, -214 }, { 0x0276u, 0 }, { 0x027Du, 10727 },
  { 0x027Eu, 0 }, { 0x0280u, -218 }, { 0x0281u, 0 }, { 0x0283u, -218 }, { 0x0284u, 0 }, { 0x0288u, -218 }, { 0x0289u, -69 }, { 0x028Au, -217 },
  { 0x028Cu, -71 }, { 0x028Du, 0 }, { 0x0292u, -219 }, { 0x0293u, 0 }, { 0x0345u, 84 }, { 0x0346u, 0 }, { 0x0371u, WC_DIFF_ALT }, { 0x0374u, 0 },
  { 0x0377u, -1 }, { 0x0378u, 0 }, { 0x037Bu, 130 }, { 0x037Eu, 0 }, { 0x03ACu, -38 }, { 0x03ADu, -37 }, { 0x03B0u, 0 }, { 0x03B1u, -32 },
  { 0x03C2u, -31 }, { 0x03C3u, -32 }, { 0x03CCu, -64 }, { 0x03CDu, -63 }, { 0x03CFu, 0 }, { 0x03D0u, -62 }, { 0x03D1u, -57 }, { 0x03D2u, 0 },
  { 0x03D5u, -47 }, { 0x03D6u, -54 }, { 0x03D7u, -8 }, { 0x03D8u, 0 }, { 0x03D9u, WC_DIFF_ALT }, { 0x03F0u, -86 }, { 0x03F1u, -80 }, { 0x03F2u, 7 },
  { 0x03F3u, 0 }, { 0x03F5u, -96 }, { 0x03F6u, 0 }, { 0x03F8u, -1 }, { 0x03F9u, 0 }, { 0x03FBu, -1 }, { 0x03FCu, 0 }, { 0x0430u, -32 },
  { 0x0450u, -80 }, { 0x0460u, 0 }, { 0x0461u, WC_DIFF_ALT }, { 0x0482u, 0 }, { 0x048Bu, WC_DIFF_ALT }, { 0x04C0u, 0 }, { 0x04C2u, WC_DIFF_ALT }, { 0x04CFu, -15 },
  { 0x04D0u, 0 }, { 0x04D1u, WC_DIFF_ALT }, { 0x0524u, 0 }, { 0x0561u, -48 }, { 0x0587u, 0 }, { 0x1D79u, WC_DIFF_35332 }, { 0x1D7Au, 0 }, { 0x1D7Du, 3814 },
  { 0x1D7Eu, 0 }, { 0x1E01u, WC_DIFF_ALT }, { 0x1E96u, 0 }, { 0x1E9Bu, -59 }, { 0x1E9Cu, 0 }, { 0x1EA1u, WC_DIFF_ALT }, { 0x1F00u, 8 }, { 0x1F08u, 0 },
  { 0x1F10u, 8 }, { 0x1F16u, 0 }, { 0x1F20u, 8 }, { 0x1F28u, 0 }, { 0x1F30u, 8 }, { 0x1F38u, 0 }, { 0x1F40u, 8 }, { 0x1F46u, 0 },
  { 0x1F51u, 8 }, { 0x1F52u, 0 }, { 0x1F53u, 8 }, { 0x1F54u, 0 }, { 0x1F55u, 8 }, { 0x1F56u, 0 }, { 0x1F57u, 8 }, { 0x1F58u, 0 },
  { 0x1F60u, 8 }, { 0x1F68u, 0 }, { 0x1F70u, 74 }, { 0x1F72u, 86 }, { 0x1F76u, 100 }, { 0x1F78u, 128 }, { 0x1F7Au, 112 }, { 0x1F7Cu, 126 },
  { 0x1F7Eu, 0 }, { 0x1F80u, 8 }, { 0x1F88u, 0 }, { 0x1F90u, 8 }, { 0x1F98u, 0 }, { 0x1FA0u, 8 }, { 0x1FA8u, 0 }, { 0x1FB0u, 8 },
  { 0x1FB2u, 0 }, { 0x1FB3u, 9 }, { 0x1FB4u, 0 }, { 0x1FBEu, -7205 }, { 0x1FBFu, 0 }, { 0x1FC3u, 9 }, { 0x1FC4u, 0 }, { 0x1FD0u, 8 },
  { 0x1FD2u, 0 }, { 0x1FE0u, 8 }, { 0x1FE2u, 0 }, { 0x1FE5u, 7 }, { 0x1FE6u, 0 }, { 0x1FF3u, 9 }, { 0x1FF4u, 0 }, { 0x214Eu, -28 },
  { 0x214Fu, 0 }, { 0x2170u, -16 }, { 0x2180u, 0 }, { 0x2184u, -1 }, { 0x2185u, 0 }, { 0x24D0u, -26 }, { 0x24EAu, 0 }, { 0x2C30u, -48 },
  { 0x2C5Fu, 0 }, { 0x2C61u, -1 }, { 0x2C62u, 0 }, { 0x2C65u, -10795 }, { 0x2C66u, -10792 }, { 0x2C67u, 0 }, { 0x2C68u, WC_DIFF_ALT }, { 0x2C6Du, 0 },
  { 0x2C73u, -1 }, { 0x2C74u, 0 }, { 0x2C76u, -1 }, { 0x2C77u, 0 }, { 0x2C81u, WC_DIFF_ALT }, { 0x2CE4u, 0 }, { 0x2D00u, -7264 }, { 0x2D26u, 0 },
  { 0xA641u, WC_DIFF_ALT }, { 0xA660u, 0 }, { 0xA663u, WC_DIFF_ALT }, { 0xA66Eu, 0 }, { 0xA681u, WC_DIFF_ALT }, { 0xA698u, 0 }, { 0xA723u, WC_DIFF_ALT }, { 0xA730u, 0 },
  { 0xA733u, WC_DIFF_ALT }, { 0xA770u, 0 }, { 0xA77Au, WC_DIFF_ALT }, { 0xA77Du, 0 }, { 0xA77Fu, WC_DIFF_ALT }, { 0xA788u, 0 }, { 0xA78Cu, -1 }, { 0xA78Du, 0 },
  { 0xFF41u, -32 }, { 0xFF5Bu, 0 }, { 0xFFFFu, 0 }
};

static const CPU_WCHAR WC_CharIsAlphaTbl[] = {
  0x0000u, 0x0041u, 0x005Bu, 0x0061u, 0x007Bu, 0x00AAu, 0x00ABu, 0x00B5u, 0x00B6u, 0x00BAu, 0x00BBu, 0x00C0u, 0x00D7u, 0x00D8u, 0x00F7u, 0x00F8u,
  0x02C2u, 0x02C6u, 0x02D2u, 0x02E0u, 0x02E5u, 0x02ECu, 0x02EDu, 0x02EEu, 0x02EFu, 0x0345u, 0x0346u, 0x0370u, 0x0375u, 0x0376u, 0x0378u, 0x037Au,
  0x037Eu, 0x0386u, 0x0387u, 0x0388u, 0x038Bu, 0x038Cu, 0x038Du, 0x038Eu, 0x03A2u, 0x03A3u, 0x03F6u, 0x03F7u, 0x0482u, 0x048Au, 0x0524u, 0x0531u,
  0x0557u, 0x0559u, 0x055Au, 0x0561u, 0x0588u, 0x05B0u, 0x05BEu, 0x05BFu, 0x05C0u, 0x05C1u, 0x05C3u, 0x05C4u, 0x05C6u, 0x05C7u, 0x05C8u, 0x05D0u,
  0x05EBu, 0x05F0u, 0x05F3u, 0x0610u, 0x061Bu, 0x0621u, 0x0658u, 0x0659u, 0x065Fu, 0x066Eu, 0x06D4u, 0x06D5u, 0x06DDu, 0x06E1u, 0x06E9u, 0x06EDu,
  0x06F0u, 0x06FAu, 0x06FDu, 0x06FFu, 0x0700u, 0x0710u, 0x0740u, 0x074Du, 0x07B2u, 0x07CAu, 0x07EBu, 0x07F4u, 0x07F6u, 0x07FAu, 0x07FBu, 0x0901u,
  0x093Au, 0x093Du, 0x094Du, 0x0950u, 0x0951u, 0x0958u, 0x0964u, 0x0971u, 0x0973u, 0x097Bu, 0x0980u, 0x0981u, 0x0984u, 0x0985u, 0x098Du, 0x098Fu,
  0x0991u, 0x0993u, 0x09A9u, 0x09AAu, 0x09B1u, 0x09B2u, 0x09B3u, 0x09B6u, 0x09BAu, 0x09BDu, 0x09C5u, 0x09C7u, 0x09C9u, 0x09CBu, 0x09CDu, 0x09CEu,
  0x09CFu, 0x09D7u, 0x09D8u, 0x09DCu, 0x09DEu, 0x09DFu, 0x09E4u, 0x09F0u, 0x09F2u, 0x0A01u, 0x0A04u, 0x0A05u, 0x0A0Bu, 0x0A0Fu, 0x0A11u, 0x0A13u,
  0x0A29u, 0x0A2Au, 0x0A31u, 0x0A32u, 0x0A34u, 0x0A35u, 0x0A37u, 0x0A38u, 0x0A3Au, 0x0A3Eu, 0x0A43u, 0x0A47u, 0x0A49u, 0x0A4Bu, 0x0A4Du, 0x0A51u,
  0x0A52u, 0x0A59u, 0x0A5Du, 0x0A5Eu, 0x0A5Fu, 0x0A70u, 0x0A76u, 0x0A81u, 0x0A84u, 0x0A85u, 0x0A8Eu, 0x0A8Fu, 0x0A92u, 0x0A93u, 0x0AA9u, 0x0AAAu,
  0x0AB1u, 0x0AB2u, 0x0AB4u, 0x0AB5u, 0x0ABAu, 0x0ABDu, 0x0AC6u, 0x0AC7u, 0x0ACAu, 0x0ACBu, 0x0ACDu, 0x0AD0u, 0x0AD1u, 0x0AE0u, 0x0AE4u, 0x0B01u,
  0x0B04u, 0x0B05u, 0x0B0Du, 0x0B0Fu, 0x0B11u, 0x0B13u, 0x0B29u, 0x0B2Au, 0x0B31u, 0x0B32u, 0x0B34u, 0x0B35u, 0x0B3Au, 0x0B3Du, 0x0B45u, 0x0B47u,
  0x0B49u, 0x0B4Bu, 0x0B4Du, 0x0B56u, 0x0B58u, 0x0B5Cu, 0x0B5Eu, 0x0B5Fu, 0x0B64u, 0x0B71u, 0x0B72u, 0x0B82u, 0x0B84u, 0x0B85u, 0x0B8Bu, 0x0B8Eu,
  0x0B91u, 0x0B92u, 0x0B96u, 0x0B99u, 0x0B9Bu, 0x0B9Cu, 0x0B9Du, 0x0B9Eu, 0x0BA0u, 0x0BA3u, 0x0BA5u, 0x0BA8u, 0x0BABu, 0x0BAEu, 0x0BBAu, 0x0BBEu,
  0x0BC3u, 0x0BC6u, 0x0BC9u, 0x0BCAu, 0x0BCDu, 0x0BD0u, 0x0BD1u, 0x0BD7u, 0x0BD8u, 0x0C01u, 0x0C04u, 0x0C05u, 0x0C0Du, 0x0C0Eu, 0x0C11u, 0x0C12u,
  0x0C29u, 0x0C2Au, 0x0C34u, 0x0C35u, 0x0C3Au, 0x0C3Du, 0x0C45u, 0x0C46u, 0x0C49u, 0x0C4Au, 0x0C4Du, 0x0C55u, 0x0C57u, 0x0C58u, 0x0C5Au, 0x0C60u,
  0x0C64u, 0x0C82u, 0x0C84u, 0x0C85u, 0x0C8Du, 0x0C8Eu, 0x0C91u, 0x0C92u, 0x0CA9u, 0x0CAAu, 0x0CB4u, 0x0CB5u, 0x0CBAu, 0x0CBDu, 0x0CC5u, 0x0CC6u,
  0x0CC9u, 0x0CCAu, 0x0CCDu, 0x0CD5u, 0x0CD7u, 0x0CDEu, 0x0CDFu, 0x0CE0u, 0x0CE4u, 0x0D02u, 0x0D04u, 0x0D05u, 0x0D0Du, 0x0D0Eu, 0x0D11u, 0x0D12u,
  0x0D29u, 0x0D2Au, 0x0D3Au, 0x0D3Du, 0x0D45u, 0x0D46u, 0x0D49u, 0x0D4Au, 0x0D4Du, 0x0D57u, 0x0D58u, 0x0D60u, 0x0D64u, 0x0D7Au, 0x0D80u, 0x0D82u,
  0x0D84u, 0x0D85u, 0x0D97u, 0x0D9Au, 0x0DB2u, 0x0DB3u, 0x0DBCu, 0x0DBDu, 0x0DBEu, 0x0DC0u, 0x0DC7u, 0x0DCFu, 0x0DD5u, 0x0DD6u, 0x0DD7u, 0x0DD8u,
  0x0DE0u, 0x0DF2u, 0x0DF4u, 0x0E01u, 0x0E3Bu, 0x0E40u, 0x0E47u, 0x0E4Du, 0x0E4Eu, 0x0E81u, 0x0E83u, 0x0E84u, 0x0E85u, 0x0E87u, 0x0E89u, 0x0E8Au,
  0x0E8Bu, 0x0E8Du, 0x0E8Eu, 0x0E94u, 0x0E98u, 0x0E99u, 0x0EA0u, 0x0EA1u, 0x0EA4u, 0x0EA5u, 0x0EA6u, 0x0EA7u, 0x0EA8u, 0x0EAAu, 0x0EACu, 0x0EADu,
  0x0EBAu, 0x0EBBu, 0x0EBEu, 0x0EC0u, 0x0EC5u, 0x0EC6u, 0x0EC7u, 0x0ECDu, 0x0ECEu, 0x0EDCu, 0x0EDEu, 0x0F00u, 0x0F01u, 0x0F40u, 0x0F48u, 0x0F49u,
  0x0F6Du, 0x0F71u, 0x0F82u, 0x0F88u, 0x0F8Cu, 0x0F90u, 0x0F98u, 0x0F99u, 0x0FBDu, 0x1000u, 0x1037u, 0x1038u, 0x1039u, 0x103Bu, 0x1040u, 0x1050u,
  0x1063u, 0x1065u, 0x1069u, 0x106Eu, 0x1087u, 0x108Eu, 0x108Fu, 0x10A0u, 0x10C6u, 0x10D0u, 0x10FBu, 0x10FCu, 0x10FDu, 0x1100u, 0x115Au, 0x115Fu,
  0x11A3u, 0x11A8u, 0x11FAu, 0x1200u, 0x1249u, 0x124Au, 0x124Eu, 0x1250u, 0x1257u, 0x1258u, 0x1259u, 0x125Au, 0x125Eu, 0x1260u, 0x1289u, 0x128Au,
  0x128Eu, 0x1290u, 0x12B1u, 0x12B2u, 0x12B6u, 0x12B8u, 0x12BFu, 0x12C0u, 0x12C1u, 0x12C2u, 0x12C6u, 0x12C8u, 0x12D7u, 0x12D8u, 0x1311u, 0x1312u,
  0x1316u, 0x1318u, 0x135Bu, 0x135Fu, 0x1360u, 0x1380u, 0x1390u, 0x13A0u, 0x13F5u, 0x1401u, 0x166Du, 0x166Fu, 0x1677u, 0x1681u, 0x169Bu, 0x16A0u,
  0x16EBu, 0x16EEu, 0x16F1u, 0x1700u, 0x170Du, 0x170Eu, 0x1714u, 0x1720u, 0x1734u, 0x1740u, 0x1754u, 0x1760u, 0x176Du, 0x176Eu, 0x1771u, 0x1772u,
  0x1774u, 0x1780u, 0x17B4u, 0x17B6u, 0x17C9u, 0x17D7u, 0x17D8u, 0x17DCu, 0x17DDu, 0x1820u, 0x1878u, 0x1880u, 0x18ABu, 0x1900u, 0x191Du, 0x1920u,
  0x192Cu, 0x1930u, 0x1939u, 0x1950u, 0x196Eu, 0x1970u, 0x1975u, 0x1980u, 0x19AAu, 0x19B0u, 0x19CAu, 0x1A00u, 0x1A1Cu, 0x1B00u, 0x1B34u, 0x1B35u,
  0x1B44u, 0x1B45u, 0x1B4Cu, 0x1B80u, 0x1BAAu, 0x1BAEu, 0x1BB0u, 0x1C00u, 0x1C36u, 0x1C4Du, 0x1C50u, 0x1C5Au, 0x1C7Eu, 0x1D00u, 0x1DC0u, 0x1E00u,
  0x1F16u, 0x1F18u, 0x1F1Eu, 0x1F20u, 0x1F46u, 0x1F48u, 0x1F4Eu, 0x1F50u, 0x1F58u, 0x1F59u, 0x1F5Au, 0x1F5Bu, 0x1F5Cu, 0x1F5Du, 0x1F5Eu, 0x1F5Fu,
  0x1F7Eu, 0x1F80u, 0x1FB5u, 0x1FB6u, 0x1FBDu, 0x1FBEu, 0x1FBFu, 0x1FC2u, 0x1FC5u, 0x1FC6u, 0x1FCDu, 0x1FD0u, 0x1FD4u, 0x1FD6u, 0x1FDCu, 0x1FE0u,
  0x1FEDu, 0x1FF2u, 0x1FF5u, 0x1FF6u, 0x1FFDu, 0x2071u, 0x2072u, 0x207Fu, 0x2080u, 0x2090u, 0x2095u, 0x2102u, 0x2103u, 0x2107u, 0x2108u, 0x210Au,
  0x2114u, 0x2115u, 0x2116u, 0x2119u, 0x211Eu, 0x2124u, 0x2125u, 0x2126u, 0x2127u, 0x2128u, 0x2129u, 0x212Au, 0x212Eu, 0x212Fu, 0x213Au, 0x213Cu,
  0x2140u, 0x2145u, 0x214Au, 0x214Eu, 0x214Fu, 0x2160u, 0x2189u, 0x24B6u, 0x24EAu, 0x2C00u, 0x2C2Fu, 0x2C30u, 0x2C5Fu, 0x2C60u, 0x2C70u, 0x2C71u,
  0x2C7Eu, 0x2C80u, 0x2CE5u, 0x2D00u, 0x2D26u, 0x2D30u, 0x2D66u, 0x2D6Fu, 0x2D70u, 0x2D80u, 0x2D97u, 0x2DA0u, 0x2DA7u, 0x2DA8u, 0x2DAFu, 0x2DB0u,
  0x2DB7u, 0x2DB8u, 0x2DBFu, 0x2DC0u, 0x2DC7u, 0x2DC8u, 0x2DCFu, 0x2DD0u, 0x2DD7u, 0x2DD8u, 0x2DDFu, 0x2DE0u, 0x2E00u, 0x2E2Fu, 0x2E30u, 0x3005u,
  0x3008u, 0x3021u, 0x302Au, 0x3031u, 0x3036u, 0x3038u, 0x303Du, 0x3041u, 0x3097u, 0x309Du, 0x30A0u, 0x30A1u, 0x30FBu, 0x30FCu, 0x3100u, 0x3105u,
  0x312Eu, 0x3131u, 0x318Fu, 0x31A0u, 0x31B8u, 0x31F0u, 0x3200u, 0x3400u, 0x4DB6u, 0x4E00u, 0x9FC4u, 0xA000u, 0xA48Du, 0xA500u, 0xA60Du, 0xA610u,
  0xA620u, 0xA62Au, 0xA62Cu, 0xA640u, 0xA660u, 0xA662u, 0xA66Fu, 0xA67Fu, 0xA698u, 0xA717u, 0xA720u, 0xA722u, 0xA789u, 0xA78Bu, 0xA78Du, 0xA7FBu,
  0xA802u, 0xA803u, 0xA806u, 0xA807u, 0xA80Bu, 0xA80Cu, 0xA828u, 0xA840u, 0xA874u, 0xA880u, 0xA8C4u, 0xA90Au, 0xA92Bu, 0xA930u, 0xA953u, 0xAA00u,
  0xAA37u, 0xAA40u, 0xAA4Eu, 0xAC00u, 0xD7A4u, 0xF900u, 0xFA2Eu, 0xFA30u, 0xFA6Bu, 0xFA70u, 0xFADAu, 0xFB00u, 0xFB07u, 0xFB13u, 0xFB18u, 0xFB1Du,
  0xFB29u, 0xFB2Au, 0xFB37u, 0xFB38u, 0xFB3Du, 0xFB3Eu, 0xFB3Fu, 0xFB40u, 0xFB42u, 0xFB43u, 0xFB45u, 0xFB46u, 0xFBB2u, 0xFBD3u, 0xFD3Eu, 0xFD50u,
  0xFD90u, 0xFD92u, 0xFDC8u, 0xFDF0u, 0xFDFCu, 0xFE70u, 0xFE75u, 0xFE76u, 0xFEFDu, 0xFF21u, 0xFF3Bu, 0xFF41u, 0xFF5Bu, 0xFF66u, 0xFFBFu, 0xFFC2u,
  0xFFC8u, 0xFFCAu, 0xFFD0u, 0xFFD2u, 0xFFD8u, 0xFFDAu, 0xFFDDu, 0xFFFFu
};

static const CPU_WCHAR WC_CharIsLowerTbl[] = {
  0x0000u, 0x00AAu, 0x00ABu, 0x00BAu, 0x00BBu, 0x00DFu, 0x00E0u, 0x0138u, 0x0139u, 0x0149u, 0x014Au, 0x018Du, 0x018Eu, 0x01AAu, 0x01ACu, 0x01BAu,
  0x01BBu, 0x01BEu, 0x01BFu, 0x01F0u, 0x01F1u, 0x0221u, 0x0222u, 0x0234u, 0x023Au, 0x023Fu, 0x0241u, 0x0250u, 0x0294u, 0x0295u, 0x02B9u, 0x02C0u,
  0x02C2u, 0x02E0u, 0x02E5u, 0x037Au, 0x037Bu, 0x0390u, 0x0391u, 0x03B0u, 0x03B1u, 0x03F3u, 0x03F4u, 0x03FCu, 0x03FDu, 0x0587u, 0x0588u, 0x1D00u,
  0x1DC0u, 0x1E96u, 0x1E9Eu, 0x1E9Fu, 0x1EA0u, 0x1F50u, 0x1F58u, 0x1FB2u, 0x1FB3u, 0x1FB4u, 0x1FB5u, 0x1FB6u, 0x1FB8u, 0x1FC2u, 0x1FC5u, 0x1FD2u,
  0x1FD4u, 0x1FD6u, 0x1FD8u, 0x1FE0u, 0x1FE8u, 0x1FF2u, 0x1FF5u, 0x1FF6u, 0x1FF8u, 0x2071u, 0x2072u, 0x207Fu, 0x2080u, 0x2090u, 0x2095u, 0x210Au,
  0x210Bu, 0x210Eu, 0x2110u, 0x2113u, 0x2114u, 0x212Fu, 0x2130u, 0x2134u, 0x2135u, 0x2139u, 0x213Au, 0x213Cu, 0x213Du, 0x2146u, 0x214Au, 0x2C74u,
  0x2C75u, 0x2C77u, 0x2C7Eu, 0x2CE4u, 0x2CE5u, 0xA730u, 0xA732u, 0xA770u, 0xA779u, 0xFB00u, 0xFB07u, 0xFB13u, 0xFB18u, 0xFF41u, 0xFF5Bu, 0xFFFFu
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static const WC_SRCH_TBL_ENTRY *WC_SrchTbl(const WC_SRCH_TBL_ENTRY *p_tbl,
                                           CPU_SIZE_T              tbl_len,
                                           CPU_WCHAR               c_key);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               WC_StrLen()
 *
 * @brief    Calculate length of a wide-character string.
 *
 * @param    p_str   Pointer to wide-character string (see Note #1).
 *
 * @return   Length of string; number of characters in string before terminating NULL character.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String length calculation terminates when :
 *           - (a) String pointer points to NULL.
 *               - (1) String buffer overlaps with NULL address.
 *               - (2) String length calculated for string up to but NOT beyond or including
 *                       the NULL address.
 *           - (b) Terminating NULL character found.
 *               - (1) String length calculated for string up to but NOT           including
 *                       the NULL character.
 *******************************************************************************************************/
CPU_SIZE_T WC_StrLen(CPU_WCHAR *p_str)
{
  CPU_SIZE_T len;

  len = WC_StrLen_N(p_str,
                    DEF_INT_CPU_U_MAX_VAL);

  return (len);
}

/****************************************************************************************************//**
 *                                               WC_StrLen_N()
 *
 * @brief    Calculate length of a wide-character string, up to a maximum number of characters.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    len_max     Maximum number of characters to search (see Note #2c).
 *
 * @return   Length of string; number of characters in string before terminating NULL character,
 *           if terminating NULL character found.
 *           Requested maximum number of characters to search,
 *           if terminating NULL character NOT found.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String length calculation terminates when :
 *           - (a) String pointer points to NULL.
 *               - (1) String buffer overlaps with NULL address.
 *               - (2) String length calculated for string up to but NOT beyond or including
 *                       the NULL address.
 *           - (b) Terminating NULL character found.
 *               - (1) String length calculated for string up to but NOT           including
 *                       the NULL character.
 *           - (c) 'len_max' number of characters searched.
 *               - (1) 'len_max' number of characters does NOT include the terminating NULL character.
 *******************************************************************************************************/
CPU_SIZE_T WC_StrLen_N(CPU_WCHAR  *p_str,
                       CPU_SIZE_T len_max)
{
  CPU_SIZE_T len;

  len = 0u;
  while ((p_str != (CPU_WCHAR *)  0)                            // Calc str len until NULL ptr (see Note #2a) ...
         && (*p_str != (CPU_WCHAR)'\0')                         // ... or NULL char found      (see Note #2b) ...
         && (len < (CPU_SIZE_T)len_max)) {                      // ... or max nbr chars srch'd (see Note #2c).
    len++;
    p_str++;
  }

  return (len);
}

/****************************************************************************************************//**
 *                                               WC_StrCopy()
 *
 * @brief    Copy source wide-character string to destination wide-character string buffer.
 *
 * @param    p_dest  Pointer to destination wide-character string buffer to receive source string
 *                   copy (see Note #1).
 *
 * @param    p_src   Pointer to source wide-character string to copy into destination string
 *                   buffer.
 *
 * @return   Pointer to destination string, if NO errors (see Note #2).
 *           Pointer to NULL,               otherwise.
 *
 * @note     (1) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
 *           - (a) Destination buffer size MUST be large enough to accommodate the entire source
 *                   string size including the terminating NULL character.
 *
 * @note     (2) String copy terminates when :
 *           - (a) Destination/Source string pointer(s) are passed NULL pointers.
 *               - (1) No string copy performed; NULL pointer returned.
 *           - (b) Destination/Source string pointer(s) points to NULL.
 *               - (1) String buffer(s) overlap with NULL address.
 *               - (2) Source string copied into destination string buffer up to but NOT beyond or
 *                       including the NULL address; destination string buffer properly terminated
 *                       with NULL character.
 *           - (c) Source string's terminating NULL character found.
 *               - (1) Entire source string copied into destination string buffer.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrCopy(CPU_WCHAR *p_dest,
                      CPU_WCHAR *p_src)
{
  p_dest = WC_StrCopy_N(p_dest,
                        p_src,
                        DEF_INT_CPU_U_MAX_VAL);

  return (p_dest);
}

/****************************************************************************************************//**
 *                                               WC_StrCopy_N()
 *
 * @brief    Copy source wide-character string to destination wide-character string buffer, up to a
 *           maximum number of characters.
 *
 * @param    p_dest      Pointer to destination wide-character string buffer to receive source string
 *                       copy (see Note #1).
 *
 * @param    p_src       Pointer to source wide-character string to copy into destination string
 *                       buffer.
 *
 * @param    len_max     Maximum number of characters to copy (see Note #2d).
 *
 * @return   Pointer to destination string, if NO errors (see Note #2).
 *           Pointer to NULL,               otherwise.
 *
 * @note     (1) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
 *           - (a) Destination buffer size MUST be large enough to accommodate the entire source
 *                   string size including the terminating NULL character.
 *
 * @note     (2) String copy terminates when :
 *           - (a) Destination/Source string pointer(s) are passed NULL pointers.
 *               - (1) No string copy performed; NULL pointer returned.
 *           - (b) Destination/Source string pointer(s) points to NULL.
 *               - (1) String buffer(s) overlap with NULL address.
 *               - (2) Source string copied into destination string buffer up to but NOT beyond or
 *                       including the NULL address; destination string buffer properly terminated
 *                       with NULL character.
 *           - (c) Source string's terminating NULL character found.
 *               - (1) Entire source string copied into destination string buffer.
 *           - (d) 'len_max' number of characters copied.
 *               - (1) 'len_max' number of characters does NOT include the terminating NULL character.
 *           See also Note #1a.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrCopy_N(CPU_WCHAR  *p_dest,
                        CPU_WCHAR  *p_src,
                        CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p_str;
  CPU_WCHAR  *p_str_next;
  CPU_SIZE_T len_copy;

  //                                                               Rtn NULL if str ptr(s) NULL      (see Note #2a).
  if (p_dest == (CPU_WCHAR *)0) {
    return  ((CPU_WCHAR *)0);
  }
  if (p_src == (CPU_WCHAR *)0) {
    return  ((CPU_WCHAR *)0);
  }

  if (len_max == (CPU_SIZE_T)0) {                               // Rtn NULL if copy len equals zero (see Note #2d).
    return ((CPU_WCHAR *)0);
  }

  p_str = p_dest;
  p_str_next = p_str;
  p_str_next++;
  len_copy = 0u;

  while ((p_str_next != (CPU_WCHAR *)0)                         // Copy str until NULL ptr(s)  [see Note #2b]  ...
         && (p_src != (CPU_WCHAR *)0)
         && (*p_src != (CPU_WCHAR)0)                            // ... or NULL char found      (see Note #2c); ...
         && (len_copy < (CPU_SIZE_T)len_max)) {                 // ... or max nbr chars copied (see Note #2d).
    *p_str = *p_src;
    p_str++;
    p_str_next++;
    p_src++;
    len_copy++;
  }

  *p_str = (CPU_WCHAR)'\0';                                     // Append NULL char (see Note #2b2).

  return (p_dest);
}

/****************************************************************************************************//**
 *                                               WC_StrCat()
 *
 * @brief    Append concatenation wide-character string to destination wide-character string.
 *
 * @param    p_dest      Pointer to destination wide-character string to append concatenation string
 *                       (see Note #1).
 *
 * @param    p_str_cat   Pointer to concatenation wide-character string to append to destination
 *                       string.
 *
 * @return   Pointer to destination string, if NO errors (see Note #2).
 *           Pointer to NULL,               otherwise.
 *
 * @note     (1) Destination string buffer size NOT validated; buffer overruns MUST be prevented by caller.
 *           - (a) Destination buffer size MUST be large enough to accommodate the entire concatenated
 *                   string size including the terminating NULL character.
 *
 * @note     (2) String concatenation terminates when :
 *           - (a) Destination/Concatenation string pointer(s) are passed NULL pointers.
 *               - (1) No string concatenation performed; NULL pointer returned.
 *           - (b) Destination string overlaps with NULL address.
 *               - (1) No string concatenation performed; NULL pointer returned.
 *           - (c) Destination/Concatenation string pointer(s) points to NULL.
 *               - (1) String buffer(s) overlap with NULL address.
 *               - (2) Concatenation string appended into destination string buffer up to but NOT
 *                       beyond or including the NULL address; destination string buffer properly
 *                       terminated with NULL character.
 *           - (d) Concatenation string's terminating NULL character found.
 *               - (1) Entire concatenation string appended to destination string.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrCat(CPU_WCHAR *p_dest,
                     CPU_WCHAR *p_str_cat)
{
  p_dest = WC_StrCat_N(p_dest,
                       p_str_cat,
                       DEF_INT_CPU_U_MAX_VAL);

  return (p_dest);
}

/****************************************************************************************************//**
 *                                               WC_StrCat_N()
 *
 * @brief    Append wide-character concatenation string to destination wide-character string, up to a
 *           maximum number of characters.
 *
 * @param    p_dest      Pointer to destination wide-character string to append concatenation string
 *                       (see Note #1).
 *
 * @param    p_str_cat   Pointer to concatenation wide-character string to append to destination
 *                       string.
 *
 * @param    len_max     Maximum number of characters to concatenate (see Note #2e).
 *
 * @return   Pointer to destination string, if NO errors (see Note #2).
 *           Pointer to NULL,               otherwise.
 *
 * @note     (1) Destination string buffer size NOT validated; buffer overruns MUST be prevented by caller.
 *               - (a) Destination buffer size MUST be large enough to accommodate the entire concatenated
 *                     string size including the terminating NULL character.
 *
 * @note     (2) String concatenation terminates when :
 *               - (a) Destination/Concatenation string pointer(s) are passed NULL pointers.
 *                   - (1) No string concatenation performed; NULL pointer returned.
 *               - (b) Destination string overlaps with NULL address.
 *                   - (1) No string concatenation performed; NULL pointer returned.
 *               - (c) Destination/Concatenation string pointer(s) points to NULL.
 *                   - (1) String buffer(s) overlap with NULL address.
 *                   - (2) Concatenation string appended into destination string buffer up to but NOT
 *                       beyond or including the NULL address; destination string buffer properly
 *                       terminated with NULL character.
 *               - (d) Concatenation string's terminating NULL character found.
 *                   - (1) Entire concatenation string appended to destination string.
 *               - (e) 'len_max' number of characters concatenated.
 *                   - (1) 'len_max' number of characters does NOT include the terminating NULL character.
 *           See also Note #1a.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrCat_N(CPU_WCHAR  *p_dest,
                       CPU_WCHAR  *p_str_cat,
                       CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p_str;
  CPU_WCHAR  *p_str_next;
  CPU_SIZE_T len_cat;

  //                                                               Rtn NULL if str ptr(s) NULL     (see Note #2a).
  if (p_dest == (CPU_WCHAR *)0) {
    return  ((CPU_WCHAR *)0);
  }
  if (p_str_cat == (CPU_WCHAR *)0) {
    return  ((CPU_WCHAR *)0);
  }

  if (len_max == (CPU_SIZE_T)0) {                               // Rtn NULL if cat len equals zero (see Note #2e).
    return  ((CPU_WCHAR *)0);
  }

  p_str = p_dest;
  while ((p_str != (CPU_WCHAR *)0)                              // Adv to end of cur dest str until NULL ptr ...
         && (*p_str != (CPU_WCHAR)0)) {                         // ... or NULL char found..
    p_str++;
  }
  if (p_str == (CPU_WCHAR *)0) {                                // If NULL str overrun, rtn NULL (see Note #2b).
    return ((CPU_WCHAR *)0);
  }

  p_str_next = p_str;
  p_str_next++;
  len_cat = 0u;

  while ((p_str_next != (CPU_WCHAR *)0)                         // Cat str until NULL ptr(s)  [see Note #2c]  ...
         && (p_str_cat != (CPU_WCHAR *)0)
         && (*p_str_cat != (CPU_WCHAR)0)                        // ... or NULL char found     (see Note #2d); ...
         && (len_cat < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cat'd (see Note #2d).
    *p_str = *p_str_cat;
    p_str++;
    p_str_next++;
    p_str_cat++;
    len_cat++;
  }

  *p_str = (CPU_WCHAR)0;                                        // Append NULL char (see Note #2c2).

  return (p_dest);
}

/****************************************************************************************************//**
 *                                               WC_StrCmp()
 *
 * @brief    Determine if two wide-character strings are identical.
 *
 * @param    p1_str  Pointer to first wide-character string (see Note #1).
 *
 * @param    p2_str  Pointer to second wide-character string (see Note #1).
 *
 * @return   0,              if strings are identical             (see Notes #3a1A, #3a2A, #3b, & #3d).
 *           Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #3a1B1, #3a2B1, & #3c).
 *           Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #3a1B2, #3a2B2, & #3c).
 *           See also Note #2b.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) (a) IEEE Std 1003.1, 2004 Edition, Section 'wcscmp() : DESCRIPTION' states that "the
 *                   strcmp() function shall compare the wide-character string pointed to by ws1 to
 *                   the string pointed to by wide-character ws2".
 *           - (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'wcscmp() : RETURN VALUE' states that
 *                       "upon completion, strncmp() shall return an integer greater than, equal to, or
 *                       less than 0".
 *               - (2) IEEE Std 1003.1, 2004 Edition, Section 'wcscmp() : DESCRIPTION' adds that "the
 *                       sign of a non-zero return value shall be determined by the sign of the
 *                       difference between the values of the first pair of wide-character codes that
 *                       differ in the objects being compared".
 *
 * @note     (3) String comparison terminates when :
 *           - (a) (1) (A) BOTH string pointer(s) are passed NULL pointers.
 *                       - (1) NULL strings identical; return 0.
 *                   - (B) (1) 'p1_str' passed a NULL pointer.
 *                           - (a) Return negative value of character pointed to by 'p2_str'.
 *                       - (2) 'p2_str' passed a NULL pointer.
 *                           - (a) Return positive value of character pointed to by 'p1_str'.
 *               - (2) (A) BOTH strings point to NULL.
 *                       - (1) Strings overlap with NULL address.
 *                       - (2) Strings identical up to but NOT beyond or including the NULL address;
 *                               return 0.
 *                   - (B) (1) 'p1_str_next' points to NULL.
 *                           - (a) 'p1_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return negative value of character pointed to by 'p2_str_next'.
 *                       - (2) 'p2_str_next' points to NULL.
 *                           - (a) 'p2_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return positive value of character pointed to by 'p1_str_next'.
 *           - (b) Terminating NULL character found in both strings.
 *               - (1) Strings identical; return 0.
 *               - (2) Only one NULL character test required in conditional since previous condition
 *                       tested character equality.
 *           - (c) Non-matching characters found.
 *               - (1) Return signed-integer difference of the character pointed to by 'p2_str'
 *                       from the character pointed to by 'p1_str'.
 * @note     (4) Since 32-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_WCHAR' native data type size MUST be 16-bit.
 *******************************************************************************************************/
CPU_INT32S WC_StrCmp(CPU_WCHAR *p1_str,
                     CPU_WCHAR *p2_str)
{
  CPU_INT32S cmp_val;

  cmp_val = WC_StrCmp_N(p1_str,
                        p2_str,
                        DEF_INT_CPU_U_MAX_VAL);

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                               WC_StrCmp_N()
 *
 * @brief    Determine if two wide-character strings are identical for up to a maximum number of
 *           characters.
 *
 * @param    p1_str      Pointer to first wide-character string (see Note #1).
 *
 * @param    p2_str      Pointer to second wide-character string (see Note #1).
 *
 * @param    len_max     Maximum number of characters to compare  (see Note  #3d).
 *
 * @return   0,              if strings are identical             (see Notes #3a1A, #3a2A, #3b, & #3d).
 *           Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #3a1B1, #3a2B1, & #3c).
 *           Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #3a1B2, #3a2B2, & #3c).
 *           See also Note #2b.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) (a) IEEE Std 1003.1, 2004 Edition, Section 'wcsncmp() : DESCRIPTION' states that :
 *               - (1) "The wcsncmp() function shall compare ... the array pointed to by ws1 to the
 *                       array pointed to by ws2" but ...
 *               - (2) "not more than 'n' wide-character codes" of either array.
 *           - (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'wcsncmp() : RETURN VALUE' states that
 *                       "upon successful completion, wcstrncmp() shall return an integer greater than,
 *                       equal to, or less than 0".
 *               - (2) IEEE Std 1003.1, 2004 Edition, Section 'wcsncmp() : DESCRIPTION' adds that
 *                       "the sign of a non-zero return value is determined by the sign of the difference
 *                       between the values of the first pair of wide-character codes ... that differ
 *                       in the objects being compared".
 * @note     (3) String comparison terminates when :
 *           - (a) (1) (A) BOTH string pointer(s) are passed NULL pointers.
 *                       - (1) NULL strings identical; return 0.
 *                   - (B) (1) 'p1_str' passed a NULL pointer.
 *                           - (a) Return negative value of character pointed to by 'p2_str'.
 *                       - (2) 'p2_str' passed a NULL pointer.
 *                           - (a) Return positive value of character pointed to by 'p1_str'.
 *               - (2) (A) BOTH strings point to NULL.
 *                       - (1) Strings overlap with NULL address.
 *                       - (2) Strings identical up to but NOT beyond or including the NULL address;
 *                               return 0.
 *                   - (B) (1) 'p1_str_next' points to NULL.
 *                           - (a) 'p1_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return negative value of character pointed to by 'p2_str_next'.
 *                       - (2) 'p2_str_next' points to NULL.
 *                           - (a) 'p2_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return positive value of character pointed to by 'p1_str_next'.
 *           - (b) Terminating NULL character found in both strings.
 *               - (1) Strings identical; return 0.
 *               - (2) Only one NULL character test required in conditional since previous condition
 *                       tested character equality.
 *           - (c) Non-matching characters found.
 *               - (1) Return signed-integer difference of the character pointed to by 'p2_str'
 *                       from the character pointed to by 'p1_str'.
 *           - (d) (1) 'len_max' passed a zero length.
 *                   - (A) Zero-length strings identical; return 0.
 *               - (2) First 'len_max' number of characters identical.
 *                   - (A) Strings identical; return 0.
 *                   See also Note #2a2.
 *
 * @note     (4) Since 32-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_WCHAR' native data type size MUST be 16-bit.
 *******************************************************************************************************/
CPU_INT32S WC_StrCmp_N(CPU_WCHAR  *p1_str,
                       CPU_WCHAR  *p2_str,
                       CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p1_str_next;
  CPU_WCHAR  *p2_str_next;
  CPU_WCHAR  char1;
  CPU_WCHAR  char2;
  CPU_INT32S cmp_val;
  CPU_SIZE_T cmp_len;

  if (len_max == 0u) {                                          // If cmp len equals zero, rtn 0      (see Note #3d1).
    return ((CPU_INT32S)0);
  }

  if (p1_str == (CPU_WCHAR *)0) {
    if (p2_str == (CPU_WCHAR *)0) {
      return ((CPU_INT32S)0);                                   // If BOTH str ptrs NULL,  rtn 0      (see Note #3a1A).
    }
    char2 = *p2_str;
    cmp_val = (CPU_INT32S)0 - (CPU_INT32S)char2;
    return (cmp_val);                                           // If p1_str NULL, rtn neg p2_str val (see Note #3a1B1).
  }
  if (p2_str == (CPU_WCHAR *)0) {
    char1 = *p1_str;
    cmp_val = (CPU_INT32S)char1;
    return (cmp_val);                                           // If p2_str NULL, rtn pos p1_str val (see Note #3a1B2).
  }

  p1_str_next = p1_str;
  p2_str_next = p2_str;
  p1_str_next++;
  p2_str_next++;
  char1 = *p1_str;
  char2 = *p2_str;
  cmp_len = 0u;
  while ((char1 == char2)                                       // Cmp strs until non-matching chars (see Note #3c) ...
         && (*p1_str != (CPU_WCHAR)0)                           // ... or NULL chars                 (see Note #3b) ...
         && (p1_str_next != (CPU_WCHAR *)0)                     // ... or NULL ptr(s) found          (see Note #3a2)
         && (p2_str_next != (CPU_WCHAR *)0)
         && (cmp_len < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cmp'd        (see Note #3d2).
    p1_str_next++;
    p2_str_next++;
    p1_str++;
    p2_str++;
    cmp_len++;
    char1 = *p1_str;
    char2 = *p2_str;
  }

  if (cmp_len == len_max) {                                     // If strs     identical for len nbr of chars, ...
    return ((CPU_INT32S)0);                                     // ... rtn 0                   (see Note #3d2).
  }

  if (char1 != char2) {                                         // If strs NOT identical, ...
    cmp_val = (CPU_INT32S)char1 - (CPU_INT32S)char2;            // ... calc & rtn char diff    (see Note #3c).
  } else if (char1 == (CPU_WCHAR)0) {                           // If NULL char(s) found, ...
    cmp_val = 0;                                                // ... strs identical; rtn 0   (see Note #3b).
  } else {
    if (p1_str_next == (CPU_WCHAR *)0) {
      if (p2_str_next == (CPU_WCHAR *)0) {                      // If BOTH next str ptrs NULL, ...
        cmp_val = (CPU_INT32S)0;                                // ... rtn 0                   (see Note #3a2A).
      } else {                                                  // If p1_str_next NULL, ...
        char2 = *p2_str_next;
        cmp_val = (CPU_INT32S)0 - (CPU_INT32S)char2;            // ... rtn neg p2_str_next val (see Note #3a2B1).
      }
    } else {                                                    // If p2_str_next NULL, ...
      char1 = *p1_str_next;
      cmp_val = (CPU_INT32S)char1;                              // ... rtn pos p1_str_next val (see Note #3a2B2).
    }
  }

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                           WC_StrCmpIgnoreCase()
 *
 * @brief    Determine if two wide-character strings are identical, ignoring case.
 *
 * @param    p1_str  Pointer to first wide-character string (see Note #1).
 *
 * @param    p2_str  Pointer to second wide-character string (see Note #1).
 *
 * @return   0,              if strings are identical             (see Notes #3a1A, #3a2A, #3b, & #3d).
 *           Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #3a1B1, #3a2B1, & #3c).
 *           Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #3a1B2, #3a2B2, & #3c).
 *           See also Note #2b.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) (1) (1) The Unicode Standard 5.0, Section 5.18, states that "caseless matching is
 *                       implemented using case folding, which is the process of mapping strings to a
 *                       canonical form where case differences are erased."
 *               - (2) The Unicode Standard 5.0, Section 3.13, D129, states that "a string X is a
 *                       caseless match for a string Y if and only if: toCasefold(X) = toCasefold(Y)".
 *           - (c) (1) IEEE Std 1003.1, 2004 Edition, Section 'wcscmp() : RETURN VALUE' states that
 *                       "upon completion, strncmp() shall return an integer greater than, equal to, or
 *                       less than 0".
 *               - (2) IEEE Std 1003.1, 2004 Edition, Section 'wcscmp() : DESCRIPTION' adds that "the
 *                       sign of a non-zero return value shall be determined by the sign of the
 *                       difference between the values of the first pair of wide-character codes that
 *                       differ in the objects being compared".
 *
 * @note     (3) String comparison terminates when :
 *           - (a) (1) (A) BOTH string pointer(s) are passed NULL pointers.
 *                       - (1) NULL strings identical; return 0.
 *                   - (B) (1) 'p1_str' passed a NULL pointer.
 *                           - (a) Return negative value of character pointed to by 'p2_str', case
 *                                   folded (see Note #5).
 *                       - (2) 'p2_str' passed a NULL pointer.
 *                           - (a) Return positive value of character pointed to by 'p1_str', case
 *                                   folded (see Note #5).
 *               - (2) (A) BOTH strings point to NULL.
 *                       - (1) Strings overlap with NULL address.
 *                       - (2) Strings identical up to but NOT beyond or including the NULL address;
 *                               return 0.
 *                   - (B) (1) 'p1_str_next' points to NULL.
 *                           - (a) 'p1_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return negative value of character pointed to by 'p2_str_next',
 *                                   case folded (see Note #5).
 *                       - (2) 'p2_str_next' points to NULL.
 *                           - (a) 'p2_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return positive value of character pointed to by 'p1_str_next',
 *                                   case folded (see Note #5).
 *           - (b) Terminating NULL character found in both strings.
 *               - (1) Strings identical; return 0.
 *               - (2) Only one NULL character test required in conditional since previous condition
 *                       tested character equality.
 *           - (c) Non-matching characters found.
 *               - (1) Return signed-integer difference of the character pointed to by 'p2_str',
 *                       case folded, from the character pointed to by 'p1_str', case folded.
 *
 * @note     (4) Since 32-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_WCHAR' native data type size MUST be 16-bit.
 *
 * @note     (5) In analogy to 'Str_CmpIgnoreCase()', the return value is calculated from the value of
 *               the character(s) from the strings, case-folded.
 *******************************************************************************************************/
CPU_INT32S WC_StrCmpIgnoreCase(CPU_WCHAR *p1_str,
                               CPU_WCHAR *p2_str)
{
  CPU_INT32S cmp_val;

  cmp_val = WC_StrCmpIgnoreCase_N(p1_str,
                                  p2_str,
                                  DEF_INT_CPU_U_MAX_VAL);

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                           WC_StrCmpIgnoreCase_N()
 *
 * @brief    Determine if two wide-character strings are identical for up to a maximum number of
 *           characters, ignoring case.
 *
 * @param    p1_str      Pointer to first wide-character string (see Note #1).
 *
 * @param    p2_str      Pointer to second wide-character string (see Note #1).
 *
 * @param    len_max     Maximum number of characters to compare  (see Note  #3d).
 *
 * @return   0,              if strings are identical             (see Notes #3a1A, #3a2A, #3b, & #3d).
 *           Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #3a1B1, #3a2B1, & #3c).
 *           Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #3a1B2, #3a2B2, & #3c).
 *           See also Note #2b.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) (a) (1) The Unicode Standard 5.0, Section 5.18, states that "caseless matching is
 *                       implemented using case folding, which is the process of mapping strings to a
 *                       canonical form where case differences are erased."
 *               - (2) The Unicode Standard 5.0, Section 3.13, D129, states that "a string X is a
 *                       caseless match for a string Y if and only if: toCasefold(X) = toCasefold(Y)".
 *           - (c) (1) IEEE Std 1003.1, 2004 Edition, Section 'wcsncmp() : RETURN VALUE' states that
 *                       "upon successful completion, wcstrncmp() shall return an integer greater than,
 *                       equal to, or less than 0".
 *               - (2) IEEE Std 1003.1, 2004 Edition, Section 'wcsncmp() : DESCRIPTION' adds that
 *                       "the sign of a non-zero return value is determined by the sign of the difference
 *                       between the values of the first pair of wide-character codes ... that differ
 *                       in the objects being compared".
 *
 * @note     (3) String comparison terminates when :
 *           - (a) (1) (A) BOTH string pointer(s) are passed NULL pointers.
 *                       - (1) NULL strings identical; return 0.
 *                   - (B) (1) 'p1_str' passed a NULL pointer.
 *                           - (a) Return negative value of character pointed to by 'p2_str', case
 *                                   folded (see Note #5).
 *                       - (2) 'p2_str' passed a NULL pointer.
 *                           - (a) Return positive value of character pointed to by 'p1_str', case
 *                                   folded (see Note #5).
 *               - (2) (A) BOTH strings point to NULL.
 *                       - (1) Strings overlap with NULL address.
 *                       - (2) Strings identical up to but NOT beyond or including the NULL address;
 *                               return 0.
 *                   - (B) (1) 'p1_str_next' points to NULL.
 *                           - (a) 'p1_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return negative value of character pointed to by 'p2_str_next',
 *                                   case folded (see Note #5).
 *                       - (2) 'p2_str_next' points to NULL.
 *                           - (a) 'p2_str' overlaps with NULL address.
 *                           - (b) Strings compared up to but NOT beyond or including the NULL address.
 *                           - (c) Return positive value of character pointed to by 'p1_str_next',
 *                                   case folded (see Note #5).
 *           - (b) Terminating NULL character found in both strings.
 *               - (1) Strings identical; return 0.
 *               - (2) Only one NULL character test required in conditional since previous condition
 *                       tested character equality.
 *           - (c) Non-matching characters found.
 *               - (1) Return signed-integer difference of the character pointed to by 'p2_str',
 *                       case folded, from the character pointed to by 'p1_str', case folded.
 *           - (d) (1) 'len_max' passed a zero length.
 *                   - (A) Zero-length strings identical; return 0.
 *               - (2) First 'len_max' number of characters identical.
 *                   - (A) Strings identical; return 0.
 *
 *                   See also Note #2a1C.
 *
 * @note     (4) Since 32-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_WCHAR' native data type size MUST be 16-bit.
 *
 * @note     (5) In analogy to 'Str_CmpIgnoreCase_N()', the return value is calculated from the value
 *               of the character(s) from the strings, case-folded.
 *******************************************************************************************************/
CPU_INT32S WC_StrCmpIgnoreCase_N(CPU_WCHAR  *p1_str,
                                 CPU_WCHAR  *p2_str,
                                 CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p1_str_next;
  CPU_WCHAR  *p2_str_next;
  CPU_WCHAR  char1;
  CPU_WCHAR  char2;
  CPU_INT32S cmp_val;
  CPU_SIZE_T cmp_len;

  if (len_max == 0u) {                                          // If cmp len equals zero, rtn 0      (see Note #3d1).
    return ((CPU_INT32S)0);
  }

  if (p1_str == (CPU_WCHAR *)0) {
    if (p2_str == (CPU_WCHAR *)0) {
      return ((CPU_INT32S)0);                                   // If BOTH str ptrs NULL,  rtn 0      (see Note #3a1A).
    }
    char2 = WC_CharToCasefold(*p2_str);
    cmp_val = (CPU_INT32S)0 - (CPU_INT32S)char2;
    return (cmp_val);                                           // If p1_str NULL, rtn neg p2_str val (see Note #3a1B1).
  }
  if (p2_str == (CPU_WCHAR *)0) {
    char1 = WC_CharToCasefold(*p1_str);
    cmp_val = (CPU_INT32S)char1;
    return (cmp_val);                                           // If p2_str NULL, rtn pos p1_str val (see Note #3a1B2).
  }

  p1_str_next = p1_str;
  p2_str_next = p2_str;
  p1_str_next++;
  p2_str_next++;
  char1 = WC_CharToCasefold(*p1_str);
  char2 = WC_CharToCasefold(*p2_str);
  cmp_len = 0u;
  while ((char1 == char2)                                       // Cmp strs until non-matching chars (see Note #3c) ...
         && (*p1_str != (CPU_WCHAR)0)                           // ... or NULL chars                 (see Note #3b) ...
         && (p1_str_next != (CPU_WCHAR *)0)                     // ... or NULL ptr(s) found          (see Note #3a2)
         && (p2_str_next != (CPU_WCHAR *)0)
         && (cmp_len < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cmp'd        (see Note #3d2).
    p1_str_next++;
    p2_str_next++;
    p1_str++;
    p2_str++;
    cmp_len++;
    char1 = WC_CharToCasefold(*p1_str);
    char2 = WC_CharToCasefold(*p2_str);
  }

  if (cmp_len == len_max) {                                     // If strs     identical for len nbr of chars, ...
    return ((CPU_INT32S)0);                                     // ... rtn 0                   (see Note #3d2).
  }

  if (char1 != char2) {                                         // If strs NOT identical, ...
    cmp_val = (CPU_INT32S)char1 - (CPU_INT32S)char2;            // ... calc & rtn char diff    (see Note #3c).
  } else if (char1 == (CPU_WCHAR)0) {                           // If NULL char(s) found, ...
    cmp_val = 0;                                                // ... strs identical; rtn 0   (see Note #3b).
  } else {
    if (p1_str_next == (CPU_WCHAR *)0) {
      if (p2_str_next == (CPU_WCHAR *)0) {                      // If BOTH next str ptrs NULL, ...
        cmp_val = (CPU_INT32S)0;                                // ... rtn 0                   (see Note #3a2A).
      } else {                                                  // If p1_str_next NULL, ...
        char2 = WC_CharToCasefold(*p2_str_next);
        cmp_val = (CPU_INT32S)0 - (CPU_INT32S)char2;            // ... rtn neg p2_str_next val (see Note #3a2B1).
      }
    } else {                                                    // If p2_str_next NULL, ...
      char1 = WC_CharToCasefold(*p1_str_next);
      cmp_val = (CPU_INT32S)char1;                              // ... rtn pos p1_str_next val (see Note #3a2B2).
    }
  }

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                               WC_StrChar()
 *
 * @brief    Search wide-character string for first occurrence of specific character.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to first occurrence of search character in string, if any.
 *           Pointer to NULL,                                           otherwise.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) String pointer points to NULL.
 *                   - (1) String overlaps with NULL address.
 *                   - (2) String searched up to but NOT beyond or including the NULL address.
 *               - (c) String's terminating NULL character found.
 *                   - (1) Search character NOT found in search string; NULL pointer returned.
 *                   - (2) Applicable ONLY IF search character is NOT the terminating NULL character.
 *               - (d) Search character found.
 *                   - (1) Return pointer to first occurrence of search character in search string.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrChar(CPU_WCHAR *p_str,
                      CPU_WCHAR srch_char)
{
  p_str = WC_StrChar_N(p_str,
                       DEF_INT_CPU_U_MAX_VAL,
                       srch_char);

  return (p_str);
}

/****************************************************************************************************//**
 *                                               WC_StrChar_N()
 *
 * @brief    Search wide-character string for first occurrence of specific character, up to a maximum
 *           number of characters.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    len_max     Maximum number of characters to search (see Notes #2e & #3).
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to first occurrence of search character in string, if any.
 *           Pointer to NULL,                                           otherwise.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) String pointer points to NULL.
 *                   - (1) String overlaps with NULL address.
 *                   - (2) String searched up to but NOT beyond or including the NULL address.
 *               - (c) String's terminating NULL character found.
 *                   - (1) Search character NOT found in search string; NULL pointer returned.
 *                   - (2) Applicable ONLY IF search character is NOT the terminating NULL character.
 *               - (d) Search character found.
 *                   - (1) Return pointer to first occurrence of search character in search string.
 *               - (e) 'len_max' number of characters searched.
 *                   - (1) 'len_max' number of characters does NOT include terminating NULL character.
 *
 * @note     (3) Ideally, the 'len_max' parameter would be the last parameter in this function's
 *               parameter list for consistency with all other custom string library functions.
 *               However, the 'len_max' parameter is ordered to comply with the standard library
 *               function's parameter list.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrChar_N(CPU_WCHAR  *p_str,
                        CPU_SIZE_T len_max,
                        CPU_WCHAR  srch_char)
{
  CPU_WCHAR  *p_str_next;
  CPU_SIZE_T len_srch;

  if (p_str == (CPU_WCHAR *)0) {                                // Rtn NULL if srch str ptr NULL    (see Note #2a).
    return ((CPU_WCHAR *)0);
  }

  if (len_max == (CPU_SIZE_T)0) {                               // Rtn NULL if srch len equals zero (see Note #2e).
    return ((CPU_WCHAR *)0);
  }

  p_str_next = p_str;
  p_str_next++;
  len_srch = 0u;
  while ((p_str_next != (CPU_WCHAR *)0)                         // Srch str until NULL ptr(s)  [see Note #2b]  ...
         && (*p_str != (CPU_WCHAR)0)                            // ... or NULL char            (see Note #2c)  ...
         && (*p_str != (CPU_WCHAR)srch_char)                    // ... or srch char found      (see Note #2d); ...
         && (len_srch < (CPU_SIZE_T)len_max)) {                 // ... or max nbr chars srch'd (see Note #2e).
    p_str++;
    p_str_next++;
    len_srch++;
  }

  if (*p_str != srch_char) {                                    // If srch char NOT found, str points to NULL; ...
    return ((CPU_WCHAR *)0);                                    // ... rtn NULL (see Notes #2b & #2c).
  }

  return (p_str);                                               // Else rtn ptr to found srch char (see Note #2d).
}

/****************************************************************************************************//**
 *                                               WC_StrChar_Last()
 *
 * @brief    Search wide-character string for last occurrence of specific character.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to last occurrence of search character in string, if any.
 *           Pointer to NULL,                                          otherwise.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) String pointer points to NULL.
 *                   - (1) String overlaps with NULL address.
 *                   - (2) String searched up to but NOT beyond or including the NULL address.
 *                   - (3) NULL address boundary handled in WC_StrLen().
 *               - (c) String searched from end to beginning.
 *                   - (1) Search character NOT found in search string; NULL pointer returned.
 *                   - (2) Applicable ONLY IF search character is NOT the terminating NULL character.
 *               - (d) Search character found.
 *                   - (1) Return pointer to first occurrence of search character in search string.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrChar_Last(CPU_WCHAR *p_str,
                           CPU_WCHAR srch_char)
{
  CPU_WCHAR  *p_str_next;
  CPU_SIZE_T str_len;

  if (p_str == (CPU_WCHAR *)0) {                                // Rtn NULL if srch str ptr NULL (see Note #2a).
    return ((CPU_WCHAR *)0);
  }

  p_str_next = p_str;
  str_len = WC_StrLen(p_str);
  p_str_next += str_len;
  while ((p_str_next != p_str)                                  // Srch str from end until beginning (see Note #2c) ...
         && (*p_str_next != srch_char)) {                       // ... until srch char found         (see Note #2d).
    p_str_next--;
  }

  if (*p_str_next != srch_char) {                               // If srch char NOT found, str points to NULL; ...
    return ((CPU_WCHAR *)0);                                    // ... rtn NULL (see Notes #2b & #2c).
  }

  return (p_str_next);                                          // Else rtn ptr to found srch char (see Note #2d).
}

/****************************************************************************************************//**
 *                                           WC_StrChar_Last_N()
 *
 * @brief    Search wide-character string for last occurrence of specific character.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to last occurrence of search character in string, if any.
 *           Pointer to NULL,                                          otherwise.
 *
 * @note     (1) String buffer NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) String pointer points to NULL.
 *                   - (1) String overlaps with NULL address.
 *                   - (2) String searched up to but NOT beyond or including the NULL address.
 *                   - (3) NULL address boundary handled in WC_StrLen_N().
 *               - (c) String searched from end to beginning.
 *                   - (1) Search character NOT found in search string; NULL pointer returned.
 *                   - (2) Applicable ONLY IF search character is NOT the terminating NULL character.
 *               - (d) Search character found.
 *                   - (1) Return pointer to first occurrence of search character in search string.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrChar_Last_N(CPU_WCHAR  *p_str,
                             CPU_WCHAR  srch_char,
                             CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p_str_next;
  CPU_SIZE_T str_len;

  if (p_str == (CPU_WCHAR *)0) {                                // Rtn NULL if srch str ptr NULL (see Note #2a).
    return ((CPU_WCHAR *)0);
  }

  p_str_next = p_str;
  str_len = WC_StrLen_N(p_str, len_max);
  p_str_next += str_len;
  while ((p_str_next != p_str)                                  // Srch str from end until beginning (see Note #2c) ...
         && (*p_str_next != srch_char)) {                       // ... until srch char found         (see Note #2d).
    p_str_next--;
  }

  if (*p_str_next != srch_char) {                               // If srch char NOT found, str points to NULL; ...
    return ((CPU_WCHAR *)0);                                    // ... rtn NULL (see Notes #2b & #2c).
  }

  return (p_str_next);                                          // Else rtn ptr to found srch char (see Note #2d).
}

/****************************************************************************************************//**
 *                                               WC_StrStr()
 *
 * @brief    Search wide-character string for first occurrence of a specific search wide-character string.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    p_srch_str  Pointer to search wide-character string (see Note #1).
 *
 * @return   Pointer to first occurrence of search string in string, if any.
 *           Pointer to NULL,                                        otherwise.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) Search string length greater than string length.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (c) Search string length equal to zero.
 *                   - (1) NULL search string at end of string returned.
 *               - (d) Entire string has been searched.
 *                   - (1) Maximum size of the search is defined as the subtraction of the
 *                           search string length from the string length.
 *                   - (2) Search string not found; NULL pointer returned.
 *               - (e) Search string found.
 *                   - (1) Search string found according to Str_Cmp_N() return value.
 *                   - (2) Return pointer to first occurrence of search string in string.
 *******************************************************************************************************/
CPU_WCHAR *WC_StrStr(CPU_WCHAR *p_str,
                     CPU_WCHAR *p_srch_str)
{
  CPU_SIZE_T  str_len;
  CPU_SIZE_T  srch_str_len;
  CPU_SIZE_T  srch_len;
  CPU_SIZE_T  srch_ix;
  CPU_BOOLEAN srch_done;
  CPU_INT32S  srch_cmp;
  CPU_WCHAR   *p_str_srch_ix;

  //                                                               Rtn NULL if str ptr(s) NULL (see Note #2a).
  if (p_str == (CPU_WCHAR *)0) {
    return ((CPU_WCHAR *)0);
  }
  if (p_srch_str == (CPU_WCHAR *)0) {
    return ((CPU_WCHAR *)0);
  }

  str_len = WC_StrLen(p_str);
  srch_str_len = WC_StrLen(p_srch_str);
  if (srch_str_len > str_len) {                                 // If srch str len > str len, rtn NULL  (see Note #2b).
    return ((CPU_WCHAR *)0);
  }
  if (srch_str_len == 0u) {                                     // If srch str len = 0, srch str equal NULL str; ...
    p_str_srch_ix = p_str + str_len;                            // ... rtn ptr to NULL str found in str (see Note #2c).
    return (p_str_srch_ix);
  }

  srch_len = str_len - srch_str_len;                            // Determine srch len (see Note #2d1).
  srch_ix = 0u;
  srch_done = DEF_NO;
  do {
    p_str_srch_ix = p_str + srch_ix;
    srch_cmp = WC_StrCmp_N(p_str_srch_ix, p_srch_str, srch_str_len);
    srch_done = (srch_cmp == 0) ? DEF_YES : DEF_NO;
    srch_ix++;
  } while ((srch_done == DEF_NO) && (srch_ix <= srch_len));

  if (srch_cmp != 0) {                                          // If srch str NOT found, rtn NULL  (see Note #2d).
    return ((CPU_WCHAR *)0);
  }

  return (p_str_srch_ix);                                       // Rtn ptr to srch str found in str (see Note #2e).
}

/****************************************************************************************************//**
 *                                               WC_StrStr_N()
 *
 * @brief    Search wide-character string for first occurrence of a specific search wide-character string.
 *
 * @param    p_str       Pointer to wide-character string (see Note #1).
 *
 * @param    p_srch_str  Pointer to search wide-character string (see Note #1).
 *
 * @param    len_max     Maximum number of characters to search (see Note #2f).
 *
 * @return   Pointer to first occurrence of search string in string, if any.
 *           Pointer to NULL,                                        otherwise.
 *
 * @note     (1) String buffers NOT modified.
 *
 * @note     (2) String search terminates when :
 *               - (a) String pointer passed a NULL pointer.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (b) Search string length greater than string length.
 *                   - (1) No string search performed; NULL pointer returned.
 *               - (c) Search string length equal to zero.
 *                   - (1) NULL search string at end of string returned.
 *               - (d) Entire string has been searched.
 *                   - (1) Maximum size of the search is defined as the subtraction of the
 *                           search string length from the string length.
 *                   - (2) Search string not found; NULL pointer returned.
 *               - (e) Search string found.
 *                   - (1) Search string found according to Str_Cmp_N() return value.
 *                   - (2) Return pointer to first occurrence of search string in string.
 *               - (f) 'len_max' number of characters searched.
 *                   - (1) 'len_max' number of characters does NOT include terminating NULL character
 *                           (see Note #2a2).
 *******************************************************************************************************/
CPU_WCHAR *WC_StrStr_N(CPU_WCHAR  *p_str,
                       CPU_WCHAR  *p_srch_str,
                       CPU_SIZE_T len_max)
{
  CPU_SIZE_T  str_len;
  CPU_SIZE_T  srch_str_len;
  CPU_SIZE_T  len_max_srch;
  CPU_SIZE_T  srch_len;
  CPU_SIZE_T  srch_ix;
  CPU_BOOLEAN srch_done;
  CPU_INT32S  srch_cmp;
  CPU_WCHAR   *p_str_srch_ix;

  //                                                               Rtn NULL if str ptr(s) NULL (see Note #2a).
  if (p_str == (CPU_WCHAR *)0) {
    return ((CPU_WCHAR *)0);
  }
  if (p_srch_str == (CPU_WCHAR *)0) {
    return ((CPU_WCHAR *)0);
  }

  if (len_max == (CPU_SIZE_T)0u) {                              // Rtn NULL if srch len = 0    (see Note #2f).
    return ((CPU_WCHAR *)0);
  }

  //                                                               Lim max srch str len (to chk > str len).
  len_max_srch = (len_max < DEF_INT_CPU_U_MAX_VAL)
                 ? (len_max + 1u) : DEF_INT_CPU_U_MAX_VAL;

  str_len = WC_StrLen_N(p_str, len_max);
  srch_str_len = WC_StrLen_N(p_srch_str, len_max_srch);
  if (srch_str_len > str_len) {                                 // If srch str len > str len, rtn NULL  (see Note #2b).
    return ((CPU_WCHAR *)0);
  }
  if (srch_str_len == 0u) {                                     // If srch str len = 0, srch str equal NULL str; ...
    p_str_srch_ix = p_str + str_len;                            // ... rtn ptr to NULL str found in str (see Note #2c).
    return (p_str_srch_ix);
  }

  srch_len = str_len - srch_str_len;                            // Determine srch len (see Note #2d1).
  srch_ix = 0u;
  srch_done = DEF_NO;
  do {
    p_str_srch_ix = p_str + srch_ix;
    srch_cmp = WC_StrCmp_N(p_str_srch_ix, p_srch_str, srch_str_len);
    srch_done = (srch_cmp == 0) ? DEF_YES : DEF_NO;
    srch_ix++;
  } while ((srch_done == DEF_NO) && (srch_ix <= srch_len));

  if (srch_cmp != 0) {                                          // If srch str NOT found, rtn NULL  (see Note #2d).
    return ((CPU_WCHAR *)0);
  }

  return (p_str_srch_ix);                                       // Rtn ptr to srch str found in str (see Note #2e).
}

/****************************************************************************************************//**
 *                                               WC_CharIsAlpha()
 *
 * @brief    Determine whether a character is an alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     an alphabetic character.
 *           DEF_NO,  if character is NOT an alphabetic character.
 *
 * @note     (1) Each character SHOULD fit into a range in the table; in case the table or algorithm
 *               is broken, the failure to find the range is handled.
 *               - (a) Table entries are sized for 16-bit characters only.  Characters belonging to the
 *                       upper code point space must be handled separately.
 *
 * @note     (2) To accelerate character mapping operations, a binary search is used to find the
 *               character range.  The table has approximately 750 ranges, so the loop should
 *               execute at most 10 times.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsAlpha(CPU_WCHAR c)
{
  CPU_BOOLEAN alpha;
  CPU_SIZE_T  ix_lo;
  CPU_SIZE_T  ix_mid = 0u;
  CPU_SIZE_T  ix_hi;
  CPU_WCHAR   c_start;
  CPU_WCHAR   c_end;
  CPU_BOOLEAN found;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    alpha = ASCII_IS_ALPHA((CPU_CHAR)c);                        // ... determine whether char is ASCII alpha char.
  } else {
#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
    if (c >= DEF_INT_16U_MAX_VAL) {                             // If char is hi Unicode char (see Note #1a) ...
      alpha = DEF_NO;                                           // ... determine whether alpha char.
      return (alpha);
    }
#endif

    found = DEF_NO;
    ix_lo = 0u;
    ix_hi = (sizeof(WC_CharIsAlphaTbl) / sizeof(CPU_WCHAR)) - 2u;

    while ((ix_lo <= ix_hi)                                     // While lo srch ix & hi srch ix valid ...
           && (found == DEF_NO)) {                              // ... & range not found               ...
                                                                // ... bisect range tbl (see Note #2).

      ix_mid = (ix_lo + ix_hi) / 2u;
      c_start = WC_CharIsAlphaTbl[ix_mid];
      c_end = WC_CharIsAlphaTbl[ix_mid + 1u];

      if (c < c_start) {                                        // If char in lower half of range tbl ...
        ix_hi = ix_mid - 1u;                                    // ... make midpoint hi val.
      } else if (c >= c_end) {                                  // If char in upper half of range tbl ...
        ix_lo = ix_mid + 1u;                                    // ... make midpoint lo val.
      } else {                                                  // Else range found.
        found = DEF_YES;
      }
    }

    if (found == DEF_NO) {                                      // If range NOT found (see Note #1) ...
      alpha = DEF_NO;                                           // ... rtn NULL.
    } else {
      alpha = ((ix_mid % 2u) == 0u) ? DEF_NO : DEF_YES;
    }
  }

  return (alpha);
}

/****************************************************************************************************//**
 *                                           WC_CharIsAlphaNum()
 *
 * @brief    Determine whether a character is an alphanumeric character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     an alphanumeric character.
 *           DEF_NO,  if character is NOT an alphanumeric character.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsAlphaNum(CPU_WCHAR c)
{
  CPU_BOOLEAN alpha_num;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    alpha_num = ASCII_IS_ALPHA_NUM((CPU_CHAR)c);                // ... determine whether char is ASCII alpha num char.
  } else {
    alpha_num = WC_CharIsDig(c);
    if (alpha_num == DEF_NO) {                                  // If char is NOT dig ...
      alpha_num = WC_CharIsAlpha(c);                            // ... test if char is alpha char.
    }
  }

  return (alpha_num);
}

/****************************************************************************************************//**
 *                                               WC_CharIsLower()
 *
 * @brief    Determine whether a character is a lowercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     a lowercase alphabetic character.
 *           DEF_NO,  if character is NOT a lowercase alphabetic character.
 *
 * @note     (1) In general, the lowercase alphabetic characters are the alphabetic characters that
 *               are preserved when converted to lowercase but changed when converted to uppercase.
 *               Several exceptions must be hard-coded for lowercase characters with no uppercase
 *               equivalent :
 *               - (a) LATIN-1 SUPPLEMENT.  U+00AA, U+00BA, U+00DF
 *               - (b) LATIN EXTENDED-A. U+0138, U+0149
 *               - (c) LATIN EXTENDED-B. U+018D, U+01AA..01AB, U+01BA, U+01BE, U+01F0, U+0221,
 *                       U+0234..0239, U+023F..0240
 *               - (d) IPA EXTENSIONS.  U+0250..0293, U+0295....02B8, U+02C0..02C1, U+02E0..02E4
 *               - (e) GREEK AND CYRILLIC. U+037A, U+0390, U+03B0, U+03F3, U+03FC
 *               - (f) ARMENIAN. U+0587
 *               - (g) PHONETIC EXTENSIONS. U+1D00..1D7F
 *               - (h) PHONETIC EXTENSIONS SUPPLEMENT. U+1D80..1DBF
 *               - (i) LATIN EXTENDED ADDITIONAL. U+1E96..1E9D, U+1E9F
 *               - (j) GREEK EXTENDED. U+1F50..1F57, U+1FB2, U+1FB4, U+1FB6..1FB7, U+1FC2..1FC4,
 *                       U+1FD2..1FD3, U+1FD6..1FD7, U+1FE0..1FE7, U+1FF2..1FF4, U+1FF6..1FF7
 *               - (k) SUPERSCRIPTS AND SUBSCRIPTS. U+2071, U+207F, U+2090..2094
 *               - (l) LETTERLIKE SYMBOLS. U+210A, U+210E..210F, U+2113, U+212F, U+2134, U+2139,
 *                       U+213C..213D, U+2146..2149
 *               - (m) LATIN EXTENDED-C. U+2C74, U+2C77..2C7D
 *               - (n) COPTIC. U+2CE4
 *               - (o) LATIN EXTENDED-D. U+A730..A731, U+A770..A778
 *               - (p) ALPHABETIC PRESENTATION FORMS. U+FB00..FB06, U+FB13..FB17
 *               - (q) HALFWIDTH AND FULLWIDTH FORMS. U+FF41..FF4F
 *               - (r) SMALL FORM VARIANTS. U+FF50..FF5A
 *               - (s) To accelerate this character mapping operation, a table & binary search are used
 *                     to detect exceptional cases.  The table has approximately 120 ranges, so the loop
 *                     should execute at most 7 times.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsLower(CPU_WCHAR c)
{
  CPU_BOOLEAN lower;
  CPU_BOOLEAN alpha;
  CPU_WCHAR   c_lower;
  CPU_WCHAR   c_upper;
  CPU_SIZE_T  ix_lo;
  CPU_SIZE_T  ix_mid = 0u;
  CPU_SIZE_T  ix_hi;
  CPU_WCHAR   c_start;
  CPU_WCHAR   c_end;
  CPU_BOOLEAN found;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    lower = ASCII_IS_LOWER((CPU_CHAR)c);                        // ... determine whether char is ASCII lower char.
  } else {
    alpha = WC_CharIsAlpha(c);
    if (alpha == DEF_NO) {                                      // If  char is NOT alpha char ...
      lower = DEF_NO;                                           // ... char is NOT lower char.
    } else {
      c_lower = WC_CharToLower(c);
      if (c_lower != c) {                                       // If char to lower char is NOT char ...
        lower = DEF_NO;                                         // ... char is NOT lower char.
      } else {
        c_upper = WC_CharToUpper(c);
        if (c_upper != c) {                                     // If char to upper char is NOT char ...
          lower = DEF_YES;                                      // ... char is    lower char.
        } else {                                                // See Note #1s.
          found = DEF_NO;
          ix_lo = 0u;
          ix_hi = (sizeof(WC_CharIsLowerTbl) / sizeof(CPU_WCHAR)) - 2u;

          while ((ix_lo <= ix_hi)                               // While lo srch ix & hi srch ix valid ...
                 && (found == DEF_NO)) {                        // ... & range not found               ...
                                                                // ... bisect range tbl.

            ix_mid = (ix_lo + ix_hi) / 2u;
            c_start = WC_CharIsLowerTbl[ix_mid];
            c_end = WC_CharIsLowerTbl[ix_mid + 1u];

            if (c < c_start) {                                  // If char in lower half of range tbl ...
              ix_hi = ix_mid - 1u;                              // ... make midpoint hi val.
            } else if (c >= c_end) {                            // If char in upper half of range tbl ...
              ix_lo = ix_mid + 1u;                              // ... make midpoint lo val.
            } else {                                            // Else range found.
              found = DEF_YES;
            }
          }

          if (found == DEF_NO) {                                // If range NOT found ...
            lower = DEF_NO;                                     // ... rtn NULL.
          } else {
            lower = ((ix_mid % 2u) == 0u) ? DEF_NO : DEF_YES;
          }
        }
      }
    }
  }

  return (lower);
}

/****************************************************************************************************//**
 *                                               WC_CharIsUpper()
 *
 * @brief    Determine whether a character is an uppercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     an uppercase alphabetic character.
 *           DEF_NO,  if character is NOT an uppercase alphabetic character.
 *
 * @note     (1) In general, the uppercase alphabetic characters are the alphabetic characters that
 *               are preserved when converted to uppercase but changed when converted to lowercase.
 *               Several exceptions must be hard-coded for uppercase characters with no lowercase
 *               equivalent :
 *               - (a) LETTERLIKE SYMBOLS. U+2102, U+2107, U+210B..210D, U+2110..2112, U+2115,
 *                       U+2119..211D, U+2124, U+2128, U+212C..U+212D, U+2130..2133, U+213E-213F,
 *                       U+2145
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsUpper(CPU_WCHAR c)
{
  CPU_BOOLEAN upper;
  CPU_BOOLEAN alpha;
  CPU_WCHAR   c_lower;
  CPU_WCHAR   c_upper;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    upper = ASCII_IS_UPPER((CPU_CHAR)c);                        // ... determine whether char is ASCII upper char.
  } else {
    alpha = WC_CharIsAlpha(c);
    if (alpha == DEF_NO) {                                      // If  char is NOT alpha char ...
      upper = DEF_NO;                                           // ... char is NOT upper char.
    } else {
      c_upper = WC_CharToUpper(c);
      if (c_upper != c) {                                       // If char to upper char is NOT char ...
        upper = DEF_NO;                                         // ... char is NOT upper char.
      } else {
        c_lower = WC_CharToLower(c);
        if (c_lower != c) {                                     // If char to lower char is NOT char ...
          upper = DEF_YES;                                      // ... char is    upper char.
        } else {
          if (c <= 0x20FFu) {
            upper = DEF_NO;
          } else if (c <= 0x214Fu) {
            upper = ( (c == 0x2102u) || (c == 0x2107u)  || ((c >= 0x210Bu) && (c <= 0x210Du))
                      || ((c >= 0x2110u) && (c <= 0x2112u)) ||  (c == 0x2115u)
                      || ((c >= 0x2119u) && (c <= 0x211Du)) ||  (c == 0x2124u) || (c == 0x2128u)
                      || ((c >= 0x212Cu) && (c <= 0x212Du)) || ((c >= 0x2130u) && (c <= 0x2133u))
                      || ((c >= 0x213Eu) && (c <= 0x213Fu)) ||  (c == 0x2145u)                  ) ? DEF_YES : DEF_NO;
          } else {
            upper = DEF_NO;
          }
        }
      }
    }
  }

  return (upper);
}

/****************************************************************************************************//**
 *                                               WC_CharIsDig()
 *
 * @brief    Determine whether a character is a decimal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     a decimal-digit character.
 *           DEF_NO,  if character is NOT a decimal-digit character.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsDig(CPU_WCHAR c)
{
  CPU_BOOLEAN dig;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    dig = ASCII_IS_DIG((CPU_CHAR)c);                            // ... determine whether char is ASCII dig.
  } else {
    dig = (((c) >= WC_CHAR_FULLWIDTH_DIG_ZERO) && ((c) <= WC_CHAR_FULLWIDTH_DIG_NINE)) ? (DEF_YES) : (DEF_NO);
  }

  return (dig);
}

/****************************************************************************************************//**
 *                                               WC_CharIsDigOct()
 *
 * @brief    Determine whether a character is an octal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     an octal-digit character.
 *           DEF_NO,  if character is NOT an octal-digit character.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsDigOct(CPU_WCHAR c)
{
  CPU_BOOLEAN dig_oct;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    dig_oct = ASCII_IS_DIG_OCT((CPU_CHAR)c);                    // ... determine whether char is ASCII oct dig.
  } else {
    dig_oct = (((c) >= WC_CHAR_FULLWIDTH_DIG_ZERO) && ((c) <= WC_CHAR_FULLWIDTH_DIG_SEVEN)) ? (DEF_YES) : (DEF_NO);
  }

  return (dig_oct);
}

/****************************************************************************************************//**
 *                                               WC_CharIsDigHex()
 *
 * @brief    Determine whether a character is an hexadecimal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     an hexadecimal-digit character.
 *           DEF_NO,  if character is NOT an hexadecimal-digit character.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsDigHex(CPU_WCHAR c)
{
  CPU_BOOLEAN dig_hex;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    dig_hex = ASCII_IS_DIG_HEX((CPU_CHAR)c);                    // ... determine whether char is ASCII hex dig.
  } else {
    dig_hex = ((((c) >= WC_CHAR_FULLWIDTH_DIG_ZERO) && ((c) <= WC_CHAR_FULLWIDTH_DIG_NINE))
               || (((c) >= WC_CHAR_FULLWIDTH_LATIN_UPPER_A) && ((c) <= WC_CHAR_FULLWIDTH_LATIN_UPPER_F))
               || (((c) >= WC_CHAR_FULLWIDTH_LATIN_LOWER_A) && ((c) <= WC_CHAR_FULLWIDTH_LATIN_LOWER_F))) ? (DEF_YES) : (DEF_NO);
  }

  return (dig_hex);
}

/****************************************************************************************************//**
 *                                               WC_CharIsSpace()
 *
 * @brief    Determine whether a character is a white-space character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, if character is     a white-space character.
 *           DEF_NO,  if character is NOT a white-space character.
 *******************************************************************************************************/
CPU_BOOLEAN WC_CharIsSpace(CPU_WCHAR c)
{
  CPU_BOOLEAN space;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char ...
    space = ASCII_IS_SPACE((CPU_CHAR)c);                        // ... determine whether char is ASCII whitespace.
  } else {
    space = (((c == WC_CHAR_NO_BREAK_SPACE)  || (c == WC_CHAR_OGHAM_SPACE_MARK)
              || (c == WC_CHAR_MONGOLIAN_VOWEL_SEPARATOR)  || (c == WC_CHAR_LINE_SEPARATOR)
              || (c == WC_CHAR_PARAGRAPH_SEPARATOR)  || (c == WC_CHAR_PARAGRAPH_SEPARATOR)
              || (c == WC_CHAR_NARROW_NO_BREAK_SPACE)  || (c == WC_CHAR_MEDIUM_MATHEMATICAL_SPACE)
              || (c == WC_CHAR_IDEOGRAPHIC_SPACE))
             || ((c >= WC_CHAR_EN_QUAD)  && (c <= WC_CHAR_HAIR_SPACE))) ? (DEF_YES) : (DEF_NO);
  }

  return (space);
}

/****************************************************************************************************//**
 *                                               WC_CharToLower()
 *
 * @brief    Convert uppercase alphabetic character to its corresponding lowercase alphabetic character.
 *
 * @param    c   Character to convert.
 *
 * @return   Lowercase equivalent of 'c', if character 'c' is a uppercase character.
 *           Character 'c',               otherwise.
 *
 * @note     (1) The Unicode character set can be divided into ranges of characters that map to
 *               characters the same character code 'distance' away.  For example, each uppercase
 *               Latin character (code points 0x41-0x5A) maps to a lowercase Latin character 0x20 code
 *               points away (0x61-0x7A).
 *               - (a) The Unicode character set contains ranges of alternating upper & equivalent
 *                     lower case characters.  To optimize the case mapping algorithm, these ranges
 *                     are handled specially by placing WC_DIFF_ALT as the distance in the case
 *                     mapping table.
 *               - (b) Each character SHOULD fit into a range in 'WC_CharToLowerTbl[]'; in case the
 *                     table or algorithm is broken, upon failure the character 'c' is returned.
 *               - (c) Case mapping is accelerated for the ASCII character set, to which most characters
 *                     are expected to belong.
 *******************************************************************************************************/
CPU_WCHAR WC_CharToLower(CPU_WCHAR c)
{
  CPU_CHAR                lower_ascii;
  CPU_WCHAR               lower;
  CPU_INT32S              diff;
  CPU_WCHAR               c_start;
  CPU_BOOLEAN             c_start_even;
  CPU_BOOLEAN             c_even;
  const WC_SRCH_TBL_ENTRY *p_tbl_entry;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char (see Note #1c) ...
    lower_ascii = ASCII_TO_LOWER((CPU_CHAR)c);
    lower = (CPU_WCHAR)lower_ascii;                             // ... convert to lower case.
    return (lower);
  }

#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  if (c >= DEF_INT_16U_MAX_VAL) {                               // If char is hi Unicode char ...
    lower = c;                                                  // ... rtn original char.
    return (lower);
  }
#endif

  //                                                               If char is Unicode char ...
  //                                                               ... srch for range.
  p_tbl_entry = WC_SrchTbl(&WC_CharToLowerTbl[0],
                           (sizeof(WC_CharToLowerTbl) / sizeof(WC_SRCH_TBL_ENTRY)),
                           c);

  if (p_tbl_entry == (WC_SRCH_TBL_ENTRY *)0) {                  // If range NOT found (see Note #1b) ...
    lower = c;                                                  // ... rtn original char.
    return (lower);
  }

  //                                                               Calc case-folded char.
  diff = p_tbl_entry->Diff;
  if (diff == WC_DIFF_35332) {
    diff = -35332;
  }

  if (diff == WC_DIFF_ALT) {                                    // Alternating range (see Note #1a).
    c_start = p_tbl_entry->Char;
    c_start_even = ((c_start % 2u) == 0u)    ? DEF_YES  : DEF_NO;
    c_even = ((c       % 2u) == 0u)    ? DEF_YES  : DEF_NO;

    lower = (c_start_even == c_even) ? (c + 1u) : (c);
  } else {                                                      // Normal or case-insensitive range.
    lower = c + (CPU_WCHAR)diff;
  }

  return (lower);
}

/****************************************************************************************************//**
 *                                               WC_CharToUpper()
 *
 * @brief    Convert lowercase alphabetic character to its corresponding uppercase alphabetic character.
 *
 * @param    c   Character to convert.
 *
 * @return   Uppercase equivalent of 'c', if character 'c' is a lowercase character.
 *           Character 'c',               otherwise.
 *
 * @note     (1) The Unicode character set can be divided into ranges of characters that map to
 *               characters the same character code 'distance' away.  For example, each lowercase
 *               Latin character (code points 0x61-0x7A) maps to an uppercase Latin character 0x20
 *               code points away (0x41-0x5A).
 *               - (a) The Unicode character set contains ranges of alternating upper & equivalent
 *                     lower case characters.  To optimize the case mapping algorithm, these ranges
 *                     are handled specially by placing WC_DIFF_ALT as the distance in the case mapping
 *                     table, upon failure the character 'c' is returned.
 *               - (b) Each character SHOULD fit into a range in 'WC_CharToUpperTbl[]'; in case the
 *                     table original algorithm is broken, the failure to find the range is handled.
 *               - (c) Case mapping is accelerated for the ASCII character set, to which most characters
 *                     are expected to belong.
 *******************************************************************************************************/
CPU_WCHAR WC_CharToUpper(CPU_WCHAR c)
{
  CPU_CHAR                upper_ascii;
  CPU_WCHAR               upper;
  CPU_INT32S              diff;
  CPU_WCHAR               c_start;
  CPU_BOOLEAN             c_start_even;
  CPU_BOOLEAN             c_even;
  const WC_SRCH_TBL_ENTRY *p_tbl_entry;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char (see Note #1c) ...
    upper_ascii = ASCII_TO_UPPER((CPU_CHAR)c);
    upper = (CPU_WCHAR)upper_ascii;                             // ... convert to upper case.
    return (upper);
  }

#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  if (c >= DEF_INT_16U_MAX_VAL) {                               // If char is hi Unicode char ...
    upper = c;                                                  // ... rtn original char.
    return (upper);
  }
#endif

  //                                                               If char is Unicode char ...
  //                                                               ... srch for range.
  p_tbl_entry = WC_SrchTbl(&WC_CharToUpperTbl[0],
                           (sizeof(WC_CharToUpperTbl) / sizeof(WC_SRCH_TBL_ENTRY)),
                           c);

  if (p_tbl_entry == (WC_SRCH_TBL_ENTRY *)0) {                  // If range NOT found (see Note #1b) ...
    upper = c;                                                  // ... rtn original char.
    return (upper);
  }

  //                                                               Calc case-folded char.
  diff = p_tbl_entry->Diff;
  if (diff == WC_DIFF_35332) {
    diff = 35332;
  }

  if (diff == WC_DIFF_ALT) {                                    // Alternating range (see Note #1a).
    c_start = p_tbl_entry->Char;
    c_start_even = ((c_start % 2u) == 0u)    ? DEF_YES  : DEF_NO;
    c_even = ((c       % 2u) == 0u)    ? DEF_YES  : DEF_NO;

    upper = (c_start_even == c_even) ? (c - 1u) : (c);
  } else {                                                      // Normal or case-insensitive range.
    upper = c + (CPU_WCHAR)diff;
  }

  return (upper);
}

/****************************************************************************************************//**
 *                                           WC_CharToCasefold()
 *
 * @brief    Case-fold character.
 *
 * @param    c   Character to case fold.
 *
 * @return   Case-folded character.
 *
 * @note     (1) Case-folding maps characters for case-insensitive string comparison.  According to
 *               the Unicode Standard 5.0, Section 3.13, D129, "A string X is a caseless match for a
 *               string Y if and only if: toCasefold(X) = toCasefold(Y)".  The case-foldings in the
 *               Unicode Character Database provided in the supplementary file, 'CaseFolding.txt', are
 *               followed in this implementation.
 *           - (a) 'CaseFolding.txt' includes common, full, simple & Turkic case-foldings.  The full
 *                 (which map single characters to multiple characters) & Turkic (which apply only
 *                 to Turkic languages) are NOT followed.
 *               - (1) If the lowercase equivalent of 'c' is different than 'c', then it is also
 *                     the case-folded equivalent of 'c'.  Case-folding need only handle the few
 *                     exceptions.
 *           - (b) The Unicode character set can be divided into ranges of characters that map to
 *                   characters the same character code 'distance' away.  For example, each uppercase
 *                   Latin character (code points 0x41-0x5A) maps to a lowercase Latin character
 *                   0x20 code points away (0x61-0x7A).
 *               - (1) The Unicode character set contains ranges of alternating upper & equivalent
 *                     lower case characters, the former mapping to the latter in case folding.  To
 *                     optimize the case folding algorithm, these ranges are handled specially by
 *                     placing WC_DIFF_ALT as the distance in the case folding table.
 *               - (2) Each character SHOULD fit into a range in 'WC_CharToCasefoldTbl[]'; in case
 *                     the table or algorithm is broken, upon failure the character 'c' is returned.
 *               - (3) Case folding is accelerated for the ASCII character set, to which most
 *                     characters are expected to belong.
 *******************************************************************************************************/
CPU_WCHAR WC_CharToCasefold(CPU_WCHAR c)
{
  CPU_WCHAR               folded;
  CPU_CHAR                lower_ascii;
  CPU_WCHAR               lower;
  CPU_INT32S              diff;
  CPU_WCHAR               c_start;
  CPU_BOOLEAN             c_start_even;
  CPU_BOOLEAN             c_even;
  const WC_SRCH_TBL_ENTRY *p_tbl_entry;

  if (c <= (CPU_WCHAR)ASCII_CHAR_DELETE) {                      // If char is ASCII char (see Note #1b3) ...
    lower_ascii = ASCII_TO_LOWER((CPU_CHAR)c);
    folded = (CPU_WCHAR)lower_ascii;                            // ... convert to lower case.
    return (folded);
  }

#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  if (c >= DEF_INT_16U_MAX_VAL) {                               // If char is hi Unicode char ...
    folded = c;                                                 // ... rtn original char.
    return (folded);
  }
#endif

  lower = WC_CharToLower(c);
  if (lower != c) {                                             // If char to lower is NOT char (see Note #1a1) ...
    folded = lower;                                             // ... folded is lower.
    return (folded);
  }

  //                                                               If char is Unicode char ...
  //                                                               ... srch for range.
  p_tbl_entry = WC_SrchTbl(&WC_CharToCasefoldTbl[0],
                           (sizeof(WC_CharToCasefoldTbl) / sizeof(WC_SRCH_TBL_ENTRY)),
                           c);

  if (p_tbl_entry == (WC_SRCH_TBL_ENTRY *)0) {                  // If range NOT found (see Note #1b2) ...
    folded = c;                                                 // ... rtn original char.
    return (folded);
  }

  //                                                               Calc case-folded char.
  diff = p_tbl_entry->Diff;

  if (diff == WC_DIFF_ALT) {                                    // Alternating range (see Note #1b1).
    c_start = p_tbl_entry->Char;
    c_start_even = ((c_start % 2u) == 0u)    ? DEF_YES : DEF_NO;
    c_even = ((c       % 2u) == 0u)    ? DEF_YES : DEF_NO;

    folded = (c_start_even == c_even) ? (c + 1u) : (c);
  } else {                                                      // Normal or case-insensitive range.
    folded = c + (CPU_WCHAR)diff;
  }

  return (folded);
}

/****************************************************************************************************//**
 *                                               WC_CharToMB()
 *
 * @brief    Convert wide-character to its corresponding multi-byte character.
 *
 * @param    p_c_dest    Pointer to buffer that will receive the multi-byte character.
 *
 * @param    c_src       Wide-character to convert.
 *
 * @return   Number of bytes stored in 'pc_dest', if character 'c_src' is a valid character.
 *
 *           (CPU_SIZE_T)-1,                      otherwise.
 *
 * @note     (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'wcrtomb() : DESCRIPTION' states that "the
 *                   wcrtomb() function shall determine the number of bytes needed to represent the
 *                   character that corresponds to the wide character given by wc ... and store the
 *                   resulting bytes in the array whose first element is pointed to by s".
 *
 *           - (b) IEEE Std 1003.1, 2004 Edition, Section 'wcrtomb() : RETURN VALUE' states that
 *                   "wcrtomb() shall return the number of bytes stored in the array object....  When
 *                   wc is not a valid wide character ... the function ... shall return (size_t)-1.".
 *
 * @note     (2) (a) Unicode Standard 5.0, Section 3.9, definition D92, defines the UTF-8 encoding
 *                   form. Table 3-6 gives the UTF-8 bit distribution for scalar values (the wide-
 *                   character value) :
 *
 *                   --------------------------------------------------------------------------
 *                   |        SCALAR VALUE        |                UTF-8 FORM                 |
 *                   |                            |  BYTE 1  |  BYTE 2  |  BYTE 3  |  BYTE 4  |
 *                   --------------------------------------------------------------------------
 *                   |          00000000 0xxxxxxx | 0xxxxxxx |          |          |          |
 *                   |          00000yyy yyxxxxxx | 110yyyyy | 10xxxxxx |          |          |
 *                   |          zzzzyyyy yyxxxxxx | 1110zzzz | 10yyyyyy | 10xxxxxx |          |
 *                   | 000uuuuu zzzzyyyy yyxxxxxx | 11110uuu | 10uuzzzz | 10yyyyyy | 10xxxxxx |
 *                   --------------------------------------------------------------------------
 *
 *           - (b) Unicode Standard 5.0, Section 3.9, definitions D99, D100 & D101, define three
 *                   schemes for determining the endianness of the wide-character (UTF-32) encoding.
 *                   The first two, UTF-32BE & UTF-32LE, define big- & little-endian schemes :
 *
 *                       UTF-32BE
 *                           Code Unit    :  00010302
 *                           Byte Sequence:  00 01 03 02
 *
 *                       UTF-32LE
 *                           Code Unit    :  00010302
 *                           Byte Sequence:  02 03 01 00
 *
 *                   The last, D101, uses a byte order marker (BOM) to "distinguish between the two
 *                   byte orders"; "[h]owever, when there is no BOM, and in the absence of a higher-level
 *                   protocol, the byte order of the UTF-32 encoding scheme is big-endian".
 *
 *                       UTF-32
 *                           Code Unit       :  00010302
 *                           Byte Sequence(s):  00 01 03 02
 *                                               00 00 FE FF 00 01 03 02
 *                                               FF FE 00 00 02 03 01 00
 *
 *               - (1) The byte-order of the wide-character (UTF-32) stream is assumed to be big-
 *                       endian.  On little-endian CPUs, the byte-order of the source character MUST
 *                       be flipped prior to processing.
 *******************************************************************************************************/
CPU_SIZE_T WC_CharToMB(CPU_CHAR  *p_c_dest,
                       CPU_WCHAR c_src)
{
  CPU_INT08U byte1;
  CPU_INT08U byte2;
  CPU_INT08U byte3;
#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  CPU_INT08U byte4;
#endif

  if (p_c_dest == (CPU_CHAR *)0) {
    return ((CPU_SIZE_T)0);
  }

#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  if (c_src > MB_MAX_SCALAR_VAL_4BYTE) {
    return ((CPU_SIZE_T)-1);
  }

  if (c_src > MB_MAX_SCALAR_VAL_3BYTE) {                        // ---------------- 4-BYTE UTF-8 CHAR -----------------
    byte4 = (CPU_INT08U)(c_src & 0x3F) | DEF_BIT_07;
    c_src >>= 6;
    byte3 = (CPU_INT08U)(c_src & 0x3F) | DEF_BIT_07;
    c_src >>= 6;
    byte2 = (CPU_INT08U)(c_src & 0x3F) | DEF_BIT_07;
    c_src >>= 6;
    byte1 = (CPU_INT08U)(c_src & 0x07) | 0xF0;

    *p_c_dest = (CPU_CHAR)byte1;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte2;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte3;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte4;

    return ((CPU_SIZE_T)4);
  }
#endif

  if (c_src > MB_MAX_SCALAR_VAL_2BYTE) {                        // ---------------- 3-BYTE UTF-8 CHAR -----------------
    byte3 = (CPU_INT08U)(c_src & 0x3Fu) | DEF_BIT_07;
    c_src >>= 6;
    byte2 = (CPU_INT08U)(c_src & 0x3Fu) | DEF_BIT_07;
    c_src >>= 6;
    byte1 = (CPU_INT08U)(c_src & 0x0Fu) | 0xE0u;

    *p_c_dest = (CPU_CHAR)byte1;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte2;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte3;

    return ((CPU_SIZE_T)3);
  }

  if (c_src > MB_MAX_SCALAR_VAL_1BYTE) {                        // ---------------- 2-BYTE UTF-8 CHAR -----------------
    byte2 = (CPU_INT08U)(c_src & 0x3Fu) | DEF_BIT_07;
    c_src >>= 6;
    byte1 = (CPU_INT08U)(c_src & 0x1Fu) | 0xC0u;

    *p_c_dest = (CPU_CHAR)byte1;
    p_c_dest++;
    *p_c_dest = (CPU_CHAR)byte2;

    return ((CPU_SIZE_T)2);
  }

  //                                                               ---------------- 1-BYTE UTF-8 CHAR -----------------
  byte1 = (CPU_INT08U)(c_src & 0x7Fu);
  *p_c_dest = (CPU_CHAR)byte1;
  return ((CPU_SIZE_T)1);
}

/****************************************************************************************************//**
 *                                               MB_CharToWC()
 *
 * @brief    Convert multi-byte character to its corresponding wide-character.
 *
 * @param    p_c_dest    Pointer to buffer that will receive the wide-character.
 *
 * @param    p_c_src     Pointer to buffer containing the multi-byte character to convert.
 *
 * @param    n           Number of bytes in 'pc_src' to inspect for multi-byte character.
 *
 * @return   Number of bytes used from 'pc_src' (between 1 & n), if 'pc_src' contains a valid non-NULL
 *                                                               character.
 *                       0,                                      if 'pc_src' contains a           NULL
 *                                                               character.
 *           CPU_SIZE_T)-2,                                      if the first 'n' bytes of 'pc_src'
 *                                                               contain an incomplete but potentially
 *                                                               valid character.
 *           CPU_SIZE_T)-1,                                      otherwise.
 *
 * @note     (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'mbrtowc() : DESCRIPTION' states that "the
 *                   mbrtowc() function shall inspect at most n bytes beginning at the byte pointed
 *                   to by s to determine the number of bytes needed to complete the next character ...."
 *                   If the function determines that the next character is completed, it shall determine
 *                   the value of the corresponding wide character and then, if pwc is not a null
 *                   pointer shall store that value in the object pointed to by pwc".
 *
 *           - (b) IEEE Std 1003.1, 2004 Edition, Section 'mbrtowc() : RETURN VALUE' states that
 *                   "mbrtowc() shall return the first of the following that applies :"
 *               - (1) "0                          If the next n or fewer bytes complete the character
 *                                                   that corresponds to the null wide character"
 *               - (2) "between 1 and n inclusive  If the next n or fewer bytes complete a valid
 *                                                   character (which is the value stored); the value
 *                                                   returned shall be the number of bytes that complete
 *                                                   the character"
 *               - (3) "(size_t)-2                 If the next n byte contribute to an incomplete
 *                                                   but potentially valid character"
 *               - (4) "(size_t)-1                 If an encoding error occurs"
 *
 * @note     (2) (a) See 'WC_CharToMB()  Note #2a'.
 *
 *           - (b) Unicode Standard 5.0, Section 3.9, Table 3-7, lists the well-formed UTF-8 byte
 *                   sequences.
 *
 *                   --------------------------------------------------------------------------
 *                   |        CODE POINTS         |                UTF-8 FORM                 |
 *                   |                            |  BYTE 1  |  BYTE 2  |  BYTE 3  |  BYTE 4  |
 *                   --------------------------------------------------------------------------
 *                   |    U+000000..U+00007F      |  00..7F  |          |          |          |
 *                   |    U+000080..U+0007FF      |  C2..DF  |  80..BF  |          |          |
 *                   |    U+000800..U+000FFF      |  E0      |  A0..BF  |  80..BF  |          |
 *                   |    U+001000..U+00CFFF      |  E1..EC  |  80..BF  |  80..BF  |          |
 *                   |    U+00D000..U+00D7FF      |  ED      |  80..9F  |  80..BF  |          |
 *                   |    U+00E000..U+00FFFF      |  EE..EF  |  80..BF  |  80..BF  |          |
 *                   |    U+010000..U+03FFFF      |  F0      |  90..BF  |  80..BF  |  80..BF  |
 *                   |    U+040000..U+0FFFFF      |  F1..F3  |  80..BF  |  80..BF  |  80..BF  |
 *                   |    U+100000..U+10FFFF      |  F4      |  80..8F  |  80..BF  |  80..BF  |
 *                   --------------------------------------------------------------------------
 *
 *               - (1) (A) Second, third & fourth byte values outside of 80..BF are disallowed
 *                           because, according to the encoding forms in Table 3-6, these are
 *                           impossible.  In certain more restrictive conditions apply.
 *
 *                   - (B) First byte values 80..BF & F8..FF are disallowed because, according to
 *                           the encoding forms in Table 3-6, these are impossible.
 *
 *                   - (C) First byte values C0..C1 are disallowed to avoid non-shortest form
 *                           representation of code points U+50..U+7F.
 *
 *                   - (D) First byte values F5..F7 are disallowed to prevent representation of
 *                           invalid code points U+140000 U+1FFFFF.
 *
 *                   - (E) If the first byte is E0, second byte values 80..9F are disallowed to
 *                           avoid non-shortest form representation of code points U+00..7FF.
 *
 *                   - (F) If the first byte is ED, second byte values A0..BF are disallowed to
 *                           prevent representation of invalid code points U+D800..U+DFFF.
 *
 *                   - (G) If the first byte is F0, second byte values 80..8F are disallowed to
 *                           avoid non-shortest form representation of code points U+0FFF..U+FFFF.
 *
 *                   - (H) If the first byte if F4, second byte values 90..BF are disallowed to
 *                           prevent representation of invalid code points U+11000..U+13FFFF.
 *
 *           - (c) Unicode Standard 5.0, Section 3.9, definitions D99, D100 & D101, define three
 *                   schemes for determining the endianness of the wide-character (UTF-32) encoding.
 *                   The first two, UTF-32BE & UTF-32LE, define big- & little-endian schemes :
 *
 *                       UTF-32BE
 *                           Code Unit    :  00010302
 *                           Byte Sequence:  00 01 03 02
 *
 *                       UTF-32LE
 *                           Code Unit    :  00010302
 *                           Byte Sequence:  02 03 01 00
 *
 *                   The last, D101, uses a byte order marker (BOM) to "distinguish between the two
 *                   byte orders"; "[h]owever, when there is no BOM, and in the absence of a higher-level
 *                   protocol, the byte order of the UTF-32 encoding scheme is big-endian".
 *
 *                       UTF-32
 *                           Code Unit       :  00010302
 *                           Byte Sequence(s):  00 01 03 02
 *                                               00 00 FE FF 00 01 03 02
 *                                               FF FE 00 00 02 03 01 00
 *
 *               - (1) The byte-order of the wide-character stream is assumed to be big-endian.
 *                       On little-endian CPUs, the byte-order of the destination character MUST be
 *                       flipped prior to storing.
 *
 *               - (2) !!!! Is this necessary?
 *******************************************************************************************************/
CPU_SIZE_T MB_CharToWC(CPU_WCHAR  *p_c_dest,
                       CPU_CHAR   *p_c_src,
                       CPU_SIZE_T n)
{
  CPU_WCHAR  c_dest;
  CPU_INT08U c_src_byte1;
  CPU_INT08U c_src_byte2;
  CPU_INT08U c_src_byte3;
#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  CPU_INT08U c_src_byte4;
#endif
  CPU_SIZE_T n_char;

  if (p_c_src == (CPU_CHAR *)0) {
    return ((CPU_SIZE_T)-1);
  }

  if (n < 1u) {                                                 // If insufficient len ...
    return ((CPU_SIZE_T)-2);                                    // ... rtn err (see Note #1b3).
  }

  c_dest = 0u;

  c_src_byte1 = (CPU_INT08U)*p_c_src;                           // Get 1st UTF-8 char.
  p_c_src++;

  if (c_src_byte1 > 0xF5u) {                                    // Validate 1st byte (see Note #2b1B, 2b1D).
    return ((CPU_SIZE_T)-1);
  }

  if (c_src_byte1 <= 0x7Fu) {                                   // ---------------- 1 -BYTE UTF-8 CHAR ----------------
    c_dest = (CPU_WCHAR)c_src_byte1;
    n_char = (c_dest == (CPU_WCHAR)0) ? 0u : 1u;
  } else {                                                      // ---------------- 2+-BYTE UTF-8 CHAR ----------------
    if (c_src_byte1 < 0xC2u) {                                  // Validate 1st UTF-8 char (see Note #2b1B, 2b1C).
      return ((CPU_SIZE_T)-1);
    }

    if (n < 2u) {                                               // If insufficient len ...
      return ((CPU_SIZE_T)-2);                                  // ... rtn err (see Note #1b3).
    }

    c_src_byte2 = (CPU_INT08U)*p_c_src;                         // Get 2nd UTF-8 char.
    p_c_src++;

    if ((c_src_byte2 < 0x80u)                                   // Validate 2nd byte (see Note #2b1A).
        || (c_src_byte2 > 0xBFu)) {
      return ((CPU_SIZE_T)-1);
    }

    if (c_src_byte1 <= 0xDFu) {                                 // 2-byte UTF-8 (U+0080..U+07FF)         ...
      c_src_byte1 &= 0x1Fu;                                     // ... form wide-character.
      c_src_byte2 &= 0x3Fu;
      c_dest = (CPU_WCHAR)((CPU_WCHAR)c_src_byte2 << 0)
               | (CPU_WCHAR)((CPU_WCHAR)c_src_byte1 << 6);
      n_char = 2u;
    } else {                                                    // ---------------- 3+-BYTE UTF-8 CHAR ----------------
      if (n < 3u) {                                             // If insufficient len ...
        return ((CPU_SIZE_T)-2);                                // ... rtn err (see Note #1b3).
      }

      c_src_byte3 = (CPU_INT08U)*p_c_src;                       // Get 3rd UTF-8 char.
      p_c_src++;

      if (c_src_byte1 <= 0xEFu) {                               // 3-byte UTF-8 (U+0800..U+FFFF)          ...
        if ((c_src_byte3 < 0x80u)                               // ... validate 3rd byte (see Note #2b1A) ...
            || (c_src_byte3 > 0xBFu)) {
          return ((CPU_SIZE_T)-1);
        }

        //                                                         ... validate 2nd byte                  ...
        if (((c_src_byte1 == 0xE0u) && (c_src_byte2 < 0xA0u))       // See Note #2b1E.
            || ((c_src_byte1 == 0xEDu) && (c_src_byte2 > 0x9Fu))) { // See Note #2b1F.
          return ((CPU_SIZE_T)-1);
        }

        c_src_byte1 &= 0x0Fu;                                       // ... form wide-character.
        c_src_byte2 &= 0x3Fu;
        c_src_byte3 &= 0x3Fu;
        c_dest = (CPU_WCHAR)((CPU_WCHAR)((CPU_WCHAR)c_src_byte3 <<  0)
                             | (CPU_WCHAR)((CPU_WCHAR)c_src_byte2 <<  6)
                             | (CPU_WCHAR)((CPU_WCHAR)c_src_byte1 << 12));
        n_char = 3u;
      } else {                                                  // ---------------- 4 -BYTE UTF-8 CHAR ----------------
#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
        if (n < 4) {                                            // If insufficient len ...
          return ((CPU_SIZE_T)-2);                              // ... rtn err (see Note #1b3).
        }

        c_src_byte4 = (CPU_INT08U)*p_c_src;                     // Get 4th UTF-8 char.

        if ((c_src_byte4 < 0x80)                                // Validate 4th byte (see Note #2b1A).
            || (c_src_byte4 > 0xBF)) {
          return ((CPU_SIZE_T)-1);
        }

        //                                                         Validate 2nd byte.
        if (c_src_byte1 == 0xF0) {                              // See Note #2b1G.
          if (c_src_byte2 < 0x90) {
            return ((CPU_SIZE_T)-1);
          }
        } else if (c_src_byte1 == 0xF4) {                       // See Note #2b1H.
          if (c_src_byte2 > 0x8F) {
            return ((CPU_SIZE_T)-1);
          }
        }

        c_src_byte1 &= 0x07;                                    // Form wide-character.
        c_src_byte2 &= 0x3F;
        c_src_byte3 &= 0x3F;
        c_src_byte4 &= 0x3F;
        c_dest = ((CPU_WCHAR)c_src_byte4 <<  0)
                 | ((CPU_WCHAR)c_src_byte3 <<  6)
                 | ((CPU_WCHAR)c_src_byte2 << 12)
                 | ((CPU_WCHAR)c_src_byte1 << 18);
        n_char = 4;

#else //                                                           If 32-bit chars not supported ...
        PP_UNUSED_PARAM(p_c_src);
        return ((CPU_SIZE_T)-1);                                // ... encoding err.
#endif
      }
    }
  }

  if (p_c_dest != (CPU_WCHAR *)0) {
    *p_c_dest = c_dest;
  }

  return (n_char);
}

/****************************************************************************************************//**
 *                                               MB_CharLen()
 *
 * @brief    Determine length of multi-byte character.
 *
 * @param    p_c     Pointer to buffer containing the multi-byte character.
 *
 * @param    n       Number of bytes in 'pc' to inspect for multi-byte character.
 *
 * @return   Number of bytes in character in 'pc' (between 1 & n), if 'pc' contains a valid non-NULL
 *                                                                       character.
 *           CPU_SIZE_T) 0,                                        if 'pc' contains a           NULL
 *                                                                       character.
 *           CPU_SIZE_T)-2,                                        if the first 'n' bytes of 'pc'
 *                                                                       contain an incomplete but potentially
 *                                                                       valid character.
 *           CPU_SIZE_T)-1,                                        otherwise.
 *
 * @note     (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'mbrlen() : DESCRIPTION' states that
 *                   "mblen() shall determine the number of bytes constituting the character pointed to
 *                   by s".
 *
 *           - (b) IEEE Std 1003.1, 2004 Edition, Section 'mbrlen() : RETURN VALUE' states that
 *                   "mbrlen() shall return the first of the following that applies :"
 *               - (1) "0                          If the next n or fewer bytes complete the character
 *                                                   that corresponds to the null wide character"
 *               - (2) "between 1 and n inclusive  If the next n or fewer bytes complete a valid
 *                                                   character (which is the value stored); the value
 *                                                   returned shall be the number of bytes that complete
 *                                                   the character"
 *               - (3) "(size_t)-2                 If the next n byte contribute to an incomplete
 *                                                   but potentially valid character"
 *               - (4) "(size_t)-1                 If an encoding error occurs"
 *******************************************************************************************************/
CPU_SIZE_T MB_CharLen(CPU_CHAR   *p_c,
                      CPU_SIZE_T n)
{
  CPU_SIZE_T c_len;

  if (p_c == (CPU_CHAR *)0) {
    return ((CPU_SIZE_T)0);
  }

  c_len = MB_CharToWC((CPU_WCHAR *)0,
                      p_c,
                      n);

  return (c_len);
}

/****************************************************************************************************//**
 *                                               WC_StrToMB()
 *
 * @brief    Convert wide-character string to a multi-byte character string, up to a maximum number of
 *           bytes.
 *
 * @param    p_dest      Pointer to buffer that will receive the multi-byte character string.
 *
 * @param    pp_src      Indirect pointer to wide-character string to convert, that will receive either
 *                       Pointer to NULL, if terminating NULL wide character reached.
 *                       Pointer to address past the last wide character converted, otherwise.
 *
 * @param    len_max     Maximum number of bytes to store in 'pdest'.
 *
 * @return   Number of bytes stored in 'pdest', if no encoding error occurs.
 *           (CPU_SIZE_T)-1,                    otherwise.
 *
 * @note     (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'wcsrtombs() : DESCRIPTION' states that
 *
 *               - (1) "The wcsrtombs() function shall convert a sequence of wide characters from the
 *                       array indirectly pointed to by src into a sequence of corresponding characters"
 *
 *               - (2) "Conversion continues up to and including a terminating null wide character,
 *                       which shall also be stored conversion shall stop earlier in the following
 *                       cases:"
 *
 *                   - (A) "When a code is reached that does not correspond to a valid character"
 *
 *                   - (B) "When the next character would exceed the limit of len total bytes to be
 *                           stored in the array pointed to by dst"
 *
 *               - (3) "If dst is not a null pointer, the pointer object pointed to by src shall be
 *                       assigned either a null pointer (if conversion stopped due to reaching a
 *                       terminating null wide character) or the address just past the last wide
 *                       character converted (if any)"
 *
 *           - (b) IEEE Std 1003.1, 2004 Edition, Section 'wcsrtombs() : RETURN VALUE' states that
 *                   "[if an encoding error occurs], the wcsrtombs() function shall ... return (size_t)-1
 *                   ... [o]therwise, it shall return the number of bytes in the resulting character
 *                   sequence, not including the terminating null (if any)".
 *
 * @note     (2) String conversion terminates when :
 *
 *           - (a) Destination/Indirect source string/Source string pointer(s) are passed NULL pointers.
 *               - (1) No string conversion performed; (CPU_SIZE_T)-1 returned.
 *
 *           - (b) Invalid character found in source string.
 *               - (1) Source string converted up to but NOT beyond or including the invalid character.
 *               - (2) Pointer to address past the last wide character converted stored in indirect
 *                       source string pointer; (CPU_SIZE_T)-1 returned.
 *
 *           - (c) Source string's terminating NULL wide character found.
 *               - (1) Entire source string converted into destination string buffer.
 *               - (2) Pointer to NULL stored in indirect source string pointer.
 *
 *           - (d) 'len_max' number of bytes stored in 'p_dest'.
 *               - (1) 'len_max' number of characters does NOT include the terminating NULL character.
 *               - (2) Pointer to address past the last wide character converted stored in indirect
 *                       source string pointer.
 *******************************************************************************************************/
CPU_SIZE_T WC_StrToMB(CPU_CHAR   *p_dest,
                      CPU_WCHAR  **pp_src,
                      CPU_SIZE_T len_max)
{
  CPU_WCHAR  *p_src;
  CPU_SIZE_T len_rem;
  CPU_SIZE_T len_char;

  //                                                               Rtn NULL if str ptr(s) NULL      (see Note #2a).
  if (pp_src == (CPU_WCHAR **)0) {
    return ((CPU_SIZE_T)-1);
  }

  p_src = *pp_src;
  if (p_src == (CPU_WCHAR *)0) {
    return ((CPU_SIZE_T)-1);
  }

  if (p_dest == (CPU_CHAR *)0) {
    return ((CPU_SIZE_T)-1);
  }

  if (len_max < MB_MAX_LEN) {                                   // Rtn NULL if copy len < max char len (see Note #2d).
    return  ((CPU_SIZE_T)0);
  }

  len_rem = len_max;
  len_char = WC_CharToMB(p_dest,
                         *p_src);

  while ((len_char <= MB_MAX_LEN)                               // Convert str until conversion err (see Note #2b) ...
         && (*p_src != 0u)                                      // ... or NULL wide char found      (see Note #2c) ...
         && (len_rem >= MB_MAX_LEN)) {                          // ... or max nbr bytes stored.     (see Note #2d).
    len_rem -= len_char;
    p_dest += len_char;
    p_src++;

    if (len_rem >= MB_MAX_LEN) {
      len_char = WC_CharToMB(p_dest,
                             *p_src);
    }
  }

  if (len_char == 0u) {                                         // If NULL wide char found ...
    *p_dest = (CPU_CHAR)'\0';                                   // ... append NULL char    ...
    *pp_src = (CPU_WCHAR *)0;                                   // ... rtn NULL ptr in indirect src str ptr.
    return (len_max - len_rem);
  } else if (len_char <= MB_MAX_LEN) {                          // If max nbr bytes stored ...
    *pp_src = p_src;                                            // ... rtn ptr past last char converted.
    return (len_max - len_rem);
  } else {                                                      // If conversion err ...
    *pp_src = p_src;                                            // ... rtn ptr past last char converted.
    return ((CPU_SIZE_T)-1);
  }
}

/****************************************************************************************************//**
 *                                               MB_StrToWC()
 *
 * @brief    Convert multi-byte character string to a wide-character string, up to a maximum number of
 *           characters.
 *
 * @param    p_dest      Pointer to buffer that will receive the wide-character string.
 *
 * @param    pp_src      Indirect pointer to multi-byte character string to convert, that will receive either
 *                       Pointer to NULL, if terminating NULL character reached.
 *                       Pointer to address past the last character converted, otherwise.
 *
 * @param    len_max     Maximum number of characters to store in 'p_dest'.
 *
 * @return   Number of characters stored in 'pdest', if no encoding error occurs.
 *
 *           (CPU_SIZE_T)-1,                         otherwise.
 *
 * @note     (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'mbsrtowcs() : DESCRIPTION' states that
 *
 *               - (1) "The mbsrtowcs() function shall convert a sequence of characters ... from the
 *                       array indirectly pointed to by src into a sequence of corresponding wide
 *                       characters"
 *
 *               - (2) "Conversion continues up to and including a terminating null character, which
 *                       shall also be stored.  Conversion shall stop earlier in either of the
 *                       following cases:"
 *
 *                   - (A) "A sequence of byte is encountered that does not form a valid character"
 *
 *                   - (B) "Len codes have been stored into the array pointed to by dst (and dst is
 *                           not a null pointer"
 *
 *               - (3) "If dst is not a null pointer, the pointer object pointed to by src shall be
 *                       assigned either a null pointer (if conversion stopped due to reaching a
 *                       terminating null character) or the address just past the last character
 *                       converted (if any)"
 *
 *           - (b) IEEE Std 1003.1, 2004 Edition, Section 'wcsrtombs() : RETURN VALUE' states that
 *                   "[if an encoding error occurs], the wcsrtombs() function shall ... return (size_t)-1
 *                   ... [o]therwise, it shall return the number of bytes in the resulting character
 *                   sequence, not including the terminating null (if any)".
 *
 * @note     (2) String conversion terminates when :
 *
 *           - (a) Destination/Indirect source string/Source string pointer(s) are passed NULL pointers.
 *               - (1) No string conversion performed; (CPU_SIZE_T)-1 returned.
 *
 *           - (b) Invalid character found in source string.
 *               - (1) Source string converted up to but NOT beyond or including the invalid character.
 *               - (2) Pointer to address past the last character converted stored in indirect source
 *                       string pointer; (CPU_SIZE_T)-1 returned.
 *
 *           - (c) Source string's terminating NULL character found.
 *               - (1) Entire source string converted into destination string buffer.
 *               - (2) Pointer to NULL stored in indirect source string pointer.
 *
 *           - (d) 'len_max' number of bytes stored in 'p_dest'.
 *               - (1) 'len_max' number of characters does NOT include the terminating NULL character.
 *               - (2) Pointer to address past the last wide character converted stored in indirect
 *                       source string pointer.
 *******************************************************************************************************/
CPU_SIZE_T MB_StrToWC(CPU_WCHAR  *p_dest,
                      CPU_CHAR   **pp_src,
                      CPU_SIZE_T len_max)
{
  CPU_CHAR   *p_src;
  CPU_SIZE_T len_copy;
  CPU_SIZE_T len_char;
  CPU_WCHAR  src_char;

  //                                                               Rtn NULL if str ptr(s) NULL      (see Note #2a).
  if (pp_src == (CPU_CHAR **)0) {
    return ((CPU_SIZE_T)-1);
  }

  p_src = *pp_src;
  if (p_src == (CPU_CHAR *)0) {
    return ((CPU_SIZE_T)-1);
  }
#if 0
  if (p_dest == (CPU_WCHAR *)0) {
    return ((CPU_SIZE_T)-1);
  }
#endif

  if (len_max == 0u) {                                          // Rtn NULL if copy len equals zero (see Note #2d).
    return  ((CPU_SIZE_T)0);
  }

  len_char = MB_CharToWC(&src_char,
                         p_src,
                         MB_MAX_LEN);
  len_copy = 0u;

  while ((len_char <= MB_MAX_LEN)                               // Convert str until conversion err (see Note #2b) ...
         && (*p_src != 0u)                                      // ... or NULL char found           (see Note #2c) ...
         && (len_copy < len_max)) {                             // ... or max nbr chars stored.     (see Note #2d).
    if (p_dest != DEF_NULL) {
      *p_dest = src_char;
      p_dest++;
    }
    p_src += len_char;
    len_copy++;

    len_char = MB_CharToWC(&src_char,
                           p_src,
                           MB_MAX_LEN);
  }

  if (len_char == 0u) {                                         // If NULL char found        ...
    if (p_dest != DEF_NULL) {
      *p_dest = (CPU_WCHAR)0;                                   // ... append NULL wide char ...
    }
    *pp_src = (CPU_CHAR *)0;                                    // ... rtn NULL ptr in indirect src str ptr.
    return (len_copy);
  } else if (len_char <= MB_MAX_LEN) {                          // If max nbr bytes stored ...
    *pp_src = p_src;                                            // ... rtn ptr past last char converted.
    return (len_copy);
  } else {                                                      // If conversion err ...
    *pp_src = p_src;                                            // ... rtn ptr past last char converted.
    return ((CPU_SIZE_T)-1);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               WC_SrchTbl()
 *
 * @brief    Search character-mapping table.
 *
 * @param    p_tbl       Pointer to table to search.
 *
 * @param    tbl_len     Length of table.
 *
 * @param    c_key       Key character to search for.
 *
 * @return   Pointer to character-mapping table entry, if range found.
 *           Pointer to NULL,                          otherwise.
 *
 * @note     (1) Each character SHOULD fit into a range in the table; in case the table or algorithm
 *               is broken, the failure to find the range is handled.
 *
 *           - (a) Table entries are sized for 16-bit characters only.  Characters belonging to the
 *                   upper code point space must be handled separately.
 *
 * @note     (2) To accelerate character mapping operations, a binary search is used to find the
 *               character range.  The tables have approximately 250-300 ranges, so the loop should
 *               execute at most 8 or 9 times.
 *******************************************************************************************************/
static const WC_SRCH_TBL_ENTRY *WC_SrchTbl(const WC_SRCH_TBL_ENTRY *p_tbl,
                                           CPU_SIZE_T              tbl_len,
                                           CPU_WCHAR               c_key)
{
  CPU_SIZE_T              ix_lo;
  CPU_SIZE_T              ix_mid = 0u;
  CPU_SIZE_T              ix_hi;
  CPU_WCHAR               c_start;
  CPU_WCHAR               c_end;
  CPU_BOOLEAN             found;
  const WC_SRCH_TBL_ENTRY *p_tbl_entry;

#if (FS_UNICODE_CFG_WCHAR_SIZE == 32)
  if (c_key >= DEF_INT_16U_MAX_VAL) {                           // See Note #1a.
    return ((WC_SRCH_TBL_ENTRY *)0);
  }
#endif

  found = DEF_NO;
  ix_lo = 0u;
  ix_hi = tbl_len - 2u;

  while ((ix_lo <= ix_hi)                                       // While lo srch ix & hi srch ix valid ...
         && (found == DEF_NO)) {                                // ... & range not found               ...
                                                                // ... bisect range tbl (see Note #2).

    ix_mid = (ix_lo + ix_hi) / 2u;
    c_start = p_tbl[ix_mid].Char;
    c_end = p_tbl[ix_mid + 1u].Char;

    if (c_key < c_start) {                                      // If char in lower half of range tbl ...
      ix_hi = ix_mid - 1u;                                      // ... make midpoint hi val.
    } else if (c_key >= c_end) {                                // If char in upper half of range tbl ...
      ix_lo = ix_mid + 1u;                                      // ... make midpoint lo val.
    } else {                                                    // Else range found.
      found = DEF_YES;
    }
  }

  if (found == DEF_NO) {                                        // If range NOT found (see Note #1) ...
    return ((WC_SRCH_TBL_ENTRY *)0);                            // ... rtn NULL.
  }

  p_tbl_entry = &p_tbl[ix_mid];
  return  (p_tbl_entry);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
