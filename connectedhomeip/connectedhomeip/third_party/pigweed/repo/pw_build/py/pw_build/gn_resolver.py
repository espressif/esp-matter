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
"""Evaluates target expressions within a GN build context."""

import argparse
from dataclasses import dataclass
import enum
import logging
import os
import re
import sys
from pathlib import Path
from typing import (
    Callable,
    Dict,
    List,
    Iterable,
    Iterator,
    NamedTuple,
    Optional,
    Tuple,
)

_LOG = logging.getLogger(__name__)


def abspath(path: Path) -> Path:
    """Turns a path into an absolute path, not resolving symlinks."""
    return Path(os.path.abspath(path))


class GnPaths(NamedTuple):
    """The set of paths needed to resolve GN paths to filesystem paths."""

    root: Path
    build: Path
    cwd: Path

    # Toolchain label or '' if using the default toolchain
    toolchain: str

    def resolve(self, gn_path: str) -> Path:
        """Resolves a GN path to a filesystem path."""
        if gn_path.startswith('//'):
            return abspath(self.root.joinpath(gn_path.lstrip('/')))

        return abspath(self.cwd.joinpath(gn_path))

    def resolve_paths(self, gn_paths: str, sep: str = ';') -> str:
        """Resolves GN paths to filesystem paths in a delimited string."""
        return sep.join(str(self.resolve(path)) for path in gn_paths.split(sep))


@dataclass(frozen=True)
class Label:
    """Represents a GN label."""

    name: str
    dir: Path
    relative_dir: Path
    toolchain: Optional['Label']
    out_dir: Path
    gen_dir: Path

    def __init__(self, paths: GnPaths, label: str):
        # Use this lambda to set attributes on this frozen dataclass.
        set_attr = lambda attr, val: object.__setattr__(self, attr, val)

        # Handle explicitly-specified toolchains
        if label.endswith(')'):
            label, toolchain = label[:-1].rsplit('(', 1)
        else:
            # Prevent infinite recursion for toolchains
            toolchain = paths.toolchain if paths.toolchain != label else ''

        set_attr('toolchain', Label(paths, toolchain) if toolchain else None)

        # Split off the :target, if provided, or use the last part of the path.
        try:
            directory, name = label.rsplit(':', 1)
        except ValueError:
            directory, name = label, label.rsplit('/', 1)[-1]

        set_attr('name', name)

        # Resolve the directory to an absolute path
        set_attr('dir', paths.resolve(directory))
        set_attr('relative_dir', self.dir.relative_to(abspath(paths.root)))

        set_attr(
            'out_dir',
            paths.build / self.toolchain_name() / 'obj' / self.relative_dir,
        )
        set_attr(
            'gen_dir',
            paths.build / self.toolchain_name() / 'gen' / self.relative_dir,
        )

    def gn_label(self) -> str:
        label = f'//{self.relative_dir.as_posix()}:{self.name}'
        return f'{label}({self.toolchain!r})' if self.toolchain else label

    def toolchain_name(self) -> str:
        return self.toolchain.name if self.toolchain else ''

    def __repr__(self) -> str:
        return self.gn_label()


class _Artifact(NamedTuple):
    path: Path
    variables: Dict[str, str]


# Matches a non-phony build statement.
_GN_NINJA_BUILD_STATEMENT = re.compile(r'^build (.+):[ \n](?!phony\b)')

_OBJECTS_EXTENSIONS = ('.o',)

# Extensions used for compilation artifacts.
_MAIN_ARTIFACTS = '', '.elf', '.a', '.so', '.dylib', '.exe', '.lib', '.dll'


def _get_artifact(entries: List[str]) -> _Artifact:
    """Attempts to resolve which artifact to use if there are multiple.

    Selects artifacts based on extension. This will not work if a toolchain
    creates multiple compilation artifacts from one command (e.g. .a and .elf).
    """
    assert entries, "There should be at least one entry here!"

    if len(entries) == 1:
        return _Artifact(Path(entries[0]), {})

    filtered = [p for p in entries if Path(p).suffix in _MAIN_ARTIFACTS]

    if len(filtered) == 1:
        return _Artifact(Path(filtered[0]), {})

    raise ExpressionError(
        f'Expected 1, but found {len(filtered)} artifacts, after filtering for '
        f'extensions {", ".join(repr(e) for e in _MAIN_ARTIFACTS)}: {entries}'
    )


