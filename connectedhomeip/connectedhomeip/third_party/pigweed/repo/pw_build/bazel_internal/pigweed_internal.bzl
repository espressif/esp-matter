#!/usr/bin/env python3

# Copyright 2022 The Pigweed Authors
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
""" An internal set of tools for creating embedded CC targets. """

load("@rules_cc//cc:action_names.bzl", "C_COMPILE_ACTION_NAME")
load("@rules_cc//cc:toolchain_utils.bzl", "find_cpp_toolchain")

DEBUGGING = [
    "-g",
]

# Standard compiler flags to reduce output binary size.
REDUCED_SIZE_COPTS = [
    "-fno-common",
    "-fno-exceptions",
    "-ffunction-sections",
    "-fdata-sections",
]

STRICT_WARNINGS_COPTS = [
    "-Wall",
    "-Wextra",
    # Make all warnings errors, except for the exemptions below.
    "-Werror",
    "-Wno-error=cpp",  # preprocessor #warning statement
    "-Wno-error=deprecated-declarations",  # [[deprecated]] attribute
]

CPP17_COPTS = [
    "-std=c++17",
    "-fno-rtti",
    "-Wnon-virtual-dtor",
    # Allow uses of the register keyword, which may appear in C headers.
    "-Wno-register",
]

DISABLE_PENDING_WORKAROUND_COPTS = [
    "-Wno-private-header",
]

PW_DEFAULT_COPTS = (
    DEBUGGING +
    REDUCED_SIZE_COPTS +
    STRICT_WARNINGS_COPTS +
    DISABLE_PENDING_WORKAROUND_COPTS
)

KYTHE_COPTS = [
    "-Wno-unknown-warning-option",
]

PW_DEFAULT_LINKOPTS = []

def add_defaults(kwargs):
    """Adds default arguments suitable for both C and C++ code to kwargs.

    Args:
        kwargs: cc_* arguments to be modified.
    """

    copts = PW_DEFAULT_COPTS + kwargs.get("copts", [])
    kwargs["copts"] = select({
        "@pigweed//pw_build:kythe": copts + KYTHE_COPTS,
        "//conditions:default": copts,
    })
    kwargs["linkopts"] = kwargs.get("linkopts", []) + PW_DEFAULT_LINKOPTS

    # Set linkstatic to avoid building .so files.
    kwargs["linkstatic"] = True

    kwargs.setdefault("features", [])

    # Crosstool--adding this line to features disables header modules, which
    # don't work with -fno-rtti. Note: this is not a command-line argument,
    # it's "minus use_header_modules".
    kwargs["features"].append("-use_header_modules")

def default_cc_and_c_kwargs(kwargs):
    """Splits kwargs into C and C++ arguments adding defaults.

    Args:
        kwargs: cc_* arguments to be modified.

    Returns:
        A tuple of (cc_cxx_kwargs cc_c_kwargs)
    """
    add_defaults(kwargs)
    kwargs.setdefault("srcs", [])

    cc = dict(kwargs.items())
    cc["srcs"] = [src for src in kwargs["srcs"] if not src.endswith(".c")]
    cc["copts"] = cc["copts"] + CPP17_COPTS

    c_srcs = [src for src in kwargs["srcs"] if src.endswith(".c")]

    if c_srcs:
        c = dict(kwargs.items())
        c["name"] += "_c"
        c["srcs"] = c_srcs + [src for src in kwargs["srcs"] if src.endswith(".h")]

        cc["deps"] = cc.get("deps", []) + [":" + c["name"]]
        return cc, c

    return cc, None

def add_cc_and_c_targets(target, kwargs):  # buildifier: disable=unnamed-macro
    """Splits target into C and C++ targets adding defaults.

    Args:
        target: cc_* target to be split.
        kwargs: cc_* arguments to be modified.
    """
    cc_kwargs, c_kwargs = default_cc_and_c_kwargs(kwargs)

    if c_kwargs:
        native.cc_library(**c_kwargs)

    target(**cc_kwargs)

def has_pw_assert_dep(deps):
    """Checks if the given deps contain a pw_assert dependency

    Args:
        deps: List of dependencies

    Returns:
        True if the list contains a pw_assert dependency.
    """
    pw_assert_targets = ["//pw_assert", "//pw_assert:pw_assert"]
    pw_assert_targets.extend(["@pigweed" + t for t in pw_assert_targets])
    for dep in deps:
        if dep in pw_assert_targets:
            return True
    return False

def _preprocess_linker_script_impl(ctx):
    cc_toolchain = find_cpp_toolchain(ctx)
    output_script = ctx.actions.declare_file(ctx.label.name + ".ld")
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )
    cxx_compiler_path = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = C_COMPILE_ACTION_NAME,
    )
    c_compile_variables = cc_common.create_compile_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        user_compile_flags = ctx.fragments.cpp.copts + ctx.fragments.cpp.conlyopts,
    )
    action_flags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = C_COMPILE_ACTION_NAME,
        variables = c_compile_variables,
    )
    env = cc_common.get_environment_variables(
        feature_configuration = feature_configuration,
        action_name = C_COMPILE_ACTION_NAME,
        variables = c_compile_variables,
    )
    ctx.actions.run(
        outputs = [output_script],
        inputs = depset(
            [ctx.file.linker_script],
            transitive = [cc_toolchain.all_files],
        ),
        executable = cxx_compiler_path,
        arguments = [
            "-E",
            "-P",
            "-xc",
            ctx.file.linker_script.short_path,
            "-o",
            output_script.path,
        ] + [
            "-D" + d
            for d in ctx.attr.defines
        ] + action_flags + ctx.attr.copts,
        env = env,
    )
    return [DefaultInfo(files = depset([output_script]))]

pw_linker_script = rule(
    _preprocess_linker_script_impl,
    attrs = {
        "copts": attr.string_list(doc = "C compile options."),
        "defines": attr.string_list(doc = "C preprocessor defines."),
        "linker_script": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "Linker script to preprocess.",
        ),
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
    },
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    fragments = ["cpp"],
)
