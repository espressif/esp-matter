/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
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
 */
/** ============================================================================
 *  @file       DeviceFamily.h
 *
 *  @brief      Infrastructure to select correct driverlib path and identify devices
 *
 *  This module enables the selection of the correct driverlib path for the current
 *  device. It also facilitates the use of per-device conditional compilation
 *  to enable minor variations in drivers between devices.
 *
 *  In order to use this functionality, DeviceFamily_XYZ must be defined as one of
 *  the supported values. The DeviceFamily_ID and DeviceFamily_DIRECTORY defines
 *  are set based on DeviceFamily_XYZ.
 */

#ifndef ti_devices_DeviceFamily__include
#define ti_devices_DeviceFamily__include

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DeviceFamily_ID_XYZ values.
 *
 * DeviceFamily_ID may be used in the preprocessor for conditional compilation.
 * DeviceFamily_ID is set to one of these values based on the top level
 * DeviceFamily_XYZ define.
 */
#define DeviceFamily_ID_CC13X0                 1
#define DeviceFamily_ID_CC26X0                 2
#define DeviceFamily_ID_CC26X0R2               3
#define DeviceFamily_ID_CC13X2                 4
#define DeviceFamily_ID_CC26X2                 5
#define DeviceFamily_ID_CC3200                 6
#define DeviceFamily_ID_CC3220                 7
#define DeviceFamily_ID_CC13X1                 8
#define DeviceFamily_ID_CC26X1                 9
#define DeviceFamily_ID_CC13X2X7               10
#define DeviceFamily_ID_CC26X2X7               11
#define DeviceFamily_ID_CC13X1_CC26X1          12
#define DeviceFamily_ID_CC13X2_CC26X2          13
#define DeviceFamily_ID_CC13X2X7_CC26X2X7      14
#define DeviceFamily_ID_CC13X4_CC26X4          15
#define DeviceFamily_ID_CC13X4                 16
#define DeviceFamily_ID_CC26X4                 17
#define DeviceFamily_ID_CC26X3                 18
#define DeviceFamily_ID_CC23X0                 19

/*
 * DeviceFamily_PARENT_XYZ values.
 *
 * DeviceFamily_PARENT may be used in the preprocessor for conditional
 * compilation. DeviceFamily_PARENT is set to one of these values based
 * on the top-level DeviceFamily_XYZ define.
 */
#define DeviceFamily_PARENT_CC13X0_CC26X0           1
#define DeviceFamily_PARENT_CC13X2_CC26X2           2
#define DeviceFamily_PARENT_CC13X1_CC26X1           3
#define DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4    4
#define DeviceFamily_PARENT_CC23X0                  6

/*
 * Lookup table that sets DeviceFamily_ID, DeviceFamily_DIRECTORY, and
 * DeviceFamily_PARENT based on the DeviceFamily_XYZ define.
 * If DeviceFamily_XYZ is undefined, a compiler error is thrown. If
 * multiple DeviceFamily_XYZ are defined, the first one encountered is used.
 */
#if defined(DeviceFamily_CC13X0)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X0
    #define DeviceFamily_DIRECTORY      cc13x0
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X0_CC26X0

#elif defined(DeviceFamily_CC13X1)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X1
    #define DeviceFamily_DIRECTORY      cc13x1_cc26x1
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X1_CC26X1

#elif defined(DeviceFamily_CC13X2)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X2
    #define DeviceFamily_DIRECTORY      cc13x2_cc26x2
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC13X2X7)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X2X7
    #define DeviceFamily_DIRECTORY      cc13x2x7_cc26x2x7
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC13X4)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X4
    #define DeviceFamily_DIRECTORY      cc13x4_cc26x4
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4

#elif defined(DeviceFamily_CC26X0)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X0
    #define DeviceFamily_DIRECTORY      cc26x0
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X0_CC26X0

#elif defined(DeviceFamily_CC26X0R2)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X0R2
    #define DeviceFamily_DIRECTORY      cc26x0r2
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X0_CC26X0

