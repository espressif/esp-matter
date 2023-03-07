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
"""Utilities for address symbolization."""

import abc
from typing import Iterable, List, Optional
from dataclasses import dataclass


@dataclass(frozen=True)
class Symbol:
    """Symbols produced by a symbolizer."""

    address: int
    name: str = ''
    file: str = ''
    line: int = 0

    def to_string(self, max_filename_len: int = 0) -> str:
        if not self.name:
            name = f'0x{self.address:08X}'
        else:
            name = self.name

        return f'{name} ({self.file_and_line(max_filename_len)})'

    def file_and_line(self, max_filename_len: int = 0) -> str:
        """Returns a file/line number string, with question marks if unknown."""

        if not self.file:
            return '??:?'

        if max_filename_len and len(self.file) > max_filename_len:
            return f'[...]{self.file[-max_filename_len:]}:{self.line}'

        return f'{self.file}:{self.line}'

    def __str__(self):
        return self.to_string()


class Symbolizer(abc.ABC):
    """An interface for symbolizing addresses."""

    @abc.abstractmethod
    def symbolize(self, address: int) -> Symbol:
        """Symbolizes an address using a loaded binary or symbol database."""

    def dump_stack_trace(
        self, addresses, most_recent_first: bool = True
    ) -> str:
        """Symbolizes and dumps a list of addresses as a stack trace.

        most_recent_first controls the hint provided at the top of the stack
        trace. If call stack depth increases with each element in the input
        list, most_recent_first should be false.
        """
        order: str = 'first' if most_recent_first else 'last'

        stack_trace: List[str] = []
        stack_trace.append(f'Stack Trace (most recent call {order}):')

        max_width = len(str(len(addresses)))
        for i, address in enumerate(addresses):
            depth = i + 1
            symbol = self.symbolize(address)

            if symbol.name:
                sym_desc = f'{symbol.name} (0x{symbol.address:08X})'
            else:
                sym_desc = f'(0x{symbol.address:08X})'

            stack_trace.append(f'  {depth:>{max_width}}: at {sym_desc}')
            stack_trace.append(f'      in {symbol.file_and_line()}')

        return '\n'.join(stack_trace)


class FakeSymbolizer(Symbolizer):
    """A fake symbolizer that only knows a fixed set of symbols."""

    def __init__(self, known_symbols: Optional[Iterable[Symbol]] = None):
        if known_symbols is not None:
            self._db = {sym.address: sym for sym in known_symbols}
        else:
            self._db = {}

    def symbolize(self, address: int) -> Symbol:
        """Symbolizes a fixed symbol database."""
        if address in self._db:
            return self._db[address]

        return Symbol(address)
