.. _module-pw_protobuf-size_report:

================================
pw_protobuf extended size report
================================
pw_protobuf can impact binary size very differently depending on how it's used.
A series of examples are provided below to illustrate how much certain use cases
affect binary size.

--------
Overview
--------
This module includes a proto encoder, two different proto decoders (one that
operates on a ``pw::stream::StreamReader`` and another that operates on an in-
memory buffer), codegen for direct wire-format encoders/decoders, and a
table-based codegen system for constructing proto messages as in-memory structs.

Here's a brief overview of the different encoder/decoder costs:

.. include:: size_report/protobuf_overview

.. note::

  There's some overhead involved in ensuring all of the encoder/decoder
  functionality is pulled in. Check the per-symbol breakdown for more details.

--------------------------------
Encoder/decoder codegen overhead
--------------------------------
The different proto serialization/deserialization codegen methods have different
overhead. Some have a higher up-front cost, but lower complexity (and therefore
smaller compiler generated code) at the sites of usage. Others trade lower
up-front code size cost for more complexity at the proto construction and read
sites.

This example uses the following proto message to construct a variety of use
cases to illustrate how code and memory requirements can change depending on
the complexity of the proto message being encoded/decoded.

.. literalinclude:: pw_protobuf_test_protos/size_report.proto
  :language: protobuf
  :lines: 14-

This proto is configured with the following options file:

.. literalinclude:: pw_protobuf_test_protos/size_report.options
  :lines: 14-

Trivial proto
=============
This is a size report for encoding/decoding the ``pw.protobuf.test.ItemInfo``
message. This is a pretty trivial message with just a few integers.

.. include:: size_report/simple_codegen_size_comparison

Optional and oneof
==================
This is a size report for encoding/decoding the
``pw.protobuf.test.ResponseInfo`` message. This is slightly more complex message
that has a few explicitly optional fields, a oneof, and a submessage.

.. include:: size_report/oneof_codegen_size_comparison
