---
# FatFs
---
## Version
_FatFs R0.13a - October 27,2017_

## Summary
_FatFs_ is provided as source & pre-built libraries with few modifications to
allow for integration with SimpleLink SDK.  Unmodified source can be found at
[`Elm-Chan FatFs`](http://elm-chan.org/fsw/ff/00index_e.html
"Elm-Chan FatFs Homepage").  Changes are as follows:

* The _source_ directory has been removed.  All _FatFs_ source files have been
moved up a directory (to _third_party/fatfs_) alongside _documents_ & _lib_
directories.

* _ff.c_:
    * Applied patch [`ff_13a_p1`](http://elm-chan.org/fsw/ff/ff_13a_p1.diff).

* _ffconf.h_:
    * Set `FF_USE_MKFS` to 1; this enables the `f_mkfs()` API.
    * Changed `FF_VOLUMES` to 4.
    * Set `FF_FS_REENTRANT` to 1; enable reentrancy support.
    * Defined `FF_SYNC_t` to _(void *)_; synchronization object type.

* _ffsystem.c_:
    * Changed synchronization & memory management function implementations to
use SimpleLink SDK APIs.

* _diskio.h_:
    * Added `diskio_fxns` struct, `disk_register()` & `disk_unregister()` in
order to allow disk_* APIs to call media specific functions.

* _diskio.c_:
    * Changed diskio APIs to call media specific functions registered during
`disk_register()`.

* Added _ffcio.c/.h_:
    * APIs to hook _FatFs_ into the _TI Compiler's `stdio.h`_ APIs.

* Added _ramdisk.c_:
    * RAM disk layer for _FatFs_.

* Added _utils_ directory:
    * Miscellaneous utilities for manipulating RAM disk images.  These are
provided as reference & are not compiled as part of the _FatFs_ library.
