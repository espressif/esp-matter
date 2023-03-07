/***************************************************************************//**
 * @file
 * @brief Common - General Defines
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup LIB_DEF LIB Def API
 * @ingroup  LIB
 * @brief      LIB Def API
 *
 * @addtogroup LIB_DEF
 * @{
 ********************************************************************************************************
 * @note     (1) This file is intended to regroup LIB capabilities that do not depend on CPU elements.
 *               'lib_utils.h' should be used if CPU is needed.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _LIB_DEF_H_
#define  _LIB_DEF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           STANDARD DEFINES
 *******************************************************************************************************/

#define  DEF_NULL                                          0

//                                                                 ----------------- BOOLEAN DEFINES ------------------
#define  DEF_FALSE                                         0u
#define  DEF_TRUE                                          1u

#define  DEF_NO                                            0u
#define  DEF_YES                                           1u

#define  DEF_DISABLED                                      0u
#define  DEF_ENABLED                                       1u

#define  DEF_INACTIVE                                      0u
#define  DEF_ACTIVE                                        1u

#define  DEF_INVALID                                       0u
#define  DEF_VALID                                         1u

#define  DEF_OFF                                           0u
#define  DEF_ON                                            1u

#define  DEF_CLR                                           0u
#define  DEF_SET                                           1u

#define  DEF_FAIL                                          0u
#define  DEF_OK                                            1u

//                                                                 ------------------- BIT DEFINES --------------------
#define  DEF_BIT_NONE                                   0x00u

#define  DEF_BIT_00                                     0x01u
#define  DEF_BIT_01                                     0x02u
#define  DEF_BIT_02                                     0x04u
#define  DEF_BIT_03                                     0x08u
#define  DEF_BIT_04                                     0x10u
#define  DEF_BIT_05                                     0x20u
#define  DEF_BIT_06                                     0x40u
#define  DEF_BIT_07                                     0x80u

#define  DEF_BIT_08                                   0x0100u
#define  DEF_BIT_09                                   0x0200u
#define  DEF_BIT_10                                   0x0400u
#define  DEF_BIT_11                                   0x0800u
#define  DEF_BIT_12                                   0x1000u
#define  DEF_BIT_13                                   0x2000u
#define  DEF_BIT_14                                   0x4000u
#define  DEF_BIT_15                                   0x8000u

#define  DEF_BIT_16                               0x00010000u
#define  DEF_BIT_17                               0x00020000u
#define  DEF_BIT_18                               0x00040000u
#define  DEF_BIT_19                               0x00080000u
#define  DEF_BIT_20                               0x00100000u
#define  DEF_BIT_21                               0x00200000u
#define  DEF_BIT_22                               0x00400000u
#define  DEF_BIT_23                               0x00800000u

#define  DEF_BIT_24                               0x01000000u
#define  DEF_BIT_25                               0x02000000u
#define  DEF_BIT_26                               0x04000000u
#define  DEF_BIT_27                               0x08000000u
#define  DEF_BIT_28                               0x10000000u
#define  DEF_BIT_29                               0x20000000u
#define  DEF_BIT_30                               0x40000000u
#define  DEF_BIT_31                               0x80000000u
#define  DEF_BIT_32                       0x0000000100000000u
#define  DEF_BIT_33                       0x0000000200000000u
#define  DEF_BIT_34                       0x0000000400000000u
#define  DEF_BIT_35                       0x0000000800000000u
#define  DEF_BIT_36                       0x0000001000000000u
#define  DEF_BIT_37                       0x0000002000000000u
#define  DEF_BIT_38                       0x0000004000000000u
#define  DEF_BIT_39                       0x0000008000000000u

#define  DEF_BIT_40                       0x0000010000000000u
#define  DEF_BIT_41                       0x0000020000000000u
#define  DEF_BIT_42                       0x0000040000000000u
#define  DEF_BIT_43                       0x0000080000000000u
#define  DEF_BIT_44                       0x0000100000000000u
#define  DEF_BIT_45                       0x0000200000000000u
#define  DEF_BIT_46                       0x0000400000000000u
#define  DEF_BIT_47                       0x0000800000000000u

