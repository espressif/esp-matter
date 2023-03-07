/***************************************************************************/ /**
 * @file
 * @brief CPC system endpoint common part
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

#ifndef SLI_CPC_SYSTEM_COMMON_H
#define SLI_CPC_SYSTEM_COMMON_H

#include "sl_enum.h"
#include "sl_status.h"
#include "sl_cpc.h"
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************/ /**
 * @addtogroup cpc_system_common
 * @brief CPC System Endpoint Common
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * System endpoint property id enum
 *
 * @brief
 *   This enum contains each of the supported property used by the
 *   CMD_SYSTEM_PROP_* system commands.
 *
 * @note
 *   This enum is encoded with 4 bytes.
 ******************************************************************************/
/// @brief Enumeration representing property used by the CMD_SYSTEM_PROP_* system commands .
SL_ENUM_GENERIC(sli_cpc_property_id_t, uint32_t)
{
  PROP_LAST_STATUS            = 0x00,
  PROP_PROTOCOL_VERSION       = 0x01,
  PROP_CAPABILITIES           = 0x02,
  PROP_SECONDARY_CPC_VERSION  = 0x03,
  PROP_SECONDARY_APP_VERSION  = 0x04,
  PROP_RX_CAPABILITY          = 0x20,
  PROP_FC_VALIDATION_VALUE    = 0x30,
  PROP_BOOTLOADER_INFO        = 0x200,
  PROP_BOOTLOADER_REBOOT_MODE = 0x202,
  PROP_SECURITY_STATE         = 0x301,
  PROP_CORE_DEBUG_COUNTERS    = 0x400,
  PROP_UFRAME_PROCESSING      = 0x500,
  PROP_ENTER_IRQ              = 0x600,
  PROP_ENDPOINT_STATE_0       = 0x1000,
  PROP_ENDPOINT_STATE_1       = 0x1001,
  PROP_ENDPOINT_STATE_2       = 0x1002,
  PROP_ENDPOINT_STATE_3       = 0x1003,
  PROP_ENDPOINT_STATE_4       = 0x1004,
  PROP_ENDPOINT_STATE_5       = 0x1005,
  PROP_ENDPOINT_STATE_6       = 0x1006,
  PROP_ENDPOINT_STATE_7       = 0x1007,
  PROP_ENDPOINT_STATE_8       = 0x1008,
  PROP_ENDPOINT_STATE_9       = 0x1009,
  PROP_ENDPOINT_STATE_10      = 0x100a,
  PROP_ENDPOINT_STATE_11      = 0x100b,
  PROP_ENDPOINT_STATE_12      = 0x100c,
  PROP_ENDPOINT_STATE_13      = 0x100d,
  PROP_ENDPOINT_STATE_14      = 0x100e,
  PROP_ENDPOINT_STATE_15      = 0x100f,
  PROP_ENDPOINT_STATE_16      = 0x1010,
  PROP_ENDPOINT_STATE_17      = 0x1011,
  PROP_ENDPOINT_STATE_18      = 0x1012,
  PROP_ENDPOINT_STATE_19      = 0x1013,
  PROP_ENDPOINT_STATE_20      = 0x1014,
  PROP_ENDPOINT_STATE_21      = 0x1015,
  PROP_ENDPOINT_STATE_22      = 0x1016,
  PROP_ENDPOINT_STATE_23      = 0x1017,
  PROP_ENDPOINT_STATE_24      = 0x1018,
  PROP_ENDPOINT_STATE_25      = 0x1019,
  PROP_ENDPOINT_STATE_26      = 0x101a,
  PROP_ENDPOINT_STATE_27      = 0x101b,
  PROP_ENDPOINT_STATE_28      = 0x101c,
  PROP_ENDPOINT_STATE_29      = 0x101d,
  PROP_ENDPOINT_STATE_30      = 0x101e,
  PROP_ENDPOINT_STATE_31      = 0x101f,
  PROP_ENDPOINT_STATE_32      = 0x1020,
  PROP_ENDPOINT_STATE_33      = 0x1021,
  PROP_ENDPOINT_STATE_34      = 0x1022,
  PROP_ENDPOINT_STATE_35      = 0x1023,
  PROP_ENDPOINT_STATE_36      = 0x1024,
  PROP_ENDPOINT_STATE_37      = 0x1025,
  PROP_ENDPOINT_STATE_38      = 0x1026,
  PROP_ENDPOINT_STATE_39      = 0x1027,
  PROP_ENDPOINT_STATE_40      = 0x1028,
  PROP_ENDPOINT_STATE_41      = 0x1029,
  PROP_ENDPOINT_STATE_42      = 0x102a,
  PROP_ENDPOINT_STATE_43      = 0x102b,
  PROP_ENDPOINT_STATE_44      = 0x102c,
  PROP_ENDPOINT_STATE_45      = 0x102d,
  PROP_ENDPOINT_STATE_46      = 0x102e,
  PROP_ENDPOINT_STATE_47      = 0x102f,
  PROP_ENDPOINT_STATE_48      = 0x1030,
  PROP_ENDPOINT_STATE_49      = 0x1031,
  PROP_ENDPOINT_STATE_50      = 0x1032,
  PROP_ENDPOINT_STATE_51      = 0x1033,
  PROP_ENDPOINT_STATE_52      = 0x1034,
  PROP_ENDPOINT_STATE_53      = 0x1035,
  PROP_ENDPOINT_STATE_54      = 0x1036,
  PROP_ENDPOINT_STATE_55      = 0x1037,
  PROP_ENDPOINT_STATE_56      = 0x1038,
  PROP_ENDPOINT_STATE_57      = 0x1039,
  PROP_ENDPOINT_STATE_58      = 0x103a,
  PROP_ENDPOINT_STATE_59      = 0x103b,
  PROP_ENDPOINT_STATE_60      = 0x103c,
  PROP_ENDPOINT_STATE_61      = 0x103d,
  PROP_ENDPOINT_STATE_62      = 0x103e,
  PROP_ENDPOINT_STATE_63      = 0x103f,
  PROP_ENDPOINT_STATE_64      = 0x1040,
  PROP_ENDPOINT_STATE_65      = 0x1041,
  PROP_ENDPOINT_STATE_66      = 0x1042,
  PROP_ENDPOINT_STATE_67      = 0x1043,
  PROP_ENDPOINT_STATE_68      = 0x1044,
  PROP_ENDPOINT_STATE_69      = 0x1045,
  PROP_ENDPOINT_STATE_70      = 0x1046,
  PROP_ENDPOINT_STATE_71      = 0x1047,
  PROP_ENDPOINT_STATE_72      = 0x1048,
  PROP_ENDPOINT_STATE_73      = 0x1049,
  PROP_ENDPOINT_STATE_74      = 0x104a,
  PROP_ENDPOINT_STATE_75      = 0x104b,
  PROP_ENDPOINT_STATE_76      = 0x104c,
  PROP_ENDPOINT_STATE_77      = 0x104d,
  PROP_ENDPOINT_STATE_78      = 0x104e,
  PROP_ENDPOINT_STATE_79      = 0x104f,
  PROP_ENDPOINT_STATE_80      = 0x1050,
  PROP_ENDPOINT_STATE_81      = 0x1051,
  PROP_ENDPOINT_STATE_82      = 0x1052,
  PROP_ENDPOINT_STATE_83      = 0x1053,
  PROP_ENDPOINT_STATE_84      = 0x1054,
  PROP_ENDPOINT_STATE_85      = 0x1055,
  PROP_ENDPOINT_STATE_86      = 0x1056,
  PROP_ENDPOINT_STATE_87      = 0x1057,
  PROP_ENDPOINT_STATE_88      = 0x1058,
  PROP_ENDPOINT_STATE_89      = 0x1059,
  PROP_ENDPOINT_STATE_90      = 0x105a,
  PROP_ENDPOINT_STATE_91      = 0x105b,
  PROP_ENDPOINT_STATE_92      = 0x105c,
  PROP_ENDPOINT_STATE_93      = 0x105d,
  PROP_ENDPOINT_STATE_94      = 0x105e,
  PROP_ENDPOINT_STATE_95      = 0x105f,
  PROP_ENDPOINT_STATE_96      = 0x1060,
  PROP_ENDPOINT_STATE_97      = 0x1061,
  PROP_ENDPOINT_STATE_98      = 0x1062,
  PROP_ENDPOINT_STATE_99      = 0x1063,
  PROP_ENDPOINT_STATE_100     = 0x1064,
  PROP_ENDPOINT_STATE_101     = 0x1065,
  PROP_ENDPOINT_STATE_102     = 0x1066,
  PROP_ENDPOINT_STATE_103     = 0x1067,
  PROP_ENDPOINT_STATE_104     = 0x1068,
  PROP_ENDPOINT_STATE_105     = 0x1069,
  PROP_ENDPOINT_STATE_106     = 0x106a,
  PROP_ENDPOINT_STATE_107     = 0x106b,
  PROP_ENDPOINT_STATE_108     = 0x106c,
  PROP_ENDPOINT_STATE_109     = 0x106d,
  PROP_ENDPOINT_STATE_110     = 0x106e,
  PROP_ENDPOINT_STATE_111     = 0x106f,
  PROP_ENDPOINT_STATE_112     = 0x1070,
  PROP_ENDPOINT_STATE_113     = 0x1071,
  PROP_ENDPOINT_STATE_114     = 0x1072,
  PROP_ENDPOINT_STATE_115     = 0x1073,
  PROP_ENDPOINT_STATE_116     = 0x1074,
  PROP_ENDPOINT_STATE_117     = 0x1075,
  PROP_ENDPOINT_STATE_118     = 0x1076,
  PROP_ENDPOINT_STATE_119     = 0x1077,
  PROP_ENDPOINT_STATE_120     = 0x1078,
  PROP_ENDPOINT_STATE_121     = 0x1079,
  PROP_ENDPOINT_STATE_122     = 0x107a,
  PROP_ENDPOINT_STATE_123     = 0x107b,
  PROP_ENDPOINT_STATE_124     = 0x107c,
  PROP_ENDPOINT_STATE_125     = 0x107d,
  PROP_ENDPOINT_STATE_126     = 0x107e,
  PROP_ENDPOINT_STATE_127     = 0x107f,
  PROP_ENDPOINT_STATE_128     = 0x1080,
  PROP_ENDPOINT_STATE_129     = 0x1081,
  PROP_ENDPOINT_STATE_130     = 0x1082,
  PROP_ENDPOINT_STATE_131     = 0x1083,
  PROP_ENDPOINT_STATE_132     = 0x1084,
  PROP_ENDPOINT_STATE_133     = 0x1085,
  PROP_ENDPOINT_STATE_134     = 0x1086,
  PROP_ENDPOINT_STATE_135     = 0x1087,
  PROP_ENDPOINT_STATE_136     = 0x1088,
  PROP_ENDPOINT_STATE_137     = 0x1089,
  PROP_ENDPOINT_STATE_138     = 0x108a,
  PROP_ENDPOINT_STATE_139     = 0x108b,
  PROP_ENDPOINT_STATE_140     = 0x108c,
  PROP_ENDPOINT_STATE_141     = 0x108d,
  PROP_ENDPOINT_STATE_142     = 0x108e,
  PROP_ENDPOINT_STATE_143     = 0x108f,
  PROP_ENDPOINT_STATE_144     = 0x1090,
  PROP_ENDPOINT_STATE_145     = 0x1091,
  PROP_ENDPOINT_STATE_146     = 0x1092,
  PROP_ENDPOINT_STATE_147     = 0x1093,
  PROP_ENDPOINT_STATE_148     = 0x1094,
  PROP_ENDPOINT_STATE_149     = 0x1095,
  PROP_ENDPOINT_STATE_150     = 0x1096,
  PROP_ENDPOINT_STATE_151     = 0x1097,
  PROP_ENDPOINT_STATE_152     = 0x1098,
  PROP_ENDPOINT_STATE_153     = 0x1099,
  PROP_ENDPOINT_STATE_154     = 0x109a,
  PROP_ENDPOINT_STATE_155     = 0x109b,
  PROP_ENDPOINT_STATE_156     = 0x109c,
  PROP_ENDPOINT_STATE_157     = 0x109d,
  PROP_ENDPOINT_STATE_158     = 0x109e,
  PROP_ENDPOINT_STATE_159     = 0x109f,
  PROP_ENDPOINT_STATE_160     = 0x10a0,
  PROP_ENDPOINT_STATE_161     = 0x10a1,
  PROP_ENDPOINT_STATE_162     = 0x10a2,
  PROP_ENDPOINT_STATE_163     = 0x10a3,
  PROP_ENDPOINT_STATE_164     = 0x10a4,
  PROP_ENDPOINT_STATE_165     = 0x10a5,
  PROP_ENDPOINT_STATE_166     = 0x10a6,
  PROP_ENDPOINT_STATE_167     = 0x10a7,
  PROP_ENDPOINT_STATE_168     = 0x10a8,
  PROP_ENDPOINT_STATE_169     = 0x10a9,
  PROP_ENDPOINT_STATE_170     = 0x10aa,
  PROP_ENDPOINT_STATE_171     = 0x10ab,
  PROP_ENDPOINT_STATE_172     = 0x10ac,
  PROP_ENDPOINT_STATE_173     = 0x10ad,
  PROP_ENDPOINT_STATE_174     = 0x10ae,
  PROP_ENDPOINT_STATE_175     = 0x10af,
  PROP_ENDPOINT_STATE_176     = 0x10b0,
  PROP_ENDPOINT_STATE_177     = 0x10b1,
  PROP_ENDPOINT_STATE_178     = 0x10b2,
  PROP_ENDPOINT_STATE_179     = 0x10b3,
  PROP_ENDPOINT_STATE_180     = 0x10b4,
  PROP_ENDPOINT_STATE_181     = 0x10b5,
  PROP_ENDPOINT_STATE_182     = 0x10b6,
  PROP_ENDPOINT_STATE_183     = 0x10b7,
  PROP_ENDPOINT_STATE_184     = 0x10b8,
  PROP_ENDPOINT_STATE_185     = 0x10b9,
  PROP_ENDPOINT_STATE_186     = 0x10ba,
  PROP_ENDPOINT_STATE_187     = 0x10bb,
  PROP_ENDPOINT_STATE_188     = 0x10bc,
  PROP_ENDPOINT_STATE_189     = 0x10bd,
  PROP_ENDPOINT_STATE_190     = 0x10be,
  PROP_ENDPOINT_STATE_191     = 0x10bf,
  PROP_ENDPOINT_STATE_192     = 0x10c0,
  PROP_ENDPOINT_STATE_193     = 0x10c1,
  PROP_ENDPOINT_STATE_194     = 0x10c2,
  PROP_ENDPOINT_STATE_195     = 0x10c3,
  PROP_ENDPOINT_STATE_196     = 0x10c4,
  PROP_ENDPOINT_STATE_197     = 0x10c5,
  PROP_ENDPOINT_STATE_198     = 0x10c6,
  PROP_ENDPOINT_STATE_199     = 0x10c7,
  PROP_ENDPOINT_STATE_200     = 0x10c8,
  PROP_ENDPOINT_STATE_201     = 0x10c9,
  PROP_ENDPOINT_STATE_202     = 0x10ca,
  PROP_ENDPOINT_STATE_203     = 0x10cb,
  PROP_ENDPOINT_STATE_204     = 0x10cc,
  PROP_ENDPOINT_STATE_205     = 0x10cd,
  PROP_ENDPOINT_STATE_206     = 0x10ce,
  PROP_ENDPOINT_STATE_207     = 0x10cf,
  PROP_ENDPOINT_STATE_208     = 0x10d0,
  PROP_ENDPOINT_STATE_209     = 0x10d1,
  PROP_ENDPOINT_STATE_210     = 0x10d2,
  PROP_ENDPOINT_STATE_211     = 0x10d3,
  PROP_ENDPOINT_STATE_212     = 0x10d4,
  PROP_ENDPOINT_STATE_213     = 0x10d5,
  PROP_ENDPOINT_STATE_214     = 0x10d6,
  PROP_ENDPOINT_STATE_215     = 0x10d7,
  PROP_ENDPOINT_STATE_216     = 0x10d8,
  PROP_ENDPOINT_STATE_217     = 0x10d9,
  PROP_ENDPOINT_STATE_218     = 0x10da,
  PROP_ENDPOINT_STATE_219     = 0x10db,
  PROP_ENDPOINT_STATE_220     = 0x10dc,
  PROP_ENDPOINT_STATE_221     = 0x10dd,
  PROP_ENDPOINT_STATE_222     = 0x10de,
  PROP_ENDPOINT_STATE_223     = 0x10df,
  PROP_ENDPOINT_STATE_224     = 0x10e0,
  PROP_ENDPOINT_STATE_225     = 0x10e1,
  PROP_ENDPOINT_STATE_226     = 0x10e2,
  PROP_ENDPOINT_STATE_227     = 0x10e3,
  PROP_ENDPOINT_STATE_228     = 0x10e4,
  PROP_ENDPOINT_STATE_229     = 0x10e5,
  PROP_ENDPOINT_STATE_230     = 0x10e6,
  PROP_ENDPOINT_STATE_231     = 0x10e7,
  PROP_ENDPOINT_STATE_232     = 0x10e8,
  PROP_ENDPOINT_STATE_233     = 0x10e9,
  PROP_ENDPOINT_STATE_234     = 0x10ea,
  PROP_ENDPOINT_STATE_235     = 0x10eb,
  PROP_ENDPOINT_STATE_236     = 0x10ec,
  PROP_ENDPOINT_STATE_237     = 0x10ed,
  PROP_ENDPOINT_STATE_238     = 0x10ee,
  PROP_ENDPOINT_STATE_239     = 0x10ef,
  PROP_ENDPOINT_STATE_240     = 0x10f0,
  PROP_ENDPOINT_STATE_241     = 0x10f1,
  PROP_ENDPOINT_STATE_242     = 0x10f2,
  PROP_ENDPOINT_STATE_243     = 0x10f3,
  PROP_ENDPOINT_STATE_244     = 0x10f4,
  PROP_ENDPOINT_STATE_245     = 0x10f5,
  PROP_ENDPOINT_STATE_246     = 0x10f6,
  PROP_ENDPOINT_STATE_247     = 0x10f7,
  PROP_ENDPOINT_STATE_248     = 0x10f8,
  PROP_ENDPOINT_STATE_249     = 0x10f9,
  PROP_ENDPOINT_STATE_250     = 0x10fa,
  PROP_ENDPOINT_STATE_251     = 0x10fb,
  PROP_ENDPOINT_STATE_252     = 0x10fc,
  PROP_ENDPOINT_STATE_253     = 0x10fd,
  PROP_ENDPOINT_STATE_254     = 0x10fe,
  PROP_ENDPOINT_STATE_255     = 0x10ff,
  PROP_ENDPOINT_STATES        = 0x1100, ///< Aggregates all endpoint state information into a single field.
};

