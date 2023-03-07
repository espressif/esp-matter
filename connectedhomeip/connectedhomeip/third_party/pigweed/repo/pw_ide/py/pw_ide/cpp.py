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
"""Configure C/C++ IDE support for Pigweed projects.

We support C/C++ code analysis via ``clangd``, or other language servers that
are compatible with the ``clangd`` compilation database format.

While clangd can work well out of the box for typical C++ codebases, some work
is required to coax it to work for embedded projects. In particular, Pigweed
projects use multiple toolchains within a distinct environment, and almost
always define multiple targets. This means compilation units are likely have
multiple compile commands and the toolchain executables are unlikely to be in
your path. ``clangd`` is not equipped to deal with this out of the box. We
handle this by:

- Processing the compilation database produced by the build system into
  multiple internally-consistent compilation databases, one for each target
  (where a "target" is a particular build for a particular system using a
  particular toolchain).

- Creating unambiguous paths to toolchain drivers to ensure the right toolchain
  is used and that clangd knows where to find that toolchain's system headers.

- Providing tools for working with several compilation databases that are
  spiritually similar to tools like ``pyenv``, ``rbenv``, etc.

In short, we take the probably-broken compilation database that the build system
generates, process it into several not-broken compilation databases in the
``pw_ide`` working directory, and provide a stable symlink that points to the
selected active target's compliation database. If ``clangd`` is configured to
point at the symlink and is set up with the right paths, you'll get code
intelligence.
"""

from collections import defaultdict
from dataclasses import dataclass
import glob
from io import TextIOBase
import json
import os
from pathlib import Path
import platform
from typing import (
    Any,
    cast,
    Callable,
    Dict,
    Generator,
    List,
    Optional,
    Tuple,
    TypedDict,
    Union,
)

from pw_ide.exceptions import (
    BadCompDbException,
    InvalidTargetException,
    MissingCompDbException,
    UnresolvablePathException,
)

from pw_ide.settings import PigweedIdeSettings, PW_PIGWEED_CIPD_INSTALL_DIR
from pw_ide.symlinks import set_symlink

_COMPDB_FILE_PREFIX = 'compile_commands'
_COMPDB_FILE_SEPARATOR = '_'
_COMPDB_FILE_EXTENSION = '.json'

_COMPDB_CACHE_DIR_PREFIX = '.cache'
_COMPDB_CACHE_DIR_SEPARATOR = '_'

COMPDB_FILE_GLOB = f'{_COMPDB_FILE_PREFIX}*{_COMPDB_FILE_EXTENSION}'
COMPDB_CACHE_DIR_GLOB = f'{_COMPDB_CACHE_DIR_PREFIX}*'

MAX_COMMANDS_TARGET_FILENAME = 'max_commands_target'

_SUPPORTED_TOOLCHAIN_EXECUTABLES = ('clang', 'gcc', 'g++')


def compdb_generate_file_path(target: str = '') -> Path:
    """Generate a compilation database file path."""

    path = Path(f'{_COMPDB_FILE_PREFIX}{_COMPDB_FILE_EXTENSION}')

    if target:
        path = path.with_name(
            f'{_COMPDB_FILE_PREFIX}'
            f'{_COMPDB_FILE_SEPARATOR}{target}'
            f'{_COMPDB_FILE_EXTENSION}'
        )

    return path


def compdb_generate_cache_path(target: str = '') -> Path:
    """Generate a compilation database cache directory path."""

    path = Path(f'{_COMPDB_CACHE_DIR_PREFIX}')

    if target:
        path = path.with_name(
            f'{_COMPDB_CACHE_DIR_PREFIX}'
            f'{_COMPDB_CACHE_DIR_SEPARATOR}{target}'
        )

    return path


