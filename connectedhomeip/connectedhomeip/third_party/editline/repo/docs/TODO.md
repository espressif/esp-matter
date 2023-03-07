TODO
====

Issues in need of work.  Mostly compatibility with GNU readline,
BSD [libedit][], and usability improvements.

Remember, the general idea is to keep this library small with no
external dependencies, except for a generic C library.


Check what's needed to run the fileman example
----------------------------------------------

The BSD libedit library has imported the GNU readline "fileman" example
into its tree to demonstrate the abilities of that library.  This would
also be quite useful for this library!

The first task is to investigate the depependencies and form TODO list
items detailing what is missing and, if possible, proposals how to
implement including any optional configure flags.


Other minor TODO's
------------------

- Instead of supporting multiline input, try the Emacs approach, line
  scrolling.
- Add support for `rl_bind_key()`, currently only en editline specific
  `el_bind_key()` exists.
- Make `char *rl_prompt;` globally visible.
- Add support for `rl_set_prompt()`
- Add support for `--enable-utf8` to configure script
- Use `strcmp(nl_langinfo(CODESET), "UTF-8")` to look for utf8 capable
  terminal
- Implement simple UTF-8 parser according to
  http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8


[gnu]:     http://www.delorie.com/gnu/docs/readline/rlman_41.html#IDX288
[libuEv]:  https://github.com/troglobit/libuev/
[libedit]: http://www.thrysoee.dk/editline/
