/*
 * Copyright (c) 2018-2019 Arm Limited
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
 * \file musca_b1_eflash_drv.c
 *
 * \brief Process specifinc implementation of GFC100 flash controller for
 *        Musca B1 board.
 */

#include "gfc100_process_spec_api.h"

void gfc100_proc_spec_set_eflash_timing(uint32_t reg_map_base,
                                        uint32_t sys_clk)
{
    (void)sys_clk;

    *(uint32_t *)reg_map_base = 0x11082801;
    *(uint32_t *)(reg_map_base + 4U) = 0x64050208;
    *(uint32_t *)(reg_map_base + 8U) = 0xa0a0a08;
}

uint32_t gfc100_proc_spec_get_eflash_word_width(uint32_t reg_map_base)
{
    (void)reg_map_base;

    return 128U;
}

uint32_t gfc100_proc_spec_get_eflash_size(uint32_t reg_map_base)
{
    (void)reg_map_base;

    return 0x200000U;
}

uint32_t gfc100_proc_spec_get_eflash_page_size(uint32_t reg_map_base)
{
    (void)reg_map_base;

    return 0x4000;
}

uint32_t gfc100_proc_spec_get_num_of_info_pages(uint32_t reg_map_base)
{
    (void)reg_map_base;

    return 3U;
}

uint32_t gfc100_proc_spec_get_error_cause(uint32_t reg_map_base)
{
    return *(uint32_t *)(reg_map_base + 0x18);
}

