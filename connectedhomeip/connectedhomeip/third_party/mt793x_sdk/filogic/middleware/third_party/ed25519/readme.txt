ed25519 module usage guide

Brief:          This module is the implementation of ed25519.
                it's only for homekit, it's not recommended to include .h files in the folder
Usage:          GCC: Include the following contents in your GCC project Makefile.
                LIBS += $(OUTPATH)/libed25519.a
                MODULE_PATH += $(MID_ED25519_PATH)                
Dependency:     N/A
Notice:         N/A
Relative doc:   N/A.
Example project:N/A. 