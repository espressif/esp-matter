# Coding Style

The Open IoT SDK uses clang-format, based on the LLVM style option, plus a number of additional rules.

## Clang-format style definition
The clang-format config is found in the [developer-tools](https://gitlab.arm.com/iot/open-iot-sdk/tools/developer-tools) project.

## Rules

* Indentation - four spaces. No tabs are allowed.
* Braces - K&R style.
* One true brace style (1TBS) - use braces for statements of type if, else, while and for (exception from K&R).
* One line per statement.
* Preprocessor macro starts at the beginning of a new line; the code inside is indented according to the code above it.
* Cases within switch are indented (exception from K&R).
* Space after statements of type if, while, for, switch. The same applies to binary operators (like, + and *) and the ternary operator (? and :).
* Each line preferably has at most 120 characters.
* Comments should use proper spelling and grammar.
* For pointers or references, the symbols * or & are adjacent to a name (object_t *this_pointer; object_t &obj). If you omit the name, place the space between the type and * (such as int * or int &).
* For function return pointers or references, the symbols * or & are adjacent to a function name (int *func() or int &func()).
* Don't leave trailing spaces at the end of lines.
* Empty lines should have no trailing spaces.
* Unix line endings are default option for files.
* Use capital letters for macros.
* A file should have an empty line at the end.

## C coding style

### Naming conventions

#### Structures

* Begin with a capital letter, and each word within a structure also begins with a capital letter (AnalogIn, BusInOut).
* Structures - suffix _t - to denote it is a user-defined type.
* Enumeration - the type name and values name - same naming convention as classes (for example MyNewEnum).

#### Functions

* Contain lower case letters.
* Words separated by underscore (wait_ms, read_u16).

## C++ coding style
Classes have the same formatting as structures (methods follow functions naming in C coding style).

### Namespaces
Namespaces used to group subsystems are lower case, such as rtos and event. If not in a specific subsystem, C++ APIs are in namespace iotsdk.

Occasionally, namespaces are used to act as-if "static singleton" objects. One example is namespace ThisThread in rtos (which was modeled after C++11 std::this_thread). These namespaces follow the class naming convention, so calls to their functions look like calls to static class methods.

Below is an example of typical namespace use in a source file:

```
using namespace rtos; // for ThisThread
using namespace std::chrono_literals; // for 1s

ThisThread::sleep_for(1s);
```

## Doxygen documentation
All functions and methods should contain documentation using Doxygen.

The doxygen style:

```
/** Blocking reading data
 *
 *  @param obj     The I2C object
 *  @param address 7-bit address (last bit is 1)
 *  @param data    The buffer for receiving
 *  @param length  Number of bytes to read
 *  @param stop    Stop to be generated after the transfer is done
 *  @return Number of read bytes
 */
int i2c_read(i2c_t *obj, int address, char *data, int length, int stop);

```

## Tooling

Code style will be checked by the CI when a MR is raised.
