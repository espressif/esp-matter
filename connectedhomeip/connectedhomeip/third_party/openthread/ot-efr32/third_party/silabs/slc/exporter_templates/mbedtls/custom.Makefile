####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 1                                                  #
####################################################################
#
#  Copyright (c) 2022, The OpenThread Authors.
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
{%  from
        'macros.jinja'
    import
        prepare_path,
        print_all_jinja_vars
    with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# ==============================================================================
# mbedtls library
# ==============================================================================
add_library({{PROJECT_NAME}}-mbedtls)

set_target_properties({{PROJECT_NAME}}-mbedtls
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

# ==============================================================================
# Includes
# ==============================================================================
set(SILABS_MBEDTLS_DIR "${SILABS_GSDK_DIR}/util/third_party/crypto/mbedtls")

{%- if C_CXX_INCLUDES %}
target_include_directories({{PROJECT_NAME}}-mbedtls
    PUBLIC
{%- for include in C_CXX_INCLUDES %}
{%- if ('util/third_party/crypto' in include) or ('platform' in include) %}
        {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
{%- endif %}
{%- endfor %}
)
{%- endif %}

# ==============================================================================
# Sources
# ==============================================================================
set(SILABS_MBEDTLS_SOURCES
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Filter-out non-mbedtls sources #}
    {%- if 'SILABS_MBEDTLS_DIR' in source %}
    {{source}}
    {%- endif %}
{%- endfor %}
)

target_sources({{PROJECT_NAME}}-mbedtls PRIVATE ${SILABS_MBEDTLS_SOURCES})

# ==============================================================================
# Compile definitions
# ==============================================================================
target_compile_definitions({{PROJECT_NAME}}-mbedtls PRIVATE ${OT_PLATFORM_DEFINES})

# ==============================================================================
# Linking
# ==============================================================================
{%- set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS %}
{%- if linker_flags %}
target_link_options({{PROJECT_NAME}}-mbedtls PRIVATE
{%- for flag in linker_flags %}
    {{ prepare_path(flag) }}
{%- endfor %}
)
{%- endif %}

target_link_libraries({{PROJECT_NAME}}-mbedtls
    PRIVATE
        ot-config
        {{PROJECT_NAME}}-config
)

{#- ========================================================================= #}
{#- Debug                                                                     #}
{#- ========================================================================= #}

{#- Change debug_template to true to print all jinja vars #}
{%- set debug_template = false %}
{%- if debug_template %}
{{ print_all_jinja_vars() }}
{%- endif %}
