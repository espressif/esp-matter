/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/

#ifndef __DEFINITIONS_H__ 
#define __DEFINITIONS_H__ 

/* Identification Strings */
#define LIBNAME "Audio Framework"

/* LIB/API version information is directly modified by the packing script, do not change */
#define LIBVERSION "1.3p5_Alpha"
#define LIB_APIVERSION_MAJOR 1
#define LIB_APIVERSION_MINOR 2

#if LIB_APIVERSION_MAJOR != XA_APIVERSION_MAJOR || \
LIB_APIVERSION_MINOR != XA_APIVERSION_MINOR
#error "Version Mismatch"
#endif

#define LIB_APIVERSION              XA_MAKE_VERSION_STR(\
                                    LIB_APIVERSION_MAJOR, \
                                    LIB_APIVERSION_MINOR)

#endif /* __DEFINITIONS_H__ */
