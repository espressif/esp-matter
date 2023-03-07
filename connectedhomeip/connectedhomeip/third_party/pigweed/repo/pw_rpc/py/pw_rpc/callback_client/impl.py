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
"""The callback-based pw_rpc client implementation."""

import inspect
import logging
import textwrap
from typing import Any, Callable, Dict, Iterable, Optional, Type

from pw_status import Status
from google.protobuf.message import Message

from pw_rpc import client, descriptors
from pw_rpc.client import PendingRpc, PendingRpcs
from pw_rpc.descriptors import Channel, Method, Service

from pw_rpc.callback_client.call import (
    UseDefault,
    OptionalTimeout,
    CallType,
    UnaryResponse,
    StreamResponse,
    Call,
    UnaryCall,
    ServerStreamingCall,
    ClientStreamingCall,
    BidirectionalStreamingCall,
    OnNextCallback,
    OnCompletedCallback,
    OnErrorCallback,
)

_LOG = logging.getLogger(__package__)


class _MethodClient:
    """A method that can be invoked for a particular channel."""

    def __init__(
        self,
        client_impl: 'Impl',
        rpcs: PendingRpcs,
        channel: Channel,
        method: Method,
        default_timeout_s: Optional[float],
    ) -> None:
        self._impl = client_impl
        self._rpcs = rpcs
        self._rpc = PendingRpc(channel, method.service, method)
        self.default_timeout_s: Optional[float] = default_timeout_s

    @property
    def channel(self) -> Channel:
        return self._rpc.channel

    @property
    def method(self) -> Method:
        return self._rpc.method

    @property
    def service(self) -> Service:
        return self._rpc.service

    @property
    def request(self) -> type:
        """Returns the request proto class."""
        return self.method.request_type

    @property
    def response(self) -> type:
        """Returns the response proto class."""
        return self.method.response_type

    def __repr__(self) -> str:
        return self.help()

    def help(self) -> str:
        """Returns a help message about this RPC."""
        function_call = self.method.full_name + '('

        docstring = inspect.getdoc(self.__call__)  # type: ignore[operator] # pylint: disable=no-member
        assert docstring is not None

        annotation = inspect.Signature.from_callable(self).return_annotation  # type: ignore[arg-type] # pylint: disable=line-too-long
        if isinstance(annotation, type):
            annotation = annotation.__name__

        arg_sep = f',\n{" " * len(function_call)}'
        return (
            f'{function_call}'
            f'{arg_sep.join(descriptors.field_help(self.method.request_type))})'
            f'\n\n{textwrap.indent(docstring, "  ")}\n\n'
            f'  Returns {annotation}.'
        )

    def _start_call(
        self,
        call_type: Type[CallType],
        request: Optional[Message],
        timeout_s: OptionalTimeout,
        on_next: Optional[OnNextCallback],
        on_completed: Optional[OnCompletedCallback],
        on_error: Optional[OnErrorCallback],
        ignore_errors: bool = False,
    ) -> CallType:
        """Creates the Call object and invokes the RPC using it."""
        if timeout_s is UseDefault.VALUE:
            timeout_s = self.default_timeout_s

        call = call_type(
            self._rpcs, self._rpc, timeout_s, on_next, on_completed, on_error
        )
        call._invoke(request, ignore_errors)  # pylint: disable=protected-access
        return call

    def _client_streaming_call_type(
        self, base: Type[CallType]
    ) -> Type[CallType]:
        """Creates a client or bidirectional stream call type.

        Applies the signature from the request protobuf to the send method.
        """

        def send(
            self, _rpc_request_proto: Optional[Message] = None, **request_fields
        ) -> None:
            ClientStreamingCall.send(self, _rpc_request_proto, **request_fields)

        _apply_protobuf_signature(self.method, send)

        return type(
            f'{self.method.name}_{base.__name__}', (base,), dict(send=send)
        )


def _function_docstring(method: Method) -> str:
    return f'''\
Invokes the {method.full_name} {method.type.sentence_name()} RPC.

This function accepts either the request protobuf fields as keyword arguments or
a request protobuf as a positional argument.
'''


def _update_call_method(method: Method, function: Callable) -> None:
    """Updates the name, docstring, and parameters to match a method."""
    function.__name__ = method.full_name
    function.__doc__ = _function_docstring(method)
    _apply_protobuf_signature(method, function)


def _apply_protobuf_signature(method: Method, function: Callable) -> None:
    """Update a function signature to accept proto arguments.

    In order to have good tab completion and help messages, update the function
    signature to accept only keyword arguments for the proto message fields.
    This doesn't actually change the function signature -- it just updates how
    it appears when inspected.
    """
    sig = inspect.signature(function)

    params = [next(iter(sig.parameters.values()))]  # Get the "self" parameter
    params += method.request_parameters()
    params.append(
        inspect.Parameter('pw_rpc_timeout_s', inspect.Parameter.KEYWORD_ONLY)
    )

    function.__signature__ = sig.replace(  # type: ignore[attr-defined]
        parameters=params
    )


