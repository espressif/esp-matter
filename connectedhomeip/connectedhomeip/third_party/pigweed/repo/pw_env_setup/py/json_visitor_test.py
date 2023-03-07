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

import json
import unittest

import six

from pw_env_setup import environment, json_visitor


# pylint: disable=super-with-arguments
class JSONVisitorTest(unittest.TestCase):
    """Tests for env_setup.json_visitor."""

    def setUp(self):
        self.env = environment.Environment()

    def _write_and_parse_json(self):
        buf = six.StringIO()
        json_visitor.JSONVisitor(self.env, buf)
        return json.loads(buf.getvalue())

    def _assert_json(self, value):
        self.assertEqual(self._write_and_parse_json(), value)

    def test_set(self):
        self.env.clear('VAR')
        self.env.set('VAR', '1')
        self._assert_json({'set': {'VAR': '1'}})

    def test_clear(self):
        self.env.set('VAR', '1')
        self.env.clear('VAR')
        self._assert_json({'set': {'VAR': None}})

    def test_append(self):
        self.env.append('VAR', 'path1')
        self.env.append('VAR', 'path2')
        self.env.append('VAR', 'path3')
        self._assert_json(
            {'modify': {'VAR': {'append': 'path1 path2 path3'.split()}}}
        )

    def test_prepend(self):
        self.env.prepend('VAR', 'path1')
        self.env.prepend('VAR', 'path2')
        self.env.prepend('VAR', 'path3')
        self._assert_json(
            {'modify': {'VAR': {'prepend': 'path3 path2 path1'.split()}}}
        )

    def test_remove(self):
        self.env.remove('VAR', 'path1')
        self._assert_json({'modify': {'VAR': {'remove': ['path1']}}})

    def test_echo(self):
        self.env.echo('echo')
        self._assert_json({})

    def test_comment(self):
        self.env.comment('comment')
        self._assert_json({})

    def test_command(self):
        self.env.command('command')
        self._assert_json({})

    def test_doctor(self):
        self.env.doctor()
        self._assert_json({})

    def test_function(self):
        self.env.function('name', 'body')
        self._assert_json({})
