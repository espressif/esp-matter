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
"""Tests the trace module."""

import json
import struct
import unittest

from pw_trace import trace

test_events = [
    trace.TraceEvent(trace.TraceType.INSTANTANEOUS, "m1", "L1", 1),
    trace.TraceEvent(trace.TraceType.INSTANTANEOUS_GROUP, "m2", "L2", 2, "G2"),
    trace.TraceEvent(trace.TraceType.ASYNC_STEP, "m3", "L3", 3, "G3", 103),
    trace.TraceEvent(trace.TraceType.DURATION_START, "m4", "L4", 4),
    trace.TraceEvent(trace.TraceType.DURATION_GROUP_START, "m5", "L5", 5, "G5"),
    trace.TraceEvent(trace.TraceType.ASYNC_START, "m6", "L6", 6, "G6", 106),
    trace.TraceEvent(trace.TraceType.DURATION_END, "m7", "L7", 7),
    trace.TraceEvent(trace.TraceType.DURATION_GROUP_END, "m8", "L8", 8, "G8"),
    trace.TraceEvent(trace.TraceType.ASYNC_END, "m9", "L9", 9, "G9", 109),
]

test_json = [
    {"ph": "I", "pid": "m1", "name": "L1", "ts": 1, "s": "p"},
    {"ph": "I", "pid": "m2", "tid": "G2", "name": "L2", "ts": 2, "s": "t"},
    {
        "ph": "n",
        "pid": "m3",
        "tid": "G3",
        "name": "L3",
        "ts": 3,
        "scope": "G3",
        "cat": "m3",
        "id": 103,
        "args": {"id": 103},
    },
    {"ph": "B", "pid": "m4", "tid": "L4", "name": "L4", "ts": 4},
    {"ph": "B", "pid": "m5", "tid": "G5", "name": "L5", "ts": 5},
    {
        "ph": "b",
        "pid": "m6",
        "tid": "G6",
        "name": "L6",
        "ts": 6,
        "scope": "G6",
        "cat": "m6",
        "id": 106,
        "args": {"id": 106},
    },
    {"ph": "E", "pid": "m7", "tid": "L7", "name": "L7", "ts": 7},
    {"ph": "E", "pid": "m8", "tid": "G8", "name": "L8", "ts": 8},
    {
        "ph": "e",
        "pid": "m9",
        "tid": "G9",
        "name": "L9",
        "ts": 9,
        "scope": "G9",
        "cat": "m9",
        "id": 109,
        "args": {"id": 109},
    },
]


class TestTraceGenerateJson(unittest.TestCase):
    """Tests generate json with various events."""

    def test_generate_single_json_event(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS,
            module="module",
            label="label",
            timestamp_us=10,
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {"ph": "I", "pid": "module", "name": "label", "ts": 10, "s": "p"},
        )

    def test_generate_multiple_json_events(self):
        json_lines = trace.generate_trace_json(test_events)
        self.assertEqual(len(test_json), len(json_lines))
        for actual, expected in zip(json_lines, test_json):
            self.assertEqual(expected, json.loads(actual))

    def test_generate_json_data_arg_label(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS,
            module="module",
            label="",  # Is replaced by data string
            timestamp_us=10,
            has_data=True,
            data_fmt="@pw_arg_label",
            data=bytes("arg", "utf-8"),
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {"ph": "I", "pid": "module", "name": "arg", "ts": 10, "s": "p"},
        )

    def test_generate_json_data_arg_group(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS_GROUP,
            module="module",
            label="label",
            timestamp_us=10,
            has_data=True,
            data_fmt="@pw_arg_group",
            data=bytes("arg", "utf-8"),
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {
                "ph": "I",
                "pid": "module",
                "name": "label",
                "tid": "arg",
                "ts": 10,
                "s": "t",
            },
        )

    def test_generate_json_data_counter(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS,
            module="module",
            label="counter",
            timestamp_us=10,
            has_data=True,
            data_fmt="@pw_arg_counter",
            data=(5).to_bytes(4, byteorder="little"),
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {
                "ph": "C",
                "pid": "module",
                "name": "counter",
                "ts": 10,
                "s": "p",
                "args": {"counter": 5},
            },
        )

    def test_generate_json_data_struct_fmt_single(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS,
            module="module",
            label="counter",
            timestamp_us=10,
            has_data=True,
            data_fmt="@pw_py_struct_fmt:H",
            data=(5).to_bytes(2, byteorder="little"),
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {
                "ph": "I",
                "pid": "module",
                "name": "counter",
                "ts": 10,
                "s": "p",
                "args": {"data_0": 5},
            },
        )

    def test_generate_json_data_struct_fmt_multi(self):
        event = trace.TraceEvent(
            event_type=trace.TraceType.INSTANTANEOUS,
            module="module",
            label="counter",
            timestamp_us=10,
            has_data=True,
            data_fmt="@pw_py_struct_fmt:Hl",
            data=struct.pack("Hl", 5, 2),
        )
        json_lines = trace.generate_trace_json([event])
        self.assertEqual(1, len(json_lines))
        self.assertEqual(
            json.loads(json_lines[0]),
            {
                "ph": "I",
                "pid": "module",
                "name": "counter",
                "ts": 10,
                "s": "p",
                "args": {"data_0": 5, "data_1": 2},
            },
        )


if __name__ == '__main__':
    unittest.main()
