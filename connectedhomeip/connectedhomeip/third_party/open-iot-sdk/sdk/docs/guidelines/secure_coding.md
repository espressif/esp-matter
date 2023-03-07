# Secure Coding

The secure coding guidelines are an extension to the team's coding guide, [CODING.md](https://gitlab.arm.com/iot/open-iot-sdk/sdk/-/tree/main/docs/guidelines/CODING.md), with the following additional rules:

## Basic Guidelines

* Initialise all variables.
* Validate the input parameters of functions that can be called by another layer/component than the current one.
* Where the size of an output parameter is supplied by an untrusted caller, check that the size is not greater than the available data before copying it to the output parameter. Alternatively, refactor the API to not permit the caller to specify the output size
* Ensure that functions are not called with parameters that will cause buffer overflow inside the function.
* Check the return values of functions.
* Prefer string functions with a strict length limit to avoid memory overruns due to missing closing 0 values (i.e. snprintf vs sprintf).
* Avoid undefined, unspecified and implementation-defined behaviours of the C99 standard.
* Limit the scope of variables to as minimum as possible.
* Limit life-cycle of variable to as minimum as possible.
* Assume code memory is non writable and data memory as non executable.
* Be aware of integer overflows.
* Be aware of side effects of integer promotion. Use type casts to enforce well defined behavior.
* Never rely on operator precedence, always use parentheses to enforce order of evaluation.
* When injecting code fragments using macro definitions, the pre-processed output must be carefully investigated for side effects.
* Use `const` keyword by default unless mutation is required.
* Ensure debugging code is not in release builds.
* Treat all warnings as errors

Notes:

Where possible, all of these rules will be checked automatically by a static analyser.

Deviations of these rules need to be clearly documented and justified.

### Banned APIs/Functions

This is the list of banned APIs for the project:

* "strcpy"
* "wcscpy"
* "strncpy"
* "strcat"
* "wcscat"
* "strncat"
* "sprintf"
* "vsprintf"
* "strtok"
* "atoi"
* "atol"
* "atoll"
* "itoa"
* "ltoa"
* "lltoa"

Use of banned APIs will be checked for in the CI.

## Coding processes

### Code Review

* All contributions must consider the Security Code Review Checklist (detailed below)

# Secure Design Guidelines

* Encrypt data at rest
* Ensure that cryptographic modules are not subject to side channel attacks
* Be aware of side-channel attacks
* Take care of Time of Check to Time of Use (TOCTOU) vulnerabilities
* Depending on their configuration, do not trust peripherals
* Apply the principle of least privilege


## IOT-OS-M Security Code Review Checklist

This checklist must be considered during the review of any contributions.

## Data at rest

Does the patch affect reading or writing data from/to temporary or persistent file systems?

- [ ] No

- [ ] Yes, look for code changes that may affect any of the following ...

Consider:


### Confidentiality

- [ ] Encryption of stored data

- [ ] Protection for cryptographic certificates and keys

- [ ] Mitigations for directory traversal vulnerabilities

### Integrity

- [ ] Cryptographic hash or signature of the stored data

- [ ] Management of cryptographic certificates and keys

- [ ] Random number generation and seeding

- [ ] Mitigations for TOCTOU vulnerabilities

- [ ] Mitigations for file I/O race conditions

- [ ] Defensive mount options for storage

- [ ] Defensive configuration of USB and other file I/O stacks

### Availability

- [ ] Access controls to storage

- [ ] Isolation or sandboxing of processes that access removable storage


## Data in motion

Does the patch add, remove or alter transmission or reception of data over a network or IPC mechanism?

- [ ] No

- [ ] Yes

Does the patch add, remove or alter a client or server process?

- [ ] No

- [ ] Yes

If you selected 'Yes' for either, look for code changes that may affect any of the following ...

Consider

### Confidentiality

- [ ] Encryption of transmitted data

- [ ] Protection for cryptographic certificates and keys

- [ ] Protection for network and wireless access credentials

- [ ] Prevention of information leakage through client/server error or status messages

- [ ] Prevention of exposed MAC addresses and other personal identifiers in network protocols

- [ ] Prevention of information leakage through padding in data

### Integrity

- [ ] Choice of network protocols and wireless pairing mechanisms

- [ ] Authentication of clients and servers

- [ ] Cryptographic hash or signature of transmitted data

- [ ] Management of cryptographic certificates and keys

- [ ] Random number generation and seeding

- [ ] Mitigations for MITM and replay attacks

- [ ] Defensive configuration of network and other connectivity stacks

- [ ] Firewall rules

### Availability

- [ ] Access controls to IPC mechanism

- [ ] Isolation or sandboxing of client/server processes

## Data in use

Does the patch add, remove or alter processing of data in memory?

- [ ] No

- [ ] Yes, look for code changes that may affect any of the following ...

Consider

### Confidentiality

- [ ] Clearing sensitive data from memory after use

- [ ] Protection of sensitive data passed to/collected from library functions

- [ ] Prevention of information leakage through logging or debug output

- [ ] Prevention of information leakage through string literals in object code

- [ ] Prevention of information leakage through symbol tables in object code

### Integrity

- [ ] Adherence to secure coding guidelines

- [ ] Protection against arithmetic over/underflow

- [ ] Input/argument validation using 'whitelisting'

- [ ] Verification of new/altered parsing/deserialisation code

- [ ] Coding mitigations for side channel attacks

- [ ] Error recovery that creates denial-of-service vulnerabilities (e.g. assertions)

### Availability

- [ ] Process ownership, permissions or capabilities

- [ ] Process isolation or sandboxing

- [ ] Software or hardware watchdog
