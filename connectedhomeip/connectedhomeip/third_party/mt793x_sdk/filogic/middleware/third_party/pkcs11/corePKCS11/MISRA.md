# MISRA Compliance

The PKCS #11 library files conform to the [MISRA C:2012](https://www.misra.org.uk/MISRAHome/MISRAC2012/tabid/196/Default.aspx)
guidelines, with some noted exceptions. Compliance is checked with Coverity static analysis.
Deviations from the MISRA standard are listed below:

### Ignored by [Coverity Configuration](https://github.com/aws/aws-iot-device-sdk-embedded-C/blob/main/tools/coverity/misra.config)
| Deviation | Category | Justification |
| :-: | :-: | :-: |
| Directive 4.5 | Advisory | Allow names that MISRA considers ambiguous (such as LogInfo and LogError) |
| Directive 4.8 | Advisory | Allow inclusion of unused types. Header files for a specific port, which are needed by all files, may define types that are not used by a specific file. |
| Directive 4.9 | Advisory | Allow inclusion of function like macros. The `assert` macro is used throughout the library for parameter validation, and logging is done using function like macros. |
| Rule 2.4 | Advisory | Allow unused tags. Some compilers warn if types are not tagged. |
| Rule 2.5 | Advisory | Allow unused macros. Library headers may define macros intended for the application's use, but are not used by a specific file. |
| Rule 3.1 | Required | Allow nested comments. C++ style `//` comments are used in example code within Doxygen documentation blocks. |
| Rule 8.13 | Advisory | A pointer should be a const qualified type whenever possible. The files *could* be const qualified but the header files `pkcs11.h`, `pkcs11t.h`, and `pkcs11f.h` are derived from the PKCS #11 specification, and cannot be modified.|

| Rule 11.5 | Advisory | Allow casts from `void *`. Fields such as publish payloads are passed as `void *` and must be cast to the correct data type before use. |
| Rule 21.1 | Required | Allow use of all macro names. For compatibility, some macros introduced in C99 are defined for use with C90 compilers. |
| Rule 21.2 | Required | Allow use of all macro and identifier names. For compatibility, some macros introduced in C99 are defined for use with C90 compilers. |

### Flagged by Coverity
| Deviation | Category | Justification |
| :-: | :-: | :-: |
| Rule 8.7 | Advisory | API functions are not used by the library outside of the files they are defined; however, they must be externally visible in order to be used by an application. |

### Suppressed with Coverity Comments
| Deviation | Category | Justification |
| :-: | :-: | :-: |
| Rule 10.5 | Advisory | The value of an expression should not be cast to an inappropriate essential type. The boolean type of the PKCS #11 standard is an unsigned char, which is an acceptable base type for a boolean type.


