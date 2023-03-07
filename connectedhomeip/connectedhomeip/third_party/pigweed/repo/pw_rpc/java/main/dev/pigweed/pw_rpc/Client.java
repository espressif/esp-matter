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

import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;
import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.internal.Packet.PacketType;
import dev.pigweed.pw_rpc.internal.Packet.RpcPacket;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;
import javax.annotation.Nullable;

/**
 * A client for a pw_rpc server. Invokes RPCs through a MethodClient and handles RPC responses
 * through the processPacket function.
 */
public class Client {
  private static final Logger logger = Logger.forClass(Client.class);

  private final Map<Integer, Channel> channels;
  private final Map<Integer, Service> services;

  private final Map<PendingRpc, MethodClient> methodClients;
  private final RpcManager rpcs;

  private final Function<PendingRpc, StreamObserver<MessageLite>> defaultObserverFactory;

  /**
   * Creates a new RPC client.
   *
   * @param channels supported channels, which are used to send requests to the server
   * @param services which RPC services this client supports; used to handle encoding and decoding
   */
  private Client(List<Channel> channels,
      List<Service> services,
      Function<PendingRpc, StreamObserver<MessageLite>> defaultObserverFactory) {
    this.channels = channels.stream().collect(Collectors.toMap(Channel::id, c -> c));
    this.services = services.stream().collect(Collectors.toMap(Service::id, s -> s));

    this.methodClients = new HashMap<>();
    this.rpcs = new RpcManager();

    this.defaultObserverFactory = defaultObserverFactory;
  }

  /**
   * Creates a new pw_rpc client.
   *
   * @param channels the set of channels for the client to send requests over
   * @param services the services to support on this client
   * @param defaultObserverFactory function that creates a default observer for each RPC
   * @return the new pw.rpc.Client
   */
  public static Client create(List<Channel> channels,
      List<Service> services,
      Function<PendingRpc, StreamObserver<MessageLite>> defaultObserverFactory) {
    return new Client(channels, services, defaultObserverFactory);
  }

  /** Creates a new pw_rpc client that logs responses when no observer is provided to calls. */
  public static Client create(List<Channel> channels, List<Service> services) {
    return create(channels, services, (rpc) -> new StreamObserver<MessageLite>() {
      @Override
      public void onNext(MessageLite value) {
        logger.atFine().log("%s received response: %s", rpc, value);
      }

      @Override
      public void onCompleted(Status status) {
        logger.atInfo().log("%s completed with status %s", rpc, status);
      }

      @Override
      public void onError(Status status) {
        logger.atWarning().log("%s terminated with error %s", rpc, status);
      }
    });
  }

  /**
   * Returns a MethodClient with the given name for the provided channelID
   *
   * @param channelId the ID for the channel through which to invoke the RPC
   * @param fullMethodName the method name as "package.Service.Method" or "package.Service/Method"
   */
  public MethodClient method(int channelId, String fullMethodName) {
    for (char delimiter : new char[] {'/', '.'}) {
      int index = fullMethodName.lastIndexOf(delimiter);
      if (index != -1) {
        return method(
            channelId, fullMethodName.substring(0, index), fullMethodName.substring(index + 1));
      }
    }
    throw new IllegalArgumentException("Invalid method name '" + fullMethodName
        + "'; does not match required package.Service/Method format");
  }

  /**
   * Returns a MethodClient on the provided channel using separate arguments for "package.Service"
   * and "Method".
   */
  public MethodClient method(int channelId, String fullServiceName, String methodName) {
    try {
      return method(channelId, Ids.calculate(fullServiceName), Ids.calculate(methodName));
    } catch (IllegalArgumentException e) {
      // Rethrow the exception with the service and method name instead of the ID.
      throw new IllegalArgumentException("Unknown RPC " + fullServiceName + '/' + methodName, e);
    }
  }

  /**
   * Returns a MethodClient instance from a Method instance.
   */
  public MethodClient method(int channelId, Method serviceMethod) {
    try {
      return method(channelId, serviceMethod.service().id(), serviceMethod.id());
    } catch (IllegalArgumentException e) {
      // Rethrow the exception with the service and method name instead of the ID.
      throw new IllegalArgumentException("Unknown RPC " + serviceMethod.fullName(), e);
    }
  }

