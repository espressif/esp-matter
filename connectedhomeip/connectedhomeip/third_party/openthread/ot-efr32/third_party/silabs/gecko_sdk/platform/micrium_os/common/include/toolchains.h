/***************************************************************************//**
 * @file
 * @brief Common - Toolchains Utils
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
 * @defgroup COMMON_TOOLCHAIN Toolchain abstraction API
 * @ingroup  COMMON
 * @brief      Toolchain abstraction API
 *
 * @addtogroup COMMON_TOOLCHAIN
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _TOOLCHAINS_H_
#define  _TOOLCHAINS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                 DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       C STANDARD VERSION DEFINES
 *******************************************************************************************************/

#define PP_C_STD_VERSION_C89         198900L
#define PP_C_STD_VERSION_C90         199000L
#define PP_C_STD_VERSION_C94         199409L
#define PP_C_STD_VERSION_C99         199901L

#if defined(__STDC_VERSION__)
    #if (__STDC_VERSION__ - 0 > 1)
        #define PP_C_STD_VERSION        __STDC_VERSION__
    #else
        #define PP_C_STD_VERSION        PP_C_STD_VERSION_C90
    #endif
#else
    #if defined(__STDC__)
        #define PP_C_STD_VERSION        PP_C_STD_VERSION_C89
    #endif
#endif

#if (PP_C_STD_VERSION - 0 >= PP_C_STD_VERSION_C89)
#define PP_C_STD_VERSION_C89_PRESENT
#endif

#if (PP_C_STD_VERSION - 0 >= PP_C_STD_VERSION_C90)
#define PP_C_STD_VERSION_C90_PRESENT
#endif

#if (PP_C_STD_VERSION - 0 >= PP_C_STD_VERSION_C94)
#define PP_C_STD_VERSION_C94_PRESENT
#endif

#if (PP_C_STD_VERSION - 0 >= PP_C_STD_VERSION_C99)
#define PP_C_STD_VERSION_C99_PRESENT
#endif

/********************************************************************************************************
 *                                           GENERAL MACROS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               PP_UNUSED_PARAM()
 *
 * @brief    Removes warning associated to a function parameter being present but not referenced in a
 *           given function.
 *
 * @param    param   Parameter that is unused.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 *******************************************************************************************************/

#ifndef  PP_UNUSED_PARAM

#if ((RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC) \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)   \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)   \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32))
#define  PP_UNUSED_PARAM(param)            (void)(param)
#else
#error  "Unknown toolchain. Will define PP_UNUSED_PARAM to nothing."
#define  PP_UNUSED_PARAM(param)
#endif

#endif

/****************************************************************************************************//**
 *                                               PP_ALIGN()
 *
 * @brief    Forces variable to be aligned on specific memory multiple.
 *
 * @param    _variable   Variable to align.
 *
 * @param    _align      Alignment required, in bytes.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 *
 * @note     (2) This macro is DEPRECATED and will be removed in a future version of this product.
 *               CMSIS __ALIGN should be used instead.
 *
 * @deprecated
 *******************************************************************************************************/

#ifndef  PP_ALIGN

#if ((RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC) \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU))
#define  PP_ALIGN(_variable, _align)                    _variable __attribute__ ((aligned(_align)))
#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
#define  _PP_ALIGN_STR(x)                               #x
#define  PP_ALIGN(_variable, _align)                    _Pragma(_PP_ALIGN_STR(data_alignment = _align)) \
  _variable
#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32)
#define  PP_ALIGN(_variable, _align)                    __declspec(align(_align))_variable
#else
#error  "Unknown toolchain. Will define PP_ALIGN to nothing."
#define  PP_ALIGN(_variable, _align)
#endif

#endif

/****************************************************************************************************//**
 *                                                   PP_WEAK()
 *
 * @brief    Abstracts weak toolchain specific pragma.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 ********************************************************************************************************
 */

#ifndef  PP_WEAK

#if (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
#define PP_WEAK  __weak
#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC)
#define PP_WEAK  __attribute__ ((weak))
#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)
#define PP_WEAK  __attribute__((weak))
#else
#define PP_WEAK
#endif

#endif

