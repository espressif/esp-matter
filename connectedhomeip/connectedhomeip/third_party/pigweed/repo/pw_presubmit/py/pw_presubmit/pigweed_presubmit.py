#!/usr/bin/env python3

# Copyright 2020 The Pigweed Authors
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
"""Runs the local presubmit checks for the Pigweed repository."""

import argparse
import json
import logging
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from typing import Callable, Iterable, List, Sequence, TextIO

try:
    import pw_presubmit
except ImportError:
    # Append the pw_presubmit package path to the module search path to allow
    # running this module without installing the pw_presubmit package.
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    import pw_presubmit

import pw_package.pigweed_packages

from pw_presubmit import (
    build,
    cli,
    cpp_checks,
    format_code,
    git_repo,
    gitmodules,
    call,
    filter_paths,
    inclusive_language,
    keep_sorted,
    npm_presubmit,
    owners_checks,
    plural,
    presubmit,
    PresubmitContext,
    PresubmitFailure,
    Programs,
    python_checks,
    shell_checks,
    todo_check,
)
from pw_presubmit.install_hook import install_git_hook

_LOG = logging.getLogger(__name__)

pw_package.pigweed_packages.initialize()

# Trigger builds if files with these extensions change.
_BUILD_FILE_FILTER = presubmit.FileFilter(
    suffix=(*format_code.C_FORMAT.extensions, '.py', '.rst', '.gn', '.gni')
)

_OPTIMIZATION_LEVELS = 'debug', 'size_optimized', 'speed_optimized'


def _at_all_optimization_levels(target):
    for level in _OPTIMIZATION_LEVELS:
        yield f'{target}_{level}'


#
# Build presubmit checks
#
def gn_clang_build(ctx: PresubmitContext):
    """Checks all compile targets that rely on LLVM tooling."""
    build_targets = [
        *_at_all_optimization_levels('host_clang'),
        'cpp14_compatibility',
        'cpp20_compatibility',
        'asan',
        'tsan',
        'ubsan',
        'runtime_sanitizers',
        # TODO(b/234876100): msan will not work until the C++ standard library
        # included in the sysroot has a variant built with msan.
    ]

    # clang-tidy doesn't run on Windows.
    if sys.platform != 'win32':
        build_targets.append('static_analysis')

    # QEMU doesn't run on Windows.
    if sys.platform != 'win32':
        # TODO(b/244604080): For the pw::InlineString tests, qemu_clang_debug
        #     and qemu_clang_speed_optimized produce a binary too large for the
        #     QEMU target's 256KB flash. Restore debug and speed optimized
        #     builds when this is fixed.
        build_targets.append('qemu_clang_size_optimized')

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_gcc_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *_at_all_optimization_levels('host_gcc'))


_HOST_COMPILER = 'gcc' if sys.platform == 'win32' else 'clang'


def gn_host_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *_at_all_optimization_levels(f'host_{_HOST_COMPILER}'))


