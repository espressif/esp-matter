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
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import dev.pigweed.pw_rpc.StreamObserverCall.StreamResponseFuture;
import dev.pigweed.pw_rpc.StreamObserverCall.UnaryResponseFuture;
import dev.pigweed.pw_rpc.internal.Packet.PacketType;
import dev.pigweed.pw_rpc.internal.Packet.RpcPacket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;

public final class StreamObserverCallTest {
  @Rule public final MockitoRule mockito = MockitoJUnit.rule();

  private static final Service SERVICE = new Service("pw.rpc.test1.TheTestService",
      Service.unaryMethod("SomeUnary", SomeMessage.class, AnotherMessage.class),
      Service.clientStreamingMethod("SomeClient", SomeMessage.class, AnotherMessage.class),
      Service.bidirectionalStreamingMethod(
          "SomeBidirectional", SomeMessage.class, AnotherMessage.class));
  private static final Method METHOD = SERVICE.method("SomeUnary");
  private static final int CHANNEL_ID = 555;

  @Mock private StreamObserver<AnotherMessage> observer;
  @Mock private Channel.Output mockOutput;

  private final RpcManager rpcManager = new RpcManager();
  private StreamObserverCall<SomeMessage, AnotherMessage> streamObserverCall;
  private PendingRpc rpc;

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

  @Before
  public void createCall() throws Exception {
    rpc = PendingRpc.create(new Channel(CHANNEL_ID, mockOutput), SERVICE, METHOD);
    streamObserverCall = StreamObserverCall.start(rpcManager, rpc, observer, null);
    rpcManager.start(rpc, streamObserverCall, SomeMessage.getDefaultInstance());
  }

  @Test
  public void startsActive() {
    assertThat(streamObserverCall.active()).isTrue();
  }

  @Test
  public void startsWithNullStatus() {
    assertThat(streamObserverCall.status()).isNull();
  }

  @Test
  public void cancel_sendsCancelPacket() throws Exception {
    streamObserverCall.cancel();

    verify(mockOutput).send(cancel());
  }

  @Test
  public void cancel_deactivates() throws Exception {
    streamObserverCall.cancel();

    assertThat(streamObserverCall.active()).isFalse();
  }

  @Test
  public void abandon_doesNotSendCancelPacket() throws Exception {
    streamObserverCall.abandon();

    verify(mockOutput, never()).send(cancel());
  }

  @Test
  public void abandon_deactivates() {
    streamObserverCall.abandon();

    assertThat(streamObserverCall.active()).isFalse();
  }

  @Test
  public void send_sendsClientStreamPacket() throws Exception {
    SomeMessage request = SomeMessage.newBuilder().setMagicNumber(123).build();
    streamObserverCall.send(request);

    verify(mockOutput)
        .send(packetBuilder()
                  .setType(PacketType.CLIENT_STREAM)
                  .setPayload(request.toByteString())
                  .build()
                  .toByteArray());
  }

  @Test
  public void send_raisesExceptionIfClosed() throws Exception {
    streamObserverCall.cancel();

    RpcError thrown = assertThrows(
        RpcError.class, () -> streamObserverCall.send(SomeMessage.getDefaultInstance()));
    assertThat(thrown.status()).isSameInstanceAs(Status.CANCELLED);
  }

  @Test
  public void finish_clientStreamEndPacket() throws Exception {
    streamObserverCall.finish();

    verify(mockOutput)
        .send(packetBuilder().setType(PacketType.CLIENT_STREAM_END).build().toByteArray());
  }

  @Test
  public void onNext_callsObserverIfActive() {
    streamObserverCall.onNext(AnotherMessage.getDefaultInstance().toByteString());

    verify(observer).onNext(AnotherMessage.getDefaultInstance());
  }

  @Test
  public void onNext_ignoresIfNotActive() throws Exception {
    streamObserverCall.cancel();
    streamObserverCall.onNext(AnotherMessage.getDefaultInstance().toByteString());

    verify(observer, never()).onNext(any());
  }

  @Test
  public void callDispatcher_onCompleted_callsObserver() {
    streamObserverCall.onCompleted(Status.ABORTED);

    verify(observer).onCompleted(Status.ABORTED);
  }

  @Test
  public void callDispatcher_onCompleted_setsActiveAndStatus() {
    streamObserverCall.onCompleted(Status.ABORTED);

    verify(observer).onCompleted(Status.ABORTED);
    assertThat(streamObserverCall.active()).isFalse();
    assertThat(streamObserverCall.status()).isEqualTo(Status.ABORTED);
  }

