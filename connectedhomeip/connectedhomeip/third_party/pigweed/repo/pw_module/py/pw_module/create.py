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
"""Creates a new Pigweed module."""

import abc
import argparse
import dataclasses
from dataclasses import dataclass
import datetime
import logging
import os
from pathlib import Path
import re
import sys

from typing import Any, Dict, Iterable, List, Optional, Type, Union

from pw_build import generate_modules_lists

_LOG = logging.getLogger(__name__)

_PIGWEED_LICENSE = f"""
# Copyright {datetime.datetime.now().year} The Pigweed Authors
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
# the License.""".lstrip()

_PIGWEED_LICENSE_CC = _PIGWEED_LICENSE.replace('#', '//')


# TODO(frolv): Adapted from pw_protobuf. Consolidate them.
class _OutputFile:
    DEFAULT_INDENT_WIDTH = 2

    def __init__(self, file: Path, indent_width: int = DEFAULT_INDENT_WIDTH):
        self._file = file
        self._content: List[str] = []
        self._indent_width: int = indent_width
        self._indentation = 0

    def line(self, line: str = '') -> None:
        if line:
            self._content.append(' ' * self._indentation)
            self._content.append(line)
        self._content.append('\n')

    def indent(
        self,
        width: Optional[int] = None,
    ) -> '_OutputFile._IndentationContext':
        """Increases the indentation level of the output."""
        return self._IndentationContext(
            self, width if width is not None else self._indent_width
        )

    @property
    def path(self) -> Path:
        return self._file

    @property
    def content(self) -> str:
        return ''.join(self._content)

    def write(self) -> None:
        print('  create  ' + str(self._file.relative_to(Path.cwd())))
        self._file.write_text(self.content)

    class _IndentationContext:
        """Context that increases the output's indentation when it is active."""

        def __init__(self, output: '_OutputFile', width: int):
            self._output = output
            self._width: int = width

        def __enter__(self):
            self._output._indentation += self._width

        def __exit__(self, typ, value, traceback):
            self._output._indentation -= self._width


class _ModuleName:
    _MODULE_NAME_REGEX = '(^[a-zA-Z]{2,})((_[a-zA-Z0-9]+)+)$'

    def __init__(self, prefix: str, main: str) -> None:
        self._prefix = prefix
        self._main = main

    @property
    def full(self) -> str:
        return f'{self._prefix}_{self._main}'

    @property
    def prefix(self) -> str:
        return self._prefix

    @property
    def main(self) -> str:
        return self._main

    @property
    def default_namespace(self) -> str:
        return f'{self._prefix}::{self._main}'

    def upper_camel_case(self) -> str:
        return ''.join(s.capitalize() for s in self._main.split('_'))

    def __str__(self) -> str:
        return self.full

    def __repr__(self) -> str:
        return self.full

    @classmethod
    def parse(cls, name: str) -> Optional['_ModuleName']:
        match = re.fullmatch(_ModuleName._MODULE_NAME_REGEX, name)
        if not match:
            return None

        return cls(match.group(1), match.group(2)[1:])


@dataclass
class _ModuleContext:
    name: _ModuleName
    dir: Path
    root_build_files: List['_BuildFile']
    sub_build_files: List['_BuildFile']
    build_systems: List[str]
    is_upstream: bool

    def build_files(self) -> Iterable['_BuildFile']:
        yield from self.root_build_files
        yield from self.sub_build_files

    def add_docs_file(self, file: Path):
        for build_file in self.root_build_files:
            build_file.add_docs_source(str(file.relative_to(self.dir)))

    def add_cc_target(self, target: '_BuildFile.CcTarget') -> None:
        for build_file in self.root_build_files:
            build_file.add_cc_target(target)

    def add_cc_test(self, target: '_BuildFile.CcTarget') -> None:
        for build_file in self.root_build_files:
            build_file.add_cc_test(target)


