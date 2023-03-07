# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
elseif(pigweed_POPULATED)
    # Null version of the logging lock
    add_library(pw_log_mdh_null_lock STATIC
        null_lock.c
    )

    if(TARGET cmsis-rtos-api)
        # CMSIS RTOS version of the logging lock
        add_library(pw_log_mdh_cmsis_rtos_lock STATIC
            cmsis_rtos_lock.c
        )

        target_link_libraries(pw_log_mdh_cmsis_rtos_lock
            PRIVATE
                cmsis-rtos-api
        )
    endif()
endif()