def _parse_build_artifacts(fd) -> Iterator[_Artifact]:
    """Partially parses the build statements in a Ninja file."""
    lines = iter(fd)

    def next_line():
        try:
            return next(lines)
        except StopIteration:
            return None

    # Serves as the parse state (only two states)
    artifact: Optional[_Artifact] = None

    line = next_line()

    while line is not None:
        if artifact:
            if line.startswith('  '):  # build variable statements are indented
                key, value = (a.strip() for a in line.split('=', 1))
                artifact.variables[key] = value
                line = next_line()
            else:
                yield artifact
                artifact = None
        else:
            match = _GN_NINJA_BUILD_STATEMENT.match(line)
            if match:
                artifact = _get_artifact(match.group(1).split())

            line = next_line()

    if artifact:
        yield artifact


def _search_target_ninja(
    ninja_file: Path, target: Label
) -> Tuple[Optional[Path], List[Path]]:
    """Parses the main output file and object files from <target>.ninja."""

    artifact: Optional[Path] = None
    objects: List[Path] = []

    _LOG.debug('Parsing target Ninja file %s for %s', ninja_file, target)

    with ninja_file.open() as fd:
        for path, _ in _parse_build_artifacts(fd):
            # Older GN used .stamp files when there is no build artifact.
            if path.suffix == '.stamp':
                continue

            if str(path).endswith(_OBJECTS_EXTENSIONS):
                objects.append(Path(path))
            else:
                assert not artifact, f'Multiple artifacts for {target}!'
                artifact = Path(path)

    return artifact, objects


def _search_toolchain_ninja(
    ninja_file: Path, paths: GnPaths, target: Label
) -> Optional[Path]:
    """Searches the toolchain.ninja file for outputs from the provided target.

    Files created by an action appear in toolchain.ninja instead of in their own
    <target>.ninja. If the specified target has a single output file in
    toolchain.ninja, this function returns its path.
    """

    _LOG.debug('Searching toolchain Ninja file %s for %s', ninja_file, target)

    # Older versions of GN used a .stamp file to signal completion of a target.
    stamp_dir = target.out_dir.relative_to(paths.build).as_posix()
    stamp_tool = 'stamp'
    if target.toolchain_name() != '':
        stamp_tool = f'{target.toolchain_name()}_stamp'
    stamp_statement = f'build {stamp_dir}/{target.name}.stamp: {stamp_tool} '

    # Newer GN uses a phony Ninja target to signal completion of a target.
    phony_dir = Path(
        target.toolchain_name(), 'phony', target.relative_dir
    ).as_posix()
    phony_statement = f'build {phony_dir}/{target.name}: phony '

    with ninja_file.open() as fd:
        for line in fd:
            for statement in (phony_statement, stamp_statement):
                if line.startswith(statement):
                    output_files = line[len(statement) :].strip().split()
                    if len(output_files) == 1:
                        return Path(output_files[0])

                    break

    return None


def _search_ninja_files(
    paths: GnPaths, target: Label
) -> Tuple[bool, Optional[Path], List[Path]]:
    ninja_file = target.out_dir / f'{target.name}.ninja'
    if ninja_file.exists():
        return (True, *_search_target_ninja(ninja_file, target))

    ninja_file = paths.build / target.toolchain_name() / 'toolchain.ninja'
    if ninja_file.exists():
        return True, _search_toolchain_ninja(ninja_file, paths, target), []

    return False, None, []


@dataclass(frozen=True)
class TargetInfo:
    """Provides information about a target parsed from a .ninja file."""

    label: Label
    generated: bool  # True if the Ninja files for this target were generated.
    artifact: Optional[Path]
    object_files: Tuple[Path]

    def __init__(self, paths: GnPaths, target: str):
        object.__setattr__(self, 'label', Label(paths, target))

        generated, artifact, objects = _search_ninja_files(paths, self.label)

        object.__setattr__(self, 'generated', generated)
        object.__setattr__(self, 'artifact', artifact)
        object.__setattr__(self, 'object_files', tuple(objects))

    def __repr__(self) -> str:
        return repr(self.label)


class ExpressionError(Exception):
    """An error occurred while parsing an expression."""


class _ArgAction(enum.Enum):
    APPEND = 0
    OMIT = 1
    EMIT_NEW = 2


class _Expression:
    def __init__(self, match: re.Match, ending: int):
        self._match = match
        self._ending = ending

    @property
    def string(self):
        return self._match.string

    @property
    def end(self) -> int:
        return self._ending + len(_ENDING)

    def contents(self) -> str:
        return self.string[self._match.end() : self._ending]

    def expression(self) -> str:
        return self.string[self._match.start() : self.end]


_Actions = Iterator[Tuple[_ArgAction, str]]


