#!/usr/bin/env python3
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
"""Tests for the Python runner."""

import os
from pathlib import Path
import platform
import tempfile
import unittest

from pw_build.gn_resolver import ExpressionError, GnPaths, Label, TargetInfo
from pw_build.gn_resolver import expand_expressions

ROOT = Path(r'C:\gn_root' if platform.system() == 'Windows' else '/gn_root')

TEST_PATHS = GnPaths(
    ROOT,
    ROOT / 'out',
    ROOT / 'some' / 'cwd',
    '//toolchains/cool:ToolChain',
)


class LabelTest(unittest.TestCase):
    """Tests GN label parsing."""

    def setUp(self):
        self._paths_and_toolchain_name = [
            (TEST_PATHS, 'ToolChain'),
            (GnPaths(*TEST_PATHS[:3], ''), ''),
        ]

    def test_root(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, '//')
            self.assertEqual(label.name, '')
            self.assertEqual(label.dir, ROOT)
            self.assertEqual(
                label.out_dir, ROOT.joinpath('out', toolchain, 'obj')
            )
            self.assertEqual(
                label.gen_dir, ROOT.joinpath('out', toolchain, 'gen')
            )

    def test_absolute(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, '//foo/bar:baz')
            self.assertEqual(label.name, 'baz')
            self.assertEqual(label.dir, ROOT.joinpath('foo/bar'))
            self.assertEqual(
                label.out_dir, ROOT.joinpath('out', toolchain, 'obj/foo/bar')
            )
            self.assertEqual(
                label.gen_dir, ROOT.joinpath('out', toolchain, 'gen/foo/bar')
            )

    def test_absolute_implicit_target(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, '//foo/bar')
            self.assertEqual(label.name, 'bar')
            self.assertEqual(label.dir, ROOT.joinpath('foo/bar'))
            self.assertEqual(
                label.out_dir, ROOT.joinpath('out', toolchain, 'obj/foo/bar')
            )
            self.assertEqual(
                label.gen_dir, ROOT.joinpath('out', toolchain, 'gen/foo/bar')
            )

    def test_relative(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, ':tgt')
            self.assertEqual(label.name, 'tgt')
            self.assertEqual(label.dir, ROOT.joinpath('some/cwd'))
            self.assertEqual(
                label.out_dir, ROOT.joinpath('out', toolchain, 'obj/some/cwd')
            )
            self.assertEqual(
                label.gen_dir, ROOT.joinpath('out', toolchain, 'gen/some/cwd')
            )

    def test_relative_subdir(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, 'tgt')
            self.assertEqual(label.name, 'tgt')
            self.assertEqual(label.dir, ROOT.joinpath('some/cwd/tgt'))
            self.assertEqual(
                label.out_dir,
                ROOT.joinpath('out', toolchain, 'obj/some/cwd/tgt'),
            )
            self.assertEqual(
                label.gen_dir,
                ROOT.joinpath('out', toolchain, 'gen/some/cwd/tgt'),
            )

    def test_relative_parent_dir(self):
        for paths, toolchain in self._paths_and_toolchain_name:
            label = Label(paths, '..:tgt')
            self.assertEqual(label.name, 'tgt')
            self.assertEqual(label.dir, ROOT.joinpath('some'))
            self.assertEqual(
                label.out_dir, ROOT.joinpath('out', toolchain, 'obj/some')
            )
            self.assertEqual(
                label.gen_dir, ROOT.joinpath('out', toolchain, 'gen/some')
            )


class ResolvePathTest(unittest.TestCase):
    """Tests GN path resolution."""

    def test_resolve_absolute(self):
        self.assertEqual(TEST_PATHS.resolve('//'), TEST_PATHS.root)
        self.assertEqual(
            TEST_PATHS.resolve('//foo/bar'), TEST_PATHS.root / 'foo' / 'bar'
        )
        self.assertEqual(
            TEST_PATHS.resolve('//foo/../baz'), TEST_PATHS.root / 'baz'
        )

    def test_resolve_relative(self):
        self.assertEqual(TEST_PATHS.resolve(''), TEST_PATHS.cwd)
        self.assertEqual(TEST_PATHS.resolve('foo'), TEST_PATHS.cwd / 'foo')
        self.assertEqual(TEST_PATHS.resolve('..'), TEST_PATHS.root / 'some')


