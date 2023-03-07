# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Binaries used by this makefile

DPKG               ?= $(shell which dpkg 2> /dev/null)
MAKE               ?= make
PYCODE_LINT          ?= yapf
PYCODE_LINT_ARGS     ?= --style='{column_limit: 79}' --recursive
PYTHON             ?= python3
PYTHON_VERSION     ?= $(shell $(PYTHON) -c "import sys; sys.stdout.write(sys.version[:3])")
SUDO               ?= sudo
NO_GRPC            ?=

# The list of Debian packages on which Cirque depends which must be
# installed before Cirque may be used.

DPKG_PREREQUISITES := \
	socat \
	psmisc \
	tigervnc-standalone-server \
	python3-pip \
	python3-setuptools \
	python3-venv \
    $(NULL)

# check-dpkg-prequisite <package>
#
# Determines and verbosely reports whether the specified Debian
# package is installed or not, exiting with the appropriate status.

define check-dpkg-prerequisite
@echo -n "Checking for $(1)...";
@if `$(DPKG) -s $(1) > /dev/null 2>&1`; then \
    echo "ok"; \
else \
    echo "failed"; \
    echo "The package '$(1)' is required and is not installed. Please run 'sudo apt-get install $(1)' to install it."; \
    exit 1; \
fi
endef

check_TARGETS = $(addprefix check-dpkg-,$(DPKG_PREREQUISITES))

check-dpkg-%: $(DPKG)
	$(call check-dpkg-prerequisite,$(*))

all: install

check-prerequisites: $(check_TARGETS)

# If CIRQUE_PATH defined, install or uninstall the specific, CIRQUE_PATH,
# location of the Cirque package. If CIRQUE_PATH is not defined, by default
# install Cirque into Python's system with install-system.

install: check-prerequisites
ifeq ($(CIRQUE_PATH),)
	$(MAKE) NO_GRPC=$(NO_GRPC) install-system
else
	$(MAKE) NO_GRPC=$(NO_GRPC) install-path
endif

uninstall:
ifeq ($(CIRQUE_PATH),)
	$(MAKE) NO_GRPC=$(NO_GRPC) uninstall-system
else
	$(MAKE) NO_GRPC=$(NO_GRPC) uninstall-path
endif

# Install Cirque into Python's shared library in a developed version.
# Develop version instead of copying Cirque package into /usr/local/lib ...,
# creates a reference to the Cirque source directory. This allows a developer
# to modify Cirque modules and test them without reinstalling Cirque.

install-develop: check-prerequisites
	# Installing Cirque for development
	$(SUDO) NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py develop

uninstall-develop:
	$(SUDO) NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py develop --uninstall
	$(MAKE) NO_GRPC=$(NO_GRPC) clean

# Install Cirque into a user's home directory (~/.local). This allows a user
# to install Cirque without requiring root privilages. The installed Cirque
# package is only visible to the user that installed it; other same system's
# users cannot find Cirque package unless they install it as well.

install-user: check-prerequisites
	# Installing Cirque into user home directory
	NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py install --user
	@echo
	@echo "Cirque package installed into users ~/.local/lib/*"
	@echo

uninstall-user:
	rm -rf ~/.local/lib/python$(PYTHON_VERSION)/site-packages/cirque*.egg

# Install Cirque into Python system-wide distribution packages. This installation
# requires root privilages. After installation every user in the system can
# use Cirque package.

install-system: check-prerequisites
	# Installing Cirque
	$(SUDO) NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py install

uninstall-system:
	$(MAKE) clean
	$(SUDO) rm -rf /usr/local/lib/python$(PYTHON_VERSION)/dist-packages/cirque-*egg

# Install Cirque package into non-standard location. Because the installed package
# location is not know to Python, the package path must be passed to PYTHON through
# PYTHONPATH environment variable. To install Cirque under /some/path run:
# make CIRQUE_PATH=/some/path
# This will create /some/path/lib/pythonX.X/site-packages location and install
# the cirque package over there.

install-path: check-prerequisites
ifeq ($(CIRQUE_PATH),)
	@echo Variable CIRQUE_PATH not set. && false
endif
	mkdir -p $(CIRQUE_PATH)/lib/python$(PYTHON_VERSION)/site-packages/; \
	export PYTHONPATH="$(CIRQUE_PATH)/lib/python$(PYTHON_VERSION)/site-packages/" ;\
	NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py install --prefix=$(CIRQUE_PATH)
	@echo
	@echo "Using custom path for a Python package is unusual."
	@echo "Remember to update PYTHONPATH for every environment that will use this package, thus run"
	@echo "export PYTHONPATH=$$PYTHONPATH:"$(CIRQUE_PATH)/lib/python$(PYTHON_VERSION)/site-packages/""
	@echo

uninstall-path:
ifeq ($(CIRQUE_PATH),)
	@echo Variable CIRQUE_PATH not set. && false
endif
	rm -rf $(CIRQUE_PATH)

distribution-build: clean
	# creates a built distribution
	NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py bdist

distribution-source: clean
	# creates a source distribution
	NO_GRPC=$(NO_GRPC) $(PYTHON) setup.py sdist

distribution: distribution-build distribution-source

release: distribution

clean:
	$(SUDO) rm -rf *.egg*
	$(SUDO) rm -rf dist
	$(SUDO) rm -rf build

pretty-check:
	$(PYCODE_LINT) $(PYCODE_LINT_ARGS) .
