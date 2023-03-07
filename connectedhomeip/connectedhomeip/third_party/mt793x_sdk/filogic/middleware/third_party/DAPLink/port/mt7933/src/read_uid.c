/*
*
* Copyright (C) 2021 MediaTek Inc., this file is modified on 2/7/2022  * by MediaTek Inc. based on Apache License, Version 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
*
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "read_uid.h"
#include "hal_boot.h"

void read_unique_id(uint32_t *id)
{
    id[0] = 0x24547933;
    id[1] = hal_boot_get_hw_ver();
    id[2] = hal_boot_get_fw_ver();
    id[3] = 0xA5A5A5A5;
}

