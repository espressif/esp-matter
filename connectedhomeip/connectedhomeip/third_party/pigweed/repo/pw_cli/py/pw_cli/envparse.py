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
"""The envparse module defines an environment variable parser."""

import argparse
from dataclasses import dataclass
import os
from typing import (
    Callable,
    Dict,
    Generic,
    IO,
    List,
    Mapping,
    Optional,
    TypeVar,
    Union,
)


class EnvNamespace(
    argparse.Namespace
):  # pylint: disable=too-few-public-methods
    """Base class for parsed environment variable namespaces."""


T = TypeVar('T')
TypeConversion = Callable[[str], T]


@dataclass
class VariableDescriptor(Generic[T]):
    name: str
    type: TypeConversion[T]
    default: Optional[T]


class EnvironmentValueError(Exception):
    """Exception indicating a bad type conversion on an environment variable.

    Stores a reference to the lower-level exception from the type conversion
    function through the __cause__ attribute for more detailed information on
    the error.
    """

    def __init__(self, variable: str, value: str):
        self.variable: str = variable
        self.value: str = value
        super().__init__(
            f'Bad value for environment variable {variable}: {value}'
        )


class EnvironmentParser:
    """Parser for environment variables.

    Args:
        prefix: If provided, checks that all registered environment variables
          start with the specified string.
        error_on_unrecognized: If True and prefix is provided, will raise an
          exception if the environment contains a variable with the specified
          prefix that is not registered on the EnvironmentParser. If None,
          checks existence of PW_ENVIRONMENT_NO_ERROR_ON_UNRECOGNIZED (but not
          value).

    Example:

        parser = envparse.EnvironmentParser(prefix='PW_')
        parser.add_var('PW_LOG_LEVEL')
        parser.add_var('PW_LOG_FILE', type=envparse.FileType('w'))
        parser.add_var('PW_USE_COLOR', type=envparse.strict_bool, default=False)
        env = parser.parse_env()

        configure_logging(env.PW_LOG_LEVEL, env.PW_LOG_FILE)
    """

    def __init__(
        self,
        prefix: Optional[str] = None,
        error_on_unrecognized: Union[bool, None] = None,
    ) -> None:
        self._prefix: Optional[str] = prefix
        if error_on_unrecognized is None:
            varname = 'PW_ENVIRONMENT_NO_ERROR_ON_UNRECOGNIZED'
            error_on_unrecognized = varname not in os.environ
        self._error_on_unrecognized: bool = error_on_unrecognized

        self._variables: Dict[str, VariableDescriptor] = {}
        self._allowed_suffixes: List[str] = []

    def add_var(
        self,
        name: str,
        # pylint: disable=redefined-builtin
        type: TypeConversion[T] = str,  # type: ignore[assignment]
        # pylint: enable=redefined-builtin
        default: Optional[T] = None,
    ) -> None:
        """Registers an environment variable.

        Args:
            name: The environment variable's name.
            type: Type conversion for the variable's value.
            default: Default value for the variable.

        Raises:
            ValueError: If prefix was provided to the constructor and name does
              not start with the prefix.
        """
        if self._prefix is not None and not name.startswith(self._prefix):
            raise ValueError(
                f'Variable {name} does not have prefix {self._prefix}'
            )

        self._variables[name] = VariableDescriptor(name, type, default)

    def add_allowed_suffix(self, suffix: str) -> None:
        """Registers an environment variable name suffix to be allowed."""

        self._allowed_suffixes.append(suffix)

    def parse_env(
        self, env: Optional[Mapping[str, str]] = None
    ) -> EnvNamespace:
        """Parses known environment variables into a namespace.

        Args:
            env: Dictionary of environment variables. Defaults to os.environ.

        Raises:
            EnvironmentValueError: If the type conversion fails.
        """
        if env is None:
            env = os.environ

        namespace = EnvNamespace()

        for var, desc in self._variables.items():
            if var not in env:
                val = desc.default
            else:
                try:
                    val = desc.type(env[var])  # type: ignore
                except Exception as err:
                    raise EnvironmentValueError(var, env[var]) from err

            setattr(namespace, var, val)

        allowed_suffixes = tuple(self._allowed_suffixes)
        for var in env:
            if (
                not hasattr(namespace, var)
                and (self._prefix is None or var.startswith(self._prefix))
                and var.endswith(allowed_suffixes)
            ):
                setattr(namespace, var, env[var])

        if self._prefix is not None and self._error_on_unrecognized:
            for var in env:
                if (
                    var.startswith(self._prefix)
                    and var not in self._variables
                    and not var.endswith(allowed_suffixes)
                ):
                    raise ValueError(f'Unrecognized environment variable {var}')

        return namespace

    def __repr__(self) -> str:
        return f'{type(self).__name__}(prefix={self._prefix})'


# List of emoji which are considered to represent "True".
_BOOLEAN_TRUE_EMOJI = set(
    [
        '✔️',
        '👍',
        '👍🏻',
        '👍🏼',
        '👍🏽',
        '👍🏾',
        '👍🏿',
        '💯',
    ]
)


def strict_bool(value: str) -> bool:
    return (
        value == '1' or value.lower() == 'true' or value in _BOOLEAN_TRUE_EMOJI
    )


# TODO(mohrr) Switch to Literal when no longer supporting Python 3.7.
# OpenMode = Literal['r', 'rb', 'w', 'wb']
OpenMode = str


class FileType:
    def __init__(self, mode: OpenMode) -> None:
        self._mode: OpenMode = mode

    def __call__(self, value: str) -> IO:
        return open(value, self._mode)
