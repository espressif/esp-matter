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

add_library(
                infineon-board-${IFX_BOARD}
                COMPONENT_bsp_design_modus/GeneratedSource/cycfg_pins.c
                TARGET_CYW930739M2EVB-01/epa.c
                TARGET_CYW930739M2EVB-01/gd25wq64e.c
                TARGET_CYW930739M2EVB-01/platform.c
           )

# Add compile option for platform board library
target_compile_definitions(
    infineon-board-${IFX_BOARD}
    PUBLIC
	CHIP_PACKAGE_WLCSP=0
        WICED_PLATFORM_EPA=1
        WICED_PLATFORM_ELNA=0
)

# Add include directory for platform board library
target_include_directories(
                            infineon-board-${IFX_BOARD}
                            PUBLIC
                                TARGET_CYW930739M2EVB-01
                                ${PROJECT_SOURCE_DIR}/third_party/infineon/btsdk-include
                          )

# Set the dependency between platform board library and platform base library
target_link_libraries(
                        infineon-board-${IFX_BOARD}
                        PRIVATE
                            infineon-base-${IFX_PLATFORM}
                     )
