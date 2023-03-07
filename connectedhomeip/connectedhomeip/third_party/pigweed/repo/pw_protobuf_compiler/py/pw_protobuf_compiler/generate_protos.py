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
"""Script that invokes protoc to generate code for .proto files."""

import argparse
import logging
import os
from pathlib import Path
import subprocess
import sys
import tempfile
from typing import Callable, Dict, List, Optional, Tuple, Union

# Make sure dependencies are optional, since this script may be run when
# installing Python package dependencies through GN.
try:
    from pw_cli.log import install as setup_logging
except ImportError:
    from logging import basicConfig as setup_logging  # type: ignore

_LOG = logging.getLogger(__name__)


def _argument_parser() -> argparse.ArgumentParser:
    """Registers the script's arguments on an argument parser."""

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument(
        '--language',
        required=True,
        choices=DEFAULT_PROTOC_ARGS,
        help='Output language',
    )
    parser.add_argument(
        '--plugin-path', type=Path, help='Path to the protoc plugin'
    )
    parser.add_argument(
        '--include-file',
        type=argparse.FileType('r'),
        help='File containing additional protoc include paths',
    )
    parser.add_argument(
        '--out-dir',
        type=Path,
        required=True,
        help='Output directory for generated code',
    )
    parser.add_argument(
        '--compile-dir',
        type=Path,
        required=True,
        help='Root path for compilation',
    )
    parser.add_argument(
        '--sources', type=Path, nargs='+', help='Input protobuf files'
    )
    parser.add_argument(
        '--protoc', type=Path, default='protoc', help='Path to protoc'
    )
    parser.add_argument(
        '--no-experimental-proto3-optional',
        dest='experimental_proto3_optional',
        action='store_false',
        help='Do not invoke protoc with --experimental_allow_proto3_optional',
    )
    parser.add_argument(
        '--no-generate-type-hints',
        dest='generate_type_hints',
        action='store_false',
        help='Do not generate pyi files for python',
    )

    return parser


def protoc_common_args(args: argparse.Namespace) -> Tuple[str, ...]:
    flags: Tuple[str, ...] = ()
    if args.experimental_proto3_optional:
        flags += ('--experimental_allow_proto3_optional',)
    return flags


def protoc_pwpb_args(
    args: argparse.Namespace, include_paths: List[str]
) -> Tuple[str, ...]:
    return (
        '--plugin',
        f'protoc-gen-custom={args.plugin_path}',
        f'--custom_opt=-I{args.compile_dir}',
        *[f'--custom_opt=-I{include_path}' for include_path in include_paths],
        '--custom_out',
        args.out_dir,
    )


def protoc_pwpb_rpc_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    return (
        '--plugin',
        f'protoc-gen-custom={args.plugin_path}',
        '--custom_out',
        args.out_dir,
    )


def protoc_go_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    return (
        '--go_out',
        f'plugins=grpc:{args.out_dir}',
    )


def protoc_nanopb_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    # nanopb needs to know of the include path to parse *.options files
    return (
        '--plugin',
        f'protoc-gen-nanopb={args.plugin_path}',
        # nanopb_opt provides the flags to use for nanopb_out. Windows doesn't
        # like when you merge the two using the `flag,...:out` syntax. Use
        # Posix-style paths since backslashes on Windows are treated like
        # escape characters.
        f'--nanopb_opt=-I{args.compile_dir.as_posix()}',
        f'--nanopb_out={args.out_dir}',
    )


def protoc_nanopb_rpc_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    return (
        '--plugin',
        f'protoc-gen-custom={args.plugin_path}',
        '--custom_out',
        args.out_dir,
    )


def protoc_raw_rpc_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    return (
        '--plugin',
        f'protoc-gen-custom={args.plugin_path}',
        '--custom_out',
        args.out_dir,
    )


def protoc_python_args(
    args: argparse.Namespace, _include_paths: List[str]
) -> Tuple[str, ...]:
    flags: Tuple[str, ...] = (
        '--python_out',
        args.out_dir,
    )

    if args.generate_type_hints:
        flags += (
            '--mypy_out',
            args.out_dir,
        )

    return flags


_DefaultArgsFunction = Callable[
    [argparse.Namespace, List[str]], Tuple[str, ...]
]

# Default additional protoc arguments for each supported language.
# TODO(frolv): Make these overridable with a command-line argument.
DEFAULT_PROTOC_ARGS: Dict[str, _DefaultArgsFunction] = {
    'go': protoc_go_args,
    'nanopb': protoc_nanopb_args,
    'nanopb_rpc': protoc_nanopb_rpc_args,
    'pwpb': protoc_pwpb_args,
    'pwpb_rpc': protoc_pwpb_rpc_args,
    'python': protoc_python_args,
    'raw_rpc': protoc_raw_rpc_args,
}

# Languages that protoc internally supports.
BUILTIN_PROTOC_LANGS = ('go', 'python')


def main() -> int:
    """Runs protoc as configured by command-line arguments."""

    parser = _argument_parser()
    args = parser.parse_args()

    if args.plugin_path is None and args.language not in BUILTIN_PROTOC_LANGS:
        parser.error(
            f'--plugin-path is required for --language {args.language}'
        )

    args.out_dir.mkdir(parents=True, exist_ok=True)

    include_paths: List[str] = []
    if args.include_file:
        include_paths = [line.strip() for line in args.include_file]

    wrapper_script: Optional[Path] = None

    # On Windows, use a .bat version of the plugin if it exists or create a .bat
    # wrapper to use if none exists.
    if os.name == 'nt' and args.plugin_path:
        if args.plugin_path.with_suffix('.bat').exists():
            args.plugin_path = args.plugin_path.with_suffix('.bat')
            _LOG.debug('Using Batch plugin %s', args.plugin_path)
        else:
            with tempfile.NamedTemporaryFile(
                'w', suffix='.bat', delete=False
            ) as file:
                file.write(f'@echo off\npython {args.plugin_path.resolve()}\n')

            args.plugin_path = wrapper_script = Path(file.name)
            _LOG.debug('Using generated plugin wrapper %s', args.plugin_path)

    cmd: Tuple[Union[str, Path], ...] = (
        args.protoc,
        f'-I{args.compile_dir}',
        *[f'-I{include_path}' for include_path in include_paths],
        *protoc_common_args(args),
        *DEFAULT_PROTOC_ARGS[args.language](args, include_paths),
        *args.sources,
    )

    try:
        process = subprocess.run(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        )
    finally:
        if wrapper_script:
            wrapper_script.unlink()

    if process.returncode != 0:
        _LOG.error(
            'Protocol buffer compilation failed!\n%s',
            ' '.join(str(c) for c in cmd),
        )
        sys.stderr.buffer.write(process.stdout)
        sys.stderr.flush()

    return process.returncode


if __name__ == '__main__':
    setup_logging()
    sys.exit(main())
