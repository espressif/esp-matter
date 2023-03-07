#!/usr/bin/env python3
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
"""Tests for the timestamp analyzer."""

import unittest
from pw_chrono.timestamp_analyzer import process_snapshot
from pw_chrono_protos import chrono_pb2


class TimestampTest(unittest.TestCase):
    """Test for the timestamp analyzer."""

    def test_no_timepoint(self):
        time_stamps = chrono_pb2.SnapshotTimestamps()
        self.assertEqual('', str(process_snapshot(time_stamps)))

    def test_timestamp_unknown_epoch_type(self):
        time_stamps = chrono_pb2.SnapshotTimestamps()

        time_point = chrono_pb2.TimePoint()
        unkown = chrono_pb2.EpochType.Enum.UNKNOWN
        time_point.clock_parameters.epoch_type = unkown

        time_stamps.timestamps.append(time_point)

        expected = '\n'.join(
            (
                'Snapshot capture timestamp',
                '    Time since unknown epoch 0:   unknown',
            )
        )

        self.assertEqual(expected, str(process_snapshot(time_stamps)))

    def test_timestamp_with_time_since_boot(self):
        time_stamps = chrono_pb2.SnapshotTimestamps()

        time_point = chrono_pb2.TimePoint()
        time_since_boot = chrono_pb2.EpochType.Enum.TIME_SINCE_BOOT
        time_point.clock_parameters.epoch_type = time_since_boot
        time_point.timestamp = 100
        time_point.clock_parameters.tick_period_seconds_numerator = 1
        time_point.clock_parameters.tick_period_seconds_denominator = 1000

        time_stamps.timestamps.append(time_point)

        expected = '\n'.join(
            ('Snapshot capture timestamp', '  Time since boot:   2:24:00')
        )

        self.assertEqual(expected, str(process_snapshot(time_stamps)))

    def test_timestamp_with_utc_wall_clock(self):
        time_stamps = chrono_pb2.SnapshotTimestamps()

        time_point = chrono_pb2.TimePoint()
        utc_wall_clock = chrono_pb2.EpochType.Enum.UTC_WALL_CLOCK
        time_point.clock_parameters.epoch_type = utc_wall_clock
        time_point.timestamp = 100
        time_point.clock_parameters.tick_period_seconds_numerator = 1
        time_point.clock_parameters.tick_period_seconds_denominator = 1000

        time_stamps.timestamps.append(time_point)

        expected = '\n'.join(
            ('Snapshot capture timestamp', '  UTC time:   1970-01-01 02:24:00')
        )

        self.assertEqual(expected, str(process_snapshot(time_stamps)))

    def test_timestamp_with_time_since_boot_and_utc_wall_clock(self):
        time_stamps = chrono_pb2.SnapshotTimestamps()

        time_point = chrono_pb2.TimePoint()
        time_since_boot = chrono_pb2.EpochType.Enum.TIME_SINCE_BOOT
        time_point.clock_parameters.epoch_type = time_since_boot
        time_point.timestamp = 100
        time_point.clock_parameters.tick_period_seconds_numerator = 1
        time_point.clock_parameters.tick_period_seconds_denominator = 1000
        time_stamps.timestamps.append(time_point)

        time_point = chrono_pb2.TimePoint()
        utc_wall_clock = chrono_pb2.EpochType.Enum.UTC_WALL_CLOCK
        time_point.clock_parameters.epoch_type = utc_wall_clock
        time_point.timestamp = 100
        time_point.clock_parameters.tick_period_seconds_numerator = 1
        time_point.clock_parameters.tick_period_seconds_denominator = 1000
        time_stamps.timestamps.append(time_point)

        expected = '\n'.join(
            (
                'Snapshot capture timestamps',
                '  Time since boot:   2:24:00',
                '  UTC time:   1970-01-01 02:24:00',
            )
        )

        self.assertEqual(expected, str(process_snapshot(time_stamps)))
