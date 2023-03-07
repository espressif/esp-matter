#!/usr/bin/env python
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
"""Generates a BUILD.gn for a Python package.

Pass the script a list of paths to the root directory of a Python package (where
setup.py is).

Don't forget to add the pw_python_package to a top-level group, or it will not
be included in the build.
"""

from datetime import datetime
from pathlib import Path
import sys
from typing import Iterable, List, NamedTuple

from pw_presubmit import git_repo

_HEADER = f"""\
# Copyright {datetime.now().year} The Pigweed Authors
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

import("//build_overrides/pigweed.gni")

import("$dir_pw_build/python.gni")
"""


class PackageFiles(NamedTuple):
    setup: List[Path]
    sources: List[Path]
    tests: List[Path]
    other: List[Path]


def _find_package_files(root_dir: Path) -> PackageFiles:
    files = git_repo.list_files(
        pathspecs=('*.py', '*.toml', '*.cfg'), repo_path=root_dir
    )

    package_files = PackageFiles([], [], [], [])

    for file in files:
        if file.parent == root_dir:
            if file.name == 'setup.py' or file.suffix != '.py':
                package_files.setup.append(file)
            elif file.stem.startswith('test_') or file.stem.endswith('_test'):
                package_files.tests.append(file)
            else:
                package_files.other.append(file)
        else:
            package_files.sources.append(file)

    return package_files


def _gn_list(name: str, files: Iterable[Path], base: Path) -> Iterable[str]:
    if files:
        yield f'  {name} = ['
        for file in files:
            yield f'    "{file.relative_to(base).as_posix()}",'
        yield '  ]'


def generate_build_gn(root_dir: Path):
    files = _find_package_files(root_dir)

    yield _HEADER

    yield 'pw_python_package("py") {'

    yield from _gn_list('setup', files.setup, root_dir)
    yield from _gn_list('sources', files.sources, root_dir)
    yield from _gn_list('tests', files.tests, root_dir)

    # Don't include the "other" files for now.
    # yield from _gn_list('other', files.other, root_dir)

    yield '}'


def main(paths: Iterable[Path]):
    for path in paths:
        path.joinpath('BUILD.gn').write_text(
            '\n'.join(generate_build_gn(path)) + '\n'
        )


if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(Path(p).resolve() for p in sys.argv[1:])
    else:
        print(__file__, '', __doc__.strip(), sep='\n', file=sys.stderr)
        sys.exit(1)
