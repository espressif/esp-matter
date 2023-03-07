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
"""Defines a class used to write code to an output buffer."""

from typing import List


class OutputFile:
    """A buffer to which data is written.

    Example:

    ```
    output = Output("hello.c")
    output.write_line('int main(void) {')
    with output.indent():
        output.write_line('printf("Hello, world");')
        output.write_line('return 0;')
    output.write_line('}')

    print(output.content())
    ```

    Produces:
    ```
    int main(void) {
      printf("Hello, world");
      return 0;
    }
    ```
    """

    INDENT_WIDTH = 2

    def __init__(self, filename: str):
        self._filename: str = filename
        self._content: List[str] = []
        self._indentation: int = 0

    def write_line(self, line: str = '') -> None:
        if line:
            self._content.append(' ' * self._indentation)
            self._content.append(line)
        self._content.append('\n')

    def indent(
        self,
        amount: int = INDENT_WIDTH,
    ) -> 'OutputFile._IndentationContext':
        """Increases the indentation level of the output."""
        return self._IndentationContext(self, amount)

    def name(self) -> str:
        return self._filename

    def content(self) -> str:
        return ''.join(self._content)

    class _IndentationContext:
        """Context that increases the output's indentation when it is active."""

        def __init__(self, output: 'OutputFile', amount: int):
            self._output = output
            self._amount: int = amount

        def __enter__(self):
            self._output._indentation += self._amount

        def __exit__(self, typ, value, traceback):
            self._output._indentation -= self._amount
