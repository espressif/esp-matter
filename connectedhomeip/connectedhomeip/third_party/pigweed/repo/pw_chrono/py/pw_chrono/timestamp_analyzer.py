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
"""Library to analyze timestamp."""

from typing import List
import datetime
from pw_chrono_protos import chrono_pb2

_UTC_EPOCH = datetime.datetime(1970, 1, 1, 00, 00, 00)

_UNKNOWN = chrono_pb2.EpochType.Enum.UNKNOWN
_TIME_SINCE_BOOT = chrono_pb2.EpochType.Enum.TIME_SINCE_BOOT
_UTC_WALL_CLOCK = chrono_pb2.EpochType.Enum.UTC_WALL_CLOCK


def process_snapshot(serialized_snapshot: bytes):
    captured_timestamps = chrono_pb2.SnapshotTimestamps()
    captured_timestamps.ParseFromString(serialized_snapshot)
    return timestamp_output(captured_timestamps)


def timestamp_output(timestamps: chrono_pb2.SnapshotTimestamps):
    output: List[str] = []
    if not timestamps.timestamps:
        return ''

    plural = '' if len(timestamps.timestamps) == 1 else 's'
    output.append(f'Snapshot capture timestamp{plural}')
    for timepoint in timestamps.timestamps:
        time = timestamp_snapshot_analyzer(timepoint)
        clock_epoch_type = timepoint.clock_parameters.epoch_type
        if clock_epoch_type == _TIME_SINCE_BOOT:
            output.append(f'  Time since boot:   {time}')
        elif clock_epoch_type == _UTC_WALL_CLOCK:
            utc_time = time + _UTC_EPOCH
            output.append(f'  UTC time:   {utc_time}')
        else:
            output.append(f'  Time since unknown epoch {_UNKNOWN}:   unknown')

    return '\n'.join(output)


def timestamp_snapshot_analyzer(
    captured_timepoint: chrono_pb2.TimePoint,
) -> datetime.timedelta:
    ticks = captured_timepoint.timestamp
    clock_period = (
        captured_timepoint.clock_parameters.tick_period_seconds_numerator
        / captured_timepoint.clock_parameters.tick_period_seconds_denominator
    )
    elapsed_seconds = ticks * clock_period

    time_delta = datetime.timedelta(seconds=elapsed_seconds)

    return time_delta