def compdb_target_from_path(filename: Path) -> Optional[str]:
    """Get a target name from a compilation database path."""

    # The length of the common compilation database file name prefix
    prefix_length = len(_COMPDB_FILE_PREFIX) + len(_COMPDB_FILE_SEPARATOR)

    if len(filename.stem) <= prefix_length:
        # This will return None for the symlink filename, and any filename that
        # is too short to be a compilation database.
        return None

    if filename.stem[:prefix_length] != (
        _COMPDB_FILE_PREFIX + _COMPDB_FILE_SEPARATOR
    ):
        # This will return None for any files that don't have the common prefix.
        return None

    return filename.stem[prefix_length:]


def _none_to_empty_str(value: Optional[str]) -> str:
    return value if value is not None else ''


def _none_if_not_exists(path: Path) -> Optional[Path]:
    return path if path.exists() else None


def compdb_cache_path_if_exists(
    working_dir: Path, target: Optional[str]
) -> Optional[Path]:
    return _none_if_not_exists(
        working_dir / compdb_generate_cache_path(_none_to_empty_str(target))
    )


def target_is_enabled(
    target: Optional[str], settings: PigweedIdeSettings
) -> bool:
    """Determine if a target is enabled.

    By default, all targets are enabled. If specific targets are defined in a
    settings file, only those targets will be enabled.
    """

    if target is None:
        return False

    if len(settings.targets) == 0:
        return True

    return target in settings.targets


def path_to_executable(
    exe: str,
    *,
    default_path: Optional[Path] = None,
    path_globs: Optional[List[str]] = None,
    strict: bool = False,
) -> Optional[Path]:
    """Return the path to a compiler executable.

    In a ``clang`` compile command, the executable may or may not include a
    path. For example:

    .. code-block:: none

       /usr/bin/clang      <- includes a path
       ../path/to/my/clang <- includes a path
       clang               <- doesn't include a path

    If it includes a path, then ``clangd`` will have no problem finding the
    driver, so we can simply return the path. If the executable *doesn't*
    include a path, then ``clangd`` will search ``$PATH``, and may not find the
    intended driver unless you actually want the default system toolchain or
    Pigweed paths have been added to ``$PATH``. So this function provides two
    options for resolving those ambiguous paths:

    - Provide a default path, and all executables without a path will be
      re-written with a path within the default path.
    - Provide the a set of globs that will be used to search for the executable,
      which will normally be the query driver globs used with clangd.

    By default, if neither of these options is chosen, or if the executable
    cannot be found within the provided globs, the pathless executable that was
    provided will be returned, and clangd will resort to searching $PATH. If you
    instead pass ``strict=True``, this will raise an exception if an unambiguous
    path cannot be constructed.

    This function only tries to ensure that all executables have a path to
    eliminate ambiguity. A couple of important things to keep in mind:

    - This doesn't guarantee that the path exists or an executable actually
      exists at the path. It only ensures that some path is provided to an
      executable.
    - An executable being present at the indicated path doesn't guarantee that
      it will work flawlessly for clangd code analysis. The clangd
      ``--query-driver`` argument needs to include a path to this executable in
      order for its bundled headers to be resolved correctly.

    This function also filters out invalid or unsupported drivers. For example,
    build systems will sometimes naively include build steps for Python or other
    languages in the compilation database, which are not usable with clangd.
    As a result, this function has four possible end states:

    - It returns a path with an executable that can be used as a ``clangd``
      driver.
    - It returns ``None``, meaning the compile command was invalid.
    - It returns the same string that was provided (as a ``Path``), if a path
      couldn't be resolved and ``strict=False``.
    - It raises an ``UnresolvablePathException`` if the executable cannot be
      placed in an unambiguous path and ``strict=True``.
    """
    maybe_path = Path(exe)

    # We were give an empty string, not a path. Not a valid command.
    if len(maybe_path.parts) == 0:
        return None

    # Determine if the executable name matches supported drivers.
    is_supported_driver = False

    for supported_executable in _SUPPORTED_TOOLCHAIN_EXECUTABLES:
        if supported_executable in maybe_path.name:
            is_supported_driver = True

    if not is_supported_driver:
        return None

    # Now, ensure the executable has a path.

    # This is either a relative or absolute path -- return it.
    if len(maybe_path.parts) > 1:
        return maybe_path

    # If we got here, there's only one "part", so we assume it's an executable
    # without a path. This logic doesn't work with a path like `./exe` since
    # that also yields only one part. So currently this breaks if you actually
    # have your compiler executable in your root build directory, which is
    # (hopefully) very rare.

    # If we got a default path, use it.
    if default_path is not None:
        return default_path / maybe_path

    # Otherwise, try to find the executable within the query driver globs.
    # Note that unlike the previous paths, this path will only succeed if an
    # executable actually exists somewhere in the query driver globs.
    if path_globs is not None:
        for path_glob in path_globs:
            for path_str in glob.iglob(path_glob):
                path = Path(path_str)
                if path.name == maybe_path.name:
                    return path.absolute()

    if strict:
        raise UnresolvablePathException(
            f'Cannot place {exe} in an unambiguous path!'
        )

    return maybe_path


