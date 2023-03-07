/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SERIAL_PRINTF_H
#define SERIAL_PRINTF_H

#define IOTSDK_SERIAL_PRINTF_MAX_SIZE 128

/**
 * Format and print a string to the serial peripheral.
 *
 * Its usage is identical printf(), except it has no return value for
 * error reporting due to the lack of return values of MCU-Driver-HAL's
 * serial API. This function is used by the examples that do not have
 * printf() retargeted.
 *
 * @note The buffer for storing a formatted string has size defined by
 *       IOTSDK_SERIAL_PRINTF_MAX_SIZE. If the string to print exceeds
 *       this size, the part beyond this size will be truncated.
 *
 * @param fmt The string to format
 * @param ... The value to insert for each format specifier
 */
void serial_printf(const char *fmt, ...);

#endif // SERIAL_PRINTF_H