NINJA_EXECUTABLE = '''\
defines =
framework_dirs =
include_dirs = -I../fake_module/public
cflags = -g3 -Og -fdiagnostics-color -g -fno-common -Wall -Wextra -Werror
cflags_c =
cflags_cc = -fno-rtti -Wnon-virtual-dtor -std=c++17 -Wno-register
target_output_name = this_is_a_test

build fake_toolchain/obj/fake_module/fake_test.fake_test.cc.o: fake_toolchain_cxx ../fake_module/fake_test.cc
  source_file_dir = ../fake_module
  source_file_name = fake_test.cc

build fake_toolchain/obj/fake_module/fake_test.fake_test_c.c.o: fake_toolchain_cc ../fake_module/fake_test_c.c

build fake_toolchain/obj/fake_module/test/fake_test.elf fake_toolchain/obj/fake_module/test/fake_test.map: fake_toolchain_link fake_toolchain/obj/fake_module/fake_test.fake_test.cc.o fake_toolchain/obj/fake_module/fake_test.fake_test_c.c.o
  ldflags = -Og -fdiagnostics-color
  libs =
  frameworks =
  output_extension =
  output_dir = host_clang_debug/obj/fake_module/test
'''

_SOURCE_SET_TEMPLATE = '''\
defines =
framework_dirs =
include_dirs = -I../fake_module/public
cflags = -g3 -Og -fdiagnostics-color -g -fno-common -Wall -Wextra -Werror
cflags_c =
cflags_cc = -fno-rtti -Wnon-virtual-dtor -std=c++17 -Wno-register
target_output_name = this_is_a_test

build fake_toolchain/obj/fake_module/fake_source_set.file_a.cc.o: fake_toolchain_cxx ../fake_module/file_a.cc
  source_file_dir = ../fake_module
  source_file_name = file_a.cc

build fake_toolchain/obj/fake_module/fake_source_set.file_b.c.o: fake_toolchain_cc ../fake_module/file_b.c

build {path} fake_toolchain/obj/fake_module/fake_source_set.file_a.cc.o fake_toolchain/obj/fake_module/fake_source_set.file_b.c.o
  ldflags = -Og -fdiagnostics-color -Wno-error=deprecated
  libs =
  frameworks =
  output_extension =
  output_dir = host_clang_debug/obj/fake_module
'''

# GN originally used empty .stamp files to mark the completion of a group of
# dependencies. GN switched to using 'phony' Ninja targets instead, which don't
# require creating a new file.
_PHONY_BUILD_PATH = 'fake_toolchain/phony/fake_module/fake_source_set: phony'
_STAMP_BUILD_PATH = 'fake_toolchain/obj/fake_module/fake_source_set.stamp:'

NINJA_SOURCE_SET = _SOURCE_SET_TEMPLATE.format(path=_PHONY_BUILD_PATH)
NINJA_SOURCE_SET_STAMP = _SOURCE_SET_TEMPLATE.format(path=_STAMP_BUILD_PATH)


def _create_ninja_files(source_set: str) -> tuple:
    tempdir = tempfile.TemporaryDirectory(prefix='pw_build_test_')

    module = Path(tempdir.name, 'out', 'fake_toolchain', 'obj', 'fake_module')
    os.makedirs(module)
    module.joinpath('fake_test.ninja').write_text(NINJA_EXECUTABLE)
    module.joinpath('fake_source_set.ninja').write_text(source_set)
    module.joinpath('fake_no_objects.ninja').write_text('\n')

    outdir = Path(tempdir.name, 'out', 'fake_toolchain', 'obj', 'fake_module')

    paths = GnPaths(
        root=Path(tempdir.name),
        build=Path(tempdir.name, 'out'),
        cwd=Path(tempdir.name, 'some', 'module'),
        toolchain='//tools:fake_toolchain',
    )

    return tempdir, outdir, paths