def command_parts(command: str) -> Tuple[str, List[str]]:
    """Return the executable string and the rest of the command tokens."""
    parts = command.split()
    head = parts[0] if len(parts) > 0 else ''
    tail = parts[1:] if len(parts) > 1 else []
    return head, tail


# This is a clumsy way to express optional keys, which is not directly
# supported in TypedDicts right now.
class BaseCppCompileCommandDict(TypedDict):
    file: str
    directory: str
    output: Optional[str]


class CppCompileCommandDictWithCommand(BaseCppCompileCommandDict):
    command: str


class CppCompileCommandDictWithArguments(BaseCppCompileCommandDict):
    arguments: List[str]


CppCompileCommandDict = Union[
    CppCompileCommandDictWithCommand, CppCompileCommandDictWithArguments
]


class CppCompileCommand:
    """A representation of a clang compilation database compile command.

    See: https://clang.llvm.org/docs/JSONCompilationDatabase.html
    """

    def __init__(
        self,
        file: str,
        directory: str,
        command: Optional[str] = None,
        arguments: Optional[List[str]] = None,
        output: Optional[str] = None,
    ) -> None:
        # Per the spec, either one of these two must be present. clangd seems
        # to prefer "arguments" when both are present.
        if command is None and arguments is None:
            raise TypeError(
                'A compile command requires either \'command\' '
                'or \'arguments\'.'
            )

        if command is None:
            raise TypeError(
                'Compile commands without \'command\' ' 'are not supported yet.'
            )

        self._command = command
        self._arguments = arguments
        self._file = file
        self._directory = directory

        executable, tokens = command_parts(command)
        self._executable_path = Path(executable)
        self._inferred_output: Optional[str] = None

        try:
            # Find the output argument and grab its value.
            output_flag_idx = tokens.index('-o')
            self._inferred_output = tokens[output_flag_idx + 1]
        except ValueError:
            # No -o found, probably not a C/C++ compile command.
            self._inferred_output = None
        except IndexError:
            # It has an -o but no argument after it.
            raise TypeError(
                'Failed to load compile command with no output argument!'
            )

        self._provided_output = output
        self.target: Optional[str] = None

    @property
    def file(self) -> str:
        return self._file

    @property
    def directory(self) -> str:
        return self._directory

    @property
    def command(self) -> Optional[str]:
        return self._command

    @property
    def arguments(self) -> Optional[List[str]]:
        return self._arguments

    @property
    def output(self) -> Optional[str]:
        # We're ignoring provided output values for now.
        return self._inferred_output

    @property
    def output_path(self) -> Optional[Path]:
        if self.output is None:
            return None

        return Path(self.directory) / Path(self.output)

    @property
    def executable_path(self) -> Path:
        return self._executable_path

    @property
    def executable_name(self) -> str:
        return self.executable_path.name

    @classmethod
    def from_dict(
        cls, compile_command_dict: Dict[str, Any]
    ) -> 'CppCompileCommand':
        return cls(
            # We want to let possible Nones through to raise at runtime.
            file=cast(str, compile_command_dict.get('file')),
            directory=cast(str, compile_command_dict.get('directory')),
            command=compile_command_dict.get('command'),
            arguments=compile_command_dict.get('arguments'),
            output=compile_command_dict.get('output'),
        )

    @classmethod
    def try_from_dict(
        cls, compile_command_dict: Dict[str, Any]
    ) -> Optional['CppCompileCommand']:
        try:
            return cls.from_dict(compile_command_dict)
        except TypeError:
            return None

    def process(
        self,
        *,
        default_path: Optional[Path] = None,
        path_globs: Optional[List[str]] = None,
        strict: bool = False,
    ) -> Optional['CppCompileCommand']:
        """Process a compile command.

        At minimum, a compile command from a clang compilation database needs to
        be correlated with its target, and this method returns the target name
        with the compile command. But it also cleans up other things we need for
        reliable code intelligence:

        - Some targets may not be valid C/C++ compile commands. For example,
          some build systems will naively include build steps for Python or for
          linting commands. We want to filter those out.

        - Some compile commands don't provide a path to the compiler executable
          (referred to by clang as the "driver"). In that case, clangd is very
          unlikely to find the executable unless it happens to be in ``$PATH``.
          The ``--query-driver`` argument to ``clangd`` allowlists
          executables/drivers for use its use, but clangd doesn't use it to
          resolve ambiguous paths. We bridge that gap here. Any executable
          without a path will be either placed in the provided default path or
          searched for in the query driver globs and be replaced with a path to
          the executable.
        """
        if self.command is None:
            raise NotImplementedError(
                'Compile commands without \'command\' ' 'are not supported yet.'
            )

        executable_str, tokens = command_parts(self.command)
        executable_path = path_to_executable(
            executable_str,
            default_path=default_path,
            path_globs=path_globs,
            strict=strict,
        )

        if executable_path is None or self.output is None:
            return None

        # TODO(chadnorvell): Some commands include the executable multiple
        # times. It's not clear if that affects clangd.
        new_command = f'{str(executable_path)} {" ".join(tokens)}'

        return self.__class__(
            file=self.file,
            directory=self.directory,
            command=new_command,
            arguments=None,
            output=self.output,
        )

    def as_dict(self) -> CppCompileCommandDict:
        base_compile_command_dict: BaseCppCompileCommandDict = {
            'file': self.file,
            'directory': self.directory,
            'output': self.output,
        }

        # TODO(chadnorvell): Support "arguments". The spec requires that a
        # We don't support "arguments" at all right now. When we do, we should
        # preferentially include "arguments" only, and only include "command"
        # when "arguments" is not present.
        if self.command is not None:
            compile_command_dict: CppCompileCommandDictWithCommand = {
                'command': self.command,
                # Unfortunately dict spreading doesn't work with mypy.
                'file': base_compile_command_dict['file'],
                'directory': base_compile_command_dict['directory'],
                'output': base_compile_command_dict['output'],
            }
        else:
            raise NotImplementedError(
                'Compile commands without \'command\' ' 'are not supported yet.'
            )

        return compile_command_dict


