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

import static com.google.common.truth.Truth.assertThat;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.fail;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import com.google.common.collect.ImmutableList;
import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;
import dev.pigweed.pw_rpc.internal.Packet.PacketType;
import dev.pigweed.pw_rpc.internal.Packet.RpcPacket;
import java.util.ArrayList;
import java.util.List;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;

public final class ClientTest {
  @Rule public final MockitoRule mockito = MockitoJUnit.rule();

  private static final Service SERVICE = new Service("pw.rpc.test1.TheTestService",
      Service.unaryMethod("SomeUnary", SomeMessage.class, AnotherMessage.class),
      Service.serverStreamingMethod("SomeServerStreaming", SomeMessage.class, AnotherMessage.class),
      Service.clientStreamingMethod("SomeClientStreaming", SomeMessage.class, AnotherMessage.class),
      Service.bidirectionalStreamingMethod(
          "SomeBidiStreaming", SomeMessage.class, AnotherMessage.class));

  private static final Method UNARY_METHOD = SERVICE.method("SomeUnary");
  private static final Method SERVER_STREAMING_METHOD = SERVICE.method("SomeServerStreaming");
  private static final Method CLIENT_STREAMING_METHOD = SERVICE.method("SomeClientStreaming");

  private static final int CHANNEL_ID = 1;

  private static final SomeMessage REQUEST_PAYLOAD =
      SomeMessage.newBuilder().setMagicNumber(54321).build();
  private static final AnotherMessage RESPONSE_PAYLOAD =
      AnotherMessage.newBuilder()
          .setResult(AnotherMessage.Result.FAILED_MISERABLY)
          .setPayload("12345")
          .build();

  private Client client;
  private List<RpcPacket> packetsSent;

  @Mock private StreamObserver<AnotherMessage> observer;

  private static byte[] response(String service, String method) {
    return response(service, method, Status.OK);
  }

  private static byte[] response(String service, String method, Status status) {
    return serverReply(
        PacketType.RESPONSE, service, method, status, SomeMessage.getDefaultInstance());
  }

  private static byte[] response(
      String service, String method, Status status, MessageLite payload) {
    return serverReply(PacketType.RESPONSE, service, method, status, payload);
  }

  private static byte[] serverStream(String service, String method, MessageLite payload) {
    return serverReply(PacketType.SERVER_STREAM, service, method, Status.OK, payload);
  }

  private static byte[] serverReply(
      PacketType type, String service, String method, Status status, MessageLite payload) {
    return packetBuilder(service, method)
        .setType(type)
        .setStatus(status.code())
        .setPayload(payload.toByteString())
        .build()
        .toByteArray();
  }

  private static RpcPacket.Builder packetBuilder(String service, String method) {
    return RpcPacket.newBuilder()
        .setChannelId(CHANNEL_ID)
        .setServiceId(Ids.calculate(service))
        .setMethodId(Ids.calculate(method));
  }

  private static RpcPacket requestPacket(String service, String method, MessageLite payload) {
    return packetBuilder(service, method)
        .setType(PacketType.REQUEST)
        .setPayload(payload.toByteString())
        .build();
  }

  @Before
  public void setup() {
    packetsSent = new ArrayList<>();
    client = Client.create(ImmutableList.of(new Channel(1, (data) -> {
      try {
        packetsSent.add(RpcPacket.parseFrom(data, ExtensionRegistryLite.getEmptyRegistry()));
      } catch (InvalidProtocolBufferException e) {
        fail("The client sent an invalid packet: " + e);
      }
    })), ImmutableList.of(SERVICE));
  }

