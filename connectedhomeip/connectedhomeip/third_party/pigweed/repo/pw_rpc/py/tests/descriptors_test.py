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
"""Tests classes in pw_rpc.descriptors."""

import unittest

from google.protobuf.message_factory import MessageFactory

from pw_protobuf_compiler import python_protos
from pw_rpc import descriptors

TEST_PROTO = """\
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


class MethodTest(unittest.TestCase):
    """Tests pw_rpc.Method."""

    def setUp(self):
        (module,) = python_protos.compile_and_import_strings([TEST_PROTO])
        service = descriptors.Service.from_descriptor(
            module.DESCRIPTOR.services_by_name['PublicService']
        )
        self._method = service.methods['SomeUnary']

    def test_get_request_with_both_message_and_kwargs(self):
        with self.assertRaisesRegex(TypeError, r'either'):
            self._method.get_request(
                self._method.request_type(), {'magic_number': 1}
            )

    def test_get_request_neither_message_nor_kwargs(self):
        self.assertEqual(
            self._method.request_type(), self._method.get_request(None, None)
        )

    def test_get_request_with_wrong_type(self):
        with self.assertRaisesRegex(TypeError, r'pw\.test1\.SomeMessage'):
            self._method.get_request('a str!', {})

    def test_get_request_with_different_message_type(self):
        msg = self._method.response_type()
        with self.assertRaisesRegex(TypeError, r'pw\.test1\.SomeMessage'):
            self._method.get_request(msg, {})

    def test_get_request_with_different_copy_of_same_message_class(self):
        some_message_clone = MessageFactory(
            self._method.request_type.DESCRIPTOR.file.pool
        ).GetPrototype(self._method.request_type.DESCRIPTOR)

        msg = some_message_clone()

        # Protobuf classes obtained with a MessageFactory may or may not be a
        # unique type, but will always use the same descriptor instance.
        self.assertIsInstance(msg, some_message_clone)
        self.assertIs(msg.DESCRIPTOR, self._method.request_type.DESCRIPTOR)

        result = self._method.get_request(msg, {})
        self.assertIs(result, msg)


if __name__ == '__main__':
    unittest.main()
