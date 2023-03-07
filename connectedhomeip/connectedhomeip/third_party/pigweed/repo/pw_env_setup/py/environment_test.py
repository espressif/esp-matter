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
"""Tests for env_setup.environment.

This tests the error-checking, context manager, and written environment scripts
of the Environment class.

Tests that end in "_ctx" modify the environment and validate it in-process.

Tests that end in "_written" write the environment to a file intended to be
evaluated by the shell, then launches the shell and then saves the environment.
This environment is then validated in the test process.
"""

import logging
import os
import subprocess
import tempfile
import unittest

import six

from pw_env_setup import environment

# pylint: disable=super-with-arguments


class WrittenEnvFailure(Exception):
    pass


def _evaluate_env_in_shell(env):
    """Write env to a file then evaluate and save the resulting environment.

    Write env to a file, then launch a shell command that sources that file
    and dumps the environment to stdout. Parse that output into a dict and
    return it.

    Args:
      env(environment.Environment): environment to write out

    Returns dictionary of resulting environment.
    """

    # Write env sourcing script to file.
    with tempfile.NamedTemporaryFile(
        prefix='pw-test-written-env-',
        suffix='.bat' if os.name == 'nt' else '.sh',
        delete=False,
        mode='w+',
    ) as temp:
        env.write(temp)
        temp_name = temp.name

    # Evaluate env sourcing script and capture output of 'env'.
    if os.name == 'nt':
        # On Windows you just run batch files and they modify your
        # environment, no need to call 'source' or '.'.
        cmd = '{} && set'.format(temp_name)
    else:
        # Using '.' instead of 'source' because 'source' is not POSIX.
        cmd = '. {} && env'.format(temp_name)

    res = subprocess.run(cmd, capture_output=True, shell=True)
    if res.returncode:
        raise WrittenEnvFailure(res.stderr)

    # Parse environment from stdout of subprocess.
    env_ret = {}
    for line in res.stdout.splitlines():
        line = line.decode()

        # Some people inexplicably have newlines in some of their
        # environment variables. This module does not allow that so we can
        # ignore any such extra lines.
        if '=' not in line:
            continue

        var, value = line.split('=', 1)
        env_ret[var] = value

    return env_ret


# pylint: disable=too-many-public-methods
class EnvironmentTest(unittest.TestCase):
    """Tests for env_setup.environment."""

    def setUp(self):
        self.env = environment.Environment()

        # Name of a variable that is already set when the test starts.
        self.var_already_set = self.env.normalize_key('var_already_set')
        os.environ[self.var_already_set] = 'orig value'
        self.assertIn(self.var_already_set, os.environ)

        # Name of a variable that is not set when the test starts.
        self.var_not_set = self.env.normalize_key('var_not_set')
        if self.var_not_set in os.environ:
            del os.environ[self.var_not_set]
        self.assertNotIn(self.var_not_set, os.environ)

        self.orig_env = os.environ.copy()

    def tearDown(self):
        self.assertEqual(os.environ, self.orig_env)

    def test_set_notpresent_ctx(self):
        self.env.set(self.var_not_set, '1')
        with self.env(export=False) as env:
            self.assertIn(self.var_not_set, env)
            self.assertEqual(env[self.var_not_set], '1')

    def test_set_notpresent_written(self):
        self.env.set(self.var_not_set, '1')
        env = _evaluate_env_in_shell(self.env)
        self.assertIn(self.var_not_set, env)
        self.assertEqual(env[self.var_not_set], '1')

    def test_set_present_ctx(self):
        self.env.set(self.var_already_set, '1')
        with self.env(export=False) as env:
            self.assertIn(self.var_already_set, env)
            self.assertEqual(env[self.var_already_set], '1')

    def test_set_present_written(self):
        self.env.set(self.var_already_set, '1')
        env = _evaluate_env_in_shell(self.env)
        self.assertIn(self.var_already_set, env)
        self.assertEqual(env[self.var_already_set], '1')

    def test_clear_notpresent_ctx(self):
        self.env.clear(self.var_not_set)
        with self.env(export=False) as env:
            self.assertNotIn(self.var_not_set, env)

    def test_clear_notpresent_written(self):
        self.env.clear(self.var_not_set)
        env = _evaluate_env_in_shell(self.env)
        self.assertNotIn(self.var_not_set, env)

    def test_clear_present_ctx(self):
        self.env.clear(self.var_already_set)
        with self.env(export=False) as env:
            self.assertNotIn(self.var_already_set, env)

    def test_clear_present_written(self):
        self.env.clear(self.var_already_set)
        env = _evaluate_env_in_shell(self.env)
        self.assertNotIn(self.var_already_set, env)

    def test_value_replacement(self):
        self.env.set(self.var_not_set, '/foo/bar/baz')
        self.env.add_replacement('FOOBAR', '/foo/bar')
        buf = six.StringIO()
        self.env.write(buf)
        assert '/foo/bar' not in buf.getvalue()

    def test_variable_replacement(self):
        self.env.set('FOOBAR', '/foo/bar')
        self.env.set(self.var_not_set, '/foo/bar/baz')
        self.env.add_replacement('FOOBAR')
        buf = six.StringIO()
        self.env.write(buf)
        print(buf.getvalue())
        assert '/foo/bar/baz' not in buf.getvalue()

    def test_nonglobal(self):
        self.env.set(self.var_not_set, '1')
        with self.env(export=False) as env:
            self.assertIn(self.var_not_set, env)
            self.assertNotIn(self.var_not_set, os.environ)

    def test_global(self):
        self.env.set(self.var_not_set, '1')
        with self.env(export=True) as env:
            self.assertIn(self.var_not_set, env)
            self.assertIn(self.var_not_set, os.environ)

    def test_set_badnametype(self):
        with self.assertRaises(environment.BadNameType):
            self.env.set(123, '123')

    def test_set_badvaluetype(self):
        with self.assertRaises(environment.BadValueType):
            self.env.set('var', 123)

    def test_prepend_badnametype(self):
        with self.assertRaises(environment.BadNameType):
            self.env.prepend(123, '123')

    def test_prepend_badvaluetype(self):
        with self.assertRaises(environment.BadValueType):
            self.env.prepend('var', 123)

    def test_append_badnametype(self):
        with self.assertRaises(environment.BadNameType):
            self.env.append(123, '123')

    def test_append_badvaluetype(self):
        with self.assertRaises(environment.BadValueType):
            self.env.append('var', 123)

    def test_set_badname_empty(self):
        with self.assertRaises(environment.BadVariableName):
            self.env.set('', '123')

    def test_set_badname_digitstart(self):
        with self.assertRaises(environment.BadVariableName):
            self.env.set('123', '123')

    def test_set_badname_equals(self):
        with self.assertRaises(environment.BadVariableName):
            self.env.set('foo=bar', '123')

    def test_set_badname_period(self):
        with self.assertRaises(environment.BadVariableName):
            self.env.set('abc.def', '123')

    def test_set_badname_hyphen(self):
        with self.assertRaises(environment.BadVariableName):
            self.env.set('abc-def', '123')

    def test_set_empty_value(self):
        with self.assertRaises(environment.EmptyValue):
            self.env.set('var', '')

    def test_set_newline_in_value(self):
        with self.assertRaises(environment.NewlineInValue):
            self.env.set('var', '123\n456')

    def test_equal_sign_in_value(self):
        with self.assertRaises(environment.BadVariableValue):
            self.env.append(self.var_already_set, 'pa=th')


