/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 *  ======== std.h ========
 */
#ifndef xdc_std__include
#define xdc_std__include

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 
 * legacy TI tools pre-define "__TI_COMPILER_VERSION__"
 * TI clang tools pre-define "__ti_version__"
 * we should stop using__ti__, but until then ...
 */
#if defined (__TI_COMPILER_VERSION__) || defined(__ti_version__)
#ifndef __ti__
#define __ti__
#endif
#endif

#ifndef TRUE
#define TRUE            1
#define FALSE           0
#endif

#define xdc_Void        void

typedef bool            xdc_Bool;
typedef char            xdc_Char;
typedef unsigned char   xdc_UChar;
typedef short           xdc_Short;
typedef unsigned short  xdc_UShort;
typedef int             xdc_Int;
typedef unsigned int    xdc_UInt;
typedef long            xdc_Long;
typedef unsigned long   xdc_ULong;
typedef long long       xdc_LLong;
typedef unsigned long long xdc_ULLong;
typedef float           xdc_Float;
typedef double          xdc_Double;
typedef long double     xdc_LDouble;
typedef size_t          xdc_SizeT;
typedef va_list         xdc_VaList;

/* Generic Extended Types */

typedef void            *xdc_Ptr;
typedef const void      *xdc_CPtr;
typedef char            *xdc_String;
typedef const char      *xdc_CString;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
typedef int             (*xdc_Fxn)();       /* function pointer */
#pragma GCC diagnostic pop
#else
typedef int             (*xdc_Fxn)();       /* function pointer */
#endif

typedef intptr_t        xdc_IArg;
typedef uintptr_t       xdc_UArg;

/* 8-bit types are not supported on some targets */
typedef int8_t          xdc_Int8;
typedef uint8_t         xdc_UInt8;
typedef uint8_t         xdc_Bits8;

typedef int16_t         xdc_Int16;
typedef uint16_t        xdc_UInt16;
typedef uint16_t        xdc_Bits16;

typedef int32_t         xdc_Int32;
typedef uint32_t        xdc_UInt32;
typedef uint32_t        xdc_Bits32;

/* 32-bit types are not supported on some targets */
typedef int64_t         xdc_Int64;
typedef uint64_t        xdc_UInt64;
typedef uint64_t        xdc_Bits64;

/* conversion macros (inline functions) */
static inline xdc_Ptr xdc_iargToPtr(xdc_IArg a) { return ((xdc_Ptr)a); }
static inline xdc_Ptr xdc_uargToPtr(xdc_UArg a) { return ((xdc_Ptr)a); }
static inline xdc_Fxn xdc_iargToFxn(xdc_IArg a) { return ((xdc_Fxn)a); }
static inline xdc_Fxn xdc_uargToFxn(xdc_UArg a) { return ((xdc_Fxn)a); }

/*
 * functions to efficiently convert a single precision float to an IArg
 * and vice-versa while maintaining client type safety
 *
 * Here the assumption is that sizeof(Float) <= sizeof(IArg);
 */
typedef union {
    xdc_Float f;
    xdc_IArg  a;
} xdc_FloatData;

static inline xdc_IArg xdc_floatToArg(xdc_Float f)
{
     xdc_FloatData u;
     u.f = f;

     return (u.a);
}

static inline xdc_Float xdc_argToFloat(xdc_IArg a)
{
     xdc_FloatData u;
     u.a = a;

     return (u.f);
}

/*
 *  short names ...
 */

#define Void            void

#define iargToPtr       xdc_iargToPtr
#define uargToPtr       xdc_uargToPtr
#define iargToFxn       xdc_iargToFxn
#define uargToFxn       xdc_uargToFxn
#define argToFloat      xdc_argToFloat
#define floatToArg      xdc_floatToArg

typedef xdc_Bool        Bool;
typedef xdc_Char        Char;
typedef xdc_UChar       UChar;
typedef xdc_Short       Short;
typedef xdc_UShort      UShort;
typedef xdc_Int         Int;
typedef xdc_UInt        UInt;
typedef xdc_Long        Long;
typedef xdc_ULong       ULong;
typedef xdc_LLong       LLong;
typedef xdc_ULLong      ULLong;
typedef xdc_Float       Float;
typedef xdc_Double      Double;
typedef xdc_LDouble     LDouble;
typedef xdc_SizeT       SizeT;
typedef xdc_VaList      VaList;
typedef xdc_Ptr         Ptr;
typedef xdc_CPtr        CPtr;
typedef xdc_String      String;
typedef xdc_CString     CString;
typedef xdc_Fxn         Fxn;
typedef xdc_IArg        IArg;
typedef xdc_UArg        UArg;

/* 8-bit types are not supported on some targets */
typedef xdc_Int8        Int8;
typedef xdc_UInt8       UInt8;
typedef xdc_Bits8       Bits8;

typedef xdc_Int16       Int16;
typedef xdc_UInt16      UInt16;
typedef xdc_Bits16      Bits16;

typedef xdc_Int32       Int32;
typedef xdc_UInt32      UInt32;
typedef xdc_Bits32      Bits32;

/* 32-bit types are not supported on some targets */
typedef xdc_Int64       Int64;
typedef xdc_UInt64      UInt64;
typedef xdc_Bits64      Bits64;

/* DEPRECATED Aliases */
#ifndef xdc__deprecated_types
#define xdc__deprecated_types 1
#endif
#if xdc__deprecated_types
#define _TI_STD_TYPES
typedef xdc_UArg        Arg;

typedef xdc_UInt8       Uint8;
typedef xdc_UInt16      Uint16;
typedef xdc_UInt32      Uint32;
typedef xdc_UInt        Uns;
#endif

/* these macros are used when building in the package.bld flow */

#ifndef __FAR__
#define __FAR__
#endif

/*
 *  ======== xdc__CODESECT ========
 *  Code-Section Directive
 *
 *  Targets can optionally #define xdc__CODESECT in their specific
 *  std.h files.  This directive is placed in front of all
 *  "extern" function declarations, and specifies a section-name in
 *  which to place this function.  This approach
 *  provides more control on combining/organizing groups of
 *  related functions into a single named sub-section (e.g.,
 *  "init-code")  If this macro is not defined by the target, an
 *  empty definition is used instead.
 */
#ifndef xdc__CODESECT
#define xdc__CODESECT(fn, sn)
#endif

/*
 *  ======== xdc__META ========
 *  Embed unreferenced string in the current file
 *
 *  Strings emebdded via xdc__META can be placed in a section that is
 *  _not_ loaded on the target but are, nevertheless, part of the
 *  executable and available to loaders.
 *
 *  Different targets may define this macro in a way that places these
 *  strings in an output section that is not loaded (and therefore does
 *  not takeup space on the target).  Unless the target provides a
 *  definition of xdc__META, the definition below simply defines
 *  as string constant in the current file.
 */
#ifndef xdc__META
#define xdc__META(n,s) __FAR__ const char (n)[] = {(s)}
#endif

#endif /* xdc_std__include */