class TargetTest(unittest.TestCase):
    """Tests querying GN target information."""

    def setUp(self):
        self._tempdir, self._outdir, self._paths = _create_ninja_files(
            NINJA_SOURCE_SET
        )

        self._rel_outdir = self._outdir.relative_to(self._paths.build)

    def tearDown(self):
        self._tempdir.cleanup()

    def test_source_set_artifact(self):
        target = TargetInfo(self._paths, '//fake_module:fake_source_set')
        self.assertTrue(target.generated)
        self.assertIsNone(target.artifact)

    def test_source_set_object_files(self):
        target = TargetInfo(self._paths, '//fake_module:fake_source_set')
        self.assertTrue(target.generated)
        self.assertEqual(
            set(target.object_files),
            {
                self._rel_outdir / 'fake_source_set.file_a.cc.o',
                self._rel_outdir / 'fake_source_set.file_b.c.o',
            },
        )

    def test_executable_object_files(self):
        target = TargetInfo(self._paths, '//fake_module:fake_test')
        self.assertEqual(
            set(target.object_files),
            {
                self._rel_outdir / 'fake_test.fake_test.cc.o',
                self._rel_outdir / 'fake_test.fake_test_c.c.o',
            },
        )

    def test_executable_artifact(self):
        target = TargetInfo(self._paths, '//fake_module:fake_test')
        self.assertEqual(
            target.artifact, self._rel_outdir / 'test' / 'fake_test.elf'
        )

    def test_non_existent_target(self):
        target = TargetInfo(
            self._paths, '//fake_module:definitely_not_a_real_target'
        )
        self.assertFalse(target.generated)
        self.assertIsNone(target.artifact)

    def test_non_existent_toolchain(self):
        target = TargetInfo(
            self._paths, '//fake_module:fake_source_set(//not_a:toolchain)'
        )
        self.assertFalse(target.generated)
        self.assertIsNone(target.artifact)


class StampTargetTest(TargetTest):
    """Test with old-style .stamp files instead of phony Ninja targets."""

    def setUp(self):
        self._tempdir, self._outdir, self._paths = _create_ninja_files(
            NINJA_SOURCE_SET_STAMP
        )

        self._rel_outdir = self._outdir.relative_to(self._paths.build)


