/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/serial_api.h"

#include <inttypes.h>

#define PW_LOG_MODULE_NAME "main"
#include "pw_log/log.h"

static mdh_serial_t *my_serial = NULL;

extern mdh_serial_t *get_example_serial();

void serial_setup()
{
    my_serial = get_example_serial();
    mdh_serial_set_baud(my_serial, 115200);
}

int main()
{
    serial_setup();
    pw_log_mdh_init(my_serial);

    PW_LOG_DEBUG("Debug log: %s", "Details about the execution");
    PW_LOG_INFO("Info log: %s", "What is going on");
    PW_LOG_WARN("Warning log: %s", "When something unexpected happen");
    PW_LOG_ERROR("Error log: %s", "In case of serious error");

    return 0;
}
