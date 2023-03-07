#!/usr/bin/env python
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
"""Generates a patch file for sources from Fuchsia's fit and stdcompat.

Run this script to update third_party/fuchsia/function.patch.
"""

from pathlib import Path
import re
import subprocess
import tempfile
from typing import Iterable, List, TextIO, Optional, Union
from datetime import datetime

PathOrStr = Union[Path, str]

HEADER = f'''# Copyright {datetime.today().year} The Pigweed Authors
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

# Patch the fit::function implementation for use in Pigweed:
#
#   - Use PW_ASSERT instead of __builtin_abort.
#   - Temporarily disable sanitizers when invoking a function for b/241567321.
#
'''.encode()


def _read_files_list(file: TextIO) -> Iterable[str]:
    """Reads the files list from the copy.bara.sky file."""
    found_list = False

    for line in file:
        if found_list:
            yield line
            if line == ']\n':
                break
        else:
            if line == 'fuchsia_repo_files = [\n':
                found_list = True
                yield '['


def _clone_fuchsia(temp_path: Path) -> Path:
    subprocess.run(
        [
            'git',
            '-C',
            temp_path,
            'clone',
            '--depth',
            '1',
            'https://fuchsia.googlesource.com/fuchsia',
        ],
        check=True,
    )

    return temp_path / 'fuchsia'


# TODO(b/248257406): Replace typing.List with list.  # pylint: disable=fixme
def _read_files(script: Path) -> List[Path]:
    with script.open() as file:
        paths_list: List[str] = eval(  # pylint: disable=eval-used
            ''.join(_read_files_list(file))
        )
        return list(Path(p) for p in paths_list if not 'lib/stdcompat/' in p)


def _add_include_before_namespace(text: str, include: str) -> str:
    return text.replace(
        '\nnamespace ', f'\n#include "{include}"\n\nnamespace ', 1
    )


_ASSERT = re.compile(r'\bassert\(')


def _patch_assert(text: str) -> str:
    replaced = text.replace('__builtin_abort()', 'PW_ASSERT(false)')
    replaced = _ASSERT.sub('PW_ASSERT(', replaced)

    if replaced == text:
        return replaced

    return _add_include_before_namespace(replaced, 'pw_assert/assert.h')


_INVOKE_PATCH = (
    '\n'
    '  // TODO(b/241567321): Remove "no sanitize" after pw_protobuf is fixed.\n'
    '  Result invoke(Args... args) const PW_NO_SANITIZE("function") {'
)


def _patch_invoke(file: Path, text: str) -> str:
    # Update internal/function.h only.
    if file.name != 'function.h' or file.parent.name != 'internal':
        return text

    text = _add_include_before_namespace(text, 'pw_preprocessor/compiler.h')
    return text.replace(
        '\n  Result invoke(Args... args) const {', _INVOKE_PATCH
    )


def _patch(file: Path) -> Optional[str]:
    text = file.read_text()
    updated = _patch_assert(text)
    updated = _patch_invoke(file, updated)
    return None if text == updated else updated


def _main() -> None:
    output_path = Path(__file__).parent

    # Clone Fuchsia to a temp directory
    with tempfile.TemporaryDirectory() as directory:
        repo = _clone_fuchsia(Path(directory))

        # Read the files list from copy.bara.sky and patch those files.
        paths = _read_files(output_path / 'copy.bara.sky')
        for file in (repo / path for path in paths):
            if (text := _patch(file)) is not None:
                print('Patching', file)
                file.write_text(text)
                subprocess.run(['clang-format', '-i', file], check=True)

        # Create a diff for the changes.
        diff = subprocess.run(
            ['git', '-C', repo, 'diff'], stdout=subprocess.PIPE, check=True
        ).stdout
        for path in paths:
            diff = diff.replace(
                path.as_posix().encode(),
                Path('third_party/fuchsia/repo', path).as_posix().encode(),
            )

    # Write the diff to function.patch.
    with output_path.joinpath('pigweed_adaptations.patch').open('wb') as output:
        output.write(HEADER)

        for line in diff.splitlines(keepends=True):
            if line == b' \n':
                output.write(b'\n')
            elif not line.startswith(b'index '):  # drop line with Git hashes
                output.write(line)


if __name__ == '__main__':
    _main()
