.. _module-pw_malloc_freelist:

------------------
pw_malloc_freelist
------------------

``pw_malloc_freelist`` implements the ``pw_malloc`` facade using a freelist
heap.

``pw_malloc_freelist`` initializes a global ``FreeListHeapBuffer`` object to
organize heap usage. Implementation details are in the ``pw_allocator`` module.

``pw_malloc_freelist`` provides wrapper functions for ``malloc``, ``free``,
``realloc`` and ``calloc`` that uses the freelist implementation of heap in
``pw_allocator``. In the GN build file, ``pw_malloc_freelist`` provides linker
options needed in ``public_configs``, which will be forwarded to the facade. In
the case of freelist, we specify the wrapper functions ``malloc, free, realloc,
calloc, _malloc_r, _free_r, _realloc_r, _calloc_r`` to replace the original libc
functions at linker time.