class _BuildFile:
    """Abstract representation of a build file for a module."""

    @dataclass
    class Target:
        name: str

        # TODO(frolv): Shouldn't be a string list as that's build system
        # specific. Figure out a way to resolve dependencies from targets.
        deps: List[str] = dataclasses.field(default_factory=list)

    @dataclass
    class CcTarget(Target):
        sources: List[Path] = dataclasses.field(default_factory=list)
        headers: List[Path] = dataclasses.field(default_factory=list)

        def rebased_sources(self, rebase_path: Path) -> Iterable[str]:
            return (str(src.relative_to(rebase_path)) for src in self.sources)

        def rebased_headers(self, rebase_path: Path) -> Iterable[str]:
            return (str(hdr.relative_to(rebase_path)) for hdr in self.headers)

    def __init__(self, path: Path, ctx: _ModuleContext):
        self._path = path
        self._ctx = ctx

        self._docs_sources: List[str] = []
        self._cc_targets: List[_BuildFile.CcTarget] = []
        self._cc_tests: List[_BuildFile.CcTarget] = []

    @property
    def path(self) -> Path:
        return self._path

    @property
    def dir(self) -> Path:
        return self._path.parent

    def add_docs_source(self, filename: str) -> None:
        self._docs_sources.append(filename)

    def add_cc_target(self, target: CcTarget) -> None:
        self._cc_targets.append(target)

    def add_cc_test(self, target: CcTarget) -> None:
        self._cc_tests.append(target)

    def write(self) -> None:
        """Writes the contents of the build file to disk."""
        file = _OutputFile(self._path, self._indent_width())

        if self._ctx.is_upstream:
            file.line(_PIGWEED_LICENSE)
            file.line()

        self._write_preamble(file)

        for target in self._cc_targets:
            file.line()
            self._write_cc_target(file, target)

        for target in self._cc_tests:
            file.line()
            self._write_cc_test(file, target)

        if self._docs_sources:
            file.line()
            self._write_docs_target(file, self._docs_sources)

        file.write()

    @abc.abstractmethod
    def _indent_width(self) -> int:
        """Returns the default indent width for the build file's code style."""

    @abc.abstractmethod
    def _write_preamble(self, file: _OutputFile) -> None:
        """Formats"""

    @abc.abstractmethod
    def _write_cc_target(
        self,
        file: _OutputFile,
        target: '_BuildFile.CcTarget',
    ) -> None:
        """Defines a C++ library target within the build file."""

    @abc.abstractmethod
    def _write_cc_test(
        self,
        file: _OutputFile,
        target: '_BuildFile.CcTarget',
    ) -> None:
        """Defines a C++ unit test target within the build file."""

    @abc.abstractmethod
    def _write_docs_target(
        self,
        file: _OutputFile,
        docs_sources: List[str],
    ) -> None:
        """Defines a documentation target within the build file."""


# TODO(frolv): The Dict here should be Dict[str, '_GnVal'] (i.e. _GnScope),
# but mypy does not yet support recursive types:
# https://github.com/python/mypy/issues/731
_GnVal = Union[bool, int, str, List[str], Dict[str, Any]]
_GnScope = Dict[str, _GnVal]


