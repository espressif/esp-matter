.. _module-pw_fuzzer:

---------
pw_fuzzer
---------
``pw_fuzzer`` provides developers with tools to write `libFuzzer`_ based
fuzzers.

Fuzzing or fuzz testing is style of testing that stochastically generates inputs
to targeted interfaces in order to automatically find defects and/or
vulnerabilities. In other words, fuzzing is simply an automated way of testing
APIs with generated data.

A fuzzer is a program that is used to fuzz a interface. It typically has three
steps that it executes repeatedly:

#. Generate a new, context-free input. This is the *fuzzing engine*. For
   ``pw_fuzzer``, this is `libFuzzer`_.
#. Use the input to exercise the targeted interface, or code being tested. This
   is the *fuzz target function*. For ``pw_fuzzer``, these are the GN
   ``sources`` and/or ``deps`` that define `LLVMFuzzerTestOneInput`_.
#. Monitor the code being tested for any abnormal conditions. This is the
   *instrumentation*. For ``pw_fuzzer``, these are sanitizer runtimes from
   LLVM's `compiler_rt`_.

.. note::

  ``pw_fuzzer`` is currently only supported on Linux and MacOS using clang.

.. image:: doc_resources/pw_fuzzer_coverage_guided.png
   :alt: Coverage Guided Fuzzing with libFuzzer
   :align: left

Writing fuzzers
===============

To write a fuzzer, a developer needs to write a fuzz target function follwing
the `fuzz target function`__ guidelines given by libFuzzer:

.. code:: cpp

  extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    DoSomethingInterestingWithMyAPI(data, size);
    return 0;  // Non-zero return values are reserved for future use.
  }

.. __: LLVMFuzzerTestOneInput_

When writing you fuzz target function, you may want to consider:

- It is acceptable to return early if the input doesn't meet some constraints,
  e.g. it is too short.
- If your fuzzer accepts data with a well-defined format, you can bootstrap
  coverage by crafting examples and adding them to a `corpus`_.
- There are tools to `split a fuzzing input`_ into multiple fields if needed;
  the `FuzzedDataProvider`_ is particularly easy to use.
- If your code acts on "transformed" inputs, such as encoded or compressed
  inputs, you may want to try `structure aware fuzzing`.
- You can do `startup initialization`_ if you need to.
- If your code is non-deterministic or uses checksums, you may want to disable
  those **only** when fuzzing by using LLVM's
  `FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION`_

.. _build:

Building fuzzers with GN
========================

To build a fuzzer, do the following:

1. Add the GN target using ``pw_fuzzer`` GN template, and add it to your the
   test group of the module:

.. code::

  # In $dir_my_module/BUILD.gn
  import("$dir_pw_fuzzer/fuzzer.gni")

  pw_fuzzer("my_fuzzer") {
    sources = [ "my_fuzzer.cc" ]
    deps = [ ":my_lib" ]
  }

  pw_test_group("tests") {
    tests = [
      ":existing_tests", ...
      ":my_fuzzer",     # <- Added!
    ]
  }

2. Select your choice of sanitizers ("address" is also the current default).
   See LLVM for `valid options`_.

.. code:: sh

  $ gn gen out --args='pw_toolchain_SANITIZERS=["address"]'

3. Build normally, e.g. using ``pw watch``.

.. _run:

Building and running fuzzers with Bazel
=======================================
To build a fuzzer, do the following:

1. Add the Bazel target using ``pw_cc_fuzz_test`` macro.

.. code:: py

  load("@pigweed//pw_fuzzer:fuzzer.bzl", "pw_cc_fuzz_test")

  pw_cc_fuzz_test(
    name = "my_fuzz_test",
    srcs = ["my_fuzzer.cc"],
    deps = [
      "@pigweed//pw_fuzzer",
      ":my_lib",
    ],
  )

2. Build and run the fuzzer.

.. code:: sh

  bazel test //my_module:my_fuzz_test

3. Swap fuzzer backend to use ASAN fuzzing engine.

.. code::

  # .bazelrc
  # Define the --config=asan-libfuzzer configuration.
  build:asan-libfuzzer \
    --@rules_fuzzing//fuzzing:cc_engine=@rules_fuzzing//fuzzing/engines:libfuzzer
  build:asan-libfuzzer \
    --@rules_fuzzing//fuzzing:cc_engine_instrumentation=libfuzzer
  build:asan-libfuzzer --@rules_fuzzing//fuzzing:cc_engine_sanitizer=asan

4. Re-run fuzz tests.

.. code::

  bazel test //my_module:my_fuzz_test --config asan-libfuzzer

Running fuzzers locally
=======================

Based on the example above, the fuzzer output will be at
``out/host/obj/my_module/my_fuzzer``. It can be invoked using the normal
`libFuzzer options`_ and `sanitizer runtime flags`_. For even more details, see
the libFuzzer section on `running a fuzzer`_.

For example, the following invocation disables "one definition rule" detection,
saves failing inputs to ``artifacts/``, treats any input that takes longer than
10 seconds as a failure, and stores the working corpus in ``corpus/``.

