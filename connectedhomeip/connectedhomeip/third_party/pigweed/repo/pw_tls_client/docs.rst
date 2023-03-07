.. _module-pw_tls_client:

--------------
pw_tls_client
--------------

This module provides a facade that defines the public APIs for establishing TLS
sessions over arbitrary transports. Two options of backends,
pw_tls_client_mbedtls and pw_tls_client_boringssl, which are based on BoringSSL
and MbedTLS libraries, are under construction.

The facade provides a class ``pw::tls_client::Session`` with Open(), Read(),
Write() and Close() methods for TLS communication. An instance is created by
``pw::tls_client::Session::Create`` method. The method takes a
``pw::tls_client::SessionOptions`` object, which is used to configure TLS
connection options. The list of supported configurations currently include:

1. Host name of the target server. This will be used as the Server Name
Indication(SNI) extension during TLS handshake.

2. User-implemented transport. The underlying transport for the TLS
communication. It is an object that implements the interface of
``pw::stream::ReaderWriter``.

The module will also provide mechanisms/APIs for users to specify sources of
trust anchors, time and entropy. These are under construction.

.. warning::
  This module is under construction, not ready for use, and the documentation
  is incomplete.

Prerequisites
=============
This module requires the following dependencies:

1. Entropy
-----------
TLS requires an entropy source for generating random bytes. Users of this
module should provide one by implementing a backend to the
``pw_tls_client:entropy`` facade. The backend defaults to
``pw_tls_client:fake_entropy`` that does nothing.

2. Chromium Verifier
---------------------
BoringSSL backend uses chromium verifier for certication verification. If the
downstream project uses BoringSSL as the backend, the sources of the verifier,
which is part of the chorimum sources, needs to be downloaded in order for
``//third_party/chromium_verifier`` to build. It is recommended to use our
support in pw_package for downloading compatible and tested version:

.. code-block:: sh

  pw package install chromium_verifier

Then follow instruction for setting ``dir_pw_third_party_chromium_verifier`` to
the path of the downloaded repo.

3. Date time
-------------
TLS needs a trust-worthy source of wall clock time in order to check
expiration. Provisioning of time source for TLS communication is very specific
to the TLS library in use. However, common TLS libraires, such as BoringSSL
and MbedTLS, support the use of C APIs ``time()`` and ``getimtofday()`` for
obtaining date time. To accomodate the use of these libraries, a facade target
``pw_tls_client:time`` is added that wraps these APIs. For GN builds,
specify the backend target with variable ``pw_tls_client_C_TIME_BACKEND``.
``pw_tls_client_C_TIME_BACKEND`` defaults to the ``pw_tls_client::build_time``
backend that returns build time.

If downstream project chooses to use other TLS libraires that handle time source
differently, then it needs to be investigated separately.

4. CRLSet
-----------
The module supports CRLSet based revocation check for certificates. A CRLSet
file specifies a list of X509 certificates that either need to be blocked, or
have been revoked by the issuer. It is introduced by chromium and primarily
used for certificate verification/revocation checks during TLS handshake. The
format of a CRLSet file is available in
https://chromium.googlesource.com/chromium/src/+/refs/heads/main/net/cert/crl_set.cc#24.

Downstream projects need to provide a CRLSet file at build time. For GN builds,
specify the path of the CRLSet file with the GN variable
``pw_tls_client_CRLSET_FILE``. This module converts the CRLSet file into
source code at build time and generates APIs for querying certificate
block/revocation status. See ``pw_tls_client/crlset.h`` for more detail.

Chromium maintains its own CRLSet that targets at the general Internet. To use it,
run the following command to download the latest version:

.. code-block:: sh

  pw package install crlset --force

The `--force` option forces CRLSet to be always re-downloaded so that it is
up-to-date. Project that are concerned about up-to-date CRLSet should always
run the above command before build.

Toolings will be provided for generating custom CRLSet files from user-provided
certificate files. The functionality is under construction.