def _infer_target_pos(target_glob: str) -> List[int]:
    """Infer the position of the target in a compilation unit artifact path."""
    tokens = Path(target_glob).parts
    positions = []

    for pos, token in enumerate(tokens):
        if token == '?':
            positions.append(pos)
        elif token == '*':
            pass
        else:
            raise ValueError(f'Invalid target inference token: {token}')

    return positions


def infer_target(
    target_glob: str, root: Path, output_path: Path
) -> Optional[str]:
    """Infer a target from a compilation unit artifact path.

    See the documentation for ``PigweedIdeSettings.target_inference``."""
    target_pos = _infer_target_pos(target_glob)

    if len(target_pos) == 0:
        return None

    # Depending on the build system and project configuration, the target name
    # may be in the "directory" or the "output" of the compile command. So we
    # need to construct the full path that combines both and use that to search
    # for the target.
    subpath = output_path.relative_to(root)
    return '_'.join([subpath.parts[pos] for pos in target_pos])


LoadableToCppCompilationDatabase = Union[
    List[Dict[str, Any]], str, TextIOBase, Path
]


class CppCompilationDatabase:
    """A representation of a clang compilation database.

    See: https://clang.llvm.org/docs/JSONCompilationDatabase.html
    """

    def __init__(self, build_dir: Optional[Path] = None) -> None:
        self._db: List[CppCompileCommand] = []

        # Only compilation databases that are loaded will have this, and it
        # contains the root directory of the build that the compilation
        # database is based on. Processed compilation databases will not have
        # a value here.
        self._build_dir = build_dir

    def __len__(self) -> int:
        return len(self._db)

    def __getitem__(self, index: int) -> CppCompileCommand:
        return self._db[index]

    def __iter__(self) -> Generator[CppCompileCommand, None, None]:
        return (compile_command for compile_command in self._db)

    def add(self, *commands: CppCompileCommand):
        """Add compile commands to the compilation database."""
        self._db.extend(commands)

    def merge(self, other: 'CppCompilationDatabase') -> None:
        """Merge values from another database into this one.

        This will not overwrite a compile command that already exists for a
        particular file.
        """
        self_dict = {c.file: c for c in self._db}

        for compile_command in other:
            if compile_command.file not in self_dict:
                self_dict[compile_command.file] = compile_command

        self._db = list(self_dict.values())

    def as_dicts(self) -> List[CppCompileCommandDict]:
        return [compile_command.as_dict() for compile_command in self._db]

    def to_json(self) -> str:
        """Output the compilation database to a JSON string."""

        return json.dumps(self.as_dicts(), indent=2, sort_keys=True)

    def to_file(self, path: Path):
        """Write the compilation database to a JSON file."""

        with open(path, 'w') as file:
            json.dump(self.as_dicts(), file, indent=2, sort_keys=True)

    @classmethod
    def load(
        cls, compdb_to_load: LoadableToCppCompilationDatabase, build_dir: Path
    ) -> 'CppCompilationDatabase':
        """Load a compilation database.

        You can provide a JSON file handle or path, a JSON string, or a native
        Python data structure that matches the format (list of dicts).
        """
        db_as_dicts: List[Dict[str, Any]]

        if isinstance(compdb_to_load, list):
            # The provided data is already in the format we want it to be in,
            # probably, and if it isn't we'll find out when we try to
            # instantiate the database.
            db_as_dicts = compdb_to_load
        else:
            if isinstance(compdb_to_load, Path):
                # The provided data is a path to a file, presumably JSON.
                try:
                    compdb_data = compdb_to_load.read_text()
                except FileNotFoundError:
                    raise MissingCompDbException()
            elif isinstance(compdb_to_load, TextIOBase):
                # The provided data is a file handle, presumably JSON.
                compdb_data = compdb_to_load.read()
            elif isinstance(compdb_to_load, str):
                # The provided data is a a string, presumably JSON.
                compdb_data = compdb_to_load

            db_as_dicts = json.loads(compdb_data)

        compdb = cls(build_dir=build_dir)

        try:
            compdb.add(
                *[
                    compile_command
                    for compile_command_dict in db_as_dicts
                    if (
                        compile_command := CppCompileCommand.try_from_dict(
                            compile_command_dict
                        )
                    )
                    is not None
                ]
            )
        except TypeError:
            # This will arise if db_as_dicts is not actually a list of dicts
            raise BadCompDbException()

        return compdb

    def process(
        self,
        settings: PigweedIdeSettings,
        *,
        default_path: Optional[Path] = None,
        path_globs: Optional[List[str]] = None,
        strict: bool = False,
    ) -> 'CppCompilationDatabasesMap':
        """Process a ``clangd`` compilation database file.

        Given a clang compilation database that may have commands for multiple
        valid or invalid targets/toolchains, keep only the valid compile
        commands and store them in target-specific compilation databases.
        """
        if self._build_dir is None:
            raise ValueError(
                'Can only process a compilation database that '
                'contains a root build directory, usually '
                'specified when loading the file. Are you '
                'trying to process an already-processed '
                'compilation database?'
            )

        clean_compdbs = CppCompilationDatabasesMap(settings)

        for compile_command in self:
            processed_command = compile_command.process(
                default_path=default_path, path_globs=path_globs, strict=strict
            )

            if (
                processed_command is not None
                and processed_command.output_path is not None
            ):
                target = infer_target(
                    settings.target_inference,
                    self._build_dir,
                    processed_command.output_path,
                )

                if target_is_enabled(target, settings):
                    # This invariant is satisfied by target_is_enabled
                    target = cast(str, target)
                    processed_command.target = target
                    clean_compdbs[target].add(processed_command)

        return clean_compdbs