/**
 ********************************************************************************************************
 *                                         PP_WEAK_VAR_DECL()
 *
 * @brief    Declares a variable as weak.
 *
 * @param    type        Variable's type.
 *
 * @param    name        Variable's name.
 *
 * @param    init_value  Variable's initial value.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 *
 * @note     (2) Some toolchain won't allow to have a strong and a weak definition of a given symbol
 *               in the same compilation unit.
 *
 * @note     (3) Some toolchain do not support weak symbols. In those cases, a strong definition is
 *               required.
 *
 * @note     (4) Some toolchain require the use of #pragma to declare a symbol as weak. If _Pragma is
 *               not available (i.e. C99 is disabled), a strong definition is required.
 *
 * @note     (5) A bug exists in some version of IAR where the compiler can make some optimizations using
 *               a weak definition even though a strong definition exists somewhere else. This has
 *               been observed only when the type is a structure. Keep that in mind if you use this macro.
 *               For instance, using the volatile type qualifier in the weak definition prevents this
 *               optimization.
 *
 * @note     (6) This macro is DEPRECATED and will be removed in a future version of this product.
 *               CMSIS __WEAK should be used instead.
 *
 * @deprecated
 *******************************************************************************************************/

#ifndef  PP_WEAK_VAR_DECL

#if (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
// See note (5).
#define PP_WEAK_VAR_DECL(type, name, init_value)  __weak type name = init_value
#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC)
#define PP_WEAK_VAR_DECL(type, name, init_value)  type __attribute__ ((weak)) name = init_value

#elif (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)
#define PP_WEAK_VAR_DECL(type, name, init_value)  extern  type name __attribute__((weak))
#elif ((RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32))
#define PP_WEAK_VAR_DECL(type, name, init_value)  extern type name
#else
#define PP_WEAK_VAR_DECL(type, name, init_value)  extern type name
#endif

#endif

/********************************************************************************************************
 *                                           ISR-RELATED MACROS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               PP_ISR_DECL()
 *
 * @brief    Declare a function, indicating to the compiler it is an ISR.
 *
 * @param    _isr    The ISR function's name.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 *
 * @note     (2) Usage is as follows:
 *                @verbatim
 *               PP_ISR_DECL(My_ISR_Function);
 *                @endverbatim
 *
 * @note     (3) This macro is DEPRECATED and will be removed in a future version of this product.
 *
 * @deprecated
 *******************************************************************************************************/

#ifndef  PP_ISR_DECL

    #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_POSIX) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_WIN32)     \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMPTY))

        #define  PP_ISR_DECL(_isr)              void _isr(void)

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)         \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
            #define  PP_ISR_DECL(_isr)            void _isr(void)
        #else
            #warning  Toolchain does not define PP_ISR_DECL.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)         \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))

            #if    (__ARMCC_VERSION < 6050000)
                #define  PP_ISR_DECL(_isr)        __irq void _isr(void)
            #else
                #define  PP_ISR_DECL(_isr)      void _isr(void) __attribute__ ((interrupt("IRQ")))
            #endif

        #else
            #warning  Toolchain does not define PP_ISR_DECL.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)         \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
            #define  PP_ISR_DECL(_isr)          void _isr(void)
        #else
            #warning  Toolchain does not define PP_ISR_DECL.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32)
        #define  PP_ISR_DECL(_isr)
    #else
        #error  Unknown toolchain does not define PP_ISR_DECL.
    #endif

#endif

/****************************************************************************************************//**
 *                                               PP_ISR_DEF()
 *
 * @brief    Define a function, indicating to the compiler that it is an ISR.
 *
 * @param    _isr    The ISR function.
 *
 * @note     (1) This macro can be overriden by defining it first in the compiler options.
 *
 * @note     (2) Usage is as follows:
 *                @verbatim
 *               PP_ISR_DEF(My_ISR_Function)
 *               {
 *                   [...]
 *               }
 *                 @endverbatim
 *
 * @note     (3) This macro is DEPRECATED and will be removed in a future version of this product.
 *
 * @deprecated
 *******************************************************************************************************/

#ifndef  PP_ISR_DEF

    #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_POSIX) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_WIN32)     \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMPTY))
        #define  PP_ISR_DEF(_isr)                               PP_ISR_DECL(_isr)

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)         \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
            #define  PP_ISR_DEF(_isr)                               PP_ISR_DECL(_isr)
        #else
            #warning  Toolchain does not define PP_ISR_DEF.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M))
            #define  PP_ISR_DEF(_isr)                               PP_ISR_DECL(_isr)
        #else
            #warning  Toolchain does not define PP_ISR_DEF.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
        #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)         \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
            #define  PP_ISR_DEF(_isr)                               PP_ISR_DECL(_isr)
        #else
            #warning  Toolchain does not define PP_ISR_DEF.
        #endif

    #elif  (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32)
        #define  PP_ISR_DEF(_isr)
    #else
        #error  Unknown toolchain does not define PP_ISR_DEF.
    #endif

#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of preprocessor module include.