  /** Returns a MethodClient with the provided service and method IDs. */
  synchronized MethodClient method(int channelId, int serviceId, int methodId) {
    Channel channel = channels.get(channelId);
    if (channel == null) {
      throw new IllegalArgumentException("Unknown channel ID " + channelId);
    }

    Service service = services.get(serviceId);
    if (service == null) {
      throw new IllegalArgumentException("Unknown service ID " + serviceId);
    }

    Method method = service.methods().get(methodId);
    if (method == null) {
      throw new IllegalArgumentException("Unknown method ID " + methodId);
    }

    PendingRpc rpc = PendingRpc.create(channel, service, method);
    if (!methodClients.containsKey(rpc)) {
      methodClients.put(rpc, new MethodClient(rpcs, rpc, defaultObserverFactory.apply(rpc)));
    }
    return methodClients.get(rpc);
  }

  /**
   * Processes a single RPC packet.
   *
   * @param data a single, binary encoded RPC packet
   * @return true if the packet was decoded and processed by this client; returns false for invalid
   *     packets or packets for a server or unrecognized channel
   */
  public boolean processPacket(byte[] data) {
    return processPacket(ByteBuffer.wrap(data));
  }

  public boolean processPacket(ByteBuffer data) {
    RpcPacket packet;
    try {
      packet = RpcPacket.parseFrom(data, ExtensionRegistryLite.getEmptyRegistry());
    } catch (InvalidProtocolBufferException e) {
      logger.atWarning().withCause(e).log("Failed to decode packet");
      return false;
    }

    if (packet.getChannelId() == 0 || packet.getServiceId() == 0 || packet.getMethodId() == 0) {
      logger.atWarning().log("Received corrupt packet with unset IDs");
      return false;
    }

    // Packets for the server use even type values.
    if (packet.getTypeValue() % 2 == 0) {
      logger.atFine().log("Ignoring %s packet for server", packet.getType().name());
      return false;
    }

    Channel channel = channels.get(packet.getChannelId());
    if (channel == null) {
      logger.atWarning().log("Received packet for unrecognized channel %d", packet.getChannelId());
      return false;
    }

    PendingRpc rpc = lookupRpc(channel, packet);
    if (rpc == null) {
      logger.atInfo().log("Ignoring packet for unknown service method");
      sendError(channel, packet, Status.NOT_FOUND);
      return true; // true since the packet was handled, even though it was invalid.
    }

    // Any packet type other than SERVER_STREAM indicates that this is the last packet for this RPC.
    StreamObserverCall<?, ?> call =
        packet.getType().equals(PacketType.SERVER_STREAM) ? rpcs.getPending(rpc) : rpcs.clear(rpc);
    if (call == null) {
      logger.atFine().log(
          "Ignoring packet for %s, which isn't pending. Pending RPCs are %s", rpc, rpcs);
      sendError(channel, packet, Status.FAILED_PRECONDITION);
      return true;
    }

    switch (packet.getType()) {
      case SERVER_ERROR: {
        Status status = decodeStatus(packet);
        logger.atWarning().log("%s failed with error %s", rpc, status);
        call.onError(status);
        break;
      }
      case RESPONSE: {
        Status status = decodeStatus(packet);
        // Server streaming an unary RPCs include a payload with their response packet.
        if (!rpc.method().isServerStreaming()) {
          logger.atFiner().log("%s completed with status %s and %d B payload",
              rpc,
              status,
              packet.getPayload().size());
          call.onNext(packet.getPayload());
        } else {
          logger.atFiner().log("%s completed with status %s", rpc, status);
        }
        call.onCompleted(status);
        break;
      }
      case SERVER_STREAM:
        logger.atFiner().log(
            "%s received server stream with %d B payload", rpc, packet.getPayload().size());
        call.onNext(packet.getPayload());
        break;
      default:
        logger.atWarning().log(
            "%s received unexpected PacketType %d", rpc, packet.getType().getNumber());
    }

    return true;
  }

  private static void sendError(Channel channel, RpcPacket packet, Status status) {
    try {
      channel.send(Packets.error(packet, status));
    } catch (ChannelOutputException e) {
      logger.atWarning().withCause(e).log("Failed to send error packet");
    }
  }

  @Nullable
  private PendingRpc lookupRpc(Channel channel, RpcPacket packet) {
    Service service = services.get(packet.getServiceId());
    if (service != null) {
      Method method = service.methods().get(packet.getMethodId());
      if (method != null) {
        return PendingRpc.create(channel, service, method);
      }
    }

    return null;
  }

  private static Status decodeStatus(RpcPacket packet) {
    Status status = Status.fromCode(packet.getStatus());
    if (status == null) {
      logger.atWarning().log(
          "Illegal status code %d in packet; using Status.UNKNOWN ", packet.getStatus());
      return Status.UNKNOWN;
    }
    return status;
  }
}
