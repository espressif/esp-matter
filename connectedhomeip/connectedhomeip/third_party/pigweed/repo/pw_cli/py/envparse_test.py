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
"""Tests for pw_cli.envparse."""

import math
import unittest

from pw_cli import envparse

# pylint: disable=no-member


class ErrorError(Exception):
    pass


def error(value: str):
    raise ErrorError('error!')


class TestEnvironmentParser(unittest.TestCase):
    """Tests for envparse.EnvironmentParser."""

    def setUp(self):
        self.raw_env = {
            'PATH': '/bin:/usr/bin:/usr/local/bin',
            'FOO': '2020',
            'ReVeRsE': 'pigweed',
        }

        self.parser = envparse.EnvironmentParser(error_on_unrecognized=True)
        self.parser.add_var('PATH')
        self.parser.add_var('FOO', type=int)
        self.parser.add_var('BAR', type=bool)
        self.parser.add_var('BAZ', type=float, default=math.pi)
        self.parser.add_var('ReVeRsE', type=lambda s: s[::-1])
        self.parser.add_var('INT', type=int)
        self.parser.add_var('ERROR', type=error)

    def test_string_value(self):
        env = self.parser.parse_env(env=self.raw_env)
        self.assertEqual(env.PATH, self.raw_env['PATH'])

    def test_int_value(self):
        env = self.parser.parse_env(env=self.raw_env)
        self.assertEqual(env.FOO, 2020)

    def test_custom_value(self):
        env = self.parser.parse_env(env=self.raw_env)
        self.assertEqual(env.ReVeRsE, 'deewgip')

    def test_empty_value(self):
        env = self.parser.parse_env(env=self.raw_env)
        self.assertEqual(env.BAR, None)

    def test_default_value(self):
        env = self.parser.parse_env(env=self.raw_env)
        self.assertEqual(env.BAZ, math.pi)

    def test_unknown_key(self):
        env = self.parser.parse_env(env=self.raw_env)
        with self.assertRaises(AttributeError):
            env.BBBBB  # pylint: disable=pointless-statement

    def test_bad_value(self):
        raw_env = {**self.raw_env, 'INT': 'not an int'}
        with self.assertRaises(envparse.EnvironmentValueError) as ctx:
            self.parser.parse_env(env=raw_env)

        self.assertEqual(ctx.exception.variable, 'INT')
        self.assertIsInstance(ctx.exception.__cause__, ValueError)

    def test_custom_exception(self):
        raw_env = {**self.raw_env, 'ERROR': 'error'}
        with self.assertRaises(envparse.EnvironmentValueError) as ctx:
            self.parser.parse_env(env=raw_env)

        self.assertEqual(ctx.exception.variable, 'ERROR')
        self.assertIsInstance(ctx.exception.__cause__, ErrorError)


class TestEnvironmentParserWithPrefix(unittest.TestCase):
    """Tests for envparse.EnvironmentParser using a prefix."""

    def setUp(self):
        self.raw_env = {
            'PW_FOO': '001',
            'PW_BAR': '010',
            'PW_BAZ': '100',
            'IGNORED': '011',
        }

    def test_parse_unrecognized_variable(self):
        parser = envparse.EnvironmentParser(
            prefix='PW_', error_on_unrecognized=True
        )
        parser.add_var('PW_FOO')
        parser.add_var('PW_BAR')

        with self.assertRaises(ValueError):
            parser.parse_env(env=self.raw_env)

    def test_parse_unrecognized_but_allowed_suffix(self):
        parser = envparse.EnvironmentParser(
            prefix='PW_', error_on_unrecognized=True
        )
        parser.add_allowed_suffix('_ALLOWED_SUFFIX')

        env = parser.parse_env(env={'PW_FOO_ALLOWED_SUFFIX': '001'})
        self.assertEqual(env.PW_FOO_ALLOWED_SUFFIX, '001')

    def test_parse_allowed_suffix_but_not_suffix(self):
        parser = envparse.EnvironmentParser(
            prefix='PW_', error_on_unrecognized=True
        )
        parser.add_allowed_suffix('_ALLOWED_SUFFIX')

        with self.assertRaises(ValueError):
            parser.parse_env(env={'PW_FOO_ALLOWED_SUFFIX_FOO': '001'})

    def test_parse_ignore_unrecognized(self):
        parser = envparse.EnvironmentParser(
            prefix='PW_', error_on_unrecognized=False
        )
        parser.add_var('PW_FOO')
        parser.add_var('PW_BAR')

        env = parser.parse_env(env=self.raw_env)
        self.assertEqual(env.PW_FOO, self.raw_env['PW_FOO'])
        self.assertEqual(env.PW_BAR, self.raw_env['PW_BAR'])

    def test_add_var_without_prefix(self):
        parser = envparse.EnvironmentParser(
            prefix='PW_', error_on_unrecognized=True
        )
        with self.assertRaises(ValueError):
            parser.add_var('FOO')


class TestStrictBool(unittest.TestCase):
    """Tests for envparse.strict_bool."""

    def setUp(self):
        self.good_bools = ['true', '1', 'TRUE', 'tRuE']
        self.bad_bools = [
            '',
            'false',
            '0',
            'foo',
            '2',
            '999',
            'ok',
            'yes',
            'no',
        ]

    def test_good_bools(self):
        self.assertTrue(
            all(envparse.strict_bool(val) for val in self.good_bools)
        )

    def test_bad_bools(self):
        self.assertFalse(
            any(envparse.strict_bool(val) for val in self.bad_bools)
        )


if __name__ == '__main__':
    unittest.main()
