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
        compile_flags,
        print_linker_flags,
        print_all_jinja_vars
    with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# ==============================================================================
# Library of platform dependencies from GSDK and generated config files
# ==============================================================================
add_library({{PROJECT_NAME}}-sdk)

set_target_properties({{PROJECT_NAME}}-sdk
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

# ==============================================================================
# Includes
# ==============================================================================
target_include_directories({{PROJECT_NAME}}-sdk PUBLIC
{%- for include in C_CXX_INCLUDES %}
    {%- if ('sample-apps' not in include) %}
    {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
    {%- endif %}
{%- endfor %}
)

target_include_directories({{PROJECT_NAME}}-sdk PRIVATE
    ${OT_PUBLIC_INCLUDES}
)

# ==============================================================================
# Sources
# ==============================================================================
target_sources({{PROJECT_NAME}}-sdk PRIVATE
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Ignore crypto sources, PAL sources, and openthread sources #}
    {%- if ('util/third_party/crypto/mbedtls' not in source)
            and ('${PROJECT_SOURCE_DIR}/src/src' not in source)
            and ('${PROJECT_SOURCE_DIR}/openthread' not in source) %}
        {%- if source.endswith('.c') or source.endswith('.cpp') or source.endswith('.h') or source.endswith('.hpp') %}
    {{source}}
        {%- endif %}
    {%- endif %}
{%- endfor %}
)

{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Ignore crypto sources, PAL sources, and openthread sources #}
    {%- if ('util/third_party/crypto/mbedtls' not in source)
            and ('${PROJECT_SOURCE_DIR}/src/src' not in source)
            and ('${PROJECT_SOURCE_DIR}/openthread' not in source) %}
        {%- if source.endswith('.s') or source.endswith('.S') %}

target_sources({{PROJECT_NAME}}-sdk PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
        {%- endif %}
    {%- endif %}
{%- endfor %}

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
# ==============================================================================
#  Compile Options
# ==============================================================================
target_compile_options({{PROJECT_NAME}}-sdk PRIVATE
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    {{ compile_flags() }}
)
{%- endif %} {#- compile_options #}

# ==============================================================================
# Linking
# ==============================================================================
target_link_libraries({{PROJECT_NAME}}-sdk
    PUBLIC
        {{PROJECT_NAME}}-mbedtls
    PRIVATE
{%- for source in SYS_LIBS+USER_LIBS %}
        {{prepare_path(source)}}
{%- endfor %}
        {{PROJECT_NAME}}-config
        ot-config
)

{% set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS %}
{%- if linker_flags -%}
# ==============================================================================
#  Linker Flags
# ==============================================================================
target_link_options({{PROJECT_NAME}}-sdk PRIVATE {{ print_linker_flags() }}
)
{%- endif %} {# linker_flags #}

{#- ========================================================================= #}
{#- Debug                                                                     #}
{#- ========================================================================= #}

{#- Change debug_template to true to print all jinja vars #}
{%- set debug_template = false %}
{%- if debug_template %}
{{ print_all_jinja_vars() }}
{%- endif %}
