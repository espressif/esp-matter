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
"""Mirrors a directory tree to another directory using hard links."""

import argparse
import os
from pathlib import Path
import shutil
from typing import Iterable, Iterator, List, Optional


def _parse_args() -> argparse.Namespace:
    """Registers the script's arguments on an argument parser."""

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--source-root',
        type=Path,
        required=True,
        help='Prefix to strip from the source files',
    )
    parser.add_argument(
        'sources', type=Path, nargs='*', help='Files to mirror to the directory'
    )
    parser.add_argument(
        '--directory',
        type=Path,
        required=True,
        help='Directory to which to mirror the sources',
    )
    parser.add_argument(
        '--path-file', type=Path, help='File with paths to files to mirror'
    )

    return parser.parse_args()


def _link_files(
    source_root: Path, sources: Iterable[Path], directory: Path
) -> Iterator[Path]:
    for source in sources:
        dest = directory / source.relative_to(source_root)
        dest.parent.mkdir(parents=True, exist_ok=True)

        if dest.exists():
            dest.unlink()

        # Use a hard link to avoid unnecessary copies. Resolve the source before
        # linking in case it is a symlink.
        source = source.resolve()
        try:
            os.link(source, dest)
            yield dest

        # If the link failed try copying. If copying fails re-raise the
        # original exception.
        except OSError:
            shutil.copy(source, dest)
            yield dest


def _link_files_or_dirs(
    paths: Iterable[Path], directory: Path
) -> Iterator[Path]:
    """Links files or directories into the output directory.

    Files are linked directly; files in directories are linked as relative paths
    from the directory.
    """

    for path in paths:
        if path.is_dir():
            files = (p for p in path.glob('**/*') if p.is_file())
            yield from _link_files(path, files, directory)
        elif path.is_file():
            yield from _link_files(path.parent, [path], directory)
        else:
            raise FileNotFoundError(f'{path} does not exist!')


def mirror_paths(
    source_root: Path,
    sources: Iterable[Path],
    directory: Path,
    path_file: Optional[Path] = None,
) -> List[Path]:
    """Creates hard links in the provided directory for the provided sources.

    Args:
      source_root: Base path for files in sources.
      sources: Files to link to from the directory.
      directory: The output directory.
      path_file: A file with file or directory paths to link to.
    """
    directory.mkdir(parents=True, exist_ok=True)

    outputs = list(_link_files(source_root, sources, directory))

    if path_file:
        paths = (Path(p).resolve() for p in path_file.read_text().splitlines())
        outputs.extend(_link_files_or_dirs(paths, directory))

    return outputs


if __name__ == '__main__':
    mirror_paths(**vars(_parse_args()))