class CppCompilationDatabasesMap:
    """Container for a map of target name to compilation database."""

    def __init__(self, settings: PigweedIdeSettings):
        self.settings = settings
        self._dbs: Dict[str, CppCompilationDatabase] = defaultdict(
            CppCompilationDatabase
        )

    def __len__(self) -> int:
        return len(self._dbs)

    def __getitem__(self, key: str) -> CppCompilationDatabase:
        return self._dbs[key]

    def __setitem__(self, key: str, item: CppCompilationDatabase) -> None:
        self._dbs[key] = item

    @property
    def targets(self) -> List[str]:
        return list(self._dbs.keys())

    def items(
        self,
    ) -> Generator[Tuple[str, CppCompilationDatabase], None, None]:
        return ((key, value) for (key, value) in self._dbs.items())

    def write(self) -> None:
        """Write compilation databases to target-specific JSON files."""
        # This also writes out a file with the name of the target that has the
        # largest number of commands, i.e., the target with the broadest
        # compilation unit coverage. We can use this as a default target of
        # last resort.
        max_commands = 0
        max_commands_target = None

        for target, compdb in self.items():
            if max_commands_target is None or len(compdb) > max_commands:
                max_commands_target = target
                max_commands = len(compdb)

            compdb.to_file(
                self.settings.working_dir / compdb_generate_file_path(target)
            )

        max_commands_target_path = (
            self.settings.working_dir / MAX_COMMANDS_TARGET_FILENAME
        )

        if max_commands_target is not None:
            if max_commands_target_path.exists():
                max_commands_target_path.unlink()

            with open(
                max_commands_target_path, 'x'
            ) as max_commands_target_file:
                max_commands_target_file.write(max_commands_target)

    @classmethod
    def merge(
        cls, *db_sets: 'CppCompilationDatabasesMap'
    ) -> 'CppCompilationDatabasesMap':
        """Merge several sets of processed compilation databases.

        If you process N compilation databases produced by a build system,
        you'll end up with N sets of processed compilation databases,
        containing databases for one or more targets each. This method
        merges them into one set of databases with one database per target.

        The expectation is that the vast majority of the time, each of the
        raw compilation databases that are processed will contain distinct
        targets, meaning that the keys of each ``CppCompilationDatabases``
        object that's merged will be unique to each object, and this operation
        is nothing more than a shallow merge.

        However, this also supports the case where targets may overlap between
        ``CppCompilationDatabases`` objects. In that case, we prioritize
        correctness, ensuring that the resulting compilation databases will
        work correctly with clangd. This means not including duplicate compile
        commands for the same file in the same target's database. The choice
        of which duplicate compile command ends up in the final database is
        unspecified and subject to change. Note also that this method expects
        the ``settings`` value to be the same between all of the provided
        ``CppCompilationDatabases`` objects.
        """
        if len(db_sets) == 0:
            raise ValueError(
                'At least one set of compilation databases is ' 'required.'
            )

        # Shortcut for the most common case.
        if len(db_sets) == 1:
            return db_sets[0]

        merged = cls(db_sets[0].settings)

        for dbs in db_sets:
            for target, db in dbs.items():
                merged[target].merge(db)

        return merged


