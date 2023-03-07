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

#pragma once

#include "pw_preprocessor/compiler.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wcast-qual");
PW_MODIFY_DIAGNOSTIC(ignored, "-Wignored-qualifiers");
PW_MODIFY_DIAGNOSTIC(ignored, "-Wpedantic");
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
PW_MODIFY_DIAGNOSTICS_POP();

#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::tls_client::test {

class FixedSizeFIFOBuffer : public stream::NonSeekableReaderWriter {
 public:
  FixedSizeFIFOBuffer() = delete;
  FixedSizeFIFOBuffer(const FixedSizeFIFOBuffer&) = delete;
  FixedSizeFIFOBuffer& operator=(const FixedSizeFIFOBuffer&) = delete;

  FixedSizeFIFOBuffer(ByteSpan buffer) : buffer_(buffer) {}
  void clear() { current_size_ = 0; }

 private:
  StatusWithSize DoRead(ByteSpan dest) override;
  Status DoWrite(ConstByteSpan data) override;
  ByteSpan buffer_;
  size_t current_size_ = 0;
};

// Writing to the server is equivalent to sending data to the server. Server
// will be invoked to process the data when being written. The write does
// not return until the server completes processing it.
// Reading from the server is equivalent to receiving data from the server.
//
// The server accepts is only for one client and echo messages it sends.
class InMemoryTestServer : public stream::NonSeekableReaderWriter {
 public:
  InMemoryTestServer() = delete;
  InMemoryTestServer(const InMemoryTestServer&) = delete;
  InMemoryTestServer& operator=(const InMemoryTestServer&) = delete;

  // `input_buffer` is for storing raw data sent from the client.
  // `output_buffer` is for storing raw data server prepare and to be sent
  // to the client.
  //
  // The required size of the buffer depends on the payload and needs to be
  // determined by the users based on use cases.
  InMemoryTestServer(ByteSpan input_buffer, ByteSpan output_buffer);

  // Initialize a test server with a private key, server certificate, and
  // CA chains (all DER format)
  Status Initialize(ConstByteSpan key,
                    ConstByteSpan cert,
                    span<const ConstByteSpan> chains);

  // Is handshake completed.
  bool SessionEstablished() { return is_handshake_done_; }

  // Returns whether a shutdown request has been received from the client.
  bool ClientShutdownReceived();

  Status GetLastBioStatus() { return last_bio_status_; }

 private:
  bssl::UniquePtr<SSL_CTX> ctx_;
  bssl::UniquePtr<SSL> ssl_;
  bool is_handshake_done_ = false;

  // Buffer for storing data sent from the client.
  FixedSizeFIFOBuffer input_buffer_;

  // Buffer for storing data prepared by the server to send to the client.
  FixedSizeFIFOBuffer output_buffer_;

  // Store the last status of BIO operation (in BioRead() and BioWrite());
  Status last_bio_status_ = OkStatus();

  // Process the data written to the server as much as possible.
  // If server is in handshake process, it processes handshake and prepares data
  // to send to the server . If server is in application data exchange
  // phase, it decrypts data and echoes back to the client. Client can retrieve
  // the message by reading from the server.
  Status ProcessPackets();

  // Methods for loading private key, certificate, and intermediate CA chain.
  Status LoadPrivateKey(ConstByteSpan key);
  Status LoadCertificate(ConstByteSpan cert);
  Status LoadCAChain(span<const ConstByteSpan> chains);

  // Methods for providing BIO interfaces.
  static int BioRead(BIO* bio, char* out, int output_length);
  static int BioWrite(BIO* bio, const char* in, int input_length);

  StatusWithSize DoRead(ByteSpan dest) override;
  Status DoWrite(ConstByteSpan data) override;
};

// A helper function to parse a DER format certificate.
pw::Result<X509*> ParseDerCertificate(pw::ConstByteSpan cert);

}  // namespace pw::tls_client::test