  @Test
  public void method_unknownMethod() {
    assertThrows(IllegalArgumentException.class, () -> client.method(CHANNEL_ID, ""));
    assertThrows(IllegalArgumentException.class, () -> client.method(CHANNEL_ID, "one"));
    assertThrows(IllegalArgumentException.class, () -> client.method(CHANNEL_ID, "hello"));
    assertThrows(
        IllegalArgumentException.class, () -> client.method(CHANNEL_ID, "abc.Service/Method"));
    assertThrows(IllegalArgumentException.class,
        () -> client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService/NotAnRpc").method());

    Service service = new Service("throwaway.NotRealService",
        Service.unaryMethod("NotAnRpc", SomeMessage.class, AnotherMessage.class));
    assertThrows(IllegalArgumentException.class,
        () -> client.method(CHANNEL_ID, service.method("NotAnRpc")));
  }

  @Test
  public void method_unknownChannel() {
    assertThrows(IllegalArgumentException.class,
        () -> client.method(0, "pw.rpc.test1.TheTestService/SomeUnary"));
    assertThrows(IllegalArgumentException.class,
        () -> client.method(999, "pw.rpc.test1.TheTestService/SomeUnary"));
  }

  @Test
  public void method_accessAsServiceSlashMethod() {
    assertThat(client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService/SomeUnary").method())
        .isSameInstanceAs(UNARY_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService/SomeServerStreaming").method())
        .isSameInstanceAs(SERVER_STREAMING_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService/SomeClientStreaming").method())
        .isSameInstanceAs(CLIENT_STREAMING_METHOD);
  }

  @Test
  public void method_accessAsServiceDotMethod() {
    assertThat(client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService.SomeUnary").method())
        .isSameInstanceAs(UNARY_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService.SomeServerStreaming").method())
        .isSameInstanceAs(SERVER_STREAMING_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService.SomeClientStreaming").method())
        .isSameInstanceAs(CLIENT_STREAMING_METHOD);
  }

  @Test
  public void method_accessAsServiceAndMethod() {
    assertThat(client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeUnary").method())
        .isSameInstanceAs(UNARY_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeServerStreaming").method())
        .isSameInstanceAs(SERVER_STREAMING_METHOD);
    assertThat(
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeClientStreaming").method())
        .isSameInstanceAs(CLIENT_STREAMING_METHOD);
  }

  @Test
  public void method_accessFromMethodInstance() {
    assertThat(client.method(CHANNEL_ID, UNARY_METHOD).method()).isSameInstanceAs(UNARY_METHOD);
    assertThat(client.method(CHANNEL_ID, SERVER_STREAMING_METHOD).method())
        .isSameInstanceAs(SERVER_STREAMING_METHOD);
    assertThat(client.method(CHANNEL_ID, CLIENT_STREAMING_METHOD).method())
        .isSameInstanceAs(CLIENT_STREAMING_METHOD);
  }

  @Test
  public void processPacket_emptyPacket_isNotProcessed() {
    assertThat(client.processPacket(new byte[] {})).isFalse();
  }

  @Test
  public void processPacket_invalidPacket_isNotProcessed() {
    assertThat(client.processPacket("This is definitely not a packet!".getBytes(UTF_8))).isFalse();
  }

  @Test
  public void processPacket_packetNotForClient_isNotProcessed() {
    assertThat(client.processPacket(RpcPacket.newBuilder()
                                        .setType(PacketType.REQUEST)
                                        .setChannelId(CHANNEL_ID)
                                        .setServiceId(123)
                                        .setMethodId(456)
                                        .build()
                                        .toByteArray()))
        .isFalse();
  }

  @Test
  public void processPacket_unrecognizedChannel_isNotProcessed() {
    assertThat(client.processPacket(RpcPacket.newBuilder()
                                        .setType(PacketType.RESPONSE)
                                        .setChannelId(CHANNEL_ID + 100)
                                        .setServiceId(123)
                                        .setMethodId(456)
                                        .build()
                                        .toByteArray()))
        .isFalse();
  }

  @Test
  public void processPacket_unrecognizedService_sendsError() {
    assertThat(client.processPacket(response("pw.rpc.test1.NotAService", "SomeUnary"))).isTrue();

    assertThat(packetsSent)
        .containsExactly(packetBuilder("pw.rpc.test1.NotAService", "SomeUnary")
                             .setType(PacketType.CLIENT_ERROR)
                             .setStatus(Status.NOT_FOUND.code())
                             .build());
  }

  @Test
  public void processPacket_unrecognizedMethod_sendsError() {
    assertThat(client.processPacket(response("pw.rpc.test1.TheTestService", "NotMethod"))).isTrue();

    assertThat(packetsSent)
        .containsExactly(packetBuilder("pw.rpc.test1.TheTestService", "NotMethod")
                             .setType(PacketType.CLIENT_ERROR)
                             .setStatus(Status.NOT_FOUND.code())
                             .build());
  }

  @Test
  public void processPacket_nonPendingMethod_sendsError() {
    assertThat(client.processPacket(response("pw.rpc.test1.TheTestService", "SomeUnary"))).isTrue();

    assertThat(packetsSent)
        .containsExactly(packetBuilder("pw.rpc.test1.TheTestService", "SomeUnary")
                             .setType(PacketType.CLIENT_ERROR)
                             .setStatus(Status.FAILED_PRECONDITION.code())
                             .build());
  }

  @Test
  public void processPacket_serverError_abortsPending() throws Exception {
    MethodClient method = client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeUnary");
    Call call = method.invokeUnary(SomeMessage.getDefaultInstance());

    assertThat(client.processPacket(serverReply(PacketType.SERVER_ERROR,
                   "pw.rpc.test1.TheTestService",
                   "SomeUnary",
                   Status.NOT_FOUND,
                   SomeMessage.getDefaultInstance())))
        .isTrue();
    assertThat(call.error()).isEqualTo(Status.NOT_FOUND);
  }

  @Test
  public void processPacket_responseToPendingUnaryMethod_callsObserver() throws Exception {
    MethodClient method = client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeUnary");

    method.invokeUnary(REQUEST_PAYLOAD, observer);

    assertThat(packetsSent)
        .containsExactly(
            requestPacket("pw.rpc.test1.TheTestService", "SomeUnary", REQUEST_PAYLOAD));

    assertThat(
        client.processPacket(response(
            "pw.rpc.test1.TheTestService", "SomeUnary", Status.ALREADY_EXISTS, RESPONSE_PAYLOAD)))
        .isTrue();

    verify(observer).onNext(RESPONSE_PAYLOAD);
    verify(observer).onCompleted(Status.ALREADY_EXISTS);
  }

  @Test
  public void processPacket_responsesToPendingServerStreamingMethod_callsObserver()
      throws Exception {
    MethodClient method =
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeServerStreaming");

    method.invokeServerStreaming(REQUEST_PAYLOAD, observer);

    assertThat(packetsSent)
        .containsExactly(
            requestPacket("pw.rpc.test1.TheTestService", "SomeServerStreaming", REQUEST_PAYLOAD));

    assertThat(client.processPacket(serverStream(
                   "pw.rpc.test1.TheTestService", "SomeServerStreaming", RESPONSE_PAYLOAD)))
        .isTrue();

    verify(observer).onNext(RESPONSE_PAYLOAD);

    assertThat(client.processPacket(response(
                   "pw.rpc.test1.TheTestService", "SomeServerStreaming", Status.UNAUTHENTICATED)))
        .isTrue();

    verify(observer).onCompleted(Status.UNAUTHENTICATED);
  }

  @Test
  public void processPacket_responsePacket_completesRpc() throws Exception {
    MethodClient method =
        client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeServerStreaming");

    method.invokeServerStreaming(REQUEST_PAYLOAD, observer);

    assertThat(client.processPacket(
                   response("pw.rpc.test1.TheTestService", "SomeServerStreaming", Status.OK)))
        .isTrue();

    verify(observer).onCompleted(Status.OK);

    assertThat(client.processPacket(serverStream(
                   "pw.rpc.test1.TheTestService", "SomeServerStreaming", RESPONSE_PAYLOAD)))
        .isTrue();

    verify(observer, never()).onNext(any());
  }

  @Test
  @SuppressWarnings("unchecked") // No idea why, but this test causes "unchecked" warnings
  public void streamObserverClient_create_invokeMethod() throws Exception {
    Channel.Output mockChannelOutput = Mockito.mock(Channel.Output.class);
    Client client = Client.create(ImmutableList.of(new Channel(1, mockChannelOutput)),
        ImmutableList.of(SERVICE),
        (rpc) -> Mockito.mock(StreamObserver.class));

    SomeMessage payload = SomeMessage.newBuilder().setMagicNumber(99).build();
    client.method(CHANNEL_ID, "pw.rpc.test1.TheTestService", "SomeUnary").invokeUnary(payload);
    verify(mockChannelOutput)
        .send(requestPacket("pw.rpc.test1.TheTestService", "SomeUnary", payload).toByteArray());
  }
}
