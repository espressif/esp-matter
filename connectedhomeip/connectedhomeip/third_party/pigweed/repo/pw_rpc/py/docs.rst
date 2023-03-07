.. _module-pw_rpc-py:

---------------------
pw_rpc Python package
---------------------
The ``pw_rpc`` Python package makes it possible to call Pigweed RPCs from
Python. The package includes a ``pw_rpc`` client library, as well as tools for
creating a ``pw_rpc`` console.

pw_rpc.client
=============
.. automodule:: pw_rpc.client
  :members: Client, ClientImpl

pw_rpc.callback_client
======================
.. automodule:: pw_rpc.callback_client
  :members:
    UnaryResponse,
    StreamResponse,
    UnaryCall,
    ServerStreamingCall,
    ClientStreamingCall,
    BidirectionalStreamingCall,

pw_rpc.descriptors
==================
.. automodule:: pw_rpc.descriptors
  :members:
    Channel,
    ChannelManipulator,

pw_rpc.console_tools
====================
.. automodule:: pw_rpc.console_tools
  :members:
     ClientInfo,
     Context,
     Watchdog,
     alias_deprecated_command,
     flattened_rpc_completions,
     help_as_repr,
