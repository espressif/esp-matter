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
"""Unit tests for pw_software_update/cli.py."""

import unittest
from pw_software_update import cli, metadata, update_bundle
from pw_software_update.tuf_pb2 import TargetsMetadata


class AddFileToBundleTest(unittest.TestCase):
    """Test adding a target file to an existing bundle"""

    def test_adding_file_to_bundle(self):
        """Adds a file to bundle"""

        bundle = update_bundle.gen_empty_update_bundle()

        target_payloads = {
            'foo': b'foo contents',
            'bar': b'bar contents',
        }

        for name, contents in target_payloads.items():
            bundle = cli.add_file_to_bundle(
                bundle=bundle, file_name=name, file_contents=contents
            )

        # Checks for existence of target in target payloads
        self.assertEqual(target_payloads['foo'], bundle.target_payloads['foo'])
        self.assertEqual(
            len(bundle.target_payloads['foo']), len(target_payloads['foo'])
        )

        self.assertEqual(target_payloads['bar'], bundle.target_payloads['bar'])
        self.assertEqual(
            len(bundle.target_payloads['bar']), len(target_payloads['bar'])
        )

    def test_adding_duplicate_file_fails(self):
        """Test for adding a duplicate target name to bundle"""

        bundle = update_bundle.gen_empty_update_bundle()

        target_payloads = {
            'foo': b'foo contents',
        }

        for name, contents in target_payloads.items():
            bundle = cli.add_file_to_bundle(
                bundle=bundle, file_name=name, file_contents=contents
            )

        # Checks for raised exceptions when adding a duplicate file name
        # in target payload
        with self.assertRaises(Exception):
            bundle = cli.add_file_to_bundle(
                bundle=bundle, file_name='foo', file_contents=b'does not matter'
            )

    def test_adding_duplicate_target_file_fails(self):
        """Test for adding a duplicate target file to bundle"""

        bundle = update_bundle.gen_empty_update_bundle()

        target_payloads = {'foo': b'asrgfdasrgfdasrgfdasrgfd'}

        for name, contents in target_payloads.items():
            bundle = cli.add_file_to_bundle(
                bundle=bundle, file_name=name, file_contents=contents
            )

        # Adding a target file with no matching target_payload file name
        target_file = metadata.gen_target_file('shoo', b'cvbfbzbz')

        signed_targets_metadata = bundle.targets_metadata['targets']
        targets_metadata = TargetsMetadata().FromString(
            signed_targets_metadata.serialized_targets_metadata
        )

        targets_metadata.target_files.append(target_file)
        bundle.targets_metadata[
            'targets'
        ].serialized_targets_metadata = targets_metadata.SerializeToString()

        # Checks for raised exception for duplicate target file
        with self.assertRaises(Exception):
            bundle = cli.add_file_to_bundle(
                bundle=bundle, file_name='shoo', file_contents=b'cvbfbzbz'
            )


if __name__ == '__main__':
    unittest.main()
