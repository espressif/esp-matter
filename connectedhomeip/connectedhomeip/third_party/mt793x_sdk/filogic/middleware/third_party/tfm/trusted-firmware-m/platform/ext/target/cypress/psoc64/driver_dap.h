/*
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
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

#ifndef __DRIVER_DAP_H__
#define __DRIVER_DAP_H__

/** DAPControl SysCall parameter: access port state */
enum cy_ap_control {
    CY_AP_DIS = 0,
    CY_AP_EN = 1
};

/** DAPControl SysCall parameter: access port name */
enum cy_ap_name {
    CY_CM0_AP = 0,
    CY_CM4_AP = 1,
    CY_SYS_AP = 2
};

/* API functions */
int cy_access_port_control(enum cy_ap_name ap, enum cy_ap_control en);

#endif /* __DRIVER_DAP_H__ */
