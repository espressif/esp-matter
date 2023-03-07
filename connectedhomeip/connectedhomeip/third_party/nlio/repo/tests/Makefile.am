#
#    Copyright (c) 2020 Project nlio Authors
#    Copyright (c) 2015-2016 Nest Labs Inc. All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#
#    Description:
#      This file is the GNU automake template for the Nest Labs Memory-
#      mapped I/O library unit tests.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

#
# Local headers to build against and distribute but not to install
# since they are not part of the package.
#
noinst_HEADERS                                 = \
    nlbyteorder-test.h                           \
    nlio-byteorder-test.h                        \
    nlio-test.h                                  \
    $(NULL)

#
# Other files we do want to distribute with the NLIO SDK.
#
EXTRA_DIST                                     = \
    $(NULL)

if NLIO_BUILD_TESTS
# C preprocessor option flags that will apply to all compiled objects in this
# makefile.

AM_CPPFLAGS                                    = \
    -I$(top_srcdir)/include                      \
    $(NULL)

COMMON_LDADD                                   = \
    $(NULL)

# Test applications that should be run when the 'check' target is run.

check_PROGRAMS                                 = \
    nlbyteorder-test                             \
    nlbyteorder-test-cxx                         \
    nlio-byteorder-test                          \
    nlio-byteorder-test-cxx                      \
    nlio-test                                    \
    nlio-test-cxx                                \
    $(NULL)

# Test applications and scripts that should be built and run when the
# 'check' target is run.

TESTS                                          = \
    $(check_PROGRAMS)                            \
    $(NULL)

# The additional environment variables and their values that will be
# made available to all programs and scripts in TESTS.

TESTS_ENVIRONMENT                              = \
    $(NULL)

# Source, compiler, and linker options for test programs.

nlbyteorder_test_LDADD                         = $(COMMON_LDADD)
nlbyteorder_test_SOURCES                       = nlbyteorder-test.c          

nlbyteorder_test_cxx_LDADD                     = $(COMMON_LDADD)
nlbyteorder_test_cxx_SOURCES                   = nlbyteorder-test-cxx.cpp    

nlio_byteorder_test_LDADD                      = $(COMMON_LDADD)
nlio_byteorder_test_SOURCES                    = nlio-byteorder-test.c       

nlio_byteorder_test_cxx_LDADD                  = $(COMMON_LDADD)
nlio_byteorder_test_cxx_SOURCES                = nlio-byteorder-test-cxx.cpp 

nlio_test_LDADD                                = $(COMMON_LDADD)
nlio_test_SOURCES                              = nlio-test.c                 

nlio_test_cxx_LDADD                            = $(COMMON_LDADD)
nlio_test_cxx_SOURCES                          = nlio-test-cxx.cpp           

#
# Foreign make dependencies
#

NLFOREIGN_SUBDIR_DEPENDENCIES                  = \
   $(NLUNIT_TEST_FOREIGN_SUBDIR_DEPENDENCY)      \
   $(NULL)

if NLIO_BUILD_COVERAGE
CLEANFILES                                     = $(wildcard *.gcda *.gcno)

if NLIO_BUILD_COVERAGE_REPORTS
# The bundle should positively be qualified with the absolute build
# path. Otherwise, VPATH will get auto-prefixed to it if there is
# already such a directory in the non-colocated source tree.

NLIO_COVERAGE_BUNDLE                           = ${abs_builddir}/${PACKAGE}${NL_COVERAGE_BUNDLE_SUFFIX}
NLIO_COVERAGE_INFO                             = ${NLIO_COVERAGE_BUNDLE}/${PACKAGE}${NL_COVERAGE_INFO_SUFFIX}

$(NLIO_COVERAGE_BUNDLE):
	$(call create-directory)

$(NLIO_COVERAGE_INFO): check-local | $(NLIO_COVERAGE_BUNDLE)
	$(call generate-coverage-report,${top_builddir})

coverage-local: $(NLIO_COVERAGE_INFO)

clean-local: clean-local-coverage

.PHONY: clean-local-coverage
clean-local-coverage:
	-$(AM_V_at)rm -rf $(NLIO_COVERAGE_BUNDLE)
endif # NLIO_BUILD_COVERAGE_REPORTS
endif # NLIO_BUILD_COVERAGE
endif # NLIO_BUILD_TESTS

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