class ExpandExpressionsTest(unittest.TestCase):
    """Tests expansion of expressions like <TARGET_FILE(//foo)>."""

    def setUp(self):
        self._tempdir, self._outdir, self._paths = _create_ninja_files(
            NINJA_SOURCE_SET
        )

    def tearDown(self):
        self._tempdir.cleanup()

    def _path(self, *segments: str, create: bool = False) -> str:
        path = Path(self._outdir, *segments)
        if create:
            os.makedirs(path.parent)
            path.touch()
        else:
            assert not path.exists()
        return str(path.relative_to(self._paths.build))

    def test_empty(self):
        self.assertEqual(list(expand_expressions(self._paths, '')), [''])

    def test_no_expressions(self):
        self.assertEqual(
            list(expand_expressions(self._paths, 'foobar')), ['foobar']
        )
        self.assertEqual(
            list(expand_expressions(self._paths, '<NOT_AN_EXPRESSION()>')),
            ['<NOT_AN_EXPRESSION()>'],
        )

    def test_incomplete_expression(self):
        for incomplete_expression in [
            '<TARGET_FILE(',
            '<TARGET_FILE(//foo)',
            '<TARGET_FILE(//foo>',
            '<TARGET_FILE(//foo) >',
            '--arg=<TARGET_FILE_IF_EXISTS(//foo) Hello>',
        ]:
            with self.assertRaises(ExpressionError):
                expand_expressions(self._paths, incomplete_expression)

    def test_target_file(self):
        path = self._path('test', 'fake_test.elf')

        for expr, expected in [
            ('<TARGET_FILE(//fake_module:fake_test)>', path),
            ('--arg=<TARGET_FILE(//fake_module:fake_test)>', f'--arg={path}'),
            (
                '--argument=<TARGET_FILE(//fake_module:fake_test)>;'
                '<TARGET_FILE(//fake_module:fake_test)>',
                f'--argument={path};{path}',
            ),
        ]:
            self.assertEqual(
                list(expand_expressions(self._paths, expr)), [expected]
            )

    def test_target_objects_no_target_file(self):
        with self.assertRaisesRegex(ExpressionError, 'no output file'):
            expand_expressions(
                self._paths, '<TARGET_FILE(//fake_module:fake_source_set)>'
            )

    def test_target_file_non_existent_target(self):
        with self.assertRaisesRegex(ExpressionError, 'generated'):
            expand_expressions(self._paths, '<TARGET_FILE(//not_real:abc123)>')

    def test_target_file_if_exists(self):
        path = self._path('test', 'fake_test.elf', create=True)

        for expr, expected in [
            ('<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>', path),
            (
                '--arg=<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>',
                f'--arg={path}',
            ),
            (
                '--argument=<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>;'
                '<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>',
                f'--argument={path};{path}',
            ),
        ]:
            self.assertEqual(
                list(expand_expressions(self._paths, expr)), [expected]
            )

    def test_target_file_if_exists_arg_omitted(self):
        for expr in [
            '<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>',
            '<TARGET_FILE_IF_EXISTS(//fake_module:fake_test(fake)>',
            '<TARGET_FILE_IF_EXISTS(//not_a_module:nothing)>',
            '--arg=<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>',
            '--argument=<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>;'
            '<TARGET_FILE_IF_EXISTS(//fake_module:fake_test)>',
        ]:
            self.assertEqual(list(expand_expressions(self._paths, expr)), [])

    def test_target_file_if_exists_error_if_never_has_artifact(self):
        for expr in [
            '<TARGET_FILE_IF_EXISTS(//fake_module:fake_source_set)>'
            'bar=<TARGET_FILE_IF_EXISTS(//fake_module:fake_source_set)>'
            '<TARGET_FILE_IF_EXISTS(//fake_module:fake_no_objects)>',
            '--foo=<TARGET_FILE_IF_EXISTS(//fake_module:fake_no_objects)>',
        ]:
            with self.assertRaises(ExpressionError):
                expand_expressions(self._paths, expr)

    def test_target_objects(self):
        self.assertEqual(
            set(
                expand_expressions(
                    self._paths,
                    '<TARGET_OBJECTS(//fake_module:fake_source_set)>',
                )
            ),
            {
                self._path('fake_source_set.file_a.cc.o'),
                self._path('fake_source_set.file_b.c.o'),
            },
        )
        self.assertEqual(
            set(
                expand_expressions(
                    self._paths, '<TARGET_OBJECTS(//fake_module:fake_test)>'
                )
            ),
            {
                self._path('fake_test.fake_test.cc.o'),
                self._path('fake_test.fake_test_c.c.o'),
            },
        )

    def test_target_objects_no_objects(self):
        self.assertEqual(
            list(
                expand_expressions(
                    self._paths,
                    '<TARGET_OBJECTS(//fake_module:fake_no_objects)>',
                )
            ),
            [],
        )

    def test_target_objects_other_content_in_arg(self):
        for arg in [
            '--foo=<TARGET_OBJECTS(//fake_module:fake_no_objects)>',
            '<TARGET_OBJECTS(//fake_module:fake_no_objects)>bar',
            '--foo<TARGET_OBJECTS(//fake_module:fake_no_objects)>bar',
            '<TARGET_OBJECTS(//fake_module:fake_no_objects)>'
            '<TARGET_OBJECTS(//fake_module:fake_no_objects)>',
            '<TARGET_OBJECTS(//fake_module:fake_source_set)>'
            '<TARGET_OBJECTS(//fake_module:fake_source_set)>',
        ]:
            with self.assertRaises(ExpressionError):
                expand_expressions(self._paths, arg)

    def test_target_objects_non_existent_target(self):
        with self.assertRaisesRegex(ExpressionError, 'generated'):
            expand_expressions(self._paths, '<TARGET_OBJECTS(//not_real)>')


class StampExpandExpressionsTest(TargetTest):
    """Test with old-style .stamp files instead of phony Ninja targets."""

    def setUp(self):
        self._tempdir, self._outdir, self._paths = _create_ninja_files(
            NINJA_SOURCE_SET_STAMP
        )

        self._rel_outdir = self._outdir.relative_to(self._paths.build)


if __name__ == '__main__':
    unittest.main()
