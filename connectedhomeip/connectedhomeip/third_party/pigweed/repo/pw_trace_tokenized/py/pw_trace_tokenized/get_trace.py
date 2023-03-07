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
"""
Generates json trace files viewable using chrome://tracing using RPCs from a
connected HdlcRpcClient.

Example usage:
python pw_trace_tokenized/py/pw_trace_tokenized/get_trace.py -s localhost:33000
  -o trace.json
  -t
  out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_rpc
  pw_trace_tokenized/pw_trace_protos/trace_rpc.proto
"""  # pylint: disable=line-too-long
# pylint: enable=line-too-long

import argparse
import glob
import logging
from pathlib import Path
import socket
import sys
from typing import Collection, Iterable, Iterator

import serial  # type: ignore
from pw_tokenizer import database
from pw_trace import trace
from pw_hdlc.rpc import HdlcRpcClient, default_channels
from pw_trace_tokenized import trace_tokenized

_LOG = logging.getLogger('pw_trace_tokenizer')

PW_RPC_MAX_PACKET_SIZE = 256
SOCKET_SERVER = 'localhost'
SOCKET_PORT = 33000
MKFIFO_MODE = 0o666


class SocketClientImpl:
    def __init__(self, config: str):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socket_server = ''
        socket_port = 0

        if config == 'default':
            socket_server = SOCKET_SERVER
            socket_port = SOCKET_PORT
        else:
            socket_server, socket_port_str = config.split(':')
            socket_port = int(socket_port_str)
        self.socket.connect((socket_server, socket_port))

    def write(self, data: bytes):
        self.socket.sendall(data)

    def read(self, num_bytes: int = PW_RPC_MAX_PACKET_SIZE):
        return self.socket.recv(num_bytes)


def _expand_globs(globs: Iterable[str]) -> Iterator[Path]:
    for pattern in globs:
        for file in glob.glob(pattern, recursive=True):
            yield Path(file)


def get_hdlc_rpc_client(
    device: str,
    baudrate: int,
    proto_globs: Collection[str],
    socket_addr: str,
    **kwargs,
):
    """Get the HdlcRpcClient based on arguments."""
    del kwargs  # ignore
    if not proto_globs:
        proto_globs = ['**/*.proto']

    protos = list(_expand_globs(proto_globs))

    if not protos:
        _LOG.critical(
            'No .proto files were found with %s', ', '.join(proto_globs)
        )
        _LOG.critical('At least one .proto file is required')
        return 1

    _LOG.debug(
        'Found %d .proto files found with %s',
        len(protos),
        ', '.join(proto_globs),
    )

    # TODO(rgoliver): When pw has a generalized transport for RPC this should
    # use it so it isn't specific to HDLC
    if socket_addr is None:
        serial_device = serial.Serial(device, baudrate, timeout=1)
        read = lambda: serial_device.read(8192)
        write = serial_device.write
    else:
        try:
            socket_device = SocketClientImpl(socket_addr)
            read = socket_device.read
            write = socket_device.write
        except ValueError:
            _LOG.exception('Failed to initialize socket at %s', socket_addr)
            return 1

    return HdlcRpcClient(read, protos, default_channels(write))


def get_trace_data_from_device(client):
    """Get the trace data using RPC from a Client"""
    data = b''
    service = client.client.channel(1).rpcs.pw.trace.TraceService
    result = service.GetTraceData().responses
    for streamed_data in result:
        data = data + bytes([len(streamed_data.data)])
        data = data + streamed_data.data
        _LOG.debug(''.join(format(x, '02x') for x in streamed_data.data))
    return data


def _parse_args():
    """Parse and return command line arguments."""

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-d', '--device', help='the serial port to use')
    parser.add_argument(
        '-b',
        '--baudrate',
        type=int,
        default=115200,
        help='the baud rate to use',
    )
    group.add_argument(
        '-s',
        '--socket-addr',
        type=str,
        help='use socket to connect to server, type default for\
            localhost:33000, or manually input the server address:port',
    )
    parser.add_argument(
        '-o',
        '--trace_output',
        dest='trace_output_file',
        help=('The json file to which to write the output.'),
    )
    parser.add_argument(
        '-t',
        '--trace_token_database',
        help='Databases (ELF, binary, or CSV) to use to lookup trace tokens.',
    )
    parser.add_argument(
        'proto_globs', nargs='+', help='glob pattern for .proto files'
    )
    parser.add_argument(
        '-f',
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
    token_database = database.load_token_database(
        args.trace_token_database, domain="trace"
    )
    _LOG.info(database.database_summary(token_database))
    client = get_hdlc_rpc_client(**vars(args))
    data = get_trace_data_from_device(client)
    events = trace_tokenized.get_trace_events(
        [token_database], data, args.ticks_per_second, args.time_offset
    )
    json_lines = trace.generate_trace_json(events)
    trace_tokenized.save_trace_file(json_lines, args.trace_output_file)


if __name__ == '__main__':
    if sys.version_info[0] < 3:
        sys.exit('ERROR: The detokenizer command line tools require Python 3.')
    _main(_parse_args())
