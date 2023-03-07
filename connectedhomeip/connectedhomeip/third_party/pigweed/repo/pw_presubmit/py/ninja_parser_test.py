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
"""Tests for ninja_parser."""

from pathlib import Path
import unittest
from unittest.mock import MagicMock, mock_open, patch

from pw_presubmit import ninja_parser

_STOP = 'ninja: build stopped:\n'

_REAL_TEST_INPUT = """
[1168/1797] cp ../../pw_software_update/py/dev_sign_test.py python/gen/pw_software_update/py/py.generated_python_package/dev_sign_test.py
[1169/1797] ACTION //pw_presubmit/py:py.lint.mypy(//pw_build/python_toolchain:python)
FAILED: python/gen/pw_presubmit/py/py.lint.mypy.pw_pystamp
python ../../pw_build/py/pw_build/python_runner.py --gn-root ../../ --current-path ../../pw_presubmit/py --default-toolchain=//pw_toolchain/default:default --current-toolchain=//pw_build/python_toolchain:python --env=MYPY_FORCE_COLOR=1 --touch python/gen/pw_presubmit/py/py.lint.mypy.pw_pystamp --capture-output --module mypy --python-virtualenv-config python/gen/pw_env_setup/pigweed_build_venv/venv_metadata.json --python-dep-list-files python/gen/pw_presubmit/py/py.lint.mypy_metadata_path_list.txt -- --pretty --show-error-codes ../../pw_presubmit/py/pw_presubmit/__init__.py ../../pw_presubmit/py/pw_presubmit/build.py ../../pw_presubmit/py/pw_presubmit/cli.py ../../pw_presubmit/py/pw_presubmit/cpp_checks.py ../../pw_presubmit/py/pw_presubmit/format_code.py ../../pw_presubmit/py/pw_presubmit/git_repo.py ../../pw_presubmit/py/pw_presubmit/inclusive_language.py ../../pw_presubmit/py/pw_presubmit/install_hook.py ../../pw_presubmit/py/pw_presubmit/keep_sorted.py ../../pw_presubmit/py/pw_presubmit/ninja_parser.py ../../pw_presubmit/py/pw_presubmit/npm_presubmit.py ../../pw_presubmit/py/pw_presubmit/pigweed_presubmit.py ../../pw_presubmit/py/pw_presubmit/presubmit.py ../../pw_presubmit/py/pw_presubmit/python_checks.py ../../pw_presubmit/py/pw_presubmit/shell_checks.py ../../pw_presubmit/py/pw_presubmit/todo_check.py ../../pw_presubmit/py/pw_presubmit/tools.py ../../pw_presubmit/py/git_repo_test.py ../../pw_presubmit/py/keep_sorted_test.py ../../pw_presubmit/py/ninja_parser_test.py ../../pw_presubmit/py/presubmit_test.py ../../pw_presubmit/py/tools_test.py ../../pw_presubmit/py/setup.py
../../pw_presubmit/py/presubmit_test.py:63: error: Module has no attribute
"Filter"  [attr-defined]
            TestData(presubmit.Filter(suffix=('.a', '.b')), 'foo.c', False...
                     ^
Found 1 error in 1 file (checked 23 source files)
[1170/1797] stamp python/obj/pw_snapshot/metadata_proto.python._mirror_sources_to_out_dir.stamp
[1171/1797] stamp python/obj/pw_software_update/py/py._mirror_sources_to_out_dir_dev_sign_test.py.stamp
[1172/1797] ACTION //pw_log:protos.python(//pw_build/python_toolchain:python)
[1173/1797] ACTION //pw_thread_freertos/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1174/1797] ACTION //pw_symbolizer/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1175/1797] ACTION //pw_symbolizer/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1176/1797] ACTION //pw_thread_freertos/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1177/1797] ACTION //pw_tls_client/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1178/1797] ACTION //pw_symbolizer/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1179/1797] ACTION //pw_console/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1180/1797] ACTION //pw_tls_client/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1181/1797] ACTION //pw_console/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1182/1797] ACTION //pw_console/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1183/1797] ACTION //pw_tls_client/py:py.lint.mypy(//pw_build/python_toolchain:python)
[1184/1797] ACTION //pw_symbolizer/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1185/1797] ACTION //pw_thread_freertos/py:py.lint.pylint(//pw_build/python_toolchain:python)
[1186/1797] ACTION //pw_tls_client/py:py.lint.pylint(//pw_build/python_toolchain:python)
ninja: build stopped: subcommand failed.
[FINISHED]
"""

