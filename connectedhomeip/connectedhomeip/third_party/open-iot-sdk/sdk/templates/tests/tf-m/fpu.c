/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#if !defined(__ARM_FP) || (__ARM_FP == 0U)
#error This test needs to be compiled with hardware floating point
#endif

#include <math.h>
#include <stdio.h>

int main()
{
    printf("Test: TF-M permits non-secure access to FPU\r\n");

    const float base = 1.234f;
    const float exponent = 5.678f;
    const float result = powf(base, exponent);
    printf("%f to the power of %f equals %f\r\n", base, exponent, result);

    printf("Done\r\n");

    return 0;
}