/***************************************************************************//**
 * Helper macros to convert an enpoint id (uint8_t) to a PROP_ENDPOINT_STATE_x
 * enum value.
 ******************************************************************************/
#define EP_ID_TO_PROPERTY_ID(ep_id)  ((sli_cpc_property_id_t)((0x00001000 | ((ep_id) & 0x000000FF))))

/***************************************************************************//**
 * Helper macros to convert a PROP_ENDPOINT_STATE_x enum value to an endpoint id
 * (uint8_t).
 ******************************************************************************/
#define PROPERTY_ID_TO_EP_ID(property_id) ((uint8_t)(property_id) & 0x000000FF)

/***************************************************************************//**
 * Helper macros to extract the two aggregated endpoint states encoded in one
 * single byte.
 ******************************************************************************/
#define AGGREGATED_STATE_LOW(agg)  ((sl_cpc_endpoint_state_t)(agg & 0x0F))
#define AGGREGATED_STATE_HIGH(agg) ((sl_cpc_endpoint_state_t)(agg >> 4))

#define GET_ENDPOINT_STATE_FROM_STATES(payload, ep_id)                      \
  ((ep_id % 2 == 0) ?  AGGREGATED_STATE_LOW(((uint8_t*)payload)[ep_id / 2]) \
   : AGGREGATED_STATE_HIGH(((uint8_t*)payload)[ep_id / 2]))

