#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

set(CDDL_GEN_BASE ${CMAKE_CURRENT_LIST_DIR} CACHE FILEPATH
  "Directory of the cddl_gen repo."
  )

include(${CMAKE_CURRENT_LIST_DIR}/cmake/extensions.cmake)
add_subdirectory(src)
