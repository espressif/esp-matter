# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

set(dir_pw_third_party_freertos "" CACHE PATH
    "Path to the FreeRTOS installation's Source directory. \
     If set, pw_third_party.freertos is provided")
set(pw_third_party_freertos_CONFIG "" CACHE STRING
    "The CMake target which provides the FreeRTOS config header")
set(pw_third_party_freertos_PORT "" CACHE STRING
    "The CMake target which provides the port specific includes and sources")
option(pw_third_party_freertos_DISABLE_TASKS_STATICS
       "Whether to disable statics inside of tasks.c")

pw_add_library(pw_third_party.freertos.disable_warnings INTERFACE
  PUBLIC_COMPILE_OPTIONS
    -Wno-unused-parameter
    -Wno-cast-qual
)

# If FreeRTOS is not configured, a script that displays an error message is used
# instead. If the build rule is used in the build it fails with this error.
if(NOT dir_pw_third_party_freertos)
  pw_add_error_target(pw_third_party.freertos
    MESSAGE
      "Attempted to build the pw_third_party.freertos without configuring it "
      "via dir_pw_third_party_freertos. "
      "See https://pigweed.dev/third_party/freertos."
  )
  return()
else(dir_pw_third_party_freertos)
  if(NOT pw_third_party_freertos_PORT)
    message(FATAL_ERROR "FreeRTOS is being used, but "
            "pw_third_party_freertos_PORT is not set.")
  endif()
  if(NOT pw_third_party_freertos_CONFIG)
    message(FATAL_ERROR "FreeRTOS is being used, but "
            "pw_third_party_freertos_CONFIG is not set.")
  endif()

  pw_add_library(pw_third_party.freertos STATIC
    HEADERS
      ${dir_pw_third_party_freertos}/include/FreeRTOS.h
      ${dir_pw_third_party_freertos}/include/StackMacros.h
      ${dir_pw_third_party_freertos}/include/croutine.h
      ${dir_pw_third_party_freertos}/include/deprecated_definitions.h
      ${dir_pw_third_party_freertos}/include/event_groups.h
      ${dir_pw_third_party_freertos}/include/list.h
      ${dir_pw_third_party_freertos}/include/message_buffer.h
      ${dir_pw_third_party_freertos}/include/mpu_prototypes.h
      ${dir_pw_third_party_freertos}/include/mpu_wrappers.h
      ${dir_pw_third_party_freertos}/include/portable.h
      ${dir_pw_third_party_freertos}/include/projdefs.h
      ${dir_pw_third_party_freertos}/include/queue.h
      ${dir_pw_third_party_freertos}/include/semphr.h
      ${dir_pw_third_party_freertos}/include/stack_macros.h
      ${dir_pw_third_party_freertos}/include/stream_buffer.h
      ${dir_pw_third_party_freertos}/include/task.h
      ${dir_pw_third_party_freertos}/include/timers.h
    PUBLIC_INCLUDES
      ${dir_pw_third_party_freertos}/include
    PUBLIC_DEPS
      ${pw_third_party_freertos_CONFIG}
      ${pw_third_party_freertos_PORT}
    SOURCES
      ${dir_pw_third_party_freertos}/croutine.c
      ${dir_pw_third_party_freertos}/event_groups.c
      ${dir_pw_third_party_freertos}/list.c
      ${dir_pw_third_party_freertos}/queue.c
      ${dir_pw_third_party_freertos}/stream_buffer.c
      ${dir_pw_third_party_freertos}/timers.c
    PRIVATE_DEPS
      pw_third_party.freertos.freertos_tasks
      pw_third_party.freertos.disable_warnings
  )
endif()

if(pw_third_party_freertos_DISABLE_TASKS_STATICS)
  set(disable_tasks_statics "static=" "PW_THIRD_PARTY_FREERTOS_NO_STATICS=1")
endif()
pw_add_library(pw_third_party.freertos.freertos_tasks STATIC
  SOURCES
    ${dir_pw_third_party_freertos}/tasks.c
  PRIVATE_DEPS
    ${pw_third_party_freertos_CONFIG}
    ${pw_third_party_freertos_PORT}
    pw_third_party.freertos.disable_warnings
  PRIVATE_INCLUDES
    ${dir_pw_third_party_freertos}/include
  PRIVATE_DEFINES
    ${disable_tasks_statics}
)

# ARM CM7 port of FreeRTOS.
pw_add_library(pw_third_party.freertos.arm_cm7 STATIC
  HEADERS
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM7/r0p1/portmacro.h
  PUBLIC_DEPS
    ${pw_third_party_freertos_CONFIG}
  PUBLIC_INCLUDES
    ${dir_pw_third_party_freertos}/include
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM7/r0p1
  SOURCES
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM7/r0p1/port.c
  PRIVATE_DEPS
    pw_third_party.freertos.disable_warnings
)

# ARM CM4F port of FreeRTOS.
pw_add_library(pw_third_party.freertos.arm_cm4f STATIC
  HEADERS
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM4F/portmacro.h
  PUBLIC_DEPS
    ${pw_third_party_freertos_CONFIG}
  PUBLIC_INCLUDES
    ${dir_pw_third_party_freertos}/include
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM4F
  SOURCES
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM4F/port.c
  PRIVATE_DEPS
    pw_third_party.freertos.disable_warnings
)

# ARM CM33F port of FreeRTOS.
pw_add_library(pw_third_party.freertos.arm_cm33f STATIC
  HEADERS
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM33F/portmacro.h
  PUBLIC_DEPS
    ${pw_third_party_freertos_CONFIG}
  PUBLIC_INCLUDES
    ${dir_pw_third_party_freertos}/include
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM33F
  SOURCES
    ${dir_pw_third_party_freertos}/portable/GCC/ARM_CM33F/port.c
  PRIVATE_DEPS
    pw_third_party.freertos.disable_warnings
)

pw_add_library(pw_third_party.freertos.config_assert INTERFACE
  HEADERS
    public/pw_third_party/freertos/config_assert.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
)
