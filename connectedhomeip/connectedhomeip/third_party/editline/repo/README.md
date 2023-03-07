Editline
========
[![License Badge][]][License] [![Travis Status]][Travis] [![Coverity Status]][Coverity Scan]


Table of Contents
-----------------

* [Introduction](#introduction)
* [API](#api)
* [Example](#example)
* [Build & Install](#build--install)
* [Origin & References](#origin--references)


Introduction
------------

This is a small [line editing][]  library.  It can be linked into almost
any program to  provide command line editing and  history functions.  It
is call compatible with the [FSF readline][] library, but at a fraction
of the  size, and as  a result fewer  features.  It is  also distributed
under a much more liberal [License][].

The small size  (<30k), lack of dependencies (ncurses  not needed!), and
the free license  should make this library interesting  to many embedded
developers.

Editline has several optional build-time features that can be enabled by
supplying different options to the GNU configure script.  See the output
from <kbd>configure --help</kbd> for details.  Some useful hints on how
to use the library is available in the `examples/` directory.

Editline is maintained collaboratively at [GitHub][].

> **Note:** Windows is not a supported target for editline.


Example
-------

Below is a very brief example to illustrate how one can use Editline to
create a simple CLI, Ctrl-D exits the program.  A slightly more advanced
example is Jush, <https://github.com/troglobit/jush/>, a small and very
simplistic UNIX shell.  The Editline sources also include an `examples/`
sub-directory.

1. Build and install the library, preferably using a [release tarball][]
   The configure script defaults to a `/usr/local` prefix.

        tar xf editline-1.15.3.tar.xz
        cd editline-1.15.3/
        ./configure --prefix=/usr
        make all
        sudo make install

2. Place the below source code in a separate project directory,
   e.g. `~/src/example.c`

```C
    #include <stdio.h>
    #include <stdlib.h>
    #include <editline.h>

    int main(void)
    {
        char *p;

        while ((p = readline("CLI> ")) != NULL) {
            puts(p);
            free(p);
        }

        return 0;
    }
```

3. Compile the example:

        cd ~/src/
        make LDLIBS=-leditline example

Here I use `make` and rely on its implicit (built-in) rules to handle
all the compiler magic, but you may want to create your own Makefile for
the project.  In particular if you don't change the default prefix
(above), because then you need to specify the search path for the
include file(s) and the library manually.

A simple `~/src/Makefile` could look like this:

    CFLAGS    = -I/usr/local/include
    LDFLAGS   = -L/usr/local/lib
    LDLIBS    = -leditline
    EXEC      = example
    OBJS      = example.o
    
    all: $(EXEC)
    
    $(EXEC): $(OBJS)
    
    clean:
            $(RM) $(OBJS) $(EXEC)
    
    distclean: clean
            $(RM) *.o *~ *.bak

Then simply type `make` from your `~/src/` directory.  You can also use
`pkg-config` for your `~/src/Makefile`, replace the following lines:

    CFLAGS    = $(shell pkg-config --cflags libeditline)
    LDFLAGS   = $(shell pkg-config --libs-only-L libeditline)
    LDLIBS    = $(shell pkg-config --libs-only-l libeditline)
    
Then simply type <kbd>make</kbd>, like above.

However, most `.rpm` based distributions `pkg-config` doesn't search in
`/usr/local` anymore, so you need to call make like this:

    PKG_CONFIG_LIBDIR=/usr/local/lib/pkgconfig make

Debian/Ubuntu based systems do not have this problem.


API
---

Here is the libeditline interfaces.  It has a small compatibility layer
to [FSF readline][], which may not be entirely up-to-date.

```C
    /* Editline specific global variables. */
    int         el_no_echo;   /* Do not echo input characters */
    int         el_no_hist;   /* Disable auto-save of and access to history,
                               * e.g. for password prompts or wizards */
    int         el_hist_size; /* Size of history scrollback buffer, default: 15 */
    
    /* Editline specific functions. */
    char *      el_find_word     (void);
    void        el_print_columns (int ac, char **av);
    el_status_t el_ring_bell     (void);
    el_status_t el_del_char      (void);
    
    /* Callback function for key binding */
    typedef el_status_t el_keymap_func_t(void);
    
    /* Bind key to a callback, use CTL('f') to change Ctrl-F, for example */
    el_status_t el_bind_key            (int key, el_keymap_func_t function);
    el_status_t el_bind_key_in_metamap (int key, el_keymap_func_t function);
    
    /* For compatibility with FSF readline. */
    int         rl_point;
    int         rl_mark;
    int         rl_end;
    int         rl_inhibit_complete;
    char       *rl_line_buffer;
    const char *rl_readline_name;
    
    void (*rl_deprep_term_function)(void);
    void rl_deprep_terminal (void);
    void rl_reset_terminal  (const char *terminal_name);

    void rl_initialize   (void);
    void rl_uninitialize (void);                         /* Free all internal memory */

    void rl_save_prompt    (void);
    void rl_restore_prompt (void);
    void rl_set_prompt     (const char *prompt);
    
    void rl_clear_message         (void);
    void rl_forced_update_display (void);

    /* Main function to use, saves history by default */
    char *readline    (const char *prompt);

    /* Use to save a read line to history, when el_no_hist is set */
    void add_history  (const char *line);
    
    /* Load and save editline history from/to a file. */
    int read_history  (const char *filename);
    int write_history (const char *filename);
    
    /* Magic completion API, see examples/cli.c for more info */
    rl_complete_func_t    *rl_set_complete_func    (rl_complete_func_t *func);
    rl_list_possib_func_t *rl_set_list_possib_func (rl_list_possib_func_t *func);
    
    /* Alternate interface to plain readline(), for event loops */
    void rl_callback_handler_install (const char *prompt, rl_vcpfunc_t *lhandler);
    void rl_callback_read_char       (void);
    void rl_callback_handler_remove  (void);
```


Build & Install
---------------

Editline was originally designed for older UNIX systems and Plan 9.  The
current maintainer works exclusively on GNU/Linux systems, so it may use
GCC and  GNU Make specific  extensions here and  there.  This is  not on
purpose and patches or pull requests to correct this are most welcome!

1. Configure editline with default features: <kbd>./configure</kbd>
2. Build the library and examples: <kbd>make all</kbd>
3. Install using <kbd>make install</kbd>

The `$DESTDIR` environment variable is honored at install.  For more
options, see <kbd>./configure --help</kbd>

Remember to run `ldconfig` after install to update the linker cache.  If
you've installed to a non-standard location (`--prefix`) you may also
have to update your `/etc/ld.so.conf`, or use `pkg-confg` to build your
application (above).

**NOTE:** RedHat/Fedora/CentOS and other `.rpm`-based distributions do
  not consider `/usr/local` as standard path anymore.  So make sure to
  `./configure --prefix=/usr`, otherwise the build system use the GNU
  default, which is `/usr/local`.  The Debian based distributions, like
  Ubuntu, do not have this problem.


Origin & References
--------------------

This [line editing][]  library was created by [Rich  Salz][] and Simmule
Turner and in 1992.  It is distributed with a “[C News][]-like” license,
similar to the [BSD license][].  Rich's current version is however under
the Apache license.  For details on  the licensing terms of this version
of the software, see [License][].

This version  of the editline  library was  forked from the  [Minix 2][]
source tree and is *not* related  to the similarily named NetBSD version
that [Jess Thrysøe][jess]  disitributes to the world  outside *BSD.  The
libraries have much in common, but  the latter is heavily refactored and
also relies  on libtermcap (usually  supplied by ncurses),  whereas this
library only uses termios from the standard C library.

Patches and  bug fixes from the  following forks, based on  the original
[comp.sources.unix][] posting, have been merged:

* Debian [libeditline][]
* [Heimdal][]
* [Festival][] speech-tools
* [Steve Tell][]'s editline patches

The version numbering  scheme today follows that of  the Debian version,
details available  in the [ChangeLog.md][].  The  current [maintainer][]
was unaware  of the Debian version  for quite some time,  so a different
name and versioning  scheme was used.  In June 2009  this was changed to
line up  alongside Debian, with  the intent  is to eventually  merge the
efforts.

Outstanding issues are listed in the [TODO.md][] file.

[GitHub]:          https://github.com/troglobit/editline
[line editing]:    https://github.com/troglobit/editline/blob/master/docs/README
[release tarball]: https://github.com/troglobit/editline/releases
[maintainer]:      http://troglobit.com
[C News]:          https://en.wikipedia.org/wiki/C_News
[TODO.md]:         https://github.com/troglobit/editline/blob/master/docs/TODO.md
[ChangeLog.md]:    https://github.com/troglobit/editline/blob/master/ChangeLog.md
[FSF readline]:    http://www.gnu.org/software/readline/
[Rich Salz]:       https://github.com/richsalz/editline/
[comp.sources.unix]: http://ftp.cs.toronto.edu/pub/white/pub/rc/editline.shar
[Minix 2]:         http://www.cise.ufl.edu/~cop4600/cgi-bin/lxr/http/source.cgi/lib/editline/
[jess]:            http://thrysoee.dk/editline/
[BSD license]:     http://en.wikipedia.org/wiki/BSD_licenses
[libeditline]:     http://packages.qa.debian.org/e/editline.html
[Heimdal]:         http://www.h5l.org
[Festival]:        http://festvox.org/festival/
[Steve Tell]:      http://www.cs.unc.edu/~tell/dist.html
[License]:         https://github.com/troglobit/editline/blob/master/LICENSE
[License Badge]:   https://img.shields.io/badge/License-C%20News-orange.svg
[Travis]:          https://travis-ci.org/troglobit/editline
[Travis Status]:   https://travis-ci.org/troglobit/editline.png?branch=master
[Coverity Scan]:   https://scan.coverity.com/projects/2982
[Coverity Status]: https://scan.coverity.com/projects/2982/badge.svg