_REAL_TEST_SUMMARY = """
[1169/1797] ACTION //pw_presubmit/py:py.lint.mypy(//pw_build/python_toolchain:python)
FAILED: python/gen/pw_presubmit/py/py.lint.mypy.pw_pystamp
python ../../pw_build/py/pw_build/python_runner.py --gn-root ../../ --current-path ../../pw_presubmit/py --default-toolchain=//pw_toolchain/default:default --current-toolchain=//pw_build/python_toolchain:python --env=MYPY_FORCE_COLOR=1 --touch python/gen/pw_presubmit/py/py.lint.mypy.pw_pystamp --capture-output --module mypy --python-virtualenv-config python/gen/pw_env_setup/pigweed_build_venv/venv_metadata.json --python-dep-list-files python/gen/pw_presubmit/py/py.lint.mypy_metadata_path_list.txt -- --pretty --show-error-codes ../../pw_presubmit/py/pw_presubmit/__init__.py ../../pw_presubmit/py/pw_presubmit/build.py ../../pw_presubmit/py/pw_presubmit/cli.py ../../pw_presubmit/py/pw_presubmit/cpp_checks.py ../../pw_presubmit/py/pw_presubmit/format_code.py ../../pw_presubmit/py/pw_presubmit/git_repo.py ../../pw_presubmit/py/pw_presubmit/inclusive_language.py ../../pw_presubmit/py/pw_presubmit/install_hook.py ../../pw_presubmit/py/pw_presubmit/keep_sorted.py ../../pw_presubmit/py/pw_presubmit/ninja_parser.py ../../pw_presubmit/py/pw_presubmit/npm_presubmit.py ../../pw_presubmit/py/pw_presubmit/pigweed_presubmit.py ../../pw_presubmit/py/pw_presubmit/presubmit.py ../../pw_presubmit/py/pw_presubmit/python_checks.py ../../pw_presubmit/py/pw_presubmit/shell_checks.py ../../pw_presubmit/py/pw_presubmit/todo_check.py ../../pw_presubmit/py/pw_presubmit/tools.py ../../pw_presubmit/py/git_repo_test.py ../../pw_presubmit/py/keep_sorted_test.py ../../pw_presubmit/py/ninja_parser_test.py ../../pw_presubmit/py/presubmit_test.py ../../pw_presubmit/py/tools_test.py ../../pw_presubmit/py/setup.py
../../pw_presubmit/py/presubmit_test.py:63: error: Module has no attribute
"Filter"  [attr-defined]
            TestData(presubmit.Filter(suffix=('.a', '.b')), 'foo.c', False...
                     ^
Found 1 error in 1 file (checked 23 source files)
"""


class TestNinjaParser(unittest.TestCase):
    """Test ninja_parser."""

    def _run(self, contents: str) -> str:  # pylint: disable=no-self-use
        path = MagicMock(spec=Path('foo/bar'))

        def mocked_open_read(*args, **kwargs):
            return mock_open(read_data=contents)(*args, **kwargs)

        with patch.object(path, 'open', mocked_open_read):
            return ninja_parser.parse_ninja_stdout(path)

    def test_simple(self) -> None:
        error = '[2/10] baz\nFAILED: something\nerror 1\nerror 2\n'
        result = self._run('[0/10] foo\n[1/10] bar\n' + error + _STOP)
        self.assertEqual(error.strip(), result.strip())

    def test_short(self) -> None:
        error = '[2/10] baz\nFAILED: something\n'
        result = self._run('[0/10] foo\n[1/10] bar\n' + error + _STOP)
        self.assertEqual(error.strip(), result.strip())

    def test_unexpected(self) -> None:
        error = '[2/10] baz\nERROR: something\nerror 1\n'
        result = self._run('[0/10] foo\n[1/10] bar\n' + error)
        self.assertEqual('', result.strip())

    def test_real(self) -> None:
        result = self._run(_REAL_TEST_INPUT)
        self.assertEqual(_REAL_TEST_SUMMARY.strip(), result.strip())


if __name__ == '__main__':
    unittest.main()