#define  DEF_BIT_48                       0x0001000000000000u
#define  DEF_BIT_49                       0x0002000000000000u
#define  DEF_BIT_50                       0x0004000000000000u
#define  DEF_BIT_51                       0x0008000000000000u
#define  DEF_BIT_52                       0x0010000000000000u
#define  DEF_BIT_53                       0x0020000000000000u
#define  DEF_BIT_54                       0x0040000000000000u
#define  DEF_BIT_55                       0x0080000000000000u

#define  DEF_BIT_56                       0x0100000000000000u
#define  DEF_BIT_57                       0x0200000000000000u
#define  DEF_BIT_58                       0x0400000000000000u
#define  DEF_BIT_59                       0x0800000000000000u
#define  DEF_BIT_60                       0x1000000000000000u
#define  DEF_BIT_61                       0x2000000000000000u
#define  DEF_BIT_62                       0x4000000000000000u
#define  DEF_BIT_63                       0x8000000000000000u

//                                                                 ------------------ ALIGN DEFINES -------------------
#define  DEF_ALIGN_MAX_NBR_OCTETS                       4096u

//                                                                 ------------------ OCTET DEFINES -------------------
#define  DEF_OCTET_NBR_BITS                                8u
#define  DEF_OCTET_MASK                                 0xFFu

#define  DEF_OCTET_TO_BIT_NBR_BITS                         3u
#define  DEF_OCTET_TO_BIT_SHIFT                          DEF_OCTET_TO_BIT_NBR_BITS
#define  DEF_OCTET_TO_BIT_MASK                          0x07u

#define  DEF_NIBBLE_NBR_BITS                               4u
#define  DEF_NIBBLE_MASK                                0x0Fu

//                                                                 --------------- NUMBER BASE DEFINES ----------------
#define  DEF_NBR_BASE_BIN                                  2u
#define  DEF_NBR_BASE_OCT                                  8u
#define  DEF_NBR_BASE_DEC                                 10u
#define  DEF_NBR_BASE_HEX                                 16u

//                                                                 ----------------- INTEGER DEFINES ------------------
#define  DEF_INT_08_NBR_BITS                               8u
#define  DEF_INT_08_MASK                                0xFFu

#define  DEF_INT_08U_MIN_VAL                               0u
#define  DEF_INT_08U_MAX_VAL                             255u

#define  DEF_INT_08S_MIN_VAL_ONES_CPL                  (-127)
#define  DEF_INT_08S_MAX_VAL_ONES_CPL                    127

#define  DEF_INT_08S_MIN_VAL                            (DEF_INT_08S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_08S_MAX_VAL                             DEF_INT_08S_MAX_VAL_ONES_CPL

#define  DEF_INT_08U_NBR_DIG_MIN                           1u
#define  DEF_INT_08U_NBR_DIG_MAX                           3u

#define  DEF_INT_08S_NBR_DIG_MIN                           3u
#define  DEF_INT_08S_NBR_DIG_MAX                           3u

#define  DEF_INT_16_NBR_BITS                              16u
#define  DEF_INT_16_MASK                              0xFFFFu

#define  DEF_INT_16U_MIN_VAL                               0u
#define  DEF_INT_16U_MAX_VAL                           65535u

#define  DEF_INT_16S_MIN_VAL_ONES_CPL                (-32767)
#define  DEF_INT_16S_MAX_VAL_ONES_CPL                  32767

#define  DEF_INT_16S_MIN_VAL                            (DEF_INT_16S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_16S_MAX_VAL                             DEF_INT_16S_MAX_VAL_ONES_CPL

#define  DEF_INT_16U_NBR_DIG_MIN                           1u
#define  DEF_INT_16U_NBR_DIG_MAX                           5u

#define  DEF_INT_16S_NBR_DIG_MIN                           5u
#define  DEF_INT_16S_NBR_DIG_MAX                           5u

#define  DEF_INT_32_NBR_BITS                              32u
#define  DEF_INT_32_MASK                          0xFFFFFFFFu

