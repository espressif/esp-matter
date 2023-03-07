# Copyright 2019 The Pigweed Authors
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
"""Renders HTML documentation using Sphinx."""

# TODO(frolv): Figure out a solution for installing all library dependencies
# to run Sphinx and build RTD docs.

import argparse
import collections
import json
import os
import shutil
import subprocess
import sys

from pathlib import Path
from typing import Dict, List, Optional, Tuple

SCRIPT_HEADER: str = '''
██████╗ ██╗ ██████╗ ██╗    ██╗███████╗███████╗██████╗     ██████╗  ██████╗  ██████╗███████╗
██╔══██╗██║██╔════╝ ██║    ██║██╔════╝██╔════╝██╔══██╗    ██╔══██╗██╔═══██╗██╔════╝██╔════╝
██████╔╝██║██║  ███╗██║ █╗ ██║█████╗  █████╗  ██║  ██║    ██║  ██║██║   ██║██║     ███████╗
██╔═══╝ ██║██║   ██║██║███╗██║██╔══╝  ██╔══╝  ██║  ██║    ██║  ██║██║   ██║██║     ╚════██║
██║     ██║╚██████╔╝╚███╔███╔╝███████╗███████╗██████╔╝    ██████╔╝╚██████╔╝╚██████╗███████║
╚═╝     ╚═╝ ╚═════╝  ╚══╝╚══╝ ╚══════╝╚══════╝╚═════╝     ╚═════╝  ╚═════╝  ╚═════╝╚══════╝
'''


def parse_args() -> argparse.Namespace:
    """Parses command-line arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--sphinx-build-dir',
        required=True,
        help='Directory in which to build docs',
    )
    parser.add_argument(
        '--conf', required=True, help='Path to conf.py file for Sphinx'
    )
    parser.add_argument(
        '--gn-root', required=True, help='Root of the GN build tree'
    )
    parser.add_argument(
        '--gn-gen-root', required=True, help='Root of the GN gen tree'
    )
    parser.add_argument(
        'sources', nargs='+', help='Paths to the root level rst source files'
    )
    parser.add_argument(
        '--out-dir',
        required=True,
        help='Output directory for rendered HTML docs',
    )
    parser.add_argument(
        '--metadata',
        required=True,
        type=argparse.FileType('r'),
        help='Metadata JSON file',
    )
    parser.add_argument(
        '--google-analytics-id',
        const=None,
        help='Enables Google Analytics with the provided ID',
    )
    return parser.parse_args()


def build_docs(
    src_dir: str, dst_dir: str, google_analytics_id: Optional[str] = None
) -> int:
    """Runs Sphinx to render HTML documentation from a doc tree."""

    # TODO(frolv): Specify the Sphinx script from a prebuilts path instead of
    # requiring it in the tree.
    command = ['sphinx-build', '-W', '-b', 'html', '-d', f'{dst_dir}/help']

    if google_analytics_id is not None:
        command.append(f'-Dgoogle_analytics_id={google_analytics_id}')

    command.extend([src_dir, f'{dst_dir}/html'])

    return subprocess.call(command)


def copy_doc_tree(args: argparse.Namespace) -> None:
    """Copies doc source and input files into a build tree."""

    def build_path(path):
        """Converts a source path to a filename in the build directory."""
        if path.startswith(args.gn_root):
            path = os.path.relpath(path, args.gn_root)
        elif path.startswith(args.gn_gen_root):
            path = os.path.relpath(path, args.gn_gen_root)

        return os.path.join(args.sphinx_build_dir, path)

    source_files = json.load(args.metadata)
    copy_paths = [build_path(f) for f in source_files]

    os.makedirs(args.sphinx_build_dir)
    for source_path in args.sources:
        os.link(
            source_path, f'{args.sphinx_build_dir}/{Path(source_path).name}'
        )
    os.link(args.conf, f'{args.sphinx_build_dir}/conf.py')

    # Map of directory path to list of source and destination file paths.
    dirs: Dict[str, List[Tuple[str, str]]] = collections.defaultdict(list)

    for source_file, copy_path in zip(source_files, copy_paths):
        dirname = os.path.dirname(copy_path)
        dirs[dirname].append((source_file, copy_path))

    for directory, file_pairs in dirs.items():
        os.makedirs(directory, exist_ok=True)
        for src, dst in file_pairs:
            os.link(src, dst)


def main() -> int:
    """Script entry point."""

    args = parse_args()

    # Clear out any existing docs for the target.
    if os.path.exists(args.sphinx_build_dir):
        shutil.rmtree(args.sphinx_build_dir)

    # TODO(b/235349854): Printing the header causes unicode problems on Windows.
    # Disabled for now; re-enable once the root issue is fixed.
    # print(SCRIPT_HEADER)
    copy_doc_tree(args)

    # Flush all script output before running Sphinx.
    print('-' * 80, flush=True)

    return build_docs(
        args.sphinx_build_dir, args.out_dir, args.google_analytics_id
    )


if __name__ == '__main__':
    sys.exit(main())
