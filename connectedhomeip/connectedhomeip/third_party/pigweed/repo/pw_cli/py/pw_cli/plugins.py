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
"""Provides general purpose plugin functionality.

As used in this module, a plugin is a Python object associated with a name.
Plugins are registered in a Registry. The plugin object is typically a function,
but can be anything.

Plugins may be loaded in a variety of ways:

- Listed in a plugins file in the file system (e.g. as "name module target").
- Registered in a Python file using a decorator (@my_registry.plugin).
- Registered directly or by name with function calls on a registry object.

This functionality can be used to create plugins for command line tools,
interactive consoles, or anything else. Pigweed's pw command uses this module
for its plugins.
"""

import collections
import collections.abc
import importlib
import inspect
import logging
from pathlib import Path
import pkgutil
import sys
from textwrap import TextWrapper
import types
from typing import Any, Callable, Dict, List, Iterable, Iterator, Optional, Set

_LOG = logging.getLogger(__name__)
_BUILT_IN = '<built-in>'


class Error(Exception):
    """Indicates that a plugin is invalid or cannot be registered."""

    def __str__(self):
        """Displays the error as a string, including the __cause__ if present.

        Adding __cause__ gives useful context without displaying a backtrace.
        """
        if self.__cause__ is None:
            return super().__str__()

        return (
            f'{super().__str__()} '
            f'({type(self.__cause__).__name__}: {self.__cause__})'
        )


def _get_module(member: object) -> types.ModuleType:
    """Gets the module or a fake module if the module isn't found."""
    module = inspect.getmodule(member)
    return module if module else types.ModuleType('<unknown>')


class Plugin:
    """Represents a Python entity registered as a plugin.

    Each plugin resolves to a Python object, typically a function.
    """

    @classmethod
    def from_name(
        cls,
        name: str,
        module_name: str,
        member_name: str,
        source: Optional[Path],
    ) -> 'Plugin':
        """Creates a plugin by module and attribute name.

        Args:
          name: the name of the plugin
          module_name: Python module name (e.g. 'foo_pkg.bar')
          member_name: the name of the member in the module
          source: path to the plugins file that declared this plugin, if any
        """

        # Attempt to access the module and member. Catch any errors that might
        # occur, since a bad plugin shouldn't be a fatal error.
        try:
            module = importlib.import_module(module_name)
        except Exception as err:
            _LOG.debug(
                'Failed to import module "%s" for "%s" plugin',
                module_name,
                name,
                exc_info=True,
            )
            raise Error(f'Failed to import module "{module_name}"') from err

        try:
            member = getattr(module, member_name)
        except AttributeError as err:
            raise Error(
                f'"{module_name}.{member_name}" does not exist'
            ) from err

        return cls(name, member, source)

    def __init__(
        self, name: str, target: Any, source: Optional[Path] = None
    ) -> None:
        """Creates a plugin for the provided target."""
        self.name = name
        self._module = _get_module(target)
        self.target = target
        self.source = source

    @property
    def target_name(self) -> str:
        return (
            f'{self._module.__name__}.'
            f'{getattr(self.target, "__name__", self.target)}'
        )

    @property
    def source_name(self) -> str:
        return _BUILT_IN if self.source is None else str(self.source)

    def run_with_argv(self, argv: Iterable[str]) -> int:
        """Sets sys.argv and calls the plugin function.

        This is used to call a plugin as if from the command line.
        """
        original_sys_argv = sys.argv
        sys.argv = [f'pw {self.name}', *argv]

        try:
            return self.target()
        finally:
            sys.argv = original_sys_argv

    def help(self, full: bool = False) -> str:
        """Returns a description of this plugin from its docstring."""
        docstring = self.target.__doc__ or self._module.__doc__ or ''
        return docstring if full else next(iter(docstring.splitlines()), '')

    def details(self, full: bool = False) -> Iterator[str]:
        yield f'help    {self.help(full=full)}'
        yield f'module  {self._module.__name__}'
        yield f'target  {getattr(self.target, "__name__", self.target)}'
        yield f'source  {self.source_name}'

    def __repr__(self) -> str:
        return (
            f'{self.__class__.__name__}(name={self.name!r}, '
            f'target={self.target_name}'
            f'{f", source={self.source_name!r}" if self.source else ""})'
        )


