/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_BUS_H
#define SL_WFX_BUS_H

#include "sl_wfx_constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* High level WF200 bus API */
sl_status_t sl_wfx_init_bus(void);

sl_status_t sl_wfx_deinit_bus(void);

sl_status_t sl_wfx_reg_read(sl_wfx_register_address_t address, void *buffer, uint32_t length);

sl_status_t sl_wfx_reg_write(sl_wfx_register_address_t address, const void *buffer, uint32_t length);

sl_status_t sl_wfx_notify_bus_thread(void);

/* WF200 basic register API */
sl_status_t sl_wfx_reg_read_16(sl_wfx_register_address_t address, uint16_t *value_out);

sl_status_t sl_wfx_reg_write_16(sl_wfx_register_address_t address, uint16_t value_in);

sl_status_t sl_wfx_reg_read_32(sl_wfx_register_address_t address, uint32_t *value_out);

sl_status_t sl_wfx_reg_write_32(sl_wfx_register_address_t address, uint32_t value_in);

sl_status_t sl_wfx_data_read(void *buffer, uint32_t length);

sl_status_t sl_wfx_data_write(const void *buffer, uint32_t length);

sl_status_t sl_wfx_apb_write_32(uint32_t address, uint32_t value_in);

sl_status_t sl_wfx_apb_read_32(uint32_t address, uint32_t *value_out);

sl_status_t sl_wfx_apb_write(uint32_t address, const void *buffer, uint32_t length);

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif // SL_WFX_BUS_H
