## Certificates -  Important to know

The Azure IoT Hub certificates presented during TLS negotiation shall be always validated using the appropriate root CA certificate(s).

The samples in this repository leverage the certificates in `certs.c` for the United States, Germany sovereign cloud and China sovereign cloud. Additionally, we provide the root certificate for use with ECC (enabled in IoT Hub Gateway V2). By default, all certs are included in the build. To select a specific cert, use one of the following options during the cmake step of your [environment setup](https://github.com/Azure/azure-iot-sdk-c/doc/devbox_setup.md).

```
cmake .. -Duse_baltimore_cert          // To use Baltimore CyberTrust Root (RSA based Root cert).
                                       // Default Root Cert supported by IoT Hub.

cmake .. -Duse_digicert_g3_cert        // To use DigiCert Global Root G3 (ECC based Root cert).
                                       // Only supported with IoT Hub Gateway V2.

cmake .. -Duse_microsoftazure_de_cert  // To use D-TRUST Root Class 3 CA 2 2009 (RSA based Root cert).
                                       // Root cert for Germany region.

cmake .. -Duse_portal_azure_cn_cert    // To use DigiCert Global Root CA (RSA based Root cert).
                                       // Root cert for China region.
```

For other regions (and private cloud environments), please use the appropriate root CA certificate.

IMPORTANT: Always prefer using the local system's Trusted Root Certificate Authority store instead of hardcoding the certificates (i.e. using certs.c which our samples require in certain combinations).

A couple of examples:

- Windows: Schannel will automatically pick up CA certificates from the store managed using `certmgr.msc`.
- Debian Linux: OpenSSL will automatically pick up CA certificates from the store installed using `apt install ca-certificates`. Adding a certificate to the store is described here: http://manpages.ubuntu.com/manpages/precise/man8/update-ca-certificates.8.html

## Additional Information

For additional guidance and important information about certificates, please refer to [this blog post](https://techcommunity.microsoft.com/t5/internet-of-things/azure-iot-tls-changes-are-coming-and-why-you-should-care/ba-p/1658456) from the security team.
