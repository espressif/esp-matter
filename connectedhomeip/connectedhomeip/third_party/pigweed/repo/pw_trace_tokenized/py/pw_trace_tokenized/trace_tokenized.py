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
"""
Generates json trace files viewable using chrome://tracing from binary
trace files.

Example usage:
python pw_trace_tokenized/py/trace_tokenized.py -i trace.bin -o trace.json
out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_basic
"""  # pylint: disable=line-too-long
# pylint: enable=line-too-long

from enum import IntEnum
import argparse
import logging
import struct
import sys
from pw_tokenizer import database, tokens
from pw_trace import trace

_LOG = logging.getLogger('pw_trace_tokenizer')


def varint_decode(encoded):
    # Taken from pw_tokenizer.decode._decode_signed_integer
    count = 0
    result = 0
    shift = 0
    for byte in encoded:
        count += 1
        result |= (byte & 0x7F) << shift
        if not byte & 0x80:
            return result, count

        shift += 7
        if shift >= 64:
            break  # Error
    return None


# Token string: "event_type|flag|module|group|label|<optional DATA_FMT>"
class TokenIdx(IntEnum):
    EVENT_TYPE = 0
    FLAG = 1
    MODULE = 2
    GROUP = 3
    LABEL = 4
    DATA_FMT = 5  # optional


def get_trace_type(type_str):
    if type_str == "PW_TRACE_EVENT_TYPE_INSTANT":
        return trace.TraceType.INSTANTANEOUS
    if type_str == "PW_TRACE_EVENT_TYPE_INSTANT_GROUP":
        return trace.TraceType.INSTANTANEOUS_GROUP
    if type_str == "PW_TRACE_EVENT_TYPE_ASYNC_START":
        return trace.TraceType.ASYNC_START
    if type_str == "PW_TRACE_EVENT_TYPE_ASYNC_STEP":
        return trace.TraceType.ASYNC_STEP
    if type_str == "PW_TRACE_EVENT_TYPE_ASYNC_END":
        return trace.TraceType.ASYNC_END
    if type_str == "PW_TRACE_EVENT_TYPE_DURATION_START":
        return trace.TraceType.DURATION_START
    if type_str == "PW_TRACE_EVENT_TYPE_DURATION_END":
        return trace.TraceType.DURATION_END
    if type_str == "PW_TRACE_EVENT_TYPE_DURATION_GROUP_START":
        return trace.TraceType.DURATION_GROUP_START
    if type_str == "PW_TRACE_EVENT_TYPE_DURATION_GROUP_END":
        return trace.TraceType.DURATION_GROUP_END
    return trace.TraceType.INVALID


def has_trace_id(token_string):
    token_values = token_string.split("|")
    return trace.event_has_trace_id(token_values[TokenIdx.EVENT_TYPE])


def has_data(token_string):
    token_values = token_string.split("|")
    return len(token_values) > TokenIdx.DATA_FMT


def create_trace_event(token_string, timestamp_us, trace_id, data):
    token_values = token_string.split("|")
    return trace.TraceEvent(
        event_type=get_trace_type(token_values[TokenIdx.EVENT_TYPE]),
        module=token_values[TokenIdx.MODULE],
        label=token_values[TokenIdx.LABEL],
        timestamp_us=timestamp_us,
        group=token_values[TokenIdx.GROUP],
        trace_id=trace_id,
        flags=token_values[TokenIdx.FLAG],
        has_data=has_data(token_string),
        data_fmt=(
            token_values[TokenIdx.DATA_FMT] if has_data(token_string) else ""
        ),
        data=data if has_data(token_string) else b'',
    )


def parse_trace_event(buffer, db, last_time, ticks_per_second):
    """Parse a single trace event from bytes"""
    us_per_tick = 1000000 / ticks_per_second
    idx = 0
    # Read token
    token = struct.unpack('I', buffer[idx : idx + 4])[0]
    idx += 4

    # Decode token
    if len(db.token_to_entries[token]) == 0:
        _LOG.error("token not found: %08x", token)
        return None

    token_string = str(db.token_to_entries[token][0])

    # Read time
    time_delta, time_bytes = varint_decode(buffer[idx:])
    timestamp_us = last_time + us_per_tick * time_delta
    idx += time_bytes

    # Trace ID
    trace_id = None
    if has_trace_id(token_string) and idx < len(buffer):
        trace_id, trace_id_bytes = varint_decode(buffer[idx:])
        idx += trace_id_bytes

    # Data
    data = None
    if has_data(token_string) and idx < len(buffer):
        data = buffer[idx:]

    # Create trace event
    return create_trace_event(token_string, timestamp_us, trace_id, data)


def get_trace_events(
    databases, raw_trace_data, ticks_per_second, time_offset: int
):
    """Handles the decoding traces."""

    db = tokens.Database.merged(*databases)
    last_timestamp = time_offset
    events = []
    idx = 0

    while idx + 1 < len(raw_trace_data):
        # Read size
        size = int(raw_trace_data[idx])
        if idx + size > len(raw_trace_data):
            _LOG.error("incomplete file")
            break

        event = parse_trace_event(
            raw_trace_data[idx + 1 : idx + 1 + size],
            db,
            last_timestamp,
            ticks_per_second,
        )
        if event:
            last_timestamp = event.timestamp_us
            events.append(event)
        idx = idx + size + 1
    return events


def get_trace_data_from_file(input_file_name):
    """Handles the decoding traces."""
    with open(input_file_name, "rb") as input_file:
        return input_file.read()
    return None


def save_trace_file(trace_lines, file_name):
    """Handles generating the trace file."""
    with open(file_name, 'w') as output_file:
        output_file.write("[")
        for line in trace_lines:
            output_file.write("%s,\n" % line)
        output_file.write("{}]")


def get_trace_events_from_file(
    databases, input_file_name, ticks_per_second, time_offset: int
):
    """Get trace events from a file."""
    raw_trace_data = get_trace_data_from_file(input_file_name)
    return get_trace_events(
        databases, raw_trace_data, ticks_per_second, time_offset
    )


def _parse_args():
    """Parse and return command line arguments."""

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        'databases',
        nargs='+',
        action=database.LoadTokenDatabases,
        help='Databases (ELF, binary, or CSV) to use to lookup tokens.',
    )
    parser.add_argument(
        '-i',
        '--input',
        dest='input_file',
        help='The binary trace input file, generated using trace_to_file.h.',
    )
    parser.add_argument(
        '-o',
        '--output',
        dest='output_file',
        help=('The json file to which to write the output.'),
    )
    parser.add_argument(
        '-t',
        '--ticks_per_second',
        type=int,
        dest='ticks_per_second',
        default=1000,
        help=('The clock rate of the trace events (Default 1000).'),
    )
    parser.add_argument(
        '--time_offset',
        type=int,
        dest='time_offset',
        default=0,
        help=('Time offset (us) of the trace events (Default 0).'),
    )

    return parser.parse_args()


def _main(args):
    events = get_trace_events_from_file(
        args.databases, args.input_file, args.ticks_per_second, args.time_offset
    )
    json_lines = trace.generate_trace_json(events)
    save_trace_file(json_lines, args.output_file)


if __name__ == '__main__':
    if sys.version_info[0] < 3:
        sys.exit('ERROR: The detokenizer command line tools require Python 3.')
    _main(_parse_args())