def callable_with_no_args(plugin: Plugin) -> None:
    """Checks that a plugin is callable without arguments.

    May be used for the validator argument to Registry.
    """
    try:
        params = inspect.signature(plugin.target).parameters
    except TypeError:
        raise Error(
            'Plugin functions must be callable, but '
            f'{plugin.target_name} is a '
            f'{type(plugin.target).__name__}'
        )

    positional = sum(p.default == p.empty for p in params.values())
    if positional:
        raise Error(
            f'Plugin functions cannot have any required positional '
            f'arguments, but {plugin.target_name} has {positional}'
        )


class Registry(collections.abc.Mapping):
    """Manages a set of plugins from Python modules or plugins files."""

    def __init__(
        self, validator: Callable[[Plugin], Any] = lambda _: None
    ) -> None:
        """Creates a new, empty plugins registry.

        Args:
          validator: Function that checks whether a plugin is valid and should
              be registered. Must raise plugins.Error is the plugin is invalid.
        """

        self._registry: Dict[str, Plugin] = {}
        self._sources: Set[Path] = set()  # Paths to plugins files
        self._errors: Dict[str, List[Exception]] = collections.defaultdict(list)
        self._validate_plugin = validator

    def __getitem__(self, name: str) -> Plugin:
        """Accesses a plugin by name; raises KeyError if it does not exist."""
        if name in self._registry:
            return self._registry[name]

        if name in self._errors:
            raise KeyError(
                f'Registration for "{name}" failed: '
                + ', '.join(str(e) for e in self._errors[name])
            )

        raise KeyError(f'The plugin "{name}" has not been registered')

    def __iter__(self) -> Iterator[str]:
        return iter(self._registry)

    def __len__(self) -> int:
        return len(self._registry)

    def errors(self) -> Dict[str, List[Exception]]:
        return self._errors

    def run_with_argv(self, name: str, argv: Iterable[str]) -> int:
        """Runs a plugin by name, setting sys.argv to the provided args.

        This is used to run a command as if it were executed directly from the
        command line. The plugin is expected to return an int.

        Raises:
          KeyError if plugin is not registered.
        """
        return self[name].run_with_argv(argv)

    def _should_register(self, plugin: Plugin) -> bool:
        """Determines and logs if a plugin should be registered or not.

        Some errors are exceptions, others are not.
        """

        if plugin.name in self._registry and plugin.source is None:
            raise Error(
                f'Attempted to register built-in plugin "{plugin.name}", but '
                'a plugin with that name was previously registered '
                f'({self[plugin.name]})!'
            )

        # Run the user-provided validation function, which raises exceptions
        # if there are errors.
        self._validate_plugin(plugin)

        existing = self._registry.get(plugin.name)

        if existing is None:
            return True

        if existing.source is None:
            _LOG.debug(
                '%s: Overriding built-in plugin "%s" with %s',
                plugin.source_name,
                plugin.name,
                plugin.target_name,
            )
            return True

        if plugin.source != existing.source:
            _LOG.debug(
                '%s: The plugin "%s" was previously registered in %s; '
                'ignoring registration as %s',
                plugin.source_name,
                plugin.name,
                self._registry[plugin.name].source,
                plugin.target_name,
            )
        elif plugin.source not in self._sources:
            _LOG.warning(
                '%s: "%s" is registered file multiple times in this file! '
                'Only the first registration takes effect',
                plugin.source_name,
                plugin.name,
            )

        return False

    def register(self, name: str, target: Any) -> Optional[Plugin]:
        """Registers an object as a plugin."""
        return self._register(Plugin(name, target, None))

    def register_by_name(
        self,
        name: str,
        module_name: str,
        member_name: str,
        source: Optional[Path] = None,
    ) -> Optional[Plugin]:
        """Registers an object from its module and name as a plugin."""
        return self._register(
            Plugin.from_name(name, module_name, member_name, source)
        )

    def _register(self, plugin: Plugin) -> Optional[Plugin]:
        # Prohibit functions not from a plugins file from overriding others.
        if not self._should_register(plugin):
            return None

        self._registry[plugin.name] = plugin
        _LOG.debug(
            '%s: Registered plugin "%s" for %s',
            plugin.source_name,
            plugin.name,
            plugin.target_name,
        )

        return plugin

    def register_file(self, path: Path) -> None:
        """Registers plugins from a plugins file.

        Any exceptions raised from parsing the file are caught and logged.
        """
        with path.open() as contents:
            for lineno, line in enumerate(contents, 1):
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                try:
                    name, module, function = line.split()
                except ValueError as err:
                    self._errors[line.strip()].append(Error(err))
                    _LOG.error(
                        '%s:%d: Failed to parse plugin entry "%s": '
                        'Expected 3 items (name, module, function), '
                        'got %d',
                        path,
                        lineno,
                        line,
                        len(line.split()),
                    )
                    continue

                try:
                    self.register_by_name(name, module, function, path)
                except Error as err:
                    self._errors[name].append(err)
                    _LOG.error(
                        '%s: Failed to register plugin "%s": %s',
                        path,
                        name,
                        err,
                    )

        self._sources.add(path)

    def register_directory(
        self,
        directory: Path,
        file_name: str,
        restrict_to: Optional[Path] = None,
    ) -> None:
        """Finds and registers plugins from plugins files in a directory.

        Args:
          directory: The directory from which to start searching up.
          file_name: The name of plugins files to look for.
          restrict_to: If provided, do not search higher than this directory.
        """
        for path in find_all_in_parents(file_name, directory):
            if not path.is_file():
                continue

            if restrict_to is not None and restrict_to not in path.parents:
                _LOG.debug(
                    "Skipping plugins file %s because it's outside of %s",
                    path,
                    restrict_to,
                )
                continue

            _LOG.debug('Found plugins file %s', path)
            self.register_file(path)

    def short_help(self) -> str:
        """Returns a help string for the registered plugins."""
        width = (
            max(len(name) for name in self._registry) + 1
            if self._registry
            else 1
        )
        help_items = '\n'.join(
            f'  {name:{width}} {plugin.help()}'
            for name, plugin in sorted(self._registry.items())
        )
        return f'supported plugins:\n{help_items}'

    def detailed_help(self, plugins: Iterable[str] = ()) -> Iterator[str]:
        """Yields lines of detailed information about commands."""
        if not plugins:
            plugins = list(self._registry)

        yield '\ndetailed plugin information:'

        wrapper = TextWrapper(
            width=80, initial_indent='   ', subsequent_indent=' ' * 11
        )

        plugins = sorted(plugins)
        for plugin in plugins:
            yield f'  [{plugin}]'

            try:
                for line in self[plugin].details(full=len(plugins) == 1):
                    yield wrapper.fill(line)
            except KeyError as err:
                yield wrapper.fill(f'error   {str(err)[1:-1]}')

            yield ''

        yield 'Plugins files:'

        if self._sources:
            yield from (
                f'  [{i}] {file}' for i, file in enumerate(self._sources, 1)
            )
        else:
            yield '  (none found)'

    def plugin(
        self, function: Optional[Callable] = None, *, name: Optional[str] = None
    ) -> Callable[[Callable], Callable]:
        """Decorator that registers a function with this plugin registry."""

        def decorator(function: Callable) -> Callable:
            self.register(function.__name__ if name is None else name, function)
            return function

        if function is None:
            return decorator

        self.register(function.__name__, function)
        return function


def find_in_parents(name: str, path: Path) -> Optional[Path]:
    """Searches parent directories of the path for a file or directory."""
    path = path.resolve()

    while not path.joinpath(name).exists():
        path = path.parent

        if path.samefile(path.parent):
            return None

    return path.joinpath(name)


def find_all_in_parents(name: str, path: Path) -> Iterator[Path]:
    """Searches all parent directories of the path for files or directories."""

    while True:
        result = find_in_parents(name, path)
        if result is None:
            return

        yield result
        path = result.parent.parent


def import_submodules(
    module: types.ModuleType, recursive: bool = False
) -> None:
    """Imports the submodules of a package.

    This can be used to collect plugins registered with a decorator from a
    directory.
    """
    path = module.__path__  # type: ignore[attr-defined]
    if recursive:
        modules = pkgutil.walk_packages(path, module.__name__ + '.')
    else:
        modules = pkgutil.iter_modules(path, module.__name__ + '.')

    for info in modules:
        importlib.import_module(info.name)