#define  DEF_INT_32U_MIN_VAL                               0u
#define  DEF_INT_32U_MAX_VAL                      4294967295u

#define  DEF_INT_32S_MIN_VAL_ONES_CPL           (-2147483647)
#define  DEF_INT_32S_MAX_VAL_ONES_CPL             2147483647

#define  DEF_INT_32S_MIN_VAL                            (DEF_INT_32S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_32S_MAX_VAL                             DEF_INT_32S_MAX_VAL_ONES_CPL

#define  DEF_INT_32U_NBR_DIG_MIN                           1u
#define  DEF_INT_32U_NBR_DIG_MAX                          10u

#define  DEF_INT_32S_NBR_DIG_MIN                          10u
#define  DEF_INT_32S_NBR_DIG_MAX                          10u

#define  DEF_INT_64_NBR_BITS                              64u
#define  DEF_INT_64_MASK                  0xFFFFFFFFFFFFFFFFu

#define  DEF_INT_64U_MIN_VAL                               0u
#define  DEF_INT_64U_MAX_VAL            18446744073709551615u

#define  DEF_INT_64S_MIN_VAL_ONES_CPL  (-9223372036854775807)
#define  DEF_INT_64S_MAX_VAL_ONES_CPL    9223372036854775807

#define  DEF_INT_64S_MIN_VAL                            (DEF_INT_64S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_64S_MAX_VAL                             DEF_INT_64S_MAX_VAL_ONES_CPL

#define  DEF_INT_64U_NBR_DIG_MIN                           1u
#define  DEF_INT_64U_NBR_DIG_MAX                          20u

#define  DEF_INT_64S_NBR_DIG_MIN                          19u
#define  DEF_INT_64S_NBR_DIG_MAX                          19u

//                                                                 ------------------- TIME DEFINES -------------------
#define  DEF_TIME_NBR_DAY_PER_WK                           7u
#define  DEF_TIME_NBR_DAY_PER_YR                         365u
#define  DEF_TIME_NBR_DAY_PER_YR_LEAP                    366u

#define  DEF_TIME_NBR_HR_PER_DAY                          24u
#define  DEF_TIME_NBR_HR_PER_WK                         (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_WK)
#define  DEF_TIME_NBR_HR_PER_YR                         (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_YR)
#define  DEF_TIME_NBR_HR_PER_YR_LEAP                    (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_MIN_PER_HR                          60u
#define  DEF_TIME_NBR_MIN_PER_DAY                       (DEF_TIME_NBR_MIN_PER_HR  * DEF_TIME_NBR_HR_PER_DAY)
#define  DEF_TIME_NBR_MIN_PER_WK                        (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_WK)
#define  DEF_TIME_NBR_MIN_PER_YR                        (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_YR)
#define  DEF_TIME_NBR_MIN_PER_YR_LEAP                   (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_SEC_PER_MIN                         60u
#define  DEF_TIME_NBR_SEC_PER_HR                        (DEF_TIME_NBR_SEC_PER_MIN * DEF_TIME_NBR_MIN_PER_HR)
#define  DEF_TIME_NBR_SEC_PER_DAY                       (DEF_TIME_NBR_SEC_PER_HR  * DEF_TIME_NBR_HR_PER_DAY)
#define  DEF_TIME_NBR_SEC_PER_WK                        (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_WK)
#define  DEF_TIME_NBR_SEC_PER_YR                        (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_YR)
#define  DEF_TIME_NBR_SEC_PER_YR_LEAP                   (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_mS_PER_SEC                        1000u
#define  DEF_TIME_NBR_uS_PER_SEC                     1000000u
#define  DEF_TIME_NBR_nS_PER_SEC                  1000000000u

#define  DEF_TIME_NBR_uS_PER_mS                         1000u
#define  DEF_TIME_NBR_nS_PER_mS                      1000000u

/********************************************************************************************************
 *                                               TRACING
 *
 * Note(s) : (1) The following trace level defines are deprecated. Unless already used, these defines
 *               should not be used, since they will be removed in a future version.
 *******************************************************************************************************/

