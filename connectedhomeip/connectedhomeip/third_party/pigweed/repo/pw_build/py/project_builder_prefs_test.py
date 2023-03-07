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
"""Tests for pw_build.project_builder_prefs"""

import argparse
import copy
from pathlib import Path
import tempfile
from typing import Any, Dict
import unittest
from unittest.mock import MagicMock

from pw_build.project_builder_argparse import add_project_builder_arguments
from pw_build.project_builder_prefs import (
    ProjectBuilderPrefs,
    _DEFAULT_CONFIG,
    load_defaults_from_argparse,
)


def _create_tempfile(content: str) -> Path:
    with tempfile.NamedTemporaryFile(
        prefix=f'{__package__}', delete=False
    ) as output_file:
        output_file.write(content.encode('UTF-8'))
        return Path(output_file.name)


class TestProjectBuilderPrefs(unittest.TestCase):
    """Tests for ProjectBuilderPrefs."""

    def setUp(self):
        self.maxDiff = None  # pylint: disable=invalid-name

    def test_load_no_existing_files(self) -> None:
        # Create a prefs instance with no loaded config.
        prefs = ProjectBuilderPrefs(
            load_argparse_arguments=add_project_builder_arguments
        )
        # Construct an expected result config.
        expected_config: Dict[Any, Any] = {}
        expected_config.update(_DEFAULT_CONFIG)
        expected_config.update(
            load_defaults_from_argparse(add_project_builder_arguments)
        )

        self.assertEqual(
            prefs._config, expected_config  # pylint: disable=protected-access
        )

    def test_apply_command_line_args(self) -> None:
        """Check command line args are applied to watch preferences."""
        # Load default command line arg values.
        defaults_from_argparse = load_defaults_from_argparse(
            add_project_builder_arguments
        )

        # Create a prefs instance with the test config file.
        prefs = ProjectBuilderPrefs(
            load_argparse_arguments=add_project_builder_arguments
        )

        # Construct an expected result config.
        expected_config: Dict[Any, Any] = copy.copy(_DEFAULT_CONFIG)
        expected_config.update(defaults_from_argparse)

        # pylint: disable=protected-access
        prefs._update_config = MagicMock(  # type: ignore
            wraps=prefs._update_config
        )
        # pylint: enable=protected-access

        args_dict = copy.deepcopy(defaults_from_argparse)
        changed_args = {
            'jobs': 8,
            'colors': False,
            'build_system_commands': [['default', 'bazel']],
        }
        args_dict.update(changed_args)

        prefs.apply_command_line_args(argparse.Namespace(**args_dict))

        # apply_command_line_args modifies build_system_commands to match the
        # prefs dict format.
        changed_args['build_system_commands'] = {
            'default': {'command': 'bazel'}
        }

        # Check that only args changed from their defaults are applied.
        # pylint: disable=protected-access
        prefs._update_config.assert_called_once_with(changed_args)
        # pylint: enable=protected-access

        # Check the result includes the project_config settings and the
        # changed_args.
        expected_config.update(changed_args)
        # pylint: disable=protected-access
        self.assertEqual(prefs._config, expected_config)
        # pylint: enable=protected-access


if __name__ == '__main__':
    unittest.main()
