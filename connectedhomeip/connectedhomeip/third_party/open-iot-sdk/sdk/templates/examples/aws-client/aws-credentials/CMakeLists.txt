# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Provide AWS credentials as an interface library
add_library(aws-credentials INTERFACE)
target_include_directories(aws-credentials
    INTERFACE
        .
)