//                                                                 Trace level, default to TRACE_LEVEL_OFF.
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                   0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                  1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                   2u
#endif

#ifndef  TRACE_LEVEL_LOG
#define  TRACE_LEVEL_LOG                                   3u
#endif

/********************************************************************************************************
 *                                               VALUE MACRO'S
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               DEF_CHK_VAL_MIN()
 *
 * @brief    Validate a value as greater than or equal to a specified minimum value.
 *
 * @param    val         Value to validate.
 *
 * @param    val_min     Minimum value to test.
 *
 * @return   DEF_OK,    Value is greater than or equal to minimum value.
 *           DEF_FAIL,  otherwise.
 *
 * @note     (1) DEF_CHK_VAL_MIN() avoids directly comparing any two values if only one of the values
 *               is negative since the negative value might be incorrectly promoted to an arbitrary
 *               unsigned value if the other value to compare is unsigned.
 *
 * @note     (2) Validation of values is limited to the range supported by the compiler &/or target
 *               environment.  All other values that underflow/overflow the supported range will
 *               modulo/wrap into the supported range as arbitrary signed or unsigned values.
 *               @n
 *               Therefore, any values that underflow the most negative signed value or overflow
 *               the most positive unsigned value supported by the compiler &/or target environment
 *               cannot be validated :
 *                @verbatim
 *                       (    N-1       N     ]
 *                       ( -(2   )  ,  2  - 1 ]
 *                       (                    ]
 *
 *                           where
 *                                   N       Number of data word bits supported by the compiler
 *                                               &/or target environment
 *               @endverbatim
 *           - (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
 *                   range since many compilers do NOT always correctly handle this value.
 *
 * @note     (3) 'val' and 'val_min' are compared to 1 instead of 0 to avoid warning generated for
 *               unsigned numbers.
 *******************************************************************************************************/

#define  DEF_CHK_VAL_MIN(val, val_min)            (((!(((val) >= 1) && ((val_min) < 1)))  \
                                                    && ((((val_min) >= 1) && ((val) < 1)) \
                                                        || ((val) < (val_min)))) ? DEF_FAIL : DEF_OK)

/****************************************************************************************************//**
 *                                               DEF_CHK_VAL_MAX()
 *
 * @brief    Validate a value as less than or equal to a specified maximum value.
 *
 * @param    val         Value to validate.
 *
 * @param    val_max     Maximum value to test.
 *
 * @return   DEF_OK,    Value is less than or equal to maximum value.
 *           DEF_FAIL,  otherwise.
 *
 * @note     (1) DEF_CHK_VAL_MAX() avoids directly comparing any two values if only one of the values
 *               is negative since the negative value might be incorrectly promoted to an arbitrary
 *               unsigned value if the other value to compare is unsigned.
 *
 * @note     (2) Validation of values is limited to the range supported by the compiler &/or target
 *               environment.  All other values that underflow/overflow the supported range will
 *               modulo/wrap into the supported range as arbitrary signed or unsigned values.
 *
 *               Therefore, any values that underflow the most negative signed value or overflow
 *               the most positive unsigned value supported by the compiler &/or target environment
 *               cannot be validated :
 *               @verbatim
 *                       (    N-1       N     ]
 *                       ( -(2   )  ,  2  - 1 ]
 *                       (                    ]
 *
 *                           where
 *                                   N       Number of data word bits supported by the compiler
 *                                               &/or target environment
 *               @endverbatim
 *           - (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
 *                   range since many compilers do NOT always correctly handle this value.
 *
 * @note     (3) 'val' and 'val_max' are compared to 1 instead of 0 to avoid warning generated for
 *               unsigned numbers.
 *******************************************************************************************************/

#define  DEF_CHK_VAL_MAX(val, val_max)            (((!(((val_max) >= 1) && ((val) < 1)))  \
                                                    && ((((val) >= 1) && ((val_max) < 1)) \
                                                        || ((val) > (val_max)))) ? DEF_FAIL : DEF_OK)

