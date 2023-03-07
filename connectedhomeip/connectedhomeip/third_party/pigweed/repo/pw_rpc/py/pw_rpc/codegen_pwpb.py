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
"""This module generates the code for pw_protobuf pw_rpc services."""

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

PROTO_H_EXTENSION = '.pwpb.h'
PWPB_H_EXTENSION = '.pwpb.h'


def _proto_filename_to_pwpb_header(proto_file: str) -> str:
    """Returns the generated pwpb header name for a .proto file."""
    filename = os.path.splitext(proto_file)[0]
    return f'{filename}{PWPB_H_EXTENSION}'


def _proto_filename_to_generated_header(proto_file: str) -> str:
    """Returns the generated C++ RPC header name for a .proto file."""
    filename = os.path.splitext(proto_file)[0]
    return f'{filename}.rpc{PROTO_H_EXTENSION}'


def _serde(method: ProtoServiceMethod) -> str:
    """Returns the PwpbMethodSerde for this method."""
    return (
        f'{RPC_NAMESPACE}::internal::kPwpbMethodSerde<'
        f'&{method.request_type().pwpb_table()}, '
        f'&{method.response_type().pwpb_table()}>'
    )


def _client_call(method: ProtoServiceMethod) -> str:
    template_args = []

    if method.client_streaming():
        template_args.append(method.request_type().pwpb_struct())

    template_args.append(method.response_type().pwpb_struct())

    return f'{client_call_type(method, "Pwpb")}<{", ".join(template_args)}>'


def _function(method: ProtoServiceMethod) -> str:
    return f'{_client_call(method)} {method.name()}'


def _user_args(method: ProtoServiceMethod) -> Iterable[str]:
    if not method.client_streaming():
        yield f'const {method.request_type().pwpb_struct()}& request'

    response = method.response_type().pwpb_struct()

    if method.server_streaming():
        yield f'::pw::Function<void(const {response}&)>&& on_next = nullptr'
        yield '::pw::Function<void(::pw::Status)>&& on_completed = nullptr'
    else:
        yield (
            f'::pw::Function<void(const {response}&, ::pw::Status)>&& '
            'on_completed = nullptr'
        )

    yield '::pw::Function<void(::pw::Status)>&& on_error = nullptr'


class PwpbCodeGenerator(CodeGenerator):
    """Generates an RPC service and client using the pw_protobuf API."""

    def name(self) -> str:
        return 'pwpb'

    def method_union_name(self) -> str:
        return 'PwpbMethodUnion'

    def includes(self, proto_file_name: str) -> Iterable[str]:
        yield '#include "pw_rpc/pwpb/client_reader_writer.h"'
        yield '#include "pw_rpc/pwpb/internal/method_union.h"'
        yield '#include "pw_rpc/pwpb/server_reader_writer.h"'

        # Include the corresponding pwpb header file for this proto file, in
        # which the file's messages and enums are generated. All other files
        # imported from the .proto file are #included in there.
        pwpb_header = _proto_filename_to_pwpb_header(proto_file_name)
        yield f'#include "{pwpb_header}"'

    def service_aliases(self) -> None:
        self.line('template <typename Response>')
        self.line(
            'using ServerWriter = '
            f'{RPC_NAMESPACE}::PwpbServerWriter<Response>;'
        )
        self.line('template <typename Request, typename Response>')
        self.line(
            'using ServerReader = '
            f'{RPC_NAMESPACE}::PwpbServerReader<Request, Response>;'
        )
        self.line('template <typename Request, typename Response>')
        self.line(
            'using ServerReaderWriter = '
            f'{RPC_NAMESPACE}::PwpbServerReaderWriter<Request, Response>;'
        )

    def method_descriptor(self, method: ProtoServiceMethod) -> None:
        impl_method = f'&Implementation::{method.name()}'

        self.line(
            f'{RPC_NAMESPACE}::internal::GetPwpbOrRawMethodFor<{impl_method}, '
            f'{method.type().cc_enum()}, '
            f'{method.request_type().pwpb_struct()}, '
            f'{method.response_type().pwpb_struct()}>('
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
                f'Pwpb{base}ResponseClientCall<'
                f'{method.response_type().pwpb_struct()}>::'
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
        self.line(f'using Request = {method.request_type().pwpb_struct()};')
        self.line(f'using Response = {method.response_type().pwpb_struct()};')
        self.line()
        self.line(
            f'static constexpr const {RPC_NAMESPACE}::'
            'PwpbMethodSerde& serde() {'
        )
        with self.indent():
            self.line(f'return {_serde(method)};')
        self.line('}')


class StubGenerator(codegen.StubGenerator):
    """Generates pw_protobuf RPC stubs."""

    def unary_signature(self, method: ProtoServiceMethod, prefix: str) -> str:
        return (
            f'::pw::Status {prefix}{method.name()}( '
            f'const {method.request_type().pwpb_struct()}& request, '
            f'{method.response_type().pwpb_struct()}& response)'
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
            f'const {method.request_type().pwpb_struct()}& request, '
            f'ServerWriter<{method.response_type().pwpb_struct()}>& writer)'
        )

    def client_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}( '
            f'ServerReader<{method.request_type().pwpb_struct()}, '
            f'{method.response_type().pwpb_struct()}>& reader)'
        )

    def bidirectional_streaming_signature(
        self, method: ProtoServiceMethod, prefix: str
    ) -> str:
        return (
            f'void {prefix}{method.name()}( '
            f'ServerReaderWriter<{method.request_type().pwpb_struct()}, '
            f'{method.response_type().pwpb_struct()}>& reader_writer)'
        )


def process_proto_file(proto_file) -> Iterable[OutputFile]:
    """Generates code for a single .proto file."""

    _, package_root = build_node_tree(proto_file)
    output_filename = _proto_filename_to_generated_header(proto_file.name)

    generator = PwpbCodeGenerator(output_filename)
    codegen.generate_package(proto_file, package_root, generator)

    codegen.package_stubs(package_root, generator, StubGenerator())

    return [generator.output]
