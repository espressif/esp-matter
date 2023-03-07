# Security of an end user IoT application
The Open IoT SDK examples are meant to be representative of typical IoT applications. They have been designed with security in mind, OTA firmware updates, securely applied using Trusted Firmware-M being a key feature.

However designing and building secure IoT devices is a vast topic, and we recommend that you design your IoT application with security in mind and follow industry best practices.

Here are some useful links:
- [Design a System on Chip: Create a System for a Secure IoT Device](https://developer.arm.com/documentation/101892/0100/The-goals-of-a-secure-IoT-device?lang=en)
- [ARM Security Technology Building a Secure System using TrustZone Technology](https://developer.arm.com/documentation/PRD29-GENC-009492/c?lang=en)
- [TrustZone for Cortex-M](https://developer.arm.com/Processors/TrustZone%20for%20Cortex-M)

Following are a few examples of security practices to consider.

## Disable the debug port

Security benefit: "Principle of least privilege" - Minimize exposed attack surfaces

[ETSI EN 303 645][1]
> **Minimize exposed attack surfaces**
>
> The "principle of least privilege" is a foundation stone of good security engineering, applicable to IoT as much as in any other field of application.
>
> **Provision 5.6-3** Device hardware should not unnecessarily expose physical interfaces to attack.
> Physical interfaces can be used by an attacker to compromise firmware or memory on a device. "Unnecessarily" refers to the manufacturer's assessment of the benefits of an open interface, used for user functionality or for debugging purposes.
>
> **Provision 5.6-4** Where a debug interface is physically accessible, it shall be disabled in software.

[Code of Practice for Consumer IoT Security][2]
> All devices and services should operate on the ‘principle of least privilege’; unused ports should be closed, hardware should not unnecessarily expose access, services should not be available if they are not used and code should be minimised to the functionality necessary for the service to operate. Software should run with appropriate privileges, taking account of both security and functionality.

## Configure the Memory Protection Unit (MPU) or Security Attribution Unit (SAU) appropriately for your application

These programmable units allow privileged software, typically an OS kernel, to define memory regions and assign memory access permission and memory attributes to each of them.

Security benefit: "Principle of least privilege" - Minimize exposed attack surfaces

[ETSI EN 303 645][1]
> **Provision 5.6-8** The device should include a hardware-level access control mechanism for memory.
>
> Software exploits often use the lack of access control in memory to execute malicious code. Access control mechanisms limit whether data in memory on the device can be executed. Suitable mechanisms include technologies such as MMUs or MPUs, executable space protection (e.g. NX bits), memory tagging, and trusted execution environments.

## Mitigate against physical attacks

Consider the following:
- Choose silicon which is resistant to side-channel attacks
- Remove hardware components only used in development
- Provide anti-tamper mechanisms (including tamper resistance, tamper evidence, tamper detection, and tamper response)

[The IoT Security Foundation][3]
> (...) the first step in protecting a device prior to going into production is to remove all physical, radio or optical ports that were there for development purposes. Of course at least one port is required to connect a production device to the local network or back into the cloud, but all other ports that are no longer required should be removed, including any circuit board tracks that connect the port into the circuitry.
>
> (...)
>
> Chips that manage critical functions are sometimes removed from circuit boards by attackers, so they can connect the chips to test equipment for analysis. To prevent this, key chips can be epoxied to the circuit board or other features such that the chip gets destroyed in the process of removing it. Also, the entire circuitry can be embedded in a block of resin, which will render it entirely inaccessible to all but the most determined.
>
> Of course the last line of defence is the device casing itself. This can provide physical protection to the innards, form part of the mounting assembly and help shield against detection of emanations. One option can be to render the device permanently disabled if the case is opened.
>
> There is always the risk that a device may get tampered with somewhere in the supply chain, perhaps in a warehouse or in transit. Anti-tamper packaging or seals can be applied to give a visual indication of any attempt on interfering with the product.

## Use a network connection back-off algorithm & flow control

Consider using a network connection back-off algorithm and flow control to mitigate against Distributed Denial of Service (DDoS) attacks which can be caused by mass-reconnections and drain the system's battery. Similarly, use flow control to prevent overflows on asynchronous buses.

## References

[1]: https://www.etsi.org/deliver/etsi_en/303600_303699/303645/02.01.01_60/en_303645v020101p.pdf (ETSI EN 303 645 Cyber Security for Consumer Internet of Things: Baseline Requirements)
[2]: https://www.gov.uk/government/publications/code-of-practice-for-consumer-iot-security/code-of-practice-for-consumer-iot-security (GOV.UK Code of Practice for Consumer IoT Security)
[3]: https://www.iotsecurityfoundation.org/best-practice-guide-articles/physical-security/ (Physical Security An IoTSF Secure Design Best Practice Guide Article, The IoT Security Foundation)
[4]: https://www.arm.com/company/news/2018/05/new-arm-ip-helps-protect-iot-devices-from-increasingly-prevalent-physical-threats (New Arm IP Helps Protect IoT Devices from Increasingly Prevalent Physical Threats, Paul Williamson, Vice President and General Manager, IoT Device IP Line of Business, Arm)

- [ETSI EN 303 645 Cyber Security for Consumer Internet of Things: Baseline Requirements](https://www.etsi.org/deliver/etsi_en/303600_303699/303645/02.01.01_60/en_303645v020101p.pdf)
- [GOV.UK Code of Practice for Consumer IoT Security](https://www.gov.uk/government/publications/code-of-practice-for-consumer-iot-security/code-of-practice-for-consumer-iot-security)
- [Physical Security An IoTSF Secure Design Best Practice Guide Article, The IoT Security Foundation](https://www.iotsecurityfoundation.org/best-practice-guide-articles/physical-security/)
- [New Arm IP Helps Protect IoT Devices from Increasingly Prevalent Physical Threats, Paul Williamson, Vice President and General Manager, IoT Device IP Line of Business, Arm](https://www.arm.com/company/news/2018/05/new-arm-ip-helps-protect-iot-devices-from-increasingly-prevalent-physical-threats)
