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
"""Executes a compilation failure test."""

import argparse
import logging
from pathlib import Path
import re
import string
import sys
import subprocess
from typing import Dict, List, Optional

import pw_cli.log

from pw_compilation_testing.generator import Compiler, Expectation, TestCase

_LOG = logging.getLogger(__package__)

_RULE_REGEX = re.compile('^rule (?:cxx|.*_cxx)$')
_NINJA_VARIABLE = re.compile('^([a-zA-Z0-9_]+) = ?')


# TODO(hepler): Could do this step just once and output the results.
def find_cc_rule(toolchain_ninja_file: Path) -> Optional[str]:
    """Searches the toolchain.ninja file for the cc rule."""
    cmd_prefix = '  command = '

    found_rule = False

    with toolchain_ninja_file.open() as fd:
        for line in fd:
            if found_rule:
                if line.startswith(cmd_prefix):
                    cmd = line[len(cmd_prefix) :].strip()
                    if cmd.startswith('ccache '):
                        cmd = cmd[len('ccache ') :]
                    return cmd

                if not line.startswith('  '):
                    break
            elif _RULE_REGEX.match(line):
                found_rule = True

    return None


def _parse_ninja_variables(target_ninja_file: Path) -> Dict[str, str]:
    variables: Dict[str, str] = {}

    with target_ninja_file.open() as fd:
        for line in fd:
            match = _NINJA_VARIABLE.match(line)
            if match:
                variables[match.group(1)] = line[match.end() :].strip()

    return variables


_EXPECTED_GN_VARS = (
    'asmflags',
    'cflags',
    'cflags_c',
    'cflags_cc',
    'cflags_objc',
    'cflags_objcc',
    'defines',
    'include_dirs',
)

_ENABLE_TEST_MACRO = '-DPW_NC_TEST_EXECUTE_CASE_'
# Regular expression to find and remove ANSI escape sequences, based on
# https://stackoverflow.com/a/14693789.
_ANSI_ESCAPE_SEQUENCES = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')


class _TestFailure(Exception):
    pass


def _red(message: str) -> str:
    return f'\033[31m\033[1m{message}\033[0m'


_TITLE_1 = '     NEGATIVE     '
_TITLE_2 = ' COMPILATION TEST '

_BOX_TOP = f'┏{"━" * len(_TITLE_1)}┓'
_BOX_MID_1 = f'┃{_red(_TITLE_1)}┃ \033[1m{{test_name}}\033[0m'
_BOX_MID_2 = f'┃{_red(_TITLE_2)}┃ \033[0m{{source}}:{{line}}\033[0m'
_BOX_BOT = f'┻{"━" * (len(_TITLE_1))}┻{"━" * (77 - len(_TITLE_1))}┓'
_FOOTER = '\n' + '━' * 79 + '┛'


def _start_failure(test: TestCase, command: str) -> None:
    print(_BOX_TOP, file=sys.stderr)
    print(_BOX_MID_1.format(test_name=test.name()), file=sys.stderr)
    print(
        _BOX_MID_2.format(source=test.source, line=test.line), file=sys.stderr
    )
    print(_BOX_BOT, file=sys.stderr)
    print(file=sys.stderr)

    _LOG.debug('Compilation command:\n%s', command)


