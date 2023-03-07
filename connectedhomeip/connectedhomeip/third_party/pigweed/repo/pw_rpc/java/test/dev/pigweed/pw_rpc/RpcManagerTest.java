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
import static org.junit.Assert.assertThrows;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoInteractions;
import static org.mockito.Mockito.verifyNoMoreInteractions;

import com.google.protobuf.MessageLite;
import dev.pigweed.pw_rpc.internal.Packet.PacketType;
import dev.pigweed.pw_rpc.internal.Packet.RpcPacket;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;

public final class RpcManagerTest {
  @Rule public final MockitoRule mockito = MockitoJUnit.rule();

  private static final Service SERVICE = new Service("pw.rpc.test1.TheTestService",
      Service.unaryMethod("SomeUnary", SomeMessage.class, SomeMessage.class),
      Service.serverStreamingMethod("SomeServerStreaming", SomeMessage.class, SomeMessage.class),
      Service.clientStreamingMethod("SomeClientStreaming", SomeMessage.class, SomeMessage.class),
      Service.bidirectionalStreamingMethod(
          "SomeBidiStreaming", SomeMessage.class, SomeMessage.class));

  private static final Method METHOD = SERVICE.method("SomeUnary");

  private static final SomeMessage REQUEST_PAYLOAD =
      SomeMessage.newBuilder().setMagicNumber(1337).build();
  private static final byte[] REQUEST = request(REQUEST_PAYLOAD);
  private static final int CHANNEL_ID = 555;

  @Mock private Channel.Output mockOutput;
  @Mock private StreamObserverCall<MessageLite, MessageLite> call;

  private PendingRpc rpc;
  private RpcManager manager;

  @Before
  public void setup() {
    rpc = PendingRpc.create(new Channel(CHANNEL_ID, mockOutput), SERVICE, METHOD);
    manager = new RpcManager();
  }

  private static byte[] request(MessageLite payload) {
    return packetBuilder()
        .setType(PacketType.REQUEST)
        .setPayload(payload.toByteString())
        .build()
        .toByteArray();
  }

  private static byte[] cancel() {
    return packetBuilder()
        .setType(PacketType.CLIENT_ERROR)
        .setStatus(Status.CANCELLED.code())
        .build()
        .toByteArray();
  }

  private static RpcPacket.Builder packetBuilder() {
    return RpcPacket.newBuilder()
        .setChannelId(CHANNEL_ID)
        .setServiceId(SERVICE.id())
        .setMethodId(METHOD.id());
  }

  @Test
  public void start_sendingFails_rpcNotPending() throws Exception {
    doThrow(new ChannelOutputException()).when(mockOutput).send(any());

    assertThrows(ChannelOutputException.class, () -> manager.start(rpc, call, REQUEST_PAYLOAD));

    verify(mockOutput).send(REQUEST);
    assertThat(manager.getPending(rpc)).isNull();
  }

  @Test
  public void start_succeeds_rpcIsPending() throws Exception {
    assertThat(manager.start(rpc, call, REQUEST_PAYLOAD)).isNull();

    assertThat(manager.getPending(rpc)).isSameInstanceAs(call);
  }

  @Test
  public void startThenCancel_rpcNotPending() throws Exception {
    assertThat(manager.start(rpc, call, REQUEST_PAYLOAD)).isNull();
    assertThat(manager.cancel(rpc)).isSameInstanceAs(call);

    assertThat(manager.getPending(rpc)).isNull();
  }

  @Test
  public void startThenCancel_sendsCancelPacket() throws Exception {
    assertThat(manager.start(rpc, call, REQUEST_PAYLOAD)).isNull();
    assertThat(manager.cancel(rpc)).isEqualTo(call);

    verify(mockOutput).send(cancel());
  }

  @Test
  public void startThenClear_sendsNothing() throws Exception {
    verifyNoMoreInteractions(mockOutput);

    assertThat(manager.start(rpc, call, REQUEST_PAYLOAD)).isNull();
    assertThat(manager.clear(rpc)).isEqualTo(call);
  }

  @Test
  public void clear_notPending_returnsNull() {
    assertThat(manager.clear(rpc)).isNull();
  }

  @Test
  public void open_rpcIsPending() {
    assertThat(manager.open(rpc, call)).isNull();

    assertThat(manager.getPending(rpc)).isSameInstanceAs(call);
    verifyNoInteractions(mockOutput);
  }
}
