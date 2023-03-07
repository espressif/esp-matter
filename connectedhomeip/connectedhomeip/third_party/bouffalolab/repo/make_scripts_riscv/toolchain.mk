##change to your toolchain path
CONFIG_TOOLPREFIX ?= $(BL60X_SDK_PATH)/toolchain/riscv/$(shell uname |cut -d '_' -f1)/bin/riscv64-unknown-elf-
#CONFIG_TOOLPREFIX ?= riscv64-unknown-elf-