class _PrependAppendEnvironmentTest(unittest.TestCase):
    """Tests for env_setup.environment."""

    def __init__(self, *args, **kwargs):
        windows = kwargs.pop('windows', False)
        pathsep = kwargs.pop('pathsep', os.pathsep)
        allcaps = kwargs.pop('allcaps', False)
        super(_PrependAppendEnvironmentTest, self).__init__(*args, **kwargs)
        self.windows = windows
        self.pathsep = pathsep
        self.allcaps = allcaps

        # If we're testing Windows behavior and actually running on Windows,
        # actually launch a subprocess to evaluate the shell init script.
        # Likewise if we're testing POSIX behavior and actually on a POSIX
        # system. Tests can check self.run_shell_tests and exit without
        # doing anything.
        real_windows = os.name == 'nt'
        self.run_shell_tests = self.windows == real_windows

    def setUp(self):
        self.env = environment.Environment(
            windows=self.windows, pathsep=self.pathsep, allcaps=self.allcaps
        )

        self.var_already_set = self.env.normalize_key('VAR_ALREADY_SET')
        os.environ[self.var_already_set] = self.pathsep.join(
            'one two three'.split()
        )
        self.assertIn(self.var_already_set, os.environ)

        self.var_not_set = self.env.normalize_key('VAR_NOT_SET')
        if self.var_not_set in os.environ:
            del os.environ[self.var_not_set]
        self.assertNotIn(self.var_not_set, os.environ)

        self.orig_env = os.environ.copy()

    def split(self, val):
        return val.split(self.pathsep)

    def tearDown(self):
        self.assertEqual(os.environ, self.orig_env)