class _GnBuildFile(_BuildFile):
    _DEFAULT_FILENAME = 'BUILD.gn'
    _INCLUDE_CONFIG_TARGET = 'public_include_path'

    def __init__(
        self,
        directory: Path,
        ctx: _ModuleContext,
        filename: str = _DEFAULT_FILENAME,
    ):
        super().__init__(directory / filename, ctx)

    def _indent_width(self) -> int:
        return 2

    def _write_preamble(self, file: _OutputFile) -> None:
        # Upstream modules always require a tests target, even if it's empty.
        has_tests = len(self._cc_tests) > 0 or self._ctx.is_upstream

        imports = []

        if self._cc_targets:
            imports.append('$dir_pw_build/target_types.gni')

        if has_tests:
            imports.append('$dir_pw_unit_test/test.gni')

        if self._docs_sources:
            imports.append('$dir_pw_docgen/docs.gni')

        file.line('import("//build_overrides/pigweed.gni")\n')
        for imp in sorted(imports):
            file.line(f'import("{imp}")')

        if self._cc_targets:
            file.line()
            _GnBuildFile._target(
                file,
                'config',
                _GnBuildFile._INCLUDE_CONFIG_TARGET,
                {
                    'include_dirs': ['public'],
                    'visibility': [':*'],
                },
            )

        if has_tests:
            file.line()
            _GnBuildFile._target(
                file,
                'pw_test_group',
                'tests',
                {
                    'tests': list(f':{test.name}' for test in self._cc_tests),
                },
            )

    def _write_cc_target(
        self,
        file: _OutputFile,
        target: _BuildFile.CcTarget,
    ) -> None:
        """Defines a GN source_set for a C++ target."""

        target_vars: _GnScope = {}

        if target.headers:
            target_vars['public_configs'] = [
                f':{_GnBuildFile._INCLUDE_CONFIG_TARGET}'
            ]
            target_vars['public'] = list(target.rebased_headers(self.dir))

        if target.sources:
            target_vars['sources'] = list(target.rebased_sources(self.dir))

        if target.deps:
            target_vars['deps'] = target.deps

        _GnBuildFile._target(file, 'pw_source_set', target.name, target_vars)

    def _write_cc_test(
        self,
        file: _OutputFile,
        target: '_BuildFile.CcTarget',
    ) -> None:
        _GnBuildFile._target(
            file,
            'pw_test',
            target.name,
            {
                'sources': list(target.rebased_sources(self.dir)),
                'deps': target.deps,
            },
        )

    def _write_docs_target(
        self,
        file: _OutputFile,
        docs_sources: List[str],
    ) -> None:
        """Defines a pw_doc_group for module documentation."""
        _GnBuildFile._target(
            file,
            'pw_doc_group',
            'docs',
            {
                'sources': docs_sources,
            },
        )

    @staticmethod
    def _target(
        file: _OutputFile,
        target_type: str,
        name: str,
        args: _GnScope,
    ) -> None:
        """Formats a GN target."""

        file.line(f'{target_type}("{name}") {{')

        with file.indent():
            _GnBuildFile._format_gn_scope(file, args)

        file.line('}')

    @staticmethod
    def _format_gn_scope(file: _OutputFile, scope: _GnScope) -> None:
        """Formats all of the variables within a GN scope to a file.

        This function does not write the enclosing braces of the outer scope to
        support use from multiple formatting contexts.
        """
        for key, val in scope.items():
            if isinstance(val, int):
                file.line(f'{key} = {val}')
                continue

            if isinstance(val, str):
                file.line(f'{key} = {_GnBuildFile._gn_string(val)}')
                continue

            if isinstance(val, bool):
                file.line(f'{key} = {str(val).lower()}')
                continue

            if isinstance(val, dict):
                file.line(f'{key} = {{')
                with file.indent():
                    _GnBuildFile._format_gn_scope(file, val)
                file.line('}')
                continue

            # Format a list of strings.
            # TODO(frolv): Lists of other types?
            assert isinstance(val, list)

            if not val:
                file.line(f'{key} = []')
                continue

            if len(val) == 1:
                file.line(f'{key} = [ {_GnBuildFile._gn_string(val[0])} ]')
                continue

            file.line(f'{key} = [')
            with file.indent():
                for string in sorted(val):
                    file.line(f'{_GnBuildFile._gn_string(string)},')
            file.line(']')

    @staticmethod
    def _gn_string(string: str) -> str:
        """Converts a Python string into a string literal within a GN file.

        Accounts for the possibility of variable interpolation within GN,
        removing quotes if unnecessary:

            "string"           ->  "string"
            "string"           ->  "string"
            "$var"             ->  var
            "$var2"            ->  var2
            "$3var"            ->  "$3var"
            "$dir_pw_foo"      ->  dir_pw_foo
            "$dir_pw_foo:bar"  ->  "$dir_pw_foo:bar"
            "$dir_pw_foo/baz"  ->  "$dir_pw_foo/baz"
            "${dir_pw_foo}"    ->  dir_pw_foo

        """

        # Check if the entire string refers to a interpolated variable.
        #
        # Simple case: '$' followed a single word, e.g. "$my_variable".
        # Note that identifiers can't start with a number.
        if re.fullmatch(r'^\$[a-zA-Z_]\w*$', string):
            return string[1:]

        # GN permits wrapping an interpolated variable in braces.
        # Check for strings of the format "${my_variable}".
        if re.fullmatch(r'^\$\{[a-zA-Z_]\w*\}$', string):
            return string[2:-1]

        return f'"{string}"'


