#!/usr/bin/env python3
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
"""Tests for snapshot metadata processing."""

import base64
import unittest
import pw_tokenizer
from pw_snapshot_metadata.metadata import MetadataProcessor, process_snapshot
from pw_snapshot_protos import snapshot_pb2
from pw_tokenizer import tokens


class MetadataProcessorTest(unittest.TestCase):
    """Tests that the metadata processor produces expected results."""

    def setUp(self):
        super().setUp()
        self.detok = pw_tokenizer.Detokenizer(
            tokens.Database(
                [
                    tokens.TokenizedStringEntry(
                        0x3A9BC4C3, 'Assert failed: 1+1 == 42'
                    ),
                    tokens.TokenizedStringEntry(0x01170923, 'gShoe'),
                ]
            )
        )

        snapshot = snapshot_pb2.Snapshot()
        snapshot.metadata.reason = b'\xc3\xc4\x9b\x3a'
        snapshot.metadata.project_name = b'$' + base64.b64encode(
            b'\x23\x09\x17\x01'
        )
        snapshot.metadata.device_name = b'hyper-fast-gshoe'
        snapshot.metadata.software_version = 'gShoe-debug-1.2.1-6f23412b+'
        snapshot.metadata.snapshot_uuid = b'\x00\x00\x00\x01'

        self.snapshot = snapshot

    def test_reason_tokenized(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(meta.reason(), 'Assert failed: 1+1 == 42')

    def test_reason_log_format(self):
        self.snapshot.metadata.reason = (
            '■msg♦Assert failed :(' '■file♦rpc_services/crash.cc'
        ).encode('utf-8')
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(
            meta.reason(), 'rpc_services/crash.cc: Assert failed :('
        )

    def test_project_name_tokenized(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(meta.project_name(), 'gShoe')

    def test_device_name_not_tokenized(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(meta.device_name(), 'hyper-fast-gshoe')

    def test_default_non_fatal(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertFalse(meta.is_fatal())

    def test_fw_version(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(
            meta.device_fw_version(), 'gShoe-debug-1.2.1-6f23412b+'
        )

    def test_snapshot_uuid(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(meta.snapshot_uuid(), '00000001')

    def test_fw_uuid_default(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        self.assertEqual(meta.fw_build_uuid(), '')

    def test_as_str(self):
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        expected = '\n'.join(
            (
                'Snapshot capture reason:',
                '    Assert failed: 1+1 == 42',
                '',
                'Reason token:      0x3a9bc4c3',
                'Project name:      gShoe',
                'Device:            hyper-fast-gshoe',
                'Device FW version: gShoe-debug-1.2.1-6f23412b+',
                'Snapshot UUID:     00000001',
            )
        )
        self.assertEqual(expected, str(meta))

    def test_as_str_fatal(self):
        self.snapshot.metadata.fatal = True
        meta = MetadataProcessor(self.snapshot.metadata, self.detok)
        expected = '\n'.join(
            (
                '                            ▪▄▄▄ ▄▄▄· ▄▄▄▄▄ ▄▄▄· ▄ ·',
                '                            █▄▄▄▐█ ▀█ • █▌ ▐█ ▀█ █',
                '                            █ ▪ ▄█▀▀█   █. ▄█▀▀█ █',
                '                            ▐▌ .▐█ ▪▐▌ ▪▐▌·▐█ ▪▐▌▐▌',
                '                            ▀    ▀  ▀ ·  ▀  ▀  ▀ .▀▀',
                '',
                'Device crash cause:',
                '    Assert failed: 1+1 == 42',
                '',
                'Reason token:      0x3a9bc4c3',
                'Project name:      gShoe',
                'Device:            hyper-fast-gshoe',
                'Device FW version: gShoe-debug-1.2.1-6f23412b+',
                'Snapshot UUID:     00000001',
            )
        )
        self.assertEqual(expected, str(meta))

    def test_no_reason(self):
        snapshot = snapshot_pb2.Snapshot()
        snapshot.metadata.fatal = True
        meta = MetadataProcessor(snapshot.metadata, self.detok)
        meta.set_pretty_format_width(40)
        expected = '\n'.join(
            (
                '        ▪▄▄▄ ▄▄▄· ▄▄▄▄▄ ▄▄▄· ▄ ·',
                '        █▄▄▄▐█ ▀█ • █▌ ▐█ ▀█ █',
                '        █ ▪ ▄█▀▀█   █. ▄█▀▀█ █',
                '        ▐▌ .▐█ ▪▐▌ ▪▐▌·▐█ ▪▐▌▐▌',
                '        ▀    ▀  ▀ ·  ▀  ▀  ▀ .▀▀',
                '',
                'Device crash cause:',
                '    UNKNOWN (field missing)',
                '',
            )
        )
        self.assertEqual(expected, str(meta))

    def test_serialized_snapshot(self):
        self.snapshot.tags['type'] = 'obviously a crash'
        expected = '\n'.join(
            (
                'Snapshot capture reason:',
                '    Assert failed: 1+1 == 42',
                '',
                'Reason token:      0x3a9bc4c3',
                'Project name:      gShoe',
                'Device:            hyper-fast-gshoe',
                'Device FW version: gShoe-debug-1.2.1-6f23412b+',
                'Snapshot UUID:     00000001',
                '',
                'Tags:',
                '  type: obviously a crash',
                '',
            )
        )
        self.assertEqual(
            expected,
            process_snapshot(self.snapshot.SerializeToString(), self.detok),
        )


if __name__ == '__main__':
    unittest.main()
