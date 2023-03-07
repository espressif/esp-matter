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

#include "DAP_config.h"
#include "gpio.h"
#include "daplink.h"
#include "util.h"
//MT7933
#include "bsp_gpio_ept_config.h"

void gpio_init(void)
{
#ifdef HAL_GPIO_MODULE_ENABLED
    bsp_ept_gpio_setting_init();
#endif
}

void gpio_set_hid_led(gpio_led_state_t state)
{
}

void gpio_set_cdc_led(gpio_led_state_t state)
{
}

void gpio_set_msc_led(gpio_led_state_t state)
{
}

uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return 0;
}

uint8_t gpio_get_reset_btn_fwrd(void)
{
    return 0;
}


uint8_t GPIOGetButtonState(void)
{
    return 0;
}

void target_forward_reset(bool assert_reset)
{
    // Do nothing - reset is forwarded in gpio_get_sw_reset
}

void gpio_set_board_power(bool powerEnabled)
{
}
