.. _module-pw_router:

---------
pw_router
---------
The ``pw_router`` module provides transport-agnostic classes for routing packets
over network links.

Common router interfaces
========================

PacketParser
------------
To work with arbitrary packet formats, routers require a common interface for
extracting relevant packet data, such as the destination. This interface is
``pw::router::PacketParser``, defined in ``pw_router/packet_parser.h``, which
must be implemented for the packet framing format used by the network.

Egress
------
The Egress class is a virtual interface for sending packet data over a network
link. Egress implementations provide a single ``SendPacket`` function, which
takes the raw packet data and transmits it.

Egresses are invoked with the ``PacketParser`` used to process the packet. This
allows additional information to be extracted from each packet to assist with
transmitting decisions. For example, if packets in a project include a priority,
egresses may use it to provide quality-of-service by dropping certain packets
under heavy load.

.. code-block:: c++

  Status MyEgress::SendPacket(
      ConstByteSpan packet, const PacketParser& parser) override {
    // Downcast the base PacketParser to the custom implementation that was
    // passed into RoutePacket().
    const CustomPacketParser& custom_parser =
        static_cast<const CustomPacketParser&>(parser);

    // Custom packet fields can now be accessed if necessary.
    if (custom_parser.priority() == MyPacketPriority::kHigh) {
      return SendHighPriorityPacket(packet);
    }

    return SendStandardPriorityPacket(packet);
  }

Some common egress implementations are provided upstream in Pigweed.

StaticRouter
============
``pw::router::StaticRouter`` is a router with a static table of address to
egress mappings. Routes in a static router never change; packets with the same
address are always sent through the same egress. If links are unavailable,
packets will be dropped.

Static routers are suitable for basic networks with persistent links.

Usage example
-------------

.. code-block:: c++

  namespace {

  // Define the router egresses.
  UartEgress uart_egress;
  BluetoothEgress ble_egress;

  // Define the routing table.
  constexpr pw::router::StaticRouter::Route routes[] = {{1, uart_egress},
                                                        {7, ble_egress}};
  pw::router::StaticRouter router(routes);

  }  // namespace

  void ProcessPacket(pw::ConstByteSpan packet) {
    HdlcFrameParser hdlc_parser;
    router.RoutePacket(packet, hdlc_parser);
  }

Size report
-----------
The following size report shows the cost of a ``StaticRouter`` with a simple
``PacketParser`` implementation and a single route using an ``EgressFunction``.

.. include:: static_router_size

Zephyr
======
To enable ``pw_router.*`` for Zephyr add ``CONFIG_PIGWEED_ROUTER=y`` to the
project's configuration. This will enable the Kconfig menu for the following:

* ``pw_router.static_router`` which can be enabled via
  ``CONFIG_PIGWEED_ROUTER_STATIC_ROUTER=y``.
* ``pw_router.egress`` which can be enabled via
  ``CONFIG_PIGWEED_ROUTER_EGRESS=y``.
* ``pw_router.packet_parser`` which can be enabled via
  ``CONFIG_PIGWEED_ROUTER_PACKET_PARSER=y``.
* ``pw_router.egress_function`` which can be enabled via
  ``CONFIG_PIGWEED_ROUTER_EGRESS_FUNCTION=y``.
