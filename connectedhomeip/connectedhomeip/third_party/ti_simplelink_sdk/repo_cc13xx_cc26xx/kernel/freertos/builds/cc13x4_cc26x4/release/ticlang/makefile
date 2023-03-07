
SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

DRIVERLIB_DIR = $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)
FREERTOSPORT_DIR = $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)

VPATH = $(FREERTOS_INSTALL_DIR)/FreeRTOS/Source:$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure:$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/MemMang:$(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/source:$(FREERTOSPORT_DIR)/kernel/freertos/dpl:$(FREERTOSPORT_DIR)/kernel/freertos/startup

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

CFLAGS = -DDeviceFamily_CC13X4_CC26X4 -c -g -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -I.. "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure" "-I$(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/source/ti/posix/ticlang" "-I$(FREERTOSPORT_DIR)/source" "-I$(DRIVERLIB_DIR)/source" "-I$(TICLANG_ARMCOMPILER)/include"


CFILES = \
    croutine.c \
    event_groups.c \
    list.c \
    queue.c \
    stream_buffer.c \
    tasks.c \
    timers.c \
    port.c \
    portasm.c \
    heap_4.c \
    ti/posix/freertos/clock.c \
    ti/posix/freertos/memory.c \
    ti/posix/freertos/mqueue.c \
    ti/posix/freertos/pthread_barrier.c \
    ti/posix/freertos/pthread_cond.c \
    ti/posix/freertos/pthread.c \
    ti/posix/freertos/pthread_mutex.c \
    ti/posix/freertos/pthread_rwlock.c \
    ti/posix/freertos/sched.c \
    ti/posix/freertos/semaphore.c \
    ti/posix/freertos/sleep.c \
    ti/posix/freertos/timer.c \
    ti/posix/freertos/PTLS.c \
    ti/posix/freertos/aeabi_portable.c \
    AppHooks_freertos.c \
    ClockPCC26X2_freertos.c \
    DebugP_freertos.c \
    HwiPCC26X2_freertos.c \
    MutexP_freertos.c \
    PowerCC26X2_freertos.c \
    QueueP_freertos.c \
    SemaphoreP_freertos.c \
    StaticAllocs_freertos.c \
    SwiP_freertos.c \
    SystemP_freertos.c \
    TimerPCC26XX_freertos.c \
    startup_cc13x4_cc26x4_ticlang.c

OBJCFILES = $(subst /,_,$(CFILES:.c=.obj))

freertos.lib: $(OBJCFILES)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) -cr $@ $(OBJCFILES)

$(CFILES):
	@ echo "Error: can't find $@, please ensure FREERTOS_INSTALL_DIR in $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak is correct (it is '$(FREERTOS_INSTALL_DIR)')"
	@ exit 1

# Create a CC rule for each source file, where the object file
# name contains the source file path (replacing / with _).
#
define CC_RULE
$(subst /,_,$(1)).obj: $(1).c
	@ echo cc $$@ ...
	@ $$(CC) $$(CFLAGS) $$< -o $$@
endef
$(foreach file,$(subst .c,,$(CFILES)),$(eval $(call CC_RULE,$(file))))

clean:
	@ echo cleaning ...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.lib > $(DEVNULL) 2>&1