@_BUILD_FILE_FILTER.apply_to_check()
def gn_quick_build_check(ctx: PresubmitContext):
    """Checks the state of the GN build by running gn gen and gn check."""
    build.gn_gen(ctx)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_full_build_check(ctx: PresubmitContext) -> None:
    build_targets = [
        *_at_all_optimization_levels('stm32f429i'),
        *_at_all_optimization_levels(f'host_{_HOST_COMPILER}'),
        'python.tests',
        'python.lint',
        'docs',
        'fuzzers',
        'pigweed_pypi_distribution',
    ]

    # TODO(b/234645359): Re-enable on Windows when compatibility tests build.
    if sys.platform != 'win32':
        build_targets.append('cpp14_compatibility')
        build_targets.append('cpp20_compatibility')

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.gn_gen(
        ctx,
        pw_unit_test_FACADE_TESTS_ENABLED=True,
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_full_qemu_check(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(
        ctx,
        *_at_all_optimization_levels('qemu_gcc'),
        *_at_all_optimization_levels('qemu_clang'),
    )


@_BUILD_FILE_FILTER.apply_to_check()
def gn_combined_build_check(ctx: PresubmitContext) -> None:
    """Run most host and device (QEMU) tests."""
    build_targets = [
        *_at_all_optimization_levels('stm32f429i'),
        *_at_all_optimization_levels(f'host_{_HOST_COMPILER}'),
        'python.tests',
        'python.lint',
        'docs',
        'fuzzers',
        'pigweed_pypi_distribution',
    ]

    # TODO(b/234645359): Re-enable on Windows when compatibility tests build.
    if sys.platform != 'win32':
        build_targets.append('cpp14_compatibility')
        build_targets.append('cpp20_compatibility')

    # clang-tidy doesn't run on Windows.
    if sys.platform != 'win32':
        build_targets.append('static_analysis')

    # QEMU doesn't run on Windows.
    if sys.platform != 'win32':
        build_targets.extend(_at_all_optimization_levels('qemu_gcc'))

        # TODO(b/244604080): For the pw::InlineString tests, qemu_clang_debug
        #     and qemu_clang_speed_optimized produce a binary too large for the
        #     QEMU target's 256KB flash. Restore debug and speed optimized
        #     builds when this is fixed.
        build_targets.append('qemu_clang_size_optimized')

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_arm_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *_at_all_optimization_levels('stm32f429i'))


@_BUILD_FILE_FILTER.apply_to_check()
def stm32f429i(ctx: PresubmitContext):
    build.gn_gen(
        ctx,
        pw_use_test_server=True,
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    with build.test_server('stm32f429i_disc1_test_server', ctx.output_dir):
        build.ninja(ctx, *_at_all_optimization_levels('stm32f429i'))


@_BUILD_FILE_FILTER.apply_to_check()
def gn_boringssl_build(ctx: PresubmitContext):
    build.install_package(ctx, 'boringssl')
    build.gn_gen(
        ctx,
        dir_pw_third_party_boringssl='"{}"'.format(
            ctx.package_root / 'boringssl'
        ),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(
        ctx,
        *_at_all_optimization_levels('stm32f429i'),
        *_at_all_optimization_levels('host_clang'),
    )


@_BUILD_FILE_FILTER.apply_to_check()
def gn_nanopb_build(ctx: PresubmitContext):
    build.install_package(ctx, 'nanopb')
    build.gn_gen(
        ctx,
        dir_pw_third_party_nanopb='"{}"'.format(ctx.package_root / 'nanopb'),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(
        ctx,
        *_at_all_optimization_levels('stm32f429i'),
        *_at_all_optimization_levels('host_clang'),
    )


@_BUILD_FILE_FILTER.apply_to_check()
def gn_crypto_mbedtls_build(ctx: PresubmitContext):
    build.install_package(ctx, 'mbedtls')
    build.gn_gen(
        ctx,
        dir_pw_third_party_mbedtls='"{}"'.format(ctx.package_root / 'mbedtls'),
        pw_crypto_SHA256_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:sha256_mbedtls'
        ),
        pw_crypto_ECDSA_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:ecdsa_mbedtls'
        ),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build_targets = [*_at_all_optimization_levels(f'host_{_HOST_COMPILER}')]

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_crypto_boringssl_build(ctx: PresubmitContext):
    build.install_package(ctx, 'boringssl')
    build.gn_gen(
        ctx,
        dir_pw_third_party_boringssl='"{}"'.format(
            ctx.package_root / 'boringssl'
        ),
        pw_crypto_SHA256_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:sha256_boringssl'
        ),
        pw_crypto_ECDSA_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:ecdsa_boringssl'
        ),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build_targets = [*_at_all_optimization_levels(f'host_{_HOST_COMPILER}')]

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_crypto_micro_ecc_build(ctx: PresubmitContext):
    build.install_package(ctx, 'micro-ecc')
    build.gn_gen(
        ctx,
        dir_pw_third_party_micro_ecc='"{}"'.format(
            ctx.package_root / 'micro-ecc'
        ),
        pw_crypto_ECDSA_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:ecdsa_uecc'
        ),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build_targets = [*_at_all_optimization_levels(f'host_{_HOST_COMPILER}')]

    # TODO(b/240982565): SocketStream currently requires Linux.
    if sys.platform.startswith('linux'):
        build_targets.append('integration_tests')

    build.ninja(ctx, *build_targets)


@_BUILD_FILE_FILTER.apply_to_check()
def gn_teensy_build(ctx: PresubmitContext):
    build.install_package(ctx, 'teensy')
    build.gn_gen(
        ctx,
        pw_arduino_build_CORE_PATH='"{}"'.format(str(ctx.package_root)),
        pw_arduino_build_CORE_NAME='teensy',
        pw_arduino_build_PACKAGE_NAME='teensy/avr',
        pw_arduino_build_BOARD='teensy40',
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(ctx, *_at_all_optimization_levels('arduino'))


@_BUILD_FILE_FILTER.apply_to_check()
def gn_pico_build(ctx: PresubmitContext):
    build.install_package(ctx, 'pico_sdk')
    build.gn_gen(
        ctx,
        PICO_SRC_DIR='"{}"'.format(str(ctx.package_root / 'pico_sdk')),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(ctx, 'pi_pico')


@_BUILD_FILE_FILTER.apply_to_check()
def gn_software_update_build(ctx: PresubmitContext):
    build.install_package(ctx, 'nanopb')
    build.install_package(ctx, 'protobuf')
    build.install_package(ctx, 'mbedtls')
    build.install_package(ctx, 'micro-ecc')
    build.gn_gen(
        ctx,
        dir_pw_third_party_protobuf='"{}"'.format(
            ctx.package_root / 'protobuf'
        ),
        dir_pw_third_party_nanopb='"{}"'.format(ctx.package_root / 'nanopb'),
        dir_pw_third_party_micro_ecc='"{}"'.format(
            ctx.package_root / 'micro-ecc'
        ),
        pw_crypto_ECDSA_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:ecdsa_uecc'
        ),
        dir_pw_third_party_mbedtls='"{}"'.format(ctx.package_root / 'mbedtls'),
        pw_crypto_SHA256_BACKEND='"{}"'.format(
            ctx.root / 'pw_crypto:sha256_mbedtls'
        ),
        pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS,
    )
    build.ninja(
        ctx,
        *_at_all_optimization_levels('host_clang'),
    )


@_BUILD_FILE_FILTER.apply_to_check()
def gn_pw_system_demo_build(ctx: PresubmitContext):
    build.install_package(ctx, 'freertos')
    build.install_package(ctx, 'nanopb')
    build.install_package(ctx, 'stm32cube_f4')
    build.install_package(ctx, 'pico_sdk')
    build.gn_gen(
        ctx,
        dir_pw_third_party_freertos='"{}"'.format(
            ctx.package_root / 'freertos'
        ),
        dir_pw_third_party_nanopb='"{}"'.format(ctx.package_root / 'nanopb'),
        dir_pw_third_party_stm32cube_f4='"{}"'.format(
            ctx.package_root / 'stm32cube_f4'
        ),
        PICO_SRC_DIR='"{}"'.format(str(ctx.package_root / 'pico_sdk')),
    )
    build.ninja(ctx, 'pw_system_demo')


@_BUILD_FILE_FILTER.apply_to_check()
def gn_qemu_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *_at_all_optimization_levels('qemu_gcc'))


@_BUILD_FILE_FILTER.apply_to_check()
def gn_qemu_clang_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_C_OPTIMIZATION_LEVELS=_OPTIMIZATION_LEVELS)
    build.ninja(ctx, *_at_all_optimization_levels('qemu_clang'))


def gn_docs_build(ctx: PresubmitContext):
    build.gn_gen(ctx)
    build.ninja(ctx, 'docs')


def gn_host_tools(ctx: PresubmitContext):
    build.gn_gen(ctx)
    build.ninja(ctx, 'host_tools')


@filter_paths(endswith=format_code.C_FORMAT.extensions)
def oss_fuzz_build(ctx: PresubmitContext):
    build.gn_gen(ctx, pw_toolchain_OSS_FUZZ_ENABLED=True)
    build.ninja(ctx, "fuzzers")


def _run_cmake(ctx: PresubmitContext, toolchain='host_clang') -> None:
    build.install_package(ctx, 'nanopb')

    env = None
    if 'clang' in toolchain:
        env = build.env_with_clang_vars()

    toolchain_path = ctx.root / 'pw_toolchain' / toolchain / 'toolchain.cmake'
    build.cmake(
        ctx,
        f'-DCMAKE_TOOLCHAIN_FILE={toolchain_path}',
        '-DCMAKE_EXPORT_COMPILE_COMMANDS=1',
        f'-Ddir_pw_third_party_nanopb={ctx.package_root / "nanopb"}',
        '-Dpw_third_party_nanopb_ADD_SUBDIRECTORY=ON',
        env=env,
    )


@filter_paths(
    endswith=(*format_code.C_FORMAT.extensions, '.cmake', 'CMakeLists.txt')
)
def cmake_clang(ctx: PresubmitContext):
    _run_cmake(ctx, toolchain='host_clang')
    build.ninja(ctx, 'pw_apps', 'pw_run_tests.modules')


@filter_paths(
    endswith=(*format_code.C_FORMAT.extensions, '.cmake', 'CMakeLists.txt')
)
def cmake_gcc(ctx: PresubmitContext):
    _run_cmake(ctx, toolchain='host_gcc')
    build.ninja(ctx, 'pw_apps', 'pw_run_tests.modules')


@filter_paths(
    endswith=(*format_code.C_FORMAT.extensions, '.bazel', '.bzl', 'BUILD')
)
def bazel_test(ctx: PresubmitContext) -> None:
    """Runs bazel test on each bazel compatible module."""
    build.bazel(
        ctx,
        'test',
        '--test_output=errors',
        '--',
        '//...',
    )


@filter_paths(
    endswith=(*format_code.C_FORMAT.extensions, '.bazel', '.bzl', 'BUILD')
)
def bazel_build(ctx: PresubmitContext) -> None:
    """Runs Bazel build on each Bazel compatible module."""
    build.bazel(
        ctx,
        'build',
        '--',
        '//...',
    )


def pw_transfer_integration_test(ctx: PresubmitContext) -> None:
    """Runs the pw_transfer cross-language integration test only.

    This test is not part of the regular bazel build because it's slow and
    intended to run in CI only.
    """
    build.bazel(
        ctx,
        'test',
        '//pw_transfer/integration_test:cross_language_small_test',
        '//pw_transfer/integration_test:cross_language_medium_read_test',
        '//pw_transfer/integration_test:cross_language_medium_write_test',
        '//pw_transfer/integration_test:cross_language_large_read_test',
        '//pw_transfer/integration_test:cross_language_large_write_test',
        '//pw_transfer/integration_test:multi_transfer_test',
        '//pw_transfer/integration_test:expected_errors_test',
        '//pw_transfer/integration_test:legacy_binaries_test',
        '--test_output=errors',
    )


#
# General presubmit checks
#


def _clang_system_include_paths(lang: str) -> List[str]:
    """Generate default system header paths.

    Returns the list of system include paths used by the host
    clang installation.
    """
    # Dump system include paths with preprocessor verbose.
    command = [
        'clang++',
        '-Xpreprocessor',
        '-v',
        '-x',
        f'{lang}',
        f'{os.devnull}',
        '-fsyntax-only',
    ]
    process = subprocess.run(
        command, check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )

    # Parse the command output to retrieve system include paths.
    # The paths are listed one per line.
    output = process.stdout.decode(errors='backslashreplace')
    include_paths: List[str] = []
    for line in output.splitlines():
        path = line.strip()
        if os.path.exists(path):
            include_paths.append(f'-isystem{path}')

    return include_paths


def edit_compile_commands(
    in_path: Path, out_path: Path, func: Callable[[str, str, str], str]
) -> None:
    """Edit the selected compile command file.

    Calls the input callback on all triplets (file, directory, command) in
    the input compile commands database. The return value replaces the old
    compile command in the output database.
    """
    with open(in_path) as in_file:
        compile_commands = json.load(in_file)
        for item in compile_commands:
            item['command'] = func(
                item['file'], item['directory'], item['command']
            )
    with open(out_path, 'w') as out_file:
        json.dump(compile_commands, out_file, indent=2)


_EXCLUDE_FROM_COPYRIGHT_NOTICE: Sequence[str] = (
    # Configuration
    # keep-sorted: start
    r'\bPW_PLUGINS$',
    r'\bconstraint.list$',
    r'^(?:.+/)?\..+$',
    # keep-sorted: end
    # Metadata
    # keep-sorted: start
    r'\bAUTHORS$',
    r'\bLICENSE$',
    r'\bOWNERS$',
    r'\bPIGWEED_MODULES$',
    r'\bgo.(mod|sum)$',
    r'\bpackage-lock.json$',
    r'\bpackage.json$',
    r'\brequirements.txt$',
    r'\byarn.lock$',
    r'^docker/tag$',
    # keep-sorted: end
    # Data files
    # keep-sorted: start
    r'\.bin$',
    r'\.csv$',
    r'\.elf$',
    r'\.gif$',
    r'\.ico$',
    r'\.jpg$',
    r'\.json$',
    r'\.png$',
    r'\.svg$',
    r'\.xml$',
    # keep-sorted: end
    # Documentation
    # keep-sorted: start
    r'\.md$',
    r'\.rst$',
    # keep-sorted: end
    # Generated protobuf files
    # keep-sorted: start
    r'\.pb\.c$',
    r'\.pb\.h$',
    r'\_pb2.pyi?$',
    # keep-sorted: end
    # Diff/Patch files
    # keep-sorted: start
    r'\.diff$',
    r'\.patch$',
    # keep-sorted: end
    # Test data
    # keep-sorted: start
    r'\bpw_presubmit/py/test/owners_checks/',
    # keep-sorted: end
)

# Regular expression for the copyright comment. "\1" refers to the comment
# characters and "\2" refers to space after the comment characters, if any.
# All period characters are escaped using a replace call.
# pylint: disable=line-too-long
_COPYRIGHT = re.compile(
    r"""(#|//|::| \*|)( ?)Copyright 2\d{3} The Pigweed Authors
\1
\1\2Licensed under the Apache License, Version 2.0 \(the "License"\); you may not
\1\2use this file except in compliance with the License. You may obtain a copy of
\1\2the License at
\1
\1(?:\2    |\t)https://www.apache.org/licenses/LICENSE-2.0
\1
\1\2Unless required by applicable law or agreed to in writing, software
\1\2distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
\1\2WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
\1\2License for the specific language governing permissions and limitations under
\1\2the License.
""".replace(
        '.', r'\.'
    ),
    re.MULTILINE,
)
# pylint: enable=line-too-long

_SKIP_LINE_PREFIXES = (
    '#!',
    '@echo off',
    ':<<',
    '/*',
    ' * @jest-environment jsdom',
    ' */',
    '{#',  # Jinja comment block
    '# -*- coding: utf-8 -*-',
    '<!--',
)


def _read_notice_lines(file: TextIO) -> Iterable[str]:
    lines = iter(file)
    try:
        # Read until the first line of the copyright notice.
        line = next(lines)
        while line.isspace() or line.startswith(_SKIP_LINE_PREFIXES):
            line = next(lines)

        yield line

        for _ in range(12):  # The notice is 13 lines; read the remaining 12.
            yield next(lines)
    except StopIteration:
        return


@filter_paths(exclude=_EXCLUDE_FROM_COPYRIGHT_NOTICE)
def copyright_notice(ctx: PresubmitContext):
    """Checks that the Pigweed copyright notice is present."""
    errors = []

    for path in ctx.paths:
        if path.stat().st_size == 0:
            continue  # Skip empty files

        with path.open() as file:
            if not _COPYRIGHT.match(''.join(_read_notice_lines(file))):
                errors.append(path)

    if errors:
        _LOG.warning(
            '%s with a missing or incorrect copyright notice:\n%s',
            plural(errors, 'file'),
            '\n'.join(str(e) for e in errors),
        )
        raise PresubmitFailure


_BAZEL_SOURCES_IN_BUILD = tuple(format_code.C_FORMAT.extensions)
_GN_SOURCES_IN_BUILD = (
    'setup.cfg',
    '.toml',
    '.rst',
    '.py',
    *_BAZEL_SOURCES_IN_BUILD,
)

SOURCE_FILES_FILTER = presubmit.FileFilter(
    endswith=_GN_SOURCES_IN_BUILD,
    suffix=('.bazel', '.bzl', '.gn', '.gni'),
    exclude=(r'zephyr.*/', r'android.*/', r'^pyproject.toml'),
)


@SOURCE_FILES_FILTER.apply_to_check()
def source_is_in_build_files(ctx: PresubmitContext):
    """Checks that source files are in the GN and Bazel builds."""
    missing = build.check_builds_for_files(
        _BAZEL_SOURCES_IN_BUILD,
        _GN_SOURCES_IN_BUILD,
        ctx.paths,
        bazel_dirs=[ctx.root],
        gn_build_files=git_repo.list_files(
            pathspecs=['BUILD.gn', '*BUILD.gn'], repo_path=ctx.root
        ),
    )

    if missing:
        _LOG.warning('All source files must appear in BUILD and BUILD.gn files')
        raise PresubmitFailure

    _run_cmake(ctx)
    cmake_missing = build.check_compile_commands_for_files(
        ctx.output_dir / 'compile_commands.json',
        (f for f in ctx.paths if f.suffix in ('.c', '.cc')),
    )
    if cmake_missing:
        _LOG.warning('The CMake build is missing %d files', len(cmake_missing))
        _LOG.warning(
            'Files missing from CMake:\n%s',
            '\n'.join(str(f) for f in cmake_missing),
        )
        # TODO(hepler): Many files are missing from the CMake build. Make this
        #     check an error when the missing files are fixed.
        # raise PresubmitFailure


def build_env_setup(ctx: PresubmitContext):
    if 'PW_CARGO_SETUP' not in os.environ:
        _LOG.warning('Skipping build_env_setup since PW_CARGO_SETUP is not set')
        return

    tmpl = ctx.root.joinpath('pw_env_setup', 'py', 'pyoxidizer.bzl.tmpl')
    out = ctx.output_dir.joinpath('pyoxidizer.bzl')

    with open(tmpl, 'r') as ins:
        cfg = ins.read().replace('${PW_ROOT}', str(ctx.root))
        with open(out, 'w') as outs:
            outs.write(cfg)

    call('pyoxidizer', 'build', cwd=ctx.output_dir)


def _valid_capitalization(word: str) -> bool:
    """Checks that the word has a capital letter or is not a regular word."""
    return bool(
        any(c.isupper() for c in word)  # Any capitalizatian (iTelephone)
        or not word.isalpha()  # Non-alphabetical (cool_stuff.exe)
        or shutil.which(word)
    )  # Matches an executable (clangd)


def commit_message_format(_: PresubmitContext):
    """Checks that the top commit's message is correctly formatted."""
    if git_repo.commit_author().endswith('gserviceaccount.com'):
        return

    lines = git_repo.commit_message().splitlines()

    # Show limits and current commit message in log.
    _LOG.debug('%-25s%+25s%+22s', 'Line limits', '72|', '72|')
    for line in lines:
        _LOG.debug(line)

    if not lines:
        _LOG.error('The commit message is too short!')
        raise PresubmitFailure

    # Ignore Gerrit-generated reverts.
    if (
        'Revert' in lines[0]
        and 'This reverts commit ' in git_repo.commit_message()
        and 'Reason for revert: ' in git_repo.commit_message()
    ):
        _LOG.warning('Ignoring apparent Gerrit-generated revert')
        return

    # Ignore Gerrit-generated relands
    if (
        'Reland' in lines[0]
        and 'This is a reland of ' in git_repo.commit_message()
        and "Original change's description: " in git_repo.commit_message()
    ):
        _LOG.warning('Ignoring apparent Gerrit-generated reland')
        return

    errors = 0

    if len(lines[0]) > 72:
        _LOG.warning(
            "The commit message's first line must be no longer than "
            '72 characters.'
        )
        _LOG.warning(
            'The first line is %d characters:\n  %s', len(lines[0]), lines[0]
        )
        errors += 1

    if lines[0].endswith('.'):
        _LOG.warning(
            "The commit message's first line must not end with a period:\n %s",
            lines[0],
        )
        errors += 1

    # Check that the first line matches the expected pattern.
    match = re.match(r'^[\w*/]+(?:{[\w* ,]+})?[\w*/]*: (?P<desc>.+)$', lines[0])
    if not match:
        _LOG.warning('The first line does not match the expected format')
        _LOG.warning(
            'Expected:\n\n  module_or_target: The description\n\n'
            'Found:\n\n  %s\n',
            lines[0],
        )
        errors += 1
    elif not _valid_capitalization(match.group('desc').split()[0]):
        _LOG.warning(
            'The first word after the ":" in the first line ("%s") must be '
            'capitalized:\n  %s',
            match.group('desc').split()[0],
            lines[0],
        )
        errors += 1

    if len(lines) > 1 and lines[1]:
        _LOG.warning("The commit message's second line must be blank.")
        _LOG.warning(
            'The second line has %d characters:\n  %s', len(lines[1]), lines[1]
        )
        errors += 1

    # Ignore the line length check for Copybara imports so they can include the
    # commit hash and description for imported commits.
    if not errors and (
        'Copybara import' in lines[0]
        and 'GitOrigin-RevId:' in git_repo.commit_message()
    ):
        _LOG.warning('Ignoring Copybara import')
        return

    # Check that the lines are 72 characters or less, but skip any lines that
    # might possibly have a URL, path, or metadata in them. Also skip any lines
    # with non-ASCII characters.
    for i, line in enumerate(lines[2:], 3):
        if any(c in line for c in ':/>') or not line.isascii():
            continue

        if len(line) > 72:
            _LOG.warning(
                'Commit message lines must be no longer than 72 characters.'
            )
            _LOG.warning('Line %d has %d characters:\n  %s', i, len(line), line)
            errors += 1

    if errors:
        _LOG.error('Found %s in the commit message', plural(errors, 'error'))
        raise PresubmitFailure


@filter_paths(endswith=(*format_code.C_FORMAT.extensions, '.py'))
def static_analysis(ctx: PresubmitContext):
    """Runs all available static analysis tools."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'python.lint', 'static_analysis')


_EXCLUDE_FROM_TODO_CHECK = (
    # keep-sorted: start
    r'.bazelrc$',
    r'.dockerignore$',
    r'.gitignore$',
    r'.pylintrc$',
    r'\bdocs/build_system.rst',
    r'\bpw_assert_basic/basic_handler.cc',
    r'\bpw_assert_basic/public/pw_assert_basic/handler.h',
    r'\bpw_blob_store/public/pw_blob_store/flat_file_system_entry.h',
    r'\bpw_build/linker_script.gni',
    r'\bpw_build/py/pw_build/copy_from_cipd.py',
    r'\bpw_cpu_exception/basic_handler.cc',
    r'\bpw_cpu_exception_cortex_m/entry.cc',
    r'\bpw_cpu_exception_cortex_m/exception_entry_test.cc',
    r'\bpw_doctor/py/pw_doctor/doctor.py',
    r'\bpw_env_setup/util.sh',
    r'\bpw_fuzzer/fuzzer.gni',
    r'\bpw_fuzzer/oss_fuzz.gni',
    r'\bpw_i2c/BUILD.gn',
    r'\bpw_i2c/public/pw_i2c/register_device.h',
    r'\bpw_kvs/flash_memory.cc',
    r'\bpw_kvs/key_value_store.cc',
    r'\bpw_log_basic/log_basic.cc',
    r'\bpw_package/py/pw_package/packages/chromium_verifier.py',
    r'\bpw_protobuf/encoder.cc',
    r'\bpw_rpc/docs.rst',
    r'\bpw_watch/py/pw_watch/watch.py',
    r'\btargets/mimxrt595_evk/BUILD.bazel',
    r'\btargets/stm32f429i_disc1/boot.cc',
    r'\bthird_party/chromium_verifier/BUILD.gn',
    # keep-sorted: end
)


@filter_paths(exclude=_EXCLUDE_FROM_TODO_CHECK)
def todo_check_with_exceptions(ctx: PresubmitContext):
    """Check that non-legacy TODO lines are valid."""  # todo-check: ignore
    todo_check.create(todo_check.BUGS_OR_USERNAMES)(ctx)


@format_code.OWNERS_CODE_FORMAT.filter.apply_to_check()
def owners_lint_checks(ctx: PresubmitContext):
    """Runs OWNERS linter."""
    owners_checks.presubmit_check(ctx.paths)


#
# Presubmit check programs
#

OTHER_CHECKS = (
    # keep-sorted: start
    # TODO(b/235277910): Enable all Bazel tests when they're fixed.
    bazel_test,
    build.gn_gen_check,
    cmake_clang,
    cmake_gcc,
    gitmodules.create(),
    gn_boringssl_build,
    gn_clang_build,
    gn_combined_build_check,
    gn_full_build_check,
    gn_full_qemu_check,
    gn_gcc_build,
    npm_presubmit.npm_test,
    # Attempts to duplicate OSS-Fuzz. Currently failing.
    oss_fuzz_build,
    pw_transfer_integration_test,
    static_analysis,
    stm32f429i,
    todo_check.create(todo_check.BUGS_OR_USERNAMES),
    # keep-sorted: end
)

# The misc program differs from other_checks in that checks in the misc
# program block CQ on Linux.
MISC = (
    # keep-sorted: start
    gn_nanopb_build,
    gn_pico_build,
    gn_pw_system_demo_build,
    gn_teensy_build,
    # keep-sorted: end
)

SANITIZERS = (cpp_checks.all_sanitizers(),)

SECURITY = (
    # keep-sorted: start
    gn_crypto_boringssl_build,
    gn_crypto_mbedtls_build,
    gn_crypto_micro_ecc_build,
    gn_software_update_build,
    # keep-sorted: end
)

# Avoid running all checks on specific paths.
PATH_EXCLUSIONS = (re.compile(r'\bthird_party/fuchsia/repo/'),)

_LINTFORMAT = (
    commit_message_format,
    copyright_notice,
    format_code.presubmit_checks(
        code_formats=format_code.CODE_FORMATS_WITH_BLACK
    ),
    inclusive_language.presubmit_check.with_filter(
        exclude=(
            r'\byarn.lock$',
            r'\bpackage-lock.json$',
        )
    ),
    cpp_checks.pragma_once,
    build.bazel_lint,
    owners_lint_checks,
    source_is_in_build_files,
    shell_checks.shellcheck if shutil.which('shellcheck') else (),
    keep_sorted.presubmit_check,
    todo_check_with_exceptions,
)

LINTFORMAT = (
    _LINTFORMAT,
    pw_presubmit.python_checks.check_python_versions,
    pw_presubmit.python_checks.gn_python_lint,
)

QUICK = (
    _LINTFORMAT,
    gn_quick_build_check,
    # TODO(b/34884583): Re-enable CMake and Bazel for Mac after we have fixed
    # the clang issues. The problem is that all clang++ invocations need the
    # two extra flags: "-nostdc++" and "${clang_prefix}/../lib/libc++.a".
    cmake_clang if sys.platform != 'darwin' else (),
)

FULL = (
    _LINTFORMAT,
    gn_host_build,
    gn_arm_build,
    gn_docs_build,
    gn_host_tools,
    bazel_test if sys.platform == 'linux' else (),
    bazel_build if sys.platform == 'linux' else (),
    # On Mac OS, system 'gcc' is a symlink to 'clang' by default, so skip GCC
    # host builds on Mac for now. Skip it on Windows too, since gn_host_build
    # already uses 'gcc' on Windows.
    gn_gcc_build if sys.platform not in ('darwin', 'win32') else (),
    # Windows doesn't support QEMU yet.
    gn_qemu_build if sys.platform != 'win32' else (),
    gn_qemu_clang_build if sys.platform != 'win32' else (),
    source_is_in_build_files,
    python_checks.gn_python_check,
    python_checks.gn_python_test_coverage,
    build_env_setup,
    # Skip gn_teensy_build if running on Windows. The Teensycore installer is
    # an exe that requires an admin role.
    gn_teensy_build if sys.platform in ['linux', 'darwin'] else (),
)

PROGRAMS = Programs(
    # keep-sorted: start
    full=FULL,
    lintformat=LINTFORMAT,
    misc=MISC,
    other_checks=OTHER_CHECKS,
    quick=QUICK,
    sanitizers=SANITIZERS,
    security=SECURITY,
    # keep-sorted: end
)


def parse_args() -> argparse.Namespace:
    """Creates an argument parser and parses arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    cli.add_arguments(parser, PROGRAMS, 'quick')
    parser.add_argument(
        '--install',
        action='store_true',
        help='Install the presubmit as a Git pre-push hook and exit.',
    )

    return parser.parse_args()


def run(install: bool, exclude: list, **presubmit_args) -> int:
    """Entry point for presubmit."""

    if install:
        install_git_hook(
            'pre-push',
            [
                'python',
                '-m',
                'pw_presubmit.pigweed_presubmit',
                '--base',
                'origin/main..HEAD',
                '--program',
                'quick',
            ],
        )
        return 0

    exclude.extend(PATH_EXCLUSIONS)
    return cli.run(exclude=exclude, **presubmit_args)


def main() -> int:
    """Run the presubmit for the Pigweed repository."""
    return run(**vars(parse_args()))


if __name__ == '__main__':
    try:
        # If pw_cli is available, use it to initialize logs.
        from pw_cli import log

        log.install(logging.INFO)
    except ImportError:
        # If pw_cli isn't available, display log messages like a simple print.
        logging.basicConfig(format='%(message)s', level=logging.INFO)

    sys.exit(main())
