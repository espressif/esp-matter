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
"""pw_protobuf compiler plugin.

This file implements a protobuf compiler plugin which generates C++ headers for
protobuf messages in the pw_protobuf format.
"""

import sys
from argparse import ArgumentParser, Namespace
from pathlib import Path
from shlex import shlex

from google.protobuf.compiler import plugin_pb2

from pw_protobuf import codegen_pwpb, options


def parse_parameter_options(parameter: str) -> Namespace:
    """Parses parameters passed through from protoc.

    These parameters come in via passing `--${NAME}_out` parameters to protoc,
    where protoc-gen-${NAME} is the supplied name of the plugin. At time of
    writing, Blaze uses --pwpb_opt, whereas the script for GN uses --custom_opt.
    """
    parser = ArgumentParser()
    parser.add_argument(
        '-I',
        '--include-path',
        dest='include_paths',
        metavar='DIR',
        action='append',
        default=[],
        type=Path,
        help='Append DIR to options file search path',
    )
    parser.add_argument(
        '--no-legacy-namespace',
        dest='no_legacy_namespace',
        action='store_true',
        help='If set, suppresses `using namespace` declarations, which '
        'disallows use of the legacy non-prefixed namespace',
    )

    # protoc passes the custom arguments in shell quoted form, separated by
    # commas. Use shlex to split them, correctly handling quoted sections, with
    # equivalent options to IFS=","
    lex = shlex(parameter)
    lex.whitespace_split = True
    lex.whitespace = ','
    lex.commenters = ''
    args = list(lex)

    return parser.parse_args(args)


def process_proto_request(
    req: plugin_pb2.CodeGeneratorRequest, res: plugin_pb2.CodeGeneratorResponse
) -> None:
    """Handles a protoc CodeGeneratorRequest message.

    Generates code for the files in the request and writes the output to the
    specified CodeGeneratorResponse message.

    Args:
      req: A CodeGeneratorRequest for a proto compilation.
      res: A CodeGeneratorResponse to populate with the plugin's output.
    """
    args = parse_parameter_options(req.parameter)
    for proto_file in req.proto_file:
        proto_options = options.load_options(
            args.include_paths, Path(proto_file.name)
        )
        output_files = codegen_pwpb.process_proto_file(
            proto_file,
            proto_options,
            suppress_legacy_namespace=args.no_legacy_namespace,
        )
        for output_file in output_files:
            fd = res.file.add()
            fd.name = output_file.name()
            fd.content = output_file.content()


def main() -> int:
    """Protobuf compiler plugin entrypoint.

    Reads a CodeGeneratorRequest proto from stdin and writes a
    CodeGeneratorResponse to stdout.
    """
    data = sys.stdin.buffer.read()
    request = plugin_pb2.CodeGeneratorRequest.FromString(data)
    response = plugin_pb2.CodeGeneratorResponse()
    process_proto_request(request, response)

    # Declare that this plugin supports optional fields in proto3.
    response.supported_features |= (  # type: ignore[attr-defined]
        response.FEATURE_PROTO3_OPTIONAL
    )  # type: ignore[attr-defined]

    sys.stdout.buffer.write(response.SerializeToString())
    return 0


if __name__ == '__main__':
    sys.exit(main())
