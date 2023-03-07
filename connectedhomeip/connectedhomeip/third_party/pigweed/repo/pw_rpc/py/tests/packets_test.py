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

from pw_status import Status

from pw_rpc.internal.packet_pb2 import PacketType, RpcPacket
from pw_rpc import packets

_TEST_REQUEST = RpcPacket(
    type=PacketType.REQUEST,
    channel_id=1,
    service_id=2,
    method_id=3,
    payload=RpcPacket(status=321).SerializeToString(),
)


class PacketsTest(unittest.TestCase):
    """Tests for packet encoding and decoding."""

    def test_encode_request(self):
        data = packets.encode_request((1, 2, 3), RpcPacket(status=321))
        packet = RpcPacket()
        packet.ParseFromString(data)

        self.assertEqual(_TEST_REQUEST, packet)

    def test_encode_response(self):
        response = RpcPacket(
            type=PacketType.RESPONSE,
            channel_id=1,
            service_id=2,
            method_id=3,
            payload=RpcPacket(status=321).SerializeToString(),
        )

        data = packets.encode_response((1, 2, 3), RpcPacket(status=321))
        packet = RpcPacket()
        packet.ParseFromString(data)

        self.assertEqual(response, packet)

    def test_encode_cancel(self):
        data = packets.encode_cancel((9, 8, 7))

        packet = RpcPacket()
        packet.ParseFromString(data)

        self.assertEqual(
            packet,
            RpcPacket(
                type=PacketType.CLIENT_ERROR,
                channel_id=9,
                service_id=8,
                method_id=7,
                status=Status.CANCELLED.value,
            ),
        )

    def test_encode_client_error(self):
        data = packets.encode_client_error(_TEST_REQUEST, Status.NOT_FOUND)

        packet = RpcPacket()
        packet.ParseFromString(data)

        self.assertEqual(
            packet,
            RpcPacket(
                type=PacketType.CLIENT_ERROR,
                channel_id=1,
                service_id=2,
                method_id=3,
                status=Status.NOT_FOUND.value,
            ),
        )

    def test_decode(self):
        self.assertEqual(
            _TEST_REQUEST, packets.decode(_TEST_REQUEST.SerializeToString())
        )

    def test_for_server(self):
        self.assertTrue(packets.for_server(_TEST_REQUEST))

        self.assertFalse(
            packets.for_server(
                RpcPacket(
                    type=PacketType.RESPONSE,
                    channel_id=1,
                    service_id=2,
                    method_id=3,
                    payload=RpcPacket(status=321).SerializeToString(),
                )
            )
        )


if __name__ == '__main__':
    unittest.main()
