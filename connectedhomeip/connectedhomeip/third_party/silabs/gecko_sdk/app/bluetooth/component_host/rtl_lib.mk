################################################################################
# Real-Time Locationing library component                                      #
################################################################################

ifeq (, $(filter $(MAKECMDGOALS), clean export help))
  ifeq (, $(filter $(UNAME), darwin linux))
    ifneq ($(CC), x86_64-w64-mingw32-gcc)
      $(error Toolchain not supported by RTL lib.)
    endif
  endif
endif

RTL_DIR = $(SDK_DIR)/util/silicon_labs/aox
ifeq ($(OS),posix)
LIBNAME := aox_static_$(UNAME)_$(UNAME_M)
else
LIBNAME := aox_static_windows_$(UNAME_M)
endif

override INCLUDEPATHS += $(RTL_DIR)/inc

override CFLAGS += -DRTL_LIB

ifeq ($(OS),posix)
override LDFLAGS += \
-L$(RTL_DIR)/lib/gcc/release \
-l$(LIBNAME) \
-lstdc++ \
-lpthread
else
override LDFLAGS += \
"$(RTL_DIR)/lib/gcc/release/lib$(LIBNAME).a" \
-lstdc++ \
-lpthread
endif

PROJ_FILES += $(wildcard $(RTL_DIR)/lib/gcc/release/*)
