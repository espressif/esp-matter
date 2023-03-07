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
"""Dataclass for a Python package."""

import configparser
from contextlib import contextmanager
import copy
from dataclasses import dataclass, asdict
import io
import json
import os
from pathlib import Path
import pprint
import re
import shutil
from typing import Any, Dict, List, Optional, Iterable

_pretty_format = pprint.PrettyPrinter(indent=1, width=120).pformat

# List of known environment markers supported by pip.
# https://peps.python.org/pep-0508/#environment-markers
_PY_REQUIRE_ENVIRONMENT_MARKER_NAMES = [
    'os_name',
    'sys_platform',
    'platform_machine',
    'platform_python_implementation',
    'platform_release',
    'platform_system',
    'platform_version',
    'python_version',
    'python_full_version',
    'implementation_name',
    'implementation_version',
    'extra',
]


@contextmanager
def change_working_dir(directory: Path):
    original_dir = Path.cwd()
    try:
        os.chdir(directory)
        yield directory
    finally:
        os.chdir(original_dir)


class UnknownPythonPackageName(Exception):
    """Exception thrown when a Python package_name cannot be determined."""


class MissingSetupSources(Exception):
    """Exception thrown when a Python package is missing setup source files.

    For example: setup.cfg and pyproject.toml.i
    """


def _sanitize_install_requires(metadata_dict: dict) -> dict:
    """Convert install_requires lists into strings joined with line breaks."""
    try:
        install_requires = metadata_dict['options']['install_requires']
        if isinstance(install_requires, list):
            metadata_dict['options']['install_requires'] = '\n'.join(
                install_requires
            )
    except KeyError:
        pass
    return metadata_dict


