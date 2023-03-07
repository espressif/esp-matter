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
"""Watch build config dataclasses."""

from dataclasses import dataclass, field
from pathlib import Path
import shlex
from typing import List, Optional, TYPE_CHECKING

if TYPE_CHECKING:
    from pw_build.project_builder_prefs import ProjectBuilderPrefs


class UnknownBuildSystem(Exception):
    """Exception for requesting unsupported build systems."""


@dataclass
class BuildCommand:
    """Store details of a single build step."""

    build_dir: Path
    build_system_command: Optional[str] = None
    build_system_extra_args: List[str] = field(default_factory=list)
    targets: List[str] = field(default_factory=list)
    command_string: str = ''

    def __post_init__(self) -> None:
        self._expanded_args: List[str] = shlex.split(self.command_string)

    def _get_build_system_args(self) -> List[str]:
        assert self.build_system_command
        if self.build_system_command.endswith(
            'make'
        ) or self.build_system_command.endswith('ninja'):
            return ['-C', str(self.build_dir), *self.targets]

        if self.build_system_command.endswith('bazel'):
            return ['--output_base', str(self.build_dir), *self.targets]

        raise UnknownBuildSystem(
            f'\n\nUnknown build system command "{self.build_system_command}" '
            f'for build directory "{self.build_dir}".\n'
            'Supported commands: ninja, bazel, make'
        )

    def get_args(
        self,
        additional_build_args: Optional[List[str]] = None,
    ) -> List[str]:
        if self.build_system_command:
            extra_args = []
            extra_args.extend(self.build_system_extra_args)
            if additional_build_args:
                extra_args.extend(additional_build_args)
            command = [
                self.build_system_command,
                *extra_args,
                *self._get_build_system_args(),
            ]
            return command
        return self._expanded_args

    def __str__(self) -> str:
        return ' '.join(shlex.quote(arg) for arg in self.get_args())


@dataclass
class BuildRecipe:
    build_dir: Path
    steps: List[BuildCommand]
    build_system_command: Optional[str] = None
    title: Optional[str] = None

    @property
    def display_name(self) -> str:
        if self.title:
            return self.title
        return str(self.build_dir)

    def targets(self) -> List[str]:
        return list(
            set(target for step in self.steps for target in step.targets)
        )

    def __str__(self) -> str:
        message = f"{self.display_name}"
        targets = self.targets()
        if targets:
            message = '{}  ({})'.format(message, ' '.join(self.targets()))
        return message


def create_build_recipes(prefs: 'ProjectBuilderPrefs') -> List[BuildRecipe]:
    """Create a list of BuildRecipes from ProjectBuilderPrefs."""
    build_recipes: List[BuildRecipe] = []

    if prefs.run_commands:
        for command_str in prefs.run_commands:
            build_recipes.append(
                BuildRecipe(
                    build_dir=Path.cwd(),
                    steps=[
                        BuildCommand(Path.cwd(), command_string=command_str)
                    ],
                    title=command_str,
                )
            )

    for build_dir, targets in prefs.build_directories.items():
        steps: List[BuildCommand] = []
        build_path = Path(build_dir)
        (
            build_system_command,
            build_system_extra_args,
        ) = prefs.build_system_commands(build_dir)

        if not targets:
            targets = []
        steps.append(
            BuildCommand(
                build_dir=build_path,
                build_system_command=build_system_command,
                build_system_extra_args=build_system_extra_args,
                targets=targets,
            )
        )

        build_recipes.append(
            BuildRecipe(
                build_dir=build_path,
                steps=steps,
                build_system_command=build_system_command,
            )
        )

    return build_recipes
