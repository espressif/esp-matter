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

add_custom_target(otbr-dbus-introspect-header ALL
    COMMAND echo "R\"INTROSPECT(" > introspect.hpp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/introspect.xml >> introspect.hpp
    COMMAND echo ")INTROSPECT\"" >> introspect.hpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

option(OTBR_ENABLE_LEGACY "enable legacy support")

add_library(otbr-dbus-server STATIC
    dbus_agent.cpp
    dbus_object.cpp
    dbus_thread_object.cpp
    error_helper.cpp
)

target_include_directories(otbr-dbus-server PRIVATE
    ${PROJECT_BINARY_DIR}/src
)

add_dependencies(otbr-dbus-server otbr-dbus-introspect-header)

if(OTBR_ENABLE_LEGACY)
    target_compile_definitions(otbr-dbus-server PRIVATE
        "OTBR_ENABLE_LEGACY=1"
    )
    target_include_directories(otbr-dbus-server PRIVATE
        ${PROJECT_SOURCE_DIR}/${OTBR_LEGACY_INCLUDE}
    )
endif()

target_link_libraries(otbr-dbus-server PUBLIC
    otbr-dbus-common
)

if(OTBR_DOC)
add_custom_target(otbr-dbus-server-doc ALL
    COMMAND gdbus-codegen --generate-docbook generated-docs ${CMAKE_CURRENT_SOURCE_DIR}/introspect.xml 
    COMMAND xmlto html generated-docs-io.openthread.BorderRouter.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)
endif()
