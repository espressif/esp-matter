/*
 * Copyright (c) 2018 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
* \file gfc100_process_specific_api.h
*
* \brief Header file for the process specific part of the
*        GFC100 flash controller
*/

#ifndef __GFC100_PROCESS_SPEC_API_H__
#define __GFC100_PROCESS_SPEC_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \brief Sets timing parameters on the process specific part
 *
 * \param[in] reg_map_base  Process specific register map base
 * \param[in] sys_clk       System clock in Hz
 */
void gfc100_proc_spec_set_eflash_timing(uint32_t reg_map_base,
                                        uint32_t sys_clk);

/**
 * \brief Gets flash memory size
 *
 * \param[in] reg_map_base  Process specific register map base
 *
 * \return Returns the size of the flash memory
 */
uint32_t gfc100_proc_spec_get_eflash_size(uint32_t reg_map_base);

/**
 * \brief Gets flash page size
 *
 * \param[in] reg_map_base  Process specific register map base
 *
 * \return Returns the page size of the flash memory
 */
uint32_t gfc100_proc_spec_get_eflash_page_size(uint32_t reg_map_base);

/**
 * \brief Gets word width of the process specific part
 *
 * \param[in] reg_map_base  Process specific register map base
 *
 * \return Returns word width of the process specific part
 */
uint32_t gfc100_proc_spec_get_eflash_word_width(uint32_t reg_map_base);

/**
 * \brief Gets number of info pages
 *
 * \param[in] reg_map_base  Process specific register map base
 *
 * \return Returns the number of info pages from the extended area
 */
uint32_t gfc100_proc_spec_get_num_of_info_pages(uint32_t reg_map_base);

/**
 * \brief Gets process specific error bits
 *
 * \param[in] reg_map_base  Process specific register map base
 *
 * \return Returns the error bits specified by the process specific part
 *         of the controller.
 */
uint32_t gfc100_proc_spec_get_error_cause(uint32_t reg_map_base);

#ifdef __cplusplus
}
#endif

#endif /* __GFC100_PROCESS_SPEC_API_H__ */

