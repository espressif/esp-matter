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
"""Tools to retrieve and parse metrics."""
from collections import defaultdict
import json
import logging
from typing import Any, Optional
from pw_tokenizer import detokenize

_LOG = logging.getLogger(__name__)


def _tree():
    """Creates a key based on given input."""
    return defaultdict(_tree)


def _insert(metrics, path_names, value):
    """Inserts any value in a leaf of the dictionary."""
    for index, path_name in enumerate(path_names):
        if index < len(path_names) - 1:
            metrics = metrics[path_name]
        elif path_name in metrics:
            # the value in this position isn't a float or int,
            # then collision occurs, throw an error.
            assert ValueError(
                'Variable already exists: {p}'.format(p=path_name)
            )
        else:
            metrics[path_name] = value


def parse_metrics(
    rpcs: Any,
    detokenizer: Optional[detokenize.Detokenizer],
    timeout_s: Optional[float],
):
    """Detokenizes metric names and retrieves their values."""
    # Creates a defaultdict that can infinitely have other defaultdicts
    # without a specified type.
    metrics: defaultdict = _tree()
    if not detokenizer:
        _LOG.error('No metrics token database set.')
        return metrics
    stream_response = rpcs.pw.metric.proto.MetricService.Get(
        pw_rpc_timeout_s=timeout_s
    )
    if not stream_response.status.ok():
        _LOG.error('Unexpected status %s', stream_response.status)
        return metrics
    for metric_response in stream_response.responses:
        for metric in metric_response.metrics:
            path_names = []
            for path in metric.token_path:
                path_name = str(
                    detokenize.DetokenizedString(
                        path, detokenizer.lookup(path), b'', False
                    )
                ).strip('"')
                path_names.append(path_name)
            value = (
                metric.as_float
                if metric.HasField('as_float')
                else metric.as_int
            )
            # inserting path_names into metrics.
            _insert(metrics, path_names, value)
    # Converts default dict objects into standard dictionaries.
    return json.loads(json.dumps(metrics))
