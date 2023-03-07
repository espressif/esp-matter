#
SUB_MODULE_DIR:= $(MODULE_DIR)/libc
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/libc

SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)/src

COMMON_INCLUDE += -I $(MODULE_DIR)/libc/inc
                  
libc_sources :=  atof.c atoi.c atol.c
libc_sources +=  atoll.c jrand48.c lrand48.c
libc_sources +=  mrand48.c nrand48.c seed48.c
libc_sources +=  srand48.c memccpy.c memcpy.c memset.c
libc_sources +=  memswap.c memchr.c memrchr.c
libc_sources +=  memmem.c memcmp.c bsearch.c
libc_sources +=  qsort.c fnmatch.c snprintf.c
libc_sources +=  sprintf.c sscanf.c vsnprintf.c
libc_sources +=  vsprintf.c vsscanf.c strcmp.c
libc_sources +=  strncmp.c strcasecmp.c strncasecmp.c
libc_sources +=  strcpy.c strlcpy.c strncpy.c
libc_sources +=  strcat.c strlcat.c strncat.c
libc_sources +=  strlen.c strnlen.c strchr.c
libc_sources +=  strrchr.c strsep.c strstr.c
libc_sources +=  strcspn.c strspn.c strxspn.c
libc_sources +=  strpbrk.c strtok.c strtok_r.c
libc_sources +=  strtol.c strtoll.c strtoul.c
libc_sources +=  strtoull.c strtoimax.c strtoumax.c
libc_sources +=  strntoimax.c strntoumax.c
libc_sources +=  stdlib.c common.c
libc_sources += ctype_isalnum.c ctype_isalpha.c \
				ctype_isascii.c ctype_isblank.c \
				ctype_iscntrl.c ctype_isdigit.c \
				ctype_isgraph.c ctype_ispunct.c \
				ctype_islower.c ctype_isprint.c \
				ctype_isspace.c ctype_isupper.c \
				ctype_tolower.c ctype_toupper.c \
				ctype_isxdigit.c ctypes.c

libc_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(libc_sources)))

common_objs_target += $(libc_objs)

$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(COMMON_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $< -o $@
