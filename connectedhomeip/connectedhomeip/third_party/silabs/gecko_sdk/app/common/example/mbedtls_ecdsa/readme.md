#  mbedTLS ECDSA Example



 This example uses hardware accelerators of the supported devices to perform ECDSA digital signature
 with mbedTLS. 



##  Getting Started



 Build the example and flash it to a supported device. Open a terminal emulator to see the output
 and input required options.


 When the application starts, three options are presented for the three supported curves
 for the ECDSA algorithm. Press 1, 2, or 3 to select
 which curve to use. If a valid option is chosen, the application will write to serial output when it has
 performed each step in the process. The example application is also programmed to measure the time spent
 in each step of the algorithm.


### Turning off the Hardware Acceleration


 To check the performance gain of the hardware acceleration, switch it off by
 switching the value of **Enable hardware acceleration of crypto operations** in the component *Mbed TLS
 common functionality*. This is found under the tab *SOFTWARE COMPONENTS* → *Platform
 → Security*. 


 **Useful tip:** Filter on *Configurable Components* and *Installed Components* to
 find the configuration file more easily. 



## Additional Information



 The example uses the CTR-DRBG, a pseudo random number generator (PRNG) included in mbedTLS to generate the
 key pair for the user. If the example is running on a device that includes a TRNG hardware module (True Random
 Number Generator), the TRNG will be used as entropy source to seed the CTR-DRBG. If the device does not incorporate a TRNG, the example will use RAIL as the entropy source. If neither of them is incorporated, dummy data will be seeded to the CTR-DRBG.
 The entropy accumulator of mbedTLS will use SHA256 to hash the the entropy data pool which is filled with data from the entropy
 sources.


 The CTR-DRBG uses AES, which is accelerated by the CRYPTO AES accelerator.


 The entropy accumulator of mbedTLS is set up to use SHA-256, which is accelerated by the
 CRYPTO SHA-256 accelerator.


 The example redirects standard I/O to the VCOM virtual serial port. Use the
 terminal emulator program to connect to the serial port with the default settings of 115200-8-N-1.


 The program asks the user to choose between three supported curves. The available
 curves with hardware acceleration support are:


* SECP256R1
* SECP192R1




