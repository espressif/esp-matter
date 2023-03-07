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
"""Tests for pw_cli.plugins."""

from pathlib import Path
import sys
import tempfile
import types
from typing import Dict, Iterator
import unittest

from pw_cli import plugins


def _no_docstring() -> int:
    return 123


def _with_docstring() -> int:
    """This docstring is brought to you courtesy of Pigweed."""
    return 456


def _create_files(directory: str, files: Dict[str, str]) -> Iterator[Path]:
    for relative_path, contents in files.items():
        path = Path(directory) / relative_path
        path.parent.mkdir(exist_ok=True, parents=True)
        path.write_text(contents)
        yield path


class TestPlugin(unittest.TestCase):
    """Tests for plugins.Plugins."""

    def test_target_name_attribute(self) -> None:
        self.assertEqual(
            plugins.Plugin('abc', _no_docstring).target_name,
            f'{__name__}._no_docstring',
        )

    def test_target_name_no_name_attribute(self) -> None:
        has_no_name = 'no __name__'
        self.assertFalse(hasattr(has_no_name, '__name__'))

        self.assertEqual(
            plugins.Plugin('abc', has_no_name).target_name,
            '<unknown>.no __name__',
        )


_TEST_PLUGINS = {
    'TEST_PLUGINS': (
        f'test_plugin {__name__} _with_docstring\n'
        f'other_plugin {__name__} _no_docstring\n'
    ),
    'nested/in/dirs/TEST_PLUGINS': f'test_plugin {__name__} _no_docstring\n',
}


class TestPluginRegistry(unittest.TestCase):
    """Tests for plugins.Registry."""

    def setUp(self) -> None:
        self._registry = plugins.Registry(
            validator=plugins.callable_with_no_args
        )

    def test_register(self) -> None:
        self.assertIsNotNone(self._registry.register('a_plugin', _no_docstring))
        self.assertIs(self._registry['a_plugin'].target, _no_docstring)

    def test_register_by_name(self) -> None:
        self.assertIsNotNone(
            self._registry.register_by_name(
                'plugin_one', __name__, '_no_docstring'
            )
        )
        self.assertIsNotNone(
            self._registry.register('plugin_two', _no_docstring)
        )

        self.assertIs(self._registry['plugin_one'].target, _no_docstring)
        self.assertIs(self._registry['plugin_two'].target, _no_docstring)

    def test_register_by_name_undefined_module(self) -> None:
        with self.assertRaisesRegex(plugins.Error, 'No module named'):
            self._registry.register_by_name(
                'plugin_two', 'not a module', 'something'
            )

    def test_register_by_name_undefined_function(self) -> None:
        with self.assertRaisesRegex(plugins.Error, 'does not exist'):
            self._registry.register_by_name('plugin_two', __name__, 'nothing')

    def test_register_fails_validation(self) -> None:
        with self.assertRaisesRegex(plugins.Error, 'must be callable'):
            self._registry.register('plugin_two', 'not function')

    def test_run_with_argv_sets_sys_argv(self) -> None:
        def stash_argv() -> int:
            self.assertEqual(['pw go', '1', '2'], sys.argv)
            return 1

        self.assertIsNotNone(self._registry.register('go', stash_argv))

        original_argv = sys.argv
        self.assertEqual(self._registry.run_with_argv('go', ['1', '2']), 1)
        self.assertIs(sys.argv, original_argv)

    def test_run_with_argv_registered_plugin(self) -> None:
        with self.assertRaises(KeyError):
            self._registry.run_with_argv('plugin_one', [])

    def test_register_cannot_overwrite(self) -> None:
        self.assertIsNotNone(self._registry.register('foo', lambda: None))
        self.assertIsNotNone(
            self._registry.register_by_name('bar', __name__, '_no_docstring')
        )

        with self.assertRaises(plugins.Error):
            self._registry.register('foo', lambda: None)

        with self.assertRaises(plugins.Error):
            self._registry.register('bar', lambda: None)

    def test_register_directory_innermost_takes_priority(self) -> None:
        with tempfile.TemporaryDirectory() as tempdir:
            paths = list(_create_files(tempdir, _TEST_PLUGINS))
            self._registry.register_directory(paths[1].parent, 'TEST_PLUGINS')

        self.assertEqual(self._registry.run_with_argv('test_plugin', []), 123)

    def test_register_directory_only_searches_up(self) -> None:
        with tempfile.TemporaryDirectory() as tempdir:
            paths = list(_create_files(tempdir, _TEST_PLUGINS))
            self._registry.register_directory(paths[0].parent, 'TEST_PLUGINS')

        self.assertEqual(self._registry.run_with_argv('test_plugin', []), 456)

    def test_register_directory_with_restriction(self) -> None:
        with tempfile.TemporaryDirectory() as tempdir:
            paths = list(_create_files(tempdir, _TEST_PLUGINS))
            self._registry.register_directory(
                paths[0].parent, 'TEST_PLUGINS', Path(tempdir, 'nested', 'in')
            )

        self.assertNotIn('other_plugin', self._registry)

    def test_register_same_file_multiple_times_no_error(self) -> None:
        with tempfile.TemporaryDirectory() as tempdir:
            paths = list(_create_files(tempdir, _TEST_PLUGINS))
            self._registry.register_file(paths[0])
            self._registry.register_file(paths[0])
            self._registry.register_file(paths[0])

        self.assertIs(self._registry['test_plugin'].target, _with_docstring)

    def test_help_uses_function_or_module_docstring(self) -> None:
        self.assertIsNotNone(self._registry.register('a', _no_docstring))
        self.assertIsNotNone(self._registry.register('b', _with_docstring))

        self.assertIn(__doc__, '\n'.join(self._registry.detailed_help(['a'])))

        self.assertNotIn(
            __doc__, '\n'.join(self._registry.detailed_help(['b']))
        )
        self.assertIn(
            _with_docstring.__doc__,
            '\n'.join(self._registry.detailed_help(['b'])),
        )

    def test_empty_string_if_no_help(self) -> None:
        fake_module_name = f'{__name__}.fake_module_for_test{id(self)}'
        fake_module = types.ModuleType(fake_module_name)
        self.assertIsNone(fake_module.__doc__)

        sys.modules[fake_module_name] = fake_module

        try:

            function = lambda: None
            function.__module__ = fake_module_name
            self.assertIsNotNone(self._registry.register('a', function))

            self.assertEqual(self._registry['a'].help(full=False), '')
            self.assertEqual(self._registry['a'].help(full=True), '')
        finally:
            del sys.modules[fake_module_name]

    def test_decorator_not_called(self) -> None:
        @self._registry.plugin
        def nifty() -> None:
            pass

        self.assertEqual(self._registry['nifty'].target, nifty)

    def test_decorator_called_no_args(self) -> None:
        @self._registry.plugin()
        def nifty() -> None:
            pass

        self.assertEqual(self._registry['nifty'].target, nifty)

    def test_decorator_called_with_args(self) -> None:
        @self._registry.plugin(name='nifty')
        def my_nifty_keen_plugin() -> None:
            pass

        self.assertEqual(self._registry['nifty'].target, my_nifty_keen_plugin)


if __name__ == '__main__':
    unittest.main()