@dataclass(frozen=True)
class CppIdeFeaturesTarget:
    """Data pertaining to a C++ code analysis target."""

    name: str
    compdb_file_path: Path
    compdb_cache_path: Optional[Path]
    is_enabled: bool


class CppIdeFeaturesState:
    """The state of the C++ analysis targets in the working directory.

    Targets can be:

    - **Available**: A compilation database is present for this target.
    - **Enabled**: Any targets are enabled by default, but a subset can be
      enabled instead in the pw_ide settings. Enabled targets need
      not be available if they haven't had a compilation database
      created through processing yet.
    - **Valid**: Is both available and enabled.
    - **Current**: The one currently activated target that is exposed to clangd.
    """

    def __init__(self, settings: PigweedIdeSettings) -> None:
        self.settings = settings

        # We filter out Nones below, so we can assume its a str
        target: Callable[[Path], str] = lambda path: cast(
            str, compdb_target_from_path(path)
        )

        # Contains every compilation database that's present in the working dir.
        # This dict comprehension looks monstrous, but it just finds targets and
        # associates the target names with their CppIdeFeaturesTarget objects.
        self.targets: Dict[str, CppIdeFeaturesTarget] = {
            target(file_path): CppIdeFeaturesTarget(
                name=target(file_path),
                compdb_file_path=file_path,
                compdb_cache_path=compdb_cache_path_if_exists(
                    settings.working_dir, compdb_target_from_path(file_path)
                ),
                is_enabled=target_is_enabled(target(file_path), settings),
            )
            for file_path in settings.working_dir.iterdir()
            if file_path.match(
                f'{_COMPDB_FILE_PREFIX}*{_COMPDB_FILE_EXTENSION}'
            )
            # This filters out the symlink
            and compdb_target_from_path(file_path) is not None
        }

        # Contains the currently selected target.
        self._current_target: Optional[CppIdeFeaturesTarget] = None

        # This is diagnostic data; it tells us what the current target should
        # be, even if the state of the working directory is corrupted and the
        # compilation database for the target isn't actually present. Anything
        # that requires a compilation database to be definitely present should
        # use `current_target` instead of these values.
        self.current_target_name: Optional[str] = None
        self.current_target_file_path: Optional[Path] = None
        self.current_target_exists: Optional[bool] = None

        # Contains the name of the target that has the most compile commands,
        # i.e., the target with the most file coverage in the project.
        self._max_commands_target: Optional[str] = None

        try:
            src_file = Path(
                os.readlink(
                    (settings.working_dir / compdb_generate_file_path())
                )
            )

            self.current_target_file_path = src_file
            self.current_target_name = compdb_target_from_path(src_file)

            if not self.current_target_file_path.exists():
                self.current_target_exists = False

            else:
                self.current_target_exists = True
                self._current_target = CppIdeFeaturesTarget(
                    name=target(src_file),
                    compdb_file_path=src_file,
                    compdb_cache_path=compdb_cache_path_if_exists(
                        settings.working_dir, target(src_file)
                    ),
                    is_enabled=target_is_enabled(target(src_file), settings),
                )
        except (FileNotFoundError, OSError):
            # If the symlink doesn't exist, there is no current target.
            pass

        try:
            with open(
                settings.working_dir / MAX_COMMANDS_TARGET_FILENAME
            ) as max_commands_target_file:
                self._max_commands_target = max_commands_target_file.readline()
        except FileNotFoundError:
            # If the file doesn't exist, a compilation database probably
            # hasn't been processed yet.
            pass

    def __len__(self) -> int:
        return len(self.targets)

    def __getitem__(self, index: str) -> CppIdeFeaturesTarget:
        return self.targets[index]

    def __iter__(self) -> Generator[CppIdeFeaturesTarget, None, None]:
        return (target for target in self.targets.values())

    @property
    def current_target(self) -> Optional[str]:
        """The name of current target used for code analysis.

        The presence of a symlink with the expected filename pointing to a
        compilation database matching the expected filename format is the source
        of truth on what the current target is.
        """
        return (
            self._current_target.name
            if self._current_target is not None
            else None
        )

    @current_target.setter
    def current_target(self, target: Optional[str]) -> None:
        settings = self.settings

        if not self.is_valid_target(target):
            raise InvalidTargetException()

        # The check above rules out None.
        target = cast(str, target)

        compdb_symlink_path = settings.working_dir / compdb_generate_file_path()

        compdb_target_path = settings.working_dir / compdb_generate_file_path(
            target
        )

        if not compdb_target_path.exists():
            raise MissingCompDbException()

        set_symlink(compdb_target_path, compdb_symlink_path)

        cache_symlink_path = settings.working_dir / compdb_generate_cache_path()

        cache_target_path = settings.working_dir / compdb_generate_cache_path(
            target
        )

        if not cache_target_path.exists():
            os.mkdir(cache_target_path)

        set_symlink(cache_target_path, cache_symlink_path)

    @property
    def max_commands_target(self) -> Optional[str]:
        """The target with the most compile commands.

        The return value is the name of the target with the largest number of
        compile commands (i.e., the largest coverage across the files in the
        project). This can be a useful "default target of last resort".
        """
        return self._max_commands_target

    @property
    def available_targets(self) -> List[str]:
        return list(self.targets.keys())

    @property
    def enabled_available_targets(self) -> Generator[str, None, None]:
        return (
            name for name, target in self.targets.items() if target.is_enabled
        )

    def is_valid_target(self, target: Optional[str]) -> bool:
        if target is None or (data := self.targets.get(target, None)) is None:
            return False

        return data.is_enabled