/****************************************************************************************************//**
 *                                               DEF_CHK_VAL()
 *
 * @brief    Validate a value as greater than or equal to a specified minimum value & less than or
 *               equal to a specified maximum value.
 *
 * @param    val         Value to validate.
 *
 * @param    val_min     Minimum value to test.
 *
 * @param    val_max     Maximum value to test.
 *
 * @return   DEF_OK,    Value is greater than or equal to minimum value AND
 *                               less    than or equal to maximum value.
 *           DEF_FAIL,  otherwise.
 *
 * @note     (1) DEF_CHK_VAL() avoids directly comparing any two values if only one of the values
 *               is negative since the negative value might be incorrectly promoted to an arbitrary
 *               unsigned value if the other value to compare is unsigned.
 *
 * @note     (2) Validation of values is limited to the range supported by the compiler &/or target
 *               environment.  All other values that underflow/overflow the supported range will
 *               modulo/wrap into the supported range as arbitrary signed or unsigned values.
 *
 *               Therefore, any values that underflow the most negative signed value or overflow
 *               the most positive unsigned value supported by the compiler &/or target environment
 *               cannot be validated :
 *               @verbatim
 *                       (    N-1       N     ]
 *                       ( -(2   )  ,  2  - 1 ]
 *                       (                    ]
 *
 *                           where
 *                                   N       Number of data word bits supported by the compiler
 *                                               &/or target environment
 *               @endverbatim
 *           - (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
 *                   range since many compilers do NOT always correctly handle this value.
 *
 * @note     (3) DEF_CHK_VAL() does NOT validate that the maximum value ('val_max') is greater than
 *               or equal to the minimum value ('val_min').
 *******************************************************************************************************/

#define  DEF_CHK_VAL(val, val_min, val_max)          (((DEF_CHK_VAL_MIN((val), (val_min)) == DEF_FAIL) \
                                                       || (DEF_CHK_VAL_MAX((val), (val_max)) == DEF_FAIL)) ? DEF_FAIL : DEF_OK)

/********************************************************************************************************
 *                                               MATH MACRO'S
 *
 * Note(s) : (1) Ideally, ALL mathematical macro's & functions SHOULD be defined in the custom mathematics
 *               library ('lib_math.*').  #### However, to maintain backwards compatibility with previously-
 *               released modules, mathematical macro & function definitions should only be moved to the
 *               custom mathematics library once all previously-released modules are updated to include the
 *               custom mathematics library.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   DEF_MIN()
 *
 * @brief    Determine the minimum of two values.
 *
 * @param    a   First  value.
 *
 * @param    b   Second value.
 *
 * @return   Minimum of the two values.
 *******************************************************************************************************/

#define  DEF_MIN(a, b)                                  (((a) < (b)) ? (a) : (b))

/****************************************************************************************************//**
 *                                                   DEF_MAX()
 *
 * @brief    Determine the maximum of two values.
 *
 * @param    a   First  value.
 *
 * @param    b   Second value.
 *
 * @return   Maximum of the two values.
 *******************************************************************************************************/

#define  DEF_MAX(a, b)                                  (((a) > (b)) ? (a) : (b))

/****************************************************************************************************//**
 *                                                   DEF_ABS()
 *
 * @brief    Determine the absolute value of a value.
 *
 * @param    a   Value to calculate absolute value.
 *
 * @return   Absolute value of the value.
 *******************************************************************************************************/

#define  DEF_ABS(a)                                     (((a) < 0) ? (-(a)) : (a))

/****************************************************************************************************//**
 *                                           DEF_IS_ADDR_ALIGNED()
 *
 * @brief    Determine if specified address is aligned.
 *
 * @param    addr    Memory address to evaluate.
 *
 * @param    align   Alignment needed (in bytes).
 *
 * @return   DEF_YES     Address is aligned.
 *           DEF_NO      Address is not aligned.
 *
 * @note     (1) Align value MUST be a power of 2.
 *******************************************************************************************************/

#define  DEF_ADDR_IS_ALIGNED(addr, align)                ((((addr) & ((align) - 1u)) == 0u) ? (DEF_YES) : (DEF_NO))

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of lib def module include.
