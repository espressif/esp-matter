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

# Add platform base library
add_library(
                infineon-base-${IFX_PLATFORM}
                30739a0/COMPONENT_30739A0/platforms/platform_entropy.c
                30739a0/COMPONENT_30739A0/platforms/platform_retarget_lock.c
                30739a0/COMPONENT_30739A0/platforms/platform_stdio.c
                30739a0/COMPONENT_30739A0/platforms/wiced_platform.c
                30739a0/COMPONENT_30739A0/platforms/wiced_platform_bt_cfg.c
                30739a0/COMPONENT_30739A0/platforms/wiced_platform_memory.c
                30739a0/COMPONENT_30739A0/platforms/wiced_platform_os.c
                30739a0/COMPONENT_30739A0/WICED/common/spar_setup.c
           )

# Add compile definition for platform base library
target_compile_definitions(
                            infineon-base-${IFX_PLATFORM}
                            PUBLIC
                                BCM20739=1
                            PRIVATE
                                CY_PLATFORM_SWDCK=WICED_P02
                                CY_PLATFORM_SWDIO=WICED_P03
                                ENABLE_DEBUG=0
                                SPAR_CRT_SETUP=spar_crt_setup
                          )

# Add include directory for platform base library
target_include_directories(
                            infineon-base-${IFX_PLATFORM}
                            PUBLIC
                                30739a0/COMPONENT_30739A0/platforms
                                30739a0/COMPONENT_30739A0/include
                                30739a0/COMPONENT_30739A0/include/hal
                                30739a0/COMPONENT_30739A0/include/internal
                                30739a0/COMPONENT_30739A0/include/stack
                                30739a0/COMPONENT_30739A0/internal/30739A0
                                30739a0/COMPONENT_30739A0/WICED/common
                          )

# Add compile option for platform base library
target_compile_options(
                        infineon-base-${IFX_PLATFORM}
                        PUBLIC
                            @${CMAKE_CURRENT_SOURCE_DIR}/30739a0/COMPONENT_30739A0/internal/30739A0/gcc/30739A0.cflag
                      )

# Add libraries used for generating platform base library
target_link_libraries(
                        infineon-base-${IFX_PLATFORM}
                        PRIVATE
                            infineon-board-${IFX_BOARD}
                     )

# Add platform board library
add_subdirectory(bsp/${IFX_BOARD})
