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
"""Tests creating pw_rpc client."""

import unittest
from typing import Optional

from pw_protobuf_compiler import python_protos
from pw_status import Status

from pw_rpc import callback_client, client, packets
import pw_rpc.ids
from pw_rpc.internal.packet_pb2 import PacketType, RpcPacket

TEST_PROTO_1 = """\
syntax = "proto3";

package pw.test1;

message SomeMessage {
  uint32 magic_number = 1;
}

message AnotherMessage {
  enum Result {
    FAILED = 0;
    FAILED_MISERABLY = 1;
    I_DONT_WANT_TO_TALK_ABOUT_IT = 2;
  }

  Result result = 1;
  string payload = 2;
}

service PublicService {
  rpc SomeUnary(SomeMessage) returns (AnotherMessage) {}
  rpc SomeServerStreaming(SomeMessage) returns (stream AnotherMessage) {}
  rpc SomeClientStreaming(stream SomeMessage) returns (AnotherMessage) {}
  rpc SomeBidiStreaming(stream SomeMessage) returns (stream AnotherMessage) {}
}
"""

TEST_PROTO_2 = """\
syntax = "proto2";

package pw.test2;

message Request {
  optional float magic_number = 1;
}

message Response {
}

service Alpha {
  rpc Unary(Request) returns (Response) {}
}

service Bravo {
  rpc BidiStreaming(stream Request) returns (stream Response) {}
}
"""


def _test_setup(output=None):
    protos = python_protos.Library.from_strings([TEST_PROTO_1, TEST_PROTO_2])
    return protos, client.Client.from_modules(
        callback_client.Impl(),
        [client.Channel(1, output), client.Channel(2, lambda _: None)],
        protos.modules(),
    )


class ChannelClientTest(unittest.TestCase):
    """Tests the ChannelClient."""

    def setUp(self) -> None:
        self._channel_client = _test_setup()[1].channel(1)

    def test_access_service_client_as_attribute_or_index(self) -> None:
        self.assertIs(
            self._channel_client.rpcs.pw.test1.PublicService,
            self._channel_client.rpcs['pw.test1.PublicService'],
        )
        self.assertIs(
            self._channel_client.rpcs.pw.test1.PublicService,
            self._channel_client.rpcs[
                pw_rpc.ids.calculate('pw.test1.PublicService')
            ],
        )

    def test_access_method_client_as_attribute_or_index(self) -> None:
        self.assertIs(
            self._channel_client.rpcs.pw.test2.Alpha.Unary,
            self._channel_client.rpcs['pw.test2.Alpha']['Unary'],
        )
        self.assertIs(
            self._channel_client.rpcs.pw.test2.Alpha.Unary,
            self._channel_client.rpcs['pw.test2.Alpha'][
                pw_rpc.ids.calculate('Unary')
            ],
        )

    def test_service_name(self) -> None:
        self.assertEqual(
            self._channel_client.rpcs.pw.test2.Alpha.Unary.service.name, 'Alpha'
        )
        self.assertEqual(
            self._channel_client.rpcs.pw.test2.Alpha.Unary.service.full_name,
            'pw.test2.Alpha',
        )

    def test_method_name(self) -> None:
        self.assertEqual(
            self._channel_client.rpcs.pw.test2.Alpha.Unary.method.name, 'Unary'
        )
        self.assertEqual(
            self._channel_client.rpcs.pw.test2.Alpha.Unary.method.full_name,
            'pw.test2.Alpha.Unary',
        )

    def test_iterate_over_all_methods(self) -> None:
        channel_client = self._channel_client
        all_methods = {
            channel_client.rpcs.pw.test1.PublicService.SomeUnary,
            channel_client.rpcs.pw.test1.PublicService.SomeServerStreaming,
            channel_client.rpcs.pw.test1.PublicService.SomeClientStreaming,
            channel_client.rpcs.pw.test1.PublicService.SomeBidiStreaming,
            channel_client.rpcs.pw.test2.Alpha.Unary,
            channel_client.rpcs.pw.test2.Bravo.BidiStreaming,
        }
        self.assertEqual(set(channel_client.methods()), all_methods)

    def test_check_for_presence_of_services(self) -> None:
        self.assertIn('pw.test1.PublicService', self._channel_client.rpcs)
        self.assertIn(
            pw_rpc.ids.calculate('pw.test1.PublicService'),
            self._channel_client.rpcs,
        )

    def test_check_for_presence_of_missing_services(self) -> None:
        self.assertNotIn('PublicService', self._channel_client.rpcs)
        self.assertNotIn('NotAService', self._channel_client.rpcs)
        self.assertNotIn(-1213, self._channel_client.rpcs)

    def test_check_for_presence_of_methods(self) -> None:
        service = self._channel_client.rpcs.pw.test1.PublicService
        self.assertIn('SomeUnary', service)
        self.assertIn(pw_rpc.ids.calculate('SomeUnary'), service)

    def test_check_for_presence_of_missing_methods(self) -> None:
        service = self._channel_client.rpcs.pw.test1.PublicService
        self.assertNotIn('Some', service)
        self.assertNotIn('Unary', service)
        self.assertNotIn(12345, service)

    def test_method_fully_qualified_name(self) -> None:
        self.assertIs(
            self._channel_client.method('pw.test2.Alpha/Unary'),
            self._channel_client.rpcs.pw.test2.Alpha.Unary,
        )
        self.assertIs(
            self._channel_client.method('pw.test2.Alpha.Unary'),
            self._channel_client.rpcs.pw.test2.Alpha.Unary,
        )


