AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = README.API README.math README.speed NTconfig.h makefile.win32 makefile.win32.dep config.h.win32

include_HEADERS = srp.h srp_aux.h t_client.h t_pwd.h t_server.h t_sha.h cstr.h

noinst_HEADERS = t_defines.h t_read.h nys_config.h yp_misc.h endXXent.h getXXent.h setXXent.h getXXbyYY.h nss_defs.h nsswitch.h

lib_LIBRARIES = libsrp.a

libsrp_a_SOURCES = \
  t_client.c t_conf.c t_conv.c t_getpass.c t_sha.c t_math.c t_misc.c \
  t_pw.c t_read.c t_server.c t_truerand.c cstr.c \
  srp.c rfc2945_client.c rfc2945_server.c srp6_client.c srp6_server.c \
  yp_misc.c yp_tpasswd.c yp_tconf.c nsw_tpasswd.c nsw_tconf.c nsswitch.c

AM_CFLAGS = @PICFLAG@

bin_PROGRAMS = tconf
noinst_PROGRAMS = clitest srvtest getpwtest srptest srpbench srp6bench

clitest_SOURCES = clitest.c
srvtest_SOURCES = srvtest.c
getpwtest_SOURCES = getpwtest.c
srptest_SOURCES = srptest.c
srpbench_SOURCES = srpbench.c
srp6bench_SOURCES = srp6bench.c
tconf_SOURCES = tconf.c

LDADD = libsrp.a

INCLUDES =