class _BazelBuildFile(_BuildFile):
    _DEFAULT_FILENAME = 'BUILD.bazel'

    def __init__(
        self,
        directory: Path,
        ctx: _ModuleContext,
        filename: str = _DEFAULT_FILENAME,
    ):
        super().__init__(directory / filename, ctx)

    def _indent_width(self) -> int:
        return 4

    def _write_preamble(self, file: _OutputFile) -> None:
        imports = ['//pw_build:pigweed.bzl']
        if self._cc_targets:
            imports.append('pw_cc_library')

        if self._cc_tests:
            imports.append('pw_cc_test')

        file.line('load(')
        with file.indent():
            for imp in sorted(imports):
                file.line(f'"{imp}",')
        file.line(')\n')

        file.line('package(default_visibility = ["//visibility:public"])\n')
        file.line('licenses(["notice"])')

    def _write_cc_target(
        self,
        file: _OutputFile,
        target: _BuildFile.CcTarget,
    ) -> None:
        _BazelBuildFile._target(
            file,
            'pw_cc_library',
            target.name,
            {
                'srcs': list(target.rebased_sources(self.dir)),
                'hdrs': list(target.rebased_headers(self.dir)),
                'includes': ['public'],
            },
        )

    def _write_cc_test(
        self,
        file: _OutputFile,
        target: '_BuildFile.CcTarget',
    ) -> None:
        _BazelBuildFile._target(
            file,
            'pw_cc_test',
            target.name,
            {
                'srcs': list(target.rebased_sources(self.dir)),
                'deps': target.deps,
            },
        )

    def _write_docs_target(
        self,
        file: _OutputFile,
        docs_sources: List[str],
    ) -> None:
        file.line('# Bazel does not yet support building docs.')
        _BazelBuildFile._target(
            file, 'filegroup', 'docs', {'srcs': docs_sources}
        )

    @staticmethod
    def _target(
        file: _OutputFile,
        target_type: str,
        name: str,
        keys: Dict[str, List[str]],
    ) -> None:
        file.line(f'{target_type}(')

        with file.indent():
            file.line(f'name = "{name}",')

            for k, vals in keys.items():
                if len(vals) == 1:
                    file.line(f'{k} = ["{vals[0]}"],')
                    continue

                file.line(f'{k} = [')
                with file.indent():
                    for val in sorted(vals):
                        file.line(f'"{val}",')
                file.line('],')

        file.line(')')


class _CmakeBuildFile(_BuildFile):
    _DEFAULT_FILENAME = 'CMakeLists.txt'

    def __init__(
        self,
        directory: Path,
        ctx: _ModuleContext,
        filename: str = _DEFAULT_FILENAME,
    ):
        super().__init__(directory / filename, ctx)

    def _indent_width(self) -> int:
        return 2

    def _write_preamble(self, file: _OutputFile) -> None:
        file.line('include($ENV{PW_ROOT}/pw_build/pigweed.cmake)')

    def _write_cc_target(
        self,
        file: _OutputFile,
        target: _BuildFile.CcTarget,
    ) -> None:
        if target.name == self._ctx.name.full:
            target_name = target.name
        else:
            target_name = f'{self._ctx.name.full}.{target.name}'

        _CmakeBuildFile._target(
            file,
            'pw_add_module_library',
            target_name,
            {
                'sources': list(target.rebased_sources(self.dir)),
                'headers': list(target.rebased_headers(self.dir)),
                'public_includes': ['public'],
            },
        )

    def _write_cc_test(
        self,
        file: _OutputFile,
        target: '_BuildFile.CcTarget',
    ) -> None:
        _CmakeBuildFile._target(
            file,
            'pw_auto_add_module_tests',
            self._ctx.name.full,
            {'private_deps': []},
        )

    def _write_docs_target(
        self,
        file: _OutputFile,
        docs_sources: List[str],
    ) -> None:
        file.line('# CMake does not yet support building docs.')

    @staticmethod
    def _target(
        file: _OutputFile,
        target_type: str,
        name: str,
        keys: Dict[str, List[str]],
    ) -> None:
        file.line(f'{target_type}({name}')

        with file.indent():
            for k, vals in keys.items():
                file.line(k.upper())
                with file.indent():
                    for val in sorted(vals):
                        file.line(val)

        file.line(')')