#elif defined(DeviceFamily_CC26X1)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X1
    #define DeviceFamily_DIRECTORY      cc13x1_cc26x1
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X1_CC26X1

#elif defined(DeviceFamily_CC26X2)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X2
    #define DeviceFamily_DIRECTORY      cc13x2_cc26x2
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC26X2X7)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X2X7
    #define DeviceFamily_DIRECTORY      cc13x2x7_cc26x2x7
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC26X3)
    /* The CC2653 is very similar to CC26X4 from a software point of view,
     * so we use the same directory and parent defines.
     */
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X3
    #define DeviceFamily_DIRECTORY      cc13x4_cc26x4
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4

#elif defined(DeviceFamily_CC26X4)
    #define DeviceFamily_ID             DeviceFamily_ID_CC26X4
    #define DeviceFamily_DIRECTORY      cc13x4_cc26x4
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4

#elif defined(DeviceFamily_CC23X0)
    #define DeviceFamily_ID             DeviceFamily_ID_CC23X0
    #define DeviceFamily_DIRECTORY      cc23x0
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC23X0

#elif defined(DeviceFamily_CC13X1_CC26X1)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X1_CC26X1
    #define DeviceFamily_DIRECTORY      cc13x1_cc26x1
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X1_CC26X1

#elif defined(DeviceFamily_CC13X2_CC26X2)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X2_CC26X2
    #define DeviceFamily_DIRECTORY      cc13x2_cc26x2
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC13X2X7_CC26X2X7)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X2X7_CC26X2X7
    #define DeviceFamily_DIRECTORY      cc13x2x7_cc26x2x7
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X2_CC26X2

#elif defined(DeviceFamily_CC13X4_CC26X4)
    #define DeviceFamily_ID             DeviceFamily_ID_CC13X4_CC26X4
    #define DeviceFamily_DIRECTORY      cc13x4_cc26x4
    #define DeviceFamily_PARENT         DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4

#elif defined(DeviceFamily_CC3200)
    #define DeviceFamily_ID             DeviceFamily_ID_CC3200
    #define DeviceFamily_DIRECTORY      cc32xx

#elif defined(DeviceFamily_CC3220)
    #define DeviceFamily_ID             DeviceFamily_ID_CC3220
    #define DeviceFamily_DIRECTORY      cc32xx

#else
    #error "DeviceFamily_XYZ undefined. You must define a DeviceFamily_XYZ!"
#endif

/* Ensure that only one DeviceFamily was specified */
#if (defined(DeviceFamily_CC13X0) + defined(DeviceFamily_CC13X1)            \
    + defined(DeviceFamily_CC13X2) + defined(DeviceFamily_CC13X2X7)         \
    + defined(DeviceFamily_CC26X0) + defined(DeviceFamily_CC26X0R2)         \
    + defined(DeviceFamily_CC26X1) + defined(DeviceFamily_CC26X2)           \
    + defined(DeviceFamily_CC26X2X7) + defined(DeviceFamily_CC26X3)         \
    + defined(DeviceFamily_CC26X4) + defined(DeviceFamily_CC13X4)           \
    + defined(DeviceFamily_CC23X0)                                          \
    + defined(DeviceFamily_CC3200) + defined(DeviceFamily_CC3220)           \
    ) > 1
    #error More then one DeviceFamily has been defined!
#endif

/*!
 *  @brief  Macro to include correct driverlib path.
 *
 *  @pre    DeviceFamily_XYZ which sets DeviceFamily_DIRECTORY must be defined
 *          first.
 *
 *  @param  x   A token containing the path of the file to include based on
 *              the root device folder. The preceding forward slash must be
 *              omitted. For example:
 *                  - #include DeviceFamily_constructPath(inc/hw_memmap.h)
 *                  - #include DeviceFamily_constructPath(driverlib/ssi.h)
 *
 *  @return Returns an include path.
 *
 */
#define DeviceFamily_constructPath(x) <ti/devices/DeviceFamily_DIRECTORY/x>

#ifdef __cplusplus
}
#endif

#endif /* ti_devices_DeviceFamily__include */
