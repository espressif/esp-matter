# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Default set of warning flags for our applications

set(IOTSDK_WARNING_FLAGS
    -fdata-sections
    -fno-exceptions
    -Wall
    -Wextra
    -Wformat-security
    -Wno-format-zero-length
    -Werror=format-security
    -Wdouble-promotion
    -Wpointer-arith
    -Wnull-dereference
    -Wredundant-decls
    -Wshadow
    -Wswitch-default
    -Wcast-align
)

set(IOTSDK_WARNING_AS_ERRORS_FLAGS
    -Werror=format-security
    -Werror=implicit-function-declaration
)
