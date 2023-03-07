/***************************************************************************//**
 * @brief Compatibility macros for bgcommon
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
#ifndef SL_BTMESH_COMPATIBILITY_MACROS_H_
#define SL_BTMESH_COMPATIBILITY_MACROS_H_
#include <stdint.h>

/*
   IAR requires that macros are given before variable definition.
   For example:
   WEAK void func() {}
   PACKSTRUCT(struct foo { int bar; });
   ALIGNED(256) struct foo bar;
 */

#if defined(__IAR_SYSTEMS_ICC__)
/* IAR ICC */
  #define __STRINGIFY(a) #a
  #ifndef ALIGNED
  #define ALIGNED(alignment)    _Pragma(__STRINGIFY(data_alignment = alignment))
  #endif
  #ifndef PACKSTRUCT
  #define PACKSTRUCT(decl)      __packed decl
  #endif
  #define WEAK                  __weak
  #define KEEP_SYMBOL           __root
#ifdef NDEBUG
  #define OPTIMIZE_SPEED        _Pragma(__STRINGIFY(optimize = speed high))
#else
/* Increasing optimization is not allowed */
  #define OPTIMIZE_SPEED
#endif
#elif defined(__GNUC__)
/* GNU GCC */
  #ifndef ALIGNED
  #define ALIGNED(alignment)    __attribute__((aligned(alignment)))
  #endif
  #ifndef PACKSTRUCT
  #if defined(_WIN32)
    #define PACKSTRUCT(decl)    decl __attribute__((packed, gcc_struct))
  #else
    #define PACKSTRUCT(decl)    decl __attribute__((packed))
  #endif
  #endif
  #define WEAK                  __attribute__((weak))
  #define KEEP_SYMBOL           __attribute__((used))
#ifdef __clang__
// clang does not support optimize attribute
  #define OPTIMIZE_SPEED
#else
  #define OPTIMIZE_SPEED        __attribute__((optimize("O3")))
#endif
#else
/* Unknown */
#ifndef PACKSTRUCT
  #define PACKSTRUCT(decl) decl
#endif
  #define WEAK
  #define KEEP_SYMBOL
#endif

#ifndef PACKED
    #ifdef __GNUC__ //GNU Packed definition
        #define PACKED __attribute__((packed))
    #elif defined(__CWCC__)   //Codewarrior
        #define PACKED
        #pragma options align=packed
    #elif defined(__IAR_SYSTEMS_ICC__)
        #define PACKED
    #else
        #define PACKED
    #endif
#endif

#ifndef ALIGNED_WORD_SIZE
    #define ALIGNED_WORD_SIZE   (4)
#endif

#ifndef PACKSTRUCT
    #ifdef __GNUC__
        #ifdef _WIN32
            #define PACKSTRUCT(decl) decl __attribute__((__packed__, gcc_struct))
        #else
            #define PACKSTRUCT(decl) decl __attribute__((__packed__))
        #endif
        #define ALIGNED __attribute__((aligned(ALIGNED_WORD_SIZE)))
    #elif defined(__IAR_SYSTEMS_ICC__)
        #define PACKSTRUCT(decl) __packed decl
        #define ALIGNED
    #elif _MSC_VER  //msvc
        #define PACKSTRUCT(decl) __pragma(pack(push, 1) ) decl __pragma(pack(pop) )
        #define ALIGNED
    #else
        #define PACKSTRUCT(a) a PACKED
    #endif
#endif

#ifndef WEAK
    #if defined(__IAR_SYSTEMS_ICC__)
/* IAR ICC*/
        #define WEAK __weak
    #elif defined(__GNUC__)
/* GNU GCC */
        #define WEAK __attribute__ ((weak))
    #else
/* Unknown */
        #define WEAK
    #endif
#endif

#endif /* SL_BTMESH_COMPATIBILITY_MACROS_H_ */
