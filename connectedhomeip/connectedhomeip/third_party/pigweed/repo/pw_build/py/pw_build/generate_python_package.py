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
"""Generates a setup.cfg file for a Python package."""

import argparse
from collections import defaultdict
import configparser
from dataclasses import dataclass
import json
from pathlib import Path
import sys
import textwrap
from typing import (
    Any,
    Dict,
    Iterable,
    Iterator,
    List,
    Optional,
    Sequence,
    Set,
    TextIO,
)

try:
    from pw_build.mirror_tree import mirror_paths
except ImportError:
    # Append this path to the module search path to allow running this module
    # before the pw_build package is installed.
    sys.path.append(str(Path(__file__).resolve().parent.parent))
    from pw_build.mirror_tree import mirror_paths


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('--label', help='Label for this Python package')
    parser.add_argument(
        '--proto-library',
        dest='proto_libraries',
        type=argparse.FileType('r'),
        default=[],
        action='append',
        help='Paths',
    )
    parser.add_argument(
        '--generated-root',
        required=True,
        type=Path,
        help='The base directory for the Python package',
    )
    parser.add_argument(
        '--setup-json',
        required=True,
        type=argparse.FileType('r'),
        help='setup.py keywords as JSON',
    )
    parser.add_argument(
        '--module-as-package',
        action='store_true',
        help='Generate an __init__.py that imports everything',
    )
    parser.add_argument(
        'files',
        type=Path,
        nargs='+',
        help='Relative paths to the files in the package',
    )
    return parser.parse_args()


def _check_nested_protos(label: str, proto_info: Dict[str, Any]) -> None:
    """Checks that the proto library refers to this package."""
    python_package = proto_info['nested_in_python_package']

    if python_package != label:
        raise ValueError(
            f"{label}'s 'proto_library' is set to {proto_info['label']}, but "
            f"that target's 'python_package' is {python_package or 'not set'}. "
            f"Set {proto_info['label']}'s 'python_package' to {label}."
        )


@dataclass(frozen=True)
class _ProtoInfo:
    root: Path
    sources: Sequence[Path]
    deps: Sequence[str]


def _collect_all_files(
    root: Path, files: List[Path], paths_to_collect: Iterable[_ProtoInfo]
) -> Dict[str, Set[str]]:
    """Collects files in output dir, adds to files; returns package_data."""
    root.mkdir(exist_ok=True)

    for proto_info in paths_to_collect:
        # Mirror the proto files to this package.
        files += mirror_paths(proto_info.root, proto_info.sources, root)

    # Find all subpackages, including empty ones.
    subpackages: Set[Path] = set()
    for file in (f.relative_to(root) for f in files):
        subpackages.update(root / path for path in file.parents)
    subpackages.remove(root)

    # Make sure there are __init__.py and py.typed files for each subpackage.
    for pkg in subpackages:
        pytyped = pkg / 'py.typed'
        if not pytyped.exists():
            pytyped.touch()
        files.append(pytyped)

        # Create an __init__.py file if it doesn't already exist.
        initpy = pkg / '__init__.py'
        if not initpy.exists():
            # Use pkgutil.extend_path to treat this as a namespaced package.
            # This allows imports with the same name to live in multiple
            # separate PYTHONPATH locations.
            initpy.write_text(
                'from pkgutil import extend_path  # type: ignore\n'
                '__path__ = extend_path(__path__, __name__)  # type: ignore\n'
            )
        files.append(initpy)

    pkg_data: Dict[str, Set[str]] = defaultdict(set)

    # Add all non-source files to package data.
    for file in (f for f in files if f.suffix != '.py'):
        pkg = file.parent

        package_name = pkg.relative_to(root).as_posix().replace('/', '.')
        pkg_data[package_name].add(file.name)

    return pkg_data


PYPROJECT_FILE = '''\
# Generated file. Do not modify.
[build-system]
requires = ['setuptools', 'wheel']
build-backend = 'setuptools.build_meta'
'''


