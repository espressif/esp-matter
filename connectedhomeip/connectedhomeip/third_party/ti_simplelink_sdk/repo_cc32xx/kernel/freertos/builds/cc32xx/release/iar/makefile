
SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

DRIVERLIB_DIR = $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)
FREERTOSPORT_DIR = $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)

VPATH = $(FREERTOS_INSTALL_DIR)/FreeRTOS/Source:$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/IAR/ARM_CM3:$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/MemMang:$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source:$(FREERTOSPORT_DIR)/kernel/freertos/dpl:$(FREERTOSPORT_DIR)/kernel/freertos/startup

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
ASM = "$(IAR_ARMCOMPILER)/bin/iasmarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

CFLAGS = -I.. "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/IAR/ARM_CM3" "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/posix/iar" "-I$(FREERTOSPORT_DIR)/source" "-I$(DRIVERLIB_DIR)/source" "-I$(IAR_ARMCOMPILER)/include" -Dewarm  --debug --silent --aeabi --cpu=Cortex-M4 --diag_suppress=Pa050,Go005 --endian=little -e --thumb

AFLAGS = -I.. "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" -S --cpu Cortex-M4 --endian little --thumb

CFILES = \
    croutine.c \
    event_groups.c \
    list.c \
    queue.c \
    stream_buffer.c \
    tasks.c \
    timers.c \
    port.c \
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
    ti/posix/freertos/Mtx.c \
    AppHooks_freertos.c \
    ClockP_freertos.c \
    DebugP_freertos.c \
    MutexP_freertos.c \
    QueueP_freertos.c \
    SemaphoreP_freertos.c \
    StaticAllocs_freertos.c \
    SwiP_freertos.c \
    SystemP_freertos.c \
    HwiPCC32XX_freertos.c \
    PowerCC32XX_freertos.c \
    startup_cc32xx_iar.c

AFILES = \
    portasm.s

OBJCFILES = $(subst /,_,$(CFILES:.c=.obj))
OBJAFILES = $(AFILES:.s=.obj)

freertos.lib: $(OBJCFILES) $(OBJAFILES)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) --silent -o $@ $(OBJCFILES) $(OBJAFILES)

$(CFILES) $(AFILES):
	@ echo "Error: can't find $@, please ensure FREERTOS_INSTALL_DIR in $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak is correct (it is '$(FREERTOS_INSTALL_DIR)')"
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

%.obj:%.s
	@ echo asm $@ ...
	@ $(ASM) $(AFLAGS) $< -o $@

clean:
	@ echo cleaning ...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.lib > $(DEVNULL) 2>&1
