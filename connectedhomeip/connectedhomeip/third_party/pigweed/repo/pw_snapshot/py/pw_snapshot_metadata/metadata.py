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
"""Library to assist processing Snapshot Metadata protos into text"""

from typing import Optional, List, Mapping
import pw_log_tokenized
import pw_tokenizer
from pw_tokenizer import proto as proto_detokenizer
from pw_snapshot_metadata_proto import snapshot_metadata_pb2

_PRETTY_FORMAT_DEFAULT_WIDTH = 80

_FATAL = (
    '▪▄▄▄ ▄▄▄· ▄▄▄▄▄ ▄▄▄· ▄ ·',
    '█▄▄▄▐█ ▀█ • █▌ ▐█ ▀█ █  ',
    '█ ▪ ▄█▀▀█   █. ▄█▀▀█ █  ',
    '▐▌ .▐█ ▪▐▌ ▪▐▌·▐█ ▪▐▌▐▌ ',
    '▀    ▀  ▀ ·  ▀  ▀  ▀ .▀▀',
)


def _process_tags(tags: Mapping[str, str]) -> Optional[str]:
    """Outputs snapshot tags as a multi-line string."""
    if not tags:
        return None

    output: List[str] = ['Tags:']
    for key, value in tags.items():
        output.append(f'  {key}: {value}')

    return '\n'.join(output)


def process_snapshot(
    serialized_snapshot: bytes, tokenizer_db: Optional[pw_tokenizer.Detokenizer]
) -> str:
    """Processes snapshot metadata and tags, producing a multi-line string."""
    snapshot = snapshot_metadata_pb2.SnapshotBasicInfo()
    snapshot.ParseFromString(serialized_snapshot)

    output: List[str] = []

    if snapshot.HasField('metadata'):
        output.extend(
            (
                str(MetadataProcessor(snapshot.metadata, tokenizer_db)),
                '',
            )
        )

    if snapshot.tags:
        tags = _process_tags(snapshot.tags)
        if tags:
            output.append(tags)
        # Trailing blank line for spacing.
        output.append('')

    return '\n'.join(output)


class MetadataProcessor:
    """This class simplifies dumping contents of a snapshot Metadata message."""

    def __init__(
        self,
        metadata: snapshot_metadata_pb2.Metadata,
        tokenizer_db: Optional[pw_tokenizer.Detokenizer],
    ):
        self._metadata = metadata
        self._tokenizer_db = (
            tokenizer_db
            if tokenizer_db is not None
            else pw_tokenizer.Detokenizer(None)
        )
        self._reason_token = self._tokenizer_db.detokenize(
            metadata.reason
        ).token
        self._format_width = _PRETTY_FORMAT_DEFAULT_WIDTH
        proto_detokenizer.detokenize_fields(self._tokenizer_db, self._metadata)

    def is_fatal(self) -> bool:
        return self._metadata.fatal

    def reason(self) -> str:
        if not self._metadata.reason:
            return 'UNKNOWN (field missing)'

        log = pw_log_tokenized.FormatStringWithMetadata(
            self._metadata.reason.decode()
        )

        return f'{log.file}: {log.message}' if log.file else log.message

    def reason_token(self) -> Optional[int]:
        """If the snapshot `reason` is tokenized, the value of the token."""
        return self._reason_token

    def project_name(self) -> str:
        return self._metadata.project_name.decode()

    def device_name(self) -> str:
        return self._metadata.device_name.decode()

    def device_fw_version(self) -> str:
        return self._metadata.software_version

    def snapshot_uuid(self) -> str:
        return self._metadata.snapshot_uuid.hex()

    def fw_build_uuid(self) -> str:
        return self._metadata.software_build_uuid.hex()

    def set_pretty_format_width(self, width: int):
        """Sets the centered width of the FATAL text for a formatted output."""
        self._format_width = width

    def __str__(self) -> str:
        """outputs a pw.snapshot.Metadata proto as a multi-line string."""
        output: List[str] = []
        if self._metadata.fatal:
            output.extend(
                (
                    *[x.center(self._format_width).rstrip() for x in _FATAL],
                    '',
                    'Device crash cause:',
                )
            )
        else:
            output.append('Snapshot capture reason:')

        output.extend(
            (
                '    ' + self.reason(),
                '',
            )
        )
        if self.reason_token():
            output.append(f'Reason token:      0x{self.reason_token():x}')

        if self._metadata.project_name:
            output.append(f'Project name:      {self.project_name()}')

        if self._metadata.device_name:
            output.append(f'Device:            {self.device_name()}')

        if self._metadata.software_version:
            output.append(f'Device FW version: {self.device_fw_version()}')

        if self._metadata.software_build_uuid:
            output.append(f'FW build UUID:     {self.fw_build_uuid()}')

        if self._metadata.snapshot_uuid:
            output.append(f'Snapshot UUID:     {self.snapshot_uuid()}')

        return '\n'.join(output)
