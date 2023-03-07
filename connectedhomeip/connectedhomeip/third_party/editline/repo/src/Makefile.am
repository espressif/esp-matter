lib_LTLIBRARIES         = libeditline.la
libeditline_la_SOURCES  = editline.c editline.h complete.c sysunix.c unix.h
libeditline_la_CFLAGS   = -std=gnu99
libeditline_la_CFLAGS  += -W -Wall -Wextra -Wundef -Wunused -Wstrict-prototypes
libeditline_la_CFLAGS  += -Werror-implicit-function-declaration -Wshadow -Wcast-qual
libeditline_la_LDFLAGS  = $(AM_LDFLAGS) -version-info 1:2:0
