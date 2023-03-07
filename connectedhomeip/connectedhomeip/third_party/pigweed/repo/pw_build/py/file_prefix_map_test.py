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
"""Tests for the file_prefix_map utility"""

from io import StringIO
import json
import unittest

from pw_build import file_prefix_map

# pylint: disable=line-too-long
JSON_SOURCE_FILES = json.dumps(
    [
        "../pigweed/pw_polyfill/standard_library_public/pw_polyfill/standard_library/assert.h",
        "protocol_buffer/gen/pigweed/pw_protobuf/common_protos.proto_library/nanopb/pw_protobuf_protos/status.pb.h",
        "../pigweed/pw_rpc/client_server.cc",
        "../pigweed/pw_rpc/public/pw_rpc/client_server.h",
        "/home/user/pigweed/out/../gen/generated_build_info.cc",
        "/home/user/pigweed/pw_protobuf/encoder.cc",
    ]
)

JSON_PATH_TRANSFORMATIONS = json.dumps(
    [
        "/home/user/pigweed/out=out",
        "/home/user/pigweed/=",
        "../=",
        "/home/user/pigweed/out=out",
    ]
)

EXPECTED_TRANSFORMED_PATHS = json.dumps(
    [
        "pigweed/pw_polyfill/standard_library_public/pw_polyfill/standard_library/assert.h",
        "protocol_buffer/gen/pigweed/pw_protobuf/common_protos.proto_library/nanopb/pw_protobuf_protos/status.pb.h",
        "pigweed/pw_rpc/client_server.cc",
        "pigweed/pw_rpc/public/pw_rpc/client_server.h",
        "out/../gen/generated_build_info.cc",
        "pw_protobuf/encoder.cc",
    ]
)


class FilePrefixMapTest(unittest.TestCase):
    def test_prefix_remap(self):
        path_list = [
            '/foo_root/root_subdir/source.cc',
            '/foo_root/root_subdir/out/../gen.cc',
        ]
        prefix_maps = [('/foo_root/root_subdir/', ''), ('out/../', 'out/')]
        expected_paths = ['source.cc', 'out/gen.cc']
        self.assertEqual(
            list(file_prefix_map.remap_paths(path_list, prefix_maps)),
            expected_paths,
        )

    def test_json_prefix_map(self):
        in_fd = StringIO(JSON_SOURCE_FILES)
        prefix_map_fd = StringIO(JSON_PATH_TRANSFORMATIONS)
        out_fd = StringIO()
        file_prefix_map.remap_json_paths(in_fd, out_fd, prefix_map_fd)
        self.assertEqual(
            json.loads(out_fd.getvalue()),
            json.loads(EXPECTED_TRANSFORMED_PATHS),
        )


if __name__ == '__main__':
    unittest.main()
