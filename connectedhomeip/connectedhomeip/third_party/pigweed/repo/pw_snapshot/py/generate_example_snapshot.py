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
"""Generates an example snapshot useful for updating documentation."""

import argparse
import sys
from typing import TextIO
from pw_snapshot_protos import snapshot_pb2
from pw_snapshot import processor
from pw_thread_protos import thread_pb2


def _add_threads(snapshot: snapshot_pb2.Snapshot) -> snapshot_pb2.Snapshot:
    # Build example idle thread.
    thread = thread_pb2.Thread()
    thread.name = 'Idle'.encode()
    thread.stack_start_pointer = 0x2001AC00
    thread.stack_end_pointer = 0x2001AA00
    thread.stack_pointer = 0x2001AB0C
    thread.state = thread_pb2.ThreadState.Enum.RUNNING
    snapshot.threads.append(thread)

    # Build example interrupt handler thread.
    thread = thread_pb2.Thread()
    thread.name = 'Main Stack (Handler Mode)'.encode()
    thread.active = True
    thread.stack_start_pointer = 0x2001B000
    thread.stack_pointer = 0x2001AE20
    thread.state = thread_pb2.ThreadState.Enum.INTERRUPT_HANDLER
    thread.raw_stack = b'\x00\xCA\xAD\xDE'
    snapshot.threads.append(thread)

    return snapshot


def _main(out_file: TextIO):
    snapshot = snapshot_pb2.Snapshot()

    snapshot.metadata.reason = (
        '■msg♦Assert failed: 1+1 == 42' '■file♦../examples/example_rpc.cc'
    ).encode('utf-8')
    snapshot.metadata.fatal = True
    snapshot.metadata.project_name = 'gShoe'.encode('utf-8')
    snapshot.metadata.software_version = 'QUANTUM_CORE-0.1.325-e4a84b1a'
    snapshot.metadata.software_build_uuid = (
        b'\xAD\x2D\x39\x25\x8C\x1B\xC4\x87'
        b'\xF0\x7C\xA7\xE0\x49\x91\xA8\x36'
        b'\xFD\xF7\xD0\xA0'
    )
    snapshot.metadata.device_name = 'GSHOE-QUANTUM_CORE-REV_0.1'.encode('utf-8')
    snapshot.metadata.snapshot_uuid = (
        b'\x84\x81\xBB\x12\xA1\x62\x16\x4F' b'\x5C\x74\x85\x5F\x6D\x94\xEA\x1A'
    )

    # Add some thread-related info.
    snapshot = _add_threads(snapshot)

    serialized_snapshot = snapshot.SerializeToString()
    out_file.write(processor.process_snapshots(serialized_snapshot))


def _parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--out-file',
        '-o',
        type=argparse.FileType('w'),
        help='File to output serialized snapshot to.',
    )

    return parser.parse_args()


if __name__ == '__main__':
    _main(**vars(_parse_args()))
    sys.exit(0)