@dataclass
class PythonPackage:
    """Class to hold a single Python package's metadata."""

    sources: List[Path]
    setup_sources: List[Path]
    tests: List[Path]
    inputs: List[Path]
    gn_target_name: str = ''
    generate_setup: Optional[Dict] = None
    config: Optional[configparser.ConfigParser] = None

    @staticmethod
    def from_dict(**kwargs) -> 'PythonPackage':
        """Build a PythonPackage instance from a dictionary."""
        transformed_kwargs = copy.copy(kwargs)

        # Transform string filenames to Paths
        for attribute in ['sources', 'tests', 'inputs', 'setup_sources']:
            transformed_kwargs[attribute] = [Path(s) for s in kwargs[attribute]]

        return PythonPackage(**transformed_kwargs)

    def __post_init__(self):
        # Read the setup.cfg file if possible
        if not self.config:
            self.config = self._load_config()

    @property
    def setup_dir(self) -> Optional[Path]:
        if not self.setup_sources:
            return None
        # Assuming all setup_source files live in the same parent directory.
        return self.setup_sources[0].parent

    @property
    def setup_py(self) -> Path:
        setup_py = [
            setup_file
            for setup_file in self.setup_sources
            if str(setup_file).endswith('setup.py')
        ]
        # setup.py will not exist for GN generated Python packages
        assert len(setup_py) == 1
        return setup_py[0]

    @property
    def setup_cfg(self) -> Optional[Path]:
        setup_cfg = [
            setup_file
            for setup_file in self.setup_sources
            if str(setup_file).endswith('setup.cfg')
        ]
        if len(setup_cfg) < 1:
            return None
        return setup_cfg[0]

    def as_dict(self) -> Dict[Any, Any]:
        """Return a dict representation of this class."""
        self_dict = asdict(self)
        if self.config:
            # Expand self.config into text.
            setup_cfg_text = io.StringIO()
            self.config.write(setup_cfg_text)
            self_dict['config'] = setup_cfg_text.getvalue()
        return self_dict

    @property
    def package_name(self) -> str:
        unknown_package_message = (
            'Cannot determine the package_name for the Python '
            f'library/package: {self.gn_target_name}\n\n'
            'This could be due to a missing python dependency in GN for:\n'
            f'{self.gn_target_name}\n\n'
        )

        if self.config:
            try:
                name = self.config['metadata']['name']
            except KeyError:
                raise UnknownPythonPackageName(
                    unknown_package_message + _pretty_format(self.as_dict())
                )
            return name
        top_level_source_dir = self.top_level_source_dir
        if top_level_source_dir:
            return top_level_source_dir.name

        actual_gn_target_name = self.gn_target_name.split(':')
        if len(actual_gn_target_name) < 2:
            raise UnknownPythonPackageName(unknown_package_message)

        return actual_gn_target_name[-1]

    @property
    def package_dir(self) -> Path:
        if self.setup_cfg and self.setup_cfg.is_file():
            return self.setup_cfg.parent / self.package_name
        root_source_dir = self.top_level_source_dir
        if root_source_dir:
            return root_source_dir
        return self.sources[0].parent

    @property
    def top_level_source_dir(self) -> Optional[Path]:
        source_dir_paths = sorted(
            set((len(sfile.parts), sfile.parent) for sfile in self.sources),
            key=lambda s: s[1],
        )
        if not source_dir_paths:
            return None

        top_level_source_dir = source_dir_paths[0][1]
        if not top_level_source_dir.is_dir():
            return None

        return top_level_source_dir

    def _load_config(self) -> Optional[configparser.ConfigParser]:
        config = configparser.ConfigParser()

        # Check for a setup.cfg and load that config.
        if self.setup_cfg:
            if self.setup_cfg.is_file():
                with self.setup_cfg.open() as config_file:
                    config.read_file(config_file)
                return config
            if self.setup_cfg.with_suffix('.json').is_file():
                return self._load_setup_json_config()

        # Fallback on the generate_setup scope from GN
        if self.generate_setup:
            config.read_dict(_sanitize_install_requires(self.generate_setup))
            return config
        return None

    def _load_setup_json_config(self) -> configparser.ConfigParser:
        assert self.setup_cfg
        setup_json = self.setup_cfg.with_suffix('.json')
        config = configparser.ConfigParser()
        with setup_json.open() as json_fp:
            json_dict = _sanitize_install_requires(json.load(json_fp))

        config.read_dict(json_dict)
        return config

    def copy_sources_to(self, destination: Path) -> None:
        """Copy this PythonPackage source files to another path."""
        new_destination = destination / self.package_dir.name
        new_destination.mkdir(parents=True, exist_ok=True)
        shutil.copytree(self.package_dir, new_destination, dirs_exist_ok=True)

    def install_requires_entries(self) -> List[str]:
        """Convert the install_requires entry into a list of strings."""
        this_requires: List[str] = []
        # If there's no setup.cfg, do nothing.
        if not self.config:
            return this_requires

        # Requires are delimited by newlines or semicolons.
        # Split existing list on either one.
        for req in re.split(
            r' *[\n;] *', self.config['options']['install_requires']
        ):
            # Skip empty lines.
            if not req:
                continue
            # Get the name part part of the dep, ignoring any spaces or
            # other characters.
            req_name_match = re.match(r'^(?P<name_part>[A-Za-z0-9_-]+)', req)
            if not req_name_match:
                continue
            req_name = req_name_match.groupdict().get('name_part', '')
            # Check if this is an environment marker.
            if req_name in _PY_REQUIRE_ENVIRONMENT_MARKER_NAMES:
                # Append this req as an environment marker for the previous
                # requirement.
                this_requires[-1] += f';{req}'
                continue
            # Normal pip requirement, save to this_requires.
            this_requires.append(req)
        return this_requires


def load_packages(
    input_list_files: Iterable[Path], ignore_missing=False
) -> List[PythonPackage]:
    """Load Python package metadata and configs."""

    packages = []
    for input_path in input_list_files:
        if ignore_missing and not input_path.is_file():
            continue
        with input_path.open() as input_file:
            # Each line contains the path to a json file.
            for json_file in input_file.readlines():
                # Load the json as a dict.
                json_file_path = Path(json_file.strip()).resolve()
                with json_file_path.open() as json_fp:
                    json_dict = json.load(json_fp)

                packages.append(PythonPackage.from_dict(**json_dict))
    return packages
