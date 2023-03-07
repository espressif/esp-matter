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
"""Tests compiling and importing Python protos on the fly."""

from pathlib import Path
import tempfile
import unittest

from pw_protobuf_compiler import python_protos
from pw_protobuf_compiler.python_protos import bytes_repr, proto_repr

PROTO_1 = """\
syntax = "proto3";

package pw.protobuf_compiler.test1;

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
  rpc Unary(SomeMessage) returns (AnotherMessage) {}
  rpc ServerStreaming(SomeMessage) returns (stream AnotherMessage) {}
  rpc ClientStreaming(stream SomeMessage) returns (AnotherMessage) {}
  rpc BidiStreaming(stream SomeMessage) returns (stream AnotherMessage) {}
}
"""

PROTO_2 = """\
syntax = "proto2";

package pw.protobuf_compiler.test2;

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

PROTO_3 = """\
syntax = "proto3";

package pw.protobuf_compiler.test2;

enum Greeting {
  YO = 0;
  HI = 1;
}

message Hello {
  repeated int64 value = 1;
  Greeting hi = 2;
}

message NestingMessage {
  message NestedMessage {
    message NestedNestedMessage {
      int32 nested_nested_field = 1;
    }

    NestedNestedMessage nested_nested_message = 1;
  }

  NestedMessage nested_message = 1;
}
"""


class TestCompileAndImport(unittest.TestCase):
    """Test compiling and importing."""

    def setUp(self):
        self._proto_dir = tempfile.TemporaryDirectory(prefix='proto_test')
        self._protos = []

        for i, contents in enumerate([PROTO_1, PROTO_2, PROTO_3], 1):
            self._protos.append(Path(self._proto_dir.name, f'test_{i}.proto'))
            self._protos[-1].write_text(contents)

    def tearDown(self):
        self._proto_dir.cleanup()

    def test_compile_to_temp_dir_and_import(self):
        modules = {
            m.DESCRIPTOR.name: m
            for m in python_protos.compile_and_import(self._protos)
        }
        self.assertEqual(3, len(modules))

        # Make sure the protobuf modules contain what we expect them to.
        mod = modules['test_1.proto']
        self.assertEqual(
            4, len(mod.DESCRIPTOR.services_by_name['PublicService'].methods)
        )

        mod = modules['test_2.proto']
        self.assertEqual(mod.Request(magic_number=1.5).magic_number, 1.5)
        self.assertEqual(2, len(mod.DESCRIPTOR.services_by_name))

        mod = modules['test_3.proto']
        self.assertEqual(mod.Hello(value=[123, 456]).value, [123, 456])


class TestProtoLibrary(TestCompileAndImport):
    """Tests the Library class."""

    def setUp(self):
        super().setUp()
        self._library = python_protos.Library(
            python_protos.compile_and_import(self._protos)
        )

    def test_packages_can_access_messages(self):
        msg = self._library.packages.pw.protobuf_compiler.test1.SomeMessage
        self.assertEqual(msg(magic_number=123).magic_number, 123)

    def test_packages_finds_across_modules(self):
        msg = self._library.packages.pw.protobuf_compiler.test2.Request
        self.assertEqual(msg(magic_number=50).magic_number, 50)

        val = self._library.packages.pw.protobuf_compiler.test2.YO
        self.assertEqual(val, 0)

    def test_packages_invalid_name(self):
        with self.assertRaises(AttributeError):
            _ = self._library.packages.nothing

        with self.assertRaises(AttributeError):
            _ = self._library.packages.pw.NOT_HERE

        with self.assertRaises(AttributeError):
            _ = self._library.packages.pw.protobuf_compiler.test1.NotARealMsg

    def test_access_modules_by_package(self):
        test1 = self._library.modules_by_package['pw.protobuf_compiler.test1']
        self.assertEqual(len(test1), 1)
        self.assertEqual(test1[0].AnotherMessage.Result.Value('FAILED'), 0)

        test2 = self._library.modules_by_package['pw.protobuf_compiler.test2']
        self.assertEqual(len(test2), 2)

    def test_access_modules_by_package_unknown(self):
        with self.assertRaises(KeyError):
            _ = self._library.modules_by_package['pw.not_real']

    def test_library_from_strings(self):
        # Replace the package to avoid conflicts with the other proto imports
        new_protos = [
            p.replace('pw.protobuf_compiler', 'proto.library.test')
            for p in [PROTO_1, PROTO_2, PROTO_3]
        ]

        library = python_protos.Library.from_strings(new_protos)

        # Make sure we can safely import the same proto contents multiple times.
        library = python_protos.Library.from_strings(new_protos)

        msg = library.packages.proto.library.test.test2.Request
        self.assertEqual(msg(magic_number=50).magic_number, 50)

        val = library.packages.proto.library.test.test2.YO
        self.assertEqual(val, 0)

    def test_access_nested_packages_by_name(self):
        self.assertIs(
            self._library.packages['pw.protobuf_compiler.test1'],
            self._library.packages.pw.protobuf_compiler.test1,
        )
        self.assertIs(
            self._library.packages.pw['protobuf_compiler.test1'],
            self._library.packages.pw.protobuf_compiler.test1,
        )
        self.assertIs(
            self._library.packages.pw.protobuf_compiler['test1'],
            self._library.packages.pw.protobuf_compiler.test1,
        )

    def test_access_nested_packages_by_name_unknown_package(self):
        with self.assertRaises(KeyError):
            _ = self._library.packages['']

        with self.assertRaises(KeyError):
            _ = self._library.packages['.']

        with self.assertRaises(KeyError):
            _ = self._library.packages['protobuf_compiler.test1']

        with self.assertRaises(KeyError):
            _ = self._library.packages.pw['pw.protobuf_compiler.test1']

        with self.assertRaises(KeyError):
            _ = self._library.packages.pw.protobuf_compiler['not here']

    def test_messages(self):
        protos = self._library.packages.pw.protobuf_compiler
        self.assertEqual(
            set(self._library.messages()),
            {
                protos.test1.SomeMessage,
                protos.test1.AnotherMessage,
                protos.test2.Request,
                protos.test2.Response,
                protos.test2.Hello,
                protos.test2.NestingMessage,
                protos.test2.NestingMessage.NestedMessage,
                protos.test2.NestingMessage.NestedMessage.NestedNestedMessage,
            },
        )


PROTO_FOR_REPR = """\
syntax = "proto3";