class ClientTest(unittest.TestCase):
    """Tests the pw_rpc Client independently of the ClientImpl."""

    def setUp(self) -> None:
        self._last_packet_sent_bytes: Optional[bytes] = None
        self._protos, self._client = _test_setup(self._save_packet)

    def _save_packet(self, packet) -> None:
        self._last_packet_sent_bytes = packet

    def _last_packet_sent(self) -> RpcPacket:
        packet = RpcPacket()
        assert self._last_packet_sent_bytes is not None
        packet.MergeFromString(self._last_packet_sent_bytes)
        return packet

    def test_channel(self) -> None:
        self.assertEqual(self._client.channel(1).channel.id, 1)
        self.assertEqual(self._client.channel(2).channel.id, 2)

    def test_channel_default_is_first_listed(self) -> None:
        self.assertEqual(self._client.channel().channel.id, 1)

    def test_channel_invalid(self) -> None:
        with self.assertRaises(KeyError):
            self._client.channel(404)

    def test_all_methods(self) -> None:
        services = self._client.services

        all_methods = {
            services['pw.test1.PublicService'].methods['SomeUnary'],
            services['pw.test1.PublicService'].methods['SomeServerStreaming'],
            services['pw.test1.PublicService'].methods['SomeClientStreaming'],
            services['pw.test1.PublicService'].methods['SomeBidiStreaming'],
            services['pw.test2.Alpha'].methods['Unary'],
            services['pw.test2.Bravo'].methods['BidiStreaming'],
        }
        self.assertEqual(set(self._client.methods()), all_methods)

    def test_method_present(self) -> None:
        self.assertIs(
            self._client.method('pw.test1.PublicService.SomeUnary'),
            self._client.services['pw.test1.PublicService'].methods[
                'SomeUnary'
            ],
        )
        self.assertIs(
            self._client.method('pw.test1.PublicService/SomeUnary'),
            self._client.services['pw.test1.PublicService'].methods[
                'SomeUnary'
            ],
        )

    def test_method_invalid_format(self) -> None:
        with self.assertRaises(ValueError):
            self._client.method('SomeUnary')

    def test_method_not_present(self) -> None:
        with self.assertRaises(KeyError):
            self._client.method('pw.test1.PublicService/ThisIsNotGood')

        with self.assertRaises(KeyError):
            self._client.method('nothing.Good')

    def test_process_packet_invalid_proto_data(self) -> None:
        self.assertIs(
            self._client.process_packet(b'NOT a packet!'), Status.DATA_LOSS
        )

    def test_process_packet_not_for_client(self) -> None:
        self.assertIs(
            self._client.process_packet(
                RpcPacket(type=PacketType.REQUEST).SerializeToString()
            ),
            Status.INVALID_ARGUMENT,
        )

    def test_process_packet_unrecognized_channel(self) -> None:
        self.assertIs(
            self._client.process_packet(
                packets.encode_response(
                    (123, 456, 789), self._protos.packages.pw.test2.Request()
                )
            ),
            Status.NOT_FOUND,
        )

    def test_process_packet_unrecognized_service(self) -> None:
        self.assertIs(
            self._client.process_packet(
                packets.encode_response(
                    (1, 456, 789), self._protos.packages.pw.test2.Request()
                )
            ),
            Status.OK,
        )

        self.assertEqual(
            self._last_packet_sent(),
            RpcPacket(
                type=PacketType.CLIENT_ERROR,
                channel_id=1,
                service_id=456,
                method_id=789,
                status=Status.NOT_FOUND.value,
            ),
        )

    def test_process_packet_unrecognized_method(self) -> None:
        service = next(iter(self._client.services))

        self.assertIs(
            self._client.process_packet(
                packets.encode_response(
                    (1, service.id, 789),
                    self._protos.packages.pw.test2.Request(),
                )
            ),
            Status.OK,
        )

        self.assertEqual(
            self._last_packet_sent(),
            RpcPacket(
                type=PacketType.CLIENT_ERROR,
                channel_id=1,
                service_id=service.id,
                method_id=789,
                status=Status.NOT_FOUND.value,
            ),
        )

    def test_process_packet_non_pending_method(self) -> None:
        service = next(iter(self._client.services))
        method = next(iter(service.methods))

        self.assertIs(
            self._client.process_packet(
                packets.encode_response(
                    (1, service.id, method.id),
                    self._protos.packages.pw.test2.Request(),
                )
            ),
            Status.OK,
        )

        self.assertEqual(
            self._last_packet_sent(),
            RpcPacket(
                type=PacketType.CLIENT_ERROR,
                channel_id=1,
                service_id=service.id,
                method_id=method.id,
                status=Status.FAILED_PRECONDITION.value,
            ),
        )

    def test_process_packet_non_pending_calls_response_callback(self) -> None:
        method = self._client.method('pw.test1.PublicService.SomeUnary')
        reply = method.response_type(payload='hello')

        def response_callback(
            rpc: client.PendingRpc, message, status: Optional[Status]
        ) -> None:
            self.assertEqual(
                rpc,
                client.PendingRpc(
                    self._client.channel(1).channel, method.service, method
                ),
            )
            self.assertEqual(message, reply)
            self.assertIs(status, Status.OK)

        self._client.response_callback = response_callback

        self.assertIs(
            self._client.process_packet(
                packets.encode_response((1, method.service, method), reply)
            ),
            Status.OK,
        )


if __name__ == '__main__':
    unittest.main()