/// @brief Enumeration representing spinel protocol status code
SL_ENUM_GENERIC(sl_cpc_system_status_t, uint32_t)
{
  STATUS_OK                = 0,  ///< Operation has completed successfully.
  STATUS_FAILURE           = 1,  ///< Operation has failed for some undefined reason.
  STATUS_UNIMPLEMENTED     = 2,  ///< The given operation has not been implemented.
  STATUS_INVALID_ARGUMENT  = 3,  ///< An argument to the given operation is invalid.
  STATUS_INVALID_STATE     = 4,  ///< The given operation is invalid for the current state of the device.
  STATUS_INVALID_COMMAND   = 5,  ///< The given command is not recognized.
  STATUS_INVALID_INTERFACE = 6,  ///< The given Spinel interface is not supported.
  STATUS_INTERNAL_ERROR    = 7,  ///< An internal runtime error has occurred.
  STATUS_SECURITY_ERROR    = 8,  ///< A security or authentication error has occurred.
  STATUS_PARSE_ERROR       = 9,  ///< An error has occurred while parsing the command.
  STATUS_IN_PROGRESS       = 10, ///< The operation is in progress and will be completed asynchronously.
  STATUS_NOMEM             = 11, ///< The operation has been prevented due to memory pressure.
  STATUS_BUSY              = 12, ///< The device is currently performing a mutually exclusive operation.
  STATUS_PROP_NOT_FOUND    = 13, ///< The given property is not recognized.
  STATUS_PACKET_DROPPED    = 14, ///< The packet was dropped.
  STATUS_EMPTY             = 15, ///< The result of the operation is empty.
  STATUS_CMD_TOO_BIG       = 16, ///< The command was too large to fit in the internal buffer.
  STATUS_NO_ACK            = 17, ///< The packet was not acknowledged.
  STATUS_CCA_FAILURE       = 18, ///< The packet was not sent due to a CCA failure.
  STATUS_ALREADY           = 19, ///< The operation is already in progress or the property was already set to the given value.
  STATUS_ITEM_NOT_FOUND    = 20, ///< The given item could not be found in the property.
  STATUS_INVALID_COMMAND_FOR_PROP = 21, ///< The given command cannot be performed on this property.
  // 22-111 : RESERVED
  STATUS_RESET_POWER_ON    = 112,
  STATUS_RESET_EXTERNAL    = 113,
  STATUS_RESET_SOFTWARE    = 114,
  STATUS_RESET_FAULT       = 115,
  STATUS_RESET_CRASH       = 116,
  STATUS_RESET_ASSERT      = 117,
  STATUS_RESET_OTHER       = 118,
  STATUS_RESET_UNKNOWN     = 119,
  STATUS_RESET_WATCHDOG    = 120,
  // 121-127 : RESERVED-RESET-CODES
  // 128 - 15,359: UNALLOCATED
  // 15,360 - 16,383: Vendor-specific
  // 16,384 - 1,999,999: UNALLOCATED
  // 2,000,000 - 2,097,151: Experimental Use Only (MUST NEVER be used in production!)
};