.. code::

  $ mkdir -p corpus
  $ ASAN_OPTIONS=detect_odr_violation=0 \
      out/host_clang_fuzz/obj/pw_fuzzer/bin/toy_fuzzer \
      -artifact_prefix=artifacts/ \
      -timeout=10 \
      corpus
  INFO: Seed: 305325345
  INFO: Loaded 1 modules   (46 inline 8-bit counters): 46 [0x38dfc0, 0x38dfee),
  INFO: Loaded 1 PC tables (46 PCs): 46 [0x23aaf0,0x23add0),
  INFO:        0 files found in corpus
  INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 4096 bytes
  INFO: A corpus is not provided, starting from an empty corpus
  #2      INITED cov: 2 ft: 3 corp: 1/1b exec/s: 0 rss: 27Mb
  #4      NEW    cov: 3 ft: 4 corp: 2/3b lim: 4 exec/s: 0 rss: 27Mb L: 2/2 MS: 2 ShuffleBytes-InsertByte-
  #11     NEW    cov: 7 ft: 8 corp: 3/7b lim: 4 exec/s: 0 rss: 27Mb L: 4/4 MS: 2 EraseBytes-CrossOver-
  #27     REDUCE cov: 7 ft: 8 corp: 3/6b lim: 4 exec/s: 0 rss: 27Mb L: 3/3 MS: 1 EraseBytes-
  #29     REDUCE cov: 7 ft: 8 corp: 3/5b lim: 4 exec/s: 0 rss: 27Mb L: 2/2 MS: 2 ChangeBit-EraseBytes-
  #445    REDUCE cov: 9 ft: 10 corp: 4/13b lim: 8 exec/s: 0 rss: 27Mb L: 8/8 MS: 1 InsertRepeatedBytes-
  #12104  NEW    cov: 11 ft: 12 corp: 5/24b lim: 122 exec/s: 0 rss: 28Mb L: 11/11 MS: 4 CMP-InsertByte-ShuffleBytes-ChangeByte- DE: "\xff\xff"-
  #12321  NEW    cov: 12 ft: 13 corp: 6/31b lim: 122 exec/s: 0 rss: 28Mb L: 7/11 MS: 2 CopyPart-EraseBytes-
  #12459  REDUCE cov: 12 ft: 13 corp: 6/28b lim: 122 exec/s: 0 rss: 28Mb L: 8/8 MS: 3 CMP-InsertByte-EraseBytes- DE: "\x00\x00"-
  #12826  REDUCE cov: 12 ft: 13 corp: 6/26b lim: 122 exec/s: 0 rss: 28Mb L: 5/8 MS: 2 ShuffleBytes-EraseBytes-
  #14824  REDUCE cov: 12 ft: 13 corp: 6/25b lim: 135 exec/s: 0 rss: 28Mb L: 4/8 MS: 3 PersAutoDict-ShuffleBytes-EraseBytes- DE: "\x00\x00"-
  #15106  REDUCE cov: 12 ft: 13 corp: 6/24b lim: 135 exec/s: 0 rss: 28Mb L: 3/8 MS: 2 ChangeByte-EraseBytes-
  ...
  #197809 REDUCE cov: 35 ft: 36 corp: 22/129b lim: 1800 exec/s: 0 rss: 79Mb L: 9/9 MS: 1 InsertByte-
  #216250 REDUCE cov: 35 ft: 36 corp: 22/128b lim: 1980 exec/s: 0 rss: 87Mb L: 8/8 MS: 1 EraseBytes-
  #242761 REDUCE cov: 35 ft: 36 corp: 22/127b lim: 2237 exec/s: 0 rss: 101Mb L: 7/8 MS: 1 EraseBytes-
  ==126148== ERROR: libFuzzer: deadly signal
      #0 0x35b981 in __sanitizer_print_stack_trace ../recipe_cleanup/clangFu99hg/llvm_build_dir/tools/clang/stage2-bins/runtimes/runtimes-x86_64-unknown-linux-gnu-bins/compiler-rt/lib/asan/asan_stack.cpp:86:3
      #1 0x2bcdb5 in fuzzer::PrintStackTrace() (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2bcdb5)
      #2 0x2a2ac9 in fuzzer::Fuzzer::CrashCallback() (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2a2ac9)
      #3 0x7f866684151f  (/lib/x86_64-linux-gnu/libpthread.so.0+0x1351f)
      #4 0x3831df in (anonymous namespace)::toy_example(char const*, char const*) /home/aarongreen/src/pigweed/out/host/../../pw_fuzzer/examples/toy_fuzzer.cc:49:15
      #5 0x3831df in LLVMFuzzerTestOneInput /home/aarongreen/src/pigweed/out/host/../../pw_fuzzer/examples/toy_fuzzer.cc:80:3
      #6 0x2a4025 in fuzzer::Fuzzer::ExecuteCallback(unsigned char const*, unsigned long) (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2a4025)
      #7 0x2a3774 in fuzzer::Fuzzer::RunOne(unsigned char const*, unsigned long, bool, fuzzer::InputInfo*, bool*) (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2a3774)
      #8 0x2a5769 in fuzzer::Fuzzer::MutateAndTestOne() (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2a5769)
      #9 0x2a6185 in fuzzer::Fuzzer::Loop(std::__Fuzzer::vector<fuzzer::SizedFile, fuzzer::fuzzer_allocator<fuzzer::SizedFile> >&) (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x2a6185)
      #10 0x294c8a in fuzzer::FuzzerDriver(int*, char***, int (*)(unsigned char const*, unsigned long)) (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x294c8a)
      #11 0x2bd422 in main ../recipe_cleanup/clangFu99hg/llvm_build_dir/tools/clang/stage2-bins/runtimes/runtimes-x86_64-unknown-linux-gnu-bins/compiler-rt/lib/fuzzer/FuzzerMain.cpp:19:10
      #12 0x7f8666684bba in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x26bba)
      #13 0x26ae19 in _start (/home/aarongreen/src/pigweed/out/host/obj/pw_fuzzer/toy_fuzzer+0x26ae19)

  NOTE: libFuzzer has rudimentary signal handlers.
        Combine libFuzzer with AddressSanitizer or similar for better crash reports.
  SUMMARY: libFuzzer: deadly signal
  MS: 1 CrossOver-; base unit: 9f479f7a6e3a21363397a25da3168218ba182a16
  0x68,0x65,0x6c,0x6c,0x6f,0x0,0x77,0x6f,0x72,0x6c,0x64,0x0,0x0,0x0,
  hello\x00world\x00\x00\x00
  artifact_prefix='artifacts'; Test unit written to artifacts/crash-6e4fdc7ffd04131ea15dd243a0890b1b606f4831
  Base64: aGVsbG8Ad29ybGQAAAA=

