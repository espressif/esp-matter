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

package dev.pigweed.pw_transfer;

import com.google.auto.value.AutoValue;
import com.google.protobuf.ByteString;
import dev.pigweed.pw_rpc.Status;
import java.util.OptionalInt;
import java.util.OptionalLong;

/**
 * Abstraction of the Chunk proto that supports different protocol versions.
 */
@AutoValue
abstract class VersionedChunk {
  public static final int UNASSIGNED_SESSION_ID = 0;

  public abstract ProtocolVersion version();

  public abstract Chunk.Type type();

  public abstract int sessionId();

  public abstract OptionalInt resourceId();

  public abstract int offset();

  public abstract int windowEndOffset();

  public abstract ByteString data();

  public abstract OptionalLong remainingBytes();

  public abstract OptionalInt maxChunkSizeBytes();

  public abstract OptionalInt minDelayMicroseconds();

  public abstract OptionalInt status();

  public static Builder builder() {
    return new AutoValue_VersionedChunk.Builder()
        .setSessionId(UNASSIGNED_SESSION_ID)
        .setOffset(0)
        .setWindowEndOffset(0)
        .setData(ByteString.EMPTY);
  }

  @AutoValue.Builder
  public abstract static class Builder {
    public abstract Builder setVersion(ProtocolVersion version);

    public abstract Builder setType(Chunk.Type type);

    public abstract Builder setSessionId(int sessionId);

    public abstract Builder setResourceId(int resourceId);

    public abstract Builder setOffset(int offset);

    public abstract Builder setWindowEndOffset(int windowEndOffset);

    public abstract Builder setData(ByteString data);

    public abstract Builder setRemainingBytes(long remainingBytes);

    public abstract Builder setMaxChunkSizeBytes(int maxChunkSizeBytes);

    public abstract Builder setMinDelayMicroseconds(int minDelayMicroseconds);

    public final Builder setStatus(Status status) {
      return setStatus(status.code());
    }

    abstract Builder setStatus(int statusCode);

    public abstract VersionedChunk build();
  }

  public static VersionedChunk fromMessage(Chunk chunk) {
    Builder builder = builder();

    ProtocolVersion version;
    if (chunk.hasProtocolVersion()) {
      if (chunk.getProtocolVersion() < ProtocolVersion.values().length) {
        version = ProtocolVersion.values()[chunk.getProtocolVersion()];
      } else {
        version = ProtocolVersion.UNKNOWN;
      }
    } else if (chunk.hasSessionId()) {
      version = ProtocolVersion.VERSION_TWO;
    } else {
      version = ProtocolVersion.LEGACY;
    }
    builder.setVersion(version);

    if (chunk.hasType()) {
      builder.setType(chunk.getType());
    } else if (chunk.getOffset() == 0 && chunk.getData().isEmpty() && !chunk.hasStatus()) {
      builder.setType(Chunk.Type.START);
    } else if (!chunk.getData().isEmpty()) {
      builder.setType(Chunk.Type.DATA);
    } else if (chunk.hasStatus()) {
      builder.setType(Chunk.Type.COMPLETION);
    } else {
      builder.setType(Chunk.Type.PARAMETERS_RETRANSMIT);
    }

    // For legacy chunks, use the transfer ID as both the resource and session IDs.
    if (version == ProtocolVersion.LEGACY) {
      builder.setSessionId(chunk.getTransferId());
      builder.setResourceId(chunk.getTransferId());
      if (chunk.hasStatus()) {
        builder.setType(Chunk.Type.COMPLETION);
      }
    } else {
      builder.setSessionId(chunk.getSessionId());
    }

    builder.setOffset((int) chunk.getOffset()).setData(chunk.getData());

    if (chunk.hasResourceId()) {
      builder.setResourceId(chunk.getResourceId());
    }
    if (chunk.hasPendingBytes()) {
      builder.setWindowEndOffset((int) chunk.getOffset() + chunk.getPendingBytes());
    } else {
      builder.setWindowEndOffset(chunk.getWindowEndOffset());
    }
    if (chunk.hasRemainingBytes()) {
      builder.setRemainingBytes(chunk.getRemainingBytes());
    }
    if (chunk.hasMaxChunkSizeBytes()) {
      builder.setMaxChunkSizeBytes(chunk.getMaxChunkSizeBytes());
    }
    if (chunk.hasMinDelayMicroseconds()) {
      builder.setMinDelayMicroseconds(chunk.getMinDelayMicroseconds());
    }
    if (chunk.hasStatus()) {
      builder.setStatus(chunk.getStatus());
    }
    return builder.build();
  }

  public static VersionedChunk.Builder createInitialChunk(
      ProtocolVersion desiredVersion, int resourceId) {
    return builder().setVersion(desiredVersion).setType(Chunk.Type.START).setResourceId(resourceId);
  }

  public Chunk toMessage() {
    Chunk.Builder chunk = Chunk.newBuilder()
                              .setType(type())
                              .setOffset(offset())
                              .setWindowEndOffset(windowEndOffset())
                              .setData(data());

    resourceId().ifPresent(chunk::setResourceId);
    remainingBytes().ifPresent(chunk::setRemainingBytes);
    maxChunkSizeBytes().ifPresent(chunk::setMaxChunkSizeBytes);
    minDelayMicroseconds().ifPresent(chunk::setMinDelayMicroseconds);
    status().ifPresent(chunk::setStatus);

    // session_id did not exist in the legacy protocol, so don't send it.
    if (version() != ProtocolVersion.LEGACY && sessionId() != UNASSIGNED_SESSION_ID) {
      chunk.setSessionId(sessionId());
    }

    if (shouldEncodeLegacyFields()) {
      chunk.setTransferId(resourceId().orElse(sessionId()));

      if (chunk.getWindowEndOffset() != 0) {
        chunk.setPendingBytes(chunk.getWindowEndOffset() - offset());
      }
    }

    if (isInitialHandshakeChunk()) {
      chunk.setProtocolVersion(version().ordinal());
    }

    return chunk.build();
  }

  private boolean isInitialHandshakeChunk() {
    return version() == ProtocolVersion.VERSION_TWO
        && (type() == Chunk.Type.START || type() == Chunk.Type.START_ACK
            || type() == Chunk.Type.START_ACK_CONFIRMATION);
  }

  public final boolean requestsTransmissionFromOffset() {
    return type() == Chunk.Type.PARAMETERS_RETRANSMIT || type() == Chunk.Type.START_ACK_CONFIRMATION
        || type() == Chunk.Type.START;
  }

  private boolean shouldEncodeLegacyFields() {
    return version() == ProtocolVersion.LEGACY || type() == Chunk.Type.START;
  }
}
