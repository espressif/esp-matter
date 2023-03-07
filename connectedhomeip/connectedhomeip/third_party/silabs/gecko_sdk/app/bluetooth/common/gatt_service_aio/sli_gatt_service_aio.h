/***************************************************************************//**
 * @file
 * @brief Automation IO GATT service internal header
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SLI_GATT_SERVICE_AIO_H
#define SLI_GATT_SERVICE_AIO_H

// AIO digital states defined by the BLE standard
#define AIO_DIGITAL_STATE_INACTIVE  0
#define AIO_DIGITAL_STATE_ACTIVE    1
#define AIO_DIGITAL_STATE_TRISTATE  2
#define AIO_DIGITAL_STATE_UNKNOWN   3

#define AIO_DIGITAL_STATE_MASK      3
#define AIO_DIGITAL_STATE_SIZE      2

// AIO digital state is represented on 1 byte, thus 4 digitals are supported
#define AIO_DIGITAL_COUNT_MAX       4

uint8_t aio_digital_in_get_num(void);
uint8_t aio_digital_in_get_state(void);
uint8_t aio_digital_out_get_num(void);
uint8_t aio_digital_out_get_state(void);
void aio_digital_out_set_state(uint8_t state);

#endif // SLI_GATT_SERVICE_AIO_H