class _LanguageGenerator:
    """Generates files for a programming language in a new Pigweed module."""

    def __init__(self, ctx: _ModuleContext) -> None:
        self._ctx = ctx

    @abc.abstractmethod
    def create_source_files(self) -> None:
        """Creates the boilerplate source files required by the language."""


class _CcLanguageGenerator(_LanguageGenerator):
    """Generates boilerplate source files for a C++ module."""

    def __init__(self, ctx: _ModuleContext) -> None:
        super().__init__(ctx)

        self._public_dir = ctx.dir / 'public'
        self._headers_dir = self._public_dir / ctx.name.full

    def create_source_files(self) -> None:
        self._headers_dir.mkdir(parents=True)

        main_header = self._new_header(self._ctx.name.main)
        main_source = self._new_source(self._ctx.name.main)
        test_source = self._new_source(f'{self._ctx.name.main}_test')

        # TODO(frolv): This could be configurable.
        namespace = self._ctx.name.default_namespace

        main_source.line(
            f'#include "{main_header.path.relative_to(self._public_dir)}"\n'
        )
        main_source.line(f'namespace {namespace} {{\n')
        main_source.line('int magic = 42;\n')
        main_source.line(f'}}  // namespace {namespace}')

        main_header.line(f'namespace {namespace} {{\n')
        main_header.line('extern int magic;\n')
        main_header.line(f'}}  // namespace {namespace}')

        test_source.line(
            f'#include "{main_header.path.relative_to(self._public_dir)}"\n'
        )
        test_source.line('#include "gtest/gtest.h"\n')
        test_source.line(f'namespace {namespace} {{')
        test_source.line('namespace {\n')
        test_source.line(
            f'TEST({self._ctx.name.upper_camel_case()}, GeneratesCorrectly) {{'
        )
        with test_source.indent():
            test_source.line('EXPECT_EQ(magic, 42);')
        test_source.line('}\n')
        test_source.line('}  // namespace')
        test_source.line(f'}}  // namespace {namespace}')

        self._ctx.add_cc_target(
            _BuildFile.CcTarget(
                name=self._ctx.name.full,
                sources=[main_source.path],
                headers=[main_header.path],
            )
        )

        self._ctx.add_cc_test(
            _BuildFile.CcTarget(
                name=f'{self._ctx.name.main}_test',
                deps=[f':{self._ctx.name.full}'],
                sources=[test_source.path],
            )
        )

        main_header.write()
        main_source.write()
        test_source.write()

    def _new_source(self, name: str) -> _OutputFile:
        file = _OutputFile(self._ctx.dir / f'{name}.cc')

        if self._ctx.is_upstream:
            file.line(_PIGWEED_LICENSE_CC)
            file.line()

        return file

    def _new_header(self, name: str) -> _OutputFile:
        file = _OutputFile(self._headers_dir / f'{name}.h')

        if self._ctx.is_upstream:
            file.line(_PIGWEED_LICENSE_CC)

        file.line('#pragma once\n')
        return file


_BUILD_FILES: Dict[str, Type[_BuildFile]] = {
    'bazel': _BazelBuildFile,
    'cmake': _CmakeBuildFile,
    'gn': _GnBuildFile,
}

_LANGUAGE_GENERATORS: Dict[str, Type[_LanguageGenerator]] = {
    'cc': _CcLanguageGenerator,
}


def _check_module_name(
    module: str,
    is_upstream: bool,
) -> Optional[_ModuleName]:
    """Checks whether a module name is valid."""

    name = _ModuleName.parse(module)
    if not name:
        _LOG.error(
            '"%s" does not conform to the Pigweed module name format', module
        )
        return None

    if is_upstream and name.prefix != 'pw':
        _LOG.error('Modules within Pigweed itself must start with "pw_"')
        return None

    return name