def _target_file(paths: GnPaths, expr: _Expression) -> _Actions:
    target = TargetInfo(paths, expr.contents())

    if not target.generated:
        raise ExpressionError(f'Target {target} has not been generated by GN!')

    if target.artifact is None:
        raise ExpressionError(f'Target {target} has no output file!')

    yield _ArgAction.APPEND, str(target.artifact)


def _target_file_if_exists(paths: GnPaths, expr: _Expression) -> _Actions:
    target = TargetInfo(paths, expr.contents())

    if target.generated:
        if target.artifact is None:
            raise ExpressionError(f'Target {target} has no output file!')

        if paths.build.joinpath(target.artifact).exists():
            yield _ArgAction.APPEND, str(target.artifact)
            return

    yield _ArgAction.OMIT, ''


def _target_objects(paths: GnPaths, expr: _Expression) -> _Actions:
    if expr.expression() != expr.string:
        raise ExpressionError(
            f'The expression "{expr.expression()}" in "{expr.string}" may '
            'expand to multiple arguments, so it cannot be used alongside '
            'other text or expressions'
        )

    target = TargetInfo(paths, expr.contents())
    if not target.generated:
        raise ExpressionError(f'Target {target} has not been generated by GN!')

    for obj in target.object_files:
        yield _ArgAction.EMIT_NEW, str(obj)


# TODO(b/234886742): Replace expressions with native GN features when possible.
_FUNCTIONS: Dict['str', Callable[[GnPaths, _Expression], _Actions]] = {
    'TARGET_FILE': _target_file,
    'TARGET_FILE_IF_EXISTS': _target_file_if_exists,
    'TARGET_OBJECTS': _target_objects,
}

_START_EXPRESSION = re.compile(fr'<({"|".join(_FUNCTIONS)})\(')
_ENDING = ')>'


def _expand_arguments(paths: GnPaths, string: str) -> _Actions:
    pos = 0

    for match in _START_EXPRESSION.finditer(string):
        if pos != match.start():
            yield _ArgAction.APPEND, string[pos : match.start()]

        ending = string.find(_ENDING, match.end())
        if ending == -1:
            raise ExpressionError(
                f'Parse error: no terminating "{_ENDING}" '
                f'was found for "{string[match.start():]}"'
            )

        expression = _Expression(match, ending)
        yield from _FUNCTIONS[match.group(1)](paths, expression)

        pos = expression.end

    if pos < len(string):
        yield _ArgAction.APPEND, string[pos:]


def expand_expressions(paths: GnPaths, arg: str) -> Iterable[str]:
    """Expands <FUNCTION(...)> expressions; yields zero or more arguments."""
    if arg == '':
        return ['']

    expanded_args: List[List[str]] = [[]]

    for action, piece in _expand_arguments(paths, arg):
        if action is _ArgAction.OMIT:
            return []

        expanded_args[-1].append(piece)
        if action is _ArgAction.EMIT_NEW:
            expanded_args.append([])

    return (''.join(arg) for arg in expanded_args if arg)


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--gn-root',
        type=Path,
        required=True,
        help=(
            'Path to the root of the GN tree; '
            'value of rebase_path("//", root_build_dir)'
        ),
    )
    parser.add_argument(
        '--current-path',
        type=Path,
        required=True,
        help='Value of rebase_path(".", root_build_dir)',
    )
    parser.add_argument(
        '--default-toolchain', required=True, help='Value of default_toolchain'
    )
    parser.add_argument(
        '--current-toolchain', required=True, help='Value of current_toolchain'
    )
    parser.add_argument(
        'files',
        metavar='FILE',
        nargs='+',
        type=Path,
        help='Files to scan for expressions to evaluate',
    )
    return parser.parse_args()


def _resolve_expressions_in_file(file: Path, paths: GnPaths):
    source = file.read_text()
    file.write_text(''.join(expand_expressions(paths, source)))


def main(
    gn_root: Path,
    current_path: Path,
    default_toolchain: str,
    current_toolchain: str,
    files: Iterable[Path],
) -> int:
    """Evaluates GN target expressions within a list of files.

    Modifies the files in-place with their resolved contents.
    """
    tool = current_toolchain if current_toolchain != default_toolchain else ''
    paths = GnPaths(
        root=abspath(gn_root),
        build=Path.cwd(),
        cwd=abspath(current_path),
        toolchain=tool,
    )

    for file in files:
        try:
            _resolve_expressions_in_file(file, paths)
        except ExpressionError as err:
            _LOG.error('Error evaluating expressions in %s:', file)
            _LOG.error('  %s', err)
            return 1

    return 0


if __name__ == '__main__':
    sys.exit(main(**vars(_parse_args())))