/// @brief Enumeration representing Bootloader reboot mode.
SL_ENUM_GENERIC(sli_cpc_system_reboot_mode_t, uint32_t)
{
  REBOOT_APPLICATION = 0, ///< At the next reboot application is executed.
  REBOOT_BOOTLOADER  = 1  ///< At the next reboot bootloader is executed
};

/***************************************************************************//**
 * Enter IRQ command parameters
 ******************************************************************************/
typedef struct {
  uint32_t start_in_ms;
  uint32_t end_in_ms;
} sli_cpc_system_enter_irq_cmd_t;

/***************************************************************************//**
 * Capabilities mask.
 *
 * @note
 *   Used with the value returned by a property-get on PROP_CAPABILITIES.
 ******************************************************************************/
#define CPC_CAPABILITIES_SECURITY_ENDPOINT_MASK (1 << 0)
#define CPC_CAPABILITIES_PACKED_ENDPOINT_MASK   (1 << 1)
#define CPC_CAPABILITIES_GPIO_ENDPOINT_MASK     (1 << 2)
#define CPC_CAPABILITIES_UART_FLOW_CONTROL_MASK (1 << 3)

/***************************************************************************//**
 * System endpoint command id enum.
 ******************************************************************************/
SL_ENUM(sli_cpc_system_cmd_id_t)
{
  CMD_SYSTEM_NOOP           = 0x00,
  CMD_SYSTEM_RESET          = 0x01,
  CMD_SYSTEM_PROP_VALUE_GET = 0x02,
  CMD_SYSTEM_PROP_VALUE_SET = 0x03,
  CMD_SYSTEM_PROP_VALUE_IS  = 0x06,
};

/***************************************************************************//**
 * System endpoint command type.
 ******************************************************************************/
#define PAYLOAD_LENGTH_MAX    16

typedef struct {
  sli_cpc_system_cmd_id_t command_id;                 ///< Identifier of the command.
  uint8_t                seq;                         ///< Command sequence number.
  uint16_t               length;                      ///< Length of the payload in bytes.
} sli_cpc_system_cmd_header_t;

typedef struct {
  sli_cpc_system_cmd_header_t header;                      ///< Command header
  uint8_t                     payload[PAYLOAD_LENGTH_MAX]; ///< Command payload.
} sli_cpc_system_cmd_t;

typedef struct {
  sli_cpc_property_id_t property_id;        ///< Identifier of the property.
  uint8_t payload[];                        ///< Property value.
} sli_cpc_system_property_cmd_t;

/** @} (end addtogroup cpc_system_common) */

#ifdef __cplusplus
}
#endif

#endif /* SL_CPC_SYSTEM_COMMON_H_ */