# TODO(mohrr) remove disable=useless-object-inheritance once in Python 3.
# pylint: disable=useless-object-inheritance
class _AppendPrependTestMixin(object):
    def test_prepend_present_ctx(self):
        orig = os.environ[self.var_already_set]
        self.env.prepend(self.var_already_set, 'path')
        with self.env(export=False) as env:
            self.assertEqual(
                env[self.var_already_set], self.pathsep.join(('path', orig))
            )

    def test_prepend_present_written(self):
        if not self.run_shell_tests:
            return

        orig = os.environ[self.var_already_set]
        self.env.prepend(self.var_already_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(
            env[self.var_already_set], self.pathsep.join(('path', orig))
        )

    def test_prepend_notpresent_ctx(self):
        self.env.prepend(self.var_not_set, 'path')
        with self.env(export=False) as env:
            self.assertEqual(env[self.var_not_set], 'path')

    def test_prepend_notpresent_written(self):
        if not self.run_shell_tests:
            return

        self.env.prepend(self.var_not_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(env[self.var_not_set], 'path')

    def test_append_present_ctx(self):
        orig = os.environ[self.var_already_set]
        self.env.append(self.var_already_set, 'path')
        with self.env(export=False) as env:
            self.assertEqual(
                env[self.var_already_set], self.pathsep.join((orig, 'path'))
            )

    def test_append_present_written(self):
        if not self.run_shell_tests:
            return

        orig = os.environ[self.var_already_set]
        self.env.append(self.var_already_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(
            env[self.var_already_set], self.pathsep.join((orig, 'path'))
        )

    def test_append_notpresent_ctx(self):
        self.env.append(self.var_not_set, 'path')
        with self.env(export=False) as env:
            self.assertEqual(env[self.var_not_set], 'path')

    def test_append_notpresent_written(self):
        if not self.run_shell_tests:
            return

        self.env.append(self.var_not_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(env[self.var_not_set], 'path')

    def test_remove_ctx(self):
        self.env.set(
            self.var_not_set,
            self.pathsep.join(('path', 'one', 'path', 'two', 'path')),
        )

        self.env.append(self.var_not_set, 'path')
        with self.env(export=False) as env:
            self.assertEqual(
                env[self.var_not_set], self.pathsep.join(('one', 'two', 'path'))
            )

    def test_remove_written(self):
        if not self.run_shell_tests:
            return

        if self.windows:
            return

        self.env.set(
            self.var_not_set,
            self.pathsep.join(('path', 'one', 'path', 'two', 'path')),
        )

        self.env.append(self.var_not_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(
            env[self.var_not_set], self.pathsep.join(('one', 'two', 'path'))
        )

    def test_remove_ctx_space(self):
        self.env.set(
            self.var_not_set,
            self.pathsep.join(('pa th', 'one', 'pa th', 'two')),
        )

        self.env.append(self.var_not_set, 'pa th')
        with self.env(export=False) as env:
            self.assertEqual(
                env[self.var_not_set],
                self.pathsep.join(('one', 'two', 'pa th')),
            )

    def test_remove_written_space(self):
        if not self.run_shell_tests:
            return

        if self.windows:
            return

        self.env.set(
            self.var_not_set,
            self.pathsep.join(('pa th', 'one', 'pa th', 'two')),
        )

        self.env.append(self.var_not_set, 'pa th')
        env = _evaluate_env_in_shell(self.env)
        self.assertEqual(
            env[self.var_not_set], self.pathsep.join(('one', 'two', 'pa th'))
        )

    def test_remove_ctx_empty(self):
        self.env.remove(self.var_not_set, 'path')
        with self.env(export=False) as env:
            self.assertNotIn(self.var_not_set, env)

    def test_remove_written_empty(self):
        if not self.run_shell_tests:
            return

        self.env.remove(self.var_not_set, 'path')
        env = _evaluate_env_in_shell(self.env)
        self.assertNotIn(self.var_not_set, env)


class WindowsEnvironmentTest(
    _PrependAppendEnvironmentTest, _AppendPrependTestMixin
):
    def __init__(self, *args, **kwargs):
        kwargs['pathsep'] = ';'
        kwargs['windows'] = True
        kwargs['allcaps'] = True
        super(WindowsEnvironmentTest, self).__init__(*args, **kwargs)


class PosixEnvironmentTest(
    _PrependAppendEnvironmentTest, _AppendPrependTestMixin
):
    def __init__(self, *args, **kwargs):
        kwargs['pathsep'] = ':'
        kwargs['windows'] = False
        kwargs['allcaps'] = False
        super(PosixEnvironmentTest, self).__init__(*args, **kwargs)
        self.real_windows = os.name == 'nt'


class WindowsCaseInsensitiveTest(unittest.TestCase):
    def test_lower_handling(self):
        # This is only for testing case-handling on Windows. It doesn't make
        # sense to run it on other systems.
        if os.name != 'nt':
            return

        lower_var = 'lower_var'
        upper_var = lower_var.upper()

        if upper_var in os.environ:
            del os.environ[upper_var]

        self.assertNotIn(lower_var, os.environ)

        env = environment.Environment()
        env.append(lower_var, 'foo')
        env.append(upper_var, 'bar')
        with env(export=False) as env_:
            self.assertNotIn(lower_var, env_)
            self.assertIn(upper_var, env_)
            self.assertEqual(env_[upper_var], 'foo;bar')


if __name__ == '__main__':
    import sys

    logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
    unittest.main()
