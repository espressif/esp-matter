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
"""A symbolizer based on llvm-symbolizer."""

import shutil
import subprocess
import threading
import json
from typing import Optional, Tuple
from pathlib import Path
from pw_symbolizer import symbolizer


class LlvmSymbolizer(symbolizer.Symbolizer):
    """A symbolizer that wraps llvm-symbolizer."""

    def __init__(self, binary: Optional[Path] = None, force_legacy=False):
        # Lets destructor return cleanly if the binary is not found.
        self._symbolizer = None
        if shutil.which('llvm-symbolizer') is None:
            raise FileNotFoundError(
                'llvm-symbolizer not installed. Run bootstrap, or download '
                'LLVM (https://github.com/llvm/llvm-project/releases/) and add '
                'the tools to your system PATH'
            )

        # Prefer JSON output as it's easier to decode.
        if force_legacy:
            self._json_mode = False
        else:
            self._json_mode = LlvmSymbolizer._is_json_compatibile()

        if binary is not None:
            if not binary.exists():
                raise FileNotFoundError(binary)

            output_style = 'JSON' if self._json_mode else 'LLVM'
            cmd = [
                'llvm-symbolizer',
                '--no-inlines',
                '--demangle',
                '--functions',
                f'--output-style={output_style}',
                '--exe',
                str(binary),
            ]
            self._symbolizer = subprocess.Popen(
                cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE
            )

            self._lock: threading.Lock = threading.Lock()

    def __del__(self):
        if self._symbolizer:
            self._symbolizer.terminate()
            self._symbolizer.wait()

    @staticmethod
    def _is_json_compatibile() -> bool:
        """Checks llvm-symbolizer to ensure compatibility"""
        result = subprocess.run(
            ('llvm-symbolizer', '--help'),
            stdout=subprocess.PIPE,
            stdin=subprocess.PIPE,
        )
        for line in result.stdout.decode().splitlines():
            if '--output-style' in line and 'JSON' in line:
                return True

        return False

    @staticmethod
    def _read_json_symbol(address, stdout) -> symbolizer.Symbol:
        """Reads a single symbol from llvm-symbolizer's JSON output mode."""
        results = json.loads(stdout.readline().decode())
        # The symbol resolution should give us at least one symbol, even
        # if it's largely empty.
        assert len(results["Symbol"]) > 0

        # Get the first symbol.
        symbol = results["Symbol"][0]

        return symbolizer.Symbol(
            address=address,
            name=symbol['FunctionName'],
            file=symbol['FileName'],
            line=symbol['Line'],
        )

    @staticmethod
    def _llvm_output_line_splitter(file_and_line: str) -> Tuple[str, int]:
        split = file_and_line.split(':')
        # LLVM file name output is as follows:
        #   path/to/src.c:123:1
        # Where the last number is the discriminator, the second to last the
        # line number, and all leading characters the file name. For now,
        # this class ignores discriminators.
        line_number_str = split[-2]
        file = ':'.join(split[:-2])

        if not line_number_str:
            raise ValueError(f'Bad symbol format: {file_and_line}')

        # For unknown file names, mark as blank.
        if file.startswith('?'):
            return ('', 0)

        return (file, int(line_number_str))

    @staticmethod
    def _read_llvm_symbol(address, stdout) -> symbolizer.Symbol:
        """Reads a single symbol from llvm-symbolizer's LLVM output mode."""
        symbol = stdout.readline().decode().strip()
        file_and_line = stdout.readline().decode().strip()

        # Might have gotten multiple symbol matches, drop all of the other ones.
        # The results of a symbol are denoted by an empty newline.
        while stdout.readline().decode() != '\n':
            pass

        if symbol.startswith('?'):
            return symbolizer.Symbol(address)

        file, line_number = LlvmSymbolizer._llvm_output_line_splitter(
            file_and_line
        )

        return symbolizer.Symbol(address, symbol, file, line_number)

    def symbolize(self, address: int) -> symbolizer.Symbol:
        """Symbolizes an address using the loaded ELF file."""
        if not self._symbolizer:
            return symbolizer.Symbol(address=address, name='', file='', line=0)

        with self._lock:
            if self._symbolizer.returncode is not None:
                raise ValueError('llvm-symbolizer closed unexpectedly')

            stdin = self._symbolizer.stdin
            stdout = self._symbolizer.stdout

            assert stdin is not None
            assert stdout is not None

            stdin.write(f'0x{address:08X}\n'.encode())
            stdin.flush()

            if self._json_mode:
                return LlvmSymbolizer._read_json_symbol(address, stdout)

            return LlvmSymbolizer._read_llvm_symbol(address, stdout)
