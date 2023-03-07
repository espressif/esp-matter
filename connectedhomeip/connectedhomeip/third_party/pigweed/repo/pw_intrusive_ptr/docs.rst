.. _module-pw_intrusive_ptr:

----------------
pw_intrusive_ptr
----------------

``pw::IntrusivePtr`` is a smart shared pointer that relies on the pointed-at
object to do the reference counting. Its API is based on ``std::shared_ptr`` but
requires the pointed-at class to provide ``AddRef()`` and ``ReleaseRef()``
methods to do the reference counting. The easiest way to do that is to
subclass ``pw::RefCounted``. Doing this will provide atomic reference counting
and a ``Ptr`` type alias for the ``IntrusivePtr<T>``.

``IntrusivePtr`` doesn't provide any weak pointer ability.

``IntrusivePtr`` with a ``RefCounted``-based class always guarantees atomic
operations on the reference counter, whereas ``std::shared_ptr`` falls back to a
non-atomic control block when threading support is not enabled due to a design
fault in the STL implementation.

Similar to ``std::shared_ptr``, ``IntrusivePtr`` doesn't provide any
thread-safety guarantees for the pointed-at object or for the pointer object
itself. I.e. assigning and reading the same ``IntrusivePtr`` from multiple
threads without external lock is not allowed.

.. code-block:: cpp

  class MyClass : public RefCounted<MyClass> {
  // ...
  };

  // Empty pointer, equals to nullptr.
  // MyClass::Ptr is the same as IntrusivePtr<MyClass>.
  MyClass::Ptr empty_ptr = IntrusivePtr<MyClass>();

  // Wrapping an externally created pointer.
  MyClass raw_ptr = new MyClass();
  MyClass::Ptr ptr_1 = MyClass::Ptr(raw_ptr);
  // raw_ptr shouldn't be used after this line if ptr_1 can go out of scope.

  // Using MakeRefCounted() helper.
  auto ptr_2 = MakeRefCounted<MyClass>(/* ... */);

``IntrusivePtr`` can be passed as an argument by either const reference or
value. Const reference is more preferable because it does not cause unnecessary
copies (which results in atomic operations on the ref count). Passing by value
is used when this ``IntrusivePtr`` is immediately stored (e.g. constructor that
stores ``IntrusivePtr`` in the object field). In this case passing by value and
move is more explicit in terms of intentions. It is also the behavior that
clang-tidy checks suggest.

``IntrusivePtr`` should almost always be returned by value. The only case when
it can be returned by const reference is the trivial getter for the object
field. When returning locally created ``IntrusivePtr`` or a pointer that was
casted to the base class it MUST be returned by value.
