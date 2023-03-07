.. _module-pw_rpc-benchmark:

===================
pw_rpc Benchmarking
===================
pw_rpc provides tools for stress testing and benchmarking a Pigweed RPC
deployment and the transport it is running over. Two components are included:

* The pw.rpc.Benchmark service and its implementation.
* A Python module that runs tests using the Benchmark service.

------------------------
pw.rpc.Benchmark service
------------------------
The Benchmark service provides a low-level RPC service for sending data between
the client and server. The service is defined in ``pw_rpc/benchmark.proto``.

A raw RPC implementation of the benchmark service is provided. This
implementation is suitable for use in any system with pw_rpc. To access this
service, add a dependency on ``"$dir_pw_rpc:benchmark"`` in GN or
``pw_rpc.benchmark`` in CMake. Then, include the service
(``#include "pw_rpc/benchmark.h"``), instantiate it, and register it with your
RPC server, like any other RPC service.

The Benchmark service was designed with the Python-based benchmarking tools in
mind, but it may be used directly to test basic RPC functionality. The service
is well suited for use in automated integration tests or in an interactive
console.

Benchmark service
==================
.. literalinclude:: benchmark.proto
  :language: protobuf
  :lines: 14-

Example
=======
.. code-block:: c++

  #include "pw_rpc/benchmark.h"
  #include "pw_rpc/server.h"

  constexpr pw::rpc::Channel kChannels[] = { /* ... */ };
  static pw::rpc::Server server(kChannels);

  static pw::rpc::BenchmarkService benchmark_service;

  void RegisterServices() {
    server.RegisterService(benchmark_service);
  }