def _get_setup_keywords(pkg_data: dict, keywords: dict) -> Dict:
    """Gather all setuptools.setup() keyword args."""
    options_keywords = dict(
        packages=list(pkg_data),
        package_data={pkg: list(files) for pkg, files in pkg_data.items()},
    )

    keywords['options'].update(options_keywords)
    return keywords


def _write_to_config(
    config: configparser.ConfigParser, data: Dict, section: Optional[str] = None
):
    """Populate a ConfigParser instance with the contents of a dict."""
    # Add a specified section if missing.
    if section is not None and not config.has_section(section):
        config.add_section(section)

    for key, value in data.items():
        # Value is a dict so create a new subsection
        if isinstance(value, dict):
            _write_to_config(
                config,
                value,
                f'{section}.{key}' if section else key,
            )
        elif isinstance(value, list):
            if value:
                assert section is not None
                # Convert the list to an allowed str format.
                config[section][key] = '\n' + '\n'.join(value)
        else:
            assert section is not None
            # Add the value as a string. See expected types here:
            # https://setuptools.readthedocs.io/en/latest/userguide/declarative_config.html#specifying-values
            config[section][key] = str(value)


def _generate_setup_cfg(
    pkg_data: dict,
    keywords: dict,
    config_file_path: Path,
) -> None:
    """Creates a setup.cfg file based on setuptools keywords."""
    setup_keywords = _get_setup_keywords(pkg_data, keywords)

    config = configparser.ConfigParser()

    _write_to_config(config, setup_keywords)

    # Write the config to a file.
    with config_file_path.open('w') as config_file:
        config.write(config_file)


def _import_module_in_package_init(all_files: List[Path]) -> None:
    """Generates an __init__.py that imports the module.

    This makes an individual module usable as a package. This is used for proto
    modules.
    """
    sources = [
        f for f in all_files if f.suffix == '.py' and f.name != '__init__.py'
    ]
    assert (
        len(sources) == 1
    ), 'Module as package expects a single .py source file'

    (source,) = sources
    source.parent.joinpath('__init__.py').write_text(
        f'from {source.stem}.{source.stem} import *\n'
    )


def _load_metadata(
    label: str, proto_libraries: Iterable[TextIO]
) -> Iterator[_ProtoInfo]:
    for proto_library_file in proto_libraries:
        info = json.load(proto_library_file)
        _check_nested_protos(label, info)

        deps = []
        for dep in info['dependencies']:
            with open(dep) as file:
                deps.append(json.load(file)['package'])

        yield _ProtoInfo(
            Path(info['root']),
            tuple(Path(p) for p in info['protoc_outputs']),
            deps,
        )


def main(
    generated_root: Path,
    files: List[Path],
    module_as_package: bool,
    setup_json: TextIO,
    label: str,
    proto_libraries: Iterable[TextIO],
) -> int:
    """Generates a setup.py and other files for a Python package."""
    proto_infos = list(_load_metadata(label, proto_libraries))
    try:
        pkg_data = _collect_all_files(generated_root, files, proto_infos)
    except ValueError as error:
        msg = '\n'.join(textwrap.wrap(str(error), 78))
        print(
            f'ERROR: Failed to generate Python package {label}:\n\n'
            f'{textwrap.indent(msg, "  ")}\n',
            file=sys.stderr,
        )
        return 1

    with setup_json:
        setup_keywords = json.load(setup_json)
        setup_keywords.setdefault('options', {})

    setup_keywords['options'].setdefault('install_requires', [])

    if module_as_package:
        _import_module_in_package_init(files)

    # Create the pyproject.toml and setup.cfg files for this package.
    generated_root.joinpath('pyproject.toml').write_text(PYPROJECT_FILE)
    _generate_setup_cfg(
        pkg_data,
        setup_keywords,
        config_file_path=generated_root.joinpath('setup.cfg'),
    )

    return 0


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
