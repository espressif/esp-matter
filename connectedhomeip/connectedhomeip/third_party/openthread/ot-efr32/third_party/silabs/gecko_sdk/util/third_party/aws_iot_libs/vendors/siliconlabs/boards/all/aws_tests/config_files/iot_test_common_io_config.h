/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/

/*
 * FreeRTOS Common IO V0.1.2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * Unit Test Board Config for Common I/O
 * @File: iot_test_common_io_config.h
 * @Brief: File for define board configuation fro IOT HAL test
 ******************************************************************************/

#define IOT_TEST_COMMON_IO_ADC_SUPPORTED             1
#define IOT_TEST_COMMON_IO_EFUSE_SUPPORTED           1
#define IOT_TEST_COMMON_IO_FLASH_SUPPORTED           1
#define IOT_TEST_COMMON_IO_GPIO_SUPPORTED            2
#define IOT_TEST_COMMON_IO_I2C_SUPPORTED             1
#define IOT_TEST_COMMON_IO_POWER_SUPPORTED           1
#define IOT_TEST_COMMON_IO_PWM_SUPPORTED             1
#define IOT_TEST_COMMON_IO_RESET_SUPPORTED           1
#define IOT_TEST_COMMON_IO_RTC_SUPPORTED             1
#define IOT_TEST_COMMON_IO_SPI_SUPPORTED             1
#define IOT_TEST_COMMON_IO_TIMER_SUPPORTED           1
#define IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED     1
#define IOT_TEST_COMMON_IO_UART_SUPPORTED            1
#define IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED        1
