# Copyright 2021 The Pigweed Authors
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

pw_add_library(pw_router.static_router STATIC
  HEADERS
    public/pw_router/static_router.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_metric
    pw_router.egress
    pw_router.packet_parser
    pw_span
  SOURCES
    static_router.cc
  PRIVATE_DEPS
    pw_log
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_ROUTER_STATIC_ROUTER)
  zephyr_link_libraries(pw_router.static_router)
endif()

pw_add_library(pw_router.egress INTERFACE
  HEADERS
    public/pw_router/egress.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_router.packet_parser
    pw_span
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_ROUTER_EGRESS)
  zephyr_link_libraries(pw_router.egress)
endif()

pw_add_library(pw_router.packet_parser INTERFACE
  HEADERS
    public/pw_router/packet_parser.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_span
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_ROUTER_PACKET_PARSER)
  zephyr_link_libraries(pw_router.packet_parser)
endif()

pw_add_library(pw_router.egress_function INTERFACE
  HEADERS
    public/pw_router/egress_function.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_function
    pw_router.egress
    pw_span
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_ROUTER_EGRESS_FUNCTION)
  zephyr_link_libraries(pw_router.egress_function)
endif()

pw_add_test(pw_router.static_router_test
  SOURCES
    static_router_test.cc
  PRIVATE_DEPS
    pw_router.egress_function
    pw_router.static_router
  GROUPS
    modules
    pw_router
)