Running fuzzers on OSS-Fuzz
===========================

Pigweed is integrated with `OSS-Fuzz`_, a continuous fuzzing infrastructure for
open source software. Fuzzers listed in in ``pw_test_groups`` will automatically
start being run within a day or so of appearing in the git repository.

Bugs produced by OSS-Fuzz can be found in its `Monorail instance`_. These bugs
include:

* A detailed report, including a symbolized backtrace.
* A revision range indicating when the bug has been detected.
* A minimized testcase, which is a fuzzer input that can be used to reproduce
  the bug.

To reproduce a bug:

#. Build_ the fuzzers as described above.
#. Download the minimized testcase.
#. Run_ the fuzzer with the testcase as an argument.

For example, if the testcase is saved as "~/Downloads/testcase"
and the fuzzer is the same as in the examples above, you could run:

.. code::

  $ ./out/host/obj/pw_fuzzer/toy_fuzzer ~/Downloads/testcase

If you need to recreate the OSS-Fuzz environment locally, you can use its
documentation on `reproducing`_ issues.

In particular, you can recreate the OSS-Fuzz environment using:

.. code::

  $ python infra/helper.py pull_images
  $ python infra/helper.py build_image pigweed
  $ python infra/helper.py build_fuzzers --sanitizer <address/undefined> pigweed

With that environment, you can run the reproduce bugs using:

.. code::

  python infra/helper.py reproduce pigweed <pw_module>_<fuzzer_name> ~/Downloads/testcase

You can even verify fixes in your local source checkout:

.. code::

  $ python infra/helper.py build_fuzzers --sanitizer <address/undefined> pigweed $PW_ROOT
  $ python infra/helper.py reproduce pigweed <pw_module>_<fuzzer_name> ~/Downloads/testcase

.. _compiler_rt: https://compiler-rt.llvm.org/
.. _corpus: https://llvm.org/docs/LibFuzzer.html#corpus
.. _FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION: https://llvm.org/docs/LibFuzzer.html#fuzzer-friendly-build-mode
.. _FuzzedDataProvider: https://github.com/llvm/llvm-project/blob/HEAD/compiler-rt/include/fuzzer/FuzzedDataProvider.h
.. _libFuzzer: https://llvm.org/docs/LibFuzzer.html
.. _libFuzzer options: https://llvm.org/docs/LibFuzzer.html#options
.. _LLVMFuzzerTestOneInput: https://llvm.org/docs/LibFuzzer.html#fuzz-target
.. _monorail instance: https://bugs.chromium.org/p/oss-fuzz
.. _oss-fuzz: https://github.com/google/oss-fuzz
.. _reproducing: https://google.github.io/oss-fuzz/advanced-topics/reproducing/
.. _running a fuzzer: https://llvm.org/docs/LibFuzzer.html#running
.. _sanitizer runtime flags: https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
.. _split a fuzzing input: https://github.com/google/fuzzing/blob/HEAD/docs/split-inputs.md
.. _startup initialization: https://llvm.org/docs/LibFuzzer.html#startup-initialization
.. _structure aware fuzzing: https://github.com/google/fuzzing/blob/HEAD/docs/structure-aware-fuzzing.md
.. _valid options: https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
