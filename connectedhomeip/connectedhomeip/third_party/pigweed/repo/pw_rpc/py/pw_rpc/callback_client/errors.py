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
"""Exceptions for RPC-related errors."""

from typing import Optional

from pw_status import Status

from pw_rpc.client import PendingRpc


class RpcTimeout(Exception):
    def __init__(self, rpc: PendingRpc, timeout: Optional[float]):
        super().__init__(
            f'No response received for {rpc.method} after {timeout} s'
        )
        self.rpc = rpc
        self.timeout = timeout


class RpcError(Exception):
    def __init__(self, rpc: PendingRpc, status: Status):
        """Raised when there is an RPC-layer error."""
        if status is Status.NOT_FOUND:
            msg = ': the RPC server does not support this RPC'
        elif status is Status.DATA_LOSS:
            msg = ': an error occurred while decoding the RPC payload'
        else:
            msg = ''

        super().__init__(f'{rpc.method} failed with error {status}{msg}')
        self.rpc = rpc
        self.status = status
