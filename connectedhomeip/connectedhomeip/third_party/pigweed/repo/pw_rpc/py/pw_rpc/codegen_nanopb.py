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
"""This module generates the code for nanopb-based pw_rpc services."""

import os
from typing import Iterable, NamedTuple, Optional

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
PROTO_CC_EXTENSION = '.pb.cc'
NANOPB_H_EXTENSION = '.pb.h'


def _serde(method: ProtoServiceMethod) -> str:
    """Returns the NanopbMethodSerde for this method."""
    return (
        f'{RPC_NAMESPACE}::internal::kNanopbMethodSerde<'
        f'{method.request_type().nanopb_fields()}, '
        f'{method.response_type().nanopb_fields()}>'
    )


def _proto_filename_to_nanopb_header(proto_file: str) -> str:
    """Returns the generated nanopb header name for a .proto file."""
    return os.path.splitext(proto_file)[0] + NANOPB_H_EXTENSION


def _proto_filename_to_generated_header(proto_file: str) -> str:
    """Returns the generated C++ RPC header name for a .proto file."""
    filename = os.path.splitext(proto_file)[0]
    return f'{filename}.rpc{PROTO_H_EXTENSION}'


def _client_call(method: ProtoServiceMethod) -> str:
    template_args = []

    if method.client_streaming():
        template_args.append(method.request_type().nanopb_struct())

    template_args.append(method.response_type().nanopb_struct())

    return f'{client_call_type(method, "Nanopb")}<{", ".join(template_args)}>'


def _function(method: ProtoServiceMethod) -> str:
    return f'{_client_call(method)} {method.name()}'


def _user_args(method: ProtoServiceMethod) -> Iterable[str]:
    if not method.client_streaming():
        yield f'const {method.request_type().nanopb_struct()}& request'

    response = method.response_type().nanopb_struct()

    if method.server_streaming():
        yield f'::pw::Function<void(const {response}&)>&& on_next = nullptr'
        yield '::pw::Function<void(::pw::Status)>&& on_completed = nullptr'
    else:
        yield (
            f'::pw::Function<void(const {response}&, ::pw::Status)>&& '
            'on_completed = nullptr'
        )

    yield '::pw::Function<void(::pw::Status)>&& on_error = nullptr'


class NanopbCodeGenerator(CodeGenerator):
    """Generates an RPC service and client using the Nanopb API."""

    def name(self) -> str:
        return 'nanopb'

    def method_union_name(self) -> str:
        return 'NanopbMethodUnion'

    def includes(self, proto_file_name: str) -> Iterable[str]:
        yield '#include "pw_rpc/nanopb/client_reader_writer.h"'
        yield '#include "pw_rpc/nanopb/internal/method_union.h"'
        yield '#include "pw_rpc/nanopb/server_reader_writer.h"'

        # Include the corresponding nanopb header file for this proto file, in
        # which the file's messages and enums are generated. All other files
        # imported from the .proto file are #included in there.
        nanopb_header = _proto_filename_to_nanopb_header(proto_file_name)
        yield f'#include "{nanopb_header}"'

    def service_aliases(self) -> None:
        self.line('template <typename Response>')
        self.line(
            'using ServerWriter = '
            f'{RPC_NAMESPACE}::NanopbServerWriter<Response>;'
        )
        self.line('template <typename Request, typename Response>')
        self.line(
            'using ServerReader = '
            f'{RPC_NAMESPACE}::NanopbServerReader<Request, Response>;'
        )
        self.line('template <typename Request, typename Response>')
        self.line(
            'using ServerReaderWriter = '
            f'{RPC_NAMESPACE}::NanopbServerReaderWriter<Request, Response>;'
        )

    def method_descriptor(self, method: ProtoServiceMethod) -> None:
        self.line(
            f'{RPC_NAMESPACE}::internal::'
            f'GetNanopbOrRawMethodFor<&Implementation::{method.name()}, '
            f'{method.type().cc_enum()}, '
            f'{method.request_type().nanopb_struct()}, '
            f'{method.response_type().nanopb_struct()}>('
        )
        with self.indent(4):
            self.line(f'{get_id(method)},  // Hash of "{method.name()}"')
            self.line(f'{_serde(method)}),')

    def client_member_function(self, method: ProtoServiceMethod) -> None:
        """Outputs client code for a single RPC method."""

        self.line(f'{_function(method)}(')
        self.indented_list(*_user_args(method), end=') const {')

        with self.indent():
            client_call = _client_call(method)
            base = 'Stream' if method.server_streaming() else 'Unary'
            self.line(
                f'return {RPC_NAMESPACE}::internal::'
                f'Nanopb{base}ResponseClientCall<'
                f'{method.response_type().nanopb_struct()}>::'
                f'Start<{client_call}>('
            )

            service_client = RPC_NAMESPACE + '::internal::ServiceClient'

            args = [
                f'{service_client}::client()',
                f'{service_client}::channel_id()',
                'kServiceId',
                get_id(method),
                _serde(method),
            ]
            if method.server_streaming():
                args.append('std::move(on_next)')

            args.append('std::move(on_completed)')
            args.append('std::move(on_error)')

            if not method.client_streaming():
                args.append('request')

            self.indented_list(*args, end=');')

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
        self.line(f'using Request = {method.request_type().nanopb_struct()};')
        self.line(f'using Response = {method.response_type().nanopb_struct()};')
        self.line()
        self.line(
            f'static constexpr const {RPC_NAMESPACE}::internal::'
            'NanopbMethodSerde& serde() {'
        )
        with self.indent():
            self.line(f'return {_serde(method)};')
        self.line('}')


class _CallbackFunction(NamedTuple):
    """Represents a callback function parameter in a client RPC call."""

    function_type: str
    name: str
    default_value: Optional[str] = None

    def __str__(self):
        param = f'::pw::Function<{self.function_type}>&& {self.name}'
        if self.default_value:
            param += f' = {self.default_value}'
        return param


class StubGenerator(codegen.StubGenerator):
    """Generates Nanopb RPC stubs."""

    def unary_signature(self, method: ProtoServiceMethod, prefix: str) -> str:
        return (
            f'::pw::Status {prefix}{method.name()}( '
            f'const {method.request_type().nanopb_struct()}& request, '
            f'{method.response_type().nanopb_struct()}& response)'
        )

    def unary_stub(
        self, method: ProtoServiceMethod, output: OutputFile
    ) -> None:
        output.write_line(codegen.STUB_REQUEST_TODO)
        output.write_line('static_cast<void>(request);')
        output.write_line(codegen.STUB_RESPONSE_TODO)
        output.write_line('static_cast<void>(response);')
        output.write_line('return ::pw::Status::Unimplemented();')

    def server_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}( '
            f'const {method.request_type().nanopb_struct()}& request, '
            f'ServerWriter<{method.response_type().nanopb_struct()}>& writer)'
        )

    def client_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}( '
            f'ServerReader<{method.request_type().nanopb_struct()}, '
            f'{method.response_type().nanopb_struct()}>& reader)'
        )

    def bidirectional_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}( '
            f'ServerReaderWriter<{method.request_type().nanopb_struct()}, '
            f'{method.response_type().nanopb_struct()}>& reader_writer)'
        )


def process_proto_file(proto_file) -> Iterable[OutputFile]:
    """Generates code for a single .proto file."""

    _, package_root = build_node_tree(proto_file)
    output_filename = _proto_filename_to_generated_header(proto_file.name)
    generator = NanopbCodeGenerator(output_filename)
    codegen.generate_package(proto_file, package_root, generator)

    codegen.package_stubs(package_root, generator, StubGenerator())

    return [generator.output]
