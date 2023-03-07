#
#  Copyright (c) 2020, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

add_executable(otbr-test-mdns
    main.cpp
)

target_link_libraries(otbr-test-mdns PRIVATE
    otbr-config
    otbr-mdns
)

add_test(
    NAME mdns-single
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-single
)

add_test(
    NAME mdns-multiple
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-multiple
)

add_test(
    NAME mdns-update
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-update
)

add_test(
    NAME mdns-stop
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-stop
)

add_test(
    NAME mdns-single-custom-host
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-single-custom-host
)

add_test(
    NAME mdns-multiple-custom-hosts
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-multiple-custom-hosts
)

add_test(
    NAME mdns-service-subtypes
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test-service-subtypes
)

set_tests_properties(
    mdns-single
    mdns-multiple
    mdns-update
    mdns-stop
    mdns-single-custom-host
    mdns-multiple-custom-hosts
    mdns-service-subtypes
    PROPERTIES
        ENVIRONMENT "OTBR_MDNS=${OTBR_MDNS};OTBR_TEST_MDNS=$<TARGET_FILE:otbr-test-mdns>"
)