  @Test
  public void callDispatcher_onError_callsObserver() {
    streamObserverCall.onError(Status.NOT_FOUND);

    verify(observer).onError(Status.NOT_FOUND);
  }

  @Test
  public void callDispatcher_onError_deactivates() {
    streamObserverCall.onError(Status.ABORTED);

    verify(observer).onError(Status.ABORTED);
    assertThat(streamObserverCall.active()).isFalse();
    assertThat(streamObserverCall.status()).isNull();
  }

  @Test
  public void unaryFuture_response_setsValue() throws Exception {
    UnaryResponseFuture<SomeMessage, AnotherMessage> call =
        new UnaryResponseFuture<>(rpcManager, rpc, SomeMessage.getDefaultInstance());

    AnotherMessage response = AnotherMessage.newBuilder().setResultValue(1138).build();
    call.onNext(response);
    assertThat(call.isDone()).isFalse();
    call.onCompleted(Status.CANCELLED);

    assertThat(call.isDone()).isTrue();
    assertThat(call.get()).isEqualTo(UnaryResult.create(response, Status.CANCELLED));
  }

  @Test
  public void unaryFuture_serverError_setsException() {
    UnaryResponseFuture<SomeMessage, AnotherMessage> call =
        new UnaryResponseFuture<>(rpcManager, rpc, SomeMessage.getDefaultInstance());

    call.onError(Status.NOT_FOUND);

    assertThat(call.isDone()).isTrue();
    ExecutionException exception = assertThrows(ExecutionException.class, call::get);
    assertThat(exception).hasCauseThat().isInstanceOf(RpcError.class);

    RpcError error = (RpcError) exception.getCause();
    assertThat(error).isNotNull();
    assertThat(error.rpc()).isEqualTo(rpc);
    assertThat(error.status()).isEqualTo(Status.NOT_FOUND);
  }

  @Test
  public void unaryFuture_noMessage_setsException() {
    UnaryResponseFuture<SomeMessage, AnotherMessage> call =
        new UnaryResponseFuture<>(rpcManager, rpc, SomeMessage.getDefaultInstance());

    call.onCompleted(Status.OK);

    assertThat(call.isDone()).isTrue();
    ExecutionException exception = assertThrows(ExecutionException.class, call::get);
    assertThat(exception).hasCauseThat().isInstanceOf(IllegalStateException.class);
  }

  @Test
  public void unaryFuture_multipleResponses_setsException() {
    UnaryResponseFuture<SomeMessage, AnotherMessage> call =
        new UnaryResponseFuture<>(rpcManager, rpc, SomeMessage.getDefaultInstance());

    AnotherMessage response = AnotherMessage.newBuilder().setResultValue(1138).build();
    call.onNext(response);
    call.onNext(response);
    call.onCompleted(Status.OK);

    assertThat(call.isDone()).isTrue();
    ExecutionException exception = assertThrows(ExecutionException.class, call::get);
    assertThat(exception).hasCauseThat().isInstanceOf(IllegalStateException.class);
  }

  @Test
  public void bidirectionalStreamingFuture_responses_setsValue() throws Exception {
    List<AnotherMessage> responses = new ArrayList<>();
    StreamResponseFuture<SomeMessage, AnotherMessage> call =
        new StreamResponseFuture<>(rpcManager, rpc, responses::add, null);

    AnotherMessage message = AnotherMessage.newBuilder().setResultValue(1138).build();
    call.onNext(message);
    call.onNext(message);
    assertThat(call.isDone()).isFalse();
    call.onCompleted(Status.OK);

    assertThat(call.isDone()).isTrue();
    assertThat(call.get()).isEqualTo(Status.OK);
    assertThat(responses).containsExactly(message, message);
  }

  @Test
  public void bidirectionalStreamingFuture_serverError_setsException() {
    StreamResponseFuture<SomeMessage, AnotherMessage> call =
        new StreamResponseFuture<>(rpcManager, rpc, (msg) -> {}, null);

    call.onError(Status.NOT_FOUND);

    assertThat(call.isDone()).isTrue();
    ExecutionException exception = assertThrows(ExecutionException.class, call::get);
    assertThat(exception).hasCauseThat().isInstanceOf(RpcError.class);

    RpcError error = (RpcError) exception.getCause();
    assertThat(error).isNotNull();
    assertThat(error.rpc()).isEqualTo(rpc);
    assertThat(error.status()).isEqualTo(Status.NOT_FOUND);
  }
}
