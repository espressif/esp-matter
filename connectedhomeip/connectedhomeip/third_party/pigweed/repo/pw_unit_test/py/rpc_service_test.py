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
"""Tests using the callback client for pw_rpc."""

import logging
from pathlib import Path
from typing import List, Tuple
import unittest
from unittest import mock

from pw_hdlc import rpc
from pw_rpc import testing
from pw_unit_test_proto import unit_test_pb2
from pw_unit_test import run_tests, EventHandler, TestCase
from pw_status import Status

# The three suites (Passing, Failing, and DISABLED_Disabled) have these cases.
_CASES = ('Zero', 'One', 'Two', 'DISABLED_Disabled')
_FILE = 'pw_unit_test/test_rpc_server.cc'

PASSING = tuple(TestCase('Passing', case, _FILE) for case in _CASES[:-1])
FAILING = tuple(TestCase('Failing', case, _FILE) for case in _CASES[:-1])
EXECUTED_TESTS = PASSING + FAILING

DISABLED_SUITE = tuple(
    TestCase('DISABLED_Disabled', case, _FILE) for case in _CASES
)

ALL_DISABLED_TESTS = (
    TestCase('Passing', 'DISABLED_Disabled', _FILE),
    TestCase('Failing', 'DISABLED_Disabled', _FILE),
    *DISABLED_SUITE,
)


class RpcIntegrationTest(unittest.TestCase):
    """Calls RPCs on an RPC server through a socket."""

    test_server_command: Tuple[str, ...] = ()
    port: int

    def setUp(self) -> None:
        self._context = rpc.HdlcRpcLocalServerAndClient(
            self.test_server_command, self.port, [unit_test_pb2]
        )
        self.rpcs = self._context.client.channel(1).rpcs
        self.handler = mock.NonCallableMagicMock(spec=EventHandler)

    def tearDown(self) -> None:
        self._context.close()

    def test_run_tests_default_handler(self) -> None:
        with self.assertLogs(logging.getLogger('pw_unit_test'), 'INFO') as logs:
            self.assertFalse(run_tests(self.rpcs))

        for test in EXECUTED_TESTS:
            self.assertTrue(any(str(test) in log for log in logs.output), test)

    def test_run_tests_calls_test_case_start(self) -> None:
        self.assertFalse(run_tests(self.rpcs, event_handlers=[self.handler]))

        self.handler.test_case_start.assert_has_calls(
            [mock.call(case) for case in EXECUTED_TESTS], any_order=True
        )

    def test_run_tests_calls_test_case_end(self) -> None:
        self.assertFalse(run_tests(self.rpcs, event_handlers=[self.handler]))

        calls = [
            mock.call(
                case,
                unit_test_pb2.SUCCESS
                if case.suite_name == 'Passing'
                else unit_test_pb2.FAILURE,
            )
            for case in EXECUTED_TESTS
        ]
        self.handler.test_case_end.assert_has_calls(calls, any_order=True)

    def test_run_tests_calls_test_case_disabled(self) -> None:
        self.assertFalse(run_tests(self.rpcs, event_handlers=[self.handler]))

        self.handler.test_case_disabled.assert_has_calls(
            [mock.call(case) for case in ALL_DISABLED_TESTS], any_order=True
        )

    def test_passing_tests_only(self) -> None:
        self.assertTrue(
            run_tests(
                self.rpcs,
                test_suites=['Passing'],
                event_handlers=[self.handler],
            )
        )
        calls = [mock.call(case, unit_test_pb2.SUCCESS) for case in PASSING]
        self.handler.test_case_end.assert_has_calls(calls, any_order=True)

    def test_disabled_tests_only(self) -> None:
        self.assertTrue(
            run_tests(
                self.rpcs,
                test_suites=['DISABLED_Disabled'],
                event_handlers=[self.handler],
            )
        )

        self.handler.test_case_start.assert_not_called()
        self.handler.test_case_end.assert_not_called()
        self.handler.test_case_disabled.assert_has_calls(
            [mock.call(case) for case in DISABLED_SUITE], any_order=True
        )

    def test_failing_tests(self) -> None:
        self.assertFalse(
            run_tests(
                self.rpcs,
                test_suites=['Failing'],
                event_handlers=[self.handler],
            )
        )
        calls = [mock.call(case, unit_test_pb2.FAILURE) for case in FAILING]
        self.handler.test_case_end.assert_has_calls(calls, any_order=True)


def _main(
    test_server_command: List[str], port: int, unittest_args: List[str]
) -> None:
    RpcIntegrationTest.test_server_command = tuple(test_server_command)
    RpcIntegrationTest.port = port
    unittest.main(argv=unittest_args)


if __name__ == '__main__':
    _main(**vars(testing.parse_test_server_args()))
