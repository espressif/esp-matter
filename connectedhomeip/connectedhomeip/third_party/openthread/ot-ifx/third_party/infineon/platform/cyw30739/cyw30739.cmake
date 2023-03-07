#
#  Copyright (c) 2021, The OpenThread Authors.
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

set(TOOLS_DIR ${PROJECT_SOURCE_DIR}/third_party/infineon/btsdk-tools)
if(CMAKE_HOST_APPLE)
    set(TOOLS_DIR ${TOOLS_DIR}/OSX)
elseif(CMAKE_HOST_WIN32)
    set(TOOLS_DIR ${TOOLS_DIR}/Windows)
else()
    set(TOOLS_DIR ${TOOLS_DIR}/Linux64)
endif()

set(BASELIB_DIR ${PROJECT_SOURCE_DIR}/third_party/infineon/platform/${IFX_PLATFORM}/30739a0/COMPONENT_30739A0)

string(REPLACE "-" ";" PATCH_NAME_LIST ${IFX_BOARD})
list(GET PATCH_NAME_LIST 0 BOARD_NAME)
list(GET PATCH_NAME_LIST 1 BOARD_VERSION)
STRING(TOUPPER ${BOARD_NAME} BOARD_NAME)
set(PATCH_DIR ${BASELIB_DIR}/internal/30739A0/patches_${BOARD_NAME}_${BOARD_VERSION})

set(PLATFORM_DIR ${BASELIB_DIR}/platforms)
set(SCRIPTS_DIR ${PROJECT_SOURCE_DIR}/third_party/infineon/platform/${IFX_PLATFORM}/scripts)
set(PATCH_SYMBOLS ${PATCH_DIR}/patch.sym)
set(BTP ${PLATFORM_DIR}/flash.btp)
set(LIB_INSTALLER_C generated/30739A0/lib_installer.c)
set(HDF_FILE ${BASELIB_DIR}/internal/30739A0/configdef30739A0.hdf)

set(LINKER_DEFINES
    FLASH0_BEGIN_ADDR=0x00500000
    FLASH0_LENGTH=0x00100000
    XIP_DS_OFFSET=0x0002D000
    XIP_LEN=0x00068000
    SRAM_BEGIN_ADDR=0x00200000
    SRAM_LENGTH=0x00070000
    AON_AREA_END=0x00284000
    ISTATIC_BEGIN=0x500C00
    ISTATIC_LEN=0x400
    NUM_PATCH_ENTRIES=256
    BTP=${BTP}
)

set(CGS_LIST
    ${PATCH_DIR}/patch.cgs
    ${PLATFORM_DIR}/platform.cgs
    ${PLATFORM_DIR}/platform_xip.cgs
)

add_custom_command(
                    OUTPUT
                        ${LIB_INSTALLER_C}
                    COMMAND
                        bash --norc --noprofile
                        ${SCRIPTS_DIR}/bt_gen_lib_installer.bash
                        --scripts=${SCRIPTS_DIR}
                        --out=${LIB_INSTALLER_C}
                        $<$<BOOL:${VERBOSE}>:--verbose>
                  )

function(add_example_target target_name source_files include_dirs link_libs)
    add_executable(
                    ${target_name}
                    ${source_files}
                    ${LIB_INSTALLER_C}
                  )

    set_property(TARGET ${target_name} PROPERTY SUFFIX .elf)

    target_include_directories(
                                ${target_name}
                                PRIVATE
                                ${include_dirs}
                              )

    add_custom_command(
                        TARGET
                            ${target_name}
                        PRE_LINK
                        COMMAND
                            bash --norc --noprofile
                            ${SCRIPTS_DIR}/bt_pre_build.bash
                            --scripts=${SCRIPTS_DIR}
                            --defs="${LINKER_DEFINES}"
                            --ld=$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>.ld
                            #--ld=$<TARGET_FILE_DIR:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}>/$<TARGET_FILE_BASE_NAME:${target_name}>.ld
                            --patch=${PATCH_SYMBOLS}
                            $<$<BOOL:${VERBOSE}>:--verbose>
                        WORKING_DIRECTORY
                            ${CMAKE_CURRENT_SOURCE_DIR}
                      )

    target_link_libraries(
                            ${target_name}
                            PRIVATE
                                ${link_libs}
                            -Wl,--whole-archive
                            infineon-base-${IFX_PLATFORM}
                            infineon-board-${IFX_BOARD}
                            -Wl,--no-whole-archive
                            -T${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.ld
                            -Wl,--cref
                            -Wl,--entry=spar_crt_setup
                            -Wl,--gc-sections
                            -Wl,--just-symbols=${PATCH_SYMBOLS}
                            -Wl,--warn-common
                            -Wl,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.map
                            -nostartfiles
                         )

    add_custom_command(
                        TARGET
                            ${target_name}
                        POST_BUILD
                        COMMAND
                            bash --norc --noprofile
                            ${SCRIPTS_DIR}/bt_post_build.bash
                            --scripts=${SCRIPTS_DIR}
                            --cross="arm-none-eabi-"
                            --tools=${TOOLS_DIR}
                            --builddir=$<TARGET_FILE_DIR:${target_name}>
                            --elfname=$<TARGET_FILE_NAME:${target_name}>
                            --appname=${target_name}
                            --appver=0x00000000
                            --hdf=${BASELIB_DIR}/internal/30739A0/configdef30739A0.hdf
                            --entry=spar_crt_setup.entry
                            --cgslist="${CGS_LIST}"
                            --cgsargs="-O DLConfigBD_ADDRBase:default"
                            --btp=${BTP}
                            --id=${PLATFORM_DIR}/IDFILE.txt
                            --overridebaudfile=${PLATFORM_DIR}/BAUDRATEFILE.txt
                            --chip=20739B2
                            --minidriver=${PLATFORM_DIR}/minidriver.hex
                            --clflags=-NOHCIRESET
                            --extras=static_config_XIP_
                            $<$<BOOL:${VERBOSE}>:--verbose>
                        WORKING_DIRECTORY
                            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()
