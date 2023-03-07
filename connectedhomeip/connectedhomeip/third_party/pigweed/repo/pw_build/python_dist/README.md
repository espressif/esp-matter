# Python Distributables
Setup and usage instructions for Pigweed Python distributables.

## Prerequisites
Python distributables require Python 3.7 or later.

## Setup
Run the included setup script found inside the unzipped directory. The setup
script will create a virtual environment called `python-venv`.

##### Linux / MacOS
```bash
setup.sh
```

Alternatively, the Linux/MacOS version of the setup script also allows
installation over an existing venv:

```bash
setup.sh <PATH_TO_EXISTING_VENV>
```

##### Windows
```
setup.bat
```

##### Constraints File
Both the Linux/MacOS and Windows versions of the setup script support the use
of a constraints file. If a file named `constraints.txt` is present in the same
directory as the setup script, it will automatically be used during setup to
constrain the versions of any Python packages that get installed (see the
official
[pip documentation](https://pip.pypa.io/en/stable/user_guide/#constraints-files)
for details).


## Usage
Once setup is complete, the Python tools can be invoked as runnable modules:

Linux/MacOS:
```bash
python-venv/bin/python -m <MODULE_NAME> [OPTIONS]
```

Windows:
```
python-venv\Scripts\python -m <MODULE_NAME> [OPTIONS]
```