def _create_main_docs_file(ctx: _ModuleContext) -> None:
    """Populates the top-level docs.rst file within a new module."""

    docs_file = _OutputFile(ctx.dir / 'docs.rst')
    docs_file.line(f'.. _module-{ctx.name}:\n')

    title = '=' * len(ctx.name.full)
    docs_file.line(title)
    docs_file.line(ctx.name.full)
    docs_file.line(title)
    docs_file.line(f'This is the main documentation file for {ctx.name}.')

    ctx.add_docs_file(docs_file.path)

    docs_file.write()


def _basic_module_setup(
    module_name: _ModuleName,
    module_dir: Path,
    build_systems: Iterable[str],
    is_upstream: bool,
) -> _ModuleContext:
    """Creates the basic layout of a Pigweed module."""
    module_dir.mkdir()

    ctx = _ModuleContext(
        name=module_name,
        dir=module_dir,
        root_build_files=[],
        sub_build_files=[],
        build_systems=list(build_systems),
        is_upstream=is_upstream,
    )

    ctx.root_build_files.extend(
        _BUILD_FILES[build](module_dir, ctx) for build in ctx.build_systems
    )

    _create_main_docs_file(ctx)

    return ctx


def _create_module(
    module: str, languages: Iterable[str], build_systems: Iterable[str]
) -> None:
    project_root = Path(os.environ.get('PW_PROJECT_ROOT', ''))
    assert project_root.is_dir()

    is_upstream = os.environ.get('PW_ROOT') == str(project_root)

    module_name = _check_module_name(module, is_upstream)
    if not module_name:
        sys.exit(1)

    if not is_upstream:
        _LOG.error(
            '`pw module create` is experimental and does '
            'not yet support downstream projects.'
        )
        sys.exit(1)

    module_dir = project_root / module

    if module_dir.is_dir():
        _LOG.error('Module %s already exists', module)
        sys.exit(1)

    if module_dir.is_file():
        _LOG.error(
            'Cannot create module %s as a file of that name already exists',
            module,
        )
        sys.exit(1)

    ctx = _basic_module_setup(
        module_name, module_dir, build_systems, is_upstream
    )

    try:
        generators = list(_LANGUAGE_GENERATORS[lang](ctx) for lang in languages)
    except KeyError as key:
        _LOG.error('Unsupported language: %s', key)
        sys.exit(1)

    for generator in generators:
        generator.create_source_files()

    for build_file in ctx.build_files():
        build_file.write()

    if is_upstream:
        modules_file = project_root / 'PIGWEED_MODULES'
        if not modules_file.exists():
            _LOG.error(
                'Could not locate PIGWEED_MODULES file; '
                'your repository may be in a bad state.'
            )
            return

        modules_gni_file = (
            project_root / 'pw_build' / 'generated_pigweed_modules_lists.gni'
        )

        # Cut off the extra newline at the end of the file.
        modules_list = modules_file.read_text().split('\n')[:-1]
        modules_list.append(module_name.full)
        modules_list.sort()
        modules_list.append('')
        modules_file.write_text('\n'.join(modules_list))
        print('  modify  ' + str(modules_file.relative_to(Path.cwd())))

        generate_modules_lists.main(
            root=project_root,
            modules_list=modules_file,
            modules_gni_file=modules_gni_file,
            warn_only=None,
        )
        print('  modify  ' + str(modules_gni_file.relative_to(Path.cwd())))

    print()
    _LOG.info(
        'Module %s created at %s',
        module_name,
        module_dir.relative_to(Path.cwd()),
    )


def register_subcommand(parser: argparse.ArgumentParser) -> None:
    csv = lambda s: s.split(',')

    parser.add_argument(
        '--build-systems',
        help=(
            'Comma-separated list of build systems the module supports. '
            f'Options: {", ".join(_BUILD_FILES.keys())}'
        ),
        type=csv,
        default=_BUILD_FILES.keys(),
        metavar='BUILD[,BUILD,...]',
    )
    parser.add_argument(
        '--languages',
        help=(
            'Comma-separated list of languages the module will use. '
            f'Options: {", ".join(_LANGUAGE_GENERATORS.keys())}'
        ),
        type=csv,
        default=[],
        metavar='LANG[,LANG,...]',
    )
    parser.add_argument(
        'module', help='Name of the module to create.', metavar='MODULE_NAME'
    )
    parser.set_defaults(func=_create_module)
