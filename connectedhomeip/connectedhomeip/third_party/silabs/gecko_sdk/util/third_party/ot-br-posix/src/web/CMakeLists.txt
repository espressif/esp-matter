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

pkg_check_modules(JSONCPP jsoncpp REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system)
set(OTBR_WEB_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}/otbr-web)

add_executable(otbr-web
    main.cpp
    web-service/ot_client.cpp
    web-service/web_server.cpp
    web-service/wpan_service.cpp
)
target_compile_definitions(otbr-web PRIVATE
    WEB_FILE_PATH=\"${OTBR_WEB_DATADIR}/frontend\"
)
# TODO remove this when Simple Http Server is replaced and web-gui is refactored
target_compile_options(otbr-web PRIVATE
    -Wno-deprecated-declarations
    -Wno-unused-lambda-capture
)
target_include_directories(otbr-web PRIVATE
    ${JSONCPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/third_party/Simple-web-server/repo
)
target_link_libraries(otbr-web PRIVATE
    $<$<BOOL:${JSONCPP_LIBRARY_DIRS}>:-L$<JOIN:${JSONCPP_LIBRARY_DIRS}," -L">>
    ${JSONCPP_LIBRARIES}
    otbr-common
    otbr-utils
    openthread-ftd
    openthread-posix
    mbedtls
    ${Boost_LIBRARIES}
    pthread
)
install(
    TARGETS otbr-web
    DESTINATION sbin
)

add_subdirectory(web-service/frontend)

add_dependencies(otbr-web otbr-web-frontend)

if(OTBR_SYSTEMD_UNIT_DIR)
    configure_file(otbr-web.service.in otbr-web.service)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/otbr-web.service
        DESTINATION ${OTBR_SYSTEMD_UNIT_DIR}
    )
else()
    configure_file(otbr-web.init.in otbr-web.init)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/otbr-web.init
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/init.d
        RENAME otbr-web)
endif()
