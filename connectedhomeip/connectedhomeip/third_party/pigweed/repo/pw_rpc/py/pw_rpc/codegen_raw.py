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
"""This module generates the code for raw pw_rpc services."""

import os
from typing import Iterable

from pw_protobuf.output_file import OutputFile
from pw_protobuf.proto_tree import ProtoServiceMethod
from pw_protobuf.proto_tree import build_node_tree
from pw_rpc import codegen
from pw_rpc.codegen import (
    client_call_type,
    get_id,
    CodeGenerator,
    RPC_NAMESPACE,
)

PROTO_H_EXTENSION = '.pb.h'


def _proto_filename_to_generated_header(proto_file: str) -> str:
    """Returns the generated C++ RPC header name for a .proto file."""
    filename = os.path.splitext(proto_file)[0]
    return f'{filename}.raw_rpc{PROTO_H_EXTENSION}'


def _proto_filename_to_stub_header(proto_file: str) -> str:
    """Returns the generated C++ RPC header name for a .proto file."""
    filename = os.path.splitext(proto_file)[0]
    return f'{filename}.raw_rpc.stub{PROTO_H_EXTENSION}'


def _function(method: ProtoServiceMethod) -> str:
    return f'{client_call_type(method, "Raw")} {method.name()}'


def _user_args(method: ProtoServiceMethod) -> Iterable[str]:
    if not method.client_streaming():
        yield '::pw::ConstByteSpan request'

    if method.server_streaming():
        yield '::pw::Function<void(::pw::ConstByteSpan)>&& on_next = nullptr'
        yield '::pw::Function<void(::pw::Status)>&& on_completed = nullptr'
    else:
        yield (
            '::pw::Function<void(::pw::ConstByteSpan, ::pw::Status)>&& '
            'on_completed = nullptr'
        )

    yield '::pw::Function<void(::pw::Status)>&& on_error = nullptr'


class RawCodeGenerator(CodeGenerator):
    """Generates an RPC service and client using the raw buffers API."""

    def name(self) -> str:
        return 'raw'

    def method_union_name(self) -> str:
        return 'RawMethodUnion'

    def includes(self, unused_proto_file_name: str) -> Iterable[str]:
        yield '#include "pw_rpc/raw/client_reader_writer.h"'
        yield '#include "pw_rpc/raw/internal/method_union.h"'
        yield '#include "pw_rpc/raw/server_reader_writer.h"'

    def service_aliases(self) -> None:
        self.line(f'using RawServerWriter = {RPC_NAMESPACE}::RawServerWriter;')
        self.line(f'using RawServerReader = {RPC_NAMESPACE}::RawServerReader;')
        self.line(
            'using RawServerReaderWriter = '
            f'{RPC_NAMESPACE}::RawServerReaderWriter;'
        )

    def method_descriptor(self, method: ProtoServiceMethod) -> None:
        impl_method = f'&Implementation::{method.name()}'

        self.line(
            f'{RPC_NAMESPACE}::internal::GetRawMethodFor<{impl_method}, '
            f'{method.type().cc_enum()}>('
        )
        self.line(f'    {get_id(method)}),  // Hash of "{method.name()}"')

    def client_member_function(self, method: ProtoServiceMethod) -> None:
        self.line(f'{_function(method)}(')
        self.indented_list(*_user_args(method), end=') const {')

        with self.indent():
            base = 'Stream' if method.server_streaming() else 'Unary'
            self.line(
                f'return {RPC_NAMESPACE}::internal::'
                f'{base}ResponseClientCall::'
                f'Start<{client_call_type(method, "Raw")}>('
            )

            service_client = RPC_NAMESPACE + '::internal::ServiceClient'
            arg = ['std::move(on_next)'] if method.server_streaming() else []

            self.indented_list(
                f'{service_client}::client()',
                f'{service_client}::channel_id()',
                'kServiceId',
                get_id(method),
                *arg,
                'std::move(on_completed)',
                'std::move(on_error)',
                '{}' if method.client_streaming() else 'request',
                end=');',
            )

        self.line('}')

    def client_static_function(self, method: ProtoServiceMethod) -> None:
        self.line(f'static {_function(method)}(')
        self.indented_list(
            f'{RPC_NAMESPACE}::Client& client',
            'uint32_t channel_id',
            *_user_args(method),
            end=') {',
        )

        with self.indent():
            self.line(f'return Client(client, channel_id).{method.name()}(')

            args = []

            if not method.client_streaming():
                args.append('request')

            if method.server_streaming():
                args.append('std::move(on_next)')

            self.indented_list(
                *args,
                'std::move(on_completed)',
                'std::move(on_error)',
                end=');',
            )

        self.line('}')

    def method_info_specialization(self, method: ProtoServiceMethod) -> None:
        self.line()
        # We have Request/Response as voids to mark raw as a special case.
        # Raw operates in ConstByteSpans, which won't be copied by copying the
        # span itself and without special treatment will lead to dangling
        # pointers.
        #
        # Helpers/traits that want to use Request/Response and should support
        # raw are required to do a special implementation for them instead that
        # will copy the actual data.
        self.line('using Request = void;')
        self.line('using Response = void;')


class StubGenerator(codegen.StubGenerator):
    """TODO(frolv) Add docstring."""

    def unary_signature(self, method: ProtoServiceMethod, prefix: str) -> str:
        return (
            f'void {prefix}{method.name()}(pw::ConstByteSpan request, '
            'pw::rpc::RawUnaryResponder& responder)'
        )

    def unary_stub(
        self, method: ProtoServiceMethod, output: OutputFile
    ) -> None:
        output.write_line(codegen.STUB_REQUEST_TODO)
        output.write_line('static_cast<void>(request);')
        output.write_line(codegen.STUB_RESPONSE_TODO)
        output.write_line('static_cast<void>(responder);')

    def server_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:

        return (
            f'void {prefix}{method.name()}('
            'pw::ConstByteSpan request, RawServerWriter& writer)'
        )

    def client_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return f'void {prefix}{method.name()}(RawServerReader& reader)'

    def bidirectional_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}('
            'RawServerReaderWriter& reader_writer)'
        )


def process_proto_file(proto_file) -> Iterable[OutputFile]:
    """Generates code for a single .proto file."""

    _, package_root = build_node_tree(proto_file)
    output_filename = _proto_filename_to_generated_header(proto_file.name)

    generator = RawCodeGenerator(output_filename)
    codegen.generate_package(proto_file, package_root, generator)

    codegen.package_stubs(package_root, generator, StubGenerator())

    return [generator.output]