def _check_results(
    test: TestCase, command: str, process: subprocess.CompletedProcess
) -> None:
    stderr = process.stderr.decode(errors='replace')

    if process.returncode == 0:
        _start_failure(test, command)
        _LOG.error('Compilation succeeded, but it should have failed!')
        _LOG.error('Update the test code so that is fails to compile.')
        raise _TestFailure

    compiler_str = command.split(' ', 1)[0]
    compiler = Compiler.from_command(compiler_str)

    _LOG.debug('%s is %s', compiler_str, compiler)
    expectations: List[Expectation] = [
        e for e in test.expectations if compiler.matches(e.compiler)
    ]

    _LOG.debug(
        '%s: Checking compilation from %s (%s) for %d of %d patterns:',
        test.name(),
        compiler_str,
        compiler,
        len(expectations),
        len(test.expectations),
    )
    for expectation in expectations:
        _LOG.debug('    %s', expectation.pattern.pattern)

    if not expectations:
        _start_failure(test, command)
        _LOG.error(
            'Compilation with %s failed, but no PW_NC_EXPECT() patterns '
            'that apply to %s were provided',
            compiler_str,
            compiler_str,
        )

        _LOG.error('Compilation output:\n%s', stderr)
        _LOG.error('')
        _LOG.error(
            'Add at least one PW_NC_EXPECT("<regex>") or '
            'PW_NC_EXPECT_%s("<regex>") expectation to %s',
            compiler.name,
            test.case,
        )
        raise _TestFailure

    no_color = _ANSI_ESCAPE_SEQUENCES.sub('', stderr)

    failed = [e for e in expectations if not e.pattern.search(no_color)]
    if failed:
        _start_failure(test, command)
        _LOG.error(
            'Compilation with %s failed, but the output did not '
            'match the expected patterns.',
            compiler_str,
        )
        _LOG.error(
            '%d of %d expected patterns did not match:',
            len(failed),
            len(expectations),
        )
        _LOG.error('')
        for expectation in expectations:
            _LOG.error(
                '  %s %s:%d: %s',
                '❌' if expectation in failed else '✅',
                test.source.name,
                expectation.line,
                expectation.pattern.pattern,
            )
        _LOG.error('')

        _LOG.error('Compilation output:\n%s', stderr)
        _LOG.error('')
        _LOG.error(
            'Update the test so that compilation fails with the '
            'expected output'
        )
        raise _TestFailure


def _execute_test(
    test: TestCase,
    command: str,
    variables: Dict[str, str],
    all_tests: List[str],
) -> None:
    variables['in'] = str(test.source)

    command = string.Template(command).substitute(variables)
    command = ' '.join(
        [
            command,
            '-DPW_NEGATIVE_COMPILATION_TESTS_ENABLED',
            # Define macros to disable all tests except this one.
            *(
                f'{_ENABLE_TEST_MACRO}{t}={1 if test.case == t else 0}'
                for t in all_tests
            ),
        ]
    )
    process = subprocess.run(command, shell=True, capture_output=True)

    _check_results(test, command, process)


def _main(
    test: TestCase, toolchain_ninja: Path, target_ninja: Path, all_tests: Path
) -> int:
    """Compiles a compile fail test and returns 1 if compilation succeeds."""
    command = find_cc_rule(toolchain_ninja)

    if command is None:
        _LOG.critical(
            'Failed to find C++ compilation command in %s', toolchain_ninja
        )
        return 2

    variables = {key: '' for key in _EXPECTED_GN_VARS}
    variables.update(_parse_ninja_variables(target_ninja))

    variables['out'] = str(
        target_ninja.parent / f'{target_ninja.stem}.compile_fail_test.out'
    )

    try:
        _execute_test(
            test, command, variables, all_tests.read_text().splitlines()
        )
    except _TestFailure:
        print(_FOOTER, file=sys.stderr)
        return 1

    return 0


def _parse_args() -> dict:
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(
        description='Emits an error when a facade has a null backend'
    )
    parser.add_argument(
        '--toolchain-ninja',
        type=Path,
        required=True,
        help='Ninja file with the compilation command for the toolchain',
    )
    parser.add_argument(
        '--target-ninja',
        type=Path,
        required=True,
        help='Ninja file with the compilation commands to the test target',
    )
    parser.add_argument(
        '--test-data',
        dest='test',
        required=True,
        type=TestCase.deserialize,
        help='Serialized TestCase object',
    )
    parser.add_argument('--all-tests', type=Path, help='List of all tests')
    return vars(parser.parse_args())


if __name__ == '__main__':
    pw_cli.log.install(level=logging.INFO)
    sys.exit(_main(**_parse_args()))
