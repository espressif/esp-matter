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
"""Device classes to interact with targets via RPC."""

import datetime
import logging
from pathlib import Path
from types import ModuleType
from typing import Any, Callable, List, Union, Optional

from pw_hdlc.rpc import HdlcRpcClient, default_channels
from pw_log_tokenized import FormatStringWithMetadata
from pw_log.proto import log_pb2
from pw_metric import metric_parser
from pw_rpc import callback_client, console_tools
from pw_status import Status
from pw_thread.thread_analyzer import ThreadSnapshotAnalyzer
from pw_thread_protos import thread_pb2
from pw_tokenizer import detokenize
from pw_tokenizer.proto import decode_optionally_tokenized
from pw_unit_test.rpc import run_tests as pw_unit_test_run_tests

# Internal log for troubleshooting this tool (the console).
_LOG = logging.getLogger('tools')
DEFAULT_DEVICE_LOGGER = logging.getLogger('rpc_device')


class Device:
    """Represents an RPC Client for a device running a Pigweed target.

    The target must have and RPC support, RPC logging.
    Note: use this class as a base for specialized device representations.
    """

    def __init__(
        self,
        channel_id: int,
        read,
        write,
        proto_library: List[Union[ModuleType, Path]],
        detokenizer: Optional[detokenize.Detokenizer],
        timestamp_decoder: Optional[Callable[[int], str]],
        rpc_timeout_s: float = 5,
        use_rpc_logging: bool = True,
    ):
        self.channel_id = channel_id
        self.protos = proto_library
        self.detokenizer = detokenizer
        self.rpc_timeout_s = rpc_timeout_s

        self.logger = DEFAULT_DEVICE_LOGGER
        self.logger.setLevel(logging.DEBUG)  # Allow all device logs through.
        self.timestamp_decoder = timestamp_decoder
        self._expected_log_sequence_id = 0

        callback_client_impl = callback_client.Impl(
            default_unary_timeout_s=self.rpc_timeout_s,
            default_stream_timeout_s=None,
        )

        def detokenize_and_log_output(data: bytes, _detokenizer=None):
            log_messages = data.decode(
                encoding='utf-8', errors='surrogateescape'
            )

            if self.detokenizer:
                log_messages = decode_optionally_tokenized(
                    self.detokenizer, data
                )

            for line in log_messages.splitlines():
                self.logger.info(line)

        self.client = HdlcRpcClient(
            read,
            self.protos,
            default_channels(write),
            detokenize_and_log_output,
            client_impl=callback_client_impl,
        )

        if use_rpc_logging:
            # Start listening to logs as soon as possible.
            self.listen_to_log_stream()

    def info(self) -> console_tools.ClientInfo:
        return console_tools.ClientInfo('device', self.rpcs, self.client.client)

    @property
    def rpcs(self) -> Any:
        """Returns an object for accessing services on the specified channel."""
        return next(iter(self.client.client.channels())).rpcs

    def run_tests(self, timeout_s: Optional[float] = 5) -> bool:
        """Runs the unit tests on this device."""
        return pw_unit_test_run_tests(self.rpcs, timeout_s=timeout_s)

    def listen_to_log_stream(self):
        """Opens a log RPC for the device's unrequested log stream.

        The RPCs remain open until the server cancels or closes them, either
        with a response or error packet.
        """
        self.rpcs.pw.log.Logs.Listen.open(
            on_next=lambda _, log_entries_proto: self._log_entries_proto_parser(
                log_entries_proto
            ),
            on_completed=lambda _, status: _LOG.info(
                'Log stream completed with status: %s', status
            ),
            on_error=lambda _, error: self._handle_log_stream_error(error),
        )

    def _handle_log_stream_error(self, error: Status):
        """Resets the log stream RPC on error to avoid losing logs."""
        _LOG.error('Log stream error: %s', error)

        # Only re-request logs if the RPC was not cancelled by the client.
        if error != Status.CANCELLED:
            self.listen_to_log_stream()

    def _handle_log_drop_count(self, drop_count: int, reason: str):
        log_text = 'log' if drop_count == 1 else 'logs'
        message = f'Dropped {drop_count} {log_text} due to {reason}'
        self._emit_device_log(logging.WARNING, '', '', message)

    def _check_for_dropped_logs(self, log_entries_proto: log_pb2.LogEntries):
        # Count log messages received that don't use the dropped field.
        messages_received = sum(
            1 if not log_proto.dropped else 0
            for log_proto in log_entries_proto.entries
        )
        dropped_log_count = (
            log_entries_proto.first_entry_sequence_id
            - self._expected_log_sequence_id
        )
        self._expected_log_sequence_id = (
            log_entries_proto.first_entry_sequence_id + messages_received
        )
        if dropped_log_count > 0:
            self._handle_log_drop_count(dropped_log_count, 'loss at transport')
        elif dropped_log_count < 0:
            _LOG.error('Log sequence ID is smaller than expected')

    def _log_entries_proto_parser(self, log_entries_proto: log_pb2.LogEntries):
        self._check_for_dropped_logs(log_entries_proto)
        for log_proto in log_entries_proto.entries:
            decoded_timestamp = self.decode_timestamp(log_proto.timestamp)
            # Parse level and convert to logging module level number.
            level = (log_proto.line_level & 0x7) * 10
            if self.detokenizer:
                message = str(
                    decode_optionally_tokenized(
                        self.detokenizer, log_proto.message
                    )
                )
            else:
                message = log_proto.message.decode('utf-8')
            log = FormatStringWithMetadata(message)

            # Handle dropped count.
            if log_proto.dropped:
                drop_reason = (
                    log_proto.message.decode('utf-8').lower()
                    if log_proto.message
                    else 'enqueue failure on device'
                )
                self._handle_log_drop_count(log_proto.dropped, drop_reason)
                continue
            self._emit_device_log(
                level,
                decoded_timestamp,
                log.module,
                log.message,
                **dict(log.fields),
            )

    def _emit_device_log(
        self,
        level: int,
        timestamp: str,
        module_name: str,
        message: str,
        **metadata_fields,
    ):
        # Fields used for console table view
        fields = metadata_fields
        fields['timestamp'] = timestamp
        fields['msg'] = message
        fields['module'] = module_name

        # Format used for file or stdout logging.
        self.logger.log(
            level,
            '%s %s%s',
            timestamp,
            f'{module_name} '.lstrip(),
            message,
            extra=dict(extra_metadata_fields=fields),
        )

    def decode_timestamp(self, timestamp: int) -> str:
        """Decodes timestamp to a human-readable value.

        Defaults to interpreting the input timestamp as nanoseconds since boot.
        Devices can override this to match their timestamp units.
        """
        if self.timestamp_decoder:
            return self.timestamp_decoder(timestamp)
        return str(datetime.timedelta(seconds=timestamp / 1e9))[:-3]

    def get_and_log_metrics(self) -> dict:
        """Retrieves the parsed metrics and logs them to the console."""
        metrics = metric_parser.parse_metrics(
            self.rpcs, self.detokenizer, self.rpc_timeout_s
        )

        def print_metrics(metrics, path):
            """Traverses dictionaries, until a non-dict value is reached."""
            for path_name, metric in metrics.items():
                if isinstance(metric, dict):
                    print_metrics(metric, path + '/' + path_name)
                else:
                    _LOG.info('%s/%s: %s', path, path_name, str(metric))

        print_metrics(metrics, '')
        return metrics

    def snapshot_peak_stack_usage(self, thread_name: Optional[str] = None):
        _, rsp = self.rpcs.pw.thread.ThreadSnapshotService.GetPeakStackUsage(
            name=thread_name
        )

        thread_info = thread_pb2.SnapshotThreadInfo()
        for thread_info_block in rsp:
            for thread in thread_info_block.threads:
                thread_info.threads.append(thread)
        for line in str(ThreadSnapshotAnalyzer(thread_info)).splitlines():
            _LOG.info('%s', line)
