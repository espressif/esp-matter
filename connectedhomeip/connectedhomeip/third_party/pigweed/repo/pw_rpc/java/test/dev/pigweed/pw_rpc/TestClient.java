// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

package dev.pigweed.pw_rpc;

import com.google.common.collect.ImmutableList;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;
import com.google.protobuf.MessageLiteOrBuilder;
import dev.pigweed.pw_rpc.internal.Packet.PacketType;
import dev.pigweed.pw_rpc.internal.Packet.RpcPacket;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.stream.Collectors;
import javax.annotation.Nullable;

/**
 * Wraps a StreamObserverMethodClient for use in tests. Provides methods for simulating the server
 * interactions with the client.
 */
public class TestClient {
  private static final int CHANNEL_ID = 1;

  private final Client client;

  private final List<RpcPacket> sentPackets = new ArrayList<>();
  private final Queue<EnqueuedPackets> enqueuedPackets = new ArrayDeque<>();
  private final Map<PacketType, Integer> sentPayloadIndices = new EnumMap<>(PacketType.class);

  @Nullable private ChannelOutputException channelOutputException = null;

  private static class EnqueuedPackets {
    private int processAfterSentPackets;
    private final List<RpcPacket> packets;

    private EnqueuedPackets(int processAfterSentPackets, List<RpcPacket> packets) {
      this.processAfterSentPackets = processAfterSentPackets;
      this.packets = packets;
    }

    private boolean shouldProcessEnqueuedPackets() {
      return processAfterSentPackets-- <= 1;
    }
  }

  public TestClient(List<Service> services) {
    Channel.Output channelOutput = packet -> {
      if (channelOutputException != null) {
        throw channelOutputException;
      }
      sentPackets.add(parsePacket(packet));

      if (!enqueuedPackets.isEmpty() && enqueuedPackets.peek().shouldProcessEnqueuedPackets()) {
        // Process any enqueued packets.
        enqueuedPackets.remove().packets.forEach(this::processPacket);
      }
    };
    client = Client.create(ImmutableList.of(new Channel(CHANNEL_ID, channelOutput)), services);
  }

  public Client client() {
    return client;
  }

  /**
   * Sets the exception to throw the next time a packet is sent. Set to null to accept the packet
   * without errors.
   *
   * <p>When Channel.Output throws an exception, TestClient does not store those outgoing packets.
   */
  public void setChannelOutputException(@Nullable ChannelOutputException exception) {
    this.channelOutputException = exception;
  }

  /** Returns all payloads that were sent since the last latestClientStreams call. */
  public <T extends MessageLite> List<T> lastClientStreams(Class<T> payloadType) {
    return sentPayloads(payloadType, PacketType.CLIENT_STREAM);
  }

  /** Simulates receiving SERVER_STREAM packets from the server. */
  public void receiveServerStream(String service, String method, MessageLiteOrBuilder... payloads) {
    RpcPacket base = startPacket(service, method, PacketType.SERVER_STREAM).build();
    for (MessageLiteOrBuilder payload : payloads) {
      processPacket(RpcPacket.newBuilder(base).setPayload(getMessage(payload).toByteString()));
    }
  }

  /**
   * Enqueues a SERVER_STREAM packet so that the client receives it after a packet is sent.
   *
   * This function may be called multiple times to create a queue of packets to process as different
   * packets are sent.
   *
   * @param afterPackets Wait until this many packets have been sent before the client receives
   *     these stream packets. The minimum value is 1. If multiple stream packets are queued,
   *     afterPackets is counted from the packet before it in the queue.
   */
  public void enqueueServerStream(
      String service, String method, int afterPackets, MessageLiteOrBuilder... payloads) {
    if (afterPackets < 1) {
      throw new IllegalArgumentException("afterPackets must be at least 1");
    }

    RpcPacket base = startPacket(service, method, PacketType.SERVER_STREAM).build();
    enqueuedPackets.add(new EnqueuedPackets(afterPackets,
        Arrays.stream(payloads)
            .map(m -> RpcPacket.newBuilder(base).setPayload(getMessage(m).toByteString()).build())
            .collect(Collectors.toList())));
  }

  /** Simulates receiving a SERVER_ERROR packet from the server. */
  public void receiveServerError(String service, String method, Status error) {
    processPacket(startPacket(service, method, PacketType.SERVER_ERROR).setStatus(error.code()));
  }

  /** Parses sent payloads for the given type of packet. */
  private <T extends MessageLite> List<T> sentPayloads(Class<T> payloadType, PacketType type) {
    int sentPayloadIndex = sentPayloadIndices.getOrDefault(type, 0);

    // Filter only the specified packets.
    List<T> newPayloads = sentPackets.stream()
                              .filter(packet -> packet.getType().equals(type))
                              .skip(sentPayloadIndex)
                              .map(p -> parseRequestPayload(payloadType, p))
                              .collect(Collectors.toList());

    // Store the index of the last read payload. Could drop the viewed packets instead to reduce
    // memory usage, but that probably won't matter in practice.
    sentPayloadIndices.put(type, sentPayloadIndex + newPayloads.size());

    return newPayloads;
  }

  private void processPacket(RpcPacket packet) {
    if (!client.processPacket(packet.toByteArray())) {
      throw new AssertionError("TestClient failed to process a packet!");
    }
  }

  private void processPacket(RpcPacket.Builder packet) {
    processPacket(packet.build());
  }

  private static RpcPacket.Builder startPacket(String service, String method, PacketType type) {
    return RpcPacket.newBuilder()
        .setType(type)
        .setChannelId(CHANNEL_ID)
        .setServiceId(Ids.calculate(service))
        .setMethodId(Ids.calculate(method));
  }

  private static RpcPacket parsePacket(byte[] packet) {
    try {
      return RpcPacket.parseFrom(packet);
    } catch (InvalidProtocolBufferException e) {
      throw new AssertionError("Decoding sent packet failed", e);
    }
  }

  private <T extends MessageLite> T parseRequestPayload(Class<T> payloadType, RpcPacket packet) {
    try {
      return payloadType.cast(Method.decodeProtobuf(
          client.method(CHANNEL_ID, packet.getServiceId(), packet.getMethodId()).method().request(),
          packet.getPayload()));
    } catch (InvalidProtocolBufferException e) {
      throw new AssertionError("Decoding sent packet payload failed", e);
    }
  }

  private MessageLite getMessage(MessageLiteOrBuilder messageOrBuilder) {
    if (messageOrBuilder instanceof MessageLite.Builder) {
      return ((MessageLite.Builder) messageOrBuilder).build();
    }
    if (messageOrBuilder instanceof MessageLite) {
      return (MessageLite) messageOrBuilder;
    }
    throw new AssertionError("Unexpected MessageLiteOrBuilder class");
  }
}
