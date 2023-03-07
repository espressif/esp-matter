cddl-gen Architecture
=====================

Since cddl-gen is a Python script that generates C code, this document is split into two sections:

1. Architecture of the Python script
2. Architecture of the generated code

Architecture of the Python script
=================================

The `cddl_gen.py` script is located in [cddl_gen/cddl_gen.py](cddl_gen/cddl_gen.py).

The functionality is spread across 5 classes:

1. CddlParser
2. CddlXcoder (inherits from CddlParser)
3. DataTranslator (inherits from CddlXcoder)
4. CodeGenerator (inherits from CddlXcoder)
5. CodeRenderer

CddlParser
----------

Each CddlParser object represents a CDDL type.
Since CDDL types can contain other types, CddlParser recursively parses a CDDL string, and spawns new instances of itself to represent the (child) types it contains.
The two most important member variables in CddlParser are `self.value` and `self.type`.
`self.type` is a string representing the base CDDL type, the options are (corresponding CBOR types are in the form of #majortype.val):

 - `"UINT"` (#0)
 - `"INT"` (#0 or #1)
 - `"NINT"` (#1)
 - `"BSTR"` (#2)
 - `"TSTR"` (#3)
 - `"FLOAT"` (#7.25, #7.26 or #7.27)
 - `"BOOL"` (#7.20 or #7.21)
 - `"NIL"` (#7.22)
 - `"LIST"` (#4)
 - `"MAP"` (#5)
 - `"GROUP"` (N/A)
 - `"UNION"` (N/A)
 - `"ANY"` (#0 - #5 or #7)
 - `"OTHER"` (N/A)

`"OTHER"` means another type defined in the same document, and is used as a pointer to that type definition.
The CDDL code that can give rise to these are described in the [README](README.md).

`self.value` means different things for different values of `self.type`.
E.g. for `"UINT"`, `self.value` has the value dictated by the type, or `None` if different values are allowed, so in the following example, `Foo` will have a `self.value` of 5, and `Bar` will have `None`.

```cddl
Foo = 5
Bar = uint
```

For container types, i.e `"LIST"`, `"MAP"`, `"GROUP"`, and `"UNION"`, `self.value` contains a list of their contents.
The code usually refers to the elements/contents in `self.value` as "children".
For `"OTHER"`, `self.value` is a string with the name of the type it refers to.
The following example shows use of both container and `"OTHER"` types.

```cddl
Foo = uint
Bar = [*Foo]
```

This will spawn 3 CddlParser objects:

1. `Foo`, which has `self.type = "UINT"` and `self.value = None`
2. An anonymous child of Bar, which has `self.type = "OTHER"`, and `self.value = "Foo"`
3. Bar, which has `self.type = "LIST"`, and `self.value` is a python `list` containing the above object.

CDDL supports many other constraints on the types, and these all have member variables in CddlParser, e.g. `self.min_qty` and `self.max_qty` which are the minimum and maximum quantity/repetitions of this type.

Children of `"MAP"` objects come in key/value pairs.
These are represented such that the values will be children of the `"MAP"` object, and the keys can be found as `self.key` in these children.

There is a member called `self.my_types`, which is a dict of all the types defined in the CDDL file.
The elements are on the form `<name>: <CddlParser object>`.
`"OTHER"` objects will look into `self.my_types[self.value]` to find its own definition.

The actual parsing of the CDDL happens with regular expressions.
There are one or more expressions for each base type.
The expressions consume a number of characters from the input string, and also capture a substring to use as the value of the type.
For container types, the regex will match the whole type, and then recursively parse the children within the matched string.

CddlXcoder
----------

CddlXcoder inherits from CddlParser, and provides common functionality for DataTranslator and CodeGenerator.

Most of the functionality falls into one of two categories:

- Common names for members in generated code. A single type possibly needs multiple member variables in the generated code to describe it, like
   - the value
   - the key associated with this value
   - the number of times it repeats
- Condition functions that make inferences about the type based on the member variables in CddlParser, like:
   - key_var_condition(): Whether it needs a key member
   - is_unambiguous(): Whether the type is completely specified, i.e. whether we know beforehand exactly how the encoding will look (e.g. `Foo = 5`).

DataTranslator
-----------

DataTranslator is for handling and manipulating CBOR on the "host".
For example, the user can compose data in YAML or JSON files and have them converted to CBOR and validated against the CDDL.
Or they can decode binary CBOR and write python code to inspect it, or just convert it back into YAML or JSON.

DataTranslator inherits from CddlXcoder and allows converting data between a number of different representations like CBOR/YAML/JSON strings, but also internal Python representations.
While the conversion is happening, the data is validated against the CDDL description.

This class relies heavily on decoding libraries for CBOR/YAML/JSON:

- [cbor2](https://pypi.org/project/cbor2/)
- [PyYAML](https://pypi.org/project/PyYAML/)
- [json](https://docs.python.org/3/library/json.html)

All three use the same internal representation of the decoded data, so it's trivial to convert between them.
The representation for all three is 1-to-1 with the corresponding Python types, (list -> list, map -> dict, uint -> int, bstr -> bytes etc.).
The only proprietary Python class used is `CBORTag` for CBOR tags.

One caveat is that CBOR supports more features than YAML/JSON, namely:

- non-text map keys
- bytes
- tags

In YAML/JSON, these are converted to maps in the following way:

- `{<key>: <value>}` => `{keyval<i>: {"key": <key>, "val": <value>}}` (i is an integer unique within this map)
- `<bytestring>` => `{"bstr": "<hex representation of bytestring>"}` or
- `<bytestring>` => `{"bstr": <CBOR decoding of bytestring>}` if `<bytestring>` is encodable as CBOR.
- `<tag, value>` => `{"tag": <tag>, "val": <value>}` where `<tag>` is the actual tag (a number), and `<value>` is the tagged value (the following CBOR object).

Finally, DataTranslator can also generate a separate internal representation using `namedtuple`s to allow browsing CBOR data by the names given in the CDDL.
(This is more analogous to how the data is accessed in the C code.)

DataTranslator functionality is tested in [tests/scripts](tests/scripts)

CodeGenerator
-------------

CodeGenerator, like DataTranslator, inherits from CddlXcoder.
Its primary purpose is to construct the individual decoding/encoding functions for the types specified in the given CDDL document.
It also constructs struct definitions used to hold the decoded data/data to be encoded.

CodeGenerator contains optimizations to reduce both the verbosity of the code and the level of indirection in the types.
For example:
 - If the type is unambiguous (i.e. its value is predetermined, like in `Foo = 5`), the code will validate it, but CodeGenerator won't include the actual value in the encompassing struct definition.
 - If a `"GROUP"` or `"UNION"` has only one child, it can be removed as a level of indirection.
 - If the type needs only a single member variable (i.e. no additional `foo_count` or `foo_key` etc.), that variable can instead be added to the parent struct, and its decoding/encoding code moved into the parent's function.
 - `"UNION"` are typically implemented as anonymous `union`s which removes one level of indirection when accessing them.

A CodeGenerator object operates in one of two modes: `"encode"` or `"decode"`.
The generated code for the two is not very different, but they call into different libraries.

Base types, like `"UINT"`, `"BOOL"`, `"FLOAT"` are represented by native C types. `"BSTR"`s and `"TSTR"`s are represented by a proprietary `cbor_string_type_t` which is just a `uint8_t` pointer and length.
These types are decoded/encoded with C code that is not generated.
More on this in the Architecture of the generated C code below.

When a type is repeated (max_qty > 1 or max_qty > min_qty), there needs to be a distinction between repeated_foo() and foo() (these can be either encoding or decoding functions).
repeated_foo() concerns itself with the individual value, while foo() concerns itself with the value including repetitions.

When invoking CodeGenerator, the user must decide which types it will need direct access to decode/encode.
These types are called "entry types" and they are typically the "outermost" types, or the types it is expected that the data will have.

The user can also use entry types when there are `"BSTR"`s that are CBOR encoded, specified as `Foo = bstr .cbor Bar`.
Usually such strings are automatically decoded/encoded by the generated code, and the objects part of the encompassing struct.
However, if the user instead wants to manually decode/encode such strings, they can add them to `self.entry_types`.
In this case, the strings will be stored as a regular `cbor_string_type_t` instead of being decoded/encoded.

CodeRenderer
------------

CodeRenderer is a standalone class that takes the result of the CodeGenerator class and constructs files.
There are 3 files constructed:

- The C file with the decoding/encoding functions.
- The H file with the public API to some functions in the C file.
- The H file with all the struct definitions (the type file). If both decoding and encoding files are generated for the same CDDL, they can share the same type file.

CodeRenderer conducts some pruning and deduplication of the list of types and functions received from CodeGenerator.


Architecture of the generated C code
====================================

In the generated C file, each type from the CDDL file gets its own decoding/encoding function, unless they are trivial types, like `Foo = uint`.
These functions are all `static`.
In addition, all entry types get public wrapper functions.

All decoding/encoding functions operate on a state variable of type `cbor_state_t` which keeps track of:

- The current position in the payload, and the end of the payload.
- The current position in a list or map, and the maximum expected number of elements.
- A list of backup states, used for saving states so they can be restored if decoding/encoding fails while processing an optional element.

Each function returns a `bool` indicating whether it was successful at decoding/encoding.
In most cases, a failure in one function will result in a failure of the whole operation.

However, in the following scenarios, a failure is fine because we don't know ahead of time whether the object will be found or not:

- An object with unknown number of repetitions (`min_qty` and `max_qty` are not the same).
- `"UNION"`s, since only one of the children should be present.

In these cases, the code attempts to decode the object. If it fails, it restores the state to before the attempt, and then tries decoding the next candidate type.

All generated functions take the form of a single if statement.
This if statement performs boolean algebra on statements depending on the children (typically only container types get a generated function).
The assignment of values in the structs mostly happens in the non-generated code.
The generated code mostly combines and validates calls into the non-generated code or other generated functions.

All functions (generated and not) have the same API structure: `bool <name>(cbor_state_t *state, <type> *result)`.
The number of arguments is kept to a minimum to reduce code size.

The exceptions to the API structure are `multi_decode`/`multi_encode` and `present_decode`/`present_encode`.
These functions accept function pointers with the above API and run them multiple times.
When this happens, the function pointers are cast to a generic function pointer type, and processed without knowledge of the type.

The non-generated files provide decoding/encoding functions for all the basic types except `"OTHER"`.
There are also housekeeping functions for managing state and logging.
This code is documented in the header files in the [include](include) folder.

The C tests for the code generation can be found in the [tests/cbor_decode](tests/cbor_decode) and [tests/cbor_encode](tests/cbor_encode) folders.