def aggregate_compilation_database_targets(
    compdb_file: LoadableToCppCompilationDatabase,
    settings: PigweedIdeSettings,
    build_dir: Path,
    *,
    default_path: Optional[Path] = None,
    path_globs: Optional[List[str]] = None,
) -> List[str]:
    """Return all valid unique targets from a ``clang`` compilation database."""
    compdbs_map = CppCompilationDatabase.load(compdb_file, build_dir).process(
        settings, default_path=default_path, path_globs=path_globs
    )

    return compdbs_map.targets


def delete_compilation_databases(settings: PigweedIdeSettings) -> None:
    """Delete all compilation databases in the working directory.

    This leaves cache directories in place.
    """
    if settings.working_dir.exists():
        for path in settings.working_dir.iterdir():
            if path.name.startswith(_COMPDB_FILE_PREFIX):
                try:
                    path.unlink()
                except FileNotFoundError:
                    pass


def delete_compilation_database_caches(settings: PigweedIdeSettings) -> None:
    """Delete all compilation database caches in the working directory.

    This leaves all compilation databases in place.
    """
    if settings.working_dir.exists():
        for path in settings.working_dir.iterdir():
            if path.name.startswith(_COMPDB_CACHE_DIR_PREFIX):
                try:
                    path.unlink()
                except FileNotFoundError:
                    pass


