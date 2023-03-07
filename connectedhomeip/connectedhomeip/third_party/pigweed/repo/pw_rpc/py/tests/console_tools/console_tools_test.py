#!/usr/bin/env python3
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
"""Tests the pw_rpc.console_tools.console module."""

import types
from typing import Optional
import unittest

import pw_status

from pw_protobuf_compiler import python_protos
import pw_rpc
from pw_rpc import callback_client
from pw_rpc.console_tools.console import (
    CommandHelper,
    Context,
    ClientInfo,
    alias_deprecated_command,
)


class TestCommandHelper(unittest.TestCase):
    def setUp(self) -> None:
        self._commands = {'command_a': 'A', 'command_B': 'B'}
        self._variables = {'hello': 1, 'world': 2}
        self._helper = CommandHelper(
            self._commands, self._variables, 'The header', 'The footer'
        )

    def test_help_contents(self) -> None:
        help_contents = self._helper.help()

        self.assertTrue(help_contents.startswith('The header'))
        self.assertIn('The footer', help_contents)

        for var_name in self._variables:
            self.assertIn(var_name, help_contents)

        for cmd_name in self._commands:
            self.assertIn(cmd_name, help_contents)

    def test_repr_is_help(self):
        self.assertEqual(repr(self._helper), self._helper.help())


_PROTO = """\
syntax = "proto3";

package the.pkg;

message SomeMessage {
  uint32 magic_number = 1;

    message AnotherMessage {
      string payload = 1;
    }

}

service Service {
  rpc Unary(SomeMessage) returns (SomeMessage.AnotherMessage);
}
"""


class TestConsoleContext(unittest.TestCase):
    """Tests console_tools.console.Context."""

    def setUp(self) -> None:
        self._protos = python_protos.Library.from_strings(_PROTO)

        self._info = ClientInfo(
            'the_client',
            object(),
            pw_rpc.Client.from_modules(
                callback_client.Impl(),
                [
                    pw_rpc.Channel(1, lambda _: None),
                    pw_rpc.Channel(2, lambda _: None),
                ],
                self._protos.modules(),
            ),
        )

    def test_sets_expected_variables(self) -> None:
        variables = Context(
            [self._info], default_client=self._info.client, protos=self._protos
        ).variables()

        self.assertIn('set_target', variables)

        self.assertIsInstance(variables['help'], CommandHelper)
        self.assertIs(variables['python_help'], help)
        self.assertIs(pw_status.Status, variables['Status'])
        self.assertIs(self._info.client, variables['the_client'])

    def test_set_target_switches_between_clients(self) -> None:
        client_1_channel = self._info.rpc_client.channel(1).channel

        client_2_channel = pw_rpc.Channel(99, lambda _: None)
        info_2 = ClientInfo(
            'other_client',
            object(),
            pw_rpc.Client.from_modules(
                callback_client.Impl(),
                [client_2_channel],
                self._protos.modules(),
            ),
        )

        context = Context(
            [self._info, info_2],
            default_client=self._info.client,
            protos=self._protos,
        )

        # Make sure the RPC service switches from one client to the other.
        self.assertIs(
            context.variables()['the'].pkg.Service.Unary.channel,
            client_1_channel,
        )

        context.set_target(info_2.client)

        self.assertIs(
            context.variables()['the'].pkg.Service.Unary.channel,
            client_2_channel,
        )

    def test_default_client_must_be_in_clients(self) -> None:
        with self.assertRaises(ValueError):
            Context(
                [self._info],
                default_client='something else',
                protos=self._protos,
            )

    def test_set_target_invalid_channel(self) -> None:
        context = Context(
            [self._info], default_client=self._info.client, protos=self._protos
        )

        with self.assertRaises(KeyError):
            context.set_target(self._info.client, 100)

    def test_set_target_non_default_channel(self) -> None:
        channel_1 = self._info.rpc_client.channel(1).channel
        channel_2 = self._info.rpc_client.channel(2).channel

        context = Context(
            [self._info], default_client=self._info.client, protos=self._protos
        )
        variables = context.variables()

        self.assertIs(variables['the'].pkg.Service.Unary.channel, channel_1)

        context.set_target(self._info.client, 2)

        self.assertIs(variables['the'].pkg.Service.Unary.channel, channel_2)

        with self.assertRaises(KeyError):
            context.set_target(self._info.client, 100)

    def test_set_target_requires_client_object(self) -> None:
        context = Context(
            [self._info], default_client=self._info.client, protos=self._protos
        )

        with self.assertRaises(ValueError):
            context.set_target(self._info.rpc_client)

        context.set_target(self._info.client)

    def test_derived_context(self) -> None:
        called_derived_set_target = False

        class DerivedContext(Context):
            def set_target(
                self,
                unused_selected_client,
                unused_channel_id: Optional[int] = None,
            ) -> None:
                nonlocal called_derived_set_target
                called_derived_set_target = True

        variables = DerivedContext(
            client_info=[self._info],
            default_client=self._info.client,
            protos=self._protos,
        ).variables()
        variables['set_target'](self._info.client)
        self.assertTrue(called_derived_set_target)


class TestAliasDeprecatedCommand(unittest.TestCase):
    def test_wraps_command_to_new_package(self) -> None:
        variables = {'abc': types.SimpleNamespace(command=lambda: 123)}
        alias_deprecated_command(variables, 'xyz.one.two.three', 'abc.command')

        self.assertEqual(variables['xyz'].one.two.three(), 123)

    def test_wraps_command_to_existing_package(self) -> None:
        variables = {
            'abc': types.SimpleNamespace(NewCmd=lambda: 456),
            'one': types.SimpleNamespace(),
        }
        alias_deprecated_command(variables, 'one.two.OldCmd', 'abc.NewCmd')

        self.assertEqual(variables['one'].two.OldCmd(), 456)

    def test_error_if_new_command_does_not_exist(self) -> None:
        variables = {
            'abc': types.SimpleNamespace(),
            'one': types.SimpleNamespace(),
        }

        with self.assertRaises(AttributeError):
            alias_deprecated_command(variables, 'one.two.OldCmd', 'abc.NewCmd')


if __name__ == '__main__':
    unittest.main()
