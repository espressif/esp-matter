// Copyright 2022 The Pigweed Authors
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

import com.google.common.collect.ImmutableList;
import com.google.protobuf.TextFormat;
import dev.pigweed.pw_hdlc.Decoder;
import dev.pigweed.pw_hdlc.Encoder;
import dev.pigweed.pw_hdlc.Frame;
import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.Channel;
import dev.pigweed.pw_rpc.ChannelOutputException;
import dev.pigweed.pw_rpc.Client;
import dev.pigweed.pw_rpc.Status;
import dev.pigweed.pw_transfer.ProtocolVersion;
import dev.pigweed.pw_transfer.TransferClient;
import dev.pigweed.pw_transfer.TransferError;
import dev.pigweed.pw_transfer.TransferService;
import dev.pigweed.pw_transfer.TransferTimeoutSettings;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.concurrent.ExecutionException;
import pw.transfer.ConfigProtos;
import pw.transfer.ConfigProtos.TransferAction;

public class JavaClient {
  private static final String SERVICE = "pw.transfer.Transfer";
  private static final Logger logger = Logger.forClass(Client.class);

  private static final int CHANNEL_ID = 1;
  private static final long RPC_HDLC_ADDRESS = 'R';
  private static final String HOSTNAME = "localhost";

  // This is the maximum size of the socket send buffers. Ideally, this is set
  // to the lowest allowed value to minimize buffering between the proxy and
  // clients so rate limiting causes the client to block and wait for the
  // integration test proxy to drain rather than allowing OS buffers to backlog
  // large quantities of data.
  //
  // Note that the OS may chose to not strictly follow this requested buffer
  // size. Still, setting this value to be as small as possible does reduce
  // bufer sizes significantly enough to better reflect typical inter-device
  // communication.
  //
  // For this to be effective, servers should also configure their sockets to a
  // smaller receive buffer size.
  private static final int MAX_SOCKET_SEND_BUFFER_SIZE = 1;

  private final HdlcRpcChannelOutput channelOutput;
  private final Client rpcClient;
  private final HdlcParseThread parseThread;

  public JavaClient(OutputStream writer, InputStream reader) {
    this.channelOutput = new HdlcRpcChannelOutput(writer, RPC_HDLC_ADDRESS);

    this.rpcClient = Client.create(ImmutableList.of(new Channel(CHANNEL_ID, this.channelOutput)),
        ImmutableList.of(TransferService.get()));

    this.parseThread = new HdlcParseThread(reader, this.rpcClient);
  }

  void startClient() {
    parseThread.start();
  }

  Client getRpcClient() {
    return this.rpcClient;
  }

  private class HdlcRpcChannelOutput implements Channel.Output {
    private final OutputStream writer;
    private final long address;

    public HdlcRpcChannelOutput(OutputStream writer, long address) {
      this.writer = writer;
      this.address = address;
    }

    public void send(byte[] packet) throws ChannelOutputException {
      try {
        Encoder.writeUiFrame(this.address, ByteBuffer.wrap(packet), this.writer);
      } catch (IOException e) {
        throw new ChannelOutputException("Failed to write HDLC UI Frame", e);
      }
    }
  }

  private class HdlcParseThread extends Thread {
    private final InputStream reader;
    private final RpcOnComplete frame_handler;
    private final Decoder decoder;

    private class RpcOnComplete implements Decoder.OnCompleteFrame {
      private final Client rpc_client;

      public RpcOnComplete(Client rpc_client) {
        this.rpc_client = rpc_client;
      }

      public void onCompleteFrame(Frame frame) {
        if (frame.getAddress() == RPC_HDLC_ADDRESS) {
          this.rpc_client.processPacket(frame.getPayload());
        }
      }
    }

    public HdlcParseThread(InputStream reader, Client rpc_client) {
      this.reader = reader;
      this.frame_handler = new RpcOnComplete(rpc_client);
      this.decoder = new Decoder(this.frame_handler);
    }

    public void run() {
      while (true) {
        int val = 0;
        try {
          val = this.reader.read();
        } catch (IOException e) {
          logger.atSevere().log("HDLC parse thread read failed");
          System.exit(1);
        }
        this.decoder.process((byte) val);
      }
    }
  }

  public static ConfigProtos.ClientConfig ParseConfigFrom(InputStream reader) throws IOException {
    byte[] buffer = new byte[reader.available()];
    reader.read(buffer);
    ConfigProtos.ClientConfig.Builder config_builder = ConfigProtos.ClientConfig.newBuilder();
    TextFormat.merge(new String(buffer, StandardCharsets.UTF_8), config_builder);
    if (config_builder.getChunkTimeoutMs() == 0) {
      throw new AssertionError("chunk_timeout_ms may not be 0");
    }
    if (config_builder.getInitialChunkTimeoutMs() == 0) {
      throw new AssertionError("initial_chunk_timeout_ms may not be 0");
    }
    if (config_builder.getMaxRetries() == 0) {
      throw new AssertionError("max_retries may not be 0");
    }
    if (config_builder.getMaxLifetimeRetries() == 0) {
      throw new AssertionError("max_lifetime_retries may not be 0");
    }
    return config_builder.build();
  }

