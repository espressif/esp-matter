/* 
 *  Copyright (c) 2013 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
/*
 *  ======== iar/targets/arm/std.h ========
 *
 */

#ifndef iar_targets_arm_STD_
#define iar_targets_arm_STD_

/* allow _TI_STD_TYPES like 'Uns' and 'Uint8' */
#ifndef xdc__deprecated_types
#define xdc__deprecated_types
#endif

/* include target-specific "portable" macros */
#if defined(xdc_target_name__) & !defined(xdc_target_macros_include__)
#include xdc__local_include(xdc_target_name__)
#endif

#include <stdint.h>     /* C99 standard integer types */

/* define the actual IAR stack base symbol */
#define __TI_STACK_BASE CSTACK$$Base

/*
 * xdc__LONGLONG__ indicates if compiler supports 'long long' type
 * xdc__BITS<n> __ indicates if compiler supports 'uint<n>_t' type
 */
#define xdc__LONGLONG__
#define xdc__BITS8__
#define xdc__BITS16__
#define xdc__BITS32__
#define xdc__BITS64__
#define xdc__INT64__
/*
 *  ======== [U]Int<n> ========
 */
typedef int_least8_t        xdc_Int8;
typedef uint_least8_t       xdc_UInt8;
typedef int_least16_t       xdc_Int16;
typedef uint_least16_t      xdc_UInt16;
typedef int_least32_t       xdc_Int32;
typedef uint_least32_t      xdc_UInt32;
typedef int_least64_t       xdc_Int64;
typedef uint_least64_t      xdc_UInt64;

/*
 *  ======== Bits<n> ========
 */
typedef uint8_t       xdc_Bits8;
typedef uint16_t      xdc_Bits16;
typedef uint32_t      xdc_Bits32;
typedef uint64_t      xdc_Bits64;

/*
 *  ======== [UI]Arg ========
 */
typedef intptr_t        xdc_IArg;
typedef uintptr_t       xdc_UArg;

/*
 *  ======== xdc__META ========
 */
#if (__VER__ >= 7010000)
#define xdc__META(n,s) __no_alloc const char (n)[] @ "xdc.meta" = {(s)}
#endif

#endif /* iar_targets_arm_STD_ */
/*
 *  @(#) iar.targets.arm; 1, 0, 0,; 7-28-2021 06:57:36; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

