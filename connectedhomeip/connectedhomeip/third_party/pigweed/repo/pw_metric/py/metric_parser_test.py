#!/usr/bin/env python3
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
"""Tests for retreiving and parsing metrics."""
from unittest import TestCase, mock, main
from pw_metric.metric_parser import parse_metrics

from pw_metric_proto import metric_service_pb2
from pw_status import Status
from pw_tokenizer import detokenize, tokens

DATABASE = tokens.Database(
    [
        tokens.TokenizedStringEntry(0x01148A48, "total_dropped"),
        tokens.TokenizedStringEntry(0x03796798, "min_queue_remaining"),
        tokens.TokenizedStringEntry(0x22198280, "total_created"),
        tokens.TokenizedStringEntry(0x534A42F4, "max_queue_used"),
        tokens.TokenizedStringEntry(0x5D087463, "pw::work_queue::WorkQueue"),
        tokens.TokenizedStringEntry(0xA7C43965, "log"),
    ]
)


class TestParseMetrics(TestCase):
    """Test parsing metrics received from RPCs"""

    def setUp(self) -> None:
        """Creating detokenizer and mocking RPC."""
        self.detokenize = detokenize.Detokenizer(DATABASE)
        self.rpc_timeout_s = 1
        self.rpcs = mock.Mock()
        self.rpcs.pw = mock.Mock()
        self.rpcs.pw.metric = mock.Mock()
        self.rpcs.pw.metric.proto = mock.Mock()
        self.rpcs.pw.metric.proto.MetricService = mock.Mock()
        self.rpcs.pw.metric.proto.MetricService.Get = mock.Mock()
        self.rpcs.pw.metric.proto.MetricService.Get.return_value = mock.Mock()
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.status = (
            Status.OK
        )
        # Creating a group and metric name for better identification.
        self.log = 0xA7C43965
        self.total_created = 0x22198280
        self.total_dropped = 0x01148A48
        self.min_queue_remaining = 0x03796798
        self.metric = [
            metric_service_pb2.Metric(
                token_path=[self.log, self.total_created],
                string_path='N/A',
                as_float=3.0,
            ),
            metric_service_pb2.Metric(
                token_path=[self.log, self.total_dropped],
                string_path='N/A',
                as_float=4.0,
            ),
        ]

    def test_invalid_detokenizer(self) -> None:
        """Test invalid detokenizer was supplied."""
        self.assertEqual(
            {},
            parse_metrics(self.rpcs, None, self.rpc_timeout_s),
            msg='Valid detokenizer.',
        )

    def test_bad_stream_status(self) -> None:
        """Test stream response has a status other than OK."""
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.status = (
            Status.ABORTED
        )
        self.assertEqual(
            {},
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Stream response was not aborted.',
        )

    def test_parse_metrics(self) -> None:
        """Test metrics being parsed and recorded."""
        # Loading metric into RPC.
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=self.metric)
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': 3.0,
                    'total_dropped': 4.0,
                }
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )

    def test_three_metric_names(self) -> None:
        """Test creating a dictionary with three paths."""
        # Creating another leaf.
        self.metric.append(
            metric_service_pb2.Metric(
                token_path=[self.log, self.min_queue_remaining],
                string_path='N/A',
                as_float=1.0,
            )
        )
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=self.metric)
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': 3.0,
                    'total_dropped': 4.0,
                    'min_queue_remaining': 1.0,
                },
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )

    def test_inserting_unknown_token(self) -> None:
        # Inserting an unknown token as a group name.
        self.metric.append(
            metric_service_pb2.Metric(
                token_path=[0x007, self.total_dropped],
                string_path='N/A',
                as_float=1.0,
            )
        )
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=self.metric)
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': 3.0,
                    'total_dropped': 4.0,
                },
                '$': {'total_dropped': 1.0},
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )

    def test_multiple_metric_response(self) -> None:
        """Tests multiple metric responses being handled."""
        # Adding more than one MetricResponses.
        metric = [
            metric_service_pb2.Metric(
                token_path=[0x007, self.total_dropped],
                string_path='N/A',
                as_float=1.0,
            )
        ]
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=self.metric),
            metric_service_pb2.MetricResponse(metrics=metric),
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': 3.0,
                    'total_dropped': 4.0,
                },
                '$': {
                    'total_dropped': 1.0,
                },
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )

    def test_paths_longer_than_two(self) -> None:
        """Tests metric paths longer than two."""
        # Path longer than two.
        longest_metric = [
            metric_service_pb2.Metric(
                token_path=[
                    self.log,
                    self.total_created,
                    self.min_queue_remaining,
                ],
                string_path='N/A',
                as_float=1.0,
            ),
        ]
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=longest_metric),
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': {'min_queue_remaining': 1.0},
                }
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )
        # Create a new leaf in log.
        longest_metric.append(
            metric_service_pb2.Metric(
                token_path=[self.log, self.total_dropped],
                string_path='N/A',
                as_float=3.0,
            )
        )
        metric = [
            metric_service_pb2.Metric(
                token_path=[0x007, self.total_dropped],
                string_path='N/A',
                as_float=1.0,
            ),
            metric_service_pb2.Metric(
                token_path=[0x007, self.total_created],
                string_path='N/A',
                as_float=2.0,
            ),
        ]
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=longest_metric),
            metric_service_pb2.MetricResponse(metrics=metric),
        ]
        self.assertEqual(
            {
                'log': {
                    'total_created': {
                        'min_queue_remaining': 1.0,
                    },
                    'total_dropped': 3.0,
                },
                '$': {
                    'total_dropped': 1.0,
                    'total_created': 2.0,
                },
            },
            parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s),
            msg='Metrics are not equal.',
        )

    def test_conflicting_keys(self) -> None:
        """Tests conflicting key and value assignment."""
        longest_metric = [
            metric_service_pb2.Metric(
                token_path=[
                    self.log,
                    self.total_created,
                    self.min_queue_remaining,
                ],
                string_path='N/A',
                as_float=1.0,
            ),
        ]
        # Creates a conflict at log/total_created, should throw an error.
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=longest_metric),
            metric_service_pb2.MetricResponse(metrics=self.metric),
        ]
        parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s)
        self.assertRaises(ValueError, msg='Expected Value Error.')

    def test_conflicting_logs(self) -> None:
        """Tests conflicting loga being streamed."""
        longest_metric = [
            metric_service_pb2.Metric(
                token_path=[self.log, self.total_created],
                string_path='N/A',
                as_float=1.0,
            ),
        ]
        # Creates a duplicate metric for log/total_created.
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=longest_metric),
            metric_service_pb2.MetricResponse(metrics=self.metric),
        ]
        parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s)
        self.assertRaises(ValueError, msg='Expected Value Error.')
        # Duplicate metrics being loaded.
        self.rpcs.pw.metric.proto.MetricService.Get.return_value.responses = [
            metric_service_pb2.MetricResponse(metrics=self.metric),
            metric_service_pb2.MetricResponse(metrics=self.metric),
        ]
        parse_metrics(self.rpcs, self.detokenize, self.rpc_timeout_s)
        self.assertRaises(ValueError, msg='Expected Value Error.')


if __name__ == '__main__':
    main()