  public static void ReadFromServer(
      int resourceId, Path fileName, TransferClient client, Status expected_status) {
    byte[] data;
    try {
      data = client.read(resourceId).get();
    } catch (ExecutionException e) {
      if (((TransferError) e.getCause()).status() != expected_status) {
        throw new AssertionError("Unexpected transfer read failure", e);
      }
      // Expected failure occurred, skip trying to write the data knowing that
      // it is missing.
      return;
    } catch (InterruptedException e) {
      throw new AssertionError("Read from server failed", e);
    }

    try {
      Files.write(fileName, data);
    } catch (IOException e) {
      logger.atSevere().log("Failed to write to output file `%s`", fileName);
      throw new AssertionError("Failed to write output file from server", e);
    }
  }

  public static void WriteToServer(
      int resourceId, Path fileName, TransferClient client, Status expected_status) {
    if (Files.notExists(fileName)) {
      logger.atSevere().log("Input file `%s` does not exist", fileName);
    }

    byte[] data;
    try {
      data = Files.readAllBytes(fileName);
    } catch (IOException e) {
      logger.atSevere().log("Failed to read input file `%s`", fileName);
      throw new AssertionError("Failed to read input file on write to server", e);
    }

    try {
      client.write(resourceId, data).get();
    } catch (ExecutionException e) {
      if (((TransferError) e.getCause()).status() != expected_status) {
        throw new AssertionError("Unexpected transfer write failure", e);
      }
    } catch (InterruptedException e) {
      throw new AssertionError("Write to server failed", e);
    }
  }

  public static void main(String[] args) {
    if (args.length != 1) {
      logger.atSevere().log("Usage: PORT");
      System.exit(1);
    }

    // The port is provided directly as a commandline argument.
    int port = Integer.parseInt(args[0]);

    ConfigProtos.ClientConfig config;
    try {
      config = ParseConfigFrom(System.in);
    } catch (IOException e) {
      throw new AssertionError("Failed to parse config file from stdin", e);
    }

    Socket socket;
    try {
      socket = new Socket(HOSTNAME, port);
    } catch (IOException e) {
      logger.atSevere().log("Failed to connect to %s:%d", HOSTNAME, port);
      throw new AssertionError("Failed to connect to server/proxy port", e);
    }
    try {
      socket.setSendBufferSize(MAX_SOCKET_SEND_BUFFER_SIZE);
    } catch (SocketException e) {
      logger.atSevere().log("Invalid socket buffer size %d", MAX_SOCKET_SEND_BUFFER_SIZE);
      throw new AssertionError("Invalid socket buffer size", e);
    }
    InputStream reader;
    OutputStream writer;

    try {
      writer = socket.getOutputStream();
      reader = socket.getInputStream();
    } catch (IOException e) {
      throw new AssertionError("Failed to open socket streams", e);
    }

    JavaClient hdlc_rpc_client = new JavaClient(writer, reader);

    hdlc_rpc_client.startClient();

    TransferClient client = new TransferClient(
        hdlc_rpc_client.getRpcClient().method(CHANNEL_ID, TransferService.get().name() + "/Read"),
        hdlc_rpc_client.getRpcClient().method(CHANNEL_ID, TransferService.get().name() + "/Write"),
        TransferTimeoutSettings.builder()
            .setTimeoutMillis(config.getChunkTimeoutMs())
            .setInitialTimeoutMillis(config.getInitialChunkTimeoutMs())
            .setMaxRetries(config.getMaxRetries())
            .setMaxLifetimeRetries(config.getMaxLifetimeRetries())
            .build());

    for (ConfigProtos.TransferAction action : config.getTransferActionsList()) {
      int resourceId = action.getResourceId();
      Path fileName = Paths.get(action.getFilePath());

      if (action.getProtocolVersion() != TransferAction.ProtocolVersion.UNKNOWN_VERSION) {
        client.setProtocolVersion(ProtocolVersion.values()[action.getProtocolVersionValue()]);
      } else {
        client.setProtocolVersion(ProtocolVersion.latest());
      }

      if (action.getTransferType() == ConfigProtos.TransferAction.TransferType.WRITE_TO_SERVER) {
        WriteToServer(resourceId, fileName, client, Status.fromCode(action.getExpectedStatus()));
      } else if (action.getTransferType()
          == ConfigProtos.TransferAction.TransferType.READ_FROM_SERVER) {
        ReadFromServer(resourceId, fileName, client, Status.fromCode(action.getExpectedStatus()));
      } else {
        throw new AssertionError("Unknown transfer action type");
      }
    }

    logger.atInfo().log("Transfer completed successfully");

    System.exit(0);
  }
}
