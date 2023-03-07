# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Adds a `covreport` target to generate a code coverage report using `gcovr`.
# Build this target using `cmake --build <build-dir> --target covereport` AFTER
# executing your tests with coverage instrumentation enabled.

if(NOT DEFINED Python3_FOUND)
    find_package(Python3 COMPONENTS Interpreter)
endif()

if(NOT Python3_FOUND)
    message(WARNING
        "Python3 is not installed. Python3 is required for gcovr. Code coverage report generation will not be enabled."
    )
    return()
endif()

find_program(GCOVR gcovr)

if(NOT GCOVR)
    message(WARNING
        "gcovr was not found. Install gcovr with `pip install gcovr` if you want to generate a code coverage report."
    )
    return()
endif()

set(GCOVR_FAIL_UNDER_LINE 0 CACHE STRING "gcovr will fail if the line coverage is below this percentage")

add_custom_target(covreport
    COMMAND ${GCOVR} --root ${PROJECT_SOURCE_DIR}
    --exclude ${FETCHCONTENT_BASE_DIR} --exclude-unreachable-branches
    --fail-under-line ${GCOVR_FAIL_UNDER_LINE} --print-summary --xml-pretty
    -o ${CMAKE_BINARY_DIR}/coverage.xml
)