Setup
=====
This module requires the following setup:

  1. Choose a ``pw_tls_client`` backend, or write one yourself.
  2. If using GN build, Specify the ``pw_tls_client_BACKEND`` GN build arg to
     point the library that provides a ``pw_tls_client`` backend. To use the
     MbedTLS backend, set variable ``pw_tls_client_BACKEND`` to
     ``//pw_tls_client_mbedtls``. To use the BoringSSL backend, set it to
     ``//pw_tls_client_boringssl``.
  3. Provide a `pw_tls_client:entropy` backend. If using GN build, specify the
     backend with variable ``pw_tls_client_ENTROPY_BACKEND``.

Module usage
============
For GN build, add ``//pw_tls_client`` to the dependency list.

The following gives an example code for using the module on host platform.
The example uses a Pigweed socket stream as the transport and performs TLS
connection to www.google.com:

.. code-block:: cpp

  // Host domain name
  constexpr char kHost[] = "www.google.com";

  constexpr int kPort = 443;

  // Server Name Indication.
  constexpr const char* kServerNameIndication = kHost;

  // An example message to send.
  constexpr char kHTTPRequest[] = "GET / HTTP/1.1\r\n\r\n";

  // pw::stream::SocketStream doesn't accept host domain name as input. Thus we
  // introduce this helper function for getting the IP address
  pw::Status GetIPAddrFromHostName(std::string_view host, pw::span<char> ip) {
    char null_terminated_host_name[256] = {0};
    auto host_copy_status = pw::string::Copy(host, null_terminated_host_name);
    if (!host_copy_status.ok()) {
      return host_copy_status.status();
    }

    struct hostent* ent = gethostbyname(null_terminated_host_name);
    if (ent == NULL) {
      return PW_STATUS_INTERNAL;
    }

    in_addr** addr_list = reinterpret_cast<in_addr**>(ent->h_addr_list);
    if (addr_list[0] == nullptr) {
      return PW_STATUS_INTERNAL;
    }

    auto ip_copy_status = pw::string::Copy(inet_ntoa(*addr_list[0]), ip);
    if (!ip_copy_status.ok()) {
      return ip_copy_status.status();
    }

    return pw::OkStatus();
  }

  int main() {
    // Get the IP address of the target host.
    char ip_address[64] = {0};
    auto get_ip_status = GetIPAddrFromHostName(kHost, ip_address);
    if (!get_ip_status.ok()) {
      return 1;
    }

    // Use a socket stream as the transport.
    pw::stream::SocketStream socket_stream;

    // Connect the socket to the remote host.
    auto socket_connect_status = socket_stream.Connect(ip_address, kPort);
    if (!socket_connect_status.ok()) {
      return 1;
    }

    // Create a TLS session. Register the transport.
    auto options = pw::tls_client::SessionOptions()
            .set_server_name(kServerNameIndication)
            .set_transport(socket_stream);
    auto tls_conn = pw::tls_client::Session::Create(options);
    if (!tls_conn.ok()) {
      // Handle errors.
      return 1;
    }

    auto open_status = tls_conn.value()->Open();
    if (!open_status.ok()) {
      // Inspect/handle error with open_status.code() and
      // tls_conn.value()->GetLastTLSStatus().
      return 1;
    }

    auto write_status = tls_conn.value()->Write(pw::as_bytes(pw::span{kHTTPRequest}));
    if (!write_status.ok()) {
      // Inspect/handle error with write_status.code() and
      // tls_conn.value()->GetLastTLSStatus().
      return 0;
    }

    // Listen for incoming data.
    std::array<std::byte, 4096> buffer;
    while (true) {
      auto res = tls_conn.value()->Read(buffer);
      if (!res.ok()) {
        // Inspect/handle error with res.status().code() and
        // tls_conn.value()->GetLastTLSStatus().
        return 1;
      }

      // Process data in |buffer|. res.value() gives the span of read bytes.
      // The following simply print to console.
      if (res.value().size()) {
        auto print_status = pw::sys_io::WriteBytes(res.value());
        if (!print_status.ok()) {
          return 1;
        }
      }

    }
  }

A list of other demos will be provided in ``//pw_tls_client/examples/``

Warning
============

Open()/Read() APIs are synchronous for now. Support for
non-blocking/asynchronous usage will be added in the future.