package pw.test3;

enum Enum {
  ZERO = 0;
  ONE = 1;
}

message Nested {
  repeated int64 value = 1;
  Enum an_enum = 2;
}

message Message {
  Nested message = 1;
  repeated Nested repeated_message = 2;

  fixed32 regular_int = 3;
  optional int64 optional_int = 4;
  repeated int32 repeated_int = 5;

  bytes regular_bytes = 6;
  optional bytes optional_bytes = 7;
  repeated bytes repeated_bytes = 8;

  string regular_string = 9;
  optional string optional_string = 10;
  repeated string repeated_string = 11;

  Enum regular_enum = 12;
  optional Enum optional_enum = 13;
  repeated Enum repeated_enum = 14;

  oneof oneof_test {
    string oneof_1 = 15;
    int32 oneof_2 = 16;
    Nested oneof_3 = 17;
  }

  map<string, Nested> mapping = 18;
}
"""


class TestProtoRepr(unittest.TestCase):
    """Tests printing protobufs."""

    def setUp(self):
        protos = python_protos.Library.from_strings(PROTO_FOR_REPR)
        self.enum = protos.packages.pw.test3.Enum
        self.nested = protos.packages.pw.test3.Nested
        self.message = protos.packages.pw.test3.Message

    def test_empty(self):
        self.assertEqual('pw.test3.Nested()', proto_repr(self.nested()))
        self.assertEqual('pw.test3.Message()', proto_repr(self.message()))

    def test_int_fields(self):
        self.assertEqual(
            'pw.test3.Message('
            'regular_int=999, '
            'optional_int=-1, '
            'repeated_int=[0, 1, 2])',
            proto_repr(
                self.message(
                    repeated_int=[0, 1, 2], regular_int=999, optional_int=-1
                ),
                wrap=False,
            ),
        )

    def test_bytes_fields(self):
        self.assertEqual(
            'pw.test3.Message('
            r"regular_bytes=b'\xFE\xED\xBE\xEF', "
            r"optional_bytes=b'', "
            r"repeated_bytes=[b'Hello\'\'\''])",
            proto_repr(
                self.message(
                    regular_bytes=b'\xfe\xed\xbe\xef',
                    optional_bytes=b'',
                    repeated_bytes=[b"Hello'''"],
                ),
                wrap=False,
            ),
        )

    def test_string_fields(self):
        self.assertEqual(
            'pw.test3.Message('
            "regular_string='hi', "
            "optional_string='', "
            'repeated_string=["\'"])',
            proto_repr(
                self.message(
                    regular_string='hi',
                    optional_string='',
                    repeated_string=[b"'"],
                ),
                wrap=False,
            ),
        )

    def test_enum_fields(self):
        self.assertEqual(
            'pw.test3.Nested(an_enum=pw.test3.Enum.ONE)',
            proto_repr(self.nested(an_enum=1)),
        )
        self.assertEqual(
            'pw.test3.Message(optional_enum=pw.test3.Enum.ONE)',
            proto_repr(self.message(optional_enum=self.enum.ONE)),
        )
        self.assertEqual(
            'pw.test3.Message(repeated_enum='
            '[pw.test3.Enum.ONE, pw.test3.Enum.ONE, pw.test3.Enum.ZERO])',
            proto_repr(self.message(repeated_enum=[1, 1, 0]), wrap=False),
        )

    def test_message_fields(self):
        self.assertEqual(
            'pw.test3.Message(message=pw.test3.Nested(value=[123]))',
            proto_repr(self.message(message=self.nested(value=[123]))),
        )
        self.assertEqual(
            'pw.test3.Message('
            'repeated_message=[pw.test3.Nested(value=[123]), '
            'pw.test3.Nested()])',
            proto_repr(
                self.message(
                    repeated_message=[self.nested(value=[123]), self.nested()]
                ),
                wrap=False,
            ),
        )

    def test_optional_shown_if_set_to_default(self):
        self.assertEqual(
            "pw.test3.Message("
            "optional_int=0, optional_bytes=b'', optional_string='', "
            "optional_enum=pw.test3.Enum.ZERO)",
            proto_repr(
                self.message(
                    optional_int=0,
                    optional_bytes=b'',
                    optional_string='',
                    optional_enum=0,
                ),
                wrap=False,
            ),
        )

    def test_oneof(self):
        self.assertEqual(
            proto_repr(self.message(oneof_1='test')),
            "pw.test3.Message(oneof_1='test')",
        )
        self.assertEqual(
            proto_repr(self.message(oneof_2=123)),
            "pw.test3.Message(oneof_2=123)",
        )
        self.assertEqual(
            proto_repr(
                self.message(oneof_3=self.nested(an_enum=self.enum.ONE))
            ),
            'pw.test3.Message('
            'oneof_3=pw.test3.Nested(an_enum=pw.test3.Enum.ONE))',
        )

        msg = self.message(oneof_1='test')
        msg.oneof_2 = 99
        self.assertEqual(proto_repr(msg), "pw.test3.Message(oneof_2=99)")

    def test_map(self):
        msg = self.message()
        msg.mapping['zero'].MergeFrom(self.nested())
        msg.mapping['one'].MergeFrom(
            self.nested(an_enum=self.enum.ONE, value=[1])
        )

        result = proto_repr(msg, wrap=False)
        self.assertRegex(result, r'^pw.test3.Message\(mapping={.*}\)$')
        self.assertIn("'zero': pw.test3.Nested()", result)
        self.assertIn(
            "'one': pw.test3.Nested(value=[1], an_enum=pw.test3.Enum.ONE)",
            result,
        )

    def test_bytes_repr(self):
        self.assertEqual(
            bytes_repr(b'\xfe\xed\xbe\xef'), r"b'\xFE\xED\xBE\xEF'"
        )
        self.assertEqual(
            bytes_repr(b'\xfe\xed\xbe\xef123'),
            r"b'\xFE\xED\xBE\xEF\x31\x32\x33'",
        )
        self.assertEqual(
            bytes_repr(b'\xfe\xed\xbe\xef1234'), r"b'\xFE\xED\xBE\xEF1234'"
        )
        self.assertEqual(
            bytes_repr(b'\xfe\xed\xbe\xef12345'), r"b'\xFE\xED\xBE\xEF12345'"
        )

    def test_wrap_multiple_lines(self):
        self.assertEqual(
            """\
pw.test3.Message(optional_int=0,
                 optional_bytes=b'',
                 optional_string='',
                 optional_enum=pw.test3.Enum.ZERO)""",
            proto_repr(
                self.message(
                    optional_int=0,
                    optional_bytes=b'',
                    optional_string='',
                    optional_enum=0,
                ),
                wrap=True,
            ),
        )

    def test_wrap_one_line(self):
        self.assertEqual(
            "pw.test3.Message(optional_int=0, optional_bytes=b'')",
            proto_repr(
                self.message(optional_int=0, optional_bytes=b''), wrap=True
            ),
        )


if __name__ == '__main__':
    unittest.main()
