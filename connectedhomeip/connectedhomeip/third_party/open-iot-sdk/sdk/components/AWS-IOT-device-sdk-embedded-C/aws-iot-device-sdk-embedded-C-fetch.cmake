# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
  aws-iot-device-sdk

  GIT_REPOSITORY https://github.com/aws/aws-iot-device-sdk-embedded-C
  GIT_TAG        75e545b0e807ab6dff9bcb0ee5942e9a58435b10
  GIT_SHALLOW    OFF
  GIT_PROGRESS   ON

)

FetchContent_Populate(aws-iot-device-sdk)
FetchContent_GetProperties(aws-iot-device-sdk)
