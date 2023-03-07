Generate code from CDDL description
===================================

CDDL is a human-readable description language defined in [IETF RFC 8610](https://datatracker.ietf.org/doc/rfc8610/).
By calling the Python script [cddl_gen.py](cddl_gen/cddl_gen.py), you can generate C code that validates/encodes/decodes CBOR data conforming to a CDDL schema.
The script can also validate and convert CBOR data to and from JSON/YAML.
If the script is imported as a module, you can use it to validate and decode CBOR data into a python structure with names (similar to the struct available in the generated code).

The generated code depends on low-level CBOR ([headers](include) and [source](src)).
There are tests for the code generation in [tests/](tests/).
For now the tests require [Zephyr](https://github.com/zephyrproject-rtos/zephyr) (if your shell is set up to build Zephyr samples, the tests should also build).

Features
========

The generated code consists of:
 - A header file containing typedefs for the types defined in the CDDL, as well as declarations for decoding functions for some types (those specified as entry types). The typedefs are the same for both encoding and decoding.
 - A C file containing all the encoding/decoding code.
   The code is split across multiple functions, and each function contains a single `if` statement which "and"s and "or"s together calls into the cbor libraries or to other generated decoding functions.

CDDL allows placing restrictions on the members of your data structure.
Restrictions can be on type, on content (e.g. values/sizes of ints or strings), and repetition (e.g. the number of members in a list).
The generated code will validate the input (i.e. the structure if encoding, or the payload for decoding), which means that it will check all the restriction set in the CDDL description, and fail if a restriction is broken.

The cbor libraries do most of the actual translation and moving of bytes, and the validation of values.

Build system
------------

There is some CMake code available which requires Zephyr to run.
When you call the [`target_cddl_source()`](cmake/extensions.cmake) CMake function, it sets up build steps necessary to call the script on the provided CDDL file, and adds the generated file as well as the cbor libraries to your project.
As long as the `target_cddl_source()` function is called in your project, you should be able to #include the generated file and use it in your code.

Introduction to CDDL
====================

In CDDL you define types from other types.
Types can be defined from base types, or from other types you define.
Types are declared with '`=`', e.g. `Foo = int` which declares the type `Foo` to be an integer, analogous to `typedef int Foo;` in C.
CDDL defines the following base types (this is not an exhaustive list):

 - `int`: Positive or negative integer
 - `uint`: Positive integer
 - `bstr`: Byte string
 - `tstr`: Text string
 - `bool`: Boolean
 - `nil`: Nil/Null value
 - `float`: Floating point value
 - `any`: Any single element

CDDL allows creating aggregate types:

 - `[]`: List. Elements don't need to have the same type.
 - `{}`: Map. Key/value pairs as are declared as `<key> => <value>` or `<key>: <value>`. Note that `:` is also used for labels.
 - `()`: Groups. Grouping with no enclosing type, which means that e.g. `Foo = [(int, bstr)]` is equivalent to `Foo = [int, bstr]`.
 - `/`: Unions. Analogous to unions in C. E.g. `Foo = int/bstr/Bar` where Foo is either an int, a bstr, or Bar (some custom type).

Literals can be used instead of the base type names:

 - Number: `Foo = 3`, where Foo is a uint with the additional requirement that it must have the value 3.
 - Number range: `Foo = -100..100`, where Foo is an int with value between -100 and 100.
 - Text string: `Foo = "hello"`, where Foo is a tstr with the requirement that it must be "hello".
 - True/False: `Foo = false`, where Foo is a bool which is always false.

Base types can also be restricted in other ways:

 - `.size`: Works for integers and strings. E.g. `Foo = uint .size 4` where Foo is a uint exactly 4 bytes long.
 - `.cbor`/`.cborseq`: E.g. `Foo = bstr .cbor Bar` where Foo is a bstr whose contents must be CBOR data decodeable as the Bar type.

An element can be repeated:

 - `?`: 0 or 1 time. E.g. `Foo = [int, ?bstr]`, where Foo is a list with an int possibly followed by a bstr.
 - `*`: 0 or more times. E.g. `Foo = [*tstr]`, where Foo is a list containing 0 or more tstrs.
 - `+`: 1 or more times. E.g. `Foo = [+Bar]`.
 - `x**y`: Between x and y times, inclusive. E.g. `Foo = {4**8(int => bstr)}` where Foo is a map with 4 to 8 key/value pairs where each key is an int and each value is a bstr.

Note that in the cddl_gen script and its generated code, the number of entries supported via `*` and `+` is affected by the default_max_qty value.

Any element can be labeled with `:`.
The label is only for readability and does not impact the data structure in any way.
E.g. `Foo = [name: tstr, age: uint]` is equivalent to `Foo = [tstr, uint]`.

See [test3_simple](tests/cbor_decode/test3_simple/) for CDDL example code.

Usage Example
=============

Code generation
---------------

This example is is taken from [test3_simple](tests/cbor_decode/test3_simple/).

If your CDDL file contains the following code:

```cddl
Timestamp = bstr .size 8

; Comments are denoted with a semicolon
Pet = [
    name: [ +tstr ],
    birthday: Timestamp,
    species: (cat: 1) / (dog: 2) / (other: 3),
]
```
Call the Python script:

```sh
python3 <cddl-gen base>/cddl_gen/cddl_gen.py -c pet.cddl code -d -t Pet --oc pet_decode.c --oh pet_decode.h
```

Or invoke its command line executable (if installed via `pip`):

```sh
cddl_gen -c pet.cddl code -d -t Pet --oc pet_decode.c --oh pet_decode.h
```

Or add the following line to your CMake code:

```cmake
target_cddl_source(app pet.cddl DECODE ENTRY_TYPES Pet)
```

And use the generated code with

```c
#include <pet_decode.h> /* The name of the header file is taken from the name of
                           the cddl file, but can also be specifiec when calling
                           the script. */

/* ... */

/* The following type and function refer to the Pet type in the CDDL, which
 * has been specified as an ENTRY_TYPE in the cmake call. */
Pet_t pet;
bool success = cbor_decode_Pet(input, sizeof(input), &pet, true);
```

The process is the same for encoding, except:
 - Change `-d` to `-e` when calling the script
 - Change `DECODE` to `ENCODE` in the Cmake call.
 - Input parameters become output parameters and vice versa in the code:

```c
#include <pet_encode.h> /* The name of the header file is taken from the name of
                           the cddl file, but can also be specifiec when calling
                           the script. */

/* ... */

/* The following type and function refer to the Pet type in the CDDL, which
 * has been specified as an ENTRY_TYPE in the cmake call. */
Pet_t pet = { /* Initialize with desired data. */ };
uint8_t output[100]; /* 100 is an example. Must be large enough for data to fit. */
uint32_t out_len;
bool success = cbor_encode_Pet(output, sizeof(output), &pet, &out_len);
```

Converting
----------

Here is an example call for converting from YAML to CBOR:

```sh
cddl_gen -c pet.cddl convert -t Pet -i mypet.yaml -o mypet.cbor
```

Which takes a yaml structure from mypet.yaml, validates it against the Pet type in the CDDL description in pet.cddl, and writes binary CBOR data to mypet.cbor.

See the tests in  <cddl-gen base>/tests/ for examples of using the python module

Running tests
=============

The tests for the generated code are based on Zephyr ztests.
Tests for the conversion functions in the script are implemented with the unittest module.

There are also test.sh scripts to quickly run all tests.
[`tests/test.sh`](tests/test.sh) runs all tests, including python tests in [`tests/scripts`](tests/scripts).
[`tests/cbor_decode/test.sh`](tests/cbor_decode/test.sh) runs all decoding tests.
[`tests/cbor_encode/test.sh`](tests/cbor_encode/test.sh) runs all encoding tests.

These tests are dependent upon the `pycodestyle` package from `pip`.
Run these scripts with no arguments.

To set up the environment to run the ztest tests, follow [Zephyr's Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html), or see the workflow in the [`.github`](.github) directory.