class _UnaryMethodClient(_MethodClient):
    def invoke(
        self,
        request: Optional[Message] = None,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        request_args: Optional[Dict[str, Any]] = None,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> UnaryCall:
        """Invokes the unary RPC and returns a call object."""
        return self._start_call(
            UnaryCall,
            self.method.get_request(request, request_args),
            timeout_s,
            on_next,
            on_completed,
            on_error,
        )

    def open(
        self,
        request: Optional[Message] = None,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        request_args: Optional[Dict[str, Any]] = None,
    ) -> UnaryCall:
        """Invokes the unary RPC and returns a call object."""
        return self._start_call(
            UnaryCall,
            self.method.get_request(request, request_args),
            None,
            on_next,
            on_completed,
            on_error,
            True,
        )


class _ServerStreamingMethodClient(_MethodClient):
    def invoke(
        self,
        request: Optional[Message] = None,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        request_args: Optional[Dict[str, Any]] = None,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> ServerStreamingCall:
        """Invokes the server streaming RPC and returns a call object."""
        return self._start_call(
            ServerStreamingCall,
            self.method.get_request(request, request_args),
            timeout_s,
            on_next,
            on_completed,
            on_error,
        )

    def open(
        self,
        request: Optional[Message] = None,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        request_args: Optional[Dict[str, Any]] = None,
    ) -> ServerStreamingCall:
        """Returns a call object for the RPC, even if the RPC cannot be invoked.

        Can be used to listen for responses from an RPC server that may yet be
        available.
        """
        return self._start_call(
            ServerStreamingCall,
            self.method.get_request(request, request_args),
            None,
            on_next,
            on_completed,
            on_error,
            True,
        )


class _ClientStreamingMethodClient(_MethodClient):
    def invoke(
        self,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> ClientStreamingCall:
        """Invokes the client streaming RPC and returns a call object"""
        return self._start_call(
            self._client_streaming_call_type(ClientStreamingCall),
            None,
            timeout_s,
            on_next,
            on_completed,
            on_error,
            True,
        )

    def open(
        self,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
    ) -> ClientStreamingCall:
        """Returns a call object for the RPC, even if the RPC cannot be invoked.

        Can be used to listen for responses from an RPC server that may yet be
        available.
        """
        return self._start_call(
            self._client_streaming_call_type(ClientStreamingCall),
            None,
            None,
            on_next,
            on_completed,
            on_error,
            True,
        )

    def __call__(
        self,
        requests: Iterable[Message] = (),
        *,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> UnaryResponse:
        return self.invoke().finish_and_wait(requests, timeout_s=timeout_s)


class _BidirectionalStreamingMethodClient(_MethodClient):
    def invoke(
        self,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
        *,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> BidirectionalStreamingCall:
        """Invokes the bidirectional streaming RPC and returns a call object."""
        return self._start_call(
            self._client_streaming_call_type(BidirectionalStreamingCall),
            None,
            timeout_s,
            on_next,
            on_completed,
            on_error,
        )

    def open(
        self,
        on_next: Optional[OnNextCallback] = None,
        on_completed: Optional[OnCompletedCallback] = None,
        on_error: Optional[OnErrorCallback] = None,
    ) -> BidirectionalStreamingCall:
        """Returns a call object for the RPC, even if the RPC cannot be invoked.

        Can be used to listen for responses from an RPC server that may yet be
        available.
        """
        return self._start_call(
            self._client_streaming_call_type(BidirectionalStreamingCall),
            None,
            None,
            on_next,
            on_completed,
            on_error,
            True,
        )

    def __call__(
        self,
        requests: Iterable[Message] = (),
        *,
        timeout_s: OptionalTimeout = UseDefault.VALUE,
    ) -> StreamResponse:
        return self.invoke().finish_and_wait(requests, timeout_s=timeout_s)


def _method_client_docstring(method: Method) -> str:
    return f'''\
Class that invokes the {method.full_name} {method.type.sentence_name()} RPC.

Calling this directly invokes the RPC synchronously. The RPC can be invoked
asynchronously using the invoke method.
'''


class Impl(client.ClientImpl):
    """Callback-based ClientImpl, for use with pw_rpc.Client."""

    def __init__(
        self,
        default_unary_timeout_s: Optional[float] = None,
        default_stream_timeout_s: Optional[float] = None,
        cancel_duplicate_calls: Optional[bool] = True,
    ) -> None:
        super().__init__()
        self._default_unary_timeout_s = default_unary_timeout_s
        self._default_stream_timeout_s = default_stream_timeout_s

        # Temporary workaround for clients that rely on mulitple in-flight
        # instances of an RPC on the same channel, which is not supported.
        # TODO(hepler): Remove this option when clients have updated.
        self._cancel_duplicate_calls = cancel_duplicate_calls

    @property
    def default_unary_timeout_s(self) -> Optional[float]:
        return self._default_unary_timeout_s

    @property
    def default_stream_timeout_s(self) -> Optional[float]:
        return self._default_stream_timeout_s

    def method_client(self, channel: Channel, method: Method) -> _MethodClient:
        """Returns an object that invokes a method using the given chanel."""

        # Temporarily attach the cancel_duplicate_calls option to the
        # PendingRpcs object.
        # TODO(hepler): Remove this workaround.
        assert self.rpcs
        self.rpcs.cancel_duplicate_calls = (  # type: ignore[attr-defined]
            self._cancel_duplicate_calls
        )

        if method.type is Method.Type.UNARY:
            return self._create_unary_method_client(
                channel, method, self.default_unary_timeout_s
            )

        if method.type is Method.Type.SERVER_STREAMING:
            return self._create_server_streaming_method_client(
                channel, method, self.default_stream_timeout_s
            )

        if method.type is Method.Type.CLIENT_STREAMING:
            return self._create_method_client(
                _ClientStreamingMethodClient,
                channel,
                method,
                self.default_unary_timeout_s,
            )

        if method.type is Method.Type.BIDIRECTIONAL_STREAMING:
            return self._create_method_client(
                _BidirectionalStreamingMethodClient,
                channel,
                method,
                self.default_stream_timeout_s,
            )

        raise AssertionError(f'Unknown method type {method.type}')

    def _create_method_client(
        self,
        base: type,
        channel: Channel,
        method: Method,
        default_timeout_s: Optional[float],
        **fields,
    ):
        """Creates a _MethodClient derived class customized for the method."""
        method_client_type = type(
            f'{method.name}{base.__name__}',
            (base,),
            dict(__doc__=_method_client_docstring(method), **fields),
        )
        return method_client_type(
            self, self.rpcs, channel, method, default_timeout_s
        )

    def _create_unary_method_client(
        self,
        channel: Channel,
        method: Method,
        default_timeout_s: Optional[float],
    ) -> _UnaryMethodClient:
        """Creates a _UnaryMethodClient with a customized __call__ method."""

        # TODO(hepler): Use / to mark the first arg as positional-only
        #     when when Python 3.7 support is no longer required.
        def call(
            self: _UnaryMethodClient,
            _rpc_request_proto: Optional[Message] = None,
            *,
            pw_rpc_timeout_s: OptionalTimeout = UseDefault.VALUE,
            **request_fields,
        ) -> UnaryResponse:
            return self.invoke(
                self.method.get_request(_rpc_request_proto, request_fields)
            ).wait(pw_rpc_timeout_s)

        _update_call_method(method, call)
        return self._create_method_client(
            _UnaryMethodClient,
            channel,
            method,
            default_timeout_s,
            __call__=call,
        )

    def _create_server_streaming_method_client(
        self,
        channel: Channel,
        method: Method,
        default_timeout_s: Optional[float],
    ) -> _ServerStreamingMethodClient:
        """Creates _ServerStreamingMethodClient with custom __call__ method."""

        # TODO(hepler): Use / to mark the first arg as positional-only
        #     when when Python 3.7 support is no longer required.
        def call(
            self: _ServerStreamingMethodClient,
            _rpc_request_proto: Optional[Message] = None,
            *,
            pw_rpc_timeout_s: OptionalTimeout = UseDefault.VALUE,
            **request_fields,
        ) -> StreamResponse:
            return self.invoke(
                self.method.get_request(_rpc_request_proto, request_fields)
            ).wait(pw_rpc_timeout_s)

        _update_call_method(method, call)
        return self._create_method_client(
            _ServerStreamingMethodClient,
            channel,
            method,
            default_timeout_s,
            __call__=call,
        )

    def handle_response(
        self,
        rpc: PendingRpc,
        context: Call,
        payload,
        *,
        args: tuple = (),
        kwargs: Optional[dict] = None,
    ) -> None:
        """Invokes the callback associated with this RPC."""
        assert not args and not kwargs, 'Forwarding args & kwargs not supported'
        context._handle_response(payload)  # pylint: disable=protected-access

    def handle_completion(
        self,
        rpc: PendingRpc,
        context: Call,
        status: Status,
        *,
        args: tuple = (),
        kwargs: Optional[dict] = None,
    ):
        assert not args and not kwargs, 'Forwarding args & kwargs not supported'
        context._handle_completion(status)  # pylint: disable=protected-access

    def handle_error(
        self,
        rpc: PendingRpc,
        context: Call,
        status: Status,
        *,
        args: tuple = (),
        kwargs: Optional[dict] = None,
    ) -> None:
        assert not args and not kwargs, 'Forwarding args & kwargs not supported'
        context._handle_error(status)  # pylint: disable=protected-access
