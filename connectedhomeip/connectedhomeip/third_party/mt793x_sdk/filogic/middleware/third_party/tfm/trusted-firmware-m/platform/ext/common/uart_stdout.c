/*
 * Copyright (c) 2017-2019 ARM Limited
 *
 * Licensed under the Apace License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apace.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uart_stdout.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "Driver_USART.h"
#include "target_cfg.h"
#include "device_cfg.h"

#define ASSERT_HIGH(X)  assert(X == ARM_DRIVER_OK)

/* Imports USART driver */
extern ARM_DRIVER_USART TFM_DRIVER_STDIO;

int stdio_output_string(const unsigned char *str, uint32_t len)
{
    int32_t ret;

    ret = TFM_DRIVER_STDIO.Send(str, len);
    if (ret != ARM_DRIVER_OK) {
        return 0;
    }
    /* Add a busy wait after sending. */
    while (TFM_DRIVER_STDIO.GetStatus().tx_busy);

    return TFM_DRIVER_STDIO.GetTxCount();
}

/* Redirects printf to TFM_DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
/* Struct FILE is implemented in stdio.h. Used to redirect printf to
 * TFM_DRIVER_STDIO
 */
FILE __stdout;
/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
    (void)f;

    /* Send byte to USART */
    (void)stdio_output_string((const unsigned char *)&ch, 1);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to TFM_DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
    (void)fd;

    /* Send string and return the number of characters written */
    return stdio_output_string((const unsigned char *)str, (uint32_t)len);
}
#elif defined(__ICCARM__)
int putchar(int ch)
{
    /* Send byte to USART */
    (void)stdio_output_string((const unsigned char *)&ch, 1);

    /* Return character written */
    return ch;
}
#endif

void stdio_init(void)
{
    int32_t ret;
    ret = TFM_DRIVER_STDIO.Initialize(NULL);
    ASSERT_HIGH(ret);
}

void stdio_uninit(void)
{
    int32_t ret;

    (void)TFM_DRIVER_STDIO.PowerControl(ARM_POWER_OFF);

    ret = TFM_DRIVER_STDIO.Uninitialize();
    ASSERT_HIGH(ret);
}
