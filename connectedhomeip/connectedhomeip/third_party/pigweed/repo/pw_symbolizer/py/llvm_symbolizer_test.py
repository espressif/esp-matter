# Copyright 2021 The Pigweed Authors
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
"""Tests for pw_symbolizer's llvm-symbolizer based symbolization."""

import subprocess
import tempfile
import unittest
import json
from pathlib import Path
import pw_symbolizer

_MODULE_PY_DIR = Path(__file__).parent.resolve()
_CPP_TEST_FILE_NAME = 'symbolizer_test.cc'

_COMPILER = 'clang++'


class TestSymbolizer(unittest.TestCase):
    """Unit tests for binary symbolization."""

    def _test_symbolization_results(self, expected_symbols, symbolizer):
        for expected_symbol in expected_symbols:
            result = symbolizer.symbolize(expected_symbol['Address'])
            self.assertEqual(result.name, expected_symbol['Expected'])
            self.assertEqual(result.address, expected_symbol['Address'])

            # Objects sometimes don't have a file/line number for some
            # reason.
            if not expected_symbol['IsObj']:
                self.assertEqual(result.file, _CPP_TEST_FILE_NAME)
                self.assertEqual(result.line, expected_symbol['Line'])

    def test_symbolization(self):
        """Tests that the symbolizer can symbolize addresses properly."""
        with tempfile.TemporaryDirectory() as exe_dir:
            exe_file = Path(exe_dir) / 'print_expected_symbols'

            # Compiles a binary that prints symbol addresses and expected
            # results as JSON.
            cmd = [
                _COMPILER,
                _CPP_TEST_FILE_NAME,
                '-gfull',
                f'-ffile-prefix-map={_MODULE_PY_DIR}=',
                '-std=c++17',
                '-fno-pic',
                '-fno-pie',
                '-nopie',
                '-o',
                exe_file,
            ]

            process = subprocess.run(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                cwd=_MODULE_PY_DIR,
            )
            self.assertEqual(process.returncode, 0)

            process = subprocess.run(
                [exe_file], stdout=subprocess.PIPE, stderr=subprocess.STDOUT
            )
            self.assertEqual(process.returncode, 0)

            expected_symbols = [
                json.loads(line)
                for line in process.stdout.decode().splitlines()
            ]

            symbolizer = pw_symbolizer.LlvmSymbolizer(exe_file)
            self._test_symbolization_results(expected_symbols, symbolizer)

            # Test backwards compatibility with older versions of
            # llvm-symbolizer.
            symbolizer = pw_symbolizer.LlvmSymbolizer(
                exe_file, force_legacy=True
            )
            self._test_symbolization_results(expected_symbols, symbolizer)


if __name__ == '__main__':
    unittest.main()
