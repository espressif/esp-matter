################################################################################
# Simple timer component                                                              #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/simple_timer

# The export feature requires all source files, but in ${OS} mac and linux are
# both "posix". As this is the only component where individual mac source is
# needed, these are added by a wildcard instead of the foreach in other components
ifneq (, $(filter $(MAKECMDGOALS), export))
  # Export only
  override C_SRC += $(wildcard $(SDK_DIR)/app/bluetooth/common_host/simple_timer/sl_simple_timer_*.c )
else
  # Build or clean
  # Mac is "posix" in ${OS}, an uname check is needed
  ifneq (, $(filter $(UNAME), darwin))
    override C_SRC += \
    $(SDK_DIR)/app/bluetooth/common_host/simple_timer/sl_simple_timer_mac.c
  else
    override C_SRC += \
    $(SDK_DIR)/app/bluetooth/common_host/simple_timer/sl_simple_timer_$(OS).c
    ifeq ($(OS), posix)
      # Add -lrt on Linux only
      override LDFLAGS += -lrt
    endif
  endif
endif
  
override CFLAGS += -DSL_CATALOG_SIMPLE_TIMER_PRESENT