class ClangdSettings:
    """Makes system-specific settings for running ``clangd`` with Pigweed."""

    def __init__(self, settings: PigweedIdeSettings):
        self.compile_commands_dir: Path = PigweedIdeSettings().working_dir
        self.clangd_path: Path = (
            Path(PW_PIGWEED_CIPD_INSTALL_DIR) / 'bin' / 'clangd'
        )

        self.arguments: List[str] = [
            f'--compile-commands-dir={self.compile_commands_dir}',
            f'--query-driver={settings.clangd_query_driver_str()}',
            '--background-index',
            '--clang-tidy',
        ]

    def command(self, system: str = platform.system()) -> str:
        """Return the command that runs clangd with Pigweed paths."""

        def make_command(line_continuation: str):
            arguments = f' {line_continuation}\n'.join(
                f'  {arg}' for arg in self.arguments
            )
            return f'\n{self.clangd_path} {line_continuation}\n{arguments}'

        if system.lower() == 'json':
            return '\n' + json.dumps(
                [str(self.clangd_path), *self.arguments], indent=2
            )

        if system.lower() in ['cmd', 'batch']:
            return make_command('`')

        if system.lower() in ['powershell', 'pwsh']:
            return make_command('^')

        if system.lower() == 'windows':
            return (
                f'\nIn PowerShell:\n{make_command("`")}'
                f'\n\nIn Command Prompt:\n{make_command("^")}'
            )

        # Default case for *sh-like shells.
        return make_command('\\')
