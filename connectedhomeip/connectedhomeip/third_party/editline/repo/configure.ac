AC_INIT(editline, 1.17.1, https://github.com/troglobit/editline/issues)
AC_CONFIG_AUX_DIR(aux)
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/editline.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile libeditline.pc src/Makefile include/Makefile man/Makefile examples/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
LT_INIT

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STAT
AC_HEADER_STDC
# Check for malloc.h instead of AC_FUNC_MALLOC/REALLOC AIX and others
# mess up the traditional malloc check.
AC_CHECK_HEADERS([malloc.h signal.h stdlib.h string.h termcap.h termio.h termios.h sgtty.h unistd.h])

# In termios.h or in sys/ioctl.g?
AC_HEADER_TIOCGWINSZ

# Overrides and types, should be a check.
AC_DEFINE([SYS_UNIX], [1], [Default to UNIX backend, should be detected.])
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_STAT
AC_CHECK_FUNCS([strchr strdup strrchr tcgetattr perror])

#
# Available features
#
AC_ARG_ENABLE(unique-history,
   [AS_HELP_STRING([--disable-unique-history],
                   [Disable uniqify of scrollback. Default: duplicate entries are ignored. Use this to save dupes.])])

AC_ARG_ENABLE(arrow-keys,
   [AS_HELP_STRING([--disable-arrow-keys], [Disable ANSI arrow keys.])])

AC_ARG_ENABLE(eof,
   [AS_HELP_STRING([--disable-eof], [Disable default EOF (Ctrl-D) behavior.])])

AC_ARG_ENABLE(sigint,
   [AS_HELP_STRING([--disable-sigint], [Disable default SIGINT (Ctrl-C) behavior.])])

AC_ARG_ENABLE(sigstop,
   [AS_HELP_STRING([--enable-sigstop], [Enable SIGSTOP (Ctrl-Z) behavior.])])

AC_ARG_ENABLE(terminal-bell,
   [AS_HELP_STRING([--enable-terminal-bell], [Enable terminal bell on completion.])])

AC_ARG_ENABLE(termcap,
   AS_HELP_STRING([--enable-termcap], [Use termcap library to query terminal size.]))

AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--enable-examples], [Build examples/ directory])],
	[], [enable_examples=no])

#
# Check what features have been enabled
#
AS_IF([test "x$enable_unique_history" != "xno"],
   AC_DEFINE(CONFIG_UNIQUE_HISTORY, 1, [Define to skip duplicate lines in the scrollback history.]))

AS_IF([test "x$enable_terminal_bell" != "xno"],
   AC_DEFINE(CONFIG_ANSI_ARROWS, 1, [Define to include ANSI arrow keys support.]))

AS_IF([test "x$enable_eof" != "xno"],
   AC_DEFINE(CONFIG_EOF, 1, [Define to enable EOF (Ctrl-D) key.]))

AS_IF([test "x$enable_sigint" != "xno"],
   AC_DEFINE(CONFIG_SIGINT, 1, [Define to enable SIGINT (Ctrl-C) key.]))

AS_IF([test "x$enable_sigstop" = "xyes"],
   AC_DEFINE(CONFIG_SIGSTOP, 1, [Define to enable SIGSTOP (Ctrl-Z) key.]))

AS_IF([test "x$enable_terminal_bell" = "xyes"],
   AC_DEFINE(CONFIG_TERMINAL_BELL, 1, [Define to enable terminal bell on completion.]))

AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

# Check for a termcap compatible library if enabled
AS_IF([test "x$enable_termcap" = "xyes"],
   AC_DEFINE(CONFIG_USE_TERMCAP, 1, [Define to use the termcap library for terminal size.])
   AC_CHECK_LIB(terminfo, tgetent, , [
      AC_CHECK_LIB(termcap, tgetent, , [
         AC_CHECK_LIB(tinfo, tgetent, , [
            AC_CHECK_LIB(curses, tgetent, , [
               AC_CHECK_LIB(ncurses, tgetent, , [
                  AC_MSG_ERROR([Cannot find a termcap capable library, try installing Ncurses.])])
               ])
            ])
         ])
      ]))

# Generate all files
AC_OUTPUT
