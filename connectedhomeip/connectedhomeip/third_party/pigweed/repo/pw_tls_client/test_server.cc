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

#include "pw_tls_client/test/test_server.h"

#include <cstring>

#include "pw_log/log.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace {

int TestBioNew(BIO* bio) {
  bio->init = 1;
  return 1;
}

long TestBioCtrl(BIO*, int, long, void*) { return 1; }

int TestBioFree(BIO*) { return 1; }

}  // namespace

namespace pw::tls_client::test {

Result<X509*> ParseDerCertificate(ConstByteSpan cert) {
  BIO* bio = BIO_new_mem_buf(cert.data(), cert.size());
  if (!bio) {
    return Status::Internal();
  }

  X509* x509 = d2i_X509_bio(bio, nullptr);
  if (x509 == nullptr) {
    PW_LOG_DEBUG("Failed to parse x509");
    BIO_free(bio);
    return Status::Internal();
  }

  BIO_free(bio);
  return x509;
}

StatusWithSize FixedSizeFIFOBuffer::DoRead(ByteSpan dest) {
  size_t to_read = std::min(current_size_, dest.size());
  memcpy(dest.data(), buffer_.data(), to_read);
  // Push out the read data.
  memmove(buffer_.data(), buffer_.data() + to_read, current_size_ - to_read);
  current_size_ -= to_read;
  return StatusWithSize(to_read);
}

Status FixedSizeFIFOBuffer::DoWrite(ConstByteSpan data) {
  if (data.size() + current_size_ > buffer_.size()) {
    PW_LOG_ERROR("Write overflow. Buffer is too small.");
    return Status::ResourceExhausted();
  }

  // Append incoming data at the end.
  memcpy(buffer_.begin() + current_size_, data.data(), data.size());
  current_size_ += data.size();
  return OkStatus();
}

InMemoryTestServer::InMemoryTestServer(ByteSpan input_buffer,
                                       ByteSpan output_buffer)
    : input_buffer_(input_buffer), output_buffer_(output_buffer) {}

int InMemoryTestServer::BioRead(BIO* bio, char* out, int output_length) {
  auto server = static_cast<InMemoryTestServer*>(bio->ptr);
  auto read = server->input_buffer_.Read(
      as_writable_bytes(span{out, static_cast<size_t>(output_length)}));
  if (!read.ok()) {
    server->last_bio_status_ = read.status();
    return -1;
  }
  if (read.value().empty()) {
    BIO_set_retry_read(bio);
    return -1;
  }
  return static_cast<int>(read.value().size());
}

int InMemoryTestServer::BioWrite(BIO* bio,
                                 const char* input,
                                 int input_length) {
  auto server = static_cast<InMemoryTestServer*>(bio->ptr);
  if (auto status = server->output_buffer_.Write(
          as_bytes(span{input, static_cast<size_t>(input_length)}));
      !status.ok()) {
    server->last_bio_status_ = status;
    return -1;
  }

  return input_length;
}

Status InMemoryTestServer::Initialize(ConstByteSpan key,
                                      ConstByteSpan cert,
                                      span<const ConstByteSpan> chains) {
  input_buffer_.clear();
  output_buffer_.clear();
  is_handshake_done_ = false;

  ctx_ = bssl::UniquePtr<SSL_CTX>(SSL_CTX_new(TLS_method()));
  if (!ctx_) {
    return Status::Internal();
  }

  if (auto status = LoadPrivateKey(key); !status.ok()) {
    return status;
  }

  if (auto status = LoadCertificate(cert); !status.ok()) {
    return status;
  }

  if (auto status = LoadCAChain(chains); !status.ok()) {
    return status;
  }

  ssl_ = bssl::UniquePtr<SSL>(SSL_new(ctx_.get()));
  if (!ssl_) {
    return Status::Internal();
  }

  static const BIO_METHOD bio_method = {
      BIO_TYPE_MEM,
      "bio boringssl test server",
      InMemoryTestServer::BioWrite,
      InMemoryTestServer::BioRead,
      nullptr,      // puts
      nullptr,      // gets
      TestBioCtrl,  // ctrl
      TestBioNew,
      TestBioFree,  // free
      nullptr       // callback_ctrl
  };

  BIO* bio = BIO_new(&bio_method);
  if (!bio) {
    return Status::Internal();
  }

  bio->ptr = this;

  SSL_set_bio(ssl_.get(), bio, bio);
  return OkStatus();
}

Status InMemoryTestServer::LoadPrivateKey(ConstByteSpan key) {
  BIO* bio = BIO_new_mem_buf(key.data(), key.size());
  if (!bio) {
    return Status::Internal();
  }

  // Requires PEM format.
  EVP_PKEY* pkey = d2i_PrivateKey_bio(bio, nullptr);
  int ret = SSL_CTX_use_PrivateKey(ctx_.get(), pkey);
  if (ret != 1) {
    BIO_free(bio);
    PW_LOG_DEBUG("Failed to load private key for test server");
    return Status::Internal();
  }

  EVP_PKEY_free(pkey);
  BIO_free(bio);
  return OkStatus();
}

Status InMemoryTestServer::LoadCertificate(ConstByteSpan cert) {
  auto res = ParseDerCertificate(cert);
  if (!res.ok()) {
    return res.status();
  }

  int ret = SSL_CTX_use_certificate(ctx_.get(), res.value());
  if (ret != 1) {
    X509_free(res.value());
    PW_LOG_DEBUG("Failed to user server certificate %d", ret);
    return Status::Internal();
  }

  X509_free(res.value());
  return OkStatus();
}

Status InMemoryTestServer::LoadCAChain(span<const ConstByteSpan> chains) {
  for (auto cert : chains) {
    auto res = ParseDerCertificate(cert);
    if (!res.ok()) {
      return res.status();
    }

    int ret = SSL_CTX_add0_chain_cert(ctx_.get(), res.value());
    if (ret != 1) {
      X509_free(res.value());
      PW_LOG_DEBUG("Failed to add certificate to chain %d", ret);
      return Status::Internal();
    }
  }
  return OkStatus();
}

bool InMemoryTestServer::ClientShutdownReceived() {
  return SSL_get_shutdown(ssl_.get()) & SSL_RECEIVED_SHUTDOWN;
}

Status InMemoryTestServer::ProcessPackets() {
  // Process handshake if it has not been completed.
  if (!is_handshake_done_) {
    int ret = SSL_accept(ssl_.get());
    if (ret != 1) {
      int ssl_err = SSL_get_error(ssl_.get(), ret);
      if (ssl_err != SSL_ERROR_WANT_READ) {
        PW_LOG_DEBUG("Error while server accepting, %d, %d", ssl_err, ret);
        return Status::Internal();
      }
    } else {
      // handshake complete.
      is_handshake_done_ = true;
    }
  }

  // Hanshake may be completed above and there may already be application data.
  if (is_handshake_done_) {
    static std::array<std::byte, 1024> buf;
    while (true) {
      int ssl_ret = SSL_read(ssl_.get(), buf.data(), buf.size());

      if (ssl_ret == 0) {
        // All input has been processed.
        break;
      }

      if (ssl_ret < 0) {
        // An error may have occured.
        int ssl_err = SSL_get_error(ssl_.get(), ssl_ret);
        if (ssl_err == SSL_ERROR_WANT_READ) {
          // Need to wait for client to finish sending data. Non-blocking
          // return.
          break;
        }

        PW_LOG_DEBUG("Error while server reading");
        return Status::Internal();
      }

      // Echo the message
      int write_status = SSL_write(ssl_.get(), buf.data(), ssl_ret);
      if (write_status <= 0) {
        PW_LOG_DEBUG("Failed to write for test server");
        return Status::Internal();
      }
    }
  }

  return OkStatus();
}

StatusWithSize InMemoryTestServer::DoRead(ByteSpan dest) {
  auto res = output_buffer_.Read(dest);
  if (!res.ok()) {
    return StatusWithSize(res.status(), 0);
  }

  return StatusWithSize(res.value().size());
}

Status InMemoryTestServer::DoWrite(ConstByteSpan data) {
  auto status = input_buffer_.Write(data);
  if (!status.ok()) {
    return status;
  }

  // Invoke the server to process the data.
  return ProcessPackets();
}

}  // namespace pw::tls_client::test
